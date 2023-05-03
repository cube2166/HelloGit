#include "./SimpleOS_Module.h"

static volatile uint16_t timer_1ms;

void SimpleOS_ISR(void)
{
	timer_1ms = timer_1ms + 1;

	CAN_ISR();
}

void SimpleOS_DelayMS(const uint16_t ms)
{
	uint16_t current = timer_1ms;
	uint16_t next = current;
	uint16_t diff = 0;

	while (diff <= ms)
	{
		current = timer_1ms;

		if (current >= next)
		{
			diff = current - next;
		}
		else
		{
			diff = 0xFFFF - next;
			diff = diff + current + 1;
		}
		CLRWDT();
	}
}

bool SimpleOS_CheckInterval(uint16_t *flushTick, uint16_t interval)
{
	uint16_t diff = 0;
	uint16_t current = timer_1ms;
	uint16_t before = *flushTick;
	bool status = false;

	if (current >= before)
	{
		diff = current - before;
	}
	else
	{
		diff = 0xFFFF - before;
		diff = diff + current + 1;
	}

	if (diff >= interval)
	{
		*flushTick = current;
		status = true;
	}
	else
	{
		status = false;
	}

	return status;
}

uint16_t SimpleOS_GetTick(void)
{
	return timer_1ms;
}

/*========================================================================================*/

static ModuleState_TypeDef SimpleOSModule_state = MODULESTATE_RESET;

void SimpleOS_init(void)
{
	SimpleOSModule_state = MODULESTATE_INITIALIZED;

	/* Write the module initialize code here	*/
	timer_1ms = 0;
	CAN_Reset_SleepTime();
}

void SimpleOS_open(void)
{
	if (SimpleOSModule_state != MODULESTATE_OPENED)
	{
		SimpleOSModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		CAN_Reset_SleepTime();
	}
	else
	{
	}
}

void SimpleOS_close(void)
{
	if (SimpleOSModule_state != MODULESTATE_CLOSED)
	{
		SimpleOSModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
	}
	else
	{
	}
}

void SimpleOS_flush(void)
{
	if (SimpleOSModule_state == MODULESTATE_OPENED)
	{
		/* Write the module flash code here	*/
	}
	else
	{
	}
}

#ifdef RTT_OUT
#define BUFFER_INDEX 0

void print_log(const char *sFormat, ...)
{
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

void print_log_0(const char *sFormat, ...)
{
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

void print_log_1(const char *sFormat, ...)
{
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_SetTerminal(1);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

void print_log_2(const char *sFormat, ...)
{
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_SetTerminal(2);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

void print_log_3(const char *sFormat, ...)
{
	va_list ParamList;
	va_start(ParamList, sFormat);
	SEGGER_RTT_SetTerminal(3);
	SEGGER_RTT_vprintf(BUFFER_INDEX, sFormat, &ParamList);
	va_end(ParamList);
}

void segger_rtt_init(char *str)
{
	SEGGER_RTT_Init();
	print_log(str);
}
#endif
