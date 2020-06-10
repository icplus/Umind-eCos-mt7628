/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    ap_apcli.c

    Abstract:
    Support Timing Measurment function.

    Who             When            What
    --------------  ----------      ----------------------------------------------
    Carter          2014-1120       created
*/

#ifdef TMR_SUPPORT
#ifdef MT_MAC

#include "rt_config.h"

inline VOID 
FillTodToa(
    IN UINT32 tod_high, 
    IN UINT32 tod_low, 
    IN UINT32 toa_high, 
    IN UINT32 toa_low, 
    OUT UINT64 *p_tod, 
    OUT UINT64 *p_toa)
{
    *p_tod = (((UINT64)(tod_high)) << 32) | tod_low;
    *p_toa = (((UINT64)(toa_high)) << 32) | toa_low;
}

VOID TmrReportParser(RTMP_ADAPTER *pAd,TMR_FRM_STRUC *tmr)
{
    struct rmac_rxd_0_tmr *ptmr_d0 = &tmr->tmr_d0;
    UINT32 *ptod_0 = &tmr->tod_0;
    UINT32 *ptoa_0 = &tmr->toa_0;
    TMR_D_6 *tmr_d6 = &tmr->tmr_d6;
    TMR_D_2 *ptmr_d2 = &tmr->tmr_d2;
    TMR_D_1 *tmr_d1 = &tmr->tmr_d1;
    UCHAR   *pta_16 = (UCHAR *)&tmr->ta_16;
    UCHAR   PeerAddr[MAC_ADDR_LEN] = {0};

    static UINT32 lastest_TOA = 0;
    static UINT32 lastest_sn = 0;
    UINT32 tod_fine = tmr_d1->field_init.tod_fine;
    UINT32 tod_low = 0;
    UINT32 toa_low = 0;
    UINT32 tod_high = 0;
    UINT32 toa_high = 0;
    UCHAR dbg_lvl = DBG_LVL_OFF;
    UCHAR dbg_lvl_error = DBG_LVL_OFF;
    UINT64 tod = 0;
    UINT64 toa = 0;
    UINT64 delta = 0;

    if (pAd->TmrRole == TMR_DISABLE)
        return;

#ifdef FTM_SUPPORT
    if (ptmr_d0->ir == TMR_IR0_TX)
        MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("TMR(%d%d)PID:0x%02X\n", 
            ptmr_d0->toa_vld, ptmr_d0->tod_vld, tmr_d1->field_init.pid));
    else
        MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("TMR: TYPE=%x, SUB_TYPE=%x, SN=0x%04X\n",
            ptmr_d0->type, ptmr_d0->sub_type, ptmr_d2->field.sn));

    dbg_lvl = DBG_LVL_INFO;
    dbg_lvl_error = DBG_LVL_WARN;

    if (DebugLevel >= dbg_lvl)
#endif /* FTM_SUPPORT */
    hex_dump("TMR RAW data: ", (UCHAR *)tmr, 28);

    tod_low = tmr->tod_0;
    toa_low = tmr->toa_0;
    tod_high = tmr->tmr_d6.field.tod_32;
    toa_high = tmr->tmr_d6.field.toa_32;
    FillTodToa(tod_high, tod_low, toa_high, toa_low, &tod, &toa);

    if (pAd->TmrCalResult > 0) {
        MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl,
                ("TmrCalResult = 0x%x\n", pAd->TmrCalResult));
        toa = toa - pAd->TmrCalResult - 64000/*16us*/ + 12800/*3.2us*/;
    }
    tod = tod - (tod_fine*33/4);

#ifdef FTM_SUPPORT
    FtmConvertTodToa(pAd, &tod, &toa);
#endif /* FTM_SUPPORT */

    /* calculate delta time */
    if (ptmr_d0->ir == TMR_IR1_RX) {
        delta = tod - toa;
    }
    else {
        delta = toa - tod;

        if (pAd->TmrState == SEND_OUT)
        {
            MtSetTmrCR(pAd, TMR_DISABLE);
            pAd->TmrState = SEND_IDLE;
        }
    }
    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl,
                ("DWORD_0: ByteCnt=%d, NC=%d, TMF=%d, "
                    "toa_vld=%d, tod_vld=%d, tod_fine=%x\n",
                    ptmr_d0->rx_byte_cnt, ptmr_d0->nc, ptmr_d0->tmf,
                    ptmr_d0->toa_vld, ptmr_d0->tod_vld, tod_fine));

    if (ptmr_d0->ir == TMR_IR1_RX)
    {	/* TMR Responder, Rx case */
        if (lastest_sn == ptmr_d2->field.sn)
            MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl_error, ("##### latest sn is same as last time\n"));

        lastest_sn = ptmr_d2->field.sn;

        MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl, ("TYPE=%x, SUB_TYPE=%x, SN=%x\n",
                ptmr_d0->type, ptmr_d0->sub_type, ptmr_d2->field.sn));
        PeerAddr[0] = ptmr_d2->field.ta_0;
        PeerAddr[1] = ptmr_d2->field.ta_1;
        NdisCopyMemory(PeerAddr+2, (UCHAR *)pta_16, 4);
		
        MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl,
                ("Readable TA = %02x:%02x:%02x:%02x:%02x:%02x\n",
                PRINT_MAC(PeerAddr)));
    }
    else
    {	/* TMR Initiator, Tx case */
        if (*ptoa_0 == lastest_TOA)
        {
            MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("##### latest TOA is same as last time\n"));

/* 2015.06.22 location plugfest: do not return */
#ifndef FTM_SUPPORT
            return;
#endif
        }
        lastest_TOA = *ptoa_0;
    }

    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl, ("DWORD_4: TOD[0:31]=0x%x\n", *ptod_0));
    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl, ("DWORD_6: TOD[32:47]=0x%x\n", tmr_d6->field.tod_32));

    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl, ("DWORD_5: TOA[0:31]=0x%x\n", *ptoa_0));
    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl, ("DWORD_6: TOA[32:47]=0x%x\n", tmr_d6->field.toa_32));
    MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, dbg_lvl,
#ifndef FTM_SUPPORT
        ("TMR Report: ir = %d, delta_high = %x, delta_low = %x\n\n",
            ptmr_d0->ir, (UINT32)(delta >> 32), (UINT32)(delta & 0x00000000FFFFFFFF)));
#else
        ("TMR Report: ir = %d, delta = 0x%016llX (unit converted)\n\n",
            ptmr_d0->ir, delta));
#endif /* FTM_SUPPORT */

#ifdef FTM_SUPPORT
	{
		PFTM_PEER_ENTRY	pFtmEntry = NULL;

		/* Get Peer FTM Entry */
		if (ptmr_d0->ir == TMR_IR1_RX)
			pFtmEntry = FtmEntrySearch(pAd, PeerAddr);
		else
			pFtmEntry = FtmGetPidPendingNode(pAd, tmr_d1->field_init.pid);

		if (!pFtmEntry)
		{
			MTWF_LOG(DBG_CAT_PROTO, CATPROTO_FTM, DBG_LVL_INFO, ("%s()#%d: invalid FTM entry !!\n", __FUNCTION__, __LINE__));
			return;
		}

		/* Record TMR */
		if (ptmr_d0->ir == TMR_IR0_TX)
		{	/* Tx case: for FTM responder */
			if (pFtmEntry->bGotTmr)
				MTWF_LOG(DBG_CAT_PROTO, CATPROTO_FTM, DBG_LVL_WARN, ("%s()#%d: bGotTmr again\n", __FUNCTION__, __LINE__));
			else
				pFtmEntry->bGotTmr = TRUE;

			NdisCopyMemory(&pFtmEntry->Tmr.HwReport, tmr, sizeof(TMR_FRM_STRUC));
			pFtmEntry->Tmr.toa = toa;
			pFtmEntry->Tmr.tod = tod;
		}
#ifdef FTM_INITIATOR
		else
		{	/* Rx case: for FTM initiator */
			ULONG IrqFlags = 0;
			PFTM_MAPPING pNode = NULL;

			if (os_alloc_mem(NULL, (UCHAR **)&pNode, sizeof(FTM_MAPPING)) == NDIS_STATUS_FAILURE)
				return;

			NdisZeroMemory(pNode, sizeof(FTM_MAPPING));

			RTMP_IRQ_LOCK(&pFtmEntry->RxTmrQLock, IrqFlags);
			NdisCopyMemory(&pNode->tmr.HwReport, tmr, sizeof(TMR_FRM_STRUC));
			pNode->tmr.toa = toa;
			pNode->tmr.tod = tod;
			pNode->sn = ptmr_d2->field.sn;

			DlListAddTail(&pFtmEntry->RxTmrQ, &pNode->list);
			RTMP_IRQ_UNLOCK(&pFtmEntry->RxTmrQLock, IrqFlags);
		}
#endif /* FTM_INITIATOR */
	}
#endif /* FTM_SUPPORT */
    return;
}

#endif /* MT_MAC */
#endif /* TMR_SUPPORT */

