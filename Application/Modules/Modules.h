#ifndef MODULES_H_
#define MODULES_H_

typedef enum
{
	MODULESTATE_RESET = 0,
	MODULESTATE_INITIALIZED,
	MODULESTATE_OPENED,
	MODULESTATE_CLOSED
} ModuleState_TypeDef;

#define U16_MSB(val) (uint8_t)(val >> (uint16_t)8)
#define U16_LSB(val) (uint8_t)(val & (uint16_t)0xFF)

#include "../config.h"

#include "../definitions.h"

#include "../hal/SAMC21E17A_hal.h"
#include "../hal/Board_Module.h"

#include "./System/SimpleOS_Module.h"
#include "./System/Eeprom_Module.h"

#include "./Meter_Module.h"
#include "./Function/CAN/CAN_Module.h"
#include "./Function/CAN/DIAG_Module.h"
#include "./Function/BLE/BLE_Package.h"
#include "./Function/BLE/BLE_Module.h"
#include "./Function/BLE/USART_Module.h"
#include "./Function/UTF_Module.h"
#include "./Function/BTN_Module.h"

#ifdef RTT_OUT
#include "../../RTT/SEGGER_RTT.h"
#endif

void Module_Initialize(void);
void Module_SetActiveMode(void);
void Module_SetOffScMode(void);
void Module_SetStandbyMode(void);
void Module_SetSleepMode(void);

#endif //MODULES_H_
