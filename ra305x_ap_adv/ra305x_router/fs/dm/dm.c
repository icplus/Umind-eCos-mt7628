//---------------------------------------------------------------------------




//#include "inc/x_typedef.h"
//#include <sys/malloc.h>
#include <stdio.h>
#include <fcntl.h>
#include <cyg/io/disk.h>
#include <cyg/io/devtab.h>
#include <dirent.h> 
#include <sys/stat.h>
#include "dm.h"
#ifdef CONFIG_USBHOST
#include "../../drivers/usbhost/include/mu_hfi_if.h"
#include "../../drivers/usbhost/include/musb_if.h"
#endif

//---------------------------------------------------------------------------

#define CB_MSG_STACK_SIZE 4096

#define DM_CB_MSG_THREAD_DEFAULT_PRIORITY 12
static cyg_thread thread_obj; 
DM_CORE_T t_dm_core = {0};
#ifdef CONFIG_USBHOST
MUSB_HOTSWAP_NFY_INFO_T t_dev_st_nfy={0};
#endif
static char surfix='C';
char cb_msg_thread_stack[CB_MSG_STACK_SIZE];
//extern int MUSB_Init(int u4Mode);

bool
usb_io_compare(const char *n1, const char *n2, const char **ptr)
{
    while (*n1 && *n2) {
        if (*n1++ != *n2++) {
            return false;
        }
    }
    if (*n1) {
        // See if the devtab name is is a substring
        if (*(n2-1) == '/') {
            *ptr = n1;
            return true;
        }
    }
    if (*n1 || *n2) {
        return false;
    }
    *ptr = n1;
    return true;
}

Cyg_ErrNo
usb_io_lookup(const char *name, cyg_io_handle_t *handle)
{
	union devtab_entry_handle_union {
        cyg_devtab_entry_t *st;
        cyg_io_handle_t h;
    } stunion;
    cyg_devtab_entry_t *t;
    Cyg_ErrNo res;
    const char *name_ptr;
	DM_DEV_DESC_T *pt_dev_desc = NULL;

    SLIST_FOREACH(pt_dev_desc, &(t_dm_core.t_dev_list), t_link){
		t = pt_dev_desc->dev;
		if(pt_dev_desc->dev == NULL) continue;
		//diag_printf("file test:subname=%s,devname=%s\n",name,t->name);
        if (usb_io_compare(name, t->name, &name_ptr)) {
            // FUTURE: Check 'avail'/'online' here
            if (t->dep_name) {
				diag_printf("dep_name is not NULL\n");
                res = usb_io_lookup(t->dep_name, &stunion.h);
                if (res != ENOERR) {
                    return res;
                }
            } else {
                stunion.st = NULL;
            }
			//diag_printf("file test:name_ptr=%s\n",name_ptr);
            if (t->lookup) {
                // This indirection + the name pointer allows the lookup routine
                // to return a different 'devtab' handle.  This will provide for
                // 'pluggable' devices, file names, etc.
                res = (t->lookup)(&t, stunion.st, name_ptr);
                if (res != ENOERR) {
                    return res;
                }
            }
            *handle = (cyg_io_handle_t)t;
			//diag_printf("file test:usb_io_lookup_end\n");
            return ENOERR;
        }
    }
    return -ENOENT;  // Not found
}
//---------------------------------------------------------------------------
static int _dm_automnt_mnt(
    void *pv_data)
{
	diag_printf("_dm_automnt_mnt_start\n");
	DM_DEV_DESC_T *pt_dev_desc=(DM_DEV_DESC_T *)pv_data;
	struct cyg_devtab_entry *dev=pt_dev_desc->dev;
	//char *pdir=dir;
	disk_channel *chan=(disk_channel*)dev->priv;
	cyg_disk_info_t *info=chan->info;
	DM_MNT_DESC_T * pt_mnt_desc=NULL;
    const char devname[20],dirname[32];
	char *pdev,*pdir;
	int dev_num,i,result = -1,result1 = -1,result2 = -1;
    strcpy(devname,dev->name);
    strcpy(dirname,dev->name);
	int length=strlen(dev->name);
	dev_num=info->partitions_num;
	for(i=0;i<dev_num;i++)
	{
        //devname ex:/disk0/1
        pdev = devname;
		pdev=devname+length;
		*pdev++='0'+i+1;*pdev=0;
        //dirname ex:/disk0_C/
        pdir = dirname;
		pdir=dirname+length-1;
        *pdir++='_';*pdir++='C'+i;
		*pdir=0;
        
		pt_mnt_desc = (DM_MNT_DESC_T *)malloc(sizeof(DM_MNT_DESC_T));
		if (pt_mnt_desc == NULL)
		{
         	diag_printf(("_dm_automnt_mnt alloc mem error.\n"));
            	return -1;
        }
       	memset((void *) pt_mnt_desc, 0, sizeof(DM_MNT_DESC_T));
        pt_mnt_desc->ui4_part_idx = i+1;
       	memcpy(pt_mnt_desc->ps_mnt_path,dirname,DM_MAX_LABEL_NAME_LEN);
        
        //add a dir at '/',here dirname should be /disk0_C
        //*(pdir-1) = 0;
        result = mkdir(dirname,O_ACCMODE);
        if(result!=0) {diag_printf("rootfs mkdir:%s fail\n",dirname);}
        
		result1 = mount(devname,pt_mnt_desc->ps_mnt_path,"fatfs");
		if(result1!=0) 
        {
            result2 = mount(devname,pt_mnt_desc->ps_mnt_path,"ntfs");
            if(result2 != 0)
            {
                diag_printf("mount %s fail\n",dirname);
                free(pt_mnt_desc);
                rmdir(dirname);
                continue;
            }  
        }     
        
        //pt_mnt_desc->ps_mnt_path=dir;
       	SLIST_INSERT_HEAD( &(pt_dev_desc->t_mnt_list),pt_mnt_desc, t_link);
        
	}
	diag_printf("_dm_automnt_mnt_end\n");
	return 0;
}

static int _dm_automnt_umnt(VOID *pv_data)
{
	diag_printf("_dm_automnt_umnt_start\n");
	DM_DEV_DESC_T *pt_dev_desc=(DM_DEV_DESC_T *)pv_data;
	struct cyg_devtab_entry *dev=pt_dev_desc->dev;
	disk_channel *chan=(disk_channel *)dev->priv;
	cyg_disk_info_t *info=chan->info;
	cyg_disk_partition_t *disk_partitions=info->partitions;
	struct cyg_devtab_entry *part_dev=chan->pdevs_dev;
	disk_channel *part_chan=chan->pdevs_chan;
	//dev_num=info->partitions_num;
	int result = -1;
    
	DM_MNT_DESC_T * pt_mnt_desc=NULL;
	SLIST_FOREACH(pt_mnt_desc, &(pt_dev_desc->t_mnt_list), t_link)
	{
		if(pt_mnt_desc == NULL)
        {
            continue;
        }
		result = umount(pt_mnt_desc->ps_mnt_path);
        SLIST_REMOVE(&(pt_dev_desc->t_mnt_list),pt_mnt_desc,_DM_MNT_DESC_T,t_link);
        if(result!=0) {diag_printf("umount %s fail\n",pt_mnt_desc->ps_mnt_path); continue;}
        int length = strlen(pt_mnt_desc->ps_mnt_path);
        char *mnt_path[100]={0};
        strcpy(mnt_path,pt_mnt_desc->ps_mnt_path);
        mnt_path[length-1] = '\0';
        diag_printf("ps_mnt_path=%s\n",mnt_path);
        result = rmdir(mnt_path);
        if(result != 0){diag_printf("warning:rootfs rmdir fail\n");continue;}
		free(pt_mnt_desc);
	}
EXIT:
	free(part_chan);
	free(part_dev);
	int i;
	cyg_disk_partition_t *part,*pre;
	for(i= 0;i < 4;i++)
	{
		part = info->partitions[i].pnext;
		while(part)
		{
			pre = part;
			part = part->pnext;
			free(pre);
		}
	}
	free(disk_partitions);
	free(info);
	free(chan);
	free(dev->name);
	free(dev);
	diag_printf("_dm_automnt_umnt_end\n");
	return 0;
}




static DM_CB_TYPE_T _dm_get_cb_type(
    int t_dev_stat)
{
    switch (t_dev_stat)
    {
        case HFI_STATUS_READY:                      return DM_CB_DEV_ATTACH;
        case HFI_STATUS_ABSENT:                      return DM_CB_DEV_DETACH;
        case HFI_STATUS_FAIL:                        return DM_CB_DEV_FAIL;
        //case SKTDEV_STATUS_DEV_UNSUPPORT:                   return DM_CB_DEV_UNSUPPORT;
        default:                          ;
    }

    return DM_CB_DEV_UNSUPPORT;
}
static DM_HW_TYPE_T _dm_get_hw_type(
    DRV_TYPE_T ui2_drv_type)
{
    switch (ui2_drv_type)
    {
        case DRVT_HW_EEPROM:        return DM_HW_TYPE_EEPROM;
        case DRVT_HW_NAND:          return DM_HW_TYPE_NAND;
        case DRVT_HW_NOR:           return DM_HW_TYPE_NOR;
        case DRVT_HW_USB:           return DM_HW_TYPE_USB;
        case DRVT_HW_IDE:           return DM_HW_TYPE_IDE;
        case DRVT_HW_FCI:           return DM_HW_TYPE_FCI;
        case DRVT_HW_1394:          return DM_HW_TYPE_1394;
        case DRVT_HW_ETHERNET:      return DM_HW_TYPE_ETHERNET;
#ifdef SYS_MHL_SUPPORT
        case DRVT_HW_MHL:           return DM_HW_TYPE_MHL;
#endif
        default:                    ;
    }

    return DM_HW_TYPE_UNKNOWN;
}
#ifdef CONFIG_USBHOST
static VOID _dm_dev_nfy_fct(void *pv_nfy_tag)
{
    DM_CB_MSG_T *t_cb_msg = NULL;
	MUSB_HOTSWAP_NFY_INFO_T *pt_sktdev_info=&t_dev_st_nfy;
    int i4_ret;
	t_cb_msg = (DM_CB_MSG_T *)malloc(sizeof(DM_CB_MSG_T));
	if(NULL == t_cb_msg)
		{
			diag_printf("_dm_dev_nfy_fct malloc fail\n");
			return;
		}
    if (pt_sktdev_info->u4Status < 3) /*  general socket device status */
    {
        
        t_cb_msg->pv_tag = pv_nfy_tag;
        t_cb_msg->t_cb_type = _dm_get_cb_type(pt_sktdev_info->u4Status);
        t_cb_msg->ui4_skt_no = pt_sktdev_info->u4SocketNo;
        t_cb_msg->t_dev_type = DM_DEV_CHR_MEDIUM;
        t_cb_msg->ui4_unit_id = pt_sktdev_info->u4CompId;
       	t_cb_msg->ui4_dev_flag = 1;
        t_cb_msg->pv_data = NULL;
        t_cb_msg->ui4_data_sz = 0;
   

        diag_printf("_dm_dev_nfy_fct: general event:0x%x device type:0x%x socket #:%d", 
                                  t_cb_msg->t_cb_type,
                                  t_cb_msg->t_dev_type,
                                  t_cb_msg->ui4_skt_no);
	}

    else
    {
        /*  suppose that the proprietary status has no pv_data */
        t_cb_msg->pv_tag = pv_nfy_tag;
        t_cb_msg->t_cb_type = _dm_get_cb_type(pt_sktdev_info->u4Status);
        t_cb_msg->ui4_skt_no = 0;
        t_cb_msg->t_dev_type = DM_DEV_UKW_TYPE_UNKNOWN;

        t_cb_msg->ui4_unit_id = 0;
        t_cb_msg->ui4_dev_flag = 0;

        t_cb_msg->pv_data = NULL;
        t_cb_msg->ui4_data_sz = 0;

        diag_printf(("_dm_dev_nfy_fct: proprietary event:0x%x", t_cb_msg->t_cb_type));
    }

    cyg_mbox_put(t_dm_core.h_cb_msg_q,(void *) t_cb_msg);

}
#endif
static void _dm_cb_msg_thread(
    void *pv_data)
{
    DM_CB_MSG_T* t_cb_msg;
    //UINT16 ui2_q_idx;
    unsigned int z_msg_size;

    //int i4_ret;

    while (1)
    {
        DM_CB_TYPE_T t_cb_type;
        DM_DEV_TYPE_T  t_dev_type;

        unsigned int ui4_unit_id;
        unsigned int ui4_skt_no;

        unsigned int ui4_dev_flag;

        void *pv_data;
        unsigned int ui4_data_size;

        DM_DEV_DESC_T *pt_cb_dev = NULL;

        z_msg_size = sizeof(DM_CB_MSG_T);

        t_cb_msg = (DM_CB_MSG_T *)cyg_mbox_get(t_dm_core.h_cb_msg_q);
		
        if (t_cb_msg == NULL)
        {
            diag_printf(("_dm_cb_msg_thread receive msg error."));
            goto fail;
        }
		diag_printf("_dm_cb_msg_thread: general event:0x%x device type:0x%x socket #:%d\n", 
                                  t_cb_msg->t_cb_type,
                                  t_cb_msg->t_dev_type,
                                  t_cb_msg->ui4_skt_no);

        t_cb_type = t_cb_msg->t_cb_type;
        t_dev_type = t_cb_msg->t_dev_type;

        ui4_unit_id = t_cb_msg->ui4_unit_id;
        ui4_skt_no = t_cb_msg->ui4_skt_no;

        ui4_dev_flag = t_cb_msg->ui4_dev_flag;

        pv_data = t_cb_msg->pv_data;
        ui4_data_size = t_cb_msg->ui4_data_sz;

        pt_cb_dev = (DM_DEV_DESC_T *) t_cb_msg->pv_tag;
		free(t_cb_msg);

        if(pt_cb_dev == NULL)
        {
            diag_printf("%s(%d): tag is NULL\n", __FUNCTION__, __LINE__);
            continue;
        }

        if ((DM_CB_CAT_T)DM_CB_CAT(t_cb_type) == DM_CB_CAT_DEV)
        {
            /*
             *  The event call-backer is the parent device.
             */

            if (t_cb_type == DM_CB_DEV_ATTACH)
            {

                DM_DEV_DESC_T *pt_dev_desc = NULL;
                DM_DEV_DESC_T *pt_dev_desc_lst = NULL;
                bool b_found = false;
                bool b_avail = true;               
				cyg_mutex_lock(&t_dm_core.h_dev_rwlock);
                
                /*
                 *  Scan the list to check if the device is already attached.
                 */

                SLIST_FOREACH(pt_dev_desc, &(t_dm_core.t_dev_list), t_link)
                {
                    if ((pt_dev_desc->pt_prnt_dev == pt_cb_dev) &&
                        (pt_dev_desc->ui4_skt_no == ui4_skt_no))
                    {
                        /*ASSERT(pt_dev_desc->pt_root_dev == 
                                      pt_cb_dev->pt_root_dev);
                        ASSERT(pt_dev_desc->t_hw_type == 
                                      pt_cb_dev->t_hw_type);
                        ASSERT(pt_dev_desc->t_dev_type == t_dev_type);
                        ASSERT(pt_dev_desc->ui4_unit_id == ui4_unit_id);*/

                        b_found = true;
						diag_printf("file test:found in dev list\n");

                        break;
                    }
                    
                    pt_dev_desc_lst = pt_dev_desc;
                }

                if (b_found == false)
                {
                    pt_dev_desc = (DM_DEV_DESC_T *)malloc(sizeof(struct _DM_DEV_DESC_T));
					if (pt_dev_desc == NULL)
    				{
        				diag_printf(("_dm_init_hw_comp alloc mem error."));
        				//i4_ret = DMR_OS;
        				goto fail;
    				}
					memset((VOID *) pt_dev_desc, 0, sizeof(DM_DEV_DESC_T));
					pt_dev_desc->ui4_dev_flag = ui4_dev_flag;
    				pt_dev_desc->pt_prnt_dev = pt_cb_dev;
    				pt_dev_desc->pt_root_dev = pt_cb_dev->pt_root_dev;
    				pt_dev_desc->t_dev_type = t_dev_type;
    				pt_dev_desc->t_hw_type = pt_cb_dev->t_hw_type;
    				pt_dev_desc->ui4_skt_no = ui4_skt_no;
    				pt_dev_desc->ui4_unit_id = ui4_unit_id;
    				pt_dev_desc->pv_data = pv_data;
    				pt_dev_desc->ui4_data_size = ui4_data_size;
    				//pt_dev_desc->h_drv = h_drv;
    				pt_dev_desc->b_avail = true;

                    if (b_avail == true)
                    {
                        if ((DM_DEV_CHR_T)DM_DEV_CHR(t_dev_type) == DM_DEV_CHR_MEDIUM)
                        {
                            //unsigned int ui4_mode;
                            char *ps_dev_name=pt_dev_desc->ps_dev_name;
                            //FM_MNT_CB_T t_mnt_cb;

                           // ui4_mode = (FM_MODE_TYPE_BLK | FM_MODE_DEV_TRUE);
							//device name
                            memcpy(ps_dev_name,"disk_",5);
							ps_dev_name+=5;
							*ps_dev_name++='0'+ui4_unit_id;
							*ps_dev_name=0;

							pt_dev_desc->dev=dev_entry_creat(ui4_unit_id);
							if(pt_dev_desc->dev == NULL) return;
                            
                        }
                        //zink CSR BT
                        else if (DM_DEV_CHR(t_dev_type) == DM_DEV_CHR_MISC)
                        {
                        }
                    }

                    pt_dev_desc->b_avail = b_avail;

                    if (pt_dev_desc_lst == NULL)
                    {
                        SLIST_INSERT_HEAD(&t_dm_core.t_dev_list,pt_dev_desc, t_link);
                    }
                    else
                    {
                        SLIST_INSERT_AFTER(pt_dev_desc_lst,pt_dev_desc,t_link);
                    }
                }
		else 
		{
		cyg_mutex_unlock(&t_dm_core.h_dev_rwlock);continue;
		}
                cyg_mutex_unlock(&t_dm_core.h_dev_rwlock);
                if (((DM_DEV_CHR_T)DM_DEV_CHR(t_dev_type) == DM_DEV_CHR_MEDIUM) && (b_avail == true))
                {
                    SLIST_INIT(&(pt_dev_desc->t_mnt_list));
                    _dm_automnt_mnt(pt_dev_desc);
                }
				test();
                continue;
            } 
            else if (t_cb_type == DM_CB_DEV_DETACH)
            {
				diag_printf("file test:DM_CB_DEV_DETACH\n");
                DM_DEV_DESC_T *pt_dev_desc = NULL;
                bool b_found = false;
                cyg_mutex_lock(&t_dm_core.h_dev_rwlock);
                
                /*
                 *  Scan the list to check if the device is already attached.
                 */

                SLIST_FOREACH(pt_dev_desc, &(t_dm_core.t_dev_list), t_link)
                {
                    if ((pt_dev_desc->pt_prnt_dev == pt_cb_dev) &&
                        (pt_dev_desc->ui4_skt_no == ui4_skt_no))
                    {
                        /*ASSERT(pt_dev_desc->pt_root_dev == 
                                      pt_cb_dev->pt_root_dev);
                        ASSERT(pt_dev_desc->t_hw_type == 
                                      pt_cb_dev->t_hw_type);*/

                        b_found = true;
						diag_printf("file test:found in dev list\n");
                        SLIST_REMOVE(&(t_dm_core.t_dev_list),pt_dev_desc, _DM_DEV_DESC_T,t_link);
                        break;
                    }
                }

                cyg_mutex_unlock(&t_dm_core.h_dev_rwlock);

                /*
                 *  Raise event request.
                 */

                if (b_found == true)
                {
                	
                    if (((DM_DEV_CHR_T)DM_DEV_CHR(t_dev_type) == DM_DEV_CHR_MEDIUM) && (pt_dev_desc->b_avail == true))
                    {
                        _dm_automnt_umnt(pt_dev_desc);
                    }
                }

				continue;
                
            } 
           /* else if ((t_cb_type == DM_CB_DEV_FAIL) || (t_cb_type == DM_CB_DEV_UNSUPPORT))
            {


                DM_DEV_DESC_T *pt_dev_desc = NULL;
                DM_DEV_DESC_T *pt_dev_desc_lst = NULL;
                BOOL b_found = FALSE;

                DM_NFY_DESC_T *pt_nfy_desc = NULL;

                ASSERT(((t_cb_type == DM_CB_DEV_FAIL) && 
                        (t_dev_type != DM_DEV_UKW_TYPE_UNKNOWN)) ||
                        ((t_cb_type == DM_CB_DEV_UNSUPPORT) &&
                        (t_dev_type == DM_DEV_UKW_TYPE_UNKNOWN)));

                if (x_rwl_read_lock(t_dm_core.h_nfy_rwlock, RWL_OPTION_WAIT) != RWLR_OK)
                {
                    DBG_INFO(("_dm_cb_msg_thread lock error."));
                    goto fail;
                }

                if (x_rwl_write_lock(t_dm_core.h_dev_rwlock, RWL_OPTION_WAIT) != RWLR_OK)
                {
                    DBG_INFO(("_dm_cb_msg_thread lock error."));
                    goto fail;
                }

                
                  //Scan the list to check if the device is already detected.
                 

                SLIST_FOREACH(pt_dev_desc, &(t_dm_core.t_dev_list), t_link)
                {
                    if ((pt_dev_desc->pt_prnt_dev == pt_cb_dev) &&
                        (pt_dev_desc->ui4_skt_no == ui4_skt_no))
                    {
                        DM_ASSERT(pt_dev_desc->pt_root_dev == 
                                      pt_cb_dev->pt_root_dev);
                        DM_ASSERT(pt_dev_desc->t_hw_type == 
                                      pt_cb_dev->t_hw_type);
                        DM_ASSERT(pt_dev_desc->t_dev_type == t_dev_type);
                        DM_ASSERT(pt_dev_desc->ui4_unit_id == ui4_unit_id);
                        DM_ASSERT(pt_dev_desc->b_avail == FALSE);

                        b_found = TRUE;

                        break;
                    }
                    
                    pt_dev_desc_lst = pt_dev_desc;
                }

                if (b_found == FALSE)
                {
                    pt_dev_desc = (DM_DEV_DESC_T *)x_mem_alloc(sizeof(DM_DEV_DESC_T));

                    if (pt_dev_desc == NULL)
                    {
                        DBG_INFO(("_dm_cb_msg_thread mem alloc error."));
                        goto error_2;
                    }

                    memset((VOID *) pt_dev_desc, 0, sizeof(DM_DEV_DESC_T));

                    i4_ret = handle_alloc((HANDLE_TYPE_T)DMT_DEV_HANDLE,
                                          pt_dev_desc,
                                          NULL,
                                          _dm_handle_free_fct,
                                          &(pt_dev_desc->h_dev));

                    if (i4_ret != HR_OK)
                    {
                        DBG_INFO(("_dm_cb_msg_thread alloc handle error."));
                        goto error_2;
                    }

                    pt_dev_desc->ui4_dev_flag = ui4_dev_flag;
                    pt_dev_desc->h_drv = NULL_HANDLE;
                    pt_dev_desc->pt_prnt_dev = pt_cb_dev;
                    pt_dev_desc->pt_root_dev = pt_cb_dev->pt_root_dev;
                    pt_dev_desc->t_dev_type = t_dev_type;
                    pt_dev_desc->t_hw_type = pt_cb_dev->t_hw_type;
                    pt_dev_desc->ui4_skt_no = ui4_skt_no;
                    pt_dev_desc->ui4_unit_id = ui4_unit_id;
                    pt_dev_desc->pv_data = pv_data;
                    pt_dev_desc->ui4_data_size = ui4_data_size;
                    pt_dev_desc->b_avail = FALSE;
                    pt_dev_desc->h_dev_root_lbl = NULL_HANDLE;
                    memset(pt_dev_desc->ps_dev_name, 
                             0, 
                             DM_MAX_LABEL_NAME_LEN);

                    if (pt_dev_desc_lst == NULL)
                    {
                        SLIST_INSERT_HEAD(pt_dev_desc, &(t_dm_core.t_dev_list), t_link);
                    }
                    else
                    {
                        SLIST_INSERT_AFTER(pt_dev_desc, pt_dev_desc_lst, t_link);
                    }
                }

                x_rwl_release_lock(t_dm_core.h_dev_rwlock);

                if (b_found == TRUE)
                {
                    continue;
                }

                

                x_rwl_release_lock(t_dm_core.h_nfy_rwlock);

                continue;

            error_2:

                if (pt_dev_desc != NULL)
                {
                    if (pt_dev_desc->h_dev != NULL_HANDLE)
                    {
                        handle_free(pt_dev_desc->h_dev, FALSE);
                    }

                    if (pt_dev_desc->pv_data != NULL)
                    {
                        x_mem_free(pt_dev_desc->pv_data);
                    }

                    x_mem_free(pt_dev_desc);
                }

                x_rwl_release_lock(t_dm_core.h_dev_rwlock);
                x_rwl_release_lock(t_dm_core.h_nfy_rwlock);

                goto fail;
            } */
        }
    }

fail:

    exit(1);

}
#ifdef CONFIG_USBHOST
static int _dm_init_hw(DRV_TYPE_T t_root_dev_type, 
    unsigned int     ui4_unit_id)
{
	DM_DEV_DESC_T       *pt_dev_desc = NULL;
	MU_FS_DEVICE_TYPE  *pHfiDriver;
	pHfiDriver = MUSB_HfiDriverGet();
	int i;
	cyg_mutex_lock(&t_dm_core.h_dev_rwlock) ;
	pt_dev_desc = (DM_DEV_DESC_T *)malloc(sizeof(struct _DM_DEV_DESC_T));
	if (pt_dev_desc == NULL)
    {
        diag_printf(("_dm_init_hw_comp alloc mem error."));
        //i4_ret = DMR_OS;
        goto fail;
    }
	memset((VOID *) pt_dev_desc, 0, sizeof(DM_DEV_DESC_T));
	pt_dev_desc->ui4_dev_flag = 1;
    pt_dev_desc->pt_prnt_dev = NULL;
    pt_dev_desc->pt_root_dev = pt_dev_desc;
    pt_dev_desc->t_dev_type = DM_DEV_CHR_MEDIUM;
    pt_dev_desc->t_hw_type = _dm_get_hw_type(t_root_dev_type);
    pt_dev_desc->ui4_skt_no = 0;
    pt_dev_desc->ui4_unit_id = ui4_unit_id;
    pt_dev_desc->pv_data = NULL;
    pt_dev_desc->ui4_data_size = 0;
    memset((VOID *)pt_dev_desc->ps_dev_name, 0, DM_MAX_LABEL_NAME_LEN);
    //pt_dev_desc->h_drv = h_drv;
    pt_dev_desc->b_avail = true;

	//set callback 
	t_dev_st_nfy.u4DevType=DM_DEV_CHR_MEDIUM;
	t_dev_st_nfy.pfNfy = _dm_dev_nfy_fct;
    t_dev_st_nfy.pvTag = pt_dev_desc;
	for (i=0; i<MUSB_UCFS_UNIT_COUNT; i++)
    {               
        if (0 > pHfiDriver->pfIoCtl(i, 
            (unsigned int)MUSB_IOCTL_SET_SOCKET_DEV_NFY, 0, (void *)&t_dev_st_nfy))
        {
            return -1;
        }       
    }

	SLIST_INSERT_HEAD(&(t_dm_core.t_dev_list),pt_dev_desc,  t_link);
    cyg_mutex_unlock(&t_dm_core.h_dev_rwlock);
	/*/     est case
	DM_CB_MSG_T t_cb_msg;
	t_cb_msg.pv_tag = pt_dev_desc;
    t_cb_msg.t_cb_type = _dm_get_cb_type(HFI_STATUS_READY);
    t_cb_msg.ui4_skt_no = 0;
    t_cb_msg.t_dev_type = DM_DEV_CHR_MEDIUM;
	t_cb_msg.ui4_unit_id = 0;
    /     _cb_msg.ui4_dev_flag = pt_sktdev_info->u4DevFlag;

    t_cb_msg.pv_data = NULL;
    t_cb_msg.ui4_data_sz = 0;
	 cyg_mbox_put(t_dm_core.h_cb_msg_q,(void *) &t_cb_msg);*/
	return 0;

fail:
    cyg_mutex_unlock(&t_dm_core.h_dev_rwlock);

    return -1;
}
#endif
void usb_fs_dm_init()
{
	int i4_ret;
	
		if (t_dm_core.b_init == true)
		{
			return;
		}
		
		cyg_mbox_create(&t_dm_core.h_cb_msg_q, 
								&t_dm_core.h_cb_msg_mbox);
	
		cyg_mutex_init(&t_dm_core.h_dev_rwlock);	
	
		SLIST_INIT(&(t_dm_core.t_dev_list));

		cyg_thread_create(DM_CB_MSG_THREAD_DEFAULT_PRIORITY,
									_dm_cb_msg_thread,
									(CYG_ADDRWORD)0,
									"cb_msg_thread",
									(void*)cb_msg_thread_stack,
									CB_MSG_STACK_SIZE,
									&t_dm_core.h_cb_msg_thrd,
									&thread_obj);
		cyg_thread_resume(t_dm_core.h_cb_msg_thrd);
#ifdef CONFIG_USBHOST
		i4_ret = _dm_init_hw(DRVT_HW_USB, 0);
#endif       
        mount("root","/","rootfs");
        

		if (i4_ret != 0)
		{
			diag_printf(("_dm_init init hw error."));
			goto fail;
		}
		  
		t_dm_core.b_init = true;
		
	
		return;
	
	fail:
	
		/*if (t_dm_core.h_nfy_rwlock != NULL_HANDLE)
		{
			x_rwl_delete_lock(t_dm_core.h_nfy_rwlock);
		}*/
	
		/*if (t_dm_core.h_dev_rwlock != NULL_HANDLE)
		{
			cyg_mutex_destroy(&t_dm_core.h_dev_rwlock);
		}*/
	
	
		return;
	
}

void test()
{
	//int ret = -1;
    //DIR fdir;
    //struct dirent *buf;
    //struct stat buf1;
   // diag_printf("enter opendir\n");
    //stat("",&buf1);
	//fdir = opendir("");
    //while ((buf = readdir (fdir)) != NULL)
    //diag_printf("name = %s\n",buf->d_name);
    //closedir(fdir);
    //int fd = creat("/DISK0_C/test.txt",0777);
    /*stat("/DISK0_C/",&buf1);
    fdir = opendir("/DISK0_C/");
    if ((buf = readdir (fdir)) != NULL)
    diag_printf("name = %s\n",buf->d_name);
    closedir(fdir);*/
    
	//FILE *fp=fopen("/DISK0_C/test.txt","r");
	//int fd=open("BOOTEX.LOG",O_RDWR);
    //diag_printf("fd=%d\n",fd);
	//char buf[512];
	//char *buf="blablablablablablablablablablablablablablablablablablablabla";
	//ret = fread(buf,1,512,fp);
	//write(fd,buf,60);
    //close(fd);
    //fclose(fp);
	//ret=read(fd,buf,512);
	//write(fd,buf,60);
    //fsync(fd);
	//diag_printf("ret=%d\n",ret);
	//diag_dump_buf_with_offset(buf,512,buf);
}
