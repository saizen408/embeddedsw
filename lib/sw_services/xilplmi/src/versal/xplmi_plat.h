/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file versal/xplmi_plat.h
*
* This file contains declarations of versal specific APIs
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  bm   07/06/2022 Initial release
*       ma   07/08/2022 Add ScatterWrite and ScatterWrite2 commands to versal
*       is   07/19/2022 Add HBM temperature monitoring registers from RTCA
*       kpt  07/21/2022 Added API and macros related to KAT
*       bm   07/22/2022 Update EAM logic for In-Place PLM Update
*       bm   07/22/2022 Added compatibility check for In-Place PLM Update
*       bm   07/22/2022 Retain critical data structures after In-Place PLM Update
*       bm   07/22/2022 Shutdown modules gracefully during update
*       bm   09/14/2022 Move ScatterWrite commands from common to versal_net
* 1.01  bm   11/07/2022 Clear SSS Cfg Error in SSSCfgSbiDma for Versal Net
*       ng   11/11/2022 Fixed doxygen file name error
*       bm   01/03/2023 Notify Other SLRs about Secure Lockdown
*       sk   01/11/2023 Added Config Space for Image Store in RTCA
*       bm   01/18/2023 Fix CFI readback logic with correct keyhole size
*
* </pre>
*
* @note
*
******************************************************************************/

#ifndef XPLMI_PLAT_H
#define XPLMI_PLAT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xplmi_hw.h"
#include "xplmi_dma.h"
#include "xplmi_event_logging.h"
#include "xplmi_cmd.h"

/************************** Constant Definitions *****************************/
#define XPLMI_PLM_BANNER	"Xilinx Versal Platform Loader and Manager \n\r"

/* PLM RunTime Configuration Area Base Address */
#define XPLMI_RTCFG_BASEADDR			(0xF2014000U)

/* Offsets of PLM Runtime Configuration Registers */
#define XPLMI_RTCFG_PMC_ERR1_STATUS_ADDR	(XPLMI_RTCFG_BASEADDR + 0x154U)
#define XPLMI_RTCFG_PSM_ERR1_STATUS_ADDR	(XPLMI_RTCFG_BASEADDR + 0x15CU)
#define XPLMI_RTCFG_SECURE_STATE_PLM_ADDR	(XPLMI_RTCFG_BASEADDR + 0x16CU)

#define XPLMI_RTCFG_PLM_MJTAG_WA		(XPLMI_RTCFG_BASEADDR + 0x188U)
#define XPLMI_RTCFG_MIO_WA_BANK_500_ADDR	(XPLMI_RTCFG_BASEADDR + 0x270U)
#define XPLMI_RTCFG_MIO_WA_BANK_501_ADDR	(XPLMI_RTCFG_BASEADDR + 0x274U)
#define XPLMI_RTCFG_MIO_WA_BANK_502_ADDR	(XPLMI_RTCFG_BASEADDR + 0x278U)
#define XPLMI_MIO_FLUSH_ALL_PINS		0x3FFFFFFU
#define XPLMI_RTCFG_RST_PL_POR_WA		(XPLMI_RTCFG_BASEADDR + 0x27CU)
#define XPLMI_RTCFG_HBM_TEMP_CONFIG_AND_MAX	(XPLMI_RTCFG_BASEADDR + 0x280U)
#define XPLMI_RTCFG_HBM_TEMP_VAL		(XPLMI_RTCFG_BASEADDR + 0x284U)
#define XPLMI_RTCFG_IMG_STORE_ADDRESS_HIGH	(XPLMI_RTCFG_BASEADDR + 0x288U)
#define XPLMI_RTCFG_IMG_STORE_ADDRESS_LOW	(XPLMI_RTCFG_BASEADDR + 0x28CU)
#define XPLMI_RTCFG_IMG_STORE_SIZE		(XPLMI_RTCFG_BASEADDR + 0x290U)

/* Masks of PLM RunTime Configuration Registers */
#define XPLMI_RTCFG_PLM_MJTAG_WA_IS_ENABLED_MASK	(0x00000001U)
#define XPLMI_RTCFG_PLM_MJTAG_WA_STATUS_MASK	(0x00000002U)

/* Shifts of PLM RunTime Configuration Registers */
#define XPLMI_RTCFG_PLM_MJTAG_WA_STATUS_SHIFT	(0x00000001U)

/* Macros for command ids */
#define XPLMI_SSIT_SYNC_SLAVES_CMD_ID	(14U)
#define XPLMI_SSIT_SYNC_MASTER_CMD_ID	(15U)
#define XPLMI_SSIT_WAIT_SLAVES_CMD_ID	(16U)

/* GIC related Macros */
#define XPLMI_GICP_SOURCE_COUNT		(0x5U)
#define XPLMI_GICP_INDEX_SHIFT		(8U)
#define XPLMI_GICPX_INDEX_SHIFT		(16U)
#define XPLMI_GICPX_LEN			(0x14U)

/*
 * PMC GIC interrupts
 */
#define XPLMI_PMC_GIC_IRQ_GICP0		(0U)
#define XPLMI_PMC_GIC_IRQ_GICP1		(1U)
#define XPLMI_PMC_GIC_IRQ_GICP2		(2U)
#define XPLMI_PMC_GIC_IRQ_GICP3		(3U)
#define XPLMI_PMC_GIC_IRQ_GICP4		(4U)

/*
 * PMC GICP0 interrupts
 */
#define XPLMI_GICP0_SRC13	(13U) /**< GPIO Interrupt */
#define XPLMI_GICP0_SRC14	(14U) /**< I2C_0 Interrupt */
#define XPLMI_GICP0_SRC15	(15U) /**< I2C_1 Interrupt */
#define XPLMI_GICP0_SRC16	(16U) /**< SPI_0 Interrupt */
#define XPLMI_GICP0_SRC17	(17U) /**< SPI_1 Interrupt */
#define XPLMI_GICP0_SRC18	(18U) /**< UART_0 Interrupt */
#define XPLMI_GICP0_SRC19	(19U) /**< UART_1 Interrupt */
#define XPLMI_GICP0_SRC20	(20U) /**< CAN_0 Interrupt */
#define XPLMI_GICP0_SRC21	(21U) /**< CAN_1 Interrupt */
#define XPLMI_GICP0_SRC22	(22U) /**< USB_0 Interrupt */
#define XPLMI_GICP0_SRC23	(23U) /**< USB_0 Interrupt */
#define XPLMI_GICP0_SRC24	(24U) /**< USB_0 Interrupt */
#define XPLMI_GICP0_SRC25	(25U) /**< USB_0 Interrupt */
#define XPLMI_GICP0_SRC26	(26U) /**< USB_0 Interrupt */
#define XPLMI_GICP0_SRC27	(27U) /**< IPI Interrupt */

/*
 * PMC GICP1 interrupts
 */
#define XPLMI_GICP1_SRC5	(5U) /**< TTC_0 Interrupt */
#define XPLMI_GICP1_SRC6	(6U) /**< TTC_0 Interrupt */
#define XPLMI_GICP1_SRC7	(7U) /**< TTC_0 Interrupt */
#define XPLMI_GICP1_SRC8	(8U) /**< TTC_1 Interrupt */
#define XPLMI_GICP1_SRC9	(9U) /**< TTC_1 Interrupt */
#define XPLMI_GICP1_SRC10	(10U) /**< TTC_1 Interrupt */
#define XPLMI_GICP1_SRC11	(11U) /**< TTC_2 Interrupt */
#define XPLMI_GICP1_SRC12	(12U) /**< TTC_2 Interrupt */
#define XPLMI_GICP1_SRC13	(13U) /**< TTC_2 Interrupt */
#define XPLMI_GICP1_SRC14	(14U) /**< TTC_3 Interrupt */
#define XPLMI_GICP1_SRC15	(15U) /**< TTC_3 Interrupt */
#define XPLMI_GICP1_SRC16	(16U) /**< TTC_3 Interrupt */
#define XPLMI_GICP1_SRC24	(24U) /**< GEM_0 Interrupt */
#define XPLMI_GICP1_SRC25	(25U) /**< GEM_0 Interrupt */
#define XPLMI_GICP1_SRC26	(26U) /**< GEM_1 Interrupt */
#define XPLMI_GICP1_SRC27	(27U) /**< GEM_1 Interrupt */
#define XPLMI_GICP1_SRC28	(28U) /**< ADMA_0 Interrupt */
#define XPLMI_GICP1_SRC29	(29U) /**< ADMA_1 Interrupt */
#define XPLMI_GICP1_SRC30	(30U) /**< ADMA_2 Interrupt */
#define XPLMI_GICP1_SRC31	(31U) /**< ADMA_3 Interrupt */

/*
 * PMC GICP2 interrupts
 */
#define XPLMI_GICP2_SRC0	(0U) /**< ADMA_4 Interrupt */
#define XPLMI_GICP2_SRC1	(1U) /**< ADMA_5 Interrupt */
#define XPLMI_GICP2_SRC2	(2U) /**< ADMA_6 Interrupt */
#define XPLMI_GICP2_SRC3	(3U) /**< ADMA_7 Interrupt */
#define XPLMI_GICP2_SRC10	(10U) /**< USB_0 Interrupt */

/*
 * PMC GICP3 interrupts
 */
#define XPLMI_GICP3_SRC30	(30U) /**< SD_0 Interrupt */
#define XPLMI_GICP3_SRC31	(31U) /**< SD_0 Interrupt */

/*
 * PMC GICP4 interrupts
 */
#define XPLMI_GICP4_SRC0	(0U) /**< SD_1 Interrupt */
#define XPLMI_GICP4_SRC1	(1U) /**< SD_1 Interrupt */
#define XPLMI_GICP4_SRC8	(8U) /**< SBI interrupt */
#define XPLMI_GICP4_SRC14	(14U) /**< RTC interrupt */

#define XPLMI_IPI_INTR_ID		(0x1B0000U)
#define XPLMI_IPI_INDEX_SHIFT	(24U)
#define XPLMI_GIC_IPI_INTR_ID		(0x1B0010U)

#define XPLMI_SBI_GICP_INDEX	(XPLMI_PMC_GIC_IRQ_GICP4)
#define XPLMI_SBI_GICPX_INDEX	(XPLMI_GICP4_SRC8)

/*
 * RTCA area KAT masks
 */
#define XPLMI_SECURE_SHA3_KAT_MASK 						(0x00000010U)
#define XPLMI_SECURE_RSA_KAT_MASK 						(0x00000020U)
#define XPLMI_SECURE_ECC_SIGN_VERIFY_SHA3_384_KAT_MASK 	(0x00000040U)
#define XPLMI_SECURE_AES_DEC_KAT_MASK 					(0x00000080U)
#define XPLMI_SECURE_AES_CMKAT_MASK 					(0x00000100U)
#define XPLMI_SECURE_AES_ENC_KAT_MASK					(0x00000400U)
#define XPLMI_SECURE_RSA_PRIVATE_DEC_KAT_MASK	 		(0x00000800U)
#define XPLMI_SECURE_ECC_SIGN_GEN_SHA3_384_KAT_MASK 	(0x00001000U)
#define XPLMI_SECURE_ECC_PWCT_KAT_MASK					(0x00002000U)

#define XPLMI_ROM_KAT_MASK		(XPLMI_SECURE_SHA3_KAT_MASK | XPLMI_SECURE_RSA_KAT_MASK | \
								XPLMI_SECURE_ECC_SIGN_VERIFY_SHA3_384_KAT_MASK | XPLMI_SECURE_AES_DEC_KAT_MASK | \
								XPLMI_SECURE_AES_CMKAT_MASK)
#define XPLMI_KAT_MASK			(XPLMI_ROM_KAT_MASK | XPLMI_SECURE_AES_ENC_KAT_MASK | XPLMI_SECURE_RSA_PRIVATE_DEC_KAT_MASK | \
								XPLMI_SECURE_ECC_SIGN_GEN_SHA3_384_KAT_MASK | XPLMI_SECURE_ECC_PWCT_KAT_MASK)

/**************************** Type Definitions *******************************/
/* Minor Error Codes */
/* Add any platform specific minor error codes from 0xA0 */
enum {
	XPLMI_ERR_CURRENT_UART_INVALID = 0x2, /**< 0x2 - Error when current uart
						selected has invalid base address */
	XPLMI_ERR_INVALID_UART_SELECT, /**< 0x3 - Error when invalid uart select
						argument is passed */
	XPLMI_ERR_INVALID_UART_ENABLE, /**< 0x4 - Error when invalid uart enable
						argument is passed */
	XPLMI_ERR_NO_UART_PRESENT, /**< 0x5 - Error when no uart is present to
						configure in run-time */
};

/***************** Macros (Inline Functions) Definitions *********************/
/*
 * SLR Types
 */
#define XPLMI_SSIT_MONOLITIC		(0x7U)
#define XPLMI_SSIT_MASTER_SLR		(0x6U)
#define XPLMI_SSIT_SLAVE0_SLR_TOP	(0x5U)
#define XPLMI_SSIT_SLAVE0_SLR_NTOP	(0x4U)
#define XPLMI_SSIT_SLAVE1_SLR_TOP	(0x3U)
#define XPLMI_SSIT_SLAVE1_SLR_NTOP	(0x2U)
#define XPLMI_SSIT_SLAVE2_SLR_TOP	(0x1U)
#define XPLMI_SSIT_INVALID_SLR		(0x0U)

#define XPlmi_InPlacePlmUpdate		NULL
#define XPlmi_PsmSequence		NULL
#define XPlmi_ScatterWrite		NULL
#define XPlmi_ScatterWrite2		NULL
#define XPlmi_SetFipsKatMask	NULL

#define GET_RTCFG_PMC_ERR_ADDR(Index)		(XPLMI_RTCFG_PMC_ERR1_STATUS_ADDR + (Index * 4U))
#define GET_RTCFG_PSM_ERR_ADDR(Index)		(XPLMI_RTCFG_PSM_ERR1_STATUS_ADDR + (Index * 4U))

/*****************************************************************************/
/**
 * @brief	This function provides the Slr Type
 *
 * @return 	SlrType
 *
 *****************************************************************************/
static inline u8 XPlmi_GetSlrType(void)
{
	u8 SlrType = (u8)(XPlmi_In32(PMC_TAP_SLR_TYPE) &
			PMC_TAP_SLR_TYPE_VAL_MASK);
	return SlrType;
}

/*****************************************************************************/
/**
 * @brief	This function masks the secure stream switch value
 *
 * @param	InputSrc	- Input source to be selected for the resource
 * @param	OutputSrc	- Output source to be selected for the resource
 *
 * @return 	None
 *
 *****************************************************************************/
static inline void XPlmi_SssMask(u32 InputSrc, u32 OutputSrc)
{
	(void)InputSrc;
	(void)OutputSrc;

	/* Not Applicable for versal */
	return;
}

/*****************************************************************************/
/**
 * @brief	This function checks if the update is Done
 *
 * @return 	FALSE
 *
 *****************************************************************************/
static inline u8 XPlmi_IsPlmUpdateDone(void)
{
	/* Not Applicable for versal */
	return (u8)FALSE;
}

/*****************************************************************************/
/**
 * @brief	This function checks if the update is in progress
 *
 * @return 	FALSE
 *
 *****************************************************************************/
static inline u8 XPlmi_IsPlmUpdateInProgress(void)
{
	/* Not Applicable for versal */
	return (u8)FALSE;
}

/****************************************************************************/
/**
* @brief	This function will initialize In-Place Update related logic
*
* @param	CompatibilityHandler is the handler used for compatibility check
*
* @return	XST_SUCCESS on success and error code on failure
*
****************************************************************************/
static inline int XPlmi_UpdateInit(void *CompatibilityHandler)
{
	(void)CompatibilityHandler;
	/* Not Applicable for Versal */
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * @brief	This function is not applicable for versal
 *
 * @return 	None
 *
 *****************************************************************************/
static inline void XPlmi_IpiIntrHandler(void *CallbackRef)
{
	(void)CallbackRef;
	/* Not Applicable for Versal */
	return;
}

/*****************************************************************************/
/**
 * @brief	This function is not applicable for versal
 *
 * @return 	None
 *
 *****************************************************************************/
static inline void XPlmi_ClearSSSCfgErr(void)
{
	/* Not Applicable for Versal */
	return;
}

/*****************************************************************************/
/**
 * @brief	This function is not applicable for versal
 *
 * @return 	None
 *
 *****************************************************************************/
static inline int XPlmi_CheckAndUpdateFipsState(void)
{
	/* Not Applicable for Versal */
	return XST_SUCCESS;
}

/************************** Function Prototypes ******************************/
u32 *XPlmi_GetLpdInitialized(void);
void XPlmi_RtcaPlatInit(void);
void XPlmi_PrintRomVersion(void);
void XPlmi_PreInit(void);
XPlmi_WaitForDmaDone_t XPlmi_GetPlmiWaitForDone(u64 DestAddr);
XPlmi_CircularBuffer *XPlmi_GetTraceLogInst(void);
void XPlmi_GetReadbackSrcDest(u32 SlrType, u64 *SrcAddr, u64 *DestAddrRead);
void XPlmi_GicAddTask(u32 PlmIntrId);
XInterruptHandler *XPlmi_GetTopLevelIntrTbl(void);
u8 XPlmi_GetTopLevelIntrTblSize(void);
int XPlmi_RegisterNEnableIpi(void);
void XPlmi_EnableIomoduleIntr(void);
int XPlmi_SetPmcIroFreq(void);
int XPlmi_GetPitResetValues(u32 *Pit1ResetValue, u32 *Pit2ResetValue);
int XPlmi_VerifyAddrRange(u64 StartAddr, u64 EndAddr);
u32 XPlmi_GetGicIntrId(u32 GicPVal, u32 GicPxVal);
u32 XPlmi_GetIpiIntrId(u32 BufferIndex);
void XPlmi_EnableIpiIntr(void);
void XPlmi_ClearIpiIntr(void);
void XPlmi_DisableCFrameIso(void);
u32 *XPlmi_GetUartBaseAddr(void);
void XPlmi_GetBootKatStatus(volatile u32 *PlmKatStatus);
void XPlmi_NotifySldSlaveSlrs(void);
void XPlmi_InterSlrSldHandshake(void);

/* Functions defined in xplmi_plat_cmd.c */
int XPlmi_CheckIpiAccess(u32 CmdId, u32 IpiReqType);
int XPlmi_ValidateCmd(u32 ModuleId, u32 ApiId);

/************************** Variable Definitions *****************************/

#ifdef __cplusplus
}
#endif

#endif  /* XPLMI_PLAT_H */
