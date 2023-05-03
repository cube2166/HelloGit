/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
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
#include "fdcan.h"

/* USER CODE BEGIN 0 */

#define MX_FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define MX_FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */
#define MX_FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U)   /* Remote Transmission Request */
#define MX_FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U)   /* Extended Identifier         */
#define MX_FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U)   /* Error State Indicator       */
#define MX_FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU)    /* Timestamp                   */
#define MX_FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U)   /* Data Length Code            */
#define MX_FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U)   /* Bit Rate Switch             */
#define MX_FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U)   /* FD Format                   */
#define MX_FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U)   /* Event FIFO Control          */
#define MX_FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U)    /* Message Marker              */
#define MX_FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U)  /* Filter Index                */
#define MX_FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U)  /* Accepted Non-matching Frame */
#define MX_FDCAN_ELEMENT_MASK_ET    ((uint32_t)0x00C00000U)    /* Event type                  */

static bool can_inited = false;

typedef struct
{
  uint32_t id;
  uint8_t len;
  bool extend;
  uint8_t data[8];
} BSP_FDCAN_Node;

#define NODE_LEN 32
typedef struct
{
  BSP_FDCAN_Node node_array[NODE_LEN];
  volatile uint8_t front;
  volatile uint8_t back;
} BSP_FDCAN_Node_Queue;

static BSP_FDCAN_Node_Queue can_queue_rx_0;
static BSP_FDCAN_Node_Queue can_queue_rx_1;
static BSP_FDCAN_Node_Queue can_queue_tx_0;

static bool BSP_FDCAN_QueueIsEmpty(BSP_FDCAN_Node_Queue *pQueue)
{
  bool tmp = false;
  if (pQueue == 0)
  {
    tmp = false;
  }
  else
  {
    if (pQueue->back == pQueue->front)
    {
      tmp = true;
    }
    else
    {
      tmp = false;
    }
  }
  return tmp;
}

static bool BSP_FDCAN_QueueIsFull(BSP_FDCAN_Node_Queue *pQueue)
{
  bool tmp = false;
  if (pQueue == 0)
  {
    tmp = true;
  }
  else
  {
    if (((pQueue->back + (uint8_t)1) % (uint8_t)NODE_LEN) == pQueue->front)
    {
      tmp = true;
    }
    else
    {
      tmp = false;
    }
  }
  return tmp;
}

static bool BSP_FDCAN_QueuePush(BSP_FDCAN_Node_Queue *pQueue, BSP_FDCAN_Node *pNode)
{
  bool tmp = false;
  if (BSP_FDCAN_QueueIsFull(pQueue) == true)
  {
    tmp = false;
  }
  else
  {
    if (pNode == 0)
    {
      tmp = false;
    }
    else
    {
      memcpy(&pQueue->node_array[pQueue->back], pNode, sizeof(BSP_FDCAN_Node));
      pQueue->back = ((pQueue->back + (uint8_t)1) % (uint8_t)NODE_LEN);
      tmp = true;
    }
  }
  return tmp;
}

static bool BSP_FDCAN_QueuePop(BSP_FDCAN_Node_Queue *pQueue, BSP_FDCAN_Node **node)
{
  bool tmp = false;
  if (BSP_FDCAN_QueueIsEmpty(pQueue) == true)
  {
    tmp = false;
  }
  else
  {
    tmp = true;
    *node = &pQueue->node_array[pQueue->front];
    pQueue->front = ((pQueue->front + (uint8_t)1) % (uint8_t)NODE_LEN);
  }
  return tmp;
}

static void BSP_FDCAN_QueueReset(BSP_FDCAN_Node_Queue *pQueue)
{
  pQueue->back = 0;
  pQueue->front = 0;
}

/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  if (can_inited == true)
    return;

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV4;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 4;
  hfdcan1.Init.NominalTimeSeg1 = 18;
  hfdcan1.Init.NominalTimeSeg2 = 5;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 4;
  hfdcan1.Init.DataTimeSeg1 = 18;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 4;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  can_inited = true;

  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PA11     ------> FDCAN1_RX
    PA12     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PA11     ------> FDCAN1_RX
    PA12     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  can_inited = false;

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

__weak void BSP_FDCAN1_RxFifo0_ISR(uint32_t *id, uint8_t *rxsize, uint8_t *data)
{
}

__weak void BSP_FDCAN1_RxFifo1_ISR(uint32_t *id, uint8_t *rxsize, uint8_t *data)
{
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{

  if (hfdcan->Instance == FDCAN1)
  {
    RxFifo0ITs &= FDCAN_IT_RX_FIFO0_NEW_MESSAGE;
    if (RxFifo0ITs != 0)
    {
      FDCAN_RxHeaderTypeDef RxHeader;
      BSP_FDCAN_Node node;

      HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, node.data);

      node.id = RxHeader.Identifier;
      node.len = ((RxHeader.DataLength & MX_FDCAN_ELEMENT_MASK_DLC) >> 16);
      (void)BSP_FDCAN_QueuePush(&can_queue_rx_0, &node);

      BSP_FDCAN1_RxFifo0_ISR(&node.id, &node.len, node.data);
    }
    else
    {
      /* nothing */
    }
  }
  else
  {
    /* nothing */
  }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
  if (hfdcan->Instance == FDCAN1)
  {
    RxFifo1ITs &= FDCAN_IT_RX_FIFO1_NEW_MESSAGE;
    if (RxFifo1ITs != 0)
    {
      FDCAN_RxHeaderTypeDef RxHeader;
      BSP_FDCAN_Node node;

      HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader, node.data);

      node.id = RxHeader.Identifier;
      node.len = ((RxHeader.DataLength & MX_FDCAN_ELEMENT_MASK_DLC) >> 16);
      (void)BSP_FDCAN_QueuePush(&can_queue_rx_1, &node);

      BSP_FDCAN1_RxFifo1_ISR(&node.id, &node.len, node.data);
    }
    else
    {
      /* nothing */
    }
  }
  else
  {
    /* nothing */
  }
}

void BSP_FDCAN1_Init(void)
{
  MX_FDCAN1_Init();
}

void BSP_FDCAN1_Open(void)
{
  HAL_StatusTypeDef status;

  FDCAN_FilterTypeDef sFilterConfig;
  /* Configure Rx filter */
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  sFilterConfig.FilterID1 = 0x7e8;
  sFilterConfig.FilterID2 = 0x7ef;

  status = HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 1;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  sFilterConfig.FilterID1 = 0x7d8;
  sFilterConfig.FilterID2 = 0x7de;

  status = HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 2;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x7ca;
  sFilterConfig.FilterID2 = 0x7ff;

  status = HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 3;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x200;
  sFilterConfig.FilterID2 = 0x6ff;

  status = HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan1, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_BLOCKING);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan1, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_BLOCKING);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }

  status = HAL_FDCAN_Start(&hfdcan1);
  if (status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* nothing */
  }
}

void BSP_FDCAN1_Close(void)
{
  HAL_FDCAN_DeInit(&hfdcan1);
}

void BSP_FDCAN1_Change_Mode(uint8_t mode)
{
  BSP_FDCAN1_Close();

  if (mode == 0)
  {
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  }
  else
  {
    hfdcan1.Init.Mode = FDCAN_MODE_BUS_MONITORING;
  }

  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }

  BSP_FDCAN1_Open();
}

static bool HAL_FDCAN1_Send_Data(uint32_t *id, uint8_t *rxsize, uint8_t *data, bool extend)
{
  bool tmp_Result = false;
  uint32_t tmp_Len = *rxsize;
  uint32_t tmp_Id = *id;
  FDCAN_TxHeaderTypeDef txHeader;
  tmp_Len <<= 16;

  HAL_StatusTypeDef tmp_Status;

  /* Prepare Tx Header */
  if (extend == true)
  {
    txHeader.IdType = FDCAN_EXTENDED_ID;
  }
  else
  {
    txHeader.IdType = FDCAN_STANDARD_ID;
    tmp_Id &= (uint32_t)0x7FF;
  }
  txHeader.Identifier = tmp_Id;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = tmp_Len;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  tmp_Status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, data);
  if (tmp_Status != HAL_OK)
  {
    tmp_Result = false;
  }
  else
  {
    tmp_Result = true;
  }
  return tmp_Result;
}

void BSP_FDCAN1_Tx_Flush(void)
{
  bool status = BSP_FDCAN_QueueIsEmpty(&can_queue_tx_0);
  BSP_FDCAN_Node *node = 0;
  while (status != true)
  {
    if (hfdcan1.State == HAL_FDCAN_STATE_BUSY)
    {
      /* Check that the Tx FIFO/Queue is not full */
      if ((hfdcan1.Instance->TXFQS & FDCAN_TXFQS_TFQF) != 0U)
      {
        /* Tx full */
        break;
      }
      else
      {
        /* Tx no full */
        status = BSP_FDCAN_QueuePop(&can_queue_tx_0, &node);

        if (status == true)
        {
          /* pop ok */
          status = HAL_FDCAN1_Send_Data(&node->id, &node->len, node->data, node->extend);
          if (status == true)
          {
            /* send ok */
            /* nothing */
          }
          else
          {
            /* send fail */
            (void)BSP_FDCAN_QueuePush(&can_queue_tx_0, node);
            break;
          }
        }
        else
        {
          /* pop fail */
          break;
        }
      }
    }
    else
    {
      /* Not start */
      break;
    }

    status = BSP_FDCAN_QueueIsEmpty(&can_queue_tx_0);
  }
}

bool BSP_FDCAN1_Send_Data(uint32_t *id, uint8_t *rxsize, uint8_t *data, bool extend)
{
  BSP_FDCAN_Node node;
  bool tmp;

  node.id = *id;
  node.len = *rxsize;
  node.extend = extend;
  memcpy(node.data, data, node.len);
  tmp = BSP_FDCAN_QueuePush(&can_queue_tx_0, &node);

  return tmp;
}

void BSP_FDCAN1_Reset_Rx_Queue_0(void)
{
  BSP_FDCAN_QueueReset(&can_queue_rx_0);
}

void BSP_FDCAN1_Reset_Rx_Queue_1(void)
{
  BSP_FDCAN_QueueReset(&can_queue_rx_1);
}

void BSP_FDCAN1_Reset_Tx_Queue(void)
{
  BSP_FDCAN_QueueReset(&can_queue_tx_0);
}

bool BSP_FDCAN1_Get_Rx_Queue_0(uint32_t *id, uint8_t *rxsize, uint8_t *data)
{
  bool tmp = false;
  BSP_FDCAN_Node *node = 0;

  tmp = BSP_FDCAN_QueuePop(&can_queue_rx_0, &node);

  if (tmp == true)
  {
    *id = node->id;
    *rxsize = node->len;
    memcpy(data, node->data, node->len);
  }
  else
  {
    /* nothing */
  }
  return tmp;
}

bool BSP_FDCAN1_Get_Rx_Queue_1(uint32_t *id, uint8_t *rxsize, uint8_t *data)
{
  bool tmp = false;
  BSP_FDCAN_Node *node = 0;

  tmp = BSP_FDCAN_QueuePop(&can_queue_rx_1, &node);

  if (tmp == true)
  {
    *id = node->id;
    *rxsize = node->len;
    memcpy(data, node->data, node->len);
  }
  else
  {
    /* nothing */
  }
  return tmp;
}

bool BSP_FDCAN1_Get_BusOff(void)
{
  HAL_StatusTypeDef tmp_Status;
  FDCAN_ProtocolStatusTypeDef tmp_ProtocolStatus;
  bool tmp_Result = false;
  tmp_Status = HAL_FDCAN_GetProtocolStatus(&hfdcan1, &tmp_ProtocolStatus);

  if (tmp_Status != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    if (tmp_ProtocolStatus.BusOff == (uint32_t)1)
    {
      tmp_Result = true;
    }
    else
    {
      tmp_Result = false;
    }
  }

  return tmp_Result;
}

/* USER CODE END 1 */
