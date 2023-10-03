/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 * @file xiicps_intr_master_example.c
 *
 * This example can run on zynqmp / versal platform evaluation board and
 * IIC controller configured master in interrupt-driven mode and Aardvark
 * Analyzer used as slave.
 *
 * It sends 18 buffers of data to slave and expects to receive the
 * same data through the IIC using the Aardvark test hardware.
 *
 * <pre> MODIFICATION HISTORY:
 *
 * Ver   Who Date     Changes
 * ----- --- -------- -----------------------------------------------
 * 1.00a jz  01/30/10 First release
 *
 * </pre>
 *
 ****************************************************************************/

/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xiicps.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xinterrupt_wrap.h"

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifndef SDT
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#endif

/*
 * The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x55
#define IIC_SCLK_RATE		100000


/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC.
 */
#define TEST_BUFFER_SIZE   	250
#define NUMBER_OF_SIZES		18

/**************************** Type Definitions ********************************/

/************************** Function Prototypes *******************************/

#ifndef SDT
int IicPsMasterIntrExample(u16 DeviceId);
#else
int IicPsMasterIntrExample(UINTPTR BaseAddress);
#endif

void Handler(void *CallBackRef, u32 Event);

/************************** Variable Definitions ******************************/

XIicPs Iic;			/* Instance of the IIC Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC. They are defined as global so that they are not on the stack.
 */
u8 SendBuffer[TEST_BUFFER_SIZE];    /* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];    /* Buffer for Receiving Data */

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
volatile u32 SendComplete;
volatile u32 RecvComplete;
volatile u32 TotalErrorCount;

/******************************************************************************/
/**
*
* Main function to call the example.
*
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/
int main(void)
{
	int Status;

	xil_printf("IIC Master Interrupt Example Test \r\n");

	/*
	 * Run the Iic Master Interrupt example , specify the Device ID that is
	 * generated in xparameters.h
	 */
#ifndef SDT
	Status = IicPsMasterIntrExample(IIC_DEVICE_ID);
#else
	Status = IicPsMasterIntrExample(XPAR_XIICPS_0_BASEADDR);
#endif
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Master Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran IIC Master Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function does a minimal test on the Iic device and driver as a
* design example. The purpose of this function is to illustrate
* how to use the XIicPs driver.
*
* This function sends data and expects to receive the same data through the IIC
* using the Aardvark test hardware.
*
* This function uses interrupt driver mode of the IIC.
*
* @param	DeviceId is the Device ID of the IicPs Device and is the
*		XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
*******************************************************************************/
#ifndef SDT
int IicPsMasterIntrExample(u16 DeviceId)
#else
int IicPsMasterIntrExample(UINTPTR BaseAddress)
#endif
{
	int Status;
	XIicPs_Config *Config;
	int Index;
	int tmp;
	int BufferSizes[NUMBER_OF_SIZES] = {1, 2, 19, 31, 32, 33, 62, 63, 64,
	65, 66, 94, 95, 96, 97, 98, 99, 250};

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
#ifndef SDT
	Config = XIicPs_LookupConfig(DeviceId);
#else
	Config = XIicPs_LookupConfig(BaseAddress);
#endif
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the IIC to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
        Status = XSetupInterruptSystem(&Iic, XIicPs_MasterInterruptHandler,
                                        Config->IntrId,
                                        Config->IntrParent,
                                        XINTERRUPT_DEFAULT_PRIORITY);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the IIC that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the IIC driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
	XIicPs_SetStatusHandler(&Iic, (void *) &Iic, Handler);

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified.
	 */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
		SendBuffer[Index] = (Index % TEST_BUFFER_SIZE);
		RecvBuffer[Index] = 0;
	}

	for(Index = 0; Index < NUMBER_OF_SIZES; Index++) {

		/* Wait for bus to become idle
		 */
		while (XIicPs_BusIsBusy(&Iic)) {
			/* NOP */
		}

		SendComplete = FALSE;

		/*
		 * Send the buffer, errors are reported by TotalErrorCount.
		 */
		XIicPs_MasterSend(&Iic, SendBuffer, BufferSizes[Index],
				IIC_SLAVE_ADDR);

		/*
		 * Wait for the entire buffer to be sent, letting the interrupt
		 * processing work in the background, this function may get
		 * locked up in this loop if the interrupts are not working
		 * correctly.
		 */
		while (!SendComplete) {
			if (0 != TotalErrorCount) {
				return XST_FAILURE;
			}
		}

		/*
		 * Wait bus activities to finish.
		 */
		while (XIicPs_BusIsBusy(&Iic)) {
			/* NOP */
		}

		/*
		 * Receive data from slave, errors are reported through
		 * TotalErrorCount.
		 */
		RecvComplete = FALSE;
		XIicPs_MasterRecv(&Iic, RecvBuffer, BufferSizes[Index],
				IIC_SLAVE_ADDR);

		while (!RecvComplete) {
			if (0 != TotalErrorCount) {
				return XST_FAILURE;
			}
		}

		/* Check for received data.
		 */
		for(tmp = 0; tmp < BufferSizes[Index]; tmp ++) {

			/*
			 * Aardvark as slave can only set up to 64 bytes for
			 * output.
			 */
			if (RecvBuffer[tmp] != tmp % 64) {
				return XST_FAILURE;
			}
		}
	}
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the IIC.  It is called from an interrupt context such that the amount
* of processing performed should be minimized.
*
* This handler provides an example of how to handle data for the IIC and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the IIC driver.
* @param	Event contains the specific kind of event that has occurred.
*
* @return	None.
*
* @note		None.
*
*******************************************************************************/
void Handler(void *CallBackRef, u32 Event)
{
	/*
	 * All of the data transfer has been finished.
	 */
	if (0 != (Event & XIICPS_EVENT_COMPLETE_RECV)){
		RecvComplete = TRUE;
	} else if (0 != (Event & XIICPS_EVENT_COMPLETE_SEND)) {
		SendComplete = TRUE;
	} else if (0 == (Event & XIICPS_EVENT_SLAVE_RDY)){
		/*
		 * If it is other interrupt but not slave ready interrupt, it is
		 * an error.
		 * Data was received with an error.
		 */
		TotalErrorCount++;
	}
}
