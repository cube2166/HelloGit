#include "./Modules.h"

void Module_Initialize(void)
{
	Board_init();
	Eeprom_init();
	CAN_init();
	BLE_init();
	BLEP_init();
	USART_init();
	DIAG_init();
	BTN_init();
}

void Module_SetActiveMode(void)
{
	Board_open();
	Eeprom_open();
	CAN_open();
	BLE_open();
	BLEP_open();
	USART_open();
	DIAG_open();
	BTN_open();
}

void Module_SetOffScMode(void)
{
	Board_close();
	Eeprom_close();
	DIAG_close();
	CAN_open();
	BLE_close();
	BLEP_close();
	USART_close();
	BTN_close();
}

void Module_SetStandbyMode(void)
{
	Board_open();
	Eeprom_open();
	DIAG_close();
	CAN_close();
	BLE_close();
	BLEP_open();
	USART_open();
	BTN_close();
}

void Module_SetSleepMode(void)
{
	Board_close();
	Eeprom_close();
	DIAG_close();
	CAN_close();
	BLE_close();
	BLEP_close();
	USART_close();
	BTN_close();
}
