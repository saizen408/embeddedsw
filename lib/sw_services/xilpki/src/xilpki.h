/******************************************************************************
* Copyright (C) 2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 * @file xilpki.h
 * @addtogroup xilpki Overview
 * This file contains the implementation of the interface functions for the
 * PKI ECDSA hardware module.
 *
 * This library supports the following features:
 *	- ECDSA signature generation support - for P256, P384 and P521 curves.
 *	- ECDSA signature verification support- for P256, P384 and P521 curves.
 *	- ECC Point multiplication support - for P256, P384 and P521 curves.
 *	- ECC Key pair generation support - for  P256, P384 and P521 curves.
 *	- ECC P256 sign generation and verification KAT.
 *	- ECC P384 sign generation and verification KAT.
 *	- ECC P521 sign generation and verification KAT.
 *	- ECC P256 pairwise consistency known answer test.
 *	- ECC P384 pairwise consistency known answer test.
 *	- ECC P521 pairwise consistency known answer test
 *
 * @{
 * @details
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who   Date      Changes
 * ----- ----  --------  -------------------------------------------------------
 * 1.0   Nava  12/05/22  Initial Release
 *
 * </pre>
 *
 * @note
 *
 ******************************************************************************/
#ifndef XILPKI_H
#define XILPKI_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files ********************************/
#include "sleep.h"
#include "xil_assert.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_util.h"
#include "xparameters.h"
#include "xilpki_ecdsa.h"
#include "xtrngpsx.h"

/************************** Constant Definitions *****************************/
/**
 * FPD_SLCR Base Address
 */
#define FPD_SLCR_BASEADDR		0xEC8C0000U

/**
 * Register: FPD_SLCR_WPROT0
 */
#define FPD_SLCR_WPROT0			( ( FPD_SLCR_BASEADDR ) + 0x00000000U )
#define FPD_SLCR_WPROT0_FULLMASK	0x00000001U
#define FPD_SLCR_WPROT0_FULLRWMASK	0x00000001U
#define FPD_SLCR_WPROT0_DEFVAL		0x1U

/*
 * Access_type: rw
 */
#define FPD_SLCR_WPROT0_ACTIVE_SHIFT	0U
#define FPD_SLCR_WPROT0_ACTIVE_WIDTH	1U
#define FPD_SLCR_WPROT0_ACTIVE_MASK	0x00000001U
#define FPD_SLCR_WPROT0_ACTIVE_DEFVAL	0x1U

/**
 * Register: FPD_SLCR_PKI_MUX_SEL
 */
#define FPD_SLCR_PKI_MUX_SEL		(( FPD_SLCR_BASEADDR ) + 0x00002000U )
#define FPD_SLCR_PKI_MUX_SEL_FULLMASK	0x00000001U
#define FPD_SLCR_PKI_MUX_SEL_FULLRWMASK	0x00000001U
#define FPD_SLCR_PKI_MUX_SEL_DEFVAL	0x0U

/*
 * Access_type: rw
 */
#define FPD_SLCR_PKI_MUX_SEL_PKI_SMUX_PATHSEL_SHIFT	0U
#define FPD_SLCR_PKI_MUX_SEL_PKI_SMUX_PATHSEL_WIDTH	1U
#define FPD_SLCR_PKI_MUX_SEL_PKI_SMUX_PATHSEL_MASK	0x00000001U
#define FPD_SLCR_PKI_MUX_SEL_PKI_SMUX_PATHSEL_DEFVAL	0x0U


/* PKI reset reg */
#define PSX_CRF_RST_PKI			(0xEC200340U)

#define FPD_PKI_CRYPTO_BASEADDR		(0x20400000000U)
#define FPD_PKI_CTRLSTAT_BASEADDR	(0x20400050000U)

/**
 * Register: PKI_ENGINE_CTRL
 */
#define FPD_PKI_ENGINE_CTRL		( ( FPD_PKI_CTRLSTAT_BASEADDR ) + 0x00000C00U )
#define FPD_PKI_ENGINE_CM_MASK		0x1U
#define FPD_PKI_ENGINE_CM_ENABLE_VAL	0x0U
#define FPD_PKI_ENGINE_CM_DISABLE_VAL	0x1U


/* PKI Soft Reset */
#define FPD_PKI_SOFT_RESET			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000038U )

/*PKI: IRQ_ENABLE */
#define FPD_PKI_IRQ_STATUS			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000088U )

/*PKI: IRQ_ENABLE */
#define FPD_PKI_IRQ_ENABLE			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000090U )

/* PKI RQ_CFG_PAGE_ADDR */
#define FPD_PKI_RQ_CFG_PAGE_ADDR_INPUT		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000100U )
#define FPD_PKI_RQ_CFG_PAGE_ADDR_OUTPUT		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000108U )

/* PKI: RQ_CFG_PAGE_SIZE */
#define FPD_PKI_RQ_CFG_PAGE_SIZE		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000120U )

/* PKI: RQ_CFG_CQID_00: Index of the completion queue associated to this request queue */
#define FPD_PKI_RQ_CFG_CQID			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000128U )

/* Type of allowed descriptors */
#define FPD_PKI_RQ_CFG_PERMISSIONS		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000130U )

/* PKI: RQ_CFG_QUEUE_DEPTH_00 */
#define FPD_PKI_RQ_CFG_QUEUE_DEPTH		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00000140U )


/* PKI: CQ_CFG_ADDR_00: Absolute address of the completion queue */
#define FPD_PKI_CQ_CFG_ADDR			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00001100U )

/* PKI: CQ_CFG_SIZE_00 */
#define FPD_PKI_CQ_CFG_SIZE			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00001108U )

/* PKI: CQ_CFG_IRQ_IDX_00 */
#define FPD_PKI_CQ_CFG_IRQ_IDX			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00001110U )

/*PKI: RQ_CTL_NEW_REQUEST_00 */
#define FPD_PKI_RQ_CTL_NEW_REQUEST		( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00002000U )

/*PKI: CQ_CTL_TRIGPOS_00 */
#define FPD_PKI_CQ_CTL_TRIGPOS			( ( FPD_PKI_CRYPTO_BASEADDR ) + 0x00002028U )

/* Error codes */
#define XPKI_INVALID_PARAM		0x2U
#define XPKI_ERROR_UNALIGN_ADDR		0x3U

/**************************** Type Definitions *******************************/
typedef struct {
	UINTPTR RQInputAddr;
	UINTPTR RQOutputAddr;
	UINTPTR CQAddr;
	u8 Is_Cm_Enabled;
} XPki_Instance;

/************************** Function Prototypes ******************************/
void XPki_Reset(void);
void XPki_SoftReset(void);
int XPki_Initialize(XPki_Instance *InstancePtr);
int XPki_EcdsaGenerateSign(XPki_Instance *InstancePtr,
			   XPki_EcdsaSignInputData *SignParams,
			   XPki_EcdsaSign *Sign);
int XPki_EcdsaVerifySign(XPki_Instance *InstancePtr,
			 XPki_EcdsaVerifyInputData *VerifyParams);
int XPki_EcdsaPointMulti(XPki_Instance *InstancePtr,
			 XPki_EcdsaPointMultiInputData *PointMultiParams,
			 XPki_EcdsaGpoint *Gpoint);
int XPki_EcdsaGenerateKeyPair(XPki_Instance *InstancePtr, XPki_EcdsaCrvType CrvType,
			      XPki_EcdsaKey *Pubkey, u8 *PrivKey);
int XPki_EcdsaVerifySignKat(XPki_Instance *InstancePtr, XPki_EcdsaCrvInfo *CrvInfo);
int XPki_EcdsaSignGenerateKat(XPki_Instance *InstancePtr, XPki_EcdsaCrvInfo *CrvInfo);
int XPki_EcdsaPwct(XPki_Instance *InstancePtr, XPki_EcdsaCrvInfo *CrvInfo,
		   XPki_EcdsaKey *Pubkey, u8 *PrivKey);
int XPki_TrngGenerateRandomNum(u8 Size, u8 *RandBuf);
void XPki_Close(void);
#endif  /* XILPKI_H */
