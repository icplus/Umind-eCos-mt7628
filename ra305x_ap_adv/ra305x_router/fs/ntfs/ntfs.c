
#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/io_fileio.h>
//#include <pkgconf/fs_fat.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

#include <cyg/infra/diag.h>
#include <cyg/fileio/fileio.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>

#include "compat.h"
#include "attrib.h"
#include "inode.h"
#include "volume.h"
#include "dir.h"
#include "unistr.h"
#include "layout.h"
#include "index.h"
#include "ntfstime.h"
//#include "security.h"
#include "reparse.h"
#include "object_id.h"
#include "efs.h"
#include "logging.h"
#include "xattrs.h"
#include "misc.h"




#include "dm.h"


//==========================================================================
// Tracing support defines

#ifdef FATFS_TRACE_FS_OP
# define TFS 1
#else
# define TFS 0
#endif

#ifdef FATFS_TRACE_FILE_OP
# define TFO 1
# define TR(_args_)   diag_printf _args_
#else
# define TFO 0
# define TR(_args_)
#endif

enum {
	CLOSE_COMPRESSED = 1,
	CLOSE_ENCRYPTED = 2,
	CLOSE_DMTIME = 4
};
static const char ntfs_bad_reparse[] = "unsupported reparse point";


//==========================================================================
// Forward definitions

// Filesystem operations
static int ntfs_mount  (cyg_fstab_entry *fste, cyg_mtab_entry *mte);
static int ntfs_umount (cyg_mtab_entry *mte);
static int ntfs_open   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int mode, cyg_file *fte);
static int ntfs_unlink (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int ntfs_mkdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int ntfs_rmdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int ntfs_rename (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2 );
static int ntfs_link   (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2, int type );
static int ntfs_opendir(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_file *fte );
static int ntfs_chdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_dir *dir_out );
static int ntfs_stat   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         struct stat *buf);
static int ntfs_getinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len );
static int ntfs_setinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len );

// File operations
static int ntfs_fo_read   (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int ntfs_fo_write  (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int ntfs_fo_lseek  (struct CYG_FILE_TAG *fp, off_t *pos, int whence );
static int ntfs_fo_ioctl  (struct CYG_FILE_TAG *fp, CYG_ADDRWORD com,
                            CYG_ADDRWORD data);
static int ntfs_fo_fsync  (struct CYG_FILE_TAG *fp, int mode );
static int ntfs_fo_close  (struct CYG_FILE_TAG *fp);
static int ntfs_fo_fstat  (struct CYG_FILE_TAG *fp, struct stat *buf );
static int ntfs_fo_getinfo(struct CYG_FILE_TAG *fp, int key,
                            void *buf, int len );
static int ntfs_fo_setinfo(struct CYG_FILE_TAG *fp, int key,
                            void *buf, int len );

// Directory operations
static int ntfs_fo_dirread (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int ntfs_fo_dirlseek(struct CYG_FILE_TAG *fp, off_t *pos, int whence);

//==========================================================================
extern bool usb_io_compare(const char *n1, const char *n2, const char **ptr);
extern Cyg_ErrNo usb_io_lookup(const char *name, cyg_io_handle_t *handle);

// -------------------------------------------------------------------------
// Fstab entry.

FSTAB_ENTRY(ntfs_fste, "ntfs", 0,
            CYG_SYNCMODE_FILE_FILESYSTEM|CYG_SYNCMODE_IO_FILESYSTEM,
            ntfs_mount,
            ntfs_umount,
            ntfs_open,
            ntfs_unlink,
            ntfs_mkdir,
            ntfs_rmdir,
            ntfs_rename,
            ntfs_link,
            ntfs_opendir,
            ntfs_chdir,
            ntfs_stat,
            ntfs_getinfo,
            ntfs_setinfo);
            
// -------------------------------------------------------------------------
// File operations.

static cyg_fileops ntfs_fileops =
{
    ntfs_fo_read,
    ntfs_fo_write,
    ntfs_fo_lseek,
    ntfs_fo_ioctl,
    cyg_fileio_seltrue,
    ntfs_fo_fsync,
    ntfs_fo_close,
    ntfs_fo_fstat,
    ntfs_fo_getinfo,
    ntfs_fo_setinfo
};
// -------------------------------------------------------------------------
// Directory file operations.

static cyg_fileops ntfs_dirops =
{
    ntfs_fo_dirread,
    (cyg_fileop_write *)cyg_fileio_enosys,
    ntfs_fo_dirlseek,
    (cyg_fileop_ioctl *)cyg_fileio_enosys,
    cyg_fileio_seltrue,
    (cyg_fileop_fsync *)cyg_fileio_enosys,
    ntfs_fo_close,
    (cyg_fileop_fstat *)cyg_fileio_enosys,
    (cyg_fileop_getinfo *)cyg_fileio_enosys,
    (cyg_fileop_setinfo *)cyg_fileio_enosys
};

// -------------------------------------------------------------------------


static int ntfs_mount  (cyg_fstab_entry *fste, cyg_mtab_entry *mte)
{
    //Cyg_ErrNo           err;
    ntfs_volume *vol;
    vol = ntfs_mount_vol(mte->devname,NTFS_MNT_RDONLY);
    if(vol == NULL) return EINVAL;
    //added by zhiqiang for write performance
    vol->data_buf = malloc(DATA_CACHE_SIZE);
    vol->buf_len = 0;
    vol->pos = 0;
    mte->data = (CYG_ADDRWORD)vol;
    return ENOERR;
}

// -------------------------------------------------------------------------
static int ntfs_umount (cyg_mtab_entry *mte)
{
    ntfs_volume *vol = (ntfs_volume*)mte->data;
    ntfs_umount_vol(vol,true);
}
// -------------------------------------------------------------------------

static int ntfs_fs_create(cyg_mtab_entry *mte, mode_t typemode,cyg_dir dir, const char *path)
{
	char *name;
	ntfschar *uname = NULL, *utarget = NULL;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	ntfs_inode *dir_ni = NULL, *ni;
	char *dir_path;
	le32 securid = 0;
	//char *path;
	gid_t gid;
	mode_t dsetgid;
	ntfschar *stream_name;
	int stream_name_len;
    //mode_t typemode = S_IFDIR | 07777;//zhiqiang change:value of typemode
	mode_t type = typemode;
	mode_t perm;
	//struct SECURITY_CONTEXT security;
	int res = 0, uname_len, utarget_len;
    path--;
    ntfs_log_trace("entering file name=%s\n",path);
	dir_path = strdup(path);
	if (!dir_path)
		return errno;
	/* Generate unicode filename. */
	name = strrchr(dir_path, '/');
    if(name == NULL) ntfs_log_trace("path is invalid\n");
	else name++;

	uname_len = ntfs_mbstoucs(name, &uname);

	if (uname_len < 0) {
		res = -errno;
		goto exit;
	}
#if 0	
    stream_name_len = ntfs_fuse_parse_path(org_path,
					 &path, &stream_name);
		/* stream name validity has been checked previously */
	if (stream_name_len < 0) {
		res = stream_name_len;
		goto exit;
	}
#endif
	/* Open parent directory. */
	*--name = 0;
	dir_ni = ntfs_pathname_to_inode(vol, NULL, dir_path);
	if (!dir_ni) {
		//free(path);
		res = errno;
		goto exit;
	}
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
		/* make sure parent directory is writeable and executable */
	if (!ntfs_fuse_fill_security_context(&security)
	       || ntfs_allowed_create(&security,
				dir_ni, &gid, &dsetgid)) {
#else
		ntfs_fuse_fill_security_context(&security);
		ntfs_allowed_create(&security, dir_ni, &gid, &dsetgid);
#endif
		if (S_ISDIR(type))
			perm = (typemode  & 0777)
				| (dsetgid & S_ISGID);
		else
			perm = typemode & 0777;
			/*
			 * Try to get a security id available for
			 * file creation (from inheritance or argument).
			 * This is not possible for NTFS 1.x, and we will
			 * have to build a security attribute later.
			 */
		if (!ctx->security.mapping[MAPUSERS])
			securid = 0;
		else
			if (ctx->inherit)
				securid = ntfs_inherited_id(&security,
					dir_ni, S_ISDIR(type));
			else
#if POSIXACLS
				securid = ntfs_alloc_securid(&security,
					security.uid, gid,
					dir_ni, perm, S_ISDIR(type));
#else
				securid = ntfs_alloc_securid(&security,
					security.uid, gid,
					perm & ~security.umask, S_ISDIR(type));
#endif
#endif
		/* Create object specified in @type. */
		switch (type) {
			case S_IFCHR:
			case S_IFBLK:
				/*ni = ntfs_create_device(dir_ni, securid,
						uname, uname_len, type,	dev);*/
				break;
			case S_IFLNK:
				utarget_len = ntfs_mbstoucs(NULL, &utarget);
				if (utarget_len < 0) {
					res = -errno;
					goto exit;
				}
				ni = ntfs_create_symlink(dir_ni, securid,
						uname, uname_len,
						utarget, utarget_len);
				break;
			default:
				ni = ntfs_create(dir_ni, securid, uname,
						uname_len, type);
				break;
		}
		if (ni) {
				/*
				 * set the security attribute if a security id
				 * could not be allocated (eg NTFS 1.x)
				 */
#if 0            
			if (ctx->security.mapping[MAPUSERS]) {

#if POSIXACLS
			   	if (!securid
				   && ntfs_set_inherited_posix(&security, ni,
					security.uid, gid,
					dir_ni, perm) < 0);
					//set_fuse_error(&res);
#else
			   	if (!securid
				   && ntfs_set_owner_mode(&security, ni,
					security.uid, gid, 
					perm & ~security.umask) < 0);
					//set_fuse_error(&res);
#endif

			}
#endif			
            //set_archive(ni);
			/* mark a need to compress the end of file */
			/*if (fi && (ni->flags & FILE_ATTR_COMPRESSED)) {
				fi->fh |= CLOSE_COMPRESSED;
			}*/
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
			/* mark a future need to fixup encrypted inode */
			if (fi
			    && ctx->efs_raw
			    && (ni->flags & FILE_ATTR_ENCRYPTED))
				fi->fh |= CLOSE_ENCRYPTED;
#endif /* HAVE_SETXATTR */
			/* mark a need to update the mtime */
			/*if (fi && ctx->dmtime)
				fi->fh |= CLOSE_DMTIME;*/
			NInoSetDirty(ni);
			/*
			 * closing ni requires access to dir_ni to
			 * synchronize the index, avoid double opening.
			 */
			if (ntfs_inode_close_in_dir(ni, dir_ni));
				//set_fuse_error(&res);
			//ntfs_fuse_update_times(dir_ni, NTFS_UPDATE_MCTIME);
		} else
			res = errno;
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	} else
		res = -errno;
#endif
#endif
	//free(path);

exit:
	free(uname);
	if (ntfs_inode_close(dir_ni));
		//set_fuse_error(&res);
	free(dir_path);
	return res;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
static int ntfs_open   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int mode, cyg_file *file)
{
    ntfs_log_trace("entering\n");
#if 0
    ntfs_inode *ni;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	ntfs_attr *na;
	int res = 0;
	char *path = strdup(name);
	ntfschar *stream_name;
	int stream_name_len;
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	int accesstype;
	struct SECURITY_CONTEXT security;
#endif

	/*stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;*/
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (ni) {
		na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
		if (na) {
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
			if (ntfs_fuse_fill_security_context(&security)) {
				if (mode & O_WRONLY)
					accesstype = S_IWRITE;
				else
					if (mode & O_RDWR)
						 accesstype = S_IWRITE | S_IREAD;
					else
						accesstype = S_IREAD;
				/*
				 * directory must be searchable
				 * and requested access allowed
				 */
				if (!ntfs_allowed_dir_access(&security,
					    path,(ntfs_inode*)NULL,ni,S_IEXEC)
				  || !ntfs_allowed_access(&security,
						ni,accesstype))
					res = -EACCES;
			}
#endif
			if ((res >= 0)
			    && (mode & (O_WRONLY | O_RDWR))) {
			/* mark a future need to compress the last chunk */
				if (na->data_flags & ATTR_COMPRESSION_MASK);
					//fi->fh |= CLOSE_COMPRESSED;
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
			/* mark a future need to fixup encrypted inode */
				if (!(na->data_flags & ATTR_IS_ENCRYPTED)
				    && (ni->flags & FILE_ATTR_ENCRYPTED));
					//fi->fh |= CLOSE_ENCRYPTED;
#endif /* HAVE_SETXATTR */
			/* mark a future need to update the mtime */
				//if (ctx->dmtime)
					//fi->fh |= CLOSE_DMTIME;
			/* deny opening metadata files for writing */
				if (ni->mft_no < FILE_first_user)
					res = -EPERM;
			}
            file->f_offset  = (mode & O_APPEND) ? na->data_size : 0;
			ntfs_attr_close(na);
		} else
			res = -errno;
        ntfs_inode_close(ni);
		/*if (ntfs_inode_close(ni))
			set_fuse_error(&res);*/
	} else
		res = -errno;
#endif
    int res = 0;
    char *path = ntfs_malloc(strlen(name)+1);
    strcpy(path,name);
    ntfs_log_trace("filename=%s\n",path);
    if(mode & O_CREAT)
    {
        mode_t typemode = S_IFREG;
        res = ntfs_fs_create(mte,typemode,dir,name);
        if(res != ENOERR) return errno;
    }
    file->f_flag   |= mode & CYG_FILE_MODE_MASK;
    file->f_type    = CYG_FILE_TYPE_FILE;
    file->f_ops     = &ntfs_fileops;
    file->f_offset  = 0;
    file->f_data    = (CYG_ADDRWORD)path;//zhiqiang:remember to free
    file->f_xops    = 0;
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_unlink (cyg_mtab_entry *mte, cyg_dir dir, const char *name)
{
    //char *path = name;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
    char *name_temp;
	ntfschar *uname = NULL;
	ntfs_inode *dir_ni = NULL, *ni;
	char *path;
	int uname_len;
	int res = 0;
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	//struct SECURITY_CONTEXT security;
#endif
#if 0
	stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;
	if (!stream_name_len)
		res = ntfs_fuse_rm(path);
	else {
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
			/*
			 * JPA deny unlinking stream if directory is not
			 * writable and executable (debatable)
			 */
		if (!ntfs_fuse_fill_security_context(&security)
		   || ntfs_allowed_dir_access(&security, path,
				(ntfs_inode*)NULL, (ntfs_inode*)NULL,
				S_IEXEC + S_IWRITE + S_ISVTX))
			res = ntfs_fuse_rm_stream(path, stream_name,
					stream_name_len);
		else
			res = -errno;
#else
		res = ntfs_fuse_rm_stream(path, stream_name, stream_name_len);
#endif
	}
	free(path);
	if (stream_name_len)
		free(stream_name);
#endif
    name--;
    path = strdup(name);
	if (!path)
		return -errno;
	/* Open object for delete. */
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	/* deny unlinking metadata files */
	if (ni->mft_no < FILE_first_user) {
		errno = EPERM;
		res = -errno;
		goto exit;
	}

	/* Generate unicode filename. */
	name_temp = strrchr(path, '/');
	name_temp++;
	uname_len = ntfs_mbstoucs(name_temp, &uname);
	if (uname_len < 0) {
		res = -errno;
		goto exit;
	}
	/* Open parent directory. */
	*--name_temp = 0;
	dir_ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!dir_ni) {
		res = -errno;
		goto exit;
	}
#if 0	
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	/* JPA deny unlinking if directory is not writable and executable */
	if (!ntfs_fuse_fill_security_context(&security)
	    || ntfs_allowed_dir_access(&security, org_path, dir_ni, ni,
				   S_IEXEC + S_IWRITE + S_ISVTX)) {
#endif
#endif
		if (ntfs_delete(vol, name, ni, dir_ni,
				 uname, uname_len))
			res = -errno;
		/* ntfs_delete() always closes ni and dir_ni */
		ni = dir_ni = NULL;
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	} else
		res = -EACCES;
#endif
#endif
exit:
	ntfs_inode_close(dir_ni);
	ntfs_inode_close(ni);
	free(uname);
	free(path);
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_mkdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *path)
{
    mode_t typemode = S_IFDIR;
	return ntfs_fs_create(mte,typemode,dir,path);
}
// -------------------------------------------------------------------------
static int ntfs_rmdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name)
{
    ntfs_volume *vol = (ntfs_volume*)mte->data;
    char *name_temp;
	ntfschar *uname = NULL;
	ntfs_inode *dir_ni = NULL, *ni;
	char *path;
	int uname_len;
	int res = 0;
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	//struct SECURITY_CONTEXT security;
#endif
#if 0
	stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;
	if (!stream_name_len)
		res = ntfs_fuse_rm(path);
	else {
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
			/*
			 * JPA deny unlinking stream if directory is not
			 * writable and executable (debatable)
			 */
		if (!ntfs_fuse_fill_security_context(&security)
		   || ntfs_allowed_dir_access(&security, path,
				(ntfs_inode*)NULL, (ntfs_inode*)NULL,
				S_IEXEC + S_IWRITE + S_ISVTX))
			res = ntfs_fuse_rm_stream(path, stream_name,
					stream_name_len);
		else
			res = -errno;
#else
		res = ntfs_fuse_rm_stream(path, stream_name, stream_name_len);
#endif
	}
	free(path);
	if (stream_name_len)
		free(stream_name);
#endif
    name--;
    path = strdup(name);
	if (!path)
		return -errno;
	/* Open object for delete. */
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	/* deny unlinking metadata files */
	if (ni->mft_no < FILE_first_user) {
		errno = EPERM;
		res = -errno;
		goto exit;
	}

	/* Generate unicode filename. */
	name_temp = strrchr(path, '/');
	name_temp++;
	uname_len = ntfs_mbstoucs(name_temp, &uname);
	if (uname_len < 0) {
		res = -errno;
		goto exit;
	}
	/* Open parent directory. */
	*--name_temp = 0;
	dir_ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!dir_ni) {
		res = -errno;
		goto exit;
	}
#if 0	
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	/* JPA deny unlinking if directory is not writable and executable */
	if (!ntfs_fuse_fill_security_context(&security)
	    || ntfs_allowed_dir_access(&security, org_path, dir_ni, ni,
				   S_IEXEC + S_IWRITE + S_ISVTX)) {
#endif
#endif
		if (ntfs_delete(vol, name, ni, dir_ni,
				 uname, uname_len))
			res = -errno;
		/* ntfs_delete() always closes ni and dir_ni */
		ni = dir_ni = NULL;
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	} else
		res = -EACCES;
#endif
#endif
exit:
	ntfs_inode_close(dir_ni);
	ntfs_inode_close(ni);
	free(uname);
	free(path);
	return res;
}

// -------------------------------------------------------------------------

static int ntfs_rename (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2 )
{
    int ret;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	char *path = name2;
	ntfs_inode *ni;
	u64 inum;
	BOOL same;
	
	ntfs_log_debug("rename: old: '%s'  new: '%s'\n", name1, name2);
	
	/*
	 *  FIXME: Rename should be atomic.
	 */
	/*stream_name_len = ntfs_fuse_parse_path(new_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;*/
	
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (ni) {
		ret = ntfs_check_empty_dir(ni);
		if (ret < 0) {
			ret = -errno;
			ntfs_inode_close(ni);
			goto out;
		}
		
		inum = ni->mft_no;
		if (ntfs_inode_close(ni)) {
			if (!ret)
		        ret = -errno;
			goto out;
		}

		//free(path);
		path = name1;

			/* silently ignore a rename to same inode */
		/*stream_name_len = ntfs_fuse_parse_path(old_path,
						&path, &stream_name);
		if (stream_name_len < 0)
			return stream_name_len;*/
	
		ni = ntfs_pathname_to_inode(vol, NULL, path);
		if (ni) {
			same = ni->mft_no == inum;
			if (ntfs_inode_close(ni))
				ret = -errno;
			else
				if (!same);
					/*ret = ntfs_fuse_rename_existing_dest(
							old_path, new_path);*/
				    ret = -EEXIST;//modifed by zhiqiang
		} else
			ret = -errno;
		goto out;
	}

	ret = ntfs_link(mte,dir1,name1,dir2, name2,0);
	if (ret)
		goto out;
	
	ret = ntfs_unlink(mte,dir1,name1);
	if (ret)
		//ntfs_unlink(new_path);
out:

	return ret;
}
// -------------------------------------------------------------------------
static int ntfs_link   (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2, int type )
{
    char *name;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	ntfschar *uname = NULL;
	ntfs_inode *dir_ni = NULL, *ni;
	char *path;
	int res = 0, uname_len;
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	BOOL samedir;
	struct SECURITY_CONTEXT security;
#endif
	if (ntfs_fuse_is_named_data_stream(old_path))
		return -EINVAL; /* n/a for named data streams. */
	if (ntfs_fuse_is_named_data_stream(new_path))
		return -EINVAL; /* n/a for named data streams. */
#endif
    name2--;
	path = strdup(name2);
	if (!path)
		return -errno;
	/* Open file for which create hard link. */
	ni = ntfs_pathname_to_inode(vol, NULL, name1);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	
	/* Generate unicode filename. */
	name = strrchr(path, '/');
	name++;
	uname_len = ntfs_mbstoucs(name, &uname);
	if (uname_len < 0) 
    {
		res = -errno;
		goto exit;
	}
	/* Open parent directory. */
	*--name = 0;
	dir_ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!dir_ni) {
		res = -errno;
		goto exit;
	}
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
	samedir = !strncmp(old_path, path, strlen(path))
			&& (old_path[strlen(path)] == '/');
		/* JPA make sure the parent directories are writeable */
	if (ntfs_fuse_fill_security_context(&security)
	   && ((!samedir && !ntfs_allowed_dir_access(&security,old_path,
			(ntfs_inode*)NULL,ni,S_IWRITE + S_IEXEC))
	      || !ntfs_allowed_access(&security,dir_ni,S_IWRITE + S_IEXEC)))
		res = -EACCES;
	else
#endif
#endif
	{
		if (ntfs_link_i(ni, dir_ni, uname, uname_len,FILE_NAME_POSIX)) {
				res = -errno;
			goto exit;
		}
	
		//set_archive(ni);
		//ntfs_fuse_update_times(ni, NTFS_UPDATE_CTIME);
		//ntfs_fuse_update_times(dir_ni, NTFS_UPDATE_MCTIME);
	}
exit:
	/* 
	 * Must close dir_ni first otherwise ntfs_inode_sync_file_name(ni)
	 * may fail because ni may not be in parent's index on the disk yet.
	 */
	if (ntfs_inode_close(dir_ni))
		res = -errno;
	if (ntfs_inode_close(ni))
		res = -errno;
	free(uname);
	free(path);
	return res;
}
// -------------------------------------------------------------------------
//assume dir always be NULL
static int ntfs_opendir(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_file *file )
{
#if 0
    int res = 0;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	ntfs_inode *ni;
	int accesstype;
	//struct SECURITY_CONTEXT security;

	/*if (ntfs_fuse_is_named_data_stream(path))
		return -EINVAL; *//* n/a for named data streams. */

	ni = ntfs_pathname_to_inode(vol, NULL, name);
	if (ni) {
#if 0 //security
		if (ntfs_fuse_fill_security_context(&security)) {
			if (fi->flags & O_WRONLY)
				accesstype = S_IWRITE;
			else
				if (fi->flags & O_RDWR)
					accesstype = S_IWRITE | S_IREAD;
				else
					accesstype = S_IREAD;
				/*
				 * directory must be searchable
				 * and requested access be allowed
				 */
			if (!strcmp(name,"/")
				? !ntfs_allowed_dir_access(&security,
					name, ni, ni, accesstype | S_IEXEC)
				: !ntfs_allowed_dir_access(&security, path,
						(ntfs_inode*)NULL, ni, S_IEXEC)
				     || !ntfs_allowed_access(&security,
						ni,accesstype))
				res = -EACCES;
		}
#endif
		if (ntfs_inode_close(ni));
			//set_fuse_error(&res);
	} else
		res = -errno;
	return res;
#endif
    ntfs_log_trace("dir name = '%s'\n",name);
    char *path = ntfs_malloc(strlen(name)+1);
    strcpy(path,name);

    file->f_type    = CYG_FILE_TYPE_FILE;
    file->f_ops     = &ntfs_dirops;
    file->f_data    = (CYG_ADDRWORD)path;//zhiqiang:remember to free
    file->f_xops    = 0;
    file->f_offset  = 0;
    return ENOERR;
}
// -------------------------------------------------------------------------
static int ntfs_chdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_dir *dir_out )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_stat   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         struct stat *buf)
{

    int res = 0;
    ntfs_volume *vol = (ntfs_volume*)mte->data;
	ntfs_inode *ni;
	ntfs_attr *na;
	char *path = strdup(name);
	ntfschar *stream_name;
	//int stream_name_len;
	BOOL withusermapping;
	//struct SECURITY_CONTEXT security;

	/*stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;*/
    ntfs_log_trace("entering\n");
	memset(buf, 0, sizeof(struct stat));
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = errno;
		goto exit;
	}
	//withusermapping = ntfs_fuse_fill_security_context(&security);
#if 0
#if !KERNELPERMS | (POSIXACLS & !KERNELACLS)
		/*
		 * make sure the parent directory is searchable
		 */
	if (withusermapping
	    && !ntfs_allowed_dir_access(&security,path,
			(!strcmp(name,"/") ? ni : (ntfs_inode*)NULL),
			ni, S_IEXEC)) {
               	res = -EACCES;
               	goto exit;
	}
#endif
#endif
	if (((ni->mrec->flags & MFT_RECORD_IS_DIRECTORY)
		|| (ni->flags & FILE_ATTR_REPARSE_POINT))) {
		if (ni->flags & FILE_ATTR_REPARSE_POINT) {
			char *target;
			int attr_size;

			errno = 0;
			target = ntfs_make_symlink(ni, "/", &attr_size);
				/*
				 * If the reparse point is not a valid
				 * directory junction, and there is no error
				 * we still display as a symlink
				 */
			if (target || (errno == EOPNOTSUPP)) {
					/* returning attribute size */
				if (target)
					buf->st_size = attr_size;
				else
					buf->st_size = sizeof(ntfs_bad_reparse);
				//buf->st_blocks = (ni->allocated_size + 511) >> 9;
				buf->st_nlink = le16_to_cpu(ni->mrec->link_count);
				buf->st_mode = S_IFLNK;
				free(target);
			} else {
				res = errno;
				goto exit;
			}
		} else {
			/* Directory. */
			//buf->st_mode = S_IFDIR | (0777 & ~ctx->dmask);
			buf->st_mode = S_IFDIR ;
			/* get index size, if not known */
			if (!test_nino_flag(ni, KnownSize)) {
				na = ntfs_attr_open(ni, AT_INDEX_ALLOCATION, NTFS_INDEX_I30, 4);
				if (na) {
					ni->data_size = na->data_size;
					ni->allocated_size = na->allocated_size;
					set_nino_flag(ni, KnownSize);
					ntfs_attr_close(na);
				}
			}
			buf->st_size = ni->data_size;
			//buf->st_blocks = ni->allocated_size >> 9;
			buf->st_nlink = 1;	/* Make find(1) work */
		}
	} else {
		/* Regular or Interix (INTX) file. */
		buf->st_mode = S_IFREG;
		buf->st_size = ni->data_size;
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
		/*
		 * return data size rounded to next 512 byte boundary for
		 * encrypted files to include padding required for decryption
		 * also include 2 bytes for padding info
		*/
		if (ctx->efs_raw
		    && (ni->flags & FILE_ATTR_ENCRYPTED)
		    && ni->data_size)
			buf->st_size = ((ni->data_size + 511) & ~511) + 2;
#endif /* HAVE_SETXATTR */

		/* 
		 * Temporary fix to make ActiveSync work via Samba 3.0.
		 * See more on the ntfs-3g-devel list.
		 */
		//buf->st_blocks = (ni->allocated_size + 511) >> 9;
		buf->st_nlink = le16_to_cpu(ni->mrec->link_count);
		if (ni->flags & FILE_ATTR_SYSTEM || 0) {
			na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED,
					0);
			if (!na) {
				if (0) {
					res = -ENOENT;
					goto exit;
				} else
					goto nodata;
			}
			if (0) {
				buf->st_size = na->data_size;
				//buf->st_blocks = na->allocated_size >> 9;
			}
			/* Check whether it's Interix FIFO or socket. */
			if (!(ni->flags & FILE_ATTR_HIDDEN) &&
					!0) {
				/* FIFO. */
				if (na->data_size == 0)
					buf->st_mode = S_IFIFO;
				/* Socket link. */
				if (na->data_size == 1)
					buf->st_mode = S_IFSOCK;
			}
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
			/* encrypted named stream */
			/* round size up to next 512 byte boundary */
			if (ctx->efs_raw && stream_name_len && 
			    (na->data_flags & ATTR_IS_ENCRYPTED) &&
			    NAttrNonResident(na)) 
				buf->st_size = ((na->data_size+511) & ~511)+2;
#endif /* HAVE_SETXATTR */

			/*
			 * Check whether it's Interix symbolic link, block or
			 * character device.
			 */
			if ((size_t)na->data_size <= sizeof(INTX_FILE_TYPES)
					+ sizeof(ntfschar) * PATH_MAX
				&& (size_t)na->data_size >
					sizeof(INTX_FILE_TYPES)
				&& !0) {
				
				INTX_FILE *intx_file;

				intx_file = ntfs_malloc(na->data_size);
				if (!intx_file) {
					res = errno;
					ntfs_attr_close(na);
					goto exit;
				}
				if (ntfs_attr_pread(na, 0, na->data_size,
						intx_file) != na->data_size) {
					res = errno;
					free(intx_file);
					ntfs_attr_close(na);
					goto exit;
				}
				if (intx_file->magic == INTX_BLOCK_DEVICE &&
						na->data_size == offsetof(
						INTX_FILE, device_end)) {
					buf->st_mode = S_IFBLK;
					/*buf->st_rdev = makedev(le64_to_cpu(
							intx_file->major),
							le64_to_cpu(
							intx_file->minor));*/
				}
				if (intx_file->magic == INTX_CHARACTER_DEVICE &&
						na->data_size == offsetof(
						INTX_FILE, device_end)) {
					buf->st_mode = S_IFCHR;
					/*buf->st_rdev = makedev(le64_to_cpu(
							intx_file->major),
							le64_to_cpu(
							intx_file->minor));*/
				}
				if (intx_file->magic == INTX_SYMBOLIC_LINK)
					buf->st_mode = S_IFLNK;
				free(intx_file);
			}
			ntfs_attr_close(na);
		}
		//buf->st_mode |= (0777 & ~ctx->fmask);
	}
	/*if (withusermapping) {
		if (ntfs_get_owner_mode(&security,ni,buf) < 0);
			//set_fuse_error(&res);
	} else {
		//buf->st_uid = ctx->uid;
       		//buf->st_gid = ctx->gid;
	}*/
	if (S_ISLNK(buf->st_mode))
		buf->st_mode |= 0777;
nodata :
	buf->st_ino = ni->mft_no;
#ifdef HAVE_STRUCT_STAT_ST_ATIMESPEC
	buf->st_atimespec = ntfs2timespec(ni->last_access_time);
	buf->st_ctimespec = ntfs2timespec(ni->last_mft_change_time);
	buf->st_mtimespec = ntfs2timespec(ni->last_data_change_time);
#elif defined(HAVE_STRUCT_STAT_ST_ATIM)
 	buf->st_atim = ntfs2timespec(ni->last_access_time);
 	buf->st_ctim = ntfs2timespec(ni->last_mft_change_time);
 	buf->st_mtim = ntfs2timespec(ni->last_data_change_time);
#elif defined(HAVE_STRUCT_STAT_ST_ATIMENSEC)
	{
	struct timespec ts;

	ts = ntfs2timespec(ni->last_access_time);
	buf->st_atime = ts.tv_sec;
	buf->st_atimensec = ts.tv_nsec;
	ts = ntfs2timespec(ni->last_mft_change_time);
	buf->st_ctime = ts.tv_sec;
	buf->st_ctimensec = ts.tv_nsec;
	ts = ntfs2timespec(ni->last_data_change_time);
	buf->st_mtime = ts.tv_sec;
	buf->st_mtimensec = ts.tv_nsec;
	}
#else
#warning "No known way to set nanoseconds in struct stat !"
	{
	struct timespec ts;

	ts = ntfs2timespec(ni->last_access_time);
	buf->st_atime = ts.tv_sec;
	ts = ntfs2timespec(ni->last_mft_change_time);
	buf->st_ctime = ts.tv_sec;
	ts = ntfs2timespec(ni->last_data_change_time);
	buf->st_mtime = ts.tv_sec;
	}
#endif
exit:
    ntfs_log_trace("\nfilename=%s mode=%d filesize=%lld serial_num=%d",name,buf->st_mode,buf->st_size,buf->st_ino);
	if (ntfs_inode_close(ni));
		//set_fuse_error(&res);
	free(path);
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_getinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_setinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_fo_read   (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    ntfs_inode *ni = NULL;
    ntfs_volume *vol = (ntfs_volume*)fp->f_mte->data;
	ntfs_attr *na = NULL;
	char *path = fp->f_data;
	int  res;
	s64 total = 0;
	s64 max_read;
    int i;
    cyg_uint64     pos    = fp->f_offset;
    ssize_t        resid  = uio->uio_resid;
    ntfs_log_trace("filename=%s,len=%d pos=%lld\n",path,resid,pos);
	/*if (!size)
		return 0;*/

	/*stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0)
		return stream_name_len;*/
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = errno;
		goto exit;
	}
	na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
	if (!na) {
		res = errno;
		goto exit;
	}
	max_read = na->data_size;
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
	/* limit reads at next 512 byte boundary for encrypted attributes */
	if (ctx->efs_raw
	    && max_read
	    && (na->data_flags & ATTR_IS_ENCRYPTED)
	    && NAttrNonResident(na)) {
		max_read = ((na->data_size+511) & ~511) + 2;
	}
#endif /* HAVE_SETXATTR */
	
	 for (i = 0; i < uio->uio_iovcnt; i++)
    {
        cyg_iovec  *iov  = &uio->uio_iov[i];
        char       *buf  = (char *) iov->iov_base;
        off_t       len  = iov->iov_len;
        if (len > max_read) 
        {
	        len = max_read;
	    }
	    while (len > 0) {
            s64 ret = ntfs_attr_pread(na, pos, len, buf + total);
            if (ret != (s64)len)
            	ntfs_log_perror("ntfs_attr_pread error reading '%s' at "
            		"offset %lld: %lld <> %lld", path, 
            		(long long)0, (long long)len, (long long)ret);
            if (ret <= 0 || ret > (s64)len) {
            	res = (ret < 0) ? -errno : -EIO;
            	goto exit;
		    }
            len -= ret;
		    pos   += ret;
		    total += ret;
            resid -= ret;
        }	
	}
ok:
	//ntfs_inode_update_times(na->ni, NTFS_UPDATE_ATIME);
	uio->uio_resid     = resid;
    fp->f_offset       = (off_t) pos;
	res = ENOERR;
    
exit:
    ntfs_log_trace("exit\n");
	if (na)
		ntfs_attr_close(na);
	if (ntfs_inode_close(ni));
		//set_fuse_error(&res);
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_fo_write  (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{

    ntfs_inode *ni = NULL;
    ntfs_volume *vol = (ntfs_volume*)fp->f_mte->data;
	ntfs_attr *na = NULL;
	char *path = fp->f_data;
	int res, total = 0;
    int i;
    //cyg_uint64 offset;
    cyg_uint64     pos    = fp->f_offset;
    ssize_t        resid  = uio->uio_resid;
	/*stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0) {
		res = stream_name_len;
		goto out;
	}*/
	ntfs_log_trace("filename=%s,len=%d pos=%lld\n",path,resid,pos);
	/*ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = errno;
		goto exit;
	}
	na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
	if (!na) {
		res = errno;
		goto exit;
	}*/
    for (i = 0; i < uio->uio_iovcnt; i++)
    {
        cyg_iovec  *iov  = &uio->uio_iov[i];
        char       *buf  = (char *) iov->iov_base;
        off_t       len  = iov->iov_len;

        // Loop over the vector writing it to the file
        // until it has all been done 
        if((DATA_CACHE_SIZE - vol->buf_len) < len)
        {
            ni = ntfs_pathname_to_inode(vol, NULL, path);
        	if (!ni) {
        		res = errno;
        		goto exit;
        	}
        	na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
        	if (!na) {
        		res = errno;
        		goto exit;
        	}
            //diag_printf("filename=%s,len=%d pos=%lld\n",path,vol->buf_len,vol->pos);
            s64 ret = ntfs_attr_pwrite(na, vol->pos, vol->buf_len, vol->data_buf);
            if (ret <= 0) {
			res = errno;
			goto exit;}
            vol->buf_len = 0;
            vol->pos = -1;
        }
        if(vol->pos == -1)
            vol->pos = pos;
        memcpy(vol->data_buf+vol->buf_len,buf,len);
        resid -= len;
        //pbuf += len;
        pos += len;
        vol->buf_len += len;
        /*while (len > 0)
        {
            cyg_uint32 l = len;
            //offset = 0;
            s64 ret = ntfs_attr_pwrite(na, pos, l, buf + total);

            // Update working vars

            if (ret <= 0) {
			res = errno;
			goto exit;
		    }
		    len   -= ret;
		    pos   += ret;
		    total  += ret;
            resid -= ret;
        }*/
    }
    uio->uio_resid     = resid;
    fp->f_offset       = (off_t) pos;
	res = ENOERR;
	/*if ((res > 0)
	    && (!ctx->dmtime
		|| (le64_to_cpu(ntfs_current_time())
		     - le64_to_cpu(ni->last_data_change_time)) > ctx->dmtime))
		ntfs_fuse_update_times(na->ni, NTFS_UPDATE_MCTIME);*/
exit:
	if (na)
		ntfs_attr_close(na);
	/*if (total)
		set_archive(ni);*/
	if (ntfs_inode_close(ni))
		res = errno;
out:	
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_fo_lseek  (struct CYG_FILE_TAG *fp, off_t *pos, int whence )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_fo_ioctl  (struct CYG_FILE_TAG *fp, CYG_ADDRWORD com,
                            CYG_ADDRWORD data)
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_fo_fsync  (struct CYG_FILE_TAG *fp, int mode )
{
#if 0
   int ret;
   ntfs_volume *vol = (ntfs_volume*)fp->f_mte->data;
		/* sync the full device */
	ret = ntfs_device_sync(vol->dev);
	if (ret)
		ret = -errno;
	return (ret);
#endif
}
// -------------------------------------------------------------------------
static int ntfs_fo_close  (struct CYG_FILE_TAG *fp)
{

   ntfs_inode *ni = NULL;
   ntfs_volume *vol = (ntfs_volume*)fp->f_mte->data;
	ntfs_attr *na = NULL;
	char *path = fp->f_data;
    s64 ret = -1;
	//ntfschar *stream_name;
	int res;

	/* Only for marked descriptors there is something to do */
	/*if (!(fi->fh & (CLOSE_COMPRESSED | CLOSE_ENCRYPTED | CLOSE_DMTIME))) {
		res = 0;
		goto out;
	}
	stream_name_len = ntfs_fuse_parse_path(org_path, &path, &stream_name);
	if (stream_name_len < 0) {
		res = stream_name_len;
		goto out;
	}*/
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni) {
		res = -errno;
		goto exit;
	}
	na = ntfs_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
	if (!na) {
		res = -errno;
		goto exit;
	}
    if(vol->buf_len)
    {
        //diag_printf("filename=%s,len=%d pos=%lld\n",path,vol->buf_len,vol->pos);
        ret = ntfs_attr_pwrite(na, vol->pos, vol->buf_len, vol->data_buf);
        if (ret <= 0) {
		res = -errno;
		goto exit;}
        vol->buf_len = 0;
        vol->pos = -1;
    }
#if 0
	res = 0;
	//if (fi->fh & CLOSE_DMTIME)
		ntfs_inode_update_times(na->ni,NTFS_UPDATE_MCTIME);
	//if (fi->fh & CLOSE_COMPRESSED)
		res = ntfs_attr_pclose(na);
#ifdef HAVE_SETXATTR	/* extended attributes interface required */
	//if (fi->fh & CLOSE_ENCRYPTED)
		res = ntfs_efs_fixup_attribute(NULL, na);
#endif /* HAVE_SETXATTR */
#endif
exit:
    free(fp->f_data);
	if (na)
		ntfs_attr_close(na);
	if (ntfs_inode_close(ni));
		res = -errno;
	return res;
}
// -------------------------------------------------------------------------
static int ntfs_fo_fstat  (struct CYG_FILE_TAG *fp, struct stat *buf )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_fo_getinfo(struct CYG_FILE_TAG *fp, int key,
                            void *buf, int len )
{
   //TODO
}
// -------------------------------------------------------------------------
static int ntfs_fo_setinfo(struct CYG_FILE_TAG *fp, int key,
                            void *buf, int len )
{
   //TODO
}
// -------------------------------------------------------------------------
/*static int ntfs_ecos_is_named_data_stream(const char *path)
{
	if (strchr(path, ':') && ctx->streams == NF_STREAMS_INTERFACE_WINDOWS)
		return 1;
	return 0;
}*/
// -------------------------------------------------------------------------
static int ntfs_ecos_filler(void *buf,
		const ntfschar *name, const int name_len, const int name_type,
		const s64 pos, const MFT_REF mref,
		const unsigned dt_type __attribute__((unused)))
{
	char *filename = NULL;
	int ret = 0;
	int filenamelen = -1;
    struct dirent *ent = (struct dirent *)buf;
    char              *nbuf    = ent->d_name;
	if (name_type == FILE_NAME_DOS)
		return 0;
	ntfs_log_trace("entering\n");
	if ((filenamelen = ntfs_ucstombs(name, name_len, &filename, 0)) < 0) {
		ntfs_log_perror("Filename decoding failed (inode %llu)",
				(unsigned long long)MREF(mref));
		return -1;
	}
	ntfs_log_trace("Filename:%s\n",filename);
	//if (ntfs_fuse_is_named_data_stream(filename)) {
	if(0){
		ntfs_log_error("Unable to access '%s' (inode %llu) with "
				"current named streams access interface.\n",
				filename, (unsigned long long)MREF(mref));
		free(filename);
		return 0;
	} else {
		struct stat st = { .st_ino = MREF(mref) };
		 
		switch (dt_type) {
		case NTFS_DT_DIR :
			st.st_mode = S_IFDIR; 
			break;
		case NTFS_DT_LNK :
			st.st_mode = S_IFLNK | 0777;
			break;
		case NTFS_DT_FIFO :
			st.st_mode = S_IFIFO;
			break;
		case NTFS_DT_SOCK :
			st.st_mode = S_IFSOCK;
			break;
		case NTFS_DT_BLK :
			st.st_mode = S_IFBLK;
			break;
		case NTFS_DT_CHR :
			st.st_mode = S_IFCHR;
			break;
		default : /* unexpected types shown as plain files */
		case NTFS_DT_REG :
			st.st_mode = S_IFREG;
			break;
		}
		
#if defined(__APPLE__) || defined(__DARWIN__)
		/* 
		 * Returning file names larger than MAXNAMLEN (255) bytes
		 * causes Darwin/Mac OS X to bug out and skip the entry. 
		 */
		if (filenamelen > MAXNAMLEN) {
			ntfs_log_debug("Truncating %d byte filename to "
				       "%d bytes.\n", filenamelen, MAXNAMLEN);
			ntfs_log_debug("  before: '%s'\n", filename);
			memset(filename + MAXNAMLEN, 0, filenamelen - MAXNAMLEN);
			ntfs_log_debug("   after: '%s'\n", filename);
		}
#endif /* defined(__APPLE__) || defined(__DARWIN__) */
	
		//ret = fill_ctx->filler(fill_ctx->buf, filename, &st, 0);
		memcpy(nbuf, filename,filenamelen);
        //ent->d_type = st.st_mode;
	}
	
	free(filename);
    if((pos == 0) ||(pos == 1)) return 0;
    else errno = ENOBUFS;
	return errno;
}

// -------------------------------------------------------------------------

static int ntfs_fo_dirread (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    //ntfs_fuse_fill_context_t fill_ctx;
	ntfs_inode *ni;
	s64 pos = fp->f_offset;
	int err = 0;
    ntfs_volume *vol = (ntfs_volume*)fp->f_mte->data;
    struct dirent     *ent     = (struct dirent *) uio->uio_iov[0].iov_base;
    char              *nbuf    = ent->d_name;
    off_t              len     = uio->uio_iov[0].iov_len;
    char *path = fp->f_data;
    memset(ent,0,len);
	ni = ntfs_pathname_to_inode(vol, NULL, path);
	if (!ni)
		return errno;
	if (ntfs_readdir(ni, &pos, ent,
			(ntfs_filldir_t)ntfs_ecos_filler))
		err = errno;
	//ntfs_inode_update_times(ni, mask);
	if (ntfs_inode_close(ni));
    if(errno == ENOBUFS)
    {
        ntfs_log_trace("right\n");
        fp->f_offset = pos+2;
        uio->uio_resid -= sizeof(struct dirent);
        err = 0;
    }
		//set_fuse_error(&err);
	return err;
}
// -------------------------------------------------------------------------
static int ntfs_fo_dirlseek(struct CYG_FILE_TAG *fp, off_t *pos, int whence)
{
   //TODO
}
// -------------------------------------------------------------------------
















