/****************************************************************************
 *mjh hello world
 ***************************************************************************
*/

//==============================================================================
//                                INCLUDE FILES
//==============================================================================
// simple include file
#include <stdio.h>
#include <cyg/kernel/kapi.h>  //HAL_REG32  cyg_thread_create

#include <cfg_id.h>
//include file in cgi_api.c for iwpriv cmd

//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================
#define WATCHDOG_TIMEOUT 30 /* 30 sec default timeout */
#define RALINK_SYSCTL_BASE 0xB0000000             //gpiomode base
#define RALINK_TIMER_BASE 0xB0000100
#define TMRSTAT     (RALINK_TIMER_BASE)  /* Timer Status Register */
#define TMR0LOAD    (TMRSTAT + 0x10)  /* Timer0 Load Value */
#define TMR0VAL     (TMRSTAT + 0x14)  /* Timer0 Counter Value */
#define TMR0CTL     (TMRSTAT + 0x18)  /* Timer0 Control */
#define TMR1LOAD    (TMRSTAT + 0x20)  /* Timer1 Load Value */
#define TMR1VAL     (TMRSTAT + 0x24)  /* Timer1 Counter Value */
#define TMR1CTL     (TMRSTAT + 0x28)  /* Timer1 Control */
#define SYSCFG      RALINK_SYSCTL_BASE + 0x10  /* System Configuration Register */
#define GPIOMODE    RALINK_SYSCTL_BASE + 0x60
#define RALINK_REG_GPIOMODE2		(GPIOMODE + 0x4)  //7628

//mjh from 7628 sdk
#define RALINK_IRQ_ADDR			RALINK_INTCL_BASE
#define RALINK_PRGIO_ADDR		RALINK_PIO_BASE    //gpio dir base

//search in rt_mmap.h pwd : sdk asm/rt2880
#define RALINK_INTCL_BASE		0xB0000200
#define RALINK_PIO_BASE			0xB0000600

#define RALINK_REG_INTENA		(RALINK_IRQ_ADDR   + 0x80)
#define RALINK_REG_INTDIS		(RALINK_IRQ_ADDR   + 0x78)

#define RALINK_REG_PIOINT		(RALINK_PRGIO_ADDR + 0x90)
#define RALINK_REG_PIOEDGE		(RALINK_PRGIO_ADDR + 0xA0)
#define RALINK_REG_PIORENA		(RALINK_PRGIO_ADDR + 0x50)
#define RALINK_REG_PIOFENA		(RALINK_PRGIO_ADDR + 0x60)
#define RALINK_REG_PIODATA		(RALINK_PRGIO_ADDR + 0x20)
#define RALINK_REG_PIODIR		(RALINK_PRGIO_ADDR + 0x00)
#define RALINK_REG_PIOSET		(RALINK_PRGIO_ADDR + 0x30)
#define RALINK_REG_PIORESET		(RALINK_PRGIO_ADDR + 0x40)
//--------------------------------------------------
#define RALINK_REG_PIO6332INT		(RALINK_PRGIO_ADDR + 0x94)
#define RALINK_REG_PIO6332EDGE		(RALINK_PRGIO_ADDR + 0xA4)
#define RALINK_REG_PIO6332RENA		(RALINK_PRGIO_ADDR + 0x54)
#define RALINK_REG_PIO6332FENA		(RALINK_PRGIO_ADDR + 0x64)
#define RALINK_REG_PIO6332DATA		(RALINK_PRGIO_ADDR + 0x24)
#define RALINK_REG_PIO6332DIR		(RALINK_PRGIO_ADDR + 0x04)
#define RALINK_REG_PIO6332SET		(RALINK_PRGIO_ADDR + 0x34)
#define RALINK_REG_PIO6332RESET		(RALINK_PRGIO_ADDR + 0x44)

#define RALINK_REG_PIO9564INT		(RALINK_PRGIO_ADDR + 0x98)
#define RALINK_REG_PIO9564EDGE		(RALINK_PRGIO_ADDR + 0xA8)
#define RALINK_REG_PIO9564RENA		(RALINK_PRGIO_ADDR + 0x58)
#define RALINK_REG_PIO9564FENA		(RALINK_PRGIO_ADDR + 0x68)
#define RALINK_REG_PIO9564DATA		(RALINK_PRGIO_ADDR + 0x28)
#define RALINK_REG_PIO9564DIR		(RALINK_PRGIO_ADDR + 0x08)
#define RALINK_REG_PIO9564SET		(RALINK_PRGIO_ADDR + 0x38)
#define RALINK_REG_PIO9564RESET		(RALINK_PRGIO_ADDR + 0x48)

//for pad
extern int is_connect(void);

//mjh
extern char *mjh_str1;
extern char *mjh_str2;
extern char *mjh_str3;
extern char *mjh_str4;
extern char *mjh_str5;
extern char *mjh_str6;
extern char *mjh_str7;

extern char *mjh_ssid;
extern int CmdIwpriv(int argc, char *argv[]);
extern char mjh_Connect;

extern char mjh_wsc_flag;
extern char mjh_wsc_link;  //ap_apcli.c
extern char mjh_wsc_fail;
extern char mjh_2_out;

//wsc key handle
extern void mjh_wsc_handler(void);
//led wifi thread
cyg_handle_t led_thread_h;
static cyg_thread cli_thread;
static char cli_stack[4096];  //4096*2

//wsc thread
cyg_handle_t wsc_thread_h;
static cyg_thread wsc_thread;
static char wsc_stack[1024];  //4096

//wsc led
int mjh_i=0;


char not_ssid_flag = 0;

//wifi
int get_singal(char *input)     //input is the name of connected wifi
{
	char *p=NULL;

	char ssid[186], bssid[20], security[23];
	char signal[9];

	char signal_shu;
	char channel_shu;

	char *string1[]={
	"ra0",
	"set",
	"SiteSurvey="
	};
	CmdIwpriv(3,string1);
	cyg_thread_delay(10);
	
	char *string2[]={
	"ra0",
	"get_site_survey"
	};
	Cmd_mjh(2,string2);
	cyg_thread_delay(10);

	p = strstr(mjh_str1, input);  //search
	if(p != NULL)   //存在ssid
	{
	p += 33;
	sscanf(p, "%s %s %s", bssid, security, signal);
	/*	
	p = p-4;
	sscanf(p, "%s %s %s %s %s", channel, ssid, bssid, security, signal);
	*/
	}
	else if(strstr(mjh_str2, input) != NULL)
	{
	sscanf(strstr(mjh_str2, input)+33, "%s %s %s", bssid, security, signal);
	/*
	p = strstr(mjh_str2, input)-4;
	sscanf(p, "%s %s %s %s %s", channel, ssid, bssid, security, signal);
	*/	
	}
	else if(strstr(mjh_str3, input) != NULL)
	{
	sscanf(strstr(mjh_str3, input)+33, "%s %s %s", bssid, security, signal);
	}
	else if(strstr(mjh_str4, input) != NULL)
	{
	sscanf(strstr(mjh_str4, input)+33, "%s %s %s", bssid, security, signal);
	}
	else if(strstr(mjh_str5, input) != NULL)
	{
	sscanf(strstr(mjh_str5, input)+33, "%s %s %s", bssid, security, signal);
	}
	else if(strstr(mjh_str6, input) != NULL)
	{
	sscanf(strstr(mjh_str6, input)+33, "%s %s %s", bssid, security, signal);
	}
	else if(strstr(mjh_str7, input) != NULL)
	{
	sscanf(strstr(mjh_str7, input)+33, "%s %s %s", bssid, security, signal);
	}
	else
	{
		not_ssid_flag = 1;
		printf("not found ssid\n");
		free(mjh_str1);
		return -1;
	}
	free(mjh_str1);
	
	signal_shu =  atoi(signal);
	led_mode();

	led_control(signal_shu);
	printf("signal = %d\n",signal_shu);
	return signal_shu;
}


//cmm_info.c change
int is_connect_old(void) //return : disconnect <0   signal >0  
{
	char *p;
	char signal;
	char *string[]={
	"apcli0",
	"show",
	"connStatus"
	};
	CmdIwpriv(3,string);
	cyg_thread_delay(10);
	
	
	//Cmd_mjh("apcli0","show","connStatus");
	if(mjh_Connect == 1)   //存在ssid  connect
	{
		printf("connect the ssid : %s\n",mjh_ssid);
		signal=get_singal(mjh_ssid);
		led_mode();
		if(not_ssid_flag == 1)
			not_ssid_flag = 0;
		else
			led_control(signal);
		return 1;
	}
	else   //not connect
	{
		printf("not connect\n");
		led_mode();
		weak();
		return -1;
	}			
}


//led all off control
void disconnect(void)
{
	unsigned int result;
	result = HAL_REG32(RALINK_REG_PIODATA);   //31  output dir set 1
	result |= 0x80000000;
	HAL_REG32(RALINK_REG_PIODATA) = result;

	result = HAL_REG32(RALINK_REG_PIO6332DATA); //32 33 34 signal led
	result |= 0x7;                         //led off
	HAL_REG32(RALINK_REG_PIO6332DATA) = result;
}

void weak(void)  //siganl led all off
{
	unsigned int result;
	result = HAL_REG32(RALINK_REG_PIO6332DATA); //32 33 34 signal led
	result |= 0x7;                         //led off
	HAL_REG32(RALINK_REG_PIO6332DATA) = result;	
}

void low(void)
{
	unsigned int result;
	disconnect();

	result = HAL_REG32(RALINK_REG_PIO6332DATA); //32
	result &= ~0x1;                         //led1 on
	HAL_REG32(RALINK_REG_PIO6332DATA) = result;

}

void middle(void)
{
	unsigned int result;
	disconnect();

	result = HAL_REG32(RALINK_REG_PIO6332DATA); //32 33
	result &= ~0x3;                         //led1 led2 on
	HAL_REG32(RALINK_REG_PIO6332DATA) = result;
}

void high(void)
{
	unsigned int result;

	result = HAL_REG32(RALINK_REG_PIO6332DATA); //32 33 34
	result &= ~0x7;                         //led1 led2 led3 on
	HAL_REG32(RALINK_REG_PIO6332DATA) = result;
}

void led_mode(void)
{
	unsigned int result;
	result = HAL_REG32(RALINK_REG_GPIOMODE2);
	result &= ~0x3fc0000;    //clear 0    31 32 33 34
	result |= 0x1540000;     //set 1
	HAL_REG32(RALINK_REG_GPIOMODE2) = result;

	result = HAL_REG32(RALINK_REG_PIODIR);   //31  output dir set 1
	result |= 0x80000000;
	HAL_REG32(RALINK_REG_PIODIR) = result;

	result = HAL_REG32(RALINK_REG_PIO6332DIR); //32 33 34
	result |= 0x07; 
	HAL_REG32(RALINK_REG_PIO6332DIR) = result;
}

void led_init(void)  //led all off
{
	unsigned int result;
	result = HAL_REG32(RALINK_REG_GPIOMODE2);
	result &= ~0x3fc0000;    //clear 0    31 32 33 34
	result |= 0x1540000;     //set 1
	HAL_REG32(RALINK_REG_GPIOMODE2) = result;

	result = HAL_REG32(RALINK_REG_PIODIR);   //31  output dir set 1
	result |= 0x80000000;
	HAL_REG32(RALINK_REG_PIODIR) = result;

	result = HAL_REG32(RALINK_REG_PIO6332DIR); //32 33 34
	result |= 0x07; 
	HAL_REG32(RALINK_REG_PIO6332DIR) = result;

	disconnect();
}


//led control due to siganl
void led_control(char signal)
{
	if(signal < 20)
		weak();
	else if(signal  < 50 && signal >= 20)
		low();
	else if(signal < 70 && signal >=50)
		middle();
	else if(signal > 70)
		high();
	else
		weak();
}

/* 1.7
void CLI_mjh_proc(void)
{

	printf("wifi thread\n");
	while(1)
	{
	is_connect();
	//printf("start\n");
	cyg_thread_delay(1000); //5000 50s  2000 20s 以10ms
	//printf("end\n");
	}	
}
*/

void CLI_mjh_proc(void)
{
	char line[100], *p;
	int opmode_value;
	printf("wifi thread\n");
	while(1)
	{

	CFG_get_str(CFG_SYS_OPMODE, line);
	opmode_value = strtol(line, &p, 10);
	if (opmode_value != 2)	   //opmode 
	{	
		is_connect();
	}
	else
		disconnect();

	cyg_thread_delay(2000); //5000 50s  2000 20s 以10ms(default:1000) 

	}	
}

void mjh_start(int cmd)
{
    led_init();
	high();
//add by rong

	cyg_thread_delay(500);
//
    if (cmd)
    {
    cyg_thread_create(4, (cyg_thread_entry_t *)CLI_mjh_proc,(cyg_addrword_t) 0,"CLI_thread",
                  (void *)&cli_stack[0], sizeof(cli_stack), &led_thread_h, &cli_thread);
    cyg_thread_resume(led_thread_h);

	printf("wifi thread out\n");
    }
}

void led_test(void)
{
	unsigned int result;

	result = HAL_REG32(RALINK_REG_PIODATA); //31
	result &= ~0x80000000;                 //led on
	HAL_REG32(RALINK_REG_PIODATA) = result;
}

//wsc led gpio 31
void led_wps(int cmd)
{
	unsigned int result;
	if(cmd == 1)  //led off
	{
	result = HAL_REG32(RALINK_REG_PIODATA);   //31  output dir set 1
	result |= 0x80000000;
	HAL_REG32(RALINK_REG_PIODATA) = result;
	}
	else if(cmd == 0)
	{
	result = HAL_REG32(RALINK_REG_PIODATA); //31
	result &= ~0x80000000;                 //led on
	HAL_REG32(RALINK_REG_PIODATA) = result;
	}
	else
		printf("arg is error\n");
	
}

void led_flash(void)
{
	//static int i=0;
	unsigned int result;
	mjh_i++;
	if(mjh_i>=800000)
	{
	mjh_i=0;
	result = HAL_REG32(RALINK_REG_PIODATA); //31
	result ^= 0x80000000;                 // yu 1 yi huo qu fan  0 wu bian hua 
	HAL_REG32(RALINK_REG_PIODATA) = result;
	}
	
}

void led_blank(void)
{
	unsigned int result;
	
	result = HAL_REG32(RALINK_REG_PIODATA); //31
	result ^= 0x80000000;                 // yu 1 yi huo qu fan  0 wu bian hua 
	HAL_REG32(RALINK_REG_PIODATA) = result;
	
	cyg_thread_delay(50); //0.5s	
}


//wsc key
void wsc_do_old(void)
{
	printf("wsc thread\n");
	while(1)
	{
	if(mjh_wsc_flag)
	{
		led_flash();
		if(mjh_wsc_link == 1)
		{
			mjh_i = 0;
			mjh_wsc_flag = 0; //clear falsh flag		
				
		}
		if(mjh_wsc_fail == 1)
		{
			mjh_i = 0;
			mjh_wsc_flag = 0; //clear falsh flag
			mjh_wsc_link = 0;
			led_wps(1);  //led oFF
		}
		/*
		else if( (pWscControl->WscStatus == STATUS_WSC_CONFIGURED) ||
		(pWscControl->WscStatus == STATUS_WSC_FAIL) ||
		(pWscControl->WscStatus == STATUS_WSC_PBC_TOO_MANY_AP)  ||
		(pWscControl->WscStatus == STATUS_WSC_NOTUSED)    ||
		(pWscControl->WscState == WSC_STATE_OFF))
			led_wps(1);	
		*/	
	}
	else
	{	
			
		mjh_i = 0;
		mjh_wsc_flag = 0;
		if(mjh_wsc_link == 1 && mjh_Connect == 1) //first link up //apcli0 connect //wsc ssid  web yao rebbot tiaojian fail
			led_wps(0);  //led on
		else if(mjh_wsc_link == 1 && mjh_2_out == 0)
			led_flash();
		else if(mjh_wsc_fail == 1 || mjh_2_out == 1)
			led_wps(1);
		else
			led_wps(1);  //led off	
	}
	mjh_wsc_handler();
	}
		
}
extern void wsc_do(void);
void mjh_wsc(int cmd)           //old 8
{
    if (cmd)
    {
    cyg_thread_create(10, (cyg_thread_entry_t *)wsc_do,(cyg_addrword_t) 0,"wsc_thread",
                  (void *)&wsc_stack[0], sizeof(wsc_stack), &wsc_thread_h, &wsc_thread);
    cyg_thread_resume(wsc_thread_h);
	printf("wsc thread out\n");
    }
}


void hello(void)
{
	printf("hello world\n");
	//led_init();
}
