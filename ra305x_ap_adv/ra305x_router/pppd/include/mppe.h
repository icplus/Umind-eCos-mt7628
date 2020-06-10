//#ifndef __MPPE_INCLUDE__

#ifndef CI_MPPE
#define CI_MPPE			18	/* config. option for MPPE */
#endif /* CI_MPPE */
#ifndef CILEN_MPPE
#define CILEN_MPPE		6	/* length of config. option */
#endif /* CILEN_MPPE */

#define MPPE_OVHD		2	/* MPPE overhead */
#define MPPE_HIST_LEN		8192	/* MPPC history size */
#define MPPE_MAX_CCOUNT		0x0FFF	/* max. coherency counter value */

typedef struct mppe_state {
    int    us_unit;	/* Interface unit number */
    u_char us_id;		/* Current id */
    u_char us_allowed;
    int    us_type;
    char   *us_number;    /* Telefone Number */
} mppe_state;


extern struct protent mppe_protent;

#define MPPE_CONFOPTION CI_MPPE		/* p[0] */
#define MPPE_STATELESS  0x01		/* p[2] - configuration bit H */
#define MPPE_40BIT	0x20			/* p[5] - configuration bit L */
#define MPPE_56BIT	0x80			/* p[5] - configuration bit M */
#define MPPE_128BIT	0x40			/* p[5] - configuration bit S */
#define MPPE_MPPC	0x01			/* p[5] - configuration bit C */

#define PPP_MPPE	0x00FD

#define MPPE_BIT_A	0x80
#define MPPE_BIT_B	0x40
#define MPPE_BIT_C	0x20
#define MPPE_BIT_D	0x10
#define MPPE_BIT_FLUSHED MPPE_BIT_A
#define MPPE_BIT_RESET		0x40	/* bit B */
#define MPPE_BIT_COMP		0x20	/* bit C */
#define MPPE_BIT_ENCRYPTED MPPE_BIT_D
#define MPPE_CCOUNT	0x0FFF

#define MPPE_40_SALT0	0xD1
#define MPPE_40_SALT1	0x26
#define MPPE_40_SALT2	0x9E

#define MPPE_MINLEN 4

#define MPPE_REQ    1
#define MPPE_RESP   2
#define MPPE_ACK    3

extern char mppe_master_send_key_40[8];
extern char mppe_master_send_key_128[16];
extern char mppe_master_recv_key_40[8];
extern char mppe_master_recv_key_128[16];
extern unsigned int mppe_allowed;

void mppe_gen_master_key __P((char *, int, unsigned char *));
void mppe_gen_master_key_v2 __P((char *, int, unsigned char *, int));

int setmppe_40(char **);
int setnomppe_40(char **);
int setmppe_128(char **);
int setnomppe_128(char **);
int setmppe_stateless(char **);
int setnomppe_stateless(char **);

//#define __MPPE_INCLUDE__
//#endif /* __MPPE_INCLUDE__ */


