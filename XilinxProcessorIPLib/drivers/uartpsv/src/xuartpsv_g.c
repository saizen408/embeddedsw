/******************************************************************************
* Copyright (C) 2017 - 2021 Xilinx, Inc.  All Rights Reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc.  All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include "xparameters.h"
#include "xuartpsv.h"

/*
 * The configuration table for devices
 */

XUartPsv_Config XUartPsv_ConfigTable[] =
{
	{
		XPAR_SBSA_UART_DEVICE_ID,
		XPAR_SBSA_UART_BASEADDR,
		XPAR_SBSA_UART_CLK_FREQ_HZ,
		XPAR_SBSA_UART_HAS_MODEM
	}
};
