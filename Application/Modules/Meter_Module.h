#ifndef Meter_Module_H_
#define Meter_Module_H_

#include "./Modules.h"

typedef enum
{
	METERSTATE_RESET = 0,
	METERSTATE_INITIALIZED,
	METERSTATE_ON,		/* Meter ON, all function ON	*/
	METERSTATE_OFFSC,	/* Meter OFF, several function still work (Display ON, CAN work)	*/
	METERSTATE_STANDBY, /* Meter operation OFF before enter sleep (Display OFF, CAN still work)	*/
	METERSTATE_HALT		/* Meter in the sleep condition	*/
} MeterState_TypeDef;

MeterState_TypeDef Meter_getStat(void);
void Meter_setStat(MeterState_TypeDef);

void Meter_init(void);
void Meter_open(void);
void Meter_close(void);
void Meter_flush(void);

#endif //Meter_Module_H_
