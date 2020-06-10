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
 * $Date: 2012/06/05 $
 * $RCSfile: hw_mjc.h $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/
 
#ifndef _HW_MJC_H_
#define _HW_MJC_H_

#ifndef CC_FPGA
#define IO_ADDR_OFST IO_VIRT
#else
#define IO_ADDR_OFST 0x20000000
#endif

#define MJC_BASE_ADDR (IO_ADDR_OFST + 0x36000)
	
	//Page SYS_1
#define MJC_SYS_00 (MJC_BASE_ADDR + 0x000)
    #define MJC_PD_INFO_DLY Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_RD_UNDER_RST Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_SEE_READ_IN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PD_INFO_32_SHIFT Fld(3,20,AC_MSKB2)//[22:20]
    #define MJC_PD_INFO_GET_INV Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PD_INFO_22_SHIFT Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_PD_INFO_SEL_0 Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PD_INFO_FREE Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_SEED_FD_OUT Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_GRP_RPT_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_BY_OUT_TRANSFORM Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_BY_IN_TRANSFORM Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_DRAM_MV_RD_CUT_OFF Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_DRAM_MV_RD_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_DRAM_MV_WR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_DRAM_OUT_RD_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_DRAM_IN_WR_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_FRM_FREEZE Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_BYPASS_MJC Fld(1,0,AC_MSKB0)//[0:0]
    #define MJC_DRAM_RW_EN Fld(5,4,AC_MSKB0)//[8:4]
#define MJC_SYS_01 (MJC_BASE_ADDR + 0x004)
    #define MJC_IN_CHROMADLY_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_IN_TEST_V_DLY Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_IN_HSYNC_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_IN_VSYNC_INV Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_IN_LINE_EXT Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_IN_UVINV Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_IN_LPF_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_IN_YUV2YC_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_IN_V_START Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_IN_H_START Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_02 (MJC_BASE_ADDR + 0x008)
    #define MJC_MJC_HEIGHT_FX Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_MJC_WIDTH Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_03 (MJC_BASE_ADDR + 0x00C)
    #define MJC_OUT_FILT_SEL_Y Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_OUT_FILT_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_OUT_HSYNC_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_OUT_VSYNC_INV Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_OUT_2CH_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_OUT_CHROMADLY_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_OUT_UVINV Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_OUT_YC2YUV_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_OUT_V_START Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_H_START Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_04 (MJC_BASE_ADDR + 0x010)
    #define MJC_B2R_INIT_FAST Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_B2R_PAT_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_PROG_FRM_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_OUT_VSYNC_DIV Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_OUT_FRM_NO Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_OUT_HEIGHT Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_WIDTH Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_05 (MJC_BASE_ADDR + 0x014)
    #define MJC_DISP_VSYNC_INV Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_DISP_HSYNC_INV Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_B2R_RPT_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_DISP_MODE Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_TEST_HSYNC_RST Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_OUT_FREE_VSYNC Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_OUT_V_TOTAL Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_H_TOTAL Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_06 (MJC_BASE_ADDR + 0x018)
    #define MJC_MJC_DBG_MODE Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_PD_INFO_PROG_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PD_INFO_PROG_NO Fld(3,20,AC_MSKB2)//[22:20]
    #define MJC_CINEMA_MODE Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_OSMP_MODE Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_OSMP_HPR Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_OSMP_FPR_TYPE Fld(2,15,AC_MSKW21)//[16:15]
    #define MJC_OSMP_2D_IN Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_SA_PRELOAD_MODE Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_DRAM_REQ_DBG Fld(3,8,AC_MSKB1)//[10:8]
    #define MJC_SET_RES_DELY Fld(4,2,AC_MSKB0)//[5:2]
    #define MJC_CLEAN_SET_RES_O Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_CLEAN_SET_RES_I Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_SYS_07 (MJC_BASE_ADDR + 0x01C)
    #define MJC_IN_SET_END_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_IN_V_STA_NEG Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_IN_H_STA_NEG Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_IN_V_END Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_IN_H_END Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_08 (MJC_BASE_ADDR + 0x020)
    #define MJC_MJC_VSYNC_DLY Fld(5,27,AC_MSKB3)//[31:27]
    #define MJC_OUT_SET_END_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_OUT_V_END Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_H_END Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_09 (MJC_BASE_ADDR + 0x024)
    #define MJC_IN_LR_FORCE_ZERO Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_IN_LR_SAMPLE_MODE Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_IN_LR_PROG_LR Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_IN_LR_PROG_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_IN_LR_FREE_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_IN_GEN_HSYNC_NO Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_IN_NO_HSYNC_LEN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_0A (MJC_BASE_ADDR + 0x028)
    #define MJC_PTG_3D_INV Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_PTG_3D_MODE Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PTG_BND_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PTG_BND_W Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_PTG_BND_H Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_0B (MJC_BASE_ADDR + 0x02C)
    #define MJC_OUT_PROG_V_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_OUT_PROG_H_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_OUT_PROG_H_END Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_PROG_H_START Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_0C (MJC_BASE_ADDR + 0x030)
    #define MJC_IN_BLACK_H Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_IN_BLACK_W Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_OUT_PROG_V_END Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OUT_PROG_V_START Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_0D (MJC_BASE_ADDR + 0x034)
    #define MJC_B2R_LBOX_Y_THRES Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_B2R_LBOX_RT_SKIP Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_B2R_LBOX_LF_SKIP Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_B2R_LBOX_DN_SKIP Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_B2R_LBOX_UP_SKIP Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_SYS_0E (MJC_BASE_ADDR + 0x038)
    #define MJC_PTG2_COLOR_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PTG2_PAT_MODE Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PTG2_FREERUN_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_PTG2_HOR_LINE_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_PTG2_PAT_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PTG2_FREERUN_SPEED Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_PTG2_LEFT_SIDE Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PTG2_ANGLE_SEL Fld(3,16,AC_MSKB2)//[18:16]
    #define MJC_PTG2_PAT_DUTY_CYCLE_MSB Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PTG2_FREQ_SEL Fld(3,12,AC_MSKB1)//[14:12]
    #define MJC_PTG2_PAT_DUTY_CYCLE_LSB Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_PTG2_YPAT1 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_PTG2_YPAT2 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_SYS_0F (MJC_BASE_ADDR + 0x03C)
    #define MJC_PTG_COLOR_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PTG_PAT_MODE Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PTG_FREERUN_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_PTG_HOR_LINE_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_PTG_PAT_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PTG_FREERUN_SPEED Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_PTG_LEFT_SIDE Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PTG_ANGLE_SEL Fld(3,16,AC_MSKB2)//[18:16]
    #define MJC_PTG_PAT_DUTY_CYCLE_MSB Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PTG_FREQ_SEL Fld(3,12,AC_MSKB1)//[14:12]
    #define MJC_PTG_PAT_DUTY_CYCLE_LSB Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_PTG_YPAT1 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_PTG_YPAT2 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_SYS_D0 (MJC_BASE_ADDR + 0x040)
    #define MJC_IN_SYNC_ACT_V Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_IN_SYNC_ACT_H Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D1 (MJC_BASE_ADDR + 0x044)
    #define MJC_IN_SYNC_SELF Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_IN_SYNC_TOT_V Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_IN_SYNC_TOT_H Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D2 (MJC_BASE_ADDR + 0x048)
    #define MJC_OUT_SYNC_SELF Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_OUT_LR3D_DLY Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_OUT_VACT_PRE_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_OUT_VACT_PRE Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_SYS_D4 (MJC_BASE_ADDR + 0x050)
    #define MJC_M_H_STR Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_M_H_LEN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D5 (MJC_BASE_ADDR + 0x054)
    #define MJC_M_V_STR Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_M_V_LEN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D6 (MJC_BASE_ADDR + 0x058)
    #define MJC_P_H_STR Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_P_H_LEN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D7 (MJC_BASE_ADDR + 0x05C)
    #define MJC_P_V_STR Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_P_V_LEN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D8 (MJC_BASE_ADDR + 0x060)
    #define MJC_DISP_HPOS Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_DISP_VPOS Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FRM_TRK_STEP Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_FRM_TRK_LINE Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_D9 (MJC_BASE_ADDR + 0x064)
    #define MJC_DITH_SUB_Y Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_DITH_SUB_U Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_DITH_SUB_V Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_DITH_DRMOD_U Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_DITH_DRMOD_Y Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_DITH_FPHASE_R Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_DITH_FPHASE_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_DITH_LEFT_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_DITH_SUBPIX_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_DITH_TABLE_EN Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_DITH_FPHASE_CTRL Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_DITH_INK_DATA_Y Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_DA (MJC_BASE_ADDR + 0x068)
    #define MJC_DITH_FPHASE_SEL Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_DITH_FPHASE_BIT Fld(3,27,AC_MSKB3)//[29:27]
    #define MJC_DITH_INK_Y Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_DITH_INK_V Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_DITH_INK_U Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_DITH_INK_DATA_U Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_DITH_INK_DATA_V Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_DB (MJC_BASE_ADDR + 0x06C)
    #define MJC_DITH_H_ACTIVE Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_TTD_BYPASS Fld(1,15,AC_MSKB1)//[15:15]
#define MJC_STA_SYS_00 (MJC_BASE_ADDR + 0x070)
    #define MJC_STA_TDDC_INT Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_SET_RES_FINISHED_O Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_SET_RES_FINISHED_I Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_STA_INIT_UNDER Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_STA_PROC_UNDER Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_STA_INT Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_STA_SYS_01 (MJC_BASE_ADDR + 0x074)
    #define MJC_STA_V_TOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_H_TOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_CRC_0 (MJC_BASE_ADDR + 0x078)
    #define MJC_CRC_RDY_I Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_CRC_RDY_O Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_STA_CRC_1 (MJC_BASE_ADDR + 0x07C)
    #define MJC_CRC_OUT_0 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_CRC_2 (MJC_BASE_ADDR + 0x080)
    #define MJC_CRC_OUT_1 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_SYS_CEN (MJC_BASE_ADDR + 0x088)
    #define MJC_CEN_SEED Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_SYS_CRC_0 (MJC_BASE_ADDR + 0x08C)
    #define MJC_OUT_CRC_V_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_OUT_CRC_CLEAR Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_OUT_CRC_H_STR Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_OUT_CRC_V_STR Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_CRC_1 (MJC_BASE_ADDR + 0x090)
    #define MJC_OUT_CRC_MODE Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_OUT_CRC_SEL Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_OUT_CRC_H_END Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_OUT_CRC_V_END Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_CRC_2 (MJC_BASE_ADDR + 0x094)
    #define MJC_OUT_CRC_MASK_Y Fld(10,20,AC_MSKW32)//[29:20]
    #define MJC_OUT_CRC_MASK_C Fld(10,10,AC_MSKW21)//[19:10]
    #define MJC_OUT_CRC_MASK_V Fld(10,0,AC_MSKW10)//[9:0]
#define MJC_STA_LR3D (MJC_BASE_ADDR + 0x098)
    #define MJC_STA_LR3D_INFO_0 Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_STA_LR3D_INFO_I Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_SYS_MID (MJC_BASE_ADDR + 0x09C)
    #define MJC_IN_MID_BLANK Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_DDDS (MJC_BASE_ADDR + 0x0A0)
    #define MJC_STA_DDDS_TRK_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_STA_DDDS_FAST Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_STA_DDDS_SLOW Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_SYS_DDDS (MJC_BASE_ADDR + 0x0A4)
    #define MJC_DDDS_TRK_INV Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_DDDS_TRK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_SYS_CRC_I0 (MJC_BASE_ADDR + 0x0A8)
    #define MJC_IN_CRC_V_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_IN_CRC_CLEAR Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_IN_CRC_H_STR Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_IN_CRC_V_STR Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_CRC_I1 (MJC_BASE_ADDR + 0x0AC)
    #define MJC_IN_CRC_MODE Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_IN_CRC_SEL Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_IN_CRC_H_END Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_IN_CRC_V_END Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_CRC_I2 (MJC_BASE_ADDR + 0x0B0)
    #define MJC_IN_CRC_MASK_Y Fld(10,20,AC_MSKW32)//[29:20]
    #define MJC_IN_CRC_MASK_C Fld(10,10,AC_MSKW21)//[19:10]
    #define MJC_IN_CRC_MASK_V Fld(10,0,AC_MSKW10)//[9:0]
#define MJC_STA_CRC_I (MJC_BASE_ADDR + 0x0B4)
    #define MJC_CRC_OUT_I Fld(32,0,AC_FULLDW)//[31:0]
	
	//Page SYS_2
#define MJC_SYS_MUTE_0 (MJC_BASE_ADDR + 0x0B8)
    #define MJC_MUTE_1TYPE Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_MUTE_2TYPE Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_MUTE_B Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MUTE_G Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MUTE_R Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_SYS_MUTE_1 (MJC_BASE_ADDR + 0x0BC)
    #define MJC_MUTE_SET Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MUTE_1UP Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_MUTE_1DN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_MUTE_2 (MJC_BASE_ADDR + 0x0C0)
    #define MJC_MUTE_1LF Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_MUTE_1RT Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_MUTE_3 (MJC_BASE_ADDR + 0x0C4)
    #define MJC_MUTE_MODE Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MUTE_2UP Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_MUTE_2DN Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_MUTE_4 (MJC_BASE_ADDR + 0x0C8)
    #define MJC_MUTE_2LF Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_MUTE_2RT Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_VCNT (MJC_BASE_ADDR + 0x0CC)
    #define MJC_STA_VSYNC_OUT Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_VSYNC_IN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_VCNT_TRIG Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_STA_OUT_VCNT Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_STA_IN_VCNT Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_PR_0 (MJC_BASE_ADDR + 0x0D0)
    #define MJC_PR_GEN_CORE_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_PR_SYNC_REGEN_CEN_DCM Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_PR_OUT_HEIGHT_X2 Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_PR_SYNC_REGEN_BYPASS Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_PR_OUT_UDP4_DISAB Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_PR_SYNC_REGEN_V_TOTAL Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_PR_SYNC_REGEN_H_TOTAL Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_PR_1 (MJC_BASE_ADDR + 0x0D4)
    #define MJC_PR_SYNC_REGEN_MJC_VSYNC_SEL Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_PR_FOR_VSYNC_MJC_TWO_PULSE Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_PR_FOR_VSYNC_MJC Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_PR_SYNC_REGEN_V_PORCH Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_SYS_H_MSB (MJC_BASE_ADDR + 0x0D8)
    #define MJC_PR_SYNC_REGEN_H_TOTAL_MSB Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_OUT_PROG_H_START_MSB Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_OUT_PROG_H_END_MSB Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_OUT_H_END_MSB Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_IN_SYNC_TOT_H_MSB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_OUT_H_TOTAL_MSB Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_STA_DISP (MJC_BASE_ADDR + 0x0DC)
    #define MJC_STA_DISP_V_TOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_DISP_H_TOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_SYS_CLK_CTRL (MJC_BASE_ADDR + 0x0E0)
    #define MJC_RST_CTRL Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_TDDC_RST Fld(1,31,AC_MSKB2)//[22:22]
    #define MJC_SVP_RST Fld(1,30,AC_MSKB2)//[21:21]
    #define MJC_DRAM_RST Fld(1,31,AC_MSKB2)//[20:20]
    #define MJC_CORE_RST Fld(1,30,AC_MSKB2)//[19:19]
    #define MJC_OUTPUT_RST Fld(1,29,AC_MSKB2)//[18:18]
    #define MJC_INPUT_RST Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_REGISTER_RST Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_CLK_MJC_DYNAMIC_SWITCH_OFF Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_CLK_DRAM_PR_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_CLK_SVP_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_CLK_M_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_CLK_I_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_CLK_O_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_SYS_INTR_CTRL (MJC_BASE_ADDR + 0x0E4)
    #define MJC_INT_POL_SEL Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_INT_CLEAR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_INT_MASK Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CLR_MODE Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_TDDC_INT_POL_SEL Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_TDDC_INT_CLEAR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_TDDC_INT_MASK Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_TDDC_CLR_MODE Fld(1,3,AC_MSKB0)//[3:3]
#define MJC_SYS_ATPG (MJC_BASE_ADDR + 0x0FC)
    #define MJC_SYS_RESERVED Fld(30,2,AC_MSKDW)//[31:2]
    #define MJC_SYS_RESERVED_IN_LR_INV Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_SYS_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_SYS_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page FB_1
#define MJC_FB_00 (MJC_BASE_ADDR + 0x100)
    #define MJC_DRAM_420 Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_DRAM_C_8BIT Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_DRAM_Y_8BIT Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_WADDR_LO_LIMIT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_01 (MJC_BASE_ADDR + 0x104)
    #define MJC_DRAM_WBURST_SIZE Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_DRAM_STATUS_CLR Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_DRAM_RPT Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_FB_TWO_FRM_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_FB_ONE_FRM_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_DRAM_RPT_422 Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_WADDR_HI_LIMIT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_02 (MJC_BASE_ADDR + 0x108)
    #define MJC_FB_CRC_START Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FB_VSYNC_DLY Fld(7,24,AC_MSKB3)//[30:24]
#define MJC_FB_03 (MJC_BASE_ADDR + 0x10C)
    #define MJC_DRAM_FRM_CNT_M1 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_DRAM_REQ_MAX_SIZE Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_DRAM_REQ_FREQ Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FB_04 (MJC_BASE_ADDR + 0x110)
    #define MJC_APL_WT_LR3D_SEL Fld(1,30,AC_MSKB3)//[30:30]
#define MJC_FB_05 (MJC_BASE_ADDR + 0x114)
    #define MJC_C_420_1BLK_MODE_DISAB Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_C_FPR_INV_ORDER Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_C_RPT_LR_BY_INPUT Fld(2,25,AC_MSKB3)//[26:25]
    #define MJC_C_OU_LR_DATA_SWP Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_C_RPT_OLD_PR_ADDRESSING Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_C_RPT_FRM_NUM Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_STA_SHRTDLY_W_DONE Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_STA_RPT_DISPLR Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_STA_RPT_R_FRM_IDX Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_STA_RPT_W_FRM_IDX Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_C_RPT_FFO_SIZE Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_STA_RPT_MB_UDF Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_STA_RPT_QMB_UDF Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_C_RPT_REQ_SIZE Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_FB_06 (MJC_BASE_ADDR + 0x118)
    #define MJC_C_DRAM_LST_ADD_PERIOD Fld(10,22,AC_MSKW32)//[31:22]
    #define MJC_STA_FRM_FLD_SIZE Fld(22,0,AC_MSKDW)//[21:0]
#define MJC_FB_08 (MJC_BASE_ADDR + 0x120)
    #define MJC_DRAM_BASE_ADDR Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_09 (MJC_BASE_ADDR + 0x124)
    #define MJC_MID_VSYNC_IN_CNT Fld(11,14,AC_MSKDW)//[24:14]
    #define MJC_MID_VSYNC_IN_PROG_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_MID_VSYNC_IN_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_DRAM_TP_W1 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_DRAM_TP_W0 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_DRAMR_444_FRMRST_SEL Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_DRAM_FRMSTART_WIDTH Fld(3,0,AC_MSKB0)//[2:0]
#define MJC_FB_0A (MJC_BASE_ADDR + 0x128)
    #define MJC_DRAM_420_IS_ODD Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_DRAM_420_AVG_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_DRAM_TEST_PAT Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_DRAMR_LAST_RST_DISAB Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_DRAMW_LAST_RST_DISAB Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_DRAM_OUT_PAT Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_DA_WAIT4_MV_TRIG_DISAB Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_RFIFO_INIT_GREEDY_MODE Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_FLIPMRR Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_BANDWIDTH_REDUCTION_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FB_0B (MJC_BASE_ADDR + 0x12C)
    #define MJC_DITH_LSB_OFF Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_DITH_ROUND_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_DITH_RDITHER_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_DITH_EDITHER_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_DITH_LFSR_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_DITH_FPHASE Fld(6,18,AC_MSKB2)//[23:18]
#define MJC_FB_0C (MJC_BASE_ADDR + 0x130)
    #define MJC_DRAM_RESCAN_1MISS_MODE Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_DRAM_READ_LEN_ENAB Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_DRAM_SWAP_MSB_LSB Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_DRAM_LAST_AUX_REV_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_DRAM_DYNAMIC_SWITCH Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_DRAM_READ_AGENT_ORDER Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_DRAMW_VSYNCRST_SEL Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_DRAM_LAST_AUX_CNTR_MAX Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_DRAM_TOPFSM_VSYNC_RST_DISAB Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_DRAM_RFIFO_WATERLEVEL_REG_HOLD Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_DRAM_RFIFO_WATERLEVEL_SEL Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_DRAMW_REQ_SIZE_M1 Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_FB_0D (MJC_BASE_ADDR + 0x134)
    #define MJC_LARBDW_R_DMAG4TIM Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_BURST_BLK_NUM_M1_G2 Fld(3,22,AC_MSKW32)//[24:22]
    #define MJC_BURST_BLK_NUM_M1_G1 Fld(3,19,AC_MSKB2)//[21:19]
    #define MJC_BURST_BLK_NUM_M1_G0 Fld(3,16,AC_MSKB2)//[18:16]
    #define MJC_PREF_4_PRL_OFST Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FB_0E (MJC_BASE_ADDR + 0x138)
    #define MJC_DRAM_BASE_ADDR_MSB Fld(3,29,AC_MSKB3)//[31:29]
    #define MJC_LARB_MJCW_R_DMAG0EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_LARB_MJCW_R_DMAG0TIM Fld(4,23,AC_MSKW32)//[26:23]
    #define MJC_LARB_MJCW_R_DMAG1EN Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_LARB_MJCW_R_DMAG1TIM Fld(4,18,AC_MSKB2)//[21:18]
    #define MJC_LARB_MJCW_R_DMAG2EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_LARB_MJCW_R_DMAG2TIM Fld(4,13,AC_MSKW21)//[16:13]
    #define MJC_LARB_MJCW_R_DMAG3EN Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_LARB_MJCW_R_DMAG3TIM Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LARB_MJCW_R_DMAG4EN Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_LARB_MJCW_R_DMAG4TIM Fld(4,3,AC_MSKB0)//[6:3]
    #define MJC_LARB_MJCW_R_DMAGBMBEG Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_LARB_MJCW_R_DMAGBMEND Fld(1,1,AC_MSKB0)//[1:1]
#define MJC_FB_0F (MJC_BASE_ADDR + 0x13C)
    #define MJC_LARB_MJCW_R_DMAGBMAGID Fld(3,29,AC_MSKB3)//[31:29]
    #define MJC_LARB_MJCW_R_DMAGBMALL Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_LARB_MJCW_R_DMTIMARBEN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_LARB_MJCW_R_DMARBRST Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_LARB_PR_R_DMAG0EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_LARB_PR_R_DMAG0TIM Fld(4,21,AC_MSKW32)//[24:21]
    #define MJC_LARB_PR_R_DMAG1EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_LARB_PR_R_DMAG1TIM Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_LARB_PR_R_DMAGBMBEG Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_LARB_PR_R_DMAGBMEND Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_LARB_PR_R_DMAGBMAGID Fld(3,11,AC_MSKB1)//[13:11]
    #define MJC_LARB_PR_R_DMAGBMALL Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_LARB_PR_R_DMTIMARBEN Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_LARB_PR_R_DMARBRST Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_LARBD_MJCWPR_R_DMAG0EN Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_LARBD_MJCWPR_R_DMAG0TIM Fld(4,3,AC_MSKB0)//[6:3]
    #define MJC_LARBD_MJCWPR_R_DMAG1EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_LARBD_MJCWPR_R_DMAGBMBEG Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_LARBD_MJCWPR_R_DMAGBMEND Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FB_10 (MJC_BASE_ADDR + 0x140)
    #define MJC_FILM_RLAST_RST_DISB Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FILM_VSYNC_SEL Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_FILM_WLAST_RST_DISB Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_FILM_BIT_TRUNC Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_FILM_BASE_ADDR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_11 (MJC_BASE_ADDR + 0x144)
    #define MJC_FILM_WBURST_SIZE Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_FILM_RBURST_SIZE Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_FILM_WENAB Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_FILM_RENAB Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FILM_RFIFO_SIZE Fld(5,16,AC_MSKB2)//[20:16]
    #define MJC_FILM_REQ_INTRVL_8X_MODE Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_FILM_REQ_INTRVL Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_FB_DBP_SEL Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_DA_DBP_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FB_12 (MJC_BASE_ADDR + 0x148)
    #define MJC_FB_RESERVED Fld(32,0,AC_FULLDW)//[31:0]
    #define MJC_RPT_FPR_LR_SWAP  Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_SWITCH_LR_PHAS_ORDER  Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_FB_RESERVED_HEIGHT  Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FB_13 (MJC_BASE_ADDR + 0x14C)
    #define MJC_DA_DBG0 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_FILM_BASE_ADDR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_14 (MJC_BASE_ADDR + 0x150)
    #define MJC_R2B_XTRA8ROW_FREQ Fld(3,26,AC_MSKB3)//[28:26]
    #define MJC_SG_HS_ENAB Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_SG_HS_NUM Fld(5,20,AC_MSKW32)//[24:20]
    #define MJC_SG_H_BLANKING Fld(12,8,AC_MSKW21)//[19:8]
    #define MJC_DA_DBG1 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FB_15 (MJC_BASE_ADDR + 0x154)
    #define MJC_LARBD_MJCWPR_R_DMAG1TIM Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_LARBD_MJCWPR_R_DMAGBMAGID Fld(3,25,AC_MSKB3)//[27:25]
    #define MJC_LARBD_MJCWPR_R_DMAGBMALL Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_LARBD_MJCWPR_R_DMTIMARBEN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_LARBD_MJCWPR_R_DMARBRST Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_LARBD_MJCR_R_DMAG0EN Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_LARBD_MJCR_R_DMAG0TIM Fld(4,17,AC_MSKB2)//[20:17]
    #define MJC_LARBD_MJCR_R_DMAGBMBEG Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_LARBD_MJCR_R_DMAGBMEND Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_LARBD_MJCR_R_DMAGBMAGID Fld(3,12,AC_MSKB1)//[14:12]
    #define MJC_LARBD_MJCR_R_DMAGBMALL Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_LARBD_MJCR_R_DMTIMARBEN Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_LARBD_MJCR_R_DMARBRST Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_CHB_3_AGNT_MODE Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_DRAM_TEST_PAT_FORCE_RDY Fld(1,7,AC_MSKB0)//[7:7]
#define MJC_STA_FB_00 (MJC_BASE_ADDR + 0x160)
    #define MJC_STA_R2B_EXTRA_NOT_FINISHED Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_LBOX_Y_IS_SMALL Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_LBOX_LOW_LINE Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_LBOX_UP_LINE Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_FB_01 (MJC_BASE_ADDR + 0x164)
    #define MJC_STA_WADDR_OOB Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_RFIFO_WTLV Fld(8,23,AC_MSKW32)//[30:23]
    #define MJC_STA_WFIFO_OVF Fld(1,22,AC_MSKB2)//[22:22]
#define MJC_STA_FB_02 (MJC_BASE_ADDR + 0x168)
    #define MJC_STA_FILM_WOVF Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_STA_FILM_WUDF Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_STA_FILM_ROVF Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_FILM_RUDF Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_LBOX_RIGHT_PIXEL Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_LBOX_LEFT_PIXEL Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_FB_03 (MJC_BASE_ADDR + 0x16C)
    #define MJC_STA_FB_CRC_RESULT_31_0 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_FB_04 (MJC_BASE_ADDR + 0x170)
    #define MJC_STA_FB_CRC_RESULT_63_32 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_FB_05 (MJC_BASE_ADDR + 0x174)
    #define MJC_STA_FB_CRC_RESULT_95_64 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_FB_06 (MJC_BASE_ADDR + 0x178)
    #define MJC_STA_FB_CRC_RESULT_127_96 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FB_LBOX_0 (MJC_BASE_ADDR + 0x17C)
    #define MJC_LBOX_DBG_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_LBOX_AUTO_SET Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_LBOX_RD_FLIP Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_LBOX_3D_OFST Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_LBOX_Y_THRES Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_LBOX_RIGHT_OFFSET Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_LBOX_LEFT_OFFSET Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LBOX_DOWN_OFFSET Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_LBOX_UP_OFFSET Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FB_LBOX_1 (MJC_BASE_ADDR + 0x180)
    #define MJC_LBOX_MANUAL_POS_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_LBOX_MANUAL_POS_DN Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_LBOX_MANUAL_POS_UP Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FB_LBOX_2 (MJC_BASE_ADDR + 0x184)
    #define MJC_LBOX_MANUAL_POS_RT Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_LBOX_MANUAL_POS_LF Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FB_LBOX_3 (MJC_BASE_ADDR + 0x188)
    #define MJC_LBOX_RT_SKIP_3D Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_LBOX_LF_SKIP_3D Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_LBOX_DN_SKIP_3D Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_LBOX_UP_SKIP_3D Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_LBOX_RIGHT_SKIP Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_LBOX_LEFT_SKIP Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LBOX_DOWN_SKIP Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_LBOX_UP_SKIP Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FB_LBOX_4 (MJC_BASE_ADDR + 0x18C)
    #define MJC_LBOX_SA_MC_RT Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_LBOX_SA_MC_LF Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_LBOX_SA_MC_DN Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_LBOX_SA_MC_UP Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_LBOX_SA_ME_RT Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_LBOX_SA_ME_LF Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LBOX_SA_ME_DN Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_LBOX_SA_ME_UP Fld(4,0,AC_MSKB0)//[3:0]
    #define MJC_LBOX_SA_ME_ALL Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FB_LR3D (MJC_BASE_ADDR + 0x190)
    #define MJC_3D_MODE Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_3D_DDP_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_3D_UDP4_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_3D_TYPE Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_3D_LLRR_MODE Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_2D_TO_3D_MODE Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FB_RPT (MJC_BASE_ADDR + 0x194)
    #define MJC_RPT_SHRTDLY_LINE_THD Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_RPT_SHRTDLY_MODE Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_FORCE_RPT_LR_FOR_HPR Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_RPT_FORCE_OUT_LR Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_DRAMW_REQ_SIZE_M1_RPT Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_STA_FB_07 (MJC_BASE_ADDR + 0x19C)
    #define MJC_STA_HS_WDT_INCONSISTENT Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_HS_WDT_INCORRECT Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_LINECNT_INCONSISTENT Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_STA_LINECNT_INCORRECT Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_STA_SHRTDLY_LINE_CNTR Fld(12,10,AC_MSKW21)//[21:10]
    #define MJC_STA_HSYNC_CNTR_BLANK Fld(9,0,AC_MSKW10)//[8:0]
#define MJC_STA_LBOX_R0 (MJC_BASE_ADDR + 0x1A0)
    #define MJC_STA_LBOX_RIGHT_PIXEL_R Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_LBOX_LEFT_PIXEL_R Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_LBOX_R1 (MJC_BASE_ADDR + 0x1A4)
    #define MJC_STA_LBOX_Y_IS_SMALL_R Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_LBOX_LOW_LINE_R Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_LBOX_UP_LINE_R Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FB_APL (MJC_BASE_ADDR + 0x1A8)
    #define MJC_APL_FORCE_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_APL_FORCE_NX Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_APL_FORCE_PV Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_APL_FORCE_CU Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_APL_0 (MJC_BASE_ADDR + 0x1AC)
    #define MJC_STA_APL_Y8_PV Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_APL_Y8_CU Fld(16,0,AC_FULLW10)//[15:0]
	
	//Page FB_2
#define MJC_FB_LBOX_5 (MJC_BASE_ADDR + 0x1B0)
    #define MJC_LBOX_MANUAL_OFST_RT Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_LBOX_MANUAL_OFST_LF Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_LBOX_MANUAL_OFST_DN Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_LBOX_MANUAL_OFST_UP Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_APL_1 (MJC_BASE_ADDR + 0x1B4)
    #define MJC_STA_APL_Y8_NX Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FB_21 (MJC_BASE_ADDR + 0x1B8)
    #define MJC_C_DA_RFIFO_SIZE_M1 Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_C_DA_IN_LR_INV Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_C_DA_PG_SIZE Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_C_DA_WFIFO_SIZE Fld(9,16,AC_MSKW32)//[24:16]
    #define MJC_C_DA_RBRST_SIZE_M1 Fld(3,12,AC_MSKB1)//[14:12]
    #define MJC_C_DA_WBRST_SIZE Fld(9,0,AC_MSKW10)//[8:0]
#define MJC_FB_19 (MJC_BASE_ADDR + 0x1F4)
    #define MJC_WADDR_OOB Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_FB_20 (MJC_BASE_ADDR + 0x1F8)
    #define MJC_LARB_PR_R_DMAG2EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_LARB_PR_R_DMAG3EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_LARB_PR_R_DMAG4EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_LARB_PR_R_DMAG5EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_LARB_PR_R_DMAG6EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_LARB_PR_R_DMAG7EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_LARB_PR_R_DMAG2TIM Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_LARB_PR_R_DMAG3TIM Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_LARB_PR_R_DMAG4TIM Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_LARB_PR_R_DMAG5TIM Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LARB_PR_R_DMAG6TIM Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_LARB_PR_R_DMAG7TIM Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FB_ATPG (MJC_BASE_ADDR + 0x1FC)
    #define MJC_FB_RESERVED2 Fld(30,2,AC_MSKDW)//[31:2]
    #define MJC_FB_RESERVED2_FPR_INV Fld(1,28,AC_MSKB3)//[28:28]	
    #define MANUAL_POS_LR_EN_R Fld(1,27,AC_MSKB3)//[27:27]	
    #define MANUAL_POS_LR_EN_L Fld(1,26,AC_MSKB3)//[26:26]  
    #define MANUAL_POS_UP_R Fld(12,14,AC_MSKDW)//[25:14]  
    #define MANUAL_POS_DN_R  Fld(12,2,AC_MSKW10)//[13:2]
    #define MJC_FB_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FB_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_STA_LARB_0 (MJC_BASE_ADDR + 0x794)
    #define MJC_STA_LARB_MJCW_REQUEST_QUEUE_EMPTY Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_LARB_PR_REQUEST_QUEUE_EMPTY Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_STA_LARBD_MJCWPR_REQUEST_QUEUE_EMPTY Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_STA_LARBD_MJCR_REQUEST_QUEUE_EMPTY Fld(1,28,AC_MSKB3)//[28:28]
#define MJC_STA_LARB_1 (MJC_BASE_ADDR + 0x798)
    #define MJC_STA_LARB_MJCW_BM_LENGTH_COUNTER Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_2 (MJC_BASE_ADDR + 0x79C)
    #define MJC_STA_LARB_PR_BM_LENGTH_COUNTER Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_3 (MJC_BASE_ADDR + 0x7A0)
    #define MJC_STA_LARBD_MJCWPR_BM_LENGTH_COUNTER_CHA Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_4 (MJC_BASE_ADDR + 0x7A4)
    #define MJC_STA_LARBD_MJCWPR_BM_LENGTH_COUNTER_CHB Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_5 (MJC_BASE_ADDR + 0x7A8)
    #define MJC_STA_LARBD_MJCR_BM_LENGTH_COUNTER_CHA Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_6 (MJC_BASE_ADDR + 0x7AC)
    #define MJC_STA_LARBD_MJCR_BM_LENGTH_COUNTER_CHB Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_7 (MJC_BASE_ADDR + 0x7B0)
    #define MJC_STA_LARB_MJCW_BM_CYCLE_COUNTER Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_8 (MJC_BASE_ADDR + 0x7B4)
    #define MJC_STA_LARB_PR_BM_CYCLE_COUNTER Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_9 (MJC_BASE_ADDR + 0x7B8)
    #define MJC_STA_LARBD_MJCWPR_BM_CYCLE_COUNTER_CHA Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_10 (MJC_BASE_ADDR + 0x7BC)
    #define MJC_STA_LARBD_MJCWPR_BM_CYCLE_COUNTER_CHB Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_11 (MJC_BASE_ADDR + 0x7C0)
    #define MJC_STA_LARBD_MJCR_BM_CYCLE_COUNTER_CHA Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_12 (MJC_BASE_ADDR + 0x7C4)
    #define MJC_STA_LARBD_MJCR_BM_CYCLE_COUNTER_CHB Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_13 (MJC_BASE_ADDR + 0x7C8)
    #define MJC_STA_LARBD_MJCWPR_BM_LENGTH_COUNTER_CHBP Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_LARB_14 (MJC_BASE_ADDR + 0x7CC)
    #define MJC_STA_LARBD_MJCWPR_BM_CYCLE_COUNTER_CHBP Fld(32,0,AC_FULLDW)//[31:0]
	
	//Page FD
#define MJC_FD_00 (MJC_BASE_ADDR + 0x700)
    #define MJC_FD_BOUND_AUTO Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FD_RESET_STA Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_FD_IGNORE_MOV_FRM Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_FD_IGNORE_V_SUB Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_FD_IGNORE_H_SUB Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FD_IGNORE_MOV_BLK Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_FD_SAW_BD_LR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_FD_SAW_BD_UD Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_FD_3DFS_MODE Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_FD_SRC_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FD_INFO_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FD_01 (MJC_BASE_ADDR + 0x704)
    #define MJC_FD_QUAN_TH4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_FD_QUAN_TH3 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FD_QUAN_TH2 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FD_QUAN_TH1 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_02 (MJC_BASE_ADDR + 0x708)
    #define MJC_FD_QUAN_VALUE2 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_FD_QUAN_VALUE1 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FD_QUAN_TH5 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_03 (MJC_BASE_ADDR + 0x70C)
    #define MJC_FD_QUAN_VALUE6 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_FD_QUAN_VALUE5 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FD_QUAN_VALUE4 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FD_QUAN_VALUE3 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_04 (MJC_BASE_ADDR + 0x710)
    #define MJC_C_FD_VST_FRM_THR Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_C_FD_VST_BLK_THR Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_C_FD_VST_PXL_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_C_FD_VST_SFT_1 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_C_FD_VST_SFT_0 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_FD_MOV_BLK_FRM_THR Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_05 (MJC_BASE_ADDR + 0x714)
    #define MJC_FD_H_SUB_FRM_THR Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_FD_H_SUB_BLK_THR Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FD_H_SUB_SUM_TH2 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FD_H_SUB_SUM_TH1 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_06 (MJC_BASE_ADDR + 0x718)
    #define MJC_FD_NO_SUB Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FD_DBG_INK_SEL Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_FD_SEQ_FRM_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_FD_BAD_RST_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_FD_DC_CNT Fld(3,20,AC_MSKB2)//[22:20]
    #define MJC_FD_SEQ_LEN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_FD_MOV_FRM_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FD_MOV_PXL_TH2 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_FD_MOV_PXL_TH1 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FD_07 (MJC_BASE_ADDR + 0x71C)
    #define MJC_FD_VST_BLK_SEL Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_FD_HST_BLK_SEL Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_FD_MOV_FRM_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_FD_MOV_FRM_MAX Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FD_08 (MJC_BASE_ADDR + 0x720)
    #define MJC_STA_FD_ADD_DET Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_FD_CASE Fld(14,17,AC_MSKW32)//[30:17]
    #define MJC_STA_FD_BAD_DET Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_STA_FD_TRIG Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_STA_FD_INFO Fld(3,0,AC_MSKB0)//[2:0]
#define MJC_FD_09 (MJC_BASE_ADDR + 0x724)
    #define MJC_STA_VST_SEQ Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_SAW_SEQ Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FD_0A (MJC_BASE_ADDR + 0x728)
    #define MJC_STA_MOV_SEQ Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_DIF_SEQ Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_FD_0B (MJC_BASE_ADDR + 0x72C)
    #define MJC_STA_FD_DBG Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FD_0D (MJC_BASE_ADDR + 0x734)
    #define MJC_FD_RT_SKIP Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_FD_LF_SKIP Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_FD_DN_SKIP Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_FD_UP_SKIP Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_FD_Y_THRES Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FD_BD_0 (MJC_BASE_ADDR + 0x738)
    #define MJC_FD_BD_DN Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_FD_BD_UP Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_FD_BD_1 (MJC_BASE_ADDR + 0x73C)
    #define MJC_FD_BD_RT Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_FD_BD_LF Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_FD_SEQ (MJC_BASE_ADDR + 0x740)
    #define MJC_STA_FD_DET_SEQ_R Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_FD_DET_SEQ_L Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_HST (MJC_BASE_ADDR + 0x744)
    #define MJC_STA_HST_FRM_SUM Fld(22,0,AC_MSKDW)//[21:0]
#define MJC_STA_VST (MJC_BASE_ADDR + 0x748)
    #define MJC_STA_VST_FRM_SUM Fld(22,0,AC_MSKDW)//[21:0]
#define MJC_STA_MOV (MJC_BASE_ADDR + 0x74C)
    #define MJC_STA_MOV_FRM_SUM Fld(22,0,AC_MSKDW)//[21:0]
#define MJC_FD_ATPG (MJC_BASE_ADDR + 0x7FC)
    #define MJC_FD_RESERVED Fld(30,2,AC_MSKDW)//[31:2]
    #define MJC_FD_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FD_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
	//Page FM_1
#define MJC_FM_00 (MJC_BASE_ADDR + 0x600)
    #define MJC_LR3D_IN_FREERUN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_VIDEO_PRTC_OFF Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FILM_PRTC_OFF Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_FRZ_MC_ORI_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_DRP_LIKE_PATCH Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_PPC_FORCE_INTLV Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_FREEZE_NUM_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FALLBACK_64 Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_ME_PRLST_PREBI Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_OUT_4X_RATE_EN Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_BYPASS_ARRANGER Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FORCE_22 Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_FORCE_32 Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_KEEP_22 Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_KEEP_32 Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_FM_EXTRA_DLY Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_OUT_DB_RATE_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PSAUTO_22 Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_PSAUTO_32 Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_MIN_DELAY Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_FRZ_DA_RDTYPE Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_DRP_RST_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_PSEUDO_DRP32_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_FALLBACK_55P2 Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_PSEUDO_DRP32_SCNCHG_ONLY Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_SYNC_RST_MANUAL Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_WCNT_SYNC_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_PPC_VIDEO_OFF Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_SCNCHG_FRM_EXT Fld(3,0,AC_MSKB0)//[2:0]
#define MJC_FM_01 (MJC_BASE_ADDR + 0x604)
    #define MJC_FREEZE_PHS Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FREEZE_IDX Fld(4,27,AC_MSKB3)//[30:27]
    #define MJC_FRZ_SCEN_CHG Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_FRZ_ME3_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FRZ_ME2_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_FRZ_ME_POS_SEL Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_FRZ_MVEXP_FLAG Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FRZ_HQRPT_LST Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_FRZ_MEMC_DEN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_EXIT_32_FILM_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_EXIT_22_FILM_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FM_02 (MJC_BASE_ADDR + 0x608)
    #define MJC_PROG_ME2EN_9_20 Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_PROG_ME2EN_8_20 Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_PROG_ME2EN_7_20 Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_PROG_ME2EN_6_20 Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_PROG_ME2EN_5_20 Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_PROG_ME2EN_4_20 Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_PROG_ME2EN_3_20 Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_PROG_ME2EN_2_20 Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PROG_ME2EN_1_20 Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PROG_ME2EN_0_20 Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_PROG_ME2EN_9_10 Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_PROG_ME2EN_8_10 Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_PROG_ME2EN_7_10 Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PROG_ME2EN_6_10 Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_PROG_ME2EN_5_10 Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_PROG_ME2EN_4_10 Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_PROG_ME2EN_3_10 Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PROG_ME2EN_2_10 Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_PROG_ME2EN_1_10 Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_PROG_ME2EN_0_10 Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_PROG_ME2EN_7_8 Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_PROG_ME2EN_6_8 Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_PROG_ME2EN_5_8 Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_PROG_ME2EN_4_8 Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_PROG_ME2EN_3_8 Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_PROG_ME2EN_2_8 Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_PROG_ME2EN_1_8 Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_PROG_ME2EN_0_8 Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_PROG_ME2EN_3_4 Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_PROG_ME2EN_2_4 Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_PROG_ME2EN_1_4 Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_PROG_ME2EN_0_4 Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_03 (MJC_BASE_ADDR + 0x60C)
    #define MJC_PROG_ME3EN_9_20 Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_PROG_ME3EN_8_20 Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_PROG_ME3EN_7_20 Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_PROG_ME3EN_6_20 Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_PROG_ME3EN_5_20 Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_PROG_ME3EN_4_20 Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_PROG_ME3EN_3_20 Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_PROG_ME3EN_2_20 Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PROG_ME3EN_1_20 Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PROG_ME3EN_0_20 Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_PROG_ME3EN_7_8 Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_PROG_ME3EN_6_8 Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_PROG_ME3EN_5_8 Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PROG_ME3EN_4_8 Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_PROG_ME3EN_3_8 Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_PROG_ME3EN_2_8 Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_PROG_ME3EN_1_8 Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PROG_ME3EN_0_8 Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_PROG_ME3EN_3_4 Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_PROG_ME3EN_2_4 Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_PROG_ME3EN_1_4 Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_PROG_ME3EN_0_4 Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_PROG_ME2EN_19_20 Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_PROG_ME2EN_18_20 Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_PROG_ME2EN_17_20 Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_PROG_ME2EN_16_20 Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_PROG_ME2EN_15_20 Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_PROG_ME2EN_14_20 Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_PROG_ME2EN_13_20 Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_PROG_ME2EN_12_20 Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_PROG_ME2EN_11_20 Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_PROG_ME2EN_10_20 Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_04 (MJC_BASE_ADDR + 0x610)
    #define MJC_WRT_PTR_MSB_MSK Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_ME_24_60_PREBI_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FRZ_LR3D_INFO_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_FRZ_LR3D_INFO Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_FRZ_ME_FILM_FLG Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_FORCE_FALLBACK_64 Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FORCE_FALLBACK_55P2 Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_NF_NDRP_DB_DLY7 Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_240HZ_LR_MODE Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_PROG_ME3EN_9_10 Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PROG_ME3EN_8_10 Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_PROG_ME3EN_7_10 Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_PROG_ME3EN_6_10 Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_PROG_ME3EN_5_10 Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PROG_ME3EN_4_10 Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_PROG_ME3EN_3_10 Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_PROG_ME3EN_2_10 Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_PROG_ME3EN_1_10 Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_PROG_ME3EN_0_10 Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_PROG_ME3EN_19_20 Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_PROG_ME3EN_18_20 Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_PROG_ME3EN_17_20 Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_PROG_ME3EN_16_20 Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_PROG_ME3EN_15_20 Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_PROG_ME3EN_14_20 Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_PROG_ME3EN_13_20 Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_PROG_ME3EN_12_20 Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_PROG_ME3EN_11_20 Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_PROG_ME3EN_10_20 Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_05 (MJC_BASE_ADDR + 0x614)
    #define MJC_FW_TD_LR_SEL_OUT Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FW_INT_NUM Fld(5,26,AC_MSKB3)//[30:26]
    #define MJC_FW_ME_FILM_FLG Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_FW_ME_FILM_PHS Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_FW_ME_FILM_IDX Fld(4,19,AC_MSKB2)//[22:19]
    #define MJC_FW_FRM_NEXT Fld(5,14,AC_MSKW21)//[18:14]
    #define MJC_FW_FRM_CURR Fld(5,9,AC_MSKB1)//[13:9]
    #define MJC_FW_FRM_PREV Fld(5,4,AC_MSKW10)//[8:4]
    #define MJC_FW_RESCAN_FRM_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_CLEAR_FW_TOO_LATE Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_FW_SET_OK Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FW_FRM_SCH_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_06 (MJC_BASE_ADDR + 0x618)
    #define MJC_FW_TD_LR_TOGGLE Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FW_WRT_CTRL_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FW_WRT_FRM_PTR Fld(5,25,AC_MSKB3)//[29:25]
    #define MJC_FW_WRT_FRM_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_FW_PAIR_FIRST Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_FW_PPC_INTLV_SEL Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_FW_FALLBACK_64 Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FW_PAIR_LAST Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_FW_MVEXP_FLAG Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_FW_ME_24_60_PREBI Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_FW_PPC_FRM_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_FW_MC_ORI_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_FW_PPC_MODE_SEL Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_FW_ME_POS_OTHR_SEL Fld(2,13,AC_MSKB1)//[14:13]
    #define MJC_FW_PAIR_FIRST_DLY Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_FW_DA_RDCTYPE Fld(2,3,AC_MSKB0)//[4:3]
    #define MJC_FW_DA_BWRDC_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_FW_ME3_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FW_ME2_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_07 (MJC_BASE_ADDR + 0x61C)
    #define MJC_FW_FRM_CHG_TTD Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FW_VIDEO_NO_INTP Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FW_FRM_DYSW Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_FW_HQRPT_LST Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_FW_FST_HQRPT_GP Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_FW_SEC_HQRPT_CHG Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FW_HQRPT_CHG Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_FW_FALLBACK_55P2 Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_FW_INT_DEN Fld(4,19,AC_MSKB2)//[22:19]
    #define MJC_FW_MEMC_CCO_DECI_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_FW_MEMC_PCO_DECI_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_FRM_WSYNC_PROG_SEL Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_FRM_WSYNC_PROG_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_FRM_WCNT_DELAY Fld(4,11,AC_MSKB1)//[14:11]
    #define MJC_FRM_LINE_THR2 Fld(4,7,AC_MSKW10)//[10:7]
    #define MJC_FRM_LINE_THR Fld(4,3,AC_MSKB0)//[6:3]
    #define MJC_FRM_SYNC_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_FREEZE_3D_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FRM_VSYNC_IN_RST_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_08 (MJC_BASE_ADDR + 0x620)
    #define MJC_PROG_RDCTYPE_3_10 Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PROG_RDCTYPE_2_10 Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PROG_RDCTYPE_1_10 Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_PROG_RDCTYPE_0_10 Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_PROG_RDCTYPE_7_8 Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_PROG_RDCTYPE_6_8 Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_PROG_RDCTYPE_5_8 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_RDCTYPE_4_8 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_RDCTYPE_3_8 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_RDCTYPE_2_8 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_RDCTYPE_1_8 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_RDCTYPE_0_8 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_RDCTYPE_3_4 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_RDCTYPE_2_4 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_RDCTYPE_1_4 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_RDCTYPE_0_4 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_FM_09 (MJC_BASE_ADDR + 0x624)
    #define MJC_FRZ_PAIR_LAST Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_SCN_CHG_FRM_SEL Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_FRZ_FRM_CHG_TTD Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FRZ_VIDEO_NO_INTP Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_PPC_EN_FRM_SEQ Fld(20,0,AC_MSKDW)//[19:0]
#define MJC_FM_0A (MJC_BASE_ADDR + 0x628)
    #define MJC_FW_FRM_NEXT_R Fld(5,10,AC_MSKB1)//[14:10]
    #define MJC_FW_FRM_CURR_R Fld(5,5,AC_MSKW10)//[9:5]
    #define MJC_FW_FRM_PREV_R Fld(5,0,AC_MSKB0)//[4:0]
#define MJC_FM_10 (MJC_BASE_ADDR + 0x640)
    #define MJC_PROG_ME_POS_OTHR_3_10 Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PROG_ME_POS_OTHR_2_10 Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PROG_ME_POS_OTHR_1_10 Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_PROG_ME_POS_OTHR_0_10 Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_PROG_ME_POS_OTHR_7_8 Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_PROG_ME_POS_OTHR_6_8 Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_PROG_ME_POS_OTHR_5_8 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_ME_POS_OTHR_4_8 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_ME_POS_OTHR_3_8 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_ME_POS_OTHR_2_8 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_ME_POS_OTHR_1_8 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_ME_POS_OTHR_0_8 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_ME_POS_OTHR_3_4 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_ME_POS_OTHR_2_4 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_ME_POS_OTHR_1_4 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_ME_POS_OTHR_0_4 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_FM_11 (MJC_BASE_ADDR + 0x644)
    #define MJC_PROG_ME_POS_OTHR_9_20 Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PROG_ME_POS_OTHR_8_20 Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PROG_ME_POS_OTHR_7_20 Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_PROG_ME_POS_OTHR_6_20 Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_PROG_ME_POS_OTHR_5_20 Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_PROG_ME_POS_OTHR_4_20 Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_PROG_ME_POS_OTHR_3_20 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_ME_POS_OTHR_2_20 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_ME_POS_OTHR_1_20 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_ME_POS_OTHR_0_20 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_ME_POS_OTHR_9_10 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_ME_POS_OTHR_8_10 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_ME_POS_OTHR_7_10 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_ME_POS_OTHR_6_10 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_ME_POS_OTHR_5_10 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_ME_POS_OTHR_4_10 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_FM_12 (MJC_BASE_ADDR + 0x648)
    #define MJC_PROG_ME_POS_OTHR_19_20 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_ME_POS_OTHR_18_20 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_ME_POS_OTHR_17_20 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_ME_POS_OTHR_16_20 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_ME_POS_OTHR_15_20 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_ME_POS_OTHR_14_20 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_ME_POS_OTHR_13_20 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_ME_POS_OTHR_12_20 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_ME_POS_OTHR_11_20 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_ME_POS_OTHR_10_20 Fld(2,0,AC_MSKB0)//[1:0]
	
	//Page FM_2
#define MJC_FM_13 (MJC_BASE_ADDR + 0x64C)
    #define MJC_PROG_RDCTYPE_9_20 Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PROG_RDCTYPE_8_20 Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PROG_RDCTYPE_7_20 Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_PROG_RDCTYPE_6_20 Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_PROG_RDCTYPE_5_20 Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_PROG_RDCTYPE_4_20 Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_PROG_RDCTYPE_3_20 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_RDCTYPE_2_20 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_RDCTYPE_1_20 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_RDCTYPE_0_20 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_RDCTYPE_9_10 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_RDCTYPE_8_10 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_RDCTYPE_7_10 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_RDCTYPE_6_10 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_RDCTYPE_5_10 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_RDCTYPE_4_10 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_FM_14 (MJC_BASE_ADDR + 0x650)
    #define MJC_FRZ_FST_HQRPT_GP Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_FRZ_SEC_HQRPT_CHG Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FRZ_HQRPT_CHG Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_FRZ_INTP_POS Fld(5,24,AC_MSKB3)//[28:24]
    #define MJC_FRZ_MEMC_CCO_DECI_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_FRZ_MEMC_PCO_DECI_EN Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_FRZ_ME_24_60_PREBI Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FRZ_ME_IDX_TOGGLE Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_PROG_RDCTYPE_19_20 Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PROG_RDCTYPE_18_20 Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_PROG_RDCTYPE_17_20 Fld(2,14,AC_MSKB1)//[15:14]
    #define MJC_PROG_RDCTYPE_16_20 Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_PROG_RDCTYPE_15_20 Fld(2,10,AC_MSKB1)//[11:10]
    #define MJC_PROG_RDCTYPE_14_20 Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_PROG_RDCTYPE_13_20 Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_PROG_RDCTYPE_12_20 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_PROG_RDCTYPE_11_20 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_PROG_RDCTYPE_10_20 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_FM_15 (MJC_BASE_ADDR + 0x654)
    #define MJC_FW_FRM_SCH_CTRL Fld(32,0,AC_FULLDW)//[31:0]
    #define MJC_FW_FRM_MVEXP Fld(1,20,AC_MSKB2)//[20]
#define MJC_FM_TRK (MJC_BASE_ADDR + 0x658)
    #define MJC_OUT_SYNC_SEL Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_FRM_MASK_OUT_LR_SEL Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_FRM_MASK_OUT_LR_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_FRM_MASK_LR_SEL Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_FRM_MASK_LR_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_FRM_MASK_O Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_FRM_MASK_I Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_FM_16 (MJC_BASE_ADDR + 0x65C)
    #define MJC_FW_MC_NUM_RGN2 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FW_MC_NUM_RGN1 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FW_MC_NUM_RGN0 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FM_REV0 (MJC_BASE_ADDR + 0x6A4)
    #define MJC_FM_RESERVED0 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_FM_04 (MJC_BASE_ADDR + 0x6A8)
    #define MJC_STA_PAIR_FIRST_DLY_HW Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_DA_FRM_NEXT_R_HW Fld(5,10,AC_MSKB1)//[14:10]
    #define MJC_STA_DA_FRM_CURR_R_HW Fld(5,5,AC_MSKW10)//[9:5]
    #define MJC_STA_DA_FRM_PREV_R_HW Fld(5,0,AC_MSKB0)//[4:0]
#define MJC_STA_FM_00 (MJC_BASE_ADDR + 0x6AC)
    #define MJC_STA_FRM_VSYNC_IN_EARLY_2 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_FRM_VSYNC_IN_LATE_2 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_FM_01 (MJC_BASE_ADDR + 0x6B0)
    #define MJC_STA_FILM_R_FIRST Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_PAIR_LAST_HW Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_STA_LR3D_O_HW Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_STA_PS_FILM_ID_HW Fld(3,26,AC_MSKB3)//[28:26]
    #define MJC_STA_FRM_CHG_TTD_HW Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_VIDEO_NO_INTP_HW Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_FRM_VSYNC_IN_EARLY Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_STA_FRM_VSYNC_IN_LATE Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_STA_FM_02 (MJC_BASE_ADDR + 0x6B4)
    #define MJC_STA_MEMC_NUM_HW Fld(5,27,AC_MSKB3)//[31:27]
    #define MJC_STA_MEMC_DEN_HW Fld(4,23,AC_MSKW32)//[26:23]
    #define MJC_STA_DA_FRM_NEXT_HW Fld(5,18,AC_MSKB2)//[22:18]
    #define MJC_STA_DA_FRM_CURR_HW Fld(5,13,AC_MSKW21)//[17:13]
    #define MJC_STA_DA_FRM_PREV_HW Fld(5,8,AC_MSKB1)//[12:8]
    #define MJC_STA_PAIR_FIRST_HW Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_STA_ME_FILM_FLG_HW Fld(2,5,AC_MSKB0)//[6:5]
    #define MJC_STA_ME_FILM_PHS_HW Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_STA_ME_FILM_IDX_HW Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_STA_FM_03 (MJC_BASE_ADDR + 0x6B8)
    #define MJC_STA_HQRPT_LST_HW Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_MVEXP_FLAG_HW Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_STA_FW_IS_TOO_LATE Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_STA_FST_HQRPT_GP_HW Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_STA_SEC_HQRPT_CHG_HW Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_STA_HQRPT_CHG_HW Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_STA_FALLBACK_55P2_HW Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_TD_LR_TOGGLE_HW Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_TD_LR_SEL_OUT Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_STA_MEMC_CCO_DECI_EN_HW Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_STA_MEMC_PCO_DECI_EN_HW Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_STA_WRT_FRM_PTR_HW Fld(5,16,AC_MSKB2)//[20:16]
    #define MJC_STA_FALLBACK_64_HW Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_STA_DA_RDCTYPE_HW Fld(2,13,AC_MSKB1)//[14:13]
    #define MJC_STA_DA_BWRDC_EN_HW Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_STA_MC_ORI_EN_HW Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_STA_ME_24_60_PREBI_HW Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_STA_ME3_EN_HW Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_STA_ME2_EN_HW Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_STA_PROG_ME_POS_OTHR_SEL_HW Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_STA_RESCAN_FRM_EN_HW Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_STA_PPC_INTLV_SEL_HW Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_STA_PPC_MODE_SEL_HW Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_STA_PPC_FRM_EN_HW Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_STA_LR3D_I Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_STA_LR3D_INFO_ONMJCFSH_HW Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_FM_ATPG (MJC_BASE_ADDR + 0x6BC)
    #define MJC_FM_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FM_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page ME_1
#define MJC_ME_00 (MJC_BASE_ADDR + 0x200)
    #define MJC_ME_WDV_BLK Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_FORCE_POS_SHFT_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_3D_SCNCHG_MODE Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_BEC_SAD_TH Fld(15,8,AC_MSKW21)//[22:8]
    #define MJC_BEC_BLK_TH Fld(3,4,AC_MSKB0)//[6:4]
#define MJC_ME_01 (MJC_BASE_ADDR + 0x204)
    #define MJC_SAD_YC_MIX_SEL Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_VAR_COR_TH Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_SAD_COR_TH Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_ZEROMV_SAD_TH Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_02 (MJC_BASE_ADDR + 0x208)
    #define MJC_SCNCHG_ZR_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_SCNCHG_FRST_LTCH_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_SCNCHG_MTHD Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_VAR_COR_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_SAD_COR_SEL Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MV_RMP_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_SCNCHG_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_SCNCHG_SEL_TH Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_ME_03 (MJC_BASE_ADDR + 0x20C)
    #define MJC_ZERO_SAD_PENLTY Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_RAND_SAD_PENLTY Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_TMPR_SAD_PENLTY_S Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_SPAT_SAD_PENLTY Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_04 (MJC_BASE_ADDR + 0x210)
    #define MJC_BD_ZERO_SAD_PENLTY Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_BD_RAND_SAD_PENLTY Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_BD_TMPR_SAD_PENLTY Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_BD_SPAT_SAD_PENLTY Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_05 (MJC_BASE_ADDR + 0x214)
    #define MJC_TMPR_2ND_PENLTY_S Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_SMOOTH_SAD_PENLTY Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_SMOOTH_TH Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_06 (MJC_BASE_ADDR + 0x218)
    #define MJC_ME_BND_DOWN Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ME_BND_UP Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ME_BND_RIGHT Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_ME_BND_LEFT Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_07 (MJC_BASE_ADDR + 0x21C)
    #define MJC_SCNCHG_SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_SCNCHG_TH1 Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_SCNCHG_TH2 Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_SCNCHG_TH3 Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_ME_08 (MJC_BASE_ADDR + 0x220)
    #define MJC_SCNCHG_TH4 Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_ME_09 (MJC_BASE_ADDR + 0x224)
    #define MJC_SMOOTH_BLK_TH Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_ME_MVPNLTY_SEL Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PNLTY_BLKTH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_ZERO_SAD_BLKTH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_ME_0A (MJC_BASE_ADDR + 0x228)
    #define MJC_HLR_BND_V Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_TMPR_BLK_TH Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_TMPR_SAD_PENLTY_M Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_TMPR_2ND_PENLTY_M Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_0B (MJC_BASE_ADDR + 0x22C)
    #define MJC_HLR_FG_V_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_HLR_FG_H_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_0C (MJC_BASE_ADDR + 0x230)
    #define MJC_LBOX_LR_ACT_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_LBOX_UD_ACT_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_HLR_BG_V_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_HLR_BG_H_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_0D (MJC_BASE_ADDR + 0x234)
    #define MJC_ADAP_GAIN_ZB_AREA_TH2 Fld(3,17,AC_MSKB2)//[19:17]
    #define MJC_LBOX_CAND_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_ADAP_GAIN_ZB_AREA_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_0E (MJC_BASE_ADDR + 0x238)
    #define MJC_SCNCHG_APL_TH Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_ME_0F (MJC_BASE_ADDR + 0x23C)
    #define MJC_ME_RESERVED1 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_ME_10 (MJC_BASE_ADDR + 0x240)
    #define MJC_GMV_PENLTY Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_EDGEV_PENALTY_MED_TH Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_ME_LPF_EDGE_TH Fld(8,8,AC_FULLB1)//[15:8]
#define MJC_ME_12 (MJC_BASE_ADDR + 0x248)
    #define MJC_ME_RESERVED2 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_ME_13 (MJC_BASE_ADDR + 0x24C)
    #define MJC_ME_RESERVED3 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_ME_14 (MJC_BASE_ADDR + 0x250)
    #define MJC_ME_RESERVED4 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_ME_15 (MJC_BASE_ADDR + 0x254)
    #define MJC_ME_RESERVED5 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_ME_16 (MJC_BASE_ADDR + 0x258)
    #define MJC_MIN_BLK_SAD_TH Fld(15,16,AC_MSKW32)//[30:16]
    #define MJC_ZEROMV_SAD_TH2 Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_17 (MJC_BASE_ADDR + 0x25C)
    #define MJC_SMOOTH_STA_TH2 Fld(15,16,AC_MSKW32)//[30:16]
    #define MJC_SMOOTH_STA_TH Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_18 (MJC_BASE_ADDR + 0x260)
    #define MJC_MVMED_NZBLK_SEL Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_MV_PNLTY_RNG Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PNLTY_BLKTH2 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_PNLTY_EG_TH Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_EDGE_PENALTY_FM_TH Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_ME_19 (MJC_BASE_ADDR + 0x264)
    #define MJC_NOZERO_MV_EN Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_FG_HLR_TYPE Fld(3,27,AC_MSKB3)//[29:27]
    #define MJC_HLR_SEL Fld(2,25,AC_MSKB3)//[26:25]
    #define MJC_HLR_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_HLR_BND_H Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_HLR_TH_H Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_HLR_TH_V Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_1A (MJC_BASE_ADDR + 0x268)
    #define MJC_ME_STIL_SAD_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ME_STIL_LBOX_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_ME_STIL_ZP_OFF Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_ME_STIL_BLKTH_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_ME_STIL_EG_OFF Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_VLRG_NZMV_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_ADAP_NZMV_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_NZMV_TH1 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_NZMV_TH2 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_1B (MJC_BASE_ADDR + 0x26C)
    #define MJC_EDGEH_WDBLK Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_EDGEV_WDBLK Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_EDGEV_OFF Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_EDGE_PENALTY_SEL Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_EDGE_PENALTY_TH1 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_EDGE_PENALTY_TH2 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_1C (MJC_BASE_ADDR + 0x270)
    #define MJC_SMOOTH_BLK_TH2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_SMOOTH_RNG Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_SMOOTH_SAD_PENLTY2 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_SMOOTH_TH2 Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_1D (MJC_BASE_ADDR + 0x274)
    #define MJC_ME_LPF_WD Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_EDGE_PENALTY_MED_TH Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_HLR_GMV_PRTC_OFF Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_EDGE0_STA_TH Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_HLR_GMV_TH_H Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_HLR_GMV_TH_V Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_1E (MJC_BASE_ADDR + 0x278)
    #define MJC_ME_LPF_SEL Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_ZERO_SMOOTH_PNLTY_EN Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_ADAP_GAIN_SEL_X Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_ADAP_GAIN_SEL_Y Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_ADAP_GAIN_MVEQ_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ADAP_GAIN_ZB_TH Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_1F (MJC_BASE_ADDR + 0x27C)
    #define MJC_BND_HLR_TH_H Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_BND_HLR_TH_V Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_BND_HLR_GMV_TH_H Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_BND_HLR_GMV_TH_V Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_20 (MJC_BASE_ADDR + 0x280)
    #define MJC_ADAP_GAIN_EG_RNG Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ADAP_GAIN_EG_TH2 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_3FRM_SEL_SAD_TH Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_ME_21 (MJC_BASE_ADDR + 0x284)
    #define MJC_HLR_OTHR_SEL Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_BND_NZMV_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_BND_NZMV_UP_BND Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_BND_NZMV_DN_BND Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_BND_NZMV_LF_BND Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_BND_NZMV_RT_BND Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_ME_22 (MJC_BASE_ADDR + 0x288)
    #define MJC_3FRM_HLR_TH_H Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_3FRM_HLR_TH_V Fld(8,16,AC_FULLB2)//[23:16]
#define MJC_ME_23 (MJC_BASE_ADDR + 0x28C)
    #define MJC_3FRM_EDGEH_WDBLK Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_3FRM_EDGEV_WDBLK Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_3FRM_EDGEV_OFF Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_3FRM_EDGE_PENALTY_SEL Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_3FRM_EDGE_PENALTY_TH1 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_3FRM_EDGE_PENALTY_TH2 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_24 (MJC_BASE_ADDR + 0x290)
    #define MJC_3FRM_SMOOTH_BLK_TH2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_3FRM_SMOOTH_RNG Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_3FRM_SMOOTH_SAD_PENLTY2 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_3FRM_SMOOTH_TH2 Fld(15,0,AC_MSKW10)//[14:0]
#define MJC_ME_25 (MJC_BASE_ADDR + 0x294)
    #define MJC_3FRM_MVPNLTY_SEL Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_3FRM_SAD_OCC_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_3FRM_STA_SEL Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_3FRM_SMOOTH_SAD_PENLTY Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_3FRM_EDGE_PENALTY_MED_TH Fld(7,8,AC_MSKB1)//[14:8]
    #define MJC_3FRM_EDGEV_PENALTY_MED_TH Fld(7,0,AC_MSKB0)//[6:0]
#define MJC_ME_26 (MJC_BASE_ADDR + 0x298)
    #define MJC_ME_MMPNLTY_SEL_2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_ADAP_GAIN_SEL2_X Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_ADAP_GAIN_SEL2_Y Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_ADAP_GAIN_EG_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ME_PPC_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_ME_27 (MJC_BASE_ADDR + 0x29C)
    #define MJC_ME_MMPNLTY_EGTH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ME_MMPNLTY_SEL Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_ME_MFPNLTY_SEL_1 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_ME_MFPNLTY_SEL_2 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_ME_MMPNLTY_BLKTH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_ME_MFPNLTY_BLKTH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_ME_28 (MJC_BASE_ADDR + 0x2A0)
    #define MJC_ME_MFPNLTY_EGTH1 Fld(8,24,AC_FULLB3)//[31:24]
#define MJC_ME_29 (MJC_BASE_ADDR + 0x2A4)
    #define MJC_ME_MF_GLBV_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ME2_MF_ME3OCC_OFF Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_ME2_MF_OCC_OFF Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_ME3_MF_OCC_OFF Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_ME_LBOX_AUTO Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_MF_LBOX_CLIP_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_ME_MFPNLTY_EGTH2 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_2A (MJC_BASE_ADDR + 0x2A8)
    #define MJC_ME_WDV_BLK_UP_BND Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_ME_WDV_BLK_DN_BND Fld(8,16,AC_FULLB2)//[23:16]
#define MJC_ME_2B (MJC_BASE_ADDR + 0x2AC)
    #define MJC_PPC_ADAP_OFF Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_PPC_ADAP_OFF_RNG Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_PPC_ADAP_OFF_EG_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_2C (MJC_BASE_ADDR + 0x2B0)
    #define MJC_ME_GMV_PNLTY2_MV_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ME_GMVBLK_EQ_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_GMV_PENLTY2 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_ME_00 (MJC_BASE_ADDR + 0x300)
    #define MJC_SUM_ZERO_SAD_SEL Fld(31,0,AC_MSKDW)//[30:0]
#define MJC_STA_ME_01 (MJC_BASE_ADDR + 0x304)
    #define MJC_LES_SEL Fld(31,0,AC_MSKDW)//[30:0]
#define MJC_STA_ME_02 (MJC_BASE_ADDR + 0x308)
    #define MJC_SCNCHG_OK Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_FINAL_CAND_NUM Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_BEC Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_03 (MJC_BASE_ADDR + 0x30C)
    #define MJC_SUM_MIN_BLK Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_SUM_BLKZERO2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_04 (MJC_BASE_ADDR + 0x310)
    #define MJC_SUM_SMOOTH_BLK2 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_SUM_SMOOTH_BLK Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_05 (MJC_BASE_ADDR + 0x314)
    #define MJC_SUM_ZERO_VAR_PREV Fld(30,0,AC_MSKDW)//[29:0]
#define MJC_STA_ME_06 (MJC_BASE_ADDR + 0x318)
    #define MJC_SUM_ZERO_VAR_CURR Fld(30,0,AC_MSKDW)//[29:0]
#define MJC_STA_ME_07 (MJC_BASE_ADDR + 0x31C)
    #define MJC_SUM_MIN_VAR_PREV Fld(30,0,AC_MSKDW)//[29:0]
#define MJC_STA_ME_08 (MJC_BASE_ADDR + 0x320)
    #define MJC_SUM_MIN_VAR_CURR Fld(30,0,AC_MSKDW)//[29:0]
#define MJC_STA_ME_09 (MJC_BASE_ADDR + 0x324)
    #define MJC_SUM_ZBSAD_VAR Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_ME_0A (MJC_BASE_ADDR + 0x328)
    #define MJC_LES_VAR Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_ME_0B (MJC_BASE_ADDR + 0x32C)
    #define MJC_SUM_BLKZERO Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_0C (MJC_BASE_ADDR + 0x330)
    #define MJC_ZERO_EDGE0_CNT Fld(23,0,AC_MSKDW)//[22:0]
#define MJC_STA_ME_0D (MJC_BASE_ADDR + 0x334)
    #define MJC_ZERO_EDGE1_CNT Fld(23,0,AC_MSKDW)//[22:0]
#define MJC_STA_ME_0E (MJC_BASE_ADDR + 0x338)
    #define MJC_ZERO_EDGE0_BLK Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_ZERO_EDGE1_BLK Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_0F (MJC_BASE_ADDR + 0x33C)
    #define MJC_ME3_SUM_ZERO_SAD Fld(31,0,AC_MSKDW)//[30:0]
#define MJC_STA_ME_10 (MJC_BASE_ADDR + 0x340)
    #define MJC_ME3_LES Fld(31,0,AC_MSKDW)//[30:0]
#define MJC_STA_ME_11 (MJC_BASE_ADDR + 0x344)
    #define MJC_ME3_SUM_MIN_BLK Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_ME3_SUM_BLKZERO2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_12 (MJC_BASE_ADDR + 0x348)
    #define MJC_ME3_ZERO_EDGE1_BLK Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_ME3_SUM_BLKZERO Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_ME_13 (MJC_BASE_ADDR + 0x34C)
    #define MJC_STA_PA_TAR_UP_SAD Fld(25,0,AC_MSKDW)//[24:0]
#define MJC_STA_ME_14 (MJC_BASE_ADDR + 0x350)
    #define MJC_STA_PA_REF_UP_SAD Fld(25,0,AC_MSKDW)//[24:0]
	
	//Page ME_2
#define MJC_STA_ME_15 (MJC_BASE_ADDR + 0x354)
    #define MJC_STA_PA_TAR_DN_SAD Fld(25,0,AC_MSKDW)//[24:0]
#define MJC_STA_ME_16 (MJC_BASE_ADDR + 0x358)
    #define MJC_STA_PA_REF_DN_SAD Fld(25,0,AC_MSKDW)//[24:0]
#define MJC_ME_TBA_0 (MJC_BASE_ADDR + 0x35C)
    #define MJC_ME3_TBA_FMEQ_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_ME3_TBA_FMEQ_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_ME3_RGN_SAD_CLP_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_ME3_VAR_MIN_PCN_SFT Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_ME3_VLPF_SAD_COR_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_ME_TBA_1 (MJC_BASE_ADDR + 0x360)
    #define MJC_ME3_TBA_TZEQ_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_ME3_TBA_TZEQ_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_ME3_TBA_TMEQ_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_ME3_TBA_TMEQ_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_ME3_TBA_SAD_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_ME3_TBA_GMV_SEL Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_ME3_TBA_MGEQ_SEL_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_ME3_TBA_MGEQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_ME3_TBA_MGEQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_ME_CLP_0 (MJC_BASE_ADDR + 0x364)
    #define MJC_ME_EDGE_PENLTY_CLIP_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_ME_MMPNLTY_CLIP_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_ME_MFPNLTY_CLIP_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_ME_MVPNLTY_CLIP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_ME_MMPNLTY_CLIP_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ME_MFPNLTY_CLIP_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_ME_MVPNLTY_CLIP_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_CLP_1 (MJC_BASE_ADDR + 0x368)
    #define MJC_ME3_EDGE_PENLTY_CLIP_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_ME3_MMPNLTY_CLIP_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_ME3_MFPNLTY_CLIP_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_ME3_MVPNLTY_CLIP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_ME3_MMPNLTY_CLIP_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_ME3_MFPNLTY_CLIP_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_ME3_MVPNLTY_CLIP_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_ME_CLP_2 (MJC_BASE_ADDR + 0x36C)
    #define MJC_ME3_EDGE_PENLTY_CLIP_TH Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_ME_EDGE_PENLTY_CLIP_TH Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_ME_TBA_2 (MJC_BASE_ADDR + 0x370)
    #define MJC_ME3_DIR_7TH_CAND_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_ME3_DIR_2TH_CAND_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_ME3_DIR_7TH_CAND_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_ME3_DIR_2TH_CAND_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_ME_ATPG (MJC_BASE_ADDR + 0x3DC)
    #define MJC_ME_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_ME_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page MV_1
#define MJC_MV_00 (MJC_BASE_ADDR + 0x800)
    #define MJC_RAND_SEED_TYPE Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_DRAM_FORCE_MV_WR Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_SORT_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_MV_NO_ERSION Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MV_WRITE_TEST Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_MV_DRAM_WRITE_ZERO Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_TD_START_ADDR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_01 (MJC_BASE_ADDR + 0x804)
    #define MJC_MV_RAND_Y_GAIN Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_MV_RAND_X_GAIN Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_TD_END_ADDR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_02 (MJC_BASE_ADDR + 0x808)
    #define MJC_ME2_USE_ME3TMP Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_PPC_RGNMV_SEL Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_GMV_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_MV_CAND_NUM_LOW Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_CAND_NUM_UP Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_X_BOUND Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_Y_BOUND Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_03 (MJC_BASE_ADDR + 0x80C)
    #define MJC_ME_ONLY_SORT_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_RAND_TABLE_TYPE Fld(2,28,AC_MSKB3)//[29:28]
    #define MJC_MV3_RAND_SRC_SEL Fld(2,26,AC_MSKB3)//[27:26]
    #define MJC_MV2_RAND_SRC_SEL Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_MV_SORT_ORDER Fld(3,20,AC_MSKB2)//[22:20]
    #define MJC_MV_3FRM_Y_BOUND Fld(8,8,AC_FULLB1)//[15:8]
#define MJC_MV_04 (MJC_BASE_ADDR + 0x810)
    #define MJC_MV_CROS_MIN_TH2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_BDR_CNT_TH Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_DIAG_MED_TH Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_CROS_MIN_TH Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_Y_DIF_COR_TH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_X_DIF_COR_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_05 (MJC_BASE_ADDR + 0x814)
    #define MJC_MV_FORCE_VSI_VALUE Fld(6,20,AC_MSKW32)//[25:20]
    #define MJC_MV_BDR_SFT Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_NRM_SFT Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_MIX_SFT Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_NRM_RSSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_MV_BDR_RSSEL Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_MV_06 (MJC_BASE_ADDR + 0x818)
    #define MJC_MV_MIX_BDR_AVG_GAIN Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_MIX_BDR_MED_GAIN Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_MIX_NRM_AVG_GAIN Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_MIX_NRM_MED_GAIN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_BDR_AVG_VSI_GAIN Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_BDR_MED_VSI_GAIN Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_NRM_AVG_VSI_GAIN Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_NRM_MED_VSI_GAIN Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_07 (MJC_BASE_ADDR + 0x81C)
    #define MJC_MV_VSI_FORCE_BDR Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_MV_VSI_MIX_EN Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_MV_VSI_SCH_NRM Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_MV_VSI_FORCE_NRM Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_VSI_DIAG_SEL Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_MV_VSI_BDR_33 Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_MV_VSI_NRM_33 Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MV_LBOX_OVL_NUM Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_LBOX_OVL_OFF Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_MV_LBOX_VSI_NUM Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_LBOX_VSI_BDR Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_08 (MJC_BASE_ADDR + 0x820)
    #define MJC_MV_CMX_SM_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_CMX_SM_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_CMX_EQ_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_CMX_EQ_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_CMX_IGNORE_TPDF Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_MV_CMX_SEL Fld(1,12,AC_MSKB1)//[12:12]
#define MJC_MV_09 (MJC_BASE_ADDR + 0x824)
    #define MJC_MV_OCC_REF_DLT_GAIN Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_MV_OCC_SLF_DLT_GAIN Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_MV_OCC_ERO_GAIN Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_OCC_SMALL_OFF Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_MV_VSI_IGNORE_GDFG Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_MV_VSI_IGNORE_BGMV Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_MV_OCC_IGNORE_FBG Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_MV_OCC_IGNORE_ZBL Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_MV_OCC_IGNORE_BDR Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_MV_NO_VSI_MODE Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_MV_NO_OCC_MODE Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_0A (MJC_BASE_ADDR + 0x828)
    #define MJC_MV_OVL_RGN_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_NO_OVL_MODE Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_OVL_Y_TH2 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_OVL_X_TH2 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_OVL_S_TH Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_OVL_Y_TH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_OVL_X_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_0B (MJC_BASE_ADDR + 0x82C)
    #define MJC_MV_42A_STILL_ONLY Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MV_42A_ST_Y_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_42A_ST_X_THR Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_0C (MJC_BASE_ADDR + 0x830)
    #define MJC_MV_GMV_INTRVL_Y Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_GMV_INTRVL_X Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_GMV_MODE Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_GMV_INTRVL_LBOX Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_GMV_BI_USE_TF Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_GMV_OSMP_LR_SEL Fld(1,12,AC_MSKB1)//[12:12]
#define MJC_MV_0D (MJC_BASE_ADDR + 0x834)
    #define MJC_RAND_Y_FAC3 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_RAND_Y_FAC2 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_RAND_Y_FAC1 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_RAND_Y_FAC0 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_RAND_X_FAC3 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_RAND_X_FAC2 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_RAND_X_FAC1 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_RAND_X_FAC0 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_0E (MJC_BASE_ADDR + 0x838)
    #define MJC_MV_CRC_START Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_MV_START_ADDR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_0F (MJC_BASE_ADDR + 0x83C)
    #define MJC_MV_CRC_CLEAR Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_MV_END_ADDR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_10 (MJC_BASE_ADDR + 0x840)
    #define MJC_MV_INSERT_ZERO_Y Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_MV_INSERT_ZERO_X Fld(1,10,AC_MSKB1)//[10:10]
#define MJC_MV_11 (MJC_BASE_ADDR + 0x844)
    #define MJC_MV_FMD_DLT_GAIN Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_MV_13 (MJC_BASE_ADDR + 0x848)
    #define MJC_MV_PSG_V_BD_HT Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_PSG_H_BD_WD Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_PSG_LR3D_MODE Fld(2,8,AC_MSKB1)//[9:8]
    #define MJC_MV_PSG_OCC_BG_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_MV_PSG_BD_AUTO Fld(1,1,AC_MSKB0)//[1:1]
#define MJC_MV_14 (MJC_BASE_ADDR + 0x84C)
    #define MJC_MV_PSG_MV_YLRG_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_PSG_BDRVSI_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PSG_NRMVSI_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_PSG_OCCSAD_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PSG_MV_SMAG_SEL Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_MV_PSG_MV_SMAG_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_PSG_MV_PMAG_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_15 (MJC_BASE_ADDR + 0x850)
    #define MJC_MV_PSG_BST_GAIN_CNT_TH4 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_PSG_BST_GAIN_CNT_TH3 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PSG_BST_GAIN_CNT_TH2 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_PSG_BST_GAIN_CNT_TH1 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PSG_BST_GAIN_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_MV_PSG_SAD_GAIN_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_MV_PSG_SAD_GAIN_COR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_16 (MJC_BASE_ADDR + 0x854)
    #define MJC_MV_PSG_MVM_GAIN_96 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_PSG_MVM_GAIN_80 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PSG_MVM_GAIN_64 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_PSG_MVM_GAIN_56 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PSG_MVM_GAIN_48 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PSG_MVM_GAIN_40 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PSG_MVM_GAIN_32 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_PSG_MVM_GAIN_24 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_17 (MJC_BASE_ADDR + 0x858)
    #define MJC_MV_PSG_BAD_MT_MSK Fld(12,16,AC_MSKW32)//[27:16]
    #define MJC_MV_PSG_MSAD_AD_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PSG_MVM_GAIN_MAX Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_18 (MJC_BASE_ADDR + 0x85C)
    #define MJC_MV_PSG_MV_ERR4_MAX Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_PSG_MV_ERR3_COR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PSG_MV_ERR3_MSK Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_PSG_MV_ERR2_MAX Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PSG_MV_ERR1_COR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PSG_MV_ERR1_MSK Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_19 (MJC_BASE_ADDR + 0x860)
    #define MJC_MV_PSG_OCC_ERR2_MAX Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PSG_OCC_ERR1_COR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PSG_OCC_ERR1_MSK Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_1A (MJC_BASE_ADDR + 0x864)
    #define MJC_MV_PSG_OCC_HST_MSK Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PSG_OSD_ERR2_MAX Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PSG_OSD_ERR2_MSK Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_PSG_OSD_ERR1_MSK Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_1B (MJC_BASE_ADDR + 0x868)
    #define MJC_MV_PSG_ZR_Y_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PSG_ZR_X_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PSG_EQ_S_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PSG_EQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_PSG_EQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_20 (MJC_BASE_ADDR + 0x86C)
    #define MJC_MV_PROG_SEL04 Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_MV_PROG_SEL03 Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_MV_PROG_SEL02 Fld(7,8,AC_MSKB1)//[14:8]
    #define MJC_MV_PROG_SEL01 Fld(7,0,AC_MSKB0)//[6:0]
#define MJC_MV_21 (MJC_BASE_ADDR + 0x870)
    #define MJC_MV_PROG_SEL08 Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_MV_PROG_SEL07 Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_MV_PROG_SEL06 Fld(7,8,AC_MSKB1)//[14:8]
    #define MJC_MV_PROG_SEL05 Fld(7,0,AC_MSKB0)//[6:0]
#define MJC_MV_22 (MJC_BASE_ADDR + 0x874)
    #define MJC_MVC_OCC_H_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MVC_OCC_V_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_SMALL_X_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_SMALL_Y_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_24 (MJC_BASE_ADDR + 0x878)
    #define MJC_MV_MESC_FRM_CNT Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_YAPL_FRM_CNT Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_MESC_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_MV_YAPL_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_MV_YAPL_GAIN Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_YAPL_REF_SEL Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_MV_YAPL_MODE Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_MV_25 (MJC_BASE_ADDR + 0x87C)
    #define MJC_MV_YAPL_LOW_CLIP_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_YAPL_LOW_THR Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MV_26 (MJC_BASE_ADDR + 0x880)
    #define MJC_MV_YAPL_HIGH_CLIP_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_YAPL_HIGH_THR Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MV_27 (MJC_BASE_ADDR + 0x884)
    #define MJC_MV_YAPL_FIX_THR Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MV_2A (MJC_BASE_ADDR + 0x890)
    #define MJC_MVC_PRJ_SFT_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MVC_PRJ_SFT_X_THR Fld(4,16,AC_MSKB2)//[19:16]
#define MJC_MV_2B (MJC_BASE_ADDR + 0x894)
    #define MJC_SMALL_DET_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MVC_OTHR_SEL Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_MVC_CRCT_SEL_2 Fld(2,22,AC_MSKB2)//[23:22]
    #define MJC_MVC_CRCT_SEL_1 Fld(2,20,AC_MSKB2)//[21:20]
    #define MJC_MVC_PFLT_SEL Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_MVC_WD_X Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_MVC_WD_Y Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MVC_CRCT_TH_H Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MVC_CRCT_TH_V Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_2C (MJC_BASE_ADDR + 0x898)
    #define MJC_MV_MAJOR_BDR_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_MAJOR_CNT_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_MAJOR_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_MAJOR_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_HLEAD_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_HLEAD_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_NZERO_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_NZERO_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_2D (MJC_BASE_ADDR + 0x89C)
    #define MJC_MV_BG_IIR_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_BG_DBG_NO_KEEP Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_MAJOR_DBG_SEL Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_HLEAD_DBG_SEL Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_BG_CNT_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_BG_KEEP_SEL Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_MV_BG_FGBG_CHK Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_MV_BG_CNT_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_MV_BG_CNT_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_MV_BG_FGBG_DFVAL Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_MV_MAJOR_CHECK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_2E (MJC_BASE_ADDR + 0x8A0)
    #define MJC_MVC_KEEP_Y_TH2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MVC_KEEP_X_TH2 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MVC_KEEP_Y_TH1 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MVC_KEEP_X_TH1 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MVC_VRNG_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MVC_TPOS_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_MVC_KEEP_MOD Fld(3,1,AC_MSKB0)//[3:1]
    #define MJC_MV_BYPASS_MVC Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_2F (MJC_BASE_ADDR + 0x8A4)
    #define MJC_MV_TOC_BIDF_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_TOC_BIDF_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_TOC_BGCHK_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_TOC_BGCHK_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_TOC_FGEX_YGAIN Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_TOC_BGEX_YGAIN Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_TOC_FGEX_XGAIN Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_TOC_BGEX_XGAIN Fld(4,0,AC_MSKB0)//[3:0]
	
	//Page MV_2
#define MJC_MV_30 (MJC_BASE_ADDR + 0x8A8)
    #define MJC_MV_BG_LG_Y_THR Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_BG_LG_X_THR Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_BG_EQ_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_BG_EQ_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_TOC_ZERO_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_TOC_ZERO_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_31 (MJC_BASE_ADDR + 0x8AC)
    #define MJC_MV_TOC_TF_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_TOC_TF_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_TOC_BGF_EX_Y Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_MV_TOC_BGF_EX_X Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_MV_TOC_BGF_EX_SEL Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_MV_TOC_BGF_TPOS_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_TOC_TPOS_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_MV_TOC_ZBL_BOTH Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_MV_TOC_RANGE_TPOS_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_MV_BGF_NO_BD Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MV_BGF_H_BD Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_BGF_V_BD Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_32 (MJC_BASE_ADDR + 0x8B0)
    #define MJC_CA_BLK_APL_FORCE_FMD Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_CA_BLK_APL_FORCE_VSI Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_CA_BLK_APL_OCC_OFF Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_CA_BLK_FBK_FORCE_FMD Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_CA_BLK_FBK_FORCE_VSI Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_CA_BLK_FBK_OCC_OFF Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_CA_OSD_REV_WT_OFF Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_CA_OSD_FORCE_FMD_TP2 Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_CA_OSD_FORCE_FMD_TP1 Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_CA_OSD_FORCE_FMD_TP0 Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_CA_OSD_FORCE_VSI_TP2 Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_CA_OSD_FORCE_VSI_TP1 Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_CA_OSD_FORCE_VSI_TP0 Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_CA_OSD_OCC_OFF_TP1 Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_CA_OSD_OCC_OFF_TP0 Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_33 (MJC_BASE_ADDR + 0x8B4)
    #define MJC_MV_TOC_FGEX_YGAIN2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_TOC_BGEX_YGAIN2 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_TOC_FGEX_XGAIN2 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_TOC_BGEX_XGAIN2 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_TOC_TFDF_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_TOC_TFDF_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_TOC_2D_PRJ_MD Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_MV_TOC_TB_CHK_MD Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_MV_34 (MJC_BASE_ADDR + 0x8B8)
    #define MJC_MV_TOC_FGEX_YMAX Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_TOC_FGEX_YMIN Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_TOC_BGEX_YMAX Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_TOC_BGEX_YMIN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_TOC_FGEX_XMAX Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_TOC_FGEX_XMIN Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_TOC_BGEX_XMAX Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_TOC_BGEX_XMIN Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_36 (MJC_BASE_ADDR + 0x8BC)
    #define MJC_MV_EXP_OVL Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_MV_EXP_OCC Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_MV_EXP_MVC Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_MV_EXP_PMC Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MV_37 (MJC_BASE_ADDR + 0x8C0)
    #define MJC_CA_LFB_GDFG_MVY_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_LFB_GDFG_MVX_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_LFB_GDFG_SM_TH1 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_LFB_GDFG_BC_TH2 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_LFB_GDFG_BC_TH1 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_LFB_GDFG_AD_TH2 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_LFB_GDFG_AD_TH1 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_38 (MJC_BASE_ADDR + 0x8C4)
    #define MJC_CA_LFB_MVY_EQ_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_LFB_MVX_EQ_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_LFB_GDFG_MAJ3_TH2 Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_CA_LFB_GDFG_MAJ3_TH1 Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_MV_39 (MJC_BASE_ADDR + 0x8C8)
    #define MJC_MV_MFB_EQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_MFB_EQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_3B (MJC_BASE_ADDR + 0x8CC)
    #define MJC_MV_FLT_CHG_VIO_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_FLT_OBJ_WD_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_FLT_ZR_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_FLT_ZR_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_FLT_SML_ALL_TH1 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_FLT_SML_ALL_TH0 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_FLT_SML_OBJ_TH1 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_FLT_SML_OBJ_TH0 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_3C (MJC_BASE_ADDR + 0x8D0)
    #define MJC_MV_FLT_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_FLT_ADF_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_MV_FLT_DEF_3X3 Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MV_FLT_BDR_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_MV_FLT_NRM_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_FLT_SEL_NEAR Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_MV_FLT_IGNORE_SML_CHG Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MV_FLT_BDR_CLST_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_FLT_NRM_CLST_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_3D (MJC_BASE_ADDR + 0x8D4)
    #define MJC_MV_FLT_DECI_EQ_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_FLT_DECI_EQ_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_FLT_CLST_EQ_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_FLT_CLST_EQ_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_FLT_SCAN_EQ_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_FLT_SCAN_EQ_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_FLT_SORT_EQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MV_FLT_SORT_EQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_CA_00 (MJC_BASE_ADDR + 0x8D8)
    #define MJC_CA_OSD_CRN_RAW_THR Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_CA_OSD_CRN_EGM_THR Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_CA_OSD_CRN_EGV_THR Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_CA_OSD_CRN_EGH_THR Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_CA_01 (MJC_BASE_ADDR + 0x8DC)
    #define MJC_CA_OSD_EG_ACC_THR Fld(6,24,AC_MSKB3)//[29:24]
    #define MJC_CA_OSD_EG_MIN Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_CA_OSD_EG_MAX Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_CA_OSD_EG_OFST Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_CA_02 (MJC_BASE_ADDR + 0x8E0)
    #define MJC_CA_OSD_CRN_MIN Fld(7,16,AC_MSKB2)//[22:16]
    #define MJC_CA_OSD_CRN_MAX Fld(7,8,AC_MSKB1)//[14:8]
    #define MJC_CA_OSD_CRN_OFST Fld(7,0,AC_MSKB0)//[6:0]
#define MJC_CA_03 (MJC_BASE_ADDR + 0x8E4)
    #define MJC_CA_OSD_CRN_EGS_THR Fld(7,24,AC_MSKB3)//[30:24]
    #define MJC_CA_OSD_EGV_BLK_THR Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_CA_OSD_EGH_BLK_THR Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_CA_OSD_CRN_BLK_THR Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_CA_04 (MJC_BASE_ADDR + 0x8E8)
    #define MJC_CA_OSD_EGV_MVY_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_OSD_EGH_MVX_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_OSD_DET_BD_LR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_OSD_DET_BD_UD Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_OSD_DET_BD_AUTO Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_CA_05 (MJC_BASE_ADDR + 0x8EC)
    #define MJC_CA_OSD_ZRY_TH2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_OSD_ZRX_TH2 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_OSD_ZRY_TH1 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_OSD_ZRX_TH1 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_OSD_ZERO_CHK_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_CA_OSD_BOC_CHK_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_CA_OSD_WHITE_THR Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_CA_06 (MJC_BASE_ADDR + 0x8F0)
    #define MJC_CA_OSD_EGV_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_CA_OSD_EGH_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_CA_OSD_CRN_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_CA_OSD_CRN_THIN_CHK Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_CA_OSD_EGV_CNT_THR Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_CA_OSD_EGH_CNT_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_CA_OSD_CRN_CNT_THR Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_CA_07 (MJC_BASE_ADDR + 0x8F4)
    #define MJC_CA_BOC_PRJ_Y_EXT Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_BOC_PRJ_X_EXT Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_BOC_EDG_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_BOC_EDG_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_FGBG_MSAD_CHK Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_CA_08 (MJC_BASE_ADDR + 0x8F8)
    #define MJC_CA_SAD_VAR_THR Fld(6,24,AC_MSKB3)//[29:24]
    #define MJC_CA_SAD_EDG_MIN Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_CA_SAD_EDG_MAX Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_CA_SAD_DBG_THR Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_CA_09 (MJC_BASE_ADDR + 0x8FC)
    #define MJC_CA_SAD_EDG_COR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_CFB_SAD_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_CFB_ZR_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_CFB_ZR_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_CFB_EQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_CFB_EQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_CA_0A (MJC_BASE_ADDR + 0x900)
    #define MJC_CA_LFB_ADP_XY_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_LFB_ADP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_CA_LFB_ADP_Y_OFST Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_LFB_ADP_X_OFST Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_LFB_ADP_Y_TH1 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_LFB_ADP_X_TH1 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_LFB_ADP_Y_TH0 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_LFB_ADP_X_TH0 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_CA_0B (MJC_BASE_ADDR + 0x904)
    #define MJC_CA_OSD_SW_RST Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_CA_OSD_ROW_DEC_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_CA_OSD_ROW_RST_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_CA_OSD_FRM_DEC_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_CA_OSD_FRM_RST_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_CA_OSD_EGV_CNT_DEC Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_CA_OSD_EGH_CNT_DEC Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_CA_OSD_CRN_CNT_DEC Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_CA_0C (MJC_BASE_ADDR + 0x908)
    #define MJC_CA_LFB_IGNORE_GBG Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_CA_LFB_IGNORE_SBG Fld(1,10,AC_MSKB1)//[10:10]
    #define MJC_CA_LFB_IGNORE_SFG Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_CA_LFB_IGNORE_SAD Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_CA_LFB_MJ3_THR Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_CA_0D (MJC_BASE_ADDR + 0x90C)
    #define MJC_CA_HOC_BOC_SET2_BKY_TH Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_HOC_BOC_SET2_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_CA_HOC_TOC_IGNORE_NBR Fld(1,6,AC_MSKB0)//[6:6]
#define MJC_CA_0E (MJC_BASE_ADDR + 0x910)
    #define MJC_CA_BLK_FBK3_MSK1 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_BLK_FBK2_MSK2 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_BLK_FBK2_MSK1 Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_BLK_FBK1_MSK2 Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_BLK_FBK1_MSK1 Fld(7,8,AC_MSKB1)//[14:8]
    #define MJC_CA_BLK_FBK4_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_CA_BLK_FBK3_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_CA_BLK_FBK2_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_CA_BLK_FBK1_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_CA_0F (MJC_BASE_ADDR + 0x914)
    #define MJC_CA_BLK_FBK_SAD_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_CA_BLK_FBK2_CNT_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_BLK_FBK1_CNT_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_STA_MV_00 (MJC_BASE_ADDR + 0x918)
    #define MJC_STA_MAJOR_MV0 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT0 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_01 (MJC_BASE_ADDR + 0x91C)
    #define MJC_STA_MAJOR_MV1 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT1 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_02 (MJC_BASE_ADDR + 0x920)
    #define MJC_STA_MAJOR_MV2 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_03 (MJC_BASE_ADDR + 0x924)
    #define MJC_STA_MAJOR_MV3 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT3 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_04 (MJC_BASE_ADDR + 0x928)
    #define MJC_STA_MAJOR_MV4 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT4 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_05 (MJC_BASE_ADDR + 0x92C)
    #define MJC_STA_MAJOR_MV5 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT5 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_06 (MJC_BASE_ADDR + 0x930)
    #define MJC_STA_MAJOR_MV6 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT6 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_07 (MJC_BASE_ADDR + 0x934)
    #define MJC_STA_MAJOR_MV7 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MAJOR_CNT7 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_08 (MJC_BASE_ADDR + 0x938)
    #define MJC_DBG_HLEAD_MV Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_09 (MJC_BASE_ADDR + 0x93C)
    #define MJC_DBG_MAJOR_MV Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MV_0A (MJC_BASE_ADDR + 0x940)
    #define MJC_DBG_MAJOR_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_DBG_HLEAD_CNT Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_MV_0E (MJC_BASE_ADDR + 0x944)
    #define MJC_MV_CRC_RDY Fld(1,31,AC_MSKB3)//[31:31]
#define MJC_STA_MV_0F (MJC_BASE_ADDR + 0x948)
    #define MJC_MV_CRC Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_STA_PSG_00 (MJC_BASE_ADDR + 0x94C)
    #define MJC_BDR_VSI_SUM Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_NRM_VSI_SUM Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_01 (MJC_BASE_ADDR + 0x950)
    #define MJC_TOTAL_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MOT_VAR_SUM Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_02 (MJC_BASE_ADDR + 0x954)
    #define MJC_BAD_MOT_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_TOTAL_MOT_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_03 (MJC_BASE_ADDR + 0x958)
    #define MJC_GOOD_VAR_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_GOOD_MOT_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_04 (MJC_BASE_ADDR + 0x95C)
    #define MJC_MVY_LRG_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_05 (MJC_BASE_ADDR + 0x960)
    #define MJC_BAD_BGMV_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_GOOD_BGMV_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_06 (MJC_BASE_ADDR + 0x964)
    #define MJC_OSD_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_CNT Fld(16,0,AC_FULLW10)//[15:0]
	
	//Page MV_3
#define MJC_STA_PSG_07 (MJC_BASE_ADDR + 0x968)
    #define MJC_MV_ERR2 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_ERR1 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_08 (MJC_BASE_ADDR + 0x96C)
    #define MJC_MV_ERR4 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_ERR3 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_09 (MJC_BASE_ADDR + 0x970)
    #define MJC_OCC_ERR2 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_ERR1 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_0A (MJC_BASE_ADDR + 0x974)
    #define MJC_OSD_ERR2 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OSD_ERR1 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_PSG_0F (MJC_BASE_ADDR + 0x978)
    #define MJC_OCC_SCHG Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_ME_SCHG Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_APL_SCHG Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_STA_OHT_00 (MJC_BASE_ADDR + 0x97C)
    #define MJC_OCC_HST_X1 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_X0 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_01 (MJC_BASE_ADDR + 0x980)
    #define MJC_OCC_HST_X3 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_X2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_02 (MJC_BASE_ADDR + 0x984)
    #define MJC_OCC_HST_X5 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_X4 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_03 (MJC_BASE_ADDR + 0x988)
    #define MJC_OCC_HST_X7 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_X6 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_04 (MJC_BASE_ADDR + 0x98C)
    #define MJC_OCC_HST_Y1 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_Y0 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_05 (MJC_BASE_ADDR + 0x990)
    #define MJC_OCC_HST_Y3 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_OCC_HST_Y2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_OHT_06 (MJC_BASE_ADDR + 0x994)
    #define MJC_OCC_HST_Y4 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_GMV_BI (MJC_BASE_ADDR + 0x998)
    #define MJC_ME2_STA_GMV_X Fld(9,8,AC_MSKW21)//[16:8]
    #define MJC_ME2_STA_GMV_Y_FIX Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_BG (MJC_BASE_ADDR + 0x99C)
    #define MJC_STA_MV_BG Fld(16,0,AC_FULLW10)//[15:0]
    #define MJC_STA_MV_BG_X Fld(9,7,AC_MSKW10)//[15:7]
    #define MJC_STA_MV_BG_Y Fld(7,0,AC_MSKB0)//[6:0]
#define MJC_STA_MH_00 (MJC_BASE_ADDR + 0x9A0)
    #define MJC_MV_X_GRM01 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM00 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_01 (MJC_BASE_ADDR + 0x9A4)
    #define MJC_MV_X_GRM03 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM02 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_02 (MJC_BASE_ADDR + 0x9A8)
    #define MJC_MV_X_GRM05 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM04 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_03 (MJC_BASE_ADDR + 0x9AC)
    #define MJC_MV_X_GRM07 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM06 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_04 (MJC_BASE_ADDR + 0x9B0)
    #define MJC_MV_X_GRM09 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM08 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_05 (MJC_BASE_ADDR + 0x9B4)
    #define MJC_MV_X_GRM11 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM10 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_06 (MJC_BASE_ADDR + 0x9B8)
    #define MJC_MV_X_GRM13 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM12 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_07 (MJC_BASE_ADDR + 0x9BC)
    #define MJC_MV_X_GRM15 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM14 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_08 (MJC_BASE_ADDR + 0x9C0)
    #define MJC_MV_X_GRM17 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM16 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_09 (MJC_BASE_ADDR + 0x9C4)
    #define MJC_MV_X_GRM19 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X_GRM18 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0A (MJC_BASE_ADDR + 0x9C8)
    #define MJC_MV_Y_GRM01 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM00 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0B (MJC_BASE_ADDR + 0x9CC)
    #define MJC_MV_Y_GRM03 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM02 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0C (MJC_BASE_ADDR + 0x9D0)
    #define MJC_MV_Y_GRM05 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM04 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0D (MJC_BASE_ADDR + 0x9D4)
    #define MJC_MV_Y_GRM07 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM06 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0E (MJC_BASE_ADDR + 0x9D8)
    #define MJC_MV_Y_GRM09 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM08 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_0F (MJC_BASE_ADDR + 0x9DC)
    #define MJC_MV_Y_GRM11 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM10 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_10 (MJC_BASE_ADDR + 0x9E0)
    #define MJC_MV_Y_GRM13 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM12 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_11 (MJC_BASE_ADDR + 0x9E4)
    #define MJC_MV_Y_GRM15 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y_GRM14 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_12 (MJC_BASE_ADDR + 0x9E8)
    #define MJC_MV_X2_GRM1 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X2_GRM0 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_13 (MJC_BASE_ADDR + 0x9EC)
    #define MJC_MV_X2_GRM3 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X2_GRM2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_14 (MJC_BASE_ADDR + 0x9F0)
    #define MJC_MV_X2_GRM5 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X2_GRM4 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_15 (MJC_BASE_ADDR + 0x9F4)
    #define MJC_MV_X2_GRM7 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_X2_GRM6 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_16 (MJC_BASE_ADDR + 0x9F8)
    #define MJC_MV_Y2_GRM1 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y2_GRM0 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_17 (MJC_BASE_ADDR + 0x9FC)
    #define MJC_MV_Y2_GRM3 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y2_GRM2 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_18 (MJC_BASE_ADDR + 0xA00)
    #define MJC_MV_Y2_GRM5 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y2_GRM4 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_19 (MJC_BASE_ADDR + 0xA04)
    #define MJC_MV_Y2_GRM7 Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_MV_Y2_GRM6 Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MH_1A (MJC_BASE_ADDR + 0xA08)
    #define MJC_STA_MV_ZERO Fld(16,16,AC_FULLW32)//[31:16]
#define MJC_MV_FRM_DS (MJC_BASE_ADDR + 0xA0C)
    #define MJC_MV_FRM_DS_GAIN_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_FRM_DS_CLIP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_FRM_DS_X_GAIN Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_FRM_DS_Y_GAIN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_FRM_DS_X_CLIP Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_FRM_DS_Y_CLIP Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_PPC (MJC_BASE_ADDR + 0xA14)
    #define MJC_PPC_SHOW_NFBLK Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_PPC_SHOW_INVALID Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_PPC_FORCE_GMV_VLE Fld(17,1,AC_MSKDW)//[17:1]
#define MJC_MV_3DR0 (MJC_BASE_ADDR + 0xA18)
    #define MJC_MV_START_ADDR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_3DR1 (MJC_BASE_ADDR + 0xA1C)
    #define MJC_MV_END_ADDR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_3DR2 (MJC_BASE_ADDR + 0xA20)
    #define MJC_TD_START_ADDR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_3DR3 (MJC_BASE_ADDR + 0xA24)
    #define MJC_TD_END_ADDR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_PAD (MJC_BASE_ADDR + 0xA28)
    #define MJC_MV_PAD_DN_OS_REF Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_MV_PAD_UP_OS_REF Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_MV_PAD_DN_OS_TAR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_PAD_UP_OS_TAR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_PAD_DN_HT Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MV_PAD_UP_HT Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_MV_PAD_DN_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_MV_PAD_UP_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_CA_PAD (MJC_BASE_ADDR + 0xA2C)
    #define MJC_CA_OSD_ZERO_CHK_PAD_OFF Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_CA_OSD_BOC_CHK_PAD_OFF Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_CA_OSD_CNT_THR_PAD_SEL Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_CA_OSD_EGV_CNT_THR_PAD Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_CA_OSD_EGH_CNT_THR_PAD Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_CA_OSD_CRN_CNT_THR_PAD Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_FORCE (MJC_BASE_ADDR + 0xA30)
    #define MJC_MV_FORCE_ME_OUT_Y Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_FORCE_ME_OUT_X Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_FORCE_ME_OUT_EN Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_CA_M2T_0 (MJC_BASE_ADDR + 0xA34)
    #define MJC_CA_M2T_MVY_TH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_M2T_MVX_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_CA_M2T_1 (MJC_BASE_ADDR + 0xA38)
    #define MJC_STA_M2T_M0_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_M2T_BG_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_CA_M2T_2 (MJC_BASE_ADDR + 0xA3C)
    #define MJC_STA_M2T_M2_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_M2T_M1_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_CA_M2T_3 (MJC_BASE_ADDR + 0xA40)
    #define MJC_STA_M2T_M0_SUM Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_CA_M2T_4 (MJC_BASE_ADDR + 0xA44)
    #define MJC_STA_M2T_BG_SUM Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_CA_M2T_5 (MJC_BASE_ADDR + 0xA48)
    #define MJC_STA_M2T_M2_SUM Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_CA_M2T_6 (MJC_BASE_ADDR + 0xA4C)
    #define MJC_STA_M2T_M1_SUM Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_TBA_0 (MJC_BASE_ADDR + 0xA50)
    #define MJC_TBA_BND_DN Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_TBA_BND_UP Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_TBA_BND_RT Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_TBA_BND_LF Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_TBA_1 (MJC_BASE_ADDR + 0xA54)
    #define MJC_TBA_ZRGMV_OFF Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_TBA_BND_LBOX_AUTO Fld(1,16,AC_MSKB2)//[16:16]
#define MJC_MV_TBA_2 (MJC_BASE_ADDR + 0xA58)
    #define MJC_TBA_RD_ZERO Fld(1,31,AC_MSKB3)//[31:31]
#define MJC_MV_TBA_3 (MJC_BASE_ADDR + 0xA5C)
    #define MJC_TBA_YEG_RTO_CLP_TH_N Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_TBA_YEG_RTO_CLP_TH_P Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_TBA_YAV_RTO_CLP_TH_N Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_TBA_YAV_RTO_CLP_TH_P Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_TBA_VAR_RTO_CLP_TH_N Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_TBA_VAR_RTO_CLP_TH_P Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_TBA_SAD_RTO_CLP_TH_N Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_TBA_SAD_RTO_CLP_TH_P Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_TBA_4 (MJC_BASE_ADDR + 0xA60)
    #define MJC_TBA_MVDF_LMRM_SUM_TH Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_TBA_MVDF_LMRM_MAX_TH Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_TBA_MVDF_LR_SUM_TH Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_TBA_MVDF_LR_MAX_TH Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_TBA_PCCV_RTO_TH Fld(5,10,AC_MSKB1)//[14:10]
    #define MJC_TBA_CNCV_RTO_TH Fld(5,5,AC_MSKW10)//[9:5]
    #define MJC_TBA_PCUC_RTO_TH Fld(5,0,AC_MSKB0)//[4:0]
#define MJC_MV_TBA_5 (MJC_BASE_ADDR + 0xA64)
    #define MJC_MV_TBA_WT_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_TBA_STR_ADR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_TBA_6 (MJC_BASE_ADDR + 0xA68)
    #define MJC_MV_TBA_END_ADR_L Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_TBA_7 (MJC_BASE_ADDR + 0xA6C)
    #define MJC_TBA_TFOCC_FORCE_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_TBA_TFOCC_FORCE_TYP Fld(3,28,AC_MSKB3)//[30:28]
    #define MJC_TBA_MV_MED_VIO_TH Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_TBA_MV_TB_VIO_TH Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_TBA_AVG_DYR_CLP_TH Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_TBA_VAR_REF_CLP_TH Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_TBA_AVG_REF_CLP_TH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_TBA_SAD_REF_CLP_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_TBA_8 (MJC_BASE_ADDR + 0xA70)
    #define MJC_TBA_MV_VIO_MSK Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_TBA_YAV_RTO_CLP_TH2_P Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_TBA_VAR_RTO_CLP_TH2_P Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_TBA_SAD_RTO_CLP_TH2_P Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_TBA_9 (MJC_BASE_ADDR + 0xA74)
    #define MJC_TBA_SMLOBJ_MODE Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_TBA_3D_MODE Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_TBA_VIDEO_ONLY Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_PCN_SAD_REF_SFT Fld(2,18,AC_MSKB2)//[19:18]
    #define MJC_PCN_DYR_REF_SFT Fld(2,16,AC_MSKB2)//[17:16]
#define MJC_CA_TBA_0 (MJC_BASE_ADDR + 0xA78)
    #define MJC_CA_TBA_PT_EQ_Y_THR Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_TBA_PT_EQ_X_THR Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_TBA_PZ_EQ_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_TBA_PZ_EQ_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_TBA_PB_EQ_Y_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_TBA_PB_EQ_X_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_TBA_NB_EQ_Y_THR Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_TBA_NB_EQ_X_THR Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_CA_TBA_1 (MJC_BASE_ADDR + 0xA7C)
    #define MJC_CA_TBA_EDG_Y_THR Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_TBA_EDG_X_THR Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_TBA_PTDF_CNT2_TH Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_CA_TBA_PTDF_CNT1_TH Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_OOB (MJC_BASE_ADDR + 0xA80)
    #define MJC_MV_WT_OOB_CLEAR Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_STA_WT_TBA_OOB Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_STA_WT_MV_OOB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_STA_WT_TD_OOB Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_STA_GMV_TF (MJC_BASE_ADDR + 0xA84)
    #define MJC_ME3_STA_GMV_X Fld(9,8,AC_MSKW21)//[16:8]
    #define MJC_ME3_STA_GMV_Y_FIX Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_GMV_M3 (MJC_BASE_ADDR + 0xA88)
    #define MJC_STA_GMV_BI_M3_X Fld(9,8,AC_MSKW21)//[16:8]
    #define MJC_STA_GMV_BI_M3_Y Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_TBA_A (MJC_BASE_ADDR + 0xA8C)
    #define MJC_TBA_GMV_SEL Fld(2,25,AC_MSKB3)//[26:25]
    #define MJC_TBA_MVEXP_OFF Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_TBA_STR_ADR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_TBA_B (MJC_BASE_ADDR + 0xA90)
    #define MJC_MV_TBA_END_ADR_R Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_MV_TBA_C (MJC_BASE_ADDR + 0xA94)
    #define MJC_TBA_CST_GAIN_SEL Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_TBA_0 (MJC_BASE_ADDR + 0xA98)
    #define MJC_STA_RTO_SAD_CNT Fld(24,0,AC_MSKDW)//[23:0]
	
	//Page MV_4
#define MJC_STA_TBA_1 (MJC_BASE_ADDR + 0xA9C)
    #define MJC_STA_RTO_AVG_CNT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_STA_TBA_2 (MJC_BASE_ADDR + 0xAA0)
    #define MJC_STA_RTO_EDG_CNT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_STA_TBA_3 (MJC_BASE_ADDR + 0xAA4)
    #define MJC_STA_CNCV_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_PCUC_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_TBA_4 (MJC_BASE_ADDR + 0xAA8)
    #define MJC_STA_TBA_GTRK_MAX_VLID_FRM_CNT Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_STA_PCCV_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MV_TBA_D (MJC_BASE_ADDR + 0xAAC)
    #define MJC_TBA_LR_REF_USE_LC Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_TBA_CNCV_RTO_TH3 Fld(5,21,AC_MSKW32)//[25:21]
    #define MJC_TBA_PCUC_RTO_TH3 Fld(5,16,AC_MSKB2)//[20:16]
    #define MJC_TBA_EDG_RTO_TH3 Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_TBA_AVG_RTO_TH3 Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_TBA_VAR_RTO_TH3 Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_TBA_SAD_RTO_TH3 Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MV_TBA_E (MJC_BASE_ADDR + 0xAB0)
    #define MJC_DIR_HOC_DET_MSK_F Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_DIR_HOC_DET_MSK_V Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_DIR_AOC2_EN_MSK_F Fld(2,6,AC_MSKB0)//[7:6]
    #define MJC_DIR_AOC1_EN_MSK_F Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_DIR_AOC2_EN_MSK_V Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_DIR_AOC1_EN_MSK_V Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_MV_TBA_F (MJC_BASE_ADDR + 0xAB4)
    #define MJC_DIR_HOC_CHK_4_MSK_V Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_DIR_HOC_CHK_3_MSK_V Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_DIR_HOC_CHK_2_MSK_V Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_DIR_HOC_CHK_1_MSK_V Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_TBA_10 (MJC_BASE_ADDR + 0xAB8)
    #define MJC_DIR_HOC_CHK_4_MSK_F Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_DIR_HOC_CHK_3_MSK_F Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_DIR_HOC_CHK_2_MSK_F Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_DIR_HOC_CHK_1_MSK_F Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_DSC_0 (MJC_BASE_ADDR + 0xABC)
    #define MJC_MV_DSC_X_GAIN_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_MV_DSC_Y_GAIN_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MV_DSC_X_CLIP_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_MV_DSC_Y_CLIP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_DSC_X_GAIN Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_MV_DSC_Y_GAIN Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_MV_DSC_Y_CLIP Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_DSC_X_CLIP Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_DSC_1 (MJC_BASE_ADDR + 0xAC0)
    #define MJC_MV_DSC_X_BLK_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_DSC_Y_BLK_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_DSC_X_FDP_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_DSC_Y_FDP_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_DSC_2 (MJC_BASE_ADDR + 0xAC4)
    #define MJC_MV_DSC_BLK_EN_V Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_MV_DSC_FDP_EN_V Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_MV_DSC_MEG_EN_V Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_MV_DSC_BFB_EN_V Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MV_DSC_BLK_EN_F Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_MV_DSC_FDP_EN_F Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_MV_DSC_MEG_EN_F Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_MV_DSC_BFB_EN_F Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MV_DSC_X_MEG_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_DSC_Y_MEG_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_FDP_0 (MJC_BASE_ADDR + 0xAC8)
    #define MJC_FDP_APLSC_OFF Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_FDP_MESC_OFF Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_STA_FRM_DROP Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_FDP_FW_FORCE Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_FDP_HW_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_FDP_PCCN_CNT_TH1 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_FDP_PCCN_CNT_TH2 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_FDP_PCCV_CNT_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MV_FDP_1 (MJC_BASE_ADDR + 0xACC)
    #define MJC_FDP_MV_TPDF_CNT_TH Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_CA_MEG_0 (MJC_BASE_ADDR + 0xAD0)
    #define MJC_CA_MEG_LMV_X_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_CA_MEG_LMV_Y_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_CA_MEG_XGEY_TH Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CA_MEG_YGEX_TH Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_CA_MEG_SEL_MV_GAIN Fld(3,4,AC_MSKB0)//[6:4]
    #define MJC_CA_MEG_SEL_MV_OFST Fld(3,0,AC_MSKB0)//[2:0]
#define MJC_CA_MEG_1 (MJC_BASE_ADDR + 0xAD4)
    #define MJC_CA_MEG_SEL_MV_CLIP Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_CA_MEG_SAD_TH Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_CA_MEG_VSI_TH Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_CA_MEG_EDG_TH Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_CA_MEG_VAR_TH Fld(4,12,AC_MSKB1)//[15:12]
#define MJC_STA_MEG_0 (MJC_BASE_ADDR + 0xAD8)
    #define MJC_STA_MEG_SAD_ERR_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MEG_VSI_ERR_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_STA_MEG_1 (MJC_BASE_ADDR + 0xADC)
    #define MJC_STA_MEG_MOV_EDG_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_STA_MEG_TEXTURE_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MVTPDF_0 (MJC_BASE_ADDR + 0xAE0)
    #define MJC_MV_TPDF_MIN_X_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MV_TPDF_MIN_Y_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MV_TPDF_DIF_X_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_TPDF_DIF_Y_TH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_STA_MVTPDF (MJC_BASE_ADDR + 0xAE4)
    #define MJC_STA_MV_TPDF_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define MJC_MV_ATPG (MJC_BASE_ADDR + 0xBFC)
    #define MJC_MV_BAK_UP Fld(30,2,AC_MSKDW)//[31:2]
    #define MJC_MV_BAK_UP_DRAM_W_NO_WAIT Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_MV_BAK_UP_DRAM_WLR_CTL Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_MV_BAK_UP_PADDING_UP Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_MV_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_MV_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page MC
#define MJC_MC_00 (MJC_BASE_ADDR + 0x400)
    #define MJC_MC_SUB_YPV_SEL Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_MC_INTP_MODE_RGN2 Fld(6,16,AC_MSKB2)//[21:16]
    #define MJC_MC_INTP_MODE_RGN1 Fld(6,8,AC_MSKB1)//[13:8]
    #define MJC_MC_INTP_MODE_RGN0 Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_MC_01 (MJC_BASE_ADDR + 0x404)
    #define MJC_MC_TRUE_RPT_MAN_PV Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_MC_TRUE_RPT_MAN_CU Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_LBOX_STA_OVM_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_LBOX_STA_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_TRUE_REPEAT Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_TRUE_REPEAT_AT_CHG Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_APL_DBG_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_SCN_DBG_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_MC_DBG_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MC_DBG_WIDTH Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MC_FREEFUN Fld(1,9,AC_MSKB1)//[9:9]
    #define MJC_FORCE_MC_INTP Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_MC_INTP_NUM Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_MC_INTP_DEN Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MC_02 (MJC_BASE_ADDR + 0x408)
    #define MJC_GET_CURR_SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_MC_OPINT_C_H_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_MC_OPINT_C_V_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_MC_OPINT_Y_H_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_MC_OPINT_Y_V_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_INTP_FMD_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_INTP_RAW_DBG_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_INTP_MIRROR_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_INTP_MIX_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_FRM_FBACK_EN_LFRT Fld(2,12,AC_MSKB1)//[13:12]
    #define MJC_MC_INTP_FBK Fld(4,8,AC_MSKB1)//[11:8]
#define MJC_MC_03 (MJC_BASE_ADDR + 0x40C)
    #define MJC_FORCE_STA_GAIN_VAL Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_FORCE_STA_GAIN_CLIP_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_FORCE_STA_GAIN_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_HQF_MODE_RGN2 Fld(2,4,AC_MSKB0)//[5:4]
    #define MJC_HQF_MODE_RGN1 Fld(2,2,AC_MSKB0)//[3:2]
    #define MJC_HQF_MODE_RGN0 Fld(2,0,AC_MSKB0)//[1:0]
#define MJC_MC_04 (MJC_BASE_ADDR + 0x410)
    #define MJC_LBOX_OFST_MODE Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_LBOX_OFST_LF_MAX Fld(4,20,AC_MSKB2)//[23:20]
    #define MJC_LBOX_OFST_UD_MAX Fld(4,16,AC_MSKB2)//[19:16]
    #define MJC_LBOX_OFST_LF_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_LBOX_OFST_UD_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_LBOX_OFST_LF_FIX Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_LBOX_OFST_UD_FIX Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MC_05 (MJC_BASE_ADDR + 0x414)
    #define MJC_UNI_MBG_FG_ONLY Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_UNI_MBG_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define MJC_OVL_UNI_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_OVL_UNI_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_UNI_MIRROR_OFF Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_42A_ST_INTP_MD Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_42A_WT_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_CMX_WT_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_FMD_WT_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_OCC_WT_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MBG_ZRY_THR Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_MBG_ZRX_THR Fld(4,8,AC_MSKB1)//[11:8]
    #define MJC_OVL_WT_EN Fld(1,1,AC_MSKB0)//[1:1]
#define MJC_MC_06 (MJC_BASE_ADDR + 0x418)
    #define MJC_BK_STA_INFO_SEL Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_BK_GRID_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_VDB_MV_GAIN Fld(2,16,AC_MSKB2)//[17:16]
    #define MJC_VDB_REV_SEL Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_VDB_SHOW_MV Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_VDB_IN_MV_PATH Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_VDB_INFO_SEL Fld(8,4,AC_MSKW10)//[11:4]
    #define MJC_VDB_MODU_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MC_07 (MJC_BASE_ADDR + 0x41C)
    #define MJC_VDB_V Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_VDB_U Fld(8,16,AC_FULLB2)//[23:16]
#define MJC_MC_08 (MJC_BASE_ADDR + 0x420)
    #define MJC_VDB_CODE_VAL Fld(16,16,AC_FULLW32)//[31:16]
    #define MJC_VDB_CODE_SEL Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_VDB_CODE_V_START Fld(4,4,AC_MSKB0)//[7:4]
    #define MJC_VDB_CODE_H_START Fld(4,0,AC_MSKB0)//[3:0]
#define MJC_MC_09 (MJC_BASE_ADDR + 0x424)
    #define MJC_CHR_INTP_SELF_MODE Fld(4,12,AC_MSKB1)//[15:12]
    #define MJC_CHR_INTP_SELF_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define MJC_VDB_UV_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_VDB_Y_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_FORCE_NO_UV Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FORCE_NO_Y Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MC_0A (MJC_BASE_ADDR + 0x428)
    #define MJC_FORCE_MV_MD2_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_FORCE_MV_MD1_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_FORCE_MVX Fld(10,16,AC_MSKW32)//[25:16]
    #define MJC_FORCE_MVY Fld(9,0,AC_MSKW10)//[8:0]
#define MJC_MC_0B (MJC_BASE_ADDR + 0x42C)
    #define MJC_DEMO_BAR_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_SPLIT_MODE Fld(2,24,AC_MSKB3)//[25:24]
    #define MJC_SPLIT_BND_RT Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_SPLIT_BND_LF Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_0C (MJC_BASE_ADDR + 0x430)
    #define MJC_BAR_V Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_BAR_U Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_BAR_Y Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_BAR_WIDTH Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_0D (MJC_BASE_ADDR + 0x434)
    #define MJC_SPLIT_BND_DN Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_SPLIT_BND_UP Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_16 (MJC_BASE_ADDR + 0x454)
    #define MJC_MC_INTP_OSD Fld(6,24,AC_MSKB3)//[29:24]
    #define MJC_OSD_BOUND_RIGHT_0 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_LEFT_0 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_17 (MJC_BASE_ADDR + 0x458)
    #define MJC_OLD_OSD_MASK Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_OSD_BAR_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_OSD_BOUND_DOWN_0 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_UP_0 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_18 (MJC_BASE_ADDR + 0x45C)
    #define MJC_OSD_BOUND_UP_2_11_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_OSD_BOUND_RIGHT_1 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_LEFT_1 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_19 (MJC_BASE_ADDR + 0x460)
    #define MJC_OSD_BOUND_DOWN_2_11_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_OSD_BOUND_DOWN_1 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_UP_1 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_1A (MJC_BASE_ADDR + 0x464)
    #define MJC_OSD_BOUND_DOWN_2 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_OSD_BOUND_UP_2_3_0 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_OSD_BOUND_RIGHT_2 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_LEFT_2 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_1B (MJC_BASE_ADDR + 0x468)
    #define MJC_OSD_BOUND_UP_4_11_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_OSD_BOUND_RIGHT_3 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_LEFT_3 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_1C (MJC_BASE_ADDR + 0x46C)
    #define MJC_OSD_BOUND_DOWN_4_11_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_OSD_BOUND_DOWN_3 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_UP_3 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_1D (MJC_BASE_ADDR + 0x470)
    #define MJC_OSD_BOUND_DOWN_4 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_OSD_BOUND_UP_4 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_OSD_BOUND_RIGHT_4 Fld(12,12,AC_MSKW21)//[23:12]
    #define MJC_OSD_BOUND_LEFT_4 Fld(12,0,AC_MSKW10)//[11:0]
#define MJC_MC_1E (MJC_BASE_ADDR + 0x474)
    #define MJC_MVY_CLIP_THR_SEL Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_MVX_CLIP_THR_SEL Fld(1,18,AC_MSKB2)//[18:18]
    #define MJC_MVY_CLIP_OUT_SEL Fld(1,17,AC_MSKB2)//[17:17]
    #define MJC_MVX_CLIP_OUT_SEL Fld(1,16,AC_MSKB2)//[16:16]
    #define MJC_MVY_BD_SCLP_OUT Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MVX_BD_SCLP_OUT Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_1F (MJC_BASE_ADDR + 0x478)
    #define MJC_MVY_BD_SCLP_THR_N Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_MVY_BD_SCLP_THR_P Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_MVX_BD_SCLP_THR_N Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MVX_BD_SCLP_THR_P Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_20 (MJC_BASE_ADDR + 0x47C)
    #define MJC_INTP_POS_3_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_2_4 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_1_4 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_MV_INTP_POS_MAUNAL_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_INTP_POS_MAUNAL_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_MC_21 (MJC_BASE_ADDR + 0x480)
    #define MJC_INTP_POS_3_8 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_2_8 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_1_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_FW_NUM_EN Fld(1,4,AC_MSKB0)//[4:4]
#define MJC_MC_22 (MJC_BASE_ADDR + 0x484)
    #define MJC_INTP_POS_7_8 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_6_8 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_5_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_4_8 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_23 (MJC_BASE_ADDR + 0x488)
    #define MJC_INTP_POS_3_10 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_2_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_1_10 Fld(8,8,AC_FULLB1)//[15:8]
#define MJC_MC_24 (MJC_BASE_ADDR + 0x48C)
    #define MJC_INTP_POS_7_10 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_6_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_5_10 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_4_10 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_25 (MJC_BASE_ADDR + 0x490)
    #define MJC_INTP_POS_9_10 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_8_10 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_26 (MJC_BASE_ADDR + 0x494)
    #define MJC_INTP_POS_03_20 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_02_20 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_01_20 Fld(8,8,AC_FULLB1)//[15:8]
#define MJC_MC_27 (MJC_BASE_ADDR + 0x498)
    #define MJC_INTP_POS_07_20 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_06_20 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_05_20 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_04_20 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_28 (MJC_BASE_ADDR + 0x49C)
    #define MJC_INTP_POS_11_20 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_10_20 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_09_20 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_08_20 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_29 (MJC_BASE_ADDR + 0x4A0)
    #define MJC_INTP_POS_15_20 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_14_20 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_13_20 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_12_20 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_2A (MJC_BASE_ADDR + 0x4A4)
    #define MJC_INTP_POS_19_20 Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_INTP_POS_18_20 Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_INTP_POS_17_20 Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_INTP_POS_16_20 Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_MC_2B (MJC_BASE_ADDR + 0x4A8)
    #define MJC_BK_LR_SEL Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_BK_Y_SEL Fld(9,12,AC_MSKW21)//[20:12]
    #define MJC_BK_X_SEL Fld(9,0,AC_MSKW10)//[8:0]
#define MJC_STA_MC_0F (MJC_BASE_ADDR + 0x4AC)
    #define MJC_STA_BK_INFO Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_MC_ATPG (MJC_BASE_ADDR + 0x4FC)
    #define MJC_MC_BAK_UP Fld(30,2,AC_MSKDW)//[31:2]
    #define MJC_MC_BAK_UP_FW_CU_GAIN Fld(9,12,AC_MSKW21)//[20:12]
    #define MJC_MC_BAK_UP_FW_PV_GAIN Fld(9,2,AC_MSKW10)//[10:2]
    #define MJC_MC_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_MC_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page SA
#define MJC_SA_00 (MJC_BASE_ADDR + 0x6C0)
    #define MJC_ME_LBOX_HALO_REDUCE_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_MC_LBOX_HALO_REDUCE_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_RESCAN_DEBUG_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_RESCAN_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_DBG_NEXT_FRM Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PREF_EARLY_LINE_CNT Fld(4,19,AC_MSKB2)//[22:19]
    #define MJC_SA_SRAM_CS_FIXED_HIGH Fld(1,2,AC_MSKB0)//[2:2]
    #define MJC_RW_TRANS_PATCH Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_LADDER_PATCH Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_SA_01 (MJC_BASE_ADDR + 0x6C4)
    #define MJC_FB_FLIP_PROT Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_PRELOAD_FRQ_CNT Fld(8,20,AC_MSKW32)//[27:20]
    #define MJC_MSK_ME_DAT Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_FW_AUTO_RP_DBGSEL Fld(7,12,AC_MSKW21)//[18:12]
    #define MJC_SA_PREF_FREQ_CNT Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_SA_02 (MJC_BASE_ADDR + 0x6E4)
    #define MJC_SA_RESERVED Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_SA_03 (MJC_BASE_ADDR + 0x6E8)
    #define MJC_ME_ONLY_SA_READY_DLY Fld(9,0,AC_MSKW10)//[8:0]
#define MJC_SA_ATPG (MJC_BASE_ADDR + 0x6FC)
    #define MJC_SA_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_SA_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page PPC
#define MJC_PPC_00 (MJC_BASE_ADDR + 0x3E0)
    #define MJC_PPC_DAT_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_ME_PPC_EDGE_SEL Fld(3,24,AC_MSKB3)//[26:24]
    #define MJC_PPC_CLEAN_NOTFINISH Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PPC_CLEAN_OVERFLOW Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_ME_PPC_PRDC_TH Fld(11,11,AC_MSKW21)//[21:11]
    #define MJC_ME_PPC_VLD_TH Fld(11,0,AC_MSKW10)//[10:0]
#define MJC_PPC_01 (MJC_BASE_ADDR + 0x3E4)
    #define MJC_PPC_RESERVED Fld(12,20,AC_MSKW32)//[31:20]
    #define MJC_ME_PPC_DAR_BND Fld(9,11,AC_MSKW21)//[19:11]
    #define MJC_ME_PPC_DAR_TH Fld(11,0,AC_MSKW10)//[10:0]
#define MJC_STA_PPC_00 (MJC_BASE_ADDR + 0x3E8)
    #define MJC_PPC_YLRG_TH Fld(5,2,AC_MSKB0)//[6:2]
    #define MJC_STA_PPC_NOTFINISH Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_STA_PPC_OVERFLOW Fld(1,0,AC_MSKB0)//[0:0]
#define MJC_PPC_ATPG (MJC_BASE_ADDR + 0x3FC)
    #define MJC_PPC_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_PPC_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page PR
#define MJC_PR_00 (MJC_BASE_ADDR + 0x7D0)
    #define MJC_FPR_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define MJC_PR_LR_SWITCH Fld(1,29,AC_MSKB3)//[29:29]
    #define MJC_PR_EO_SWITCH Fld(1,28,AC_MSKB3)//[28:28]
    #define MJC_PR_WREQ_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define MJC_PR_RREQ_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define MJC_PR_RLAST_RST_DISAB Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_PR_WLAST_RST_DISAB Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_PR_BASE_ADDR Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_PR_01 (MJC_BASE_ADDR + 0x7D4)
    #define MJC_PR_W_VSYNCRST_SEL Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_PR_R_VSYNCRST_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_PR_WADDR_HI_LIMIT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_PR_02 (MJC_BASE_ADDR + 0x7D8)
    #define MJC_PR_WREQ_SIZE_M1 Fld(4,28,AC_MSKB3)//[31:28]
    #define MJC_PR_RREQ_SIZE_M1 Fld(4,24,AC_MSKB3)//[27:24]
    #define MJC_PR_WADDR_LO_LIMIT Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_PR_03 (MJC_BASE_ADDR + 0x7DC)
    #define MJC_PR_RINTER_REQ_REST Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_PR_RFIFO_FULL_INIT_MODE Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_PR_READ_LEN_ENAB Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_PR_STA_CLR Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_PR_RFIFO_SIZE_M1 Fld(5,16,AC_MSKB2)//[20:16]
    #define MJC_PR_10B Fld(1,15,AC_MSKB1)//[15:15]
    #define MJC_PR_DYN Fld(1,14,AC_MSKB1)//[14:14]
    #define MJC_PR_SWP Fld(1,13,AC_MSKB1)//[13:13]
    #define MJC_PR_MRR Fld(1,12,AC_MSKB1)//[12:12]
    #define MJC_PR_FLP Fld(1,11,AC_MSKB1)//[11:11]
    #define MJC_PR_ADDR_MSB Fld(3,8,AC_MSKB1)//[10:8]
    #define MJC_PR_SG_LR Fld(1,7,AC_MSKB0)//[7:7]
    #define MJC_PR_IN_LR_INV Fld(1,6,AC_MSKB0)//[6:6]
    #define MJC_PR_FRM_BUF_NUM Fld(1,5,AC_MSKB0)//[5:5]
    #define MJC_PR_MJC1W_PR1R_MODE Fld(1,4,AC_MSKB0)//[4:4]
    #define MJC_HPR_MODE Fld(1,3,AC_MSKB0)//[3:3]
    #define MJC_PR_MJC_FLIP_UDP4 Fld(1,2,AC_MSKB0)//[2:2]
#define MJC_STA_PR_00 (MJC_BASE_ADDR + 0x7E0)
    #define MJC_STA_PR_RW_IDX_CONFLICT Fld(1,25,AC_MSKB3)//[25:25]
    #define MJC_STA_PR_IN_DISPLR Fld(1,24,AC_MSKB3)//[24:24]
    #define MJC_STA_PRR_OVF_FLAG Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_STA_PRR_UDF_FLAG Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_STA_PRR_REQ_OVF_FLAG Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_STA_PRR_REQ_UDF_FLAG Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_STA_PRR_WATER_LV Fld(9,8,AC_MSKW21)//[16:8]
    #define MJC_STA_PRR_REQ_WATER_LV Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_STA_PR_01 (MJC_BASE_ADDR + 0x7E4)
    #define MJC_STA_PRW_OVF_FLAG Fld(1,23,AC_MSKB2)//[23:23]
    #define MJC_STA_PRW_UDF_FLAG Fld(1,22,AC_MSKB2)//[22:22]
    #define MJC_STA_PRW_REQ_OVF_FLAG Fld(1,21,AC_MSKB2)//[21:21]
    #define MJC_STA_PRW_REQ_UDF_FLAG Fld(1,20,AC_MSKB2)//[20:20]
    #define MJC_STA_PRW_WADDR_OOB Fld(1,19,AC_MSKB2)//[19:19]
    #define MJC_STA_PRW_WATER_LV Fld(9,8,AC_MSKW21)//[16:8]
    #define MJC_STA_PRW_REQ_WATER_LV Fld(6,0,AC_MSKB0)//[5:0]
#define MJC_PR_04 (MJC_BASE_ADDR + 0x7E8)
    #define MJC_PR_RESERVED Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_PR_05 (MJC_BASE_ADDR + 0x7EC)
    #define MJC_PR_WADDR_OOB Fld(24,0,AC_MSKDW)//[23:0]
#define MJC_PR_ATPG (MJC_BASE_ADDR + 0x7FC)
    #define MJC_PR_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_PR_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]
	
	//Page FPGA
#define MJC_FPGA_00 (MJC_BASE_ADDR + 0xF00)
    #define MJC_C_FPGA_CTRL_0 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_01 (MJC_BASE_ADDR + 0xF04)
    #define MJC_C_FPGA_CTRL_1 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_02 (MJC_BASE_ADDR + 0xF08)
    #define MJC_C_FPGA_CTRL_2 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_03 (MJC_BASE_ADDR + 0xF0C)
    #define MJC_C_FPGA_CTRL_3 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_04 (MJC_BASE_ADDR + 0xF10)
    #define MJC_C_FPGA_CTRL_4 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_05 (MJC_BASE_ADDR + 0xF14)
    #define MJC_C_FPGA_CTRL_5 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_06 (MJC_BASE_ADDR + 0xF18)
    #define MJC_C_FPGA_CTRL_6 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_07 (MJC_BASE_ADDR + 0xF1C)
    #define MJC_C_FPGA_CTRL_7 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_08 (MJC_BASE_ADDR + 0xF20)
    #define MJC_C_FPGA_CTRL_8 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_09 (MJC_BASE_ADDR + 0xF24)
    #define MJC_C_FPGA_CTRL_9 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_10 (MJC_BASE_ADDR + 0xF28)
    #define MJC_C_FPGA_CTRL_10 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_11 (MJC_BASE_ADDR + 0xF2C)
    #define MJC_C_FPGA_CTRL_11 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_12 (MJC_BASE_ADDR + 0xF30)
    #define MJC_C_FPGA_CTRL_12 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_13 (MJC_BASE_ADDR + 0xF34)
    #define MJC_C_FPGA_CTRL_13 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_14 (MJC_BASE_ADDR + 0xF38)
    #define MJC_C_FPGA_CTRL_14 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_15 (MJC_BASE_ADDR + 0xF3C)
    #define MJC_C_FPGA_CTRL_15 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_16 (MJC_BASE_ADDR + 0xF40)
    #define MJC_STA_FPGA_VERSION Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_17 (MJC_BASE_ADDR + 0xF44)
    #define MJC_STA_FPGA_OT Fld(8,24,AC_FULLB3)//[31:24]
    #define MJC_STA_FPGA_MV Fld(8,16,AC_FULLB2)//[23:16]
    #define MJC_STA_FPGA_MC Fld(8,8,AC_FULLB1)//[15:8]
    #define MJC_STA_FPGA_ME Fld(8,0,AC_FULLB0)//[7:0]
#define MJC_FPGA_18 (MJC_BASE_ADDR + 0xF48)
    #define MJC_STA_FPGA_OWNER Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_19 (MJC_BASE_ADDR + 0xF4C)
    #define MJC_STA_FPGA_19 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_20 (MJC_BASE_ADDR + 0xF50)
    #define MJC_STA_FPGA_20 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_21 (MJC_BASE_ADDR + 0xF54)
    #define MJC_STA_FPGA_21 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_22 (MJC_BASE_ADDR + 0xF58)
    #define MJC_STA_FPGA_22 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_23 (MJC_BASE_ADDR + 0xF5C)
    #define MJC_STA_FPGA_23 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_24 (MJC_BASE_ADDR + 0xF60)
    #define MJC_STA_FPGA_24 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_25 (MJC_BASE_ADDR + 0xF64)
    #define MJC_STA_FPGA_25 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_26 (MJC_BASE_ADDR + 0xF68)
    #define MJC_STA_FPGA_26 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_27 (MJC_BASE_ADDR + 0xF6C)
    #define MJC_STA_FPGA_27 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_28 (MJC_BASE_ADDR + 0xF70)
    #define MJC_STA_FPGA_28 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_29 (MJC_BASE_ADDR + 0xF74)
    #define MJC_STA_FPGA_29 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_30 (MJC_BASE_ADDR + 0xF78)
    #define MJC_STA_FPGA_30 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_31 (MJC_BASE_ADDR + 0xF7C)
    #define MJC_STA_FPGA_31 Fld(32,0,AC_FULLDW)//[31:0]
#define MJC_FPGA_ATPG (MJC_BASE_ADDR + 0xF80)
    #define MJC_FPGA_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define MJC_FPGA_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
 
#endif // #ifndef _HW_MJC_H_

