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
 * $RCSfile: dm.h,v $
 * $Revision: #10 $
 * $Date: 2013/01/20 $
 * $Author: xiaojun.zheng $
 * $SWAuthor: $
 *
 * Description: 
 *         This file contains all the transition effect interface APIs  
 *---------------------------------------------------------------------------*/

#ifndef _DM_H_
#define _DM_H_

//#define _DRV_SUP_USB_OVER_CURRENT_


#include <sys/queue.h>
#include <cyg/kernel/kapi.h> 
#include <cyg/io/devtab.h>




/*
 *  Handle types supported by the Device Manager.
 */
//#define DMT_DEV_HANDLE  (HT_GROUP_DM + ((HANDLE_TYPE_T) 0))
//#define DMT_NFY_HANDLE  (HT_GROUP_DM + ((HANDLE_TYPE_T) 1))
//#define DMT_IOC_HANDLE  (HT_GROUP_DM + ((HANDLE_TYPE_T) 2))

//#define DM_DEV_SKT_MAX_SLOT_NS        6         /**<Maximum slot number of a socket device        */
//#define DM_DEV_MED_DISC_MAX_LAYER_NS  5         /**<Maximum layer number of a medium disc        */
//#define DM_DEV_MAX_MODEL_NAME_LEN     64        /**<Maximum length of the device's model name        */
#define DM_MAX_LABEL_NAME_LEN         32        /**<Maximum length of device's path name        */
//#define DM_DEV_ROOT_DEV_LABEL         "/dev"    /**<The string lable for the root device        */

typedef void* HANDLE;
typedef enum _DM_CB_CAT_T
{
    DM_CB_CAT_DEV = 0x0,
    DM_CB_CAT_LDR = 0x100
} DM_CB_CAT_T;
typedef enum _DM_HW_TYPE_T  
{
    DM_HW_TYPE_UNKNOWN  = 0,
    DM_HW_TYPE_EEPROM   = 1,
    DM_HW_TYPE_NAND     = 2,
    DM_HW_TYPE_NOR      = 3,
    DM_HW_TYPE_USB      = 4,
    DM_HW_TYPE_IDE      = 5,
    DM_HW_TYPE_FCI      = 6,
    DM_HW_TYPE_1394     = 7,
    DM_HW_TYPE_ETHERNET = 8,
    DM_HW_TYPE_MHL      = 9,
    /* ChunZhou, add for virtual ISO device */
    DM_HW_TYPE_VIRTUAL  = 10
} DM_HW_TYPE_T;
typedef enum _DRV_TYPE_T  
{
    DRVT_HW_EEPROM   = 0,
    DRVT_HW_NAND     = 1,
    DRVT_HW_NOR      = 2,
    DRVT_HW_USB      = 3,
    DRVT_HW_IDE      = 4,
    DRVT_HW_FCI      = 5,
    DRVT_HW_1394     = 6,
    DRVT_HW_ETHERNET = 7,
    /* ChunZhou, add for virtual ISO device */
    DRVT_HW_MHL  	 = 8
} DRV_TYPE_T;

typedef enum _DM_CB_TYPE_T   /*  all the CB from device driver should be enumerated here */
{
    DM_CB_DEV_UNSUPPORT    = DM_CB_CAT_DEV,
    DM_CB_DEV_FAIL,
    DM_CB_DEV_ATTACH,
    DM_CB_DEV_DETACH,

    DM_CB_TRAY_ERROR       = DM_CB_CAT_LDR,
    DM_CB_TRAY_OPEN,
    DM_CB_TRAY_CLOSE,
    DM_CB_TRAY_OPENING,
    DM_CB_TRAY_CLOSING,
    DM_CB_DISC_LOADING,
    DM_CB_DISC_ABSENT,
    DM_CB_DISC_PRESENT
} DM_CB_TYPE_T;

typedef enum _DM_DEV_CHR_T
{
    DM_DEV_CHR_UNKNOWN  = 0x0,
    DM_DEV_CHR_SOCKET   = 0x100,
    DM_DEV_CHR_MEDIUM   = 0x200,
    /*zink CSR BT*/
    DM_DEV_CHR_MISC     = 0x400,

    /* ChunZhou, add for virtual ISO device */
    DM_DEV_CHR_VIRTUAL  = 0x800,
    /*guangjie add, for usb input device, ex:mouse, keyboard, etc*/
    DM_DEV_CHR_INPUT    = 0x900
} DM_DEV_CHR_T;

typedef enum _DM_DEV_TYPE_T
{
    DM_DEV_UKW_TYPE_UNKNOWN     = DM_DEV_CHR_UNKNOWN,

    DM_DEV_SKT_TYPE_UNKNOWN     = DM_DEV_CHR_SOCKET,
    DM_DEV_SKT_TYPE_EEPROM_HW,
    DM_DEV_SKT_TYPE_NAND_HW,
    DM_DEV_SKT_TYPE_NOR_HW,
    DM_DEV_SKT_TYPE_USB_HW,
    DM_DEV_SKT_TYPE_MHL_HW,
    DM_DEV_SKT_TYPE_IDE_HW,
    DM_DEV_SKT_TYPE_FCI_HW,
    DM_DEV_SKT_TYPE_1394_HW,    
    DM_DEV_SKT_TYPE_HUB,
    DM_DEV_SKT_TYPE_CARD_READER,
    DM_DEV_SKT_TYPE_OPTI_DRV,
    DM_DEV_SKT_TYPE_ETHERNET,

    DM_DEV_MED_TYPE_UNKNOWN     = DM_DEV_CHR_MEDIUM,
    DM_DEV_MED_TYPE_EEPROM,
    DM_DEV_MED_TYPE_NAND,
    DM_DEV_MED_TYPE_NOR,
    DM_DEV_MED_TYPE_HDD,
    DM_DEV_MED_TYPE_DISC,
    DM_DEV_MED_TYPE_MEM_CARD,
    DM_DEV_MED_TYPE_MASS_STRG,
    DM_DEV_MED_TYPE_PTP_MTP,
    DM_DEV_MED_TYPE_DLNA_DMS,
    DM_DEV_MED_TYPE_SMB,
    /*zink CSR BT*/
    DM_DEV_MISC_TYPE_UNKNOWN     = DM_DEV_CHR_MISC,
    DM_DEV_MISC_TYPE_USB_BT,

    
    /* ChunZhou, add for virtual ISO device */
    DM_DEV_VIRTUAL_TYPE_UNKNOWN     = DM_DEV_CHR_VIRTUAL,
    DM_DEV_VIRTUAL_TYPE_ISO,
    /*guangjie add, for usb input device, ex:mouse, keyboard, etc*/
    DM_DEV_INPUT_DEVICE         = DM_DEV_CHR_INPUT
} DM_DEV_TYPE_T;

#define DM_DEV_CHR(x) ((DM_DEV_CHR_T)(x & 0xFF00))    /**< Device characteristic mask*/


#define DM_CB_CAT(x) (x & 0xFF00)

typedef struct _DM_MNT_DESC_T
{
    unsigned int ui4_part_idx;

    //char ps_part_name[DM_MAX_LABEL_NAME_LEN];
    char ps_mnt_path[DM_MAX_LABEL_NAME_LEN];

    SLIST_ENTRY(_DM_MNT_DESC_T) t_link;
} DM_MNT_DESC_T;

typedef struct _DM_DEV_DESC_T
{
    //void*      h_drv;  /*  handle get from Resource Manager */

    unsigned int  ui4_dev_flag;
	DM_HW_TYPE_T  t_hw_type;
    DM_DEV_TYPE_T t_dev_type;
	
    struct _DM_DEV_DESC_T *pt_root_dev;
    struct _DM_DEV_DESC_T *pt_prnt_dev;
	
	unsigned int ui4_unit_id;
    unsigned int ui4_skt_no;

    void *pv_data;
    unsigned int ui4_data_size;

    bool  b_avail;

    char ps_dev_name[DM_MAX_LABEL_NAME_LEN];

    SLIST_ENTRY(_DM_DEV_DESC_T) t_link;


    SLIST_HEAD(mnt_list,_DM_MNT_DESC_T)t_mnt_list ;
    cyg_mutex_t h_mnt_rwlock;
    void *pv_resv;
	struct cyg_devtab_entry *dev;
} DM_DEV_DESC_T;




typedef struct _DM_CB_MSG_T
{
    DM_CB_TYPE_T  t_cb_type;
    unsigned int t_dev_type;

    unsigned int ui4_unit_id;
    unsigned int ui4_skt_no;

    unsigned int ui4_dev_flag;

    void *pv_data;
    unsigned int ui4_data_sz;

   /*  This is used to identify the issuer device that issues the CB and
    *  in type of _DM_DEV_DESC_T.  Since the CB msgs are ordered in queue,
    *  we will not encounter the case that this points to an invalid parent.
    */

    void *pv_tag;
} DM_CB_MSG_T;





typedef struct _DM_CORE_T
{
    //SLIST_T(_DM_NFY_DESC_T) t_nfy_list;
    //cyg_mutex_t h_nfy_rwlock;

    SLIST_HEAD(dev_list,_DM_DEV_DESC_T)t_dev_list;
    cyg_mutex_t h_dev_rwlock;

  /*
   *  We use linked list rather than msg queue since we need to remove the un-delivered
   *  msgs when the listener is removed.
   */


    cyg_handle_t h_cb_msg_thrd;
    cyg_handle_t h_cb_msg_q;
	cyg_mbox h_cb_msg_mbox;
    bool b_init;
} DM_CORE_T;

extern void usb_fsdm_init(void);

//static int _dm_automnt_umnt(void *pv_data);
//static int _dm_automnt_mnt(void *pv_data);
//static cyg_devtab_entry* dev_entry_creat(unsigned int number);

    
#endif

