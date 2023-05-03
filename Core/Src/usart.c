/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "../../Application/Modules/Modules.h"

static bool usart_inited = false;

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  if (usart_inited == true)
    return;

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  usart_inited = true;

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA0     ------> USART2_CTS
    PA1     ------> USART2_RTS
    PA15     ------> USART2_RX
    PB3     ------> USART2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA0     ------> USART2_CTS
    PA1     ------> USART2_RTS
    PA15     ------> USART2_RX
    PB3     ------> USART2_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

    usart_inited = false;

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

__weak void USART_RX_Callback(void)
{
}

__weak void USART_RX_ErrorCallback(void)
{
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    USART_RX_Callback();
  }
  else
  {
    /* nothing */
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  uint32_t isrflags = READ_REG(huart->Instance->ISR);
  if (isrflags != 0)
  {
    /* nothing */
  }

  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_PE)) != RESET)
  {
    __HAL_UART_CLEAR_PEFLAG(huart);
  }
  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_FE)) != RESET)
  {
    __HAL_UART_CLEAR_FEFLAG(huart);
  }

  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_NE)) != RESET)
  {
    __HAL_UART_CLEAR_NEFLAG(huart);
  }

  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) != RESET)
  {
    __HAL_UART_CLEAR_OREFLAG(huart);
  }
  USART_RX_ErrorCallback();
}

bool BSP_USART_Read(uint8_t *buffer, const size_t size)
{
  bool tmp = true;
  if (HAL_UART_Receive_IT(&huart2, buffer, size) != HAL_OK)
  {
    tmp = false;
  }
  return tmp;
}

void BSP_USART_Write(uint8_t *buffer, const size_t size)
{
  while (huart2.Lock ==  HAL_LOCKED){};
  HAL_UART_Transmit_IT(&huart2, buffer, size);
}

bool BSP_USART_WriteIsBusy(void)
{
  bool result = false;

  if ((huart2.gState & HAL_UART_STATE_READY) != HAL_UART_STATE_READY)
  {
    /* not init */
    result = true;
  }
  else
  {
    if ((huart2.gState & HAL_UART_STATE_BUSY_TX) == HAL_UART_STATE_BUSY_TX)
    {
      /* Tx Busy */
      result = true;
    }
    else
    {
      result = false;
    }
  }
  return result;
}

void BSP_USART_Close(void)
{
  HAL_UART_DeInit(&huart2);
}

void BSP_USART_Open(void)
{
  MX_USART2_UART_Init();
}

/* USER CODE END 1 */
