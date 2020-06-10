/*
 *  Copyright (c) 1998 by the University of Southern California.
 *  All rights reserved.
 *
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation in source and binary forms for lawful
 *  purposes and without fee is hereby granted, provided
 *  that the above copyright notice appear in all copies and that both
 *  the copyright notice and this permission notice appear in supporting
 *  documentation, and that any documentation, advertising materials,
 *  and other materials related to such distribution and use acknowledge
 *  that the software was developed by the University of Southern
 *  California and/or Information Sciences Institute.
 *  The name of the University of Southern California may not
 *  be used to endorse or promote products derived from this software
 *  without specific prior written permission.
 *
 *  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY REPRESENTATIONS
 *  ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
 *  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
 *  NON-INFRINGEMENT.
 *
 *  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
 *  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
 *  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
 *  THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  Other copyrights might apply to parts of this software and are so
 *  noted when applicable.
 */
/*
 *  Questions concerning this software should be directed to 
 *  Pavlin Ivanov Radoslavov (pavlin@catarina.usc.edu)
 *
 *  $Id: config.c,v 1.6 2000/02/23 16:10:26 itojun Exp $
 */
/*
 * Part of this program has been derived from mrouted.
 * The mrouted program is covered by the license in the accompanying file
 * named "LICENSE.mrouted".
 *
 * The mrouted program is COPYRIGHT 1989 by The Board of Trustees of
 * Leland Stanford Junior University.
 *
 * $FreeBSD$
 */

#include "defs.h"
#ifdef HAVE_GETIFADDRS
#include <ifaddrs.h>
#endif 


/*
 * Forward declarations.
 */
static char *next_word  __P((char **));
static int parse_phyint __P((char *s));
static void add_phaddr  __P((struct uvif *v, struct sockaddr_in6 *addr,
			     struct in6_addr *mask));
static mifi_t ifname2mifi __P((char *ifname));
static int wordToOption __P((char *));
static int parse_filter __P((char *s));
#if 0 /* not used */
static int parse_default_source_metric __P((char *));
static int parse_default_source_preference __P((char *));
#endif

/*
 * Query the kernel to find network interfaces that are multicast-capable
 * and install them in the uvifs array.
 */
void 
config_vifs_from_kernel()
{
    register struct uvif *v=NULL;
    register vifi_t vifi;
    struct sockaddr_in6 addr;
    struct in6_addr mask, prefix;
    short flags;
#ifdef HAVE_GETIFADDRS
    struct ifaddrs *ifap=NULL, *ifa=NULL;//init by ziqiang 
#else
    int n;
    int num_ifreq = 64;
    struct ifconf ifc;
    struct ifreq *ifrp=NULL, *ifend=NULL;//init by ziqiang 
#endif 

    total_interfaces = 0; /* The total number of physical interfaces */
    
#ifdef HAVE_GETIFADDRS
    if (getifaddrs(&ifap))
    	{
	syslog(LOG_ERR, "getiaddrs() error  ifap = %p\n",ifap);
	return ;//by ziqiang
    	}

    /*
     * Loop through all of the interfaces.
     */
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
	/*
	 * Ignore any interface for an address family other than IPv6.
	 */
	if (ifa->ifa_addr->sa_family != AF_INET6) {
	    total_interfaces++;  /* Eventually may have IPv6 address later */
	    continue;
	}

	memcpy(&addr, ifa->ifa_addr, sizeof(struct sockaddr_in6));
	
	flags = ifa->ifa_flags;
	if ((flags & (IFF_LOOPBACK | IFF_MULTICAST)) != IFF_MULTICAST)
	    continue;

	/*
	 * Get netmask of the address.
	 */
	//memcpy(&mask, &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr,
	   //    sizeof(mask));
	inet_pton(AF_INET6, "ffff:ffff:ffff:ffff::", &mask);

	if (IN6_IS_ADDR_LINKLOCAL(&addr.sin6_addr)) {
		addr.sin6_scope_id = if_nametoindex(ifa->ifa_name);
#ifdef __KAME__
		/*
		 * Hack for KAME kernel. Set sin6_scope_id field of a
		 * link local address and clear the index embedded in
		 * the address.
		 */
		/* clear interface index */
		addr.sin6_addr.s6_addr[2] = 0;
		addr.sin6_addr.s6_addr[3] = 0;
#endif
	}

	/*
	 * If the address is connected to the same subnet as one already
	 * installed in the uvifs array, just add the address to the list
	 * of addresses of the uvif.
	 */
	for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v) {
	    if (strcmp(v->uv_name, ifa->ifa_name) == 0) {
		    add_phaddr(v, &addr, &mask);
		    break;
	    }
	}
	if (vifi != numvifs)
	    continue;

	/*
	 * If there is room in the uvifs array, install this interface.
	 */
	if (numvifs == MAXMIFS) {
	    syslog(LOG_WARNING, "too many ifs, ignoring %s", ifa->ifa_name);
	    continue;
	}

	/*
	 * Everyone below is a potential vif interface.
	 * We don't care if it has wrong configuration or not configured
	 * at all.
	 */
	total_interfaces++;
	
	v = &uvifs[numvifs];
	v->uv_flags		= 0;
	v->uv_metric		= DEFAULT_METRIC;
	v->uv_admetric		= 0;
	v->uv_rate_limit	= DEFAULT_PHY_RATE_LIMIT;
	v->uv_dst_addr		= allpim6routers_group;
	v->uv_prefix.sin6_addr	= prefix;
	v->uv_subnetmask	= mask;
	strncpy(v->uv_name, ifa->ifa_name, IFNAMSIZ);
	v->uv_ifindex	        = if_nametoindex(v->uv_name);
	v->uv_groups		= (struct listaddr *)NULL;
	v->uv_dvmrp_neighbors   = (struct listaddr *)NULL;
	NBRM_CLRALL(v->uv_nbrmap);
	v->uv_querier           = (struct listaddr *)NULL;
	v->uv_prune_lifetime    = 0;
	v->uv_acl               = (struct vif_acl *)NULL;
	v->uv_leaf_timer        = 0;
	v->uv_addrs		= (struct phaddr *)NULL;
	v->uv_filter		= (struct vif_filter *)NULL;
	v->uv_pim_hello_timer   = 0;
	v->uv_gq_timer          = 0;
	v->uv_pim_neighbors	= (struct pim_nbr_entry *)NULL;
	v->uv_local_pref        = default_source_preference;
	v->uv_local_metric      = default_source_metric;
	add_phaddr(v, &addr, &mask);
	
	if (flags & IFF_POINTOPOINT)
	    v->uv_flags |= (VIFF_REXMIT_PRUNES | VIFF_POINT_TO_POINT);
	syslog(LOG_INFO,
	    "installing %s as if #%u - rate=%d",
	    v->uv_name, numvifs, v->uv_rate_limit);
	++numvifs;

	/*
	 * If the interface is not yet up, set the vifs_down flag to
	 * remind us to check again later.
	 */
	if (!(flags & IFF_UP)) {
	    v->uv_flags |= VIFF_DOWN;
	    vifs_down = TRUE;
	}
    }

    freeifaddrs(ifap);
#else /* !HAVE_GETIFADDRS */

    ifc.ifc_len = num_ifreq * sizeof(struct ifreq);
    ifc.ifc_buf = calloc(ifc.ifc_len, sizeof(char));
    while (ifc.ifc_buf) {
	caddr_t newbuf;

	if (ioctl(udp_socket, SIOCGIFCONF, (char *)&ifc) < 0)
	    syslog(LOG_ERR, "ioctl SIOCGIFCONF");
	
	/*
	 * If the buffer was large enough to hold all the addresses
	 * then break out, otherwise increase the buffer size and
	 * try again.
	 *
	 * The only way to know that we definitely had enough space
	 * is to know that there was enough space for at least one
	 * more struct ifreq. ???
	 */
	if ((num_ifreq * sizeof(struct ifreq)) >=
	    ifc.ifc_len + sizeof(struct ifreq))
	    break;
	
	num_ifreq *= 2;
	ifc.ifc_len = num_ifreq * sizeof(struct ifreq);
	newbuf = realloc(ifc.ifc_buf, ifc.ifc_len);
	if (newbuf == NULL)
	    free(ifc.ifc_buf);
	ifc.ifc_buf = newbuf;
    }
    if (ifc.ifc_buf == NULL)
	syslog(LOG_ERR, "config_vifs_from_kernel: ran out of memory");
    
    ifrp = (struct ifreq *)ifc.ifc_buf;
    ifend = (struct ifreq *)(ifc.ifc_buf + ifc.ifc_len);
    /*
     * Loop through all of the interfaces.
     */
    for (; ifrp < ifend; ifrp = (struct ifreq *)((char *)ifrp + n)) {
	struct ifreq ifr;
	struct in6_ifreq ifr6;
#ifdef HAVE_SA_LEN
	n = ifrp->ifr_addr.sa_len + sizeof(ifrp->ifr_name);
	if (n < sizeof(*ifrp))
	    n = sizeof(*ifrp);
#else
	n = sizeof(*ifrp);
#endif /* HAVE_SA_LEN */
	
	/*
	 * Ignore any interface for an address family other than IPv6.
	 */
	if (ifrp->ifr_addr.sa_family != AF_INET6) {
	    total_interfaces++;  /* Eventually may have IPv6 address later */
	    continue;
	}

	memcpy(&addr, &ifrp->ifr_addr, sizeof(struct sockaddr_in6));
	
	/*
	 * Need a template to preserve address info that is
	 * used below to locate the next entry.  (Otherwise,
	 * SIOCGIFFLAGS stomps over it because the requests
	 * are returned in a union.)
	 */
	memcpy(ifr.ifr_name, ifrp->ifr_name, sizeof(ifr.ifr_name));
	memcpy(ifr6.ifr_name, ifrp->ifr_name, sizeof(ifr6.ifr_name));

	/*
	 * Ignore loopback interfaces and interfaces that do not
	 * support multicast.
	 */
	if (ioctl(udp_socket, SIOCGIFFLAGS, (char *)&ifr) < 0)
	    syslog(LOG_ERR, "ioctl SIOCGIFFLAGS for %s", ifr.ifr_name);
	flags = ifr.ifr_flags;
	if ((flags & (IFF_LOOPBACK | IFF_MULTICAST)) != IFF_MULTICAST)
	    continue;

	/*
	 * Get netmask of the address.
	 */
	ifr6.ifr_addr = *(struct sockaddr_in6 *)&ifrp->ifr_addr;
	if (ioctl(udp_socket, SIOCGIFNETMASK_IN6, (char *)&ifr6) < 0)
	    syslog(LOG_ERR, "ioctl SIOCGIFNETMASK_IN6 for %s",
		ifr6.ifr_name);
	memcpy(&mask, &ifr6.ifr_addr.sin6_addr, sizeof(mask));

	if (IN6_IS_ADDR_LINKLOCAL(&addr.sin6_addr)) {
		addr.sin6_scope_id = if_nametoindex(ifrp->ifr_name);
#ifdef __KAME__
		/*
		 * Hack for KAME kernel. Set sin6_scope_id field of a
		 * link local address and clear the index embedded in
		 * the address.
		 */
		/* clear interface index */
		addr.sin6_addr.s6_addr[2] = 0;
		addr.sin6_addr.s6_addr[3] = 0;
#endif
	}

	/*
	 * If the address is connected to the same subnet as one already
	 * installed in the uvifs array, just add the address to the list
	 * of addresses of the uvif.
	 */
	for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v) {
	    if (strcmp(v->uv_name, ifr.ifr_name) == 0) {
		    add_phaddr(v, &addr, &mask);
		    break;
	    }
	}
	if (vifi != numvifs)
	    continue;

	/*
	 * If there is room in the uvifs array, install this interface.
	 */
	if (numvifs == MAXMIFS) {
	    syslog(LOG_WARNING, "too many ifs, ignoring %s", ifr.ifr_name);
	    continue;
	}

	/*
	 * Everyone below is a potential vif interface.
	 * We don't care if it has wrong configuration or not configured
	 * at all.
	 */
	total_interfaces++;
	
	v = &uvifs[numvifs];
	v->uv_flags		= 0;
	v->uv_metric		= DEFAULT_METRIC;
	v->uv_admetric		= 0;
	v->uv_rate_limit	= DEFAULT_PHY_RATE_LIMIT;
	v->uv_dst_addr		= allpim6routers_group;
	v->uv_prefix.sin6_addr	= prefix;
	v->uv_subnetmask	= mask;
	strncpy(v->uv_name, ifr.ifr_name, IFNAMSIZ);
	v->uv_ifindex	        = if_nametoindex(v->uv_name);
	v->uv_groups		= (struct listaddr *)NULL;
	v->uv_dvmrp_neighbors   = (struct listaddr *)NULL;
	NBRM_CLRALL(v->uv_nbrmap);
	v->uv_querier           = (struct listaddr *)NULL;
	v->uv_prune_lifetime    = 0;
	v->uv_acl               = (struct vif_acl *)NULL;
	v->uv_leaf_timer        = 0;
	v->uv_addrs		= (struct phaddr *)NULL;
	v->uv_filter		= (struct vif_filter *)NULL;
	v->uv_pim_hello_timer   = 0;
	v->uv_gq_timer          = 0;
	v->uv_pim_neighbors	= (struct pim_nbr_entry *)NULL;
	v->uv_local_pref        = default_source_preference;
	v->uv_local_metric      = default_source_metric;
	add_phaddr(v, &addr, &mask);
	
	if (flags & IFF_POINTOPOINT)
	    v->uv_flags |= (VIFF_REXMIT_PRUNES | VIFF_POINT_TO_POINT);
	syslog(LOG_INFO,
	    "installing %s as if #%u - rate=%d",
	    v->uv_name, numvifs, v->uv_rate_limit);
	++numvifs;
	
	/*
	 * If the interface is not yet up, set the vifs_down flag to
	 * remind us to check again later.
	 */
	if (!(flags & IFF_UP)) {
	    v->uv_flags |= VIFF_DOWN;
	    vifs_down = TRUE;
	}
    }
#endif /* HAVE_GETIFADDRS */
}

static void
add_phaddr(v, addr, mask)
	struct uvif *v;
	struct sockaddr_in6 *addr;
	struct in6_addr *mask;
{
	struct phaddr *pa;
	int i;

	if ((pa = malloc(sizeof(*pa))) == NULL)
		{
		syslog(LOG_ERR, "add_phaddr: memory exhausted");
		return ;
		}

	memset(pa, 0, sizeof(*pa));
	pa->pa_addr = *addr;
	pa->pa_subnetmask = *mask;

	/*
	 * install the prefix of the address derived from the address
	 * and the mask.
	 */
	for (i = 0; i < sizeof(struct in6_addr); i++)
		pa->pa_prefix.sin6_addr.s6_addr[i] =
			addr->sin6_addr.s6_addr[i] & mask->s6_addr[i];
	pa->pa_prefix.sin6_scope_id = addr->sin6_scope_id;

	if (IN6_IS_ADDR_LINKLOCAL(&addr->sin6_addr)) {
		if (v->uv_linklocal) {
			syslog(LOG_WARNING,
			    "add_phaddr: found more than one link-local "
			    "address on %s",
			    v->uv_name);
		}
		v->uv_linklocal = pa;	/* relace anyway */
	}

	/* link into chain */
	pa->pa_next = v->uv_addrs;
	v->uv_addrs = pa;
}

static mifi_t
ifname2mifi(ifname)
	char *ifname;
{
	mifi_t mifi;
	struct uvif *v;

	for (mifi = 0, v = uvifs; mifi < numvifs; ++mifi, ++v) {
		if (strcmp(v->uv_name, ifname) == 0)
		    break;
	}
	return(mifi);
}

#define UNKNOWN        -1
#define EMPTY           1
#define PHYINT          2
#define DEFAULT_SOURCE_METRIC     3
#define DEFAULT_SOURCE_PREFERENCE 4
#define FILTER 5

/*
 * function name: wordToOption
 * input: char *word, a pointer to the word
 * output: int; a number corresponding to the code of the word
 * operation: converts the result of the string comparisons into numerics.
 * comments: called by config_vifs_from_file()
 */
static int 
wordToOption(word)
    char *word;
{
	if (EQUAL(word, ""))
		return EMPTY;
	if (EQUAL(word, "phyint"))
		return PHYINT;
	if (EQUAL(word, "default_source_metric"))
		return DEFAULT_SOURCE_METRIC;
	if (EQUAL(word, "default_source_preference"))
		return DEFAULT_SOURCE_PREFERENCE;
	if (EQUAL(word, "filter"))
		return FILTER;
	return UNKNOWN;
}

/*
 * function name: parse_phyint
 * input: char *s, pointing to the parsing point of the file
 * output: int (TRUE if the parsing was successful, o.w. FALSE)
 * operation: parses the physical interface file configurations, if any.
 * The general form is:
 *     phyint <ifname> [disable]
 */
static int
parse_phyint(s)
    char *s;
{
    char *w, c, *ifname;
    vifi_t vifi;
    struct uvif *v;
    u_int n;
    
    if (EQUAL((w = next_word(&s)), "")) {
	return(FALSE);
    }		/* if empty */
    ifname = w;
    
    for (vifi = 0, v = uvifs; vifi < numvifs; ++vifi, ++v) {
	if (vifi == numvifs) {
	    syslog(LOG_WARNING,
		"phyint %s in is not a configured interface",
		ifname);
	    return(FALSE);
	}	/* if vifi == numvifs */

	if (strcmp(v->uv_name, ifname))
	    continue;
	
	while (!EQUAL((w = next_word(&s)), "")) {
	    if (EQUAL(w, "disable"))
		v->uv_flags |= VIFF_DISABLED;
	    else if (EQUAL(w, "nolistener"))
		v->uv_flags |= VIFF_NOLISTENER;
	    else if(EQUAL(w, "preference")) {
                if(EQUAL((w = next_word(&s)), "")) 
                    syslog(LOG_WARNING,
                        "Missing preference for phyint %s",
                        ifname);
                else if (sscanf(w, "%u%c", &n, &c) != 1 ||
                         n < 1 || n > 255 )
                    syslog(LOG_WARNING,
                        "Invalid preference '%s' for phyint %s",
                        w, ifname);
		else {
			syslog(LOG_DEBUG,
			    "Config setting default local preference on %s to %d.", 
			    ifname, n);
		    v->uv_local_pref = n;
		}
	    
	    } else if(EQUAL(w, "metric")) {
                if(EQUAL((w = next_word(&s)), "")) 
                    syslog(LOG_WARNING,
                        "Missing metric for phyint %s",
                        ifname);
                else if (sscanf(w, "%u%c", &n, &c) != 1 ||
                         n < 1 || n > 1024 )
                    syslog(LOG_WARNING,
                        "Invalid metric '%s' for phyint %s",
                        w, ifname);
		else {
			syslog(LOG_DEBUG,
			    "Config setting default local metric on %s to %d.", 
			    ifname, n);
		    v->uv_local_metric = n;
		}
	    }
	}		/* if not empty */
	break;
    }
    return(TRUE);
}

static int
parse_filter(s)
	char *s;
{
	char *w, *groups, *p;
	mifi_t mifi;
	struct in6_addr grp1, grp2;
	if_set filterset;
	struct mrtfilter *filter;
	int plen = 0, filtertype;

	if (EQUAL((groups = next_word(&s)), "")) {
		syslog(LOG_WARNING, "Missing multicast group");
		return(FALSE);
	}

	/*
	 * Group address specification. Valid format are the followings.
	 * - Group1-Group2: specifies a numerical range of a scope.
	 * - GroupPrefix/Prefixlen: specifies a prefix of a scope. If the
	 *   Prefixlen is omitted, it means the exact match.
	 */
	if ((p = strchr(groups, '-')) != NULL) { /* Group1-Group2 */
		char *maddr1, *maddr2;

		maddr1 = groups;
		maddr2 = p + 1;
		*p = '\0';
		if (inet_pton(AF_INET6, maddr1, (void *)&grp1) != 1 ||
		    !IN6_IS_ADDR_MULTICAST(&grp1)) {
			syslog(LOG_WARNING, "invalid group address %s", maddr1);
			return(FALSE);
		}
		if (inet_pton(AF_INET6, maddr2, (void *)&grp2) != 1 ||
		    !IN6_IS_ADDR_MULTICAST(&grp2)) {
			syslog(LOG_WARNING, "invalid group address %s", maddr2);
			return(FALSE);
		}
		filtertype = FILTER_RANGE;
	}
	else if ((p = strchr(groups, '/')) != NULL) { /* GroupPrefix/Plen */
		char *mprefix = groups;
		int plen = atoi(p + 1);
		*p = '\0';
		if (inet_pton(AF_INET6, mprefix, (void *)&grp1) != 1 ||
		    !IN6_IS_ADDR_MULTICAST(&grp1)) {
			syslog(LOG_WARNING, "invalid group prefix %s", mprefix);
			return(FALSE);
		}
		if (plen < 0 || plen > 128) {
			syslog(LOG_WARNING, "invalid prefix length %s", p + 1);
			return(FALSE);
		}
		filtertype = FILTER_PREFIX;
	}
	else {
		if (inet_pton(AF_INET6, groups, (void *)&grp1) != 1) {
			syslog(LOG_WARNING, "invalid group address %s", groups);
			return(FALSE);
		}
		plen = 128;	/* exact match */
		filtertype = FILTER_PREFIX;
	}

	IF_ZERO(&filterset);
	while (!EQUAL((w = next_word(&s)), "")) {
		if ((mifi = ifname2mifi(w)) == MAXMIFS) {
			/* XXX: scope consideration?? */
			syslog(LOG_WARNING,
			    "phyint %s is not a configured interface", w);
			return(FALSE);
		}
		if (mifi >= IF_SETSIZE) return FALSE;//fix klocwork warning
		IF_SET(mifi, &filterset);
	}
	if (IF_ISEMPTY(&filterset)) {
		syslog(LOG_WARNING,
		    "filter set is empty. ignore it.");
		return(FALSE);
	}

	filter = add_filter(filtertype, &grp1, &grp2, plen);
	IF_COPY(&filterset, &filter->ifset);

	return(TRUE);
}

#if 0
void 
config_vifs_from_file()
{
	FILE *f;
	char linebuf[100];
	char *w, *s;
	int option;

	if ((f = fopen(configfilename, "r")) == NULL) {
		if (errno != ENOENT) log(LOG_WARNING, errno, "can't open %s", 
					 configfilename);
		return;
	}

	while (fgets(linebuf, sizeof(linebuf), f) != NULL) {
		s = linebuf;
		w = next_word(&s);
		option = wordToOption(w);
		switch(option) {
		 case EMPTY:
			 continue;
			 break;
		 case PHYINT:
			 parse_phyint(s);
			 break;
		 case FILTER:
			 parse_filter(s);
			 break;
		 default:
			 log(LOG_WARNING, 0, "unknown command '%s' in %s",
			     w, configfilename);
		}
	}
	fclose(f);
}
#endif

static char *
next_word(s)
    char **s;
{
    char *w;
    
    w = *s;
    while (*w == ' ' || *w == '\t')
	w++;
    
    *s = w;
    for(;;) {
	switch (**s) {
	case ' '  :
	case '\t' :
	    **s = '\0';
	    (*s)++;
	    return(w);
	case '\n' :
	case '#'  :
	    **s = '\0';
	    return(w);
	case '\0' :
	    return(w);
	default   :
	    if (isascii(**s) && isupper(**s))
		**s = tolower(**s);
	    (*s)++;
	}
    }
}

