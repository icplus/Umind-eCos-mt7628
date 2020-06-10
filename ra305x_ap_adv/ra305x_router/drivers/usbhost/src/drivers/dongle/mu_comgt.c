/*
 *  comgt version 0.31 - 3G/GPRS datacard management utility
 *
 *  Copyright (C) 2003  Paul Hardwick <paul@peck.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  See comgt.doc for more configuration and usage information.
 *
 */

 /***************************************************************************
* $Id: //WIFI_SOC/release/SDK_4_1_0_0/source/user/comgt-0.32/comgt.c#1 $
 ****************************************************************************/


//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <signal.h>
//#include <termio.h>
//#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include "cfg_def.h"
#include "mu_drcdf.h"
#include "mu_comgt.h"
#include <cyg/kernel/kapi.h>

#include "brd_cnf.h" //Jody
#include "usb.h" //Jody
#include "ehci.h" //Jody
#include "ehci_def.h" //Jody

#include "mu_dongle.h"

#define MAXLABELS 3000  /* Maximum number of labels */
#define MAXGOSUBS 128   /* Max depth */
#define STRINGL 1024    /* String lengths.  Also, max script line length */
#define MAXPATH 1024    /* Max filename length (less or equal to STRINGL) */
#define MAXTOKEN 20     /* Maximum token or label length */
#define GTDEVICE "/dev/modem"

#define BOOL unsigned char
#define NVARS 286       /* a-z, a0-z9 == 26*11 */

//int eCos_ATDFlag = 0;
extern MGC_DongleDevice *eCos_pDongleDevice;
extern ehci_Irp MTK_pRxIrp;//Jody
extern ehci_Irp MTK_pTxIrp;//Jody
extern MUSB_ControlIrp *MTK_pControlIrp;//Jody
extern unsigned long EHCI_TransferPipe;
extern int ECOS_USBHost_State;
extern char chk_link;
extern int dongle_mon(int cmd);
extern cyg_handle_t dongle_handle;
extern cyg_handle_t dongle_plus_handle;

static int MGC_RecATResponse(MUSB_Irp *Irp);

char AT_ResMessage[64];
char AT_Allresponse[128];
int ATCommand_Length = 0;
int Response_Length = 0;
char ATCommand[128];
char Get_ATParm[64];
char ATCFUN[] = "AT+CFUN=1\r";
char ATCPIN[] = "AT+CPIN?\r";
char ATCGATT[] = "AT+CGATT=1\r";
//char ATCGDCONT[] = "AT+CGDCONT=1,\"IP\",\"3gnet\"\r";
//char ATCGDCONT[] = "AT+CGDCONT=1,\"IP\",";
char ATCGDCONT[64];
//char ATD[] = "ATD*99#\r";
char ATD[64];

static void MGC_DongleATCommandComplete(void *pParam, const MUSB_Irp *pIrp)
{
  MGC_DongleDevice *pDongle = NULL;
  //MUSB_Irp *pBulkIrp = pIrp;
  MUSB_Irp *pBulkIrp = &(eCos_pDongleDevice->BulkInIrp);
  int res = 0, num;
  //char ch[32];
  pDongle = (MGC_DongleDevice *)pParam;

  diag_printf("enter %s, pDongle->bState:%d\n", __func__, pDongle->bState);
  switch(pDongle->bState)
  {	
    case SEND_AT_CFUN:
	  MGC_RecATResponse(pBulkIrp);
	  pDongle->bState = RECEIVE_AT_CFUN_RESPONSE;
	  Response_Length = 0;
	  break;
	  
	case RECEIVE_AT_CFUN_RESPONSE:
	  //diag_printf("pBulkIrp->dwActualLength:%d\n", pBulkIrp->dwActualLength);
	  if(pBulkIrp->dwActualLength > 0)
	  { 
	       //for(num = 0; num < pBulkIrp->dwActualLength; num ++)
		  //diag_printf("AT_CFUN_ResMessage[%d]:%c ", num, AT_ResMessage[num]);
		//diag_printf("\n");
		  
		memcpy(&(AT_Allresponse[Response_Length]), AT_ResMessage, pBulkIrp->dwActualLength);
		Response_Length += pBulkIrp->dwActualLength;
	  }
	  //diag_printf("Response_Length:%d, ATCommand_Length:%d\n", Response_Length, ATCommand_Length);	
	  
	  if(Response_Length > ATCommand_Length)
	  {
	    for(num = 0; num < Response_Length-1; num ++)
		{
		  if((AT_Allresponse[num] == 'O') && (AT_Allresponse[num+1] == 'K') && (num >= (ATCommand_Length-1)))
		  {
		    diag_printf("CFUN response ok!\n");
		    pDongle->bState = SEND_AT_CPIN;
		    MGC_SendATCmd(ATCPIN);
		    break;
		  }
		}
		if(num == (Response_Length-1))
		{
		  diag_printf("CFUN response error!\n");
                for(num = 0; num < Response_Length; num ++)
		    diag_printf("AT_Allresponse[%d]:%c\n", num, AT_Allresponse[num]);
				
		 pDongle->bState = SEND_AT_CFUN;
		 MGC_SendATCmd(ATCFUN);
		}
	  }else
	  {
	    MGC_RecATResponse(pBulkIrp);
	  }  
	  break;

	case SEND_AT_CPIN:
	  MGC_RecATResponse(pBulkIrp);
	  pDongle->bState = RECEIVE_AT_CPIN_RESPONSE;
	  Response_Length = 0;
	  break;

	case RECEIVE_AT_CPIN_RESPONSE:
	  if(pBulkIrp->dwActualLength > 0)
	  {
		//for(num = 0; num < pBulkIrp->dwActualLength; num ++)
	         //diag_printf("AT_ResMessage[%d]:%c ", num, AT_ResMessage[num]);
		//diag_printf("\n");
			
		memcpy(&(AT_Allresponse[Response_Length]), AT_ResMessage, pBulkIrp->dwActualLength);
		Response_Length += pBulkIrp->dwActualLength;
	  }
		  
      if(Response_Length > ATCommand_Length)
	  {
		for(num = 0; num < Response_Length-1; num ++)
		{
		  if((AT_Allresponse[num] == 'R') && (AT_Allresponse[num+1] == 'E') && (num >= (ATCommand_Length-1)))
		  {
			diag_printf("CPIN response ok!\n");
			//cyg_thread_delay(1000);
			pDongle->bState = SEND_AT_CGATT;
			MGC_SendATCmd(ATCGATT);
			break;
		  }
		}
		if(num == (Response_Length-1))
		{
		  diag_printf("CPIN response error!\n");
		  for(num = 0; num < Response_Length; num ++)
			diag_printf("AT_Allresponse[%d]:%c\n", num, AT_Allresponse[num]);
		  
		   pDongle->bState = SEND_AT_CPIN;
		   MGC_SendATCmd(ATCPIN);
		}
	  }else
	  {
		MGC_RecATResponse(pBulkIrp);
	  } 
	  break;

	case SEND_AT_CGATT:
		  MGC_RecATResponse(pBulkIrp);
		  pDongle->bState = RECEIVE_AT_CGATT_RESPONSE;
		  Response_Length = 0;
		  break;
	
	case RECEIVE_AT_CGATT_RESPONSE:
		  if(pBulkIrp->dwActualLength > 0)
		  {
			//for(num = 0; num < pBulkIrp->dwActualLength; num ++)
			  //diag_printf("AT_ResMessage[%d]:%c ", num, AT_ResMessage[num]);
			//diag_printf("\n");
				
			memcpy(&(AT_Allresponse[Response_Length]), AT_ResMessage, pBulkIrp->dwActualLength);
			Response_Length += pBulkIrp->dwActualLength;
		  }
			  
		  if(Response_Length > ATCommand_Length)
		  {
			for(num = 0; num < Response_Length-1; num ++)
			{
			  if((AT_Allresponse[num] == 'O') && (AT_Allresponse[num+1] == 'K') && (num >= (ATCommand_Length-1)))
			  {
				diag_printf("CGATT response ok!\n");
				pDongle->bState = SEND_AT_CGDCONT;
				CFG_get(CFG_DG_APN,&Get_ATParm);
				sprintf(ATCGDCONT, "AT+CGDCONT=1,\"IP\",\"%s\"\r", Get_ATParm);
				MGC_SendATCmd(ATCGDCONT);
				break;
			  }
			}
			if(num == (Response_Length-1))
			{
			  diag_printf("device not attach network!\n");
			  for(num = 0; num < Response_Length; num ++)
				diag_printf("AT_Allresponse[%d]:%c\n", num, AT_Allresponse[num]);

			  pDongle->bState = SEND_AT_CGATT;
			  MGC_SendATCmd(ATCGATT);
			}
		  }else
		  {
			MGC_RecATResponse(pBulkIrp);
		  } 
		  break;
  
	case SEND_AT_CGDCONT:
	  MGC_RecATResponse(pBulkIrp);
	  pDongle->bState = RECEIVE_AT_CGDCONT_RESPONSE;
	  Response_Length = 0;
	  break;
	  
	case RECEIVE_AT_CGDCONT_RESPONSE:
       if(pBulkIrp->dwActualLength > 0)
	  {
	       //for(num = 0; num < pBulkIrp->dwActualLength; num ++)
		  //diag_printf("AT_ResMessage[%d]:%c ", num, AT_ResMessage[num]);
		//diag_printf("\n");
		  
		memcpy(&(AT_Allresponse[Response_Length]), AT_ResMessage, pBulkIrp->dwActualLength);
		Response_Length += pBulkIrp->dwActualLength;
	  }
		
	  if(Response_Length > ATCommand_Length)
	  {
	    for(num = 0; num < Response_Length-1; num ++)
		{
		  if((AT_Allresponse[num] == 'O') && (AT_Allresponse[num+1] == 'K') && (num >= (ATCommand_Length-1)))
		  {
		    diag_printf("CGDCONT response ok!\n");
		    pDongle->bState = SEND_AT_ATD;
		    CFG_get(CFG_DG_NUM, &Get_ATParm);
		    sprintf(ATD, "ATD%s\r", Get_ATParm);
		    MGC_SendATCmd(ATD);
		    break;
		  }
		}
		if(num == (Response_Length-1))
		{
		  diag_printf("CGDCONT response error!\n");
                for(num = 0; num < Response_Length; num ++)
		    diag_printf("AT_Allresponse[%d]:%c\n", num, AT_Allresponse[num]);

		  pDongle->bState = SEND_AT_CGDCONT;
		  CFG_get(CFG_DG_APN,&Get_ATParm);
		  sprintf(ATCGDCONT, "AT+CGDCONT=1,\"IP\",\"%s\"\r", Get_ATParm);
		  MGC_SendATCmd(ATCGDCONT);
		}
	  }else
	  {
	    MGC_RecATResponse(pBulkIrp);
	  }  	  
	  break;

	case SEND_AT_ATD:
	  MGC_RecATResponse(pBulkIrp);
	  pDongle->bState = RECEIVE_AT_ATD_RESPONSE;
	  Response_Length = 0;
	  break;
	  
	case RECEIVE_AT_ATD_RESPONSE:
		if(pBulkIrp->dwActualLength > 0)
		{
		  //for(num = 0; num < pBulkIrp->dwActualLength; num ++)
		  //	diag_printf("AT_ATD_ResMessage[%d]:%c ", num, AT_ResMessage[num]);
		  //diag_printf("\n");
			
		  memcpy(&(AT_Allresponse[Response_Length]), AT_ResMessage, pBulkIrp->dwActualLength);
		  Response_Length += pBulkIrp->dwActualLength;
		}
		  
		if(Response_Length > ATCommand_Length)
		{
		  for(num = 0; num < Response_Length-1; num ++)
		  {
			if((AT_Allresponse[num] == 'C') && (AT_Allresponse[num+1] == 'O') && (num >= (ATCommand_Length-1)))
			{
			  diag_printf("ATD response ok!\n");
			  //cyg_thread_delay(1000);
			  //if(!eCos_ATDFlag) //first send
			  //{
			    chk_link = 1;
			    mon_snd_cmd(MON_CMD_WAN_INIT);
			    //eCos_ATDFlag = 1;
			    break;
			  //}else
			  //{
			    //cyg_thread_resume(dongle_handle);
	                 //cyg_thread_resume(dongle_plus_handle);
			   // eCos_ATDFlag = 0;
			  //}
			}
		  }
		  if(num == (Response_Length-1))
		  {
			diag_printf("ATD response error!\n");
			for(num = 0; num < Response_Length; num ++)
			  diag_printf("AT_Allresponse[%d]:%c\n", num, AT_Allresponse[num]);
			
			pDongle->bState = SEND_AT_ATD;
		       CFG_get(CFG_DG_NUM, &Get_ATParm);
		       sprintf(ATD, "ATD%s\r", Get_ATParm);
		        MGC_SendATCmd(ATD);
		  }
		}else
		{
		  MGC_RecATResponse(pBulkIrp);
		}
	  break;
	  
	default:
	  diag_printf("pDongle->bState:%d, bad value, error!\n", __func__, pDongle->bState);
	  break;
   }
	  
}


int MGC_ATCmdStart(void)
{
  char cmd[] = "AT+CFUN=1\r";
  int res;
  MUSB_Irp *pIrp;
  pIrp = &(eCos_pDongleDevice->BulkOutIrp);  
  diag_printf("enter MGC_ATCmdStart\n");

  ATCommand_Length = strlen(cmd);
  memcpy(ATCommand, cmd, strlen(cmd));
  //pIrp->pBuffer = eCos_pDongleDevice->aSetupTx;
  pIrp->pBuffer = cmd;
  pIrp->dwLength = strlen(cmd);
  //pControlIrp->pInBuffer         = 0;
  //pControlIrp->dwInLength        = 0;
  //pControlIrp->dwStatus          = 0;
  //pControlIrp->dwActualOutLength = 0;
  //pControlIrp->dwActualInLength  = 0;
  pIrp->pfIrpComplete     = MGC_DongleATCommandComplete;
  pIrp->pCompleteParam    = (void *)eCos_pDongleDevice;
  //MTK_pIrp = pIrp;
  MTK_pTxIrp.irp = pIrp;
  MTK_pTxIrp.ePipe = eCos_pDongleDevice->hBulkOutPipe;
  ECOS_USBHost_State = MTK_USBHost_DongleTransfer;
  eCos_pDongleDevice->bState = SEND_AT_CFUN;

  res = submit_dongle_bulk_msg(eCos_pDongleDevice->pPort, eCos_pDongleDevice->hBulkOutPipe, pIrp->pBuffer, pIrp->dwLength);
}

int MGC_SendATCmd(char *command)
{
  char *cmd = command;
  int res;
  MUSB_Irp *pIrp; 
  diag_printf("enter MGC_SendATCmd, cmd:%s\n", cmd);

  pIrp = &(eCos_pDongleDevice->BulkOutIrp); 
  ATCommand_Length = strlen(cmd);
  memcpy(ATCommand, cmd, strlen(cmd));
  pIrp->pBuffer = cmd;
  pIrp->dwLength = strlen(cmd);
  //pIrp->dwStatus          = 0;
  pIrp->dwActualLength = 0;
  pIrp->pfIrpComplete     = MGC_DongleATCommandComplete;
  pIrp->pCompleteParam    = (void *)eCos_pDongleDevice;
  MTK_pTxIrp.irp = pIrp;
  MTK_pTxIrp.ePipe = eCos_pDongleDevice->hBulkOutPipe;

  res = submit_dongle_bulk_msg(eCos_pDongleDevice->pPort, eCos_pDongleDevice->hBulkOutPipe, pIrp->pBuffer, pIrp->dwLength);
}

static int MGC_RecATResponse(MUSB_Irp *Irp)
{
  int res = 0;

  memset(AT_ResMessage, 0, sizeof(AT_ResMessage));
  Irp->pBuffer                 = &AT_ResMessage;
  Irp->dwLength		  = 64;
  Irp->dwActualLength     = 0;
  Irp->pfIrpComplete       = MGC_DongleATCommandComplete;
  Irp->pCompleteParam    = (void *)eCos_pDongleDevice;
  
  MTK_pRxIrp.irp = Irp;
  MTK_pRxIrp.ePipe = eCos_pDongleDevice->hBulkInPipe;

  //EHCI_TransferPipe = eCos_pDongleDevice->hBulkInPipe;
  res = submit_dongle_bulk_msg(eCos_pDongleDevice->pPort, eCos_pDongleDevice->hBulkInPipe, Irp->pBuffer, Irp->dwLength);
}


