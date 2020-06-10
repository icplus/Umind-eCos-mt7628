/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

/*
 * MUSBStack-S USB Controller Driver Interface (UCDI).
 * $Revision: #1 $
 */

#ifndef __MUSB_CDI_H__
#define __MUSB_CDI_H__

#include "mu_arch.h"
#include "mu_tools.h"
#include "mu_dev.h"
#include "mu_stat.h"
#include "mu_dsi.h"
#include "sys/bsdtypes.h"

/************************* CDI CONSTANTS **************************/

/** Current UCDI version */
#define MUSB_CDI_REVISION	0x0100

/**
 * MUSB_PortType.
 * This is the type of USB port.
 */
typedef enum
{
    /** Unknown port type */
    MUSB_PORT_UNKNOWN,
    /** Function-only port */
    MUSB_PORT_TYPE_FUNCTION,
    /** Host-only port */
    MUSB_PORT_TYPE_HOST,
    /** OTG port */
    MUSB_PORT_TYPE_OTG
} MUSB_PortType;

/**
 * MUSB_PortSpeed.
 * This is the native speed of a USB port.
 */
typedef enum
{
    /** Unknown port speed */
    MUSB_PORT_SPEED_UNKNOWN,
    /** Low-speed USB */
    MUSB_PORT_SPEED_LOW,
    /** Full-speed USB */
    MUSB_PORT_SPEED_FULL,
    /** High-speed USB */
    MUSB_PORT_SPEED_HIGH
} MUSB_PortSpeed;

/**
 * MUSB_State.
 * The USB states passed to NewUsbState callback.
 */
typedef enum
{
    /** Powered off */
    MUSB_POWER_OFF,
    /** USB Powered state */
    MUSB_POWERED,
    /** USB Default state */
    MUSB_DEFAULT,
    /** USB Address state */
    MUSB_ADDRESS,
    /** USB Configured state */
    MUSB_CONFIGURED,
    /** Suspended (upon resume, the pre-suspend state will be signalled) */
    MUSB_SUSPENDED    
} MUSB_State;

/**
 * MUSB_OtgState.
 * The USB/OTG states passed to NewOtgState callback.
 */
typedef enum
{
    /** OTG IDLE state */
    MUSB_AB_IDLE = 0x00,
    /** OTG WAIT_VRISE state */
    MUSB_WAIT_VRISE = 0x01,
    /** OTG B_PERIPHERAL state */
    MUSB_B_PERIPHERAL= 0x11,
    /** OTG WAIT_ACON state */
    MUSB_B_WAIT_ACON = 0x12,
    /** OTG B_HOST state */
    MUSB_B_HOST = 0x13,
    /** OTG SRP_INIT state */
    MUSB_B_SRP_INIT = 0x14,
    /** OTG A_PERIPHERAL state */
    MUSB_A_PERIPHERAL= 0x21,
    /** OTG WAIT_BCON state */
    MUSB_A_WAIT_BCON = 0x22,
    /** OTG A_HOST state */
    MUSB_A_HOST = 0x23,
    /** OTG A_SUSPEND state */
    MUSB_A_SUSPEND = 0x24,
    /** Special transitional state */
    MUSB_S_TRANSITION=0x30
} MUSB_OtgState;

/**
 * MUSB_ConnectionSpeed.
 * This is the speed of a USB connection.
 */
typedef enum
{
    /** Unknown connection speed */
    MUSB_CONNECTION_SPEED_UNKNOWN,
    /** Low-speed USB */
    MUSB_CONNECTION_SPEED_LOW,
    /** Full-speed USB */
    MUSB_CONNECTION_SPEED_FULL,
    /** High-speed USB */
    MUSB_CONNECTION_SPEED_HIGH
} MUSB_ConnectionSpeed;

/**
 * MUSB_TargetListCode.
 * The "opcodes" in target list.
 * A target list consists of a byte array, with these opcodes,
 * each followed by relevant data.
 * The order of items in the list is significant; the algorithm is first-match.
 * Examples:
 * <dl>
 * <dt>Simple VID/PID-based acceptance list:
 * <dd>MUSB_TARGET_VID, 0xab, 0x14, MUSB_TARGET_PID, 0x34, 0x12, MUSB_TARGET_ACCEPT, 0,<br>
 * MUSB_TARGET_VID, 0x5a, 0xa5, MUSB_TARGET_PID, 0x00, 0x05, MUSB_TARGET_ACCEPT, 1
 * <dt>Accept a communication device using Ethernet control model:
 * <dd>MUSB_TARGET_CLASS, 2, MUSB_TARGET_INTERFACE, 0, MUSB_TARGET_CLASS, 6, MUSB_TARGET_ACCEPT, 0
 * <dt>Accept all printers, regardless of VID/PID:
 * <dd>MUSB_TARGET_INTERFACE, 0, MUSB_TARGET_CLASS, 7, MUSB_TARGET_SUBCLASS, 1, MUSB_TARGET_ACCEPT, 2
 * <dt>Reject all printers with a uni-directional interface, but accept all other printers:
 * <dd>MUSB_TARGET_INTERFACE, 0, MUSB_TARGET_CLASS, 7, MUSB_TARGET_SUBCLASS, 1, MUSB_TARGET_PROTOCOL, 1, MUSB_TARGET_REJECT,<br>
 * MUSB_TARGET_INTERFACE, 0, MUSB_TARGET_CLASS, 7, MUSB_TARGET_SUBCLASS, 1, MUSB_TARGET_ACCEPT, 0
 * </dl>
 */
typedef enum
{
    /** 
     * Silently skipped; allows padding for cleaner-looking static declarations
     */
    MUSB_TARGET_UNKNOWN,
    
    /** Match a VID; followed by 2-byte VID (USB byte order) */
    MUSB_TARGET_VID,
    
    /** Match a PID; followed by 2-byte PID (USB byte order) */
    MUSB_TARGET_PID,
    
    /** 
     * Match a Device BCD code; 
     * followed by 2-byte Device BCD code (USB byte order)
     */
    MUSB_TARGET_DEVICE_BCD,
    
    /** 
     * Match the existence of a non-primary configuration and 
     * traverse to it; followed by 1-byte config index
     */
    MUSB_TARGET_CONFIG,
    
    /**
     * Match the existence of an interface and traverse to it; 
     * followed by 1-byte interface number
     */
    MUSB_TARGET_INTERFACE,
    
    /** 
     * Match a class at the current level (device or interface); 
     * followed by 1-byte class code
     */
    MUSB_TARGET_CLASS,
    
    /** 
     * Match a subclass at the current level (device or interface); 
     * followed by 1-byte subclass code
     */
    MUSB_TARGET_SUBCLASS,
    
    /** 
     * Match a protocol at the current level (device or interface); 
     * followed by 1-byte protocol code
     */
    MUSB_TARGET_PROTOCOL,
    
    /** 
     * Accept a target meeting previously-encountered criteria; 
     * followed by a 1-byte array index into the host client array.
     */
    MUSB_TARGET_ACCEPT,
    
    /** 
     * Reject a target meeting previously-encountered criteria; 
     * move on to next list item
     */
    MUSB_TARGET_REJECT
    
} MUSB_TargetListCode;

/*************************** CDI TYPES ****************************/

typedef void (*MUSB_pfSocketNfy)(void *pvSocketInfo);
typedef void (*MUSB_pfOverCurrentNfy)(UINT32 u4PortNum, UINT32 u4Status);

typedef struct _MUSB_SOCKET_NFY_INFO_T{
    UINT32 u4DeviceClass;   /* OUT: device class code */
    UINT32 u4Status;            /* OUT: device status */
    void *pvParentDevice;    /* OUT: Driver parent of device pointer*/ 
    void *pvDevice;               /* OUT: Driver device pointer */  
    UINT32 u4SocketNo;        /* OUT:  the socket number which the child device is 
                                                            attached to, Start from 0 */
    UINT32 u4CompId;          /* IN: usb_socket_mw set u4CompId id */
    void *prDevStatusInfo;   /* IN: usb_socket_mw set USB_DEV_STATUS_INFO_T pointer */
    MUSB_pfSocketNfy pfSocketNfy;    /* IN: usb_socket_mw set call back */
} MUSB_SOCKET_NFY_INFO_T;

/**
 * MUSB_Port.
 * A USB receptacle (today, this maps one-to-one to an Inventra controller).
 *
 * @field bUsbPort the h/w USB port number.
 *
 * @field Type the type of this port
 *
 * @field Speed the native speed of this port
 *
 * @field pPrivateData port-specific data; not to be interpreted by CDI client
 */
typedef struct 
{
    void* pPrivateData; /* MGC_Port* pPortImpl; */
    uint8_t bUsbPort;    
    MUSB_PortType Type;
    MUSB_PortSpeed Speed;
    MUSB_SOCKET_NFY_INFO_T *prSocketInfo; /* Support socket device manager. */
    MUSB_pfOverCurrentNfy pfOverCurrentNfy;    /* IN: usb_socket_mw set call back */
} MUSB_Port;

struct _MUSB_Device;
/**
 * MUSB_Device.
 * A device automatically addressed (but not configured)
 * by the UCD when the controller is in host mode.
 * Descriptors are left in USB byte order.
 * To get the value in a 16-bit field assuming you cast to the desired type, 
 * use MUSB_SWAP16P(&(pDescriptor->wShortField)).
 *
 * @field pPort the port on which the device is connected
 *
 * @field ConnectionSpeed connection speed
 * 
 * @field bBusAddress as per the USB standard; 1-127 are valid
 *
 * @field DeviceDescriptor as per the USB standard
 *
 * @field pCurrentConfiguration a device driver should 
 * set this to the apConfigDescriptors element in use.
 *
 * @field apConfigDescriptors as a convenience, an array of pointers to
 * configuration descriptors inside the descriptor buffer
 *
 * @field pDescriptorBuffer buffer of all "full" configuration descriptors
 * (that is, the set of descriptors associated with a configuration,
 * implied by the wTotalLength field of the configuration descriptor
 *
 * @field wDescriptorBufferLength the length (in bytes) of the descriptor buffer
 *
 * @field pPrivateData implementation data; not to be interpreted by UCDI clients
 * 
 * @field pDriverPrivateData driver instance data; not to be interpreted by UCD
 */
typedef struct _MUSB_Device
{
    MUSB_Port* pPort;
    void* pPrivateData;             /* MGC_Device* pDevice*/
    void* pDriverPrivateData;   /* MGC_MsdDevice* pMsdDevice or MUSB_HubDriverContext* */
    MUSB_DeviceDescriptor DeviceDescriptor;
    MUSB_ConfigurationDescriptor* pCurrentConfiguration;
    MUSB_ConfigurationDescriptor** apConfigDescriptors;
    uint8_t* pDescriptorBuffer;
    uint16_t wDescriptorBufferLength;
    uint16_t wLangId;
    uint8_t* pLangId;
    uint8_t bGotLangId;
    uint8_t* pStrManufacturerBuffer;
    uint8_t bGotStrManufacturer;
    uint8_t* pStrProductBuffer;
    uint8_t bGotStrProduct;
    uint8_t* pStrSerialNumBuffer;    
    uint8_t bGotStrSerialNum;
    MUSB_ConnectionSpeed ConnectionSpeed;
    uint8_t bBusAddress;
    struct _MUSB_Device* pParentUsbDevice;
    uint8_t bHubPort;
    uint8_t bDeviceClass;    
    uint8_t bPortCount;    /* Number of ports on HUB or card reader. */
    uint8_t bSupport;       /* Device is accepted or not. */    
} MUSB_Device;

/** A handle to a port's bus */
typedef void* MUSB_BusHandle;

/**
 * MUSB_DeviceEndpoint.
 * Specification of a device-side endpoint for host use.
 *
 * @field UsbDescriptor the endpoint descriptor as per the USB standard
 *
 * @field pDevice associated device
 *
 * @field wNakLimit maximum number of NAK handshakes seen before
 * declaring an error (0xffff to disable NAK limit feature).
 * This works as per USB 2.0, so at full-speed it is the number of NAKs,
 * and at high-speed it is m, where the number of NAKs is 2^(m-1)
 */
typedef struct
{
    const MUSB_Device* pDevice;
    MUSB_EndpointDescriptor UsbDescriptor;
    uint16_t wNakLimit;
} MUSB_DeviceEndpoint;

/**
 * MUSB_EndpointResource.
 * A (local) resource for communicating with an endpoint.
 * A resource may be bound to a remote endpoint (e.g. Interrupt or Isochronous traffic),
 * and is always bound on the device side.
 * NOTE: members are not declared volatile; code will cast as necessary.
 *
 * @field dwBufferSize size (in bytes) of the underlying FIFO, ring buffer, etc.
 *
 * @field bmFlags flag bitmask (combination of MUSB_ENDPOINT_RESOURCE_FLAG_*)
 *
 * @field pPrivateData implementation data; not to be interpreted by CDI clients
 */
typedef struct
{
    uint32_t dwBufferSize;
    uint32_t bmFlags;
    void* pPrivateData;   /* MGC_EndpointResource* pMGC_EndpointResource */
} MUSB_EndpointResource;
/**
 * Flag denoting local resource is bound (not shared with other remote
 * endpoints [e.g. multiple bulk endpoints can share local resources])
 */
#define MUSB_ENDPOINT_RESOURCE_FLAG_BIND	(1 << 0)

/** Flag for DMA capability */
#define MUSB_ENDPOINT_RESOURCE_FLAG_DMA		(1 << 1)

/**
 * Pipe handle
 */
typedef void* MUSB_Pipe;

struct _MUSB_ControlIrp;

/**
 * Control IRP completion handler.
 * A completion handler for control transfers.
 * @param 1 pCompleteParam from IRP
 * @param 2 IRP pointer
 */
//typedef void (*MUSB_pfControlIrpComplete)(void*, struct _MUSB_ControlIrp*);
typedef void (*MUSB_pfControlIrpComplete)(void*);//Jody

/**
 * MUSB_ControlIrp.
 * IRP as per USB spec, for control transfers.
 * NOTE: members are not declared volatile; code will cast as necessary.
 *
 * @field pDevice pointer to target device
 *
 * @field pOutBuffer OUT data buffer, including the 8-byte standard request
 *
 * @field dwOutLength OUT data buffer length, in bytes
 *
 * @field pInBuffer IN data buffer
 *
 * @field dwInLength IN buffer length, in bytes
 *
 * @field dwStatus status on completion (updated before completion handler is called)
 *
 * @field dwActualOutLength actual OUT transfer length
 *
 * @field dwActualInLength actual IN transfer length
 *
 * @field pfIrpComplete required completion handler
 *
 * @field pCompleteParam passed to completion callback
 */
typedef struct _MUSB_ControlIrp
{
    const MUSB_Device* pDevice;
    const uint8_t* pOutBuffer;
    uint32_t dwOutLength;
    uint8_t* pInBuffer;
    uint32_t dwInLength;
    uint32_t dwStatus;
    uint32_t dwActualOutLength;
    uint32_t dwActualInLength;
    MUSB_pfControlIrpComplete pfIrpComplete;
    void* pCompleteParam;
} MUSB_ControlIrp;

struct _MUSB_Irp;

/**
 * Bulk/Interrupt IRP completion handler.
 * A completion handler for bulk/interrupt transfers.
 * @param 1 pCompleteParam from IRP
 * @param 2 IRP pointer
 */
typedef void (*MUSB_pfIrpComplete)(void*, const struct _MUSB_Irp*);

/**
 * MUSB_Irp (IRP for bulk/interrupt traffic).
 * IRP as per USB spec, for bulk/interrupt traffic.
 * NOTE: members are not declared volatile; code will cast as necessary.
 *
 * @field hPipe pipe handle, as returned by MUSB_OpenPipe or MUSB_Claim
 *
 * @field bAllowShortTransfer TRUE to treat a shorter-than-expected receive as successful,
 * or to transmit a null packet if the last packet is the endpoint's packet size
 *
 * @field pBuffer data buffer
 *
 * @field dwLength data buffer length, in bytes
 *
 * @field dwStatus status on completion (updated before completion handler is called)
 *
 * @field dwActualLength actual transfer length
 *
 * @field pfIrpComplete required completion handler
 *
 * @field pCompleteParam passed to completion callback
 *
 * @field bIsrCallback TRUE to issue callback in interrupt context;
 * FALSE to issue from background
 *
 * @field bAllowDma TRUE to allow DMA; FALSE otherwise
 */
typedef struct _MUSB_Irp
{
    MUSB_Pipe hPipe;
    uint8_t* pBuffer;
    uint32_t dwLength;
    uint32_t dwStatus;
    uint32_t dwActualLength;
    MUSB_pfIrpComplete pfIrpComplete;
    void* pCompleteParam;
    uint8_t bAllowShortTransfer;
    uint8_t bIsrCallback;
    uint8_t bAllowDma;
} MUSB_Irp, MUSB_BulkIrp, MUSB_InterruptIrp;

struct _MUSB_IsochIrp;

typedef struct _ehci_Irp
{
  MUSB_Irp *irp;
  MUSB_Pipe ePipe;
}ehci_Irp;

/**
 * Isochronous IRP completion handler.
 * A completion handler for isochronous transfers.
 * @param 1 pCompleteParam from IRP
 * @param 2 IRP pointer
 */
typedef void (*MUSB_pfIsochIrpComplete)(void*, struct _MUSB_IsochIrp*);

#ifdef MUSB_ISO_STREAM
typedef struct _MUSB_IsoPtr
{
    uint32_t dwRp;
    uint32_t dwWp;
    uint32_t dwWEp;  /// bottom of write pointer before wrapped.
    uint8_t bEmpty;  /// After read pointer increase, read = write pointer.
} MUSB_IsoPtr;

#else

typedef struct _MUSB_IsoPacket 
{
    UINT32 dwDataLength;		/* expected length */
    UINT32 dwActualDataLength;
    UINT32 dwStatus;
}MUSB_IsoPacket;

#endif

/**
 * MUSB_IsochIrp (IRP for isochronous traffic).
 * IRP as per USB spec, for isochronous traffic.
 * NOTE: members are not declared volatile; code will cast as necessary.
 *
 * @field hPipe pipe handle, as returned by MUSB_OpenPipe or MUSB_Claim
 *
 * @field wFrameCount the number of (micro)frames of data represented by this IRP
 *
 * @field pBuffer data buffer
 *
 * @field adwLength array of transfer lengths (in bytes), one per (micro)frame
 *
 * @field adwStatus array of storage for status codes, one per (micro)frame
 *
 * @field adwActualLength array of storage for actual transfer lengths, one per (micro)frame
 *
 * @field wCurrentFrame current (micro)frame, updated before calling callback
 * (but this should be considered volatile by completion handler)
 *
 * @field wCallbackInterval how often, in (micro)frames, to call the completion
 * handler before the IRP's last frame is reached
 * (0 to disable this feature)
 *
 * @field pfIrpComplete required completion handler
 *
 * @field pCompleteParam passed to completion callback
 *
 * @field bIsrCallback TRUE to issue callback in interrupt context;
 * FALSE to issue from background
 *
 * @field bAllowDma TRUE to allow DMA; FALSE otherwise
 */
typedef struct _MUSB_IsochIrp
{
    MUSB_Pipe hPipe;
    uint8_t* pBuffer;
#ifdef MUSB_ISO_STREAM
    uint32_t dwLength;    /* Normally, this is set with MaxPktSize. */
    uint32_t dwBufSize;
    uint32_t dwThreshold;    
    MUSB_IsoPtr *prIsoPtr;
#else
    MUSB_IsoPacket *prISOpacket;  /*Element number is depended on wFrameCount*/
    uint16_t wFrameCount;
    uint16_t wCurrentFrame;
    uint16_t wCallbackInterval;
#endif
    MUSB_pfIsochIrpComplete pfIrpComplete;
    void* pCompleteParam;
    uint8_t bIsrCallback;
    uint8_t bAllowDma;
} MUSB_IsochIrp;

/**
 * USB state notification.
 * A callback to inform a client of a new USB state.
 * <p>
 * NOTE: this is called from interrupt context.
 * Its purpose is informational only; 
 * the callee is expected to do something low-overhead like updating state variables.
 * The callee should not call OS services unless they are interrupt-safe.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 bus "handle"
 * @param 3 USB state
 */
typedef void (*MUSB_pfNewUsbState)(void*, MUSB_BusHandle, MUSB_State);

/**
 * SETUP servicing.
 * Requests a function client to service a control request from the host.
 * This is called only if the request cannot be fulfilled using
 * the client's provided descriptors.
 * <p>
 * NOTE: this is called from interrupt context.
 * The callee should not call OS services unless they are interrupt-safe.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 bus "handle"
 * @param 3 unique sequence number for use in responding to this request
 * @param 3 request from host; 8-byte standard request plus any OUT data
 * @param 4 total length, in bytes, of request data
 *
 * @return TRUE on success (should have called MUSB_DeviceResponse)
 * @return FALSE to stall request (if it is known immediately that it is a bad request)
 * @see MUSB_DeviceResponse
 */
typedef uint8_t (*MUSB_pfDeviceRequest)(void*, MUSB_BusHandle, uint32_t,
	const uint8_t*, uint16_t);

/**
 * Device configuration callback.
 *
 * The UCD calls this when a SET_CONFIG() has been received from the host.
 * The callee is responsible for preparing suitable IRPs and starting transfers as applicable
 * (for example, an IRP should be submitted for each OUT endpoint, in anticipation of receiving
 * traffic from the host).
 * This call must complete in (500ms - UCD overhead), so a quick response is warranted.
 * For most devices, the IRPs and associated buffers may be declared statically,
 * so very little delay is expected.
 * <p>
 * NOTE: this is called from interrupt context.
 * The callee should not call OS services unless they are interrupt-safe.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 bus "handle"
 * @param 3 bConfigurationValue config value; 0 => unconfigured, non-zero => configured
 * @param 4 array of pipe handles; one for each endpoint in the configuration,
 * in the same order as they appear in the descriptors
 *
 * @return TRUE if configuration is acceptable and ready for traffic
 * @return FALSE to issue STALL handshake
 */
typedef uint8_t (*MUSB_pfDeviceConfigSelected)(void*, MUSB_BusHandle, uint8_t, MUSB_Pipe*);

/**
 * Interface set callback.
 *
 * The UCD calls this when a SET_INTERFACE() has been received from the host.
 * The callee is responsible for preparing suitable IRPs and starting transfers as applicable
 * (for example, an IRP should be submitted for each OUT endpoint, in anticipation of receiving
 * traffic from the host).
 * This call must complete in (500ms - UCD overhead), so a quick response is warranted.
 * For most devices, the IRPs and associated buffers may be declared statically,
 * so very little delay is expected.
 * <p>
 * NOTE: this is called from interrupt context.
 * The callee should not call OS services unless they are interrupt-safe.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 bus "handle"
 * @param 3 bInterfaceNumber interface number from request
 * @param 4 bAlternateSetting alternate setting from request
 * @param 5 array of pipe handles; one for each endpoint in the configuration,
 * in the same order as they appear in the descriptors
 *
 * @return TRUE if setting is acceptable and ready for traffic
 * @return FALSE to issue STALL handshake
 */
typedef uint8_t (*MUSB_pfInterfaceSet)(void*, MUSB_BusHandle, uint8_t, uint8_t, MUSB_Pipe*);

/**
 * MUSB_FunctionClient.
 * Registration information for a function client.
 *
 * @field pPrivateData client data; not to be interpreted by UCD
 * (but UCD passes it to client callbacks)
 *
 * @field wStringDescriptorCount the number of string descriptors per language,
 * if string descriptors are used.
 *
 * @field pStandardDescriptors a buffer of USB descriptors,
 * applicable for full or low speed, in USB byte order.
 * The suggested order is device, followed by strings if desired, 
 * and then each full configuration (config/interface/endpoint/etc.).
 * The positions of descriptors in the array imply their indexes as seen by the host.
 * String descriptors are organized like a two-dimensional array with
 * language ID as the major index.  So, for example, to support 2 string
 * descriptors for the two languages English (U.S.) and Hungarian,
 * the following order is required:
 * <ol>
 * <li>descriptor 0 (supported languages),
 * <li>descriptor 1 English, descriptor 2 English, 
 * <li>descriptor 1 Hungarian, descriptor 2 Hungarian.
 * </ol>
 * @field wDescriptorBufferLength the length, in bytes, 
 * of the descriptor buffer
 *
 * @field pbSelfPowered pointer to whether the client is self-powered;
 * the client may update this when not in a USB configured state
 *
 * @field pControlBuffer a buffer the UCD will use to store all incoming
 * control requests
 * @field wControlBufferLength size, in bytes, of control buffer
 *
 * @field pfDeviceRequest callback to service host SETUP requests
 *
 * @field pfDeviceConfigSelected callback for configuration
 *
 * @field pfInterfaceSet callback for interface setting
 * 
 * @field pfUsbState callback to notify client of relevant USB state changes
 * 
 * @field pHighSpeedDescriptors a buffer of USB descriptors,
 * applicable to high-speed operation, in USB byte order.
 * The expected content is device qualifier, followed by
 * each full configuration.
 * The UCD will use these descriptors and the ones in the
 * pStandardDescriptors to form an appropriate response to the host's
 * queries, depending on the actual negotiated bus speed.
 * 
 * @field wHighSpeedDescriptorLength the length, in bytes, 
 * of the high-speed descriptor buffer
 */
typedef struct
{
    void* pPrivateData;             /* may be NULL in callback function */
    const uint8_t* pStandardDescriptors;
    uint16_t wDescriptorBufferLength;
    uint16_t wStringDescriptorCount;
    const uint8_t* pHighSpeedDescriptors;
    uint16_t wHighSpeedDescriptorLength;
    uint16_t wControlBufferLength;
    uint8_t* pControlBuffer;
    uint8_t* pbSelfPowered;
    MUSB_pfDeviceRequest pfDeviceRequest;
    MUSB_pfDeviceConfigSelected pfDeviceConfigSelected;
    MUSB_pfInterfaceSet pfInterfaceSet;
    MUSB_pfNewUsbState pfUsbState;
} MUSB_FunctionClient;

/**
 * Device connection notification.
 * Informs a host client of connection of a device (peripheral).
 *
 * @param 1 pPrivateData from the driver's registration struct
 * @param 2 bus "handle"
 * @param 3 pointer to device information
 * @param 4 pointer to the peripheral list entry which caused the
 * driver to be selected
 *
 * @return TRUE on success (device is tentatively accepted; may later call MUSB_RejectDevice)
 * @return FALSE if device is not supported (on OTG, relinquish host role if possible)
 */
typedef uint8_t (*MUSB_pfDeviceConnected)(void*, MUSB_BusHandle, MUSB_Device*,
	const uint8_t*);

/**
 * Device disconnection notification.
 * Informs a host client of disconnection of a device (peripheral).
 *
 * @param 1 pPrivateData from the driver's registration struct
 * @param 2 bus "handle"
 * @param 3 pointer to device information
 *
 * @return TRUE on success.
 * @return FALSE if device is not supported.
 */
typedef uint8_t (*MUSB_pfDeviceDisconnected)(void*, const MUSB_BusHandle, MUSB_Device*);

/**
 * Timer expiration callback.
 * This optional callback is called when an armed timer expires.
 * @param 1 pPrivateData from the driver's registration struct
 * @param 2 bus "handle"
 */
typedef void (*MUSB_pfDriverTimerExpired)(void*, MUSB_BusHandle);

/**
 * Bus suspension notification.
 * This optional callback is called when the bus is suspended.
 * @param 1 pPrivateData from the driver's registration struct
 * @param 2 bus "handle"
 */
typedef void (*MUSB_pfBusSuspended)(void*, MUSB_BusHandle);

/**
 * Bus resumption notification.
 * This optional callback is called when the bus is resumed.
 * @param 1 pPrivateData from the driver's registration struct
 * @param 2 bus "handle"
 */
typedef void (*MUSB_pfBusResumed)(void*, MUSB_BusHandle);

/**
 * MUSB_DeviceDriver (registration for a device driver).
 * Registration information for a device driver.
 *
 * @field pPrivateData client data; not to be interpreted by the UCD
 * (but the UCD passes it to client callbacks)
 *
 * @field bTimerCount how many millisecond-resolution timers this driver needs
 * 
 * @field adwTimerResolutions array of timer resolutions, one per timer
 * 
 * @field pfDeviceConnected callback for device connection notification
 *
 * @field pfDeviceDisconnected callback for device disconnection notification
 * 
 * @field pfBusSuspended if non-NULL, callback for bus suspension
 * 
 * @field pfBusResumed if non-NULL, callback for bus resumption
 */
typedef struct _MUSB_DeviceDriver
{
    void* pPrivateData;     /* may be NULL in callback function */
    MUSB_pfDeviceConnected pfDeviceConnected;
    MUSB_pfDeviceDisconnected pfDeviceDisconnected;
    MUSB_pfBusSuspended pfBusSuspended;
    MUSB_pfBusResumed pfBusResumed;
} MUSB_DeviceDriver;

/**
 * Host error notification.
 * A callback to inform an host client of an host error.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 session "handle"
 * @param 3 error code (MUSB_STATUS_UNSUPPORTED_DEVICE, MUSB_STATUS_UNSUPPORTED_HUB, 
 *                                   MUSB_STATUS_UNSUPPORTED_REMOVE)
 */
typedef void (*MUSB_pfHostError)(void*, MUSB_BusHandle, uint32_t);


/**
 * MUSB_HostClient.
 * Registration information for a host client.
 * 
 * @field pPeripheralList array containing target list
 * (see MUSB_TargetListCode)
 * 
 * @field wPeripheralListLength byte count of peripheral list
 *
 * @field aDeviceDriverList by supplying an array of host clients,
 * the targeted peripheral list can be used as a driver selection mechanism.
 * 
 * @field bDeviceDriverListLength the number of elements in the device driver list
 *
 * @field dwHostError record host status errors.
 *  (MGC_M_STATUS_ERROR, MUSB_STATUS_UNSUPPORTED_DEVICE, 
 * MUSB_STATUS_UNSUPPORTED_HUB)
 *
 * @field pfHostError callback to notify client of host errors
 */
typedef struct
{
    const uint8_t* pPeripheralList;
    uint16_t wPeripheralListLength;
    MUSB_DeviceDriver* aDeviceDriverList;
    uint8_t bDeviceDriverListLength;
    uint32_t dwHostError;
    void* pPrivateData;    /* may be NULL in callback function */    
    MUSB_pfHostError pfHostError;    
} MUSB_HostClient;

/**
 * OTG state notification.
 * A callback to inform an OTG client of a new OTG state.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 session "handle"
 * @param 3 OTG state
 */
typedef void (*MUSB_pfNewOtgState)(void*, MUSB_BusHandle, MUSB_OtgState);

/**
 * OTG error notification.
 * A callback to inform an OTG client of an OTG error.
 *
 * @param 1 pPrivateData from the client's registration struct
 * @param 2 session "handle"
 * @param 3 error code (MUSB_STATUS_OTG_NO_RESPONSE, MUSB_STATUS_UNSUPPORTED_DEVICE, 
 * MUSB_STATUS_UNSUPPORTED_HUB, or MUSB_STATUS_OTG_SRP_FAIL)
 */
typedef void (*MUSB_pfOtgError)(void*, MUSB_BusHandle, uint32_t);

/**
 * MUSB_OtgClient.
 * Registration information for an OTG client.
 *
 * @field pPrivateData client data; not to be interpreted by UCD
 * (but UCD passes it to client callbacks)
 *
 * @field pbDesireHostRole pointer to whether the role of host is desired 
 * at the next possible opportunity.  
 * This applies to the B-device only (it will have no effect on the A-device).
 *
 * @field pfOtgState callback to notify client of OTG state changes
 *
 * @field pfOtgError callback to notify client of OTG errors
 */
typedef struct
{
    void* pPrivateData;    /* may be NULL in callback function */
    uint8_t* pbDesireHostRole;
    MUSB_pfNewOtgState pfOtgState;
    MUSB_pfOtgError pfOtgError;
} MUSB_OtgClient;

/**
 * Callback for enumeration completion on a hub port
 * @param pHubUsbDevice USB device representation of Hub. 
 * The new device is connected to this hub.
 * @param pHubUsbDevice Pointer to parent USB host or hub. 
 * @param pChildUsbDevice Pointer to new USB device enumerated by UCD. 
 * @param bStatus TRUE: Eumeration success, FALSE: error.  
 */
typedef void (*MUSB_pfHubEnumerationComplete)(
    MUSB_Device *pHubUsbDevice, 
    MUSB_Device *pChildUsbDevice, 
    uint8_t bStatus
    );

/**
 * Callback for enumeration completion or device disconnect on a hub port
 * @param pHubUsbDevice USB device representation of Hub. 
 * The new device is connected to this hub.
 * @param pHubUsbDevice Pointer to parent USB host or hub. 
 * @param pChildUsbDevice Pointer to new USB device enumerated by UCD. 
 * @param dwChildUsbDeviceStatus Child USB device status.  
 */
typedef void (*MUSB_pfChildDeviceUpdate)(
    MUSB_Device *pHubUsbDevice, 
    MUSB_Device *pChildUsbDevice, 
    uint32_t    dwChildUsbDeviceStatus
    );

/************************* CDI FUNCTIONS **************************/

/**
 * Count available ports.
 * Discover the number of ports available.
 * @return the number of available ports
 */
extern uint16_t MUSB_CountPorts(void);

/**
 * Get a port.
 * @param wPortIndex the index (counting from 0) of the desired port
 * @return port pointer on success; NULL on failure
 */
extern MUSB_Port* MUSB_GetPort(uint16_t wPortIndex);

/**
 * Register a function client.
 * Typically, a function client is registered once, at "boot" time.
 *
 * @param pPort the port, which must be of Type==MUSB_PORT_TYPE_FUNCTION
 *
 * @param pFunctionClient provides the necessary information for the UCD
 * to implement standard requests, and callbacks for 
 * the UCD to notify the client of required events
 *
 * @return a non-NULL bus "handle" on success
 */
extern MUSB_BusHandle MUSB_RegisterFunctionClient(
    MUSB_Port* pPort,
    MUSB_FunctionClient* pFunctionClient
    );

/**
 * Register a host client, powering up the USB.
 *
 * @param pPort the port, which must be of Type==MUSB_PORT_TYPE_HOST
 *
 * @param pHostClient provides the necessary information for the UCD
 * to determine supported devices, and the callbacks for the UCD
 * to notify each device driver of required events
 *
 * @return a non-NULL bus "handle" on success
 */
extern MUSB_BusHandle MUSB_RegisterHostClient(
    MUSB_Port* pPort,
    MUSB_HostClient* pHostClient
    );

/**
 * Register an OTG client.
 * If the underlying port is the A-device, this will power-up the USB,
 * and the host role is initially assumed.
 * If the underlying port is the B-device, this will connect to the USB,
 * and the function role is initially assumed.
 * Under client control, such a session may be relinquished
 * to another host-capable OTG device, or the bus may be suspended
 * to save power.
 *
 * @param pPort the port, which must be of Type==MUSB_PORT_TYPE_OTG
 *
 * @param pFunctionClient provides the necessary information for the UCD
 * to implement standard requests, and callbacks for 
 * the UCD to notify the client of required events
 *
 * @param pHostClient provides the necessary information for the UCD
 * to determine supported devices, and the callbacks for the UCD
 * to notify each device driver of required events
 *
 * @param pOtgClient provides the necessary information for the UCD
 * to implement required behavior, and callbacks for 
 * the UCD to notify the client of required events
 *
 * @return a non-NULL bus "handle" on success
 */
#ifdef MUSB_OTG_FUNC
extern MUSB_BusHandle MUSB_RegisterOtgClient(
    MUSB_Port* pPort,
    MUSB_FunctionClient* pFunctionClient,
    MUSB_HostClient* pHostClient,
    MUSB_OtgClient* pOtgClient
    );
#endif 
/**
 * Get current (micro)frame.
 * Get the current (micro)frame number on a bus.
 * @param hBus the bus "handle"
 * @return the current (micro)frame number
 */
#if 0
extern uint32_t MUSB_GetBusFrame(MUSB_BusHandle hBus);
#endif
/**
 * Deactivate a client.
 * Attempts to deactivate the currently-registered client.
 * If it is a host client or host-role OTG client,
 * the bus will be suspended.
 * If it is a function client or function-role OTG client,
 * the controller will disconnect from the USB if supported.
 * After a successful call, the given handle is no longer valid
 * for use in other calls.
 * @param hBus the bus "handle"
 * @return TRUE on success
 * @return FALSE if the operation is not possible (soft-disconnect not supported)
 */
#if 0 
extern uint8_t MUSB_DeactivateClient(MUSB_BusHandle hBus);
#endif
/**
 * Request use of the bus.
 * If the current OTG state allows this transition,
 * the bus will be requested.
 * If this is the A-device, the session will be started
 * and default to host.
 * If this is the B-device, SRP will be initiated.
 * @param hBus the bus "handle"
 * @return TRUE on success
 * @return FALSE if the operation is not possible (wrong OTG state)
 */
#ifdef MUSB_OTG_FUNC
extern uint8_t MUSB_RequestBus(MUSB_BusHandle hBus);
#endif
/**
 * Suspend the bus (host and OTG only).
 * @param hBus the bus "handle"
 */
extern void MUSB_SuspendBus(MUSB_BusHandle hBus);

/**
 * Resume the bus.
 * @param hBus the bus "handle"
 */
extern void MUSB_ResumeBus(MUSB_BusHandle hBus);

/**
 * Relinquish host role (OTG only).
 * For OTG, relinquish the host role to another.
 * The mechanism for reporting the status of this operation is the client's
 * pfOtgState callback.
 * @param hBus the bus "handle"
 */
#ifdef MUSB_OTG_FUNC
extern void MUSB_RelinquishHost(MUSB_BusHandle hBus);
#endif
/**
 * (Advanced) Control parsing of control requests from the host.
 * By default, control requests from the host are parsed,
 * so that most standard requests are handled by the stack,
 * leaving a function client creator to code class- or vendor-specific parsing.
 * In rare cases, the automatic parser needs to be disabled for a time
 * (for example, to implement the transport phase of the Device Firmware Upgrade class).
 * @param hBus the bus "handle"
 * @param bParse TRUE to enable the stack's parser; FALSE to disable it.
 * While parsing is disabled, the function client will receive ALL control OUT data.
 */
#if 0 
extern void MUSB_SetFunctionParse(MUSB_BusHandle hBus, uint8_t bParse);
#endif
/**
 * (Advanced) Bind to an endpoint as function.
 * Most clients will not need this function, since appropriate pipes are 
 * automatically opened and passed in the ConfigSelected/InterfaceSet callbacks.
 * However, if a function does not use the USB device framework mechanism
 * to describe its endpoints, it can use this call to open a pipe for a given endpoint.
 * It is still necessary to provide an endpoint descriptor in the USB device framework
 * format, since most of this information is needed to program the underlying controller.
 * @param hBus the bus "handle"
 * @param pEndpointDesc pointer to a suitable endpoint descriptor
 * @return non-NULL pipe handle on success
 * @return NULL on failure (no compatible local endpoint resource or out of memory)
 */
#if 0 
extern MUSB_Pipe MUSB_BindFunctionEnd(MUSB_BusHandle hBus, 
				      const MUSB_EndpointDescriptor* pEndpointDesc);
#endif
/**
 * (Advanced) Close a function pipe.
 * Most clients will not need this function, since the automatically-opened
 * pipes are automatically closed when appropriate (alternate interface setting selected,
 * different configuration selected, or device unconfigured).
 * However, if a function does not use the USB device framework mechanism
 * to describe its endpoints, it should use this call at appropriate times
 * to close pipes it opened via MUSB_BindFunctionEnd.
 * @param hPipe pipe "handle"
 */
#if 0 
extern void MUSB_CloseFunctionPipe(MUSB_Pipe hPipe);
#endif
/**
 * Declare device unsupported.
 * A host-role client calls this after accepting the given device
 * in the device-connection callback, and subsequently determining that
 * it does not actually support the device.
 * @param hBus the bus "handle"
 * @param pDevice the rejected device
 */
extern void MUSB_RejectDevice(MUSB_BusHandle hBus, MUSB_Device* pDevice);

/**
 * Declare interfaces unsupported.
 * A host-role client wishing to support compound devices calls this 
 * when there are interfaces in the configuration that it does not support.
 * The UCD will construct a virtual device/config including only these 
 * interface descriptors (and any that follow them), and run driver selection
 * on the new virtual device, giving other drivers the opportunity to use it.
 * @param hBus the bus "handle"
 * @param pDevice device pointer
 * @param abInterfaceNumber array of unclaimed interface numbers
 * @param bInterfaceCount the count of elements in abInterfaceNumber
 */
#if 0 
extern void MUSB_RejectInterfaces(MUSB_BusHandle hBus, MUSB_Device* pDevice,
				  uint8_t* abInterfaceNumber, uint8_t bInterfaceCount);
#endif
/**
 * Enumerate a device on a hub port.
 * @param pHubDevice the hub's device pointer
 * @param bHubPort the hub port number
 * @param bSpeed device speed
 * @param pfHubEnumerateDevice completion callback
 * @return status code
 */
extern uint32_t MUSB_EnumerateDevice(MUSB_Device* pHubDevice, uint8_t bHubPort,
	uint8_t bSpeed, MUSB_pfHubEnumerationComplete pfHubEnumerateDevice);

/**
 * Set the suspend state for a hub port (and therefore the bus behind it).
 * @param pHubDevice the hub's device pointer
 * @param bHubPort the hub port number
 * @param bSuspend TRUE to suspend the hub port;
 * FALSE to resume a previously-suspended hub port
 * @return status code
 */
#if 0 
extern uint32_t MUSB_SetTreeSuspend(MUSB_Device* pHubDevice, uint8_t bHubPort,
				    uint8_t bSuspend);
#endif
/**
 * A device has been disconnected.
 * @param pDevice device pointer
 */
extern void MUSB_DeviceDisconnected(MUSB_Device* pDevice);

/**
 * Open a pipe as host.
 * Host clients call this to establish a virtual path to communicate
 * with a remote endpoint.
 * Resources for Interrupt and Isochronous remote endpoints
 * are bound to the pipe; other types may be shared.
 *
 * @param hBus the bus "handle"
 * @param pRemoteEnd pointer to a description of the desired remote endpoint
 * @param pEndpointResource if non-NULL, points to desired resource properties
 * upon call, and updated upon return
 *
 * @return non-NULL pipe "handle" on success
 */
extern MUSB_Pipe MUSB_OpenPipe(MUSB_BusHandle hBus,
			       const MUSB_DeviceEndpoint* pRemoteEnd,
			       MUSB_EndpointResource* pEndpointResource);

/**
 * Respond to host request.
 * Send a response to a host request on the default endpoint.
 * @param hBus the bus "handle"
 * @param dwSequenceNumber sequence number from request callback
 * @param pResonseData buffer of response data, relevant if request was IN
 * @param wResponseDataLength length of buffer
 * @param bStall TRUE to send the STALL handshake
 * @return status code
 */
extern uint32_t MUSB_DeviceResponse(MUSB_BusHandle hBus, uint32_t dwSequenceNumber,
	const uint8_t* pResponseData, uint16_t wResponseDataLength, uint8_t bStall);

/**
 * Close a pipe.
 * @param hPipe pipe "handle"
 * @return 0 on success; error code otherwise
 */
extern void MUSB_ClosePipe(MUSB_Pipe hPipe);

/**
 * Get whether a pipe is halted.
 * @param hPipe pipe "handle"
 * @return TRUE if the pipe is halted; FALSE otherwise
 */
#if 0 
extern uint8_t MUSB_GetPipeHalt(MUSB_Pipe hPipe);
#endif
/**
 * Set whether a pipe is halted.
 * @param hPipe pipe "handle"
 * @param TRUE to halt pipe; FALSE to resume it
 * @return 0 on success; error code otherwise
 */
extern uint32_t MUSB_SetPipeHalt(MUSB_Pipe hPipe, uint8_t bHalt);

/**
 * Flush the given pipe.
 * @param hPipe pipe "handle"
 * @return 0 on success; error code otherwise
 */
extern uint32_t MUSB_FlushPipe(MUSB_Pipe hPipe);

#ifdef MUSB_DMA_BUF 
/**
 * Allocate a DMA buffer for use with the given pipe.
 * @param hPipe pipe "handle"
 * @param dwLength requested buffer size, in bytes
 * @return non-NULL buffer on success; NULL on failure
 */
extern uint8_t* MUSB_AllocDmaBuffer(MUSB_Pipe hPipe, uint32_t dwLength);

/**
 * Free a previously-allocated DMA buffer.
 * @param hPipe pipe "handle"
 * @param pBuffer buffer pointer
 */
extern void MUSB_FreeDmaBuffer(MUSB_Pipe hPipe, uint8_t* pBuffer);
#endif

/**
 * Start a single control transfer.
 *
 * @param pPort port pointer
 * @param pIrp pointer to the control IRP
 *
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_StartControlTransfer(MUSB_Port* pPort, MUSB_ControlIrp* pIrp);

/**
 * Cancel a control transfer
 *
 * @param pPort port pointer
 * @param pIrp pointer to the control IRP
 *
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_CancelControlTransfer(MUSB_Port* pPort, MUSB_ControlIrp* pIrp);

/**
 * Initiate bulk or interrupt traffic.
 * Start a single bulk transfer, or schedule an interrupt transfer.
 * Interrupt transfers remain scheduled until cancelled.
 *
 * @param pIrp pointer to the IRP
 *
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_StartTransfer(MUSB_Irp* pIrp);

/**
 * Update interrupt data.
 * Signal that new interrupt data is ready for transmission
 *
 * @param pIrp pointer to an interrupt-transmitting IRP
 *
 * @return 0 on success; error code on failure
 */
#if 0 
extern uint32_t MUSB_InterruptReady(MUSB_Irp* pIrp);
#endif
/**
 * Cancel a bulk/interrupt transfer.
 * @param pIrp pointer to the IRP
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_CancelTransfer(MUSB_Irp* pIrp);
/**
 * Schedule an isochronous transfer.
 * Upon success, it will remain scheduled until cancelled.
 * The IRP is essentially a ring buffer,
 * so the application is expected to partially fill/empty
 * it when its completion handler is called.
 *
 * @param dwWaitCount how many (micro)frames to wait before starting
 * @param pIsochIrp pointer to the IRP
 *
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_ScheduleIsochTransfer(
    uint32_t dwWaitCount, MUSB_IsochIrp* pIsochIrp);

/**
 * Cancel an isochronous transfer.
 *
 * @param pIsochIrp pointer to the IRP
 *
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_CancelIsochTransfer(MUSB_IsochIrp* pIsochIrp);

/**
 * Set diagnostics level.
 * Set the diagnostics level if compiled with diagnostics capability
 * #ifdef MUSB_DIAG.
 * If compiled with MUSB_DIAG undefined or 0, this call has no effect.
 * @param bLevel 0 to disable diagnostics, 1 for some messages,
 * 2 for more messages, 3 for the most
 */
extern void MUSB_SetDiagnosticLevel(uint8_t bLevel);

/**
 * Arm a one-shot timer.
 * This is specifically intended for use by class drivers.
 *
 * @param hBus bus "handle"
 * @param bTimerIndex: Timer Index 1 is reserved for class drive
 * @param dwTime timeout (milliseconds)
 * @param pParam parameter to expiration callback
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_ArmTimer(MUSB_BusHandle hBus,  
	uint8_t bTimerIndex, uint32_t dwTime, 
	MUSB_pfDriverTimerExpired pfDriverTimerExpired, void* pParam);

/**
 * Disarm a one-shot timer.
 * This is specifically intended for use by class drivers.
 *
 * @param hBus bus "handle"
 * @param bTimerIndex the 0-based index from the driver's bTimerCount
 * @return 0 on success; error code on failure
 */
extern uint32_t MUSB_CancelTimer(MUSB_BusHandle hBus, 
        uint8_t bTimerIndex);


/*
 * Use bus reset to recover device do not ack properly.
 */
extern uint8_t MUSB_SilentReset(MUSB_Device* pDevice);

/**
 * MUSB_SetEhset.
 * This is specifically intended for use by embedded host compilance test.
 *
 * @param dwEnable enable embedded test mode. ((wPid >= 0x0101) && (wPid <= 0x0108))
 * @return 0 on success; error code on failure
 */
#ifdef MUSB_EHSET 
extern uint32_t MUSB_SetEhset(uint32_t dwEnable, uint16_t wPid);
#endif

#ifdef MUSB_POWERDOWN
extern uint32_t MUSB_PowerDownSet(uint32_t u4Case);
#endif

/** Enable high speed 
     Set this variable before MUSB_NewController().
*/
extern uint8_t MGC_bWantHighSpeed;


/** disable usb suspend mode 
     Set this variable before MGC_HdrcProgramBusState().
*/
extern uint8_t MGC_bWantSuspend;

/** Set USB HS_TERMC [4:0] in PHY CONTROL REGISTER 1
     It is used in MUSB_BoardSoftReset().
     If MGC_bHSTERMC = 0xFF, MUSB_BoardSoftReset() will 
     use h/w default value to set HSTERMC.
*/
extern uint8_t MGC_bHSTERMC;

#endif	/* multiple inclusion protection */
