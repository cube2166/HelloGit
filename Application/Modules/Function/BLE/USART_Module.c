#include "./USART_Module.h"

static uint8_t sercom_rxTmp[1];
static bool processedFlag = false;

/*=============================== USART buffer ======================================================*/
#define BUFFER_LEN 512
typedef struct
{
	uint8_t array[BUFFER_LEN];
	volatile uint16_t front;
	volatile uint16_t back;
} USART_Buffer;
static USART_Buffer rx_buffer;

static bool USART_BufferIsEmpty()
{
	bool tmp = false;
	if (rx_buffer.back == rx_buffer.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static bool USART_BufferIsFull()
{
	bool tmp = false;
	if (((rx_buffer.back + (uint16_t)1) % (uint16_t)BUFFER_LEN) == rx_buffer.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static void USART_BufferPush(uint8_t data)
{
	bool tmp_Result;

	tmp_Result = USART_BufferIsFull();
	if (tmp_Result == true)
	{
	}
	else
	{
		rx_buffer.array[rx_buffer.back] = data;
		rx_buffer.back = ((rx_buffer.back + (uint16_t)1) % (uint16_t)BUFFER_LEN);
	}
}

static bool USART_BufferPop(uint8_t *data)
{
	bool tmp = false;
	bool tmp_Result;

	tmp_Result = USART_BufferIsEmpty();
	if (tmp_Result == true)
	{
		tmp = false;
	}
	else
	{
		tmp = true;
		*data = rx_buffer.array[rx_buffer.front];
		rx_buffer.front = ((rx_buffer.front + (uint16_t)1) % (uint16_t)BUFFER_LEN);
	}
	return tmp;
}

void USART_BufferReset(void)
{
	rx_buffer.back = 0;
	rx_buffer.front = 0;
}

/*=============================== USART queue ======================================================*/

typedef struct
{
	BLEP_FUNC_PTR funcPtr;
	BLEP_PackageInfoTypedef *info;
} USART_Node;

#define NODE_LEN 20
typedef struct
{
	USART_Node node_array[NODE_LEN];
	uint8_t front;
	uint8_t back;
} USART_Queue;

static USART_Node ur_node;
static USART_Queue ur_queue;

static bool USART_QueueIsEmpty()
{
	bool tmp = false;
	if (ur_queue.back == ur_queue.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static bool USART_QueueIsFull()
{
	bool tmp = false;
	if (((ur_queue.back + (uint8_t)1) % (uint8_t)NODE_LEN) == ur_queue.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static void USART_QueuePush(BLEP_FUNC_PTR funcPtr, BLEP_PackageInfoTypedef *info)
{
	if (USART_QueueIsFull() == true)
	{
	}
	else
	{
		ur_queue.node_array[ur_queue.back].funcPtr = funcPtr;
		ur_queue.node_array[ur_queue.back].info = info;
		ur_queue.back = ((ur_queue.back + (uint8_t)1) % (uint8_t)NODE_LEN);
	}
}

static bool USART_QueuePop(USART_Node *node)
{
	bool tmp = false;
	if (USART_QueueIsEmpty() == true)
	{
		tmp = false;
	}
	else
	{
		tmp = true;
		node->funcPtr = ur_queue.node_array[ur_queue.front].funcPtr;
		node->info = ur_queue.node_array[ur_queue.front].info;
		ur_queue.front = ((ur_queue.front + (uint8_t)1) % (uint8_t)NODE_LEN);
	}
	return tmp;
}

void USART_QueueReset(void)
{
	ur_queue.back = 0;
	ur_queue.front = 0;
}

/*-------------------------------------------------------------------------------------------------------*/

/*=============================== USART function ======================================================*/

void USART_RX_Callback(void)
{
	USART_BufferPush(sercom_rxTmp[0]);
	processedFlag = BSP_USART_Read(sercom_rxTmp, 1);
}

void USART_RX_ErrorCallback(void)
{
	processedFlag = BSP_USART_Read(sercom_rxTmp, 1);
}

void USART_SendData(BLEP_FUNC_PTR funcPtr, BLEP_PackageInfoTypedef *infoPtr)
{
	USART_QueuePush(funcPtr, infoPtr);
}

bool USART_ReadData(uint8_t *data)
{
	bool status = false;
	bool tmp_Result;

	tmp_Result = USART_BufferPop(data);
	if (tmp_Result == true)
	{
		status = true;
	}
	else
	{
		status = false;
	}

	return status;
}


/*========================================================================================*/

static ModuleState_TypeDef USARTModule_state = MODULESTATE_RESET;
void USART_SoftwareReset(void)
{
	USARTModule_state = MODULESTATE_INITIALIZED;

	USART_QueueReset();
	USART_BufferReset();
	BSP_USART_Close();
}

void USART_init(void)
{
	USARTModule_state = MODULESTATE_INITIALIZED;

	BSP_USART_Open();
	processedFlag = BSP_USART_Read(sercom_rxTmp, 1);
	USART_QueueReset();
	USART_BufferReset();
}

void USART_open(void)
{
	if (USARTModule_state != MODULESTATE_OPENED)
	{
		USARTModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		BSP_USART_Open();
		processedFlag = BSP_USART_Read(sercom_rxTmp, 1);
		USART_QueueReset();
		USART_BufferReset();
	}
	else
	{
		/* nothing */
	}
}

void USART_close(void)
{
	if (USARTModule_state != MODULESTATE_CLOSED)
	{
		USARTModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/

		USART_QueueReset();
		USART_BufferReset();
		/* Disable the USART before configurations */
		BSP_USART_Close();
	}
	else
	{
		/* nothing */
	}
}

void USART_flush(void)
{
	bool tmp_Result;
	if (USARTModule_state == MODULESTATE_OPENED)
	{
		if(processedFlag == false)
		{
			processedFlag = BSP_USART_Read(sercom_rxTmp, 1);
		}
		else
		{
			/* nothing */
		}
		
		tmp_Result = BSP_USART_WriteIsBusy();
		if (tmp_Result == false)
		{
			tmp_Result = USART_QueuePop(&ur_node);
			if (tmp_Result == true)
			{
				if (ur_node.funcPtr != 0)
				{
					uint8_t *buffer;
					uint8_t size;
					ur_node.funcPtr(&buffer, &size, ur_node.info);
					BSP_USART_Write(buffer, (size_t)size);
#ifdef RTT_OUT
					uint8_t ind = 0;
					uint16_t sysms = SimpleOS_GetTick();
					if (sysms < 10)
					{
						RTT_PRINT_2("(0000%d) ", sysms);
					}
					else if (sysms < 100)
					{
						RTT_PRINT_2("(000%d) ", sysms);
					}
					else if (sysms < 1000)
					{
						RTT_PRINT_2("(00%d) ", sysms);
					}
					else if (sysms < 10000)
					{
						RTT_PRINT_2("(0%d) ", sysms);
					}
					else
					{
						RTT_PRINT_2("(%d) ", sysms);
					}

					RTT_PRINT_2("Tx: ");

					for (ind = 0; ind < size; ind++)
					{
						if (buffer[ind] < 0x10)
						{
							RTT_PRINT_2("0%x ", buffer[ind]);
						}
						else
						{
							RTT_PRINT_2("%x ", buffer[ind]);
						}
					}
					RTT_PRINT_2("\n");
#endif
				}
				else
				{
					/* ignore */
				}
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
	else
	{
		/* nothing */
	}
}
