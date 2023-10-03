/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022-2023, Advanced Micro Devices, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file xocp_defs.h
* @addtogroup xocp_api_ids XilOcp API IDs
* @{
*
* @cond xocp_internal
* This file contains the xilocp API IDs
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.1   am   12/21/22 Initial release
*       am   01/10/23 Added XOCP_DME_NONCE_SIZE_IN_BITS macro for dme
*
* </pre>
* @note
*
* @endcond
******************************************************************************/

#ifndef XOCP_DEF_H
#define XOCP_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_cache.h"

/************************** Constant Definitions ****************************/

/***************** Macros (Inline Functions) Definitions *********************/

#ifndef XOCP_CACHE_DISABLE
	#if defined(__microblaze__)
		#define XOcp_DCacheFlushRange(SrcAddr, Len) Xil_DCacheFlushRange((UINTPTR)SrcAddr, Len)
	#else
		#define XOcp_DCacheFlushRange(SrcAddr, Len) Xil_DCacheFlushRange((INTPTR)SrcAddr, Len)
	#endif
#else
	#define XOcp_DCacheFlushRange(SrcAddr, Len) {}
#endif /**< Cache Invalidate function */

#define XOCP_API(ApiId)	((u32)ApiId)
				/**< Macro to typecast XOCP API ID */

#define XOCP_API_ID_MASK	0xFFU
				/**< Mask for API ID in Secure IPI command */

#define XOCP_EXTENDED_HASH_SIZE_IN_BYTES	(48U) /**< Extended hash buffer size in bytes */

#define XOCP_EXTENDED_HASH_SIZE_IN_BITS	(384U) /**< Extended hash buffer size in bits */

#define XOCP_DME_NONCE_SIZE_IN_BITS		(256U) /**< Nonce buffer size in bits */

/***************************** Include Files *********************************/

/************************** Constant Definitions ****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/**************************** Type Definitions *******************************/

/**
 * @name  XOcp API ids
 * @{
 */
typedef enum {
	XOCP_API_FEATURES = 0U,	/**< 0U */
	XOCP_API_EXTENDPCR,	/**< 1U */
	XOCP_API_GETPCR,	/**< 2U */
	XOCP_API_GETPCRLOG,	/**< 3U */
	XOCP_API_GENDMERESP,	/**< 4U */
	XOCP_API_DEVAKINPUT,	/**< 5U */
	XOCP_API_GETCERTUSERCFG,/**< 6U */
	XOCP_API_GETX509CERT,	/**< 7U */
	XOCP_API_ATTESTWITHDEVAK,/**< 8U */
	XOCP_API_MAX		/**< 9U */
} XOcp_ApiId;
/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* XOCP_DEF_H */