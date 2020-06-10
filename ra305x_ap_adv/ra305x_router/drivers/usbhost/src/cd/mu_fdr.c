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
 * MUSBStack-S FDRC implementation.
 * $Revision: #1 $
 */

#include "mu_impl.h"

#ifdef MUSB_FDRC

/*
 * Implementations of FDRC-specific functions
 */

#include "fdrc_cnf.h"

#include "mu_diag.h"

#include "mu_fdrpr.h"
#include "mu_fdrdf.h"
#include "mu_pippr.h"
#include "mu_funpr.h"
#include "mu_hstpr.h"

/*
 * Discover HDRC FIFO configuration
 */
uint8_t MGC_FdrcInit(MGC_Port* pPort)
{
#ifdef MUSB_C_DYNFIFO_DEF
    MGC_EndpointResource end;
    uint8_t bEnd;
    MGC_Controller* pImplController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pImplController->pControllerAddressIst;
#endif

#ifdef MUSB_C_DYNFIFO_DEF
    pPort->bEndCount = MUSB_C_NUM_EPS;
    pPort->wEndMask =  1;
    pPort->pPrivateData = MUSB_MemAlloc(pPort->bEndCount *
	sizeof(MGC_EndpointResource));
    if(pPort->pPrivateData)
    {
	if(MUSB_ArrayInit(&(pPort->LocalEnds), sizeof(MGC_EndpointResource), 
	    pPort->bEndCount, pPort->pPrivateData))
	{
	    /* add endpoint 0 */
	    MUSB_MemSet((void*)&end, 0, sizeof(end));
	    end.bIsFifoShared = TRUE;
	    end.bTrafficType = MUSB_ENDPOINT_XFER_CONTROL;
	    end.wMaxPacketSizeTx = MGC_END0_FIFOSIZE;
	    end.wMaxPacketSizeRx = MGC_END0_FIFOSIZE;
	    end.bIsClaimed = TRUE;
	    MUSB_ArrayAppend(&(pPort->LocalEnds), &end);
	    MGC_Write8(pBase, MGC_O_FDRC_INDEX, 0);
	    MGC_Write8(pBase, MGC_O_FDRC_RXFIFO2, 3 << 4);
	    MGC_Write8(pBase, MGC_O_FDRC_TXFIFO2, 3 << 4);
	    MGC_Write8(pBase, MGC_O_FDRC_RXFIFO1, 0);
	    MGC_Write8(pBase, MGC_O_FDRC_TXFIFO1, 0);

	    /* add others, but don't set sizes */
	    for(bEnd = 1; bEnd < (uint8_t)(pPort->bEndCount & 0xff); bEnd++)
	    {
		MUSB_MemSet((void*)&end, 0, sizeof(end));
		end.bLocalEnd = bEnd;
		MUSB_ArrayAppend(&(pPort->LocalEnds), &end);
		pPort->wEndMask |= (1 << bEnd);
	    }

	    return TRUE;
	}
	else
	{
	    MUSB_MemFree(pPort->pPrivateData);
	}
    }
    return FALSE;
#else
    return MGC_DrcInit(pPort);
#endif
}

void MGC_FdrcDynamicFifoLocation(MGC_Port* pPort, 
				 uint8_t bEnd, 
				 uint8_t bIsTx,
				 uint8_t bSet,
				 uint32_t* pdwBase, 
				 uint8_t* pbSize83, 
				 uint8_t* pbDoubleBuffered)
{
#ifdef MUSB_C_DYNFIFO_DEF
    uint8_t bVal;
    MGC_Controller* pImplController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pImplController->pControllerAddressIst;
    MGC_EndpointResource* pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), bEnd);

    MGC_Write8(pBase, MGC_O_FDRC_INDEX, bEnd);
    if(bSet)
    {
	/* set new location/size */
	bVal = *pbSize83 << MGC_S_FIFO_SZ;
	if(*pbDoubleBuffered)
	{
	    bVal |= MGC_M_FIFO_DPB;
	}
	bVal |= (uint8_t)((*pdwBase >> 11) & 0xf);
	if(bIsTx)
	{
	    pEnd->bTxFifoSize = bVal;
	    MGC_Write8(pBase, MGC_O_FDRC_TXFIFO2, bVal);
	    MGC_Write8(pBase, MGC_O_FDRC_TXFIFO1, (uint8_t)((*pdwBase >> 3) & 0xff));
	}
	else
	{
	    pEnd->bRxFifoSize = bVal;
	    MGC_Write8(pBase, MGC_O_FDRC_RXFIFO2, bVal);
	    MGC_Write8(pBase, MGC_O_FDRC_RXFIFO1, (uint8_t)((*pdwBase >> 3) & 0xff));
	}
    }
    else
    {
	/* get current location/size */
	if(bIsTx)
	{
	    bVal = pEnd->bTxFifoSize;
	    *pdwBase = MGC_Read8(pBase, MGC_O_FDRC_TXFIFO1) << 3;
	}
	else
	{
	    bVal = pEnd->bRxFifoSize;
	    *pdwBase = MGC_Read8(pBase, MGC_O_FDRC_RXFIFO1) << 3;
	}
	*pdwBase |= (bVal & 0xf) << 11;
	*pbSize83 = (bVal & MGC_M_FIFO_SZ) >> MGC_S_FIFO_SZ;
	*pbDoubleBuffered = (bVal & MGC_M_FIFO_DPB) ? TRUE : FALSE;
    }
#endif	/* dynamic FIFOs */
}

#ifdef MUSB_C_DYNFIFO_DEF
#define MGC_DFIFO_TOTAL (1 << (MUSB_C_RAM_BITS + 2))
#define MGC_DFIFO_HIGH (MGC_DFIFO_TOTAL >> 1)
MGC_EndpointResource* MGC_FdrcBindDynamicEndpoint(MGC_Port* pPort, 
						  const MUSB_DeviceEndpoint* pUsbEnd,
						  MUSB_EndpointResource* pRequest,
						  uint8_t bBind, uint8_t* pbIsTx)
{
    return MGC_AllocateDynamicFifo(pPort, pUsbEnd, bBind, pbIsTx,
	pPort->bEndCount, MGC_DFIFO_TOTAL, MGC_DFIFO_HIGH);
}
#endif	/* dynamic FIFOs */

/*
 * FDRC Interrupt Service Routine
 */
int32_t MGC_FdrcIsr(void* pParam)
{
    int32_t result;
    uint8_t bIntrUsbValue;
    uint16_t wIntrTxValue, wIntrRxValue;
    MUSB_Controller* pController = (MUSB_Controller*)pParam;
    MGC_Controller* pControllerImpl = (MGC_Controller*)pController->pPrivateData;
    uint8_t* pBase = (uint8_t*)pControllerImpl->pControllerAddressIst;
    /* save index */
    uint8_t bIndex = MGC_Read8(pBase, MGC_O_FDRC_INDEX);

    /* read registers */
    bIntrUsbValue = MGC_Read8(pBase, MGC_O_FDRC_INTRUSB);
    wIntrTxValue = MGC_Read8(pBase, MGC_O_FDRC_INTRTX1) | (MGC_Read8(pBase, MGC_O_FDRC_INTRTX2) << 8);
    wIntrRxValue = MGC_Read8(pBase, MGC_O_FDRC_INTRRX1) | (MGC_Read8(pBase, MGC_O_FDRC_INTRRX2) << 8);

    /* call common ISR */
    result = MGC_DrcIsr(pControllerImpl, bIntrUsbValue, wIntrTxValue, wIntrRxValue);

    /* restore index */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, bIndex);

    return result;
}

/*
 * Program the FDRC to start (enable interrupts, etc.).
 */
uint32_t MGC_FdrcStart(MGC_Controller* pController)
{
    /* Appu: Three new variables are added */
    uint8_t bIntrUsbValue;
    uint16_t wIntrTxValue, wIntrRxValue;

    uint16_t val;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    MGC_Port* pPort = pController->pPort;

    /*  Set INT enable registers */
    val = 0xffff & pPort->wEndMask;
    MGC_DIAG1(3, pController, "Setting IntrTxE to ", val, 16, 4);
    MGC_Write8(pBase, MGC_O_FDRC_INTRTX1E, (val & 0xff));
    MGC_Write8(pBase, MGC_O_FDRC_INTRTX2E, ((val >> 8) & 0xff));
    val = 0xfffe & pPort->wEndMask;
    MGC_DIAG1(3, pController, "Setting IntrRxE to ", val, 16, 4);
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, (val & 0xff));
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, ((val >> 8) & 0xff));
#ifdef MUSB_SCHEDULER
    /* TODO: enable SOF interrupts only when needed (maybe in ProgramBusState when the schedule is non-empty) */
    val = 0xff;
#else
    val = (uint16_t)~MGC_M_INTR_SOF;
#endif
    MGC_Write8(pBase, MGC_O_FDRC_INTRUSBE, (val & 0xff));
    MGC_DIAG1(3, pController, "Setting IntrUSBE to ", (val & 0xff), 16, 2);

    /* TODO: always set ISOUPDATE in POWER (periph mode) and leave it on! */
    pPort->bOtgState = (uint8_t)MUSB_AB_IDLE;

    /* Appu added this: These registers were not written. Let us try this */
    MGC_Write8(pBase, MGC_O_FDRC_FADDR, 0);
    MGC_Write8(pBase, MGC_O_FDRC_DEVCTL, 0);

    /*Appu : Changes as per 1.1: Clear Interrupt Register, Initialize all other registers */
    bIntrUsbValue = MGC_Read8(pBase, MGC_O_FDRC_INTRUSB);
    wIntrTxValue = MGC_Read8(pBase, MGC_O_FDRC_INTRTX1) | (MGC_Read8(pBase, MGC_O_FDRC_INTRTX2) << 8);
    wIntrRxValue = MGC_Read8(pBase, MGC_O_FDRC_INTRRX1) | (MGC_Read8(pBase, MGC_O_FDRC_INTRRX2) << 8);
    MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, MGC_M_TXCSR1_FLUSHFIFO | MGC_M_TXCSR1_CLRDATATOG);
    MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, 0);
    MGC_Write8(pBase, MGC_O_FDRC_TXMAXP, 8);
    MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, MGC_M_RXCSR1_FLUSHFIFO | MGC_M_RXCSR1_CLRDATATOG);
    MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, 0);
    MGC_Write8(pBase, MGC_O_FDRC_RXMAXP, 8);	

    return 0;
}

/*
 * Program the FDRC to stop (disable interrupts, etc.).
 */
uint32_t MGC_FdrcStop(MGC_Controller* pController)
{
    uint8_t temp;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    /* disable interrupts */
    MGC_Write8(pBase, MGC_O_FDRC_INTRUSBE, 0);
    MGC_Write8(pBase, MGC_O_FDRC_INTRTX1E, 0);
    MGC_Write8(pBase, MGC_O_FDRC_INTRTX2E, 0);
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, 0);
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, 0);

    /*  flush pending interrupts */
    temp = MGC_Read8(pBase, MGC_O_FDRC_INTRUSB);
    temp = MGC_Read8(pBase, MGC_O_FDRC_INTRTX1);
    temp = MGC_Read8(pBase, MGC_O_FDRC_INTRTX2);
    temp = MGC_Read8(pBase, MGC_O_FDRC_INTRRX1);
    temp = MGC_Read8(pBase, MGC_O_FDRC_INTRRX2);

    return 0;
}

uint32_t MGC_FdrcDestroy(MGC_Controller* pController)
{
    MGC_Port* pPort = pController->pPort;

    VERIFY(MGC_HostDestroy(pPort));
    if(pPort->pPrivateData)
    {
	MUSB_MemFree(pPort->pPrivateData);
    }
    return MGC_FunctionDestroy(pPort);
}

/* Fill the bus state info from the controller */
uint32_t MGC_FdrcReadBusState(MGC_Port* pPort)
{
    uint32_t temp;
    uint8_t devctl;
    uint8_t power;
    uint8_t vbus;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    /* bConnectorId, bIsSession, bIsHost, bIsLowSpeed */
    devctl = MGC_Read8(pBase, MGC_O_FDRC_DEVCTL);
    MGC_DIAG1(3, pController, "DevCtl=", devctl, 16, 2);
    if(devctl & MGC_M_DEVCTL_SESSION)
    {
	pPort->bConnectorId = (devctl & MGC_M_DEVCTL_BDEVICE) >> 7;
    }
    else
    {
	pPort->bConnectorId = MGC_CID_UNKNOWN;
    }
    pPort->bIsSession = (devctl & MGC_M_DEVCTL_SESSION) ? TRUE : FALSE;
    pPort->bIsHost = (devctl & MGC_M_DEVCTL_HM) ? TRUE : FALSE;
    pPort->bIsLowSpeed = (devctl & MGC_M_DEVCTL_LSDEV) ? TRUE : FALSE;

    /* bIsSuspend, bIsReset, bIsResume, bIsSession, bBusVoltage */
    power = MGC_Read8(pBase, MGC_O_FDRC_POWER);
    MGC_DIAG1(3, pController, "POWER=", power, 16, 2);
    pPort->bIsSuspend = (power & MGC_M_POWER_SUSPENDM) ? TRUE : FALSE;
    pPort->bIsReset = (power & MGC_M_POWER_RESET) ? TRUE : FALSE;
    pPort->bIsResume = (power & MGC_M_POWER_RESUME) ? TRUE : FALSE;
    vbus = (power & MGC_VBUS_MASK) >> 4;
    switch (vbus)
    {
    case 0:
	pPort->bBusVoltage = MGC_VBUS_BELOW_SESSION_END;
	break;
    case 1:
	pPort->bBusVoltage = MGC_VBUS_ABOVE_SESSION_END;
	break;
    case 3:
	pPort->bBusVoltage = MGC_VBUS_ABOVE_AVALID;
	break;
    case 7:
	pPort->bBusVoltage = MGC_VBUS_ABOVE_VBUS_VALID;
	break;
    default:
	/* TODO: shouldn't there be other valid cases? */
	pPort->bBusVoltage = MGC_VBUS_ERROR;
    }

    /* dwFrame */
    temp = MGC_Read8(pBase, MGC_O_FDRC_FRAME2);
    temp = (temp << 8) + MGC_Read8(pBase, MGC_O_FDRC_FRAME1);
    /* if some one is using this, then have protection for updating it */
    pPort->dwFrame = temp;

    return 0;
}

/* Program the bus state from the port (see MGC_Port) */
uint32_t MGC_FdrcProgramBusState(MGC_Port* pPort)
{
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    uint8_t power = MGC_Read8(pBase, MGC_O_FDRC_POWER);
    uint8_t nPower = power;
    uint8_t devctl = MGC_Read8(pBase, MGC_O_FDRC_DEVCTL);
    uint8_t nDevCtl = devctl;
    uint8_t bFuncAddr = MGC_Read8(pBase, MGC_O_FDRC_FADDR);

    /* reset? */
    if(pPort->bWantReset)
    {
	nPower |= MGC_M_POWER_RESET;
    }
    else
    {
	nPower &= ~MGC_M_POWER_RESET;
    }

    /* suspend? */
    if(pPort->bWantSuspend)
    {
	if(pPort->bIsHost)
	{
	    nPower |= MGC_M_POWER_SUSPENDM;
	}
	else
	{
	    nPower |= MGC_M_POWER_ENSUSPEND;
	}
    }
    else
    {
	nPower &= ~MGC_M_POWER_SUSPENDM;
	nPower &= ~MGC_M_POWER_ENSUSPEND;
    }

    /* session? */
    if(pPort->bWantSession)
    {
	nDevCtl |= MGC_M_DEVCTL_SESSION;
    }
    else
    {
	nDevCtl &= ~MGC_M_DEVCTL_SESSION;
    }

    /* host? */
    if(pPort->bWantHost)
    {
	nDevCtl |= MGC_M_DEVCTL_HR;
    }
    else
    {
	nDevCtl &= ~MGC_M_DEVCTL_HR;
    }

    /* make it so, but only on changes */
    if(pPort->bFuncAddr != bFuncAddr)
    {
	MGC_DIAG1(3, pController, "Updating FADDR to ", pPort->bFuncAddr, 10, 0);
	MGC_Write8(pBase, MGC_O_FDRC_FADDR, pPort->bFuncAddr);
    }
    if(nPower != power)
    {
	MGC_DIAG1(3, pController, "Updating POWER to ", nPower, 16, 2);
	MGC_Write8(pBase, MGC_O_FDRC_POWER, nPower);
    }
    if(nDevCtl != devctl)
    {
	MGC_DIAG1(3, pController, "Updating DevCtl to ", nDevCtl, 16, 2);
	MGC_Write8(pBase, MGC_O_FDRC_DEVCTL, nDevCtl);
    }

    return 0;
}

/*
* Find the FDRC's first (host) or required (function) 
* local endpoint resource to communicate with the given remote endpoint (host)
* or to be identified with the given endpoint descriptor (function).
*/
MGC_EndpointResource* MGC_FdrcBindEndpoint(MGC_Port* pPort, 
					   const MUSB_DeviceEndpoint* pUsbEnd,
					   MUSB_EndpointResource* pRequest, 
					   uint8_t bBind)
{
    uint8_t bInterval;
    uint8_t csr2, bEnd, bIsTx, bTrafficType;
    uint8_t reg = 0;
    MGC_EndpointResource* pEnd;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

#ifdef MUSB_C_DYNFIFO_DEF
    pEnd = MGC_FdrcBindDynamicEndpoint(pPort, pUsbEnd, pRequest, bBind, &bIsTx);
#else
    pEnd = MGC_DrcBindEndpoint(pPort, pUsbEnd, pRequest, bBind, &bIsTx);
#endif
    if(pEnd && bBind)
    {
	/* prepare endpoint registers according to flags */
	bTrafficType = bIsTx ? pEnd->bTrafficType : pEnd->bRxTrafficType;
	bEnd = pEnd->bLocalEnd;
	MGC_Write8(pBase, MGC_O_FDRC_INDEX, bEnd);

	/* proto reg */
	reg |= bIsTx ? (pEnd->bBusEnd & MUSB_ENDPOINT_NUMBER_MASK) : 
	(pEnd->bRxBusEnd & MUSB_ENDPOINT_NUMBER_MASK);
	if(bEnd)
	{
	    reg |= bTrafficType << 4;
	}
	if(bIsTx)
	{
	    /* transmit */
	    if(pPort->bIsHost)
	    {
		/* protocol/endpoint/interval/NAKlimit */
		if(bEnd)
		{
		    MGC_Write8(pBase, MGC_O_FDRC_TXTYPE, reg);
		    /* 
		     * TODO: at least for bulk, setting the upper 4 interval bits causes the core 
		     * to continuously assert Rx interrupts with RxError set
		     */
		    bInterval = MUSB_MIN(pUsbEnd->UsbDescriptor.bInterval, 0xff);
		    if(MUSB_ENDPOINT_XFER_BULK == bTrafficType)
		    {
			/* for bulk, use NAK limit and always use some limit */
			bInterval = MUSB_MIN(pUsbEnd->wNakLimit, 0xff);
			bInterval = MUSB_MAX(bInterval, 2);
			if(0xffff == pUsbEnd->wNakLimit)
			{
			    bInterval = 0;
			}
		    }
		    MGC_Write8(pBase, MGC_O_FDRC_TXINTERVAL, bInterval);
		}
		else
		{
		    MGC_Write8(pBase, MGC_O_FDRC_NAKLIMIT0, 0xff);
		}
	    }

	    /* CSR */
	    if(bEnd)
	    {
		/* packet size */
		MGC_Write8(pBase, MGC_O_FDRC_TXMAXP, (uint8_t)(pEnd->wPacketSize >> 3));
		csr2 = MGC_M_TXCSR2_MODE;
#ifdef MUSB_ISO
		if(!pPort->bIsHost && (pEnd->bTrafficType == MUSB_ENDPOINT_XFER_ISOC))
		{
		    csr2 |= MGC_M_TXCSR2_ISO;
		}
#endif
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, csr2);
		/* flush twice in case of double packet buffering */
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, 
		    (MGC_M_TXCSR1_FLUSHFIFO | MGC_M_TXCSR1_CLRDATATOG));
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, 
		    (MGC_M_TXCSR1_FLUSHFIFO | MGC_M_TXCSR1_CLRDATATOG));
	    }
	}
	else
	{
	    /* receive */
	    if(pPort->bIsHost)
	    {
		/* protocol/endpoint/interval/NAKlimit */
		if(bEnd)
		{
		    MGC_Write8(pBase, MGC_O_FDRC_RXTYPE, reg);
		    /* 
		     * TODO: at least for bulk, setting the upper 4 interval bits causes the core 
		     * to continuously assert Rx interrupts with RxError set
		     */
		    bInterval = MUSB_MIN(pUsbEnd->UsbDescriptor.bInterval, 0xff);
		    if(MUSB_ENDPOINT_XFER_BULK == bTrafficType)
		    {
			/* for bulk, use NAK limit and always use some limit */
			bInterval = MUSB_MIN(pUsbEnd->wNakLimit, 0xff);
			bInterval = MUSB_MAX(bInterval, 2);
			if(0xffff == pUsbEnd->wNakLimit)
			{
			    bInterval = 0;
			}
		    }
		    MGC_Write8(pBase, MGC_O_FDRC_RXINTERVAL, bInterval);
		}
		else
		{
		    MGC_Write8(pBase, MGC_O_FDRC_NAKLIMIT0, 0xff);
		}
	    }

	    /* CSR */
	    if(bEnd)
	    {
		csr2 = 0;
		/* packet size */
		MGC_Write8(pBase, MGC_O_FDRC_RXMAXP, (uint8_t)(pEnd->wRxPacketSize >> 3));
		if(pPort->bIsHost)
		{
		    csr2 = 0;
		}
		else
		{
#ifdef MUSB_ISO
		    if(bTrafficType == MUSB_ENDPOINT_XFER_ISOC)
		    {
			csr2 = MGC_M_RXCSR2_P_ISO;
		    }
		    csr2 |= MGC_M_RXCSR2_AUTOCLEAR;
#endif
		}
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, 0);/* mode bit=0 for RX */
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, csr2);
		/* twice in case of double packet buffering */
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, MGC_M_RXCSR1_CLRDATATOG);
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, MGC_M_RXCSR1_CLRDATATOG);
	    }
	}
    }
    return pEnd;
}

/*
 * Program the FDRC to initiate reception.
 */
uint32_t MGC_FdrcStartRx(MGC_Port* pPort, MGC_EndpointResource* pEnd, 
			 uint8_t* pBuffer, uint32_t dwTotalBytes, 
			 void* pIrp, uint8_t bAllowDma)
{
#ifdef MUSB_DMA
    MUSB_DmaController* pDmaController;
    MUSB_DmaChannel* pDmaChannel;
    uint8_t bDmaOk;
#endif
    uint16_t wRxCount;
    MGC_BsrItem qItem;
    uint8_t bResult;
    uint8_t bCsr1, bCsr2, reg, bIntrRxE1, bIntrRxE2;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    MUSB_SystemServices* pServices = pController->pSystemServices;
    uint8_t bEnd = pEnd->bLocalEnd;

    /* select hw ep */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, bEnd);

    /* clear mode bit in TxCSR2 */
    bCsr2 = MGC_Read8(pBase, MGC_O_FDRC_TXCSR2);
    if((bCsr2 & MGC_M_TXCSR2_MODE) && pEnd->bIsFifoShared)
    {
	bCsr2  &= ~MGC_M_TXCSR2_MODE;
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, bCsr2);
    }

    /* read RxCSRs for preparation */
    bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
    bCsr2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);

    /* prepare for the non-DMA case */
    bCsr2 &= ~(MGC_M_RXCSR2_DMAENAB | MGC_M_RXCSR2_DMAMODE);
    if(pPort->bIsHost)
    {
	bCsr1 |= MGC_M_RXCSR1_H_REQPKT;
	bCsr1 &= ~MGC_M_RXCSR1_RXPKTRDY;

	/* write protocol/endpoint */
	reg = pEnd->bRxTrafficType << MGC_S_TYPE_PROTO;
	reg |= pEnd->bRxBusEnd & MGC_M_TYPE_REMOTE_END;
	MGC_Write8(pBase, MGC_O_FDRC_RXTYPE, reg);
    }

    pEnd->bBsrResidual = pEnd->bIsrResidual;

    /* disable endpoint interrupt */
    bIntrRxE1 = MGC_Read8(pBase, MGC_O_FDRC_INTRRX1E);
    bIntrRxE2 = MGC_Read8(pBase, MGC_O_FDRC_INTRRX2E);
    if(bEnd < 8)
    {
	MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, bIntrRxE1 & ~(1 << bEnd));
    }
    else
    {
	MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, bIntrRxE2 & ~(1 << (bEnd - 8)));
    }

    if(!pPort->bIsHost)
    {
	/* handle residual if any */
	if(MGC_Read8(pBase, MGC_O_FDRC_RXCSR1) & MGC_M_RXCSR1_RXPKTRDY)
	{
	    /* poll until IRP is complete, since handing it to the ISR will always have races */
	    while(TRUE)
	    {
		if(bCsr1 & MGC_M_RXCSR1_RXPKTRDY)
		{
		    wRxCount = MGC_ReadRxCount(pController);
		    MUSB_DIAG1(3, "StartRx: residual byte count=", wRxCount, 16, 0);
#ifdef MUSB_DMA
		    pDmaController = pPort->pDmaController;
		    pDmaChannel = pEnd->pRxDmaChannel;
		    if(!bAllowDma && pDmaChannel)
		    {
			/* release previously-allocated channel */
			pDmaController->pfDmaReleaseChannel(pDmaChannel);
			pEnd->pRxDmaChannel = NULL;
		    }
		    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
		    {
			/* candidate for DMA */
			if(pDmaController && !pDmaChannel)
			{
			    pDmaChannel = pEnd->pRxDmaChannel = pDmaController->pfDmaAllocateChannel(
				pDmaController->pPrivateData, bEnd, FALSE, 
				pEnd->bRxTrafficType, pEnd->wRxPacketSize);
			}
			if(pDmaChannel)
			{
			    pDmaChannel->dwActualLength = 0L;
			    pEnd->bDmaTx = FALSE;

			    /*
			    * Speculate successful DMA programming and clear these fields if not.
			    * This is because although third-party DMACs obey the interface, 
			    * the Inventra DMAC in some cases will immediately activate, finish,
			    * and interrupt us before the RxCSR write below, and these fields
			    * are needed by its ISR.
			    */
			    pEnd->bDmaResidual = TRUE;
			    pEnd->pRxIrp = pIrp;

			    bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, 
				pEnd->wRxPacketSize, 0, pBuffer, 
				MGC_MIN(wRxCount, pDmaChannel->dwMaxLength));
			    if(bDmaOk)
			    {
				if(((volatile MGC_EndpointResource*)pEnd)->pRxIrp)
				{
				    /* external DMAC; need to assert request line */
				    bCsr2 = MGC_Read8(pBase, MGC_O_FDRC_TXCSR2);
				    bCsr2 |= (MGC_M_RXCSR2_AUTOCLEAR | MGC_M_RXCSR2_DMAENAB);
				    /* this special sequence is required to get DMAReq to activate */
				    MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, bCsr2 | MGC_M_RXCSR2_DMAMODE);
				    MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, bCsr2);
				}
				return 0;
			    }
			    else
			    {
				pEnd->bDmaResidual = FALSE;
				pEnd->pRxIrp = NULL;
				pDmaController->pfDmaReleaseChannel(pDmaChannel);
				pEnd->pRxDmaChannel = NULL;
			    }
			}
		    }
#endif
		    bResult = MGC_PipePacketReceived(pPort, pEnd, wRxCount, 
			FALSE, pIrp);

		    /* clear recv condition if necessary */
		    bCsr1 &= ~MGC_M_RXCSR1_RXPKTRDY;
		    if((wRxCount < pEnd->wRxPacketSize) || !(bCsr2 & MGC_M_RXCSR2_AUTOCLEAR))
		    {
			MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bCsr1);
		    }
		    /* complete IRP if necessary */
		    if(bResult)
		    {
			if(MGC_CompleteIrp(&qItem, pEnd, (uint8_t)MUSB_STATUS_OK, FALSE, pIrp))
			{
			    VERIFY(pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem));
			}
			MGC_DIAG(3, pPort->pController, "IRP Complete - StartRx]");
			/* re-enable interrupt */
			MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, bIntrRxE1);
			MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, bIntrRxE2);
			return 0;
		    }
		}

		/* in the non-DMA case we may be slow enough that another packet is ready now */
		bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);

	    }
	}
    }

    /* no residual; set current IRP */
    pEnd->pRxIrp = pIrp;

#ifdef MUSB_DMA
    pDmaController = pPort->pDmaController;
    pDmaChannel = pEnd->pRxDmaChannel;
    if(!bAllowDma && pDmaChannel)
    {
	/* release previously-allocated channel */
	pDmaController->pfDmaReleaseChannel(pDmaChannel);
	pEnd->pRxDmaChannel = NULL;
    }
    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
    {
	/* candidate for DMA */
	if(pDmaController && !pDmaChannel)
	{
	    pDmaChannel = pEnd->pRxDmaChannel = pDmaController->pfDmaAllocateChannel(
		pDmaController->pPrivateData, bEnd, FALSE, 
		pEnd->bRxTrafficType, pEnd->wRxPacketSize);
	}
	if(pDmaChannel)
	{
	    pEnd->bDmaTx = FALSE;
	    pDmaChannel->dwActualLength = 0L;
	    /* Program DMA channel */
	    bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, 
		pEnd->wRxPacketSize, pDmaChannel->bDesiredMode, pBuffer, 
		MGC_MIN(dwTotalBytes, pDmaChannel->dwMaxLength));
	    if(bDmaOk)
	    {
		/* DMA channel is ready; finish RxCSR programming */
		if(pPort->bIsHost)
		{
		    bCsr1 &= ~MGC_M_RXCSR1_H_REQPKT;
		    bCsr2 |= MGC_M_RXCSR2_H_AUTOREQ;
		}
		bCsr2 |= (MGC_M_RXCSR2_AUTOCLEAR | MGC_M_RXCSR2_DMAENAB | 
		    (pDmaChannel->bDesiredMode ? MGC_M_RXCSR2_DMAMODE : 0));
	    }
	    else
	    {
		pDmaController->pfDmaReleaseChannel(pDmaChannel);
		pEnd->pDmaChannel = NULL;
	    }
	}
    }
#endif

    /* write RxCSRs */
    if(pPort->bIsHost)
    {
	MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bCsr1);
    }
    MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, bCsr2);

#ifdef MUSB_DMA
    if(bAllowDma && pDmaChannel && bDmaOk)
    {
	MGC_DIAG(3, pPort->pController, "StartRx]");
	return 0;
    }
#endif

    /* re-enable interrupt */
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, bIntrRxE1);
    MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, bIntrRxE2);

    MGC_DIAG(3, pPort->pController, "StartRx]");

    return 0;
}

/*
 * Program the FDRC to initiate transmit.
 */
uint32_t MGC_FdrcStartTx(MGC_Port* pPort, MGC_EndpointResource* pEnd, 
			 const uint8_t* pBuffer, uint32_t dwTotalBytes,
			 void* pGenIrp)
{
#ifdef MUSB_DMA
    MUSB_DmaController* pDmaController;
    MUSB_DmaChannel* pDmaChannel;
    uint8_t bDmaOk;
#endif
    uint32_t dwLoadCount;
    uint8_t reg;
    uint8_t bCsr1 = 0;
    uint8_t bCsr2 = 0;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
#ifdef MUSB_DMA
    uint8_t bAllowDma = FALSE;
    MUSB_Irp* pIrp = NULL;
    MUSB_IsochIrp* pIsochIrp = NULL;
    MUSB_ControlIrp* pControlIrp = NULL;
    MUSB_SystemServices* pServices = pController->pSystemServices;
#endif
    uint8_t bEnd = pEnd->bLocalEnd;

    /* select hw ep */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, bEnd);

    if(pPort->bIsHost && bEnd)
    {
	/* host mode; write protocol/endpoint */
	reg = pEnd->bTrafficType << MGC_S_TYPE_PROTO;
	reg |= pEnd->bBusEnd & MGC_M_TYPE_REMOTE_END;
	MGC_Write8(pBase, MGC_O_FDRC_TXTYPE, reg);
    }

    /* determine how much to load into FIFO for non-DMA case */
    dwLoadCount = MGC_MIN(pEnd->wPacketSize, dwTotalBytes);

    if(bEnd)
    {
	/* read CSR for preparation */
	bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_TXCSR1) & ~MGC_M_TXCSR1_P_UNDERRUN;
	bCsr2 = MGC_Read8(pBase, MGC_O_FDRC_TXCSR2);
    }
    else
    {
	/* ensure FIFO ready for writing */
	MGC_Write8(pBase, MGC_O_FDRC_CSR0, 0);
    }
    
#ifdef MUSB_DMA
    switch(pEnd->bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_ISOC:
	pIsochIrp = (MUSB_IsochIrp*)pGenIrp;
	bAllowDma = pIsochIrp->bAllowDma;
	break;
    case MUSB_ENDPOINT_XFER_CONTROL:
	pControlIrp = (MUSB_ControlIrp*)pGenIrp;
	bAllowDma = pControlIrp->bAllowDma;
	break;
    default:
	pIrp = (MUSB_Irp*)pGenIrp;
	bAllowDma = pIrp->bAllowDma;
    }
    pDmaController = pPort->pDmaController;
    pDmaChannel = pEnd->pDmaChannel;
    if(!bAllowDma && pDmaChannel)
    {
	/* release previously-allocated channel */
	pDmaController->pfDmaReleaseChannel(pDmaChannel);
	pEnd->pDmaChannel = NULL;
    }
    else if(bAllowDma && pServices->pfSystemToBusAddress(pServices->pPrivateData, pBuffer))
    {
	/* candidate for DMA */
	if(pDmaController && !pDmaChannel)
	{
	    pDmaChannel = pEnd->pDmaChannel = pDmaController->pfDmaAllocateChannel(
		pDmaController->pPrivateData, bEnd, TRUE, 
		pEnd->bTrafficType, pEnd->wPacketSize);
	}
	if(pDmaChannel)
	{
	    bDmaOk = pDmaController->pfDmaProgramChannel(pDmaChannel, 
		pEnd->wPacketSize, pDmaChannel->bDesiredMode, pBuffer, 
		MGC_MIN(dwTotalBytes, pDmaChannel->dwMaxLength));
	    if(bDmaOk)
	    {
		pEnd->bDmaTx = TRUE;
		pDmaChannel->dwActualLength = 0L;
		dwLoadCount = 0;
		if(bEnd)
		{
		    bCsr2 |= (MGC_M_TXCSR2_AUTOSET | MGC_M_TXCSR2_DMAENAB | 
			(pDmaChannel->bDesiredMode ? MGC_M_TXCSR2_DMAMODE : 0));
		}
	    }
	    else
	    {
		pDmaController->pfDmaReleaseChannel(pDmaChannel);
		pEnd->pDmaChannel = NULL;
	    }
	}
    }
    if(!bDmaOk)
    {
#endif
	/* load FIFO */
	pPort->pfLoadFifo(pPort, pEnd->bLocalEnd, dwLoadCount, pBuffer);
	pEnd->dwTxSize = dwLoadCount;
	/* prepare CSR */
	if(bEnd)
	{
	    /* since we cannot clear DMAMODE before/during clearing DMAENAB, never clear DMAMODE */
	    bCsr2 &= ~(MGC_M_TXCSR2_AUTOSET | MGC_M_TXCSR2_DMAENAB);
	    bCsr1 |= MGC_M_TXCSR1_TXPKTRDY;
	}
	else
	{
	    /* endpoint 0 is a fixed case */
	    bCsr1 = MGC_M_CSR0_H_SETUPPKT | MGC_M_CSR0_TXPKTRDY;
	}
#ifdef MUSB_DMA
    }
#endif

    /* write CSRs */
    pEnd->pTxIrp = pGenIrp;
    if(bEnd)
    {
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, bCsr1);
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, bCsr2 | MGC_M_TXCSR2_MODE);
    }
    else
    {
	MGC_Write8(pBase, MGC_O_FDRC_CSR0, bCsr1);
    }

    return 0;
}

/* Program the FDRC to flush the given endpoint wrt the given USB direction */
uint32_t MGC_FdrcFlushEndpoint(MGC_Port* pPort, MGC_EndpointResource* pEnd, 
			       uint8_t bDir, uint8_t bReuse)
{
    uint8_t bCsr1;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    uint8_t bTx = FALSE;

    /* select hw ep */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, pEnd->bLocalEnd);

    /* determine TX/RX */
    if(bDir & MUSB_DIR_IN)
    {
	if(!pPort->bIsHost)
	{
	    bTx = TRUE;
	}
    }
    else
    {
	if(pPort->bIsHost)
	{
	    bTx = TRUE;
	}
    }

    if(bTx)
    {
#ifdef MUSB_DMA
	if(pEnd->pDmaChannel && pEnd->bDmaTx)
	{
	    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
	    pEnd->pDmaChannel = NULL;
	}
#endif
	/*  flush the xmit fifo, reset the data tog to 0 */
	MGC_DIAG1(2, pController, "Flushing Tx on ", pEnd->bLocalEnd, 16, 2);
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, 
	    (MGC_M_TXCSR1_FLUSHFIFO | MGC_M_TXCSR1_CLRDATATOG));
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, MGC_M_TXCSR1_FLUSHFIFO);
    }
    else
    {
	if(pPort->bIsHost)
	{
	    if(pEnd->bLocalEnd)
	    {
		bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
		bCsr1 &= ~MGC_M_RXCSR1_H_REQPKT;
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bCsr1);
	    }
	    else
	    {
		bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_CSR0);
		bCsr1 &= ~MGC_M_CSR0_H_REQPKT;
		MGC_Write8(pBase, MGC_O_FDRC_CSR0, bCsr1);
	    }
	}
#ifdef MUSB_DMA
	if(pEnd->pRxDmaChannel && !pEnd->bDmaTx)
	{
	    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
	    pEnd->pRxDmaChannel = NULL;
	}
#endif
	/*  flush the recv fifo, reset the data tog to 0 */
	MGC_DIAG1(2, pController, "Flushing Rx on ", pEnd->bLocalEnd, 16, 2);
	MGC_Write8(pBase, MGC_O_FDRC_RXCSR1,
	    (MGC_M_RXCSR1_FLUSHFIFO | MGC_M_RXCSR1_CLRDATATOG));
	MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, MGC_M_RXCSR1_FLUSHFIFO);
    }

#ifdef MUSB_C_DYNFIFO_DEF
    if(bReuse)
    {
	pEnd->wMaxPacketSizeTx = pEnd->wMaxPacketSizeRx = 0;
    }
#endif

    return 0;
}

/* Program the FDRC to set/clear the halting (stall) of an endpoint */
uint32_t MGC_FdrcHaltEndpoint(MGC_Port* pPort, MGC_EndpointResource* pEnd, 
			      uint8_t bDir, uint8_t bHalt)
{
    uint8_t val, val2;
    uint8_t bTx = FALSE;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    /* determine TX/RX */
    if(bDir & MUSB_DIR_IN)
    {
	if(!pPort->bIsHost)
	{
	    bTx = TRUE;
	}
    }
    else
    {
	if(pPort->bIsHost)
	{
	    bTx = TRUE;
	}
    }

    MGC_Write8(pBase, MGC_O_FDRC_INDEX, pEnd->bLocalEnd);
    
    if(bTx)
    {
	/* Tx */
	val = MGC_Read8(pBase, MGC_O_FDRC_TXCSR1);
	if(bHalt)
	{
	    if(pPort->bIsHost)
	    {
		/* mark for stopping */
		pEnd->bStopTx = TRUE;
		if(!pEnd->pTxIrp)
		{
		    /* if already stopped, indicate it */
		    pEnd->bIsHalted = TRUE;
		}
	    }
	    else
	    {
		val &= ~MGC_M_TXCSR1_P_UNDERRUN;
		val |= MGC_M_TXCSR1_P_SENDSTALL;
	    }
	}
	else
	{
	    if(pPort->bIsHost)
	    {
		/* restart Tx traffic */
		pEnd->bIsHalted = FALSE;
		/* start next IRP (if any) */
		MGC_StartNextIrp(pPort, pEnd, TRUE);
	    }
	    else
	    {
		val &= ~(MGC_M_TXCSR1_P_SENDSTALL | MGC_M_TXCSR1_P_UNDERRUN);
		/* apparently the hardware is not doing this: */
		val |= MGC_M_TXCSR1_CLRDATATOG;
	    }
	}
	MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, val);
    }
    else
    {
	/* Rx */
	val = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
	if(bHalt)
	{
	    if(pPort->bIsHost)
	    {
		/* stop Rx traffic */
		pEnd->bIsRxHalted = TRUE;
		val2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);
		val2 &= ~(MGC_M_RXCSR2_H_AUTOREQ | MGC_M_RXCSR2_DMAENAB);
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, val2);
		val &= ~MGC_M_RXCSR1_H_REQPKT;
	    }
	    else
	    {
		val |= MGC_M_RXCSR1_P_SENDSTALL;
	    }
	}
	else
	{
	    if(pPort->bIsHost)
	    {
		/* restart Rx traffic */
		pEnd->bIsRxHalted = FALSE;
		if(pEnd->pRxIrp)
		{
		    /* restart pending IRP */
#ifdef MUSB_DMA
		    if(pEnd->pRxDmaChannel && !pEnd->bDmaTx)
		    {
			val2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);
			MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, 
			    val2 | (MGC_M_RXCSR2_H_AUTOREQ | MGC_M_RXCSR2_DMAENAB));
		    }
		    else
		    {
			val |=  MGC_M_RXCSR1_H_REQPKT;
		    }
#else
		    val |=  MGC_M_RXCSR1_H_REQPKT;
#endif
		}
		else
		{
		    /* start next IRP (if any) */
		    MGC_StartNextIrp(pPort, pEnd, FALSE);
		}
	    }
	    else
	    {
		val &= ~MGC_M_RXCSR1_P_SENDSTALL;
		/* apparently the hardware is not doing this: */
		val |= MGC_M_RXCSR1_CLRDATATOG;
	    }
	}
	MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, val);
    }

    return 0;
}

/*
* Respond on default endpoint
*/
uint32_t MGC_FdrcDefaultEndResponse(MGC_Port* pPort, uint8_t bStall)
{
    uint16_t wSize;
    uint8_t bCsrVal;
    uint8_t bVal = MGC_M_CSR0_P_SVDRXPKTRDY;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    MGC_Write8(pBase, MGC_O_FDRC_INDEX, 0);
    bCsrVal = MGC_Read8(pBase, MGC_O_FDRC_CSR0);

    if(bCsrVal & MGC_M_CSR0_P_SETUPEND)
    {
	bVal |= MGC_M_CSR0_P_SVDSETUPEND;
    }
    /* prepare stall response if required */
    if(bStall)
    {
	bVal |= MGC_M_CSR0_P_SENDSTALL;
    }
    else if(pPort->wSetupDataSize)
    {
	/* we need to transmit; ack RxPktRdy BEFORE loading FIFO */
	MGC_Write8(pBase, MGC_O_FDRC_CSR0, bVal);
	bVal = 0;
	/* start transmit */
	wSize = MUSB_MIN(64, pPort->wSetupDataSize);
	MGC_FdrcLoadFifo(pPort, 0, wSize, pPort->pSetupData);
	pPort->wSetupDataIndex = wSize;
	bVal |= MGC_M_CSR0_TXPKTRDY;
	if(wSize == pPort->wSetupDataSize)
	{
	    bVal |= MGC_M_CSR0_P_DATAEND;
	    pPort->bEnd0Status = TRUE;
	    pPort->wSetupDataIndex = 0;
	    pPort->wSetupDataSize = 0;
	}
    }
    else
    {
	/* just acknowledge end of data */
	bVal |= MGC_M_CSR0_P_DATAEND;
	pPort->bEnd0Status = TRUE;
    }

    /* write CSR0 */
    MGC_DIAG1(3, pPort->pController, "DefaultEndResponse: Setting CSR0 to ", bVal, 16, 2);
    MGC_Write8(pBase, MGC_O_FDRC_CSR0, bVal);

    return 0;
}

/*
 * Handle default endpoint interrupt
 */
uint8_t MGC_FdrcServiceDefaultEnd(MGC_Port* pPort, MGC_BsrItem* pItem)
{
    uint16_t wCsrVal, wCount;
    MGC_EndpointResource* pEnd = MUSB_ArrayFetch(&(pPort->LocalEnds), 0);
    uint8_t bError = FALSE;
    uint8_t bResult = FALSE;
    uint8_t status = MUSB_STATUS_OK;
    uint8_t* pBase = (uint8_t*)pPort->pController->pControllerAddressIsr;

    MGC_Write8(pBase, MGC_O_FDRC_INDEX, 0);
    wCsrVal = MGC_Read8(pBase, MGC_O_FDRC_CSR0);
    wCount = MGC_Read8(pBase, MGC_O_FDRC_COUNT0);
    MGC_DIAG1(3, pPort->pController, "CSR0=", wCsrVal, 16, 2);
    MGC_DIAG1(3, pPort->pController, "Count0=", wCount, 16, 2);

    if(pPort->bIsHost)
    {
	if(wCsrVal & MGC_M_CSR0_H_RXSTALL)
	{
	    MGC_DIAG(2, pPort->pController, "End 0 stall\n");
	    status = MUSB_STATUS_STALLED;
	    bError = TRUE;
	}
	else if(wCsrVal & MGC_M_CSR0_H_ERROR)
	{
	    MGC_DIAG(2, pPort->pController, "end 0: no response (error)\n");
	    status = MUSB_STATUS_NACK_LIMIT;
	    bError = TRUE;
	}
	else if(wCsrVal & MGC_M_CSR0_H_NAKTIMEOUT)
	{
	    MGC_DIAG(2, pPort->pController, "end 0: no response (NAK timeout)\n");
	    status = MUSB_STATUS_NACK_LIMIT;
	    bError = TRUE;
	}
	if(MUSB_STATUS_NACK_LIMIT == status)
	{
	    /* use the proper sequence to abort the transfer */
	    if(wCsrVal & MGC_M_CSR0_H_REQPKT)
	    {
		wCsrVal &= ~MGC_M_CSR0_H_REQPKT;
		MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)wCsrVal);
		wCsrVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
		MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)wCsrVal);
	    }
	    else
	    {
		wCsrVal |= MGC_M_CSR02_FLUSHFIFO;
		MGC_Write8(pBase, MGC_O_FDRC_CSR02, (uint8_t)wCsrVal);
		MGC_Write8(pBase, MGC_O_FDRC_CSR02, (uint8_t)wCsrVal);
		wCsrVal &= ~MGC_M_CSR0_H_NAKTIMEOUT;
		MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)wCsrVal);
	    }
	}

	if(bError)
	{
	    /* clear it */
	    MGC_Write8(pBase, MGC_O_FDRC_CSR0, 0);
	}
    }
    else if(wCsrVal & MGC_M_CSR0_P_SENTSTALL)
    {
	bError = TRUE;
	wCsrVal &= ~MGC_M_CSR0_P_SENTSTALL;
	MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)wCsrVal);
    }
    else if(wCsrVal & MGC_M_CSR0_P_SETUPEND)
    {
	pPort->wSetupDataIndex = 0;
	pPort->wSetupDataSize = 0;
    }
    else if(!wCount && pPort->bEnd0Status)
    {
	/* this is the "acknowledge" interrupt */
	pPort->bEnd0Status = FALSE;
	MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)0);

	/* set address if required */
	if(pPort->bSetAddress)
	{
	    MGC_FdrcProgramBusState(pPort);
	    pPort->bSetAddress = FALSE;
	}
	return FALSE;
    }

    if(bError)
    {
	if(pPort->bIsHost)
	{
	    if(MGC_CompleteIrp(pItem, pEnd, status, TRUE, pEnd->pTxIrp))
	    {
		bResult = TRUE;
	    }
	    /* start next transfer if any */
	    MGC_StartNextControlTransfer(pPort);
	}
    }
    else
    {
	/* call common logic */
	bResult = MGC_DrcServiceDefaultEnd(pPort, pItem, &wCsrVal, wCount);

	/* write CSR0 if needed */
	if(wCsrVal)
	{
	    MGC_Write8(pBase, MGC_O_FDRC_CSR0, (uint8_t)wCsrVal);
	    MGC_DIAG1(3, pPort->pController, "Setting CSR0 to ", wCsrVal, 16, 2);
	}
    }

    return bResult;
}

/*
 * Service an endpoint Tx interrupt
 */
uint8_t MGC_FdrcServiceTransmitAvail(MGC_Port* pPort, uint16_t wEndIndex, 
				     MGC_BsrItem* pItem)
{
    uint8_t bVal;
    MGC_EndpointResource* pEnd;
    uint8_t bResult = FALSE;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    pItem->bStatus = MUSB_STATUS_OK;
    pItem->bCause = MGC_BSR_CAUSE_NONE;

    MGC_Write8(pBase, MGC_O_FDRC_INDEX, (uint8_t)wEndIndex);
    bVal = MGC_Read8(pBase, MGC_O_FDRC_TXCSR1);

    pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), 
	wEndIndex);
    do
    {
	if(pPort->bIsHost)
	{
	    /* check for TX error */
	    if(bVal & MGC_M_TXCSR1_H_ERROR)
	    {
		status = MUSB_STATUS_TRANSMIT_ERROR;
	    }
	    /* check for stall */
	    if(bVal & MGC_M_TXCSR1_H_RXSTALL)
	    {
		status = MUSB_STATUS_STALLED;
	    }
	    if(status)
	    {
		/* reset error bits */
		bVal &= ~(MGC_M_TXCSR1_H_ERROR | MGC_M_TXCSR1_H_RXSTALL);
		/* TODO: in HDRC, we set FRCDATATOG */
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, bVal);
		bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
		/* start next IRP if any */
		if(pEnd->bStopTx)
		{
		    pEnd->bIsHalted = TRUE;
		}
		return bResult;
	    }
	    else
	    {
		if(pEnd)
		{
#ifdef MUSB_DMA
		    if(pEnd->pDmaChannel)
		    {
			if(MUSB_DMA_STATUS_FREE == 
			    pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel))
			{
			    pEnd->dwTxOffset = pEnd->pDmaChannel->dwActualLength;
			}
		    }
		    else
#endif
			pEnd->dwTxOffset += pEnd->dwTxSize;
		    bResult = MGC_PipeTransmitReady(pPort, pEnd);
		}
		else
		{
		    bResult = TRUE;
		    status = MUSB_STATUS_INTERNAL_ERROR;
		}
	    }
	    if(bResult)
	    {
#ifdef MUSB_DMA
		if(pEnd->pDmaChannel)
		{
		    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
		    pEnd->pDmaChannel = NULL;
		}
		/* clear DMA bits */
		bVal = MGC_Read8(pBase, MGC_O_FDRC_TXCSR2);
		bVal &= ~(MGC_M_TXCSR2_DMAENAB | MGC_M_TXCSR2_DMAMODE);
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, bVal);
#endif
		MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
		/* start next IRP if any */
		if(pEnd->bStopTx)
		{
		    pEnd->bIsHalted = TRUE;
		}
	    }
	}
	else
	{
	    /* Function role */

	    if(bVal & MGC_M_TXCSR1_P_SENTSTALL)
	    {
		bVal &= ~MGC_M_TXCSR1_P_SENTSTALL;
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, bVal);
		break;
	    }
	    if(bVal & MGC_M_TXCSR1_P_UNDERRUN)
	    {
		bVal &= ~MGC_M_TXCSR1_P_UNDERRUN;
		MGC_Write8(pBase, MGC_O_FDRC_TXCSR1, bVal);
	    }

	    if(pEnd)
	    {
#ifdef MUSB_DMA
		if(pEnd->pDmaChannel)
		{
		    if(MUSB_DMA_STATUS_FREE == 
			pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pDmaChannel))
		    {
			pEnd->dwTxOffset = pEnd->pDmaChannel->dwActualLength;
		    }
		}
		else
#endif
		    pEnd->dwTxOffset += pEnd->dwTxSize;
		if(pEnd->pTxIrp)
		{
		    bResult = MGC_PipeTransmitReady(pPort, pEnd);
		    if(bResult)
		    {
#ifdef MUSB_DMA
			if(pEnd->pDmaChannel)
			{
			    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pDmaChannel);
			    pEnd->pDmaChannel = NULL;
			}
			/* clear DMA bits */
			bVal = MGC_Read8(pBase, MGC_O_FDRC_TXCSR2);
			bVal &= ~(MGC_M_TXCSR2_DMAENAB | MGC_M_TXCSR2_DMAMODE);
			MGC_Write8(pBase, MGC_O_FDRC_TXCSR2, bVal);
#endif
			bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, TRUE, pEnd->pTxIrp);
		    }
		}
	    }
	}
    } while(FALSE);

    return bResult;
}

/*
 * Service an endpoint Rx interrupt
 */
uint8_t MGC_FdrcServiceReceiveReady(MGC_Port* pPort, uint16_t wEndIndex, 
				    MGC_BsrItem* pItem)
{
    uint8_t bVal;
    uint8_t bVal2;
    MGC_EndpointResource* pEnd;
    uint16_t wRxCount;
    uint8_t bResult = FALSE;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;

    pItem->bStatus = MUSB_STATUS_OK;
    pItem->bCause = MGC_BSR_CAUSE_NONE;

    /* select end */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, (uint8_t)wEndIndex);
    bVal = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
    bVal2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);

    pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), 
	wEndIndex);
    do
    {
	if(pPort->bIsHost)
	{
	    /* check for RX error */
	    if(bVal & (MGC_M_RXCSR1_H_ERROR | MGC_M_RXCSR1_DATAERR))
	    {
		status = MUSB_STATUS_RECEIVE_ERROR;
		/* do the proper sequence to abort the transfer */
		bVal &= ~MGC_M_RXCSR1_H_REQPKT;
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
	    }
	    /* check for stall */
	    if(bVal & MGC_M_RXCSR1_H_RXSTALL)
	    {
		status = MUSB_STATUS_STALLED;
		/* avoid a race with next StartRx by clearing ReqPkt */
		bVal &= ~MGC_M_RXCSR1_H_REQPKT;
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
	    }
	    /* be sure a packet is ready for unloading */
	    if(!status && !(bVal & MGC_M_RXCSR1_RXPKTRDY))
	    {
		status = MUSB_STATUS_INTERNAL_ERROR;
	    }
	    if(status)
	    {
		/* reset error bits */
		/* TODO: don't know if middle one will work; docs inconsistent */
		bVal &= ~(MGC_M_RXCSR1_H_ERROR | MGC_M_RXCSR1_DATAERR | MGC_M_RXCSR1_H_RXSTALL);
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
		bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);
		bResult = TRUE;
	    }
	    else
	    {
		if(pEnd && pEnd->pRxIrp)
		{
		    /* we are expecting traffic */
#ifdef MUSB_DMA
		    if(pEnd->pRxDmaChannel)
		    {
			if(MUSB_DMA_STATUS_FREE == 
			    pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
			{
			    pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
			}
		    }
#endif
		    wRxCount = MGC_ReadRxCount(pController);
		    bResult = MGC_PipePacketReceived(pPort, pEnd, wRxCount, 
			TRUE, pEnd->pRxIrp);
		    if(bResult)
		    {
#ifdef MUSB_DMA
			if(pEnd->pRxDmaChannel)
			{
			    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
			    pEnd->pRxDmaChannel = NULL;
			}
#endif
			/* clear recv condition */
			bVal = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
			bVal &= ~MGC_M_RXCSR1_RXPKTRDY;
			MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
			/* complete IRP */
			MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);
		    }
		}
		else
		{
		    bResult = TRUE;
		    status = MUSB_STATUS_INTERNAL_ERROR;
		}
	    }
	}
	else
	{
	    /*
	    *  PERIPHERAL RX ( HOST OUT )
	    */
	    if((bVal & MGC_M_RXCSR1_P_SENTSTALL) ||
		!(bVal & MGC_M_RXCSR1_RXPKTRDY))
	    {
		bVal &= ~MGC_M_RXCSR1_P_SENTSTALL;
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
		break;  /* exit int processing */
	    }
	    /*
	    *  Fresh packet received
	    */
	    if(!pEnd || pEnd->bIsHalted)
	    {
		/* couldn't find endpoint or it is halted */
		status = pEnd ? 0 : MUSB_STATUS_INTERNAL_ERROR;
		MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, MGC_M_RXCSR1_P_SENDSTALL);
		break;
	    }
	    if(pEnd && pEnd->pRxIrp)
	    {
		/* we are expecting traffic */
		while(pEnd->pRxIrp &&
		    ((pEnd->bBsrResidual != pEnd->bIsrResidual) || (bVal & MGC_M_RXCSR1_RXPKTRDY)))
		{
		    if(pEnd->bBsrResidual != pEnd->bIsrResidual)
		    {
			pEnd->bBsrResidual++;
		    }
#ifdef MUSB_DMA
		    /* if we were using DMA, update actual byte count */
		    if(pEnd->pRxDmaChannel)
		    {
			if(MUSB_DMA_STATUS_FREE == 
			    pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
			{
			    pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
			    pEnd->bDmaResidual = FALSE;
			}
		    }
		    if(pEnd->bDmaResidual)
		    {
			return FALSE;
		    }
#endif
		    /* read FIFO count and see if current IRP is satisfied */
		    wRxCount = MGC_ReadRxCount(pController);
		    bResult = MGC_PipePacketReceived(pPort, pEnd, wRxCount, 
			FALSE, pEnd->pRxIrp);
		    /* re-read CSR */
		    bVal2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);
		    /* clear recv condition if necessary */
		    if((wRxCount < pEnd->wRxPacketSize) || !(bVal2 & MGC_M_RXCSR2_AUTOCLEAR))
		    {
			bVal &= ~MGC_M_RXCSR1_RXPKTRDY;
			MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bVal);
		    }
		    if(bResult)
		    {
			/* current IRP is satisfied */
#ifdef MUSB_DMA
			if(pEnd->pRxDmaChannel)
			{
			    pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
			    pEnd->pRxDmaChannel = NULL;
			}
#endif
			bResult = MGC_CompleteIrp(pItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);

			/* clear DMA bits */
			bVal2 &= ~(MGC_M_RXCSR2_DMAENAB | MGC_M_RXCSR2_DMAMODE);
			MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, bVal2);

		    }
		}
	    }
	    else
	    {
		pEnd->bIsrResidual++;
	    }
	}   /* END: PERIPHERAL RX ( HOST OUT )  */

    } while(FALSE);

    return bResult;
}

/*
 * Load an endpoint FIFO
 */
void MGC_FdrcLoadFifo(MGC_Port* pPort, uint8_t bEnd, uint32_t dwCount, const uint8_t* pSource)
{
#ifdef MUSB_FDRC_AHB
    uint32_t dwIndex, dwIndex32;
    uint32_t dwCount32 = ((uint32_t)pSource & 3) ? 0 : (dwCount >> 2);
#else
    uint32_t dwIndex;
#endif
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);

#ifdef MUSB_FDRC_AHB
    /* doublewords when possible */
    for(dwIndex = dwIndex32 = 0; dwIndex32 < dwCount32; dwIndex32++, dwIndex += 4)
    {
	MGC_Write32(pBase, bFifoOffset, *((uint32_t*)&(pSource[dwIndex])));
    }
    while(dwIndex < dwCount)
    {
	MGC_Write8(pBase, bFifoOffset, pSource[dwIndex++]);
    }
#else
    /* 8-bit only */
    for(dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
	MGC_Write8(pBase, bFifoOffset, pSource[dwIndex]);
    }
#endif

}

/*
 * Unload an endpoint FIFO
 */
void MGC_FdrcUnloadFifo(MGC_Port* pPort, uint8_t bEnd, uint32_t dwCount, uint8_t* pDest)
{
#ifdef MUSB_FDRC_AHB
    uint32_t dwIndex, dwIndex32;
    uint32_t dwCount32 = ((uint32_t)pDest & 3) ? 0 : (dwCount >> 2);
#else
    uint32_t dwIndex;
#endif
    MGC_Controller* pController = pPort->pController;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIst;
    uint8_t bFifoOffset = MGC_FIFO_OFFSET(bEnd);

#ifdef MUSB_FDRC_AHB
    /* doublewords when possible */
    for(dwIndex = dwIndex32 = 0; dwIndex32 < dwCount32; dwIndex32++, dwIndex += 4)
    {
	*((uint32_t*)&(pDest[dwIndex])) = MGC_Read32(pBase, bFifoOffset);
    }
    while(dwIndex < dwCount)
    {
	pDest[dwIndex++] = MGC_Read8(pBase, bFifoOffset);
    }
#else
    /* 8-bit only */
    for(dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
	pDest[dwIndex] = MGC_Read8(pBase, bFifoOffset);
    }
#endif
}

uint8_t MGC_FdrcDmaChannelStatusChanged(
    void* pPrivateData, uint8_t bLocalEnd, uint8_t bTransmit)
{
#ifdef MUSB_DMA
    MGC_BsrItem qItem;
    uint8_t bQueue;
    MGC_EndpointResource* pEnd;
    uint16_t wRxCount;
    uint8_t bCsr1, bCsr2, bIntrRxE1, bIntrRxE2;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Port* pPort = (MGC_Port*)pPrivateData;
    MGC_Controller* pController = pPort->pController;
    MUSB_SystemServices* pServices = pController->pSystemServices;
    uint8_t* pBase = (uint8_t*)pController->pControllerAddressIsr;
    /* save index */
    uint8_t bIndex = MGC_Read8(pBase, MGC_O_FDRC_INDEX);

    pEnd = (MGC_EndpointResource*)MUSB_ArrayFetch(&(pPort->LocalEnds), 
	bLocalEnd);
    if((bTransmit && !pEnd->pDmaChannel) || (!bTransmit && !pEnd->pRxDmaChannel))
    {
	return FALSE;
    }

    /* TODO: something more specific might be better */
    if(bTransmit)
    {
	bQueue = MGC_FdrcServiceTransmitAvail(pPort, bLocalEnd, &qItem);
	if(bQueue)
	{
	    qItem.bLocalEnd = bLocalEnd;
	    VERIFY(pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem));
	}
    }
    else
    {
	/* select hw ep */
	MGC_Write8(pBase, MGC_O_FDRC_INDEX, bLocalEnd);

	bCsr1 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR1);
	bCsr2 = MGC_Read8(pBase, MGC_O_FDRC_RXCSR2);
	/* update actual byte count */
	if(pEnd->pRxDmaChannel)
	{
	    if(MUSB_DMA_STATUS_FREE == 
		pPort->pDmaController->pfDmaGetChannelStatus(pEnd->pRxDmaChannel))
	    {
		pEnd->dwRxOffset = pEnd->pRxDmaChannel->dwActualLength;
		pEnd->bDmaResidual = FALSE;
	    }
	}
	if(pEnd->bDmaResidual)
	{
	    return FALSE;
	}
	/* read FIFO count and see if current IRP is satisfied */
	wRxCount = MGC_ReadRxCount(pController);
	bQueue = MGC_PipePacketReceived(pPort, pEnd, wRxCount, 
	    FALSE, pEnd->pRxIrp);
	/* clear recv condition if necessary */
	if((wRxCount < pEnd->wRxPacketSize) || !(bCsr2 & MGC_M_RXCSR2_AUTOCLEAR))
	{
	    bCsr1 &= ~MGC_M_RXCSR1_RXPKTRDY;
	    MGC_Write8(pBase, MGC_O_FDRC_RXCSR1, bCsr1);
	}
	if(bQueue)
	{
	    /* current IRP is satisfied */
	    if(pEnd->pRxDmaChannel)
	    {
		pPort->pDmaController->pfDmaReleaseChannel(pEnd->pRxDmaChannel);
		pEnd->pRxDmaChannel = NULL;
	    }
	    bQueue = MGC_CompleteIrp(&qItem, pEnd, (uint8_t)status, FALSE, pEnd->pRxIrp);

	    /* clear DMA bits */
	    MGC_Write8(pBase, MGC_O_FDRC_RXCSR2, 
		bCsr2 & ~(MGC_M_RXCSR2_DMAENAB | MGC_M_RXCSR2_DMAMODE));
	    /* enable endpoint interrupt */
	    bIntrRxE1 = MGC_Read8(pBase, MGC_O_FDRC_INTRRX1E);
	    bIntrRxE2 = MGC_Read8(pBase, MGC_O_FDRC_INTRRX2E);
	    if(bLocalEnd < 8)
	    {
		MGC_Write8(pBase, MGC_O_FDRC_INTRRX1E, bIntrRxE1 | (1 << bLocalEnd));
	    }
	    else
	    {
		MGC_Write8(pBase, MGC_O_FDRC_INTRRX2E, bIntrRxE2 | (1 << (bLocalEnd - 8)));
	    }

	    qItem.bLocalEnd = bLocalEnd;
	    VERIFY(pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem));
	}
    }
    pPort->bDmaCompleted = bQueue;

    /* restore index */
    MGC_Write8(pBase, MGC_O_FDRC_INDEX, bIndex);

    return bQueue;

#else

    return FALSE;

#endif
}

#ifdef MUSB_DIAG

/*
* Dump info global to the FDRC
*/
int32_t MGC_FdrcDumpState(MGC_Controller* pController)
{
    /* TODO */
    return 0;
}

/*
* Dump info relevant to the given local endpoint resource
*/
int32_t MGC_FdrcDumpEndpoint(MGC_Controller* pController, MUSB_EndpointResource* pEnd)
{
    /* TODO */
    return 0;
}

#endif	/* diagnostics enabled */

#endif	/* FDRC enabled */

