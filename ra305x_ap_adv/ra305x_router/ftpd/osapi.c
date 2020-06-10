/*****************************************************************************
 * $File:   osapi.c
 *
 * $Author: Hua Shao
 * $Date:   August, 2013
 *
 * QSB0aW55IEZUUCBzZXJ2ZXIgYnkgSHVhIFNoYW8uIChzaGhlcmVAZ21haWwuY29tKQ==
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <dirent.h>

#include "osapi.h"
#include "ftp.h"


/* In a multi-thread ftp server, we must keep in mind that each client may be
   a totally independent user. They all have no idea about operations which
   is going on in other users' thread. They don't care.

   But as for the server, it must be careful to avoid the interference with each
   other. It's server's responsibility to make users believe that they are the
   only one in the system. This is especially important to those global data.
   mixturing them together will cause terrible chaos in the server.

   Basically we can rely on the mechanism provided by pthread library. ( pthread
   supports storing and acquiring thread specific data.) But unfortunately the
   target system we are about to run on does not support pthread interface.

   We have to implement the mechanism ourself.
*/

typedef struct _TSD  /* TSD -> Thread Specific Data */
{
    cyg_handle_t key;
    void * data;
    struct _TSD * next;
} TSD;


static TSD _tsd_ = {0, NULL, NULL};


void * thread_data_get(void)
{
    TSD * tsd = &_tsd_;
    cyg_handle_t tid = cyg_thread_self();

    while(tsd)
    {
        if(tsd->key == tid)
            return tsd->data;
        tsd = tsd->next;
    }

    return NULL;
}


int thread_data_set(void * data)
{
    TSD * tsd = NULL;
    cyg_handle_t tid = cyg_thread_self();

    WEBLOG_API();

    tsd = thread_data_get();
    if(tsd)
    {
        // key found, just update the data.
        tsd->data = data;
    }
    else
    {
        // not found, create a new TSD.
        tsd = (TSD *)malloc(sizeof(TSD));
        if(!tsd)
            return -1;

        // insert into the chain.
        tsd->key = tid;
        tsd->data = data;
        tsd->next = _tsd_.next;
        _tsd_.next = tsd;
    }

    return 0;
}

int thread_data_del(void)
{
    TSD * cur = _tsd_.next;
    TSD * pre = &_tsd_;
    cyg_handle_t tid = cyg_thread_self();

    while(cur)
    {
        if(cur->key == tid)
        {
            pre->next = cur->next;
            free(cur);
            break;
        }
        pre = cur;
        cur = cur->next;
    }

    return 0;
}


static char * get_abs_path(const char * path, char * abspath, size_t size)
{
    /* absolute path */
    if (path[0] == '/')
    {
        sprintf(abspath, path);
    }
    /* relative path */
    else
    {
        ftp_getcwd(abspath, size);
        if(0 != endswith(abspath, "/"))
            strcat(abspath, "/");
        strcat(abspath, path);
    }

    //SHFTPD_DEBUG("<%s> [%s]=[%s]\n", __FUNCTION__, path, abspath);

    return abspath;
}


static int is_pathinroot(const char * path)
{
    char * p = NULL;
    char * q = NULL;

    if('/' != path[0])
        return 0;
    if(0 == path[1])
        return 1;

    /*  /aaa/
        /aaa */
    p = (char *)path+1;
    q = strchr(p, '/');

    if(!q || q == p+strlen(p)-1)
        return 1;

    return 0;
}



int is_folder(char * path)
{
    int ret = 0;
    struct stat buf;

    ret = stat(path, &buf);
    if(ret != 0)
    {
        SHFTPD_DEBUG("<%s> Unexpected error %d in _stat.\n", __FUNCTION__, ret);
        return FALSE;
    }
    else if(S_IFDIR & buf.st_mode)
        return TRUE;

    return FALSE;
}


static char * path_cat(char * path1, char * path2)
{
    if('/' == path1[strlen(path1)-1])
        strcat(path1, path2);
    else
    {
        strcat(path1, "/");
        strcat(path1, path2);
    }
    return path1;
}

#ifdef NO_STDC_BUF
int ftp_fopen(const char * path, int mode)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s, %d)\n", __FUNCTION__, path, mode);

    get_abs_path(path, vpath, sizeof(vpath));

    if(is_pathinroot(vpath))
        return -1;

    return open(vpath, mode);
}
#else
FILE * ftp_fopen(const char * path, const char * mode)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s, %s)\n", __FUNCTION__, path, mode);

    get_abs_path(path, vpath, sizeof(vpath));

    if(is_pathinroot(vpath))
        return NULL;

    return fopen(vpath, mode);
}
#endif


int ftp_stat(const char * path, struct stat * buf)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s, %p)\n", __FUNCTION__, path, buf);

    get_abs_path(path, vpath, sizeof(vpath));

    return stat(vpath, buf);
}

int ftp_lstat(const char * path, struct stat * buf)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s, %p)\n", __FUNCTION__, path, buf);

    get_abs_path(path, vpath, sizeof(vpath));

    return stat(vpath, buf);
}



int ftp_remove(const char * path)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, vpath, sizeof(vpath));

    if(is_pathinroot(vpath))
        return -1;

    return remove(vpath);
}

int ftp_unlink(const char * path)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, vpath, sizeof(vpath));

    if(is_pathinroot(vpath))
        return -1;

    return unlink(vpath);
}


int ftp_access(const char * path, int mode)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s, %d)\n", __FUNCTION__, path, mode);

    get_abs_path(path, vpath, sizeof(vpath));

    return access(vpath, mode);
}


char * ftp_getcwd(char * buf, size_t size)
{
    char * root = (char *)thread_data_get();

    if(!buf)
    {
        if(root)
        {
            SHFTPD_DEBUG("current dir [%s]\n", root);
            return root;
        }
        else
        {
            SHFTPD_DEBUG("current dir [NULL]!\n","");
            return NULL;
        }
    }

    if(root)
    {
        strncpy(buf, root, size);
        SHFTPD_DEBUG("current dir [%s]\n", buf);
        return buf;
    }
    else
    {
        SHFTPD_DEBUG("%s() fail, key not found!\n", __FUNCTION__);
        return NULL;
    }

    return buf;
}


int ftp_chdir(const char * path)
{
    int ret = 0;
    char * root = NULL;
    char vpath[PATH_MAX] = {0};
    struct stat stbuf;

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, vpath, sizeof(vpath));

    ret = ftp_stat(vpath, &stbuf);
    if(ret != 0)
    {
        SHFTPD_DEBUG("<%s> stat(%s) error : %s .\n", __FUNCTION__, vpath, strerror(errno));
        return -1;
    }

    if(!(S_IFDIR & stbuf.st_mode))
    {
        SHFTPD_DEBUG("<%s> [%s] is not a folder!\n", __FUNCTION__, vpath);
        return -1;
    }

    if(0 != startswith(vpath, SHFTPD_ROOTDIR))
    {
        SHFTPD_DEBUG("<%s> illegal path [%s], outside the root!\n", __FUNCTION__, vpath);
        return -1;
    }

    root = (char *)thread_data_get();
    if(!root)
    {
        SHFTPD_DEBUG("<%s> error, key not found!\n", __FUNCTION__);
        return -1;
    }

    SHFTPD_DEBUG("<%s> from [%s] to [%s]\n", __FUNCTION__, root, vpath);
    sprintf(root, "%s", vpath);
    return 0;
}

DIR * ftp_opendir(const char * path)
{
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, vpath, sizeof(vpath));

    return opendir(vpath);
}


int ftp_mkdir(const char * path)
{
    int ret = 0;
    char vpath[PATH_MAX] = {0};

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, vpath, sizeof(vpath));

    if(is_pathinroot(vpath))
        return -1;

    ret = mkdir(vpath, 0777);
    if(0 != ret)
    {
        SHFTPD_DEBUG("<%s> Mission Fail : %s. \n", __FUNCTION__, strerror(errno));
        return -1;
    }
    else
    {
        SHFTPD_DEBUG("<%s> Mission Complete.\n", __FUNCTION__);
        return 0;
    }

}


int ftp_rmdir(const char * path)
{
    int ret = 0;
    char abs_path[PATH_MAX] = {0};
    char sub_path[PATH_MAX] = {0};
    struct dirent *ent  = NULL;
    DIR * dir = NULL;

    SHFTPD_DEBUG("<api> %s(%s)\n", __FUNCTION__, path);

    get_abs_path(path, abs_path, sizeof(abs_path));

    if('/' == abs_path[strlen(abs_path)-1])
        abs_path[strlen(abs_path)-1] = 0;

    if(is_pathinroot(abs_path))
        return -1;

    dir = opendir(abs_path);
    if(!dir)
    {
        SHFTPD_DEBUG("fail to opendir(%s)\n", __FUNCTION__, abs_path);
        return -1;
    }

    while((ent = readdir(dir)) != NULL)
    {
        if(dot_dir(ent->d_name)) continue;

        path_cat(abs_path, ent->d_name);
        if(is_folder(sub_path))
            ret = ftp_remove(sub_path);
        else
            ret = ftp_rmdir(sub_path);
        if(ret < 0) goto __quit;
    }
    closedir(dir);
    ret = rmdir(abs_path);

__quit:
    if(0 != ret)
    {
        SHFTPD_DEBUG("<%s> Mission Fail : %s. \n", __FUNCTION__, strerror(errno));
        return -1;
    }
    else
    {
        SHFTPD_DEBUG("<%s> Mission Complete.\n", __FUNCTION__);
        return 0;
    }
}

int ftp_move(const char * from, const char * to)
{
    int ret = 0;
    char abs_from[PATH_MAX] = {0};
    char abs_to[PATH_MAX] = {0};

    get_abs_path(from, abs_from, sizeof(abs_from));
    get_abs_path(to, abs_to, sizeof(abs_to));

    SHFTPD_DEBUG("<%s> from \"%s\" to \"%s\". \n",
        __FUNCTION__, abs_from, abs_to);

    if(is_pathinroot(abs_to))
        return -1;

    ret = rename(abs_from, abs_to);
    if(0 != ret)
    {
        WEBLOG_ERROR("<%s> Mission Fail : %s. \n", __FUNCTION__, strerror(errno));
        return -1;
    }
    else
    {
        WEBLOG_INFO("<%s> Mission Complete.\n", __FUNCTION__);
        return 0;
    }
}



int endswith(char * str, char * suffix)
{
    int len1 = strlen(str);
    int len2 = strlen(suffix);

    if(!str || ! suffix)
        return -1;

    if(len2>len1)
        return -1;

    return strncmp(str+len1-len2, suffix, len2);
}

int startswith(char * str, char * prefix)
{
    return strncmp(str, prefix, strlen(prefix));
}


void timing(char * str)
{
    time_t tm = time(&tm);
    diag_printf("<%s> %d, %s\n", __FUNCTION__, (int)tm, str);
}


void shftpd_debug(const char *file, int line, const char *fmt, ...)
{
    va_list ap;
    static int shftpd_debug_on = 1;

    if (!shftpd_debug_on)
        return;

    fprintf(stdout, "(%s:%d:%u)", file, line, cyg_thread_self());
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

cyg_handle_t get_thread_by_name(const char * name)
{
    cyg_handle_t thread = 0;
    cyg_uint16 id = 0;
    int more = 0;

    do
    {
        cyg_thread_info info;
        more=cyg_thread_get_next(&thread, &id );
        if (thread==0)
            break;

        cyg_thread_get_info(thread, id, &info );
        if(!info.name)
            continue;

        if(0 == strcmp(name, info.name))
        {
            //SHFTPD_DEBUG("<%s> found %s, id %d, handle %u.\n",
            //    __FUNCTION__, info.name, id, thread);
            return thread;
        }
    } while (more) ;

    return 0;
}


