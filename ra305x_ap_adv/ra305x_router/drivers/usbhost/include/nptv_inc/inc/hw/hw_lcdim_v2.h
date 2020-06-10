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
/*-----------------------------------------------------------------------------
 *
 * $Author: dtvbm11 $
 * $Date  $
 * $RCSfile: hw_ospe.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

// *********************************************************************
// Down Scaler related Register and Macro
// *********************************************************************
#ifndef _HW_LDM_PQ_V2_H_
#define _HW_LDM_PQ_V2_H_

#include "typedef.h"
#include "x_hal_5381.h"
#include "hw_vdoin.h"

#define LDM_OFFSET (VDOIN_COLOR_BASE+0xD000)

//Page LOCDIM2_1
#define LD2_MODE_00 (LDM_OFFSET + 0x800)
    #define C_LD_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_LD_BYPASS Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_3D_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define C_LD_INK Fld(3,26,AC_MSKB3)//[28:26]
    #define C_LD_BLK_VERT_NUM Fld(4,22,AC_MSKW32)//[25:22]
    #define C_LD_BLK_HOR_NUM Fld(5,17,AC_MSKB2)//[21:17]
    #define C_LD_SCENE_CHG Fld(1,16,AC_MSKB2)//[16:16]
    #define C_LD_3D_RIGHT_FIRST Fld(1,10,AC_MSKB1)//[10:10]
    #define C_LD_MAX_PXL_TH Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_01 (LDM_OFFSET + 0x804)
    #define C_LD_BLK_NUM Fld(8,24,AC_FULLB3)//[31:24]
    #define C_LD_BLK_HEIGHT Fld(11,13,AC_MSKW21)//[23:13]
    #define C_LD_BLK_WIDTH Fld(13,0,AC_MSKW10)//[12:0]
#define LD2_MODE_02 (LDM_OFFSET + 0x808)
    #define C_LD_BLK_CTR_MODE Fld(3,29,AC_MSKB3)//[31:29]
    #define C_LD_BLK_CTR_V1 Fld(11,16,AC_MSKW32)//[26:16]
    #define C_LD_BLK_CTR_H1 Fld(13,0,AC_MSKW10)//[12:0]
// Reserved Register    
//    #define LD2_MODE_03 (LDM_OFFSET + 0x80C)
//       #define C_LD_BLK_CTR_V2 Fld(11,16,AC_MSKW32)//[26:16]
//       #define C_LD_BLK_CTR_H2 Fld(13,0,AC_MSKW10)//[12:0]
#define LD2_MODE_04 (LDM_OFFSET + 0x810)
    #define C_LD_DIM_WET_3 Fld(6,24,AC_MSKB3)//[29:24]
    #define C_LD_DIM_WET_2 Fld(6,16,AC_MSKB2)//[21:16]
    #define C_LD_DIM_WET_1 Fld(6,8,AC_MSKB1)//[13:8]
    #define C_LD_DIM_WET_0 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_05 (LDM_OFFSET + 0x814)
    #define C_LD_DIM_WET_4 Fld(6,22,AC_MSKW32)//[27:22]
    #define C_LD_MAX_DIM Fld(10,12,AC_MSKW21)//[21:12]
    #define C_LD_MIN_DIM Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_06 (LDM_OFFSET + 0x818)
    #define C_LD_BLKCONF_MODE Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_BLKCONF_ZERO_STEP Fld(5,25,AC_MSKB3)//[29:25]
    #define C_LD_DIM_STEP_SHIFT Fld(2,23,AC_MSKW32)//[24:23]
    #define C_LD_DY_DIM_SLOPE Fld(6,17,AC_MSKB2)//[22:17]
    #define C_LD_DY_DIM_OFST Fld(5,12,AC_MSKW21)//[16:12]
    #define C_LD_MAX_UP_DIM_STEP Fld(6,6,AC_MSKW10)//[11:6]
    #define C_LD_MAX_DN_DIM_STEP Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_07 (LDM_OFFSET + 0x81C)
    #define C_LD_LP_REF_COEF2 Fld(5,23,AC_MSKW32)//[27:23]
    #define C_LD_LP_REF_COEF Fld(5,18,AC_MSKB2)//[22:18]
// Reserved Register        
//    #define C_LD_LP_VERT_RATIO Fld(3,15,AC_MSKW21)//[17:15]
//    #define C_LD_LP_HOR_RATIO Fld(3,12,AC_MSKB1)//[14:12]
//    #define C_LD_LP_VERT Fld(6,6,AC_MSKW10)//[11:6]
//    #define C_LD_LP_HOR Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_08 (LDM_OFFSET + 0x820)
    #define C_LD_SPI_ADDR_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_SPI_RISING_SEL Fld(2,22,AC_MSKB2)//[23:22]
    #define C_LD_SPI_LIMIT_LNUM Fld(7,15,AC_MSKW21)//[21:15]
    #define C_LD_RGB_WEIGHT Fld(4,10,AC_MSKB1)//[13:10]
    #define C_LD_LPF_WEIGHT Fld(5,5,AC_MSKW10)//[9:5]
    #define C_LD_LP_OFST Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_MODE_09 (LDM_OFFSET + 0x824)
    #define C_LD_FORCE_DIM_VALUE Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FORCE_DIM_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define C_LD_PHASE_VERT_RATIO Fld(3,16,AC_MSKB2)//[18:16]
    #define C_LD_PHASE_HOR_RATIO Fld(3,12,AC_MSKB1)//[14:12]
    #define C_LD_PHASE_VERT Fld(5,5,AC_MSKW10)//[9:5]
    #define C_LD_PHASE_HOR Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_MODE_10 (LDM_OFFSET + 0x828)
    #define C_LD_PHASE_BLK_HOR_7 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_HOR_6 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_HOR_5 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_HOR_4 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_3 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_2 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_1 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_0 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_11 (LDM_OFFSET + 0x82C)
    #define C_LD_PHASE_BLK_HOR_15 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_HOR_14 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_HOR_13 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_HOR_12 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_11 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_10 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_9 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_8 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_12 (LDM_OFFSET + 0x830)
    #define C_LD_PHASE_BLK_HOR_20 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_19 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_18 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_17 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_16 Fld(4,0,AC_MSKB0)//[3:0]
    
#define LD2_MODE_13 (LDM_OFFSET + 0x834)
    #define C_LD_PHASE_BLK_VERT_7 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_VERT_6 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_VERT_5 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_VERT_4 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_3 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_2 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_1 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_0 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_14 (LDM_OFFSET + 0x838)
    #define C_LD_PHASE_BLK_VERT_15 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_VERT_14 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_VERT_13 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_VERT_12 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_11 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_10 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_9 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_8 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_15 (LDM_OFFSET + 0x83C)
    #define C_LD_PHASE_BLK_VERT_20 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_19 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_18 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_17 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_16 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_GAIN_LIMIT_00 (LDM_OFFSET + 0x840)
    #define C_LD_GAIN_LIMIT_0 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_1 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_01 (LDM_OFFSET + 0x844)
    #define C_LD_GAIN_LIMIT_2 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_3 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_02 (LDM_OFFSET + 0x848)
    #define C_LD_GAIN_LIMIT_4 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_5 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_03 (LDM_OFFSET + 0x84C)
    #define C_LD_GAIN_LIMIT_6 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_7 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_04 (LDM_OFFSET + 0x850)
    #define C_LD_GAIN_LIMIT_8 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_9 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_05 (LDM_OFFSET + 0x854)
    #define C_LD_GAIN_LIMIT_10 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_11 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_06 (LDM_OFFSET + 0x858)
    #define C_LD_GAIN_LIMIT_12 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_13 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_07 (LDM_OFFSET + 0x85C)
    #define C_LD_GAIN_LIMIT_14 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_15 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_08 (LDM_OFFSET + 0x860)
    #define C_LD_GAIN_LIMIT_16 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_17 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_09 (LDM_OFFSET + 0x864)
    #define C_LD_GAIN_LIMIT_18 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_19 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_10 (LDM_OFFSET + 0x868)
    #define C_LD_GAIN_LIMIT_20 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_21 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_11 (LDM_OFFSET + 0x86C)
    #define C_LD_GAIN_LIMIT_22 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_23 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_12 (LDM_OFFSET + 0x870)
    #define C_LD_GAIN_LIMIT_24 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_25 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_13 (LDM_OFFSET + 0x874)
    #define C_LD_GAIN_LIMIT_26 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_27 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_14 (LDM_OFFSET + 0x878)
    #define C_LD_GAIN_LIMIT_28 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_29 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_15 (LDM_OFFSET + 0x87C)
    #define C_LD_GAIN_LIMIT_30 Fld(12,16,AC_MSKW32)//[27:16]
    #define C_LD_GAIN_LIMIT_31 Fld(12,0,AC_MSKW10)//[11:0]
#define LD2_GAIN_LIMIT_16 (LDM_OFFSET + 0x880)
    #define C_LD_GAIN_LIMIT_32 Fld(12,16,AC_MSKW32)//[27:16]
#define LD2_SRAM_IF_00 (LDM_OFFSET + 0x884)
    #define C_LD_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define C_LD_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define C_LD_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define C_LD_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define C_LD_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define C_LD_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define C_LD_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_01 (LDM_OFFSET + 0x888)
    #define LD_CPU_SRAM_ADDR Fld(7,16,AC_MSKB2)//[22:16]
    #define C_LD_ADDR Fld(7,0,AC_MSKB0)//[6:0]
#define LD2_SRAM_IF_02 (LDM_OFFSET + 0x88C)
    #define C_LD_WDATA_0 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_03 (LDM_OFFSET + 0x890)
    #define C_LD_WDATA_1 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_04 (LDM_OFFSET + 0x894)
    #define C_LD_WDATA_2 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_05 (LDM_OFFSET + 0x898)
    #define C_LD_WDATA_3 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_06 (LDM_OFFSET + 0x89C)
    #define C_LD_WDATA_4 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_07 (LDM_OFFSET + 0x8A0)
    #define C_LD_WDATA_5 Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_08 (LDM_OFFSET + 0x8A4)
    #define LD_RDATA_0 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_09 (LDM_OFFSET + 0x8A8)
    #define LD_RDATA_1 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_10 (LDM_OFFSET + 0x8AC)
    #define LD_RDATA_2 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_11 (LDM_OFFSET + 0x8B0)
    #define LD_RDATA_3 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_12 (LDM_OFFSET + 0x8B4)
    #define LD_RDATA_4 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_13 (LDM_OFFSET + 0x8B8)
    #define LD_RDATA_5 Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_14 (LDM_OFFSET + 0x8BC)
    #define C_LD_CPURW_SEL_0 Fld(1,9,AC_MSKB1)//[9:9]
    #define C_LD_CPURW_ACTIVE Fld(1,8,AC_MSKB1)//[8:8]
    #define LD_CPU_SEL_SRAM_1 Fld(1,7,AC_MSKB0)//[7:7]
    #define LD_CPU_SEL_SRAM_0 Fld(1,6,AC_MSKB0)//[6:6]
    #define LD_TABLE_WAIT_VSYNC Fld(1,5,AC_MSKB0)//[5:5]
    #define C_LD_FILL_TABLE_READY Fld(1,4,AC_MSKB0)//[4:4]
    #define LD_READ_FINISH Fld(1,3,AC_MSKB0)//[3:3]
    #define C_LD_READ Fld(1,2,AC_MSKB0)//[2:2]
    #define LD_WRITE_FINISH Fld(1,1,AC_MSKB0)//[1:1]
    #define C_LD_WRITE Fld(1,0,AC_MSKB0)//[0:0]
#define LD2_DBUG (LDM_OFFSET + 0x8C0)
    #define LD_ERR_R_INIT_LP Fld(1,31,AC_MSKB3)//[31:31]
    #define C_LD_CLEAR_ERR Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_DBG_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_STATUS_00 (LDM_OFFSET + 0x8C8)
    #define LD_IS_RIGHT_FRAME Fld(1,31,AC_MSKB3)//[31:31]
    #define LD_MAX_INFO Fld(10,8,AC_MSKW21)//[17:8]
    #define C_LD_READ_BLK_NUM Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_STATUS_01 (LDM_OFFSET + 0x8CC)
    #define LD_OUT_DIM Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_TAR_DIM Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_CONFIDENCE Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_STATUS_02 (LDM_OFFSET + 0x8D0)
    #define LD_YSUM Fld(31,0,AC_MSKDW)//[30:0]
#define LD2_STATUS_03 (LDM_OFFSET + 0x8D4)
    #define LD_BIN_03_00 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_STATUS_04 (LDM_OFFSET + 0x8D8)
    #define LD_BIN_07_04 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_STATUS_05 (LDM_OFFSET + 0x8DC)
    #define LD_BIN_11_08 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_STATUS_06 (LDM_OFFSET + 0x8E0)
    #define LD_BIN_15_12 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_INK_DATA (LDM_OFFSET + 0x8E4)
    #define C_LD_INK_B Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_INK_G Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_INK_R Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DEMO_00 (LDM_OFFSET + 0x8E8)
    #define C_LD_DEMO_YSTART Fld(16,16,AC_FULLW32)//[31:16]
    #define C_LD_DEMO_XSTART Fld(16,0,AC_FULLW10)//[15:0]
#define LD2_DEMO_01 (LDM_OFFSET + 0x8EC)
    #define C_LD_DEMO_YEND Fld(16,16,AC_FULLW32)//[31:16]
    #define C_LD_DEMO_XEND Fld(16,0,AC_FULLW10)//[15:0]
#define LD2_DEMO_02 (LDM_OFFSET + 0x8F0)
    #define C_LD_DEMO_REGION_SWAP Fld(1,2,AC_MSKB0)//[2:2]
    #define C_LD_DEMO_NOHIT_MODE Fld(2,0,AC_MSKB0)//[1:0]
#define LD2_DIM_RESULT_00 (LDM_OFFSET + 0x8F4)
    #define LD_DIM_READY Fld(1,30,AC_MSKB3)//[30:30]
    #define LD_DIM_002 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_001 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_000 Fld(10,0,AC_MSKW10)//[9:0]
// End of Local Dimming Result Retrieval
#define LD2_DIM_RESULT_66 (LDM_OFFSET + 0x9FC)
    #define LD_DIM_199 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_198 Fld(10,0,AC_MSKW10)//[9:0]

    
#endif

