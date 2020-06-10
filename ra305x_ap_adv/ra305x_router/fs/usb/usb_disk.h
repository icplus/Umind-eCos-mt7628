#ifndef CYGONCE_IDE_DISK_H
#define CYGONCE_IDE_DISK_H
//==========================================================================
//
//      usb_disk.h
//
//      IDE polled mode disk driver  defines
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
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


// ----------------------------------------------------------------------------
extern struct cyg_devtab_entry* dev_entry_creat(cyg_uint32 number);

static cyg_bool usb_disk_init(struct cyg_devtab_entry *tab);

static Cyg_ErrNo usb_disk_read(disk_channel *chan, 
                              void         *buf,
                              cyg_uint32    len, 
                              cyg_uint64    block_num); 
        

static Cyg_ErrNo usb_disk_write(disk_channel *chan, 
                               const void   *buf,
                               cyg_uint32    len, 
                               cyg_uint64    block_num); 
 
static Cyg_ErrNo usb_disk_get_config(disk_channel *chan, 
                                    cyg_uint32    key,
                                    const void   *xbuf, 
                                    cyg_uint32   *len);

static Cyg_ErrNo usb_disk_set_config(disk_channel *chan, 
                                    cyg_uint32    key,
                                    const void   *xbuf, 
                                    cyg_uint32   *len);

static Cyg_ErrNo usb_disk_lookup(struct cyg_devtab_entry **tab,
                                struct cyg_devtab_entry  *sub_tab,
                                const char               *name);

/*DISK_FUNS(usb_disk_funs, 
          usb_disk_read, 
          usb_disk_write, 
          usb_disk_get_config,
          usb_disk_set_config
);*/
static disk_funs usb_disk_funs = {                                              \
  usb_disk_read,                                                             \
  usb_disk_write,                                                            \
  usb_disk_get_config,                                                       \
  usb_disk_set_config                                                        \
};

// ----------------------------------------------------------------------------

#define USB_DISK_INSTANCE(_number_,_mbr_supp_,_name_)     \                                                                     \
DISK_CHANNEL(usb_disk_channel##_number_,                                \
             usb_disk_funs,                                             \
             NULL,                                   \
             usb_disk_controller,                                       \
             _mbr_supp_,                                                \
             4                                                          \
);                                                                      \
cyg_devtab_entry_t usb_disk_io##_number_ = {                   \
   _name_,                                                               \
   0,                                                           \
   &cyg_io_disk_devio,                                                           \
   usb_disk_init,                                                               \
   usb_disk_lookup,                                                             \
   &usb_disk_channel##_number_ ,                                                               \
   CYG_DEVTAB_STATUS_BLOCK                                              \
};


// ----------------------------------------------------------------------------

#endif // CYGONCE_IDE_DISK_H
