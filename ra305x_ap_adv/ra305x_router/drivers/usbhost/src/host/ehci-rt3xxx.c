/*
 * Ralink 3XXX(3883) EHCI Host Controller Driver
 *
 * Author: Ying Yuan Huang <yyhuang@ralinktech.com.tw>
 * Based on "ehci-fsl.c" by Randy Vinson <rvinson@mvista.com>
 *
 * 2009 (c) Ralink Technology, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

//#include <linux/platform_device.h>
#include "ralink_usb.h"
#include "hcd.h"
#include "ehci.h"
#include "ehci-hcd.c"

#include "mu_impl.h" //Jody
#include <cyg/hal/hal_if.h> //Jody
#include "x_debug.h" //Jody
#include "mu_drcdf.h" //Jody
#include "mtk/mtk-phy.h"

struct QH qh_list_global __attribute__((aligned(32)));
struct QH *qh_list = NULL;
struct QH alloc_qh[2] __attribute__((aligned(32)));
struct qTD alloc_td[6] __attribute__((aligned (32)));

unsigned int EHCI_toggle[2] = {0, 0};		/* one bit for each endpoint ([0] = IN, [1] = OUT) */
unsigned int eCos_dongleRead_flag = 0;
unsigned int eCos_dongleWrite_flag = 0;

extern u16 EHCI_MaxPacket;


//#define KSEG1ADDR(a)		((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | 0xa0000000))

//#define EHCI_virt_to_bus(x) virt_to_bus((volatile void *)(x))
//#define EHCI_virt_to_bus(x) ((x) & 0x1fffffff)

void static inline rt_writel(u32 val, unsigned long reg)
{
    *(volatile u32 *)(reg) = val;
}

static inline u32 rt_readl(unsigned long reg)
{
    return (*(volatile u32 *)reg);
}

static int rt_set_host(void)
{
	u32 val = rt_readl(SYSCFG1);
	// host mode
	val |= USB0_HOST_MODE;		
	rt_writel(val, SYSCFG1);
}

static int rt_usbhost_reset(void)
{
	u32 val = rt_readl(RT2880_RSTCTRL_REG);
	val |= RALINK_UHST_RST;
	rt_writel(val, RT2880_RSTCTRL_REG);
	val &= ~(RALINK_UHST_RST);
	rt_writel(val, RT2880_RSTCTRL_REG);
	mdelay (100);
}

static int rt3xxx_ehci_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int retval = 0;

	/* No USB-PCI space. */
	ehci->caps = hcd->regs /* + 0x100 */;
	ehci->regs = hcd->regs /* + 0x100 */ + HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	ehci_reset(ehci);

	retval = ehci_init(hcd);
	if (retval)
		return retval;

	ehci_port_power(ehci, 0);

	return retval;
}

int MT7620_handshake(unsigned long ptr, u32 mask, u32 done, int ms)
{
	u32	result;

	do {
		result = mtk_readl(ptr);
		if (result == ~(u32)0)		/* card removed */
			return -ENODEV;
		result &= mask;
		if (result == done)
			return 0;
		//udelay (1);
		//cyg_thread_delay(1);
		CYGACC_CALL_IF_DELAY_US(1000);
		ms--;
	} while (ms > 0);
	
	return -ETIMEDOUT;
}

int read_ehci_registers_test(void)
{
	//struct ehci_hcd *ehci = hcd_to_ehci(hcd);
    u32 read_hcs_params = 0;
	u32 read_hcc_params = 0;
	u32 read_command = 0;
	u32 read_status = 0;
	u32 read_intr_enable = 0;
    u32 read_port0_status = 0;
	
	//diag_printf("****** enter:%s\n", __func__);

	read_hcs_params = rt_readl(EHCI_CAPS_HCS_PARAMS);
	read_hcc_params = rt_readl(EHCI_CAPS_HCC_PARAMS);
	read_command = rt_readl(EHCI_REGS_COMMAND);
	read_status = rt_readl(EHCI_REGS_STATUS);
	read_intr_enable = rt_readl(EHCI_REGS_INTR_ENABLE);
	read_port0_status = rt_readl(EHCI_REGS_PORT_STATUS);

	diag_printf("\n****** enter:%s, read EHCI registers:\n", __func__);
    diag_printf("                   read_hcs_params:0x%x\n", read_hcs_params);
	diag_printf("                   read_hcc_params:0x%x\n", read_hcc_params);
	diag_printf("                   read_command:0x%x\n", read_command);
	diag_printf("                   read_status:0x%x\n", read_status);
	diag_printf("                   read_port0_status:0x%x\n", read_port0_status);
	

}

#if 0
static int rt_handshake(unsigned long ptr, u32 mask, u32 done, int ms)
{
	u32 result, number;
	do {
		result = mtk_readl(ptr);
		if (result == ~(uint32_t)0)
			return -1;
		result &= mask;
		if (result == done)
			return 0;
		//udelay(1);
		CYGACC_CALL_IF_DELAY_US(1000); // udelay(1)		
		ms--;
	} while (ms > 0);
	
	return -1;
}

static void MTK_PortConnectReset(int port)
{
    u32 temp, ret;
	u32 status, change;
    //cyg_thread_delay(20);
	CYGACC_CALL_IF_DELAY_US(200 * 1000); // mdelay(200)

    temp = mtk_readl(EHCI_REGS_PORT_STATUS);
	if (temp & PORT_OWNER){
		diag_printf("enter:%s, error!\n", __func__);
		return ;
	}
	temp &= ~PORT_RWC_BITS;
	temp |= PORT_RESET;
	temp &= ~PORT_PE;	
	mtk_writel(temp, EHCI_REGS_PORT_STATUS);

	//cyg_thread_delay(5);
	CYGACC_CALL_IF_DELAY_US(500 * 1000); // mdelay(50)

	ret = mtk_readl(EHCI_REGS_PORT_STATUS);
	if (ret & PORT_RESET){
		int ret;
		diag_printf("******enter %s, force reset\n", __func__);
		/* force reset to complete */
		ret = ret & ~(PORT_RESET | PORT_RWC_BITS);
		mtk_writel(ret, EHCI_REGS_PORT_STATUS);
		ret = rt_handshake(EHCI_REGS_PORT_STATUS, PORT_RESET, 0, 200);
		if (ret)
		  diag_printf("******enter %s, port reset error\n", __func__);
	}

	diag_printf("******after %s, port reset complete!\n", __func__);
	//MTK_GetPortStatus(1, &status, &change);

	//if(change)
	//	MTK_ClearPortFeature(1, USB_PORT_FEAT_C_RESET);

}
#endif

int mt7620_ehci_init(void)
{
	int retval = 0;
	u32 temp = 0, hcs_params = 0, test = 0;
	u32 command = 0;
    u32 hcc_paramer =0;
	
	usb_dbg("****** enter:%s\n", __func__);

	try_wake_up();

	// change port0 to host mode
	//rt_set_host();

	//ehci_reset(ehci);
	command = rt_readl(EHCI_REGS_COMMAND);
	command |= CMD_RESET;
	rt_writel(command, EHCI_REGS_COMMAND);
	retval = MT7620_handshake(EHCI_REGS_COMMAND, CMD_RESET, 0, 500);
	if(retval < 0)
	  diag_printf("MT7620_handshake error!\n");
	
#ifdef CONFIG_RALINK_MT7628
    u3phy_init();
	u3phy_ops->init(u3phy);
	if (u3phy_ops->u2_slew_rate_calibration)
      u3phy_ops->u2_slew_rate_calibration(u3phy);
    else
      diag_printf("WARN: PHY doesn't implement u2 slew rate calibration function\n");
#endif
	//retval = ehci_init(hcd);//need Jody
	//if (retval)
	//	return retval;

	//ehci_port_power(ehci, 0);

	qh_list = KSEG1ADDR(&qh_list_global);
	//qh_list = &qh_list_global;

	/* Set head of reclaim list */
	memset(qh_list, 0, sizeof(struct QH));
	//qh_list->qh_link = EHCI_virt_to_bus(cpu_to_hc32((uint32_t)qh_list | QH_LINK_TYPE_QH));
	//qh_list->qh_endpt1 = cpu_to_hc32((1 << 15) | (USB_SPEED_HIGH << 12));
	//qh_list->qh_curtd = cpu_to_hc32(QT_NEXT_TERMINATE);
	//qh_list->qh_overlay.qt_next = cpu_to_hc32(QT_NEXT_TERMINATE);
	//qh_list->qh_overlay.qt_altnext = cpu_to_hc32(QT_NEXT_TERMINATE);
	//qh_list->qh_overlay.qt_token = cpu_to_hc32(0x40);

	qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh_list | QH_LINK_TYPE_QH));
	qh_list->qh_endpt1 = cpu_to_le32((1 << 15) | (USB_SPEED_HIGH << 12));
	qh_list->qh_curtd = cpu_to_le32(QT_NEXT_TERMINATE);
	qh_list->qh_overlay.qt_next = cpu_to_le32(QT_NEXT_TERMINATE);
	qh_list->qh_overlay.qt_altnext = cpu_to_le32(QT_NEXT_TERMINATE);
	qh_list->qh_overlay.qt_token = cpu_to_le32(0x40);

	/* Set async. queue head pointer. */
	mtk_writel(EHCI_virt_to_bus((uint32_t)qh_list), EHCI_REGS_ASYNC_NEXT);

	
	temp = rt_readl(EHCI_REGS_PORT_STATUS);
	temp &= ~PORT_RWC_BITS;
	hcs_params = rt_readl(EHCI_CAPS_HCS_PARAMS);
	if (hcs_params & (1<<4))
	  rt_writel(temp | (1<<12), EHCI_REGS_PORT_STATUS);

	//MTK_PortConnectReset(1);
  
	/* Start the host controller. */
	hcc_paramer = rt_readl(EHCI_CAPS_HCC_PARAMS);
	command = rt_readl(EHCI_REGS_COMMAND);

	command &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<1));
	command |= CMD_RUN;
	if(hcc_paramer & (1<<2))
		command &= ~((1<<11)|(3<<8));
	rt_writel(command, EHCI_REGS_COMMAND);

	//rt_writel(0x10005, EHCI_REGS_COMMAND); /* Turn On Interrupts */

	//enable interrupt
	rt_writel(INTR_MASK, EHCI_REGS_INTR_ENABLE); /* Turn On Interrupts */

	/* take control over the ports */
	command = rt_readl(EHCI_REGS_CONFIGFLAG);
	command |= (1<<0);
	rt_writel(command, EHCI_REGS_CONFIGFLAG);

	//read_ehci_registers_test();

	return retval;
}


static const struct hc_driver rt3xxx_ehci_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Ralink EHCI Host Controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,
	.reset			= rt3xxx_ehci_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.get_frame_number	= ehci_get_frame,
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#if defined(CONFIG_PM)
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
#endif

	.relinquish_port        = ehci_relinquish_port,
	.port_handed_over       = ehci_port_handed_over,
	.clear_tt_buffer_complete       = ehci_clear_tt_buffer_complete,
	.endpoint_reset		= ehci_endpoint_reset,

};

struct usb_hcd *usb_create_hcd (const struct hc_driver *driver, const char *bus_name)
{
	struct usb_hcd *hcd;

	hcd = malloc(sizeof(*hcd) + driver->hcd_priv_size);
	if (!hcd) {
		diag_printf("hcd alloc failed\n");
		return NULL;
	}
	memset(hcd, 0, sizeof(*hcd) + driver->hcd_priv_size);

	//usb_bus_init(&hcd->self);//below for usb_bus_init
	memset (&hcd->self.devmap, 0, sizeof(struct usb_devmap));
	hcd->self.devnum_next = 1;
	hcd->self.root_hub = NULL;
	hcd->self.busnum = -1;
	hcd->self.bandwidth_allocated = 0;
	hcd->self.bandwidth_int_reqs  = 0;
	hcd->self.bandwidth_isoc_reqs = 0;
	INIT_LIST_HEAD (&hcd->self.bus_list);
	
	//hcd->self.controller = dev;
	hcd->self.bus_name = bus_name;
	//hcd->self.uses_dma = (dev->dma_mask != NULL);

	//mutex_init(&hcd->bandwidth_mutex);

	hcd->driver = driver;
	hcd->product_desc = (driver->product_desc) ? driver->product_desc :
			"USB Host Controller";
	return hcd;
}
//EXPORT_SYMBOL_GPL(usb_create_hcd);

struct usb_device *usb_alloc_dev(struct usb_device *parent,
				 struct usb_bus *bus, unsigned port1)
{
	struct usb_device *dev;
	struct usb_hcd *usb_hcd = container_of(bus, struct usb_hcd, self);
	unsigned root_hub = 0;

	dev = malloc(sizeof(*dev));
	if (!dev)
		return NULL;
    memset(dev, 0, sizeof(*dev));
	
	/* Root hubs aren't true devices, so don't allocate HCD resources */
	if (usb_hcd->driver->alloc_dev && parent &&
		!usb_hcd->driver->alloc_dev(usb_hcd, dev)) {
		//usb_put_hcd(bus_to_hcd(bus));
		kfree(dev);
		return NULL;
	}

	//device_initialize(&dev->dev);
	//dev->dev.bus = &usb_bus_type;
	//dev->dev.type = &usb_device_type;
	//dev->dev.groups = usb_device_groups;
	//dev->dev.dma_mask = bus->controller->dma_mask;
	//set_dev_node(&dev->dev, dev_to_node(bus->controller));
	dev->state = USB_STATE_ATTACHED;
	//atomic_set(&dev->urbnum, 0);

	INIT_LIST_HEAD(&dev->ep0.urb_list);
	dev->ep0.desc.bLength = USB_DT_ENDPOINT_SIZE;
	dev->ep0.desc.bDescriptorType = USB_DT_ENDPOINT;
	/* ep0 maxpacket comes later, from device descriptor */
	//usb_enable_endpoint(dev, &dev->ep0, false);
	dev->ep_out[0] = &dev->ep0;
	dev->ep_in[0] = &dev->ep0;
	dev->ep0.enabled = 1;
	dev->can_submit = 1;

	/* Save readable and stable topology id, distinguishing devices
	 * by location for diagnostics, tools, driver model, etc.  The
	 * string is a path along hub ports, from the root.  Each device's
	 * dev->devpath will be stable until USB is re-cabled, and hubs
	 * are often labeled with these port numbers.  The name isn't
	 * as stable:  bus->busnum changes easily from modprobe order,
	 * cardbus or pci hotplugging, and so on.
	 */
	if (unlikely(!parent)) {
		dev->devpath[0] = '0';
		dev->route = 0;

		//dev->dev.parent = bus->controller;
		//dev_set_name(&dev->dev, "usb%d", bus->busnum);
		root_hub = 1;
	} else {
		/* match any labeling on the hubs; it's one-based */
		if (parent->devpath[0] == '0') {
			snprintf(dev->devpath, sizeof dev->devpath,
				"%d", port1);
			/* Root ports are not counted in route string */
			dev->route = 0;
		} else {
			snprintf(dev->devpath, sizeof dev->devpath,
				"%s.%d", parent->devpath, port1);
			/* Route string assumes hubs have less than 16 ports */
			if (port1 < 15)
				dev->route = parent->route +
					(port1 << ((parent->level - 1)*4));
			else
				dev->route = parent->route +
					(15 << ((parent->level - 1)*4));
		}

		//dev->dev.parent = &parent->dev;
		//dev_set_name(&dev->dev, "%d-%s", bus->busnum, dev->devpath);

		/* hub driver sets up TT records */
	}

	dev->portnum = port1;
	dev->bus = bus;
	dev->parent = parent;
	INIT_LIST_HEAD(&dev->filelist);

#ifdef	CONFIG_PM
	dev->autosuspend_delay = usb_autosuspend_delay * HZ;
	dev->connect_time = jiffies;
	dev->active_duration = -jiffies;
#endif
	if (root_hub)	/* Root hub always ok [and always wired] */
		dev->authorized = 1;
	/*
	else {
		dev->authorized = usb_hcd->authorized_default;
		dev->wusb = usb_bus_is_wusb(bus)? 1 : 0;
	}
	*/
	return dev;
}

static int register_root_hub(struct usb_hcd *hcd)
{
	//struct device *parent_dev = hcd->self.controller;
	struct usb_device *usb_dev = hcd->self.root_hub;
	const int devnum = 1;
	int retval;

	usb_dev->devnum = devnum;
	usb_dev->bus->devnum_next = devnum + 1;
	memset (&usb_dev->bus->devmap.devicemap, 0,
			sizeof usb_dev->bus->devmap.devicemap);
	//set_bit (devnum, usb_dev->bus->devmap.devicemap);
	//usb_set_device_state(usb_dev, USB_STATE_ADDRESS);
	usb_dev->state = USB_STATE_ADDRESS;

	//mutex_lock(&usb_bus_list_lock);

	usb_dev->ep0.desc.wMaxPacketSize = cpu_to_le16(64);
	retval = usb_get_device_descriptor(usb_dev, USB_DT_DEVICE_SIZE);
	if (retval != sizeof usb_dev->descriptor) {
		//mutex_unlock(&usb_bus_list_lock);
		diag_printf("can't read usb device descriptor %d\n", retval);
		return (retval < 0) ? retval : -EMSGSIZE;
	}

	retval = usb_new_device (usb_dev);
	if (retval) {
		diag_printf ("can't register root hub for usb, %d\n", retval);
	}
	//mutex_unlock(&usb_bus_list_lock);

	if (retval == 0) {
		//spin_lock_irq (&hcd_root_hub_lock);
		hcd->rh_registered = 1;
		//spin_unlock_irq (&hcd_root_hub_lock);

		/* Did the HC die before the root hub was registered? */
		//if (hcd->state == HC_STATE_HALT)
		//	usb_hc_died (hcd);	/* This time clean up */
	}

	return retval;
}

int usb_add_hcd(struct usb_hcd *hcd, unsigned int irqnum)
{
	int retval;
	struct usb_device *rhdev;

	dev_info(hcd->self.controller, "%s\n", hcd->product_desc);

	hcd->authorized_default = hcd->wireless? 0 : 1;
	//set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	/* HC is in reset state, but accessible.  Now do the one-time init,
	 * bottom up so that hcds can customize the root hubs before khubd
	 * starts talking to them.  (Note, bus id is assigned early too.)
	 */
	/*
	if ((retval = hcd_buffer_create(hcd)) != 0) {
		dev_dbg(hcd->self.controller, "pool alloc failed\n");
		return retval;
	}
       
	if ((retval = usb_register_bus(&hcd->self)) < 0)
		goto err_register_bus;
	*/

	if ((rhdev = usb_alloc_dev(NULL, &hcd->self, 0)) == NULL) {
		diag_printf("unable to allocate root hub\n");
		retval = -ENOMEM;
		//goto err_allocate_root_hub;
		return retval;
	}
	hcd->self.root_hub = rhdev;

	switch (hcd->driver->flags & HCD_MASK) {
	case HCD_USB11:
		rhdev->speed = USB_SPEED_FULL;
		break;
	case HCD_USB2:
		rhdev->speed = USB_SPEED_HIGH;
		break;
	case HCD_USB3:
		rhdev->speed = USB_SPEED_SUPER;
		break;
	default:
		//goto err_set_rh_speed;
		diag_printf("can not find speed!");
		return -1;
	}

	/* wakeup flag init defaults to "everything works" for root hubs,
	 * but drivers can override it in reset() if needed, along with
	 * recording the overall controller's system wakeup capability.
	 */
	//device_init_wakeup(&rhdev->dev, 1);

	/* "reset" is misnamed; its role is now one-time init. the controller
	 * should already have been reset (and boot firmware kicked off etc).
	 */
	if (hcd->driver->reset && (retval = hcd->driver->reset(hcd)) < 0) {
		diag_printf("can't setup\n");
		//goto err_hcd_driver_setup;
		return -1;
	}
	hcd->rh_pollable = 1;

    /*
	// NOTE: root hub and controller capabilities may not be the same 
	if (device_can_wakeup(hcd->self.controller)
			&& device_can_wakeup(&hcd->self.root_hub->dev))
		dev_dbg(hcd->self.controller, "supports USB remote wakeup\n");
       */
	/* enable irqs just before we start the controller */
	if (hcd->driver->irq) {

		/* IRQF_DISABLED doesn't work as advertised when used together
		 * with IRQF_SHARED. As usb_hcd_irq() will always disable
		 * interrupts we can remove it here.
		 */
		/*
		if (irqflags & IRQF_SHARED)
			irqflags &= ~IRQF_DISABLED;
             
		snprintf(hcd->irq_descr, sizeof(hcd->irq_descr), "%s:usb%d",
				hcd->driver->description, hcd->self.busnum);
		if ((retval = request_irq(irqnum, &usb_hcd_irq, irqflags,
				hcd->irq_descr, hcd)) != 0) {
			dev_err(hcd->self.controller,
					"request interrupt %d failed\n", irqnum);
			goto err_request_irq;
		}
		 */
		/*
	    cyg_interrupt_create(irqnum,
                        0,
                        (cyg_addrword_t)hcd,
                        (cyg_ISR_t *)hcd->driver->irq,
                        (cyg_DSR_t *)0,
                        &USB_host_interrupt_handle,
                        &USB_host_interrupt);
	    cyg_interrupt_attach(USB_host_interrupt_handle);
	    cyg_interrupt_configure(irqnum, 1, 0);
        cyg_interrupt_unmask(irqnum);
             */
		hcd->irq = irqnum;
		diag_printf("irq %d, %s 0x%08llx\n", irqnum,
				(hcd->driver->flags & HCD_MEMORY) ?
					"io mem" : "io base",
					(unsigned long long)hcd->rsrc_start);
	} else {
		hcd->irq = -1;
		if (hcd->rsrc_start)
			diag_printf("%s 0x%08llx\n",
					(hcd->driver->flags & HCD_MEMORY) ?
					"io mem" : "io base",
					(unsigned long long)hcd->rsrc_start);
	}

	if ((retval = hcd->driver->start(hcd)) < 0) {
		diag_printf("startup error %d\n", retval);
		//goto err_hcd_driver_start;
		return retval;
	}

	/* starting here, usbcore will pay attention to this root hub */
	rhdev->bus_mA = min(500u, hcd->power_budget);
	if ((retval = register_root_hub(hcd)) != 0){
		//goto err_register_root_hub;
		diag_printf("register root hub error!\n");
		return retval;
	}
	
	//if (hcd->uses_new_polling && HCD_POLL_RH(hcd))
	//	usb_hcd_poll_rh_status(hcd);
	return retval;
#if 0
error_create_attr_group:
	if (HC_IS_RUNNING(hcd->state))
		hcd->state = HC_STATE_QUIESCING;
	spin_lock_irq(&hcd_root_hub_lock);
	hcd->rh_registered = 0;
	spin_unlock_irq(&hcd_root_hub_lock);

#ifdef CONFIG_USB_SUSPEND
	cancel_work_sync(&hcd->wakeup_work);
#endif
	mutex_lock(&usb_bus_list_lock);
	usb_disconnect(&rhdev);		/* Sets rhdev to NULL */
	mutex_unlock(&usb_bus_list_lock);
err_register_root_hub:
	hcd->rh_pollable = 0;
	clear_bit(HCD_FLAG_POLL_RH, &hcd->flags);
	del_timer_sync(&hcd->rh_timer);
	hcd->driver->stop(hcd);
	hcd->state = HC_STATE_HALT;
	clear_bit(HCD_FLAG_POLL_RH, &hcd->flags);
	del_timer_sync(&hcd->rh_timer);
err_hcd_driver_start:
	if (hcd->irq >= 0)
		free_irq(irqnum, hcd);
err_request_irq:
err_hcd_driver_setup:
err_set_rh_speed:
	usb_put_dev(hcd->self.root_hub);
err_allocate_root_hub:
	usb_deregister_bus(&hcd->self);
err_register_bus:
	hcd_buffer_destroy(hcd);
	return retval;
#endif
} 
//EXPORT_SYMBOL_GPL(usb_add_hcd);


int rt3xxx_ehci_probe(void)
{
	struct usb_hcd *hcd;
	const struct hc_driver *driver = &rt3xxx_ehci_hc_driver;
	struct resource *res;
	int irq;
	int retval = 0;

	if (usb_disabled())
		return -ENODEV;

    /*
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "Found HC with no IRQ.\n");
		return -ENODEV;
	}
	irq = res->start;
       */
    irq = IRQ_RT3XXX_USB;  

	
	hcd = usb_create_hcd(driver, "rt3xxx" /*dev_name(&pdev->dev)*/);
	if (!hcd) {
		retval = -ENOMEM;
		return retval;
	}

    #if 0 //Jody
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev,	"Found HC with no register addr.\n");
		retval = -ENODEV;
		goto fail_request_resource;
	}
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = res->end - res->start + 1;
    #endif
	
    hcd->rsrc_start = EHCI_register_start;
	hcd->rsrc_len = EHCI_register_end- EHCI_register_start + 1;

	#if 0//Jody
	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, driver->description)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		retval = -EBUSY;
		goto fail_request_resource;
	}

	hcd->regs = ioremap_nocache(hcd->rsrc_start, hcd->rsrc_len);
	if (hcd->regs == NULL) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		retval = -EFAULT;
		goto fail_ioremap;
	}
    #endif
	
	// reset host controller
	//rt_usbhost_reset();

	// wake up usb module from power saving mode...
	try_wake_up();

	// change port0 to host mode
	rt_set_host();

    #if 0//Jody
	retval = usb_add_hcd(hcd, irq, IRQF_DISABLED | IRQF_SHARED);
	if (retval)
		goto fail_add_hcd;

	return retval;

fail_add_hcd:
	iounmap(hcd->regs);
fail_ioremap:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
fail_request_resource:
	usb_put_hcd(hcd);
fail_create_hcd:
	dev_err(&pdev->dev, "RT3xxx EHCI init fail. %d\n", retval);
	#endif
	/*
	cyg_interrupt_create(irq,
                        0,
                        (cyg_addrword_t)0,
                        (cyg_ISR_t *)MGC_AfsUdsIsr,
                        (cyg_DSR_t *)0,
                        &USB_host_interrupt_handle,
                        &USB_host_interrupt);
	cyg_interrupt_attach(USB_host_interrupt_handle);
	cyg_interrupt_configure(irq, 1, 0);
    cyg_interrupt_unmask(irq);
	*/
	if (driver->reset && (retval = driver->reset(hcd)) < 0) {
		diag_printf("setup error %d\n", retval);
		return retval;
	}

	if ((retval = driver->start(hcd)) < 0) {
		diag_printf("startup error %d\n", retval);
		return retval;
	}
	
	return retval;
}

static int rt3xxx_ehci_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	/* ehci_shutdown() is supposed to be called implicitly in 
	   ehci-hcd common code while removing module, but it isn't. */
	ehci_shutdown(hcd);

    #if 0//Jody
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);

	if(!usb_find_device(0x0, 0x0)) // No any other USB host controller.
		try_sleep();
    #endif
	
	return 0;
}

static void *ehci_alloc(size_t sz, size_t align, int td_num)
{
//	static struct QH qh __attribute__((aligned(32)));
//	static struct qTD td[3] __attribute__((aligned (32)));
	static int ntds=0;
	void *p;

	switch (sz) {
	case sizeof(struct QH):
		p = &alloc_qh;
		ntds = 0;
		break;
	case sizeof(struct qTD):
		if (ntds == 3) {
			diag_printf("out of TDs\n");
			return NULL;
		}
		p = &alloc_td[ntds];
		ntds++;
		break;
	default:
		diag_printf("unknown allocation size\n");
		return NULL;
	}

	p = KSEG1ADDR(p);
	//memset(p, 0, sz); //Jody
	
	return p;
}

static void *ehci_dongle_alloc(size_t sz, size_t align, int td_num)
{
//	static struct QH qh __attribute__((aligned(32)));
//	static struct qTD td[3] __attribute__((aligned (32)));
	//static int ntds=0;
	void *p;

	switch (sz) {
	case sizeof(struct QH):
		p = &alloc_qh[td_num];
		//ntds = 0;
		break;
	case sizeof(struct qTD):
		/*
		if (ntds == 6) {
			diag_printf("out of TDs\n");
			return NULL;
		}
		*/
		p = &alloc_td[td_num];
		//ntds++;
		break;
	default:
		diag_printf("unknown allocation size\n");
		return NULL;
	}

	p = KSEG1ADDR(p);
	//memset(p, 0, sz); //Jody
	
	return p;
}


#define cache_op(op,addr)						\
		__asm__ __volatile__(						\
		"	.set	push					\n" \
		"	.set	noreorder				\n" \
		"	.set	mips3\n\t				\n" \
		"	cache	%0, %1					\n" \
		"	.set	pop 				\n" \
		:								\
		: "i" (op), "R" (*(unsigned char *)(addr)))

void flush_dcache_range(unsigned long start_addr, unsigned long stop)
{
  unsigned long lsize = 32;
  unsigned long addr = start_addr & ~(lsize - 1);
  unsigned long aend = (stop - 1) & ~(lsize - 1);
	
  while (1) 
  {
    cache_op(HIT_WRITEBACK_INV_D, addr);
	if (addr == aend)
	  break;
	addr += lsize;
  }
}

static int ehci_td_buffer(struct qTD *td, void *buf, size_t sz)
{
	uint32_t addr, delta, next;
	int idx;
	//void *buf_noncache;
	addr = (uint32_t) buf;
	
	flush_dcache_range(addr, addr + sz);
	idx = 0;
	while (idx < 5) {
		//td->qt_buffer[idx] = EHCI_virt_to_bus(cpu_to_hc32(addr));
		td->qt_buffer[idx] = EHCI_virt_to_bus(cpu_to_le32(addr));
		next = (addr + 4096) & ~4095;
		delta = next - addr;
		if (delta >= sz)
			break;
		sz -= delta;
		addr = next;
		idx++;
	}

	if (idx == 5) {
		diag_printf("out of buffer pointers (%u bytes left)\n", sz);
		return -1;
	}

	//buf_noncache = KSEG1ADDR(buf);
	//memcpy(buf_noncache, buf, sz);

	return 0;
}

static void ehci_free(void *p, size_t sz)
{
}
static inline void ehci_flush_dcache(struct QH *qh)
{
}

static inline void ehci_invalidate_dcache(struct QH *qh)
{
}

static int ehci_submit_async(MUSB_Port *pPort, unsigned long pipe, void *buffer,
		   int length, struct devrequest *req)
{
	struct QH *qh;
	struct qTD *td;
	volatile struct qTD *vtd;
	unsigned long ts;
	uint32_t *tdp;
	uint32_t endpt, token, usbsts;
	uint32_t c, toggle;
	uint32_t cmd;
	//uint16_t maxpacket = 64;
	int ret = 0, bEnd;
    
	if (req != NULL)
		usb_dbg("ehci_submit_async(): req=%d, type=%d, value=%d, index=%d, wlength:%d\n",
		      req->request, req->requesttype, le16_to_cpu(req->value), le16_to_cpu(req->index), le16_to_cpu(req->length));
	else
		usb_dbg("ehci_submit_async():length:%d\n", length);
    
	qh = ehci_alloc(sizeof(struct QH), 32, 0);
	if (qh == NULL) {
		diag_printf("unable to allocate QH\n");
		return -1;
	}
	
	qh->qh_link = EHCI_virt_to_bus((uint32_t)qh_list | QH_LINK_TYPE_QH);
	//qh->qh_link = ((uint32_t)qh_list | QH_LINK_TYPE_QH);
	//c = (usb_pipespeed(pipe) != USB_SPEED_HIGH &&
	//     usb_pipeendpoint(pipe) == 0) ? 1 : 0;
	c = 0;//Jody
	endpt = (8 << 28) |
	    (c << 27) |
	    //(usb_maxpacket(dev, pipe) << 16) |
	    (EHCI_MaxPacket << 16) |
	    (0 << 15) |
	    (1 << 14) |
	    (usb_pipespeed(pipe) << 12) |
	    (usb_pipeendpoint(pipe) << 8) |
	    (0 << 7) | (usb_pipedevice(pipe) << 0);
	
	//qh->qh_endpt1 = cpu_to_hc32(endpt);
	//qh->qh_endpt1 = cpu_to_le32(endpt);//Jody
	qh->qh_endpt1 = endpt;//Jody

	//usb_dbg("qh->qh_endpt1:0x%x\n", qh->qh_endpt1);
	
	endpt = (1 << 30) |
	    //(dev->portnr << 23) |
	    //(dev->parent->devnum << 16) | (0 << 8) | (0 << 0);
	    (0 << 23) |
	    (1 << 16) | (0 << 8) | (0 << 0);
	//qh->qh_endpt2 = cpu_to_le32(endpt);
	//qh->qh_overlay.qt_next = cpu_to_le32(QT_NEXT_TERMINATE);
	//qh->qh_overlay.qt_altnext = cpu_to_le32(QT_NEXT_TERMINATE);
	
	qh->qh_endpt2 = endpt;
	qh->qh_overlay.qt_next = QT_NEXT_TERMINATE;
	qh->qh_overlay.qt_altnext = QT_NEXT_TERMINATE;
	//usb_dbg("qh->qh_endpt2:0x%x\n", qh->qh_endpt2);

	td = NULL;
	tdp = &qh->qh_overlay.qt_next;

	toggle = mtk_usb_gettoggle(usb_pipeendpoint(pipe), usb_pipeout(pipe));

	if (req != NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32, 0);
		if (td == NULL) {
			diag_printf("unable to allocate SETUP td\n");
			goto fail;
		}

		//td->qt_next = cpu_to_le32(QT_NEXT_TERMINATE);
		//td->qt_altnext = cpu_to_le32(QT_NEXT_TERMINATE);
		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = (0 << 31) |
		    (sizeof(*req) << 16) |
		    (1 << 15) | (0 << 12) | (3 << 10) | (2 << 8) | (0x80 << 0);
		
		//td->qt_token = cpu_to_le32(token);
		td->qt_token = token;
		//usb_dbg("setup token:0x%x\n", td->qt_token);

		if (ehci_td_buffer(td, req, sizeof(*req)) != 0) {
			diag_printf("unable construct SETUP td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		//*tdp = EHCI_virt_to_bus(cpu_to_le32((uint32_t) td));
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		
		tdp = &td->qt_next;
		toggle = 1;
	}
    
	if(ECOS_USBHost_State == MTK_USBHost_StorageInit)
	{
	  toggle = 0;
	  ECOS_USBHost_State = MTK_USBHost_StorageEnable;
	}

	if (length > 0 || req == NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32, 1);

		if (td == NULL) {
			diag_printf("unable to allocate DATA td\n");
			goto fail;
		}
		
		//td->qt_next = cpu_to_le32(QT_NEXT_TERMINATE);
		//td->qt_altnext = cpu_to_le32(QT_NEXT_TERMINATE);
		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		
		token = (toggle << 31) |
		    (length << 16) |
		    ((req == NULL ? 1 : 0) << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 1 : 0) << 8) | (0x80 << 0);

		//td->qt_token = cpu_to_le32(token);
		td->qt_token = token;
	    //usb_dbg("data token:0x%x\n", td->qt_token);
		
		if (ehci_td_buffer(td, buffer, length) != 0) {
			diag_printf("unable construct DATA td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		//*tdp = EHCI_virt_to_bus(cpu_to_le32((uint32_t) td));
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		tdp = &td->qt_next;
	}

	if (req != NULL) {
		td = ehci_alloc(sizeof(struct qTD), 32, 2);
		if (td == NULL) {
			diag_printf("unable to allocate ACK td\n");
			goto fail;
		}

		//td->qt_next = cpu_to_le32(QT_NEXT_TERMINATE);
		//td->qt_altnext = cpu_to_le32(QT_NEXT_TERMINATE);
		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = 0;
		token = (toggle << 31) |
		    (0 << 16) |
		    (1 << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 0 : 1) << 8) | (0x80 << 0);

		//td->qt_token = cpu_to_le32(token);
		//*tdp = EHCI_virt_to_bus(cpu_to_le32((uint32_t) td));
		td->qt_token = token;
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		//usb_dbg("ack token:0x%x\n", td->qt_token);
		
		tdp = &td->qt_next;
	}

    /*
	cmd = mtk_readl(EHCI_REGS_COMMAND);
	if(cmd & CMD_ASE){
	  int ret = 0;
	  // Disable async schedule. 
	  cmd &= ~CMD_ASE;
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	
	  ret = MT7620_handshake(EHCI_REGS_STATUS, STS_ASS, 0, 100);
	  if (ret < 0) {
		diag_printf("EHCI fail timeout STD_ASS reset\n");
	  }
	
	  qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh_list | QH_LINK_TYPE_QH));
	}
       */

	//qh_list->qh_link = EHCI_virt_to_bus(cpu_to_hc32((uint32_t) qh | QH_LINK_TYPE_QH));
	qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));

	/* Flush dcache */
	//ehci_flush_dcache(qh_list);

	//usbsts = mtk_readl(EHCI_REGS_STATUS);
	//mtk_writel((usbsts & 0x3f), EHCI_REGS_STATUS);

	/* Enable async. schedule. */
	cmd = mtk_readl(EHCI_REGS_COMMAND);
	if(cmd | CMD_RUN == 0)
	{
	  cmd |= CMD_RUN; 
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}
	if(cmd | CMD_ASE == 0)
	{   
	 cmd |= CMD_ASE;
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}
	
	//read_ehci_registers_test();
	return  0;

fail:
	//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
	td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	while (td != (void *)QT_NEXT_TERMINATE) {
		qh->qh_overlay.qt_next = td->qt_next;
		ehci_free(td, sizeof(*td));
		//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
		td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	}
	ehci_free(qh, sizeof(*qh));
	return -1;
}

static int ehci_submit_dongleRead_async(MUSB_Port *pPort, unsigned long pipe, void *buffer,
		   int length, struct devrequest *req)
{
	struct QH *qh, *qh1;
	struct qTD *td;
	volatile struct qTD *vtd;
	unsigned long ts;
	uint32_t *tdp;
	uint32_t endpt, token, usbsts;
	uint32_t c, toggle;
	uint32_t cmd;
	//uint16_t maxpacket = 64;
	int ret = 0, bEnd;
    
	if (req != NULL)
		usb_dbg("ehci_submit_dongleRead_async(): req=%d, type=%d, value=%d, index=%d, wlength:%d\n",
		      req->request, req->requesttype, le16_to_cpu(req->value), le16_to_cpu(req->index), le16_to_cpu(req->length));
	else
		usb_dbg("ehci_submit_dongleRead_async():pipe:0x%x, length:%d\n", pipe, length);
    
	qh = ehci_dongle_alloc(sizeof(struct QH), 32, 0);//read queue head
	qh1 = ehci_dongle_alloc(sizeof(struct QH), 32, 1);//write queue head
	if (qh == NULL) {
		diag_printf("unable to allocate QH\n");
		return -1;
	}

	if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh_list | QH_LINK_TYPE_QH)))
	{
	  qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));
	  qh->qh_link = EHCI_virt_to_bus((uint32_t)qh_list | QH_LINK_TYPE_QH);
	}else if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh1| QH_LINK_TYPE_QH)))
	{
	  qh1->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));
	  qh->qh_link = EHCI_virt_to_bus((uint32_t)qh_list | QH_LINK_TYPE_QH);
	}else if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh| QH_LINK_TYPE_QH)))
	{
	}
	
	c = 0;//Jody
	endpt = (8 << 28) |
	    (c << 27) |
	    (EHCI_MaxPacket << 16) |
	    (0 << 15) |
	    (1 << 14) |
	    (usb_pipespeed(pipe) << 12) |
	    (usb_pipeendpoint(pipe) << 8) |
	    (0 << 7) | (usb_pipedevice(pipe) << 0);
	
	qh->qh_endpt1 = endpt;//Jody	
	endpt = (1 << 30) |
	    (0 << 23) |
	    (1 << 16) | (0 << 8) | (0 << 0);
	
	qh->qh_endpt2 = endpt;
	qh->qh_overlay.qt_next = QT_NEXT_TERMINATE;
	qh->qh_overlay.qt_altnext = QT_NEXT_TERMINATE;

	td = NULL;
	tdp = &qh->qh_overlay.qt_next;

	toggle = mtk_usb_gettoggle(usb_pipeendpoint(pipe), usb_pipeout(pipe));

	if (req != NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 0);
		if (td == NULL) {
			diag_printf("unable to allocate SETUP td\n");
			goto fail;
		}

		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = (0 << 31) |
		    (sizeof(*req) << 16) |
		    (1 << 15) | (0 << 12) | (3 << 10) | (2 << 8) | (0x80 << 0);
		td->qt_token = token;

		if (ehci_td_buffer(td, req, sizeof(*req)) != 0) {
			diag_printf("unable construct SETUP td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		
		tdp = &td->qt_next;
		toggle = 1;
	}

	if (length > 0 || req == NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 1);

		if (td == NULL) {
			diag_printf("unable to allocate DATA td\n");
			goto fail;
		}
		
		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		
		token = (toggle << 31) |
		    (length << 16) |
		    ((req == NULL ? 1 : 0) << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 1 : 0) << 8) | (0x80 << 0);

		td->qt_token = token;
		
		if (ehci_td_buffer(td, buffer, length) != 0) {
			diag_printf("unable construct DATA td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		tdp = &td->qt_next;
	}

	if (req != NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 2);
		if (td == NULL) {
			diag_printf("unable to allocate ACK td\n");
			goto fail;
		}

		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = 0;
		token = (toggle << 31) |
		    (0 << 16) |
		    (1 << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 0 : 1) << 8) | (0x80 << 0);

		td->qt_token = token;
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		
		tdp = &td->qt_next;
	}

	//qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));

	//usbsts = mtk_readl(EHCI_REGS_STATUS);
	//mtk_writel((usbsts & 0x3f), EHCI_REGS_STATUS);

	/* Enable async. schedule. */
	cmd = mtk_readl(EHCI_REGS_COMMAND);
	if(cmd | CMD_RUN == 0)
	{
	  cmd |= CMD_RUN; 
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}
	if(cmd | CMD_ASE == 0)
	{   
	 cmd |= CMD_ASE;
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}
	eCos_dongleRead_flag = 1;
	//diag_printf("end of ehci_submit_dongleRead_async()\n");
	return  0;

fail:
	//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
	td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	while (td != (void *)QT_NEXT_TERMINATE) {
		qh->qh_overlay.qt_next = td->qt_next;
		ehci_free(td, sizeof(*td));
		//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
		td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	}
	ehci_free(qh, sizeof(*qh));
	return -1;
}

static int ehci_submit_dongleWrite_async(MUSB_Port *pPort, unsigned long pipe, void *buffer,
		   int length, struct devrequest *req)
{
	struct QH *qh, *qh0;
	struct qTD *td;
	volatile struct qTD *vtd;
	unsigned long ts;
	uint32_t *tdp;
	uint32_t endpt, token, usbsts;
	uint32_t c, toggle;
	uint32_t cmd;
	//uint16_t maxpacket = 64;
	int ret = 0, bEnd;
    
	if (req != NULL)
		usb_dbg("ehci_submit_dongleWrite_async(): req=%d, type=%d, value=%d, index=%d, wlength:%d\n",
		      req->request, req->requesttype, le16_to_cpu(req->value), le16_to_cpu(req->index), le16_to_cpu(req->length));
	else
		usb_dbg("ehci_submit_dongleWrite_async():pipe:0x%x, length:%d\n", pipe, length);
	
	qh0 = ehci_dongle_alloc(sizeof(struct QH), 32, 0);//read queue head
	qh = ehci_dongle_alloc(sizeof(struct QH), 32, 1); //write queue head
	if (qh == NULL) {
		diag_printf("unable to allocate QH\n");
		return -1;
	}

	if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh_list | QH_LINK_TYPE_QH)))
	{
	  qh_list->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));
	  qh->qh_link = EHCI_virt_to_bus((uint32_t)qh_list | QH_LINK_TYPE_QH);
	}else if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh0| QH_LINK_TYPE_QH)))
	{
	  qh0->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));
	  qh->qh_link = EHCI_virt_to_bus((uint32_t)qh_list | QH_LINK_TYPE_QH);
	}else if(qh_list->qh_link ==  EHCI_virt_to_bus(cpu_to_le32((uint32_t)qh| QH_LINK_TYPE_QH)))
	{ 
	}
	
	c = 0;//Jody
	endpt = (8 << 28) |
	    (c << 27) |
	    (EHCI_MaxPacket << 16) |
	    (0 << 15) |
	    (1 << 14) |
	    (usb_pipespeed(pipe) << 12) |
	    (usb_pipeendpoint(pipe) << 8) |
	    (0 << 7) | (usb_pipedevice(pipe) << 0);	
	qh->qh_endpt1 = endpt;//Jody

	endpt = (1 << 30) |
	    (0 << 23) |
	    (1 << 16) | (0 << 8) | (0 << 0);
	
	qh->qh_endpt2 = endpt;
	qh->qh_overlay.qt_next = QT_NEXT_TERMINATE;
	qh->qh_overlay.qt_altnext = QT_NEXT_TERMINATE;
	//usb_dbg("qh->qh_endpt2:0x%x\n", qh->qh_endpt2);

	td = NULL;
	tdp = &qh->qh_overlay.qt_next;

	toggle = mtk_usb_gettoggle(usb_pipeendpoint(pipe), usb_pipeout(pipe));

	if (req != NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 3);
		if (td == NULL) {
			diag_printf("unable to allocate SETUP td\n");
			goto fail;
		}

		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = (0 << 31) |
		    (sizeof(*req) << 16) |
		    (1 << 15) | (0 << 12) | (3 << 10) | (2 << 8) | (0x80 << 0);
		td->qt_token = token;
		//usb_dbg("setup token:0x%x\n", td->qt_token);

		if (ehci_td_buffer(td, req, sizeof(*req)) != 0) {
			diag_printf("unable construct SETUP td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		tdp = &td->qt_next;
		toggle = 1;
	}

	if (length > 0 || req == NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 4);

		if (td == NULL) {
			diag_printf("unable to allocate DATA td\n");
			goto fail;
		}
		
		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		
		token = (toggle << 31) |
		    (length << 16) |
		    ((req == NULL ? 1 : 0) << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 1 : 0) << 8) | (0x80 << 0);
		td->qt_token = token;
	    //usb_dbg("data token:0x%x\n", td->qt_token);
		
		if (ehci_td_buffer(td, buffer, length) != 0) {
			diag_printf("unable construct DATA td\n");
			ehci_free(td, sizeof(*td));
			goto fail;
		}
		
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		tdp = &td->qt_next;
	}

	if (req != NULL) {
		td = ehci_dongle_alloc(sizeof(struct qTD), 32, 5);
		if (td == NULL) {
			diag_printf("unable to allocate ACK td\n");
			goto fail;
		}

		td->qt_next = QT_NEXT_TERMINATE;
		td->qt_altnext = QT_NEXT_TERMINATE;
		token = 0;
		token = (toggle << 31) |
		    (0 << 16) |
		    (1 << 15) |
		    (0 << 12) |
		    (3 << 10) |
		    ((usb_pipein(pipe) ? 0 : 1) << 8) | (0x80 << 0);

		td->qt_token = token;
		*tdp = EHCI_virt_to_bus((uint32_t) td);
		//usb_dbg("ack token:0x%x\n", td->qt_token);
		
		tdp = &td->qt_next;
	}

	//qh0->qh_link = EHCI_virt_to_bus(cpu_to_le32((uint32_t) qh | QH_LINK_TYPE_QH));
	//usbsts = mtk_readl(EHCI_REGS_STATUS);
	//mtk_writel((usbsts & 0x3f), EHCI_REGS_STATUS);

	/* Enable async. schedule. */
	cmd = mtk_readl(EHCI_REGS_COMMAND);
	if(cmd | CMD_RUN == 0)
	{
	  cmd |= CMD_RUN; 
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}
	if(cmd | CMD_ASE == 0)
	{   
	 cmd |= CMD_ASE;
	  mtk_writel(cmd, EHCI_REGS_COMMAND);
	}

	eCos_dongleWrite_flag = 1;
	return  0;

fail:
	//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
	td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	while (td != (void *)QT_NEXT_TERMINATE) {
		qh->qh_overlay.qt_next = td->qt_next;
		ehci_free(td, sizeof(*td));
		//td = (void *)hc32_to_cpu(qh->qh_overlay.qt_next);
		td = (void *)le32_to_cpu(qh->qh_overlay.qt_next);
	}
	ehci_free(qh, sizeof(*qh));
	return -1;
}

int submit_bulk_msg(MUSB_Port *pPort, unsigned long pipe, void *buffer, int length)
{
    /*
	if (usb_pipetype(pipe) != PIPE_BULK) {
		diag_printf("non-bulk pipe (type=%lu)\n", usb_pipetype(pipe));
		return -1;
	}
	*/
	
	return ehci_submit_async(pPort, pipe, buffer, length, NULL);
}

int submit_dongle_bulk_msg(MUSB_Port *pPort, unsigned long pipe, void *buffer, int length)
{
      int pipeIn = 0;
	pipeIn = usb_pipein(pipe) ? 1 : 0;

	if(pipeIn)
	{
	  //eCos_dongleRead_flag = 1;
	  return ehci_submit_dongleRead_async(pPort, pipe, buffer, length, NULL);
	}
	else
	{
	  //eCos_dongleWrite_flag = 1;
	  return ehci_submit_dongleWrite_async(pPort, pipe, buffer, length, NULL);
	}
}

int submit_control_msg(MUSB_Port *pPort, unsigned long pipe, void *buffer,
		   int length, struct devrequest *setup)
{
#if 0 //Jody
	if (usb_pipetype(pipe) != PIPE_CONTROL) {
		debug("non-control pipe (type=%lu)", usb_pipetype(pipe));
		return -1;
	}

	if (usb_pipedevice(pipe) == rootdev) {
		if (rootdev == 0)
			dev->speed = USB_SPEED_HIGH;
		return ehci_submit_root(dev, pipe, buffer, length, setup);
	}
#endif

	return ehci_submit_async(pPort, pipe, buffer, length, setup);
}


#if 0 //Jody
MODULE_ALIAS("rt3xxx-ehci");

static struct platform_driver rt3xxx_ehci_driver = {
	.probe = rt3xxx_ehci_probe,
	.remove = rt3xxx_ehci_remove,
	.shutdown = usb_hcd_platform_shutdown,
	.driver = {
		.name = "rt3xxx-ehci",
	},
};
#endif

