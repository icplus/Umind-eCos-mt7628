//==========================================================================
//
//      fatfs_supp.c
//
//      FAT file system support functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
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
// Author(s):           Savin Zlobec <savin@elatec.si> 
// Date:                2003-06-30
//
//####DESCRIPTIONEND####
//
//==========================================================================

//#include <pkgconf/fs_fat.h>
#include <pkgconf/infra.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>
#include "include/fatfs.h"
#include <blib/blib.h>

#include <sys/types.h>
#include <ctype.h>

#include "fatfs.h"

//==========================================================================
// FAT defines & macros

// -------------------------------------------------------------------------
// FAT dir entry attributes macros

#define DENTRY_IS_RDONLY(_dentry_)  (S_FATFS_ISRDONLY((_dentry_)->attr))
#define DENTRY_IS_HIDDEN(_dentry_)  (S_FATFS_ISHIDDEN((_dentry_)->attr))
#define DENTRY_IS_SYSTEM(_dentry_)  (S_FATFS_ISSYSTEM((_dentry_)->attr))
#define DENTRY_IS_VOLUME(_dentry_)  (S_FATFS_ISVOLUME((_dentry_)->attr))
#define DENTRY_IS_DIR(_dentry_)     (S_FATFS_ISDIR((_dentry_)->attr))
#define DENTRY_IS_ARCHIVE(_dentry_) (S_FATFS_ISARCHIVE((_dentry_)->attr))

#define DENTRY_IS_DELETED(_dentry_) \
    (0xE5 == (cyg_uint8)((_dentry_)->name[0]))

#define DENTRY_IS_ZERO(_dentry_) \
    (0x00 == (cyg_uint8)((_dentry_)->name[0]))

// -------------------------------------------------------------------------
// FAT disk data access macros

// FIXME: support big endian machines!
   
#define GET_BYTE(_data_, _var_, _off_) \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_) ) )

#define GET_WORD(_data_, _var_, _off_)                      \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_) ) |         \
             *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) << 8)

#define GET_DWORD(_data_, _var_, _off_)                         \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_))             |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) << 8   |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 2 ) << 16  |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 3 ) << 24)

#define GET_BYTES(_data_, _var_, _size_, _off_) \
    memcpy((void *)(_var_), (void*)(((cyg_uint8 *)_data_)+(_off_)),_size_)

#define SET_BYTE(_data_, _val_, _off_) \
    (*( ((cyg_uint8 *)_data_) + (_off_) ) = _val_)

#define SET_WORD(_data_, _val_, _off_)                                   \
    do {                                                                 \
        *( ((cyg_uint8 *)_data_) + (_off_) )     = _val_         & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) = (_val_ >> 8)  & 0xFF; \
    } while (0)

#define SET_DWORD(_data_, _val_, _off_)                                  \
    do {                                                                 \
        *( ((cyg_uint8 *)_data_) + (_off_) )     = _val_         & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) = (_val_ >> 8)  & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 2 ) = (_val_ >> 16) & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 3 ) = (_val_ >> 24) & 0xFF; \
    } while (0)

#define SET_BYTES(_data_, _var_, _size_, _off_) \
    memcpy((void *)(((cyg_uint8 *)_data_)+(_off_)), (void *)(_var_), _size_)

#define FAT_OFFSET(bs, off)         \
        (((cyg_uint8 *)(bs)) + (off))
#define FAT_GET_VAL8(bs, off)       \
        (*(cyg_uint8 *)FAT_OFFSET(bs, off))
#define FAT_GET_VAL16(bs, off)      \
    ((cyg_uint16)(*FAT_OFFSET(bs, off)) +              \
    (((cyg_uint16)(*FAT_OFFSET(bs, off + 1))) << 8))
    
#define FAT_GET_VAL32(bs, off)      \
    ((cyg_uint32)(*FAT_OFFSET(bs, off)) +              \
    (((cyg_uint32)(*FAT_OFFSET(bs, off + 1))) << 8) +  \
    (((cyg_uint32)(*FAT_OFFSET(bs, off + 2))) << 16) + \
    (((cyg_uint32)(*FAT_OFFSET(bs, off + 3))) << 24))


#define FAT_SET_VAL8(bs, off, val)  \
        (*(cyg_uint8 *)FAT_OFFSET(bs, off) = val)
#define FAT_SET_VAL16(bs, off, val) \
        (*FAT_OFFSET(bs, off)     = (cyg_uint8)((cyg_uint16)(val) & 0xff));           \
        (*FAT_OFFSET(bs, off + 1) = (cyg_uint8)(((cyg_uint16)(val) >> 8) & 0xff))
        
#define FAT_SET_VAL32(bs, off, val) \
        (*FAT_OFFSET(bs, off)     = (cyg_uint8)((cyg_uint32)(val) & 0xff));           \
        (*FAT_OFFSET(bs, off + 1) = (cyg_uint8)(((cyg_uint32)(val) >> 8) & 0xff));    \
        (*FAT_OFFSET(bs, off + 2) = (cyg_uint8)(((cyg_uint32)(val) >> 16) & 0xff));   \
        (*FAT_OFFSET(bs, off + 3) = (cyg_uint8)(((cyg_uint32)(val) >> 24) & 0xff))
// -------------------------------------------------------------------------
// FAT table entries types 

#define TENTRY_REGULAR  0 // Used when entry points to next file cluster 
#define TENTRY_FREE     1 // Free cluster
#define TENTRY_LAST     2 // Last cluster of file 
#define TENTRY_RESERVED 3 // Reserved cluster
#define TENTRY_BAD      4 // Bad cluster 

// -------------------------------------------------------------------------
// FAT table structures size 

#define DENTRY_SIZE 0x20 // Dir entry size

// -------------------------------------------------------------------------
// Time & date defines 

#define JD_1_JAN_1970 2440588 // 1 Jan 1970 in Julian day number

/*
 *  FAT directory entry field offset
 */
#define FAT_DENTRY_NAME      (0)
#define FAT_DENTRY_ATTR      (FAT_DENTRY_NAME + 11)
#define FAT_DENTRY_NTRES     (FAT_DENTRY_ATTR + 1)
#define FAT_DENTRY_CRT_TIME_TENTH   (FAT_DENTRY_NTRES + 1)
#define FAT_DENTRY_CRT_TIME  (FAT_DENTRY_CRT_TIME_TENTH + 1)
#define FAT_DENTRY_CRT_DATE  (FAT_DENTRY_CRT_TIME + 2)
#define FAT_DENTRY_ACC_DATE  (FAT_DENTRY_CRT_DATE + 2)
#define FAT_DENTRY_CLUS_HI   (FAT_DENTRY_ACC_DATE + 2)
#define FAT_DENTRY_WRT_TIME  (FAT_DENTRY_CLUS_HI + 2)
#define FAT_DENTRY_WRT_DATE  (FAT_DENTRY_WRT_TIME + 2)
#define FAT_DENTRY_CLUS_LO   (FAT_DENTRY_WRT_DATE + 2)
#define FAT_DENTRY_FILE_SIZE (FAT_DENTRY_CLUS_LO + 2)

#define FAT_LDENTRY_ORDER    (0)
#define FAT_LDENTRY_NAME1    (FAT_LDENTRY_ORDER + 1)
#define FAT_LDENTRY_ATTR     (FAT_LDENTRY_NAME1 + 10)
#define FAT_LDENTRY_TYPE     (FAT_LDENTRY_ATTR + 1)
#define FAT_LDENTRY_CHKSUM   (FAT_LDENTRY_TYPE + 1)
#define FAT_LDENTRY_NAME2    (FAT_LDENTRY_CHKSUM + 1)
#define FAT_LDENTRY_FST_CLUS_LO (FAT_LDENTRY_NAME2 + 12)
#define FAT_LDENTRY_NAME3    (FAT_LDENTRY_FST_CLUS_LO + 2)

#define FAT_ATTR_READ_ONLY  (0x01)
#define FAT_ATTR_HIDDEN     (0x02)
#define FAT_ATTR_SYSTEM     (0x04)
#define FAT_ATTR_VOLUME_ID  (0x08)
#define FAT_ATTR_DIRECTORY  (0x10)
#define FAT_ATTR_ARCHIVE    (0x20)
#define FAT_ATTR_LONG_NAME  (FAT_ATTR_READ_ONLY | \
                             FAT_ATTR_HIDDEN | \
                             FAT_ATTR_SYSTEM | \
                             FAT_ATTR_VOLUME_ID)
#define FAT_ATTR_LONG_NAME_MASK (FAT_ATTR_READ_ONLY | \
                                 FAT_ATTR_HIDDEN | \
                                 FAT_ATTR_SYSTEM | \
                                 FAT_ATTR_VOLUME_ID | \
                                 FAT_ATTR_DIRECTORY | \
                                 FAT_ATTR_ARCHIVE)


// -------------------------------------------------------------------------
// Code tracing defines 

#ifdef FATFS_TRACE_DIR_ENTRY
# define TDE 1
#else
# define TDE 0
#endif

#ifdef FATFS_TRACE_CLUSTER
# define TCL 1
#else
# define TCL 0
#endif

#ifdef FATFS_TRACE_DATA
# define TDO 1
# define TR(_args_)   diag_printf _args_ 
#else
# define TDO 0   
# define TR(_args_)  
#endif

//==========================================================================
// FAT structures 

// -------------------------------------------------------------------------
// FAT table boot record structure 
   
typedef struct fat_boot_record_s
{
    cyg_uint16    jump;           // 00h : Jump code
//  cyg_uint8     jump0;          //                 + NOP
    char          oem_name[8+1];  // 03h : OEM name
    cyg_uint16    bytes_per_sec;  // 0Bh : cyg_bytes per sector
    cyg_uint8     sec_per_clu;    // 0Dh : Sectors per cluster
    cyg_uint16    res_sec_num;    // 0Eh : Number of reserved sectors
    cyg_uint8     fat_tbls_num;   // 10h : Number of copies of fat
    cyg_uint16    max_root_dents; // 11h : Maximum number of root dir entries
    cyg_uint16    sec_num_32;     // 13h : Number of sectors in partition < 32MB
    cyg_uint8     media_desc;     // 15h : Media descriptor
    cyg_uint16    sec_per_fat;    // 16h : Sectors per FAT
    cyg_uint16    sec_per_track;  // 18h : Sectors per track
    cyg_uint16    heads_num;      // 1Ah : Number of heads
    cyg_uint32    hsec_num;       // 1Ch : Number of hidden sectors
    cyg_uint32    sec_num;        // 20h : Number of sectors in partition
    cyg_uint8     exe_marker[2];  // 1FEh: Executable marker (55h AAh)

// FAT32 specific fields
 
    cyg_uint32    sec_per_fat_32; // 24h : Sectors per FAT
    cyg_uint16    ext_flags;      // 28h : Flags
    cyg_uint16    fs_ver;         // 2Ah : FS version
    cyg_uint32    root_cluster;   // 2Ch : Root dir cluster
    cyg_uint16    fs_info_sec;    // 30h : Sector number of FSINFO structure
    cyg_uint16    bk_boot_sec;    // 32h : Sector number of backup boot record
//  cyg_uint8     reserved[12];   // 34h : Reserved

// Fields with different locations on FAT12/16 and FAT32 
        
    cyg_uint8     drv_num;        // 24h (40h) : Drive number of partition 
//  cyg_uint8     reserved1;      // 25h (41h) : Reserved 1
    cyg_uint8     ext_sig;        // 26h (42h) : Extended signature
    cyg_uint32    ser_num;        // 27h (43h) : Serial number of partition
    char          vol_name[11+1]; // 2Bh (47h) : Volume name of partition
    char          fat_name[8+1];  // 36h (52h) : FAT name
   
} fat_boot_record_t;

// -------------------------------------------------------------------------
// FAT dir entry structure 
 
typedef struct fat_raw_dir_entry_s
{
    char       name[8+1];   // 00h : Name
    char       ext[3+1];    // 08h : Extension
    cyg_uint8  attr;        // 0Bh : Attribute
    cyg_uint8  nt_reserved; // 0Ch : Win NT Reserved field
    cyg_uint8  crt_sec_100; // 0Dh : Creation time ms stamp 0 - 199
    cyg_uint16 crt_time;    // 0Eh : Creation time
    cyg_uint16 crt_date;    // 10h : Creation date
    cyg_uint16 acc_date;    // 12h : Last access date
    cyg_uint16 cluster_HI;  // 14h : Starting cluster HI WORD (FAT32)
    cyg_uint16 wrt_time;    // 16h : Time    
    cyg_uint16 wrt_date;    // 18h : Date
    cyg_uint16 cluster;     // 1Ah : Starting cluster 
    cyg_uint32 size;        // 1Ch : Size of the file    

    //for long file name.added by zhiqiang
    cyg_uint8 s_utf8[4*(FM_MAX_FILE_LEN+1)];
    cyg_uint8 uil_use_dent;
    cyg_uint16 ui2_len;
    cyg_uint32 ui4_name_off;
} fat_raw_dir_entry_t;
cyg_uint8 sec_per_cluster = 8;
cyg_uint32 sector_size = 512;


// -------------------------------------------------------------------------
// FAT cluster opts 
 
typedef enum cluster_opts_e
{
    CO_NONE       = 0x00, // NULL option
    CO_EXTEND     = 0x01, // Extend cluster chain if one cluster too short
    CO_ERASE_NEW  = 0x02, // Erase newly allocated cluster
    CO_MARK_LAST  = 0x04  // Mark  newly allocated cluster as last
} cluster_opts_t;

//==========================================================================
// Utility functions 
typedef cyg_uint16 UTF16;
typedef cyg_uint8 UTF8;
/*
	UCS-2    UTF-8 ֽ()
	0000 - 007F  0xxxxxxx
	0080 - 07FF 110xxxxx 10xxxxxx
	0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
	*/
#define UTF8_ONE_START      (0xOOO1)
#define UTF8_ONE_END        (0x007F)
#define UTF8_TWO_START      (0x0080)
#define UTF8_TWO_END        (0x07FF)
#define UTF8_THREE_START    (0x0800)
#define UTF8_THREE_END      (0xFFFF)
	 
	 
void UTF16ToUTF8(UTF16* pUTF16Start, UTF16* pUTF16End, UTF8* pUTF8Start, UTF8* pUTF8End)
{
    UTF16* pTempUTF16 = pUTF16Start;
    UTF8* pTempUTF8 = pUTF8Start;
    while (pTempUTF16 < pUTF16End)
    {
        if (*pTempUTF16 <= UTF8_ONE_END
            && pTempUTF8 + 1 < pUTF8End)
        {
            //0000 - 007F  0xxxxxxx
            *pTempUTF8++ = (UTF8)*pTempUTF16;
        }
        else if(*pTempUTF16 >= UTF8_TWO_START && *pTempUTF16 <= UTF8_TWO_END
            && pTempUTF8 + 2 < pUTF8End)
        {
            //0080 - 07FF 110xxxxx 10xxxxxx
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if(*pTempUTF16 >= UTF8_THREE_START && *pTempUTF16 <= UTF8_THREE_END
            && pTempUTF8 + 3 < pUTF8End)
        {
            //0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
    *pTempUTF8 = 0;
}

UTF16 UTF8ToUTF16(UTF8* pUTF8Start, UTF8* pUTF8End, UTF16* pUTF16Start, UTF16* pUTF16End)
{
    UTF16* pTempUTF16 = pUTF16Start;
    UTF8* pTempUTF8 = pUTF8Start;
    UTF16 len =0;
 	    while (pTempUTF8 < pUTF8End && pTempUTF16+1 < pUTF16End)
    {
        if (*pTempUTF8 >= 0xE0 && *pTempUTF8 <= 0xEF)//3ֽڵĸʽ
        {
            //0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
            *pTempUTF16 |= ((*pTempUTF8++ & 0xEF) << 12);
            *pTempUTF16 |= ((*pTempUTF8++ & 0x3F) << 6);
            *pTempUTF16 |= (*pTempUTF8++ & 0x3F);
 
        }
        else if (*pTempUTF8 >= 0xC0 && *pTempUTF8 <= 0xDF)//2ֽڵĸʽ
        {
            //0080 - 07FF 110xxxxx 10xxxxxx
            *pTempUTF16 |= ((*pTempUTF8++ & 0x1F) << 6);
            *pTempUTF16 |= (*pTempUTF8++ & 0x3F);
        }
        else if(*pTempUTF8 >= 0 && *pTempUTF8 <= 0x7F)//1ֽڵĸʽ
        {
            //0000 - 007F  0xxxxxxx
            *pTempUTF16 = *pTempUTF8++;
        }
        else
        {
            break;
        }
        pTempUTF16++;
        len++;
    }
    *pTempUTF16 = 0;
    return len;
}



// -------------------------------------------------------------------------
// get_val_log2()
// Gets the log2 of given value or returns 0 if value is not a power of 2. 
 
static cyg_uint32 
get_val_log2(cyg_uint32 val)
{
    cyg_uint32 i, log2;
    
    i    = val;
    log2 = 0;
    
    while (0 == (i & 1))
    {
        i >>= 1;
        log2++;
    }

    if (i != 1) return 0;
    else        return log2;
}

// -------------------------------------------------------------------------
// cluster_to_block_pos()
// Converts cluster position to blib block position.

static void
cluster_to_block_pos(fatfs_disk_t *disk,
                     cyg_uint32    cluster,
                     cyg_uint32    cluster_pos,
                     cyg_uint32   *block, 
                     cyg_uint32   *block_pos)
{
    cyg_uint32 block_size_log2 = disk->sector_size_log2;   
    *block = (cluster - 2) << (disk->cluster_size_log2 - block_size_log2);
    *block += disk->fat_data_pos >> block_size_log2;
    *block_pos = cluster_pos;
}

// -------------------------------------------------------------------------
// disk_write()
// Writes data to disk.

static __inline__ int
disk_write(fatfs_disk_t *disk, 
           void         *buf,
           cyg_uint32   *len,
           cyg_uint32    pos)
{
    cyg_uint32  bnum = disk->fat_tbl_pos >> disk->sector_size_log2;
    pos -= disk->fat_tbl_pos;
    //if (pos >= disk->cluster_size)
    {
        cyg_uint32 temp = (pos >> (disk->sector_size_log2)) / sec_per_cluster;
        bnum += temp * sec_per_cluster;
        pos   = (pos & (disk->cluster_size- 1)); 
        //TR(("\ndisk write bnum=%d,pos=%d\n",bnum,pos));
    }
    return cyg_blib_write(&disk->blib, buf, len, bnum, pos);
}

// -------------------------------------------------------------------------
// disk_read()
// Reads data from disk.
static __inline__ int
disk_read(fatfs_disk_t *disk, 
          void         *buf,
          cyg_uint32   *len,
          cyg_uint32    pos)
{
    cyg_uint32  bnum = disk->fat_tbl_pos >> disk->sector_size_log2;
    pos -= disk->fat_tbl_pos;
    TR(("\n sector_size=%d sector_size=_log2=%d,disk->cluster_size=%d bnum=%d pos=%d\n",disk->sector_size,disk->sector_size_log2,disk->cluster_size,bnum,pos));
    //diag_printf("\n sector_size=%d sector_size=_log2=%d,disk->cluster_size=%d bnum=%d pos=%d\n",disk->sector_size,disk->sector_size_log2,disk->cluster_size,bnum,pos);
    //if ((pos % disk->cluster_size) == 0)
    {
        cyg_uint32 temp = (pos >> (disk->sector_size_log2)) / sec_per_cluster;
        bnum += temp * sec_per_cluster;
        pos   = (pos & (disk->cluster_size- 1));   
    }
    //pos   = (pos & (disk->cluster_size- 1));
    TR(("\ndisk read bnum=%d,pos=%d\n",bnum,pos));
    //diag_printf("\ndisk read bnum=%d,pos=%d\n",bnum,pos);
    return cyg_blib_read(&disk->blib, buf, len, bnum, pos);
    //return err = cyg_io_bread(disk->dev_h,(void *)buf,&len,pos);
}
// -------------------------------------------------------------------------
// disk_cluster_write_block()
// Writes data to disk at specified cluster position.

static __inline__ int
disk_cluster_write_block(fatfs_disk_t *disk,
                   void         *buf,
                   cyg_uint32   *len,
                   cyg_uint32    cluster,
                   cyg_uint32    cluster_pos)
{
    cyg_uint32 block, block_pos;

    cluster_to_block_pos(disk, cluster, cluster_pos, &block, &block_pos);
    //return ENOERR;
    return cyg_blib_write_block(&disk->blib, buf, len, block, block_pos);
}

// -------------------------------------------------------------------------
// disk_cluster_write()
// Writes data to disk at specified cluster position.

static __inline__ int
disk_cluster_write(fatfs_disk_t *disk,
                   void         *buf,
                   cyg_uint32   *len,
                   cyg_uint32    cluster,
                   cyg_uint32    cluster_pos)
{
    cyg_uint32 block, block_pos;

    cluster_to_block_pos(disk, cluster, cluster_pos, &block, &block_pos);
    //return ENOERR;
    return cyg_blib_write(&disk->blib, buf, len, block, block_pos);
}

// -------------------------------------------------------------------------
// disk_cluster_read()
// Reads data from disk at specified cluster position.

static __inline__ int
disk_cluster_read(fatfs_disk_t *disk,
                  void         *buf,
                  cyg_uint32   *len,
                  cyg_uint32    cluster,
                  cyg_uint32    cluster_pos)
{
    cyg_uint32 block, block_pos;

    cluster_to_block_pos(disk, cluster, cluster_pos, &block, &block_pos);

    return cyg_blib_read(&disk->blib, buf, len, block, block_pos);
}

// -------------------------------------------------------------------------
// jdays_to_gdate()
// Converts juilan days into gregorian date.
 
static void
jdays_to_gdate(cyg_uint32 jd, int *day, int *month, int *year)
{
    cyg_uint32 l, n, i, j;

    l = jd + 68569;
    n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    j = (80 * l) / 2447;
    *day = l - (2447 * j) / 80;

    l = j / 11;
    *month = j + 2 - (12 * l);
    *year = 100 * (n - 49) + i + l;
}

// -------------------------------------------------------------------------
// gdate_to_jdays()
// Converts gregorian date to juilan days.
 
static void
gdate_to_jdays(int day, int month, int year, cyg_uint32 *jd)
{
    *jd = day - 32075 + 1461*(year + 4800 + (month - 14)/12)/4 +
          367*(month - 2 - (month - 14)/12*12)/12 - 
          3*((year + 4900 + (month - 14)/12)/100)/4;
}
 
// -------------------------------------------------------------------------
// date_unix2dos()
// Converts unix timestamp to dos time and date. 
                 
static void
date_unix2dos(time_t  unix_timestamp, 
              cyg_uint16 *dos_time,
              cyg_uint16 *dos_date)
{
    cyg_uint32 jd;
    cyg_uint16 dtime, ddate;
    int        hour, min, sec;
    int        day, month, year;
    
    hour = (unix_timestamp / 3600) % 24;
    min  = (unix_timestamp / 60) % 60;
    sec  =  unix_timestamp % 60;

    jd = JD_1_JAN_1970 + unix_timestamp / (3600 * 24);
    jdays_to_gdate(jd, &day, &month, &year);

    CYG_TRACE7(TDE, "timestamp=%d date=%d:%d:%d %d-%d-%d",
               unix_timestamp, hour, min, sec, year, month, day);

    if (year < 1980)
        year = 1980;

    dtime = (hour << 11) | (min << 5) | (sec >> 1);
    ddate = ((year - 1980) << 9) | (month << 5) | day;
 
    CYG_TRACE2(TDE, "dos time=%d date=%d", dtime, ddate);
    
    if (NULL != dos_time) *dos_time = dtime;
    if (NULL != dos_date) *dos_date = ddate;
}

// -------------------------------------------------------------------------
// date_dos2unix()
// Converts dos time and date to unix timestamp. 
 
static void
date_dos2unix(cyg_uint16  dos_time, 
              cyg_uint16  dos_date, 
              time_t *unix_timestamp)
{
    cyg_uint32 ts; 
    int        hour, min, sec;
    int        day, month, year;
    
    sec        = (dos_time & ((1<<5)-1)) * 2;
    dos_time >>= 5;
    min        = (dos_time & ((1<<6)-1));
    dos_time >>= 6;
    hour       = dos_time;
    
    day        = (dos_date & ((1<<5)-1));
    dos_date >>= 5;
    month      = (dos_date & ((1<<4)-1));
    dos_date >>= 4;
    year       = dos_date + 1980;

    gdate_to_jdays(day, month, year, &ts);

    ts -= JD_1_JAN_1970;
    ts  = (ts * 24 * 3600) + (sec + min * 60 + hour * 3600);
    
    *unix_timestamp = ts;

    CYG_TRACE2(TDE, "dos time=%d date=%d", dos_time, dos_date);
    CYG_TRACE7(TDE, "timestamp=%d date=%d:%d:%d %d-%d-%d",
                    ts, hour, min, sec, year, month, day);
}

//==========================================================================
// FAT boot record functions 

#if TDE 

// -------------------------------------------------------------------------
// print_boot_record()
// Prints FAT boot record.

static void
print_boot_record(fat_boot_record_t* fbr)
{
    diag_printf("FAT: FBR jump code:       0x%02X\n", fbr->jump);
    diag_printf("FAT: FBR oem name:       '%.8s'\n",  fbr->oem_name);
    diag_printf("FAT: FBR bytes per sec:   %u\n",     fbr->bytes_per_sec);
    diag_printf("FAT: FBR sec per cluster: %u\n",     fbr->sec_per_clu);
    diag_printf("FAT: FBR reserved sec:    %u\n",     fbr->res_sec_num);
    diag_printf("FAT: FBR fat tbls num:    %u\n",     fbr->fat_tbls_num);
    diag_printf("FAT: FBR max root dents:  %u\n",     fbr->max_root_dents);
    diag_printf("FAT: FBR sec num (32):    %u\n",     fbr->sec_num_32);
    diag_printf("FAT: FBR media desc:      0x%02X\n", fbr->media_desc);
    diag_printf("FAT: FBR sec per fat:     %u\n",     fbr->sec_per_fat);
    diag_printf("FAT: FBR sec per track:   %u\n",     fbr->sec_per_track);
    diag_printf("FAT: FBR heads num:       %u\n",     fbr->heads_num);
    diag_printf("FAT: FBR hidden sec num:  %u\n",     fbr->hsec_num);
    diag_printf("FAT: FBR sec num:         %u\n",     fbr->sec_num);

    if (0 == fbr->sec_per_fat)
    {
        diag_printf("FAT: FBR sec per fat32:   %u\n",     fbr->sec_per_fat_32);
        diag_printf("FAT: FBR ext flags:       0x%04X\n", fbr->ext_flags);
        diag_printf("FAT: FBR fs ver:          %u\n",     fbr->fs_ver);
        diag_printf("FAT: FBR root cluster:    %u\n",     fbr->root_cluster);
        diag_printf("FAT: FBR fs info sec:     %u\n",     fbr->fs_info_sec);
    }
    
    diag_printf("FAT: FBR drv num:         %u\n",     fbr->drv_num);
    diag_printf("FAT: FBR ext sig:         0x%02X\n", fbr->ext_sig);
    diag_printf("FAT: FBR ser num:         0x%08X\n", fbr->ser_num);
    diag_printf("FAT: FBR vol name:       '%.11s'\n", fbr->vol_name);
    diag_printf("FAT: FBR fat name:       '%.8s'\n",  fbr->fat_name);
    diag_printf("FAT: FBR exe mark:        0x%02X 0x%02X\n", 
                fbr->exe_marker[0], fbr->exe_marker[1]);
}

#endif // TDE

// -------------------------------------------------------------------------
// read_boot_record()
// Reads FAT boot record from disk.
 
static int 
read_boot_record(fatfs_disk_t *disk, fat_boot_record_t *fbr)
{
    //cyg_uint8 data[0x5A];
    cyg_uint32 len = 1;
    int err;
    cyg_uint8 *data = (cyg_uint8*)malloc(disk->sector_size);
    //len = 0x5A;
    //err = disk_read(disk, (void*)data, &len, 0);
    err = cyg_io_bread(disk->dev_h,(void *)data,&len,0);
    if (err != ENOERR)
        return err;
   
    GET_WORD(data,  fbr->jump,           0x00);
    GET_BYTES(data, fbr->oem_name, 8,    0x03);
    GET_WORD(data,  fbr->bytes_per_sec,  0x0B);
    GET_BYTE(data,  fbr->sec_per_clu,    0x0D);
    GET_WORD(data,  fbr->res_sec_num,    0x0E);
    GET_BYTE(data,  fbr->fat_tbls_num,   0x10);
    GET_WORD(data,  fbr->max_root_dents, 0x11);
    GET_WORD(data,  fbr->sec_num_32,     0x13);
    GET_BYTE(data,  fbr->media_desc,     0x15);
    GET_WORD(data,  fbr->sec_per_fat,    0x16);
    GET_WORD(data,  fbr->sec_per_track,  0x18);
    GET_WORD(data,  fbr->heads_num,      0x1A);
    GET_DWORD(data, fbr->hsec_num,       0x1C);
    GET_DWORD(data, fbr->sec_num,        0x20);

    // This is a quick check for FAT12/16 or FAT32 boot record.
    // The sec_per_fat field must be 0 on FAT32, since this
    // field plays a crucial role in detection of the FAT type 
    // (12,16,32) it is quite safe to make this assumption.
    if (0 == fbr->sec_per_fat)
    {
        GET_DWORD(data, fbr->sec_per_fat_32, 0x24);
        GET_WORD(data,  fbr->ext_flags,      0x28);
        GET_WORD(data,  fbr->fs_ver,         0x2A);
        GET_DWORD(data, fbr->root_cluster,   0x2C);
        GET_WORD(data,  fbr->fs_info_sec,    0x30);
        GET_WORD(data,  fbr->bk_boot_sec,    0x32);
        GET_BYTE(data,  fbr->drv_num,        0x40);
        GET_BYTE(data,  fbr->ext_sig,        0x42);
        GET_DWORD(data, fbr->ser_num,        0x43);
        GET_BYTES(data, fbr->vol_name, 11,   0x47);
        GET_BYTES(data, fbr->fat_name, 8,    0x52);
    }
    else
    {
        GET_BYTE(data,  fbr->drv_num,        0x24);
        GET_BYTE(data,  fbr->ext_sig,        0x26);
        GET_DWORD(data, fbr->ser_num,        0x27);
        GET_BYTES(data, fbr->vol_name, 11,   0x2B);
        GET_BYTES(data, fbr->fat_name, 8,    0x36);
    }
    
    // Read the end marker
    /*len = 0x02;
    err = disk_read(disk, (void*)data, &len, 0x1FE);
    if (err != ENOERR)
        return err;
    GET_BYTES(data, fbr->exe_marker, 2,  0);*/

    GET_BYTES(data, fbr->exe_marker, 2,  0x1FE);

    // Zero terminate strings
    fbr->oem_name[8]  = '\0';
    fbr->vol_name[11] = '\0';
    fbr->fat_name[8]  = '\0';
    free(data);
#if TDE 
    print_boot_record(fbr);
#endif
    
    return ENOERR;
}

//==========================================================================
// FAT table entry functions 

// -------------------------------------------------------------------------
// read_tentry()
// Reads FAT table entry from disk.

static int
read_tentry(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    cyg_uint8  data[4];
    cyg_uint32 pos, num3;
    cyg_uint32 e;
    cyg_uint32 len;
    int err;

    switch (disk->fat_type)
    {
        case FATFS_FAT12:
            num3 = num * 3;
            pos  = disk->fat_tbl_pos + (num3 >> 1);
            len  = 2;
    
            err = disk_read(disk, (void*)data, &len, pos);
            if (err != ENOERR)
                return err;

            GET_WORD(data, e, 0x00);

            if (0 == (num3 & 1)) *entry = e        & 0x0FFF;
            else                 *entry = (e >> 4) & 0x0FFF;

            break;
            
        case FATFS_FAT16:
            pos = disk->fat_tbl_pos + (num << 1);
            len = 2;
    
            err = disk_read(disk, (void*)data, &len, pos);
            if (err != ENOERR)
                return err;

            GET_WORD(data, e, 0x00);
            *entry = e;

            break; 
            
        case FATFS_FAT32:
            pos = disk->fat_tbl_pos + (num << 2);
            len = 4;
            TR(("pos=%d num=%d fat_tbl_pos=%d\n",pos,num,disk->fat_tbl_pos));
            err = disk_read(disk, (void*)data, &len, pos);
            if (err != ENOERR)
                {diag_printf("disk_read fat fail\n");return err;}

            GET_DWORD(data, e, 0x00);
            *entry = e & 0x0FFFFFFF;

            break;

        default:
            CYG_ASSERT(false, "Unknown FAT type");
    }
    return ENOERR;
}

// -------------------------------------------------------------------------
// write_tentry()
// Writes FAT table entry to disk (to all copies of FAT).
 
static int
write_tentry(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    cyg_uint8  data[4];
    cyg_uint32 pos=0, num3; 
    cyg_uint32 e;
    cyg_uint32 len;
    int i;
    int err;

    switch (disk->fat_type)
    {
        case FATFS_FAT12:
            num3 = num * 3;
            pos  = disk->fat_tbl_pos + (num3 >> 1);
            len  = 2;
   
            err = disk_read(disk, (void*)data, &len, pos);
            if (err != ENOERR)
                return err;

            GET_WORD(data, e, 0x00);
  
            if (0 == (num3 & 1)) e = (e & 0xF000) | (*entry & 0x0FFF);
            else                 e = (e & 0x000F) | ((*entry & 0x0FFF) << 4);
    
            SET_WORD(data, e, 0x00);

            break;

        case FATFS_FAT16:
            pos = disk->fat_tbl_pos + (num << 1);
            len = 2;
    
            e = *entry;
            SET_WORD(data, e, 0x00);

            break;  
            
        case FATFS_FAT32:
            pos = disk->fat_tbl_pos + (num << 2);
            len = 4;
            TR(("fat_tbl_pos=%d num=%d pos=%d\n",disk->fat_tbl_pos,num,pos));
            err = disk_read(disk, (void*)data, &len, pos);
            if (err != ENOERR)
                return err;

            GET_DWORD(data, e, 0x00);

            e = (e & 0xF0000000) | *entry;
    
            SET_DWORD(data, e, 0x00);

            break; 

        default:
            CYG_ASSERT(false, "Unknown FAT type");
    }
    
    for (i = 0; i < disk->fat_tbls_num; i++)
    {
        err = disk_write(disk, (void*)data, &len, pos);
        if (err != ENOERR)
            return err;

        pos += disk->fat_tbl_size;
    }
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// get_tentry_type()
// Gets the type of FAT table entry.
 
static int
get_tentry_type(fatfs_disk_t *disk, cyg_uint32 entry)
{
    int type;

    switch (disk->fat_type)
    {
        case FATFS_FAT12:
            if (entry < 0x0FF0)
            {
                if (0x0000 == entry)  type = TENTRY_FREE;
                else                  type = TENTRY_REGULAR;
            }
            else if (entry >= 0x0FF8) type = TENTRY_LAST;
            else if (0x0FF7 == entry) type = TENTRY_BAD;
            else                      type = TENTRY_RESERVED;

            break;

        case FATFS_FAT16:
            if (entry < 0xFFF0)
            {
                if (0x0000 == entry)  type = TENTRY_FREE;
                else                  type = TENTRY_REGULAR;
            }
            else if (entry >= 0xFFF8) type = TENTRY_LAST;
            else if (0xFFF7 == entry) type = TENTRY_BAD;
            else                      type = TENTRY_RESERVED;

            break;

        case FATFS_FAT32:

            if (entry < 0x0FFFFFF0)
            {
                if (0x00000000 == entry)  type = TENTRY_FREE;
                else                      type = TENTRY_REGULAR;
            }
            else if (entry >= 0x0FFFFFF8) type = TENTRY_LAST;
            else if (0x0FFFFFF7 == entry) type = TENTRY_BAD;
            else                          type = TENTRY_RESERVED;

            break;

        default:
            CYG_ASSERT(false, "Unknown FAT type");
            type = TENTRY_BAD; // least likely to cause damage
    }
    return type;
}

// -------------------------------------------------------------------------
// set_tentry_type()
// Sets the type of FAT table entry.
 
static void 
set_tentry_type(fatfs_disk_t *disk, cyg_uint32 *entry, cyg_uint32 type)
{
    switch (disk->fat_type)
    {
        case FATFS_FAT12:
            switch (type)
            {
                case TENTRY_FREE:     *entry = 0x0000; return;
                case TENTRY_LAST:     *entry = 0x0FF8; return;
                case TENTRY_RESERVED: *entry = 0x0FF0; return;
                case TENTRY_BAD:      *entry = 0x0FF7; return;      
                default:
                    CYG_ASSERT(false, "Unknown tentry type");
            }
            break;
            
        case FATFS_FAT16:
            switch (type)
            {
                case TENTRY_FREE:     *entry = 0x0000; return;
                case TENTRY_LAST:     *entry = 0xFFF8; return;
                case TENTRY_RESERVED: *entry = 0xFFF0; return;
                case TENTRY_BAD:      *entry = 0xFFF7; return;
                default:
                    CYG_ASSERT(false, "Unknown tentry type");
            }
            break;
            
        case FATFS_FAT32:
            switch (type)
            {
                case TENTRY_FREE:     *entry = 0x00000000; return;
                case TENTRY_LAST:     *entry = 0x0FFFFFF8; return;
                case TENTRY_RESERVED: *entry = 0x0FFFFFF0; return;
                case TENTRY_BAD:      *entry = 0x0FFFFFF7; return;      
                default:
                    CYG_ASSERT(false, "Unknown tentry type");
            }
            break;

        default:
            CYG_ASSERT(false, "Unknown FAT type");
    }
}

// -------------------------------------------------------------------------
// get_tentry_next_cluster()
// Gets the the next file cluster number from FAT table entry.
 
static __inline__ cyg_uint32 
get_tentry_next_cluster(fatfs_disk_t *disk, cyg_uint32 entry)
{
    return entry;
}

// -------------------------------------------------------------------------
// set_tentry_next_cluster()
// Sets the the next cluster number to FAT table entry.
 
static __inline__ void 
set_tentry_next_cluster(fatfs_disk_t *disk, 
                        cyg_uint32   *entry, 
                        cyg_uint32    next_cluster)
{
    *entry = next_cluster;
}

//==========================================================================
// FAT cluster functions 

// -------------------------------------------------------------------------
// erase_cluster()
// Erases cluster (fills with 0x00).

static int
erase_cluster(fatfs_disk_t *disk, cyg_uint32 cluster)
{
    cyg_uint8  data[32];
    cyg_uint32 pos;
    cyg_uint32 len;
    int err, i;
    
    pos = 0;
    len = 32;
    memset((void*)data, 0x00, len);
    
    CYG_TRACE1(TCL, "erasing cluster=%d", cluster);
    TR(("erasing cluster=%d\n", cluster));

    for (i = 0; i < (disk->cluster_size >> 5); i++)
    {
        err = disk_cluster_write(disk, (void*)data, &len, cluster, pos);
        if (err != ENOERR)
            return err;

        pos += len;
    }
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// mark_cluster()
// Marks cluster (sets the cluster's FAT table entry to given type). 

static int
mark_cluster(fatfs_disk_t *disk, cyg_uint32 cluster, cyg_uint32 type)
{
    cyg_uint32 tentry;
 
    set_tentry_type(disk, &tentry, type);
    TR(("mark_cluster tentry=%x\n",tentry));
    return write_tentry(disk, cluster, &tentry);
}

// -------------------------------------------------------------------------
// link_cluster()
// Links two clusters.

static int
link_cluster(fatfs_disk_t *disk, cyg_uint32 cluster1, cyg_uint32 cluster2)
{
    cyg_uint32 tentry;
    
    set_tentry_next_cluster(disk, &tentry, cluster2);
    return write_tentry(disk, cluster1, &tentry);
}

// -------------------------------------------------------------------------
// find_next_free_cluster()
// Finds first free cluster starting from given cluster.
// If none is available free_cluster is set to 0.
// If CO_MARK_LAST is set in opts the found cluster is marked as LAST.
// If CO_ERASE_NEW is set in opts the found cluster is erased.

static int
find_next_free_cluster(fatfs_disk_t   *disk,
                       cyg_uint32      start_cluster, 
                       cyg_uint32     *free_cluster,
                       cluster_opts_t  opts)
{
    cyg_uint32 c, tentry;
    int        err;

    if (start_cluster < 2) c = 2;
    else                   c = start_cluster + 1;

    *free_cluster = 0;

    CYG_TRACE1(TCL, "starting at cluster=%d", c);
    TR(("starting at cluster=%d\n", c));
   
    // Search from the starting cluster to the end of FAT and
    // from start of FAT to the starting cluster
    while (c != start_cluster)
    {
        // Check for end of FAT
        if (c >= disk->fat_tbl_nents)
        {
            c = 2;
            if (c >= start_cluster)
                break;
        }

        err = read_tentry(disk, c, &tentry);
        if (err != ENOERR)
            {diag_printf("read FAT fail\n");return err;}

        if (TENTRY_FREE == get_tentry_type(disk, tentry))
        {
            CYG_TRACE1(TCL, "found free cluster=%d", c);
            //diag_printf("found free cluster=%d\n",c);
            *free_cluster = c;
            
            if (opts & CO_MARK_LAST)
                err = mark_cluster(disk, c, TENTRY_LAST);
            if ((err == ENOERR) && (opts & CO_ERASE_NEW))
                err = erase_cluster(disk, c);
            
            return err;
        }
        c++;
    }   

    // No free clusters found

    CYG_TRACE0(TCL, "!!! no free clusters found");
    TR(("!!! no free clusters found\n"));
    return ENOSPC;
}

// -------------------------------------------------------------------------
// find_and_append_cluster()
// Finds a free cluster on disk and appends it to the given cluster. 
// New cluster is marked as LAST. 

static int
find_and_append_cluster(fatfs_disk_t   *disk, 
                        cyg_uint32      cluster, 
                        cyg_uint32     *new_cluster,
                        cluster_opts_t  opts)
{
    cyg_uint32 free_cluster;
    int        err;
    TR(("enter find_and_append_cluster\n"));
    err = find_next_free_cluster(disk, cluster, 
        &free_cluster, opts | CO_MARK_LAST);
    if (err != ENOERR)
        return err;

    err = link_cluster(disk, cluster, free_cluster);
    if (err != ENOERR)
        return err;

    *new_cluster = free_cluster;

    CYG_TRACE2(TCL, "appending new cluster=%d to cluster=%d", 
                    free_cluster, cluster);
    TR(("appending new cluster=%d to cluster=%d\n", 
                    free_cluster, cluster));
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// find_nth_cluster0()
// Finds nth cluster in chain (ie nth cluster of file) searching
// from given position. The result is returned by the same position
// variable. 
 
static int
find_nth_cluster0(fatfs_disk_t     *disk,
                  fatfs_data_pos_t *pos, 
                  cyg_uint32        n)
{
    cyg_uint32 cluster, cluster_snum;
    int        err = ENOERR;
 
    if (pos->cluster_snum == n)
        return ENOERR;

    cluster      = pos->cluster;
    cluster_snum = pos->cluster_snum;
   
    CYG_TRACE4(TCL, "cluster=%d snum=%d n=%d n_to_search=%d",
                    cluster, cluster_snum, n, n-cluster_snum);
    TR(("cluster=%d snum=%d n=%d n_to_search=%d\n",
                    cluster, cluster_snum, n, n-cluster_snum));
    // Adjust the number of clusters that should be
    // walked according to the given position
    n -= cluster_snum;

    // Walk the cluster chain for n clusters or until last cluster
    while (n > 0)
    {
        cyg_uint32 tentry;

        err = read_tentry(disk, cluster, &tentry);
        if (err != ENOERR)
            return err;

        switch (get_tentry_type(disk, tentry))
        {
            case TENTRY_REGULAR:
                break;
            case TENTRY_LAST:
                CYG_TRACE1(TCL, "chain end at n=%d", n);
                err = EEOF; // File has less clusters than given n
                            // this err should be caught by the 
                            // calling function 
                goto out;
            default:
                // Inconsistant FAT table state !!!
                CYG_TRACE2(TCL, "!!! inconsistant FAT tentry=%x n=%d", 
                                tentry, n);
                err = EIO;                 
                goto out;
        }
        cluster = get_tentry_next_cluster(disk, tentry);
        cluster_snum++;
        n--;
    }
    
out:
    pos->cluster      = cluster;
    pos->cluster_snum = cluster_snum;

    CYG_TRACE2(TCL, "nth cluster=%d snum=%d", cluster, cluster_snum);

    return err;
}

// -------------------------------------------------------------------------
// find_nth_cluster()
// Finds nth cluster in chain (ie nth cluster of file) searching 
// from given position. The result is returned by the same position
// variable. If the chain ends one cluster before the given nth cluster 
// and the CO_EXTEND is specified, than the chain is extended by one cluster.
 
static int
find_nth_cluster(fatfs_disk_t     *disk,
                 fatfs_data_pos_t *pos, 
                 cyg_uint32        n,
                 cluster_opts_t    opts)
{
    int err;
   
    // Find nth cluster 
    err = find_nth_cluster0(disk, pos, n);    

    // EEOF meens that the cluster chain ended early
    if ((err != EEOF) || !(opts & CO_EXTEND))
        return err;
    
    // Check if one cluster short
    if (pos->cluster_snum == (n - 1))
    {
        // Extend the chain for one cluster
        cyg_uint32 new_cluster;

        // Append new cluster to the end of chain
        err = find_and_append_cluster(disk, pos->cluster, &new_cluster, opts);
        if (err != ENOERR)
            return err;

        // Update position
        pos->cluster       = new_cluster;
        pos->cluster_snum += 1;
        pos->cluster_pos   = 0;

        CYG_TRACE1(TCL, "appended new cluster=%d", new_cluster);
        TR(("appended new cluster=%d\n", new_cluster));
    }
    
    return err;
}

// -------------------------------------------------------------------------
// get_next_cluster()
// Gets next cluster in chain (ie next cluster of file).
// If CO_EXTEND is specified and the current cluster is last in the 
// chain then the chain is extended by one cluster.

static int
get_next_cluster(fatfs_disk_t     *disk,
                 fatfs_data_pos_t *pos,
                 cluster_opts_t    opts)
{
    int err;

    err = find_nth_cluster(disk, pos, pos->cluster_snum + 1, opts);
    if (err != ENOERR)
        return err;

    // Reset inside cluster position
    pos->cluster_pos = 0;
 
    return ENOERR;
}

// -------------------------------------------------------------------------
// get_position_from_off()
// Gets position from given offset. The search is started from the
// given position and the result is returned by the same variable. 
// If CO_EXTEND is specified the file is extended if one cluster too short.

static int
get_position_from_off(fatfs_disk_t     *disk,
                      cyg_uint32        first_cluster,
                      cyg_uint32        offset,
                      fatfs_data_pos_t *pos,
                      cluster_opts_t    opts)
{
    fatfs_data_pos_t new_pos;
    cyg_uint32       n;
    int              err;
    TR(("get_position_from_off first_cluster=%d offset=%d cluster=%d cluster_snum=%d cluster_pos=%d\n",
            first_cluster,offset,pos->cluster,pos->cluster_snum,pos->cluster_pos));
    // Position inside the cluster
    new_pos.cluster_pos = offset & (disk->cluster_size - 1);

    // Cluster seq number to be searched for
    n = offset >> disk->cluster_size_log2;

    if (n < pos->cluster_snum)
    { 
        // Start searching from first cluster
        new_pos.cluster      = first_cluster;
        new_pos.cluster_snum = 0;
    }
    else
    {
        // Start searching from the current position
        new_pos.cluster      = pos->cluster;
        new_pos.cluster_snum = pos->cluster_snum;
    }

    err = find_nth_cluster(disk, &new_pos, n, opts);
    
    // Err could be EEOF wich means that the given 
    // offset if out of given file (cluster chain)

    if (EEOF == err)
        new_pos.cluster_pos = disk->cluster_size; 
    
    *pos = new_pos; 
    
    return err;
} 

// -------------------------------------------------------------------------
// free_cluster_chain()
// Marks all clusters FREE from given cluster to the last cluster in chain.

static int
free_cluster_chain(fatfs_disk_t *disk, cyg_uint32 start_cluster)
{
    cyg_uint32 c, next_c, tentry;
    bool       last;
    int        err;

    CYG_TRACE1(TCL, "start cluster=%d", start_cluster);
    TR(("start cluster=%d", start_cluster));
    if(0 == start_cluster) return ENOERR;
    c = next_c = start_cluster;
    last = false;
    while (!last)
    {
        err = read_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        switch (get_tentry_type(disk, tentry))
        {
            case TENTRY_LAST:
                // Last cluster in chain
                last = true;
                break;
            case TENTRY_REGULAR:
                // Get next cluster in chain
                next_c = get_tentry_next_cluster(disk, tentry);
                break;
            default:
                CYG_TRACE2(TCL, "!!! inconsistant FAT tentry=%x c=%d", 
                                tentry, c);
                return EIO;
        }

        // Set the current tentry to FREE 
        set_tentry_type(disk, &tentry, TENTRY_FREE);
        err = write_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        // Next cluster in chain
        c = next_c; 
    }

    CYG_TRACE1(TCL, "last cluster=%d", c);
    
    return ENOERR;
}

//==========================================================================
// FAT dir entry functions 

// -------------------------------------------------------------------------
// print_raw_dentry()
// Prints FAT directory entry. 

#if TDE
static void
print_raw_dentry(fat_raw_dir_entry_t* dentry)
{
    if (DENTRY_IS_DELETED(dentry))
        diag_printf("FAT: FDE name:    '?%.7s'\n", &dentry->name[1]);
    else    
        diag_printf("FAT: FDE name:    '%.8s'\n", dentry->name);
    diag_printf("FAT: FDE ext:     '%.3s'\n", dentry->ext);
    diag_printf("FAT: FDE attr:     %c%c%c%c%c%c\n", 
                (DENTRY_IS_RDONLY(dentry)  ? 'R' : '-'),
                (DENTRY_IS_HIDDEN(dentry)  ? 'H' : '-'),
                (DENTRY_IS_SYSTEM(dentry)  ? 'S' : '-'),
                (DENTRY_IS_VOLUME(dentry)  ? 'V' : '-'),
                (DENTRY_IS_DIR(dentry)     ? 'D' : '-'),
                (DENTRY_IS_ARCHIVE(dentry) ? 'A' : '-'));
    diag_printf("FAT: FDE crt time: %u\n", dentry->crt_time);
    diag_printf("FAT: FDE crt date: %u\n", dentry->crt_date);
    diag_printf("FAT: FDE acc date: %u\n", dentry->acc_date);
    diag_printf("FAT: FDE wrt time: %u\n", dentry->wrt_time);
    diag_printf("FAT: FDE wrt date: %u\n", dentry->wrt_date);
    diag_printf("FAT: FDE cluster:  %u\n", (dentry->cluster_HI << 16) | dentry->cluster);
    diag_printf("FAT: FDE size:     %u\n", dentry->size);
}
#endif // TDE

// -------------------------------------------------------------------------
// read_raw_dentry()
// Reads dir entry from disk.
 
static int
read_raw_dentry(fatfs_disk_t        *disk,
                fatfs_data_pos_t    *pos,
                fat_raw_dir_entry_t *dentry,
                cyg_uint8*    rawdent)
{
    cyg_uint8  data[DENTRY_SIZE];
    cyg_uint32 len;
    int err;

    CYG_TRACE3(TDE, "cluster=%d snum=%d pos=%d",
               pos->cluster, pos->cluster_snum, pos->cluster_pos); 
	/*TR(("cluster=%d snum=%d pos=%d\n",
               pos->cluster, pos->cluster_snum, pos->cluster_pos));*/
   
    len = DENTRY_SIZE;

    // Check if we are reading the FAT12/16 root directory
    if (0 == pos->cluster)
        err = disk_read(disk, (void*)data, &len,  
                        disk->fat_root_dir_pos + pos->cluster_pos);
    else
        err = disk_cluster_read(disk, (void*)data, &len, 
                                pos->cluster, pos->cluster_pos);
    if (err != ENOERR)
        return err;
    if(NULL != dentry){
    GET_BYTES(data, dentry->name,     8, 0x00);
    GET_BYTES(data, dentry->ext,      3, 0x08);
    GET_BYTE(data,  dentry->attr,        0x0B);
    GET_BYTE(data,  dentry->nt_reserved, 0x0C);
    GET_BYTE(data,  dentry->crt_sec_100, 0x0D);
    GET_WORD(data,  dentry->crt_time,    0x0E);
    GET_WORD(data,  dentry->crt_date,    0x10);
    GET_WORD(data,  dentry->acc_date,    0x12);
    GET_WORD(data,  dentry->cluster_HI,  0x14);
    GET_WORD(data,  dentry->wrt_time,    0x16);
    GET_WORD(data,  dentry->wrt_date,    0x18);
    GET_WORD(data,  dentry->cluster,     0x1A);
    GET_DWORD(data, dentry->size,        0x1C);

     // Zero terminate strings
    dentry->name[8] = '\0';    
    dentry->ext[3]  = '\0';    
  
#if TDE    
    print_raw_dentry(dentry);
#endif
    }
    if(NULL != rawdent)
    memcpy(rawdent,data,32);
    return ENOERR;
}

// -------------------------------------------------------------------------
// write_raw_dentry()
// Writes raw dir entry to disk. 
 
static int
write_raw_dentry(fatfs_disk_t        *disk,
                 fatfs_data_pos_t    *pos, 
                 fat_raw_dir_entry_t *dentry)
{
    cyg_uint8  data[DENTRY_SIZE];
    cyg_uint32 len;
    int err;

    CYG_TRACE3(TDE, "cluster=%d snum=%d pos=%d\n",
               pos->cluster, pos->cluster_snum, pos->cluster_pos); 
   TR(("write_raw_dentry:cluster=%d snum=%d pos=%d\n",
               pos->cluster, pos->cluster_snum, pos->cluster_pos));
    SET_BYTES(data, dentry->name,     8, 0x00);
    SET_BYTES(data, dentry->ext,      3, 0x08);
    SET_BYTE(data,  dentry->attr,        0x0B);
    SET_BYTE(data,  dentry->nt_reserved, 0x0C);
    SET_BYTE(data,  dentry->crt_sec_100, 0x0D);
    SET_WORD(data,  dentry->crt_time,    0x0E);
    SET_WORD(data,  dentry->crt_date,    0x10);
    SET_WORD(data,  dentry->acc_date,    0x12);
    SET_WORD(data,  dentry->cluster_HI,  0x14);
    SET_WORD(data,  dentry->wrt_time,    0x16);
    SET_WORD(data,  dentry->wrt_date,    0x18);
    SET_WORD(data,  dentry->cluster,     0x1A);
    SET_DWORD(data, dentry->size,        0x1C);
   
    len = DENTRY_SIZE;
    //diag_dump_buf_with_offset(data,32,data);
    // Check if we are writting to the FAT12/16 root directory
    if (0 == pos->cluster)
    {
        err = disk_write(disk, (void*)data, &len, 
                         disk->fat_root_dir_pos + pos->cluster_pos);
        TR(("disk_write error=%d\n",err));
    }
        
    else
    {
        err = disk_cluster_write(disk, (void*)data, &len,
                                 pos->cluster, pos->cluster_pos);
        if(err != ENOERR)
            TR(("disk_cluster_write error=%d\n",err));
    }
        
    if (err != ENOERR)
        return err;

#if TDE    
    print_raw_dentry(dentry);
#endif

    return ENOERR;
}

// -------------------------------------------------------------------------
// raw_dentry_set_deleted()
// Sets the dentry filename first char to 0xE5 (ie deleted). 
 
static __inline__ void 
raw_dentry_set_deleted(fatfs_disk_t *disk, fat_raw_dir_entry_t *dentry)
{
    dentry->name[0] = 0xE5;
}

// -------------------------------------------------------------------------
// get_raw_dentry_filename()
// Gets the filename from given dir entry. 
 
static void 
get_raw_dentry_filename(fat_raw_dir_entry_t *dentry, char *name)
{
    int   i     = 0;
    char *cptr  = dentry->s_utf8;
    char *cname = name;
    TR(("name =%s\n",cptr));
    //diag_dump_buf_with_offset( cptr, 64, cptr );
    memcpy(cname,cptr,4*(FM_MAX_FILE_LEN+1));
    //diag_dump_buf_with_offset( cname, 2*(FM_MAX_FILE_LEN+1), cname );
    /*while (*cptr != ' ' && i < 8)
    {
        *cname++ = *cptr++; //i++;
    }
    cptr = dentry->ext;

    if (*cptr != ' ')
    {
        *cname++ = '.'; i = 0;
        while (*cptr != ' ' && i < 3)
        {
            *cname++ = *cptr++; i++;
        }
    }
    *cname = '\0';*/

    /*CYG_TRACE3(TDE, "dos name='%s' dos ext='%s' filename='%s'",
                    dentry->name, dentry->ext, name);*/
}

// -------------------------------------------------------------------------
// set_raw_dentry_filename()
// Sets the filename of given dir entry. 
 
static void 
set_raw_dentry_filename(fat_raw_dir_entry_t *dentry, 
                        const char          *name, 
                        int                  namelen)
{
    int         i, nidx;
    const char *cname;
    char       *cptr;

    // Special case check
    if ('.' == name[0])
    {
        //diag_printf("\ncreat . or .. name=%s\n",name);
        if ('\0' == name[1])
        {
            strcpy(dentry->name, ".       ");
            strcpy(dentry->ext,  "   ");
            return;
        }
        else if ('.' == name[1] && '\0' == name[2])
        {
            strcpy(dentry->name, "..      ");
            strcpy(dentry->ext,  "   ");
            return;
        }
    }
    
    if (0 == namelen)
        namelen = 9999;
    
    nidx  = 0;
    cname = name;
    cptr  = dentry->name;
    for (i = 0; i < 8; i++)
    {
        if (*cname != '.' && *cname != '\0' && nidx++ < namelen)
            *cptr++ = toupper(*cname++);
        else
            *cptr++ = ' ';
    }
    *cptr = '\0';
    
   /*while (*cname != '.' && *cname != '\0' && nidx++ < namelen)
        cname++;*/
   
    if ('.' == *cname && nidx++ < namelen) 
        cname++;
    
    cptr = dentry->ext;
    for (i = 0; i < 3; i++)
    {
        if (*cname != '.' && *cname != '\0' && nidx++ < namelen)
            *cptr++ = toupper(*cname++); 
        else
            *cptr++ = ' ';
    }
    *cptr = '\0';

    CYG_TRACE3(TDE, "filename='%s' dos name='%s' dos ext='%s'", 
                    name, dentry->name, dentry->ext);
    //diag_dump_buf_with_offset( name, 32, name );
    TR(("filename='%s' dos name='%s' dos ext='%s'\n", 
                    name, dentry->name, dentry->ext));
}

static cyg_uint8 _fat_longname_chksum(
    cyg_uint8       *ps_name)
{
    cyg_uint8 ui1_sum = 0;
    cyg_uint32 i;

    for (i = 11; i != 0; i--)
    {
        ui1_sum = (cyg_uint8)(((ui1_sum & 1) ? 0x80 : 0) + (ui1_sum >> 1) + *ps_name++);
    }
    return ui1_sum;
}
static void _fat_trans_unicode_endian(
    cyg_uint8           *ps_cpu,
    const cyg_uint8     *ps_fat)
{
    cyg_uint8       *pc_cpu;
    const cyg_uint8 *pc_fat;

    for (pc_cpu = ps_cpu, pc_fat = ps_fat;
         (*pc_fat != 0) || (*(pc_fat + 1) != 0);
         pc_cpu += 2, pc_fat += 2)
    {
#if defined _CPU_LITTLE_ENDIAN_
        *pc_cpu = *pc_fat;
        *(pc_cpu + 1) = *(pc_fat + 1);
#else
        *pc_cpu = *(pc_fat + 1);
        *(pc_cpu + 1) = *pc_fat;
#endif /* _CPU_LITTLE_ENDIAN_ */
    }
    *pc_cpu = 0;
    *(pc_cpu + 1) = 0;
}
static void _fat_short_name_strcpy(
    cyg_uint8       *pc_dst,
    cyg_uint8       *pc_dent,
    cyg_uint8       ui1_nt_res)
{
    cyg_uint8 *pc = pc_dst;
    cyg_int32 i;

    //ASSERT(pc_dst  != NULL);
    //ASSERT(pc_dent != NULL);

    for (i = 0; i < 11; i++)
    {
        if (pc_dent[i] == 0x20)
        {
            if (i >= 8)
            {
                break;
            }
            continue;
        }
        if (i == 8)
        {
            *pc++ = (cyg_uint8)('.');
        }

        if ((((i < 8) && (ui1_nt_res & 0x08)) || ((i >= 8) && (ui1_nt_res & 0x10))) &&
            ((pc_dent[i] >= 'A') && (pc_dent[i] <= 'Z')))
        {
            *pc++ = pc_dent[i] + 'a' - 'A';
            
        }
        else
        {
            *pc++ = pc_dent[i];
        }
    }
    *pc = 0;
}

static void _fat_get_dentry(
    fat_raw_dir_entry_t *pt_dent,
    cyg_uint8        *pui1_dent)
{
    //ASSERT(pt_dent != NULL);
    //ASSERT(pui1_dent != NULL);

    memcpy(pt_dent->name, FAT_OFFSET(pui1_dent, FAT_DENTRY_NAME), 8);
    pt_dent->name[8] = 0;
    memcpy(pt_dent->ext, FAT_OFFSET(pui1_dent, 8), 3);
    pt_dent->ext[3] = 0;
    pt_dent->attr = FAT_GET_VAL8(pui1_dent, FAT_DENTRY_ATTR);
    pt_dent->crt_sec_100 = FAT_GET_VAL8(pui1_dent, FAT_DENTRY_CRT_TIME_TENTH);
    pt_dent->crt_time = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_CRT_TIME);
    pt_dent->crt_date = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_CRT_DATE);
    pt_dent->acc_date = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_ACC_DATE);
    pt_dent->wrt_time = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_WRT_TIME);
    pt_dent->wrt_date = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_WRT_DATE);
    pt_dent->cluster_HI= ((FAT_GET_VAL16(pui1_dent, FAT_DENTRY_CLUS_HI)) << 16);
    pt_dent->cluster = FAT_GET_VAL16(pui1_dent, FAT_DENTRY_CLUS_LO);
    pt_dent->size = FAT_GET_VAL32(pui1_dent, FAT_DENTRY_FILE_SIZE);

    /*pt_dent->t_clus_cache.ui4_head_pclus
        = pt_dent->t_clus_cache.ui4_tail_pclus
        = pt_dent->t_clus_cache.at_cache[0].ui4_pclus
        = pt_dent->ui4_first_clus;
    pt_dent->t_clus_cache.ui4_head_lclus
        = pt_dent->t_clus_cache.ui4_tail_lclus
        = pt_dent->t_clus_cache.at_cache[0].ui4_lclus
        = pt_dent->t_clus_cache.ui4_last_valid_entry
        = 0;*/
    
    /*if (pt_dent->ui1_attr & FAT_ATTR_DIRECTORY)
    {
        pt_dent->b_need_rebuild = true;
    }
    else
    {
        pt_dent->b_need_rebuild = FALSE;
    }
    pt_dent->pt_index_tbl = NULL;
    pt_dent->ui4_sub_ent_num = 0;
    pt_dent->ui4_blk_num = 0;*/
}
// -------------------------------------------------------------------------
// read_next_raw_dentry()
// Gets next dir entry searching from given position to the end.
// If EEOF is returned there are no more entries in given dir.
 
static int
read_next_raw_dentry(fatfs_disk_t        *disk,
                     fatfs_data_pos_t    *pos,
                     fat_raw_dir_entry_t *dentry)
{
    int err = ENOERR;
    cyg_uint8   pui1_dent[32];
    cyg_int32   i4_ret = ENOERR;
    cyg_uint8   ui1_ent_num = 0;
    cyg_uint8   ui1_attr;
    cyg_uint8   ui1_nt_res;
    cyg_uint8   ui1_chksum  = 0;
    cyg_uint16 ui2_str_off;
    cyg_uint8 *s_utf16 = (cyg_uint8*)malloc(2*(FM_MAX_FILE_LEN + 6));
    memset(dentry->s_utf8,0,4*(FM_MAX_FILE_LEN+1));
    if (s_utf16 == NULL) return EEOF;
    // If we are reading the root dir on FAT32 we have
    // to correct the position to the root dir cluster
    if (FATFS_FAT32 == disk->fat_type && 0 == pos->cluster)
        pos->cluster = disk->fat_root_dir_cluster;
        
    while (true)
    {
        // FAT12/16 root dir check
        if (0 == pos->cluster) 
        {
            if (pos->cluster_pos >= disk->fat_root_dir_size)
                err = EEOF;
        }
        else
        {
            // Change cluster if needed
            if (pos->cluster_pos >= disk->cluster_size)
                err = get_next_cluster(disk, pos, CO_NONE);
        }

        if (err != ENOERR)
            break;

        err = read_raw_dentry(disk, pos,NULL,pui1_dent);
        if (err != ENOERR)
            {TR(("read_raw_dentry fail\n"));return err;}
            //diag_dump_buf_with_offset( pui1_dent, 32, pui1_dent );
        if (pui1_dent[0] == 0x00)
        {
            CYG_TRACE0(TDE, "end of dir"); 
			TR(("end of dir"));
            i4_ret = EEOF;
            goto EXIT;
        }
        if (pui1_dent[0] == 0xE5)
        {
            //dir entry deleted
            pos->cluster_pos += 32;
            ui1_ent_num = 0; /* reset long name entry */
            TR(("pui1_dent[0] == 0xE5\n"));
            continue;
        }
        
        GET_BYTE(pui1_dent,ui1_attr,FAT_DENTRY_ATTR);
        GET_BYTE(pui1_dent,ui1_nt_res,FAT_DENTRY_NTRES);
        
#if 0
                if (ui1_attr == FAT_ATTR_VOLUME_ID)
                {
                    ui1_ent_num = 0; /* reset long name entry */
                    continue; /* skip this one */
                }
#endif        
        if (((ui1_attr & FAT_ATTR_LONG_NAME_MASK) == (cyg_uint8)FAT_ATTR_LONG_NAME) ||
            (ui1_ent_num != 0))
        {
            if ((*pui1_dent & 0x40) &&
                ((ui1_attr & FAT_ATTR_LONG_NAME_MASK) == (cyg_uint8)FAT_ATTR_LONG_NAME)) /*(ui1_ent_num == 0)*/
            {
                GET_BYTE(pui1_dent,ui1_chksum,FAT_LDENTRY_CHKSUM);
                GET_BYTE(pui1_dent,ui1_ent_num,FAT_LDENTRY_ORDER);
                ui1_ent_num = (ui1_ent_num& 0x3F) + 1;
                /* The maxium number of the long entries is 20.  */
                if (ui1_ent_num <= 21)
                {
                    //pt_dent->ui1_use_dent = ui1_ent_num; /* including short entry */
                    memset(s_utf16, 0, 2 * (FM_MAX_FILE_LEN + 6));
                }
                else
                {
                    ui1_ent_num = 0;
                    pos->cluster_pos += 32;
                    TR(("second continue\n"));
                    continue;
                }
            }

            if (ui1_ent_num <= 1)
            {
                /* By ChunZhou: Make sure it is short entry */
                if ((ui1_attr & FAT_ATTR_LONG_NAME_MASK) != (cyg_uint8)FAT_ATTR_LONG_NAME)
                {
                    /* [DTV139934] invalid directory entry name */
                    if (((pui1_dent[0] <= 0x20) && (pui1_dent[0] != 0x05)) ||
                        (pui1_dent[0] == 0x22) || 
                        ((pui1_dent[0] >= 0x2A) && (pui1_dent[0] <= 0x2C)) ||
                        (pui1_dent[0] == 0x2E) ||
                        ((pui1_dent[0] >= 0x3A) && (pui1_dent[0] <= 0x3F)) ||
                        ((pui1_dent[0] >= 0x5B) && (pui1_dent[0] <= 0x5D)) ||
                        (pui1_dent[0] == 0x7C))
                    {
                        TR(("3 continue\n"));
                        pos->cluster_pos += 32;
                        ui1_ent_num = 0; /* reset long name entry */
                        continue;        /* invalid directory entry name, skip it */
                    }

                    /* short dir entry */
                    if (ui1_chksum != _fat_longname_chksum(pui1_dent))
                    {
                        /* Skip this long entry */
                        /* By ChunZhou,BUG here:[DTV00137979]
                           "An 8-bit checksum is computed on the name contained in the short directory entry
                           at the time the short and long directory entries are created. All 11 characters 
                           of the name in the short entry are used in the checksum calculation. The check 
                           sum is placed in every long entry. If any of the check sums in the set of long 
                           entries do not agree with the computed checksum of the name contained in the 
                           short entry, then the long entries are treated as orphans."
                           So I should "treat the long entries as orphans", but the short entry still makes
                           sense.
                        */
                        //ui1_ent_num = 0;
                        //TR(("4 continue\n"));
                        diag_printf("WARNING:wrong chksum,work around\n");
                        /* Added by ChunZhou {*/
                        //pos->cluster_pos -= 32;
                        /* Added by ChunZhou }*/
                        //continue;
                    }

                    /*if (_fat_check_dentry(pui1_dent, pt_fs_info) == FALSE)
                    {
                        //Skip this long entry 
                        ui1_ent_num = 0;
                        continue;
                    }*/

                    //_fat_trans_unicode_endian(s_utf16, s_utf16);
                    //TR(("s_utf16=%s\n",(char *)s_utf16));
                    //diag_dump_buf_with_offset( s_utf16, 2 * (FM_MAX_FILE_LEN + 6), s_utf16 );
                    //dentry->ui2_len = (cyg_uint16)strlen((char *)s_utf16);
                    UTF16ToUTF8((UTF16*)s_utf16,
                                (UTF16*)(s_utf16+2*(FM_MAX_FILE_LEN+6)),
                                (UTF8*)dentry->s_utf8,
                                (UTF8*)(dentry->s_utf8+4*(FM_MAX_FILE_LEN+1)));
                    //memcpy( (char *)dentry->s_utf8,(char *)s_utf16, 4*(FM_MAX_FILE_LEN+1));
                    //diag_dump_buf_with_offset( dentry->s_utf8, 2*(FM_MAX_FILE_LEN+1), dentry->s_utf8 );
                    //ASSERT(i4_ret == ENOERR);
                    //TR(("name=%s\n",(char *)dentry->s_utf8));
                    dentry->ui4_name_off = pos->cluster_pos;
                    break;  /* found a long name dir entry */
                }
                else
                {
                    /* Skip this long entry */
                    ui1_ent_num = 0;
                    pos->cluster_pos += 32;
                    TR(("5 continue\n"));
                    continue;
                }
            }
            else
            {
                cyg_uint8 temp_chksum;
                GET_BYTE(pui1_dent,temp_chksum,FAT_LDENTRY_CHKSUM);
                if (ui1_chksum != temp_chksum)
                {
                    /* Skip remaining long entries */
                    pos->cluster_pos += ((ui1_ent_num - 1) * 32);
                    ui1_ent_num = 0;
                    TR(("6 continue\n"));
                    continue;
                }

                ui2_str_off = (cyg_uint16)((ui1_ent_num - 2) * 26); /* 13 unicode characters */
                TR(("ui1_ent_num =%d\n",ui1_ent_num));
                /*
                 *  The name is currently based on little endian,
                 *  UCS2 encoding.
                 */
                memcpy(&s_utf16[ui2_str_off],
                         FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME1),
                         10);
                memcpy(&s_utf16[ui2_str_off + 10],
                         FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME2),
                         12);
                memcpy(&s_utf16[ui2_str_off + 22],
                         FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME3),
                         4);
                //diag_dump_buf_with_offset( &s_utf16[ui2_str_off], 26, &s_utf16[ui2_str_off] );
                TR(("s_utf16=%s\n",(char *)&s_utf16[ui2_str_off]));
            }

            ui1_ent_num--;
            pos->cluster_pos += 32;
            TR(("7 continue\n"));
            continue;
        }
        else if ( (ui1_attr & FAT_ATTR_VOLUME_ID))
        {
            ui1_ent_num = 0;
            pos->cluster_pos += 32;
            continue;
        }

        /*if ( (FALSE == IS_XENTRY_MOUNT_ROOT(pt_xent) )
            && pui1_dent[0] == 0x2E
            && pui1_dent[2] == 0x20 && pui1_dent[3] == 0x20
            && pui1_dent[4] == 0x20 && pui1_dent[5] == 0x20
            && pui1_dent[6] == 0x20 && pui1_dent[7] == 0x20
            && pui1_dent[8] == 0x20 && pui1_dent[9] == 0x20
            && pui1_dent[10] == 0x20)
        {
            // "." or ".." in sub-directory 
            ui1_ent_num = 0;
        }*/
        if (((pui1_dent[0] <= 0x20) && (pui1_dent[0] != 0x05)) ||
            (pui1_dent[0] == 0x22) || 
            ((pui1_dent[0] >= 0x2A) && (pui1_dent[0] <= 0x2C)) ||
            (pui1_dent[0] == 0x2E) ||
            ((pui1_dent[0] >= 0x3A) && (pui1_dent[0] <= 0x3F)) ||
            ((pui1_dent[0] >= 0x5B) && (pui1_dent[0] <= 0x5D)) ||
            (pui1_dent[0] == 0x7C))
        {
            ui1_ent_num = 0; /* reset long name entry */
            pos->cluster_pos += 32;
            TR(("8 continue\n"));
            continue;        /* invalid directory entry name, skip it */
        }
        //pt_dent->ui1_use_dent = 1;
        
        _fat_short_name_strcpy(dentry->s_utf8, pui1_dent, ui1_nt_res);
        dentry->ui2_len = (cyg_uint16)strlen((const char *)dentry->s_utf8);
        dentry->ui4_name_off = pos->cluster_pos;

        /*if (_fat_check_dentry(pui1_dent, pt_fs_info) == FALSE)
        {
            //Skip this entry 
            ui1_ent_num = 0;
            continue;
        }*/

        break; /* found a short name dir entry */
    }    
EXIT:

    if (i4_ret == ENOERR)
    {   
        _fat_get_dentry(dentry, pui1_dent);       
    }
    TR(("i4_ret = %d\n",i4_ret));
  free( s_utf16);

    return i4_ret;
}

// -------------------------------------------------------------------------
// get_free_raw_dentry()
// Gets free dir entry slot searching from given position extending the
// directory if needed. If an deleated entry is found it is reused.

static int
get_free_raw_dentry(fatfs_disk_t     *disk, 
                    cyg_uint16 name_len,
                    fatfs_data_pos_t *pos)
{
    fat_raw_dir_entry_t raw_dentry;
    fatfs_data_pos_t    cpos;
    int                 err = ENOERR;
    cyg_uint8   ui1_use_dent = 0;
    cyg_uint8   ui1_free = 0;
    cpos = *pos;  

    // If we are reading the root dir on FAT32 we have
    // to correct the position to the root dir cluster
    if (FATFS_FAT32 == disk->fat_type && 0 == cpos.cluster)
        cpos.cluster = disk->fat_root_dir_cluster;
 
    CYG_TRACE3(TDE, "cluster=%d snum=%d pos=%d", 
                    pos->cluster, pos->cluster_snum, pos->cluster_pos);
    TR(("get_free_raw_dentry:cluster=%d snum=%d pos=%d\n", 
                    pos->cluster, pos->cluster_snum, pos->cluster_pos));
    
    ui1_use_dent = (cyg_uint8)((name_len / 13) + 1 + (((name_len % 13) == 0) ? 0 : 1));
    
    while (ui1_free < ui1_use_dent)
    {
        // FAT12/16 root dir check
        if (0 == cpos.cluster) 
        {
            if (cpos.cluster_pos >= disk->fat_root_dir_size)
                err = ENOSPC;
        }
        else
        { 
            // Change cluster if needed
            if (cpos.cluster_pos >= disk->cluster_size)
            {
                //ui1_free = 0;
                //TODO:how to do with long file name when change cluster
                err = get_next_cluster(disk, &cpos, CO_EXTEND | CO_ERASE_NEW);
            }
        }

        if (err != ENOERR)
            return err;

        err = read_raw_dentry(disk, &cpos, &raw_dentry,NULL);
        if (err != ENOERR)
            return err;
        cpos.cluster_pos += DENTRY_SIZE;
        
        if (DENTRY_IS_DELETED(&raw_dentry) || DENTRY_IS_ZERO(&raw_dentry))
        {
            CYG_TRACE3(TDE, "deleted dentry at cluster=%d snum=%d pos=%d",
                            cpos.cluster, cpos.cluster_snum, cpos.cluster_pos);
            TR(("deleted or empty dentry at cluster=%d snum=%d pos=%d\n",
                            cpos.cluster, cpos.cluster_snum, cpos.cluster_pos));

            ui1_free++;
            
            //*pos = cpos;
            //return ENOERR;
        }
        else
        {
            ui1_free = 0;
        }
       
        
    }
    //if(cpos.cluster == pos->cluster)
        cpos.cluster_pos -= (ui1_free)*DENTRY_SIZE;
    *pos = cpos;
    //diag_printf("ui1_free=%d  pos->cluster_pos=%d\n",ui1_free,pos->cluster_pos);
    return ENOERR;
}
 
// -------------------------------------------------------------------------
// raw_to_dentry()
// Converts raw FAT dir entry to dir entry. 
 
static void
raw_to_dentry(fat_raw_dir_entry_t *raw_dentry,
              fatfs_data_pos_t    *raw_dentry_pos,
              fatfs_dir_entry_t   *dentry)
{
    get_raw_dentry_filename(raw_dentry, dentry->filename);
    memcpy(dentry->short_name,raw_dentry->name,8);
    memcpy(dentry->short_name+8,raw_dentry->ext,3);
    dentry->short_name[11] = 0;
    if (DENTRY_IS_DIR(raw_dentry))
        dentry->mode = __stat_mode_DIR;
    else
        dentry->mode = __stat_mode_REG;
    
//#ifdef CYGCFG_FS_FAT_USE_ATTRIBUTES
    dentry->attrib = raw_dentry->attr;
//#endif // CYGCFG_FS_FAT_USE_ATTRIBUTES

    date_dos2unix(raw_dentry->crt_time, raw_dentry->crt_date, &dentry->ctime);
    date_dos2unix(0,                    raw_dentry->acc_date, &dentry->atime);
    date_dos2unix(raw_dentry->wrt_time, raw_dentry->wrt_date, &dentry->mtime);
    
    dentry->size       = raw_dentry->size;
    dentry->priv_data  = raw_dentry->nt_reserved;
    dentry->cluster    = raw_dentry->cluster | (raw_dentry->cluster_HI << 16);
    dentry->disk_pos   = *raw_dentry_pos;
}

// -------------------------------------------------------------------------
// dentry_to_raw()
// Converts dir entry to raw FAT dir entry. 
 
static void
dentry_to_raw(fatfs_dir_entry_t *dentry, fat_raw_dir_entry_t *raw_dentry)
{
    set_raw_dentry_filename(raw_dentry, dentry->short_name, 0);
    memcpy(raw_dentry->s_utf8,dentry->filename,4*(FM_MAX_FILE_LEN+1));
    if (__stat_mode_DIR == dentry->mode)
        raw_dentry->attr = S_FATFS_DIR;
    else
        raw_dentry->attr = S_FATFS_ARCHIVE;
//#ifdef CYGCFG_FS_FAT_USE_ATTRIBUTES
    raw_dentry->attr = dentry->attrib;
//#endif // CYGCFG_FS_FAT_USE_ATTRIBUTES

        
    date_unix2dos(dentry->ctime, &raw_dentry->crt_time, &raw_dentry->crt_date);
    date_unix2dos(dentry->atime, NULL,                  &raw_dentry->acc_date);
    date_unix2dos(dentry->mtime, &raw_dentry->wrt_time, &raw_dentry->wrt_date);
    
    raw_dentry->crt_sec_100 = 0; //FIXME
    raw_dentry->size        = dentry->size;
    raw_dentry->nt_reserved = dentry->priv_data;
    raw_dentry->cluster     = dentry->cluster & 0xFFFF;
    raw_dentry->cluster_HI  = dentry->cluster >> 16;
}

//==========================================================================
// FAT data functions 

// -------------------------------------------------------------------------
// read_data()
// Reads data from given position. 
 
static int
read_data(fatfs_disk_t     *disk,
          void             *data,
          cyg_uint32       *len,
          fatfs_data_pos_t *pos)
{
    cyg_uint8   *buf   = (cyg_uint8 *) data;
    cyg_uint32   size  = *len;
    int          err   = ENOERR;

    CYG_TRACE4(TDO, "len=%d cluster=%d snum=%d pos=%d",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos);
    TR(("ENTER read_data\n"));
    while (size > 0)
    {
        cyg_uint32 csize;

        // Check if we are still inside current cluster
        if (pos->cluster_pos >= disk->cluster_size)
        {
            // Get next cluster of file 
            err = get_next_cluster(disk, pos, CO_NONE);
            if (err != ENOERR)
                goto out;
        }
        
        // Adjust the data chunk size to be read to the cluster boundary
        if (size > (disk->cluster_size - pos->cluster_pos))
            csize = disk->cluster_size - pos->cluster_pos;
        else
            csize = size;

        CYG_TRACE4(TDO, "-- len=%d cluster=%d snum=%d pos=%d",
                        csize, pos->cluster, pos->cluster_snum,
                        pos->cluster_pos);
        /*TR(("-- len=%d cluster=%d snum=%d pos=%d",
                        csize, pos->cluster, pos->cluster_snum,
                        pos->cluster_pos));*/
        err = disk_cluster_read(disk, (void*)buf, &csize, 
                                pos->cluster, pos->cluster_pos);
        if (err != ENOERR)
            goto out;

        // Adjust running variables

        buf              += csize;
        size             -= csize; 
        pos->cluster_pos += csize;
    }
    
out:
    *len -= size;

    CYG_TRACE1(TDO, "total len=%d", *len);

    return err;
}

// write_data_block()
// Writes data to given position. 
 
static int
write_data_block(fatfs_disk_t     *disk,
           void             *data,
           cyg_uint32       *len,
           fatfs_data_pos_t *pos)
{
    cyg_uint8   *buf   = (cyg_uint8 *) data;
    cyg_uint32   size  = *len;
    int          err   = ENOERR;

    CYG_TRACE4(TDO, "len=%d cluster=%d snum=%d pos=%d",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos);
    TR(("write_data len=%d cluster=%d snum=%d pos=%d\n",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos));
    while (size > 0)
    {
        cyg_uint32 csize;

        // Check if we are still inside current cluster
        if (pos->cluster_pos >= disk->cluster_size)
        {
            // Get next cluster of file, if at the last 
            // cluster try to extend the cluster chain 
            err = get_next_cluster(disk, pos, CO_EXTEND);
            if (err != ENOERR)
                {diag_printf("get_next_cluster fail\n");goto out;}
        }
        
        // Adjust the data chunk size to be read to the cluster boundary
        if (size > (disk->cluster_size - pos->cluster_pos))
            csize = disk->cluster_size - pos->cluster_pos;
        else
            csize = size;
 
        CYG_TRACE4(TDO, "-- len=%d cluster=%d snum=%d pos=%d",
                        csize, pos->cluster, pos->cluster_snum, 
                        pos->cluster_pos);
        TR(("-- len=%d cluster=%d snum=%d pos=%d\n",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos));
        err = disk_cluster_write_block(disk, (void*)buf, &csize, 
                                 pos->cluster, pos->cluster_pos);
        if (err != ENOERR)
            {diag_printf("disk_cluster_write fail\n");goto out;}

        // Adjust running variables
        
        buf              += csize;
        size             -= csize;
        pos->cluster_pos += csize;
    }
    
out:
    *len -= size;

    CYG_TRACE1(TDO, "total len=%d", *len);

    return err;
}

// -------------------------------------------------------------------------
// write_data()
// Writes data to given position. 
 
static int
write_data(fatfs_disk_t     *disk,
           void             *data,
           cyg_uint32       *len,
           fatfs_data_pos_t *pos)
{
    cyg_uint8   *buf   = (cyg_uint8 *) data;
    cyg_uint32   size  = *len;
    int          err   = ENOERR;

    CYG_TRACE4(TDO, "len=%d cluster=%d snum=%d pos=%d",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos);
    TR(("write_data len=%d cluster=%d snum=%d pos=%d\n",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos));
    while (size > 0)
    {
        cyg_uint32 csize;

        // Check if we are still inside current cluster
        if (pos->cluster_pos >= disk->cluster_size)
        {
            // Get next cluster of file, if at the last 
            // cluster try to extend the cluster chain 
            err = get_next_cluster(disk, pos, CO_EXTEND);
            if (err != ENOERR)
                {diag_printf("get_next_cluster fail\n");goto out;}
        }
        
        // Adjust the data chunk size to be read to the cluster boundary
        if (size > (disk->cluster_size - pos->cluster_pos))
            csize = disk->cluster_size - pos->cluster_pos;
        else
            csize = size;
 
        CYG_TRACE4(TDO, "-- len=%d cluster=%d snum=%d pos=%d",
                        csize, pos->cluster, pos->cluster_snum, 
                        pos->cluster_pos);
        TR(("-- len=%d cluster=%d snum=%d pos=%d\n",
                    *len, pos->cluster, pos->cluster_snum, 
                    pos->cluster_pos));
        err = disk_cluster_write(disk, (void*)buf, &csize, 
                                 pos->cluster, pos->cluster_pos);
        if (err != ENOERR)
            {diag_printf("disk_cluster_write fail\n");goto out;}

        // Adjust running variables
        
        buf              += csize;
        size             -= csize;
        pos->cluster_pos += csize;
    }
    
out:
    *len -= size;

    CYG_TRACE1(TDO, "total len=%d", *len);

    return err;
}

//==========================================================================
// Misc functions 

// -------------------------------------------------------------------------
// init_dir_entry()
// Initializes attributes of a new dir entry. 
 
static void
init_dir_entry(fatfs_dir_entry_t *dentry, 
               const char        *name, 
               int                namelen,
               mode_t             mode,
               cyg_uint32         parent_cluster, 
               cyg_uint32         first_cluster, 
               fatfs_data_pos_t  *pos)
{
    if (0 == namelen)
        namelen = 12;
    if((name == ".")||(name == ".."))
    {
        strncpy(dentry->short_name, name, namelen);
        dentry->short_name[namelen] = '\0';
    }
    memcpy(dentry->filename, name, namelen);
    dentry->filename[namelen] = '\0';
    
    dentry->mode  = mode;

//#ifdef CYGCFG_FS_FAT_USE_ATTRIBUTES
    if (S_ISDIR(dentry->mode))
        dentry->attrib = S_FATFS_DIR;
    else
        dentry->attrib = S_FATFS_ARCHIVE;
//#endif // CYGCFG_FS_FAT_USE_ATTRIBUTES

    dentry->ctime = 
    dentry->atime =
    dentry->mtime = cyg_timestamp();

    dentry->priv_data      = 0;
    dentry->size           = 0;
    dentry->cluster        = first_cluster;
    dentry->parent_cluster = parent_cluster;
    dentry->disk_pos       = *pos;
}

// -------------------------------------------------------------------------
// is_root_dir_entry()
// Check if the given dir entry is the root dir entry. 
 
static __inline__ bool
is_root_dir_entry(fatfs_dir_entry_t *dentry)
{
    return ('\0' == dentry->filename[0] && 0 == dentry->cluster);
}

//==========================================================================
//==========================================================================
// Exported functions 

// -------------------------------------------------------------------------
// fatfs_init()
// Gets disk info. 
 
int
fatfs_init(fatfs_disk_t *disk)
{
    cyg_uint32        sec_num, sec_per_fat, root_dir_sec_num;
    cyg_uint32        data_sec_num, data_clu_num;
    fat_boot_record_t boot_rec;
    int               err;

    CYG_CHECK_DATA_PTRC(disk);
    err = read_boot_record(disk, &boot_rec);
    if (err != ENOERR)    
        {TR(("read_boot_record ERROR\n"));return err;}

    // Check some known boot record values
    if (!(0x29 == boot_rec.ext_sig ||
          0x28 == boot_rec.ext_sig)    ||
        0x55 != boot_rec.exe_marker[0] ||
        0xAA != boot_rec.exe_marker[1])
        {TR(("boot_rec.ext_sig ERROR\n"));return EINVAL;}

    // Sector and cluster sizes 
    disk->sector_size       = boot_rec.bytes_per_sec;
    disk->sector_size_log2  = get_val_log2(disk->sector_size);
    disk->cluster_size      = boot_rec.bytes_per_sec * boot_rec.sec_per_clu;
    disk->cluster_size_log2 = get_val_log2(disk->cluster_size);
    sec_per_cluster = boot_rec.sec_per_clu;
    sector_size = disk->sector_size;
    // Sector and cluster size should always be a power of 2
    if (0 == disk->sector_size_log2 || 0 == disk->cluster_size_log2)
        {TR(("sector_size_log2 ERROR\n"));return EINVAL;}

    // Determine number of sectors
    if (boot_rec.sec_num_32 != 0)
        sec_num = boot_rec.sec_num_32;
    else
        sec_num = boot_rec.sec_num;

    // Determine number of sectors per fat
    if (boot_rec.sec_per_fat != 0)
        sec_per_fat = boot_rec.sec_per_fat;
    else
        sec_per_fat = boot_rec.sec_per_fat_32;
    
    // Number of sectors used by root directory 
    root_dir_sec_num = ((boot_rec.max_root_dents * DENTRY_SIZE) +
                        (boot_rec.bytes_per_sec - 1)) / boot_rec.bytes_per_sec;
        
    // Number of data sectors
    data_sec_num = sec_num - (boot_rec.res_sec_num + 
        (boot_rec.fat_tbls_num * sec_per_fat) + root_dir_sec_num);
    
    // Number of data clusters
    data_clu_num = data_sec_num / boot_rec.sec_per_clu;

    // FAT table size and position
    disk->fat_tbl_pos   = boot_rec.bytes_per_sec * boot_rec.res_sec_num;
    disk->fat_tbl_size  = boot_rec.bytes_per_sec * sec_per_fat;   
    disk->fat_tbl_nents = data_clu_num + 2;
    disk->fat_tbls_num  = boot_rec.fat_tbls_num;

    // Determine the type of FAT based on number of data clusters
    if (data_clu_num < 4085)
        disk->fat_type  = FATFS_FAT12;
    else if (data_clu_num < 65525) 
        disk->fat_type  = FATFS_FAT16;
    else 
        disk->fat_type  = FATFS_FAT32;

    // Determine root dir and data positions
    if (FATFS_FAT32 != disk->fat_type)
    {
        disk->fat_root_dir_pos     = disk->fat_tbl_pos + 
                                     disk->fat_tbls_num * disk->fat_tbl_size;
        disk->fat_root_dir_size    = boot_rec.max_root_dents * DENTRY_SIZE;
        disk->fat_root_dir_nents   = boot_rec.max_root_dents;
        disk->fat_root_dir_cluster = 0;
        disk->fat_data_pos         = disk->fat_root_dir_pos + 
                                     disk->fat_root_dir_size;
    }
    else
    {
        disk->fat_root_dir_pos     = 0;
        disk->fat_root_dir_size    = 0;
        disk->fat_root_dir_nents   = 0;
        disk->fat_root_dir_cluster = boot_rec.root_cluster; 
        disk->fat_data_pos         = disk->fat_tbl_pos +
                                     disk->fat_tbls_num * disk->fat_tbl_size;
    }
    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_get_root_dir_entry()
// Gets root dir entry. 

void
fatfs_get_root_dir_entry(fatfs_disk_t *disk, fatfs_dir_entry_t *dentry)
{
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dentry);
    
    dentry->mode           = __stat_mode_DIR;
//#ifdef CYGCFG_FS_FAT_USE_ATTRIBUTES
    dentry->attrib         = S_FATFS_DIR;
//#endif // CYGCFG_FS_FAT_USE_ATTRIBUTES
    dentry->size           = disk->fat_root_dir_size;
    dentry->ctime          = 0;
    dentry->atime          = 0;
    dentry->mtime          = 0;
    dentry->filename[0]    = '\0';
    dentry->short_name[0]    = '\0';
    dentry->cluster        = 0;
    dentry->parent_cluster = 0;

    dentry->disk_pos.cluster      = 0;
    dentry->disk_pos.cluster_snum = 0;
    dentry->disk_pos.cluster_pos  = 0;
}

// -------------------------------------------------------------------------
// fatfs_get_disk_usage()
// Gets disk space.

int
fatfs_get_disk_usage(fatfs_disk_t *disk,
                     cyg_uint32   *total_clusters,
                     cyg_uint32   *free_clusters)
{
    cyg_uint32 c, nfree, tentry;
    int        err;

    nfree = 0;
    for (c = 2; c < disk->fat_tbl_nents; c++)
    {
        err = read_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        if (TENTRY_FREE == get_tentry_type(disk, tentry))
            nfree++;
    }

    *total_clusters = disk->fat_tbl_nents - 2;
    *free_clusters  = nfree;
  
    return ENOERR;
}


// -------------------------------------------------------------------------
// fatfs_read_dir_entry()
// Reads dir entry at given position.
// If there is no dir entry at given position the next closest is returned 
// and the position is updated. If EEOF error is returned, then there are 
// no more dir entries in given dir. 

int
fatfs_read_dir_entry(fatfs_disk_t      *disk,
                     fatfs_dir_entry_t *dir,
                     fatfs_data_pos_t  *pos,
                     fatfs_dir_entry_t *dentry)
{
    fat_raw_dir_entry_t raw_dentry;
    int                 err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(pos);
    CYG_CHECK_DATA_PTRC(dentry);
 
    err = read_next_raw_dentry(disk, pos, &raw_dentry);
    if (err != ENOERR)
       return err;

    raw_to_dentry(&raw_dentry, pos, dentry);
	/*TR(("dentry name=%s cluster=%d\n",
                            raw_dentry.name, (raw_dentry.cluster_HI<<16)+raw_dentry.cluster));*/
    dentry->parent_cluster = dir->cluster;

    // Increment position for next call
    pos->cluster_pos += DENTRY_SIZE;
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_initpos()
// Initializes position to the start of the given file.

int
fatfs_initpos(fatfs_disk_t      *disk, 
              fatfs_dir_entry_t *file,
              fatfs_data_pos_t  *pos)
{
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    CYG_CHECK_DATA_PTRC(pos);
    
    pos->cluster      = file->cluster;
    pos->cluster_snum = 0;
    pos->cluster_pos  = 0;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_setpos()
// Sets the file position from offset.

int
fatfs_setpos(fatfs_disk_t      *disk, 
             fatfs_dir_entry_t *file,
             fatfs_data_pos_t  *pos,
             cyg_uint32         offset)
{
    int err;
    
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    CYG_CHECK_DATA_PTRC(pos);
    
    err = get_position_from_off(disk, file->cluster, offset, pos, CO_NONE);

    if (EEOF == err && offset == file->size)
        return ENOERR;
    else
        return err;
}

// -------------------------------------------------------------------------
// fatfs_getpos()
// Gets the file offset from position.

cyg_uint32 
fatfs_getpos(fatfs_disk_t      *disk,
             fatfs_dir_entry_t *file,
             fatfs_data_pos_t  *pos)
{
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    CYG_CHECK_DATA_PTRC(pos);
    
    return (pos->cluster_snum << disk->cluster_size_log2) + pos->cluster_pos;
}

/* iso8859-1 -> cp850 */

/*
 * 0 - character disallowed in long file name.
 * 1 - character should be replaced by '_' in DOS file name, 
 *     and generation number inserted.
 * 2 - character ('.' and ' ') should be skipped in DOS file name,
 *     and generation number inserted.
 */
static cyg_uint8 ac_unicode_to_oem[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, (cyg_uint8)'_', 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_', 0x2D, 0x2E, (cyg_uint8)'_', 
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_',
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, (cyg_uint8)'_', (cyg_uint8)'_', (cyg_uint8)'_', 0x5E, 0x5F,
    0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x7B, (cyg_uint8)'_', 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/*-----------------------------------------------------------------------------
 * Name: _fat_utf8_to_oem
 *
 * Description: Convert illegal characters to '-'.
 *
 * Inputs:      ps_utf8
 *                  String to be converted.
 *
 * Outputs:     ps_oem
 *                  Buffer for new string. It must be as large as ps_utf8 or
 *                  larger than ps_utf8.
 *
 * Returns: true
 *              The string is lossy conversion.
 *          FALSE
 *              The string is successful conversion.
 ----------------------------------------------------------------------------*/
static bool _fat_utf8_to_oem(
    cyg_uint8 *ps_oem,
    cyg_uint8 *ps_utf8)
{
    cyg_uint8 *pc_oem, *pc_utf8;
    bool b_lossy = false;

    for (pc_oem = ps_oem, pc_utf8 = ps_utf8;
         *pc_utf8 != 0;
         pc_oem++, pc_utf8++)
    {
        *pc_oem = ac_unicode_to_oem[*pc_utf8];
        if (*pc_oem == '_')
        {
            b_lossy = true;
        }
    }

    return b_lossy;
}

/*-----------------------------------------------------------------------------
 * Name: _fat_utf8_to_upper
 *
 * Description: Convert a string to upper case. Only characters from 'a' to 'z'
 *              will be converted.
 *
 * Inputs:      ps_lname
 *                  String to be converted.
 *
 * Outputs:     ps_uname
 *                  Buffer for new string. It must be as large as ps_lname or
 *                  larger than ps_lname.
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static void _fat_utf8_to_upper(
    cyg_uint8       *ps_uname,
    const cyg_uint8 *ps_lname)
{
    cyg_uint8       *pc_uname;
    const cyg_uint8 *pc_lname;

    for (pc_uname = ps_uname, pc_lname = ps_lname;
         *pc_lname != 0;
         pc_uname++, pc_lname++)
    {
        *pc_uname = ((*pc_lname >= 'a') && (*pc_lname <= 'z')) ?
                     *pc_lname + ('Z' - 'z') :
                     *pc_lname;
    }
    *pc_uname = 0;
}

/*-----------------------------------------------------------------------------
 * Name: _fat_is_not_dos_name
 *
 * Description: Check if the given file name is in dos 8.3 format.
 *
 * Inputs:  ps_utf8
 *              A null-terminated string.
 *
 * Outputs: -
 *
 * Returns: true
 *              The string conforms to 8.3 format.
 *          FALSE
 *              The string does not conform to 8.3 format.
 ----------------------------------------------------------------------------*/
static bool _fat_is_dos_format(
    const cyg_uint8 *ps_utf8)
{
    const cyg_uint8 *pc;
    bool  b_has_dot   = false;
    cyg_uint32 i4_name_len = 0;
    cyg_uint32 i4_ext_len  = 0;

    //ASSERT(ps_utf8 != NULL);

    for (pc = ps_utf8; *pc != '\0'; pc++)
    {
        if (*pc == '.')
        {
            if (b_has_dot == true)
            {
                return false;
            }

            b_has_dot = true;
        }
        else
        {
            if (b_has_dot == false)
            {
                i4_name_len++;
            }
            else
            {
                i4_ext_len++;
            }
            if ((i4_name_len > 8) || (i4_ext_len > 3))
            {
                return false;
            }
        }
    }

    return true;
}
/*-----------------------------------------------------------------------------
 * Name: _fat_copy_dos_name
 *
 * Description: Put a 8.3 name to a 11 bytes of memory space in the directory
 *              entry format.
 *
 * Inputs:      ps_dos_name
 *                  11 bytes buffer.
 *
 * Outputs:     ps_uname
 *                  DOS name.
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static void _fat_copy_dos_name(
    cyg_uint8       *ps_dos_name,
    const cyg_uint8 *ps_uname)
{
    cyg_uint8 *pc_dname;
    const cyg_uint8 *pc_uname;
    const cyg_uint8 *pc_fst_period = NULL;
    const cyg_uint8 *pc_period = NULL;

    memset(ps_dos_name, (cyg_uint8)' ', 11);
    
    pc_dname = ps_dos_name;
    pc_uname = ps_uname;

    while ((*pc_uname != 0) && (*pc_uname != '.') && (pc_dname - ps_dos_name < 8))
    {
        *pc_dname++ = *pc_uname++;
    }
    while (*pc_uname != 0)
    {
        if (*pc_uname == '.')
        {
            if (pc_fst_period == NULL)
            {
                pc_fst_period = pc_uname;
            }
            pc_period = pc_uname;
        }
        pc_uname++;
    }
    if (pc_period != NULL)
    {
        pc_uname = pc_fst_period;
        while ((pc_uname != pc_period) && ((pc_dname - ps_dos_name) < 8))
        {
            if (*pc_uname != '.')
            {
                *pc_dname++ = *pc_uname;
            }
            pc_uname++;
        }

        pc_uname = pc_period + 1;
        pc_dname = ps_dos_name + 8;
        while ((*pc_uname != 0) && ((pc_dname - ps_dos_name) < 11))
        {
            *pc_dname++ = *pc_uname++;
        }
    }
}
/*-----------------------------------------------------------------------------
 * Name: _fat_strip_space_and_period
 *
 * Description: Strip all leading and embedded spaces from the given string.
 *              And strip all leading periods from it, too.
 *
 * Inputs:      ps_org
 *                  Original string.
 *
 * Outputs:     ps_new
 *                  New string
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static void _fat_strip_space_and_period(
    cyg_uint8 *ps_new,
    cyg_uint8 *ps_org)
{
    cyg_uint8 *pc_new, *pc_org;

    for (pc_new = ps_new, pc_org = ps_org;
         *pc_org != 0;
         pc_org++)
    {
        if (*pc_org != ' ')
        {
            *pc_new++ = *pc_org;
        }
    }
    *pc_new = 0;

    for (pc_org = ps_new; *pc_org == '.'; pc_org++)
    {
        /* strip leading periods */
    }

    for (pc_new = ps_new; *pc_org != 0; pc_org++)
    {
        *pc_new++ = *pc_org;
    }
}
/*-----------------------------------------------------------------------------
 * Name: _fat_generate_numeric_tail
 *
 * Description: Generate file name in 8.3 format with numeric tail. To optimize
 *              the performance, we simply select the maximum number to append.
 *              That is, for example, if SAMPLE~1.TXT and SAMPLE~3.TXT exist,
 *              the new file name will be SAMPLE~4.TXT instead of SAMPLE~2.TXT.
 *              It does cause some problem, but it rarely happens since the 
 *              number ranges from 1 to 999999.
 *
 * Inputs:  pt_xent
 *              Reference to the xentry of the parent directory that will
 *              contain this new file.
 *          pui1_short_name
 *              The new file name string without numeric tail.
 *
 * Outputs: pui1_short_name
 *              The new file name with numeric tail.
 *
 * Returns: ENOERR
 *              Success.
 ----------------------------------------------------------------------------*/
static cyg_uint32 _fat_generate_numeric_tail(
    fatfs_disk_t      *disk,
    fatfs_dir_entry_t *pt_xent,
    cyg_uint8       *pui1_short_name)
{
    //FM_RW_ARG_T t_io;
    cyg_uint8       pui1_dent[32];
    cyg_uint8       ui1_attr;
    cyg_uint64      ui8_offset;
    cyg_uint32       i4_new_num;
    cyg_uint32       i4_can_num;
    cyg_uint32       i;
    cyg_uint32       j;
    cyg_uint32       i4_ret;

    //ASSERT(pt_xent != NULL);
    //ASSERT(pui1_short_name != NULL);
    fatfs_data_pos_t pos;
    ui8_offset = 0;
    i4_can_num = 1;
    
    pos.cluster      = pt_xent->parent_cluster;
    pos.cluster_snum = 0;
    pos.cluster_pos  = 0;
    if (FATFS_FAT32 == disk->fat_type && 0 == pos.cluster)
        pos.cluster = disk->fat_root_dir_cluster;
    while (1)
    {
       /* t_io.pt_xent        = pt_xent;
        t_io.pv_data        = pui1_dent;
        t_io.ui4_count      = 32;
        t_io.ui8_offset     = ui8_offset;
        t_io.ui4_done_bytes = 0;
        t_io.ui2_flag       = FM_IO_SYNC;
        t_io.h_bin_sema     = pt_xent->pt_mount->h_sync_sema;*/

        //i4_ret = _fat_rw_buf(&t_io, true, true);
        ;
        i4_ret = read_raw_dentry(disk,&pos,NULL,pui1_dent);
        if (i4_ret != ENOERR)
            {TR(("read_raw_dentry fail\n"));break;}
        
       // ASSERT(t_io.ui4_done_bytes == 32);
        pos.cluster_pos += 32;

        if (pui1_dent[0] == 0x00)
        {
            break;
        }
        
        ui1_attr = FAT_GET_VAL8(pui1_dent, FAT_DENTRY_ATTR);

        if ((pui1_dent[0] == 0xE5) ||
            (ui1_attr == FAT_ATTR_VOLUME_ID) ||
            ((ui1_attr & FAT_ATTR_LONG_NAME_MASK) == (cyg_uint8)(FAT_ATTR_LONG_NAME)))
        {
            continue;
        }

        if (pui1_dent[8] == pui1_short_name[8] &&
            pui1_dent[9] == pui1_short_name[9] &&
            pui1_dent[10] == pui1_short_name[10])
        {
            i4_new_num = 0;

            for (i = 0; i < 7; i++)
            {
                if (pui1_dent[i] == '~')
                {
                    for (j = i + 1; j < 8; j++)
                    {
                        i4_new_num = i4_new_num * 10 + (cyg_int32)(pui1_dent[j] - '0');
                    }
                    if (i4_new_num >= i4_can_num)
                    {
                        i4_can_num = i4_new_num + 1;
                    }

                    break;
                }

                if (pui1_dent[i] != pui1_short_name[i])
                {
                    break;
                }
            }
        }
    }

    //ASSERT((i4_can_num >= 1) && (i4_can_num <= 999999));
    if (i4_ret == 0)
    {
        for (i = 7; i > 1; i--)
        {
            pui1_short_name[i] = (cyg_uint8)'0' + (cyg_uint8)(i4_can_num % 10);
            i4_can_num /= 10;
            if (i4_can_num == 0)
            {
                i--;
                break;
            }
        }
        pui1_short_name[i] = (cyg_uint8)'~';
    }

    return i4_ret;
}

/*-----------------------------------------------------------------------------

----------------------------------------------------------------------------*/

static void fatfs_generate_dos_name(
    fatfs_disk_t      *disk,
    fatfs_dir_entry_t     *pt_prnt_xent,
    cyg_uint8           *ps_dos_name,
    const cyg_uint8     *ps_utf8)
{
    cyg_uint8 *ps_uname;
    bool b_lossy_conv;
    
    //ASSERT(pt_prnt_xent != NULL);
    //ASSERT(ps_dos_name != NULL);
    //ASSERT(ps_utf8 != NULL);
    //diag_printf("ps_utf8=%s\n",(char*)ps_utf8);
    ps_uname = malloc(4 * (FM_MAX_FILE_LEN + 1));
    if (ps_uname == NULL)
    {
        return ;
    }
    
    _fat_utf8_to_upper(ps_uname, ps_utf8);
    b_lossy_conv = _fat_utf8_to_oem(ps_uname, ps_uname);
    //diag_printf("ps_uname=%s lossy_conv=%d\n",(char*)ps_uname,b_lossy_conv);
    if (!b_lossy_conv)
    {
        b_lossy_conv = !_fat_is_dos_format(ps_utf8);
    }
    _fat_strip_space_and_period(ps_uname, ps_uname);
    //diag_printf("ps_uname=%s\n",(char*)ps_uname);
    _fat_copy_dos_name(ps_dos_name, ps_uname);
    //diag_printf("ps_dos_name=%s\n",(char*)ps_dos_name);
    ps_dos_name[11] = 0;
    if (b_lossy_conv == true)
    {
        _fat_generate_numeric_tail(disk,pt_prnt_xent, ps_dos_name);
    }
    //diag_printf("ps_dos_name=%s\n",(char*)ps_dos_name);
    free(ps_uname);    
}

/*-----------------------------------------------------------------------------
 * Name: _fat_set_dentry
 *
 * Description: Convert from FAT_DENTRY_T structure to on-disk format directory
 *              entry.
 *
 * Inputs:  pt_dent
 *              Reference to a FAT_DENTRY_T object.
 *
 * Outputs: pui1_dent
 *              Buffer of on disk raw data of a directory entry.
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
static void _fat_set_dentry(
    cyg_uint8           *pui1_dent,
    fat_raw_dir_entry_t    *pt_dent)
{
    //ASSERT(pui1_dent != NULL);
    //ASSERT(pt_dent != NULL);

    SET_BYTES(pui1_dent, pt_dent->name,     8, 0x00);
    SET_BYTES(pui1_dent, pt_dent->ext,      3, 0x08);
    SET_BYTE(pui1_dent,  pt_dent->attr,        0x0B);
    SET_BYTE(pui1_dent,  pt_dent->nt_reserved, 0x0C);
    SET_BYTE(pui1_dent,  pt_dent->crt_sec_100, 0x0D);
    SET_WORD(pui1_dent,  pt_dent->crt_time,    0x0E);
    SET_WORD(pui1_dent,  pt_dent->crt_date,    0x10);
    SET_WORD(pui1_dent,  pt_dent->acc_date,    0x12);
    SET_WORD(pui1_dent,  pt_dent->cluster_HI,  0x14);
    SET_WORD(pui1_dent,  pt_dent->wrt_time,    0x16);
    SET_WORD(pui1_dent,  pt_dent->wrt_date,    0x18);
    SET_WORD(pui1_dent,  pt_dent->cluster,     0x1A);
    SET_DWORD(pui1_dent, pt_dent->size,        0x1C);
    if (pt_dent->attr & FAT_ATTR_DIRECTORY)
    {
        FAT_SET_VAL32(pui1_dent, FAT_DENTRY_FILE_SIZE, 0);
    }
    else
    {
        FAT_SET_VAL32(pui1_dent, FAT_DENTRY_FILE_SIZE, pt_dent->size);
    }
}


// -------------------------------------------------------------------------
// fatfs_write_longname_dir_entry()
// Writes dir entry to disk.
//added by zhiqiang
int
fatfs_write_longname_dir_entry(fatfs_disk_t *disk, fatfs_dir_entry_t *dentry)
{
    fat_raw_dir_entry_t raw_dentry;
    int                 err;
    cyg_uint8 *s_utf16 = NULL;
    cyg_uint8   ui1_use_dent=0;
    cyg_uint8   ui1_val=0;
    cyg_uint16 utf16_len = 0;
    cyg_uint8 pui1_dent[32];
    cyg_uint8 ui1_chksum;
    char *name = dentry->filename;
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dentry);

    s_utf16 = (cyg_uint8*)malloc(512 * sizeof(cyg_uint8));   
    if (s_utf16 == NULL)
    {
        return ENOMEM;
    }
    memset(s_utf16,0,512);
    int utf8_len = strlen(name);
    TR(("utf8 name len=%d\n",utf8_len));
    //diag_dump_buf_with_offset( name, utf8_len, name );
    utf16_len = UTF8ToUTF16((UTF8*)name,(UTF8*)(name+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512));
    //diag_dump_buf_with_offset( s_utf16, 64, s_utf16 );
   // ASSERT(err == ENOERR);
    //utf16_len = (cyg_uint16)(strlen((const UTF16 *)s_utf16));
    TR(("utf16 name len=%d\n",utf16_len));
    if (utf16_len > 2*(FM_MAX_FILE_LEN+6))
    {
        free(s_utf16);
        TR(("file name too long\n"));
        return ENAMETOOLONG;
    }
    ui1_use_dent = (cyg_uint8)((utf16_len / 13) + 1 + (((utf16_len % 13) == 0) ? 0 : 1));
    TR(("create file ui1_use_dent=%d\n",ui1_use_dent));
   
    // Create new file dir entry
    
    TR(("file name=%s\n",name));
    fatfs_generate_dos_name(disk,
                           dentry, 
                           (cyg_uint8*)dentry->short_name,
                           (cyg_uint8*)name);
    //diag_dump_buf_with_offset( dentry->short_name, 32, dentry->short_name );
    TR(("dos name=%s\n",(char*)dentry->short_name));
    memset(pui1_dent, 0xFF, 32);
    FAT_SET_VAL8(pui1_dent, FAT_LDENTRY_ATTR, 0x0F);
    FAT_SET_VAL8(pui1_dent, FAT_LDENTRY_TYPE, 0);
    ui1_chksum = _fat_longname_chksum(dentry->short_name);
    FAT_SET_VAL8(pui1_dent, FAT_LDENTRY_CHKSUM, ui1_chksum);
    FAT_SET_VAL16(pui1_dent, FAT_LDENTRY_FST_CLUS_LO, 0);

    for (ui1_val = ui1_use_dent; ui1_val > 0; ui1_val--)
    {
        const cyg_uint8 *pui1_src;
        cyg_uint8 *pui1_dst;

        if (ui1_val == 1)
        {
            /*
             *  Short entry.
             */
            dentry_to_raw(dentry,&raw_dentry);
            _fat_set_dentry(pui1_dent,&raw_dentry);            
        }
        else
        {
            /*
             *  Long entries.
             */
            pui1_src = s_utf16 + (ui1_val - 2) * 26;

            if (ui1_val == ui1_use_dent)
            {
                FAT_SET_VAL8(pui1_dent, FAT_LDENTRY_ORDER, (cyg_uint8)((ui1_val - 1) | 0x40));
                utf16_len = (cyg_uint16)((utf16_len % 13) * 2);
                if (utf16_len == 0)
                {
                    utf16_len = 26;
                }
                else
                {
                    utf16_len += 2;
                }
            }
            else
            {
                FAT_SET_VAL8(pui1_dent, FAT_LDENTRY_ORDER, (cyg_uint8)(ui1_val - 1));
                utf16_len = 26;
            }
        
            /*
             *  Note that ui2_len includes end zero.
             */
            pui1_dst = FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME1);
            int i;
            for (i = 0; i < utf16_len; i++)
            {
                if (i == 10)
                {
                    pui1_dst = FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME2);
                }
                else if (i == 22)
                {
                    pui1_dst = FAT_OFFSET(pui1_dent, FAT_LDENTRY_NAME3);
                }

                *pui1_dst++ = *pui1_src++;
            }
        }
        int data_len = DENTRY_SIZE;
        TR(("start write\n"));
        //diag_dump_buf_with_offset( pui1_dent, 32, pui1_dent );
        err = write_data(disk,pui1_dent,&data_len,&dentry->disk_pos);
        if (err != ENOERR)
        {
            TR(("create file write data fail\n"));
            free(s_utf16);
            return err;
        }

        //pos.cluster_pos += 32;
    }
    dentry->disk_pos.cluster_pos-= DENTRY_SIZE;
    free(s_utf16);
    
    //dentry_to_raw(dentry, &raw_dentry);
    //err = write_raw_dentry(disk, &dentry->disk_pos, &raw_dentry);
    return err;
}

// -------------------------------------------------------------------------
// fatfs_write_dir_entry()
// Writes dir entry to disk.
 
int
fatfs_write_dir_entry(fatfs_disk_t *disk, fatfs_dir_entry_t *dentry)
{
    fat_raw_dir_entry_t raw_dentry;
    int                 err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dentry);
    //_fat_copy_dos_name(ps_dos_name, dentry->filename);
    dentry_to_raw(dentry, &raw_dentry);
    err = write_raw_dentry(disk, &dentry->disk_pos, &raw_dentry);
    return err;
}

// -------------------------------------------------------------------------
// fatfs_delete_file()
// Marks dir entry as deleted and frees its cluster chain.
 
int
fatfs_delete_file(fatfs_disk_t *disk, fatfs_dir_entry_t *file)
{
    fat_raw_dir_entry_t raw_dentry;
    int                 err;
    int      ui1_use_dent;
    cyg_uint16 utf16_len = 0;
    cyg_uint8 *s_utf16 = NULL;
    fatfs_data_pos_t    pos;
    
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    
    if (is_root_dir_entry(file))
        return EINVAL;
    
    CYG_TRACE1(TDE, "filename='%s'", file->filename);
    TR(("ENTER fatfs_delete_file filename='%s'\n",file->filename));

    dentry_to_raw(file, &raw_dentry);

    // Check if we are about to delete a directory
    if (DENTRY_IS_DIR(&raw_dentry))
    {
        fat_raw_dir_entry_t raw_cdentry;
        
        int                 i = 0;
        
        fatfs_initpos(disk, file, &pos);
        
        CYG_TRACE0(TDE, "got directory");

        // Count number of entries in this dir    

        while (true)
        {
            err = read_next_raw_dentry(disk, &pos, &raw_cdentry);

            if (EEOF == err)
                break;
            else if (err != ENOERR)
                return err;

            pos.cluster_pos += DENTRY_SIZE;
            i++; 
        }
        CYG_TRACE1(TDE, "child count=%d", i);
        
        // Check if the dir is empty (except '.' and '..')
        if (i > 2)
            return EPERM; 
    }    
    
    // Free file clusters
    free_cluster_chain(disk, raw_dentry.cluster | (raw_dentry.cluster_HI << 16));
    //raw_dentry_set_deleted(disk, &raw_dentry);
    //err = write_raw_dentry(disk, &file->disk_pos, &raw_dentry);
    s_utf16 = (cyg_uint8*)malloc(512 * sizeof(cyg_uint8)); 
    if (s_utf16 == NULL)
    {
        return ENOMEM;
    }
    int utf8_len = strlen(file->filename);
    int i;
    utf16_len = UTF8ToUTF16((UTF8*)file->filename,(UTF8*)(file->filename+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512)); 
    cyg_uint8  ui1_e5 = 0xE5;
    ui1_use_dent = (cyg_uint8)((utf16_len / 13) + 1 + (((utf16_len % 13) == 0) ? 0 : 1));
    pos = file->disk_pos;
    for (pos.cluster_pos = pos.cluster_pos - (ui1_use_dent - 1) * 32, i = 0;
         i < ui1_use_dent;
         pos.cluster_pos += 31, i++)
    {
        //err = _fat_rw_buf(&t_io, FALSE, FALSE);
        int data_len = 1;
        err = write_data(disk,&ui1_e5,&data_len,&pos);
        if (err != ENOERR)
        {
            return err;
        }
    }
    TR(("fatfs_delete_file end\n"));
    return err;
} 

// -------------------------------------------------------------------------
// fatfs_create_file()
// Creates a new file.
 
int
fatfs_create_file(fatfs_disk_t      *disk, 
                  fatfs_dir_entry_t *dir, 
                  const char        *name,
                  int                namelen,
                  fatfs_dir_entry_t *dentry)
{
    fatfs_data_pos_t pos;
    int              err;
    cyg_uint8 *s_utf16 = NULL;
    cyg_uint16 utf16_len = 0;
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(name);
    CYG_CHECK_DATA_PTRC(dentry);
 
    CYG_TRACE2(TDE, "filename='%s' parent='%s'", name, dir->filename);
    TR(("fatfs_create_file filename='%s' parent='%s'\n", name, dir->filename));

    s_utf16 = (cyg_uint8*)malloc(512 * sizeof(cyg_uint8));   
    if (s_utf16 == NULL)
    {
        return ENOMEM;
    }
    memset(s_utf16,0,512);
    int utf8_len = strlen(name);
    utf16_len = UTF8ToUTF16((UTF8*)name,(UTF8*)(name+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512));
    if (utf16_len > 2*(FM_MAX_FILE_LEN+6))
    {
        free(s_utf16);
        diag_printf("file name too long\n");
        return ENAMETOOLONG;
    }
    free(s_utf16);
    
    fatfs_initpos(disk, dir, &pos);
     // Get free dir entry in parent dir
    err = get_free_raw_dentry(disk,utf16_len,&pos);
    if (err != ENOERR)
       {TR(("Get free dir entry\n")); return err;}
       
    init_dir_entry(dentry, 
                   name, 
                   namelen, 
                   __stat_mode_REG, 
                   dir->cluster, 
                   0, 
                   &pos); 
    err = fatfs_write_longname_dir_entry(disk, dentry);
    if (err != ENOERR)
       return err;  
    
    TR(("fatfs_create_file end\n"));
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_create_dir()
// Creates a new directory.
 
int
fatfs_create_dir(fatfs_disk_t      *disk, 
                 fatfs_dir_entry_t *dir, 
                 const char        *name,
                 int                namelen,
                 fatfs_dir_entry_t *dentry)
{
    fatfs_dir_entry_t cdentry;
    fatfs_data_pos_t  pos;
    cyg_uint32        free_cluster;
    int               err;
    cyg_uint8 *s_utf16 = NULL;
    cyg_uint16 utf16_len = 0;
    
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(name);
    CYG_CHECK_DATA_PTRC(dentry);
 
    CYG_TRACE2(TDE, "filename='%s' parent='%s'", name, dir->filename);
    TR(("fatfs_create_dir filename='%s' parent='%s'\n", name, dir->filename));
    s_utf16 = (cyg_uint8*)malloc(512 * sizeof(cyg_uint8));   
    if (s_utf16 == NULL)
    {
        return ENOMEM;
    }
    memset(s_utf16,0,512);
    int utf8_len = strlen(name);
    utf16_len = UTF8ToUTF16((UTF8*)name,(UTF8*)(name+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512));
    if (utf16_len > 2*(FM_MAX_FILE_LEN+6))
    {
        free(s_utf16);
        diag_printf("file name too long\n");
        return ENAMETOOLONG;
    }
    free(s_utf16);
   
 
    // Get free cluster
    err = find_next_free_cluster(disk, 
                                 0, 
                                 &free_cluster, 
                                 CO_MARK_LAST | CO_ERASE_NEW);
    if (err != ENOERR)
        return err;
    
    fatfs_initpos(disk, dir, &pos);

    err = get_free_raw_dentry(disk,utf16_len,&pos);
    if (err != ENOERR)
       {TR(("Get free dir entry\n")); return err;}
       
    init_dir_entry(dentry, 
                   name, 
                   namelen, 
                   __stat_mode_DIR, 
                   dir->cluster, 
                   free_cluster, 
                   &pos);
    
    err = fatfs_write_longname_dir_entry(disk, dentry);
    if (err != ENOERR)
       return err;
    // Create '.' and '..' dir entries

    fatfs_initpos(disk, dentry, &pos);

    CYG_TRACE0(TDE, "Creating '.' entry");

    init_dir_entry(&cdentry, 
                   ".", 
                   0, 
                   __stat_mode_DIR, 
                   dentry->cluster, 
                   dentry->cluster, 
                   &pos);
    err = fatfs_write_dir_entry(disk, &cdentry);
    if (err != ENOERR)
        return err;
          
    pos.cluster_pos += DENTRY_SIZE;

    CYG_TRACE0(TDE, "Creating '..' entry");
    
    init_dir_entry(&cdentry, 
                   "..", 
                   0, 
                   __stat_mode_DIR,
                   dentry->cluster, 
                   dir->cluster, 
                   &pos); 
    err = fatfs_write_dir_entry(disk, &cdentry);
    if (err != ENOERR)
        return err;
    TR(("fatfs_create_dir END\n"));
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_trunc_file()
// Truncates a file to zero length.

int
fatfs_trunc_file(fatfs_disk_t *disk, fatfs_dir_entry_t *file)
{
    int err;
    
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
 
    CYG_TRACE1(TDE, "file='%s'", file->filename);
    
    if (S_ISDIR(file->mode))
        return EINVAL; 

    if (0 == file->size)
        return ENOERR;
   
    err = free_cluster_chain(disk, file->cluster);
    if (err != ENOERR)
        return err;
    
    // Update file attributes

    file->cluster = 0;
    file->size    = 0;
    file->mtime   =
    file->atime   = cyg_timestamp();
    TR(("file='%s'\n", file->filename));

    return fatfs_write_dir_entry(disk, file);
}

// -------------------------------------------------------------------------
// fatfs_rename_file()
// Renames a file.
 
int
fatfs_rename_file(fatfs_disk_t      *disk, 
                  fatfs_dir_entry_t *dir1, 
                  fatfs_dir_entry_t *target,
                  fatfs_dir_entry_t *dir2, 
                  const char        *name,
                  int                namelen)
{
    fatfs_data_pos_t    new_pos;
    fatfs_data_pos_t    old_pos = target->disk_pos;
    int                 err;
    cyg_uint8 *s_utf16 = NULL;
    cyg_uint16 utf16_len = 0;
    cyg_uint16 ui1_use_dent = 0;
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir1);
    CYG_CHECK_DATA_PTRC(target);
    CYG_CHECK_DATA_PTRC(dir2);
    CYG_CHECK_DATA_PTRC(name);
    //diag_printf("fatfs_rename_file name=%s namelen=%d\n",name,namelen);
    if (is_root_dir_entry(target))
        return EINVAL;
    // Delete dentry at old location
    s_utf16 = (cyg_uint8*)malloc(512 * sizeof(cyg_uint8)); 
    if (s_utf16 == NULL)
    {
        return ENOMEM;
    }
    int utf8_len = strlen(target->filename);
    int i;
    utf16_len = UTF8ToUTF16((UTF8*)target->filename,(UTF8*)(target->filename+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512)); 
    cyg_uint8  ui1_e5 = 0xE5;
    ui1_use_dent = (cyg_uint8)((utf16_len / 13) + 1 + (((utf16_len % 13) == 0) ? 0 : 1));
    old_pos = target->disk_pos;
    for (old_pos.cluster_pos = old_pos.cluster_pos - (ui1_use_dent - 1) * 32, i = 0;
         i < ui1_use_dent;
         old_pos.cluster_pos += 31, i++)
    {
        //err = _fat_rw_buf(&t_io, FALSE, FALSE);
        int data_len = 1;
        err = write_data(disk,&ui1_e5,&data_len,&old_pos);
        if (err != ENOERR)
        {
            return err;
        }
    }
    
    memcpy(target->filename, name, namelen);
    target->filename[namelen] = '\0';
   
    // Moving around in same dir

   /* if (dir1 == dir2)
    {
        CYG_TRACE0(TDE, "same dir");
        TR(("same dir\n"));
        return fatfs_write_dir_entry(disk, target); 
    }*/
    
    CYG_TRACE0(TDE, "different dirs"); 
    
    memset(s_utf16,0,512);
    utf8_len = strlen(name);
    utf16_len = UTF8ToUTF16((UTF8*)name,(UTF8*)(name+utf8_len),(UTF16 *)s_utf16,(UTF16 *)(s_utf16+512));
    if (utf16_len > 2*(FM_MAX_FILE_LEN+6))
    {
        free(s_utf16);
        diag_printf("file name too long\n");
        return ENAMETOOLONG;
    }
    free(s_utf16);
    fatfs_generate_dos_name(disk,
                           target, 
                           (cyg_uint8*)target->short_name,
                           (cyg_uint8*)name);
    // Moving around in different dirs

    fatfs_initpos(disk, dir2, &new_pos);

    CYG_TRACE0(TDE, "writing to new dir"); 

    // Get free dir entry in target dir

    err = get_free_raw_dentry(disk,utf16_len, &new_pos);
    if (err != ENOERR)
        return err;

    // Write file dentry to new location
    target->disk_pos = new_pos;
    //dentry_to_raw(target, &raw_dentry);
    err = fatfs_write_longname_dir_entry(disk, target);
    if (err != ENOERR)
       return err;  
   
    CYG_TRACE0(TDE, "deleting from old dir"); 
    
   /* // Delete dentry at old location

    raw_dentry_set_deleted(disk, &raw_dentry);
    raw_dentry.size    = 0;
    raw_dentry.cluster = 0;
    raw_dentry.cluster_HI = 0;
    err = write_raw_dentry(disk, &target->disk_pos, &raw_dentry);
    if (err != ENOERR)
        return err;*/
   
    // Set file new position and parent cluster

    //target->disk_pos       = new_pos;
    target->parent_cluster = dir2->cluster;
 
    // If we moved a directory, we also have to correct the '..' entry  

    /*if ( S_ISDIR(target->mode) )
    {
        fat_raw_dir_entry_t raw_cdentry;
        fatfs_data_pos_t    pos;
       
        fatfs_initpos(disk, target, &pos);

        CYG_TRACE0(TDE, "moving directory - correcting '..' entry");

        while (true)
        {
            err = read_next_raw_dentry(disk, &pos, &raw_cdentry);

            if (EEOF == err)
                return EINVAL; // This dir doesn't have the '..' entry,
                               // that means something is very wrong
            else if (err != ENOERR)
                return err;

            if (0 == strncmp("..", raw_cdentry.name, 2))
            {
                raw_cdentry.cluster = dir2->cluster & 0xFFFF;
                raw_cdentry.cluster_HI = dir2->cluster >> 16;
                err = write_raw_dentry(disk, &pos, &raw_cdentry);
                if (err != ENOERR)
                    return err;
                break;
            }

            pos.cluster_pos += DENTRY_SIZE;
        }
    }*/
   
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_read_data()
// Reads data from disk. 
 
int
fatfs_read_data(fatfs_disk_t      *disk,
                fatfs_dir_entry_t *file,
                fatfs_data_pos_t  *pos,
                void              *data,
                cyg_uint32        *len)
{
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    CYG_CHECK_DATA_PTRC(data);
    CYG_CHECK_DATA_PTRC(len);
    CYG_CHECK_DATA_PTRC(pos);

    return read_data(disk, data, len, pos);
}

// -------------------------------------------------------------------------
// fatfs_write_data()
// Writes data to disk. 
 
int
fatfs_write_data(fatfs_disk_t      *disk,
                 fatfs_dir_entry_t *file,
                 fatfs_data_pos_t  *pos,
                 void              *data,
                 cyg_uint32        *len)
{
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(file);
    CYG_CHECK_DATA_PTRC(data);
    CYG_CHECK_DATA_PTRC(len);
    CYG_CHECK_DATA_PTRC(pos);

    // Check if this file has a zero size and no first cluster

    if (0 == file->size && 0 == file->cluster)
    {
        cyg_uint32 free_cluster;

        CYG_TRACE0(TDO, "new cluster for zero file");

        err = find_next_free_cluster(disk, 0, &free_cluster, CO_MARK_LAST);

        if (err != ENOERR)
        {
            *len = 0;
            diag_printf("find_next_free_cluster fail\n");
            return err;
        }

        file->cluster = free_cluster;
        fatfs_initpos(disk, file, pos);
    }

    return write_data_block(disk, data, len, pos);
    return ENOERR;
}

// -------------------------------------------------------------------------
// Support routines
// These enable the definition of local versions of certain routines
// if the given packages are not present.

#ifndef CYGPKG_LIBC_I18N

__externC int
toupper( int c )
{
    return (('a' <= c) && (c <= 'z')) ? c - 'a' + 'A' : c ;
}

#endif

#ifndef CYGFUN_LIBC_STRING_BSD_FUNCS

__externC int
strcasecmp( const char *s1, const char *s2 )
{
    int ret;
    CYG_REPORT_FUNCNAMETYPE( "strcasecmp", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (*s1 != '\0' && toupper(*s1) == toupper(*s2))
    {
        s1++;
        s2++;
    }

    ret = toupper(*(unsigned char *) s1) - toupper(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

__externC int
strncasecmp( const char *s1, const char *s2, size_t n )
{
    int ret;
    CYG_REPORT_FUNCNAMETYPE( "strncasecmp", "returning %d" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    if (n == 0)
    {
        CYG_REPORT_RETVAL(0);
        return 0;
    }

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (n-- != 0 && toupper(*s1) == toupper(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    ret = toupper(*(unsigned char *) s1) - toupper(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

#endif

// -------------------------------------------------------------------------
// EOF fatfs_supp.c
