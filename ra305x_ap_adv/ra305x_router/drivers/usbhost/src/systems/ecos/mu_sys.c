/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

/*
 * uC/OS-II implementation of a system for Controller Driver
 * $Revision: #2 $
 */
#include "plat_arc.h"
#include "mu_tools.h"
#include "plat_cnf.h"
#include "mu_sys.h"
#include "mu_mem.h"
#include "mu_stdio.h"
#include "mu_impl.h"

//#include <cyg/kernel/kapi.h>//Jody add
#include <cyg/hal/drv_api.h>//Jody add
#include "ehci.h"
#include "ehci_def.h"
#include "x_debug.h"
#include "mu_drcdf.h"

typedef struct
{
    MUSB_pfUcosIsr pfIsr;
    void *pIsrParam;
    HANDLE_T hTimerHdl;    
    uint16_t wIrq;
    uint8_t bIndex;
} MGC_AfsUds;// Jody

extern struct QH *qh_list;
extern int MT7620_USB_interruptHandleState;
//extern unsigned long EHCI_TransferPipe;
/***************************** TYPES ******************************/

/** Wrapping Ucos Timer */

typedef struct
{
    uint32_t dwTime; 
    uint32_t dwTimeCount; /* 10 ms base */     
    uint32_t dwTimeOut;
    uint32_t dwIndex;
    MUSB_pfTimerExpired pfExpired;
    void *pExpireParam; /* MGC_Controller ControllerImpl */
} MGC_UcosTimerWrapper;

/* System */
typedef struct _MGC_UcosSystem
{
    MUSB_Controller *pController;
    uint8_t *aQueueMsg;
    uint32_t dwQueueHead;
    uint32_t dwQueueTail;
    MGC_UcosTimerWrapper *aTimer;
    void *pBoardPrivateData; /* MGC_AfsUds* pUds */
    MUSB_SystemServices Services;
    uint8_t bThread;
    HANDLE_T BsrLock;
    HANDLE_T h_th_hdl;
    HANDLE_T ClassLock;  /* Prevent Mbot CBW/DATA/CSW sequence is interrupted by other thread.*/
    HANDLE_T IrpLock;  /* Prevent race condition of two or more Irps writing register.*/
	HANDLE_T msgq_id;        // Message queue ID, Jody
} MGC_UcosSystem;

/**************************** GLOBALS *****************************/
uint8_t MGC_bSystemCount = 0;

#ifdef MUSB_PORT_CONFIG
uint8_t MGC_bUsbCount = 0;
uint8_t MGC_bUsbPortDef[MUSB_MAX_CONTROLLERS];
#endif
static MGC_UcosSystem *MGC_apUcosSystem[MUSB_MAX_CONTROLLERS];

static MGC_UcosTimerWrapper MGC_aTimer[MUSB_MAX_CONTROLLERS][MUSB_MAX_TIMERS];

//static uint32_t MGC_dwTimerBase = 10; /* MS */
static uint32_t MGC_dwTimerBase = 1; /* 1tick = 10MS */


//#define MGC_SYS_LOCK()         _rCrit = x_crit_start()//Jody
//#define MGC_SYS_UNLOCK()    x_crit_end(_rCrit)
#define MGC_SYS_LOCK()        
#define MGC_SYS_UNLOCK()

//static CRIT_STATE_T _rCrit;
static UINT8 _rCrit;//Jody

#define USBSTACKSIZE	4096
static char usb_stack[USBSTACKSIZE];
static cyg_thread usb_thread_obj;
static cyg_handle_t usb_handle;
static cyg_mbox usb_tsk_mbox;
cyg_flag_t usbEvents;
/*************************** FUNCTIONS ****************************/

/*
 * ISR, arranged to be called by board-specific code.
 */
static uint8_t MGC_UcosControllerIsr(void *pPrivateData)
{
    int32_t isrValue;

    MGC_UcosSystem *pSystem;
    int32_t i4_ret = 0;

    //MUSB_ASSERT(pPrivateData);
    pSystem = (MGC_UcosSystem *)pPrivateData;
    //MUSB_ASSERT(pSystem);
    //MUSB_ASSERT(pSystem->pController);
    //MUSB_ASSERT(pSystem->pController->pfIsr);

	if(ECOS_USBHost_State <= MTK_USBHost_StorageInit)
	  diag_printf("###### first enter:%s\n", __func__);

	MT7620_USB_interruptHandleState = 2;

    /* call controller ISR */
    isrValue = pSystem->pController->pfIsr(pSystem->pController->pIsrParam);

    /* wake BSR task if indicated */
    if (isrValue > 0)
    {
        //i4_ret = (int32_t)x_sema_unlock(pSystem->BsrLock);
        //MUSB_ASSERT(OSR_OK == i4_ret);
        if(ECOS_USBHost_State <= MTK_USBHost_StorageInit)
          diag_printf("###### enter:%s, isrValue:%d, cyg_mbox_put interrupt message!\n", __func__, isrValue);

	MT7620_USB_interruptHandleState = 6;
        //cyg_mbox_put(pSystem->msgq_id, MUSB_INTERRUPT);
        cyg_flag_setbits(&usbEvents, MUSB_INTERRUPT);
	MT7620_USB_interruptHandleState = 7;
    }
	else if (isrValue <= 0)
    {
        diag_printf("###### enter:%s, ISR return error, isrValue:%d!\n", __func__, isrValue);
        return FALSE;
    }

    //UNUSED(i4_ret);
    return TRUE;
}

//  MTK Notice: Max Liao, 2006/10/03.
//  Time out handler. Unlock BSR task to process pfExpireCallback.
static void MGC_UcosArmTimerOut(HANDLE_T pt_tm_handle, void *pv_tag)
{
    MGC_UcosSystem *pSystem;
    MGC_UcosTimerWrapper *pTimer;
    int32_t i4_ret = 0;
    uint32_t i;
    uint8_t bTimeOutOccur = FALSE;

    UNUSED(pt_tm_handle);
    MUSB_ASSERT(pv_tag);

    pSystem =  (MGC_UcosSystem *)pv_tag;
    pTimer = pSystem->aTimer;
    MUSB_ASSERT(pTimer);

    for (i=0; i<MUSB_MAX_TIMERS; i++)
    {
        if ((pTimer->dwTime) && (!pTimer->dwTimeOut))
        {
            pTimer->dwTimeCount += MGC_dwTimerBase;
            if (pTimer->dwTime == pTimer->dwTimeCount)
            {
                diag_printf("***###*** enter:%s, timer%d expired!\n", __func__, i);
                pTimer->dwTimeOut = TRUE;
                bTimeOutOccur = TRUE;
            }
        }
        pTimer ++;
    }

    if (bTimeOutOccur)
    {
      /*//Jody
          i4_ret = (int32_t)x_sema_unlock(pSystem->BsrLock);        
          MUSB_ASSERT(OSR_OK == i4_ret);
          UNUSED(i4_ret);
          */
      diag_printf("***###*** enter:%s, cyg_mbox_tryput MUSB_TIMER_EXPIRE!\n", __func__);
      //cyg_mbox_put(pSystem->msgq_id, MUSB_TIMER_EXPIRE);
      cyg_flag_setbits(&usbEvents, MUSB_TIMER_EXPIRE);
    }
}

/*
 * The UCD calls this to arm a timer (periodic or one-shot).
 * Call the board-specific function.
 * wIndex: 
 * Timer Index 0 is reserved for EP0 device configuration. 
 * Timer Index 1 is reserved for Hub class device configuration. 
 * Timer Index 2 is reserved for Mass storage class driver. 
 */
static uint8_t MGC_UcosArmTimer(void *pPrivate, uint16_t wIndex, uint32_t dwTime, uint8_t bPeriodic,
                                MUSB_pfTimerExpired pfExpireCallback, void *pExpireParam)
{
    MGC_UcosSystem *pSystem;
    MGC_UcosTimerWrapper *pTimer;

	diag_printf("***###*** enter:%s, wIndex:%d, dwTime:%d, bPeriodic:%d\n", __func__, wIndex, dwTime, bPeriodic);
	
    MUSB_ASSERT(pPrivate);
    MUSB_ASSERT(pfExpireCallback);
    UNUSED(bPeriodic);
    
    pSystem = (MGC_UcosSystem *)pPrivate;

    MUSB_ASSERT(wIndex < MUSB_MAX_TIMERS);
    pTimer = &pSystem->aTimer[wIndex];
    MUSB_ASSERT(pTimer);
    MUSB_ASSERT(pTimer->dwTime == 0);
    
    pTimer->dwTime = dwTime;
    pTimer->dwTimeCount = 0;
    pTimer->pfExpired = pfExpireCallback;
    pTimer->dwTimeOut = FALSE;
    pTimer->dwIndex = (uint32_t)wIndex;
    pTimer->pExpireParam = pExpireParam;

    return TRUE;
}

/*
 * The UCD calls this to cancel a timer.
 * Call the board-specific function.
 */
static uint8_t MGC_UcosCancelTimer(void *pPrivate, uint16_t wIndex)
{
    MGC_UcosSystem *pSystem;
    MGC_UcosTimerWrapper *pTimer;    
    uint8_t bRet = FALSE;

    if (pPrivate)
    {
        pSystem = (MGC_UcosSystem *)pPrivate;
        MUSB_ASSERT(wIndex < MUSB_MAX_TIMERS);
        pTimer = &pSystem->aTimer[wIndex];
        MUSB_ASSERT(pTimer);

        if (pTimer->dwTime > 0)
        {
            MUSB_MemSet((void *)pTimer, 0, sizeof(MGC_UcosTimerWrapper));
        }
        bRet = TRUE;
    }

    return bRet;
}

/*
* Controller calls this to enqueue a background item
*/
static uint8_t MGC_UcosQueueBackgroundItem(void *pPrivate, const void *pItem)
{
    MGC_UcosSystem *pSystem;

    //MUSB_ASSERT(pPrivate);
    //MUSB_ASSERT(pItem);
    pSystem = (MGC_UcosSystem *)pPrivate;
    //MUSB_ASSERT(pSystem->pController);

	MT7620_USB_interruptHandleState = 5;

	usb_dbg("###### enter:%s, bCause:%d\n", __func__, ((MGC_BsrItem *)pItem)->bCause);

	//diag_printf("###### enter:%s, pSystem->dwQueueHead:0x%x, pSystem->pController->wQueueLength:0x%x, \
	//		pSystem->dwQueueTail:0x%x\n", __func__, pSystem->dwQueueHead, pSystem->pController->wQueueLength, pSystem->dwQueueTail);

    /* Post a message to the queue */
    if (((pSystem->dwQueueHead + 1) %pSystem->pController->wQueueLength) != pSystem->dwQueueTail)
    {

		#if 0 //Jody
        MUSB_MemCopy((void *)(
                         (uint32_t)pSystem->aQueueMsg + (pSystem->dwQueueHead
                                                             *pSystem->pController->wQueueItemSize)),
                     pItem,
                     pSystem->pController->wQueueItemSize);
        #endif
        memcpy((void *)((uint32_t)pSystem->aQueueMsg + (pSystem->dwQueueHead*pSystem->pController->wQueueItemSize)),
                     pItem,
                     pSystem->pController->wQueueItemSize);

		
        pSystem->dwQueueHead = (pSystem->dwQueueHead + 1) % pSystem->pController->wQueueLength;

        //VERIFY(OSR_OK == x_sema_unlock(pSystem->BsrLock));//Jody

		//diag_printf("###### enter:%s, return TRUE! pSystem->dwQueueHead:0x%x, pSystem->dwQueueTail:0x%x\n", __func__, pSystem->dwQueueHead, pSystem->dwQueueTail);
        return (TRUE);
    }

	//diag_printf("###### enter:%s, return FALSE!\n", __func__);
    return (FALSE);
}

/*
* Controller calls this to dequeue a background item
*/
static uint8_t MGC_UcosDequeueBackgroundItem(void *pPrivate, void *pItem)
{
    MGC_UcosSystem *pSystem;

    MUSB_ASSERT(pPrivate);
    MUSB_ASSERT(pItem);
    pSystem = (MGC_UcosSystem *)pPrivate;
    MUSB_ASSERT(pSystem->pController);

    /* Read a message from Queue */
    if (pSystem->dwQueueTail != pSystem->dwQueueHead)
    {
        MUSB_MemCopy(pItem,
                     (void *)((uint32_t)
                                  pSystem->aQueueMsg + (pSystem->dwQueueTail
                                                            *pSystem->pController->wQueueItemSize)),
                     pSystem->pController->wQueueItemSize);

        pSystem->dwQueueTail = (pSystem->dwQueueTail + 1) % pSystem->pController->wQueueLength;

        return (TRUE);
    }

    return (FALSE);
}

static void MGC_UcosFlushBackgroundQueue(void *pPrivate)
{
    MGC_UcosSystem *pSystem;

    MUSB_ASSERT(pPrivate);
    pSystem = (MGC_UcosSystem *)pPrivate;

    pSystem->dwQueueTail = 0;
    pSystem->dwQueueHead = 0;
    return;
}

/*
* Controller calls this to enter a lock
*/
static uint8_t MGC_UcosClassLock(void *pPrivate, uint16_t wIndex)
{
    MGC_UcosSystem *pSystem;

    int32_t i4_ret = 0;

    MUSB_ASSERT(pPrivate);
    UNUSED(wIndex);    
    
    pSystem = (MGC_UcosSystem *)pPrivate;

    //i4_ret = (int32_t)x_sema_lock(pSystem->ClassLock, X_SEMA_OPTION_WAIT);//Jody
    MUSB_ASSERT(OSR_OK == i4_ret);
    UNUSED(i4_ret);    
    return (TRUE);
}

/*
* Controller calls this to exit a lock
*/
static uint8_t MGC_UcosClassUnlock(void *pPrivate, uint16_t wIndex)
{
    MGC_UcosSystem *pSystem;

    int32_t i4_ret = 0;

    MUSB_ASSERT(pPrivate);
    UNUSED(wIndex);    
    
    pSystem = (MGC_UcosSystem *)pPrivate;

    /* Release the semaphore */
    //i4_ret = (int32_t)x_sema_unlock(pSystem->ClassLock);//Jody

    return (OSR_OK == i4_ret);
}

/*
* Controller calls this to enter a lock
*/
static uint8_t MGC_UcosIrpLock(void *pPrivate, uint16_t wIndex)
{
    MGC_UcosSystem *pSystem;

    int32_t i4_ret = 0;

    MUSB_ASSERT(pPrivate);
    UNUSED(wIndex);    
    UNUSED(pPrivate); 
    UNUSED(i4_ret);
    UNUSED(pSystem);
    MGC_SYS_LOCK();
    return TRUE;
}

/*
* Controller calls this to exit a lock
*/
static uint8_t MGC_UcosIrpUnlock(void *pPrivate, uint16_t wIndex)
{
    MGC_UcosSystem *pSystem;

    int32_t i4_ret = 0;

    MUSB_ASSERT(pPrivate);
    UNUSED(wIndex);    
    UNUSED(pPrivate); 
    UNUSED(i4_ret);
    UNUSED(pSystem);
    MGC_SYS_UNLOCK();
    return (TRUE);
}

/*
* Controller calls this to enter a lock
*/
static uint32_t MGC_UcosIrqLock(void *pPrivate)
{
    MGC_UcosSystem *pSystem;
    
    MUSB_ASSERT(pPrivate);
    pSystem = (MGC_UcosSystem *)pPrivate;

    return MUSB_BoardIrqLock(pSystem->pBoardPrivateData);
}

static void MGC_UcosIrqUnlock(void *pPrivate, uint32_t dwPrevIrq)
{
    MGC_UcosSystem *pSystem;
    
    MUSB_ASSERT(pPrivate);
    pSystem = (MGC_UcosSystem *)pPrivate;

    MUSB_BoardIrqUnlock(pSystem->pBoardPrivateData, dwPrevIrq);
}

static uint32_t MGC_UcosIsIrqPending(void *pPrivate)
{
    MGC_UcosSystem *pSystem;
    
    MUSB_ASSERT(pPrivate);
    pSystem = (MGC_UcosSystem *)pPrivate;

    return MUSB_BoardIsIrqPending(pSystem->pBoardPrivateData);
}

/* Back ground service Routine */
void MGC_UcosBsrThread(MGC_UcosSystem *pParam)
{
    cyg_flag_value_t i4Ret;
    MGC_UcosSystem *pSystem = pParam;
    uint32_t i;
    MGC_UcosTimerWrapper *pTimer;
	u32 cmd;

    //UNUSED(i4Ret);
    //MUSB_ASSERT(pParam);
    //pSystem = (MGC_UcosSystem *)(*((uint32_t *)pParam));

    while (1)
    {
         i4Ret = cyg_flag_wait(&usbEvents,
                     MUSB_INTERRUPT|MUSB_TIMER_EXPIRE, CYG_FLAG_WAITMODE_OR | CYG_FLAG_WAITMODE_CLR);
	  //i4Ret = (int)cyg_mbox_get(pSystem->msgq_id);
	  {
	    
	    MT7620_USB_interruptHandleState = 8;	  
	    if(ECOS_USBHost_State <= MTK_USBHost_Enumeration)
             diag_printf("###### enter:%s, MGC_bSystemCount:%d, i4Ret:%d\n", __func__, MGC_bSystemCount, i4Ret);
            	
        // Check timer time out must in front of pfBsr(). Because it will activate timer event in pfBsr().
       #if 0
		cmd = mtk_readl(EHCI_REGS_COMMAND);
		if(cmd & CMD_ASE)
		{
		  int ret = 0;
		  // Disable async schedule. 
		  cmd &= ~CMD_ASE;
		  mtk_writel(cmd, EHCI_REGS_COMMAND);
		  /*
		  ret = MT7620_handshake(EHCI_REGS_STATUS, STS_ASS, 0, 100);
		  if (ret < 0) 
		  {
			diag_printf("EHCI fail timeout STD_ASS reset\n");
		  }
		  */
		  //qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh_list | QH_LINK_TYPE_QH));
		}
      #endif
        // check isr.
        if(i4Ret == MUSB_INTERRUPT)
		{
          if (pSystem->dwQueueTail != pSystem->dwQueueHead)
          {
            
            if(ECOS_USBHost_State <= MTK_USBHost_Enumeration)
              diag_printf("###### enter:%s, pSystem->dwQueueTail:0x%x, pSystem->dwQueueHead:0x%x\n", __func__, pSystem->dwQueueTail, pSystem->dwQueueHead);
             
			pSystem->pController->pfBsr(pSystem->pController->pBsrParam);
          }else
          {
            diag_printf("###### enter:%s, pSystem->dwQueueTail== pSystem->dwQueueHead, pSystem->dwQueueHead:0x%x\n", __func__, pSystem->dwQueueHead);
          }
        }else if(i4Ret == MUSB_TIMER_EXPIRE)
		{
		  //diag_printf("###### enter:%s, MGC_bSystemCount:%d, i4Ret:%d\n", __func__, MGC_bSystemCount, i4Ret);
          for (i=0; i<MUSB_MAX_TIMERS; i++)
          {
            pTimer = &pSystem->aTimer[i];
            if ((pTimer->dwTime) && (pTimer->dwTimeOut))
            {
                pTimer->dwTimeOut = FALSE;
                pTimer->dwTime = 0;
                // process timer event.
                if (pTimer->pfExpired)
                {
                    diag_printf("***###*** enter:%s, timer%d expired!\n", __func__, i);
                    pTimer->pfExpired(pTimer->pExpireParam, pTimer->dwIndex);
                }
            }
          }
        }else
        {
          diag_printf("enter MGC_UcosBsrThread, cyg_mbox_get wrong value:%d\n", i4Ret);
        }
	  }
    }
}

/*
 * Delete all the resources allocated for the given system
 */
#ifndef MUSB_CODE_SHRINK
static void MGC_UcosDestroyController(MGC_UcosSystem *pSystem)
{
    //    uint16_t wIndex;
    int32_t i4_ret = 0;

    MUSB_ASSERT(pSystem);

    diag_printf("@@@@@@ enter:%s\n", __func__);

#ifndef MUSB_CODE_SHRINK
    MUSB_StopController(pSystem->pController);
    MUSB_DestroyController(pSystem->pController);
    MUSB_BoardDestroyController(pSystem->pBoardPrivateData);
#endif

    /* Delete the Queues created and release corresponding memory */

    if (pSystem->aQueueMsg)
    {
        MUSB_MemFree(pSystem->aQueueMsg);
        pSystem->aQueueMsg = NULL;
    }

    /* Delete the timer created and release corresponding memory */
    pSystem->aTimer = NULL;

    /* destroy mailbox for BSR task */
    //i4_ret = x_sema_delete(pSystem->BsrLock);//Jody
    //MUSB_ASSERT(OSR_OK == i4_ret);

    /* destroy mailbox for class driver */
    //i4_ret = x_sema_delete(pSystem->ClassLock);//Jody
    //MUSB_ASSERT(OSR_OK == i4_ret);

    //i4_ret = x_sema_delete(pSystem->IrpLock);//Jody
    //MUSB_ASSERT(OSR_OK == i4_ret);

    /* Delete thread */
    if (pSystem->bThread)
    {
        /* destroy BSR task */
        cyg_thread_exit();//Jody
        pSystem->bThread = 0;
    }

    MUSB_MemFree(pSystem);

    UNUSED(i4_ret);    
}
#endif
/*
* Initialize the system with the services
*/
static MGC_UcosSystem *MGC_UcosInitController(const MUSB_UcosController *pControllerInfo,
                                              uint8_t bBsrPriority)
{
    MGC_UcosSystem *pSystem;

    uint8_t *pBaseIsr;
    uint8_t *pBaseBsr;
    int32_t err;
    //    uint16_t 	wIndex;
    uint32_t dwStatus;
    CHAR szPSbsr [8] = {0};
	uint8_t save_count = MGC_bSystemCount;

    if(pControllerInfo)
	  usb_dbg("****** enter:%s, dwBsrPriority:%d\n", __func__, bBsrPriority);
	else
	  diag_printf("****** enter:%s, pControllerInfo == NULL\n", __func__);

    //pSystem = (MGC_UcosSystem *)MUSB_MemAlloc(sizeof(MGC_UcosSystem));
	pSystem = (MGC_UcosSystem *)malloc(sizeof(MGC_UcosSystem));
    if(!pSystem){
	  diag_printf("###### enter:%s, no memory!", __func__);
	  return NULL;
    }
		
	memset(pSystem,0,sizeof(MGC_UcosSystem));

    if (pSystem)
    {
        pSystem->Services.wVersion = MUSB_SYSTEM_SERVICES_VERSION;

        pSystem->Services.pPrivateData = (void *)pSystem;
        pSystem->Services.pfSystemToBusAddress = MUSB_BoardSystemToBusAddress;
        pSystem->Services.pfQueueBackgroundItem = MGC_UcosQueueBackgroundItem;
        pSystem->Services.pfDequeueBackgroundItem = MGC_UcosDequeueBackgroundItem;
        pSystem->Services.pfFlushBackgroundQueue = MGC_UcosFlushBackgroundQueue;
        pSystem->Services.pfArmTimer = MGC_UcosArmTimer;
        pSystem->Services.pfCancelTimer = MGC_UcosCancelTimer;
        pSystem->Services.pfClassLock = MGC_UcosClassLock;
        pSystem->Services.pfClassUnlock = MGC_UcosClassUnlock;
        pSystem->Services.pfIrpLock = MGC_UcosIrpLock;
        pSystem->Services.pfIrpUnlock = MGC_UcosIrpUnlock;
        pSystem->Services.pfIrqLock = MGC_UcosIrqLock;
        pSystem->Services.pfIrqUnlock = MGC_UcosIrqUnlock;
        pSystem->Services.pfIsIrqPending = MGC_UcosIsIrqPending;

        /* for structured error handling: */
        /* try target-specific init */
        MUSB_ASSERT(pControllerInfo);

        pBaseIsr = pControllerInfo->pBase;
        pBaseBsr = pControllerInfo->pBase;
        pSystem->pBoardPrivateData =
            MUSB_BoardInitController(pSystem, MGC_UcosControllerIsr, pControllerInfo, &pBaseIsr,
                                     &pBaseBsr);

        if (!pSystem->pBoardPrivateData)
        {
            diag_printf("###### enter %s, pSystem->pBoardPrivateData == NULL!", __func__);
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }
        /* try UCD init */
        pSystem->pController = MUSB_NewController(pControllerInfo->wType, pBaseIsr, pBaseBsr);

        if (!pSystem->pController)
        {
            diag_printf("###### enter %s, pSystem->pController == NULL!", __func__);
		
#ifndef MUSB_CODE_SHRINK       
            MUSB_BoardDestroyController(pSystem->pBoardPrivateData);
#endif
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }
		usb_dbg("###### enter %s, pSystem->pController != NULL, MGC_bSystemCount:%d\n", __func__, MGC_bSystemCount);

		
        /* prepare queue */
        pSystem->aQueueMsg = malloc(
                                 pSystem->pController->wQueueLength *pSystem->pController->wQueueItemSize);

        if (!pSystem->aQueueMsg)
        {
#ifndef MUSB_CODE_SHRINK              
            MGC_UcosDestroyController(pSystem);
#endif
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }

        //MUSB_MemSet(pSystem->aQueueMsg, 0,
        //            pSystem->pController->wQueueLength *pSystem->pController->wQueueItemSize);
        memset(pSystem->aQueueMsg, 0,
                    pSystem->pController->wQueueLength *pSystem->pController->wQueueItemSize);

        /* Initialize queue head and tail to null */
        pSystem->dwQueueTail = 0;
        pSystem->dwQueueHead = 0;
        

        /* prepare timer */
        //MUSB_ASSERT(MGC_bSystemCount < MUSB_MAX_CONTROLLERS);
        if (MGC_bSystemCount >= MUSB_MAX_CONTROLLERS)
        {
#ifndef MUSB_CODE_SHRINK              
            MGC_UcosDestroyController(pSystem);
#endif
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }        
        pSystem->aTimer = &MGC_aTimer[MGC_bSystemCount][0];
        //MUSB_MemSet(pSystem->aTimer, 0, MUSB_MAX_TIMERS * sizeof(MGC_UcosTimerWrapper));
        memset(pSystem->aTimer, 0, MUSB_MAX_TIMERS * sizeof(MGC_UcosTimerWrapper));
        
        /* create mailbox to wake the BSR task */
        //VERIFY(OSR_OK == x_sema_create(&pSystem->BsrLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK));

        //  MTK Notice: Max Liao, 2006/11/23.
        //  Class semphore protect BSR task and user's task sending job at the same time.
        //VERIFY(OSR_OK == x_sema_create(&pSystem->ClassLock, X_SEMA_TYPE_BINARY,
        //                               X_SEMA_STATE_UNLOCK));

        //Prevent race condition of two or more Irps writing register.
        //VERIFY(OSR_OK == x_sema_create(&pSystem->IrpLock, X_SEMA_TYPE_BINARY,
        //                               X_SEMA_STATE_UNLOCK));

        /* try target-specific timer init now that we know the requirements */
        if (!MUSB_BoardInitTimers(pSystem->pBoardPrivateData))
        {
#ifndef MUSB_CODE_SHRINK
            MGC_UcosDestroyController(pSystem);
#endif
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }

        // Repeat timer.
        (void)MUSB_BoardArmTimer(pSystem->pBoardPrivateData, 0, MGC_dwTimerBase, TRUE,
                             MGC_UcosArmTimerOut, pSystem);
       
		usb_dbg("###### enter %s, before MUSB_StartController(), MGC_bSystemCount:%d\n", __func__, MGC_bSystemCount);

        /* start the controller */
		
        dwStatus = MUSB_StartController(pSystem->pController, &(pSystem->Services));

        if (MUSB_STATUS_OK != dwStatus)
        {
           diag_printf("###### enter %s, MUSB_STATUS_OK != dwStatus!", __func__);
#ifndef MUSB_CODE_SHRINK               
            MGC_UcosDestroyController(pSystem);
#endif
            //MUSB_MemFree(pSystem);
            free(pSystem);
            return NULL;
        }
              
		usb_dbg("###### enter:%s, after MUSB_StartController(), MGC_bSystemCount:%d\n", __func__, MGC_bSystemCount);

        //SPrintf(szPSbsr, "USB_%d", pControllerInfo->dwInfo);
        diag_printf("\n\n######################\n");
        diag_printf("enter  MGC_UcosInitController, before mbox creat and thread creat\n");
	 diag_printf("\n\n######################\n");
	 cyg_flag_init(&usbEvents);
	 //cyg_mbox_create(&pSystem->msgq_id, &usb_tsk_mbox );
		
	  /* Create the BSR task*/
	 cyg_thread_create(bBsrPriority, MGC_UcosBsrThread, (cyg_addrword_t)pSystem, "usb host", (void *)&usb_stack, USBSTACKSIZE, &usb_handle, &usb_thread_obj);					
	 cyg_thread_resume(usb_handle);
               
        pSystem->bThread = (MUSB_UCOS_TASK_PRIORITY + bBsrPriority) - MGC_bSystemCount;
    }

    return pSystem;
}

#ifdef MUSB_PORT_CONFIG
/*
 * Startup code calls this
 */
uint8_t MUSB_InitSystem(uint32_t dwBsrPriority)
{
    MGC_UcosSystem *pSystem;

    uint8_t bIndex;

    MGC_bSystemCount = 0;
    MUSB_MemSet(MGC_apUcosSystem, 0, sizeof(MGC_apUcosSystem));

    for (bIndex = 0; bIndex < (uint8_t)MUSB_MAX_CONTROLLERS; bIndex++)
    {
    	if(MGC_bUsbPortDef[bIndex])
	{
	    LOG(0, "USB Port %d is initial.\n", bIndex);
        pSystem = MGC_UcosInitController(&(MUSB_aUcosController[bIndex]), 
                            (uint8_t)(dwBsrPriority & 0xff));
        if (pSystem)
        {
            MGC_apUcosSystem[MGC_bSystemCount++ ] = pSystem;

            // turn on irq.
            MGC_UcosIrqUnlock(pSystem, 1);
        }
    }
    }

    return TRUE;
}
#else
/*
 * Startup code calls this
 */
uint8_t MUSB_InitSystem(uint32_t dwBsrPriority)
{
    MGC_UcosSystem *pSystem;
	MGC_AfsUds *pUds;

    uint8_t bIndex;
	usb_dbg("****** enter:%s, dwBsrPriority:%d\n", __func__, dwBsrPriority);
	
    MGC_bSystemCount = 0;
    //MUSB_MemSet(MGC_apUcosSystem, 0, sizeof(MGC_apUcosSystem));
	memset(MGC_apUcosSystem, 0, sizeof(MGC_apUcosSystem));

    for (bIndex = 0; bIndex < (uint8_t)MUSB_MAX_CONTROLLERS; bIndex++)
    {
	        pSystem = MGC_UcosInitController(&(MUSB_aUcosController[bIndex]), 
	                            (uint8_t)(dwBsrPriority & 0xff));
	        if (pSystem)
	        {
	            usb_dbg("****** after MGC_UcosInitController, MGC_bSystemCount:%d\n", MGC_bSystemCount);
				
	            MGC_apUcosSystem[MGC_bSystemCount ++] = pSystem;

	            // turn on irq.
	            //MGC_UcosIrqUnlock(pSystem, 1);
	            pUds = pSystem->pBoardPrivateData;
	            //cyg_interrupt_unmask(pUds->wIrq);
	            cyg_drv_interrupt_unmask(pUds->wIrq);
	        }
    }
    return TRUE;
}
#endif


#ifndef MUSB_CODE_SHRINK
void MUSB_DestroySystem(void)
{
    uint8_t bIndex;

	diag_printf("@@@@@@ enter:%s\n", __func__);

    for (bIndex = 0; bIndex < MGC_bSystemCount; bIndex++)
    {
        MGC_UcosDestroyController(MGC_apUcosSystem[bIndex]);
    }
}
#endif

uint32_t MUSB_Sleep(uint32_t dwTime)
{
    //x_thread_delay(dwTime);
	cyg_thread_delay(dwTime);

    return 0;
}

