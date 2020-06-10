/*	$KAME: dhcp6s.c,v 1.162 2005/10/04 11:53:32 suz Exp $	*/
/*
 * Copyright (C) 1998 and 1999 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 /*
#if 0
#include <cyg/kernel/kapi.h>
#include <cyg/fileio/fileio.h>
#include <pkgconf/system.h>
#include <pkgconf/net.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <sys/uio.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <errno.h>

#include <net/if.h>
#ifdef __FreeBSD__
#include <net/if_var.h>
#endif

#include <netinet/in.h>
#define	__KAME_NETINET_IN_H_INCLUDED_
#include <netinet6/in6.h>
#undef __KAME_NETINET_IN_H_INCLUDED_

#ifdef __KAME__
#include <netinet6/in6_var.h>
#endif


#include <arpa/inet.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <netdb.h>
#include <limits.h>
#endif*/


#include <sys/param.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/bsdtypes.h>
 #include <sys/ioctl.h>

#include <sys/cdefs.h>

#include <sys/cdefs.h>

#include <net/if.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip_mroute.h>
#include <netinet/icmp6.h>
#include <netinet6/in6_var.h>
#include <netinet6/ip6_var.h>
#include <netinet6/ip6_mroute.h>
 #include <net/netdb.h>
#include <arpa/inet.h>
 
#include <time.h>
#include <errno.h>
#include <syslog.h>


#include <dhcp6.h>
#include <common.h>
#include <timer.h>
#include <auth.h>
#include <base64.h>
#include <lease.h>
#include <config.h>

#include <cfg_id.h>
typedef unsigned int wchar_t;
#include <stdlib.h>



#define CTLSKEW 300

enum
{
    INITIATE=1,
    SHUTDOWN=2,
};


typedef enum { DHCP6_BINDING_IA } dhcp6_bindingtype_t;

struct dhcp6_binding {
	TAILQ_ENTRY(dhcp6_binding) link;

	dhcp6_bindingtype_t type;

	/* identifier of the binding */
	struct duid clientid;
	/* additional identifiers for IA-based bindings */
	int iatype;
	u_int32_t iaid;

	/*
	 * configuration information of this binding,
	 * which is type-dependent.
	 */
	union {
		struct dhcp6_list uv_list;
	} val;
#define val_list val.uv_list

	u_int32_t duration;
	time_t updatetime;
	struct dhcp6_timer *timer;
};
static TAILQ_HEAD(dhcp6_binding_list, dhcp6_binding) dhcp6_binding_head;

struct relayinfo {
	TAILQ_ENTRY(relayinfo) link;

	u_int hcnt;		/* hop count */
	struct in6_addr linkaddr; /* link address */
	struct in6_addr peeraddr; /* peer address */
	struct dhcp6_vbuf relay_ifid; /* Interface ID (if provided) */
	struct dhcp6_vbuf relay_msg; /* relay message */
};
TAILQ_HEAD(relayinfolist, relayinfo);

static int debug = 0;


int ifidx;
int insock=-1;			/* inbound UDP port */
int outsock=-1;			/* outbound UDP port */
int ctlsock = -1;		/* control TCP port */
static const struct sockaddr_in6 *sa6_any_downstream, *sa6_any_relay;
static struct msghdr rmh;
static char rdatabuf[BUFSIZ];
static int rmsgctllen;
static char *rmsgctlbuf;
static struct duid server_duid;
static struct dhcp6_list arg_dnslist;


cyg_handle_t dhcp6s_thread_h ;
cyg_thread   dhcp6s_thread;

static char dhcp6s_stack[8192];//[ STACK_SIZE ];//change dhcpd_stack size:8192->5*1024

static int dhcp6s_running ;
 int stateless_only=0;
static struct cf_list * dns_head=NULL;
struct pool_conf *prefix_pool;

struct dhcp6_poolspec *prefix_poolspec;



static void usage __P((void));
static int server6_init __P((void));
static void server6_mainloop __P((void));
static void server6_recv __P((int));
static void process_signals __P((void));
static void free_relayinfo __P((struct relayinfo *));
static int process_relayforw __P((struct dhcp6 **, struct dhcp6opt **,
    struct relayinfolist *, struct sockaddr *));
static int set_statelessinfo __P((int, struct dhcp6_optinfo *));
static int react_solicit __P((struct dhcp6_if *, struct dhcp6 *, ssize_t,
    struct dhcp6_optinfo *, struct sockaddr *, int, struct relayinfolist *));
static int react_request __P((struct dhcp6_if *, struct in6_pktinfo *,
    struct dhcp6 *, ssize_t, struct dhcp6_optinfo *, struct sockaddr *, int,
    struct relayinfolist *));
static int react_renew __P((struct dhcp6_if *, struct in6_pktinfo *,
    struct dhcp6 *, ssize_t, struct dhcp6_optinfo *, struct sockaddr *, int,
    struct relayinfolist *));
static int react_rebind __P((struct dhcp6_if *, struct dhcp6 *, ssize_t,
    struct dhcp6_optinfo *, struct sockaddr *, int, struct relayinfolist *));
static int react_release __P((struct dhcp6_if *, struct in6_pktinfo *,
    struct dhcp6 *, ssize_t, struct dhcp6_optinfo *, struct sockaddr *, int,
    struct relayinfolist *));
static int react_decline __P((struct dhcp6_if *, struct in6_pktinfo *,
    struct dhcp6 *, ssize_t, struct dhcp6_optinfo *, struct sockaddr *, int,
    struct relayinfolist *));
static int react_confirm __P((struct dhcp6_if *, struct in6_pktinfo *,
    struct dhcp6 *, ssize_t,
    struct dhcp6_optinfo *, struct sockaddr *, int, struct relayinfolist *));
static int react_informreq __P((struct dhcp6_if *, struct dhcp6 *, ssize_t,
    struct dhcp6_optinfo *, struct sockaddr *, int, struct relayinfolist *));
static int server6_send __P((int, struct dhcp6_if *, struct dhcp6 *,
    struct dhcp6_optinfo *, struct sockaddr *, int, struct dhcp6_optinfo *,
    struct relayinfolist *, struct host_conf *));
static int make_ia_stcode __P((int, u_int32_t, u_int16_t,
    struct dhcp6_list *));
static int update_ia __P((int, struct dhcp6_listval *,
    struct dhcp6_list *, struct dhcp6_optinfo *));
static int release_binding_ia __P((struct dhcp6_listval *, struct dhcp6_list *,
    struct dhcp6_optinfo *));
static int decline_binding_ia __P((struct dhcp6_listval *, struct dhcp6_list *,
    struct dhcp6_optinfo *));
static int make_ia __P((struct dhcp6_listval *, struct dhcp6_list *,
    struct dhcp6_list *, struct host_conf *, int));
static int make_match_ia __P((struct dhcp6_listval *, struct dhcp6_list *,
    struct dhcp6_list *));
static int make_iana_from_pool __P((struct dhcp6_poolspec *,
    struct dhcp6_listval *, struct dhcp6_list *));
static void calc_ia_timo __P((struct dhcp6_ia *, struct dhcp6_list *,
    struct host_conf *));
static void update_binding_duration __P((struct dhcp6_binding *));
static struct dhcp6_binding *add_binding __P((struct duid *,
    dhcp6_bindingtype_t, int, u_int32_t, void *));
static struct dhcp6_binding *find_binding __P((struct duid *,
    dhcp6_bindingtype_t, int, u_int32_t));
static void update_binding __P((struct dhcp6_binding *));
static void remove_binding __P((struct dhcp6_binding *));
static void free_binding __P((struct dhcp6_binding *));
static struct dhcp6_timer *binding_timo __P((void *));
static struct dhcp6_listval *find_binding_ia __P((struct dhcp6_listval *,
    struct dhcp6_binding *));
static char *bindingstr __P((struct dhcp6_binding *));
static int process_auth __P((struct dhcp6 *, ssize_t, struct host_conf *,
    struct dhcp6_optinfo *, struct dhcp6_optinfo *));
static inline char *clientstr __P((struct host_conf *, struct duid *));
extern void dhcp6_server_init_pool(char *startaddr,char *endaddr);

extern int  dhcp6_server_interface_config_init(char * device_name,char *vlife,char *plife);
extern void free_binding_list(struct dhcp6_binding_list *binding);

extern  struct pool_conf *pool_conflist;
int delegation_router = 0;
int  dhcp6s_start(int argc, char* argv[])
{
	char * dns,*startaddr,*endaddr,*link,*domainlist;
	unsigned char stateless=0;
	char * found;
	char name[64];
	char plife[10]={0};
	char vlife[10]={0};

	//skip stop/start
	argc--;
	*argv++;

	//set debug level
	//setloglevel(LOG_DEBUG);
	setloglevel(0);
	if (dhcp6s_running)
	{	
		dprintf(LOG_DEBUG, FNAME, "Server is aready running ");
		return 1;
	}

	TAILQ_INIT(&arg_dnslist);
	TAILQ_INIT(&dnslist);
	TAILQ_INIT(&dnsnamelist);
	TAILQ_INIT(&siplist);
	TAILQ_INIT(&sipnamelist);
	TAILQ_INIT(&ntplist);
	TAILQ_INIT(&nislist);
	TAILQ_INIT(&nisnamelist);
	TAILQ_INIT(&nisplist);
	TAILQ_INIT(&nispnamelist);
	TAILQ_INIT(&bcmcslist);
	TAILQ_INIT(&bcmcsnamelist);
	pool_conflist == NULL;

	prefix_pool = malloc(sizeof(*prefix_pool));
	prefix_poolspec=malloc(sizeof(*prefix_poolspec));
	if (prefix_pool==NULL ||prefix_poolspec ==NULL)
		dprintf(LOG_ERR, FNAME, "cann't alloc memory ");

	memset(prefix_pool,0,sizeof(*prefix_pool));
	memset(prefix_poolspec,0,sizeof(*prefix_poolspec));
	while (argc--) {
			found = strstr(*argv, "=" );
			if (found != NULL) {
				found[0] = '\0';
				*found++;
			}	
			sprintf(name, "%s", *argv++);
			if (!strcmp(name, "startaddr")){
				startaddr = found;		
			}else if (!strcmp(name, "endaddr")) {
				endaddr = found;
			} else if (!strcmp(name, "dns")){
				struct cf_list dns_list;
				struct in6_addr addr;
				memset(&dns_list,0,sizeof(struct cf_list));
				inet_pton(AF_INET6, found, &addr);
				dns_list.ptr =& addr;
				configure_addr(&dns_list, &dnslist, "DNS") ;
			}else if (!strcmp(name, "domainlist")){
				domainlist = found;
				struct cf_list domain_list;
				struct in6_addr *addr;
				memset(&domain_list,0,sizeof(struct cf_list));
				domain_list.ptr= found;
				configure_domain(&domain_list, &dnsnamelist, "DNS");
			}else if (!strcmp(name, "link0")) {
				strcpy(server_interface,found);
				link = found;
			} else if (!strcmp(name,"stateless")){
				stateless_only = 1;
				}
			else  if (!strcmp(name,"vlife")){
				strcpy(vlife,found);
				} 
			else  if (!strcmp(name,"plife")){
				
				strcpy(plife,found);
				}
			/*
			*T1 and T2 can be deterimined by plife.
			*T1= plife/2
			*T2= (plife*4)/5
			*the calcuate is in calc_ia_timo()
			else  if (!strcmp(name,"T1")){
				;
				}
			 if (!strcmp(name,"T2")){
				;
			 	}
			 	*/
			 	else if (!strcmp(name,"prefix_start")){
					inet_pton(AF_INET6, found, &(prefix_pool->min));
			 		} 	else if (!strcmp(name,"prefix_end")){
					inet_pton(AF_INET6, found, &(prefix_pool->max));
			 		} 
				else if (!strcmp(name,"prefix_len")){
			 		prefix_pool->prefix_len = atoi(found);
					diag_printf("\nprefix_pool.prefix_len = %d \n",prefix_pool->prefix_len );
			 		} else if (!strcmp(name,"prefix_vlife")){
			 		prefix_poolspec->vltime= atoi(found);
			 		} 	else  if (!strcmp(name,"prefix_plife")){
			 		prefix_poolspec->pltime= atoi(found);
			 		} else if (!strcmp(name,"delegating_router"))
					delegation_router = 1;
		}
	prefix_poolspec->name = strdup("prefix");
	prefix_pool->name  = prefix_poolspec->name;
	prefix_pool->next  = pool_conflist;
	pool_conflist  = prefix_pool;
/*address pool init*/
	if (stateless_only== 0 )
		dhcp6_server_init_pool(startaddr,endaddr);//get address range from profile to pool_conf

	// struct dhcp6_ifconf *dhcp6_server_ifconflist; save config
	dhcp6_server_interface_config_init(server_interface,vlife,plife);

	if (server_interface && ifinit(server_interface,DHCP6_MODE_SERVER) == NULL)
		return (1);

	configure_commit(DHCP6_MODE_SERVER);//copy config from dhcp6_ifconf to dhcp6_if

	/* prohibit a mixture of old and new style of DNS server config */
	if (!TAILQ_EMPTY(&arg_dnslist)) {
		if (!TAILQ_EMPTY(&dnslist)) {
			dprintf(LOG_INFO, FNAME, "do not specify DNS servers "
			    "both by command line and by configuration file.");
			return (1);
		}
		dhcp6_move_list(&dnslist, &arg_dnslist);
		TAILQ_INIT(&arg_dnslist);
	}
	dhcp6_timer_init(DHCP6_MODE_SERVER);
	server6_init();
	
	{	
	/*	int val;
		if((CFG_get( CFG_LAN_DHCP_v6_EN, &val) == -1) || (val != 1))
			return;*/	
		if (dhcp6s_running == 0)
		{			
		dhcp6s_running = 1;
			cyg_thread_create
			(
				8,									/* scheduling info (eg pri) */
				(cyg_thread_entry_t *)server6_mainloop, /* entry function */
				0,									/* entry data */
				"DHCPv6 server",						/* optional thread name */
				dhcp6s_stack,						/* stack base, NULL = alloc */
				sizeof(dhcp6s_stack),				/* stack size, */
				&dhcp6s_thread_h,				/* returned thread handle */
				&dhcp6s_thread					/* put thread here */
			);	
			cyg_thread_resume(dhcp6s_thread_h);
		}
	}
	return (0);
}

/*------------------------------------------------------------*/

int  dhcp6s_stop(int argc, char* argv[])
{

	if ( dhcp6s_running == 0)	
	dprintf(LOG_ERR, FNAME, "DHCPv6 Server Daemon is not running");
	else
	dhcp6s_running = 0; //stop the dhcpv6 client daemon
	return 0;
}


int
server6_init()
{
	struct addrinfo hints;
	struct addrinfo *res, *res2;
	int error;
	int on = 1;
	struct ipv6_mreq mreq6;
	static struct iovec iov;
	static struct sockaddr_in6 sa6_any_downstream_storage;
	static struct sockaddr_in6 sa6_any_relay_storage;
	long long duid_time;

	TAILQ_INIT(&dhcp6_binding_head);
	if (lease_init() != 0) {
		dprintf(LOG_ERR, FNAME, "failed to initialize the lease table");
		return (1);
	}

	ifidx = if_nametoindex(server_interface);
	if (ifidx == 0) {
		dprintf(LOG_ERR, FNAME, "invalid interface %s", server_interface);
		exit(1);
	}

	/* get our DUID */

		
	if (get_duid(server_interface, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to get a DUID");
		return (1);
	}


	/* initialize send/receive buffer */
	iov.iov_base = (caddr_t)rdatabuf;
	iov.iov_len = sizeof(rdatabuf);
	rmh.msg_iov = &iov;
	rmh.msg_iovlen = 1;
	rmsgctllen = CMSG_SPACE(sizeof(struct in6_pktinfo));
	if ((rmsgctlbuf = (char *)malloc(rmsgctllen)) == NULL) {
		dprintf(LOG_ERR, FNAME, "memory allocation failed");
		return (1);
	}

	/* initialize socket */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(NULL, DH6PORT_UPSTREAM, &hints, &res);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	insock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (insock < 0) {
		dprintf(LOG_ERR, FNAME, "socket(insock): %s",
		    strerror(errno));
		return (1);
	}
	if (setsockopt(insock, SOL_SOCKET, SO_REUSEPORT, &on,
		       sizeof(on)) < 0) {
		dprintf(LOG_ERR, FNAME, "setsockopt(insock, SO_REUSEPORT): %s",
		    strerror(errno));
		return (1);
	}
	if (setsockopt(insock, SOL_SOCKET, SO_REUSEADDR, &on,
		       sizeof(on)) < 0) {
		dprintf(LOG_ERR, FNAME, "setsockopt(insock, SO_REUSEADDR): %s",
		    strerror(errno));
		return (1);
	}
#ifdef IPV6_RECVPKTINFO
	if (setsockopt(insock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on,
		       sizeof(on)) < 0) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(inbound, IPV6_RECVPKTINFO): %s",
		    strerror(errno));
		return (1);
	}
#else
	if (setsockopt(insock, IPPROTO_IPV6, IPV6_PKTINFO, &on,
		       sizeof(on)) < 0) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(inbound, IPV6_PKTINFO): %s",
		    strerror(errno));
		return (1);
	}
#endif
#ifdef IPV6_V6ONLY
	if (setsockopt(insock, IPPROTO_IPV6, IPV6_V6ONLY,
	    &on, sizeof(on)) < 0) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(inbound, IPV6_V6ONLY): %s", strerror(errno));
		return (1);
	}
#endif

	if (bind(insock, res->ai_addr, res->ai_addrlen) < 0) {
		dprintf(LOG_ERR, FNAME, "bind(insock): %s", strerror(errno));
		return (1);
	}
	freeaddrinfo(res);

	hints.ai_flags = 0;
	error = getaddrinfo(DH6ADDR_ALLAGENT, DH6PORT_UPSTREAM, &hints, &res2);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	memset(&mreq6, 0, sizeof(mreq6));
	mreq6.ipv6mr_interface = ifidx;
	memcpy(&mreq6.ipv6mr_multiaddr,
	    &((struct sockaddr_in6 *)res2->ai_addr)->sin6_addr,
	    sizeof(mreq6.ipv6mr_multiaddr));
	if (setsockopt(insock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
	    &mreq6, sizeof(mreq6))) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(insock, IPV6_JOIN_GROUP): %s",
		    strerror(errno));
		return (1);
	}
	freeaddrinfo(res2);

	hints.ai_flags = 0;
	error = getaddrinfo(DH6ADDR_ALLSERVER, DH6PORT_UPSTREAM,
			    &hints, &res2);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	memset(&mreq6, 0, sizeof(mreq6));
	mreq6.ipv6mr_interface = ifidx;
	memcpy(&mreq6.ipv6mr_multiaddr,
	    &((struct sockaddr_in6 *)res2->ai_addr)->sin6_addr,
	    sizeof(mreq6.ipv6mr_multiaddr));
	if (setsockopt(insock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
	    &mreq6, sizeof(mreq6))) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(insock, IPV6_JOIN_GROUP): %s",
		    strerror(errno));
		return (1);
	}
	freeaddrinfo(res2);

	hints.ai_flags = 0;
	error = getaddrinfo(NULL, DH6PORT_DOWNSTREAM, &hints, &res);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	outsock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (outsock < 0) {
		dprintf(LOG_ERR, FNAME, "socket(outsock): %s",
		    strerror(errno));
		return (1);
	}
	/* set outgoing interface of multicast packets for DHCP reconfig */
	if (setsockopt(outsock, IPPROTO_IPV6, IPV6_MULTICAST_IF,
	    &ifidx, sizeof(ifidx)) < 0) {
		dprintf(LOG_ERR, FNAME,
		    "setsockopt(outsock, IPV6_MULTICAST_IF): %s",
		    strerror(errno));
		return (1);
	}

	freeaddrinfo(res);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	error = getaddrinfo("::", DH6PORT_DOWNSTREAM, &hints, &res);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	memcpy(&sa6_any_downstream_storage, res->ai_addr, res->ai_addrlen);
		sa6_any_downstream =
		(const struct sockaddr_in6*)&sa6_any_downstream_storage;
	
	freeaddrinfo(res);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	
	error = getaddrinfo("::", DH6PORT_UPSTREAM, &hints, &res);
	if (error) {
		dprintf(LOG_ERR, FNAME, "getaddrinfo: %s",
		    gai_strerror(error));
		return (1);
	}
	
	memcpy(&sa6_any_relay_storage, res->ai_addr, res->ai_addrlen);
	sa6_any_relay =
		(const struct sockaddr_in6*)&sa6_any_relay_storage;	
	freeaddrinfo(res);


	return;
}

static void
server6_mainloop()
{
	struct timeval w;
	int ret;
	fd_set r;
	int maxsock;
	
	while (dhcp6s_running) {

		 dhcp6_check_timer(DHCP6_MODE_SERVER,&w);

		FD_ZERO(&r);
		FD_SET(insock, &r);
		maxsock = insock;
		w.tv_sec=2;w.tv_usec=0;//in order to check if the daemon need to exit
		ret = select(maxsock + 1, &r, NULL, NULL, &w);
		if (dhcp6s_running==0)
			break;
		switch (ret) {
		case -1:
			if (errno != EINTR) {
				dprintf(LOG_ERR, FNAME, "select: %s",
				    strerror(errno));
				//exit(1);
			}
			continue;
		case 0:		/* timeout */
			break;
		default:
			break;
		}
		if (ret>0){
		if (FD_ISSET(insock, &r))
			{			
			dprintf(LOG_DEBUG, FNAME, "server receive package");
			server6_recv(insock);
			}
	}
	}
	dprintf(LOG_ERR,FNAME,"Daemon Exit\n");

	dhcp6_clear_list(&arg_dnslist);
	dhcp6_clear_list(&dnslist);
	dhcp6_clear_list(&dnsnamelist);
	dhcp6_clear_list(&siplist);
	dhcp6_clear_list(&sipnamelist);
	dhcp6_clear_list(&ntplist);
	dhcp6_clear_list(&nislist);
	dhcp6_clear_list(&nisnamelist);
	dhcp6_clear_list(&nisplist);
	dhcp6_clear_list(&nispnamelist);
	dhcp6_clear_list(&bcmcslist);
	dhcp6_clear_list(&bcmcsnamelist);
	free_binding_list(&dhcp6_binding_head);
	free_dynamic_hostconf(&dynamic_hostconf_head);
	//free pool config
	{
		struct pool_conf *pool, *pool_next;

		dprintf(LOG_DEBUG, FNAME, "called");

		for (pool = pool_conflist; pool; pool = pool_next) {
			pool_next = pool->next;
			free(pool->name);
			free(pool);
		}
		free(prefix_poolspec);
		prefix_poolspec=NULL;
		pool_conflist = NULL;
	}
	ifclear(dhcp6_if_s);
	dhcp6_if_s = NULL;
	free(rmsgctlbuf);
	if (insock>0)
		{
		close(insock);
		insock = -1;
		}
	if (outsock>0)
		{
		close(outsock);
		outsock = -1;
		}

	//TODO:Check if there is any memory leakage
}


static void
server6_recv(s)
	int s;
{
	ssize_t len;
	struct sockaddr_storage from;
	int fromlen;
	struct msghdr mhdr;
	struct iovec iov;
	char cmsgbuf[BUFSIZ];
	struct cmsghdr *cm;
	struct in6_pktinfo *pi = NULL;
	struct dhcp6_if *ifp;
	struct dhcp6 *dh6;
	struct dhcp6_optinfo optinfo;
	struct dhcp6opt *optend;
	struct relayinfolist relayinfohead;
	struct relayinfo *relayinfo;

	TAILQ_INIT(&relayinfohead);

	memset(&iov, 0, sizeof(iov));
	memset(&mhdr, 0, sizeof(mhdr));

	iov.iov_base = rdatabuf;
	iov.iov_len = sizeof(rdatabuf);
	mhdr.msg_name = &from;
	mhdr.msg_namelen = sizeof(from);
	mhdr.msg_iov = &iov;
	mhdr.msg_iovlen = 1;
	mhdr.msg_control = (caddr_t)cmsgbuf;
	mhdr.msg_controllen = sizeof(cmsgbuf);

	if ((len = recvmsg(insock, &mhdr, 0)) < 0) {
		dprintf(LOG_ERR, FNAME, "recvmsg: %s", strerror(errno));
		return;
	}
	fromlen = mhdr.msg_namelen;

	for (cm = (struct cmsghdr *)CMSG_FIRSTHDR(&mhdr); cm;
	     cm = (struct cmsghdr *)CMSG_NXTHDR(&mhdr, cm)) {
		if (cm->cmsg_level == IPPROTO_IPV6 &&
		    cm->cmsg_type == IPV6_PKTINFO &&
		    cm->cmsg_len == CMSG_LEN(sizeof(struct in6_pktinfo))) {
			pi = (struct in6_pktinfo *)(CMSG_DATA(cm));
		}
	}
	if (pi == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to get packet info");
		return;
	}
	/*
	 * DHCPv6 server may receive a DHCPv6 packet from a non-listening 
	 * interface, when a DHCPv6 relay agent is running on that interface.
	 * This check prevents such reception.
	 */
	if (pi->ipi6_ifindex != ifidx)
		return;
	if ((ifp = find_ifconfbyid((unsigned int)pi->ipi6_ifindex,DHCP6_MODE_SERVER)) == NULL) {
		dprintf(LOG_INFO, FNAME, "unexpected interface (%d)",
		    (unsigned int)pi->ipi6_ifindex);
		return;
	}

	dh6 = (struct dhcp6 *)rdatabuf;

	if (len < sizeof(*dh6)) {
		dprintf(LOG_INFO, FNAME, "short packet (%d bytes)", len);
		return;
	}

	dprintf(LOG_DEBUG, FNAME, "received %s from %s",
	    dhcp6msgstr(dh6->dh6_msgtype),
	    addr2str((struct sockaddr *)&from));

	/*
	 * A server MUST discard any Solicit, Confirm, Rebind or
	 * Information-request messages it receives with a unicast
	 * destination address.
	 * [RFC3315 Section 15.]
	 */
	if (!IN6_IS_ADDR_MULTICAST(&pi->ipi6_addr) &&
	    (dh6->dh6_msgtype == DH6_SOLICIT ||
	    dh6->dh6_msgtype == DH6_CONFIRM ||
	    dh6->dh6_msgtype == DH6_REBIND ||
	    dh6->dh6_msgtype == DH6_INFORM_REQ)) {
		dprintf(LOG_INFO, FNAME, "invalid unicast message");
		return;
	}

	/*
	 * A server never receives a relay reply message.  Since relay
	 * replay messages will annoy option parser below, we explicitly
	 * reject them here.
	 */
	if (dh6->dh6_msgtype == DH6_RELAY_REPLY) {
		dprintf(LOG_INFO, FNAME, "relay reply message from %s",
		    addr2str((struct sockaddr *)&from));
		return;
		
	}

	optend = (struct dhcp6opt *)(rdatabuf + len);
	if (dh6->dh6_msgtype == DH6_RELAY_FORW) {
		if (process_relayforw(&dh6, &optend, &relayinfohead,
		    (struct sockaddr *)&from)) {
			goto end;
		}
		/* dh6 and optend should have been updated. */
		len = (ssize_t)((char *)optend - (char *)dh6);
	}

	/*
	 * parse and validate options in the message
	 */
	dhcp6_init_options(&optinfo);
	if (dhcp6_get_options((struct dhcp6opt *)(dh6 + 1),
	    optend, &optinfo) < 0) {
		dprintf(LOG_INFO, FNAME, "failed to parse options");
		goto end;
	}

	switch (dh6->dh6_msgtype) {
	case DH6_SOLICIT:
		(void)react_solicit(ifp, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_REQUEST:
		(void)react_request(ifp, pi, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_RENEW:
		(void)react_renew(ifp, pi, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_REBIND:
		(void)react_rebind(ifp, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_RELEASE:
		(void)react_release(ifp, pi, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_DECLINE:
		/*
		*RFC3633:12.1
		*The Confirm and Decline message types are not used with Prefix Delegation
		*/
		if (delegation_router)
			break;
		(void)react_decline(ifp, pi, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_CONFIRM:
		/*
		*RFC3633:12.1
		*The Confirm and Decline message types are not used with Prefix Delegation
		*/
		if (delegation_router)
			break;
		(void)react_confirm(ifp, pi, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	case DH6_INFORM_REQ:
		(void)react_informreq(ifp, dh6, len, &optinfo,
		    (struct sockaddr *)&from, fromlen, &relayinfohead);
		break;
	default:
		dprintf(LOG_INFO, FNAME, "unknown or unsupported msgtype (%s)",
		    dhcp6msgstr(dh6->dh6_msgtype));
		break;
	}

	dhcp6_clear_options(&optinfo);

  end:
	while ((relayinfo = TAILQ_FIRST(&relayinfohead)) != NULL) {
		TAILQ_REMOVE(&relayinfohead, relayinfo, link);
		free_relayinfo(relayinfo);
	}

	return;
}

static void
free_relayinfo(relayinfo)
	struct relayinfo *relayinfo;
{
	if (relayinfo->relay_ifid.dv_buf)
		dhcp6_vbuf_free(&relayinfo->relay_ifid);

	if (relayinfo->relay_msg.dv_buf)
		dhcp6_vbuf_free(&relayinfo->relay_msg);

	free(relayinfo);
}

static int
process_relayforw(dh6p, optendp, relayinfohead, from)
	struct dhcp6 **dh6p;
	struct dhcp6opt **optendp;
	struct relayinfolist *relayinfohead;
	struct sockaddr *from;
{
	struct dhcp6_relay *dh6relay = (struct dhcp6_relay *)*dh6p;
	struct dhcp6opt *optend = *optendp;
	struct relayinfo *relayinfo;
	struct dhcp6_optinfo optinfo;
	int len;

  dprintf(LOG_INFO, FNAME, "Entry");

  again:
	len = (void *)optend - (void *)dh6relay;
	if (len < sizeof (*dh6relay)) {
		dprintf(LOG_INFO, FNAME, "short relay message from %s",
		    addr2str(from));
		return (-1);
	}
	  dprintf(LOG_INFO, FNAME, "1");
	  /*
	dprintf(LOG_DEBUG, FNAME,
	    "dhcp6 relay: hop=%d, linkaddr=%s, peeraddr=%s",
	    dh6relay->dh6relay_hcnt,
	    in6addr2str(&dh6relay->dh6relay_linkaddr, 0),
	    in6addr2str(&dh6relay->dh6relay_peeraddr, 0));
	dprintf(LOG_INFO, FNAME, "2");*/

	/*
	 * parse and validate options in the relay forward message.
	 */
	dhcp6_init_options(&optinfo);
	if (dhcp6_get_options((struct dhcp6opt *)(dh6relay + 1),
	    optend, &optinfo) < 0) {
		dprintf(LOG_INFO, FNAME, "failed to parse options");
		return (-1);
	}

	/* A relay forward message must include a relay message option */
	if (optinfo.relaymsg_msg == NULL) {
		dprintf(LOG_INFO, FNAME, "relay forward from %s "
		    "without a relay message", addr2str(from));
		return (-1);
	}

	/* relay message must contain a DHCPv6 message. */
	len = optinfo.relaymsg_len;
	if (len < sizeof (struct dhcp6)) {
		dprintf(LOG_INFO, FNAME,
		    "short packet (%d bytes) in relay message", len);
		return (-1);
	}

	if ((relayinfo = malloc(sizeof (*relayinfo))) == NULL) {
		dprintf(LOG_ERR, FNAME, "failed to allocate relay info");
		return (-1);
	}
	memset(relayinfo, 0, sizeof (*relayinfo));

	relayinfo->hcnt = dh6relay->dh6relay_hcnt;
	memcpy(&relayinfo->linkaddr, &dh6relay->dh6relay_linkaddr,
	    sizeof (relayinfo->linkaddr));
	memcpy(&relayinfo->peeraddr, &dh6relay->dh6relay_peeraddr,
	    sizeof (relayinfo->peeraddr));

	if (dhcp6_vbuf_copy(&relayinfo->relay_msg, &optinfo.relay_msg))
		goto fail;
	if (optinfo.ifidopt_id &&
	    dhcp6_vbuf_copy(&relayinfo->relay_ifid, &optinfo.ifidopt)) {
		goto fail;
	}

	TAILQ_INSERT_HEAD(relayinfohead, relayinfo, link);

	dhcp6_clear_options(&optinfo);

	optend = (struct dhcp6opt *)(relayinfo->relay_msg.dv_buf + len);
	dh6relay = (struct dhcp6_relay *)relayinfo->relay_msg.dv_buf;
	dprintf(LOG_INFO, FNAME, "here 1");

	if (dh6relay->dh6relay_msgtype != DH6_RELAY_FORW) {
		*dh6p = (struct dhcp6 *)dh6relay;
		*optendp = optend;
		return (0);
	}

	goto again;

  fail:
	free_relayinfo(relayinfo);
	dhcp6_clear_options(&optinfo);

	return (-1);
}

/*
 * Set stateless configuration information to a option structure.
 * It is the caller's responsibility to deal with error cases.
 */
static int
set_statelessinfo(type, optinfo)
	int type;
	struct dhcp6_optinfo *optinfo;
{
	/* SIP domain name */
	if (dhcp6_copy_list(&optinfo->sipname_list, &sipnamelist)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to copy SIP domain list");
		return (-1);
	}

	/* SIP server */
	if (dhcp6_copy_list(&optinfo->sip_list, &siplist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy SIP servers");
		return (-1);
	}

	/* DNS server */
	if (dhcp6_copy_list(&optinfo->dns_list, &dnslist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy DNS servers");
		return (-1);
	}

	/* DNS search list */
	if (dhcp6_copy_list(&optinfo->dnsname_list, &dnsnamelist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy DNS search list");
		return (-1);
	}

	/* NTP server */
	if (dhcp6_copy_list(&optinfo->ntp_list, &ntplist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy NTP servers");
		return (-1);
	}

	/* NIS domain name */
	if (dhcp6_copy_list(&optinfo->nisname_list, &nisnamelist)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to copy NIS domain list");
		return (-1);
	}

	/* NIS server */
	if (dhcp6_copy_list(&optinfo->nis_list, &nislist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy NIS servers");
		return (-1);
	}

	/* NIS+ domain name */
	if (dhcp6_copy_list(&optinfo->nispname_list, &nispnamelist)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to copy NIS+ domain list");
		return (-1);
	}

	/* NIS+ server */
	if (dhcp6_copy_list(&optinfo->nisp_list, &nisplist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy NIS+ servers");
		return (-1);
	}

	/* BCMCS domain name */
	if (dhcp6_copy_list(&optinfo->bcmcsname_list, &bcmcsnamelist)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to copy BCMCS domain list");
		return (-1);
	}

	/* BCMCS server */
	if (dhcp6_copy_list(&optinfo->bcmcs_list, &bcmcslist)) {
		dprintf(LOG_ERR, FNAME, "failed to copy BCMCS servers");
		return (-1);
	}

	/*
	 * Information refresh time.  Only include in a response to
	 * an Information-request message.
	 */
	if (type == DH6_INFORM_REQ &&
	    optrefreshtime != DH6OPT_REFRESHTIME_UNDEF) {
		optinfo->refreshtime = (int64_t)optrefreshtime;
	}

	return (0);
}

static int
react_solicit(ifp, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct host_conf *client_conf;
	int resptype, do_binding = 0, error;

	/*
	 * Servers MUST discard any Solicit messages that do not include a
	 * Client Identifier option.
	 * [RFC3315 Section 15.2]
	 */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	} else {
		dprintf(LOG_DEBUG, FNAME, "client ID %s",
		    duidstr(&optinfo->clientID));
	}

	/*
	 * Servers MUST discard any Solicit messages that do include a
	 * Server Identifier option.
	 * [RFC3315 Section 15.2]
	 */
	if (optinfo->serverID.duid_len) {
		dprintf(LOG_INFO, FNAME, "server ID option found");
		return (-1);
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME, "found a host configuration for %s",
		    client_conf->name);
	}

	/*
	 * configure necessary options based on the options in solicit.
	 */
	dhcp6_init_options(&roptinfo);

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}

	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* preference (if configured) */
	if (ifp->server_pref != DH6OPT_PREF_UNDEF)
		roptinfo.pref = ifp->server_pref;

	/* add other configuration information */
	if (set_statelessinfo(DH6_SOLICIT, &roptinfo)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to set other stateless information");
		goto fail;
	}

	/*
	 * see if we have information for requested options, and if so,
	 * configure corresponding options.
	 */
	if (optinfo->rapidcommit && (ifp->allow_flags & DHCIFF_RAPID_COMMIT))
		do_binding = 1;

	/*
	 * The delegating router MUST include an IA_PD option, identifying any
	 * prefix(es) that the delegating router will delegate to the
	 * requesting router.  [RFC3633 Section 11.2]
	 */
	if (!TAILQ_EMPTY(&optinfo->iapd_list)) {
		int found = 0;
		struct dhcp6_list conflist;
		struct dhcp6_listval *iapd;

		TAILQ_INIT(&conflist);
		if (client_conf == NULL && prefix_poolspec->name) {
			if ((client_conf = create_dynamic_hostconf(&optinfo->clientID,
				prefix_poolspec)) == NULL)
				dprintf(LOG_NOTICE, FNAME,
			    	"failed to make host configuration");
		}

		/* make a local copy of the configured prefixes */
		if (client_conf &&
		    dhcp6_copy_list(&conflist, &client_conf->prefix_list)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make local data");
			goto fail;
		}

		for (iapd = TAILQ_FIRST(&optinfo->iapd_list); iapd;
		    iapd = TAILQ_NEXT(iapd, link)) {
			/*
			 * find an appropriate prefix for each IA_PD,
			 * removing the adopted prefixes from the list.
			 * (dhcp6s cannot create IAs without client config)
			 */
			if (client_conf &&
			    make_ia(iapd, &conflist, &roptinfo.iapd_list,
			    client_conf, do_binding) > 0)
				found = 1;
		}

		dhcp6_clear_list(&conflist);

		if (!found) {
			/*
			 * If the delegating router will not assign any
			 * prefixes to any IA_PDs in a subsequent Request from
			 * the requesting router, the delegating router MUST
			 * send an Advertise message to the requesting router
			 * that includes a Status Code option with code
			 * NoPrefixAvail.
			 * [dhcpv6-opt-prefix-delegation-01 Section 10.2]
			 */
			u_int16_t stcode = DH6OPT_STCODE_NOPREFIXAVAIL;

			if (dhcp6_add_listval(&roptinfo.stcode_list,
			    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL)
				goto fail;
		}
	}

	if (!TAILQ_EMPTY(&optinfo->iana_list)) {
		int found = 0;
		struct dhcp6_list conflist;
		struct dhcp6_listval *iana;

		if (client_conf == NULL && ifp->pool.name) {
			if ((client_conf = create_dynamic_hostconf(&optinfo->clientID,
				&ifp->pool)) == NULL)
				dprintf(LOG_NOTICE, FNAME,
			    	"failed to make host configuration");
		}
		TAILQ_INIT(&conflist);

		/* make a local copy of the configured addresses */
		if (client_conf &&
		    dhcp6_copy_list(&conflist, &client_conf->addr_list)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make local data");
			goto fail;
		}

		for (iana = TAILQ_FIRST(&optinfo->iana_list); iana;
		    iana = TAILQ_NEXT(iana, link)) {
			/*
			 * find an appropriate address for each IA_NA,
			 * removing the adopted addresses from the list.
			 * (dhcp6s cannot create IAs without client config)
			 */
			if (client_conf &&
			    make_ia(iana, &conflist, &roptinfo.iana_list,
			    client_conf, do_binding) > 0)
				found = 1;
		}

		dhcp6_clear_list(&conflist);

		if (!found) {
			u_int16_t stcode = DH6OPT_STCODE_NOADDRSAVAIL;

			if (dhcp6_add_listval(&roptinfo.stcode_list,
			    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL)
				goto fail;
		}
	}

	if (optinfo->rapidcommit && (ifp->allow_flags & DHCIFF_RAPID_COMMIT)) {
		/*
		 * If the client has included a Rapid Commit option and the
		 * server has been configured to respond with committed address
		 * assignments and other resources, responds to the Solicit
		 * with a Reply message.
		 * [RFC3315 Section 17.2.1]
		 */
		roptinfo.rapidcommit = 1;
		resptype = DH6_REPLY;
	} else
		resptype = DH6_ADVERTISE;

	error = server6_send(resptype, ifp, dh6, optinfo, from, fromlen,
			     &roptinfo, relayinfohead, client_conf);
	dhcp6_clear_options(&roptinfo);
	return (error);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_request(ifp, pi, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct in6_pktinfo *pi;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct host_conf *client_conf;
	int ret = 0;

	/* message validation according to Section 15.4 of RFC3315 */

	/* the message must include a Server Identifier option */
	if (optinfo->serverID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no server ID option");
		return (-1);
	}
	/* the contents of the Server Identifier option must match ours */
	if (duidcmp(&optinfo->serverID, &server_duid)) {
		dprintf(LOG_INFO, FNAME, "server ID mismatch");
		return (-1);
	}
	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/*
	 * When the server receives a Request message via unicast from a
	 * client to which the server has not sent a unicast option, the server
	 * discards the Request message and responds with a Reply message
	 * containing a Status Code option with value UseMulticast, a Server
	 * Identifier option containing the server's DUID, the Client
	 * Identifier option from the client message and no other options.
	 * [RFC3315 18.2.1]
	 * (Our current implementation never sends a unicast option.)
	 * Note: a request message encapsulated in a relay server option can be
	 * unicasted.
	 */
	if (!IN6_IS_ADDR_MULTICAST(&pi->ipi6_addr) &&
	    TAILQ_EMPTY(relayinfohead)) {
		u_int16_t stcode = DH6OPT_STCODE_USEMULTICAST;

		dprintf(LOG_INFO, FNAME, "unexpected unicast message from %s",
		    addr2str(from));
		if (dhcp6_add_listval(&roptinfo.stcode_list,
		    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
			dprintf(LOG_ERR, FNAME, "failed to add a status code");
			goto fail;
		}
		server6_send(DH6_REPLY, ifp, dh6, optinfo, from,
		    fromlen, &roptinfo, relayinfohead, client_conf);
		goto end;
	}

	/*
	 * See if we have to make a binding of some configuration information
	 * for the client.
	 */
	
	/*
	 * When a delegating router receives a Request message from a
	 * requesting router that contains an IA_PD option, and the delegating
	 * router is authorized to delegate prefix(es) to the requesting
	 * router, the delegating router selects the prefix(es) to be delegated
	 * to the requesting router.
	 * [RFC3633 Section 12.2]
	 */
	if (!TAILQ_EMPTY(&optinfo->iapd_list)) {
		struct dhcp6_list conflist;
		struct dhcp6_listval *iapd;

		TAILQ_INIT(&conflist);

		/* make a local copy of the configured prefixes */
		if (client_conf &&
		    dhcp6_copy_list(&conflist, &client_conf->prefix_list)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make local data");
			goto fail;
		}

		for (iapd = TAILQ_FIRST(&optinfo->iapd_list); iapd;
		    iapd = TAILQ_NEXT(iapd, link)) {
			/*
			 * Find an appropriate prefix for each IA_PD,
			 * removing the adopted prefixes from the list.
			 * The prefixes will be bound to the client.
			 */
			if ((ret=make_ia(iapd, &conflist, &roptinfo.iapd_list,
			    client_conf, 1)) == 0) {
				/*
				 * We could not find any prefixes for the IA.
				 * RFC3315 specifies to include NoAddrsAvail
				 * for the IA in the address configuration
				 * case (Section 18.2.1).  We follow the same
				 * logic for prefix delegation as well.
				 */
				if (make_ia_stcode(DHCP6_LISTVAL_IAPD,
				    iapd->val_ia.iaid,
				    DH6OPT_STCODE_NOPREFIXAVAIL,
				    &roptinfo.iapd_list)) {
					dprintf(LOG_NOTICE, FNAME,
					    "failed to make an option list");
					dhcp6_clear_list(&conflist);
					goto fail;
				}
			}

		}

		dhcp6_clear_list(&conflist);
	}

	if (!TAILQ_EMPTY(&optinfo->iana_list)) {
		struct dhcp6_list conflist;
		struct dhcp6_listval *iana;

		if (client_conf == NULL && ifp->pool.name) {
			if ((client_conf = create_dynamic_hostconf(&optinfo->clientID,
				&ifp->pool)) == NULL)
				dprintf(LOG_NOTICE, FNAME,
			    	"failed to make host configuration");
		}
		TAILQ_INIT(&conflist);

		/* make a local copy of the configured prefixes */
		if (client_conf &&
		    dhcp6_copy_list(&conflist, &client_conf->addr_list)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make local data");
			goto fail;
		}

		for (iana = TAILQ_FIRST(&optinfo->iana_list); iana;
		    iana = TAILQ_NEXT(iana, link)) {
			/*
			 * Find an appropriate address for each IA_NA,
			 * removing the adopted addresses from the list.
			 * The addresses will be bound to the client.
			 */
			if ((ret =make_ia(iana, &conflist, &roptinfo.iana_list,
			    client_conf, 1)) == 0) {
				if (make_ia_stcode(DHCP6_LISTVAL_IANA,
				    iana->val_ia.iaid,
				    DH6OPT_STCODE_NOADDRSAVAIL,
				    &roptinfo.iana_list)) {
					dprintf(LOG_NOTICE, FNAME,
					    "failed to make an option list");
					dhcp6_clear_list(&conflist);
					goto fail;
				}
			}			
		if (ret == -2&&make_ia_stcode(DHCP6_LISTVAL_IANA,
				    iana->val_ia.iaid,
				    DH6OPT_STCODE_NOTONLINK,
				    &roptinfo.iana_list)) {
					dprintf(LOG_NOTICE, FNAME,
					    "failed to make an option list");
					dhcp6_clear_list(&conflist);
					goto fail;
				}
		}

		dhcp6_clear_list(&conflist);
	}

	/*
	 * If the Request message contained an Option Request option, the
	 * server MUST include options in the Reply message for any options in
	 * the Option Request option the server is configured to return to the
	 * client.
	 * [RFC3315 18.2.1]
	 * Note: our current implementation always includes all information
	 * that we can provide.  So we do not have to check the option request
	 * options.
	 */
#if 0
	for (opt = TAILQ_FIRST(&optinfo->reqopt_list); opt;
	     opt = TAILQ_NEXT(opt, link)) {
		;
	}
#endif

	/*
	 * Add options to the Reply message for any other configuration
	 * information to be assigned to the client.
	 */
	if (set_statelessinfo(DH6_REQUEST, &roptinfo)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to set other stateless information");
		goto fail;
	}

	/* send a reply message. */
	(void)server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, client_conf);

  end:
	dhcp6_clear_options(&roptinfo);
	return (0);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_renew(ifp, pi, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct in6_pktinfo *pi;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct dhcp6_listval *ia;
	struct host_conf *client_conf;

	/* message validation according to Section 15.6 of RFC3315 */

	/* the message must include a Server Identifier option */
	if (optinfo->serverID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no server ID option");
		return (-1);
	}
	/* the contents of the Server Identifier option must match ours */
	if (duidcmp(&optinfo->serverID, &server_duid)) {
		dprintf(LOG_INFO, FNAME, "server ID mismatch");
		return (-1);
	}
	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/*
	 * When the server receives a Renew message via unicast from a
	 * client to which the server has not sent a unicast option, the server
	 * discards the Request message and responds with a Reply message
	 * containing a status code option with value UseMulticast, a Server
	 * Identifier option containing the server's DUID, the Client
	 * Identifier option from the client message and no other options.
	 * [RFC3315 18.2.3]
	 * (Our current implementation never sends a unicast option.)
	 */
	if (!IN6_IS_ADDR_MULTICAST(&pi->ipi6_addr) &&
	    TAILQ_EMPTY(relayinfohead)) {
		u_int16_t stcode = DH6OPT_STCODE_USEMULTICAST;

		dprintf(LOG_INFO, FNAME, "unexpected unicast message from %s",
		    addr2str(from));
		if (dhcp6_add_listval(&roptinfo.stcode_list,
		    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
			dprintf(LOG_ERR, FNAME, "failed to add a status code");
			goto fail;
		}
		server6_send(DH6_REPLY, ifp, dh6, optinfo, from,
		    fromlen, &roptinfo, relayinfohead, client_conf);
		goto end;
	}

	/*
	 * Locates the client's binding and verifies that the information
	 * from the client matches the information stored for that client.
	 */
	for (ia = TAILQ_FIRST(&optinfo->iapd_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (update_ia(DH6_RENEW, ia, &roptinfo.iapd_list, optinfo))
			goto fail;
	}
	for (ia = TAILQ_FIRST(&optinfo->iana_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (update_ia(DH6_RENEW, ia, &roptinfo.iana_list, optinfo))
			goto fail;
	}

	/* add other configuration information */
	if (set_statelessinfo(DH6_RENEW, &roptinfo)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to set other stateless information");
		goto fail;
	}

	(void)server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, client_conf);

  end:
	dhcp6_clear_options(&roptinfo);
	return (0);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_rebind(ifp, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct dhcp6_listval *ia;
	struct host_conf *client_conf;

	/* message validation according to Section 15.7 of RFC3315 */

	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	/* the message must not include a server Identifier option */
	if (optinfo->serverID.duid_len) {
		dprintf(LOG_INFO, FNAME, "server ID option is included in "
		    "a rebind message");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/*
	 * Locates the client's binding and verifies that the information
	 * from the client matches the information stored for that client.
	 */
	for (ia = TAILQ_FIRST(&optinfo->iapd_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (update_ia(DH6_REBIND, ia, &roptinfo.iapd_list, optinfo))
			goto fail;
	}
	for (ia = TAILQ_FIRST(&optinfo->iana_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (update_ia(DH6_REBIND, ia, &roptinfo.iana_list, optinfo))
			goto fail;
	}

	/*
	 * If the returned iana/pd_list is empty, we do not have an explicit
	 * knowledge about validity nor invalidity for any IA_NA/PD information
	 * in the Rebind message.  In this case, we should rather ignore the
	 * message than to send a Reply with empty information back to the
	 * client, which may annoy the recipient.  However, if we have at least
	 * one useful information, either positive or negative, based on some
	 * explicit knowledge, we should reply with the responsible part.
	 */
	if (TAILQ_EMPTY(&roptinfo.iapd_list) &&
	    TAILQ_EMPTY(&roptinfo.iana_list)) {
		dprintf(LOG_INFO, FNAME, "no useful information for a rebind");
		goto fail;	/* discard the rebind */
	}

	/* add other configuration information */
	if (set_statelessinfo(DH6_REBIND, &roptinfo)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to set other stateless information");
		goto fail;
	}

	(void)server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, client_conf);

	dhcp6_clear_options(&roptinfo);
	return (0);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_release(ifp, pi, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct in6_pktinfo *pi;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct dhcp6_listval *ia;
	struct host_conf *client_conf;
	u_int16_t stcode;

	/* message validation according to Section 15.9 of RFC3315 */

	/* the message must include a Server Identifier option */
	if (optinfo->serverID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no server ID option");
		return (-1);
	}
	/* the contents of the Server Identifier option must match ours */
	if (duidcmp(&optinfo->serverID, &server_duid)) {
		dprintf(LOG_INFO, FNAME, "server ID mismatch");
		return (-1);
	}
	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/*
	 * When the server receives a Release message via unicast from a
	 * client to which the server has not sent a unicast option, the server
	 * discards the Release message and responds with a Reply message
	 * containing a Status Code option with value UseMulticast, a Server
	 * Identifier option containing the server's DUID, the Client
	 * Identifier option from the client message and no other options.
	 * [RFC3315 18.2.6]
	 * (Our current implementation never sends a unicast option.)
	 */
	if (!IN6_IS_ADDR_MULTICAST(&pi->ipi6_addr) &&
	    TAILQ_EMPTY(relayinfohead)) {
		u_int16_t stcode = DH6OPT_STCODE_USEMULTICAST;

		dprintf(LOG_INFO, FNAME, "unexpected unicast message from %s",
		    addr2str(from));
		if (dhcp6_add_listval(&roptinfo.stcode_list,
		    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
			dprintf(LOG_ERR, FNAME, "failed to add a status code");
			goto fail;
		}
		server6_send(DH6_REPLY, ifp, dh6, optinfo, from,
		    fromlen, &roptinfo, relayinfohead, client_conf);
		goto end;
	}

	/*
	 * Locates the client's binding and verifies that the information
	 * from the client matches the information stored for that client.
	 */
	for (ia = TAILQ_FIRST(&optinfo->iapd_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (release_binding_ia(ia, &roptinfo.iapd_list, optinfo))
			goto fail;
	}
	for (ia = TAILQ_FIRST(&optinfo->iana_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (release_binding_ia(ia, &roptinfo.iana_list, optinfo))
			goto fail;
	}

	/*
	 * After all the addresses have been processed, the server generates a
	 * Reply message and includes a Status Code option with value Success.
	 * [RFC3315 Section 18.2.6]
	 */
	stcode = DH6OPT_STCODE_SUCCESS;
	if (dhcp6_add_listval(&roptinfo.stcode_list,
	    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to add a status code");
		goto fail;
	}

	(void)server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, client_conf);

  end:
	dhcp6_clear_options(&roptinfo);
	return (0);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_decline(ifp, pi, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct in6_pktinfo *pi;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct dhcp6_listval *ia;
	struct host_conf *client_conf;
	u_int16_t stcode;

	/* message validation according to Section 15.8 of RFC3315 */

	/* the message must include a Server Identifier option */
	if (optinfo->serverID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no server ID option");
		return (-1);
	}
	/* the contents of the Server Identifier option must match ours */
	if (duidcmp(&optinfo->serverID, &server_duid)) {
		dprintf(LOG_INFO, FNAME, "server ID mismatch");
		return (-1);
	}
	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	/*
	 * When the server receives a Decline message via unicast from a
	 * client to which the server has not sent a unicast option, the server
	 * discards the Decline message and responds with a Reply message
	 * containing a Status Code option with value UseMulticast, a Server
	 * Identifier option containing the server's DUID, the Client
	 * Identifier option from the client message and no other options.
	 * [RFC3315 18.2.6]
	 * (Our current implementation never sends a unicast option.)
	 */
	if (!IN6_IS_ADDR_MULTICAST(&pi->ipi6_addr) &&
	    TAILQ_EMPTY(relayinfohead)) {
		stcode = DH6OPT_STCODE_USEMULTICAST;

		dprintf(LOG_INFO, FNAME, "unexpected unicast message from %s",
		    addr2str(from));
		if (dhcp6_add_listval(&roptinfo.stcode_list,
		    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
			dprintf(LOG_ERR, FNAME, "failed to add a status code");
			goto fail;
		}
		server6_send(DH6_REPLY, ifp, dh6, optinfo, from,
		    fromlen, &roptinfo, relayinfohead, client_conf);
		goto end;
	}

	/*
	 * Locates the client's binding on IA-NA and verifies that the
	 * information from the client matches the information stored
	 * for that client.  (IA-PD is just ignored [RFC3633 12.1])
	 */
	for (ia = TAILQ_FIRST(&optinfo->iana_list); ia;
	    ia = TAILQ_NEXT(ia, link)) {
		if (decline_binding_ia(ia, &roptinfo.iana_list, optinfo))
			goto fail;
	}

	/*
	 * After all the addresses have been processed, the server generates a
	 * Reply message and includes a Status Code option with value Success.
	 * [RFC3315 Section 18.2.7]
	 */
	stcode = DH6OPT_STCODE_SUCCESS;
	if (dhcp6_add_listval(&roptinfo.stcode_list,
	    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to add a status code");
		goto fail;
	}

	(void)server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, client_conf);

  end:
	dhcp6_clear_options(&roptinfo);
	return (0);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
react_confirm(ifp, pi, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct in6_pktinfo *pi;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	struct dhcp6_list conflist;
	struct dhcp6_listval *iana, *iaaddr;
	struct host_conf *client_conf;
	u_int16_t stcode = DH6OPT_STCODE_SUCCESS;
	int error;

	/* message validation according to Section 15.5 of RFC3315 */

	/* the message may not include a Server Identifier option */
	if (optinfo->serverID.duid_len) {
		dprintf(LOG_INFO, FNAME, "server ID option found");
		return (-1);
	}
	/* the message must include a Client Identifier option */
	if (optinfo->clientID.duid_len == 0) {
		dprintf(LOG_INFO, FNAME, "no client ID option");
		return (-1);
	}

	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}
	/* copy client information back */
	if (duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	/* process authentication */
	if (process_auth(dh6, len, client_conf, optinfo, &roptinfo)) {
		dprintf(LOG_INFO, FNAME, "failed to process authentication "
		    "information for %s",
		    clientstr(client_conf, &optinfo->clientID));
		goto fail;
	}

	if (client_conf == NULL && ifp->pool.name) {
		if ((client_conf = create_dynamic_hostconf(&optinfo->clientID,
			&ifp->pool)) == NULL) {
			dprintf(LOG_NOTICE, FNAME,
		    	"failed to make host configuration");
			goto fail;
		}
	}
	TAILQ_INIT(&conflist);
	/* make a local copy of the configured addresses */
	if (dhcp6_copy_list(&conflist, &client_conf->addr_list)) {
		dprintf(LOG_NOTICE, FNAME,
		    "failed to make local data");
		goto fail;
	}

	/*
	 * the message must include an IPv6 address to be confirmed
	 * [RFC3315 18.2]. (IA-PD is just ignored [RFC3633 12.1])
	 */
	if (TAILQ_EMPTY(&optinfo->iana_list)) {
		dprintf(LOG_INFO, FNAME, "no IA-NA option found");
		goto fail;
	}
	for (iana = TAILQ_FIRST(&optinfo->iana_list); iana;
	    iana = TAILQ_NEXT(iana, link)) {
		if (TAILQ_EMPTY(&iana->sublist)) {
			dprintf(LOG_INFO, FNAME,
			    "no IA-ADDR option found in IA-NA %d",
			    iana->val_ia.iaid);
			goto fail;
		}

		/*
		 * check whether the confirmed prefix matches 
		 * the prefix from where the message originates.
		 * XXX: prefix length is assumed to be 64
		 */
		for (iaaddr = TAILQ_FIRST(&iana->sublist); iaaddr;
		    iaaddr = TAILQ_NEXT(iaaddr, link)) {
		
			struct in6_addr *confaddr = &iaaddr->val_statefuladdr6.addr;
			struct in6_addr *linkaddr;
			struct sockaddr_in6 *src = (struct sockaddr_in6 *)from;

			if (!IN6_IS_ADDR_LINKLOCAL(&src->sin6_addr)) {
				/* CONFIRM is relayed via a DHCP-relay */
				struct relayinfo *relayinfo;

				if (relayinfohead == NULL) {
					dprintf(LOG_INFO, FNAME,
					    "no link-addr found");
					goto fail;
				}
				relayinfo = TAILQ_LAST(relayinfohead, relayinfolist);

				/* XXX: link-addr is supposed to be a global address */
				linkaddr = &relayinfo->linkaddr;
			} else {
				/* CONFIRM is directly arrived */
				linkaddr = &ifp->addr;
			}

			if (memcmp(linkaddr, confaddr, 8) != 0) {
				dprintf(LOG_INFO, FNAME,
				    "%s does not seem to belong to %s's link",
				    in6addr2str(confaddr, 0),
				    in6addr2str(linkaddr, 0));
				stcode = DH6OPT_STCODE_NOTONLINK;
				goto send_reply;
			}
		}
	}

	/* 
	 * even when the given address seems to be on the appropriate link,
	 * the confirm should be ignore if there's no corrensponding IA-NA
	 * configuration.
	 */
	for (iana = TAILQ_FIRST(&optinfo->iana_list); iana;
	    iana = TAILQ_NEXT(iana, link)) {
		if (make_ia(iana, &conflist, /*&roptinfo.iana_list*/NULL,
		    client_conf, 1) == 0) {
			dprintf(LOG_DEBUG, FNAME,
			    "IA-NA configuration not found");
			goto fail;
		}
	}

send_reply:
	if (dhcp6_add_listval(&roptinfo.stcode_list,
	    DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL)
		goto fail;
	error = server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
			     &roptinfo, relayinfohead, client_conf);

	dhcp6_clear_options(&roptinfo);
	dhcp6_clear_list(&conflist);

	return (error);

  fail:
	dhcp6_clear_options(&roptinfo);
	dhcp6_clear_list(&conflist);
	return (-1);
}

static int
react_informreq(ifp, dh6, len, optinfo, from, fromlen, relayinfohead)
	struct dhcp6_if *ifp;
	struct dhcp6 *dh6;
	ssize_t len;
	struct dhcp6_optinfo *optinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
{
	struct dhcp6_optinfo roptinfo;
	int error;

	/*
	 * An IA option is not allowed to appear in an Information-request
	 * message.  Such a message SHOULD be discarded.
	 * [RFC3315 Section 15]
	 */
	if (!TAILQ_EMPTY(&optinfo->iapd_list)) {
		dprintf(LOG_INFO, FNAME,
		    "information request contains an IA_PD option");
		return (-1);
	}
	if (!TAILQ_EMPTY(&optinfo->iana_list)) {
		dprintf(LOG_INFO, FNAME,
		    "information request contains an IA_NA option");
		return (-1);
	}

	/* if a server identifier is included, it must match ours. */
	if (optinfo->serverID.duid_len &&
	    duidcmp(&optinfo->serverID, &server_duid)) {
		dprintf(LOG_INFO, FNAME, "server DUID mismatch");
		return (-1);
	}

	/*
	 * configure necessary options based on the options in request.
	 */
	dhcp6_init_options(&roptinfo);

	/* server identifier option */
	if (duidcpy(&roptinfo.serverID, &server_duid)) {
		dprintf(LOG_ERR, FNAME, "failed to copy server ID");
		goto fail;
	}

	/* copy client information back (if provided) */
	if (optinfo->clientID.duid_id &&
	    duidcpy(&roptinfo.clientID, &optinfo->clientID)) {
		dprintf(LOG_ERR, FNAME, "failed to copy client ID");
		goto fail;
	}

	/* set stateless information */
	if (set_statelessinfo(DH6_INFORM_REQ, &roptinfo)) {
		dprintf(LOG_ERR, FNAME,
		    "failed to set other stateless information");
		goto fail;
	}

	error = server6_send(DH6_REPLY, ifp, dh6, optinfo, from, fromlen,
	    &roptinfo, relayinfohead, NULL);

	dhcp6_clear_options(&roptinfo);
	return (error);

  fail:
	dhcp6_clear_options(&roptinfo);
	return (-1);
}

static int
update_ia(msgtype, iap, retlist, optinfo)
	int msgtype;
	struct dhcp6_listval *iap;
	struct dhcp6_list *retlist;
	struct dhcp6_optinfo *optinfo;
{
	struct dhcp6_binding *binding;
	struct host_conf *client_conf;

	/* get per-host configuration for the client, if any. */
	if ((client_conf = find_hostconf(&optinfo->clientID))) {
		dprintf(LOG_DEBUG, FNAME,
		    "found a host configuration named %s", client_conf->name);
	}

	if ((binding = find_binding(&optinfo->clientID, DHCP6_BINDING_IA,
	    iap->type, iap->val_ia.iaid)) == NULL ) 
	    {
		/*
		 * Behavior in the case where the delegating router cannot
		 * find a binding for the requesting router's IA_PD as
		 * described in RFC3633 Section 12.2.  It is derived from
		 * Sections 18.2.3 and 18.2.4 of RFC3315, and the two sets
		 * of behavior are identical.
		 */
		dprintf(LOG_INFO, FNAME, "no binding found for %s",
		    duidstr(&optinfo->clientID));

		switch (msgtype) {
		case DH6_RENEW:
			/*
			 * If the delegating router cannot find a binding for
			 * the requesting router's IA_PD the delegating router
			 * returns the IA_PD containing no prefixes with a
			 * Status Code option set to NoBinding in the Reply
			 * message.
			 */
			if (make_ia_stcode(iap->type, iap->val_ia.iaid,
			    DH6OPT_STCODE_NOBINDING, retlist)) {
				dprintf(LOG_NOTICE, FNAME,
				    "failed to make an option list");
				return (-1);
			}
			break;
		case DH6_REBIND:
			/*
			 * If it can be determined the prefixes are not
			 * appropriate from the delegating router's explicit
			 * configuration, it MAY send a Reply message to
			 * the requesting router containing the IA_PD with the
			 * lifetimes of the prefixes in the IA_PD set to zero.
			 *
			 * If unable to determine, the Rebind message is
			 * discarded.
			 *
			 * XXX: it is not very clear what the explicit
			 * configuration means.  Thus, we always discard the
			 * message.
			 */
			return (-1);
		default:	/* XXX: should be a bug */
			dprintf(LOG_ERR, FNAME, "impossible message type %s",
			    dhcp6msgstr(msgtype));
			return (-1);
		}
	} else {	/* we found a binding */
		struct dhcp6_list ialist;
		struct dhcp6_listval *lv;
		struct dhcp6_prefix prefix;
		struct dhcp6_statefuladdr saddr;
		struct dhcp6_ia ia;

		TAILQ_INIT(&ialist);
		update_binding(binding);

		/* see if each information to be renewed is still valid. */
		for (lv = TAILQ_FIRST(&iap->sublist); lv;
		    lv = TAILQ_NEXT(lv, link)) {
			struct dhcp6_listval *blv;

			switch (iap->type) {
			case DHCP6_LISTVAL_IAPD:
				if (lv->type != DHCP6_LISTVAL_PREFIX6)
					continue;

				prefix = lv->val_prefix6;
				blv = dhcp6_find_listval(&binding->val_list,
				    DHCP6_LISTVAL_PREFIX6, &prefix, 0);
				if (blv == NULL) {
					dprintf(LOG_DEBUG, FNAME,
					    "%s/%d is not found in %s",
					    in6addr2str(&prefix.addr, 0),
					    prefix.plen, bindingstr(binding));
					prefix.pltime = 0;
					prefix.vltime = 0;
				} else {
					prefix.pltime =
					    blv->val_prefix6.pltime;
					prefix.vltime =
					    blv->val_prefix6.vltime;
				}
				//see above comment in switch case when the case is DH6_REBIND
				if ((blv || msgtype == DH6_REBIND)&& dhcp6_add_listval(&ialist,
				    DHCP6_LISTVAL_PREFIX6, &prefix, NULL)
				    == NULL) {
					dprintf(LOG_NOTICE, FNAME,
					    "failed  to copy binding info");
					dhcp6_clear_list(&ialist);
					return (-1);
				}
				if (blv == NULL)
				{
					int stcode = DH6OPT_STCODE_NOBINDING;
					if (dhcp6_add_listval(&ialist, DHCP6_LISTVAL_STCODE,
					&stcode, NULL) == NULL) {
							dprintf(LOG_NOTICE, FNAME, "failed to make an option list");
							return (-1);
						}
				}
				break;
			case DHCP6_LISTVAL_IANA:
				if (lv->type != DHCP6_LISTVAL_STATEFULADDR6)
					continue;

				saddr = lv->val_statefuladdr6;
				blv = dhcp6_find_listval(&binding->val_list,
				    DHCP6_LISTVAL_STATEFULADDR6, &saddr, 0);
				if (blv == NULL) {
					dprintf(LOG_DEBUG, FNAME,
					    "%s is not found in %s",
					    in6addr2str(&saddr.addr, 0),
					    bindingstr(binding));
					saddr.pltime = 0;
					saddr.vltime = 0;
				} else {
					saddr.pltime =
					    blv->val_statefuladdr6.pltime;
					saddr.vltime =
					    blv->val_statefuladdr6.vltime;
				}

				if (dhcp6_add_listval(&ialist,
				    DHCP6_LISTVAL_STATEFULADDR6, &saddr, NULL)
				    == NULL) {
					dprintf(LOG_NOTICE, FNAME,
					    "failed  to copy binding info");
					dhcp6_clear_list(&ialist);
					return (-1);
				}
				if (blv == NULL)
					{
					int stcode = DH6OPT_STCODE_NOBINDING;
					if (dhcp6_add_listval(&ialist, DHCP6_LISTVAL_STCODE,
					&stcode, NULL) == NULL) {
							dprintf(LOG_NOTICE, FNAME, "failed to make an option list");
							return (-1);
						}
					}
				break;
			default:
				dprintf(LOG_ERR, FNAME, "unsupported IA type");
				return (-1); /* XXX */
			}
		}

		memset(&ia, 0, sizeof(ia));
		ia.iaid = binding->iaid;
		/* determine appropriate T1 and T2 */
		calc_ia_timo(&ia, &ialist, client_conf);

		if (dhcp6_add_listval(retlist, iap->type,
		    &ia, &ialist) == NULL) {
			dhcp6_clear_list(&ialist);
			return (-1);
		}
		dhcp6_clear_list(&ialist);
	}

	return (0);
}

static int
release_binding_ia(iap, retlist, optinfo)
	struct dhcp6_listval *iap;
	struct dhcp6_list *retlist;
	struct dhcp6_optinfo *optinfo;
{
	struct dhcp6_binding *binding;

	if ((binding = find_binding(&optinfo->clientID, DHCP6_BINDING_IA,
	    iap->type, iap->val_ia.iaid)) == NULL) {
		/*
		 * For each IA in the Release message for which the server has
		 * no binding information, the server adds an IA option using
		 * the IAID from the Release message and includes a Status Code
		 * option with the value NoBinding in the IA option.
		 */
		if (make_ia_stcode(iap->type, iap->val_ia.iaid,
		    DH6OPT_STCODE_NOBINDING, retlist)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make an option list");
			return (-1);
		}
	} else {
		struct dhcp6_listval *lv, *lvia;

		/*
		 * If the IAs in the message are in a binding for the client
		 * and the addresses in the IAs have been assigned by the
		 * server to those IAs, the server deletes the addresses from
		 * the IAs and makes the addresses available for assignment to
		 * other clients.
		 * [RFC3315 Section 18.2.6]
		 * RFC3633 is not very clear about the similar case for IA_PD,
		 * but we apply the same logic.
		 */
		for (lv = TAILQ_FIRST(&iap->sublist); lv;
		    lv = TAILQ_NEXT(lv, link)) {
			if ((lvia = find_binding_ia(lv, binding)) != NULL) {
				switch (binding->iatype) {
					case DHCP6_LISTVAL_IAPD:
						dprintf(LOG_DEBUG, FNAME,
						    "bound prefix %s/%d "
						    "has been released",
						    in6addr2str(&lvia->val_prefix6.addr,
						    0),
						    lvia->val_prefix6.plen);
						break;
					case DHCP6_LISTVAL_IANA:
						release_address(&lvia->val_prefix6.addr);
						dprintf(LOG_DEBUG, FNAME,
						    "bound address %s "
						    "has been released",
						    in6addr2str(&lvia->val_prefix6.addr,
						    0));
						break;
				}

				TAILQ_REMOVE(&binding->val_list, lvia, link);
				dhcp6_clear_listval(lvia);
				if (TAILQ_EMPTY(&binding->val_list)) {
					/*
					 * if the binding has become empty,
					 * stop procedure.
					 */
					remove_binding(binding);
					return (0);
				}
			}
		}
	}

	return (0);
}

static int
decline_binding_ia(iap, retlist, optinfo)
	struct dhcp6_listval *iap;
	struct dhcp6_list *retlist;
	struct dhcp6_optinfo *optinfo;
{
	struct dhcp6_binding *binding;
	struct dhcp6_listval *lv, *lvia;

	if ((binding = find_binding(&optinfo->clientID, DHCP6_BINDING_IA,
	    iap->type, iap->val_ia.iaid)) == NULL) {
		/*
		 * For each IA in the Decline message for which the server has
		 * no binding information, the server adds an IA option using
		 * the IAID from the Release message and includes a Status Code
		 * option with the value NoBinding in the IA option.
		 */
		if (make_ia_stcode(iap->type, iap->val_ia.iaid,
		    DH6OPT_STCODE_NOBINDING, retlist)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to make an option list");
			return (-1);
		}

		return (0);
	}

	/*
	 * If the IAs in the message are in a binding for the client and the 
	 * addresses in the IAs have been assigned by the server to those IAs,
	 * the server deletes the addresses from the IAs and makes the addresses
	 * available for assignment to other clients. [RFC3315 Section 18.2.7]
	 */
	for (lv = TAILQ_FIRST(&iap->sublist); lv;
	    lv = TAILQ_NEXT(lv, link)) {
		if (binding->iatype != DHCP6_LISTVAL_IANA) {
			/* should never reach here */
			continue;
		}

		if ((lvia = find_binding_ia(lv, binding)) == NULL) {
			dprintf(LOG_DEBUG, FNAME, "no binding found "
			    "for address %s",
			    in6addr2str(&lv->val_statefuladdr6.addr, 0));
			continue;
		}

		dprintf(LOG_DEBUG, FNAME,
		    "bound address %s has been marked as declined",
		    in6addr2str(&lvia->val_statefuladdr6.addr, 0));
		decline_address(&lvia->val_statefuladdr6.addr);

		TAILQ_REMOVE(&binding->val_list, lvia, link);
		dhcp6_clear_listval(lvia);
		if (TAILQ_EMPTY(&binding->val_list)) {
			/*
			 * if the binding has become empty,
			 * stop procedure.
			 */
			remove_binding(binding);
			return (0);
		}
	}

	return (0);
}


static int
server6_send(type, ifp, origmsg, optinfo, from, fromlen,
    roptinfo, relayinfohead, client_conf)
	int type;
	struct dhcp6_if *ifp;
	struct dhcp6 *origmsg;
	struct dhcp6_optinfo *optinfo, *roptinfo;
	struct sockaddr *from;
	int fromlen;
	struct relayinfolist *relayinfohead;
	struct host_conf *client_conf;
{
	char replybuf[BUFSIZ];
	struct sockaddr_in6 dst;
	int len, optlen;
	int relayed = 0;
	struct dhcp6 *dh6;
	struct relayinfo *relayinfo;

	if (sizeof(struct dhcp6) > sizeof(replybuf)) {
		dprintf(LOG_ERR, FNAME, "buffer size assumption failed");
		return (-1);
	}

	dh6 = (struct dhcp6 *)replybuf;
	len = sizeof(*dh6);
	memset(dh6, 0, sizeof(*dh6));
	dh6->dh6_msgtypexid = origmsg->dh6_msgtypexid;
	dh6->dh6_msgtype = (u_int8_t)type;

	/* set options in the reply message */
	if ((optlen = dhcp6_set_options(type, (struct dhcp6opt *)(dh6 + 1),
	    (struct dhcp6opt *)(replybuf + sizeof(replybuf)), roptinfo)) < 0) {
		dprintf(LOG_INFO, FNAME, "failed to construct reply options");
		return (-1);
	}
	len += optlen;
	dprintf(LOG_INFO, FNAME, " 1");

	/* calculate MAC if necessary, and put it to the message */
	switch (roptinfo->authproto) {
	case DHCP6_AUTHPROTO_DELAYED:
		if (client_conf == NULL || client_conf->delayedkey == NULL) {
			/* This case should have been caught earlier */
			dprintf(LOG_ERR, FNAME, "authentication required "
			    "but not key provided");
			break;
		}
		if (dhcp6_calc_mac((char *)dh6, len, roptinfo->authproto,
		    roptinfo->authalgorithm,
		    roptinfo->delayedauth_offset + sizeof(*dh6),
		    client_conf->delayedkey)) {
			dprintf(LOG_WARNING, FNAME, "failed to calculate MAC");
			return (-1);
		}
		break;
	default:
		break;		/* do nothing */
	}

	/* construct a relay chain, if necessary */
	for (relayinfo = TAILQ_FIRST(relayinfohead); relayinfo;
	    relayinfo = TAILQ_NEXT(relayinfo, link)) {
		struct dhcp6_optinfo relayopt;
		struct dhcp6_vbuf relaymsgbuf;
		struct dhcp6_relay *dh6relay;

		relayed = 1;
		dhcp6_init_options(&relayopt);

		relaymsgbuf.dv_len = len;
		relaymsgbuf.dv_buf = replybuf;
		if (dhcp6_vbuf_copy(&relayopt.relay_msg, &relaymsgbuf))
			return (-1);
		if (relayinfo->relay_ifid.dv_buf &&
		    dhcp6_vbuf_copy(&relayopt.ifidopt,
		    &relayinfo->relay_ifid)) {
			dhcp6_vbuf_free(&relayopt.relay_msg);
			return (-1);
		}

		/* we can safely reuse replybuf here */
		dh6relay = (struct dhcp6_relay *)replybuf;
		memset(dh6relay, 0, sizeof (*dh6relay));
		dh6relay->dh6relay_msgtype = DH6_RELAY_REPLY;
		dh6relay->dh6relay_hcnt = relayinfo->hcnt;
		bcopy(&relayinfo->linkaddr, &dh6relay->dh6relay_linkaddr,
		    sizeof (dh6relay->dh6relay_linkaddr));
		bcopy( &relayinfo->peeraddr,&dh6relay->dh6relay_peeraddr,
		    sizeof (dh6relay->dh6relay_peeraddr));

		len = sizeof(*dh6relay);
		if ((optlen = dhcp6_set_options(DH6_RELAY_REPLY,
		    (struct dhcp6opt *)(dh6relay + 1),
		    (struct dhcp6opt *)(replybuf + sizeof(replybuf)),
		    &relayopt)) < 0) {
			dprintf(LOG_INFO, FNAME,
			    "failed to construct relay message");
			dhcp6_clear_options(&relayopt);
			return (-1);
		}
		len += optlen;

		dhcp6_clear_options(&relayopt);
	}

	/* specify the destination and send the reply */
	dst = relayed ? *sa6_any_relay : *sa6_any_downstream;
	dst.sin6_addr = ((struct sockaddr_in6 *)from)->sin6_addr;
	dst.sin6_scope_id = ((struct sockaddr_in6 *)from)->sin6_scope_id;
	
	if (transmit_sa(outsock, (struct sockaddr *)&dst, replybuf, len) != 0) 
		{
				dprintf(LOG_ERR, FNAME,
		    "transmit failed: %s", strerror(errno));
		dprintf(LOG_ERR, FNAME, "transmit %s to %s failed",
		    dhcp6msgstr(type),in6addr2str(&dst.sin6_addr, 0));
		return (-1);
	}

	dprintf(LOG_DEBUG, FNAME, "transmit %s to %s",
	    dhcp6msgstr(type), addr2str((struct sockaddr *)&dst));

	return (0);
}

static int
make_ia_stcode(iatype, iaid, stcode, retlist)
	int iatype;
	u_int16_t stcode;
	u_int32_t iaid;
	struct dhcp6_list *retlist;
{
	struct dhcp6_list stcode_list;
	struct dhcp6_ia ia_empty;

	memset(&ia_empty, 0, sizeof(ia_empty));
	ia_empty.iaid = iaid;

	TAILQ_INIT(&stcode_list);
	if (dhcp6_add_listval(&stcode_list, DHCP6_LISTVAL_STCODE,
	    &stcode, NULL) == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to make an option list");
		return (-1);
	}

	if (dhcp6_add_listval(retlist, iatype,
	    &ia_empty, &stcode_list) == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to make an option list");
		dhcp6_clear_list(&stcode_list);
		return (-1);
	}
	dhcp6_clear_list(&stcode_list);

	return (0);
}

static int
make_ia(spec, conflist, retlist, client_conf, do_binding)
	struct dhcp6_listval *spec;
	struct dhcp6_list *conflist, *retlist;
	struct host_conf *client_conf;
	int do_binding;
{
	struct dhcp6_binding *binding;
	struct dhcp6_list ialist;
	struct dhcp6_listval *specia;
	struct dhcp6_ia ia;
	int found = 0;
	int ret = 0;

	/*
	 * If we happen to have a binding already, update the binding and
	 * return it.  Perhaps the request is being retransmitted.
	 */
	if ((binding = find_binding(&client_conf->duid, DHCP6_BINDING_IA,
	    spec->type, spec->val_ia.iaid)) != NULL) {
		struct dhcp6_list *blist = &binding->val_list;
		struct dhcp6_listval *bia, *v;

		dprintf(LOG_DEBUG, FNAME, "we have a binding already: %s",
		    bindingstr(binding));
	/*
	*What if the binding address or prefix is not the same as request?
	*/

		{	/* we found a binding */
				struct dhcp6_listval *lv;
				struct dhcp6_prefix prefix;
				struct dhcp6_statefuladdr saddr;

				update_binding(binding);		
				/* see if each information to be renewed is still valid. */
				for (lv = TAILQ_FIRST(&spec->sublist); lv;
					lv = TAILQ_NEXT(lv, link)) {
					struct dhcp6_listval *blv;
		
					switch (spec->type) {
					case DHCP6_LISTVAL_IAPD:
						if (lv->type != DHCP6_LISTVAL_PREFIX6)
							continue;
		
						prefix = lv->val_prefix6;
						blv = dhcp6_find_listval(&binding->val_list,
							DHCP6_LISTVAL_PREFIX6, &prefix, 0);
						if (blv == NULL) {
							return -2;
						} 
						
						break;
					case DHCP6_LISTVAL_IANA:
						if (lv->type != DHCP6_LISTVAL_STATEFULADDR6)
							continue;
		
						saddr = lv->val_statefuladdr6;
						blv = dhcp6_find_listval(&binding->val_list,
							DHCP6_LISTVAL_STATEFULADDR6, &saddr, 0);
						if (blv == NULL) 
							return -2;
		
						break;
					default:
						dprintf(LOG_ERR, FNAME, "unsupported IA type");
						return (-1); /* XXX */
					}
				}
			}


		update_binding(binding);

		memset(&ia, 0, sizeof(ia));
		ia.iaid = spec->val_ia.iaid;
		/* determine appropriate T1 and T2 */
		calc_ia_timo(&ia, blist, client_conf);
		if (retlist&&dhcp6_add_listval(retlist, spec->type, &ia, blist)
		    == NULL) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to copy binding info");
			return (0);
		}

		/* remove bound values from the configuration */
		for (bia = TAILQ_FIRST(blist); bia;
		    bia = TAILQ_NEXT(bia, link)) {
			if ((v = dhcp6_find_listval(conflist,
			    bia->type, &bia->uv, 0)) != NULL) {
				TAILQ_REMOVE(conflist, v, link);
				dhcp6_clear_listval(v);
			}
		}

		return (1);
	}

	/*
	 * trivial case:
	 * if the configuration is empty, we cannot make any IA.
	 */
	/*
	if (TAILQ_EMPTY(conflist)) {
		if (spec->type != DHCP6_LISTVAL_IANA ||
			client_conf->pool.name == NULL) {
			return (0);
		}
	}
*/
	TAILQ_INIT(&ialist);

	/* First, check if we can meet the client's requirement */
	for (specia = TAILQ_FIRST(&spec->sublist); specia;
	    specia = TAILQ_NEXT(specia, link)) {
		/* try to find an IA that matches the spec best. */
		if (!TAILQ_EMPTY(conflist)) {
			if (make_match_ia(specia, conflist, &ialist))
				found++;
		} else /*  if (spec->type == DHCP6_LISTVAL_IANA &&//for  delegating router support
			client_conf->pool.name != NULL) */{
			if (make_iana_from_pool(&client_conf->pool, specia, &ialist))//specify a address,
				found++;
			else 
				{
					if (!IN6_IS_ADDR_UNSPECIFIED(&specia->val_statefuladdr6.addr))//TO BE CHECK
					return -2;
				}
		}
	}
	if (found == 0) {
		if (!TAILQ_EMPTY(conflist)) {
			struct dhcp6_listval *v;

			/* use the first IA in the configuration list */
			for (v = TAILQ_FIRST(conflist); v; v = TAILQ_NEXT(v, link)) {
				if (spec->type != DHCP6_LISTVAL_IANA)
					break;	/* always use the first IA for non-IANA */
				if (!is_leased(&v->val_statefuladdr6.addr))
					break;
			}
			if (v && dhcp6_add_listval(&ialist, v->type, &v->uv, NULL)) {
				found = 1;
				TAILQ_REMOVE(conflist, v, link);
				dhcp6_clear_listval(v);
			}
		} else if (/*spec->type == DHCP6_LISTVAL_IANA &&*///for prefix support
			client_conf->pool.name != NULL) {
			if (make_iana_from_pool(&client_conf->pool, NULL, &ialist))//if we fail when get a specify address,get again
				found = 1;
		}
	}
	if (found) {
		memset(&ia, 0, sizeof(ia));
		ia.iaid = spec->val_ia.iaid;
		/* determine appropriate T1 and T2 */
		calc_ia_timo(&ia, &ialist, client_conf);

		/* make a binding for the set if necessary */
		if (do_binding) {
			if (add_binding(&client_conf->duid, DHCP6_BINDING_IA,
			    spec->type, spec->val_ia.iaid, &ialist) == NULL) {
				dprintf(LOG_NOTICE, FNAME,
				    "failed to make a binding");
				found = 0;
			}
		}
		if (found) {
			/* make an IA for the set */
			if (retlist&&dhcp6_add_listval(retlist, spec->type,
			    &ia, &ialist) == NULL)
				found = 0;
		}
		dhcp6_clear_list(&ialist);
	}

		return (found);
}

static int
make_match_ia(spec, conflist, retlist)
	struct dhcp6_listval *spec;
	struct dhcp6_list *conflist, *retlist;
{
	struct dhcp6_listval *match;
	int matched = 0;

	/* do we have the exact value specified? */
	match = dhcp6_find_listval(conflist, spec->type, &spec->uv, 0);

	/* if not, make further search specific to the IA type. */
	if (!match) {
		switch (spec->type) {
		case DHCP6_LISTVAL_PREFIX6:
			match = dhcp6_find_listval(conflist, spec->type,
			    &spec->uv, MATCHLIST_PREFIXLEN);
			break;
		case DHCP6_LISTVAL_STATEFULADDR6:
			/* No "partial match" for addresses */
			if (is_leased(&spec->val_statefuladdr6.addr))
				match = 0;
			break;
		default:
			dprintf(LOG_ERR, FNAME, "unsupported IA type");
			return (0); /* XXX */
		}
	}

	/*
	 * if found, remove the matched entry from the configuration list
	 * and copy the value in the returned list.
	 */
	if (match) {
		if (dhcp6_add_listval(retlist, match->type,
		    &match->uv, NULL)) {
			matched = 1;
			TAILQ_REMOVE(conflist, match, link);
			dhcp6_clear_listval(match);
		}
	}

	return (matched);
}

/* making sublist of iana */
static int
make_iana_from_pool(poolspec, spec, retlist)
	struct dhcp6_poolspec *poolspec;
	struct dhcp6_listval *spec;
	struct dhcp6_list *retlist;
{
	struct dhcp6_statefuladdr saddr;
	struct dhcp6_prefix  sprefix;
	struct pool_conf *pool;
	int found = 0;

	dprintf(LOG_DEBUG, FNAME, "called");

	if ((pool = find_pool(poolspec->name)) == NULL) {
		dprintf(LOG_ERR, FNAME, "pool '%s' not found", poolspec->name);
		return (0);
	}

if (strcmp(pool->name,"prefix")){
	if (spec) {
		memcpy(&saddr.addr, &spec->val_statefuladdr6.addr, sizeof(saddr.addr));
		if (is_available_in_pool(pool, &saddr.addr)) {
			found = 1;
		}
	} else {
		if (get_free_address_from_pool(pool, &saddr.addr)) {
			found = 1;
		}
	}
} else {
		if (spec ) {
		memcpy(&sprefix.addr, &spec->val_prefix6.addr, sizeof(sprefix.addr));
		if (is_available_in_pool(pool, &sprefix.addr)) {
			found = 1;
		}
	} else {
		if (get_free_prefix_from_pool(pool, &sprefix.addr)) {
			found = 1;
		}
	}
}

	if (found) {
		if (strcmp(pool->name,"prefix")){
		saddr.pltime = poolspec->pltime;
		saddr.vltime = poolspec->vltime;

		if (!dhcp6_add_listval(retlist, DHCP6_LISTVAL_STATEFULADDR6,
			&saddr, NULL)) {
			return (0);
		}
		}else{
		sprefix.pltime = poolspec->pltime;
		sprefix.vltime = poolspec->vltime;
		sprefix.plen = pool->prefix_len;
		if (!dhcp6_add_listval(retlist, DHCP6_LISTVAL_PREFIX6,
			&sprefix, NULL)) {
			return (0);
		}
		if (IN6_IS_ADDR_UNSPECIFIED(&sprefix.addr)){
			u_int16_t stcode = DH6OPT_STCODE_NOPREFIXAVAIL;

			if (dhcp6_add_listval(&retlist,DHCP6_LISTVAL_STCODE, &stcode, NULL) == NULL)
				dprintf(LOG_ERR, FNAME, "Add stcode error");
			}
		}
	}

	dprintf(LOG_DEBUG, FNAME, "returns (found=%d)", found);

	return (found);
}

static void
calc_ia_timo(ia, ialist, client_conf)
	struct dhcp6_ia *ia;
	struct dhcp6_list *ialist; /* this should not be empty */
	struct host_conf *client_conf; /* unused yet */
{
	struct dhcp6_listval *iav;
	u_int32_t base = DHCP6_DURATION_INFINITE;
	int iatype;

	iatype = TAILQ_FIRST(ialist)->type;
	for (iav = TAILQ_FIRST(ialist); iav; iav = TAILQ_NEXT(iav, link)) {
		if (iav->type != iatype) {
			dprintf(LOG_ERR, FNAME,
			    "assumption failure: IA list is not consistent");
			return  (1); /* XXX */
		}
		switch (iatype) {
		case DHCP6_LISTVAL_PREFIX6:
		case DHCP6_LISTVAL_STATEFULADDR6:
			if (base == DHCP6_DURATION_INFINITE ||
			    iav->val_prefix6.pltime < base)
				base = iav->val_prefix6.pltime;
			break;
		}
	}

	switch (iatype) {
	case DHCP6_LISTVAL_PREFIX6:
	case DHCP6_LISTVAL_STATEFULADDR6:
		/*
		 * Configure the timeout parameters as recommended in
		 * Section 22.4 of RFC3315 and Section 9 of RFC3633.
		 * We could also set the parameters to 0 if we let the client
		 * decide the renew timing (not implemented yet).
		 */
		if (base == DHCP6_DURATION_INFINITE) {
			ia->t1 = DHCP6_DURATION_INFINITE;
			ia->t2 = DHCP6_DURATION_INFINITE;
		} else {
			ia->t1 = base / 2;
			ia->t2 = (base * 4) / 5;
		}
		break;
	}
}

static void
update_binding_duration(binding)
	struct dhcp6_binding *binding;
{
	struct dhcp6_list *ia_list = &binding->val_list;
	struct dhcp6_listval *iav;
	int duration = DHCP6_DURATION_INFINITE;
	u_int32_t past, min_lifetime;
	time_t now = time(NULL);

	min_lifetime = 0;
	past = (u_int32_t)(now >= binding->updatetime ?
	    now - binding->updatetime : 0);

	switch (binding->type) {
	case DHCP6_BINDING_IA:
		/*
		 * Binding configuration is a list of IA parameters.
		 * Determine the minimum valid lifetime.
		 */
		for (iav = TAILQ_FIRST(ia_list); iav;
		    iav = TAILQ_NEXT(iav, link)) {
			u_int32_t lifetime;

			switch (binding->iatype) {
			case DHCP6_LISTVAL_IAPD:
				lifetime = iav->val_prefix6.vltime;
				break;
			case DHCP6_LISTVAL_IANA:
				lifetime = iav->val_statefuladdr6.vltime;
				break;
			default:
				dprintf(LOG_ERR, FNAME, "unsupported IA type");
				return;	/* XXX */
			}

			if (min_lifetime == 0 ||
			    (lifetime != DHCP6_DURATION_INFINITE &&
			    lifetime < min_lifetime))
				min_lifetime = lifetime;
		}

		if (past < min_lifetime)
			duration = min_lifetime - past;
		else
			duration = 0;

		break;
	default:
		/* should be internal error. */
		dprintf(LOG_ERR, FNAME, "unknown binding type (%d)",
		    binding->type);
		return;
	}

	binding->duration = duration;
}

static struct dhcp6_binding *
add_binding(clientid, btype, iatype, iaid, val0)
	struct duid *clientid;
	dhcp6_bindingtype_t btype;
	int iatype;
	u_int32_t iaid;
	void *val0;
{
	struct dhcp6_binding *binding = NULL;
	u_int32_t duration = DHCP6_DURATION_INFINITE;

	if ((binding = malloc(sizeof(*binding))) == NULL) {
		dprintf(LOG_NOTICE, FNAME, "failed to allocate memory");
		return (NULL);
	}
	memset(binding, 0, sizeof(*binding));
	binding->type = btype;
	if (duidcpy(&binding->clientid, clientid)) {
		dprintf(LOG_NOTICE, FNAME, "failed to copy DUID");
		goto fail;
	}
	binding->iatype = iatype;
	binding->iaid = iaid;

	/* construct configuration information for this binding */
	switch (btype) {
	case DHCP6_BINDING_IA:
		TAILQ_INIT(&binding->val_list);
		if (dhcp6_copy_list(&binding->val_list,
		    (struct dhcp6_list *)val0)) {
			dprintf(LOG_NOTICE, FNAME,
			    "failed to copy binding data");
			goto fail;
		}
		/* lease address */
		if (iatype == DHCP6_LISTVAL_IANA) {
			struct dhcp6_list *ia_list = &binding->val_list;
			struct dhcp6_listval *lv, *lv_next;

			for (lv = TAILQ_FIRST(ia_list); lv; lv = lv_next) {
				lv_next = TAILQ_NEXT(lv, link);

				if (lv->type != DHCP6_LISTVAL_STATEFULADDR6) {
					dprintf(LOG_ERR, FNAME,
						"unexpected binding value type(%d)", lv->type);
					continue;
				}

				if (!lease_address(&lv->val_statefuladdr6.addr)) {
					dprintf(LOG_NOTICE, FNAME,
						"cannot lease address %s",
						in6addr2str(&lv->val_statefuladdr6.addr, 0));
					TAILQ_REMOVE(ia_list, lv, link);
					dhcp6_clear_listval(lv);
				}
			}
			if (TAILQ_EMPTY(ia_list)) {
				dprintf(LOG_NOTICE, FNAME, "cannot lease any address");
				goto fail;
			}
		}
		break;
	default:
		dprintf(LOG_ERR, FNAME, "unexpected binding type(%d)", btype);
		goto fail;
	}

	/* calculate duration and start timer accordingly */
	binding->updatetime = time(NULL);
	update_binding_duration(binding);
	if (binding->duration != DHCP6_DURATION_INFINITE) {
		struct timeval timo;

		binding->timer = dhcp6_add_timer(binding_timo, binding,DHCP6_MODE_SERVER);
		if (binding->timer == NULL) {
			dprintf(LOG_NOTICE, FNAME, "failed to add timer");
			goto fail;
		}
		timo.tv_sec = (long)duration;
		timo.tv_usec = 0;
		dhcp6_set_timer(&timo, binding->timer);
	}

	TAILQ_INSERT_TAIL(&dhcp6_binding_head, binding, link);

	dprintf(LOG_DEBUG, FNAME, "add a new binding %s", bindingstr(binding));

	return (binding);

  fail:
	if (binding)
		free_binding(binding);
	return (NULL);
}

static struct dhcp6_binding *
find_binding(clientid, btype, iatype, iaid)
	struct duid *clientid;
	dhcp6_bindingtype_t btype;
	int iatype;
	u_int32_t iaid;
{
	struct dhcp6_binding *bp;

	for (bp = TAILQ_FIRST(&dhcp6_binding_head); bp;
	    bp = TAILQ_NEXT(bp, link)) {
		if (bp->type != btype || duidcmp(&bp->clientid, clientid))
			continue;

		if (btype == DHCP6_BINDING_IA &&
		    (bp->iatype != iatype || bp->iaid != iaid))
			continue;

		return (bp);
	}

	return (NULL);
}

static void
update_binding(binding)
	struct dhcp6_binding *binding;
{
	struct timeval timo;

	dprintf(LOG_DEBUG, FNAME, "update binding %s for %s",
	    bindingstr(binding), duidstr(&binding->clientid));

	/* update timestamp and calculate new duration */
	binding->updatetime = time(NULL);
	update_binding_duration(binding);

	/* if the lease duration is infinite, there's nothing to do. */
	if (binding->duration == DHCP6_DURATION_INFINITE)
		return;

	/* reset the timer with the duration */
	timo.tv_sec = (long)binding->duration;
	timo.tv_usec = 0;
	dhcp6_set_timer(&timo, binding->timer);
}

static void
remove_binding(binding)
	struct dhcp6_binding *binding;
{
	dprintf(LOG_DEBUG, FNAME, "remove a binding %s",
	    bindingstr(binding));

	if (binding->timer)
		dhcp6_remove_timer(&binding->timer);

	TAILQ_REMOVE(&dhcp6_binding_head, binding, link);

	free_binding(binding);
}

static void
free_binding(binding)
	struct dhcp6_binding *binding;
{
	duidfree(&binding->clientid);

	/* free configuration info in a type dependent manner. */
	switch (binding->type) {
	case DHCP6_BINDING_IA:
		/* releaes address */
		if (binding->iatype == DHCP6_LISTVAL_IANA) {
			struct dhcp6_list *ia_list = &binding->val_list;
			struct dhcp6_listval *lv;

			for (lv = TAILQ_FIRST(ia_list); lv; lv = TAILQ_NEXT(lv, link)) {
				if (lv->type != DHCP6_LISTVAL_STATEFULADDR6) {
					dprintf(LOG_ERR, FNAME,
						"unexpected binding value type(%d)", lv->type);
					continue;
				}
				release_address(&lv->val_statefuladdr6.addr);
			}
		}
		dhcp6_clear_list(&binding->val_list);
		break;
	default:
		dprintf(LOG_ERR, FNAME, "unknown binding type %d",
		    binding->type);
		break;
	}

	free(binding);
}



void free_binding_list(struct dhcp6_binding_list *binding)
{
	struct dhcp6_binding *bp,*tmp=NULL;

	for (bp = TAILQ_FIRST(binding); bp;) {
	   	tmp =   TAILQ_NEXT(bp, link);
		remove_binding(bp);
		bp = tmp;
	}

	return ;
}


static struct dhcp6_timer *
binding_timo(arg)
	void *arg;
{
	struct dhcp6_binding *binding = (struct dhcp6_binding *)arg;
	struct dhcp6_list *ia_list = &binding->val_list;
	struct dhcp6_listval *iav, *iav_next;
	time_t now = time(NULL);
	u_int32_t past, lifetime;
	struct timeval timo;

	past = (u_int32_t)(now >= binding->updatetime ?
	    now - binding->updatetime : 0);

	switch (binding->type) {
	case DHCP6_BINDING_IA:
		for (iav = TAILQ_FIRST(ia_list); iav; iav = iav_next) {
			iav_next = TAILQ_NEXT(iav, link);

			switch (binding->iatype) {
			case DHCP6_LISTVAL_IAPD:
			case DHCP6_LISTVAL_IANA:
				lifetime = iav->val_prefix6.vltime;
				break;
			default:
				dprintf(LOG_ERR, FNAME, "internal error: "
				    "unknown binding type (%d)",
				    binding->iatype);
				return (NULL); /* XXX */
			}

			if (lifetime != DHCP6_DURATION_INFINITE &&
			    lifetime <= past) {
				dprintf(LOG_DEBUG, FNAME, "bound prefix %s/%d"
				    " in %s has expired",
				    in6addr2str(&iav->val_prefix6.addr, 0),
				    iav->val_prefix6.plen,
				    bindingstr(binding));
				if (binding->iatype == DHCP6_LISTVAL_IANA) 
					release_address(&iav->val_prefix6.addr);
				TAILQ_REMOVE(ia_list, iav, link);
				dhcp6_clear_listval(iav);
			}
		}

		/* If all IA parameters have expired, remove the binding. */
		if (TAILQ_EMPTY(ia_list)) {
			remove_binding(binding);
			return (NULL);
		}

		break;
	default:
		dprintf(LOG_ERR, FNAME, "unknown binding type %d",
		    binding->type);
		return (NULL);	/* XXX */
	}

	update_binding_duration(binding);

	/* if the lease duration is infinite, there's nothing to do. */
	if (binding->duration == DHCP6_DURATION_INFINITE)
		return (NULL);

	/* reset the timer with the duration */
	timo.tv_sec = (long)binding->duration;
	timo.tv_usec = 0;
	dhcp6_set_timer(&timo, binding->timer);

	return (binding->timer);
}

static struct dhcp6_listval *
find_binding_ia(key, binding)
	struct dhcp6_listval *key;
	struct dhcp6_binding *binding;
{
	struct dhcp6_list *ia_list = &binding->val_list;

	switch (binding->type) {
	case DHCP6_BINDING_IA:
		return (dhcp6_find_listval(ia_list, key->type, &key->uv, 0));
	default:
		dprintf(LOG_ERR, FNAME, "unknown binding type %d",
		    binding->type);
		return (NULL);	/* XXX */
	}
}

static char *
bindingstr(binding)
	struct dhcp6_binding *binding;
{
	static char strbuf[LINE_MAX];	/* XXX: thread unsafe */
	char *iatype = NULL;

	switch (binding->type) {
	case DHCP6_BINDING_IA:
		switch (binding->iatype) {
		case DHCP6_LISTVAL_IAPD:
			iatype = "PD";
			break;
		case DHCP6_LISTVAL_IANA:
			iatype = "NA";
			break;
		}

		snprintf(strbuf, sizeof(strbuf),
		    "[IA: duid=%s, type=%s, iaid=%lu, duration=%lu]",
		    duidstr(&binding->clientid), iatype, (u_long)binding->iaid,
		    (u_long)binding->duration);
		break;
	default:
		dprintf(LOG_ERR, FNAME, "unexpected binding type(%d)",
		    binding->type);
		return ("???");
	}

	return (strbuf);
}

static int
process_auth(dh6, len, client_conf, optinfo, roptinfo)
	struct dhcp6 *dh6;
	ssize_t len;
	struct host_conf *client_conf;
	struct dhcp6_optinfo *optinfo, *roptinfo;
{
	u_int8_t msgtype = dh6->dh6_msgtype;
	int authenticated = 0;
	struct keyinfo *key;

	/*
	 * if the client wanted DHCPv6 authentication, check if a secret
	 * key is available for the client.
	 */
	switch (optinfo->authproto) {
	case DHCP6_AUTHPROTO_UNDEF:
		/*
		 * The client did not include authentication option.  What if
		 * we had sent authentication information?  The specification
		 * is not clear, but we should probably accept it, since the
		 * client MAY ignore the information in advertise messages.
		 */
		return (0);
	case DHCP6_AUTHPROTO_DELAYED:
		if (optinfo->authalgorithm != DHCP6_AUTHALG_HMACMD5) {
			dprintf(LOG_INFO, FNAME, "unknown authentication "
			    "algorithm (%d) required by %s",
			    optinfo->authalgorithm,
			    clientstr(client_conf, &optinfo->clientID));
			break;	/* give up with this authentication */
		}

		if (optinfo->authrdm != DHCP6_AUTHRDM_MONOCOUNTER) {
			dprintf(LOG_INFO, FNAME,
			    "unknown RDM (%d) required by %s",
			    optinfo->authrdm,
			    clientstr(client_conf, &optinfo->clientID));
			break;	/* give up with this authentication */
		}

		/* see if we have a key for the client */
		if (client_conf == NULL || client_conf->delayedkey == NULL) {
			dprintf(LOG_INFO, FNAME, "client %s wanted "
			    "authentication, but no key found",
			    clientstr(client_conf, &optinfo->clientID));
			break;
		}
		key = client_conf->delayedkey;
		dprintf(LOG_DEBUG, FNAME, "found key %s for client %s",
		    key->name, clientstr(client_conf, &optinfo->clientID));

		if (msgtype == DH6_SOLICIT) {
			if (!(optinfo->authflags & DHCP6OPT_AUTHFLAG_NOINFO)) {
				/*
				 * A solicit message should not contain
				 * authentication information.
				 */
				dprintf(LOG_INFO, FNAME,
				    "authentication information "
				    "provided in solicit from %s",
				    clientstr(client_conf,
				    &optinfo->clientID)); 
				/* accept it anyway. (or discard?) */
			}
		} else {
			/* replay protection */
			if (!client_conf->saw_previous_rd) {
				dprintf(LOG_WARNING, FNAME,
				    "previous RD value for %s is unknown "
				    "(accept it)", clientstr(client_conf,
				    &optinfo->clientID));
			} else {
				if (dhcp6_auth_replaycheck(optinfo->authrdm,
				    client_conf->previous_rd,
				    optinfo->authrd)) {
					dprintf(LOG_INFO, FNAME,
					    "possible replay attack detected "
					    "for client %s",
					    clientstr(client_conf,
					    &optinfo->clientID));
					break;
				}
			}

			if ((optinfo->authflags & DHCP6OPT_AUTHFLAG_NOINFO)) {
				dprintf(LOG_INFO, FNAME,
				    "client %s did not provide authentication "
				    "information in %s",
				    clientstr(client_conf, &optinfo->clientID),
				    dhcp6msgstr(msgtype));
				break;
			}

			/*
			 * The client MUST use the same key used by the server
			 * to generate the authentication information.
			 * [RFC3315 Section 21.4.4.3]
			 * The RFC does not say what the server should do if
			 * the client breaks this rule, but it should be
			 * natural to interpret this as authentication failure.
			 */
			if (optinfo->delayedauth_keyid != key->keyid ||
			    optinfo->delayedauth_realmlen != key->realmlen ||
			    memcmp(optinfo->delayedauth_realmval, key->realm,
			    key->realmlen) != 0) {
				dprintf(LOG_INFO, FNAME, "authentication key "
				    "mismatch with client %s",
				    clientstr(client_conf,
				    &optinfo->clientID));
				break;
			}

			/* check for the key lifetime */
			if (dhcp6_validate_key(key)) {
				dprintf(LOG_INFO, FNAME, "key %s has expired",
				    key->name);
				break;
			}

			/* validate MAC */
			if (dhcp6_verify_mac((char *)dh6, len,
			    optinfo->authproto, optinfo->authalgorithm,
			    optinfo->delayedauth_offset + sizeof(*dh6), key)
			    == 0) {
				dprintf(LOG_DEBUG, FNAME,
				    "message authentication validated for "
				    "client %s", clientstr(client_conf,
				    &optinfo->clientID));
			} else {
				dprintf(LOG_INFO, FNAME, "invalid message "
				    "authentication");
				break;
			}
		}

		roptinfo->authproto = optinfo->authproto;
		roptinfo->authalgorithm = optinfo->authalgorithm;
		roptinfo->authrdm = optinfo->authrdm;

		if (get_rdvalue(roptinfo->authrdm, &roptinfo->authrd,
		    sizeof(roptinfo->authrd))) {
			dprintf(LOG_ERR, FNAME, "failed to get a replay "
			    "detection value for %s",
			    clientstr(client_conf, &optinfo->clientID));
			break;	/* XXX: try to recover? */
		}

		roptinfo->delayedauth_keyid = key->keyid;
		roptinfo->delayedauth_realmlen = key->realmlen;
		roptinfo->delayedauth_realmval =
		    malloc(roptinfo->delayedauth_realmlen);
		if (roptinfo->delayedauth_realmval == NULL) {
			dprintf(LOG_ERR, FNAME, "failed to allocate memory "
			    "for authentication realm for %s",
			    clientstr(client_conf, &optinfo->clientID));
			break;
		}
		memcpy(roptinfo->delayedauth_realmval, key->realm,
		    roptinfo->delayedauth_realmlen);

		authenticated = 1;

		break;
	default:
		dprintf(LOG_INFO, FNAME, "client %s wanted authentication "
		    "with unsupported protocol (%d)",
		    clientstr(client_conf, &optinfo->clientID),
		    optinfo->authproto);
		return (-1);	/* or simply ignore it? */
	}

	if (authenticated == 0) {
		if (msgtype != DH6_SOLICIT) {
			/*
			 * If the message fails to pass the validation test,
			 * the server MUST discard the message.
			 * [RFC3315 Section 21.4.5.2]
			 */
			return (-1);
		}
	} else {
		/* Message authenticated.  Update RD counter. */
		if (msgtype != DH6_SOLICIT && client_conf != NULL) {
			client_conf->previous_rd = optinfo->authrd;
			client_conf->saw_previous_rd = 1;
		}
	}

	return (0);
}

static inline char *
clientstr(conf, duid)
	struct host_conf *conf;
	struct duid *duid;
{
	if (conf != NULL)
		return (conf->name);

	return (duidstr(duid));
}
