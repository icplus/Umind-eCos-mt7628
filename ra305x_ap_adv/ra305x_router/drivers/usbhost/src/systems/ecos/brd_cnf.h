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

#ifndef __MUSB_NONE_BOARD_CONFIG_H__
#define __MUSB_NONE_BOARD_CONFIG_H__

#ifndef MUSB_VBUS_ERROR_RETRIES
#define MUSB_VBUS_ERROR_RETRIES 4
#endif

#ifndef MUSB_BABBLE_RETRIES
#define MUSB_BABBLE_RETRIES 4
#endif

//#define MUSB_MAX_CONTROLLERS	(4)
#define MUSB_MAX_CONTROLLERS	(1)//Jody
#define MUSB_INTERRUPT           0x01 //Jody
#define MUSB_TIMER_EXPIRE        0x02 //Jody

#define MUSB_DEV_CONFIG_TIMER_IDX  0
#define MUSB_DEV_CONFIG_TIMER  1

#define MUSB_HUB_TIMER_IDX  (MUSB_DEV_CONFIG_TIMER_IDX + MUSB_DEV_CONFIG_TIMER)
#define MUSB_HUB_TIMER  6 /* Should = MUSB_HUB_MAX_HUB_SUPPORTED define in mu_hcore.h */

#define MUSB_MSD_CONF_TIMER_IDX  (MUSB_HUB_TIMER_IDX + MUSB_HUB_TIMER)
#define MUSB_MSD_CONF_TIMER  1  /*Use for delay in mu_mapi.c after config msd device*/

#define MUSB_MSD_TIMER_IDX  (MUSB_MSD_CONF_TIMER_IDX + MUSB_MSD_CONF_TIMER)
#define MUSB_MSD_TIMER  1 /*Use for check medium in mu_mscsi.c*/

#define MUSB_SICD_TIMER_IDX  (MUSB_MSD_TIMER_IDX + MUSB_MSD_TIMER)
#define MUSB_SICD_TIMER  1 /*Use for delay in mu_sicdapi.c after config sicd device*/

#define MUSB_AUD_TIMER_IDX  (MUSB_SICD_TIMER_IDX + MUSB_SICD_TIMER)
#define MUSB_AUD_TIMER  1 /*Use for delay in mu_audio.c after config audio device*/

#define MUSB_HID_TIMER_IDX  (MUSB_AUD_TIMER_IDX + MUSB_AUD_TIMER)
#define MUSB_HID_TIMER  1 /*Use for delay in mu_hid.c after config hid device*/

#define MUSB_DONGLE_TIMER_IDX  (MUSB_HID_TIMER_IDX + MUSB_HID_TIMER)
#define MUSB_DONGLE_TIMER  1 

#define MUSB_BT_TIMER_IDX  (MUSB_DONGLE_TIMER_IDX + MUSB_DONGLE_TIMER)
#define MUSB_BT_TIMER  1 /*Use for delay in mu_hid.c after config hid device*/

#define MUSB_MAX_TIMERS  (MUSB_BT_TIMER_IDX + MUSB_BT_TIMER)

#define	MUSB_TS_SESSREQ	    8	/* SRP, WAIT_A_BCON, WAIT_B_ACON */
#define	MUSB_MAX_TRANSITION_TM   250	/* 250 msec for transition coalesce */
#define MUSB_TB_SRP_FAIL	5

/** Required task stack size for this target */
#define MUSB_UCOS_TASK_STACK_SIZE        (4*1024)

/** Required task stack size for this target */
#define MUSB_UCOS_TASK_PRIORITY        (60)

/** Tick period on this target */
#define MUSB_UCOS_TICK_PERIOD       10

/** PCI vendor ID for this target's controller(s) */
#define MUSB_PCI_VENDOR_ID	    0x14AB

/** PCI device ID for this target's controller(s) */
#define MUSB_PCI_DEVICE_ID		0x0010 

/* AFS doesn't provide OS delete functions, so define fake ones */
#define OSSemDel(a, b, c)
#define OSQDel(a, b, c)
#define OSMboxDel(a, b, c)

#define OS_DEL_ALWAYS	0xff

#endif	/* multiple inclusion protection */
