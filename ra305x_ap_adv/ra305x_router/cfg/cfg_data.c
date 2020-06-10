/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    cfg_data.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
*/

//==============================================================================
//                                INCLUDE FILES
//==============================================================================
#include <cfg_def.h>


char cfg_data[CFG_OBJ_SIZE];


struct cfgid_str_t cfgid_str_table[]=
{
    { CFG_ID_HEADER , "CFG_HDR"  },
    { CFG_ID_CHKSUM , "CFG_CS" },

#define CFG_STR_ENTRY_( c)     { CFG_##c, #c }

    CFG_STR_ENTRY_( DDNS_EN ),
    CFG_STR_ENTRY_( DDNS_SET ),
    CFG_STR_ENTRY_( DDNS_IP ),

    CFG_STR_ENTRY_( FW_FLT_CLN ),    
    CFG_STR_ENTRY_( FW_FLT_CLN_EN ),
    CFG_STR_ENTRY_( FW_FLT_MAC ),
    CFG_STR_ENTRY_( FW_FLT_MAC_EN ),
    CFG_STR_ENTRY_( FW_FLT_MAC_MODE ),
    CFG_STR_ENTRY_( FW_FLT_URL ),
    CFG_STR_ENTRY_( FW_FLT_URL_EN ),
    CFG_STR_ENTRY_( FW_FRAG_OK ),
    CFG_STR_ENTRY_( FW_HACKER_ATT ),
    CFG_STR_ENTRY_( FW_ICMP_RESP ),
    CFG_STR_ENTRY_( FW_IDENT_DIS ),
    CFG_STR_ENTRY_( FW_NBIOS_DROP ),
    CFG_STR_ENTRY_( FW_PING_DIS_LAN ),
    CFG_STR_ENTRY_( FW_PING_DIS_WAN ),
    CFG_STR_ENTRY_( FW_PSCAN_DROP ),
    //CFG_STR_ENTRY_( FW_RM_EN ),
    //CFG_STR_ENTRY_( FW_RM_IP ),
    //CFG_STR_ENTRY_( FW_RM_PORT ),
    CFG_STR_ENTRY_( FW_IP_SPOOF ),
    CFG_STR_ENTRY_( FW_SHORT ),
    CFG_STR_ENTRY_( FW_PING_DEATH ),
    CFG_STR_ENTRY_( FW_LAND ),
    CFG_STR_ENTRY_( FW_SNORK ),
    CFG_STR_ENTRY_( FW_UDP_PLOOP ),
    CFG_STR_ENTRY_( FW_TCP_NULL ),
    CFG_STR_ENTRY_( FW_SMURF ),
    CFG_STR_ENTRY_( FW_SYN_FLOOD ),
    CFG_STR_ENTRY_( FW_SYNF_RATE ),
    CFG_STR_ENTRY_( FW_EN ),
    CFG_STR_ENTRY_( FW_MSN_DIS ),
    CFG_STR_ENTRY_( FW_QQ_DIS ),
    CFG_STR_ENTRY_( FW_FLT_PKT ),
    CFG_STR_ENTRY_( FW_FLT_DN_MODE ),
    CFG_STR_ENTRY_( FW_FLT_DNB ),
    CFG_STR_ENTRY_( FW_FLT_DNA ),


    CFG_STR_ENTRY_( LAN_DHCPD_DOMAIN ),
    CFG_STR_ENTRY_( LAN_DHCPD_EN ),
    CFG_STR_ENTRY_( LAN_DHCPD_END ),
    CFG_STR_ENTRY_( LAN_DHCPD_LEASET ),
    CFG_STR_ENTRY_( LAN_DHCPD_START ),
    CFG_STR_ENTRY_( LAN_GW ),
    CFG_STR_ENTRY_( LAN_IP ),
    CFG_STR_ENTRY_( LAN_MSK ),
    CFG_STR_ENTRY_( LAN_IP_MODE ),
    CFG_STR_ENTRY_( LAN_DHCPD_SLEASE ),
    CFG_STR_ENTRY_( LAN_DHCPD_DLEASE ),
    CFG_STR_ENTRY_( LAN_DHCPD_PASS_WDN ),
    CFG_STR_ENTRY_( LAN_PHY ),
    CFG_STR_ENTRY_( LAN_GW_v6 ),
    CFG_STR_ENTRY_( LAN_IP_v6 ),
    CFG_STR_ENTRY_( LAN_MSK_v6 ),
    CFG_STR_ENTRY_( LAN_DHCP_v6_EN ),
    CFG_STR_ENTRY_( DHCPV6_START ),
    CFG_STR_ENTRY_( DHCPV6_END ),
    CFG_STR_ENTRY_( DHCPV6_VLIFE ),
    CFG_STR_ENTRY_( DHCPV6_PLIFE ),
    CFG_STR_ENTRY_( DHCPV6_SDUID ),

    CFG_STR_ENTRY_( NAT_DMZ_EN ),
    CFG_STR_ENTRY_( NAT_DMZ_HOST ),
    CFG_STR_ENTRY_( NAT_PMAP ),
    CFG_STR_ENTRY_( NAT_PTRG ),
    CFG_STR_ENTRY_( NAT_VTS ),
    CFG_STR_ENTRY_( NAT_ALG_FTP ),
    CFG_STR_ENTRY_( NAT_ALG_NM ),
    CFG_STR_ENTRY_( NAT_ALG_PPTP ),
    CFG_STR_ENTRY_( NAT_ALG_MSN ),
    CFG_STR_ENTRY_( NAT_ALG_IPSEC ),
    CFG_STR_ENTRY_( NAT_EN ),
    CFG_STR_ENTRY_( NAT_ALG_FTP_NSP ),
    CFG_STR_ENTRY_( NAT_ALG_BATTLEN ),
    CFG_STR_ENTRY_( NAT_SDMZ_MAC ),
    CFG_STR_ENTRY_( NAT_ALG_L2TP ),
    CFG_STR_ENTRY_( NAT_SCHED ),
    CFG_STR_ENTRY_( NAT_ALG_RTSP ),
    CFG_STR_ENTRY_( NAT_ALG_SIP ),
    CFG_STR_ENTRY_( NAT_ALG_RTSP_NSP ),

    CFG_STR_ENTRY_( RT_ENTRY ),

    CFG_STR_ENTRY_( SYS_ADMPASS ),
    CFG_STR_ENTRY_( SYS_DAYLITE ),
    CFG_STR_ENTRY_( SYS_DOMAIN ),
    CFG_STR_ENTRY_( SYS_IDLET ),
    CFG_STR_ENTRY_( SYS_NAME ),
    CFG_STR_ENTRY_( SYS_PASS ),
    CFG_STR_ENTRY_( SYS_RM_EN ),
    CFG_STR_ENTRY_( SYS_RM_IP ),
    CFG_STR_ENTRY_( SYS_RM_PORT ),
    CFG_STR_ENTRY_( SYS_TZONE ),
    CFG_STR_ENTRY_( SYS_TZONE1 ),
    CFG_STR_ENTRY_( SYS_NTPTYPE ),
    CFG_STR_ENTRY_( SYS_NTPSRV ),
    CFG_STR_ENTRY_( SYS_DAYLITE_SM ),
    CFG_STR_ENTRY_( SYS_DAYLITE_SD ),
    CFG_STR_ENTRY_( SYS_DAYLITE_EM ),
    CFG_STR_ENTRY_( SYS_DAYLITE_ED ),
    CFG_STR_ENTRY_( SYS_USERS ),
    CFG_STR_ENTRY_( SYS_WAN ),
    CFG_STR_ENTRY_( SYS_LAN ),
    CFG_STR_ENTRY_( SYS_OPMODE),
    CFG_STR_ENTRY_( SYS_LM ),

    CFG_STR_ENTRY_( SYS_TIME ),
	CFG_STR_ENTRY_( SYS_Language ),

    CFG_STR_ENTRY_( UPP_AD_TIME ),
    CFG_STR_ENTRY_( UPP_EN ),
    CFG_STR_ENTRY_( UPP_PORT ),
    CFG_STR_ENTRY_( UPP_SUB_TIMEOUT ),

    CFG_STR_ENTRY_( WAN_DHCP_BPA_EN ),
    CFG_STR_ENTRY_( WAN_DHCP_MAC ),
    CFG_STR_ENTRY_( WAN_DHCP_NAME ),
    CFG_STR_ENTRY_( WAN_DNS ),
    CFG_STR_ENTRY_( WAN_GW ),
    CFG_STR_ENTRY_( WAN_IP ),
    CFG_STR_ENTRY_( WAN_MSK ),
    CFG_STR_ENTRY_( WAN_IP_MODE ),
    CFG_STR_ENTRY_( WAN_IP_MANY ),
    CFG_STR_ENTRY_( WAN_IP_MANY_IP ),
    CFG_STR_ENTRY_( WAN_IP_MANY_MSK ),
    CFG_STR_ENTRY_( WAN_DHCP_MAC_EN ),
    CFG_STR_ENTRY_( WAN_DHCP_BPA_USER ),
    CFG_STR_ENTRY_( WAN_DHCP_BPA_PASS ),
    CFG_STR_ENTRY_( WAN_DHCP_BPA_SERV ),
    CFG_STR_ENTRY_( WAN_DHCP_REQIP ),
    CFG_STR_ENTRY_( WAN_MTU ),
    CFG_STR_ENTRY_( WAN_DHCP_MTU ),
    CFG_STR_ENTRY_( WAN_DNS_EN ),
    CFG_STR_ENTRY_( WAN_DNS_DEF ),
    CFG_STR_ENTRY_( WAN_DHCP_MAN ),
    CFG_STR_ENTRY_( WAN_PHY ),
    CFG_STR_ENTRY_( WAN_DNS_v6 ),
    CFG_STR_ENTRY_( WAN_GW_v6 ),
    CFG_STR_ENTRY_( WAN_IP_v6 ),
    CFG_STR_ENTRY_( WAN_MSK_v6 ),
    CFG_STR_ENTRY_( WAN_DHCP_v6_EN ),
    CFG_STR_ENTRY_( WAN_DHCPV6_CDUID ),

    CFG_STR_ENTRY_( POE_AUTO ),
    CFG_STR_ENTRY_( POE_IDLET ),
    CFG_STR_ENTRY_( POE_MTU ),
    CFG_STR_ENTRY_( POE_PASS ),
    CFG_STR_ENTRY_( POE_SVC ),
    CFG_STR_ENTRY_( POE_USER ),
    CFG_STR_ENTRY_( POE_AC ),
    CFG_STR_ENTRY_( POE_ST ),
    CFG_STR_ENTRY_( POE_ET ),
    CFG_STR_ENTRY_( POE_SEID ),
    CFG_STR_ENTRY_( POE_SIPE ),
    CFG_STR_ENTRY_( POE_SIP ),
    CFG_STR_ENTRY_( POE_EN ),
    CFG_STR_ENTRY_( POE_DFLRT ),
    CFG_STR_ENTRY_( POE_PASSWDALG ),
    CFG_STR_ENTRY_( POE_PA_MODE ),
    CFG_STR_ENTRY_( POE_WANIF ),
#ifndef CONFIG_DUALWAN
    CFG_STR_ENTRY_( POE_IP ),
    CFG_STR_ENTRY_( POE_MSK ),
#endif
    CFG_STR_ENTRY_( POE_GW ),
    CFG_STR_ENTRY_( POE_DNS ),

    CFG_STR_ENTRY_( PTP_AUTO ),
    CFG_STR_ENTRY_( PTP_HOST ),
    CFG_STR_ENTRY_( PTP_ID ),
    CFG_STR_ENTRY_( PTP_IDLET ),
    CFG_STR_ENTRY_( PTP_IP ),
    CFG_STR_ENTRY_( PTP_MSK ),
    CFG_STR_ENTRY_( PTP_MTU ),
    CFG_STR_ENTRY_( PTP_PASS ),
    CFG_STR_ENTRY_( PTP_SVR ),
    CFG_STR_ENTRY_( PTP_USER ),
    CFG_STR_ENTRY_( PTP_SVRDM ),
    CFG_STR_ENTRY_( PTP_SVRD_SEL ),
    CFG_STR_ENTRY_( PTP_WANIF ),
    CFG_STR_ENTRY_( PTP_CLONE_MAC ),
    CFG_STR_ENTRY_( PTP_DHCP_MAC_EN ),
    CFG_STR_ENTRY_( PTP_MPPE_EN ),
    CFG_STR_ENTRY_( PTP_GW),
    CFG_STR_ENTRY_( PTP_DNS),

    CFG_STR_ENTRY_( L2T_AUTO ),
    CFG_STR_ENTRY_( L2T_IDLET ),
    CFG_STR_ENTRY_( L2T_IP ),
    CFG_STR_ENTRY_( L2T_MSK ),
    CFG_STR_ENTRY_( L2T_MTU ),
    CFG_STR_ENTRY_( L2T_PASS ),
    CFG_STR_ENTRY_( L2T_SVR ),
    CFG_STR_ENTRY_( L2T_USER ),
    CFG_STR_ENTRY_( L2T_SVRDM ),
    CFG_STR_ENTRY_( L2T_SVRD_SEL ),
    CFG_STR_ENTRY_( L2T_WANIF ),
    CFG_STR_ENTRY_( L2T_CLONE_MAC ),
    CFG_STR_ENTRY_( L2T_DHCP_MAC_EN ),
    CFG_STR_ENTRY_( L2T_GW),
    CFG_STR_ENTRY_( L2T_DNS),
	
    CFG_STR_ENTRY_( DNS_EN ),
    CFG_STR_ENTRY_( DNS_SVR ),
    CFG_STR_ENTRY_( DNS_FIX ),
    CFG_STR_ENTRY_( DNS_DEF ),
    CFG_STR_ENTRY_( DNS_FIX_FIRST ),
    CFG_STR_ENTRY_( DNS_HOSTS ),

    CFG_STR_ENTRY_( QOS_EN ),
    CFG_STR_ENTRY_( QOS_IPTABLE ),
	
    CFG_STR_ENTRY_( WLN_AP_MAC ),
    CFG_STR_ENTRY_( WLN_AP_MODE ),
    CFG_STR_ENTRY_( WLN_AP_PEER ),
    CFG_STR_ENTRY_( WLN_AUTH ),
    CFG_STR_ENTRY_( WLN_BasicRate ),
    CFG_STR_ENTRY_( WLN_BeaconPeriod ),
    CFG_STR_ENTRY_( WLN_Channel ),
    CFG_STR_ENTRY_( WLN_CLN_MAC ),
    CFG_STR_ENTRY_( WLN_DtimPeriod ),
    CFG_STR_ENTRY_( WLN_FLT_ALLOW ),
    CFG_STR_ENTRY_( WLN_FLT_EN ),
    CFG_STR_ENTRY_( WLN_FLT_MAC ),
    CFG_STR_ENTRY_( WLN_PKTSZ ),
    CFG_STR_ENTRY_( WLN_PREAMP ),
    CFG_STR_ENTRY_( WLN_RTSThreshold ),
    CFG_STR_ENTRY_( WLN_SSID ),
    CFG_STR_ENTRY_( WLN_SSIDBCST ),
    CFG_STR_ENTRY_( WLN_TxPower ),
    CFG_STR_ENTRY_( WLN_WEP_EN ),
    CFG_STR_ENTRY_( WLN_WEP_KEY ),
    CFG_STR_ENTRY_( WLN_WepKeyLen ),
    CFG_STR_ENTRY_( WLN_WEP_KEYMODE ),
    CFG_STR_ENTRY_( WLN_WEP_KEYSET ),
    CFG_STR_ENTRY_( WLN_WEP_PASSPHASE ),
    CFG_STR_ENTRY_( WLN_EN ),
    CFG_STR_ENTRY_( WLN_DOMAIN ),
    CFG_STR_ENTRY_( WLN_TxRate ),
    CFG_STR_ENTRY_( WLN_RADIUS_Server ),
    CFG_STR_ENTRY_( WLN_RADIUS_Port ),
    CFG_STR_ENTRY_( WLN_RADIUS_SECRETE ),
    CFG_STR_ENTRY_( WLN_WPA_REKEYTIME ),
    CFG_STR_ENTRY_( WLN_WPA_ENCRYTYPE ),
    CFG_STR_ENTRY_( WLN_WPA_PASSPHASE ),
    CFG_STR_ENTRY_( WLN_RATEMODE ),

    CFG_STR_ENTRY_( WLN_MODE ),
    CFG_STR_ENTRY_( WLN_SECURITY_MODE ),
    CFG_STR_ENTRY_( WLN_WPA_REKEYTYPE ),
    CFG_STR_ENTRY_( WLN_BGProtection ),
    CFG_STR_ENTRY_( WLN_DISABLE_OLBC ),
    CFG_STR_ENTRY_( WLN_TxBurst ),
    CFG_STR_ENTRY_( WLN_PktAggregate ),
    CFG_STR_ENTRY_( WLN_TURBO ),
    CFG_STR_ENTRY_( WLN_ShortSlot ),
    CFG_STR_ENTRY_( WLN_IEEE80211H ),
    CFG_STR_ENTRY_( WLN_PREAUTH ),
    CFG_STR_ENTRY_( WLN_NOFORWARD ),
    CFG_STR_ENTRY_( WLN_ANTENNA ),
    CFG_STR_ENTRY_( WLN_SENS ),
	CFG_STR_ENTRY_( WLN_APCLI_CONFIGED ),

/* WLAN, added by Rorscha */
    CFG_STR_ENTRY_( WLN_WirelessMode ),
    CFG_STR_ENTRY_( WLN_CountryCode ),
    CFG_STR_ENTRY_( WLN_AutoChannelSelect ),
    CFG_STR_ENTRY_( WLN_BssidNum ),
    CFG_STR_ENTRY_( WLN_HideSSID ),
    CFG_STR_ENTRY_( WLN_WdsEnable ),
    CFG_STR_ENTRY_( WLN_WdsEncrypType ),
    CFG_STR_ENTRY_( WLN_DefaultKeyID ),
    CFG_STR_ENTRY_( WLN_WdsKey ),
    CFG_STR_ENTRY_( WLN_Wds1Key ),
    CFG_STR_ENTRY_( WLN_Wds2Key ),
    CFG_STR_ENTRY_( WLN_Wds3Key ),
    CFG_STR_ENTRY_( WLN_Wds4Key ),
    CFG_STR_ENTRY_( WLN_HT_OpMode ),
    CFG_STR_ENTRY_( WLN_HT_BW ),
    CFG_STR_ENTRY_( WLN_HT_GI ),
    CFG_STR_ENTRY_( WLN_HT_MCS ),
    CFG_STR_ENTRY_( WLN_HT_HTC ),
    CFG_STR_ENTRY_( WLN_HT_RDG ),
    CFG_STR_ENTRY_( WLN_HT_LinkAdapt ),
    CFG_STR_ENTRY_( WLN_HT_EXTCHA ),
    CFG_STR_ENTRY_( WLN_HT_AMSDU ),
    CFG_STR_ENTRY_( WLN_EncrypType ),
    CFG_STR_ENTRY_( WLN_WdsPhyMode ),
    CFG_STR_ENTRY_( WLN_ModuleName ),
    CFG_STR_ENTRY_( WLN_HT_STBC ),
    CFG_STR_ENTRY_( WLN_HT_DisallowTKIP ),
    CFG_STR_ENTRY_( WLN_HT_40MHZ_INTOLERANT ),
    CFG_STR_ENTRY_( WLN_HT_RxStream ),
    CFG_STR_ENTRY_( WLN_HT_TxStream ),
    CFG_STR_ENTRY_( WLN_WiFiTest ),
    CFG_STR_ENTRY_( WLN_FragThreshold ),
    CFG_STR_ENTRY_( WLN_TxPreamble ),
    CFG_STR_ENTRY_( WLN_WmmCapable ),
    CFG_STR_ENTRY_( WLN_APSDCapable ),
    CFG_STR_ENTRY_( WLN_DLSCapable ),
    CFG_STR_ENTRY_( WLN_APAifsn ),
    CFG_STR_ENTRY_( WLN_APCwmin ),
    CFG_STR_ENTRY_( WLN_APCwmax ),
    CFG_STR_ENTRY_( WLN_APTxop ),
    CFG_STR_ENTRY_( WLN_APACM ),
    CFG_STR_ENTRY_( WLN_BSSAifsn ),
    CFG_STR_ENTRY_( WLN_BSSCwmin ),
    CFG_STR_ENTRY_( WLN_BSSCwmax ),
    CFG_STR_ENTRY_( WLN_BSSTxop ),
    CFG_STR_ENTRY_( WLN_BSSACM ),
    CFG_STR_ENTRY_( WLN_AckPolicy ),
    CFG_STR_ENTRY_( WLN_HT_AutoBA ),
    CFG_STR_ENTRY_( WLN_HT_MpduDensity ),
    CFG_STR_ENTRY_( WLN_HT_BAWinSize ),
    CFG_STR_ENTRY_( WLN_HT_BADecline ),
    CFG_STR_ENTRY_( WLN_NintendoCapable ),
    CFG_STR_ENTRY_( WLN_RADIUS_Key ),
    CFG_STR_ENTRY_( WLN_session_timeout_interval ),
    CFG_STR_ENTRY_( WLN_idle_timeout_interval ),
    CFG_STR_ENTRY_( WLN_AccessPolicy ),
    CFG_STR_ENTRY_( WLN_AccessPolicy0 ),
    CFG_STR_ENTRY_( WLN_AccessPolicy1 ),
    CFG_STR_ENTRY_( WLN_AccessPolicy2 ),
    CFG_STR_ENTRY_( WLN_AccessPolicy3 ),
    CFG_STR_ENTRY_( WLN_wanConnectionMode ),
    CFG_STR_ENTRY_( WLN_WapiEnable ),
    CFG_STR_ENTRY_( WLN_HT_DisallowTKIP ),
    CFG_STR_ENTRY_( WLN_WscModeOption ),
    CFG_STR_ENTRY_( WLN_WscConfigured ),
    CFG_STR_ENTRY_( WLN_WscPinCode ),
    CFG_STR_ENTRY_( WLN_WdsList ),
    CFG_STR_ENTRY_( WLN_WdsDefaultKeyID ),
    CFG_STR_ENTRY_( WLN_KeyType ),

    CFG_STR_ENTRY_( WLN_SSIDNum ),
    CFG_STR_ENTRY_( WLN_CountryRegion ),
    CFG_STR_ENTRY_( WLN_CountryRegionABand ),
    CFG_STR_ENTRY_( WLN_DisableOLBC ),
    CFG_STR_ENTRY_( WLN_TxAntenna ),
    CFG_STR_ENTRY_( WLN_RxAntenna ),
    CFG_STR_ENTRY_( WLN_TurboRate ),
    CFG_STR_ENTRY_( WLN_NoForwarding ),
    CFG_STR_ENTRY_( WLN_NoForwardingBTNBSSID ),
    CFG_STR_ENTRY_( WLN_SecurityMode ),
    CFG_STR_ENTRY_( WLN_VLANEnable ),
    CFG_STR_ENTRY_( WLN_VLANName ),
    CFG_STR_ENTRY_( WLN_VLANID ),
    CFG_STR_ENTRY_( WLN_VLANPriority ),
    CFG_STR_ENTRY_( WLN_WscConfMode ),
    CFG_STR_ENTRY_( WLN_WscConfStatus ),
    CFG_STR_ENTRY_( WLN_WscAKMP ),
    CFG_STR_ENTRY_( WLN_WscActionIndex ),
    CFG_STR_ENTRY_( WLN_WscRegResult ),
    CFG_STR_ENTRY_( WLN_WscUseUPnP ),
    CFG_STR_ENTRY_( WLN_WscUseUFD ),
    CFG_STR_ENTRY_( WLN_WscSSID ),
    CFG_STR_ENTRY_( WLN_WscKeyMGMT ),
    CFG_STR_ENTRY_( WLN_WscConfigMethod ),
    CFG_STR_ENTRY_( WLN_WscAuthType ),
    CFG_STR_ENTRY_( WLN_WscEncrypType ),
    CFG_STR_ENTRY_( WLN_WscNewKey ),
    CFG_STR_ENTRY_( WLN_IEEE8021X ),
    CFG_STR_ENTRY_( WLN_CSPeriod ),
    CFG_STR_ENTRY_( WLN_PreAuth ),
    CFG_STR_ENTRY_( WLN_AuthMode ),
    CFG_STR_ENTRY_( WLN_RekeyInterval ),
    CFG_STR_ENTRY_( WLN_RekeyMethod ),
    CFG_STR_ENTRY_( WLN_PMKCachePeriod ),
    CFG_STR_ENTRY_( WLN_WPAPSK1 ),
    CFG_STR_ENTRY_( WLN_WPAPSK ),
    CFG_STR_ENTRY_( WLN_WPAPSK2 ),
    CFG_STR_ENTRY_( WLN_WPAPSK3 ),
    CFG_STR_ENTRY_( WLN_WPAPSK4 ),
    CFG_STR_ENTRY_( WLN_Key1Type ),
    CFG_STR_ENTRY_( WLN_Key1Str ),
    CFG_STR_ENTRY_( WLN_Key1Str1 ),
    CFG_STR_ENTRY_( WLN_Key1Str2 ),
    CFG_STR_ENTRY_( WLN_Key1Str3 ),
    CFG_STR_ENTRY_( WLN_Key1Str4 ),
    CFG_STR_ENTRY_( WLN_Key2Type ),
    CFG_STR_ENTRY_( WLN_Key2Str ),
    CFG_STR_ENTRY_( WLN_Key2Str1 ),
    CFG_STR_ENTRY_( WLN_Key2Str2 ),
    CFG_STR_ENTRY_( WLN_Key2Str3 ),
    CFG_STR_ENTRY_( WLN_Key2Str4 ),
    CFG_STR_ENTRY_( WLN_Key3Type ),
    CFG_STR_ENTRY_( WLN_Key3Str ),
    CFG_STR_ENTRY_( WLN_Key3Str1 ),
    CFG_STR_ENTRY_( WLN_Key3Str2 ),
    CFG_STR_ENTRY_( WLN_Key3Str3 ),
    CFG_STR_ENTRY_( WLN_Key3Str4 ),
    CFG_STR_ENTRY_( WLN_Key4Type ),
    CFG_STR_ENTRY_( WLN_Key4Str ),
    CFG_STR_ENTRY_( WLN_Key4Str1 ),
    CFG_STR_ENTRY_( WLN_Key4Str2 ),
    CFG_STR_ENTRY_( WLN_Key4Str3 ),
    CFG_STR_ENTRY_( WLN_Key4Str4 ),
    CFG_STR_ENTRY_( WLN_WapiPskType ),
    CFG_STR_ENTRY_( WLN_WapiPsk1 ),
    CFG_STR_ENTRY_( WLN_WapiPsk2 ),
    CFG_STR_ENTRY_( WLN_WapiPsk3 ),
    CFG_STR_ENTRY_( WLN_WapiPsk4 ),
    CFG_STR_ENTRY_( WLN_HSCounter ),
    CFG_STR_ENTRY_( WLN_HT_PROTECT ),
    CFG_STR_ENTRY_( WLN_HT_MIMOPS ),
    CFG_STR_ENTRY_( WLN_AccessControlList0 ),
    CFG_STR_ENTRY_( WLN_AccessControlList1 ),
    CFG_STR_ENTRY_( WLN_AccessControlList2 ),
    CFG_STR_ENTRY_( WLN_AccessControlList3 ),
    CFG_STR_ENTRY_( WLN_WirelessEvent ),
    CFG_STR_ENTRY_( WLN_RADIUS_Acct_Server ),
    CFG_STR_ENTRY_( WLN_RADIUS_Acct_Port ),
    CFG_STR_ENTRY_( WLN_RADIUS_Acct_Key ),
    CFG_STR_ENTRY_( WLN_staWirelessMode ),
    CFG_STR_ENTRY_( WLN_upnpEnabled ),
    CFG_STR_ENTRY_( WLN_pppoeREnabled ),
    CFG_STR_ENTRY_( WLN_RDRegion ),
    CFG_STR_ENTRY_( WLN_NewVersion ),
    CFG_STR_ENTRY_( WLN_SSID1 ),
    CFG_STR_ENTRY_( WLN_SSID2 ),
    CFG_STR_ENTRY_( WLN_SSID3 ),
    CFG_STR_ENTRY_( WLN_SSID4 ),
    CFG_STR_ENTRY_( WLN_RADIUS_Key1 ),
    CFG_STR_ENTRY_( WLN_RADIUS_Key2 ),
    CFG_STR_ENTRY_( WLN_RADIUS_Key3 ),
    CFG_STR_ENTRY_( WLN_RADIUS_Key4 ),
    CFG_STR_ENTRY_( WLN_ApCliEnable ),
    CFG_STR_ENTRY_( WLN_ApCliBssid ),
    CFG_STR_ENTRY_( WLN_ApCliAuthMode ),
    CFG_STR_ENTRY_( WLN_ApCliEncrypType ),
    CFG_STR_ENTRY_( WLN_ApCliKey1Type ),
    CFG_STR_ENTRY_( WLN_ApCliKey2Type ),
    CFG_STR_ENTRY_( WLN_ApCliKey3Type ),
    CFG_STR_ENTRY_( WLN_ApCliKey4Type ),
    CFG_STR_ENTRY_( WLN_ApCliKey1Str ),
    CFG_STR_ENTRY_( WLN_ApCliKey2Str ),
    CFG_STR_ENTRY_( WLN_ApCliKey3Str ),
    CFG_STR_ENTRY_( WLN_ApCliKey4Str ),
    CFG_STR_ENTRY_( WLN_ApCliDefaultKeyID ),
    CFG_STR_ENTRY_( WLN_ApCliWPAPSK ),
    CFG_STR_ENTRY_( WLN_ApCliSsid ),
    CFG_STR_ENTRY_( WLN_WapiKeyType ),
    CFG_STR_ENTRY_( WLN_WapiKey ),
    CFG_STR_ENTRY_( WLN_HT_BSSCoexistence ),
    CFG_STR_ENTRY_( WLN_RadioOn ),
    CFG_STR_ENTRY_( WLN_IgmpSnEnable ),
    CFG_STR_ENTRY_( WLN_SSIDIndex ),
    CFG_STR_ENTRY_( WLN_PMFMFPC ),
    CFG_STR_ENTRY_( WLN_PMFMFPR ),
    CFG_STR_ENTRY_( WLN_PMFSHA256 ),
    CFG_STR_ENTRY_( WLN_MACRepeaterEn ),
    CFG_STR_ENTRY_( WLN_MACRepeaterOuiMode ),
    
    CFG_STR_ENTRY_( STS_FMW_VER ),
    CFG_STR_ENTRY_( STS_FMW_INFO ),

    CFG_STR_ENTRY_( HW_VER ),
    CFG_STR_ENTRY_( HW_SN ),
    CFG_STR_ENTRY_( HW_NAME ),
    CFG_STR_ENTRY_( HW_PARM ),
/*
    CFG_STR_ENTRY_( HW_LAN_MAC ),
    CFG_STR_ENTRY_( HW_WAN_MAC ),
    CFG_STR_ENTRY_( HW_WLN_MAC ),
*/
	CFG_STR_ENTRY_( HW_MAC ),
	CFG_STR_ENTRY_( HW_BOT_VER ),
	CFG_STR_ENTRY_( HW_APP ),
	CFG_STR_ENTRY_( HW_VLAN ),
	CFG_STR_ENTRY_( HW_INTF ),
	CFG_STR_ENTRY_( HW_RUN_LOC ),
	CFG_STR_ENTRY_( HW_IMG_SZ ),
	CFG_STR_ENTRY_( HW_IP ),
	CFG_STR_ENTRY_( HW_MSK ),
	CFG_STR_ENTRY_( HW_GW ),
	CFG_STR_ENTRY_( HW_SVR ),
	CFG_STR_ENTRY_( HW_IMG_LOC ),
	CFG_STR_ENTRY_( HW_CFG_LOC ),
	CFG_STR_ENTRY_( HW_CFG_SZ ),
	CFG_STR_ENTRY_( HW_WLN_GAIN ),

    CFG_STR_ENTRY_( LOG_RM_EN ),
    CFG_STR_ENTRY_( LOG_RM_TYPE ),
    CFG_STR_ENTRY_( LOG_SYS_IP ),
    CFG_STR_ENTRY_( LOG_MAIL_IP ),
    CFG_STR_ENTRY_( LOG_E_MAIL ),
    CFG_STR_ENTRY_( LOG_MODE ),

	CFG_STR_ENTRY_( SOC_WDG ),
	
#ifdef CONFIG_IPTV
	 CFG_STR_ENTRY_( IPTV_EN ),
	 CFG_STR_ENTRY_( IPTV_PORT ),
#endif
    { 0, 0 }
};


#define	 CFG_ID_STR_SZ (sizeof(cfgid_str_table)/sizeof(struct cfgid_str_t) - 1)

int cfgid_str_table_sz=CFG_ID_STR_SZ;

