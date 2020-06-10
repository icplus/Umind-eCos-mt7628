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

#ifndef __MUSB_DRC_DEFS_H__
#define __MUSB_DRC_DEFS_H__

#include "mu_tools.h"

enum MTK_USB_State
{
  MTK_USBHost_Init,
  MTK_USBHost_Enumeration,
  MTK_USBHost_StorageInit,
  MTK_USBHost_StorageEnable,
  MTK_USBHost_DongleTransfer,
  MTK_USBHost_DonglePPPTransfer
};
extern int ECOS_USBHost_State;

/*
 * DRC-specific definitions
 * $Revision: #1 $
 */
#ifdef MTK_MHDRC
#define MGC_MAX_USB_ENDS	9
#define MGC_USB_PORT0_ENDS  6
#define MGC_USB_PORT1_ENDS  6
#define MGC_USB_PORT2_ENDS  9
#define MGC_USB_PORT3_ENDS  9
#else
#define MGC_MAX_USB_ENDS	5
#endif

#define MGC_END0_FIFOSIZE    64      /* this is non-configurable */

#define MGC_M_FIFO_EP0     0x20

#define MGC_O_DRC_INDEX		0x0E
#define	MGC_O_DRC_FIFOSIZE	0x1F

/* Interrupt register bit masks */
#define MGC_M_INTR_SUSPEND    0x01
#define MGC_M_INTR_RESUME     0x02
#define MGC_M_INTR_RESET      0x04
#define MGC_M_INTR_BABBLE     0x04
#define MGC_M_INTR_SOF        0x08 
#define MGC_M_INTR_CONNECT    0x10
#define MGC_M_INTR_DISCONNECT 0x20
#define MGC_M_INTR_SESSREQ    0x40
#define MGC_M_INTR_VBUSERROR  0x80   /* FOR SESSION END */

#define MGC_M_INTR_EP0      0x01  /* FOR EP0 INTERRUPT */

/* DEVCTL */

#define MGC_M_DEVCTL_BDEVICE    0x80   
#define MGC_M_DEVCTL_FSDEV      0x40
#define MGC_M_DEVCTL_LSDEV      0x20
#define MGC_M_DEVCTL_HM         0x04
#define MGC_M_DEVCTL_HR         0x02
#define MGC_M_DEVCTL_SESSION    0x01

/* CSR0 */
#define MGC_M_CSR0_TXPKTRDY       0x0002
#define MGC_M_CSR0_RXPKTRDY       0x0001

/* CSR0 in Peripheral mode */
#define MGC_M_CSR0_P_SVDSETUPEND  0x0080
#define MGC_M_CSR0_P_SVDRXPKTRDY  0x0040
#define MGC_M_CSR0_P_SENDSTALL    0x0020
#define MGC_M_CSR0_P_SETUPEND     0x0010
#define MGC_M_CSR0_P_DATAEND      0x0008
#define MGC_M_CSR0_P_SENTSTALL    0x0004

/* CSR0 in Host mode */
#define	MGC_M_CSR0_H_NAKTIMEOUT   0x0080
#define MGC_M_CSR0_H_STATUSPKT    0x0040
#define MGC_M_CSR0_H_REQPKT       0x0020
#define MGC_M_CSR0_H_ERROR        0x0010
#define MGC_M_CSR0_H_SETUPPKT     0x0008
#define MGC_M_CSR0_H_RXSTALL      0x0004

/*
*  CID input signal indicates the ID pin of mini-A/B connector
*  on FDRC, 'CID' is available in DevCtl, 
*  on HDRC, 'B-Device' in DevCtl is valid only while a session is in progress
*/
#define MGC_CID_UNKNOWN    2
#define MGC_CID_A_DEVICE   0
#define MGC_CID_B_DEVICE   1

/*
*  A_DEVICE, B_DEVICE must be qualified by CID_VALID for valid context.
*  x is a pointer to the core object.
*/

#define MGC_A_DEVICE(x)         ((x)->cid == CID_A_DEVICE)
#define MGC_B_DEVICE(x)         ((x)->cid == CID_B_DEVICE)

/* Vbus values */
#define MGC_VBUS_BELOW_SESSION_END  0
#define MGC_VBUS_ABOVE_SESSION_END  1
#define MGC_VBUS_ABOVE_AVALID       2
#define MGC_VBUS_ABOVE_VBUS_VALID   3
#define MGC_VBUS_ERROR		    	0xff

#define FEATURE_SOFT_CONNECT 1
#define FEATURE_DMA_PRESENT 2
#define FEATURE_HDRC_FS 4
#define FEATURE_HIGH_BW 8
#define FEATURE_DFIFO 16
#define FEATURE_MULTILAYER 32
#define FEATURE_I2C 64

#endif	/* multiple inclusion protection */
