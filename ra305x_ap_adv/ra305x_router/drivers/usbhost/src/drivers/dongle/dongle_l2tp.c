/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

#include <dongle_l2tp.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <network.h>
#include <sys/time.h>
#include <mu_pppd.h>
#include <cfg_net.h>
#include <cfg_def.h>
#include <sys_status.h>
#include <eventlog.h>

#define DONGLE_TASK_PRIORITY    6
#define STACKSIZE	1024*6 //4096*2

struct l2tp_if  *dongle_IfList = NULL;
struct l2tp_cfg *dongle_cfgIfList = 0;
//static char l2tp_stack[STACKSIZE];
static char *dongle_stack=NULL;
static cyg_thread dongle_thread_obj;
cyg_handle_t dongle_handle;

//static char l2tp_plus_stack[STACKSIZE];
static char *dongle_plus_stack=NULL;
static cyg_thread dongle_plus_thread_obj;
cyg_handle_t dongle_plus_handle;
//cyg_sem_t _hDongle_waitSemLock;
cyg_sem_t _hDongle_readWaitSemLock;
cyg_sem_t _hDongle_writeWaitSemLock;
cyg_mutex_t _hDongle_readMutexLock;
int _hDongle_readMutexLock_flag = 0;
cyg_mutex_t _hDongle_writeMutexLock;
int _hDongle_writeMutexLock_flag = 0;

cyg_flag_t DongleEvents;
cyg_handle_t dongle_msgq_id;
int DONGLE_SEND_QHEAD;
int DONGLE_SEND_QTAIL;
struct l2tp_mbox_struc DONGLE_PACKET_QUEUE[DONGLE_SEND_QUEUE_SIZE];
int eCosDongleRunning = 0;

extern int encaps_dongle(struct l2tp_if *ifp,struct mbuf *m0);
l2tp_peer dongle_peercfg;


void	dongle_receive_process(struct l2tp_if *pInfo);
void	dongle_send_process(void);
static int close_dongle(struct l2tp_if *pInfo);
static int dongle_reconnect_pppd(struct l2tp_if *pInfo);
static void dongle_StartConnect(L2tpSelector *es);
int l2tp_HandleEvent(L2tpSelector *es, struct l2tp_if *pInfo);
int l2tp_HandleEvent_BoostMode(L2tpSelector *es, struct l2tp_if *pInfo, L2tpHandler *readable_eh, int fd, fd_set readfds);//Jody work
static int dongle_stop(struct l2tp_cfg *pParam);

struct l2tp_cfg dongle_config_param;
static int manual_flag = 0;
static int force_conn = 0;
struct l2tp_if  Dongle_InfoListArray[1];
static int  Dongle_InfoListIndex = 0; 
static  struct l2tp_cfg Dongle_IfListArray[1];
static int  Dongle_IfListIndex; 
static int user_stop;
int dongle_send_process_stop;
static cyg_mbox dongle_tsk_mbox;

extern int (*dialup_checkp) __P((void));
int dongle_dialup_checkp(void);
extern int gettimeofday (struct timeval *, struct timezone *tz);

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_IfNameToInfoList         
*---------------------------------------------------------------
* FUNCTION: This functions searchs L2TP interface information 
*
* INPUT:    pppname - Which L2TP interface to search 
* OUTPUT:   None 
* RETURN:   L2TP interface pointer                                               
* NOTE:     
*---------------------------------------------------------------*/
struct l2tp_if *dongle_IfNameToInfoList(char *pppname)
{
    struct l2tp_if *pInfo;
    
    if (dongle_IfList == NULL)
    {
        return NULL;
    }
    else
    {
        for(pInfo = dongle_IfList; pInfo != NULL; pInfo = pInfo->next)
        {
            if (strcmp(pppname, pInfo->t_pppname) == 0)
            {
                return pInfo;
            }
        }
    }
    return NULL;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_InfoListAdd         
*---------------------------------------------------------------
* FUNCTION: Adds an interface to L2TP_IfList.
*
* INPUT:    pNew    -  which L2TP interfacene to add
* OUTPUT:   None 
* RETURN:   OK or ERROR                                               
* NOTE:     
*---------------------------------------------------------------*/
static int dongle_InfoListAdd(struct l2tp_if *pNew)
{
    struct l2tp_if **ppInfo = &dongle_IfList;

    pNew->next = NULL;
    cyg_scheduler_lock();

    while(*ppInfo)
    {
        if (strcmp(pNew->t_pppname, (*ppInfo)->t_pppname) == 0)
        {
            diag_printf("\n\rl2tp_InfoListAdd: Re-Add L2TP interface %s", 
                   pNew->t_pppname);

            cyg_scheduler_unlock();
            return ERROR;
        } 

        ppInfo = &((*ppInfo)->next);
    }

    *ppInfo = pNew;

    cyg_scheduler_unlock();

    return OK;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_InfoListDel         
*---------------------------------------------------------------
* FUNCTION: Deletes a L2TP interface from L2TP_IfList
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static int dongle_InfoListDel(struct l2tp_if *pDel)
{
    struct l2tp_if **ppInfo = &dongle_IfList;

    cyg_scheduler_lock();

    while (*ppInfo && *ppInfo != pDel )
    {
        ppInfo = &((*ppInfo)->next);
    }
    
    if (*ppInfo == dongle_IfList)
        dongle_IfList = NULL;
    else if (*ppInfo)
        *ppInfo = (*ppInfo)->next;

    cyg_scheduler_unlock();

    return OK;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_DaemonStart         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_DaemonStart(struct l2tp_cfg *pParam)
{
    L2tpSelector *es;
    int debugmask = 1;
    struct l2tp_if *pInfo;
    int sever_fqdn;
    char sever_name[256];
	
    if (dongle_IfNameToInfoList (pParam->pppname) != NULL)
    {
        diag_printf("dongle interface has been started.\n");
        return OK;
    }
    user_stop = 0;
    dongle_send_process_stop = 0;
    DONGLE_SEND_QHEAD = 0;
    DONGLE_SEND_QTAIL = 0;
    Dongle_InfoListIndex = 0;
    pInfo = &Dongle_InfoListArray[Dongle_InfoListIndex++];
    memset(pInfo, 0, sizeof(struct l2tp_if)); 
    es = &pInfo->es;
    es->pInfo = (void *)pInfo;
	
    //l2tp_random_init();
	
	memset(&dongle_peercfg, 0, sizeof(dongle_peercfg));
	dongle_peercfg.addr.sin_port = htons(1701);
	dongle_peercfg.addr.sin_family = AF_INET;
	dongle_peercfg.hide_avps = 0;
	dongle_peercfg.retain_tunnel = 0;
	dongle_peercfg.validate_peer_ip = 1;

    ld_SetBitMask(debugmask);
    
	#if 0
	CFG_get( CFG_L2T_SVRD_SEL, &sever_fqdn );
	if(sever_fqdn)
	{
	    struct hostent *he;
	    
		CFG_get( CFG_L2T_SVRDM, sever_name );
		he = gethostbyname(sever_name);
		if (!he) 
		{
			diag_printf("Could not resolve %s as IP address: error\n",sever_name);
	    	return -1;
		}
		memcpy(&l2tp_peercfg.addr.sin_addr, he->h_addr_list[0], he->h_length);
	}
	else
	{
		struct in_addr in;
		CFG_get_str(CFG_L2T_SVR, sever_name);
		inet_aton(sever_name, &in);
		l2tp_peercfg.addr.sin_addr.s_addr = in.s_addr;
	}	
	#endif
	
	//init PPP parameters
    strcpy(pInfo->t_ifname, pParam->tunnel_ifname);
    strcpy(pInfo->t_pppname, pParam->pppname);
    strcpy(pInfo->t_user, pParam->username); 
    strcpy(pInfo->t_password, pParam->password);

    pInfo->t_idle  = pParam->idle_time;;
    pInfo->t_mtu   = pParam->mtu;
    pInfo->t_mru   = pInfo->t_mtu;

	
    pInfo->if_ppp.defaultroute = TRUE;
    pInfo->if_ppp.pOutput = encaps_dongle;
    pInfo->if_ppp.pIfStopHook = close_dongle;
    pInfo->if_ppp.pIfReconnectHook = dongle_reconnect_pppd;

    /*	
	if(l2tp_NetworkInit(es)<0)
	{
		mon_snd_cmd(MON_CMD_WAN_UPDATE);
		return -1;
	}	
    */
	// Adds to list
    dongle_InfoListAdd(pInfo);

    if(!eCosDongleRunning)
    {
       cyg_flag_init(&DongleEvents);
	cyg_semaphore_init(&_hDongle_readWaitSemLock, 0);
	cyg_semaphore_init(&_hDongle_writeWaitSemLock, 0);
	cyg_mutex_init(&_hDongle_readMutexLock);
	cyg_mutex_init(&_hDongle_writeMutexLock);

	cyg_mbox_create( &pInfo->msgq_id, &dongle_tsk_mbox );
	dongle_msgq_id = pInfo->msgq_id;
	eCosDongleRunning = 1;
    }else
    cyg_mbox_create( &pInfo->msgq_id, &dongle_tsk_mbox );
	
	if(dongle_stack)
		free(dongle_stack);
	dongle_stack =(char *)malloc(STACKSIZE);
	cyg_thread_create(DONGLE_TASK_PRIORITY, dongle_receive_process , (cyg_addrword_t)pInfo, "dongle receive",
                     (void *)dongle_stack, STACKSIZE, &dongle_handle, &dongle_thread_obj);
    
	if(dongle_plus_stack)
		free(dongle_plus_stack);
	dongle_plus_stack =(char *)malloc(STACKSIZE);
    cyg_thread_create(DONGLE_TASK_PRIORITY, dongle_send_process , (cyg_addrword_t)0, "dongle send",
                     (void *)dongle_plus_stack, STACKSIZE, &dongle_plus_handle, &dongle_plus_thread_obj); 
    cyg_thread_resume(dongle_handle); 
    cyg_thread_resume(dongle_plus_handle); 

    diag_printf("enter dongle_DaemonStart, dongle send and receive ok!\n");
    return OK;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_InfoListDel         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_receive_process(struct l2tp_if *pInfo)
{
	int i;
	L2tpSelector *es;
	L2tpHandler *eh;
	struct timeval now;
	l2tp_tunnel *tunnel;
	
	SysLog(LOG_USER|LOG_INFO|LOGM_L2TP,"dongle start up");
	es = &pInfo->es;
	while(1)
	{
	      dongle_PppEstablish(pInfo);
#if 0
		l2tp_StartConnect(es);
		while(1)
		{
			cyg_thread_delay(1);
			if(pInfo->ses->tunnel == NULL)
			{
				mon_snd_cmd(MON_CMD_WAN_UPDATE);
			}
			else
			{
				es->nestLevel++;
				for (eh=es->handlers; eh; eh=eh->next) 
				{
					if (eh->flags & EVENT_FLAG_DELETED) continue;
					if (eh->flags & EVENT_TIMER_BITS)
					{
	    				gettimeofday(&now, NULL);
			    		if (eh->tmout.tv_sec < now.tv_sec)
			    		{	
			    			eh->flags |= EVENT_FLAG_DELETED;
			    			es->opsPending = 1;
			    			if(pInfo->ses->tunnel != NULL)
								lt_HandleTimeout(NULL, 0, 0, (l2tp_tunnel *)pInfo->ses->tunnel);
			    		}	
					}
					if (eh->flags & EVENT_FLAG_READABLE) 
					{
						l2tp_TunnelCtl(es,eh->fd);
					}	
				}
				es->nestLevel--;
				if (!es->nestLevel && es->opsPending) 
				{
					l2tp_PendingChanges(es);
   				}
			
			}		
			tunnel = (l2tp_tunnel *)pInfo->ses->tunnel;
			if (tunnel != NULL && tunnel->state == TUNNEL_ESTABLISHED)
			{
				break;
			}	
			dongle_ScanCmd();
			//if(pInfo->ses->tunnel !=NULL)
			//	pInfo->ses->tunnel = NULL;
		}	

		while(1) 
		{
			i = l2tp_HandleEvent(es,pInfo);
			if (i < 0) 
			{
    			return;
   			}
   			if(i==2)
   			   break;		
		}
#endif
         while(1)
         {
	     i = lf_DongleTakeFromBSD(pInfo);
	     if(i < 0)
		return;
         }
	}	
}	

/*--------------------------------------------------------------
* ROUTINE NAME - l2tpc_send_process         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_send_process(void)
{
	cyg_flag_value_t events;

	while(1)
	{
	    events =
	    cyg_flag_wait(&DongleEvents,
                     DONGLE_EVENT_RX|DONGLE_EVENT_QUIT,
                     CYG_FLAG_WAITMODE_OR | CYG_FLAG_WAITMODE_CLR);
	    if(events & DONGLE_EVENT_RX)
	    {
			while(DONGLE_SEND_QHEAD!=DONGLE_SEND_QTAIL)
			{	
				encaps_dongle_usermode(DONGLE_PACKET_QUEUE[DONGLE_SEND_QHEAD].ifp,
		    	 				DONGLE_PACKET_QUEUE[DONGLE_SEND_QHEAD].m0);
		    	 				
				DONGLE_SEND_QHEAD++;
				if(DONGLE_SEND_QHEAD>=DONGLE_SEND_QUEUE_SIZE)
					DONGLE_SEND_QHEAD = 0;
					
			}
		}	
	 	if(events & DONGLE_EVENT_QUIT)
	 		break;
	} 	
	while(DONGLE_SEND_QHEAD!=DONGLE_SEND_QTAIL)
	{
		m_freem(DONGLE_PACKET_QUEUE[DONGLE_SEND_QHEAD].m0);
		DONGLE_SEND_QHEAD++;
		if(DONGLE_SEND_QHEAD>=DONGLE_SEND_QUEUE_SIZE)
			DONGLE_SEND_QHEAD = 0;
	}	
	//cyg_semaphore_destroy(&L2tp_Queue_Lock);
	cyg_thread_exit();
}		

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_StartConnect         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static void dongle_StartConnect(L2tpSelector *es)
{
    l2tp_session *sess;

    sess = ls_CallINS(&dongle_peercfg, "foobar", es, NULL);
    if (!sess) 
	{
		diag_printf("L2TP:%s",l2tp_get_errmsg());
		return;
    }
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_IfListAdd		                            
*---------------------------------------------------------------
* FUNCTION:  
* INPUT:										
* OUTPUT:
* RETURN:	
*--------------------------------------------------------------*/
static unsigned int dongle_IfListAdd(struct l2tp_cfg *pParam)
{
    struct l2tp_cfg *pTemp, *pPrev, *pNew;

    // Add to Monitor list
    pPrev = 0;
    pTemp = dongle_cfgIfList;
    while (pTemp)
    {
        if (strcmp(pTemp->pppname, pParam->pppname) == 0)
            return 0;   // Already hooked

        pPrev = pTemp;
        pTemp = pTemp->next;
    }
	pNew = &Dongle_IfListArray[Dongle_IfListIndex++];
	memset(pNew,0,sizeof(struct l2tp_cfg));

    memcpy(pNew, pParam, sizeof(struct l2tp_cfg));
    pNew->next = 0;

    // Append to the list
    if (pPrev == 0)
        dongle_cfgIfList = pNew;
    else
        pPrev->next = pNew;
    
    return 0;
}


#if 0
/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_IfListDel		                            
*---------------------------------------------------------------
* FUNCTION:  
* INPUT:										
* OUTPUT:
* RETURN:	
*--------------------------------------------------------------*/
static unsigned int l2tp_IfListDel(char *pppname)
{
    struct l2tp_cfg *pTemp, *pPrev;

    // Add to Monitor list
    pPrev = 0;
    pTemp = dongle_cfgIfList;
    while (pTemp)
    {
        if (strcmp(pTemp->pppname, pppname) == 0)
            break;

        pPrev = pTemp;
        pTemp = pTemp->next;
    }    

    // delete from the list if found
    if (pTemp)
    {
        if (pPrev == 0)
            dongle_cfgIfList = pTemp->next;
        else
            pPrev->next = pTemp->next;
        free (pTemp);
    }
}
#endif

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_InfoListDel         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_init_param(struct l2tp_cfg *pParam)
{
	int mtu=1516;
	
	//CFG_get( CFG_L2T_MTU, &mtu);
	pParam->mtu = mtu;
	//CFG_get( CFG_L2T_IDLET, &pParam->idle_time);
	//CFG_get(CFG_L2T_AUTO, &pParam->auto_reconnect);
	strcpy(pParam->pppname, "ppp0");
      strcpy(pParam->tunnel_ifname, WAN_NAME);
      //CFG_get(CFG_L2T_USER,&pParam->username);
      //CFG_get(CFG_L2T_PASS,&pParam->password);

    return OK;
}	

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_InfoListDel         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_doConnect(void)	
{
	struct l2tp_cfg *pParam;

      diag_printf("enter %s\n", __func__);
	pParam = &dongle_config_param;
	memset((char *)pParam,0,sizeof(struct l2tp_cfg));
	dongle_init_param(pParam);
	if(manual_flag)
	    pParam->auto_reconnect = 2;
	dongle_start(pParam);
	manual_flag = 0;
	force_conn = 0;
	
	return OK;
}

/*--------------------------------------------------------------
* ROUTINE NAME - L2tp_down         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_down(void)
{
	struct l2tp_cfg *pParam;
	struct l2tp_if *pInfo;
	PPP_IF_VAR_T *pPppIf;
	void *data;
	int ret = 0;
	
	dialup_checkp = NULL;
	pParam = &dongle_config_param;
	pInfo = dongle_IfNameToInfoList (pParam->pppname);
	if(!pInfo)
	   return;
	data = pInfo->ses->tunnel;
	pPppIf =&pInfo->if_ppp;

 	ret = dongle_stop(pParam);
	if(ret)
	{
          diag_printf("enter %s, dongle_stop return error!\n", __func__);
	}
	
	if(pPppIf->ifnet.if_reset && pInfo)
	{	
	    diag_printf("enter %s, die(pPppIf, 1)\n", __func__);
	    pPppIf->hungup = 1;
         die(pPppIf, 1);
       }
	
	cyg_thread_delay(200);	
	diag_printf("dongle down...\n");
	//dongle_send_process_stop++;
       //cyg_flag_setbits(&DongleEvents, DONGLE_EVENT_QUIT);
	if(user_stop || Dongle_InfoListIndex>0 )
	{
	/*
	   dongle_InfoListDel(pInfo);
       Dongle_InfoListIndex--;
       if(pInfo->msgq_id)
       {
           cyg_mbox_delete(pInfo->msgq_id);
           pInfo->msgq_id = 0;	
       }
	*/
	/*
       if(dongle_handle)
       {
	       cyg_thread_delete(dongle_handle); // some condition need force terminated
	       if(dongle_stack)
	       {
	       	 free(dongle_stack);
		     dongle_stack =NULL;
	       }
	       dongle_handle = 0;    
	} 
	   
	if(dongle_plus_handle)
       {
	       cyg_thread_delete(dongle_plus_handle); // some condition need force terminated
	       if(dongle_plus_stack)
	       {
	       	 free(dongle_plus_stack);
		     dongle_plus_stack =NULL;
	       }
	       dongle_plus_handle = 0;    
	   }
	 */  
	}
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_start         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_start(struct l2tp_cfg *pParam)
{
    dialup_checkp = NULL;
    if (pParam->auto_reconnect == 0) //keep alive
    {
    	pParam->idle_time = 0;
    	if(!force_conn)
	  dongle_DaemonStart(pParam);
    }	
    else if(pParam->auto_reconnect == 1) //auto connection
    {
    	dialup_checkp = dongle_dialup_checkp;
    }
    else if(pParam->auto_reconnect == 2) //connect by manual
    {
    	pParam->idle_time = 0;
    }
    else
    {
      diag_printf("l2tp: wrong connect mode\n");	
    }  
	
    if(force_conn)
      dongle_DaemonStart(pParam);
    Dongle_IfListIndex = 0;
    dongle_IfListAdd(pParam);
	
    return 0;	
}


/*--------------------------------------------------------------
* ROUTINE NAME - close_l2tp         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static int close_dongle(struct l2tp_if *pInfo)
{
	void *data = pInfo->ses->tunnel;
	
	SysLog(LOG_USER|LOG_INFO|LOGM_L2TP,"dongle clean up");
	/*
	lt_Cleanup(&pInfo->es);
	if(pInfo->ses->tunnel)
		lt_FreeAllHandler(data);	
	*/
	return 0;	
}	
/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_stop         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static int dongle_stop(struct l2tp_cfg *pParam)
{
	
    int cmd;
    struct l2tp_if *pInfo;

    if ((pInfo = dongle_IfNameToInfoList (pParam->pppname)) == NULL)
    {
        diag_printf("enter %s, pInfo == NULL\n");
        return ERROR;
    }
	
    if (pInfo->msgq_id == 0)
    {
        diag_printf("enter %s, pInfo->msgq_id == 0\n");
        return ERROR;
    }
	
    cmd = DONGLE_CMD_STOP;    
    
    cyg_mbox_put(pInfo->msgq_id, cmd);
    
    return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - L2tp_HandleEvent         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_HandleEvent(L2tpSelector *es, struct l2tp_if *pInfo)
{
    fd_set readfds, writefds;
    fd_set *rd, *wr;
    unsigned int flags;

    struct timeval abs_timeout, now;
    struct timeval timeout;
    struct timeval *tm;
    struct timeval tt={1,0};
    L2tpHandler *eh;

    int r = 0;
    int errno_save = 0;
    int foundTimeoutEvent = 0;
    int foundReadEvent = 0;
    int foundWriteEvent = 0;
    int maxfd = -1;
    int pastDue;
    int try_cnt=0;

     /* Build the select sets */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    eh = es->handlers;
    for (eh=es->handlers; eh; eh=eh->next) 
	{
		if (eh->flags & EVENT_FLAG_DELETED) continue;
		if (eh->flags & EVENT_FLAG_READABLE) 
		{
			foundReadEvent = 1;
			FD_SET(eh->fd, &readfds);
			if (eh->fd > maxfd) maxfd = eh->fd;
		}
		if (eh->flags & EVENT_FLAG_WRITEABLE) 
		{
			foundWriteEvent = 1;
			FD_SET(eh->fd, &writefds);
			if (eh->fd > maxfd) maxfd = eh->fd;
		}
		if (eh->flags & EVENT_TIMER_BITS) 
		{
			if (!foundTimeoutEvent) 
			{
				abs_timeout = eh->tmout;
				foundTimeoutEvent = 1;
			} 
			else 
			{
				if (eh->tmout.tv_sec < abs_timeout.tv_sec ||
					(eh->tmout.tv_sec == abs_timeout.tv_sec &&
					 eh->tmout.tv_usec < abs_timeout.tv_usec)) 
				{
					abs_timeout = eh->tmout;
				}
			}
		}
    }
	
    if (foundReadEvent) 
	{
		rd = &readfds;
    } 
	else 
	{
		rd = NULL;
    }
	
    if (foundWriteEvent) 
	{
		wr = &writefds;
    } 
	else 
	{
		wr = NULL;
    }

    if (foundTimeoutEvent) 
	{
		gettimeofday(&now, NULL);
		/* Convert absolute timeout to relative timeout for select */
		timeout.tv_usec = abs_timeout.tv_usec - now.tv_usec;
		timeout.tv_sec = abs_timeout.tv_sec - now.tv_sec;
		if (timeout.tv_usec < 0) 
		{
			timeout.tv_usec += 1000000;
			timeout.tv_sec--;
		}
		if (timeout.tv_sec < 0 ||
			(timeout.tv_sec == 0 && timeout.tv_usec < 0)) 
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
		}
		tm = &timeout;
    } 
	else 
	{
		tm = NULL;
    }

    if (foundReadEvent || foundWriteEvent || foundTimeoutEvent) 
	{
		if(tm)	
			try_cnt = tm->tv_sec;	
		for(;;) 
		{
			dongle_ScanCmd();
			if(try_cnt<0)
				break;
			//r = select(maxfd+1, rd, wr, NULL, tm);
			r = select(maxfd+1, rd, wr, NULL, &tt);
			if (r <= 0) 
			{
				if(r==0)
				{
					try_cnt--;
					continue;
				}	
				if (errno == EINTR) continue;
			}
			break;
		}
    }
   	
    if (foundTimeoutEvent) gettimeofday(&now, NULL);
    errno_save = errno;
    es->nestLevel++;
    if (r >= 0) 
	{
		/* Call handlers */
		for (eh=es->handlers; eh; eh=eh->next) 
		{
			/* Pending delete for this handler?  Ignore it */
			if (eh->flags & EVENT_FLAG_DELETED) continue;
	
			flags = 0;
			if ((eh->flags & EVENT_FLAG_READABLE) &&
			FD_ISSET(eh->fd, &readfds)) 
			{
				flags |= EVENT_FLAG_READABLE;
			}
			if ((eh->flags & EVENT_FLAG_WRITEABLE) &&
				FD_ISSET(eh->fd, &writefds)) 
			{
				flags |= EVENT_FLAG_WRITEABLE;
			}
			if (eh->flags & EVENT_TIMER_BITS) 
			{
				pastDue = (eh->tmout.tv_sec < now.tv_sec ||
					   (eh->tmout.tv_sec == now.tv_sec &&
						eh->tmout.tv_usec <= now.tv_usec));
				if (pastDue) 
				{
					flags |= EVENT_TIMER_BITS;
					if (eh->flags & EVENT_FLAG_TIMER) 
					{
					/* Timer events are only called once */
					es->opsPending = 1;
					eh->flags |= EVENT_FLAG_DELETED;
					}
				}
			}
			/* Do callback */
			if (flags) 
			{
				eh->fn(es, eh->fd, flags, eh->data);
			}
		}
    }

    es->nestLevel--;

    if (!es->nestLevel && es->opsPending) 
	{
		l2tp_PendingChanges(es);
    }
    errno = errno_save;
    return r;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_hangup         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
/*
int dongle_hangup(struct l2tp_if *pInfo)
{
    int cmd;

	if(!pInfo->msgq_id)
		return -1;
    cmd = DONGLE_CMD_HANGUP;
    cyg_mbox_tryput(pInfo->msgq_id, cmd);
    
    return 0;
}
*/
/*--------------------------------------------------------------
* ROUTINE NAME - L2tp_HandleEvent         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static void usage()
{
	diag_printf("usage: l2tp [up|down]\n");
}

int dongle_cmd(int argc,char *argv[])
{
	struct l2tp_cfg  *pParam;
	struct l2tp_if   *pInfo;
	PPP_IF_VAR_T *pPppIf;
	
	pParam = &dongle_config_param;
	pInfo = l2tp_IfNameToInfoList(pParam->pppname);
	pPppIf =&pInfo->if_ppp;
	
	if(argc<1)
	{
	   usage();
	   return 1;
	}   
	
	if ( !strcmp(argv[0], "up" ))
	{
		  if(SYS_wan_up != CONNECTED)
		  {
		  	force_conn =1;
		  	mon_snd_cmd(MON_CMD_WAN_UPDATE);
		  }	
	}	
	else if(!strcmp(argv[0], "down" ))
	{
		   //if(!pInfo)	
		   //		return;
		   mon_snd_cmd( MON_CMD_WAN_STOP );
		   cyg_thread_delay(50);
		   //if disconnect button push, change to manual mode
#ifdef CONFIG_POE_KL_FORCE_DISCON			   
		   if(pParam->auto_reconnect != 1) // for auto-connection, pParam->idle_time should not be zero
		   {	
		   		manual_flag = 1;
		   		pParam->auto_reconnect = 2; 
		   }		
#endif		 
		   mon_snd_cmd(MON_CMD_WAN_UPDATE);
    }	
	else
	  usage();
	return 0;
}
#define _string(s) #s
#define	STR(s)	_string(s)
/*--------------------------------------------------------------
* ROUTINE NAME - L2tp_HandleEvent         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int Get_dongle_up_flag(void)
{
   int flags = 0;
   int wan_mode,auto_mode;
   
   CFG_get( CFG_WAN_IP_MODE, &wan_mode);
   CFG_get( CFG_L2T_AUTO, &auto_mode);

   IfLib_GetFlag (&flags, "ppp0");

   if(wan_mode==5 && auto_mode==1)
   {
      if((flags&IFF_UP)) 
      {
          return true;
      }    
      else
      {
          return false;	
      }    
   }
   else
   {
      return true;       
   }   
}
/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_DaemonWakeup         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_DaemonWakeup(void)
{
	if(dongle_config_param.auto_reconnect == 1)
    	l2tp_DaemonStart(&dongle_config_param);
}  	

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_reconnect_pppd         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static int dongle_reconnect_pppd(struct l2tp_if *pInfo)
{
//    int cmd;
	
    return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_dialup_checkp         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_dialup_checkp(void)
{
	if(!Get_dongle_up_flag())
	{
		diag_printf("Dial up: dongle_DaemonWakeup\n");
		dongle_DaemonWakeup();
		return 0;
	}	
	else
	    return 1;
}	

/*--------------------------------------------------------------
* ROUTINE NAME - l2tp_ScanCmd         
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_ScanCmd(void)
{
	struct l2tp_cfg  *pParam;
	struct l2tp_if   *pInfo;
	int cmd;
	
	pParam = &dongle_config_param;
	pInfo =	dongle_IfNameToInfoList (pParam->pppname); 	
	if(!pInfo)
		return 1;
		
	if((cmd = (int)cyg_mbox_tryget(pInfo->msgq_id)) != 0)
       {
              diag_printf("enter %s, cmd:%d\n", __func__, cmd);
           	if (cmd == DONGLE_CMD_STOP || cmd == DONGLE_CMD_HANGUP)
           	{
           	       /*
           		if(pInfo->if_ppp.ifnet.if_reset && pInfo)
			{	
	   			pInfo->if_ppp.hungup = 1;
    				die(&(pInfo->if_ppp), 1);
    			}
    			else
    			{
    			
    				void *data = pInfo->ses->tunnel;
				if(pInfo->ses->tunnel)
    				{
					lt_StopTunnel(pInfo->ses->tunnel,"Force stop tunnel");
			     		if(pInfo->ses->tunnel)
			     		{
			     			lt_ForceFinallyCleanup(data);
			     		}	
			     	}
			
    			}	
    			*/
			dongle_send_process_stop++;
			if(Sock>0)
			{
				close(Sock);
				Sock = -1;	
			}
    			cyg_flag_setbits(&DongleEvents, DONGLE_EVENT_QUIT);
			dongle_InfoListDel(pInfo);
    			Dongle_InfoListIndex--;
   			if(pInfo->msgq_id)
    			{
    				cyg_mbox_delete(pInfo->msgq_id);
    				pInfo->msgq_id = 0;
    			}	
    			cyg_thread_exit();   
       		}	
    }
	return 0;
}	
