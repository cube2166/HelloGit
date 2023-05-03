#include "./Meter_Module.h"

static MeterState_TypeDef meterState, setState;
static uint16_t meterFlush_per1ms,
	meterFlush_per10ms,
	meterFlush_per25ms,
	meterFlush_per50ms,
	meterFlush_per100ms,
	meterFlush_per200ms,
	meterFlush_per1000ms;
static uint8_t shutdown_Cnt;
static uint8_t continuation_Cnt;

/*-------------------------------------------
 * 	Function Meter_opModeProc()
 * 	Check peripheral conditoins like IGN condition and CAN bus condition
 * 	Manage and set the meter enter relative operation mode
 *
 ------------------------------------------*/
static void Meter_opModeProc(void)
{
	bool tmp_SleepFlag = CAN_Get_SleepFlag();
	bool tmp_ShutdownFlag = BLEP_Get_ShutdownFlag();
	bool tmp_WhiteListFlag = CAN_Get_WhiteListFlag();

	switch (meterState)
	{
	case METERSTATE_RESET: /* force to run initialize routine	*/
		Meter_init();
		break;
	case METERSTATE_INITIALIZED:
		Meter_setStat(METERSTATE_ON);
		break;
	case METERSTATE_ON:
		if (tmp_SleepFlag == true)
		{
			Meter_setStat(METERSTATE_STANDBY);
			shutdown_Cnt = 0;
		}
		else
		{
			/* nothing */
		}
		break;
	case METERSTATE_OFFSC:
		continuation_Cnt = continuation_Cnt + 1;
		if (continuation_Cnt >= (uint8_t)50)
		{
			continuation_Cnt = 0;
			Meter_setStat(METERSTATE_HALT);
		}
		else
		{
			if (tmp_WhiteListFlag == true)
			{
				Meter_setStat(METERSTATE_ON);
			}
			else
			{
				/* ignore */
			}
		}
		break;
	case METERSTATE_STANDBY:
		shutdown_Cnt = shutdown_Cnt + 1;
		if ((tmp_ShutdownFlag == true) || (shutdown_Cnt >= (uint8_t)50))
		{
			shutdown_Cnt = 0;
			Meter_setStat(METERSTATE_HALT);
		}
		else
		{
			/* nothing */
		}
		break;
	case METERSTATE_HALT:
		// Meter_setStat(METERSTATE_OFFSC);
		Meter_setStat(METERSTATE_ON);
		continuation_Cnt = 0;
		break;
	}
}

/*========================================================================================*/
/* Special routine only executed in Halt mode	*/
static void Meter_updateHalt(void)
{ /* Execute periodically every wake-up from Halt	*/
	hal_halt();
	// CAN_Reset_SleepFlag();
	Meter_opModeProc();
	meterFlush_per1ms = SimpleOS_GetTick();
	meterFlush_per10ms = SimpleOS_GetTick();
	meterFlush_per25ms = SimpleOS_GetTick();
	meterFlush_per50ms = SimpleOS_GetTick();
	meterFlush_per100ms = SimpleOS_GetTick();
	meterFlush_per200ms = SimpleOS_GetTick();
	meterFlush_per1000ms = SimpleOS_GetTick();
}

/*========================================================================================*/

/*---------------------- Routine update -------------------------------------------------------------*/
static void Meter_update_1ms(void)
{
	USART_flush();
}

static void Meter_update_10ms(void)
{
	Eeprom_flush();
	BTN_flush();
}

static void Meter_update_25ms(void)
{
}

static void Meter_update_50ms(void)
{
	BLE_flush();
}

static void Meter_update_100ms(void)
{
}

static void Meter_update_200ms(void)
{
}

static void Meter_update_1000ms(void)
{
}

/*-------------------------------------------------------------------------------------------------------*/

/*=============================== Meter state switch ======================================================*/
static void Meter_enterOnStat(void)
{ /* Set modules to enter Meter ON mode	*/
	if (meterState != METERSTATE_ON)
	{
		meterState = METERSTATE_ON;
		Module_SetActiveMode();
	}
	else
	{
	}
}

static void Meter_enterOffScStat(void)
{ /* Set modules to enter OffScreen mode	*/
	if (meterState != METERSTATE_OFFSC)
	{
		meterState = METERSTATE_OFFSC;
		Module_SetOffScMode();
	}
	else
	{
	}
}

static void Meter_enterStandbyStat(void)
{ /* Set Modules to enter Standby mode	*/
	if (meterState != METERSTATE_STANDBY)
	{
		meterState = METERSTATE_STANDBY;
		Module_SetStandbyMode();
	}
	else
	{
	}
}

static void Meter_enterHaltStat(void)
{ /* Set modules to enter Meter Halt mode	*/
	if (meterState != METERSTATE_HALT)
	{
		meterState = METERSTATE_HALT;
		Module_SetSleepMode();
	}
	else
	{
	}
}

/*============================================================================*/
static ModuleState_TypeDef MeterModule_stat = MODULESTATE_RESET;

void Meter_init(void)
{
	MeterModule_stat = MODULESTATE_INITIALIZED;

	/* Write the module initialize code here	*/
	meterState = METERSTATE_INITIALIZED;
	setState = METERSTATE_INITIALIZED;

	Module_Initialize();
}

void Meter_open(void)
{
	if (MeterModule_stat == MODULESTATE_OPENED)
		return;
	MeterModule_stat = MODULESTATE_OPENED;

	/* Write the module open code here	*/
	meterFlush_per1ms = SimpleOS_GetTick();
	meterFlush_per10ms = SimpleOS_GetTick();
	meterFlush_per25ms = SimpleOS_GetTick();
	meterFlush_per50ms = SimpleOS_GetTick();
	meterFlush_per100ms = SimpleOS_GetTick();
	meterFlush_per200ms = SimpleOS_GetTick();
	meterFlush_per1000ms = SimpleOS_GetTick();

	Meter_opModeProc();
}

void Meter_close(void)
{
	if (MeterModule_stat == MODULESTATE_CLOSED)
		return;

	MeterModule_stat = MODULESTATE_CLOSED;
	/* Write the module close code here	*/
}

void Meter_flush(void)
{
	bool tmp_Result;
	if (MeterModule_stat == MODULESTATE_OPENED)
	{
		BLEP_flush();
		DIAG_flush();
		CAN_flush();

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per1ms, (uint16_t)1);
		if (tmp_Result == true)
		{
			Meter_update_1ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per10ms, (uint16_t)10);
		if (tmp_Result == true)
		{
			Meter_opModeProc();
			Meter_update_10ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per25ms, (uint16_t)25);
		if (tmp_Result == true)
		{
			Meter_update_25ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per50ms, (uint16_t)50);
		if (tmp_Result == true)
		{
			Meter_update_50ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per100ms, (uint16_t)100);
		if (tmp_Result == true)
		{
			Meter_update_100ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per200ms, (uint16_t)200);
		if (tmp_Result == true)
		{
			Meter_update_200ms();
		}

		tmp_Result = SimpleOS_CheckInterval(&meterFlush_per1000ms, (uint16_t)1000);
		if (tmp_Result == true)
		{
			Meter_update_1000ms();
		}

		/* Switch between state: The mode change within Meter_opModeProc() will be executed here */
		if (meterState != setState)
		{
			switch (setState)
			{
			case METERSTATE_INITIALIZED:
				break;
			case METERSTATE_ON:
				Meter_enterOnStat();
				break;
			case METERSTATE_OFFSC:
				Meter_enterOffScStat();
				break;
			case METERSTATE_STANDBY:
				Meter_enterStandbyStat();
				break;
			case METERSTATE_RESET:
				break;
			case METERSTATE_HALT:
				Meter_enterHaltStat();
				break;
			}
			meterState = setState;
		}

		/* Specified function in each meter state. NOTE:Not included in the flush routine */
		switch (meterState)
		{
		case METERSTATE_INITIALIZED:
			break;
		case METERSTATE_ON:
			break;
		case METERSTATE_OFFSC:
			break;
		case METERSTATE_STANDBY:
			break;
		case METERSTATE_RESET:
			break;
		case METERSTATE_HALT:
			Meter_updateHalt();
			break;
		}
	}
}

/*-----------------------------------------------------------*/
MeterState_TypeDef Meter_getStat(void)
{
	return meterState;
}

void Meter_setStat(MeterState_TypeDef newStat)
{
	setState = newStat;
}

/* END Meter_Module */
