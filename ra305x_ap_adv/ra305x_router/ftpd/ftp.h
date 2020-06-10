/*****************************************************************************
 * $File:   ftp.c
 *
 * $Author: Hua Shao
 * $Date:   August, 2013
 *
 * QSB0aW55IEZUUCBzZXJ2ZXIgYnkgSHVhIFNoYW8uIChzaGhlcmVAZ21haWwuY29tKQ==
 *****************************************************************************/

#ifndef SHFTPD_H
#define SHFTPD_H

#define SHFTPD_ARR_LEN(arr)                 (sizeof(arr)/sizeof(arr[0]))
#define SHFTPD_ROOTDIR                      "/"
#define SHFTPD_VER                          "1.0"
#define SHFTPD_DEF_SRV_PORT                 21
#define SHFTPD_LISTEN_QU_LEN                16

#define SHFTPD_OK        0
#define SHFTPD_NG        (-1)

#define SHFTPD_CHECK_LOGIN()  \
        do { \
                if (!shftpd_cur_user) { \
                        shftpd_send_resp(ctrlfd, 530, "first please"); \
                        return SHFTPD_NG; \
                } \
        } while(0)

struct shftpd_cmd_struct {
        char *cmd_name;
        int (*cmd_handler)(int ctrlfd, char *cmd_line);
};

struct shftpd_user_struct {
        char user[128];
        char pass[128];
};


#endif /* SHFTPD_H */

