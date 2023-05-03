#include "./SAMC21E17A_hal.h"

/*==================== HAL Module APIs =======================*/
void hal_halt(void)
{
	GPIO_InitTypeDef GPIO_Initstruct = {0};
	GPIO_Initstruct.Pin = GPIO_PIN_2;
	GPIO_Initstruct.Mode = GPIO_MODE_INPUT;
	GPIO_Initstruct.Pull = GPIO_PULLUP;
	GPIO_Initstruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_Initstruct);

	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW);

	__HAL_RCC_PWR_CLK_ENABLE();
	SLEEP();
}

void hal_close(void)
{
}

void hal_flush(void)
{
	CLRWDT();
}
