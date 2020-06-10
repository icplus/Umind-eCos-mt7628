//==========================================================================
//
//      ide_disk.c
//
//      IDE polled mode disk driver 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2006 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    iz
// Contributors: 
// Date:         2004-10-16
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/drv_api.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>
#include <stdio.h>

#include "usb_disk.h"
#ifdef CONFIG_USBHOST
#include "../../drivers/usbhost/include/mu_hfi_if.h"
#include "../../drivers/usbhost/include/musb_if.h"
#endif



// ----------------------------------------------------------------------------

#ifdef CYGDBG_IO_DISK_DEBUG
# define DEBUG 1
#endif

#ifdef DEBUG
# define D(fmt,args...) diag_printf(fmt, ## args)
#else
# define D(fmt,args...)
#endif

// ----------------------------------------------------------------------------

// No h/w controller structure is needed in this driver, but the address of the
// second argument is taken anyway.

DISK_CONTROLLER(usb_disk_controller, usb_disk_controller);

#ifdef CYGVAR_DEVS_DISK_IDE_DISK0
//USB_DISK_INSTANCE(0, 0, 0, true, CYGDAT_IO_DISK_IDE_DISK0_NAME);
#endif


#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define VAR_NAME(number) usb_disk_io_##number
#ifdef CONFIG_USBHOST
static MBlockFunc_T MUSBBlkFuncTbl;
#endif
// ----------------------------------------------------------------------------
extern struct cyg_devtab_entry* dev_entry_creat(cyg_uint32 number)
{
	char *devname = NULL;
	char *pindex;
	devname = (char *)malloc(20);
	memset(devname,0,20);
	if(NULL == devname){diag_printf("dev_entry_creat:devname malloc failed\n");goto err0;}
	sprintf(devname,"/DISK%d/",number);
	pindex=devname+5;
	//diag_printf("index=%d\n",*pindex)
	/*struct cyg_devtab_entry* usb_disk_channel_part_dev=(struct cyg_devtab_entry*)malloc(4*sizeof(struct cyg_devtab_entry));
	disk_channel *usb_disk_channel_part_chan=(disk_channel *)malloc(4*sizeof(struct disk_channel));
	cyg_disk_partition_t *usb_disk_channel_part_tab=(cyg_disk_partition_t*)malloc(4*sizeof(struct cyg_disk_partition_t));*/
	struct cyg_devtab_entry* usb_disk_channel_part_dev=(struct cyg_devtab_entry*)malloc(sizeof(struct cyg_devtab_entry));
	if(NULL == usb_disk_channel_part_dev){diag_printf("dev_entry_creat:usb_disk_channel_part_dev malloc failed\n");goto err1;}
	
	disk_channel *usb_disk_channel_part_chan=(disk_channel *)malloc(sizeof(struct disk_channel));
	if(NULL == usb_disk_channel_part_chan){diag_printf("dev_entry_creat:usb_disk_channel_part_chan malloc failed\n");goto err2;}
	
	cyg_disk_partition_t *usb_disk_channel_part_tab=(cyg_disk_partition_t*)malloc(4*sizeof(struct cyg_disk_partition_t));
	if(NULL == usb_disk_channel_part_tab){diag_printf("dev_entry_creat:usb_disk_channel_part_tab malloc failed\n");goto err3;}
	
	cyg_disk_info_t *usb_disk_channel_disk_info=(cyg_disk_info_t*)malloc(sizeof(struct cyg_disk_info_t));
	if(NULL == usb_disk_channel_disk_info){diag_printf("dev_entry_creat:usb_disk_channel_disk_info malloc failed\n");goto err4;}
	
	disk_channel *usb_disk_channel=(disk_channel *)malloc(sizeof(struct disk_channel));
	if(NULL == usb_disk_channel){diag_printf("dev_entry_creat:usb_disk_channel malloc failed\n");goto err5;}
	
	/*memset((VOID *) usb_disk_channel_part_dev, 0, 4*sizeof(struct cyg_devtab_entry));
	memset((VOID *) usb_disk_channel_part_chan, 0, 4*sizeof(struct disk_channel));
	memset((VOID *) usb_disk_channel_part_tab, 0, 4*sizeof(struct cyg_disk_partition_t));*/
	memset((VOID *) usb_disk_channel_part_dev, 0, sizeof(struct cyg_devtab_entry));
	memset((VOID *) usb_disk_channel_part_chan, 0, sizeof(struct disk_channel));
	memset((VOID *) usb_disk_channel_part_tab, 0, 4*sizeof(struct cyg_disk_partition_t));
	memset((VOID *) usb_disk_channel_disk_info, 0, sizeof(cyg_disk_info_t));
	memset((VOID *) usb_disk_channel, 0, sizeof(disk_channel));

    //init disk_channel
	usb_disk_channel_disk_info->partitions=usb_disk_channel_part_tab;
	usb_disk_channel_disk_info->partitions_num=4;
	usb_disk_channel->funs=&usb_disk_funs;
	usb_disk_channel->callbacks=&cyg_io_disk_callbacks;
	usb_disk_channel->dev_priv=pindex;
	usb_disk_channel->controller=&usb_disk_controller;
	usb_disk_channel->info=usb_disk_channel_disk_info;
	usb_disk_channel->partition=NULL;
	usb_disk_channel->pdevs_dev=usb_disk_channel_part_dev;
	usb_disk_channel->pdevs_chan=usb_disk_channel_part_chan;
	usb_disk_channel->mbr_support=true;
	usb_disk_channel->valid=false;
	usb_disk_channel->init=false;
	usb_disk_channel->mounts=0;
    //init devtab
	struct cyg_devtab_entry *VAR_NAME(number)=(struct cyg_devtab_entry*)malloc(sizeof(struct cyg_devtab_entry));
	if(NULL == VAR_NAME(number)){diag_printf("dev_entry_creat:devtab malloc failed\n");goto err6;}
	memset((VOID *) VAR_NAME(number), 0, sizeof(struct cyg_devtab_entry));
	VAR_NAME(number)->name=devname;
	VAR_NAME(number)->dep_name=NULL;
	VAR_NAME(number)->handlers=&cyg_io_disk_devio;
	VAR_NAME(number)->init=usb_disk_init;
	VAR_NAME(number)->lookup=usb_disk_lookup;
	VAR_NAME(number)->priv=usb_disk_channel;
	VAR_NAME(number)->status=CYG_DEVTAB_STATUS_BLOCK;
	if(0 == VAR_NAME(number)->init(VAR_NAME(number)))
	{
			diag_printf("dev init fail\n");
            goto err6;
	}
	return VAR_NAME(number);
err6:
	free(VAR_NAME(number));
err5:
	free(usb_disk_channel);
err4:
	free(usb_disk_channel_disk_info);
err3:
	free(usb_disk_channel_part_tab);
err2:
	free(usb_disk_channel_part_chan);
err1:
	free(usb_disk_channel_part_dev);
err0:
	free(devname);return NULL;
	
}


static cyg_bool 
usb_disk_init(struct cyg_devtab_entry *tab)
{
	//diag_printf("usb_disk_init\n");
	disk_channel *chan = (disk_channel *) tab->priv;
    cyg_disk_identify_t ident;
	cyg_uint64 sectors_num;
	int index = *(char *)chan->dev_priv;
    
	
    if (chan->init) 
        return true;
#ifdef CONFIG_USBHOST
    MBlockFunc_T *pMUSBBlkFuncTbl = &MUSBBlkFuncTbl;
    MUSB_GetBlkFuncTbl(pMUSBBlkFuncTbl);
	pMUSBBlkFuncTbl->pfnSetLun(index-48);
    sectors_num = pMUSBBlkFuncTbl->pfnGetSize();
    ident.max_transfer = 16*1024;
    //ident.phys_block_size = pMUSBBlkFuncTbl->pfnGetBlockSize();
#else
    sectors_num = 0;
    ident.max_transfer = 32;
#endif
    //ident.cylinders_num  = ide_idData->num_cylinders;
    ident.heads_num = 255;
    ident.sectors_num = 63;
    
	//diag_printf("sectors_num=%d\n",sectors_num);
    
	//test case
	//sectors_num = 1048575;
	
	ident.lba_sectors_num = sectors_num;
	ident.phys_block_size = pMUSBBlkFuncTbl->pfnGetBlockSize();;

    if (!(chan->callbacks->disk_init)(tab))
        return false;

    if (ENOERR != (chan->callbacks->disk_connected)(tab, &ident))
        return false;

    return true;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo 
usb_disk_lookup(struct cyg_devtab_entry **tab, 
                struct cyg_devtab_entry  *sub_tab,
                const char *name)
{
	//diag_printf("usb_disk_lookup\n");
    disk_channel *chan = (disk_channel *) (*tab)->priv;
    return (chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

// ----------------------------------------------------------------------------
//len:block length; block_num:start block;
#if 0
const char databuf[512]={0xeb,0x58,0x90,0x4d,0x53,0x44,0x4f,0x53,0x35,0x2e,0x30,0x00,0x02,0x08,0x20,0x00,
						0x02,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0x3f,0x00,0xff,0x00,0x3f,0x00,0x00,0x00,
						0x01,0x4b,0x77,0x00,0xc4,0x1d,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
						0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x01,0x29,0x99,0xb5,0x38,0x34,0x4e,0x4f,0x20,0x4e,0x41,0x4d,0x45,0x20,0x20,
						0x20,0x20,0x46,0x41,0x54,0x33,0x32,0x20,0x20,0x20,0x33,0xc9,0x8e,0xd1,0xbc,0xf4,
						0x7b,0x8e,0xc1,0x8e,0xd9,0xbd,0x00,0x7c,0x88,0x4e,0x02,0x8a,0x56,0x40,0xb4,0x08,
						0xcd,0x13,0x73,0x05,0xb9,0xff,0xff,0x8a,0xf1,0x66,0x0f,0xb6,0xc6,0x40,0x66,0x0f,
						0xb6,0xd1,0x80,0xe2,0x3f,0xf7,0xe2,0x86,0xcd,0xc0,0xed,0x06,0x41,0x66,0x0f,0xb7,
						0xc9,0x66,0xf7,0xe1,0x66,0x89,0x46,0xf8,0x83,0x7e,0x16,0x00,0x75,0x38,0x83,0x7e,
						0x2a,0x00,0x77,0x32,0x66,0x8b,0x46,0x1c,0x66,0x83,0xc0,0x0c,0xbb,0x00,0x80,0xb9,
						0x01,0x00,0xe8,0x2b,0x00,0xe9,0x48,0x03,0xa0,0xfa,0x7d,0xb4,0x7d,0x8b,0xf0,0xac,
						0x84,0xc0,0x74,0x17,0x3c,0xff,0x74,0x09,0xb4,0x0e,0xbb,0x07,0x00,0xcd,0x10,0xeb,
						0xee,0xa0,0xfb,0x7d,0xeb,0xe5,0xa0,0xf9,0x7d,0xeb,0xe0,0x98,0xcd,0x16,0xcd,0x19,
						0x66,0x60,0x66,0x3b,0x46,0xf8,0x0f,0x82,0x4a,0x00,0x66,0x6a,0x00,0x66,0x50,0x06,
						0x53,0x66,0x68,0x10,0x00,0x01,0x00,0x80,0x7e,0x02,0x00,0x0f,0x85,0x20,0x00,0xb4,
						0x41,0xbb,0xaa,0x55,0x8a,0x56,0x40,0xcd,0x13,0x0f,0x82,0x1c,0x00,0x81,0xfb,0x55,
						0xaa,0x0f,0x85,0x14,0x00,0xf6,0xc1,0x01,0x0f,0x84,0x0d,0x00,0xfe,0x46,0x02,0xb4,
						0x42,0x8a,0x56,0x40,0x8b,0xf4,0xcd,0x13,0xb0,0xf9,0x66,0x58,0x66,0x58,0x66,0x58,
						0x66,0x58,0xeb,0x2a,0x66,0x33,0xd2,0x66,0x0f,0xb7,0x4e,0x18,0x66,0xf7,0xf1,0xfe,
						0xc2,0x8a,0xca,0x66,0x8b,0xd0,0x66,0xc1,0xea,0x10,0xf7,0x76,0x1a,0x86,0xd6,0x8a,
						0x56,0x40,0x8a,0xe8,0xc0,0xe4,0x06,0x0a,0xcc,0xb8,0x01,0x02,0xcd,0x13,0x66,0x61,
						0x0f,0x82,0x54,0xff,0x81,0xc3,0x00,0x02,0x66,0x40,0x49,0x0f,0x85,0x71,0xff,0xc3,
						0x4e,0x54,0x4c,0x44,0x52,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d,0x0a,0x52,0x65,
						0x6d,0x6f,0x76,0x65,0x20,0x64,0x69,0x73,0x6b,0x73,0x20,0x6f,0x72,0x20,0x6f,0x74,
						0x68,0x65,0x72,0x20,0x6d,0x65,0x64,0x69,0x61,0x2e,0xff,0x0d,0x0a,0x44,0x69,0x73,
						0x6b,0x20,0x65,0x72,0x72,0x6f,0x72,0xff,0x0d,0x0a,0x50,0x72,0x65,0x73,0x73,0x20,
						0x61,0x6e,0x79,0x20,0x6b,0x65,0x79,0x20,0x74,0x6f,0x20,0x72,0x65,0x73,0x74,0x61,
						0x72,0x74,0x0d,0x0a,0x00,0x00,0x00,0x00,0x00,0xac,0xcb,0xd8,0x00,0x00,0x55,0xaa};
#endif
static Cyg_ErrNo usb_disk_read(disk_channel *chan, 
                              void         *buf,
                              cyg_uint32    len, 
                              cyg_uint64    block_num)
{
#ifdef CONFIG_USBHOST	
	char index=*(char *)chan->dev_priv-48;
	cyg_disk_info_t *info = chan->info;
	MBlockFunc_T *pMUSBBlkFuncTbl = &MUSBBlkFuncTbl;
	MUSB_GetBlkFuncTbl(pMUSBBlkFuncTbl);
	pMUSBBlkFuncTbl->pfnSetLun(index);
	//memcpy(buf,databuf,512);
   	//diag_printf("usb_disk_read  block_num =%lld block_size =%d,len = %d,index = %d\n",block_num,info->block_size,len*info->block_size,index);

    //if(0 > fgUsbIoRead(block_num*info->block_size,(cyg_uint32)buf,len*512)) 
    //memset(buf,0xff,len*info->block_size);
    if(0 > pMUSBBlkFuncTbl->pfnRead(block_num,(cyg_uint32)buf,len*info->block_size))
	{
        diag_printf("read fail\n");
        return -EIO; 
    }
    //diag_dump_buf_with_offset( buf, len*info->block_size, buf );
#endif
    return ENOERR;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo 
usb_disk_write(disk_channel *chan, 
               const void   *buf,
               cyg_uint32    len, 
               cyg_uint64    block_num)
{
#ifdef CONFIG_USBHOST
    //diag_printf("usb_disk_write buf=%p len=%d block_num=%d\n",buf,len,block_num);
    
	char index=*(char *)chan->dev_priv-48;
	cyg_disk_info_t *info = chan->info;
	MBlockFunc_T *pMUSBBlkFuncTbl = &MUSBBlkFuncTbl;
	MUSB_GetBlkFuncTbl(pMUSBBlkFuncTbl);
	pMUSBBlkFuncTbl->pfnSetLun(index);
    /*diag_printf("raw data\n");
    diag_dump_buf_with_offset( buf, len*info->block_size, buf );
    diag_printf("\n");*/
	if(0 > pMUSBBlkFuncTbl->pfnWrite(block_num,buf,len*info->block_size)) 
	{
		diag_printf("write fail\n");	
		return -EIO; 
	}
    /*diag_printf("after write\n");
    diag_dump_buf_with_offset( buf, len*info->block_size, buf );
    diag_printf("\n");*/
#endif
    return ENOERR;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo
usb_disk_get_config(disk_channel *chan, 
                    cyg_uint32    key,
                    const void   *xbuf, 
                    cyg_uint32   *len)
{
    return -EINVAL;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo
usb_disk_set_config(disk_channel *chan, 
                    cyg_uint32    key,
                    const void   *xbuf, 
                    cyg_uint32   *len)
{
    return -EINVAL;
}

//EOF ide_disk.c
