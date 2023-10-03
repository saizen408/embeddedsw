/******************************************************************************
* Copyright (c) 2019 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022-2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/******************************************************************************/
/**
*
* @file versal/server/xnvm_efuse_hw.h
* @addtogroup xnvm_versal_Efuse_HW XilNvm Versal Efuse HW Reg
* @{
* This file contains NVM library eFUSE controller register definitions
*
* @cond xnvm_internal
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- ---------- --------------------------------------------------------
* 1.0   kal  08/16/2019 Initial release
* 2.0	kal  02/27/2019	Added eFuse rows, Cache offsets
* 2.1   kal  07/09/2020 Replaced hardcoded CLK_REF_FREQ to the
*                       XPAR_PSU_PSS_REF_CLK_FREQ_HZ
*	am   08/19/2020 Resolved MISRA C violations.
*	kal  09/03/2020 Fixed Security CoE review comments
*	am   10/13/2020 Resolved MISRA C violations
* 2.3   am   11/23/2020 Resolved MISRA C violations
*	kal  01/07/2021	Added new macros for BootEnvCtrl and SecurityMisc1
*			eFuse rows
*	kal  02/20/2021 Added new macros for sysmon measure registers
*   kpt  05/20/2021 Added macro XNVM_EFUSE_PUF_SYN_CACHE_READ_ROW for
*                   PUF cache read
* 2.4   kal  07/13/2021 Fixed doxygen warnings
*       har  09/16/2021 Added macro for offset for ANLG TRIM 2 register
* 3.0	kal  07/12/2022	Moved common hw definitions to xnvm_efuse_common_hw.h
* 3.1   skg  12/07/2022 Added Additional PPKs related macros
*
* </pre>
*
* @note
*
* @endcond
*******************************************************************************/

#ifndef XNVM_EFUSE_HW_H
#define XNVM_EFUSE_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Include Files *********************************/
#include "xparameters.h"

/*************************** Constant Definitions *****************************/
/**
 * @name  PMC sysmon sat0 base address
 */
/**< SYSMONPSV_SAT0 Base Address */
#define XNVM_EFUSE_SYSMONPSV_SAT0_BASEADDR		(0xF1280000U)
/** @} */

/**
 * @name  PMC sysmon sat1 base address
 */
/**< SYSMONPSV_SAT1 Base Address */
#define XNVM_EFUSE_SYSMONPSV_SAT1_BASEADDR		(0xF1290000U)
/** @} */

/**
 * @name  PMC sysmon measure0 offset
 */
/**< SYSMONPSV_SAT_MEASURE0 Offset */
#define XNVM_EFUSE_SYSMONPSV_SAT_MEASURE0_OFFSET	(0x00000524U)
/** @} */

/**
 * @name Register: EFUSE_CTRL_CFG
 */
/**< eFUSE CTRL STATUS Register Masks */
#define XNVM_EFUSE_CTRL_CFG_MARGIN_RD_MASK    		(0x00000004U)

/* access_type: ro  */
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_PASS_MASK	(0x00000800U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_DONE_MASK	(0x00000400U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_PASS_MASK	(0x00000200U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_DONE_MASK	(0x00000100U)
#define XNVM_EFUSE_CTRL_STATUS_AES_CRC_PASS_MASK		(0x00000080U)
#define XNVM_EFUSE_CTRL_STATUS_AES_CRC_DONE_MASK		(0x00000040U)
/** @} */

/**
 *  @name eFUSE Cache Register Offsets
 */
/**< eFUSE Cache Register Offsets */
#define XNVM_EFUSE_CACHE_TBITS0_SVD_OFFSET    			(0x00000000U)
#define XNVM_EFUSE_CACHE_ANLG_TRIM_2_OFFSET			(0x0000000CU)
#define XNVM_EFUSE_CACHE_ANLG_TRIM_3_OFFSET			(0x00000010U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_OFFSET			(0x00000094U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_OFFSET			(0x000000A0U)
#define XNVM_EFUSE_CACHE_PUF_CHASH_OFFSET			(0x000000A8U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_OFFSET		(0x000000ACU)
#define XNVM_EFUSE_CACHE_SECURITY_MISC_0_OFFSET			(0x000000E4U)
#define XNVM_EFUSE_CACHE_SECURITY_MISC_1_OFFSET			(0x000000E8U)
#define XNVM_EFUSE_CACHE_ANLG_TRIM_6_OFFSET			(0x000000F4U)
#define XNVM_EFUSE_CACHE_ANLG_TRIM_7_OFFSET			(0x000000F8U)
#define XNVM_EFUSE_CACHE_PPK0_HASH_0_OFFSET			(0x00000100U)
#define XNVM_EFUSE_CACHE_METAHEADER_IV_RANGE_0_OFFSET		(0x00000180U)
#define XNVM_EFUSE_CACHE_TRIM_AMS_12_OFFSET			(0x000001B0U)
/** @} */

/**
 * @name Register: EFUSE_CACHE_SECURITY_CONTROL_REG
 */
/**< eFUSE Cache Security Control Register Masks And Shifts */
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_MASK	(0xc0000000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_MASK	(0x10000000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_LOCK_DBG_DIS_MASK	(0x00600000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_DEBUG_DIS_MASK	(0x00180000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_MASK		(0x00040000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_MASK	(0x00020000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_MASK	(0x00010000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_MASK	(0x00008000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_MASK	(0x00004000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_MASK	(0x00002000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_MASK	(0x00001000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_MASK	(0x00000800U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_MASK	(0x00000600U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_MASK	(0x00000100U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_MASK	(0x00000080U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_MASK	(0x00000040U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_MASK		(0x00000004U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_ERROUT_DIS_MASK	(0x00000002U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_DIS_MASK		(0x00000001U)

#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_SHIFT	(30U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_SHIFT		(28U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_LOCK_DBG_DIS_1_0_SHIFT	(21U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_DEBUG_DIS_1_0_SHIFT	(19U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_SHIFT			(18U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_SHIFT		(17U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_SHIFT		(16U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_SHIFT		(15U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_0_SHIFT	(14U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_SHIFT		(13U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_0_SHIFT	(12U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_SHIFT		(11U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_SHIFT		(9U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_SHIFT		(8U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_SHIFT		(7U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_SHIFT		(6U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_SHIFT		(2U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_ERROR_OUT_DIS_SHIFT	(1U)
/** @} */

/**
 * @name Register: EFUSE_CACHE_PUF_ECC_CTRL
 */
/**< eFUSE CACHE PUF ECC CTRL Register Masks And Shifts */
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_REGEN_DIS_MASK	(0x80000000U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_HD_INVLD_MASK		(0x40000000U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_ECC_23_0_MASK		(0x00ffffffU)

#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_REGEN_DIS_SHIFT		(31U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_HD_INVLD_SHIFT		(30U)
/** @} */

/**
 * @name Register: EFUSE_CACHE_TBITS0_SVD
 */
/**< eFUSE Cache TBITS0_SVD Register Masks */
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_3_MASK    		(0x08000000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_2_MASK    		(0x04000000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_37_PROT_MASK    	(0x00200000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_40_PROT_MASK    	(0x00100000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_42_PROT_MASK    	(0x00080000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_58_PROT_MASK    	(0x00040000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_1_MASK		(0x00000002U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_0_MASK		(0x00000001U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_43_PROT_MASK 		(0x02000004U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_57_PROT_MASK 		(0x01000008U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_64_87_PROT_MASK		(0x00810000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_96_99_PROT_MASK		(0x00420000U)
/** @} */

/**
 * @name  Register: EFUSE_CACHE_MISC_CTRL
 */
/**< eFUSE Cache MISC CTRL Register Masks And Shifts */
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_MASK	(0xc0000000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_MASK	(0x20000000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_MASK	(0x00600000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_MASK	(0x00180000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_MASK		(0x00008000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_MASK		(0x00004000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_MASK	(0x00000100U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_MASK		(0x000000c0U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_MASK		(0x00000030U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_MASK		(0x0000000cU)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK3_INVLD_1_0_MASK		(0x00000600U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK4_INVLD_1_0_MASK		(0x00001800U)

#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_SHIFT	   	(30U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_SHIFT   		(29U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_SHIFT   		(21U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_SHIFT   		(19U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_SHIFT   		(15U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_SHIFT   			(14U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_SHIFT   		(8U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK4_INVLD_1_0_SHIFT			(11U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK3_INVLD_1_0_SHIFT			(9U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_SHIFT			(6U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_SHIFT			(4U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_SHIFT			(2U)


/** @} */

/**
 * @name  Register: EFUSE_CACHE_SECURITY_MISC_1
 */
/**< eFUSE Cache SECURITY_MISC_1 Masks And Shifts */
#define XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_MBIST_EN_2_0_MASK    	(0x00001c00U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_PMC_MBIST_EN_2_0_MASK    	(0x00000380U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_NOC_SC_EN_2_0_MASK   	(0x00000070U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_VOLT_MON_EN_1_0_MASK	(0x0000000cU)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_TEMP_MON_EN_1_0_MASK	(0x00000003U)

#define XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_MBIST_EN_2_0_SHIFT   		(10U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_PMC_MBIST_EN_2_0_SHIFT   		(7U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_LPD_NOC_SC_EN_2_0_SHIFT  		(4U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_VOLT_MON_EN_1_0_SHIFT   	(2U)
#define XNVM_EFUSE_CACHE_SEC_MISC_1_SYSMON_TEMP_MON_EN_1_0_SHIFT   	(0U)
/** @} */

/**
 * @name  Register: EFUSE_CACHE_BOOT_ENV_CTRL
 */
/**< eFUSE Cache BOOT_ENV_CTRL Masks And Shifts */
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_EN_MASK	(0x00200000U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_EN_MASK	(0x00100000U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_HOT_MASK	(0x00060000U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PMC_MASK	(0x00003000U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PSLP_MASK	(0x00000c00U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_SOC_MASK	(0x00000200U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_COLD_MASK	(0x00000003U)

#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_EN_SHIFT		(21U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_EN_SHIFT		(20U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_HOT_SHIFT		(17U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PMC_SHIFT		(12U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_PSLP_SHIFT		(10U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_VOLT_SOC_SHIFT		(9U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_SYSMON_TEMP_COLD_SHIFT		(0U)
/** @} */

/**
 * @name Register: SYSMON_SAT_REG
 */
/**< SYSMON SAT0/1 Register Masks */
#define XNVM_EFUSE_SYSMON_SAT_ADDR_ID_MASK              (0x3fc00000U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_MODE_MASK          (0x00300000U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_AMUX_CTRL_MASK     (0x000f0000U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_ABUS_SW1_MASK      (0x0000ff00U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_ABUS_SW0_MASK      (0x000000ffU)

#define XNVM_EFUSE_SYSMON_SAT_ADDR_ID_SHIFT   		(22U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_MODE_SHIFT   	(20U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_AMUX_CTRL_SHIFT   	(16U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_ABUS_SW1_SHIFT   	(8U)
#define XNVM_EFUSE_SYSMON_SAT_CONFIG_ABUS_SW0_SHIFT   	(0U)
/** @} */

/**
 * @name  EFUSE row numbers
 */
/**< EFUSE Row numbers */
#define XNVM_EFUSE_TBITS_XILINX_CTRL_ROW		(0U)
#define XNM_EFUSE_GLITCH_ANLG_TRIM_3			(4U)
#define XNVM_EFUSE_DNA_START_ROW			(8U)
#define XNVM_EFUSE_AES_KEY_START_ROW			(12U)
#define XNVM_EFUSE_USER_KEY_0_START_ROW			(20U)
#define XNVM_EFUSE_USER_KEY_1_START_ROW			(28U)
#define XNVM_EFUSE_USER_KEY_1_END_ROW			(35U)
#define XNVM_EFUSE_BOOT_ENV_CTRL_ROW			(37U)
#define XNVM_EFUSE_MISC_CTRL_ROW			(40U)
#define XNVM_EFUSE_PUF_AUX_ROW				(41U)
#define XNVM_EFUSE_PUF_CHASH_ROW			(42U)
#define XNVM_EFUSE_SECURITY_CONTROL_ROW			(43U)
#define XNVM_EFUSE_REVOCATION_ID_0_ROW			(44U)
#define XNVM_EFUSE_DEC_EFUSE_ONLY_ROW			(57U)
#define XNVM_EFUSE_SECURITY_MISC_1_ROW			(58U)
#define XNVM_EFUSE_PPK_0_HASH_START_ROW			(64U)
#define XNVM_EFUSE_PPK_1_HASH_START_ROW			(72U)
#define XNVM_EFUSE_PPK_2_HASH_START_ROW			(80U)
#define XNVM_EFUSE_PPK_3_HASH_START_ROW			(176U)
#define XNVM_EFUSE_PPK_4_HASH_START_ROW			(184U)
#define XNVM_EFUSE_OFFCHIP_REVOKE_0_ROW			(88U)
#define XNVM_EFUSE_META_HEADER_IV_START_ROW		(96U)
#define XNVM_EFUSE_BLACK_OBFUS_IV_START_ROW		(116U)
#define XNVM_EFUSE_PLM_IV_START_ROW			(119U)
#define XNVM_EFUSE_DATA_PARTITION_IV_START_ROW		(122U)
#define XNVM_EFUSE_PUF_SYN_START_ROW			(129U)
#define XNVM_EFUSE_USER_FUSE_START_ROW			(129U)
#define XNVM_EFUSE_PUF_SYN_CACHE_READ_ROW       (641U)
/** @} */

/**
 * @name  EFUSE row count
 */
/**< EFUSE row count numbers */
#define XNVM_EFUSE_AES_KEY_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_USER_KEY_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_PPK_HASH_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_DEC_EFUSE_ONLY_NUM_OF_ROWS		(1U)
#define XNVM_EFUSE_SECURITY_MISC_1_NUM_OF_ROWS		(1U)
#define XNVM_EFUSE_GLITCH_NUM_OF_ROWS			(1U)
#define XNVM_EFUSE_PUF_SYN_DATA_NUM_OF_ROWS		(127U)
#define XNVM_EFUSE_PUF_CHASH_NUM_OF_ROWS		(1U)
#define XNVM_EFUSE_PUF_AUX_NUM_OF_ROWS			(1U)
#define XNVM_EFUSE_IV_NUM_OF_ROWS			(3U)
#define XNVM_EFUSE_DNA_NUM_OF_ROWS			(4U)
#define XNVM_EFUSE_BOOT_ENV_CTRL_NUM_OF_ROWS		(1U)
/** @} */

/**
 * @name  EFUSE column numbers
 */
/**< EFUSE column numbers */
#define XNVM_EFUSE_ROW_43_1_PROT_COLUMN			(2U)
#define XNVM_EFUSE_ROW_57_1_PROT_COLUMN			(3U)
#define XNVM_EFUSE_ROW64_87_1_PROT_COLUMN		(16U)
#define XNVM_EFUSE_ROW96_99_1_PROT_COLUMN		(17U)
#define XNVM_EFUSE_ROW_58_PROT_COLUMN			(18U)
#define XNVM_EFUSE_ROW_42_PROT_COLUMN			(19U)
#define XNVM_EFUSE_ROW_40_PROT_COLUMN			(20U)
#define XNVM_EFUSE_ROW_37_PROT_COLUMN			(21U)
#define XNVM_EFUSE_ROW96_99_0_PROT_COLUMN		(22U)
#define XNVM_EFUSE_ROW64_87_0_PROT_COLUMN		(23U)
#define XNVM_EFUSE_ROW_57_0_PROT_COLUMN			(24U)
#define XNVM_EFUSE_ROW_43_0_PROT_COLUMN			(25U)
#define XNVM_EFUSE_GLITCH_WRLK_COLUMN			(31U)
#define XNVM_EFUSE_GLITCH_ROM_EN_COLUMN			(29U)
#define XNVM_EFUSE_GLITCH_HALT_EN_0_COLUMN		(30U)
#define XNVM_EFUSE_GLITCH_HALT_EN_1_COLUMN		(31U)
#define XNVM_EFUSE_HALT_BOOT_ERROR_1			(22U)
#define XNVM_EFUSE_HALT_BOOT_ERROR_0			(21U)
#define XNVM_EFUSE_HALT_ENV_ERROR_1				(20U)
#define XNVM_EFUSE_HALT_ENV_ERROR_0				(19U)
#define XNVM_EFUSE_PUF_ECC_PUF_CTRL_REGEN_DIS_COLUMN	(31U)
#define XNVM_EFUSE_PUF_ECC_PUF_CTRL_HD_INVLD_COLUMN	(30U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif	/* XNVM_EFUSE_HW_H */
