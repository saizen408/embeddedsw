/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file  xttcps_intr_example.c
*
* This file contains a example using two timer counters in the Triple Timer
* Counter (TTC) module in the Ps block in interrupt mode.
*
* The example proceeds using interleaving interrupt handling from both
* timer counters. One timer counter, Ticker, counts how many interrupts
* has occurred to it, and updates a flag for another timer counter upon
* a given threshold. Another timer counter, PWM, waits for the flag set
* from the Ticker, and increases its duty cycle. When the duty cycle of
* PWM reaches 100, the example terminates.
*
* @note
*  The example may take seconds to minutes to finish. A small setting of
*  PWM_DELTA_DUTY gives a long running time, while a large setting makes
*  the example finishes faster.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver  Who    Date     Changes
* ---- ------ -------- ---------------------------------------------
* 1.00 drg/jz 01/23/10 First release
* 3.01 pkp	  01/30/16 Modified SetupTimer to remove XTtcps_Stop before TTC
*					   configuration as it is added in xttcps.c in
*					   XTtcPs_CfgInitialize
* 3.2  mus    10/28/16 Updated TmrCntrSetup as per prototype of
*                      XTtcPs_CalcIntervalFromFreq
* 3.10 mus    05/20/19 Update example to make it generic to run on any
*                      intended TTC device
*      aru    05/30/19 Updated the exapmle to use XTtcPs_InterruptHandler().
* 3.12 mus    07/13/20 Updated SettingsTable, to hold settings for PWM and tick
*                       timer device setting at correct indices. It fixes
*                       CR#1069191.
* 3.16 adk    04/19/22 Fix infinite loop in the example by adding polled
* 		       timeout loop.
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include <stdio.h>
#include <stdlib.h>
#include "xstatus.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xttcps_example.h"
#include "xinterrupt_wrap.h"

/************************** Constant Definitions *****************************/
#if defined (PLATFORM_ZYNQ)
#define NUM_DEVICES    9U
#else
#define NUM_DEVICES    12U
#endif

/*
 * Constants to set the basic operating parameters.
 * PWM_DELTA_DUTY is critical to the running time of the test. Smaller values
 * make the test run longer.
 */
#define	TICK_TIMER_FREQ_HZ	100  /* Tick timer counter's output frequency */
#define	PWM_OUT_FREQ		350  /* PWM timer counter's output frequency */

#define PWM_DELTA_DUTY	50 /* Initial and increment to duty cycle for PWM */
#define TICKS_PER_CHANGE_PERIOD TICK_TIMER_FREQ_HZ * 5 /* Tick signals PWM */
#define XTTCPS_SW_TIMEOUT_VAL	20000000U /* Wait for 20 sec */

#define TTC_TICK_DEVICE	0U
#define TTC_PWM_DEVICE	1U
#define TTC_COUNTER_OFFSET	4U

/**************************** Type Definitions *******************************/
typedef struct {
	u32 OutputHz;	/* Output frequency */
	XInterval Interval;	/* Interval value */
	u8 Prescaler;	/* Prescaler value */
	u16 Options;	/* Option settings */
} TmrCntrSetup;

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

static int TmrInterruptExample(void);  /* Main test */

/* Set up routines for timer counters */
static int SetupTicker(void);
static int SetupPWM(void);
static int SetupTimer(UINTPTR BaseAddr, u16 CounterNum, XTtcPs *TickTimer, u16 DeviceIndex);

/* Interleaved interrupt test for both timer counters */
static int WaitForDutyCycleFull(void);

static int SetupInterruptSystem(u16 IntcDeviceID, XScuGic *IntcInstancePtr);

static void TickHandler(void *CallBackRef, u32 StatusEvent);
static void PWMHandler(void *CallBackRef, u32 StatusEvent);

/************************** Variable Definitions *****************************/

static XTtcPs TickTimer;
static XTtcPs PWMTimer;

static TmrCntrSetup SettingsTable[NUM_DEVICES] = {
	{200, 0, 0, 0}, /* PWM timer counter initial setup, only output freq */
	{100, 0, 0, 0},	/* Ticker timer counter initial setup, only output freq */
};


static u32 MatchValue;  /* Match value for PWM, set by PWM interrupt handler,
			updated by main test routine */

static volatile u32 PWM_UpdateFlag;	/* Flag used by Ticker to signal PWM */
static volatile u8 ErrorCount;		/* Errors seen at interrupt time */
static volatile u32 TickCount;		/* Ticker interrupts between PWM change */


/*****************************************************************************/
/**
*
* This function calls the Ttc interrupt example.
*
* @param	None
*
* @return
*		- XST_SUCCESS to indicate Success
*		- XST_FAILURE to indicate Failure.
*
* @note		None
*
*****************************************************************************/
int main(void)
{
	int Status;

	xil_printf("TTC Interrupt Example Test\r\n");

	Status = TmrInterruptExample();
	if (Status != XST_SUCCESS) {
		xil_printf("TTC Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran TTC Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt example.
*
* @param	None.
*
* @return
*		- XST_SUCCESS to indicate Success
*		- XST_FAILURE to indicate Failure.
*
* @note		None
*
****************************************************************************/
static int TmrInterruptExample(void)
{
	int Status;

	/*
	 * Make sure the interrupts are disabled, in case this is being run
	 * again after a failure.
	 */

	/*
	 * Set up the Ticker timer
	 */
	Status = SetupTicker();
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Set up  the PWM timer
	 */
	Status = SetupPWM();
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable interrupts
	 */

	Status = WaitForDutyCycleFull();
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Stop the counters
	 */
	XTtcPs_Stop(&TickTimer);

	XTtcPs_Stop(&PWMTimer);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function sets up the Ticker timer.
*
* @param	None
*
* @return	XST_SUCCESS if everything sets up well, XST_FAILURE otherwise.
*
* @note		None
*
*****************************************************************************/
int SetupTicker(void)
{
	int Status;
	TmrCntrSetup *TimerSetup;
	XTtcPs *TtcPsTick;

	TimerSetup = &(SettingsTable[TTC_TICK_DEVICE]);

	/*
	 * Set up appropriate options for Ticker: interval mode without
	 * waveform output.
	 */
	TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_WAVE_DISABLE);

	/*
	 * Calling the timer setup routine
	 *  . initialize device
	 *  . set options
	 */
	Status = SetupTimer(XTTCPS_BASEADDRESS, XTTCPS_COUNTER_NUM1, &TickTimer, TTC_TICK_DEVICE);
	if(Status != XST_SUCCESS) {
		return Status;
	}

	TtcPsTick = &TickTimer;

	/*
	 * Connect to the interrupt controller
	 */
	Status = XSetupInterruptSystem(TtcPsTick, (Xil_ExceptionHandler)XTtcPs_InterruptHandler, \
	TtcPsTick->Config.IntrId[XTTCPS_COUNTER_NUM1], TtcPsTick->Config.IntrParent, \
	XINTERRUPT_DEFAULT_PRIORITY);
	XTtcPs_SetStatusHandler(TtcPsTick, TtcPsTick,
		              (XTtcPs_StatusHandler)TickHandler);
	/*
	 * Enable the interrupts for the tick timer/counter
	 * We only care about the interval timeout.
	 */
	XTtcPs_EnableInterrupts(TtcPsTick, XTTCPS_IXR_INTERVAL_MASK);

	/*
	 * Start the tick timer/counter
	 */
	XTtcPs_Start(TtcPsTick);

	return Status;
}

/****************************************************************************/
/**
*
* This function sets up the waveform output timer counter (PWM).
*
* @param	None
*
* @return	XST_SUCCESS if everything sets up well, XST_FAILURE otherwise.
*
* @note		None
*
*****************************************************************************/
int SetupPWM(void)
{
	int Status;
	TmrCntrSetup *TimerSetup;
	XTtcPs *TtcPsPWM;

	TimerSetup = &(SettingsTable[TTC_PWM_DEVICE]);

	/*
	 * Set up appropriate options for PWM: interval mode  and
	 * match mode for waveform output.
	 */
	TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_MATCH_MODE);

	/*
	 * Calling the timer setup routine
	 * 	initialize device
	 * 	set options
	 */
	Status = SetupTimer(XTTCPS_BASEADDRESS, XTTCPS_COUNTER_NUM2, &PWMTimer, TTC_PWM_DEVICE);
	if(Status != XST_SUCCESS) {
		return Status;
	}

	TtcPsPWM = &PWMTimer;

	/*
	 * Connect to the interrupt controller
	 */
	Status = XSetupInterruptSystem(TtcPsPWM, (Xil_ExceptionHandler)XTtcPs_InterruptHandler, \
	TtcPsPWM->Config.IntrId[XTTCPS_COUNTER_NUM2], TtcPsPWM->Config.IntrParent, \
	XINTERRUPT_DEFAULT_PRIORITY);

	XTtcPs_SetStatusHandler(TtcPsPWM, TtcPsPWM,
		              (XTtcPs_StatusHandler) PWMHandler);
	/*
	 * Enable the interrupts for the tick timer/counter
	 * We only care about the interval timeout.
	 */
	XTtcPs_EnableInterrupts(TtcPsPWM, XTTCPS_IXR_INTERVAL_MASK);

	/*
	 * Start the tick timer/counter
	 */
	XTtcPs_Start(TtcPsPWM);

	return Status;
}
/****************************************************************************/
/**
*
* This function uses the interrupt inter-locking between the ticker timer
* counter and the waveform output timer counter. When certain amount of
* interrupts have happened to the ticker timer counter, a flag, PWM_UpdateFlag,
* is set to true.
*
* When PWM_UpdateFlag for the waveform timer counter is true, the duty
* cycle for PWM timer counter is increased by PWM_DELTA_DUTY.
* The function exits successfully when the duty cycle for PWM timer
* counter reaches beyond 100.
*
* @param	None
*
* @return	XST_SUCCESS if duty cycle successfully reaches beyond 100,
*		otherwise, XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
int WaitForDutyCycleFull(void)
{
	TmrCntrSetup *TimerSetup;
	u8 DutyCycle;		/* The current output duty cycle */
	XTtcPs *TtcPs_PWM;	/* Pointer to the instance structure */
	u32 PollCount = XTTCPS_SW_TIMEOUT_VAL;

	TimerSetup = &(SettingsTable[TTC_PWM_DEVICE]);
	TtcPs_PWM = &PWMTimer;

	/*
	 * Initialize some variables used by the interrupts and in loops.
	 */
	DutyCycle = PWM_DELTA_DUTY;
	PWM_UpdateFlag = TRUE;
	ErrorCount = 0;

	/*
	 * Loop until 100% duty cycle in the PWM output.
	 */
	while (DutyCycle <= 100) {

		/*
		 * If error occurs, disable interrupts, and exit.
		 */
		if ((0 != ErrorCount) || (PollCount == 0)) {
			return XST_FAILURE;
		}

		/*
		 * The Ticker interrupt sets a flag for PWM to update its duty
		 * cycle.
		 */
		if (PWM_UpdateFlag) {
			/* Calculate the new register setting here, not at the
			 * time critical interrupt level.
			 */
			MatchValue = (TimerSetup->Interval * DutyCycle) / 100;

			/*
			 * Change the PWM duty cycle
			 */
			DutyCycle += PWM_DELTA_DUTY;

			/*
			 * Enable the PWM Interval interrupt
			 */
			XTtcPs_EnableInterrupts(TtcPs_PWM,
						 XTTCPS_IXR_INTERVAL_MASK);

			PWM_UpdateFlag = FALSE;

		} else {
			PollCount--;
			usleep(1U);
		}
	}

	return XST_SUCCESS;
}
/****************************************************************************/
/**
*
* This function sets up a timer counter device, using the information in its
* setup structure.
*  . initialize device
*  . set options
*  . set interval and prescaler value for given output frequency.
*
* @param	BaseAddr is the base address for the device.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*****************************************************************************/
int SetupTimer(UINTPTR BaseAddr, u16 CounterNum, XTtcPs *Timer, u16 device)
{
	int Status;
	XTtcPs_Config *Config;
	TmrCntrSetup *TimerSetup;

	TimerSetup = &SettingsTable[device];

	/*
	 * Look up the configuration based on the device identifier
	 */
	Config = XTtcPs_LookupConfig(BaseAddr);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the device
	 */
	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress +	\
		TTC_COUNTER_OFFSET*CounterNum);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the options
	 */
	XTtcPs_SetOptions(Timer, TimerSetup->Options);

	/*
	 * Timer frequency is preset in the TimerSetup structure,
	 * however, the value is not reflected in its other fields, such as
	 * IntervalValue and PrescalerValue. The following call will map the
	 * frequency to the interval and prescaler values.
	 */
	XTtcPs_CalcIntervalFromFreq(Timer, TimerSetup->OutputHz,
		&(TimerSetup->Interval), &(TimerSetup->Prescaler));

	/*
	 * Set the interval and prescale
	 */
	XTtcPs_SetInterval(Timer, TimerSetup->Interval);
	XTtcPs_SetPrescaler(Timer, TimerSetup->Prescaler);

	return XST_SUCCESS;
}

/***************************************************************************/
/**
*
* This function is the handler which handles the periodic tick interrupt.
* It updates its count, and set a flag to signal PWM timer counter to
* update its duty cycle.
*
* This handler provides an example of how to handle data for the TTC and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the TTC driver.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void TickHandler(void *CallBackRef, u32 StatusEvent)
{
	if (0 != (XTTCPS_IXR_INTERVAL_MASK & StatusEvent)) {
		TickCount++;

		if (TICKS_PER_CHANGE_PERIOD == TickCount) {
			TickCount = 0;
			PWM_UpdateFlag = TRUE;
		}

	}
	else {
		/*
		 * The Interval event should be the only one enabled. If it is
		 * not it is an error
		 */
		ErrorCount++;
	}
}

/***************************************************************************/
/**
*
* This function is the handler which handles the PWM interrupt.
*
* It updates the match register to reflect the change on duty cycle. It also
* disable interrupt at the end. The interrupt will be enabled by the Ticker
* timer counter.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is a pointer to the MatchValue variable.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
static void PWMHandler(void *CallBackRef, u32 StatusEvent)
{
	XTtcPs *Timer;

	Timer = &PWMTimer;

	if (0 != (XTTCPS_IXR_INTERVAL_MASK & StatusEvent)) {
		XTtcPs_SetMatchValue(Timer, 0, MatchValue);
	}
	else {
		/*
		 * If it is not Interval event, it is an error.
		 */
		ErrorCount++;
	}

	XTtcPs_DisableInterrupts(Timer, XTTCPS_IXR_ALL_MASK);
}
