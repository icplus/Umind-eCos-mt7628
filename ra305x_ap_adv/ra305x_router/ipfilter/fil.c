/*
 * Copyright (C) 1993-2001 by Darren Reed.
 *
 * See the IPFILTER.LICENCE file for details on licencing.
 */

#include <sys/errno.h>
#include <sys/bsdtypes.h>
#include <sys/param.h>
#include <sys/time.h>
#if defined(__NetBSD__) && (NetBSD >= 199905) && !defined(IPFILTER_LKM) && \
    defined(_KERNEL)
# include "opt_ipfilter_log.h"
#endif
#if (defined(KERNEL) || defined(_KERNEL)) && defined(__FreeBSD_version) && \
    (__FreeBSD_version >= 220000)
# if (__FreeBSD_version >= 400000)
#  ifndef KLD_MODULE
//Eddy #   include "opt_inet6.h"
#  endif
#  if (__FreeBSD_version == 400019)
#   define CSUM_DELAY_DATA
#  endif
# endif
//Eddy # include <sys/filio.h>
#else
# include <sys/ioctl.h>
#endif

#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>

#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <ip_compat.h>
#ifdef	USE_INET6
#include <netinet/icmp6.h>
#include <netinet6/in6_var.h>
#endif
#include <netinet/tcpip.h>

#include <ip_fil.h>
#include <ip_nat.h>
#include <ip_frag.h>
#include <ip_state.h>
#include <ip_proxy.h>
#include <ip_auth.h>
#include <ip_nat.h>
# if defined(__FreeBSD_version) && (__FreeBSD_version >= 300000)
#  include <sys/malloc.h>
#  if defined(_KERNEL) && !defined(IPFILTER_LKM)
//Eddy #   include "opt_ipfilter.h"
#  endif
# endif
#ifndef	MIN
# define	MIN(a,b)	(((a)<(b))?(a):(b))
#endif
#include "ipl.h"
#include <ip_defense.h>
#include "eventlog.h"
#include <cfg_net.h>
#include <cfg_id.h>
#include <sys_status.h>

#define	FR_VERBOSE(verb_pr)
#define	FR_DEBUG(verb_pr)
#define	IPLLOG(a, c, d, e)		ipflog(a, c, d, e)

struct	filterstats frstats[2] = {{0,0,0,0,0},{0,0,0,0,0}};
struct	frentry	*ipfilter[2][2] = { { NULL, NULL }, { NULL, NULL } },
#ifdef	USE_INET6
		*ipfilter6[2][2] = { { NULL, NULL }, { NULL, NULL } },
		*ipacct6[2][2] = { { NULL, NULL }, { NULL, NULL } },
#endif
		*ipacct[2][2] = { { NULL, NULL }, { NULL, NULL } };
struct	frgroup *ipfgroups[3][2];
int	fr_flags = IPF_LOGGING;
int	fr_active = 0;
int	fr_chksrc = 0;
int	fr_minttl = 3;
int	fr_minttllog = 1;

#ifdef	CONFIG_NAPT_IP_TUPLE
ip5tup_t    iptup[MAX_IP_5_TUP_NUM];
#endif

#if defined(IPFILTER_DEFAULT_BLOCK)
int	fr_pass = FR_NOMATCH|FR_BLOCK;
#else
int	fr_pass = (IPF_DEFAULT_PASS|FR_NOMATCH);
#endif
char	ipfilter_version[] = IPL_VERSION;

fr_info_t	frcache[2];

static	int	frflushlist __P((int, minor_t, int *, frentry_t **));
static	void	frsynclist __P((frentry_t *));

void *block_ifp[PRIMARY_WANIP_NUM]={0};
int drop_portscan=0;
int auth_port_open=0;
extern int nat_enabled;
extern int ping_id;
#ifdef CONFIG_SUPERDMZ
extern char *superdmz_mac;
#endif

/*
 * bit values for identifying presence of individual IP options
 */
struct	optlist	ipopts[20] = {
	{ IPOPT_NOP,	0x000001 },
	{ IPOPT_RR,	0x000002 },
	{ IPOPT_ZSU,	0x000004 },
	{ IPOPT_MTUP,	0x000008 },
	{ IPOPT_MTUR,	0x000010 },
	{ IPOPT_ENCODE,	0x000020 },
	{ IPOPT_TS,	0x000040 },
	{ IPOPT_TR,	0x000080 },
	{ IPOPT_SECURITY, 0x000100 },
	{ IPOPT_LSRR,	0x000200 },
	{ IPOPT_E_SEC,	0x000400 },
	{ IPOPT_CIPSO,	0x000800 },
	{ IPOPT_SATID,	0x001000 },
	{ IPOPT_SSRR,	0x002000 },
	{ IPOPT_ADDEXT,	0x004000 },
	{ IPOPT_VISA,	0x008000 },
	{ IPOPT_IMITD,	0x010000 },
	{ IPOPT_EIP,	0x020000 },
	{ IPOPT_FINN,	0x040000 },
	{ 0,		0x000000 }
};

/*
 * bit values for identifying presence of individual IP security options
 */
struct	optlist	secopt[8] = {
	{ IPSO_CLASS_RES4,	0x01 },
	{ IPSO_CLASS_TOPS,	0x02 },
	{ IPSO_CLASS_SECR,	0x04 },
	{ IPSO_CLASS_RES3,	0x08 },
	{ IPSO_CLASS_CONF,	0x10 },
	{ IPSO_CLASS_UNCL,	0x20 },
	{ IPSO_CLASS_RES2,	0x40 },
	{ IPSO_CLASS_RES1,	0x80 }
};

int need_dialup_flag = 0;

int is_block_ifp(void *ifp)
{
	int i;
	
	if (ifp == NULL)
		return 0;

	for (i=0; i<PRIMARY_WANIP_NUM; i++) {
		if (block_ifp[i] == ifp)
			return 1;
	}
	
	return 0;
}

/*
 * compact the IP header into a structure which contains just the info.
 * which is useful for comparing IP headers with.
 */
void	fr_makefrip(hlen, ip, fin)
int hlen;
ip_t *ip;
fr_info_t *fin;
{
	u_short optmsk = 0, secmsk = 0, auth = 0;
	int i, mv, ol, off, p, plen, v;
	fr_ip_t *fi = &fin->fin_fi;
	struct optlist *op;
	u_char *s, opt;
	tcphdr_t *tcp;

	fin->fin_rev = 0;
	fin->fin_fr = NULL;
	fin->fin_tcpf = 0;
	fin->fin_data[0] = 0;
	fin->fin_data[1] = 0;
	fin->fin_rule = -1;
	fin->fin_group = -1;
	fin->fin_icode = ipl_unreach;
	v = fin->fin_v;
	fi->fi_v = v;
	fin->fin_hlen = hlen;
	if (v == 4) {
		fin->fin_id = ip->ip_id;
		fi->fi_tos = ip->ip_tos;
		off = (ip->ip_off & IP_OFFMASK);
		tcp = (tcphdr_t *)((char *)ip + hlen);
		(*(((u_short *)fi) + 1)) = (*(((u_short *)ip) + 4));
		fi->fi_src.i6[1] = 0;
		fi->fi_src.i6[2] = 0;
		fi->fi_src.i6[3] = 0;
		fi->fi_dst.i6[1] = 0;
		fi->fi_dst.i6[2] = 0;
		fi->fi_dst.i6[3] = 0;
		fi->fi_saddr = ip->ip_src.s_addr;
		fi->fi_daddr = ip->ip_dst.s_addr;
		p = ip->ip_p;
		fi->fi_fl = (hlen > sizeof(ip_t)) ? FI_OPTIONS : 0;
		if (ip->ip_off & (IP_MF|IP_OFFMASK))
			fi->fi_fl |= FI_FRAG;
		plen = ip->ip_len;
		fin->fin_dlen = plen - hlen;
	}
#ifdef	USE_INET6
	else if (v == 6) {
		ip6_t *ip6 = (ip6_t *)ip;

		off = 0;
		p = ip6->ip6_nxt;
		fi->fi_p = p;
		fi->fi_ttl = ip6->ip6_hlim;
		tcp = (tcphdr_t *)(ip6 + 1);
		fi->fi_src.in6 = ip6->ip6_src;
		fi->fi_dst.in6 = ip6->ip6_dst;
		fin->fin_id = (u_short)(ip6->ip6_flow & 0xffff);
		fi->fi_tos = 0;
		fi->fi_fl = 0;
		plen = ntohs(ip6->ip6_plen);
		fin->fin_dlen = plen;
		plen += sizeof(*ip6);
	}
#endif
	else
		return;

	fin->fin_off = off;
	fin->fin_plen = plen;
	fin->fin_dp = (char *)tcp;
	fin->fin_misc = 0;
	off <<= 3;

	switch (p)
	{
#ifdef USE_INET6
	case IPPROTO_ICMPV6 :
	{
		int minicmpsz = sizeof(struct icmp6_hdr);
		struct icmp6_hdr *icmp6;

		if (fin->fin_dlen > 1) {
			fin->fin_data[0] = *(u_short *)tcp;

			icmp6 = (struct icmp6_hdr *)tcp;

			switch (icmp6->icmp6_type)
			{
			case ICMP6_ECHO_REPLY :
			case ICMP6_ECHO_REQUEST :
				minicmpsz = ICMP6_MINLEN;
				break;
			case ICMP6_DST_UNREACH :
			case ICMP6_PACKET_TOO_BIG :
			case ICMP6_TIME_EXCEEDED :
			case ICMP6_PARAM_PROB :
				minicmpsz = ICMP6ERR_IPICMPHLEN;
				break;
			default :
				break;
			}
		}

		if (!(plen >= minicmpsz))
			fi->fi_fl |= FI_SHORT;

		break;
	}
#endif
	case IPPROTO_ICMP :
	{
		int minicmpsz = sizeof(struct icmp);
		icmphdr_t *icmp;

		if (!off && (fin->fin_dlen > 1)) {
			fin->fin_data[0] = *(u_short *)tcp;

			icmp = (icmphdr_t *)tcp;

			switch (icmp->icmp_type)
			{
			case ICMP_ECHOREPLY :
			case ICMP_ECHO :
			/* Router discovery messages - RFC 1256 */
			case ICMP_ROUTERADVERT :
			case ICMP_ROUTERSOLICIT :
				minicmpsz = ICMP_MINLEN;
				break;
			/*
			 * type(1) + code(1) + cksum(2) + id(2) seq(2) +
			 * 3*timestamp(3*4)
			 */
			case ICMP_TSTAMP :
			case ICMP_TSTAMPREPLY :
				minicmpsz = 20;
				break;
			/*
			 * type(1) + code(1) + cksum(2) + id(2) seq(2) +
			 * mask(4)
			 */
			case ICMP_MASKREQ :
			case ICMP_MASKREPLY :
				minicmpsz = 12;
				break;
			default :
				break;
			}
		}

		if ((!(plen >= hlen + minicmpsz) && !off) ||
		    (off && off < sizeof(struct icmp)))
			fi->fi_fl |= FI_SHORT;

		break;
	}
	case IPPROTO_TCP :
		fi->fi_fl |= FI_TCPUDP;
#ifdef	USE_INET6
		if (v == 6) {
			if (plen < sizeof(struct tcphdr))
				fi->fi_fl |= FI_SHORT;
		} else
#endif
		if (v == 4) {
			if ((!IPMINLEN(ip, tcphdr) && !off) ||
			     (off && off < sizeof(struct tcphdr)))
				fi->fi_fl |= FI_SHORT;
		}
		if (!(fi->fi_fl & FI_SHORT) && !off)
			fin->fin_tcpf = tcp->th_flags;
		goto getports;
	case IPPROTO_UDP :
		fi->fi_fl |= FI_TCPUDP;
#ifdef	USE_INET6
		if (v == 6) {
			if (plen < sizeof(struct udphdr))
				fi->fi_fl |= FI_SHORT;
		} else
#endif
		if (v == 4) {
			if ((!IPMINLEN(ip, udphdr) && !off) ||
			    (off && off < sizeof(struct udphdr)))
				fi->fi_fl |= FI_SHORT;
		}
getports:
		if (!off && (fin->fin_dlen > 3)) {
			fin->fin_data[0] = ntohs(tcp->th_sport);
			fin->fin_data[1] = ntohs(tcp->th_dport);
		}
		break;
	case IPPROTO_ESP :
#ifdef	USE_INET6
		if (v == 6) {
			if (plen < 8)
				fi->fi_fl |= FI_SHORT;
		} else
#endif
		if (v == 4) {
			if (((ip->ip_len < hlen + 8) && !off) ||
			    (off && off < 8))
				fi->fi_fl |= FI_SHORT;
		}
		break;
	default :
		break;
	}

#ifdef	USE_INET6
	if (v == 6) {
		fi->fi_optmsk = 0;
		fi->fi_secmsk = 0;
		fi->fi_auth = 0;
		return;
	}
#endif

	for (s = (u_char *)(ip + 1), hlen -= (int)sizeof(*ip); hlen > 0; ) {
		opt = *s;
		if (opt == '\0')
			break;
		else if (opt == IPOPT_NOP)
			ol = 1;
		else {
			if (hlen < 2)
				break;
			ol = (int)(*(s + 1));
			if (ol < 2 || ol > hlen)
				break;
		}
		for (i = 9, mv = 4; mv >= 0; ) {
			op = ipopts + i;
			if (opt == (u_char)op->ol_val) {
				optmsk |= op->ol_bit;
				if (opt == IPOPT_SECURITY) {
					struct optlist *sp;
					u_char	sec;
					int j, m;

					sec = *(s + 2);	/* classification */
					for (j = 3, m = 2; m >= 0; ) {
						sp = secopt + j;
						if (sec == sp->ol_val) {
							secmsk |= sp->ol_bit;
							auth = *(s + 3);
							auth *= 256;
							auth += *(s + 4);
							break;
						}
						if (sec < sp->ol_val)
							j -= m--;
						else
							j += m--;
					}
				}
				break;
			}
			if (opt < op->ol_val)
				i -= mv--;
			else
				i += mv--;
		}
		hlen -= ol;
		s += ol;
	}
	if (auth && !(auth & 0x0100))
		auth &= 0xff00;
	fi->fi_optmsk = optmsk;
	fi->fi_secmsk = secmsk;
	fi->fi_auth = auth;
}


/*
 * check an IP packet for TCP/UDP characteristics such as ports and flags.
 */
int fr_tcpudpchk(ft, fin)
frtuc_t *ft;
fr_info_t *fin;
{
	register u_short po, tup;
	register char i;
	register int err = 1;
	
	/*
	 * Both ports should *always* be in the first fragment.
	 * So far, I cannot find any cases where they can not be.
	 *
	 * compare destination ports
	 */
	if ((i = (int)ft->ftu_dcmp)) {
		po = ft->ftu_dport;
		tup = fin->fin_data[1];
		/*
		 * Do opposite test to that required and
		 * continue if that succeeds.
		 */
		if (!--i && tup != po) /* EQUAL */
			err = 0;
		else if (!--i && tup == po) /* NOTEQUAL */
			err = 0;
		else if (!--i && tup >= po) /* LESSTHAN */
			err = 0;
		else if (!--i && tup <= po) /* GREATERTHAN */
			err = 0;
		else if (!--i && tup > po) /* LT or EQ */
			err = 0;
		else if (!--i && tup < po) /* GT or EQ */
			err = 0;
		else if (!--i &&	   /* Out of range */
			 //(tup >= po && tup <= ft->ftu_dtop))
			 (tup > po && tup < ft->ftu_dtop))
			err = 0;
		else if (!--i &&	   /* In range */
			 //(tup <= po || tup >= ft->ftu_dtop))
			 (tup < po || tup > ft->ftu_dtop))
			err = 0;
	}
	/*
	 * compare source ports
	 */
	if (err && (i = (int)ft->ftu_scmp)) {
		po = ft->ftu_sport;
		tup = fin->fin_data[0];
		if (!--i && tup != po)
			err = 0;
		else if (!--i && tup == po)
			err = 0;
		else if (!--i && tup >= po)
			err = 0;
		else if (!--i && tup <= po)
			err = 0;
		else if (!--i && tup > po)
			err = 0;
		else if (!--i && tup < po)
			err = 0;
		else if (!--i &&	   /* Out of range */
			 //(tup >= po && tup <= ft->ftu_stop))
			 (tup > po && tup < ft->ftu_stop))
			err = 0;
		else if (!--i &&	   /* In range */
			 //(tup <= po || tup >= ft->ftu_stop))
			 (tup < po || tup > ft->ftu_stop))
			err = 0;
	}

	/*
	 * If we don't have all the TCP/UDP header, then how can we
	 * expect to do any sort of match on it ?  If we were looking for
	 * TCP flags, then NO match.  If not, then match (which should
	 * satisfy the "short" class too).
	 */
	if (err && (fin->fin_fi.fi_p == IPPROTO_TCP)) {
		if (fin->fin_fl & FI_SHORT)
			return !(ft->ftu_tcpf | ft->ftu_tcpfm);
		/*
		 * Match the flags ?  If not, abort this match.
		 */
		if (ft->ftu_tcpfm &&
		    ft->ftu_tcpf != (fin->fin_tcpf & ft->ftu_tcpfm)) {
			FR_DEBUG(("f. %#x & %#x != %#x\n", fin->fin_tcpf,
				 ft->ftu_tcpfm, ft->ftu_tcpf));
			err = 0;
		}
	}
	return err;
}

/*
 * Check the input/output list of rules for a match and result.
 * Could be per interface, but this gets real nasty when you don't have
 * kernel sauce.
 */
int fr_scanlist(passin, ip, fin, m)
u_32_t passin;
ip_t *ip;
register fr_info_t *fin;
void *m;
{
	register struct frentry *fr;
	register fr_ip_t *fi = &fin->fin_fi;
	int rulen, portcmp = 0, off, skip = 0, logged = 0;
	u_32_t pass, passt =0, passl=0;
	frentry_t *frl;

	frl = NULL;
	pass = passin;
	fr = fin->fin_fr;
	fin->fin_fr = NULL;
	off = fin->fin_off;
	
	if ((fi->fi_fl & FI_TCPUDP) && (fin->fin_dlen > 3) && !off)
		portcmp = 1;

	for (rulen = 0; fr; fr = fr->fr_next, rulen++) {
		if (skip) {
			FR_VERBOSE(("%d (%#x)\n", skip, fr->fr_flags));
			skip--;
			continue;
		}
		/* 25/12/03 Roger ++,  */	
		if(fr->fr_flags & FR_INACTIVE_SCHED)
			continue;
		
		/* 25/12/03 Roger --,  */	
		/*
		 * In all checks below, a null (zero) value in the
		 * filter struture is taken to mean a wildcard.
		 *
		 * check that we are working for the right interface
		 */
#ifdef	_KERNEL
# if	(BSD >= 199306)
		if (fin->fin_out != 0) {
			if ((fr->fr_oifa &&
			     (fr->fr_oifa != ((mb_t *)m)->m_pkthdr.rcvif)))
				continue;
		}
# endif
#else
		if (opts & (OPT_VERBOSE|OPT_DEBUG))
			printf("\n");
#endif

		FR_VERBOSE(("%c", fr->fr_skip ? 's' :
				  (pass & FR_PASS) ? 'p' : 
				  (pass & FR_AUTH) ? 'a' :
				  (pass & FR_ACCOUNT) ? 'A' :
				  (pass & FR_NOMATCH) ? 'n' : 'b'));

		if (fr->fr_ifa && fr->fr_ifa != fin->fin_ifp)
			continue;

		FR_VERBOSE((":i"));
		{
			register u_32_t	*ld, *lm, *lip;
			register int i;

			lip = (u_32_t *)fi;
			lm = (u_32_t *)&fr->fr_mip;
			ld = (u_32_t *)&fr->fr_ip;
			i = ((*lip & *lm) != *ld);
			FR_DEBUG(("0. %#08x & %#08x != %#08x\n",
				   *lip, *lm, *ld));
			if (i)
				continue;
			/*
			 * We now know whether the packet version and the
			 * rule version match, along with protocol, ttl and
			 * tos.
			 */
			lip++, lm++, ld++;
			/*
			 * Unrolled loops (4 each, for 32 bits).
			 */
			FR_DEBUG(("1a. %#08x & %#08x != %#08x\n",
				   *lip, *lm, *ld));
			/* 24/12/03 Roger ++,  check whether in the scope, not support ipv6*/	
			if(*(u_32_t *)&(fr->fr_tsrc)) {
				i |= ((ntohl(*lip) < ntohl(*ld)) || (ntohl(*lip) > ntohl(*(u_32_t *)&(fr->fr_tsrc)))) << 5;
				lip += 4;
				lm += 4;
				ld += 4;
			} else {
			/* 24/12/03 Roger --,  */	
			i |= ((*lip++ & *lm++) != *ld++) << 5;
			if (fi->fi_v == 6) {
				FR_DEBUG(("1b. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 5;
				FR_DEBUG(("1c. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 5;
				FR_DEBUG(("1d. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 5;
			} else {
				lip += 3;
				lm += 3;
				ld += 3;
			}
			} /* 24/12/03 Roger,  add*/	
			i ^= (fr->fr_flags & FR_NOTSRCIP);
			if (i)
				continue;
			FR_DEBUG(("2a. %#08x & %#08x != %#08x\n",
				   *lip, *lm, *ld));
			/* 24/12/03 Roger ++,  check whether in the scope, not support ipv6*/	
			if(*(u_32_t *)&(fr->fr_tdst)) {
				i |= ((ntohl(*lip) < ntohl(*ld)) || (ntohl(*lip) > ntohl(*(u_32_t *)&(fr->fr_tdst)))) << 6;
				lip += 4;
				lm += 4;
				ld += 4;
			} else {
			/* 24/12/03 Roger --,  */	
			i |= ((*lip++ & *lm++) != *ld++) << 6;
			if (fi->fi_v == 6) {
				FR_DEBUG(("2b. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 6;
				FR_DEBUG(("2c. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 6;
				FR_DEBUG(("2d. %#08x & %#08x != %#08x\n",
					   *lip, *lm, *ld));
				i |= ((*lip++ & *lm++) != *ld++) << 6;
			} else {
				lip += 3;
				lm += 3;
				ld += 3;
			}
			}/* 24/12/03 Roger,  add*/
			i ^= (fr->fr_flags & FR_NOTDSTIP);
			if (i)
				continue;
			FR_DEBUG(("3. %#08x & %#08x != %#08x\n",
				   *lip, *lm, *ld));
			i |= ((*lip++ & *lm++) != *ld++);
			FR_DEBUG(("4. %#08x & %#08x != %#08x\n",
				   *lip, *lm, *ld));
			i |= ((*lip & *lm) != *ld);
			if (i)
				continue;
		}
		/*
		 * If a fragment, then only the first has what we're looking
		 * for here...
		 */
		if (!portcmp && (fr->fr_dcmp || fr->fr_scmp || fr->fr_tcpf ||
				 fr->fr_tcpfm))
			continue;
		if (fi->fi_fl & FI_TCPUDP) {
			if (!fr_tcpudpchk(&fr->fr_tuc, fin))
				continue;
		} else if (fr->fr_icmpm || fr->fr_icmp) {
			if (((fi->fi_p != IPPROTO_ICMP) &&
			    (fi->fi_p != IPPROTO_ICMPV6)) || off ||
			    (fin->fin_dlen < 2))
				continue;
			if ((fin->fin_data[0] & fr->fr_icmpm) != fr->fr_icmp) {
				FR_DEBUG(("i. %#x & %#x != %#x\n",
					 fin->fin_data[0], fr->fr_icmpm,
					 fr->fr_icmp));
				continue;
			}
		}
		FR_VERBOSE(("*"));

		if (fr->fr_flags & FR_NOMATCH) {
			passt = passl;
			passl = passin;
			fin->fin_fr = frl;
			frl = NULL;
			if (fr->fr_flags & FR_QUICK)
				break;
			continue;
		}

		passl = passt;
		passt = fr->fr_flags;
		frl = fin->fin_fr;
		fin->fin_fr = fr;
#if (BSD >= 199306) && (defined(_KERNEL) || defined(KERNEL))
//		if (securelevel <= 0) /* 29/10/03 Roger,  */	
#endif
			if ((passt & FR_CALLNOW) && fr->fr_func)
				passt = (*fr->fr_func)(passt, ip, fin);
#ifdef  IPFILTER_LOG
		/*
		 * Just log this packet...
		 */
		if ((passt & FR_LOGMASK) == FR_LOG) {
			if (!IPLLOG(passt, ip, fin, m)) {
				if (passt & FR_LOGORBLOCK)
					passt |= FR_BLOCK|FR_QUICK;
				ATOMIC_INCL(frstats[fin->fin_out].fr_skip);
			}
			ATOMIC_INCL(frstats[fin->fin_out].fr_pkl);
			logged = 1;
		}
#endif /* IPFILTER_LOG */
		ATOMIC_INCL(fr->fr_hits);
		if (passt & FR_ACCOUNT)
			fr->fr_bytes += (U_QUAD_T)ip->ip_len;
		else
			fin->fin_icode = fr->fr_icode;
		fin->fin_rule = rulen;
		fin->fin_group = fr->fr_group;
		if (fr->fr_grp != NULL) {
			fin->fin_fr = fr->fr_grp;
			passt = fr_scanlist(passt, ip, fin, m);
			if (fin->fin_fr == NULL) {
				fin->fin_rule = rulen;
				fin->fin_group = fr->fr_group;
				fin->fin_fr = fr;
			}
			if (passt & FR_DONTCACHE)
				logged = 1;
		}
		if (!(skip = fr->fr_skip) && (passt & FR_LOGMASK) != FR_LOG)
			pass = passt;
		FR_DEBUG(("pass %#x\n", pass));
		if (passt & FR_QUICK)
			break;
	}
	if (logged)
		pass |= FR_DONTCACHE;
	pass |= (fi->fi_fl << 24);
	return pass;
}


#ifdef CONFIG_TCP_DROP_ILL_SYN_ACK
int fr_block_ill_tcp_syn_ack(ip, hlen)
ip_t *ip;
int hlen;
{
	tcphdr_t *tcp;
	
	tcp = (tcphdr_t *) ((char *)ip + hlen);
	
	if ((tcp->th_flags & TH_SYN) && 
		(tcp->th_flags & TH_ACK) && 
		(htonl(tcp->th_seq)==1)) {

		/*  attacker's false SYN&ACK frame  */
		return 1;
	}
	
	return 0;
}
#endif


#ifdef CONFIG_DROP_ILL_HTTP_RESP
#define FR_ATTACKER_IP_ID		1691
#define FR_ATTACKER_MIN_PAYLOAD_LEN	64
int fr_block_ill_http_response(ip, hlen)
ip_t *ip;
int hlen;
{
	tcphdr_t *tcph;
	u_char *phttp;

	if (ip->ip_id != FR_ATTACKER_IP_ID)
		return 0;

#if 1
	tcph = (tcphdr_t *) ((char *)ip + hlen);
	if ((tcph->th_sport == 80) && (tcph->th_flags & TH_FIN) && ((ip->ip_len -hlen) > FR_ATTACKER_MIN_PAYLOAD_LEN)) {
		phttp = (u_char *)tcph + (tcph->th_off << 2);
		if (strncmp("HTTP/1.0 200 OK\r\nServer: MySocket Server\r\n", phttp, 42) == 0) {
			/*  attacker's http response  */
			return 1;
		}
	}
#else
	if ((ip->ip_ttl > 62) || (ip->ip_ttl < 60))
		return 0;
	tcph = (tcphdr_t *) ((char *)ip + hlen);
	if ((tcph->th_sport == 80) && (tcph->th_flags & TH_FIN) && ((ip->ip_len -hlen) > FR_ATTACKER_MIN_PAYLOAD_LEN)) {
		/*  attacker's http response  */
		return 1;
}
#endif
	return 0;
}

#endif


#ifdef CONFIG_TCP_BLOCK_NETSNIPER
#define FR_ATTACKER_MIN_HTTP_LEN	64
static int fr_block_ill_netsniper_frame(ip, hlen)
ip_t *ip;
int hlen;
{
	tcphdr_t *tcph;
	u_char *phttp, *p, *pend;
	int pk_len;

	tcph = (tcphdr_t *) ((char *)ip + hlen);
	
	if (tcph->th_flags & TH_RST) {
		/*  Reset frame  */
		if (!(tcph->th_flags & TH_ACK) && (tcph->th_ack != 0)) {
			/*  Attacker's reset frame  */
			return 1;
		}
	} else {
		pk_len = ip->ip_len - hlen - (tcph->th_off << 2);
		if ((tcph->th_sport == 80) && (tcph->th_flags & TH_FIN) && (pk_len > FR_ATTACKER_MIN_HTTP_LEN)) {
			phttp = (u_char *)tcph + (tcph->th_off << 2);
			if (strncmp("HTTP/1.1 302", phttp, 12) != 0)
				return 0;
			
			pend = phttp + pk_len;
			
			/*  Locate next line  */
			for (p=phttp+12; p<pend; p++) {
				if ((p[0] == '\r') && (p[1] == '\n')) {
					p += 2;
					break;
				}
			}
			if (p >= pend)
				return 0;
			if (strncmp("Location: http://", p, 17) != 0)
				return 0;
			for (p+=17; p<pend; p++)
				if (*p == '/')
					break;
			if (p >= pend)
				return 0;
			if (strncmp("/bmp/account/", p, 13) == 0) {
				/*  Attacker's redirect frame  */
				return 1;
			}
			
		}
	}
	return 0;
}
#endif



#ifdef CONFIG_NAPT_IP_TUPLE
int fr_ip5tup_match(ip)
ip_t *ip;
{
    int i=0;
    struct icmp *icmphdr= NULL;
    tcphdr_t *tcphdr= NULL;
    udphdr_t *udphdr= NULL;
    u_short ohlen = ip->ip_hl << 2;
    
    for (; i < MAX_IP_5_TUP_NUM; i ++) {
        if (iptup[i].bused == 0) {
            continue;
        }
        if (iptup[i].dip.s_addr == ip->ip_src.s_addr && 
            iptup[i].sip.s_addr == ip->ip_dst.s_addr &&
            iptup[i].proto == ip->ip_p) {

            if (ip->ip_p == IPPROTO_ICMP) {
                icmphdr = (struct icmp *)((u_char *)ip + ohlen);
                if (icmphdr->icmp_id == iptup[i].sport) {
                    break;
                }
            }
            else if (ip->ip_p == IPPROTO_TCP) {
                tcphdr = (tcphdr_t *)((u_char *)ip + ohlen);
                if (tcphdr->th_sport == iptup[i].dport && tcphdr->th_dport == iptup[i].sport) {
                    break;
                }
            }
            else if (ip->ip_p == IPPROTO_UDP) {
                udphdr = (udphdr_t *)((u_char *)ip + ohlen);
                if (udphdr->uh_sport == iptup[i].dport && udphdr->uh_dport == iptup[i].sport) {
                    break;
                }
            }
        }
    }
    
    if (i >= MAX_IP_5_TUP_NUM) {
        /* not match */
        return (-1);
    }
    
    /* match */
    return 0;
    
}


int fr_ip5tup_add(ip)
ip_t *ip;
{
    int i = 0;
    struct icmp *icmphdr= NULL;
    tcphdr_t *tcphdr= NULL;
    udphdr_t *udphdr= NULL;
    u_short ohlen = ip->ip_hl << 2;

    for (i = 0; i < MAX_IP_5_TUP_NUM; i ++) {
        if (iptup[i].bused == 1) {
            if (iptup[i].dip.s_addr == ip->ip_dst.s_addr && iptup[i].sip.s_addr == ip->ip_src.s_addr) {
                if (ip->ip_p == IPPROTO_ICMP) {
                    icmphdr = (struct icmp *)((u_char *)ip + ohlen);
                    if (iptup[i].sport == icmphdr->icmp_id) {
                        iptup[i].timeout = MAX_IP_5_TUPLE_TIMEOUT;
                        return 0;
                    }
                }
                else if (ip->ip_p == IPPROTO_TCP) {
                    tcphdr = (tcphdr_t *)((u_char *)ip + ohlen);
                    if (iptup[i].sport == tcphdr->th_sport &&
                        iptup[i].dport == tcphdr->th_dport) {
                        iptup[i].timeout = MAX_IP_5_TUPLE_TIMEOUT;
                        return 0;
                    }
                }
                else if (ip->ip_p == IPPROTO_UDP) {
                    udphdr = (udphdr_t *)((u_char *)ip + ohlen);
                    if (iptup[i].sport == udphdr->uh_sport &&
                        iptup[i].dport == udphdr->uh_dport) {
                        iptup[i].timeout = MAX_IP_5_TUPLE_TIMEOUT;
                        return 0;
                    }
                }
            }
        }
    }
    
    for (i = 0; i < MAX_IP_5_TUP_NUM; i ++) {
        if (iptup[i].bused == 0) {

            iptup[i].dip.s_addr = ip->ip_dst.s_addr;
            iptup[i].sip.s_addr = ip->ip_src.s_addr;
            iptup[i].proto = ip->ip_p;
            if (ip->ip_p == IPPROTO_ICMP) {
                icmphdr = (struct icmp *)((u_char *)ip + ohlen);
                iptup[i].sport = icmphdr->icmp_id;
            }
            else if (ip->ip_p == IPPROTO_TCP) {
                tcphdr = (tcphdr_t *)((u_char *)ip + ohlen);
                iptup[i].sport = tcphdr->th_sport;
                iptup[i].dport = tcphdr->th_dport;
            }
            else if (ip->ip_p == IPPROTO_UDP) {
                udphdr = (udphdr_t *)((u_char *)ip + ohlen);
                iptup[i].sport = udphdr->uh_sport;
                iptup[i].dport = udphdr->uh_dport;
            }
            iptup[i].bused = 1;
            iptup[i].timeout = MAX_IP_5_TUPLE_TIMEOUT;
            break;
        }
    }
    return 0;
}


/*
 * fr_ip5tup_timeout - aging out the ip tuple item .
 */
void fr_ip5tup_timeout(void)
{
    int i = 0;
    int cnt = 0;

    for (i = 0; i < MAX_IP_5_TUP_NUM; i ++)
    {
        /* no pkt using the ip 5 tuple, aging out it */
        if (iptup[i].bused)
        {
            cnt ++;
            if (iptup[i].timeout) {
                iptup[i].timeout --;
            }

            if (0 == iptup[i].timeout) {
                bzero((char *)&iptup[i], sizeof(ip5tup_t));
            }
        }        
    }
    
    timeout((timeout_fun *)fr_ip5tup_timeout, (void *)NULL, 100);
    
    return;
}

#endif




/*
 * frcheck - filter check
 * check using source and destination addresses/ports in a packet whether
 * or not to pass it on or not.
 */
int fr_check(ip, hlen, ifp, out, mp)
mb_t **mp;
ip_t *ip;
int hlen;
void *ifp;
int out;
{
	/*
	 * The above really sucks, but short of writing a diff
	 */
	fr_info_t frinfo, *fc;
	register fr_info_t *fin = &frinfo;
	int changed, error = EHOSTUNREACH, v = ip->ip_v;
	frentry_t *fr = NULL, *list;
	u_32_t pass, apass;
	register mb_t *m = *mp;
	int p, len, drop = 0, logit = 0;
	mb_t *mc = NULL;
	int up;
	tcphdr_t *tcp;
	u_short dport = 0;
	int pppoe_mode = 0;
	int wan_mode = 0;
		
#  ifdef M_CANFASTFWD
	/*
	 * XXX For now, IP Filter and fast-forwarding of cached flows
	 * XXX are mutually exclusive.  Eventually, IP Filter should
	 * XXX get a "can-fast-forward" filter rule.
	 */
	m->m_flags &= ~M_CANFASTFWD;
#  endif /* M_CANFASTFWD */
#  ifdef CSUM_DELAY_DATA
	/*
	 * disable delayed checksums.
	 */
	if ((out != 0) && (m->m_pkthdr.csum_flags & CSUM_DELAY_DATA)) {
		in_delayed_cksum(m);
		m->m_pkthdr.csum_flags &= ~CSUM_DELAY_DATA;
	}
#  endif /* CSUM_DELAY_DATA */

# ifdef	USE_INET6
	if (v == 6) {
		len = ntohs(((ip6_t*)ip)->ip6_plen);
		if (!len)
			return -1;	/* potential jumbo gram */
		len += sizeof(ip6_t);
		p = ((ip6_t *)ip)->ip6_nxt;
	} else
# endif
	{
		p = ip->ip_p;
		len = ip->ip_len;
	}

	if ((p == IPPROTO_TCP || p == IPPROTO_UDP ||
	    (v == 4 && p == IPPROTO_ICMP)
# ifdef USE_INET6
	    || (v == 6 && p == IPPROTO_ICMPV6)
# endif
	   )) {
		int plen = 0;

		if ((v == 6) || (ip->ip_off & IP_OFFMASK) == 0)
			switch(p)
			{
			case IPPROTO_TCP:
				plen = sizeof(tcphdr_t);
				break;
			case IPPROTO_UDP:
				plen = sizeof(udphdr_t);
				break;
			/* 96 - enough for complete ICMP error IP header */
			case IPPROTO_ICMP:
				plen = ICMPERR_MAXPKTLEN - sizeof(ip_t);
				break;
			case IPPROTO_ESP:
				plen = 8;
				break;
# ifdef USE_INET6
	    		case IPPROTO_ICMPV6 :
				/*
				 * XXX does not take intermediate header
				 * into account
				 */
				plen = ICMP6ERR_MINPKTLEN + 8 - sizeof(ip6_t);
				break;
# endif
			}
		up = MIN(hlen + plen, len);

		if (up > m->m_len) {
			if ((*mp = m_pullup(m, up)) == 0) {
				ATOMIC_INCL(frstats[out].fr_pull[1]);
				return -1;
			} else {
				ATOMIC_INCL(frstats[out].fr_pull[0]);
				m = *mp;
				ip = mtod(m, ip_t *);
			}
		} else
			up = 0;
	} else
		up = 0;
	
	changed = 0;
	fin->fin_ifp = ifp;
	fin->fin_v = v;
	fin->fin_out = out;
	fin->fin_mp = mp;
	fin->fin_nat = NULL;
		
	fr_makefrip(hlen, ip, fin);

# ifdef	USE_INET6
	if (v == 6) {
		ATOMIC_INCL(frstats[0].fr_ipv6[out]);
		if (((ip6_t *)ip)->ip6_hlim < fr_minttl) {
			ATOMIC_INCL(frstats[0].fr_badttl);
			if (fr_minttllog & 1)
				logit = -3;
			if (fr_minttllog & 2)
				drop = 1;
		}
	} else
# endif
	if (!out) {
		if (fr_chksrc && !fr_verifysrc(ip->ip_src, ifp)) {
			ATOMIC_INCL(frstats[0].fr_badsrc);
			if (fr_chksrc & 1)
				drop = 1;
			if (fr_chksrc & 2)
				logit = -2;
		} else if (ip->ip_ttl < fr_minttl) {
			ATOMIC_INCL(frstats[0].fr_badttl);
			if (fr_minttllog & 1)
				logit = -3;
			if (fr_minttllog & 2)
				drop = 1;
		}
		if((ifp == block_ifp) && check_attacker(fin))
			goto drop_frame;
	}
	if (drop) {
drop_frame:
# ifdef	IPFILTER_LOG
		if (logit) {
			fin->fin_group = logit;
			pass = FR_INQUE|FR_NOMATCH|FR_LOGB;
			(void) IPLLOG(pass, ip, fin, m);
		}
# endif
		m_freem(m);

		return error;
	}

	pass = fr_pass;
	if (fin->fin_fl & FI_SHORT) {
		ATOMIC_INCL(frstats[out].fr_short);
	}

	READ_ENTER(&ipf_mutex);

	/*
	 * Check auth now.  This, combined with the check below to see if apass
	 * is 0 is to ensure that we don't count the packet twice, which can
	 * otherwise occur when we reprocess it.  As it is, we only count it
	 * after it has no auth. table matchup.  This also stops NAT from
	 * occuring until after the packet has been auth'd.
	 */
	//apass = fr_checkauth(ip, fin); /*  no need to do authenication */	
	apass = 0;

	if (!out) {
#ifdef	USE_INET6
		if (v == 6)
			list = ipacct6[0][fr_active];
		else
#endif
			list = ipacct[0][fr_active];
		changed = ip_natin(ip, fin);
		if (!apass && (fin->fin_fr = list) &&
		    (fr_scanlist(FR_NOMATCH, ip, fin, m) & FR_ACCOUNT)) {
			ATOMIC_INCL(frstats[0].fr_acct);
		}
	}
	else
	{
		ip_natout_match(ip, fin);
	}

	if (!apass) {
		if ((fin->fin_fl & FI_FRAG) == FI_FRAG)
			fr = ipfr_knownfrag(ip, fin);
		if (!fr && !(fin->fin_fl & FI_SHORT))
		{
			//fr = fr_checkstate(ip, fin);
			fr = nat_checkstate(ip, fin);
		}
		if (fr != NULL)
			pass = fr->fr_flags;
		if (fr && (pass & FR_LOGFIRST))
			pass &= ~(FR_LOGFIRST|FR_LOG);
	}
	
	if (apass || !fr) {
		/*
		 * If a packet is found in the auth table, then skip checking
		 * the access lists for permission but we do need to consider
		 * the result as if it were from the ACL's.
		 */
		if (!apass) {
			fc = frcache + out;
			if (!bcmp((char *)fin, (char *)fc, FI_CSIZE)) {
				/*
				 * copy cached data so we can unlock the mutex
				 * earlier.
				 */
				bcopy((char *)fc, (char *)fin, FI_COPYSIZE);
				ATOMIC_INCL(frstats[out].fr_chit);
				if ((fr = fin->fin_fr)) {
					ATOMIC_INCL(fr->fr_hits);
					pass = fr->fr_flags;
				}
			} else {
#ifdef	USE_INET6
				if (v == 6)
					list = ipfilter6[out][fr_active];
				else
#endif
					list = ipfilter[out][fr_active];
				if ((fin->fin_fr = list))
					pass = fr_scanlist(fr_pass, ip, fin, m);
				if (!(pass & (FR_KEEPSTATE|FR_DONTCACHE)))
					bcopy((char *)fin, (char *)fc,
					      FI_COPYSIZE);
				if (pass & FR_NOMATCH) {
					ATOMIC_INCL(frstats[out].fr_nom);
					fin->fin_fr = NULL;
				}
			}
		} else
			pass = apass;
		fr = fin->fin_fr;

		/*
		 * If we fail to add a packet to the authorization queue,
		 * then we drop the packet later.  However, if it was added
		 * then pretend we've dropped it already.
		 */
#if 0 /* 12/04/13 Eddy  */	
		if ((pass & FR_AUTH)) {
			if (fr_newauth((mb_t *)m, fin, ip) != 0) {
				m = *mp = NULL;
				error = 0;
			} else
				error = ENOSPC;
		}

		if (pass & FR_PREAUTH) {
			READ_ENTER(&ipf_auth);
			if ((fin->fin_fr = ipauth) &&
			    (pass = fr_scanlist(0, ip, fin, m))) {
				ATOMIC_INCL(fr_authstats.fas_hits);
			} else {
				ATOMIC_INCL(fr_authstats.fas_miss);
			}
			RWLOCK_EXIT(&ipf_auth);
		}
#endif
		fin->fin_fr = fr;
		if ((pass & (FR_KEEPFRAG|FR_KEEPSTATE)) == FR_KEEPFRAG) {
			if (fin->fin_fl & FI_FRAG) {
				if (ipfr_newfrag(ip, fin) == -1) {
					ATOMIC_INCL(frstats[out].fr_bnfr);
				} else {
					ATOMIC_INCL(frstats[out].fr_nfr);
				}
			} else {
				ATOMIC_INCL(frstats[out].fr_cfr);
			}
		}
		//if (pass & FR_KEEPSTATE) {
		//	if (fr_addstate(ip, fin, NULL, 0) == NULL) {
		//		ATOMIC_INCL(frstats[out].fr_bads);
		//	} else {
		//		ATOMIC_INCL(frstats[out].fr_ads);
		//	}
		//}
		if (pass & FR_KEEPSTATE) {
			nat_addstate(ip, fin);
		}
	} else if (fr != NULL) {
		pass = fr->fr_flags;
		if (pass & FR_LOGFIRST)
			pass &= ~(FR_LOGFIRST|FR_LOG);
	}
		
		if (fr && fr->fr_func && !(pass & FR_CALLNOW))
			pass = (*fr->fr_func)(pass, ip, fin);

	/*
	 * Only count/translate packets which will be passed on, out the
	 * interface.
	 */
	if (out && (pass & FR_PASS)) {
#ifdef	USE_INET6
		if (v == 6)
			list = ipacct6[1][fr_active];
		else
#endif
			list = ipacct[1][fr_active];
		if (list != NULL) {
			u_32_t sg, sr;

			fin->fin_fr = list;
			sg = fin->fin_group;
			sr = fin->fin_rule;
			if (fr_scanlist(FR_NOMATCH, ip, fin, m) & FR_ACCOUNT) {
				ATOMIC_INCL(frstats[1].fr_acct);
			}
			fin->fin_group = sg;
			fin->fin_rule = sr;
			fin->fin_fr = fr;
		}
		changed = ip_natout_change(ip, fin);
	} else
		fin->fin_fr = fr;
	// 080409: add mss clamp to routing mode
	if (is_block_ifp(fin->fin_ifp))
		ip_natoff(ip, fin);
	/* maybe nat has been created but fw must block the traffic, like virtual server*/	
	if(!(pass & FR_PASS) && (pass & FR_BLOCK))
	{
		nat_t *nat;
		nat = fin->fin_nat;
		
		if(nat)
			nat->nat_age = 1;
	}
	RWLOCK_EXIT(&ipf_mutex);

#ifdef	IPFILTER_LOG
	if ((fr_flags & FF_LOGGING) || (pass & FR_LOGMASK)) {
		if ((fr_flags & FF_LOGNOMATCH) && (pass & FR_NOMATCH)) {
			pass |= FF_LOGNOMATCH;
			ATOMIC_INCL(frstats[out].fr_npkl);
			goto logit;
		} else if (((pass & FR_LOGMASK) == FR_LOGP) ||
		    ((pass & FR_PASS) && (fr_flags & FF_LOGPASS))) {
			if ((pass & FR_LOGMASK) != FR_LOGP)
				pass |= FF_LOGPASS;
			ATOMIC_INCL(frstats[out].fr_ppkl);
			goto logit;
		} else if (((pass & FR_LOGMASK) == FR_LOGB) ||
			   ((pass & FR_BLOCK) && (fr_flags & FF_LOGBLOCK))) {
			if ((pass & FR_LOGMASK) != FR_LOGB)
				pass |= FF_LOGBLOCK;
			ATOMIC_INCL(frstats[out].fr_bpkl);
logit:
			if (!IPLLOG(pass, ip, fin, m)) {
				ATOMIC_INCL(frstats[out].fr_skip);
				if ((pass & (FR_PASS|FR_LOGORBLOCK)) ==
				    (FR_PASS|FR_LOGORBLOCK))
					pass ^= FR_PASS|FR_BLOCK;
			}
		}
	}
#endif /* IPFILTER_LOG */


	/*
	 * Only allow FR_DUP to work if a rule matched - it makes no sense to
	 * set FR_DUP as a "default" as there are no instructions about where
	 * to send the packet.
	 */
	if (fr && (pass & FR_DUP))
		mc = m_copym(m, 0, M_COPYALL, M_DONTWAIT);
	
	// 2009-08-28: fix connect-on-demand for ping DNS
	CFG_get(CFG_POE_AUTO, &pppoe_mode);
	CFG_get(CFG_WAN_IP_MODE, &wan_mode);
	#ifdef	CONFIG_PPTP_PPPOE
	//Added by haitao,for pppoe-->pptp vpn case.
	int pptp_wanif = 0;
	CFG_get(CFG_PTP_WANIF, &pptp_wanif);
	if(wan_mode == PPTPMODE && pptp_wanif == 2)wan_mode=PPPOEMODE;
	#endif
	if (wan_mode == PPPOEMODE || wan_mode == DUALWAN_PPPOEMODE)
	{
		if (need_dialup_flag == 0 && pppoe_mode == 1)
		{
			if (fin->fin_p == IPPROTO_UDP)
			{
				tcp = (tcphdr_t *)fin->fin_dp;
				dport = tcp->th_dport;
				if (dport == 53)
					need_dialup_flag = 1;
			}
		}
	}
	
	/* 19/02/04 Roger ++,  force to block inbound packets which no rule match in WAN*/	
	if(	(!out) && 
		(!(fin->fin_nat)) && 
		is_block_ifp(ifp))
	{
		if(check_localaddr(ifp, fin->fin_daddr))
		{ //ours packets
			switch(fin->fin_p)
			{
				case IPPROTO_TCP:
				case IPPROTO_UDP:
					// 2009-04-15, NAT off, port checking needs to be done here.
					if (nat_enabled == 0)
					{
						tcp = (tcphdr_t *)fin->fin_dp;
						dport = tcp->th_dport;
						if (check_localport(fin->fin_daddr, dport, fin->fin_p) != 0)
							fin->fin_misc |= FM_OURSPORT;
					}
					if((fin->fin_misc & FM_OURSPORT) || (auth_port_open && (fin->fin_p == IPPROTO_TCP)&& (fin->fin_data[1] == 113)))
					{
						pass |= FR_PASS;
						pass &= ~FR_BLOCK;
					}
					else
					{// maybe attacker do port scan
						if(ip->ip_off == 0 && drop_portscan)
						{
							if(!(fin->fin_fl & FI_FRAG) || ((fin->fin_fl & FI_FRAG)&(fin->fin_off == 0)))
							{
								attacker_record(fin);
							}
							pass &= ~FR_PASS;
							pass |= FR_BLOCK;
						}
					}
					break;
				case IPPROTO_ICMP:
					if(DEF_SMURF_ATTACK && (!(fin->fin_fl & FI_FRAG) || ((fin->fin_fl & FI_FRAG)&(fin->fin_off == 0))))
					{
						struct icmp *icmp;
						
						icmp = (icmphdr_t *)fin->fin_dp;
						if((icmp->icmp_type == ICMP_ECHOREPLY) && (ping_id!=icmp->icmp_id))
						{
							pass &= ~FR_PASS;
							pass |= FR_BLOCK;
							security_log(fin, STR_Smurf, LOG_AUTH);
							break;
						}
					}
					break;
				default:
					pass |= FR_PASS;
					pass &= ~FR_BLOCK;
					break;
			}
		}
		else
		{
#ifdef CONFIG_NAPT_IP_TUPLE
    		u_short if_index;
			struct sockaddr_in addr;
			struct ifnet *lan = NULL;
			struct ifaddr *ia = NULL;
                    
            		addr.sin_addr.s_addr = ip->ip_dst.s_addr;
            		addr.sin_family = AF_INET;

            		/* the net of dst_ip equal to the net of lan,record the ip tuple  */
            		lan = ifunit("eth0");
            		if (lan) {
                		for (ia = lan->if_addrhead.tqh_first; ia;
                         		ia = ia->ifa_link.tqe_next) {
                        		if (((struct sockaddr_in *)ia->ifa_addr)->sin_family != addr.sin_family)
                            			continue;

                        		if ((ip->ip_dst.s_addr & ((struct sockaddr_in *)ia->ifa_netmask)->sin_addr.s_addr) == 
                            			(((struct sockaddr_in *)ia->ifa_addr)->sin_addr.s_addr & ((struct sockaddr_in *)ia->ifa_netmask)->sin_addr.s_addr)) {
                            			fr_ip5tup_add(ip);
                        		}
                		}
            		}
#else
			if(nat_enabled)
			{
				pass &= ~FR_PASS;
				pass |= FR_BLOCK;
			}
#endif
			
		}
	}
	/* 19/02/04 Roger --,  */	
	
	if (pass & FR_PASS) 
	{
		ATOMIC_INCL(frstats[out].fr_pass);
	}
	/* 17/12/04 Roger,  */	
	else if((pass & FR_SEC_LOG) && DEF_IP_SPOOFING && (fin->fin_fr &&fin->fin_fr->fr_seclog == 1))
	{
		security_log(fin, STR_IP_Spoofing, LOG_AUTH);
	} 
	else if (pass & FR_BLOCK) 
	{
		ATOMIC_INCL(frstats[out].fr_block);
		/*
		 * Should we return an ICMP packet to indicate error
		 * status passing through the packet filter ?
		 * WARNING: ICMP error packets AND TCP RST packets should
		 * ONLY be sent in repsonse to incoming packets.  Sending them
		 * in response to outbound packets can result in a panic on
		 * some operating systems.
		 */
		if (!out) {
			if (changed == -1)
				/*
				 * If a packet results in a NAT error, do not
				 * send a reset or ICMP error as it may disrupt
				 * an existing flow.  This is the proxy saying
				 * the content is bad so just drop the packet
				 * silently.
				 */
				;
			else if (pass & FR_RETICMP) {
				int dst;

				if ((pass & FR_RETMASK) == FR_FAKEICMP)
					dst = 1;
				else
					dst = 0;
				send_icmp_err(ip, ICMP_UNREACH, fin, dst);
				ATOMIC_INCL(frstats[0].fr_ret);
			} else if (((pass & FR_RETMASK) == FR_RETRST) &&
				   !(fin->fin_fl & FI_SHORT)) {
				if (send_reset(ip, fin) == 0) {
					ATOMIC_INCL(frstats[1].fr_ret);
				}
			}
		} else {
			if (pass & FR_RETRST)
				error = ECONNRESET;
		}
		security_log(fin, STR_FILTER, LOG_DROPPK);
	}
	
	/*
	 * If we didn't drop off the bottom of the list of rules (and thus
	 * the 'current' rule fr is not NULL), then we may have some extra
	 * instructions about what to do with a packet.
	 * Once we're finished return to our caller, freeing the packet if
	 * we are dropping it (* BSD ONLY *).
	 */
	if (((changed == -1)||(fin->fin_misc & FM_BADNAT)) && (pass & FR_PASS)) {
		pass &= ~FR_PASS;
		pass |= FR_BLOCK;
	}
#if 0 /* 07/10/04 Roger,  */	
	if (fr) {
		frdest_t *fdp = &fr->fr_tif;

		if (((pass & FR_FASTROUTE) && !out) ||
		    (fdp->fd_ifp && fdp->fd_ifp != (struct ifnet *)-1)) {
			(void) ipfr_fastroute(m, mp, fin, fdp);
			m = *mp;
		}

		if (mc != NULL)
			(void) ipfr_fastroute(mc, &mc, fin, &fr->fr_dif);
	}
#endif
	if (!(pass & FR_PASS) && m) {
		m_freem(m);
		m = *mp = NULL;
	}
#ifdef CONFIG_SUPERDMZ
	//else if(superdmz_mac && (!out) && fin->fin_nat && (ifp == block_ifp) && m)
	else if(superdmz_mac && m && (!out) && fin->fin_nat && (ip->ip_dst.s_addr == SYS_wan_ip))
	{
		nat_t *nat;
		
		nat = fin->fin_nat;
		if((nat->nat_flags & IPN_DMZ) == 0)
		{
			goto notsdmz;
		}
		
    	ip->ip_ttl -= IPTTLDEC;
    	NTOHS(ip->ip_len);
		NTOHS(ip->ip_off);
		NTOHS(ip->ip_id);
		ip->ip_sum = 0;
		ip->ip_sum = in_cksum(m, hlen);
		LIB_etherpacket_send(GETUNIT(SYS_lan_if, 4), m, superdmz_mac, 0, 0);
		m = *mp = NULL;
		return error;
	}
notsdmz:
#endif 

	return (pass & FR_PASS) ? 0 : error;
}


#ifdef CONFIG_SUPERDMZ
struct ifnet *check_staticif(struct ifnet *ifp, ip_t *ip)
{
	struct ifnet *sdmzifp = GETUNIT(SYS_lan_if, 4);
	
	if(superdmz_mac && (ip->ip_dst.s_addr == SYS_wan_ip) && (ifp !=sdmzifp))
		return sdmzifp;
	else
		return ifp;
}

int static_output(struct ifnet *ifp, mb_t *m, ip_t *ip)
{
	if(superdmz_mac && m && (ip->ip_dst.s_addr == SYS_wan_ip))
	{
		LIB_etherpacket_send(ifp, m, superdmz_mac, 0, 0);
		return 1;
	}
	return 0;
}
#endif
/*
 * ipf_cksum
 * addr should be 16bit aligned and len is in bytes.
 * length is in bytes
 */
u_short ipf_cksum(addr, len)
register u_short *addr;
register int len;
{
	register u_32_t sum = 0;

	for (sum = 0; len > 1; len -= 2)
		sum += *addr++;

	/* mop up an odd byte, if necessary */
	if (len == 1)
		sum += *(u_char *)addr;

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	return (u_short)(~sum);
}


/*
 * NB: This function assumes we've pullup'd enough for all of the IP header
 * and the TCP header.  We also assume that data blocks aren't allocated in
 * odd sizes.
 */
u_short fr_tcpsum(m, ip, tcp)
mb_t *m;
ip_t *ip;
tcphdr_t *tcp;
{
	u_short *sp, slen, ts;
	u_int sum, sum2;
	int hlen;

	/*
	 * Add up IP Header portion
	 */
	hlen = ip->ip_hl << 2;
	slen = ip->ip_len - hlen;
	sum = htons((u_short)ip->ip_p);
	sum += htons(slen);
	sp = (u_short *)&ip->ip_src;
	sum += *sp++;	/* ip_src */
	sum += *sp++;
	sum += *sp++;	/* ip_dst */
	sum += *sp++;
	ts = tcp->th_sum;
	tcp->th_sum = 0;
#ifdef	KERNEL
# if SOLARIS
	sum2 = ip_cksum(m, hlen, sum);	/* hlen == offset */
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	sum2 = ~sum2 & 0xffff;
# else /* SOLARIS */
#  if defined(BSD) || defined(sun)
#   if BSD >= 199306
	m->m_data += hlen;
#   else
	m->m_off += hlen;
#   endif
	m->m_len -= hlen;
	sum2 = in_cksum(m, slen);
	m->m_len += hlen;
#   if BSD >= 199306
	m->m_data -= hlen;
#   else
	m->m_off -= hlen;
#   endif
	/*
	 * Both sum and sum2 are partial sums, so combine them together.
	 */
	sum = (sum & 0xffff) + (sum >> 16);
	sum = ~sum & 0xffff;
	sum2 += sum;
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
#  else /* defined(BSD) || defined(sun) */
{
	union {
		u_char	c[2];
		u_short	s;
	} bytes;
	u_short len = ip->ip_len;
# if defined(__sgi)
	int add;
# endif

	/*
	 * Add up IP Header portion
	 */
	sp = (u_short *)&ip->ip_src;
	len -= (ip->ip_hl << 2);
	sum = ntohs(IPPROTO_TCP);
	sum += htons(len);
	sum += *sp++;	/* ip_src */
	sum += *sp++;
	sum += *sp++;	/* ip_dst */
	sum += *sp++;
	if (sp != (u_short *)tcp)
		sp = (u_short *)tcp;
	sum += *sp++;	/* sport */
	sum += *sp++;	/* dport */
	sum += *sp++;	/* seq */
	sum += *sp++;
	sum += *sp++;	/* ack */
	sum += *sp++;
	sum += *sp++;	/* off */
	sum += *sp++;	/* win */
	sum += *sp++;	/* Skip over checksum */
	sum += *sp++;	/* urp */

# ifdef	__sgi
	/*
	 * In case we had to copy the IP & TCP header out of mbufs,
	 * skip over the mbuf bits which are the header
	 */
	if ((caddr_t)ip != mtod(m, caddr_t)) {
		hlen = (caddr_t)sp - (caddr_t)ip;
		while (hlen) {
			add = MIN(hlen, m->m_len);
			sp = (u_short *)(mtod(m, caddr_t) + add);
			hlen -= add;
			if (add == m->m_len) {
				m = m->m_next;
				if (!hlen) {
					if (!m)
						break;
					sp = mtod(m, u_short *);
				}
				PANIC((!m),("fr_tcpsum(1): not enough data"));
			}
		}
	}
# endif

	if (!(len -= sizeof(*tcp)))
		goto nodata;
	while (len > 1) {
		if (((caddr_t)sp - mtod(m, caddr_t)) >= m->m_len) {
			m = m->m_next;
			PANIC((!m),("fr_tcpsum(2): not enough data"));
			sp = mtod(m, u_short *);
		}
		if (((caddr_t)(sp + 1) - mtod(m, caddr_t)) > m->m_len) {
			bytes.c[0] = *(u_char *)sp;
			m = m->m_next;
			PANIC((!m),("fr_tcpsum(3): not enough data"));
			sp = mtod(m, u_short *);
			bytes.c[1] = *(u_char *)sp;
			sum += bytes.s;
			sp = (u_short *)((u_char *)sp + 1);
		}
		if ((u_long)sp & 1) {
			bcopy((char *)sp++, (char *)&bytes.s, sizeof(bytes.s));
			sum += bytes.s;
		} else
			sum += *sp++;
		len -= 2;
	}
	if (len)
		sum += ntohs(*(u_char *)sp << 8);
nodata:
	while (sum > 0xffff)
		sum = (sum & 0xffff) + (sum >> 16);
	sum2 = (u_short)(~sum & 0xffff);
}
#  endif /*  defined(BSD) || defined(sun) */
# endif /* SOLARIS */
#else /* KERNEL */
	for (; slen > 1; slen -= 2)
		sum += *sp++;
	if (slen)
		sum += ntohs(*(u_char *)sp << 8);
	while (sum > 0xffff)
		sum = (sum & 0xffff) + (sum >> 16);
	sum2 = (u_short)(~sum & 0xffff);
#endif /* KERNEL */
	tcp->th_sum = ts;
	return sum2;
}


frgroup_t *fr_findgroup(num, flags, which, set, fgpp)
u_32_t num, flags;
minor_t which;
int set;
frgroup_t ***fgpp;
{
	frgroup_t *fg, **fgp;

	if (which == IPL_LOGAUTH)
		fgp = &ipfgroups[2][set];
	else if (flags & FR_ACCOUNT)
		fgp = &ipfgroups[1][set];
	else if (flags & (FR_OUTQUE|FR_INQUE))
		fgp = &ipfgroups[0][set];
	else
		return NULL;

	while ((fg = *fgp))
		if (fg->fg_num == num)
			break;
		else
			fgp = &fg->fg_next;
	if (fgpp)
		*fgpp = fgp;
	return fg;
}


frgroup_t *fr_addgroup(num, fp, which, set)
u_32_t num;
frentry_t *fp;
minor_t which;
int set;
{
	frgroup_t *fg, **fgp;

	if ((fg = fr_findgroup(num, fp->fr_flags, which, set, &fgp)))
		return fg;

	KMALLOC(fg, frgroup_t *);
	if (fg) {
		fg->fg_num = num;
		fg->fg_next = *fgp;
		fg->fg_head = fp;
		fg->fg_start = &fp->fr_grp;
		*fgp = fg;
	}
	return fg;
}


void fr_delgroup(num, flags, which, set)
u_32_t num, flags;
minor_t which;
int set;
{
	frgroup_t *fg, **fgp;
 
	if (!(fg = fr_findgroup(num, flags, which, set, &fgp)))
		return;
 
	*fgp = fg->fg_next;
	KFREE(fg);
}



/*
 * recursively flush rules from the list, descending groups as they are
 * encountered.  if a rule is the head of a group and it has lost all its
 * group members, then also delete the group reference.
 */
static int frflushlist(set, unit, nfreedp, listp)
int set;
minor_t unit;
int *nfreedp;
frentry_t **listp;
{
	register int freed = 0, i;
	register frentry_t *fp;

	while ((fp = *listp)) {
		*listp = fp->fr_next;
		if (fp->fr_grp) {
			i = frflushlist(set, unit, nfreedp, &fp->fr_grp);
			MUTEX_ENTER(&ipf_rw);
			fp->fr_ref -= i;
			MUTEX_EXIT(&ipf_rw);
		}

		ATOMIC_DEC32(fp->fr_ref);
		if (fp->fr_grhead) {
			fr_delgroup(fp->fr_grhead, fp->fr_flags, 
				    unit, set);
			fp->fr_grhead = 0;
		}
		if (fp->fr_ref == 0) {
			KFREE(fp);
			freed++;
		} else
			fp->fr_next = NULL;
	}
	*nfreedp += freed;
	return freed;
}


int frflush(unit, proto, flags)
minor_t unit;
int proto, flags;
{
	int flushed = 0, set;

	if (unit != IPL_LOGIPF)
		return 0;
	WRITE_ENTER(&ipf_mutex);
	bzero((char *)frcache, sizeof(frcache[0]) * 2);

	set = fr_active;
	if (flags & FR_INACTIVE)
		set = 1 - set;

	if (flags & FR_OUTQUE) {
#ifdef	USE_INET6
		if (proto == 0 || proto == 6) {
			(void) frflushlist(set, unit,
					   &flushed, &ipfilter6[1][set]);
			(void) frflushlist(set, unit,
					   &flushed, &ipacct6[1][set]);
		}
#endif
		if (proto == 0 || proto == 4) {
			(void) frflushlist(set, unit,
					   &flushed, &ipfilter[1][set]);
			(void) frflushlist(set, unit,
					   &flushed, &ipacct[1][set]);
		}
	}
	if (flags & FR_INQUE) {
#ifdef	USE_INET6
		if (proto == 0 || proto == 6) {
			(void) frflushlist(set, unit,
					    &flushed, &ipfilter6[0][set]);
			(void) frflushlist(set, unit,
					   &flushed, &ipacct6[0][set]);
		}
#endif
		if (proto == 0 || proto == 4) {
			(void) frflushlist(set, unit,
					   &flushed, &ipfilter[0][set]);
			(void) frflushlist(set, unit,
					   &flushed, &ipacct[0][set]);
		}
	}
	RWLOCK_EXIT(&ipf_mutex);
	return flushed;
}


char *memstr(src, dst, slen, dlen)
char *src, *dst;
int slen, dlen;
{
	char *s = NULL;

	while (dlen >= slen) {
		if (bcmp(src, dst, slen) == 0) {
			s = dst;
			break;
		}
		dst++;
		dlen--;
	}
	return s;
}


void fixskip(listp, rp, addremove)
frentry_t **listp, *rp;
int addremove;
{
	frentry_t *fp;
	int rules = 0, rn = 0;

	for (fp = *listp; fp && (fp != rp); fp = fp->fr_next, rules++)
		;

	if (!fp)
		return;

	for (fp = *listp; fp && (fp != rp); fp = fp->fr_next, rn++)
		if (fp->fr_skip && (rn + fp->fr_skip >= rules))
			fp->fr_skip += addremove;
}

/*
 * count consecutive 1's in bit mask.  If the mask generated by counting
 * consecutive 1's is different to that passed, return -1, else return #
 * of bits.
 */
int	countbits(ip)
u_32_t	ip;
{
	u_32_t	ipn;
	int	cnt = 0, i, j;

	ip = ipn = ntohl(ip);
	for (i = 32; i; i--, ipn *= 2)
		if (ipn & 0x80000000)
			cnt++;
		else
			break;
	ipn = 0;
	for (i = 32, j = cnt; i; i--, j--) {
		ipn *= 2;
		if (j > 0)
			ipn++;
	}
	if (ipn == ip)
		return cnt;
	return -1;
}


/*
 * return the first IP Address associated with an interface
 */
int fr_ifpaddr(v, ifptr, inp)
int v;
void *ifptr;
struct in_addr *inp;
{
# ifdef	USE_INET6
	struct in6_addr *inp6 = NULL;
# endif
	struct ifnet *ifp = ifptr;
	struct in_addr in;
	struct sockaddr_in *sin;
	struct ifaddr *ifa;

#   if	(__FreeBSD_version >= 300000)
	ifa = TAILQ_FIRST(&ifp->if_addrhead);
#   else
#    if defined(__NetBSD__) || defined(__OpenBSD__)
	ifa = ifp->if_addrlist.tqh_first;
#    else
	ifa = ifp->if_addrlist;

#    endif /* __NetBSD__ || __OpenBSD__ */
#   endif /* __FreeBSD_version >= 300000 */
#   if (BSD < 199306) && !(/*IRIX6*/defined(__sgi) && defined(IFF_DRVRLOCK))
	sin = (struct sockaddr_in *)&ifa->ifa_addr;
#   else
	sin = (struct sockaddr_in *)ifa->ifa_addr;
	while (sin && ifa) {
		if ((v == 4) && (sin->sin_family == AF_INET))
			break;
#    ifdef USE_INET6
		if ((v == 6) && (sin->sin_family == AF_INET6)) {
			inp6 = &((struct sockaddr_in6 *)sin)->sin6_addr;
			if (!IN6_IS_ADDR_LINKLOCAL(inp6) &&
			    !IN6_IS_ADDR_LOOPBACK(inp6))
				break;
		}
#    endif
#    if	(__FreeBSD_version >= 300000)
		ifa = TAILQ_NEXT(ifa, ifa_link);
#    else
#     if defined(__NetBSD__) || defined(__OpenBSD__)
		ifa = ifa->ifa_list.tqe_next;
#     else
		ifa = ifa->ifa_next;
#     endif
#    endif /* __FreeBSD_version >= 300000 */
		if (ifa)
			sin = (struct sockaddr_in *)ifa->ifa_addr;
	}
	if (ifa == NULL)
		sin = NULL;
	if (sin == NULL)
		return -1;
#   endif /* (BSD < 199306) && (!__sgi && IFF_DRVLOCK) */
#    ifdef	USE_INET6
	if (v == 6)
		bcopy((char *)inp6, (char *)inp, sizeof(*inp6));
	else
#    endif
	{
		in = sin->sin_addr;
		*inp = in;
	}

	return 0;
}


static void frsynclist(fr)
register frentry_t *fr;
{
	for (; fr; fr = fr->fr_next) {
		if (fr->fr_ifa != NULL) {
			fr->fr_ifa = GETUNIT(fr->fr_ifname, fr->fr_ip.fi_v);
			if (fr->fr_ifa == NULL)
				fr->fr_ifa = (void *)-1;
		}
		if (fr->fr_grp)
			frsynclist(fr->fr_grp);
	}
}

#if 0 /* 27/07/04 Roger,  */	
void frsync()
{
# if !SOLARIS
	register struct ifnet *ifp;

#  if defined(__OpenBSD__) || ((NetBSD >= 199511) && (NetBSD < 1991011)) || \
     (defined(__FreeBSD_version) && (__FreeBSD_version >= 300000))
#   if (NetBSD >= 199905) || defined(__OpenBSD__)
	for (ifp = ifnet.tqh_first; ifp; ifp = ifp->if_list.tqe_next)
#   else
	for (ifp = ifnet.tqh_first; ifp; ifp = ifp->if_link.tqe_next)
#   endif
#  else
	for (ifp = ifnet; ifp; ifp = ifp->if_next)
#  endif
	{
		ip_natsync(ifp);
		ip_statesync(ifp);
	}
	ip_natsync((struct ifnet *)-1);
# endif /* !SOLARIS */

	WRITE_ENTER(&ipf_mutex);
	frsynclist(ipacct[0][fr_active]);
	frsynclist(ipacct[1][fr_active]);
	frsynclist(ipfilter[0][fr_active]);
	frsynclist(ipfilter[1][fr_active]);
#ifdef	USE_INET6
	frsynclist(ipacct6[0][fr_active]);
	frsynclist(ipacct6[1][fr_active]);
	frsynclist(ipfilter6[0][fr_active]);
	frsynclist(ipfilter6[1][fr_active]);
#endif
	RWLOCK_EXIT(&ipf_mutex);
}
#endif

/*
 * In the functions below, bcopy() is called because the pointer being
 * copied _from_ in this instance is a pointer to a char buf (which could
 * end up being unaligned) and on the kernel's local stack.
 */
int ircopyptr(a, b, c)
void *a, *b;
size_t c;
{
	caddr_t ca;
	int err;

	bcopy(a, &ca, sizeof(ca));
	
	if(!ca){diag_printf("\nError:ircopyptr source NULL\n");return EFAULT;}
	if(!b){diag_printf("\nError:ircopyptr dest NULL\n");return EFAULT;}
	
	err = copyin(ca, b, c);
	if (err)
		err = EFAULT;
	return err;
}


int iwcopyptr(a, b, c)
void *a, *b;
size_t c;
{
	caddr_t ca;
	int err;

	bcopy(b, &ca, sizeof(ca));
	
	if(!a){diag_printf("\nError:ircopyptr source NULL\n");return EFAULT;}
	if(!ca){diag_printf("\nError:ircopyptr dest NULL\n");return EFAULT;}
	
	err = copyout(a, ca, c);
	if (err)
		err = EFAULT;
	return err;
}


int fr_lock(data, lockp)
caddr_t data;
int *lockp;
{
	int arg, error;

	error = IRCOPY(data, (caddr_t)&arg, sizeof(arg));
	if (!error) {
		error = IWCOPY((caddr_t)lockp, data, sizeof(*lockp));
		if (!error)
			*lockp = arg;
	}
	return error;
}


void fr_getstat(fiop)
friostat_t *fiop;
{
	bcopy((char *)frstats, (char *)fiop->f_st, sizeof(filterstats_t) * 2);
	fiop->f_locks[0] = fr_state_lock;
	fiop->f_locks[1] = fr_nat_lock;
	fiop->f_locks[2] = fr_frag_lock;
	//fiop->f_locks[3] = fr_auth_lock;
	fiop->f_fin[0] = ipfilter[0][0];
	fiop->f_fin[1] = ipfilter[0][1];
	fiop->f_fout[0] = ipfilter[1][0];
	fiop->f_fout[1] = ipfilter[1][1];
	fiop->f_acctin[0] = ipacct[0][0];
	fiop->f_acctin[1] = ipacct[0][1];
	fiop->f_acctout[0] = ipacct[1][0];
	fiop->f_acctout[1] = ipacct[1][1];
#ifdef	USE_INET6
	fiop->f_fin6[0] = ipfilter6[0][0];
	fiop->f_fin6[1] = ipfilter6[0][1];
	fiop->f_fout6[0] = ipfilter6[1][0];
	fiop->f_fout6[1] = ipfilter6[1][1];
	fiop->f_acctin6[0] = ipacct6[0][0];
	fiop->f_acctin6[1] = ipacct6[0][1];
	fiop->f_acctout6[0] = ipacct6[1][0];
	fiop->f_acctout6[1] = ipacct6[1][1];
#else
	fiop->f_fin6[0] = NULL;
	fiop->f_fin6[1] = NULL;
	fiop->f_fout6[0] = NULL;
	fiop->f_fout6[1] = NULL;
	fiop->f_acctin6[0] = NULL;
	fiop->f_acctin6[1] = NULL;
	fiop->f_acctout6[0] = NULL;
	fiop->f_acctout6[1] = NULL;
#endif
	fiop->f_active = fr_active;
	fiop->f_froute[0] = ipl_frouteok[0];
	fiop->f_froute[1] = ipl_frouteok[1];

	fiop->f_running = fr_running;
	fiop->f_groups[0][0] = ipfgroups[0][0];
	fiop->f_groups[0][1] = ipfgroups[0][1];
	fiop->f_groups[1][0] = ipfgroups[1][0];
	fiop->f_groups[1][1] = ipfgroups[1][1];
	fiop->f_groups[2][0] = ipfgroups[2][0];
	fiop->f_groups[2][1] = ipfgroups[2][1];
#ifdef  IPFILTER_LOG
	fiop->f_logging = 1;
#else
	fiop->f_logging = 0;
#endif
	fiop->f_defpass = fr_pass;
	strncpy(fiop->f_version, ipfilter_version, sizeof(fiop->f_version));
}


#ifdef	USE_INET6
int icmptoicmp6types[ICMP_MAXTYPE+1] = {
	ICMP6_ECHO_REPLY,	/* 0: ICMP_ECHOREPLY */
	-1,			/* 1: UNUSED */
	-1,			/* 2: UNUSED */
	ICMP6_DST_UNREACH,	/* 3: ICMP_UNREACH */
	-1,			/* 4: ICMP_SOURCEQUENCH */
	ND_REDIRECT,		/* 5: ICMP_REDIRECT */
	-1,			/* 6: UNUSED */
	-1,			/* 7: UNUSED */
	ICMP6_ECHO_REQUEST,	/* 8: ICMP_ECHO */
	-1,			/* 9: UNUSED */
	-1,			/* 10: UNUSED */
	ICMP6_TIME_EXCEEDED,	/* 11: ICMP_TIMXCEED */
	ICMP6_PARAM_PROB,	/* 12: ICMP_PARAMPROB */
	-1,			/* 13: ICMP_TSTAMP */
	-1,			/* 14: ICMP_TSTAMPREPLY */
	-1,			/* 15: ICMP_IREQ */
	-1,			/* 16: ICMP_IREQREPLY */
	-1,			/* 17: ICMP_MASKREQ */
	-1,			/* 18: ICMP_MASKREPLY */
};


int	icmptoicmp6unreach[ICMP_MAX_UNREACH] = {
	ICMP6_DST_UNREACH_ADDR,		/* 0: ICMP_UNREACH_NET */
	ICMP6_DST_UNREACH_ADDR,		/* 1: ICMP_UNREACH_HOST */
	-1,				/* 2: ICMP_UNREACH_PROTOCOL */
	ICMP6_DST_UNREACH_NOPORT,	/* 3: ICMP_UNREACH_PORT */
	-1,				/* 4: ICMP_UNREACH_NEEDFRAG */
	ICMP6_DST_UNREACH_NOTNEIGHBOR,	/* 5: ICMP_UNREACH_SRCFAIL */
	ICMP6_DST_UNREACH_ADDR,		/* 6: ICMP_UNREACH_NET_UNKNOWN */
	ICMP6_DST_UNREACH_ADDR,		/* 7: ICMP_UNREACH_HOST_UNKNOWN */
	-1,				/* 8: ICMP_UNREACH_ISOLATED */
	ICMP6_DST_UNREACH_ADMIN,	/* 9: ICMP_UNREACH_NET_PROHIB */
	ICMP6_DST_UNREACH_ADMIN,	/* 10: ICMP_UNREACH_HOST_PROHIB */
	-1,				/* 11: ICMP_UNREACH_TOSNET */
	-1,				/* 12: ICMP_UNREACH_TOSHOST */
	ICMP6_DST_UNREACH_ADMIN,	/* 13: ICMP_UNREACH_ADMIN_PROHIBIT */
};
#endif

