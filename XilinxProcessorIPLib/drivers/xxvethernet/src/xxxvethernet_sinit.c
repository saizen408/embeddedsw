/******************************************************************************
* Copyright (C) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/**
*
* @file xxxvethernet_sinit.c
* @addtogroup xxvethernet_v1_7
* @{
*
* This file contains static initialization functionality for XXV Ethernet driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.0   hk   6/16/17  First release
* 1.1   mj   3/30/18  Add XXxvEthernet_LookupConfigBaseAddr(UINTPTR Baseaddr)
*                     API to lookup config by base address.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xxxvethernet.h"
#ifndef SDT
#include "xparameters.h"
#endif

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* XXxvEthernet_LookupConfig returns a reference to an XXxvEthernet_Config
* structure based on an unique device id, <i>DeviceId</i>. The return value
* will refer to an entry in the device configuration table defined in the
* xxxvethernet_g.c file.
*
* @param	DeviceId is the unique ID for the lookup operation.
*
* @return
*		- Returns a reference to a config record in the
*		  configuration table (in xxxvethernet_g.c) corresponding to
*		  <i>DeviceId</i>, or NULL
*		- NULL if no match is found.
*
******************************************************************************/
#ifndef SDT
XXxvEthernet_Config *XXxvEthernet_LookupConfig(u16 DeviceId)
{
	extern XXxvEthernet_Config XXxvEthernet_ConfigTable[];
	XXxvEthernet_Config *CfgPtr = NULL;
	int Index;

	for (Index = 0; Index < XPAR_XXXVETHERNET_NUM_INSTANCES; Index++) {
		if (XXxvEthernet_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XXxvEthernet_ConfigTable[Index];
			break;
		}
	}

	return (CfgPtr);
}
#else
XXxvEthernet_Config *XXxvEthernet_LookupConfig(UINTPTR BaseAddress)
{
	extern XXxvEthernet_Config XXxvEthernet_ConfigTable[];
	XXxvEthernet_Config *CfgPtr = NULL;
	int Index;

	for (Index = 0x0; XXxvEthernet_ConfigTable[Index].Name != NULL; Index++) {
		if ((XXxvEthernet_ConfigTable[Index].BaseAddress == BaseAddress) ||
		     !BaseAddress) {
			CfgPtr = &XXxvEthernet_ConfigTable[Index];
			break;
		}
	}

	return (CfgPtr);
}
#endif

/*****************************************************************************/
/**
* XXxvEthernet_LookupConfigBaseAddr returns a reference to an
* XXxvEthernet_Config structure based on base address. The return value
* will refer to an entry in the device configuration table defined in the
* xxxvethernet_g.c file.
*
* @param	 Baseaddr is the base address of the device to lookup for.
*
* @return
*		- Returns a reference to a config record in the
*		  configuration table (in xxxvethernet_g.c) corresponding to
*		  <i>Baseaddr</i>, or NULL
*		- NULL if no match is found.
*
******************************************************************************/
#ifndef SDT
XXxvEthernet_Config *XXxvEthernet_LookupConfigBaseAddr(UINTPTR Baseaddr)
{
	extern XXxvEthernet_Config XXxvEthernet_ConfigTable[];
	XXxvEthernet_Config *CfgPtr = NULL;
	int Index;

	for (Index = 0; Index < XPAR_XXXVETHERNET_NUM_INSTANCES; Index++) {
		if (XXxvEthernet_ConfigTable[Index].BaseAddress == Baseaddr) {
			CfgPtr = &XXxvEthernet_ConfigTable[Index];
			break;
		}
	}

	return (CfgPtr);
}
#endif
/** @} */
