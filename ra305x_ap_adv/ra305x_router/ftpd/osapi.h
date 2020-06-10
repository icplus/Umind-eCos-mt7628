/*****************************************************************************
 * $File:   osapi.h
 *
 * $Author: Hua Shao
 * $Date:   August, 2013
 *
 * QSB0aW55IEZUUCBzZXJ2ZXIgYnkgSHVhIFNoYW8uIChzaGhlcmVAZ21haWwuY29tKQ==
 *****************************************************************************/


#ifndef OSAPI_H
#define OSAPI_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cyg/kernel/kapi.h>
#include <cyg/fileio/fileio.h>
#include <pkgconf/system.h>
#include <pkgconf/net.h>
#include <cyg/infra/diag.h>


#define WEBLOG_API()        printf("<api> %s, L%d, %s.\n",__FUNCTION__,__LINE__,__FILE__)
#define WEBLOG_INFO(...)    SHFTPD_DEBUG(__VA_ARGS__)
#define WEBLOG_ERROR(...)   SHFTPD_DEBUG(__VA_ARGS__)

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define dot_dir(s)      (!(s[s[0]!='.'?0:s[1]!='.'?1:2]!=0))

#ifndef NO_STDC_BUF
#define NO_STDC_BUF
#endif



/* API Hack! */

int     endswith(char * str, char * suffix);
int     startswith(char * str, char * prefix);

#ifdef NO_STDC_BUF
int     ftp_fopen(const char * path, int mode);
#else
FILE *  ftp_fopen(const char * path, const char * mode);
#endif
int     ftp_stat(const char * path, struct stat * buf);
int     ftp_lstat(const char * path, struct stat * buf);
int     ftp_access(const char * pathname, int mode);
int     ftp_remove(const char * pathname);
int     ftp_unlink(const char * pathname);
char *  ftp_getcwd(char * buf, size_t size);
int     ftp_chdir(const char * pathname);
DIR *   ftp_opendir(const char * name);
int     ftp_mkdir(const char * path);
int     ftp_rmdir(const char * path);
int     ftp_move(const char * from, const char * to);

int     thread_data_set(void * data);
void *  thread_data_get(void);
int     thread_data_del(void);

void    timing(char * str);

void    shftpd_debug(const char *file, int line, const char *fmt, ...);
#define SHFTPD_DEBUG(fmt, ...) \
            shftpd_debug(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define SHFTPD_TRACE(...) \
            printf("<trace> %s,%s, L%d, %s.\n", __VA_ARGS__, __FUNCTION__,__LINE__,__FILE__)
cyg_handle_t get_thread_by_name(const char * name);


#endif /* OSAPI_H */

