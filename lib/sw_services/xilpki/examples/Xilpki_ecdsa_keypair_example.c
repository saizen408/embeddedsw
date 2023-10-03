/******************************************************************************
* Copyright (C) 2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 * This example will demonstrate how to generate the ECDSA KeyPair's and
 * verification(Pairwise consistency test) using PKI library APIs.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who     Date      Changes
 * ----- ------  --------  -----------------------------------------------------
 * 1.0   Nava    10/25/22  First release
 *
 * </pre>
 *
 ******************************************************************************/
/****************************** Include Files *********************************/
#include "xilpki.h"

/***************** Macros (Inline Functions) Definitions *********************/
/* Below definition are user configurable to reserve the memory space for
 * Request queue and completion queue pages for PKI internal Operations.
 *
 * @note: This example supports only keystone-B platform.
 */
#define RQ_CFG_INPUT_PAGE_ADDR		0x540000U
#define RQ_CFG_OUTPUT_PAGE_ADDR		0x560000U
#define CQ_CFG_ADDR			0x580000U

#define MAX_BUFF_SIZE			66U

/*****************************************************************************/
int main(void)
{
	XPki_Instance InstanceParam = {0};
	XPki_EcdsaCrvInfo CrvInfo = {0};
	XPki_EcdsaKey PubKey = {0};
	u8 PubKeyQx[MAX_BUFF_SIZE] = {0};
	u8 PubKeyQy[MAX_BUFF_SIZE] = {0};
	u8 PrivKey[MAX_BUFF_SIZE] = {0};
	int Status = XST_FAILURE;

	InstanceParam.RQInputAddr = RQ_CFG_INPUT_PAGE_ADDR;
	InstanceParam.RQOutputAddr = RQ_CFG_OUTPUT_PAGE_ADDR;
	InstanceParam.CQAddr = CQ_CFG_ADDR;

	Status = XPki_Initialize(&InstanceParam);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to initialize the PKI module with error: 0x%x\r\n", Status);
		goto END;
	}

	PubKey.Qx = PubKeyQx;
	PubKey.Qy = PubKeyQy;

	Status = XPki_EcdsaGenerateKeyPair(&InstanceParam, ECC_NIST_P256, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P256 KeyPair generation failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P256 KeyPair generation test ran Successfully\r\n");
	}

	CrvInfo.CrvType = ECC_NIST_P256;
	CrvInfo.Crv = ECC_PRIME;

	Status = XPki_EcdsaPwct(&InstanceParam, &CrvInfo, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P256 Pairwise consistency test failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P256 Pairwise consistency test ran Successfully\r\n");
	}

	Status = XPki_EcdsaGenerateKeyPair(&InstanceParam, ECC_NIST_P384, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P384 KeyPair generation failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P384 KeyPair generation test ran Successfully\r\n");
	}

	CrvInfo.CrvType = ECC_NIST_P384;
	CrvInfo.Crv = ECC_PRIME;

	Status = XPki_EcdsaPwct(&InstanceParam, &CrvInfo, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P384 Pairwise consistency test failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P384 Pairwise consistency test ran Successfully\r\n");
	}

	Status = XPki_EcdsaGenerateKeyPair(&InstanceParam, ECC_NIST_P521, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P521 KeyPair generation failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P521 KeyPair generation test ran Successfully\r\n");
	}

	CrvInfo.CrvType = ECC_NIST_P521;
	CrvInfo.Crv = ECC_PRIME;

	Status = XPki_EcdsaPwct(&InstanceParam, &CrvInfo, &PubKey, PrivKey);
	if (Status != XST_SUCCESS) {
		xil_printf("ECDSA P521 Pairwise consistency test failed with error: 0x%x\r\n", Status);
		goto END;
	} else {
		xil_printf("ECDSA P521 Pairwise consistency test ran Successfully\r\n");
	}
END:
	XPki_Close();

	return Status;
}
