/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file net/client/xnvm_efuseclient.c
*
* This file contains the implementation of the client interface functions for
* eFUSE programming for Versal_Net.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 3.0  har  07/19/22  Initial release
* 3.1  skg  10/28/22  Added In body comments for APIs
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xil_cache.h"
#include "xil_util.h"
#include "xnvm_efuseclient.h"
#include "xnvm_efuseclient_hw.h"
#include "xnvm_mailbox.h"
#include "xnvm_validate.h"

/************************** Constant Definitions *****************************/
#define XNVM_ADDR_HIGH_SHIFT					(32U)
#define XNVM_MAX_PAYLOAD_LEN					(7U)

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_ECC_23_0_MASK		(0x00ffffffU)

/************************** Function Prototypes ******************************/
static void XNvm_EfuseCreateWriteKeyCmd(XNvm_AesKeyWriteCdo* AesKeyWrCdo, XNvm_AesKeyType KeyType, u32 AddrLow, u32 AddrHigh);
static void XNvm_EfuseCreateWritePpkCmd(XNvm_PpkWriteCdo* PpkWrCdo, XNvm_PpkType PpkType, u32 AddrLow, u32 AddrHigh);
static void XNvm_EfuseCreateWriteIvCmd(XNvm_IvWriteCdo* IvWrCdo, XNvm_IvType IvType, u32 AddrLow, u32 AddrHigh);
static void XNvm_EfuseCreateReadEfuseCacheCmd(XNvm_RdCacheCdo* RdCacheCdo, u16 StartOffset, u8 RegCount, u32 AddrLow, u32 AddrHigh);
static void XNvm_EfuseCreateWritePufCmd(XNvm_PufWriteCdo* PufWrCdo, u32 AddrLow, u32 AddrHigh);

/************************** Variable Definitions *****************************/
/*****************************************************************************/
/**
 * @brief	This function sends IPI request to program eFuses with
 * 		user provided data
 *
 * @param	InstancePtr 	Pointer to the client instance
 * @param	DataAddr	Address of the data structure where the eFUSE
 * 				data to be programmed is stored
 *
 * @return	- XST_SUCCESS - If the eFUSE programming is successful
 * 		- XST_FAILURE - If there is a failure
 *
 *@section Implementation
 ******************************************************************************/
int XNvm_EfuseWrite(XNvm_ClientInstance *InstancePtr, const u64 DataAddr)
{
	volatile int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_EfuseDataAddr *EfuseData = (XNvm_EfuseDataAddr *)DataAddr;
	XNvm_EfuseAesKeys *AesKeys = (XNvm_EfuseAesKeys *)EfuseData->AesKeyAddr;
	XNvm_EfusePpkHash *EfusePpk = (XNvm_EfusePpkHash *)EfuseData->PpkHashAddr;
	XNvm_EfuseIvs *Ivs = (XNvm_EfuseIvs *)EfuseData->IvAddr;

    /**
	 * 	Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

    /**
	 *  @{ validates AES,UsrKey,PPK0,PPK1,PPK2,IVs and Sends an IPI request to the PLM by using the XNvm_EfuseWrite CDO command.
	 *     Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *     If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	if (AesKeys->PrgmAesKey == TRUE) {
		Status = XNvm_EfuseValidateAesKeyWriteReq(XNVM_EFUSE_AES_KEY);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_AesKeyWriteCdo *KeyWrCdo = (XNvm_AesKeyWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteKeyCmd(KeyWrCdo, XNVM_EFUSE_AES_KEY,  (u32)(UINTPTR)(AesKeys->AesKey),
			(u32)((UINTPTR)(AesKeys->AesKey) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32*)KeyWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "AES Key write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (AesKeys->PrgmUserKey0 == TRUE) {
		Status = XNvm_EfuseValidateAesKeyWriteReq(XNVM_EFUSE_USER_KEY_0);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_AesKeyWriteCdo *KeyWrCdo = (XNvm_AesKeyWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteKeyCmd(KeyWrCdo, XNVM_EFUSE_USER_KEY_0, (u32)(UINTPTR)(AesKeys->UserKey0),
			(u32)((UINTPTR)(AesKeys->UserKey0) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)KeyWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "USER Key0 write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (AesKeys->PrgmUserKey1 == TRUE) {
		Status = XNvm_EfuseValidateAesKeyWriteReq(XNVM_EFUSE_USER_KEY_1);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_AesKeyWriteCdo *KeyWrCdo = (XNvm_AesKeyWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteKeyCmd(KeyWrCdo, XNVM_EFUSE_USER_KEY_1, (u32)(UINTPTR)(AesKeys->UserKey1),
			(u32)((UINTPTR)(AesKeys->UserKey1) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)KeyWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "User Key1 write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (EfusePpk->PrgmPpk0Hash == TRUE) {
		Status = XNvm_EfuseValidatePpkHashWriteReq(XNVM_EFUSE_PPK0);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_PpkWriteCdo *PpkWrCdo = (XNvm_PpkWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWritePpkCmd(PpkWrCdo, XNVM_EFUSE_PPK0, (u32)(UINTPTR)(EfusePpk->Ppk0Hash),
			(u32)((UINTPTR)(EfusePpk->Ppk0Hash) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)PpkWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "PPK0 hash write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (EfusePpk->PrgmPpk1Hash == TRUE) {
		Status = XNvm_EfuseValidatePpkHashWriteReq(XNVM_EFUSE_PPK1);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_PpkWriteCdo *PpkWrCdo = (XNvm_PpkWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWritePpkCmd(PpkWrCdo, XNVM_EFUSE_PPK1, (u32)(UINTPTR)(EfusePpk->Ppk1Hash),
			(u32)((UINTPTR)(EfusePpk->Ppk1Hash) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)PpkWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "PPK1 hash write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (EfusePpk->PrgmPpk2Hash == TRUE) {
		Status = XNvm_EfuseValidatePpkHashWriteReq(XNVM_EFUSE_PPK2);
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_PpkWriteCdo *PpkWrCdo = (XNvm_PpkWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWritePpkCmd(PpkWrCdo, XNVM_EFUSE_PPK2, (u32)(UINTPTR)(EfusePpk->Ppk2Hash),
			(u32)((UINTPTR)(EfusePpk->Ppk2Hash) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)PpkWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "PPK2 hash write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	Status = XNvm_EfuseWriteIVs(InstancePtr, (u64)(UINTPTR)Ivs, FALSE);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Payload[0U] =  Header(0U, (u32)XNVM_API_ID_EFUSE_RELOAD_N_PRGM_PROT_BITS);
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, Payload, XNVM_PAYLOAD_LEN_1U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to program IV as requested by the user
 *
 * @param	InstancePtr	Pointer to the client instance
 * @param	IvAddr		Address of the IV to be programmed
 * @param	EnvDisFlag	Environmental monitoring flag set by the user.
 *                      when set to true it will not check for voltage
 *                      and temparature limits.
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseWriteIVs(XNvm_ClientInstance *InstancePtr, const u64 IvAddr, const u32 EnvDisFlag)
{
	volatile int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_EfuseIvs *Ivs = NULL;
	u32 Size;
	u32 TotalSize = sizeof(XNvm_EfuseIvs);

	if (EnvDisFlag != TRUE) {
                //TODO Temp and Voltage checks
        }

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

    /**
	 *  @{ Get shared memory allocated by the user using XMailbox_GetSharedMem API to store data structure in the user memory,
	 *     Perform validation on the size of the shared memory, if size is less than the total size XST_FAILURE is returned
	 */
	Size = XMailbox_GetSharedMem(InstancePtr->MailboxPtr, (u64**)(UINTPTR)&Ivs);

	if ((Ivs == NULL) || (Size < TotalSize)) {
		goto END;
	}

	Status = Xil_SMemSet(Ivs, TotalSize, 0U, TotalSize);
	if (Status != XST_SUCCESS) {
		goto END;
	}

    /**
	 *  Fill the EfuseData structure with the IV address, Environmental disable flag and remaining as NULL
	 */
	Ivs = (XNvm_EfuseIvs *)IvAddr;

	Xil_DCacheFlushRange((UINTPTR)Ivs, TotalSize);

    /**
	 *  @{ Send an IPI request to the PLM by using the XNvm_EfuseWrite CDO command.
     *     Wait for IPI response from PLM  with a default timeout of 300 seconds
	 */
	if (Ivs->PrgmMetaHeaderIv == TRUE) {
		Status = XNvm_EfuseValidateIvWriteReq(XNVM_EFUSE_META_HEADER_IV_RANGE, (XNvm_Iv*)(UINTPTR)(Ivs->MetaHeaderIv));
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_IvWriteCdo *IvWrCdo = (XNvm_IvWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteIvCmd(IvWrCdo, XNVM_EFUSE_META_HEADER_IV_RANGE, (u32)(UINTPTR)(Ivs->MetaHeaderIv),
			(u32)((UINTPTR)(Ivs->MetaHeaderIv) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)IvWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "Metaheader IV write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (Ivs->PrgmBlkObfusIv == TRUE) {
		Status = XNvm_EfuseValidateIvWriteReq(XNVM_EFUSE_BLACK_IV, (XNvm_Iv*)(UINTPTR)(Ivs->BlkObfusIv));
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_IvWriteCdo *IvWrCdo = (XNvm_IvWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteIvCmd(IvWrCdo, XNVM_EFUSE_BLACK_IV, (u32)(UINTPTR)(Ivs->BlkObfusIv),
			(u32)((UINTPTR)(Ivs->BlkObfusIv) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)IvWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "Black IV write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (Ivs->PrgmPlmIv == TRUE) {
		Status = XNvm_EfuseValidateIvWriteReq(XNVM_EFUSE_PLM_IV_RANGE, (XNvm_Iv*)(UINTPTR)(Ivs->PlmIv));
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_IvWriteCdo *IvWrCdo = (XNvm_IvWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteIvCmd(IvWrCdo, XNVM_EFUSE_PLM_IV_RANGE, (u32)(UINTPTR)(Ivs->PlmIv),
			(u32)((UINTPTR)(Ivs->PlmIv) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)IvWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "PLM IV write failed;"
				"Error Code = %x\r\n", Status);
			goto END;
		}
	}

	if (Ivs->PrgmDataPartitionIv == TRUE) {
		Status = XNvm_EfuseValidateIvWriteReq(XNVM_EFUSE_DATA_PARTITION_IV_RANGE, (XNvm_Iv*)(UINTPTR)(Ivs->DataPartitionIv));
		if (Status != XST_SUCCESS) {
			goto END;
		}

		XNvm_IvWriteCdo *IvWrCdo = (XNvm_IvWriteCdo *)(UINTPTR)Payload;
		XNvm_EfuseCreateWriteIvCmd(IvWrCdo, XNVM_EFUSE_DATA_PARTITION_IV_RANGE, (u32)(UINTPTR)(Ivs->DataPartitionIv),
			(u32)((UINTPTR)(Ivs->DataPartitionIv) >> XNVM_ADDR_HIGH_SHIFT));

		Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)IvWrCdo, XNVM_PAYLOAD_LEN_4U);
		if (Status != XST_SUCCESS) {
			XNvm_Printf(XNVM_DEBUG_GENERAL, "Data Partition IV write failed;"
				"Error Code = %x\r\n", Status);
		}
	}

	Payload[0U] =  Header(0U, (u32)XNVM_API_ID_EFUSE_RELOAD_N_PRGM_PROT_BITS);
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, Payload, XNVM_PAYLOAD_LEN_1U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to program Secure Control Bits
 * 		requested by the user
 *
 * @param	InstancePtr	Pointer to the client instance
 * @param	SecCtrlBits	Value of Secure Control  Bits to be programmed
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseWriteSecCtrlBits(XNvm_ClientInstance *InstancePtr, u32 SecCtrlBits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_SecCtrlBitsWriteCdo *SecCtrlBitsWrCdo = (XNvm_SecCtrlBitsWriteCdo *)(UINTPTR)Payload;

	SecCtrlBitsWrCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_WRITE_SEC_CTRL_BITS);
	SecCtrlBitsWrCdo->Pload.EnvMonitorDis = FALSE;
	SecCtrlBitsWrCdo->Pload.SecCtrlBits = SecCtrlBits;

    /**
	 *  @{ Send an IPI request to the PLM by using the XNvm_EfuseWrite CDO command.
     *     Wait for IPI response from PLM  with a default timeout of 300 seconds
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)SecCtrlBitsWrCdo, XNVM_PAYLOAD_LEN_3U);
	if (Status != XST_SUCCESS) {
		XNvm_Printf(XNVM_DEBUG_GENERAL, "Secure Control Bits write failed;"
			"Error Code = %x\r\n", Status);
		goto END;
	}

	Payload[0U] =  Header(0U, (u32)XNVM_API_ID_EFUSE_RELOAD_N_PRGM_PROT_BITS);
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, Payload, XNVM_PAYLOAD_LEN_1U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to program Puf helper data
 * 		requested by the user
 *
 * @param	InstancePtr	Pointer to the client instance
 * @param	PufHdAddr	Address of the XNvm_EfusePufHdAddr structure
 * 				where the user provided helper data to be programmed
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseWritePuf(XNvm_ClientInstance *InstancePtr, const u64 PufHdAddr)
{
	volatile int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_EfusePufHdAddr *EfusePuf = (XNvm_EfusePufHdAddr *)PufHdAddr;

    /**
	 *	Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_PufWriteCdo *PufWrCdo = (XNvm_PufWriteCdo *)(UINTPTR)Payload;
	XNvm_EfuseCreateWritePufCmd(PufWrCdo, (u32)(UINTPTR)EfusePuf,
			(u32)((UINTPTR)EfusePuf >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the XNvm_EfuseWritePuf CDO command.
     *	  Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response

	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32*)PufWrCdo, XNVM_PAYLOAD_LEN_3U);
	if (Status != XST_SUCCESS) {
		XNvm_Printf(XNVM_DEBUG_GENERAL, "Writing PUF data failed;"
			"Error Code = %x\r\n", Status);
		goto END;
	}

	Payload[0U] =  Header(0U, (u32)XNVM_API_ID_EFUSE_RELOAD_N_PRGM_PROT_BITS);
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, Payload, XNVM_PAYLOAD_LEN_1U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read Puf data from the eFUSE cache.
 *
 * @param	InstancePtr	Pointer to the client instance
 * @param	PufHdAddr	Address of the XNvm_EfusePufHdAddr structure
 * 				where the user provided helper data to be programmed
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 			- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadPuf(XNvm_ClientInstance *InstancePtr, u64 PufHdAddr)
{
	volatile int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	XNvm_EfusePufHdAddr *EfusePuf = (XNvm_EfusePufHdAddr *)PufHdAddr;
	u32 ReadPufHd[XNVM_PUF_FORMATTED_SYN_DATA_LEN_IN_WORDS] = {0U};
	u32 ReadChash = 0U;
	u32 ReadAux = 0U;
	u32 ReadRoSwap = 0U;

	/**
     *	Read helper data
	 */
	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_PUF_SYN_DATA_OFFSET, XNVM_PUF_FORMATTED_SYN_DATA_LEN_IN_WORDS,
		(u32)(UINTPTR)ReadPufHd, (u32)((UINTPTR)(ReadPufHd) >> XNVM_ADDR_HIGH_SHIFT));

	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	/**
     *	Read Chash
	 */
	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_PUF_CHASH_OFFSET, 1U, (u32)(UINTPTR)&ReadChash,
		(u32)((UINTPTR)(&ReadChash) >> XNVM_ADDR_HIGH_SHIFT));

	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Xil_DCacheInvalidateRange((UINTPTR)&ReadChash, XNVM_WORD_LEN);

	/**
     *	Read Aux
	 */
	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_PUF_ECC_CTRL_OFFSET, 1U, (u32)(UINTPTR)&ReadAux,
		(u32)((UINTPTR)(&ReadAux) >> XNVM_ADDR_HIGH_SHIFT));

	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Xil_DCacheInvalidateRange((UINTPTR)&ReadAux, XNVM_WORD_LEN);

	/**
     *	Read RO Swap
	 */
	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CAHCE_PUF_RO_SWAP_OFFSET, 1U, (u32)(UINTPTR)&ReadRoSwap,
		(u32)((UINTPTR)(&ReadRoSwap) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the XNvm_EfuseReadPuf CDO command.
	 *     Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *     If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Xil_DCacheInvalidateRange((UINTPTR)&ReadRoSwap, XNVM_WORD_LEN);

	Status = Xil_SMemCpy(EfusePuf->EfuseSynData, XNVM_PUF_FORMATTED_SYN_DATA_LEN_IN_WORDS, ReadPufHd,
		XNVM_PUF_FORMATTED_SYN_DATA_LEN_IN_WORDS, XNVM_PUF_FORMATTED_SYN_DATA_LEN_IN_WORDS);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	EfusePuf->Chash = ReadChash;
	EfusePuf->Aux = ReadAux & XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_ECC_23_0_MASK;
	EfusePuf->RoSwap = ReadRoSwap;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read IV eFuses
 * 		requested by the user
 *
 * @param	InstancePtr 	Pointer to the client instance
 * @param	IvAddr		Address of the output buffer to store the
 * 				IV eFuse data
 * @param	IvType		Type of the IV to read out
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadIv(XNvm_ClientInstance *InstancePtr, const u64 IvAddr,
	const XNvm_IvType IvType)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo = (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u16 StartOffset = 0U;

    /**
     * Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
     */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	switch(IvType) {
		case XNVM_EFUSE_META_HEADER_IV_RANGE:
			StartOffset = XNVM_EFUSE_CACHE_METAHEADER_IV_RANGE_0_OFFSET;
			break;
		case XNVM_EFUSE_BLACK_IV:
			StartOffset = XNVM_EFUSE_CACHE_BLACK_IV_0_OFFSET;
			break;
		case XNVM_EFUSE_PLM_IV_RANGE:
			StartOffset = XNVM_EFUSE_CACHE_PLM_IV_RANGE_0_OFFSET;
			break;
		case XNVM_EFUSE_DATA_PARTITION_IV_RANGE:
			StartOffset = XNVM_EFUSE_CACHE_DATA_PARTITION_IV_RANGE_0_OFFSET;
			break;
		default:
			Status =  (int)XST_INVALID_PARAM;
			break;
	}

	if (Status == XST_INVALID_PARAM) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, StartOffset, XNVM_EFUSE_IV_LEN_IN_WORDS, (u32)IvAddr,
		(u32)(IvAddr >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 *  @{ Send an IPI request to the PLM by using the XNvm_EfuseReadIv CDO command.
	 *     Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *     If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read Revocation ID eFuses
 * 		requested by the user
 *
 * @param	InstancePtr 	Pointer to the client instance
 * @param	RevokeIdAddr	Address of the output buffer to store the
 * 				Revocation ID eFuse data
 * @param 	RevokeIdNum	Revocation ID to be read out
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadRevocationId(XNvm_ClientInstance *InstancePtr, const u64 RevokeIdAddr,
			const XNvm_RevocationId RevokeIdNum)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u16 StartOffset = XNVM_EFUSE_CACHE_REVOCATION_ID_0_OFFSET + ((u16)RevokeIdNum * XNVM_WORD_LEN);

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, StartOffset, 1U, (u32)RevokeIdAddr,
		(u32)(RevokeIdAddr >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the XNvm_EfuseReadRevocationId CDO command.
     *	  Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read User eFuses
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	UserFuseAddr	Address of the output buffer to store the
 * 				User eFuse data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadUserFuses(XNvm_ClientInstance *InstancePtr, u64 UserFuseAddr)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	XNvm_EfuseUserDataAddr *UserFuseData = (XNvm_EfuseUserDataAddr *)UserFuseAddr;
	u16 StartOffset = XNVM_EFUSE_CACHE_USER_FUSE_START_OFFSET + UserFuseData->StartUserFuseNum * XNVM_WORD_LEN;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, StartOffset, UserFuseData->NumOfUserFuses, (u32)(UINTPTR)UserFuseData->UserFuseDataAddr,
		(u32)((UINTPTR)(UserFuseData->UserFuseDataAddr) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the XNvm_EfuseReadUserFuses CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read MiscCtrlBits
 * 		requested by the user
 *
 * @param	InstancePtr 	Pointer to the client instance
 * @param	MiscCtrlBits	Address of the output buffer to store the
 * 				MiscCtrlBits eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadMiscCtrlBits(XNvm_ClientInstance *InstancePtr, const u64 MiscCtrlBits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u32 ReadReg;
	XNvm_EfuseMiscCtrlBits *MiscCtrlBitsData = (XNvm_EfuseMiscCtrlBits *)MiscCtrlBits;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_MISC_CTRL_OFFSET, 1U, (u32)(UINTPTR)&ReadReg,
		(u32)((UINTPTR)(&ReadReg) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadMiscCtrlBits CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	MiscCtrlBitsData->GlitchDetHaltBootEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_SHIFT);
	MiscCtrlBitsData->GlitchDetRomMonitorEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_SHIFT);
	MiscCtrlBitsData->HaltBootError =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_SHIFT);
	MiscCtrlBitsData->HaltBootEnv =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_SHIFT);
	MiscCtrlBitsData->CryptoKatEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_SHIFT);
	MiscCtrlBitsData->LbistEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_SHIFT);
	MiscCtrlBitsData->SafetyMissionEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_SHIFT);
	MiscCtrlBitsData->Ppk0Invalid =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_SHIFT);
	MiscCtrlBitsData->Ppk1Invalid =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_SHIFT);
	MiscCtrlBitsData->Ppk2Invalid =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_MASK) >>
		XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_SHIFT);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read SecCtrlBits
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	SecCtrlBits	Address of the output buffer to store the
 * 				SecCtrlBits eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadSecCtrlBits(XNvm_ClientInstance *InstancePtr, const u64 SecCtrlBits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u32 ReadReg;
	XNvm_EfuseSecCtrlBits *SecCtrlBitsData = (XNvm_EfuseSecCtrlBits *)SecCtrlBits;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_SECURITY_CONTROL_OFFSET, 1U, (u32)(UINTPTR)&ReadReg ,
		(u32)((UINTPTR)(&ReadReg) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadSecCtrlBits CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	SecCtrlBitsData->AesDis =
		(u8)(ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_DIS_MASK);
	SecCtrlBitsData->JtagErrOutDis =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_ERROUT_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_ERROR_OUT_DIS_SHIFT);
	SecCtrlBitsData->JtagDis =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_SHIFT);
	SecCtrlBitsData->Ppk0WrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_SHIFT);
	SecCtrlBitsData->Ppk1WrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_SHIFT);
	SecCtrlBitsData->Ppk2WrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_SHIFT);
	SecCtrlBitsData->AesCrcLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_SHIFT);
	SecCtrlBitsData->AesWrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_SHIFT);
	SecCtrlBitsData->UserKey0CrcLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_0_SHIFT);
	SecCtrlBitsData->UserKey0WrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_SHIFT);
	SecCtrlBitsData->UserKey1CrcLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_0_SHIFT);
	SecCtrlBitsData->UserKey1WrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_SHIFT);
	SecCtrlBitsData->SecDbgDis =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AUTH_JTAG_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AUTH_JTAG_DIS_1_0_SHIFT);
	SecCtrlBitsData->SecLockDbgDis =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AUTH_JTAG_LOCK_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_AUTH_JTAG_LOCK_DIS_1_0_SHIFT);
	SecCtrlBitsData->BootEnvWrLk =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_SHIFT);
	SecCtrlBitsData->RegInitDis =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_SHIFT);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read SecMisc1Bits
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	SecMisc1Bits	Address of the output buffer to store the
 * 				SecMisc1Bits eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadSecMisc1Bits(XNvm_ClientInstance *InstancePtr, const u64 SecMisc1Bits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u32 ReadReg;
	XNvm_EfuseSecMisc1Bits *SecMisc1BitsData = (XNvm_EfuseSecMisc1Bits *)SecMisc1Bits;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_SEC_MISC1_OFFSET, 1U, (u32)(UINTPTR)&ReadReg ,
		(u32)((UINTPTR)(&ReadReg) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadSecMisc1Bits CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	SecMisc1BitsData->LpdMbistEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_MBIST_EN_2_0_MASK) >>
		XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_MBIST_EN_2_0_SHIFT);
	SecMisc1BitsData->PmcMbistEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SEC_MISC_1_PMC_MBIST_EN_2_0_MASK) >>
		XNVM_EFUSE_CACHE_SEC_MISC_1_PMC_MBIST_EN_2_0_SHIFT);
	SecMisc1BitsData->LpdNocScEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_NOC_SC_EN_2_0_MASK) >>
		XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_NOC_SC_EN_2_0_SHIFT);
	SecMisc1BitsData->SysmonVoltMonEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_VOLT_MON_EN_1_0_MASK) >>
		XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_VOLT_MON_EN_1_0_SHIFT);
	SecMisc1BitsData->SysmonTempMonEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_TEMP_MON_EN_1_0_MASK) >>
		XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_TEMP_MON_EN_1_0_SHIFT);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read BootEnvCtrlBits
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	BootEnvCtrlBits	Address of the output buffer to store the
 * 				BootEnvCtrlBits eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadBootEnvCtrlBits(XNvm_ClientInstance *InstancePtr, const u64 BootEnvCtrlBits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u32 ReadReg;
	XNvm_EfuseBootEnvCtrlBits *BootEnvCtrlBitsData = (XNvm_EfuseBootEnvCtrlBits *)BootEnvCtrlBits;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_OFFSET, 1U, (u32)(UINTPTR)&ReadReg ,
		(u32)((UINTPTR)(&ReadReg) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadBootEnvCtrlBits CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	BootEnvCtrlBitsData->SysmonTempEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_EN_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_EN_SHIFT);
	BootEnvCtrlBitsData->SysmonVoltEn =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_EN_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_EN_SHIFT);
	BootEnvCtrlBitsData->SysmonTempHot =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_HOT_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_HOT_SHIFT);
	BootEnvCtrlBitsData->SysmonVoltPmc =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PMC_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PMC_SHIFT);
	BootEnvCtrlBitsData->SysmonVoltPslp =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PSLP_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PSLP_SHIFT);
	BootEnvCtrlBitsData->SysmonVoltSoc =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_SOC_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_SOC_SHIFT);
	BootEnvCtrlBitsData->SysmonTempCold =
		(u8)((ReadReg &
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_COLD_MASK) >>
		XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_COLD_SHIFT);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read PufSecCtrlBits
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	PufSecCtrlBits	Address of the output buffer to store the
 * 				PufSecCtrlBits eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadPufSecCtrlBits(XNvm_ClientInstance *InstancePtr, const u64 PufSecCtrlBits)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u32 ReadSecurityCtrlReg;
	u32 ReadEccCtrlReg;
	XNvm_EfusePufSecCtrlBits *PufSecCtrlBitsData = (XNvm_EfusePufSecCtrlBits *)PufSecCtrlBits;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_SECURITY_CONTROL_OFFSET, 1U, (u32)(UINTPTR)&ReadSecurityCtrlReg,
		(u32)((UINTPTR)(&ReadSecurityCtrlReg) >> XNVM_ADDR_HIGH_SHIFT));

	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_PUF_ECC_CTRL_OFFSET, 1U, (u32)(UINTPTR)&ReadEccCtrlReg,
		(u32)((UINTPTR)(&ReadEccCtrlReg) >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadPufSecCtrlBits CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	PufSecCtrlBitsData->PufRegenDis =
		(u8)((ReadEccCtrlReg &
		XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_REGEN_DIS_MASK) >>
		XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_REGEN_DIS_SHIFT);
	PufSecCtrlBitsData->PufHdInvalid =
		(u8)((ReadEccCtrlReg &
		XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_HD_INVLD_MASK) >>
		XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_HD_INVLD_SHIFT);
	PufSecCtrlBitsData->PufTest2Dis =
		(u8)((ReadSecurityCtrlReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_SHIFT);
	PufSecCtrlBitsData->PufDis =
		(u8)((ReadSecurityCtrlReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_SHIFT);
	PufSecCtrlBitsData->PufSynLk =
		(u8)((ReadSecurityCtrlReg &
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_MASK) >>
		XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_SHIFT);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read OffChip ID eFuses
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	OffChipIdAddr	Address of the output buffer to store the
 * 				OffChip ID eFuse data
 * @param	OffChipIdNum	OffChip ID number to be read out
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadOffchipRevokeId(XNvm_ClientInstance *InstancePtr, const u64 OffChipIdAddr,
	const XNvm_OffchipId OffChipIdNum)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u16 StartOffset = XNVM_EFUSE_CACHE_OFFCHIP_REVOKE_0_OFFSET + ((u16)OffChipIdNum * XNVM_WORD_LEN);

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, StartOffset, 1U, (u32)OffChipIdAddr ,(u32)(OffChipIdAddr >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadOffchipRevokeId CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read PpkHash
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	PpkHashAddr	Address of the output buffer to store the
 * 				PpkHashAddr eFuses data
 * @param 	PpkHashType	Type of the PpkHash to be read out
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadPpkHash(XNvm_ClientInstance *InstancePtr, const u64 PpkHashAddr, const XNvm_PpkType PpkHashType)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;
	u16 StartOffset = 0U;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	switch(PpkHashType) {
		case XNVM_EFUSE_PPK0:
			StartOffset = XNVM_EFUSE_CACHE_PPK0_HASH_0_OFFSET;
			break;
		case XNVM_EFUSE_PPK1:
			StartOffset = XNVM_EFUSE_CACHE_PPK0_HASH_1_OFFSET;
			break;
		case XNVM_EFUSE_PPK2:
			StartOffset = XNVM_EFUSE_CACHE_PPK0_HASH_2_OFFSET;
			break;
		default:
			Status = (int)XST_INVALID_PARAM;
			break;
	}

	if (Status == XST_INVALID_PARAM) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, StartOffset, XNVM_EFUSE_PPK_HASH_LEN_IN_WORDS,
		(u32)PpkHashAddr ,(u32)(PpkHashAddr >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadPpkHash CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read DecEfuseOnly eFuses
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	DecOnlyAddr	Address of the output buffer to store the
 * 				DecEfuseOnly eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadDecOnly(XNvm_ClientInstance *InstancePtr, const u64 DecOnlyAddr)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_DEC_ONLY_OFFSET, 1U, (u32)DecOnlyAddr,
		(u32)(DecOnlyAddr >> XNVM_ADDR_HIGH_SHIFT));

    /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadDecOnly CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function sends IPI request to read DNA eFuses
 * 		requested by the user
 *
 * @param	InstancePtr Pointer to the client instance
 * @param	DnaAddr		Address of the output buffer to store the
 * 				DNA eFuses data
 *
 * @return	- XST_SUCCESS - If the read is successful
 * 		- XST_FAILURE - If there is a failure
 *
 ******************************************************************************/
int XNvm_EfuseReadDna(XNvm_ClientInstance *InstancePtr, const u64 DnaAddr)
{
	int Status = XST_FAILURE;
	u32 Payload[XNVM_MAX_PAYLOAD_LEN];
	XNvm_RdCacheCdo* RdCacheCdo =  (XNvm_RdCacheCdo*)(UINTPTR)Payload;

    /**
	 *  Perform input parameter validation on InstancePtr. Return XST_FAILURE If input parameters are invalid
	 */
	if ((InstancePtr == NULL) || (InstancePtr->MailboxPtr == NULL)) {
		goto END;
	}

	XNvm_EfuseCreateReadEfuseCacheCmd(RdCacheCdo, XNVM_EFUSE_CACHE_DNA_OFFSET, XNVM_EFUSE_DNA_IN_WORDS, (u32)DnaAddr,
		(u32)(DnaAddr >> XNVM_ADDR_HIGH_SHIFT));

     /**
	 * @{ Send an IPI request to the PLM by using the EfuseReadDna CDO command.
	 *    Wait for IPI response from PLM  with a default timeout of 300 seconds.
	 *    If the timeout exceeds then error is returned otherwise it returns the status of the IPI response
	 */
	Status = XNvm_ProcessMailbox(InstancePtr->MailboxPtr, (u32 *)RdCacheCdo, XNVM_PAYLOAD_LEN_4U);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function creates payload for Write PUF CDO.
 *
 * @param	PufWrCdo	Pointer to the Write PUF Key CDO
 * @param	AddrLow		Lower Address of the PUF data buffer
 * @param	AddrHigh	Higher Address of the PUF data buffer
 *
 ******************************************************************************/
static void XNvm_EfuseCreateWritePufCmd(XNvm_PufWriteCdo* PufWrCdo, u32 AddrLow, u32 AddrHigh)
{
	PufWrCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_WRITE_PUF);
	PufWrCdo->Pload.AddrLow = AddrLow;
	PufWrCdo->Pload.AddrHigh = AddrHigh;
}

/*****************************************************************************/
/**
 * @brief	This function creates payload for Write AES Key CDO.
 *
 * @param	AesKeyWrCdo	Pointer to the Write AES Key CDO
 * @param	KeyType		Type of the Key
 * @param	AddrLow		Lower Address of the key buffer
 * @param	AddrHigh	Higher Address of the key buffer
 *
 ******************************************************************************/
static void XNvm_EfuseCreateWriteKeyCmd(XNvm_AesKeyWriteCdo* AesKeyWrCdo, XNvm_AesKeyType KeyType, u32 AddrLow, u32 AddrHigh)
{
	AesKeyWrCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_WRITE_AES_KEY);
	AesKeyWrCdo->Pload.KeyType = KeyType;
	AesKeyWrCdo->Pload.AddrLow = AddrLow;
	AesKeyWrCdo->Pload.AddrHigh = AddrHigh;
}

/*****************************************************************************/
/**
 * @brief	This function creates payload for Write PPK Hash CDO.
 *
 * @param	PpkWrCdo	Pointer to the Write PPK Hash CDO
 * @param	PpkType		Type of PPK
 * @param	AddrLow		Lower Address of the PPK Hash buffer
 * @param	AddrHigh	Higher Address of the PPK Hash buffer
 *
 ******************************************************************************/
static void XNvm_EfuseCreateWritePpkCmd(XNvm_PpkWriteCdo* PpkWrCdo, XNvm_PpkType PpkType, u32 AddrLow, u32 AddrHigh)
{
	PpkWrCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_WRITE_PPK_HASH);
	PpkWrCdo->Pload.PpkType = PpkType;
	PpkWrCdo->Pload.AddrLow = AddrLow;
	PpkWrCdo->Pload.AddrHigh = AddrHigh;
}

/*****************************************************************************/
/**
 * @brief	This function creates payload for Write IV CDO.
 *
 * @param	IvWrCdo		Pointer to the Write IV CDO
 * @param	IvType		Type of IV
 * @param	AddrLow		Lower Address of the IV buffer
 * @param	AddrHigh	Higher Address of the IV buffer
 *
 ******************************************************************************/
static void XNvm_EfuseCreateWriteIvCmd(XNvm_IvWriteCdo* IvWrCdo, XNvm_IvType IvType, u32 AddrLow, u32 AddrHigh)
{
	IvWrCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_WRITE_IV);
	IvWrCdo->Pload.IvType = IvType;
	IvWrCdo->Pload.AddrLow = AddrLow;
	IvWrCdo->Pload.AddrHigh = AddrHigh;
}

/*****************************************************************************/
/**
 * @brief	This function creates payload for Read Efuse Cache CDO.
 *
 * @param	RdCacheCdo	Pointer to the Read eFUSE Cache CDO
 * @param	StartOffset	Start offset of cache register
 * @param	RegCount	Number of registers to be read
 * @param	AddrLow		Lower Address of the output buffer
 * @param	AddrHigh	Higher Address of the output buffer
 *
 ******************************************************************************/
static void XNvm_EfuseCreateReadEfuseCacheCmd(XNvm_RdCacheCdo* RdCacheCdo, u16 StartOffset, u8 RegCount, u32 AddrLow, u32 AddrHigh)
{
	RdCacheCdo->CdoHdr = Header(0U, (u32)XNVM_API_ID_EFUSE_READ_CACHE);
	RdCacheCdo->Pload.StartOffset = StartOffset;
	RdCacheCdo->Pload.RegCount = RegCount;
	RdCacheCdo->Pload.AddrLow = AddrLow;
	RdCacheCdo->Pload.AddrHigh = AddrHigh;
}
