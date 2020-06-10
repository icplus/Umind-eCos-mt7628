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
 * uC/OS target-specific code for any target supported by the ARM Firmware Suite
 * $Revision: #4 $
 */

#include "mu_cdi.h"
#include "mu_diag.h"
#include "mu_mem.h"
#include "mu_sys.h"
#include "brd_cnf.h"
#include "brd_main.h"
#include "mu_hdrdf.h"

#include "x_debug.h" //Jody
#include "x_assert.h" //Jody
//#include <cyg/kernel/kapi.h>//Jody
#include <cyg/hal/drv_api.h>//Jody add
//#include "ehci_def.h"//Jody
#include "ehci.h"//Jody
#include "ralink_usb.h"//Jody

/***************************** TYPES ******************************/
/**
 * MGC_AfsUds.
 * Board-specific UDS instance data.
 * @field pfIsr ISR
 * @field pIsrParam parameter to pass controller ISR
 * @field hTimerHdl: MTK OSAI handler. 
 * @field wIrq interrupt number
 * @field wTimerCount how many wrappers
 * @field bIndex our index into the global array
 */
typedef struct
{
    MUSB_pfUcosIsr pfIsr;
    void *pIsrParam;
    HANDLE_T hTimerHdl;    
    uint16_t wIrq;
    uint8_t bIndex;
} MGC_AfsUds;

extern int mt7620_ehci_init(void);//Jody


/*************************** FORWARDS *****************************/


/**************************** GLOBALS *****************************/

/** since AFS doesn't allow for instance data on ISRs or timer callbacks */
static MGC_AfsUds _arMGC_apAfsUds[MUSB_MAX_CONTROLLERS];

static cyg_handle_t counter;
static cyg_alarm alarm_s;

#if 0
static uint32_t MGC_Cmr1_RegValue;
static uint32_t MGC_Cmr2_RegValue;
#endif
//  add for debug memory allocation and free.
#ifdef MUSB_MEMDEBUG

MGC_AfsMem MGC_Mem[MGC_MEM_ALLOC_ITEM];
uint32_t MGC_Mem_Index;
uint32_t MGC_Mem_Size;

#endif


int MT7620_USB_interruptHandleState = 0;

/*************************** FUNCTIONS ****************************/
static void MGC_AfsUdsDsr(cyg_vector_t wInterruptNumber, cyg_ucount32 count, cyg_addrword_t data)
{
    uint8_t bIndex;
    MGC_AfsUds *pUds;
    int32_t result = -1;
    MT7620_USB_interruptHandleState = 1;

	//diag_printf("enter: MGC_AfsUdsDsr\n");
	
    if(ECOS_USBHost_State <= MTK_USBHost_StorageInit)
      diag_printf("######USB irq, enter:%s, wInterruptNumber:%d, MGC_bSystemCount:%d\n", __func__, wInterruptNumber,MGC_bSystemCount);
	
    for (bIndex = 0; bIndex < MGC_bSystemCount; bIndex++)
    {
        MUSB_ASSERT(bIndex < (uint8_t)MUSB_MAX_CONTROLLERS);
        if (bIndex < (uint8_t)MUSB_MAX_CONTROLLERS)
        {
            pUds = &_arMGC_apAfsUds[bIndex];
            if ((uint16_t)wInterruptNumber == pUds->wIrq)
            {
                result = pUds->pfIsr(pUds->pIsrParam);
				cyg_drv_interrupt_unmask(wInterruptNumber);
                return;
            }
        }
    }
}

static cyg_uint32 MGC_AfsUdsIsr(cyg_vector_t wInterruptNumber, cyg_addrword_t data)
{
    //diag_printf("\n\nenter: MGC_AfsUdsIsr, wInterruptNumber:%d\n", wInterruptNumber);
	
	cyg_drv_interrupt_mask(wInterruptNumber);
  	cyg_drv_interrupt_acknowledge(wInterruptNumber);
	
	return CYG_ISR_CALL_DSR;	/*  schedule DSR  */
}



#if 0
static void MUSB_Int_Disable(void* pBase)
{
    MUSB_ASSERT(pBase);

    MGC_Cmr1_RegValue = MGC_Read32(pBase, 0x04);
    MGC_Cmr2_RegValue = MGC_Read32(pBase, 0x08);
    MGC_Write32(pBase, 0x04, (MGC_Cmr1_RegValue & 0xff80ffff));
    MGC_Write32(pBase, 0x08, (MGC_Cmr2_RegValue & 0x00ffff01));
    return;
}

static void MUSB_Int_Enable(void* pBase)
{
    MGC_Write32(pBase, 0x04, MGC_Cmr1_RegValue);
    MGC_Write32(pBase, 0x08, MGC_Cmr2_RegValue);
    return;
}
#endif

static uint8_t MUSB_BoardSoftReset(void* pBase)
{
#if 0//Jody
    uint32_t u4Reg = 0;
    uint8_t nDevCtl = 0;
    MUSB_ASSERT(pBase);

    u4Reg = 0x00cfff00;
    IO_WRITE32(0xf0060200, 0, u4Reg);

    // VRT insert R enable 
    u4Reg = MGC_PHY_Read32(pBase,M_REG_PHYC0);
    u4Reg |= 0x4000;
    MGC_PHY_Write32(pBase, M_REG_PHYC0, u4Reg);

    //eye diagram improve   
    //TX_Current_EN setting 11 , 25:24 = 11 //force enable
    u4Reg = MGC_PHY_Read32(pBase, 0x1C);	
    u4Reg |= (uint32_t)0x01000000;	  
    MGC_PHY_Write32(pBase, 0x1C, u4Reg);

    //Soft Reset, RG_RESET for Soft RESET
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x68);
    u4Reg |=   0x00004000; 
    //MGC_PHY_Write32(pBase, (uint32_t)0x68, u4Reg);
    u4Reg &=  ~0x00004000; 
    MGC_PHY_Write32(pBase, (uint32_t)0x68, u4Reg);

    //otg bit setting
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x6C);
    u4Reg &= ~0x3f3f;
    u4Reg |=  0x3f2c;
    MGC_PHY_Write32(pBase, (uint32_t)0x6C, u4Reg);
 		
    //suspendom control
    u4Reg = MGC_PHY_Read32(pBase,(uint32_t)0x68);
    u4Reg &=  ~0x00040000; 
    MGC_PHY_Write32(pBase, (uint32_t)0x68, u4Reg);

    u4Reg = MGC_Read8(pBase,M_REG_PERFORMANCE3);
    u4Reg |=  0x80;
    u4Reg &= ~0x40;
    MGC_Write8(pBase, M_REG_PERFORMANCE3, (uint8_t)u4Reg);

    // MT5368/MT5396/MT5389 
    //0xf0050070[10]= 1 for Fs Hub + Ls Device 
    u4Reg = MGC_Read8(pBase, (uint32_t)0x71);
    u4Reg |= 0x04;
    MGC_Write8(pBase, 0x71, (uint8_t)u4Reg)

    // This is important: TM1_EN
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MGC_Read32(pBase,0x604);
    u4Reg |= 0x01;
    MGC_Write32(pBase, 0x604, u4Reg);
	
    // open session.
    nDevCtl = MGC_Read8(pBase, MGC_O_HDRC_DEVCTL);   
    nDevCtl |= MGC_M_DEVCTL_SESSION;        
    MGC_Write8(pBase, MGC_O_HDRC_DEVCTL, nDevCtl);

    // FS/LS Slew Rate change
    u4Reg = MGC_PHY_Read32(pBase, 0x10);
    u4Reg &= (uint32_t)(~0x000000ff);
    u4Reg |= (uint32_t)0x00000011;
    MGC_PHY_Write32(pBase, 0x10, u4Reg);

    // HS Slew Rate
    u4Reg = MGC_PHY_Read32(pBase, 0x10);
    u4Reg &= (uint32_t)(~0x00070000);
    u4Reg |= (uint32_t)0x00040000;
    MGC_PHY_Write32(pBase, 0x10, u4Reg);

    // This is important: TM1_EN
    // speed up OTG setting for checking device connect event after MUC_Initial().
    u4Reg = MGC_Read32(pBase, 0x604);
    u4Reg &= ~0x01;
    MGC_Write32(pBase, 0x604, u4Reg);

    LOG(5, "[USB]Nuclues pBase = 0x%08X init ok.\n", (uint32_t)pBase);
#endif
    return TRUE;
}

static cyg_interrupt USB_host_interrupt;
static cyg_handle_t  USB_host_interrupt_handle;

void *MUSB_BoardInitController(void *pPrivateData, MUSB_pfUcosIsr pfIsr,
                               const MUSB_UcosController *pControllerInfo, uint8_t **ppBaseIsr,
                               uint8_t **ppBaseBsr)
{
    MGC_AfsUds *pUds;
    VOID (*pfnOldIsr)(UINT16);

    MUSB_ASSERT(pControllerInfo);
    UNUSED(ppBaseIsr);       
    MUSB_ASSERT(ppBaseBsr);

	/*
    if (!MUSB_BoardSoftReset(*ppBaseBsr))
    {
        return NULL;
    }
      */
    //MUSB_Int_Disable(*ppBaseBsr);

	//enable port power.//Jody
	
    u32 temp = 0, hcs_params = 0;

	mt7620_ehci_init();
	usb_dbg("****** enter:%s, after:MT7620_ehci_init\n", __func__);
			
    //  change to static allocation.
    //MUSB_ASSERT(MGC_bSystemCount < (uint8_t)MUSB_MAX_CONTROLLERS);
    if (MGC_bSystemCount >= (uint8_t)MUSB_MAX_CONTROLLERS)
    {
        return NULL;
    }
    pUds = &_arMGC_apAfsUds[MGC_bSystemCount];

    //MUSB_MemSet(pUds, 0, sizeof(MGC_AfsUds));
    memset(pUds, 0, sizeof(MGC_AfsUds));
	
	usb_dbg("****** enter:%s, MGC_bSystemCount:%d\n", __func__, MGC_bSystemCount);
	if(!pControllerInfo)
		diag_printf("pControllerInfo == NULL\n");

    pUds->wIrq = pControllerInfo->dwInfo;
    pUds->pfIsr = pfIsr;
    pUds->pIsrParam = pPrivateData;
   
    /* assign the interrupt */
    //VERIFY(OSR_OK == x_reg_isr((UINT16)pUds->wIrq, MGC_AfsUdsIsr, &pfnOldIsr));
    //UNUSED(pfnOldIsr);

    //VERIFY(BIM_DisableIrq(pUds->wIrq));
    //MUSB_Int_Enable(*ppBaseBsr);

	//added by Jody
	/* Set up to handle interrupts */

	diag_printf("****** enter:%s, pUds->wIrq:%d\n", __func__, pControllerInfo->dwInfo);
	/*
	cyg_interrupt_create(pUds->wIrq,
                        0,
                        (cyg_addrword_t)24,
                        (cyg_ISR_t *)MGC_AfsUdsIsr,
                        (cyg_DSR_t *)0,
                        &USB_host_interrupt_handle,
                        &USB_host_interrupt);
	cyg_interrupt_attach(USB_host_interrupt_handle);
	cyg_interrupt_configure(pUds->wIrq, 1, 0);
       */   
    cyg_drv_interrupt_create(pUds->wIrq,
                        0,
                        (cyg_addrword_t)0,
                        (cyg_ISR_t *)&MGC_AfsUdsIsr,
                        (cyg_DSR_t *)&MGC_AfsUdsDsr,
                        &USB_host_interrupt_handle,
                        &USB_host_interrupt);
	cyg_drv_interrupt_attach(USB_host_interrupt_handle);
	cyg_drv_interrupt_configure(pUds->wIrq, 1, 0);
    
    
    pUds->bIndex = MGC_bSystemCount;
    return pUds;
}

uint8_t MUSB_BoardInitTimers(void *pPrivateData)
{
    MGC_AfsUds *pUds;
    
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;

    //VERIFY(OSR_OK == x_timer_create(&pUds->hTimerHdl));
    
   
    return TRUE;
}

#ifndef MUSB_CODE_SHRINK
void MUSB_BoardDestroyController(void *pPrivateData)
{
    UNUSED(pPrivateData);
}
#endif

uint8_t MUSB_BoardArmTimer(void *pPrivateData, uint16_t wIndex, uint32_t dwTime, uint8_t bPeriodic,
                           x_os_timer_cb_fct pfExpireCallback, void *pParam)
{
    MGC_AfsUds *pUds;
    TIMER_FLAG_T e_flags;

    UNUSED(wIndex);
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;

    usb_dbg("Timer:%d Start delay = %d ms.\n", wIndex, dwTime*10);
    //e_flags = (bPeriodic) ? X_TIMER_FLAG_REPEAT : X_TIMER_FLAG_ONCE;
    e_flags = (bPeriodic) ? dwTime : 0;//Jody
    //VERIFY(OSR_OK == cyg_timer_start(pUds->hTimerHdl, dwTime, e_flags, pfExpireCallback, pParam));
    //added by Jody

	cyg_clock_to_counter(cyg_real_time_clock(),&counter);
	cyg_alarm_create(counter,
			 pfExpireCallback,
			 (cyg_addrword_t)pParam,
			 &pUds->hTimerHdl,
			 &alarm_s);
	
	cyg_alarm_initialize(pUds->hTimerHdl, cyg_current_time()+dwTime, e_flags);
	cyg_alarm_enable(pUds->hTimerHdl);
	
    return TRUE;
}

void MUSB_BoardCancelTimer(void *pPrivateData, uint16_t wIndex)
{
    MGC_AfsUds *pUds;

    UNUSED(wIndex);
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;

    LOG(9, "Timer:%d Stop.\n", wIndex);
    
    //VERIFY(OSR_OK == x_timer_stop(pUds->hTimerHdl));
    cyg_alarm_delete(pUds->hTimerHdl);//Jody
}

/*
* Controller calls this to get a bus address (for DMA) from a system address
*/
void *MUSB_BoardSystemToBusAddress(const void *pSysAddr)
{
    return (void *)pSysAddr;
}

/*
* Controller calls this to enter a lock irq
*/
uint32_t MUSB_BoardIrqLock(void *pPrivateData)
{
    uint32_t u4Ret = 0;
    MGC_AfsUds* pUds;
    
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;

    // disable USB interrupt.
    //u4Ret = (UINT32)BIM_IsIrqEnabled(pUds->wIrq);//Jody

    if (u4Ret)
    {
        //VERIFY(BIM_DisableIrq(pUds->wIrq));//Jody
    }

    return u4Ret;
}

/*
* Controller calls this to enter a unlock irq
*/
void MUSB_BoardIrqUnlock(void *pPrivateData, uint32_t dwPrevIrq)
{
    MGC_AfsUds* pUds;
    
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;
   
    // interrupt is previous enable.
    if (dwPrevIrq)
    {
        // enable USB interrupt.
        //VERIFY(BIM_EnableIrq(pUds->wIrq));//Jody
    }
}

/*
* Controller calls this to get irq status.
*/
uint32_t MUSB_BoardIsIrqPending(void *pPrivateData)
{
    uint32_t u4Ret = 0;
    MGC_AfsUds* pUds;
    
    MUSB_ASSERT(pPrivateData);
    pUds = (MGC_AfsUds *)pPrivateData;

    // disable USB interrupt.
    //u4Ret = (UINT32)BIM_IsIrqPending(pUds->wIrq);//Jody

    return u4Ret;
}

