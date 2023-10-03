/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc. All rights reserved.
* Copyright (c) 2022 - 2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file versal_net/xplmi_wdt.c
*
* This file contains the PLMI WDT functionality related code for versal_net.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  bm   07/06/2022 Initial release
* 1.01  ng   11/11/2022 Fixed doxygen file name error
*       bm   01/14/2023 Remove bypassing of PLM Set Alive during boot
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xplmi_wdt.h"
#include "xplmi_hw.h"
#include "xplmi_status.h"
#include "xplmi_debug.h"
#include "xplmi.h"
#include "xplmi_update.h"
#include "xplmi_proc.h"
#ifdef XPLMI_PMC_WDT
#include "xwdttb.h"
#endif

/************************** Constant Definitions *****************************/
#define XPLMI_WDT_PERIODICITY		(100U)
#define XPLMI_SCHEDULER_PERIOD		(10U)

#define XPLMI_WDT_PERIODICITY_MIN	(15U)
#define XPLMI_WDT_PERIODICITY_MAX	(1000U)
#define XPLMI_SPP_WDT_PERIODICITY_MIN	(45U)

#define XPLMI_MIO_NUM_PER_BANK		(26U)

#define XPLMI_PM_STMIC_LMIO_0		(0x14104001U)
#define XPLMI_PM_STMIC_LMIO_25		(0x1410401aU)
#define XPLMI_PM_STMIC_PMIO_0		(0x1410801bU)
#define XPLMI_PM_STMIC_PMIO_51		(0x1410804eU)
#define XPLMI_PM_DEV_PMC_WDT		(0x1821C035U)

#ifdef XPLMI_PMC_WDT
#define XPLMI_WDT_VERSION	(1U)
#define XPLMI_WDT_LCVERSION	(1U)
#define XPLMI_PMC_WDT_GWOR_ADDR		(0xF03F2008U)
#endif

/**************************** Type Definitions *******************************/
typedef struct {
	u8 PlmLiveStatus; /**< PLM sets this bit to indicate it is alive */
	u8 IsEnabled; /**< Used to indicate if WDT is enabled or not */
	u32 Periodicity; /**< WDT period at which PLM should set the
			   live status */
	u32 GpioAddr; /**< GPIO address corresponding to MIO used for WDT */
	u32 GpioMask; /**< GPIO Mask corresponding to MIO used for WDT */
	u32 LastResetPeriod; /**< Last reset period is used to check last tick time */
} XPlmi_Wdt;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
#ifdef XPLMI_PMC_WDT
static void XPlmi_SetGWdtTimeout(u32 Time);
static int XPlmi_WdtDrvInit(void);
#endif

/************************** Variable Definitions *****************************/
static XPlmi_Wdt WdtInstance = {
	.PlmLiveStatus = (u8)FALSE,
	.IsEnabled = (u8)FALSE,
	.Periodicity = XPLMI_WDT_PERIODICITY,
	.GpioAddr = 0U,
	.GpioMask = 0U,
	.LastResetPeriod = 0U
};


#ifdef XPLMI_PMC_WDT
typedef struct {
	XPlmi_Wdt WdtInst; /**< Wdt Instance for PMC WDT */
	XWdtTb DrvInst; /**< Driver Instance for PMC WDT */
} XPlmi_PmcWdt;

static XPlmi_PmcWdt PmcWdtInstance __attribute__ ((aligned(4U))) = {
	.WdtInst.PlmLiveStatus = (u8)FALSE,
	.WdtInst.IsEnabled = (u8)FALSE,
	.WdtInst.Periodicity = XPLMI_WDT_PERIODICITY,
	.WdtInst.GpioAddr = 0U,
	.WdtInst.GpioMask = 0U,
	.WdtInst.LastResetPeriod = 0U
};

/* Export PmcWdtInstance to be stored during In-Place PLM Update */
EXPORT_GENERIC_DS(PmcWdtInstance, XPLMI_WDT_DS_ID, XPLMI_WDT_VERSION,
	XPLMI_WDT_LCVERSION, sizeof(PmcWdtInstance), (u32)(UINTPTR)&PmcWdtInstance);

/*****************************************************************************/
/**
 * @brief	This function initializes wdttb driver
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XPlmi_WdtDrvInit(void)
{
	int Status = XST_FAILURE;
	XWdtTb_Config *Config;
	u32 *WdtClkFreq;

	if (XPlmi_IsPlmUpdateDone() == (u8)TRUE) {
		Status = XST_SUCCESS;
		goto END;
	}

	Config = XWdtTb_LookupConfig(XPLMI_PMC_WDT_DEVICE_ID);
	if (NULL == Config) {
		goto END;
	}

	Status = XWdtTb_CfgInitialize(&PmcWdtInstance.DrvInst, Config,
                        Config->BaseAddr);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	if (XPLMI_PLATFORM == PMC_TAP_VERSION_SPP) {
		WdtClkFreq = XPlmi_GetPmcIroFreq();
		PmcWdtInstance.DrvInst.Config.Clock = *WdtClkFreq;
	}

END:
	return Status;
}
#endif

/*****************************************************************************/
/**
 * @brief	This function enables the WDT and sets NodeId and periodicity.
 *		It also verifies the parameters.
 *
 * @param	NodeId NodeId is the MIO node to be used by PLM for toggling.
 * @param	Periodicity at which MIO value should be toggled.
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
int XPlmi_EnableWdt(u32 NodeId, u32 Periodicity)
{
	int Status = XST_FAILURE;
	u32 MioNum;

	/** Check for Valid Node ID */
	if (NodeId == XPLMI_PM_DEV_PMC_WDT) {

#ifdef XPLMI_PMC_WDT
		if ((XPLMI_PLATFORM == PMC_TAP_VERSION_SPP) &&
			(Periodicity < XPLMI_SPP_WDT_PERIODICITY_MIN)) {
			Status = (int)XPLMI_ERR_WDT_PERIODICITY;
			goto END;
		}
		else if (Periodicity < XPLMI_WDT_PERIODICITY_MIN) {
			Status = (int)XPLMI_ERR_WDT_PERIODICITY;
			goto END;
		}

		/* Initialize wdttb driver */
		Status = XPlmi_WdtDrvInit();
		if (Status != XST_SUCCESS) {
			Status = XPLMI_ERR_PMC_WDT_DRV_INIT;
			goto END;
		}
		PmcWdtInstance.WdtInst.Periodicity = Periodicity;
		PmcWdtInstance.WdtInst.IsEnabled = (u8)TRUE;
		/* Set GWDT timeout */
		XPlmi_SetGWdtTimeout(Periodicity);
#else
		Status = (int)XPLMI_ERR_PMC_WDT_NOT_ENABLED;
		goto END;
#endif
	}
	else {
		if ((Periodicity < XPLMI_WDT_PERIODICITY_MIN) ||
		    (Periodicity > XPLMI_WDT_PERIODICITY_MAX)) {
			Status = (int)XPLMI_ERR_WDT_PERIODICITY;
			goto END;
		}

		if ((NodeId >= XPLMI_PM_STMIC_LMIO_0) &&
		    (NodeId <= XPLMI_PM_STMIC_LMIO_25)) {
			/* LPD MIO is used */
			if (XPlmi_IsLpdInitialized() != (u8)TRUE) {
				Status = (int)XPLMI_ERR_WDT_LPD_NOT_INITIALIZED;
				goto END;
			}
			XPlmi_SetLpdInitialized(LPD_WDT_INITIALIZED);
			MioNum = NodeId - XPLMI_PM_STMIC_LMIO_0;
			WdtInstance.GpioAddr = PS_GPIO_DATA_0_OFFSET;
			WdtInstance.GpioMask = (u32)(1U) << MioNum;
		}
		else if ((NodeId >= XPLMI_PM_STMIC_PMIO_0) &&
			 (NodeId <= XPLMI_PM_STMIC_PMIO_51)) {
			/* PMC MIO used */
			MioNum = NodeId - XPLMI_PM_STMIC_PMIO_0;
			if (MioNum < XPLMI_MIO_NUM_PER_BANK) {
				WdtInstance.GpioAddr = PMC_GPIO_DATA_0_OFFSET;
			}
			else {
				WdtInstance.GpioAddr = PMC_GPIO_DATA_1_OFFSET;
			}
			WdtInstance.GpioMask = (u32)(1U) <<
						(MioNum % XPLMI_MIO_NUM_PER_BANK);
		}
		else {
			Status = (int)XPLMI_ERR_WDT_NODE_ID;
			goto END;
		}

		WdtInstance.Periodicity = Periodicity;
		WdtInstance.IsEnabled = (u8)TRUE;
	}
	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function configures the default WDT configuration if
 * 		the wdt is not enabled and ROM SWDT usage bit is enabled in
 * 		EFUSE
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
int XPlmi_DefaultSWdtConfig(void)
{
	int Status = XST_FAILURE;
#ifdef XPLMI_PMC_WDT
	u32 CntVal;
	u32 *WdtClkFreq;
	u32 Period;

	if ((XPlmi_RomSwdtUsage() == (u8)TRUE) &&
		(PmcWdtInstance.WdtInst.IsEnabled == (u8)FALSE)) {
		CntVal = XPlmi_In32(XPLMI_PMC_WDT_GWOR_ADDR);
		WdtClkFreq = XPlmi_GetPmcIroFreq();
		if (XPLMI_PLATFORM != PMC_TAP_VERSION_SPP) {
			*WdtClkFreq /= 4U;
		}
		Period = (u32)((u64)CntVal * 2U * 1000U / *WdtClkFreq);
		Status = XPlmi_EnableWdt(XPLMI_PM_DEV_PMC_WDT, Period);
	}
	else {
		Status = XST_SUCCESS;
	}
#else
	Status = XST_SUCCESS;
#endif

	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function disables the WDT. This is required when LPD
 *		is powered down and if LPD MIO is used. Also required
 *		when debugging.
 *
 * @param	NodeId is the Node ID of external WDT or internal PMC WDT
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_DisableWdt(u32 NodeId)
{
	if (NodeId == XPLMI_WDT_INTERNAL) {
#ifdef XPLMI_PMC_WDT
		PmcWdtInstance.WdtInst.IsEnabled = (u8)FALSE;
#endif
	}
	else {
		WdtInstance.IsEnabled = (u8)FALSE;
	}
}

/*****************************************************************************/
/**
 * @brief	This function stops PMC WDT. Used during In-Place PLM update.
 *
 * @param	NodeId is the Node ID of external WDT or internal PMC WDT
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_StopWdt(u32 NodeId)
{
	if (NodeId == XPLMI_WDT_INTERNAL) {
#ifdef XPLMI_PMC_WDT
		if (PmcWdtInstance.WdtInst.IsEnabled == (u8)TRUE) {
			XWdtTb_Stop(&PmcWdtInstance.DrvInst);
		}
#endif
	}
}

/*****************************************************************************/
/**
 * @brief	This function Sets the PLM Status.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_SetPlmLiveStatus(void)
{
#ifdef XPLMI_PMC_WDT
	PmcWdtInstance.WdtInst.PlmLiveStatus = (u8)TRUE;
#endif
	WdtInstance.PlmLiveStatus = (u8)TRUE;
}

/*****************************************************************************/
/**
 * @brief	This function clears the PLM status.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_ClearPlmLiveStatus(void)
{
#ifdef XPLMI_PMC_WDT
	PmcWdtInstance.WdtInst.PlmLiveStatus = (u8)FALSE;
#endif
	WdtInstance.PlmLiveStatus = (u8)FALSE;
}

/*****************************************************************************/
/**
 * @brief	This function is used to refresh WDT before timeout
 *
 * @param	NodeId is the Node ID of external WDT or internal PMC WDT
 *
 * @return	None
 *
 *****************************************************************************/
static void XPlmi_RefreshWdt(u32 NodeId)
{
	XPlmi_Wdt *WdtPtr = NULL;
	u32 MinPeriodicity = XPLMI_WDT_PERIODICITY_MIN;

	if (NodeId == XPLMI_WDT_EXTERNAL) {
		WdtPtr = &WdtInstance;
	}
	else {
#ifdef XPLMI_PMC_WDT
		WdtPtr = &PmcWdtInstance.WdtInst;
#endif
	}

	/** if WDT is not enabled, just return */
	if (WdtPtr->IsEnabled == (u8)FALSE) {
		goto END;
	}

	if (XPLMI_PLATFORM == PMC_TAP_VERSION_SPP) {
		MinPeriodicity = XPLMI_SPP_WDT_PERIODICITY_MIN;
	}

	WdtPtr->LastResetPeriod += XPLMI_SCHEDULER_PERIOD;

	/** Toggle MIO only when last reset period exceeds periodicity */
	if (WdtPtr->LastResetPeriod >
	    (WdtPtr->Periodicity - MinPeriodicity)) {
		if (WdtPtr->PlmLiveStatus == (u8)TRUE) {
			XPlmi_KickWdt(NodeId);
			WdtPtr->PlmLiveStatus = (u8)FALSE;
		}
		WdtPtr->LastResetPeriod = 0U;
	}

END:
	return;

}

#ifdef XPLMI_PMC_WDT
/*****************************************************************************/
/**
 * @brief	This function is used to set GWDT timeout
 *
 * @param	Time is timeout in ms
 *
 * @return	None
 *
 *****************************************************************************/
static void XPlmi_SetGWdtTimeout(u32 Time)
{

	if (PmcWdtInstance.WdtInst.IsEnabled == (u8)FALSE) {
		goto END;
	}
	XWdtTb_Stop(&PmcWdtInstance.DrvInst);
	XWdtTb_SetGenericWdtWindowTimeOut(&PmcWdtInstance.DrvInst, Time / 2U);
	XWdtTb_Start(&PmcWdtInstance.DrvInst);
END:
	return;
}
#endif

/*****************************************************************************/
/**
 * @brief	This function is used to kick the WDT
 *
 * @param	NodeId is the Node ID of external WDT or internal PMC WDT
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_KickWdt(u32 NodeId)
{
	if (NodeId == XPLMI_WDT_EXTERNAL) {
		if (WdtInstance.IsEnabled == (u8)FALSE) {
			goto END;
		}
		XPlmi_Out32(WdtInstance.GpioAddr,
		XPlmi_In32(WdtInstance.GpioAddr) ^ WdtInstance.GpioMask);
	}
	else {
#ifdef XPLMI_PMC_WDT
		if (PmcWdtInstance.WdtInst.IsEnabled == (u8)FALSE) {
			goto END;
		}
		XWdtTb_RestartWdt(&PmcWdtInstance.DrvInst);
#endif
	}
END:
	return;
}

/*****************************************************************************/
/**
 * @brief	This function is used to restore wdt after the In-Place PLM Update
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_RestoreWdt(void)
{
#ifdef XPLMI_PMC_WDT
	if (PmcWdtInstance.WdtInst.IsEnabled == (u8)TRUE) {
		PmcWdtInstance.WdtInst.LastResetPeriod = 0U;
		XPlmi_SetGWdtTimeout(PmcWdtInstance.WdtInst.Periodicity);
	}
#endif
}

/*****************************************************************************/
/**
 * @brief	This function is handler for WDT. Scheduler calls this
 *		function periodically to check the PLM Live status and to
 *		toggle the MIO.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
void XPlmi_WdtHandler(void)
{
	XPlmi_RefreshWdt(XPLMI_WDT_EXTERNAL);
#ifdef XPLMI_PMC_WDT
	XPlmi_RefreshWdt(XPLMI_WDT_INTERNAL);
#endif
}
