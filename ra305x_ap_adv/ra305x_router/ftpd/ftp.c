/*****************************************************************************
 * $File:   ftp.c
 *
 * $Author: Hua Shao
 * $Date:   August, 2013
 *
 * QSB0aW55IEZUUCBzZXJ2ZXIgYnkgSHVhIFNoYW8uIChzaGhlcmVAZ21haWwuY29tKQ==
 *****************************************************************************/



#include <unistd.h>


#include <netdb.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* MS Windows also support these POSIX headers. */
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "ftp.h"
#include "osapi.h"

#undef  FTPDATA_BUFSIZ
#define FTPDATA_BUFSIZ (1024*20)



/* local functions declarations */
static int shftpd_cmd_user(int,char*);
static int shftpd_cmd_pass(int,char*);
static int shftpd_cmd_pwd(int, char*);
static int shftpd_cmd_cwd(int, char*);
static int shftpd_cmd_mkd(int, char*);
static int shftpd_cmd_rmd(int, char*);
static int shftpd_cmd_syst(int,char*);
static int shftpd_cmd_noop(int,char*);
static int shftpd_cmd_list(int,char*);
static int shftpd_cmd_size(int,char*);
static int shftpd_cmd_dele(int,char*);
static int shftpd_cmd_type(int,char*);
static int shftpd_cmd_port(int,char*);
static int shftpd_cmd_pasv(int,char*);
static int shftpd_cmd_retr(int,char*);
static int shftpd_cmd_stor(int,char*);
static int shftpd_cmd_rnfr(int,char*);
static int shftpd_cmd_rnto(int,char*);
static int shftpd_cmd_cdup(int,char*);
static int shftpd_cmd_opts(int,char*);
static int shftpd_cmd_quit(int,char*);

/* global variables */
int shftpd_srv_port = SHFTPD_DEF_SRV_PORT;
int shftpd_cur_child_num;
int shftpd_quit_flag;
struct shftpd_user_struct *shftpd_cur_user;
int shftpd_pasv_fd = -1;
int shftpd_pasv_connfd = -1;
int shftpd_port_connfd = -1;
char root_dir[PATH_MAX] = {0};

/* in some cases, ftp uses 2 commands to perform a single operation.
   for example, "RNFR" and "RNTO" perform "rename" operation. Thus we
   have to remember the path specified in the 1st command.
*/
char ftp_path_standby[PATH_MAX] = {0};



typedef struct
{
    char            thread_name[16];
    cyg_thread      thread_info;
    char *          thread_stack;
    cyg_handle_t    thread_handle;
    int             connfd;
} FtpdThreadInfo;

/*
* Currently supported ftp commands.
*/
struct shftpd_cmd_struct shftpd_cmds[] =
{
    {"USER", shftpd_cmd_user},
    {"PASS", shftpd_cmd_pass},
    {"PWD",  shftpd_cmd_pwd},
    {"CWD",  shftpd_cmd_cwd},
    {"MKD",  shftpd_cmd_mkd},
    {"RMD",  shftpd_cmd_rmd},
    {"XPWD", shftpd_cmd_pwd},
    {"LIST", shftpd_cmd_list},
    {"NLST", shftpd_cmd_list},
    {"SYST", shftpd_cmd_syst},
    {"NOOP", shftpd_cmd_noop},
    {"TYPE", shftpd_cmd_type},
    {"SIZE", shftpd_cmd_size},
    {"DELE", shftpd_cmd_dele},
    {"RMD",  shftpd_cmd_dele},
    {"PORT", shftpd_cmd_port},
    {"PASV", shftpd_cmd_pasv},
    {"RETR", shftpd_cmd_retr},
    {"STOR", shftpd_cmd_stor},
    {"RNFR", shftpd_cmd_rnfr},
    {"RNTO", shftpd_cmd_rnto},
    {"CDUP", shftpd_cmd_cdup},
    {"OPTS", shftpd_cmd_opts},
    {"QUIT", shftpd_cmd_quit},
    { NULL,         NULL}
};

/*
* Anonymous users, you can add more user/pass pairs here.
*/
struct shftpd_user_struct shftpd_users[] =
{
    {"anonymous", ""},
    {"ftp", ""}
};

/*
* Ftp server's responses.
*/
char shftpd_srv_resps[][256] =
{
    "150 Begin transfer\r\n",
    "200 OK\r\n",
    "213 %d\r\n",
    "215 UNIX Type: L8\r\n",
    "220 SHFTPD " SHFTPD_VER " Server\r\n",
    "221 Goodbye\r\n",
    "226 Transfer complete\r\n",
    "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",
    "230 User %s logged in\r\n",
    "250 Requested file action OK, completed.\r\n",
    "257 \"%s\" OK.\r\n",
    "331 Password required for %s\r\n",
    "350 Requested file action OK, pending further information.\r\n",
    "450 Requested file action NG, file not available.\r\n",
    "500 Unsupport command %s\r\n",
    "530 Login %s\r\n",
    "550 Error\r\n",
};

#define FTPD_DEF_PRIORITY           (8)
#define FTPD_HANDLE_STACKSIZE       (1024*8)
#ifndef NO_STDC_BUF
#define NO_STDC_BUF                 (1)
#endif
/* daemon!  */
#define FTPD_DAEMON_STACKSIZE        (1024*5)
static cyg_handle_t ftpd_daemon_hdl;
static char ftpd_daemon_stack[FTPD_DAEMON_STACKSIZE];
static cyg_thread ftpd_daemon_thread;


static int thread_counter = 0;
static int thread_id = 0;





//extern int url_encode(char * url, char * newurl);
//extern int url_decode(char * url);

typedef unsigned char uchar;

int url_encode(char * url, char * newurl)
{
    uchar * p   = (uchar *)url;
    char buf[9] = {0};

    WEBLOG_API();

   // ASSERT(newurl);
   // ASSERT(url);

    while(*p > 0)
    {
        if(*p > 'z' || *p < ' ')
            snprintf(buf, sizeof(buf), "%%%02x", (unsigned int)*p);
        else
            snprintf(buf, sizeof(buf), "%c", *p);
        strcat(newurl, buf);
        p++;
    }

#ifdef WEBCFG_DEBUG
    dumphex(newurl, url, strlen(url));
#endif
    return 0;
}


int url_decode(char * url)
{
    char *dest  = url;
    char *data  = url;
    int len     = strlen(url);

    while (len-- > 0)
    {
        //if (*data == '+')
        //{
        //    *dest = ' ';
        //}
        //else
        if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2)))
        {
            *dest = (char) dwordtoi(data + 1);
            data += 2;
            len -= 2;
        }
        else
        {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = 0;
    return dest - url;
}








int shftpd_thread_init(void)
{
    char * root = NULL;
    cyg_handle_t tid = cyg_thread_self();

    WEBLOG_API();

    thread_counter++;
    thread_id++;

    SHFTPD_DEBUG("<%s> thread[%04d-%u]: thread %d activated ...\n",
        __FUNCTION__, thread_id, tid, thread_counter);

    root = (char *)malloc(1024);
    if(!root) return -1;

    snprintf(root, 1024, "%s", SHFTPD_ROOTDIR);
    thread_data_set(root);

    return 0;
}

int shftpd_thread_uninit(void * pvarg)
{
    char * root = NULL;

    WEBLOG_API();

    root = (char *)thread_data_get();
    if(root) free(root);

    thread_data_del();
    thread_counter--;

    return 0;
}


/*
* Create ftp server's listening socket.
*/
static int shftpd_create_srv(void)
{
    int fd;
    int err;
    struct sockaddr_in srvaddr;

    WEBLOG_API();

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        SHFTPD_DEBUG("socket() failed: %s\n", strerror(errno));
        return fd;
    }

#if 1
    int on = 1;
    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (err < 0)
    {
        SHFTPD_DEBUG("setsockopt() failed: %s\n", strerror(errno));
        close(fd);
        return SHFTPD_NG;
    }
#endif
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(shftpd_srv_port);
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(fd, (struct sockaddr*)&srvaddr, sizeof(srvaddr));
    if (err < 0)
    {
        SHFTPD_DEBUG("bind() failed: %s\n", strerror(errno));
        close(fd);
        return SHFTPD_NG;
    }

    err = listen(fd, SHFTPD_LISTEN_QU_LEN);
    if (err < 0)
    {
        SHFTPD_DEBUG("listen() failed: %s\n", strerror(errno));
        close(fd);
        return SHFTPD_NG;
    }

    return fd;

}

/*
* Map server response's number to the msg pointer.
*/
static char * shftpd_srv_resp_num2msg(int num)
{
    int i;
    char buf[8];

    WEBLOG_API();


    snprintf(buf, sizeof(buf), "%d", num);
    if (strlen(buf) != 3)
        return NULL;

    for (i = 0; i < SHFTPD_ARR_LEN(shftpd_srv_resps); i++)
        if (strncmp(buf, shftpd_srv_resps[i], strlen(buf)) == 0)
            return shftpd_srv_resps[i];

    return NULL;
}

static int shftpd_send_data(int fd, char *msg, int len)
{
    int n, off = 0, left = len;

    while (1)
    {
        n = send(fd, msg + off, left, 0);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            return n;
        }
        if (n < left)
        {
            left -= n;
            off += n;
            continue;
        }
        return len;
    }
}

static int shftpd_recv_msg(int fd, char buf[], int len)
{
    int n;
    while (1)
    {
        n = recv(fd, buf, len, 0);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            return n;
        }
        return n;
    }
}

static int shftpd_send_resp(int fd, int num, ...)
{
    char *cp = shftpd_srv_resp_num2msg(num);
    va_list ap;
    char strbuf[PATH_MAX];

    SHFTPD_DEBUG("%d\n", num);

    if (!cp)
    {
        SHFTPD_DEBUG("shftpd_srv_resp_num2msg(%d) failed\n", num);
        return SHFTPD_NG;
    }

    va_start(ap, num);
    vsnprintf(strbuf, sizeof(strbuf), cp, ap);
    va_end(ap);

    SHFTPD_DEBUG("send resp:%s\n", strbuf);
    if (shftpd_send_data(fd, strbuf, strlen(strbuf)) != strlen(strbuf))
    {
        SHFTPD_DEBUG("shftpd_send_data() failed: %s\n", strerror(errno));
        return SHFTPD_NG;
    }

    return SHFTPD_OK;
}

static void shftpd_trim_lineend(char *cp)
{
    if (cp && strlen(cp) > 0)
    {
        char *cp2 = &cp[strlen(cp) - 1];

        while (*cp2 == '\r' || *cp2 == '\n')
            if (--cp2 < cp)
                break;
        cp2[1] = '\0';
    }
}

static int shftpd_get_connfd(void)
{
    int fd;

    if (shftpd_pasv_fd >= 0)
    {
        fd = accept(shftpd_pasv_fd, NULL, NULL);
        if (fd >= 0)
        {
            close(shftpd_pasv_fd);
            shftpd_pasv_fd = -1;
            shftpd_pasv_connfd = fd;
            return fd;
        }
        else
            SHFTPD_DEBUG("accept() failed:%s\n", strerror(errno));
    }
    else if (shftpd_port_connfd >= 0)
        return shftpd_port_connfd;

    return (-1);
}

static int shftpd_close_all_fd(void)
{

    WEBLOG_API();

    if (shftpd_pasv_fd >= 0)
    {
        close(shftpd_pasv_fd);
        shftpd_pasv_fd = -1;
    }
    if (shftpd_pasv_connfd >= 0)
    {
        close(shftpd_pasv_connfd);
        shftpd_pasv_connfd = -1;
    }
    if (shftpd_port_connfd >= 0)
    {
        close(shftpd_port_connfd);
        shftpd_port_connfd = -1;
    }
    return SHFTPD_OK;
}

static int shftpd_cmd_user(int ctrlfd, char *cmdline)
{
    char *cp = strchr(cmdline, ' ');

    WEBLOG_API();

    if (cp)
    {
        int i;

        for (i = 0; i < SHFTPD_ARR_LEN(shftpd_users); i++)
            if (strcmp(cp + 1, shftpd_users[i].user) == 0)
            {
                SHFTPD_DEBUG("user(%s) is found\n", cp + 1);
                shftpd_cur_user = &shftpd_users[i];
                break;
            }

        if (!shftpd_cur_user)
            SHFTPD_DEBUG("user(%s) not found\n", cp + 1);

        /*
         * If user name is bad, we still don't close the connection
         * and send back the 331 response to ask for password.
         */
        return shftpd_send_resp(ctrlfd, 331, cp + 1);
    }

    return shftpd_send_resp(ctrlfd, 550);
}

static int shftpd_cmd_pass(int ctrlfd, char *cmdline)
{
    char *space = strchr(cmdline, ' ');

    WEBLOG_API();

    if (shftpd_cur_user && space)
    {
        if (strlen(shftpd_cur_user->pass) == 0 ||
            strcmp(space + 1, shftpd_cur_user->pass) == 0)
        {
            SHFTPD_DEBUG("password for %s OK\n", shftpd_cur_user->user);
            return shftpd_send_resp(ctrlfd, 230, shftpd_cur_user->user);
        }
        SHFTPD_DEBUG("password for %s ERR\n", shftpd_cur_user->user);
    }

    /*
     * User and pass don't match or
     * cmd line does not contain a space character.
     */
    shftpd_cur_user = NULL;
    return shftpd_send_resp(ctrlfd, 530, "incorrect");
}

static int shftpd_cmd_pwd(int ctrlfd, char *cmdline)
{
    char curdir[PATH_MAX];
    char *cp;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    ftp_getcwd(curdir, sizeof(curdir));
    if(0 != startswith(curdir, SHFTPD_ROOTDIR))
    {
        return shftpd_send_resp(ctrlfd, 550);
    }
    else if(0 != strcmp(SHFTPD_ROOTDIR, "/"))
    {
        cp = &curdir[strlen(SHFTPD_ROOTDIR)]; // alway return relative path!
    }
    else
    {
        cp = curdir;
    }
    SHFTPD_DEBUG("cwd == [%s]!\n", curdir);
    SHFTPD_DEBUG("home == [%s]!\n", SHFTPD_ROOTDIR);
    SHFTPD_DEBUG("cp == [%s]!\n", cp);
    return shftpd_send_resp(ctrlfd, 257, (*cp == '\0') ? "/" : cp);
}

static int shftpd_cmd_cwd(int ctrlfd, char *cmdline)
{
    char * path = NULL;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    path = strchr(cmdline, ' ');
    if(!path)
    {
        SHFTPD_DEBUG("invalid cmd!","\n");
        return shftpd_send_resp(ctrlfd, 550);
    }

    path++; // skip space!

#if 1
    if (0 == strcmp(path, ".."))
    {
        return shftpd_cmd_cdup(ctrlfd, NULL);
    }

    if(ftp_chdir(path) < 0)
        return shftpd_send_resp(ctrlfd, 550);
    else
        return shftpd_send_resp(ctrlfd, 200);
#else
    ftp_getcwd(curdir, sizeof(curdir));
    SHFTPD_DEBUG("ftp_getcwd => %s\n", curdir);
    if (strcmp(curdir, root_dir) == 0 &&
        space[1] == '.' &&
        space[2] == '.')
        return shftpd_send_resp(ctrlfd, 550);

    /* Absolute path */
    if (space[1] == '/')
    {
        SHFTPD_DEBUG("absolute path [%s]!\n", space);
        if (ftp_chdir(root_dir) == 0)
        {
            SHFTPD_DEBUG("ftp_chdir(%s) ok!\n", root_dir);
            if (space[2] == '\0' || ftp_chdir(space+2) == 0)
            {
                SHFTPD_DEBUG("ftp_chdir(%s) ok!\n", space+2);
                return shftpd_send_resp(ctrlfd, 250);
            }
            else
                SHFTPD_DEBUG("%s, %s, error %s\n", space, space+2, strerror(errno));
        }
        else
            SHFTPD_DEBUG("ftp_chdir(%s) error, %s!\n", root_dir, strerror(errno));
        ftp_chdir(curdir);
        return shftpd_send_resp(ctrlfd, 550);
    }
    else
    {
        SHFTPD_DEBUG("relative path [%s]!\n", space);
        /* Relative path */
        if (ftp_chdir(space+1) == 0)
            return shftpd_send_resp(ctrlfd, 250);
        else
            SHFTPD_DEBUG("ftp_chdir(%s) error, %s!\n", root_dir, strerror(errno));
    }
    ftp_chdir(curdir);
    SHFTPD_DEBUG("unknow error!\n");
#endif

    return shftpd_send_resp(ctrlfd, 550);
}


static int shftpd_cmd_mkd(int ctrlfd, char *cmdline)
{
    char * vpath = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if(!vpath)
    {
        SHFTPD_DEBUG("invalid cmd!","\n");
        return shftpd_send_resp(ctrlfd, 550);
    }

    vpath++; // skip space!

    SHFTPD_DEBUG("path => [%s]\n", vpath);

    if(0 == ftp_access(vpath, F_OK))
    {
        SHFTPD_DEBUG("path [%s] already exists!\n", vpath);
        return shftpd_send_resp(ctrlfd, 550);
    }

    if(0 == ftp_mkdir(vpath))
    {
        return shftpd_send_resp(ctrlfd, 257, vpath);
    }
    else
    {
        return shftpd_send_resp(ctrlfd, 550);
    }
}


static int shftpd_cmd_rmd(int ctrlfd, char *cmdline)
{
    char * vpath = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if(!vpath)
    {
        SHFTPD_DEBUG("invalid cmd!","\n");
        return shftpd_send_resp(ctrlfd, 550);
    }

    vpath++; // skip space!

    SHFTPD_DEBUG("path => [%s]\n", vpath);

    if(0 != ftp_access(vpath, F_OK))
    {
        SHFTPD_DEBUG("path [%s] not available!\n", vpath);
        return shftpd_send_resp(ctrlfd, 550, vpath);
    }

    if(0 == ftp_rmdir(vpath))
    {
        return shftpd_send_resp(ctrlfd, 250);
    }
    else
    {
        return shftpd_send_resp(ctrlfd, 550);
    }
}



/*
* This function acts as a implementation like 'ls -l' shell command.
*/
static int shftpd_get_list(char buf[], int len)
{
    DIR *dir = NULL;
    struct dirent * ent = NULL;
    int off = 0;
    char vpath[256] = {0};

    WEBLOG_API();

	memset(vpath,0,sizeof(vpath));
    ftp_getcwd(vpath, sizeof(vpath));
    SHFTPD_DEBUG("list %s:\n", vpath);

    dir = ftp_opendir(vpath);
    if (!dir)
    {
        SHFTPD_DEBUG("ftp_opendir() failed:%s\n", strerror(errno));
        return SHFTPD_NG;
    }

    buf[0] = '\0';

    while ((ent = readdir(dir)) != NULL)
    {
        char fullpath[256] = {0};
        char * filename = ent->d_name;
        struct stat st;
        char mode[] = "----------";
#if 0  /* eCos does not support pwd & grp API. */
        struct passwd *pwd;
        struct group *grp;
#endif
        struct tm *ptm ;
        char timebuf[64];
        int timelen;
		int tmp_len=0,src_len=0;

		memset(fullpath,0,sizeof(fullpath));
        if (strcmp(ent->d_name, ".") == 0 ||
            strcmp(ent->d_name, "..") == 0)
            continue;
		SHFTPD_DEBUG("<ent>d_name:%s  filename :%s\n\n",ent->d_name,filename);

		
		/*
		*when filename has chinese, snprintf() may turn out a incorrect result
		*/
		bcopy(vpath,fullpath,strlen(vpath));
        if(0 == endswith(vpath, "/"))
			;
            //snprintf(fullpath, sizeof(fullpath), "%s", vpath);
        else
			strcat(fullpath,"/");
            //snprintf(fullpath, sizeof(fullpath), "%s/", vpath);
		tmp_len = strlen(fullpath);
		src_len = strlen(filename);		
		bcopy(filename,fullpath+tmp_len,src_len);
        SHFTPD_DEBUG("<%s> found %s  src_len=%d\n", __FUNCTION__, fullpath,src_len);



        if (ftp_stat(fullpath, &st) < 0)
        {
            closedir(dir);
            SHFTPD_DEBUG("ftp_stat() failed:%s\n", strerror(errno));
            return SHFTPD_NG;
        }

        if (S_ISDIR(st.st_mode))
            mode[0] = 'd';
        if (st.st_mode & S_IRUSR)
            mode[1] = 'r';
        if (st.st_mode & S_IWUSR)
            mode[2] = 'w';
        if (st.st_mode & S_IXUSR)
            mode[3] = 'x';
        if (st.st_mode & S_IRGRP)
            mode[4] = 'r';
        if (st.st_mode & S_IWGRP)
            mode[5] = 'w';
        if (st.st_mode & S_IXGRP)
            mode[6] = 'x';
        if (st.st_mode & S_IROTH)
            mode[7] = 'r';
        if (st.st_mode & S_IWOTH)
            mode[8] = 'w';
        if (st.st_mode & S_IXOTH)
            mode[9] = 'x';
        mode[10] = '\0';
        off += snprintf(buf + off, len - off, "%s ", mode);

        /* hard link number, this field is nonsense for ftp */
        off += snprintf(buf + off, len - off, "%d ", 1);

#if 1
        /* user */
        off += snprintf(buf + off, len - off, "%s ", "ftp");
        /* group */
        off += snprintf(buf + off, len - off, "%s ", "ftp");
#else
        /* user */
        if ((pwd = getpwuid(st.st_uid)) == NULL)
        {
            closedir(dir);
            return SHFTPD_NG;
        }
        off += snprintf(buf + off, len - off, "%s ", pwd->pw_name);
        /* group */
        if ((grp = getgrgid(st.st_gid)) == NULL)
        {
            closedir(dir);
            return SHFTPD_NG;
        }
        off += snprintf(buf + off, len - off, "%s ", grp->gr_name);
#endif
        /* size */
        off += snprintf(buf + off, len - off, "%u ", 10, (int)st.st_size);

        /* mtime */
        ptm = localtime(&st.st_mtime);
        if (ptm && (timelen = strftime(timebuf, sizeof(timebuf), "%b %d %H:%S", ptm)) > 0)
        {
            timebuf[timelen] = '\0';
            off += snprintf(buf + off, len - off, "%s ", timebuf);
        }
        else
        {
            closedir(dir);
            return SHFTPD_NG;
        }

        url_encode(vpath, fullpath);
        char * p = fullpath + strlen(fullpath)-1;
        if(0 == endswith(fullpath, "/"))
            p++;
        url_encode(filename, p);

		/*
		*when filename has chinese, snprintf() may turn out a incorrect result
		*/
        //off += snprintf(buf + off, len - off, "%s\r\n", filename);
		strcat(filename,"\r\n");
		tmp_len = strlen(filename);
        SHFTPD_DEBUG("<%s> check %s  tmp_len=%d\n", __FUNCTION__, filename,tmp_len);		
		bcopy(filename,buf+off,((tmp_len< len-off)?tmp_len:len-off));
		off +=((tmp_len< len-off)?tmp_len:len-off);
    }
    SHFTPD_DEBUG("\n response:%s\n%s\n", strerror(errno), buf);
    if(dir) closedir(dir);

    return off;
}

static int shftpd_cmd_list(int ctrlfd, char *cmdline)
{
    char *buf = NULL;
    size_t buflen = 0;
    int n;
    int fd;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if ((fd = shftpd_get_connfd()) < 0)
    {
        SHFTPD_DEBUG("LIST cmd:no available fd%s", "\n");
        goto err_label;
    }

    shftpd_send_resp(ctrlfd, 150);

    buflen = 64*1024;
    do
    {
        buflen /= 2;
        buf = (char *)malloc(buflen);
    }
    while(!buf);

    /*
     * Get the 'ls -l'-like result and send it to client.
     */
    n = shftpd_get_list(buf, buflen);
    if (n >= 0)
    {
        if (shftpd_send_data(fd, buf, n) != n)
        {
            SHFTPD_DEBUG("shftpd_send_data() failed: %s\n", strerror(errno));
            goto err_label;
        }
        close(fd);
    }
    else
    {
        SHFTPD_DEBUG("shftpd_get_list() failed %s", "\n");
        goto err_label;
    }

    //shftpd_close_all_fd();
    if(buf) free(buf);
    return shftpd_send_resp(ctrlfd, 226);

err_label:
    if(buf) free(buf);
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 550);
}

static int shftpd_cmd_syst(int ctrlfd, char *cmdline)
{

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();
    return shftpd_send_resp(ctrlfd, 215);
}

static int shftpd_cmd_noop(int ctrlfd, char *cmdline)
{

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();
    return shftpd_send_resp(ctrlfd, 200);
}

static int shftpd_cmd_size(int ctrlfd, char *cmdline)
{
    char *space = strchr(cmdline, ' ');
    struct stat st;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if (!space || ftp_lstat(space + 1, &st) < 0)
    {
        SHFTPD_DEBUG("SIZE cmd err: %s\n", cmdline);
        return shftpd_send_resp(ctrlfd, 550);
    }

    return shftpd_send_resp(ctrlfd, 213, st.st_size);
}

static int shftpd_cmd_dele(int ctrlfd, char *cmdline)
{
    char * path = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if(!path)
    {
        SHFTPD_DEBUG("wrong path in cmd: %s\n", cmdline);
        return shftpd_send_resp(ctrlfd, 550);
    }
    path++;

    printf("--------L%d\n", __LINE__);

    if(0 != ftp_access(path, F_OK))
    {
        printf("--------L%d\n", __LINE__);
        SHFTPD_DEBUG("path %s does not exist!\n", path);
        return shftpd_send_resp(ctrlfd, 550);
    }
    printf("--------L%d\n", __LINE__);

    if (ftp_remove(path) < 0)
    {
        printf("--------L%d\n", __LINE__);
        SHFTPD_DEBUG("DELE [%s] fail : %s.\n", path, strerror(errno));
        return shftpd_send_resp(ctrlfd, 550);
    }
    printf("--------L%d\n", __LINE__);

    return shftpd_send_resp(ctrlfd, 200);
}

static int shftpd_cmd_type(int ctrlfd, char *cmdline)
{

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    /*
     * Just send back 200 response and do nothing
     */
    return shftpd_send_resp(ctrlfd, 200);
}

/*
* Parse PORT cmd and fetch the ip and port,
* and both in network byte order.
*/
static int shftpd_get_port_mode_ipport(char *cmdline, unsigned int *ip, unsigned short *port)
{
    char *cp = strchr(cmdline, ' ');
    int i;
    unsigned char buf[6];

    WEBLOG_API();

    if (!cp)
        return SHFTPD_NG;

    for (cp++, i = 0; i < SHFTPD_ARR_LEN(buf); i++)
    {
        buf[i] = atoi(cp);
        cp = strchr(cp, ',');
        if (!cp && i < SHFTPD_ARR_LEN(buf) - 1)
            return SHFTPD_NG;
        cp++;
    }

    if (ip)
        *ip = *(unsigned int*)&buf[0];

    if (port)
        *port = *(unsigned short*)&buf[4];

    return SHFTPD_OK;
}

/*
* Ftp client shipped with Windows XP uses PORT
* mode as default to connect ftp server.
*/
static int shftpd_cmd_port(int ctrlfd, char *cmdline)
{
    unsigned int ip;
    unsigned short port;
    struct sockaddr_in sin;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if (shftpd_get_port_mode_ipport(cmdline, &ip, &port) != SHFTPD_OK)
    {
        SHFTPD_DEBUG("shftpd_get_port_mode_ipport() failed%s", "\n");
        goto err_label;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip;
    sin.sin_port = port;

    SHFTPD_DEBUG("PORT cmd:%s:%d\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

    if (shftpd_port_connfd >= 0)
    {
        close(shftpd_port_connfd);
        shftpd_port_connfd = -1;
    }

    shftpd_port_connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (shftpd_port_connfd < 0)
    {
        SHFTPD_DEBUG("socket() failed:%s\n", strerror(errno));
        goto err_label;
    }

    if (connect(shftpd_port_connfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        SHFTPD_DEBUG("bind() failed:%s\n", strerror(errno));
        goto err_label;
    }

    SHFTPD_DEBUG("PORT mode connect OK%s", "\n");
    return shftpd_send_resp(ctrlfd, 200);

err_label:
    if (shftpd_port_connfd >= 0)
    {
        close(shftpd_port_connfd);
        shftpd_port_connfd = -1;
    }
    return shftpd_send_resp(ctrlfd, 550);

}

static int shftpd_cmd_pasv(int ctrlfd, char *cmdline)
{
    struct sockaddr_in pasvaddr;
    socklen_t len;
    unsigned int ip;
    unsigned short port;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if (shftpd_pasv_fd >= 0)
    {
        close(shftpd_pasv_fd);
        shftpd_pasv_fd = -1;
    }

    shftpd_pasv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (shftpd_pasv_fd < 0)
    {
        SHFTPD_DEBUG("socket() failed: %s\n", strerror(errno));
        return shftpd_send_resp(ctrlfd, 550);
    }

    /*
     * must bind to the same interface as ctrl connectin came from.
     */
    len = sizeof(pasvaddr);
    getsockname(ctrlfd, (struct sockaddr*)&pasvaddr, &len);
    pasvaddr.sin_port = 0;

    if (bind(shftpd_pasv_fd, (struct sockaddr*)&pasvaddr, sizeof(pasvaddr)) < 0)
    {
        SHFTPD_DEBUG("bind() failed: %s\n", strerror(errno));
        close(shftpd_pasv_fd);
        shftpd_pasv_fd = -1;
        return shftpd_send_resp(ctrlfd, 550);
    }

    if (listen(shftpd_pasv_fd, SHFTPD_LISTEN_QU_LEN) < 0)
    {
        SHFTPD_DEBUG("listen() failed: %s\n", strerror(errno));
        close(shftpd_pasv_fd);
        shftpd_pasv_fd = -1;
        return shftpd_send_resp(ctrlfd, 550);
    }

    len = sizeof(pasvaddr);
    getsockname(shftpd_pasv_fd, (struct sockaddr*)&pasvaddr, &len);
    ip = ntohl(pasvaddr.sin_addr.s_addr);
    port = ntohs(pasvaddr.sin_port);
    SHFTPD_DEBUG("local bind: %s:%d\n", inet_ntoa(pasvaddr.sin_addr), port);

    /*
     * write local ip/port into response msg
     * and send to client.
     */
    return shftpd_send_resp(ctrlfd, 227, (ip>>24)&0xff, (ip>>16)&0xff,
                           (ip>>8)&0xff, ip&0xff, (port>>8)&0xff, port&0xff);

}

static int shftpd_cmd_retr(int ctrlfd, char *cmdline)
{
    char *buf = NULL;
    size_t buflen = 0;
    char *space = strchr(cmdline, ' ');
    struct stat st;
    int n;
#ifdef NO_STDC_BUF
    int fd = -1;
#else
    FILE * fp = NULL;
#endif
    int datafd = -1;
    char * vpath = NULL;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if (!space || ftp_lstat(space + 1, &st) < 0)
    {
        SHFTPD_DEBUG("RETR cmd err: %s\n", cmdline);
        goto err_label;
    }

    if ((datafd = shftpd_get_connfd()) < 0)
    {
        SHFTPD_DEBUG("shftpd_get_connfd() failed%s", "\n");
        goto err_label;
    }

    shftpd_send_resp(ctrlfd, 150);


#if 1
    vpath = space+1;
/*
    ftp_getcwd(vpath, sizeof(vpath));
    if(0 != endswith(vpath, "/"))
        strcat(vpath, "/");
    strcat(vpath, space+1);
*/
#ifdef NO_STDC_BUF
    fd = ftp_fopen(vpath, O_RDONLY);
    if(fd < 0)
#else
    fp = ftp_fopen(vpath, "rb");
    if(!fp)
#endif
    {
        SHFTPD_DEBUG("ftp_fopen(%s) failed: %s\n", vpath, strerror(errno));
        goto err_label;
    }

    buflen = 512*1024;
    do
    {
        buflen /= 2;
        buf = (char *)malloc(buflen);
    }
    while(!buf);

    int total = st.st_size;

    time_t tm1;
    time_t tm2;
    int counter = 0;
    time(&tm1);
    while (1)
    {
#ifdef NO_STDC_BUF
        if ((n = read(fd, buf, buflen)) < 0)
#else
        if ((n = fread(buf, 1, buflen, fp)) < 0)
#endif
        {
            if (errno == EINTR)
                continue;
            SHFTPD_DEBUG("fread() failed: %s\n", strerror(errno));
            goto err_label;
        }
        //WEBLOG_INFO("<%s> progress: read %10d, ", __FUNCTION__, n);

        if (n == 0)
            break;

        if (shftpd_send_data(datafd, buf, n) != n)
        {
            SHFTPD_DEBUG("shftpd_send_data() failed: %s\n", strerror(errno));
            goto err_label;
        }
        //WEBLOG_INFO("send %10d to client\n", __FUNCTION__, n);
#if 0
        counter += n;
        time(&tm2);
        if((tm2-tm1)>0)
            SHFTPD_DEBUG(" %12u/%u %6uKB/s\n",
                counter, total, counter/(tm2-tm1)/1024);
#endif
    }

    SHFTPD_DEBUG("RETR(%s) OK\n", vpath);
    if(buf) free(buf);
#ifdef NO_STDC_BUF
    if(fd>=0) close(fd );
#else
    if(fp) fclose(fp);
#endif
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 226);

err_label:
    if(buf) free(buf);
#ifdef NO_STDC_BUF
    if(fd>=0) close(fd );
#else
    if(fp) fclose(fp);
#endif
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 550);

#else
    /* begin to read file and write it to conn socket */
    if ((fd = open(space + 1, O_RDONLY)) < 0)
    {
        SHFTPD_DEBUG("open() failed: %s\n", strerror(errno));
        goto err_label;
    }

    while (1)
    {
        if ((n = read(fd, buf, sizeof(buf))) < 0)
        {
            if (errno == EINTR)
                continue;
            SHFTPD_DEBUG("read() failed: %s\n", strerror(errno));
            goto err_label;
        }

        if (n == 0)
            break;

        if (shftpd_send_data(connfd, buf, n) != n)
        {
            SHFTPD_DEBUG("shftpd_send_data() failed: %s\n", strerror(errno));
            goto err_label;
        }
    }

    SHFTPD_DEBUG("RETR(%s) OK\n", space + 1);
    if (fd >= 0)
        close(fd);
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 226);

err_label:
    if (fd >= 0)
        close(fd);
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 550);

#endif
}

static int shftpd_cmd_stor(int ctrlfd, char *cmdline)
{
    char *buf = NULL;
    size_t buflen = 512*1024;
    char * vpath = strchr(cmdline, ' ');
    int n;
    int left, off;
    int connfd;
#ifdef NO_STDC_BUF
    int fd = -1;
#else
    FILE * fp = NULL;
#endif

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    /*
     * Should add some permission control mechanism here.
     */
    if (!vpath)// || ftp_lstat(space + 1, &st) == 0)
    {
        SHFTPD_DEBUG("STOR cmd err: %s\n", cmdline);
        goto err_label;
    }
    vpath++;

    if ((connfd = shftpd_get_connfd()) < 0)
    {
        SHFTPD_DEBUG("shftpd_get_connfd() failed%s", "\n");
        goto err_label;
    }

#ifdef NO_STDC_BUF
    fd = ftp_fopen(vpath, O_WRONLY | O_CREAT | O_TRUNC);
    if(fd < 0)
#else
    fp = ftp_fopen(vpath, "wb");
    if(!fp)
#endif
    {
        SHFTPD_DEBUG("ftp_fopen(%s) failed: %s\n", vpath, strerror(errno));
        goto err_label;
    }

    shftpd_send_resp(ctrlfd, 150);

    do
    {
        buflen /= 2;
        buf = (char *)malloc(buflen);
    }
    while(!buf);


    /* begin to read data from socket and wirte to disk file */
    time_t tm1;
    time_t tm2;
    int counter = 0;
    time(&tm1);
    while (1)
    {
        if ((n = shftpd_recv_msg(connfd, buf, buflen)) < 0)
        {
            SHFTPD_DEBUG("shftpd_recv_msg() failed: %s\n", strerror(errno));
            goto err_label;
        }
        //WEBLOG_INFO("<%s> progress: recv %10d ", __FUNCTION__, n);

        if (n == 0)
        {
            SHFTPD_DEBUG("recv 0, client closed? %s.\n", strerror(errno));
            break;
        }


#if 1
        #ifdef NO_STDC_BUF
        left = write(fd, buf + off, n);
        #else
        if ((left = fwrite(buf + off, 1, n, fp)) < 0)
        #endif
        if (left != n)
        {
            SHFTPD_DEBUG("write(%s) failed:%s. %d/%d\n", vpath, strerror(errno), left, n);
            //goto err_label;
        }

#else
        left = n;
        off = 0;
        while (left > 0)
        {
            int nwrite;
#ifdef NO_STDC_BUF
            if ((nwrite = write(fd, buf + off, left)) < 0)
#else
            if ((nwrite = fwrite(buf + off, 1, left, fp)) < 0)
#endif
            {
                if (errno == EINTR)
                    continue;
                SHFTPD_DEBUG("write(%s) failed:%s\n", vpath, strerror(errno));
                goto err_label;
            }
            //WEBLOG_INFO("%d -> %s\n", __FUNCTION__,nwrite/n, vpath);
            off += nwrite;
            left -= nwrite;
        }
#endif

#if 0
        counter += n;
        time(&tm2);
        if((tm2-tm1)>0)
            SHFTPD_DEBUG(" %12u/* %6uKB/s\n",
                counter, counter/(tm2-tm1)/1024);
#endif
    }

    SHFTPD_DEBUG("STOR(%s) OK\n", vpath);

#ifdef NO_STDC_BUF
    if(fd>=0) close(fd );
#else
    if(fp) fclose(fp);
#endif
    if(buf) free(buf);
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 226);

err_label:
#ifdef NO_STDC_BUF
    if(fd>=0) close(fd );
#else
    if(fp) fclose(fp);
#endif
    if(buf) free(buf);
    shftpd_close_all_fd();
    return shftpd_send_resp(ctrlfd, 550);
}


static int shftpd_cmd_rnfr(int ctrlfd, char *cmdline)
{
    char * space = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    SHFTPD_DEBUG("space => [%s]\n", space);

    if(0 == ftp_access(space+1, F_OK))
    {
        snprintf(ftp_path_standby, sizeof(ftp_path_standby),
            "%s", space +1);
        return shftpd_send_resp(ctrlfd, 350);
    }
    else
    {
        ftp_path_standby[0] = 0;
        return shftpd_send_resp(ctrlfd, 450);
    }
}


static int shftpd_cmd_rnto(int ctrlfd, char *cmdline)
{
    char * space = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    SHFTPD_DEBUG("space => [%s]\n", space);

    if(0 == ftp_move(ftp_path_standby, space+1))
    {
        ftp_path_standby[0] = 0;
        return shftpd_send_resp(ctrlfd, 250);
    }
    else
    {
        ftp_path_standby[0] = 0;
        return shftpd_send_resp(ctrlfd, 550);
    }
}


static int shftpd_cmd_cdup(int ctrlfd, char *cmdline)
{
    char path[PATH_MAX] = {0};
    char * p = NULL;

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    ftp_getcwd(path, sizeof(path));
    /*
        1 [/]           already root
        2 [/aa/bbb]     no trailing slash
        3 [/aa/bbb/]    with trailing slash
    */

    if(0 == strcmp(path, "/"))
    {
        SHFTPD_DEBUG("already under root!\n", path);
        return shftpd_send_resp(ctrlfd, 550);
    }

    /* compute the parent path */
    p = strrchr(path, '/');
    if(!p)
    {
        SHFTPD_DEBUG("internal path error![%s]\n", path);
        return shftpd_send_resp(ctrlfd, 550);
    }

    if(p == path && 0 != *(p+1))  // [/aaa]
    {
        *(p+1) = 0;
    }
    else if(p == &path[strlen(path)] && p>path) // [/aaa/] or [/aaa/bbb/]
    {
        *p = 0;
        p = strrchr(path, '/');
        if(p == path) // [/aaa/]
            *(p+1) = 0;
        else   // [/aaa/bbb/]
            *p = 0;
    }
    else // [/aaa/bbb]
    {
        *p = 0; // evil
    }

    if(0 == ftp_chdir(path))
        return shftpd_send_resp(ctrlfd, 200);
    else
        return shftpd_send_resp(ctrlfd, 550);
}


static int shftpd_cmd_opts(int ctrlfd, char *cmdline)
{
    char * value = strchr(cmdline, ' ');

    WEBLOG_API();

    SHFTPD_CHECK_LOGIN();

    if(!value)
    {
        SHFTPD_DEBUG("invalid cmd [%s]\n", cmdline);
        return shftpd_send_resp(ctrlfd, 550);
    }
    value++;

    if(0 == strcasecmp("utf8 on", value))
    {
        return shftpd_send_resp(ctrlfd, 200);
    }

    return shftpd_send_resp(ctrlfd, 550);
}



static int shftpd_cmd_quit(int ctrlfd, char *cmdline)
{

    WEBLOG_API();

    shftpd_send_resp(ctrlfd, 221);
    shftpd_quit_flag = 1;
    return SHFTPD_OK;
}

static int shftpd_cmd_request(int ctrlfd, char buf[])
{
    char *end = &buf[strlen(buf) - 1];
    char *space = strchr(buf, ' ');
    int i;
    char save;
    int err;

    WEBLOG_API();

    if (*end == '\n' || *end == '\r')
    {
        /*
         * this is a valid ftp request.
         */
        shftpd_trim_lineend(buf);

        if (!space)
            space = &buf[strlen(buf)];

        save = *space;
        *space = '\0';
        for (i = 0; shftpd_cmds[i].cmd_name; i++)
        {
            if (strcasecmp(buf, shftpd_cmds[i].cmd_name) == 0)
            {
                *space = save;
                SHFTPD_DEBUG("recved a valid ftp cmd:[%s]\n", buf);
                //diag_dump_buf_with_offset(buf, strlen(buf), buf);

                return shftpd_cmds[i].cmd_handler(ctrlfd, buf);
            }
        }

        /*
         * unrecognized cmd
         */
        *space = save;
        SHFTPD_DEBUG("recved a unsupported ftp cmd:[%s]\n", buf);

        *space = '\0';
        err = shftpd_send_resp(ctrlfd, 500, buf);
        *space = save;

        return err;
    }

    SHFTPD_DEBUG("recved a invalid ftp cmd:[%s]\n", buf);

    /*
     * Even if it's a invalid cmd, we should also send
     * back a response to prevent client from blocking.
     */
    return shftpd_send_resp(ctrlfd, 550);
}


void shftpd_ctrl_conn_handler(cyg_addrword_t pvarg)
{
    int connfd = ((FtpdThreadInfo *)pvarg)->connfd;
    char buf[256];
    int buflen;
    int err = SHFTPD_OK;


    WEBLOG_API();

    shftpd_thread_init();

    /*
     * Control connection has set up,
     * we can send out the first ftp msg.
     */
    if (shftpd_send_resp(connfd, 220) != SHFTPD_OK)
    {
        SHFTPD_DEBUG("shftpd_send_resp faile, Close the ctrl connection!","\n");
        goto __quit;
    }


    /*
     * Begin to interact with client and one should implement
     * a state machine here for full compatibility. But we only
     * show a demonstration ftp server and i do my best to
     * simplify it. Base on this skeleton, you can write a
     * full-funtional ftp server if you like. ;-)
     */
    while (1)
    {
        buflen = shftpd_recv_msg(connfd, buf, sizeof(buf));
        if (buflen < 0)
        {
            SHFTPD_DEBUG("shftpd_recv_msg() failed: %s\n", strerror(errno));
            err = SHFTPD_NG;
            break;
        }
        if (buflen == 0)
        {
            /* this means client launch a active close */
            SHFTPD_DEBUG("client launch a active close: %s\n", strerror(errno));
            break;
        }

        buf[buflen] = '\0';
		
        shftpd_cmd_request(connfd, buf);

        /*
         * The negative return value from shftpd_cmd_request
         * should not cause the breaking of ctrl connection.
         * Only when the client send QUIT cmd, we exit and
         * launch a active close.
         */

        if (shftpd_quit_flag)
        {
            SHFTPD_DEBUG("shftpd_quit_flag set to 1!","\n");
            break;
        }
    }

__quit:

    close(connfd);
    SHFTPD_DEBUG("Close the ctrl connection OK%s", "\n");

    shftpd_thread_uninit((void *)pvarg);

    extern cyg_handle_t reaper_mbox_handle;
    SHFTPD_DEBUG("<%s> thread[%u]: about to exit.\n", __FUNCTION__, ((FtpdThreadInfo *)pvarg)->thread_handle);
    cyg_mbox_put(reaper_mbox_handle, (void *)pvarg);
#if 0
    printf("<%s> thread[%u]: suicide msg sent.\n", __FUNCTION__, ((FtpdThreadInfo *)pvarg)->thread_handle);
#endif
}


void FTPD_Daemon(cyg_addrword_t pvarg)
{
    int listenfd;
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    WEBLOG_API();

    listenfd = shftpd_create_srv();
    if (listenfd < 0)
    {
        SHFTPD_DEBUG("Failed to create listening socket!", "\n");
    }

    while (1)
    {
        FtpdThreadInfo * ftpinfo = NULL;
        SHFTPD_DEBUG("Server ready, wait client's connection...%s", "\n");

        connfd = accept(listenfd, &cliaddr, &len);
        if (connfd < 0)
        {
            SHFTPD_DEBUG("accept() failed: %s\n", strerror(errno));
            continue;
        }

        SHFTPD_DEBUG("accept a conn from %s:%d\n",
                     inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        ftpinfo = (FtpdThreadInfo*)malloc(sizeof(FtpdThreadInfo));
        if(!ftpinfo)
        {
            SHFTPD_DEBUG("malloc FtpdThreadInfo fail!","\n");
            continue;
        }

        memset(ftpinfo, 0, sizeof(FtpdThreadInfo));
        ftpinfo->connfd = connfd;
        ftpinfo->thread_stack = (char *)malloc(FTPD_HANDLE_STACKSIZE);
        if(!ftpinfo->thread_stack)
        {
            SHFTPD_DEBUG("malloc ftpinfo->thread_stack fail!","\n");
            free(ftpinfo);
            continue;
        }

        snprintf(ftpinfo->thread_name, sizeof(ftpinfo->thread_name), "ftpx-%d", thread_id);
        cyg_thread_create(FTPD_DEF_PRIORITY,
                        shftpd_ctrl_conn_handler ,
                        (cyg_addrword_t)ftpinfo,
                        ftpinfo->thread_name,
                        (void *)ftpinfo->thread_stack,
                        FTPD_HANDLE_STACKSIZE,
                        &ftpinfo->thread_handle,
                        &ftpinfo->thread_info);
        cyg_thread_resume(ftpinfo->thread_handle);
    }

    return;
}



void FTPD_Init(void)
{
    WEBLOG_API();
    cyg_thread_create(FTPD_DEF_PRIORITY,
                    FTPD_Daemon,
                    (cyg_addrword_t)0,
                    "ftpd",
                    (void *)ftpd_daemon_stack,
                    FTPD_DAEMON_STACKSIZE,
                    &ftpd_daemon_hdl,
                    &ftpd_daemon_thread);
    cyg_thread_resume(ftpd_daemon_hdl);

}


