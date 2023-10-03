/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file	xilsecure_versal_aes_server_example.c
* @addtogroup xsecure_versal_aes_example XilSecure AES API Example Generic Usage
* @{
*
* @note
* This example illustrates the usage of Versal AES Server APIs,
* by encrypting the data with provided key and IV and decrypts the encrypted data
* and compares with original data and checks for GCM tag match.
*
* MODIFICATION HISTORY:
* <pre>
* Ver   Who    Date     Changes
* ----- ------ -------- -------------------------------------------------
* 1.0   kal    04/21/21 First Release
* 4.5   kal    04/21/21 Updated file version to sync with library version
*       har    06/02/21 Fixed GCC warnings for R5 compiler
* 4.7   kpt    12/01/21 Replaced library specific,standard utility functions
*                       with xilinx maintained functions
*
* </pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xsecure_aes.h"
#include "xil_util.h"
#include "xpmcdma.h"

/************************** Constant Definitions *****************************/

/* Harcoded KUP key for encryption of data */
#define	XSECURE_AES_KEY	\
	"F878B838D8589818E868A828C8488808F070B030D0509010E060A020C0408000"

/* Hardcoded IV for encryption of data */
#define	XSECURE_IV	"D2450E07EA5DE0426C0FA133"

#define XSECURE_DATA	\
	"1234567808F070B030D0509010E060A020C0408000A5DE08D85898A5A5FEDCA10134" \
	"ABCDEF12345678900987654321123487654124456679874309713627463801AD1056"

#define XSECURE_AAD			"67e21cf3cb29e0dcbc4d8b1d0cc5334b"

#define XSECURE_DATA_SIZE		(68)
#define XSECURE_DATA_SIZE_IN_BITS	(XSECURE_DATA_SIZE * 8U)
#define XSECURE_IV_SIZE			(12)
#define XSECURE_IV_SIZE_IN_BITS		(XSECURE_IV_SIZE * 8U)
#define XSECURE_KEY_SIZE		(32)
#define XSECURE_KEY_SIZE_IN_BITS	(XSECURE_KEY_SIZE * 8U)
#define XSECURE_AAD_SIZE		(16)
#define XSECURE_AAD_SIZE_IN_BITS	(XSECURE_AAD_SIZE * 8U)

#define XSECURE_PMCDMA_DEVICEID		PMCDMA_0_DEVICE_ID

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

static int SecureAesExample(void);

/************************** Variable Definitions *****************************/
static u8 Iv[XSECURE_IV_SIZE];
static u8 Key[XSECURE_KEY_SIZE];

#if defined (__GNUC__)
static u8 Data[XSECURE_DATA_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.Data")));
static u8 DecData[XSECURE_DATA_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.DecData")));
static u8 EncData[XSECURE_DATA_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.EncData")));
static u8 GcmTag[XSECURE_SECURE_GCM_TAG_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.GcmTag")));
static u8 Aad[XSECURE_AAD_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.Aad")));
#elif defined (__ICCARM__)
#pragma data_alignment = 64
static u8 Data[XSECURE_DATA_SIZE];
#pragma data_alignment = 64
static u8 DecData[XSECURE_DATA_SIZE];
#pragma data_alignment = 64
static u8 EncData[XSECURE_DATA_SIZE ];
#pragma data_alignment = 64
static u8 GcmTag[XSECURE_SECURE_GCM_TAG_SIZE];
#pragma data_alignment = 64
static u8 Aad[XSECURE_AAD_SIZE];
#endif

/************************** Function Definitions ******************************/
int main(void)
{
	int Status = XST_FAILURE;

	Status = Xil_ConvertStringToHexBE(
			(const char *) (XSECURE_AES_KEY),
				Key, XSECURE_KEY_SIZE_IN_BITS);
	if (Status != XST_SUCCESS) {
		xil_printf(
			"String Conversion error (KEY):%08x !!!\r\n", Status);
		goto END;
	}

	Status = Xil_ConvertStringToHexBE(
			(const char *) (XSECURE_IV),
				Iv, XSECURE_IV_SIZE_IN_BITS);
	if (Status != XST_SUCCESS) {
		xil_printf(
			"String Conversion error (IV):%08x !!!\r\n", Status);
		goto END;
	}

	Status = Xil_ConvertStringToHexBE(
			(const char *) (XSECURE_DATA),
				Data, XSECURE_DATA_SIZE_IN_BITS);
	if (Status != XST_SUCCESS) {
		xil_printf(
			"String Conversion error (Data):%08x !!!\r\n", Status);
		goto END;
	}

	Status = Xil_ConvertStringToHexBE((const char *) (XSECURE_AAD), Aad,
		XSECURE_AAD_SIZE_IN_BITS);
	if (Status != XST_SUCCESS) {
		xil_printf("String Conversion error (AAD):%08x !!!\r\n", Status);
		goto END;
	}

	Xil_DCacheFlushRange((UINTPTR)Iv, XSECURE_IV_SIZE);
	Xil_DCacheFlushRange((UINTPTR)Data, XSECURE_DATA_SIZE);
	Xil_DCacheFlushRange((UINTPTR)Key, XSECURE_KEY_SIZE);
	Xil_DCacheFlushRange((UINTPTR)Aad, XSECURE_AAD_SIZE);

	/* Encryption and decryption of the data */
	Status = SecureAesExample();
	if(Status == XST_SUCCESS) {
		xil_printf("\r\nSuccessfully ran Versal AES example\r\n");
	}
	else {
		xil_printf("\r\nVersal AES example failed\r\n");
	}

END:
	return Status;
}

/****************************************************************************/
/**
*
* This function encrypts the data with provided AES key and IV and decrypts
* the encrypted data also checks whether GCM tag is matched or not and finally
* compares the decrypted data with the original data provided.
*
* @param	None
*
* @return
*		- XST_FAILURE if the Aes example was failed.
*		- XST_SUCCESS if the Aes example was successful
*
* @note		None.
*
****************************************************************************/
/** //! [Generic AES example] */
static int SecureAesExample(void)
{
	XPmcDma_Config *Config;
	int Status = XST_FAILURE;
	u32 Index;
	XPmcDma PmcDmaInstance;
	XSecure_Aes Secure_Aes;

	/* Initialize PMC DMA driver */
	Config = XPmcDma_LookupConfig(XSECURE_PMCDMA_DEVICEID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XPmcDma_CfgInitialize(&PmcDmaInstance, Config,
					Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	/* Initialize the Aes driver so that it's ready to use */
	Status = XSecure_AesInitialize(&Secure_Aes, &PmcDmaInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("Failure at Aes initialize, Status = 0x%x \r\n",
			Status);
		goto END;
	}

	/* Write AES key */
	Status = XSecure_AesWriteKey(&Secure_Aes, XSECURE_AES_USER_KEY_0,
				XSECURE_AES_KEY_SIZE_256, (UINTPTR)Key);
	if (Status != XST_SUCCESS) {
		xil_printf("Failure at key write\n\r");
		goto END;
	}

	xil_printf("Data to be encrypted: \n\r");
	for (Index = 0; Index < XSECURE_DATA_SIZE; Index++) {
		xil_printf("%02x", Data[Index]);
	}
	xil_printf( "\r\n\n");


	Xil_DCacheInvalidateRange((UINTPTR)EncData, XSECURE_DATA_SIZE);
	Xil_DCacheInvalidateRange((UINTPTR)GcmTag, XSECURE_SECURE_GCM_TAG_SIZE);

	/* Encryption of Data */
	/*
	 * If all the data to be encrypted is contiguous one can call
	 * XSecure_AesEncryptData API directly after
	 * XSecure_AesEncryptInit() call
	 */
	Status = XSecure_AesEncryptInit(&Secure_Aes, XSECURE_AES_USER_KEY_0,
					XSECURE_AES_KEY_SIZE_256, (UINTPTR)Iv);
	if (Status != XST_SUCCESS) {
		xil_printf(" Aes encrypt init is failed\n\r");
		goto END;
	}

	Status = XSecure_AesUpdateAad(&Secure_Aes, (UINTPTR)Aad, XSECURE_AAD_SIZE);
	if (Status != XST_SUCCESS) {
		xil_printf(" Aes update aad failed %x\n\r", Status);
		goto END;
	}
	Status = XSecure_AesEncryptUpdate(&Secure_Aes, (UINTPTR)Data,(UINTPTR)EncData,
						XSECURE_DATA_SIZE, TRUE);
	if (Status != XST_SUCCESS) {
		xil_printf(" Aes encrypt update is failed\n\r");
		goto END;
	}

	Status = XSecure_AesEncryptFinal(&Secure_Aes, (UINTPTR)GcmTag);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed at GCM tag generation\n\r");
		goto END;
	}

	Xil_DCacheInvalidateRange((UINTPTR)EncData, XSECURE_DATA_SIZE);
	Xil_DCacheInvalidateRange((UINTPTR)GcmTag, XSECURE_SECURE_GCM_TAG_SIZE);

	xil_printf("Encrypted data: \n\r");
	for (Index = 0; Index < XSECURE_DATA_SIZE; Index++) {
		xil_printf("%02x", EncData[Index]);
	}
	xil_printf( "\r\n");

	xil_printf("GCM tag: \n\r");
	for (Index = 0; Index < XSECURE_SECURE_GCM_TAG_SIZE; Index++) {
		xil_printf("%02x", GcmTag[Index]);
	}
	xil_printf( "\r\n\n");

	Xil_DCacheInvalidateRange((UINTPTR)DecData, XSECURE_DATA_SIZE);

	/* Decrypts the encrypted data */
	/*
	 * If data to be decrypted is contiguous one can also call
	 * single API XSecure_AesDecryptData after
	 * XSecure_AesDecryptInit() call
	 */
	Status = XSecure_AesDecryptInit(&Secure_Aes, XSECURE_AES_USER_KEY_0,
					XSECURE_AES_KEY_SIZE_256, (UINTPTR)Iv);
	if (Status != XST_SUCCESS) {
		xil_printf("Error in decrypt initi %x\n\r", Status);
		goto END;
	}
	Status = XSecure_AesUpdateAad(&Secure_Aes, (UINTPTR)Aad, XSECURE_AAD_SIZE);
	if (Status != XST_SUCCESS) {
		xil_printf(" Aes update aad failed %x\n\r", Status);
		goto END;
	}
	Status = XSecure_AesDecryptUpdate(&Secure_Aes, (UINTPTR)EncData, (UINTPTR)DecData,
						 XSECURE_DATA_SIZE, TRUE);
	if (Status != XST_SUCCESS) {
		xil_printf("Aes decrypt update failed %x\n\r", Status);
		goto END;
	}
	Status = XSecure_AesDecryptFinal(&Secure_Aes, (UINTPTR)GcmTag);
	if (Status != XST_SUCCESS) {
		xil_printf("Decryption failure- GCM tag was not matched\n\r");
		goto END;
	}

	Xil_DCacheInvalidateRange((UINTPTR)DecData, XSECURE_DATA_SIZE);

	xil_printf("Decrypted data \n\r");
	for (Index = 0; Index < XSECURE_DATA_SIZE; Index++) {
		xil_printf("%02x", DecData[Index]);
	}
	xil_printf( "\r\n");

	/* Comparison of Decrypted Data with original data */
	Status = Xil_SMemCmp(Data, XSECURE_DATA_SIZE, DecData,
			XSECURE_DATA_SIZE, XSECURE_DATA_SIZE);
	if (Status != XST_SUCCESS) {
		xil_printf("Failure during comparison of the data\n\r");
		goto END;
	}

END:
	return Status;
}
/** //! [Generic AES example] */
/** @} */
