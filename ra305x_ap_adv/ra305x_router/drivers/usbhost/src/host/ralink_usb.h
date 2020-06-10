

#ifndef __RALINK_USB_H__

#include "rt_mmap.h"//Jody
#include "types.h"//Jody
//#include "ohci.h"//Jody

//#define SYSCFG1			(RALINK_SYSCTL_BASE + 0x14)
#define SYSCFG1			(0xb0000000 + 0x14)
#define USB0_HOST_MODE		(1UL<<10)

//#define RT2880_CLKCFG1_REG      (RALINK_SYSCTL_BASE + 0x30)
#define RT2880_CLKCFG1_REG      (0xb0000000 + 0x30)

//#define RT2880_RSTCTRL_REG      (RALINK_SYSCTL_BASE + 0x34)
#define RT2880_RSTCTRL_REG      (0xb0000000 + 0x34)

#define RALINK_UHST_RST         (1<<22)
#define RALINK_UDEV_RST         (1<<25)

#define IRQ_RT3XXX_USB      18
#define EHCI_register_start 0xb01c0000
#define EHCI_register_end   0xb01c0fff
#define OHCI_register_start 0xb01c1000
#define OHCI_register_end   0xb01c1fff

/*
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
*/

extern int mt7620_ehci_init(void);//Jody

inline static void try_wake_up(void)
{
	unsigned int val;

	//val = le32_to_cpu(*(volatile unsigned long *)(RT2880_CLKCFG1_REG));
	val = (*(volatile unsigned long *)(RT2880_CLKCFG1_REG));
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_MT7620)
	val = val | (RALINK_UPHY0_CLK_EN | RALINK_UPHY1_CLK_EN) ;
#elif defined (CONFIG_RALINK_RT5350)
	/* one port only */
	val = val | (RALINK_UPHY0_CLK_EN) ;
#else
#error	"no define platform"
#endif

	//*(volatile unsigned long *)(RT2880_CLKCFG1_REG) = cpu_to_le32(val);
	*(volatile unsigned long *)(RT2880_CLKCFG1_REG) = (val);
	//udelay(10000);	// enable port0 & port1 Phy clock
	//cyg_thread_delay(1);

	//val = le32_to_cpu(*(volatile unsigned long *)(RT2880_RSTCTRL_REG));
	val = (*(volatile unsigned long *)(RT2880_RSTCTRL_REG));
	val = val & ~(RALINK_UHST_RST | RALINK_UDEV_RST);
	//*(volatile unsigned long *)(RT2880_RSTCTRL_REG) = cpu_to_le32(val);
	*(volatile unsigned long *)(RT2880_RSTCTRL_REG) = (val);
	//udelay(10000);	// toggle reset bit 25 & 22 to 0
	//cyg_thread_delay(1);
}

inline static void try_sleep(void)
{
	unsigned int val;

	//val = le32_to_cpu(*(volatile unsigned long *)(RT2880_CLKCFG1_REG));
	val = (*(volatile unsigned long *)(RT2880_CLKCFG1_REG));
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_MT7620)
	val = val & ~(RALINK_UPHY0_CLK_EN | RALINK_UPHY1_CLK_EN);
#elif defined (CONFIG_RALINK_RT5350)
	val = val & ~(RALINK_UPHY0_CLK_EN);
#else
#error	"no define platform"
#endif
	//*(volatile unsigned long *)(RT2880_CLKCFG1_REG) = cpu_to_le32(val);
	*(volatile unsigned long *)(RT2880_CLKCFG1_REG) = (val);
	//udelay(10000);  // disable port0 & port1 Phy clock
	cyg_thread_delay(1);

	//val = le32_to_cpu(*(volatile unsigned long *)(RT2880_RSTCTRL_REG));
	val = (*(volatile unsigned long *)(RT2880_RSTCTRL_REG));
	val = val | (RALINK_UHST_RST | RALINK_UDEV_RST);
	//*(volatile unsigned long *)(RT2880_RSTCTRL_REG) = cpu_to_le32(val);
	*(volatile unsigned long *)(RT2880_RSTCTRL_REG) = (val);
	//udelay(10000);  // toggle reset bit 25 & 22 to 1
	cyg_thread_delay(1);
}

#endif /* __RALINK_USB_H__ */
