/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file xsecure_plat_defs.h
*
* This file contains the xsecure API IDs for versalnet
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 5.0   bm   07/06/22 Initial release
*       kpt  07/24/22 Added XSECURE_API_KAT and additional KAT ids
*       dc   08/26/22 Removed gaps in between the API IDs
* 5.1   kpt  01/04/22 Add macros related to KAT for external modules
*
* </pre>
* @note
*
******************************************************************************/

#ifndef XSECURE_PLAT_DEFS_H
#define XSECURE_PLAT_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

/************************** Constant Definitions ****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**< KAT macros */
#define XSECURE_KAT_HDR_LEN		(3U)
#define XSECURE_MAX_KAT_MASK_LEN	(3U)
#define XSECURE_MIN_KAT_MASK_LEN	(1U)

/**< Node ids */
#define XSECURE_DDR_0_NODE_ID		(0x18320010U)
#define XSECURE_DDR_1_NODE_ID		(0x18320011U)
#define XSECURE_DDR_2_NODE_ID		(0x18320012U)
#define XSECURE_DDR_3_NODE_ID		(0x18320013U)
#define XSECURE_DDR_4_NODE_ID		(0x18320014U)
#define XSECURE_DDR_5_NODE_ID		(0x18320015U)
#define XSECURE_DDR_6_NODE_ID		(0x18320016U)
#define XSECURE_DDR_7_NODE_ID		(0x18320017U)
#define XSECURE_HNIC_NODE_ID		(0x4230031U)
#define XSECURE_CPM5N_NODE_ID		(0x4218032U)
#define XSECURE_PCIDE_NODE_ID		(0xc410053U)
#define XSECURE_PKI_NODE_ID		(0xC410089U)

#define XSECURE_WORD_LEN		(4U)

/************************** Variable Definitions *****************************/

/**************************** Type Definitions *******************************/
/**< XilSecure API ids */
typedef enum {
	XSECURE_API_FEATURES = 0U,		/**< 0U */
	XSECURE_API_RSA_SIGN_VERIFY,		/**< 1U */
	XSECURE_API_RSA_PUBLIC_ENCRYPT,		/**< 2U */
	XSECURE_API_RSA_PRIVATE_DECRYPT,	/**< 3U */
	XSECURE_API_SHA3_UPDATE,			/**< 4U */
	XSECURE_API_ELLIPTIC_GENERATE_KEY,	/**< 5U */
	XSECURE_API_ELLIPTIC_GENERATE_SIGN,	/**< 6U */
	XSECURE_API_ELLIPTIC_VALIDATE_KEY,	/**< 7U */
	XSECURE_API_ELLIPTIC_VERIFY_SIGN,	/**< 8U */
	XSECURE_API_AES_INIT,				/**< 9U */
	XSECURE_API_AES_OP_INIT,			/**< 10U */
	XSECURE_API_AES_UPDATE_AAD,		/**< 11U */
	XSECURE_API_AES_ENCRYPT_UPDATE,		/**< 12U */
	XSECURE_API_AES_ENCRYPT_FINAL,		/**< 13U */
	XSECURE_API_AES_DECRYPT_UPDATE,		/**< 14U */
	XSECURE_API_AES_DECRYPT_FINAL,		/**< 15U */
	XSECURE_API_AES_KEY_ZERO,		/**< 16U */
	XSECURE_API_AES_WRITE_KEY,		/**< 17U */
	XSECURE_API_AES_LOCK_USER_KEY,		/**< 18U */
	XSECURE_API_AES_KEK_DECRYPT,		/**< 19U */
	XSECURE_API_AES_SET_DPA_CM,		/**< 20U */
	XSECURE_API_KAT,			/**< 21U */
	XSECURE_API_TRNG_GENERATE,		/**< 22U */
	XSECURE_API_AES_PERFORM_OPERATION, /**< 23U */
	XSECURE_API_UPDATE_CRYPTO_STATUS,  /**< 24U */
	XSECURE_API_MAX,			/**< 25U */
} XSecure_ApiId;

/**< XilSecure KAT ids */
typedef enum {
	XSECURE_API_AES_DECRYPT_KAT = 0U,		/**< 0U */
	XSECURE_API_AES_DECRYPT_CM_KAT,			/**< 1U */
	XSECURE_API_RSA_PUB_ENC_KAT,			/**< 2U */
	XSECURE_API_ELLIPTIC_SIGN_VERIFY_KAT,	/**< 3U */
	XSECURE_API_SHA3_KAT,					/**< 4U */
	XSECURE_API_AES_ENCRYPT_KAT,			/**< 5U */
	XSECURE_API_RSA_PRIVATE_DEC_KAT,		/**< 6U */
	XSECURE_API_ELLIPTIC_SIGN_GEN_KAT,		/**< 7U */
	XSECURE_API_TRNG_KAT,					/**< 8U */
	XSECURE_API_UPDATE_KAT_STATUS,			/**< 9U */
} XSecure_KatId;

typedef enum {
	XSECURE_DDR_0_AES_GCM_KAT = 0U,
	XSECURE_DDR_0_AES_XTS_KAT,
	XSECURE_DDR_0_KDF_KAT,
	XSECURE_DDR_0_TRNG_KAT,
	XSECURE_DDR_1_AES_GCM_KAT,
	XSECURE_DDR_1_AES_XTS_KAT,
	XSECURE_DDR_1_KDF_KAT,
	XSECURE_DDR_1_TRNG_KAT,
	XSECURE_DDR_2_AES_GCM_KAT,
	XSECURE_DDR_2_AES_XTS_KAT,
	XSECURE_DDR_2_KDF_KAT,
	XSECURE_DDR_2_TRNG_KAT,
	XSECURE_DDR_3_AES_GCM_KAT,
	XSECURE_DDR_3_AES_XTS_KAT,
	XSECURE_DDR_3_KDF_KAT,
	XSECURE_DDR_3_TRNG_KAT,
	XSECURE_DDR_4_AES_GCM_KAT,
	XSECURE_DDR_4_AES_XTS_KAT,
	XSECURE_DDR_4_KDF_KAT,
	XSECURE_DDR_4_TRNG_KAT,
	XSECURE_DDR_5_AES_GCM_KAT,
	XSECURE_DDR_5_AES_XTS_KAT,
	XSECURE_DDR_5_KDF_KAT,
	XSECURE_DDR_5_TRNG_KAT,
	XSECURE_DDR_6_AES_GCM_KAT,
	XSECURE_DDR_6_AES_XTS_KAT,
	XSECURE_DDR_6_KDF_KAT,
	XSECURE_DDR_6_TRNG_KAT,
	XSECURE_DDR_7_AES_GCM_KAT,
	XSECURE_DDR_7_AES_XTS_KAT,
	XSECURE_DDR_7_KDF_KAT,
	XSECURE_DDR_7_TRNG_KAT,
} XSecure_DDRKatId;

typedef enum {
	XSECURE_HNIC_AES_GCM_ENC_KAT = 0U,
	XSECURE_HNIC_AES_GCM_DECRYPT_KAT,
	XSECURE_HNIC_AES_CM_ENC_KAT,
	XSECURE_HNIC_AES_CM_DEC_KAT,
	XSECURE_HNIC_AES_GCM_EN_CHECK,
	XSECURE_HNIC_AES_GCM_BYPASS_CHECK,
} XSecure_HnicKatId;

typedef enum {
	XSECURE_CPM5N_AES_XTS_ENC_KAT = 8U,
	XSECURE_CPM5N_AES_XTS_DEC_KAT,
	XSECURE_CPM5N_AES_CM_ENC_KAT,
	XSECURE_CPM5N_AES_CM_DEC_KAT,
	XSECURE_CPM5N_AES_XTS_EN_CHECK,
	XSECURE_CPM5N_AES_XTS_BYPASS_CHECK,
} XSecure_Cpm5nKatId;

typedef enum {
	XSECURE_PCIDE_AES_GCM_ENC_KAT = 16U,
	XSECURE_PCIDE_AES_GCM_DEC_KAT
} XSecure_PciIdeKatId;

typedef enum {
	XSECURE_PKI_ECC_P192_SIGN_GEN_KAT = 0U,
	XSECURE_PKI_ECC_P192_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P192_PWCT,
	XSECURE_PKI_ECC_P192_CM_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_P192_CM_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P192_CM_PWCT,
	XSECURE_PKI_ECC_P384_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_P384_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P384_PWCT,
	XSECURE_PKI_ECC_P384_CM_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_P384_CM_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P384_CM_PWCT,
	XSECURE_PKI_ECC_P521_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_P521_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P521_PWCT,
	XSECURE_PKI_ECC_P521_CM_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_P521_CM_SIGN_VER_KAT,
	XSECURE_PKI_ECC_P521_CM_PWCT,
	XSECURE_PKI_ECC_Secp256k1_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_Secp256k1_SIGN_VER_KAT,
	XSECURE_PKI_ECC_Secp256k1_PWCT,
	XSECURE_PKI_ECC_Secp256k1_CM_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_Secp256k1_CM_SIGN_VER_KAT,
	XSECURE_PKI_ECC_Secp256k1_CM_PWCT,
	XSECURE_PKI_ECC_fp256_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_fp256_SIGN_VER_KAT,
	XSECURE_PKI_ECC_fp256k_PWCT,
	XSECURE_PKI_ECC_fp256_CM_SIGN_GEN_KAT,
	XSECURE_PKI_ECC_fp256_CM_SIGN_VER_KAT,
	XSECURE_PKI_ECC_fp256k_CM_PWCT,
	XSECURE_PKI_EdDSA_Ed25519_SIGN_GEN_KAT,
	XSECURE_PKI_EdDSA_Ed25519_SIGN_VER_KAT,
	XSECURE_PKI_EdDSA_Ed25519_PWCT = 32U,
	XSECURE_PKI_EdDSA_Ed25519_CM_SIGN_GEN_KAT,
	XSECURE_PKI_EdDSA_Ed25519_CM_SIGN_VER_KAT,
	XSECURE_PKI_EdDSA_Ed25519_CM_PWCT,
	XSECURE_PKI_EdDSA_Ed448_SIGN_GEN_KAT,
	XSECURE_PKI_EdDSA_Ed448_SIGN_VER_KAT,
	XSECURE_PKI_EdDSA_Ed448_PWCT,
	XSECURE_PKI_EdDSA_Ed448_CM_SIGN_GEN_KAT,
	XSECURE_PKI_EdDSA_Ed448_CM_SIGN_VER_KAT,
	XSECURE_PKI_EdDSA_Ed448_CM_PWCT,
	XSECURE_PKI_ECDH_Curve25519_ENC_KAT,
	XSECURE_PKI_ECDH_Curve25519_DEC_KAT,
	XSECURE_PKI_ECDH_Curve25519_PWCT,
	XSECURE_PKI_ECDH_Curve25519_CM_ENC_KAT,
	XSECURE_PKI_ECDH_Curve25519_CM_DEC_KAT,
	XSECURE_PKI_ECDH_Curve25519_CM_PWCT,
	XSECURE_PKI_ECDH_Curve448_ENC_KAT,
	XSECURE_PKI_ECDH_Curve448_DEC_KAT,
	XSECURE_PKI_ECDH_Curve448_PWCT,
	XSECURE_PKI_ECDH_Curve448_CM_ENC_KAT,
	XSECURE_PKI_ECDH_Curve448_CM_DEC_KAT,
	XSECURE_PKI_ECDH_Curve448_CM_PWCT,
	XSECURE_PKI_RSA_2048_SIGN_GEN_KAT,
	XSECURE_PKI_RSA_2048_SIGN_VER_KAT,
	XSECURE_PKI_RSA_2048_ENC_KAT,
	XSECURE_PKI_RSA_2048_DEC_KAT,
	XSECURE_PKI_RSA_2048_PWCT,
	XSECURE_PKI_RSA_2048_CM_SIGN_GEN_KAT,
	XSECURE_PKI_RSA_2048_CM_SIGN_VER_KAT,
	XSECURE_PKI_RSA_2048_CM_ENC_KAT,
	XSECURE_PKI_RSA_2048_CM_DEC_KAT,
	XSECURE_PKI_RSA_2048_CM_PWCT,
	XSECURE_PKI_RSA_3072_SIGN_GEN_KAT = 64U,
	XSECURE_PKI_RSA_3072_SIGN_VER_KAT,
	XSECURE_PKI_RSA_3072_ENC_KAT,
	XSECURE_PKI_RSA_3072_DEC_KAT,
	XSECURE_PKI_RSA_3072_PWCT,
	XSECURE_PKI_RSA_3072_CM_SIGN_GEN_KAT,
	XSECURE_PKI_RSA_3072_CM_SIGN_VER_KAT,
	XSECURE_PKI_RSA_3072_CM_ENC_KAT,
	XSECURE_PKI_RSA_3072_CM_DEC_KAT,
	XSECURE_PKI_RSA_3072_CM_PWCT,
	XSECURE_PKI_RSA_4096_SIGN_GEN_KAT,
	XSECURE_PKI_RSA_4096_SIGN_VER_KAT,
	XSECURE_PKI_RSA_4096_ENC_KAT,
	XSECURE_PKI_RSA_4096_DEC_KAT,
	XSECURE_PKI_RSA_4096_PWCT,
	XSECURE_PKI_RSA_4096_CM_SIGN_GEN_KAT,
	XSECURE_PKI_RSA_4096_CM_SIGN_VER_KAT,
	XSECURE_PKI_RSA_4096_CM_ENC_KAT,
	XSECURE_PKI_RSA_4096_CM_DEC_KAT,
	XSECURE_PKI_RSA_4096_CM_PWCT,
	XSECURE_PKI_TRNG_HEALTH_TEST,
	XSECURE_PKI_TRNG_DRBG_KAT,
	XSECURE_PKI_SHA2_256_KAT,
	XSECURE_PKI_SHA2_384_KAT,
	XSECURE_PKI_SHA2_512_KAT,
} XSecure_PkiKatId;

typedef enum {
	XSECURE_CRYPTO_STATUS_SET = 0U,
	XSECURE_CRYPTO_STATUS_CLEAR,
}XSecure_CryptoStatusOp;

#ifdef __cplusplus
}
#endif

#endif  /* XSECURE_PLAT_DEFS_H */