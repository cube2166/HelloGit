#include "./Board_Module.h"

bool LINK_GetVal(void)
{
	bool tmp = false;
	GPIO_PinState pinState = HAL_GPIO_ReadPin(LINK_GPIO_Port, LINK_Pin);
	if (pinState == GPIO_PIN_SET)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

void PWR_Ctrl_PutVal(bool onoff)
{
	if (onoff == true)
	{
		HAL_GPIO_WritePin(PWR_CTRL_GPIO_Port, PWR_CTRL_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(PWR_CTRL_GPIO_Port, PWR_CTRL_Pin, GPIO_PIN_SET);
	}
}

void BT_Reset_PutVal(bool onoff)
{
	if (onoff == true)
	{
		HAL_GPIO_WritePin(BT_RESET_GPIO_Port, BT_RESET_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(BT_RESET_GPIO_Port, BT_RESET_Pin, GPIO_PIN_SET);
	}
}

void CAN_Set_PutVal(bool onoff)
{
	if (onoff == true)
	{
		HAL_GPIO_WritePin(CAN_SEL_GPIO_Port, CAN_SEL_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(CAN_SEL_GPIO_Port, CAN_SEL_Pin, GPIO_PIN_SET);
	}
}

/*----------------------------------------------------------------------*/
static ModuleState_TypeDef boardModule_stat = MODULESTATE_RESET;

void Board_init(void)
{
	boardModule_stat = MODULESTATE_INITIALIZED;
}

void Board_open(void)
{
	if (boardModule_stat != MODULESTATE_OPENED)
	{
		boardModule_stat = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		PWR_Ctrl_PutVal(true);
		BT_Reset_PutVal(true);
	}
	else
	{
	}
}

void Board_close(void)
{
	if (boardModule_stat != MODULESTATE_CLOSED)
	{
		boardModule_stat = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
		PWR_Ctrl_PutVal(false);
		BT_Reset_PutVal(true);
	}
	else
	{
	}
}

void Board_flush(void)
{
	if (boardModule_stat == MODULESTATE_OPENED)
	{
		/* Write the module flash code here	*/
	}
	else
	{
	}
}
