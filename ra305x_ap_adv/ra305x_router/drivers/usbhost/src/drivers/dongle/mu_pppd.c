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


//#include <sys_inc.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
//Dennis #include <malloc.h>
#include <sys/malloc.h>
#include <network.h>
#include <dongle_l2tp.h>
#include <mu_pppd.h>
#include "mu_cdi.h"//Jody
#include <mu_dongle.h>
#include "ehci.h" //Jody
#include "ehci_def.h" //Jody

#define HANDLER_NAME "sync-pppd"

#define MAX_FDS 256

/* Options for invoking pppd */
#define MAX_OPTS 64
extern cyg_flag_t DongleEvents;
extern int  DONGLE_SEND_QHEAD;
extern int  DONGLE_SEND_QTAIL;
//extern cyg_sem_t L2tp_Queue_Lock;
extern struct l2tp_mbox_struc DONGLE_PACKET_QUEUE[ DONGLE_SEND_QUEUE_SIZE];
extern int dongle_send_process_stop;
extern MGC_DongleDevice *eCos_pDongleDevice;

//extern int ECOS_USBHost_State;
extern unsigned long EHCI_TransferPipe;
extern MUSB_Irp *MTK_pIrp;
extern ehci_Irp MTK_pTxIrp;
extern cyg_sem_t _hDongle_writeWaitSemLock;
extern cyg_mutex_t _hDongle_writeMutexLock;
extern int _hDongle_writeMutexLock_flag;
extern unsigned int eCos_xaccm[8];

/*
 * FCS lookup table as calculated by the table generator.
 */
static unsigned short fcstab[256] = {
   0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
   0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/*--------------------------------------------------------------
* ROUTINE NAME - l2_PppClose         
*---------------------------------------------------------------
* FUNCTION: Kills pppd
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_PppClose(l2tp_session *ses)
{
	l2tp_tunnel *tunnel = (l2tp_tunnel *)ses;
    L2tpSelector *es = tunnel->es;
    struct l2tp_if *ifp;
	
	ifp = (struct l2tp_if *)es->pInfo;
	if (ifp->if_ppp.ifnet.if_reset)
	{
		ifp->if_ppp.hungup = 1;
		die(&ifp->if_ppp, 1);
	}
	
	return;
}


/*--------------------------------------------------------------
* ROUTINE NAME - l2_PppClose         
*---------------------------------------------------------------
* FUNCTION: Forks a pppd process
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int dongle_PppEstablish(struct l2tp_if *ifp)
{
    PppEth_Start(&ifp->if_ppp);//create ppp deamon, and start ppp connect.
	
    return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - encaps_l2tp         
*---------------------------------------------------------------
* FUNCTION: Forks a pppd process
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int encaps_dongle	(
				struct l2tp_if *ifp,
				struct mbuf *m0    
				)
{
	//diag_printf("enter %s\n", __func__);
      /*
	if(dongle_send_process_stop)
	{
		m_freem(m0);
		return 1;
	}	
      */
	DONGLE_PACKET_QUEUE[DONGLE_SEND_QTAIL].ifp = ifp;
	DONGLE_PACKET_QUEUE[DONGLE_SEND_QTAIL].m0 = m0;
	DONGLE_SEND_QTAIL++;
	if(DONGLE_SEND_QTAIL >= DONGLE_SEND_QUEUE_SIZE)
		DONGLE_SEND_QTAIL = 0;
	cyg_flag_setbits(&DongleEvents, DONGLE_EVENT_RX);
	return 0;
	
}	

unsigned short pppfcs16(fcs, cp, len) 
	 unsigned short fcs;
	 unsigned char *cp;
	 int len;
{   
	//ASSERT(sizeof (u16) == 2);
	//ASSERT(((u16) -1) > 0);
	while (len--) 
	  fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
	
	return (fcs); 
}

void Ehci_bulkOutTransferCompleteCallback(void *pParam, const MUSB_Irp *pIrp)
{  
   if(_hDongle_writeMutexLock_flag)
   {
      cyg_semaphore_post(&_hDongle_writeWaitSemLock);
      cyg_mutex_unlock(&_hDongle_writeMutexLock);
      _hDongle_writeMutexLock_flag = 0;
   }
}


/*--------------------------------------------------------------
* ROUTINE NAME - encaps_dongle_usermode         
*---------------------------------------------------------------
* FUNCTION: Forks a pppd process
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int encaps_dongle_usermode(
						struct l2tp_if *ifp,
						struct mbuf *m0    
						)
{
	struct mbuf *m;
	//unsigned char buf[4096+EXTRA_HEADER_ROOM];
	static unsigned char *buf = 0;
	static unsigned char buf_output[2048];

	char *p, *point;
	int len = 0, value = 0;
	int s, num = 0;
	unsigned short trialfcs;

	 int res, lcp_send = 0;
       MUSB_Irp *pIrp;

	 //diag_printf("enter encaps_dongle_usermode, before cyg_mutex_lock\n");
       cyg_mutex_lock(&_hDongle_writeMutexLock);
	 _hDongle_writeMutexLock_flag = 1;
	 //diag_printf("enter encaps_dongle_usermode, after cyg_mutex_lock\n");
       pIrp = &(eCos_pDongleDevice->BulkOutIrp);  
  
	if(!buf)
		buf = malloc(2048);
	if(buf == NULL)
	{
		diag_printf("L2tp: cannot allocate memory\n");
		return 1;
	}	
	s = splimp();
	m = m0;
       p = buf+1;
	point = buf+1;
    /* copy payload into buffer */
	while (m)
       {
    	  len += m->m_len;
         if (m->m_len)
         {
            memcpy(p, mtod(m, char *), m->m_len);
			
            p += m->m_len;
            m = m->m_next;
          }
          else
          {
			diag_printf("L2TP: Wrong m->m_len value\n"); 	
			break;
          }   
      }
      m_freem(m0);
      splx(s);
    /*
    for(res = 0; res < len; res ++)
    {
      diag_printf("0x%x    ", *(point + res)); 
	if(((res+1) %16) == 0)
	  diag_printf("\n");
    }
    diag_printf("\n");	
   */

    trialfcs = 0xffff;
    trialfcs = pppfcs16( trialfcs, point, len );
    trialfcs = ~trialfcs; 
    *buf = 0x7e; //head flag
    /*
    *(buf+len+1) = (trialfcs & 0xff); //FCS
    *(buf+len+2) = ((trialfcs >> 8) & 0xff); //FCS
    *(buf+len+3) = 0x7e; //end flag
     */ 
    for(res = 0; res < (len-2); res ++)
    {
      if((buf[res] == 0xc0) && (buf[res+1] == 0x21)  && (buf[res+2] != 0x9)) //except lcp echo request
      { 
        lcp_send = 1;
	  break;
      	}
    }
    num = 0;
    point = buf_output;

    if(lcp_send)
    {
      *(buf+len+1) = (trialfcs & 0xff); //FCS
      *(buf+len+2) = ((trialfcs >> 8) & 0xff); //FCS
      *(buf+len+3) = 0x7e; //end flag
      for(res = 0; res < (len+4); res ++)
      {
      	  if(buf[res] < 0x20)
	  {	
	    *point = 0x7d;
	     point ++;
	    *point = buf[res] ^ 0x20;
	     point ++;
	     num += 2;
        }else
        {
	    *point = buf[res];
	    point ++;
	    num ++;
        }
      }
	  
      pIrp->pBuffer = buf_output;	  
    }else
    {
      /*
      diag_printf("\n***********************\n");
      for(res = 0; res < 8; res ++)
	  diag_printf("eCos_xaccm[%d]:0x%x,  ", res, eCos_xaccm[res]);
      diag_printf("\n***********************\n");
      */
      //pIrp->pBuffer = buf;
      //num = (len +4);
      *(buf+len+1) = (trialfcs & 0xff); //FCS
      *(buf+len+2) = ((trialfcs >> 8) & 0xff); //FCS
      
      *point = buf[0];
	point ++;
	num ++;

	if((buf[1] == 0xff) && (buf[2] == 0x03))
	{
	   *point = buf[1];
	    point ++;
	    num ++;

	   *point = buf[2];
	   point ++;
	   num ++;
	 }
        for(res = num; res < (len+3); res ++)
       {
      	  if(eCos_xaccm[buf[res] >> 5] & (1<< (buf[res] & 0x1f)))
	  {	
	    *point = 0x7d;
	     point ++;
	    *point = buf[res] ^ 0x20;
	     point ++;
	     num += 2;
        }else
        {
	    *point = buf[res];
	    point ++;
	    num ++;
        }
      } 
      *point = 0x7e;
	num ++;

      pIrp->pBuffer = buf_output;  
    }

    pIrp->dwLength = num;
    //pIrp->dwStatus = 0;
    pIrp->dwActualLength = 0;
    pIrp->pfIrpComplete     = Ehci_bulkOutTransferCompleteCallback;
    pIrp->pCompleteParam    =  (void *)eCos_pDongleDevice;
    //MTK_pIrp = pIrp;
    //EHCI_TransferPipe = eCos_pDongleDevice->hBulkOutPipe;

    MTK_pTxIrp.irp = pIrp;
    MTK_pTxIrp.ePipe = eCos_pDongleDevice->hBulkOutPipe;

    res = submit_dongle_bulk_msg(eCos_pDongleDevice->pPort, eCos_pDongleDevice->hBulkOutPipe, pIrp->pBuffer, pIrp->dwLength);    
    cyg_semaphore_wait(&_hDongle_writeWaitSemLock);
#if 0
    if(!cyg_semaphore_timed_wait(&_hDongle_waitSemLock, cyg_current_time( ) + 1000))
    {
      if(_hDongle_readWriteMutexFlag)
      {
                /*
                value = mtk_readl(EHCI_REGS_COMMAND);
                if(value & CMD_ASE)
                {
                  // Disable async schedule. 
	           value &= ~CMD_ASE;
                  mtk_writel(value, EHCI_REGS_COMMAND);
                 }
                 */
                 //cyg_semaphore_post(&_hDongle_waitSemLock);
                cyg_mutex_unlock(&_hDongle_readWriteMutexLock);
                 _hDongle_readWriteMutexFlag = 0;
		    cyg_thread_delay(1);
                 diag_printf("######enter bulkout.release mutex and semphore.######\n");
      }else
        diag_printf("send dongle!_hDongle_readWriteMutexFlag:%d\n", _hDongle_readWriteMutexFlag);
    }else
      diag_printf("result of encaps_dongle_usermode, pIrp->dwLength:%d\n", pIrp->dwLength);
#endif
	//diag_printf("result of encaps_dongle_usermode, pIrp->dwLength:%d\n", pIrp->dwLength);

    /*
    for(res = 0; res < pIrp->dwLength; res ++)
    {
      diag_printf("%x  ", *(pIrp->pBuffer+res));
	if(((res+1) %16) == 0)
	  diag_printf("\n");
    }
    diag_printf("\n");	
    */
    return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME - l2_HandleFrame         
*---------------------------------------------------------------
* FUNCTION: Forks a pppd process
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
void dongle_HandleFrame(struct l2tp_if *ifp, unsigned char *buf,size_t len)
{
    //l2tp_tunnel *tunnel = (l2tp_tunnel *)ses;
   // L2tpSelector *es = tunnel->es;
    struct mbuf *m = NULL;
    //struct l2tp_if *ifp;
    int s;
	
    //ifp = (struct l2tp_if *)es->pInfo;
    /* Add framing bytes */
    //*--buf = 0x03;
    //*--buf = 0xFF;
    //len += 2;
    s = splimp();
    // Send to ppp
    //diag_printf("enter dongle_HandleFrame\n");

	
    MGETHDR(m, M_DONTWAIT, MT_DATA);
    if (m == NULL)
    {
       diag_printf("enter dongle_HandleFrame, m == NULL\n");
		splx(s);
		return ;
    }
	//diag_printf("enter dongle_HandleFrame2\n");
    MCLGET(m, M_DONTWAIT);
    if ((m->m_flags & M_EXT) == 0) 
    {
          diag_printf("enter dongle_HandleFrame, m->m_flags & M_EXT\n");
    	  splx(s);
		(void) m_free(m);
		return ;
	}
	//diag_printf("enter dongle_HandleFrame3\n");
    // copy data to mbuf
    m->m_len = len;
    bcopy(buf, m->m_data, len);
    m->m_flags |= M_PKTHDR;
    m->m_pkthdr.len = len;
    m->m_pkthdr.rcvif = &ifp->if_ppp.ifnet;
    m->m_next = 0;
	
    splx(s);
    // Send to ppp
    ppppktin(&ifp->if_ppp, m);
}
