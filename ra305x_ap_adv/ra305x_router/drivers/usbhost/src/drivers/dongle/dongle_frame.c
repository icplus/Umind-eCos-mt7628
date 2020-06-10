
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

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <dongle_l2tp.h>
#include <mu_pppd.h>
#include <mu_cdi.h>
#include <mu_drcdf.h>
#include <mu_dongle.h>
#include "ehci.h" //Jody
#include "ehci_def.h" //Jody

extern MGC_DongleDevice *eCos_pDongleDevice;
extern int ECOS_USBHost_State;
extern unsigned long EHCI_TransferPipe;
extern MUSB_Irp *MTK_pIrp;
extern cyg_mutex_t _hDongle_readMutexLock;
extern int _hDongle_readMutexLock_flag;
static cyg_handle_t bulkCounter;
static cyg_alarm bulkAlarm_obj;
cyg_handle_t bulkAlarm_hdl;
int Ehci_bulkTransferTimer_flag = 0;
extern ehci_Irp MTK_pRxIrp;

#define PULL_UINT16(buf, cursor, val) \
do { \
    val = ((uint16_t) buf[cursor]) * 256 + (uint16_t) buf[cursor+1]; \
    cursor += 2; \
} while(0)

#define PUSH_UINT16(buf, cursor, val) \
do { \
    buf[cursor] = val / 256; \
    buf[cursor+1] = val & 0xFF; \
    cursor += 2; \
} while (0)

#define GET_AVP_LEN(x) ((((x)[0] & 3) * 256) + (x)[1])

static int dgram_add_random_vector_avp(l2tp_dgram *dgram);

static void dgram_do_hide(uint16_t type,
uint16_t len,
unsigned char *value,
unsigned char const *secret,
size_t secret_len,
unsigned char const *random,
size_t random_len,
unsigned char *output);

static unsigned char *dgram_do_unhide(uint16_t type,
										uint16_t *len,
										unsigned char *value,
										unsigned char const *secret,
										size_t secret_len,
										unsigned char const *random,
										size_t random_len,
										unsigned char *output);

/* Description of AVP's indexed by AVP type */
struct avp_descrip {
    char const *name;		/* AVP name */
    int can_hide;		/* Can AVP be hidden? */
    uint16_t min_len;		/* Minimum PAYLOAD length */
    uint16_t max_len;		/* Maximum PAYLOAD length (9999 = no limit) */
};

static struct avp_descrip avp_table[] = {
    /* Name                     can_hide min_len max_len */
    {"Message Type",            0,       2,      2}, /* 0 */
    {"Result Code",             0,       2,   9999}, /* 1 */
    {"Protocol Version",        0,       2,      2}, /* 2 */
    {"Framing Capabilities",    1,       4,      4}, /* 3 */
    {"Bearer Capabilities",     1,       4,      4}, /* 4 */
    {"Tie Breaker",             0,       8,      8}, /* 5 */
    {"Firmware Revision",       1,       2,      2}, /* 6 */
    {"Host Name",               0,       0,   9999}, /* 7 */
    {"Vendor Name",             1,       0,   9999}, /* 8 */
    {"Assigned Tunnel ID",      1,       2,      2}, /* 9 */
    {"Receive Window Size",     0,       2,      2}, /* 10 */
    {"Challenge",               1,       0,   9999}, /* 11 */
    {"Q.931 Cause Code",        0,       3,   9999}, /* 12 */
    {"Challenge Response",      1,      16,     16}, /* 13 */
    {"Assigned Session ID",     1,       2,      2}, /* 14 */
    {"Call Serial Number",      1,       4,      4}, /* 15 */
    {"Minimum BPS",             1,       4,      4}, /* 16 */
    {"Maximum BPS",             1,       4,      4}, /* 17 */
    {"Bearer Type",             1,       4,      4}, /* 18 */
    {"Framing Type",            1,       4,      4}, /* 19 */
    {"Unknown",                 0,       0,   9999}, /* 20 */
    {"Called Number",           1,       0,   9999}, /* 21 */
    {"Calling Number",          1,       0,   9999}, /* 22 */
    {"Sub-Address",             1,       0,   9999}, /* 23 */
    {"TX Connect Speed",        1,       4,      4}, /* 24 */
    {"Physical Channel ID",     1,       4,      4}, /* 25 */
    {"Intial Received Confreq", 1,       0,   9999}, /* 26 */
    {"Last Sent Confreq",       1,       0,   9999}, /* 27 */
    {"Last Received Confreq",   1,       0,   9999}, /* 28 */
    {"Proxy Authen Type",       1,       2,      2}, /* 29 */
    {"Proxy Authen Name",       1,       0,   9999}, /* 30 */
    {"Proxy Authen Challenge",  1,       0,   9999}, /* 31 */
    {"Proxy Authen ID",         1,       2,      2}, /* 32 */
    {"Proxy Authen Response",   1,       0,   9999}, /* 33 */
    {"Call Errors",             1,      26,     26}, /* 34 */
    {"ACCM",                    1,      10,     10}, /* 35 */
    {"Random Vector",           0,       0,   9999}, /* 36 */
    {"Private Group ID",        1,       0,   9999}, /* 37 */
    {"RX Connect Speed",        1,       4,      4}, /* 38 */
    {"Sequencing Required",     0,       0,      0}  /* 39 */
};

/* A free list of L2TP dgram strucures.  Allocation of L2TP dgrams
must be fast... */
static l2tp_dgram *dgram_free_list = NULL;
extern cyg_sem_t _hDongle_readWaitSemLock;

/*--------------------------------------------------------------
* ROUTINE NAME -          
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static void describe_pulled_avp(uint16_t vendor,
								uint16_t type,
								uint16_t len,
								int hidden,
								int mandatory,
								void *val)
{
    unsigned char *buf = val;
    uint16_t i;
    int ascii = 1;
	
    fprintf(stderr, "Pulled avp: len=%d ", (int) len);
    if (vendor == VENDOR_IETF) {
		fprintf(stderr, "type='%s' ",avp_table[type].name);
    } else {
		fprintf(stderr, "type=%d/%d ", (int) vendor, (int) type);
    }
    fprintf(stderr, "M=%d H=%d ", mandatory ? 1 : 0, hidden ? 1 : 0);
	
    fprintf(stderr, "val='");
    for (i=0; i<len; i++) {
		if (buf[i] < 32 || buf[i] > 126) {
			ascii = 0;
			break;
		}
    }
    for (i=0; i<len; i++) {
		if (ascii) {
			fprintf(stderr, "%c", buf[i]);
		} else {
			fprintf(stderr, "%02X ", (unsigned int) buf[i]);
		}
    }
    fprintf(stderr, "'\n");
}

static void
bulkTransferExpireCallback(cyg_handle_t alarm, cyg_addrword_t data)
{
   int value = 0;

   //diag_printf("######enter bulkTransferExpireCallback.######\n");
   {
     value = mtk_readl(EHCI_REGS_COMMAND);
     if(value & CMD_ASE)
     {
       // Disable async schedule. 
	 value &= ~CMD_ASE;
       mtk_writel(value, EHCI_REGS_COMMAND);
     }

     cyg_semaphore_post(&_hDongle_readWaitSemLock);
     cyg_mutex_unlock(&_hDongle_readMutexLock);
     diag_printf("######enter bulkTransferExpireCallback.release mutex and semphore.######\n");
   }
   //else
   //diag_printf("no need to use timer!_hDongle_readWriteMutexFlag:%d\n", _hDongle_readWriteMutexFlag);
}

int Ehci_BulkTransferTimer( int dwTime, int bPeriodic,
                           x_os_timer_cb_fct pfExpireCallback, void *pParam)
{
    int e_flags;

    //diag_printf("Start delay = %d ms.\n", dwTime*10);
    
    e_flags = (bPeriodic) ? dwTime : 0;//Jody
    cyg_clock_to_counter(cyg_real_time_clock(),&bulkCounter);
    cyg_alarm_create(bulkCounter,
			 pfExpireCallback,
			 (cyg_addrword_t)pParam,
			 &bulkAlarm_hdl,
			 &bulkAlarm_obj);
	
     cyg_alarm_initialize(bulkAlarm_hdl, cyg_current_time()+dwTime, e_flags);
     cyg_alarm_enable(bulkAlarm_hdl);
     Ehci_bulkTransferTimer_flag = 1;
	
    return TRUE;
}

void Ehci_bulkInTransferCompleteCallback(void *pParam, const MUSB_Irp *pIrp)
{
   //diag_printf("enter Ehci_bulkInTransferCompleteCallback, MTK_pRxIrp.irp->dwActualLength:%d\n", 
   //	          MTK_pRxIrp.irp->dwActualLength);
   if(_hDongle_readMutexLock_flag)
   {
     _hDongle_readMutexLock_flag = 0;
     cyg_semaphore_post(&_hDongle_readWaitSemLock);
     cyg_mutex_unlock(&_hDongle_readMutexLock);     
   }
}
/*--------------------------------------------------------------
* ROUTINE NAME -          
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
int lf_DongleTakeFromBSD(struct l2tp_if *ifp)
{
    /* EXTRA_HEADER_ROOM bytes for other headers like PPPoE, etc. */
	
    static unsigned char inbuf[MAX_PACKET_LEN+EXTRA_HEADER_ROOM];
    static unsigned char inbuf_response[MAX_PACKET_LEN];
    unsigned char *buf;	
    int lcp_response = 0;
    int num = 0, value = 0;
    int res, r;
    MUSB_Irp *pIrp;
    pIrp = &(eCos_pDongleDevice->BulkInIrp);  
    //diag_printf("enter %s\n", __func__);
	
    /* Active part of buffer */
    buf = inbuf + EXTRA_HEADER_ROOM;
    ECOS_USBHost_State = MTK_USBHost_DonglePPPTransfer;
	
   // while(1) 
   //	{
		lcp_response = 0;
		dongle_ScanCmd();
	
		//diag_printf("enter lf_DongleTakeFromBSD, before cyg_mutex_lock!\n");
		cyg_mutex_lock(&_hDongle_readMutexLock);
		_hDongle_readMutexLock_flag = 1;
		//diag_printf("enter lf_DongleTakeFromBSD, after cyg_mutex_lock!\n");
		
		memset(inbuf, 0, MAX_PACKET_LEN+EXTRA_HEADER_ROOM);
		memset(inbuf_response, 0, MAX_PACKET_LEN);		
		pIrp->pBuffer = buf;
		//pIrp->dwLength = MAX_PACKET_LEN;
		pIrp->dwLength = MAX_PACKET_LEN;
		//pIrp->dwStatus		  = 0;
		pIrp->dwActualLength = 0;
		pIrp->pfIrpComplete 	= Ehci_bulkInTransferCompleteCallback;
		pIrp->pCompleteParam	= (void *)eCos_pDongleDevice;	
		//MTK_pIrp = pIrp;
		//EHCI_TransferPipe = eCos_pDongleDevice->hBulkInPipe;
		MTK_pRxIrp.irp = pIrp;
		MTK_pRxIrp.ePipe = eCos_pDongleDevice->hBulkInPipe;

		res = submit_dongle_bulk_msg(eCos_pDongleDevice->pPort, eCos_pDongleDevice->hBulkInPipe, pIrp->pBuffer, pIrp->dwLength);
		//Ehci_BulkTransferTimer(500, 0, bulkTransferExpireCallback, 0);
		_hDongle_readMutexLock_flag = 2;
		cyg_semaphore_wait(&_hDongle_readWaitSemLock);

#if 0
		if(!cyg_semaphore_timed_wait(&_hDongle_waitSemLock, cyg_current_time( ) + 500))
		{
		  diag_printf("cyg_semaphore_timed_wait._hDongle_readWriteMutexFlag:%dn", _hDongle_readWriteMutexFlag);
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
                 //cyg_mutex_unlock(&_hDongle_readWriteMutexLock);
                 //_hDongle_readWriteMutexFlag = 0;
				 
		    value = mtk_readl(EHCI_REGS_COMMAND);
		    value |= CMD_IAAD;
                 mtk_writel(value, EHCI_REGS_COMMAND);	  

		    //cyg_thread_delay(1);
                 diag_printf("######enter bulkin.release mutex and semphore.pIrp->dwActualLength:%d######\n", pIrp->dwActualLength);
               }else
                 diag_printf("no need to use timer!_hDongle_readWriteMutexFlag:%d\n", _hDongle_readWriteMutexFlag);
		}else
             diag_printf("enter lf_DongleTakeFromBSD,  pIrp->dwActualLength:%d\n",  pIrp->dwActualLength);
#endif		
           //diag_printf("enter lf_DongleTakeFromBSD,  pIrp->dwActualLength:%d\n",  pIrp->dwActualLength);
#if 1
           if(pIrp->dwActualLength)
           {
             if(pIrp->dwActualLength > 3)
		{
		  for(r = 0; r< (pIrp->dwActualLength -2); r ++)
		  {
		    if((buf[r] == 0xc0) && (buf[r+1] == 0x21) && (buf[r+2] != 10)) //except lcp echo reply
		    {
		      lcp_response = 1;
		      break;
		    }
		  }	
            	}
			 
            num = 0;
            if(lcp_response)
            {
	        for(r = 0; r< pIrp->dwActualLength; r ++)
              {
                if(buf[r] == 0x7e)
			continue;
                if( buf[r] != 0x7d)
		     inbuf_response[num++] = buf[r];
		   else if(buf[r] == 0x7d)
		    { 	
		      inbuf_response[num++] = buf[r+1] ^ 0x20;
		      r ++;
		    }
              }
	        num =num - 2; //delete FCS		
		 dongle_HandleFrame(ifp, inbuf_response, num);  
               /*
	        for(r = 0; r< num; r ++)
		   diag_printf("%x, ",inbuf_response[r]);
		 diag_printf("\n"); 
              */
		 //cyg_thread_delay(10);           
            }else
            {
              if(pIrp->dwActualLength > 3)
	       { 
	         if((buf[0] == 0x7e) && (buf[pIrp->dwActualLength -1] == 0x7e))
	         {
	           //diag_printf("enter lf_DongleTakeFromBSD, ipcp: pIrp->dwActualLength:%d\n",  pIrp->dwActualLength);
	           //num = pIrp->dwActualLength-2; 
			   
		    for(r = 1; r< pIrp->dwActualLength-3; r++) //do not care FCS and end flag
		    {
		      if(buf[r] != 0x7d)
			  inbuf_response[num ++] = 	buf[r];
		      else
		      { 
		        r++;
			 buf[r] ^= 0x20;
			 inbuf_response[num ++] = 	buf[r];
		      }       
		    }
		    /*
		    for(r = 0; r< num; r ++)
		      diag_printf("%x, ", inbuf_response[r]);
		    diag_printf("\n");
		    */
		    dongle_HandleFrame(ifp, inbuf_response, num);    		    
	          }
		  }else
	        {
	          diag_printf("buf[0]:0x%x, buf[%d]:0x%x\n", buf[0], pIrp->dwActualLength -1, buf[pIrp->dwActualLength -1] );
	          dongle_HandleFrame(ifp, buf, pIrp->dwActualLength);
	        }	
            }
          }else
	    {
	      diag_printf("It is terrible!pIrp->dwActualLength:%d\n", pIrp->dwActualLength);
	      //cyg_thread_delay(1);
	    }

           //MTK_pRxIrp.irp = 0;
           //MTK_pRxIrp.ePipe = 0;
#endif
   // }

    return 0;
}

/*--------------------------------------------------------------
* ROUTINE NAME -          
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static void dgram_do_hide(uint16_t type,
							uint16_t len,
							unsigned char *value,
							unsigned char const *secret,
							size_t secret_len,
							unsigned char const *random_data,
							size_t random_len,
							unsigned char *output)
{
    MD5_CTX ctx;
    unsigned char t[2];
    unsigned char digest[16];
    size_t done;
    size_t todo = len;
	
    /* Put type in network byte order */
    t[0] = type / 256;
    t[1] = type & 255;
	
    /* Compute initial pad */
    MD5Init(&ctx);
    MD5Update(&ctx, t, 2);
    MD5Update(&ctx, secret, secret_len);
    MD5Update(&ctx, random_data, random_len);
    MD5Final(digest, &ctx);
	
    /* Insert length */
    output[0] = digest[0] ^ (len / 256);
    output[1] = digest[1] ^ (len & 255);
    output += 2;
    done = 2;
	
    /* Insert value */
    while(todo) 
	{
		*output = digest[done] ^ *value;
		++output;
		++value;
		--todo;
		++done;
		if (done == 16 && todo) 
		{
			/* Compute new digest */
			done = 0;
			MD5Init(&ctx);
			MD5Update(&ctx, secret, secret_len);
			MD5Update(&ctx, output-16, 16);
			MD5Final(digest, &ctx);
		}
    }
}

/*--------------------------------------------------------------
* ROUTINE NAME -          
*---------------------------------------------------------------
* FUNCTION: 
*
* INPUT:    None 
* OUTPUT:   None 
* RETURN:   None                                               
* NOTE:     
*---------------------------------------------------------------*/
static unsigned char * dgram_do_unhide(uint16_t type,
										uint16_t *len,
										unsigned char *value,
										unsigned char const *secret,
										size_t secret_len,
										unsigned char const *random_data,
										size_t random_len,
										unsigned char *output)
{
    MD5_CTX ctx;
    unsigned char t[2];
    unsigned char digest[16];
    uint16_t tmplen;
    unsigned char *orig_output = output;
    size_t done, todo;
	
    /* Put type in network byte order */
    t[0] = type / 256;
    t[1] = type & 255;
	
    /* Compute initial pad */
    MD5Init(&ctx);
    MD5Update(&ctx, t, 2);
    MD5Update(&ctx, secret, secret_len);
    MD5Update(&ctx, random_data, random_len);
    MD5Final(digest, &ctx);
	
    /* Get hidden length */
    tmplen =
	((uint16_t) (digest[0] ^ value[0])) * 256 +
	(uint16_t) (digest[1] ^ value[1]);
	
    value += 2;
	
    if (tmplen > *len-8) 
	{
		l2tp_set_errmsg("Hidden length %d too long in AVP of length %d",
		(int) tmplen, (int) *len);
		return NULL;
    }
	
    /* Adjust len.  Add 6 to compensate for pseudo-header */
    *len = tmplen + 6;
	
    /* Decrypt remainder */
    done = 2;
    todo = tmplen;
    while(todo) 
	{
		*output = digest[done] ^ *value;
		++output;
		++value;
		--todo;
		++done;
		if (done == 16 && todo) 
		{
			/* Compute new digest */
			done = 0;
			MD5Init(&ctx);
			MD5Update(&ctx, secret, secret_len);
			MD5Update(&ctx, value-16, 16);
			MD5Final(digest, &ctx);
		}
    }
    return orig_output;
}

