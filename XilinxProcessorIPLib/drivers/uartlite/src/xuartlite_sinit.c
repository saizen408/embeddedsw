/******************************************************************************
* Copyright (C) 2005 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xuartlite_sinit.c
* @addtogroup uartlite Overview
* @{
*
* The implementation of the XUartLite component's static initialization
* functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.01a jvb  10/13/05 First release
* 2.00a ktn  10/20/09 Updated to use HAL Processor APIs.
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xstatus.h"
#ifndef SDT
#include "xparameters.h"
#endif
#include "xuartlite_i.h"

/************************** Constant Definitions ****************************/


/**************************** Type Definitions ******************************/


/***************** Macros (Inline Functions) Definitions ********************/


/************************** Variable Definitions ****************************/


/************************** Function Prototypes *****************************/

/****************************************************************************
*
* Looks up the device configuration based on the unique device ID.  The table
* UartliteConfigTable contains the configuration info for each device in the
* system.
*
* @param	DeviceId is the unique device ID to match on.
*
* @return	A pointer to the configuration data for the device, or
*		NULL if no match was found.
*
* @note		None.
*
******************************************************************************/
#ifndef SDT
XUartLite_Config *XUartLite_LookupConfig(u16 DeviceId)
{
	XUartLite_Config *CfgPtr = NULL;
	u32 Index;

	for (Index=0; Index < XPAR_XUARTLITE_NUM_INSTANCES; Index++) {
		if (XUartLite_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XUartLite_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}
#else
XUartLite_Config *XUartLite_LookupConfig(UINTPTR BaseAddress)
{
	XUartLite_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = (u32)0x0; XUartLite_ConfigTable[Index].Name != NULL; Index++) {
		if ((XUartLite_ConfigTable[Index].RegBaseAddr == BaseAddress) ||
		    !BaseAddress) {
			CfgPtr = &XUartLite_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}
#endif
/****************************************************************************/
/**
*
* Initialize a XUartLite instance.  The receive and transmit FIFOs of the
* UART are not flushed, so the user may want to flush them. The hardware
* device does not have any way to disable the receiver such that any valid
* data may be present in the receive FIFO. This function disables the UART
* interrupt. The baudrate and format of the data are fixed in the hardware
* at hardware build time.
*
* @param	InstancePtr is a pointer to the XUartLite instance.
* @param	DeviceId is the unique id of the device controlled by this
*		XUartLite instance.  Passing in a device id associates the
*		generic XUartLite instance to a specific device, as chosen by
*		the caller or application developer.
*
* @return
* 		- XST_SUCCESS if everything starts up as expected.
* 		- XST_DEVICE_NOT_FOUND if the device is not found in the
*			configuration table.
*
* @note		None.
*
*****************************************************************************/
#ifndef SDT
int XUartLite_Initialize(XUartLite *InstancePtr, u16 DeviceId)
#else
int XUartLite_Initialize(XUartLite *InstancePtr, UINTPTR BaseAddress)
#endif
{
	XUartLite_Config *ConfigPtr;

	/*
	 * Assert validates the input arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/*
	 * Lookup the device configuration in the configuration table. Use this
	 * configuration info when initializing this component.
	 */
#ifndef SDT
	ConfigPtr = XUartLite_LookupConfig(DeviceId);
#else
	ConfigPtr = XUartLite_LookupConfig(BaseAddress);
#endif

	if (ConfigPtr == (XUartLite_Config *)NULL) {
		return XST_DEVICE_NOT_FOUND;
	}
	return XUartLite_CfgInitialize(InstancePtr, ConfigPtr,
					ConfigPtr->RegBaseAddr);
}

/** @} */
