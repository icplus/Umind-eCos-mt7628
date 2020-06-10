/* NetBSD
 * 
 *  taken from: FILEVERSION 9906180
 *
 * ppp_mppe.c - MPPE "compressor/decompressor" module.
 *
 * Copyright (c) 1994 ��p�� Magos��yi <mag@bunuel.tii.matav.hu>
 * All rights reserved.
 * Copyright (c) 1999 Tim Hockin, Cobalt Networks Inc. <thockin@cobaltnet.com>
 * Copyright (c) 1999 Darrin B. Jewell <dbj@netbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, provided that the above copyright
 * notice appears in all copies.  This software is provided without any
 * warranty, express or implied. The Australian National University
 * makes no representations about the suitability of this software for
 * any purpose.
 *
 * IN NO EVENT SHALL THE AUSTRALIAN NATIONAL UNIVERSITY BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 * THE AUSTRALIAN NATIONAL UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * THE AUSTRALIAN NATIONAL UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE AUSTRALIAN NATIONAL UNIVERSITY HAS NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS,
 * OR MODIFICATIONS.
 *
 * From: deflate.c,v 1.1 1996/01/18 03:17:48 paulus Exp
 */

//#define MPPE_DEBUG
//#define MPEE_DEBUG	4

#ifdef MPPE
#include <sys/param.h>
#include <sys/types.h>
//#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <pppd.h>
//#include <net/ppp_defs.h>

#define PACKETPTR	struct mbuf *
#include <ppp_comp.h>

//#include "rc4.h"
#include "rc4_enc.c"
#include "sha1dgst.c"
#include <mppe.h>

extern void
GetNewKeyFromSHA(unsigned char *StartKey,
    unsigned char *SessionKey,
    unsigned long SessionKeyLength,
    unsigned char *InterimKey);

#if defined(LKM) || defined(_LKM)
int mppe_in_use = 0;
#define MOD_DEC_USE_COUNT (mppe_in_use--)
#define MOD_INC_USE_COUNT (mppe_in_use++)
#else
#define MOD_DEC_USE_COUNT while (0)
#define MOD_INC_USE_COUNT while (0)
#endif

#ifndef CI_MPPE
#define CI_MPPE			18	/* config. option for MPPE */
#define CILEN_MPPE		6	/* length of config. option */
#endif

#ifdef MPPE_DEBUG
#define DPRINTF(x) diag_printf x
#else
#define DPRINTF(x)
#endif

extern int mppe_bitkeylen;
/*
 * State for a mppe "(de)compressor".
 */
struct ppp_mppe_state {
    //unsigned int	bitkeylen;
    unsigned int	ccount; /*coherency count */
    RC4_KEY		RC4_send_key; /* chap-ms-v2 dictates 2 keys */
    RC4_KEY		RC4_recv_key;
    unsigned char	session_send_key[16];
    unsigned char	session_recv_key[16];
    unsigned char	mppe;
    unsigned char	master_send_key[16];
    unsigned char	master_recv_key[16];
    int			keylen;
    int 		bitkeylen;
    int                 stateless;
    int                 decomp_error;
    unsigned int	bits;
    int			unit;
    int			debug;
    int			mru;
    struct compstat 	stats;
};

#define MPPE_CCOUNT_FROM_PACKET(ibuf)	((((ibuf)[PPPHEADER_LEN] & 0x0f) << 8) + (ibuf)[PPPHEADER_LEN+1])
#define MPPE_BITS(ibuf) 	((ibuf)[PPPHEADER_LEN] & 0xf0 )
#define MPPE_CTRLHI(state)	((((state)->ccount & 0xf00)>>8)|((state)->bits))
#define MPPE_CTRLLO(state)	((state)->ccount & 0xff)
#define MPPE_OVHD	(PPPHEADER_LEN + 2)

void mppe_synchronize_key(struct ppp_mppe_state *);
void mppe_initialize_key(struct ppp_mppe_state *);
void mppe_change_key(struct ppp_mppe_state *);
void mppe_update_count(struct ppp_mppe_state *);

/* Procedures from the MPPE draft */
void
mppe_synchronize_key(struct ppp_mppe_state *state)
{
    /* get new keys and flag our state as such */
    RC4_set_key(&(state->RC4_send_key),state->keylen,state->session_send_key);
    RC4_set_key(&(state->RC4_recv_key),state->keylen,state->session_recv_key);

    state->bits=MPPE_BIT_FLUSHED|MPPE_BIT_ENCRYPTED;
}


void
mppe_initialize_key(struct ppp_mppe_state *state)
{
    /* generate new session keys */
    GetNewKeyFromSHA(state->master_send_key, state->master_send_key,
	state->keylen, state->session_send_key);
    GetNewKeyFromSHA(state->master_recv_key, state->master_recv_key,
	state->keylen, state->session_recv_key);

    if(state->keylen == 8) {
	/* cripple them from 64bit->40bit */
	/* yfchou added for 56bit */
	    if (mppe_bitkeylen == 40) {
        state->session_send_key[0]=state->session_recv_key[0] = MPPE_40_SALT0;
        state->session_send_key[1]=state->session_recv_key[1] = MPPE_40_SALT1;
        state->session_send_key[2]=state->session_recv_key[2] = MPPE_40_SALT2;
    }
        else{
        	state->session_send_key[0]=state->session_recv_key[0] = MPPE_40_SALT0;
        }	
    }

    mppe_synchronize_key(state);
}


void
mppe_change_key(struct ppp_mppe_state *state)
{
    unsigned char InterimSendKey[16];
    unsigned char InterimRecvKey[16];

    /* get temp keys */
    GetNewKeyFromSHA(state->master_send_key, state->session_send_key,
	state->keylen, InterimSendKey);
    GetNewKeyFromSHA(state->master_recv_key, state->session_recv_key,
	state->keylen, InterimRecvKey);

    /* build RC4 keys from the temp keys */
    RC4_set_key(&(state->RC4_send_key), state->keylen, InterimSendKey);
    
    RC4_set_key(&(state->RC4_recv_key), state->keylen, InterimRecvKey);
    

    /* make new session keys */
    RC4_1(&(state->RC4_send_key), state->keylen, InterimSendKey,
	state->session_send_key);
    RC4_1(&(state->RC4_recv_key), state->keylen, InterimRecvKey,
	state->session_recv_key);

    if(state->keylen == 8)
    {
	/* cripple them from 64->40 bits*/
	if (mppe_bitkeylen == 40) {
        state->session_send_key[0]=state->session_recv_key[0] = MPPE_40_SALT0;
        state->session_send_key[1]=state->session_recv_key[1] = MPPE_40_SALT1;
        state->session_send_key[2]=state->session_recv_key[2] = MPPE_40_SALT2;
        }else {
        	state->session_send_key[0]=state->session_recv_key[0] = MPPE_40_SALT0;
    	}		
    }

    /* make the final rc4 keys */
    RC4_set_key(&(state->RC4_send_key), state->keylen, state->session_send_key);
    RC4_set_key(&(state->RC4_recv_key), state->keylen, state->session_recv_key);
	//yfchou modify
    //state->bits=MPPE_BIT_ENCRYPTED;
    state->bits |= MPPE_BIT_FLUSHED;
}


#if defined(MPPE_DEBUG) && (MPEE_DEBUG > 1)
typedef u_int8_t __u8;
/* Utility procedures to print a buffer in hex/ascii */
static void
ppp_print_hex (register __u8 *out, const __u8 *in, int count)
{
	register __u8 next_ch;
	static char hex[] = "0123456789ABCDEF";

	while (count-- > 0) {
		next_ch = *in++;
		*out++ = hex[(next_ch >> 4) & 0x0F];
		*out++ = hex[next_ch & 0x0F];
		++out;
	}
}


static void
ppp_print_char (register __u8 *out, const __u8 *in, int count)
{
	register __u8 next_ch;

	while (count-- > 0) {
		next_ch = *in++;

		if (next_ch < 0x20 || next_ch > 0x7e)
			*out++ = '.';
		else {
			*out++ = next_ch;
			if (next_ch == '%')   /* printk/syslogd has a bug !! */
				*out++ = '%';
		}
	}
	*out = '\0';
}

static void
ppp_print_buffer (const __u8 *name, const __u8 *buf, int count)
{
	__u8 line[44];

	if (name != (__u8 *) NULL)
		printf("ppp: %s, count = %d\n", name, count);

	while (count > 8) {
		memset (line, 32, 44);
		ppp_print_hex (line, buf, 8);
		ppp_print_char (&line[8 * 3], buf, 8);
		printf("%s\n", line);
		count -= 8;
		buf += 8;
	}

	if (count > 0) {
		memset (line, 32, 44);
		ppp_print_hex (line, buf, count);
		ppp_print_char (&line[8 * 3], buf, count);
		printf("%s\n", line);
	}
}
#endif

/* our 'compressor' proper */
void	*mppe_comp_alloc __P((unsigned char *, int));
void	mppe_comp_free __P((void *));
int	mppe_comp_init __P((void *, unsigned char *,
					int, int, int, int));
int	mppe_decomp_init __P((void *, unsigned char *,
					int, int, int, int, int));
int	mppe_compress __P((void *, struct mbuf **,
					struct mbuf *,int, int));
void	mppe_incomp __P((void *, struct mbuf *));
int	mppe_decompress __P((void *, struct mbuf *, struct mbuf **));
void	mppe_comp_reset __P((void *));
void	mppe_comp_stats __P((void *, struct compstat *));


/* cleanup the compressor */
void
mppe_comp_free(void *arg)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *) arg;

    if (state) {
	    //free(state,M_DEVBUF);
	    free(state);
	    MOD_DEC_USE_COUNT;
    }
}


/* allocate space for a compressor.  */
void *
mppe_comp_alloc(unsigned char *options, int opt_len)
{
    struct ppp_mppe_state *state;

    if (((2*8)+3 != opt_len && (2*16)+3 != opt_len) /* 2 keys + 3 */
	|| options[0] != CI_MPPE || options[1] != CILEN_MPPE) {
	DPRINTF(("ppp/mppe: compress rejected: opt_len=%u,o[0]=%x,o[1]=%x\n",
	    opt_len,options[0],options[1]));
	if (opt_len == 32) {
	    DPRINTF(("ppp/mppe: try increasing CCP_MAX_OPTION_LENGTH in ppp-comp.h\n"));
	}
	return NULL;
    }

    //state = (struct ppp_mppe_state *)malloc(sizeof(*state), M_DEVBUF, M_NOWAIT);
    state = (struct ppp_mppe_state *)malloc(sizeof(*state));
    if (state == NULL)
	return NULL;

    MOD_INC_USE_COUNT;

    memset (state, 0, sizeof (struct ppp_mppe_state));

    /* write the data in options to the right places */
    memcpy(&state->stateless,options+2,1);
    state->keylen = (opt_len-3)/2;
    memcpy(state->master_send_key,options+3,state->keylen);
    memcpy(state->master_recv_key,options+3+state->keylen,state->keylen);

    mppe_initialize_key(state);

    return (void *) state;
}


int
mppe_comp_init(void *arg, unsigned char *options, int opt_len, int unit, 
		int hdrlen, int debug)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;

    if (options[0] != CI_MPPE || options[1] != CILEN_MPPE) {
    	DPRINTF(("ppp%d/mppe: compress rejected: opt_len=%u,o[0]=%x,o[1]=%x\n",
	    state->unit,opt_len,options[0],options[1]));
	return 0;
    }
    state->ccount = 0;
    state->unit  = unit;
    state->debug = debug;

	//yfchou added
	memcpy(&state->stateless,options+2,1);
	
    /* 19 is the min (2*keylen) + 3 */
    if(opt_len >= 19) {
        memcpy(&state->stateless,options+2,1);

    	state->keylen = (opt_len-3)/2;
    	memcpy(state->master_send_key,options+3,state->keylen);
    	memcpy(state->master_recv_key,options+3+state->keylen,state->keylen);

    	mppe_initialize_key(state);
    }
    return 1;
}


int
mppe_decomp_init(void *arg, unsigned char *options, int opt_len, int unit,
		int hdrlen, int mru, int debug)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;

    if (options[0] != CI_MPPE || options[1] != CILEN_MPPE) {
	DPRINTF(("ppp%d/MPPE: options are bad: %x %x\n",
		 state->unit,options[0],options[1]));
	return 0;
    }

    state->ccount = 0;
    state->unit  = unit;
    state->debug = debug;
    state->mru = mru;

    /* 19 is the min (2*keylen)+3 */
    if(opt_len >= 19) {
	memcpy(&state->stateless,options+2,1);

	state->keylen = (opt_len-3)/2;
	memcpy(state->master_send_key,options+3,state->keylen);
	memcpy(state->master_recv_key,options+3+state->keylen,state->keylen);

	mppe_initialize_key(state);
    }

    return 1;
}


void
mppe_comp_reset(void *arg)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;

    DPRINTF(("mppe_comp_reset\n"));

    (state->stats).ratio = 0;

    mppe_synchronize_key(state);
}


void
mppe_update_count(struct ppp_mppe_state *state)
{
	//state->stateless = 0;
    if(!state->stateless)
    {
        if ( 0xff == (state->ccount&0xff)){ 
	    /* time to change keys */
	 //fix stateless mode =0 fail,by Ziqiang Yu
	     mppe_change_key(state);
	    state->bits &=(~ MPPE_BIT_FLUSHED);
	    if ( 0xfff == (state->ccount&0xfff)){
		state->ccount = 0;
	    } else {
		(state->ccount)++;
	    }
        } else {
            state->ccount++;
        }
    } else {
        if ( 0xFFF == (state->ccount & 0xFFF)) {
	    state->ccount = 0;
        } else {
	    (state->ccount)++;
	}
       	mppe_change_key(state);
    }
}


/* the big nasty */
int
mppe_compress(void *arg, struct mbuf **mret, struct mbuf *mp,
		int isize, int osize)
{
    unsigned char *rptr;
    struct ppp_mppe_state *state = (struct ppp_mppe_state *) arg;
    int proto, olen;
    unsigned char *wptr;
	
    *mret = NULL;

	//diag_printf("DBG: MPPE Compress\n");
#ifdef DIAGNOSTIC
    /* XXX: dbj -- we cheat, and allow ourselves to go up to MPPE_OVHD
     * past the osize handed to us.  This implies we may send a packet
     * greater than the mtu.  We don't have much other choice.  
     */
    if (osize+MPPE_OVHD < isize+MPPE_OVHD) {
      /* XXX: what should we do here?  If we return NULL, the data
       * will go out unencrypted!
       */
      panic("ppp%d/mppe: Not enough space to encrypt packet: %d<%d+%d!\n",
	  state->unit,isize, osize, MPPE_OVHD);
    }

    /* Sanity check that at least the header is contigous,
     * if this ever fails, we should just copy the header out by hand
     * rather than actually do a pullup.  This can't happen
     * with the current if_ppp implementation
     */
    if (mp->m_len < PPPHEADER_LEN) {
      panic("ppp%d/mppe: m_pullup required to get ppp header!",
	    state->unit);
    }
#endif

    rptr = mtod(mp, unsigned char *);

#if defined(MPPE_DEBUG) && (MPEE_DEBUG > 1)
    ppp_print_buffer("mppe_encrypt",rptr,isize);
#endif


    /* Check that the protocol is in the range we handle. */
    //yfchou added for addr/cntrl
    proto = PPP_PROTOCOL(rptr);
    //proto = ((((u_char *)(rptr))[0] << 8) + ((u_char *)(rptr))[1]);
    if (proto < 0x0021 || proto > 0x00FA )
	return 0;

#ifdef DIAGNOSTIC
    if (!(mp->m_flags & M_PKTHDR)) {
      panic("ppp%d/mppe: expecting pkthdr in mbuf",state->unit);
    }
#endif

#ifdef PPTP_BOOST_VERSION	// fix mppe problem in mppe_compress() for PPTP_BOOST_VERSION
	/* Allocate an mbuf chain to hold the encrypted packet */
	{
		struct mbuf *m;
		
		MGETHDR(m, M_DONTWAIT, MT_DATA);
		if (m== NULL)
			return NULL;
		M_COPY_PKTHDR(m,mp);
		MCLGET(m, M_DONTWAIT);
		if ((m->m_flags & M_EXT) == 0) {
			m_freem(m);
			return NULL;
		}
		//diag_printf("[SHT]:cluster buf size:%d\n", (m)->m_ext.ext_size);
		m->m_data += 16*4;
		*mret = m;
	}

#else //PPTP_BOOST_VERSION
    /* Allocate an mbuf chain to hold the encrypted packet */
    {
      struct mbuf *mfirst = NULL;
      struct mbuf *mprev;
      struct mbuf *m = NULL;
      int bleft = isize+MPPE_OVHD;
      
      do {
	mprev = m;
	MGET(m,M_DONTWAIT, MT_DATA);
	if (m == NULL) {
	  m_freem(mfirst);
	  /* XXX: what should we do here?  If we return NULL, the data
	   * will go out unencrypted. We can't use M_WAITOK, since this
	   * will get get called from splsoftnet()
	   */
	  //panic("ppp%d/mppe: unable to allocate mbuf to encrypt packet",
		//state->unit);
	/* 14/01/05 Roger ++,  */	
		diag_printf("ppp%d/mppe: unable to allocate mbuf to encrypt packet",
		state->unit);
		return 0;
	/* 14/01/05 Roger --,  */	
	}
	m->m_len = 0;
	if (mfirst == NULL) {
	  mfirst = m;
	  M_COPY_PKTHDR(m,mp);
	  if (bleft > MHLEN) {
	    MCLGET(m, M_DONTWAIT);
	  }
	} else {
	  mprev->m_next = m;
	  if (bleft > MLEN) {
	    MCLGET(m, M_DONTWAIT);
	  }
	}
	bleft -= M_TRAILINGSPACE(m);
      } while (bleft > 0);
      *mret = mfirst;
    }

#endif	//PPTP_BOOST_VERSION
    wptr = mtod(*mret, unsigned char *);

    /* Copy over the PPP header and store the 2-byte sequence number. */
    MAKE_PPPHEADER(wptr, PPP_MPPE);
    
    *wptr++ = MPPE_CTRLHI(state);
    *wptr++ = MPPE_CTRLLO(state);
    (*mret)->m_len = MPPE_OVHD;
	#ifdef PPTP_BOOST_VERSION	// fix mppe problem in mppe_compress() for PPTP_BOOST_VERSION
	if ((*mret)->m_flags & M_PKTHDR)(*mret)->m_pkthdr.len += MPPE_OVHD;  
	#endif

    state->bits=MPPE_BIT_ENCRYPTED;
    mppe_update_count(state);

    /* March down input and output mbuf chains, encoding with RC4 */
    {
      struct mbuf *mi = mp;	/* mbuf in */
      struct mbuf *mo = *mret;	/* mbuf out */
      int maxi, maxo;
	  if(!mo)
	  	{
	  	panic("mppe_compress mo=NULL\n");
		return 0;
	  	}
      maxi = mi->m_len;
      maxo = M_TRAILINGSPACE(mo);
      while (mi) {
	if ((maxi < maxo) && mo) {
	  RC4_1(&(state->RC4_send_key),maxi,
	      mtod(mi,unsigned char *)+mi->m_len-maxi,
	      mtod(mo,unsigned char *)+mo->m_len);
	  mo->m_len += maxi;
	  maxo -= maxi;
	  mi = mi->m_next;
	  if (mi) {
	    maxi = mi->m_len;
	  }
	} else if ((maxi > maxo) && mo) {
	  RC4_1(&(state->RC4_send_key),maxo,
	      mtod(mi,unsigned char *)+mi->m_len-maxi,
	      mtod(mo,unsigned char *)+mo->m_len);
	  mo->m_len += maxo;
	  maxi -= maxo;
	  mo = mo->m_next;
	  if (mo) {
	    maxo = M_TRAILINGSPACE(mo);
	  }
	} else if (mo){
	  RC4_1(&(state->RC4_send_key),maxi,
	      mtod(mi,unsigned char *)+mi->m_len-maxi,
	      mtod(mo,unsigned char *)+mo->m_len);
	  mo->m_len += maxi;
	  mi = mi->m_next;
	  mo = mo->m_next;
	  if (mi&&mo) {
	    maxi = mi->m_len;
	    maxo = M_TRAILINGSPACE(mo);
	  }
	}
      }
    }
    olen=isize+MPPE_OVHD;
    (state->stats).comp_bytes += isize;
    (state->stats).comp_packets++;

#if defined(MPPE_DEBUG) && (MPEE_DEBUG > 1)
    ppp_print_buffer("mppe_encrypt out",mtod(*mret, unsigned char *),olen);
#endif
	
    return olen;
}


void
mppe_comp_stats(void *arg, struct compstat *stats)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;

    /* this _SHOULD_ always be 1 */
    (state->stats).ratio = (state->stats).unc_bytes/(state->stats).comp_bytes;

    *stats = state->stats;
   
}


/* the other big nasty */
int
mppe_decompress(void *arg, struct mbuf *mp, struct mbuf **mret)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;
    int seq;
    unsigned char *ibuf;
    int isize; 
    unsigned char *obuf;

    if (!mp) {
	DPRINTF(("ppp%d/mppe: null input packet\n",state->unit));
	return DECOMP_ERROR;
    }

    /* Sanity check that at least the header is contigous,
     * and the packet is long enough.
     */
    if (mp->m_len <= PPPHEADER_LEN + MPPE_OVHD) {
	if (state->debug) {
	    DPRINTF(("ppp%d/mppe: short start mbuf (len=%d)\n",
		state->unit, mp->m_len));
	}

	return DECOMP_ERROR;
    }

    ibuf = mtod(mp,unsigned char *);

    /* Check the sequence number. */
    seq = MPPE_CCOUNT_FROM_PACKET(ibuf);

    if(!state->stateless && (MPPE_BITS(ibuf) & MPPE_BIT_FLUSHED)) {
        state->decomp_error = 0;
        state->ccount = seq;
    }

    if(state->decomp_error) {
	DPRINTF(("ppp%d/mppe: decomp_err=%d\n", state->decomp_error));
        return DECOMP_ERROR;
    }

    if (seq != state->ccount) {
	if (state->debug) {
	    DPRINTF(("ppp%d/mppe: decompress: bad seq # %d, expected %d\n",
		   state->unit, seq, state->ccount));
	}

        while(state->ccount != seq) {
            mppe_update_count(state);
	}

	/*
	 * Let the packet in anyway.  It allows the tunnel to stay alive
	 * whereas returning DECOMP_ERROR would disable the MPPE
	 * decompressor in the kernel for that connection.
	 */
#if 1
        mppe_update_count(state);

	return DECOMP_ERROR;
#endif
    }

    if(!(MPPE_BITS(ibuf) & MPPE_BIT_ENCRYPTED)) {
        DPRINTF(("ppp%d/mppe: ERROR: not an encrypted packet\n",state->unit));
        mppe_synchronize_key(state);
	return DECOMP_ERROR;
    }

    /* Allocate an mbuf chain to hold the decrypted packet */
    {
	struct mbuf *mfirst = 0;
	struct mbuf *mprev;
	struct mbuf *m = 0;
	int bleft;
	isize = 0;
	for (m=mp; m; m= m->m_next) isize += m->m_len;
	bleft = isize-MPPE_OVHD;
	do {
	    mprev = m;
	    MGET(m,M_DONTWAIT, MT_DATA);
	    if (m == NULL) {
		m_freem(mfirst);
		DPRINTF(("ppp%d/mppe: unable to allocate mbuf to decrypt packet\n",
		    state->unit));
		return DECOMP_ERROR;
	    }
	    m->m_len = 0;
	    if (mfirst == NULL) {
		mfirst=m;
		M_COPY_PKTHDR(m,mp);
		if (bleft > MHLEN) {
		    MCLGET(m, M_DONTWAIT);
		}
	    } else {
		mprev->m_next = m;
		if (bleft > MLEN) {
		    MCLGET(m, M_DONTWAIT);
		}
	    }
	    bleft -= M_TRAILINGSPACE(m);
	} while (bleft > 0);
	*mret = mfirst;
    }

    obuf = mtod(*mret,unsigned char *);

    {
	if(!state->stateless && (MPPE_BITS(ibuf) & MPPE_BIT_FLUSHED))
	    mppe_synchronize_key(state);
	mppe_update_count(state);

	/* March down input and output mbuf chains, decoding with RC4 */
	{
	    struct mbuf *mi = mp;	/* mbuf in */
	    struct mbuf *mo = *mret;	/* mbuf out */
	    int maxi, maxo;
		if(!mo)
			{
			panic("mppe_decompress mo=NULL\n");
			return DECOMP_ERROR;
			}
	    maxi = mi->m_len-MPPE_OVHD;	/* adjust for MPPE_OVERHEAD */
	    maxo = M_TRAILINGSPACE(mo);
	    while (mi) {
		if ((maxi < maxo)&&mo) {
		    RC4_1(&(state->RC4_recv_key),maxi,
			mtod(mi,unsigned char *)+mi->m_len-maxi,
			mtod(mo,unsigned char *)+mo->m_len);
		    mo->m_len += maxi;
		    maxo -= maxi;
		    mi = mi->m_next;
		    if (mi) {
			maxi = mi->m_len;
		    }
		} else if ((maxi > maxo)&&mo) {
		    RC4_1(&(state->RC4_recv_key),maxo,
			mtod(mi,unsigned char *)+mi->m_len-maxi,
			mtod(mo,unsigned char *)+mo->m_len);
		    mo->m_len += maxo;
		    maxi -= maxo;
		    mo = mo->m_next;
		    if (mo) {
			maxo = M_TRAILINGSPACE(mo);
		    }
		} else if (mo){
		    RC4_1(&(state->RC4_recv_key),maxi,
			mtod(mi,unsigned char *)+mi->m_len-maxi,
			mtod(mo,unsigned char *)+mo->m_len);
		    mo->m_len += maxi;
		    mi = mi->m_next;
		    mo = mo->m_next;
		    if (mi &&mo) {
			maxi = mi->m_len;
			maxo = M_TRAILINGSPACE(mo);
		    }
		}
	    }
	}

	(state->stats).unc_bytes += (isize-MPPE_OVHD);
	(state->stats).unc_packets ++;

	return DECOMP_OK;
    }
}


/* Incompressible data has arrived - add it to the history.  */
void
mppe_incomp(void *arg, struct mbuf *mp)
{
    struct ppp_mppe_state *state = (struct ppp_mppe_state *)arg;
    struct mbuf *m;
    for (m=mp;m;m = m->m_next) {
      (state->stats).inc_bytes += m->m_len;
    }
    (state->stats).inc_packets++;
}


/*************************************************************
 * Module interface table
 *************************************************************/

/*
 * Procedures exported to if_ppp.c.
 */
struct compressor ppp_mppe = {
    CI_MPPE,			/* compress_proto */
    mppe_comp_alloc,		/* comp_alloc */
    mppe_comp_free,		/* comp_free */
    mppe_comp_init,		/* comp_init */
    mppe_comp_reset,		/* comp_reset */
    mppe_compress,		/* compress */
    mppe_comp_stats,		/* comp_stat */
    mppe_comp_alloc,		/* decomp_alloc */
    mppe_comp_free,		/* decomp_free */
    mppe_decomp_init,		/* decomp_init */
    mppe_comp_reset,		/* decomp_reset */
    mppe_decompress,		/* decompress */
    mppe_incomp,		/* incomp */
    mppe_comp_stats,		/* decomp_stat */
};

#endif


