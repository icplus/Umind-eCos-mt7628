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

/**
 * @file Local Dimming driver interface
 *
 * @author dragon_chen@mtk.com.tw
 */

#ifndef _DRV_LCDIM_V2_H_
#define _DRV_LCDIM_V2_H_

// *-----------------------------------------------------------------------------
// * structure / global variables
// *-----------------------------------------------------------------------------
#define LCDIM_LIGHTPROF_NUM	        21*128
#define LCDIM_DIMMING_NUM           33
#define LCDIM_GAIN_NUM              33
#define LCDIM_DEMO_ROUND            3
#define LCDIM_MAX_BLK_NUM           200
#define LCDIM_MAX_HIST_NUM          16

extern UINT8 u1LightProfileTable[LCDIM_LIGHTPROF_NUM];
extern UINT16 u2LcdimGainTable[LCDIM_GAIN_NUM];


void vDrvLcdimWriteLightProfile(UINT8 *u1LightProfile);
void vDrvLcdimReadLightProfile(UINT8 *u1LightProfile);
void vDrvLcdimWriteDimTable(UINT16 *u2DimTable);
void vDrvLcdimReadDimTable(UINT16 *u2DimTable);
void vDrvLcdimWriteGainTable(UINT16 *u2GainTable);
void vDrvLcdimWriteGainLimitTable(UINT16 *u2GainLimitTable);
void vDrvLcdimReadGainTable(UINT16 *u2GainTable);
void vDrvLcdimQualitySWInit(void);
void vDrvLcdimQualityHWInit(void);
void vDrvLcDimQualityInit(void);
BOOL bDrvLcDimPanelInit(void);
BOOL bDrvLcdimDetectBlkDimChange(void);
BOOL bDrvLcdimDetectSceneChange(void);
void vDrvLcdimResetTemporalFilter(UINT8 bOnOff);
void vDrvLcdimSCGProc(void);
void vDrvLcdimSPIProc(void);
void vDrvLcdimProc(void);
void vDrvLcdimSetPhase(UINT8 *u1LcdimPhase);
void vDrvSetLcDimOnOff(UINT8 bOnOff);
void vDrvLcdimForceBlockDimOnOff(UINT8 bOnOff);
void vDrvLcdimForceBlockDimCtrl(UINT8 u1PosH, UINT8 u1PosV, UINT8 u1BlockOnOff, UINT8 u1ForceLevel);
void vDrvGetLcDimDimmingResult(UINT8 *u1DimmingResult); 
BOOL bDrvLcDimSetPanelInfo(UINT32 u4BlockNumH, UINT32 u4BlockNumV, UINT8 u1LightDirection);
void vDrvLcdimSetInk(UINT8 u1InkType);
void vDrvLcdimSetInkColor(UINT16 u2ColorR, UINT16 u2ColorG, UINT16 u2ColorB);
UINT32 u4DrvLcdimReadBlockInfo(UINT8 u1H, UINT8 u1V, UINT8 u1InfoType);
BOOL vDrvVerifyLcDimLightProfile(UINT8 *bLcDimLightProfile);
BOOL vDrvVerifyLcDimDimTBL(UINT16 *u2LcDimDimTbl);
BOOL vDrvVerifyLcDimGainTBL(UINT16 *bu2LcDimGainTbl);
void vDrvSetLcDSOnOff(BOOL bOnOff);
void vDrvLcdimSetDSDemo(UINT8 u1DemoType, BOOL bOnOff);
void vDrvLcdimSetDemoEffect(UINT8 *bDimmingResult);
void vDrvLcdimSetDemoParam(UINT8 bSel);
UINT32 u4QueAverage(UINT32 u4Value, UINT8 bItem, UINT32 u4QueSize);
void vDrvClearDataQue(void);
void vDrvAdaptiveLcDimUpdateParam(void);
void vDrvLcDimCreateAdaptiveCurve(UINT16 *u2LcdimCurveDft, UINT16 *u2LcdimCurveStr, UINT32 u4Confidence);
void vDrvAdaptiveCurveRemapping(UINT16 *u2LcdimCurveDft, UINT16 *u2LcdimCurveStr);
void vDrvAdaptiveLcDim(UINT16 *u2LcdimCurveDft, UINT16 *u2LcdimCurveStr);
#endif //_DRV_LCDIM_H_

