#include "./DIAG_Module.h"

#define DIAG_STANDARD_ID 0X7E0
#define DIAG_EXTENDED_ID 0x18DB33F0

#define DIAG_EMPTY_FILL 0x55
#define DIAG_POS_ACK 0x40
#define DIAG_NEG_ACK (uint8_t) SID_NEGATIVE_RESPONSE

static uint32_t 		diag_Rx_Id; 		/* Diagnosis Receive ID */
static uint8_t 			diag_Rx_Len; 		/* Diagnosis Receive Length */
static uint8_t 			diag_Rx_Data[8]; 	/* Diagnosis Receive Data */
static uint16_t 		diag_Rx_DID; 		/* Diagnosis Receive Data Identifier */
static DIAGSID_TypeDef	diag_Rx_SID; 		/* Diagnosis Receive Service Identifier */
static DIAGNRC_TypeDef	diag_Rx_NRC; 		/* Diagnosis Receive Negative Response Code */

static uint32_t 		diag_Tx_ID; 		/* Diagnosis Transmit ID */
static uint8_t 			diag_Tx_Len; 		/* Diagnosis Transmit Length */
static uint8_t 			diag_Tx_Data[8]; 	/* Diagnosis Transmit Data */
static uint16_t 		diag_Tx_DID; 		/* Diagnosis Transmit Data Identifier */
static DIAGSID_TypeDef 	diag_Tx_SID; 		/* Diagnosis Transmit Service Identifier */

static uint8_t 			diag_Tx_Rpt; 		/* Diagnosis Transmit Repeat Counter */

static bool 			diag_Talk_Status; 	/* Diagnosis Talk Status */

static bool 			diag_Rx_CF_Status; 	/* Diagnosis Receive Continue Frame Status */
static uint16_t 		diag_Rx_CF_Len; 	/* Diagnosis Receive Continue Frame Data Length */
static uint16_t 		diag_Rx_Actual_Len; /* Diagnosis Receive Data Actual Length */
static uint8_t 			diag_Rx_CF_Sn; 		/* Diagnosis Receive Continue Frame Serial Number */

static uint8_t 			*diag_Data_Buf_Ptr; /* Diagnosis Data Buffer Pointer */
static uint8_t 			diag_Data_Buf[70]; 	/* Diagnosis Data Buffer */

static bool 			diag_Extend_Session;/* Diagnosis Session Status */

static bool 			search_Mode_SDS; 	/* Search Mode Send SDS Flag */

static uint16_t 		search_Mode_Time; 	/* Search Mode Time */
static uint16_t 		reset_Mode_Time; 	/* Reset Mode Time */
static uint16_t 		session_Mode_Time; 	/* Session Mode Time */
static uint16_t 		ffd02_Mode_Time; 	/* FFD02 Mode Time */
static uint16_t 		commandID_Mode_Time; /* Command Mode Time */

static uint16_t 		timeOut_Time; 		/* TimeOut Time */

static bool 			session_Mode_Toggle; /* Session Mode Toggle */

static bool 			testerPresent_Flag; /* Tester Present TimeOut Flag */
static uint16_t 		testerPresent_Time;	/* Tester Present Time */

static uint16_t 		diag_Time; 			/* Diagnosis Time */
static uint16_t 		diag_Max_Time; 		/* Diagnosis Max Time */

static bool 			commandID_Request; 	/* From Ble Request Command ID Data */
static bool 			commandID_Finish; 	/* Diagnosis Command ID Query Complete */
static DIAGSTATUS_TypeDef commandID_Status; /* To Ble Command ID Status */

static bool 			ffd01_Request; 		/* From Ble Request FFD01 Data */
static bool 			ffd01_Finish; 		/* Diagnosis FFD01 Query Complete */
static uint16_t 		ffd01_Ask_ID; 		/* Diagnosis FFD01 Ask ID */
static DIAGSTATUS_TypeDef ffd01_Status; 	/* To Ble FFD01 Status */

static bool 			ffd02_Request; 		/* From Ble Request FFD02 Data */
static bool 			ffd02_Finish; 		/* Diagnosis FFD02 Query Complete */
static uint16_t 		ffd02_Ask_ID; 		/* Diagnosis FFD02 Ask ID */
static DIAGSTATUS_TypeDef ffd02_Status; 	/* To Ble FFD02 Status */

static bool 			ffd03_Request; 		/* From Ble Request FFD03 Data */
static bool 			ffd03_Finish; 		/* Diagnosis FFD03 Query Complete */
static uint16_t 		ffd03_Ask_ID; 		/* Diagnosis FFD03 Ask ID */
static DIAGSTATUS_TypeDef ffd03_Status; 	/* To Ble FFD03 Status */

/* ==================================CID DATA================================== */
static uint8_t 			cid_Vin_Data[CID_VIN_LEN];
static uint8_t 			cid_Ecu_Software_Number_Data[CID_ECU_SOFTWARE_NUMBER_LEN];
static uint8_t 			cid_Registered_Model_Data[CID_REGISTERED_MODEL_LEN];
static uint8_t 			cid_Ecu_Energization_Total_Time_Data[CID_ECU_ENERGIZATION_TOTAL_TIME_LEN];
static uint8_t 			cid_Ecu_Count_Data[CID_ECU_COUNT_LEN];
static uint8_t 			cid_Travel_Distance_Data[CID_TRAVEL_DISTANCE_LEN];
static uint8_t 			cid_Eg_Operation_Time_Data[CID_EG_OPERATION_TIME_LEN];
/* ---------------------------------------------------------------------------- */

/* ==================================LID DATA================================== */
static uint8_t 			lid_h01_Data[LID_H01_LEN];
static uint8_t 			lid_h48_Data[LID_H48_LEN];
static bool 			lid_h01_Asked;
static bool 			lid_h48_Asked;
/* ---------------------------------------------------------------------------- */

/* ==================================FFD DATA================================== */
#define FFD_BUFFER_COUNT 5

static uint8_t 			ffd_Buf_1_Data[70];
static uint8_t 			ffd_Buf_2_Data[70];
static uint8_t 			ffd_Buf_3_Data[70];
static uint8_t 			ffd_Buf_4_Data[70];
static uint8_t 			ffd_Buf_5_Data[70];
static uint8_t 			*ffd_Buf_Array[] = {ffd_Buf_1_Data, ffd_Buf_2_Data, ffd_Buf_3_Data, 
											ffd_Buf_4_Data, ffd_Buf_5_Data};
static uint8_t 			ffd_Buf_Cnt = 0;
/* ---------------------------------------------------------------------------- */

typedef enum
{
	DIAGSTATE_RESET = 0,
	DIAGSTATE_SEARCH,
	DIAGSTATE_STANDBY,
	DIAGSTATE_SESSION,
	DIAGSTATE_COMMANDID,
	DIAGSTATE_FFD01,
	DIAGSTATE_FFD02,
	DIAGSTATE_FFD03,
} DIAGState_TypeDef;

static DIAGState_TypeDef diag_State;	/* DIAG_Module Current State */
static DIAGState_TypeDef diag_SetState; /* DIAG_Module Next State */

static bool DIAG_Get_FIFOElement(void)
{
	bool status = false;

	status = BSP_FDCAN1_Get_Rx_Queue_1(&diag_Rx_Id, &diag_Rx_Len, diag_Rx_Data);

	return status;
}

/* =================================DIAG Queue================================= */

typedef struct
{
	DIAGSID_TypeDef sid;
	uint16_t data;
	uint32_t id;
} UDS_Node;

#define NODE_LEN 80
typedef struct
{
	UDS_Node node_array[NODE_LEN];
	uint8_t front;
	uint8_t back;
} UDS_Queue;

static UDS_Node *uds_node;
static UDS_Queue uds_queue;

static bool DIAG_QueueIsEmpty(void)
{
	bool tmp = false;
	if (uds_queue.back == uds_queue.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static bool DIAG_QueueIsFull(void)
{
	bool tmp = false;
	if (((uds_queue.back + (uint8_t)1) % (uint8_t)NODE_LEN) == uds_queue.front)
	{
		tmp = true;
	}
	else
	{
		tmp = false;
	}
	return tmp;
}

static void DIAG_QueuePush(uint32_t id, DIAGSID_TypeDef sid, uint16_t data)
{
	if (DIAG_QueueIsFull() == true)
	{
	}
	else
	{
		uds_queue.node_array[uds_queue.back].sid = sid;
		uds_queue.node_array[uds_queue.back].data = data;
		uds_queue.node_array[uds_queue.back].id = id;
		uds_queue.back = ((uds_queue.back + (uint8_t)1) % (uint8_t)NODE_LEN);
	}
}

static bool DIAG_QueuePop(UDS_Node **node)
{
	bool tmp = false;
	if (DIAG_QueueIsEmpty() == true)
	{
		tmp = false;
	}
	else
	{
		tmp = true;
		*node = &uds_queue.node_array[uds_queue.front];
		uds_queue.front = ((uds_queue.front + (uint8_t)1) % (uint8_t)NODE_LEN);
	}
	return tmp;
}

static void DIAG_QueueReset(void)
{
	uds_queue.back = 0;
	uds_queue.front = 0;
	uds_node = 0;
}
/* ---------------------------------------------------------------------------- */

static void DIAG_Set_PUD_TimeOut(uint16_t val)
{
	diag_Time = SimpleOS_GetTick();
	diag_Max_Time = val;
}

static void DIAG_Send_PUD(uint32_t id)
{
	diag_Tx_ID = id;
	diag_Tx_Len = 8;
	(void)CAN_Send_DATA(diag_Tx_ID, diag_Tx_Len, diag_Tx_Data);
	diag_Talk_Status = true;
	DIAG_Set_PUD_TimeOut(50);
}

static void DIAG_ReSend_PUD(void)
{
	(void)CAN_Send_DATA(diag_Tx_ID, diag_Tx_Len, diag_Tx_Data);
}

static void DIAG_Send_FlowControl(DIAGFC_TypeDef fs, uint8_t bs, uint8_t stmin)
{
	uint8_t array[8];
	(void)memset(array, DIAG_EMPTY_FILL, (size_t)8);
	array[0] = (uint8_t)0x30;
	array[0] |= (uint8_t)fs;
	array[1] = bs;
	array[2] = stmin;
	(void)CAN_Send_DATA(diag_Tx_ID, 8, array);
}

static void DIAG_Send_TesterPresent(DIAGSID_TypeDef sid, uint16_t data)
{
	uint16_t len_val = 0;
	uint8_t array[8];
	(void)memset(array, DIAG_EMPTY_FILL, (size_t)8);
	array[0] = (uint8_t)PCI_SINGLE_FRAME;
	array[1] = (uint8_t)sid;
	len_val = len_val + 1;
	array[2] = U16_LSB(data);
	len_val = len_val + 1;
	len_val &= (uint16_t)0x0F;
	array[0] |= (uint16_t)len_val;
	(void)CAN_Send_DATA(diag_Tx_ID, 8, array);
}

static void DIAG_Handle_CID_Eg_Operation_Time(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_EG_OPERATION_TIME_LEN)
	{
		(void)memcpy(cid_Eg_Operation_Time_Data, data, (size_t)CID_EG_OPERATION_TIME_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
		commandID_Finish = true;
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Travel_Distance(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_TRAVEL_DISTANCE_LEN)
	{
		(void)memcpy(cid_Travel_Distance_Data, data, (size_t)CID_TRAVEL_DISTANCE_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Ecu_Count(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_ECU_COUNT_LEN)
	{
		(void)memcpy(cid_Ecu_Count_Data, data, (size_t)CID_ECU_COUNT_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Ecu_Energization_Total_Time(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_ECU_ENERGIZATION_TOTAL_TIME_LEN)
	{
		(void)memcpy(cid_Ecu_Energization_Total_Time_Data, data, (size_t)CID_ECU_ENERGIZATION_TOTAL_TIME_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Registered_Model(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_REGISTERED_MODEL_LEN)
	{
		(void)memcpy(cid_Registered_Model_Data, data, (size_t)CID_REGISTERED_MODEL_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Ecu_Software_Number(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_ECU_SOFTWARE_NUMBER_LEN)
	{
		(void)memcpy(cid_Ecu_Software_Number_Data, data, (size_t)CID_ECU_SOFTWARE_NUMBER_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_CID_Vin(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)CID_VIN_LEN)
	{
		(void)memcpy(cid_Vin_Data, data, (size_t)CID_VIN_LEN);
		commandID_Mode_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Handle_LID_H01(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)LID_H01_LEN)
	{
		(void)memcpy(lid_h01_Data, data, (size_t)LID_H01_LEN);
		lid_h01_Asked = true;
	}
	else
	{
		/* ignore */
	}
}

static uint32_t vehicle_ODO = 0;
static uint32_t accu_ODO = 0;
static void DIAG_Handle_LID_H48(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)LID_H48_LEN)
	{
		(void)memcpy(lid_h48_Data, data, (size_t)LID_H48_LEN);

		lid_h48_Asked = true;

		uint8_t ind = 0;
		uint32_t current_ODO = 0;
		uint32_t tmp_ODO = 0;
		uint8_t *h48_Ptr = data;
		for (ind = 0; ind < 4; ind++)
		{
			uint32_t shift = (3 - ind) * 8;
			uint32_t tmp = (uint32_t)h48_Ptr[ind] << shift;
			if (((uint32_t)0xFFFFFFFF - current_ODO) >= tmp)
			{
				current_ODO = current_ODO + tmp;
			}
			else
			{
				current_ODO = (uint32_t)0xFFFFFFFF;
			}
		}

		if (current_ODO >= vehicle_ODO)
		{
			tmp_ODO = (current_ODO - vehicle_ODO);
		}
		else
		{
			if (vehicle_ODO >= current_ODO)
			{
				tmp_ODO = (vehicle_ODO - current_ODO);
			}
			else
			{
				tmp_ODO = 0;
			}
		}

		vehicle_ODO = current_ODO;

		if (((uint32_t)0xFFFFFFFF - accu_ODO) >= tmp_ODO)
		{
			accu_ODO = accu_ODO + tmp_ODO;
		}
		else
		{
			accu_ODO = (uint32_t)0xFFFFFFFF;
		}
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_Add_FFD01_Group(void)
{
	uint16_t group = ((ffd01_Ask_ID & (uint16_t)0x00F0) >> (uint16_t)4);
	group = group + 1;
	if (group > (uint16_t)LID_FFD1_GROUP_END)
	{
		DIAG_QueueReset();
		ffd01_Finish = true;
	}
	else
	{
		DIAG_QueueReset();
		group <<= (uint16_t)4;
		group |= (uint16_t)0xFF00;
		ffd01_Ask_ID = group;
	}
}

static void DIAG_Handle_FFD01(uint16_t did, uint8_t *data, uint8_t len)
{
	ffd_Buf_Cnt = ffd_Buf_Cnt + 1;
	if (ffd_Buf_Cnt >= FFD_BUFFER_COUNT)
	{
		ffd_Buf_Cnt = 0;
	}
	else
	{
		/* nothing */
	}

	uint8_t *ptr = ffd_Buf_Array[ffd_Buf_Cnt];

	(void)memset(ptr, 0, 70);
	(void)memcpy(ptr, data, len);
	BLE_Send_FFDdata(did, ptr, ffd_Buf_Cnt, 1);
	if ((did & (uint16_t)0x000F) == (uint16_t)LID_FFD1_ITEM_END)
	{
		DIAG_Add_FFD01_Group();
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_Handle_FFD02(uint16_t did, uint8_t *data, uint8_t len)
{
	ffd_Buf_Cnt = ffd_Buf_Cnt + 1;
	if (ffd_Buf_Cnt >= FFD_BUFFER_COUNT)
	{
		ffd_Buf_Cnt = 0;
	}
	else
	{
		/* nothing */
	}

	uint8_t *ptr = ffd_Buf_Array[ffd_Buf_Cnt];
	did |= (uint16_t)0xFF00;

	(void)memset(ptr, 0, 70);
	(void)memcpy(ptr, data, len);
	BLE_Send_FFDdata(did, ptr, ffd_Buf_Cnt, 2);
	if (did == (uint16_t)0xFF7F)
	{
		ffd02_Finish = true;
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_Handle_FFD03(uint16_t did, uint8_t *data, uint8_t len)
{
	ffd_Buf_Cnt = ffd_Buf_Cnt + 1;
	if (ffd_Buf_Cnt >= FFD_BUFFER_COUNT)
	{
		ffd_Buf_Cnt = 0;
	}
	else
	{
		/* nothing */
	}

	uint8_t *ptr = ffd_Buf_Array[ffd_Buf_Cnt];
	did |= (uint16_t)0xFF00;

	(void)memset(ptr, 0, 70);
	(void)memcpy(ptr, data, len);
	BLE_Send_FFDdata(did, ptr, ffd_Buf_Cnt, 3);
	if (did == (uint16_t)0xFF09)
	{
		ffd03_Finish = true;
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_Handle_PUD(uint16_t did, uint8_t *src_dataPtr, uint16_t pud_dataLne, uint8_t src_dataLen, uint8_t sn, DIAGPCI_TypeDef pci)
{
	bool data_Finish = false;

	if (pci == PCI_SINGLE_FRAME)
	{
		if (diag_Rx_CF_Status == false)
		{
			/* single frame */
			diag_Rx_Actual_Len = pud_dataLne;

			(void)memcpy(diag_Data_Buf, src_dataPtr, src_dataLen);

			diag_Talk_Status = false;
			DIAG_Set_PUD_TimeOut(0);

			/* process id in this */
			data_Finish = true;
		}
		else
		{
			/* ignore */
		}
	}
	else if (pci == PCI_FIRST_FRAME)
	{
		if (diag_Rx_CF_Status == false)
		{
			if (pud_dataLne <= src_dataLen)
			{
				DIAG_Send_FlowControl(FC_FS_FFDL, 0, 0);
				DIAG_Set_PUD_TimeOut(0);
			}
			else if (pud_dataLne <= (uint16_t)70)
			{
				/* first frame */
				diag_Rx_Actual_Len = pud_dataLne;
				diag_Rx_CF_Len = pud_dataLne;

				diag_Rx_CF_Status = true;
				diag_Rx_CF_Sn = 0;
				diag_Data_Buf_Ptr = diag_Data_Buf;

				(void)memcpy(diag_Data_Buf, src_dataPtr, src_dataLen);
				diag_Data_Buf_Ptr += src_dataLen;
				diag_Rx_CF_Len = diag_Rx_CF_Len - src_dataLen;
				diag_Rx_CF_Sn = diag_Rx_CF_Sn + 1;

				DIAG_Send_FlowControl(FC_FS_BS, 0, 0);
				DIAG_Set_PUD_TimeOut(150);
			}
			else
			{
				DIAG_Send_FlowControl(FC_FS_FFDL, 0, 0);
				DIAG_Set_PUD_TimeOut(0);
			}
		}
		else
		{
			/* ignore */
		}
	}
	else if (pci == PCI_CONSECUTIVE_FRAME)
	{
		if (diag_Rx_CF_Status == true)
		{
			/* continue frame */
			if (diag_Rx_CF_Sn == sn)
			{
				if (diag_Rx_CF_Len >= (uint16_t)7)
				{
					(void)memcpy(diag_Data_Buf_Ptr, src_dataPtr, 7);
					diag_Data_Buf_Ptr += (uint8_t)7;
					diag_Rx_CF_Len -= (uint16_t)7;
					diag_Rx_CF_Sn = diag_Rx_CF_Sn + 1;
					diag_Rx_CF_Sn = diag_Rx_CF_Sn & 0xF;
					DIAG_Set_PUD_TimeOut(150);

					if (diag_Rx_CF_Len == (uint16_t)0)
					{
						diag_Rx_CF_Status = false;
						diag_Data_Buf_Ptr = 0;
						diag_Rx_CF_Sn = 0;

						diag_Talk_Status = false;
						DIAG_Set_PUD_TimeOut(0);

						/* process id in this */
						data_Finish = true;
					}
					else
					{
						/* ignore */
					}
				}
				else
				{
					(void)memcpy(diag_Data_Buf_Ptr, src_dataPtr, diag_Rx_CF_Len);
					diag_Rx_CF_Len = 0;

					diag_Rx_CF_Status = false;
					diag_Data_Buf_Ptr = 0;
					diag_Rx_CF_Sn = 0;

					diag_Talk_Status = false;
					DIAG_Set_PUD_TimeOut(0);

					/* process id in this */
					data_Finish = true;
				}
			}
			else
			{
				diag_Rx_CF_Sn = (uint8_t)0xFF;
				DIAG_Set_PUD_TimeOut(150);
				(void)memset(diag_Data_Buf_Ptr, 0, 70);
			}
		}
		else
		{
			/* ignore */
		}
	}
	else
	{
		/* ignore */
	}

	if (data_Finish == true)
	{
		data_Finish = false;
		if (did == (uint16_t)CID_VIN)
		{
			DIAG_Handle_CID_Vin(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_ECU_SOFTWARE_NUMBER)
		{
			DIAG_Handle_CID_Ecu_Software_Number(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_REGISTERED_MODEL)
		{
			DIAG_Handle_CID_Registered_Model(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_ECU_ENERGIZATION_TOTAL_TIME)
		{
			DIAG_Handle_CID_Ecu_Energization_Total_Time(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_ECU_COUNT)
		{
			DIAG_Handle_CID_Ecu_Count(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_TRAVEL_DISTANCE)
		{
			DIAG_Handle_CID_Travel_Distance(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)CID_EG_OPERATION_TIME)
		{
			DIAG_Handle_CID_Eg_Operation_Time(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)LID_H01)
		{
			DIAG_Handle_LID_H01(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else if (did == (uint16_t)LID_H48)
		{
			DIAG_Handle_LID_H48(diag_Data_Buf, diag_Rx_Actual_Len);
		}
		else
		{
			if ((did >= (uint16_t)LID_FFD2_START) && (did <= (uint16_t)LID_FFD2_LAST))
			{
				DIAG_Handle_FFD02(did, diag_Data_Buf, diag_Rx_Actual_Len);
			}
			else if ((did & (uint16_t)0xFF00) == (uint16_t)0xFF00)
			{
				DIAG_Handle_FFD01(did, diag_Data_Buf, diag_Rx_Actual_Len);
			}
			else if ((did >= (uint16_t)LID_FFD3_START) && (did <= (uint16_t)LID_FFD3_LAST))
			{
				DIAG_Handle_FFD03(did, diag_Data_Buf, diag_Rx_Actual_Len);
			}
			else
			{
				/* ignore */
			}
		}
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_Analyze_PUD(void)
{
	DIAGPCI_TypeDef tmp_pci = (DIAGPCI_TypeDef)(diag_Rx_Data[0] & (uint8_t)0xF0);
	uint16_t tmp_len = 0;
	uint16_t tmp_sn = 0;

	switch (tmp_pci)
	{
	case PCI_SINGLE_FRAME:
		if (diag_Rx_Data[1] != (uint8_t)SID_NEGATIVE_RESPONSE)
		{
			diag_Rx_SID = (DIAGSID_TypeDef)(diag_Rx_Data[1] - ((uint8_t)DIAG_POS_ACK));
			if (diag_Rx_SID == diag_Tx_SID)
			{
				if (diag_Rx_SID == SID_START_DIAGNOSTIC_SESSION)
				{
					diag_Rx_DID = diag_Rx_Data[2];
					if (diag_Rx_DID == diag_Tx_DID)
					{
						diag_Talk_Status = false;
						DIAG_Set_PUD_TimeOut(0);

						if (diag_Rx_DID == SF_SESSION_EXTENDED)
						{
							diag_Extend_Session = true;
						}
						else
						{
							/* Ignore */
						}
					}
					else
					{
						/* Ignore PUD */
					}
				}
				else if (diag_Rx_SID == SID_READ_DATA_BY_LOCAL_ID)
				{
					diag_Rx_DID = diag_Rx_Data[2];
					if (diag_Rx_DID == diag_Tx_DID)
					{
						tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
						tmp_len = tmp_len - 2;
						DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[3], tmp_len, tmp_len, 0, tmp_pci);
					}
					else
					{
						/* Ignore PUD */
					}
				}
				else if (diag_Rx_SID == SID_READ_DATA_BY_COMMAND_ID)
				{
					diag_Rx_DID = (diag_Rx_Data[2] << (uint8_t)8);
					diag_Rx_DID |= diag_Rx_Data[3];
					if (diag_Rx_DID == diag_Tx_DID)
					{
						tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
						tmp_len = tmp_len - 3;
						DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[4], tmp_len, tmp_len, 0, tmp_pci);
					}
					else
					{
						/* Ignore PUD */
					}
				}
				else if (diag_Rx_SID == SID_READ_FREEZE_FRAME_DATA)
				{
					diag_Rx_DID = diag_Tx_DID;
					diag_Rx_DID &= (uint16_t)0xFFF0;
					diag_Rx_DID |= (diag_Rx_Data[2] - (uint8_t)1);
					if (diag_Rx_DID == diag_Tx_DID)
					{
						tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
						tmp_len = tmp_len - 2;
						DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[3], tmp_len, tmp_len, 0, tmp_pci);
					}
					else
					{
						/* Ignore PUD */
					}
				}
				else
				{
					/* Ignore PUD */
				}
			}
			else
			{
				/* Ignore PUD */
			}
		}
		else
		{
			diag_Rx_SID = (DIAGSID_TypeDef)diag_Rx_Data[2];
			if (diag_Rx_SID == diag_Tx_SID)
			{
				diag_Rx_NRC = (DIAGNRC_TypeDef)diag_Rx_Data[3];
				if (diag_Rx_NRC != NRC_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING)
				{
					DIAG_Set_PUD_TimeOut(0);
				}
				else
				{
					DIAG_Set_PUD_TimeOut(5000);
				}
			}
			else
			{
				/* Ignore PUD */
			}
		}
		break;
	case PCI_FIRST_FRAME:
		diag_Rx_SID = (DIAGSID_TypeDef)(diag_Rx_Data[2] - ((uint8_t)DIAG_POS_ACK));
		if (diag_Rx_SID == diag_Tx_SID)
		{
			if (diag_Rx_SID == SID_READ_DATA_BY_LOCAL_ID)
			{
				diag_Rx_DID = diag_Rx_Data[3];
				if (diag_Rx_DID == diag_Tx_DID)
				{
					tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
					tmp_len <<= (uint8_t)8;
					tmp_len |= diag_Rx_Data[1];
					tmp_len = tmp_len - 2;
					DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[4], tmp_len, 4, 0, tmp_pci);
				}
				else
				{
					/* Ignore PUD */
				}
			}
			else if (diag_Rx_SID == SID_READ_DATA_BY_COMMAND_ID)
			{
				diag_Rx_DID = (diag_Rx_Data[3] << (uint8_t)8);
				diag_Rx_DID |= diag_Rx_Data[4];
				if (diag_Rx_DID == diag_Tx_DID)
				{
					tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
					tmp_len <<= (uint8_t)8;
					tmp_len |= diag_Rx_Data[1];
					tmp_len = tmp_len - 3;
					DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[5], tmp_len, 3, 0, tmp_pci);
				}
				else
				{
					/* Ignore PUD */
				}
			}
			else if (diag_Rx_SID == SID_READ_FREEZE_FRAME_DATA)
			{
				diag_Rx_DID = diag_Tx_DID;
				diag_Rx_DID &= (uint16_t)0xFFF0;
				diag_Rx_DID |= (diag_Rx_Data[3] - (uint8_t)1);
				if (diag_Rx_DID == diag_Tx_DID)
				{
					tmp_len = diag_Rx_Data[0] & (uint8_t)0x0F;
					tmp_len <<= (uint8_t)8;
					tmp_len |= diag_Rx_Data[1];
					tmp_len = tmp_len - 2;
					DIAG_Handle_PUD(diag_Rx_DID, &diag_Rx_Data[4], tmp_len, 4, 0, tmp_pci);
				}
				else
				{
					/* Ignore PUD */
				}
			}
			else
			{
				/* Ignore PUD */
			}
		}
		else
		{
			/* Ignore PUD */
		}
		break;
	case PCI_CONSECUTIVE_FRAME:
		tmp_sn = diag_Rx_Data[0] & (uint8_t)0x0F;
		DIAG_Handle_PUD(diag_Tx_DID, &diag_Rx_Data[1], 0, 0, tmp_sn, tmp_pci);
		break;
	case PCI_FLOW_CONTROL:
		break;
	}
}

static bool DIAG_Create_PUD(DIAGSID_TypeDef sid, uint16_t data)
{
	uint16_t len_val = 0;
	bool status = true;
	(void)memset(diag_Tx_Data, DIAG_EMPTY_FILL, (size_t)8);
	if (sid == SID_START_DIAGNOSTIC_SESSION)
	{
		diag_Tx_Rpt = 3;
		diag_Tx_SID = sid;
		diag_Tx_DID = data;
		diag_Tx_Data[0] = (uint8_t)PCI_SINGLE_FRAME;
		diag_Tx_Data[1] = (uint8_t)sid;
		len_val = len_val + 1;
		diag_Tx_Data[2] = U16_LSB(data);
		len_val = len_val + 1;
		len_val &= (uint16_t)0x0F;
		diag_Tx_Data[0] |= (uint16_t)len_val;
	}
	else if (sid == SID_READ_DATA_BY_LOCAL_ID)
	{
		diag_Tx_Rpt = 3;
		diag_Tx_SID = sid;
		diag_Tx_DID = data;
		diag_Tx_Data[0] = (uint8_t)PCI_SINGLE_FRAME;
		diag_Tx_Data[1] = (uint8_t)sid;
		len_val = len_val + 1;
		diag_Tx_Data[2] = U16_LSB(data);
		len_val = len_val + 1;
		len_val &= (uint16_t)0x0F;
		diag_Tx_Data[0] |= (uint16_t)len_val;
	}
	else if (sid == SID_READ_DATA_BY_COMMAND_ID)
	{
		diag_Tx_Rpt = 3;
		diag_Tx_SID = sid;
		diag_Tx_DID = data;
		diag_Tx_Data[0] = (uint8_t)PCI_SINGLE_FRAME;
		diag_Tx_Data[1] = (uint8_t)sid;
		len_val = len_val + 1;
		diag_Tx_Data[2] = U16_MSB(data);
		len_val = len_val + 1;
		diag_Tx_Data[3] = U16_LSB(data);
		len_val = len_val + 1;
		diag_Tx_Data[4] = 0;
		len_val = len_val + 1;
		diag_Tx_Data[5] = 0;
		len_val = len_val + 1;
		len_val &= (uint16_t)0x0F;
		diag_Tx_Data[0] |= (uint16_t)len_val;
	}
	else if (sid == SID_READ_FREEZE_FRAME_DATA)
	{
		diag_Tx_Rpt = 3;
		diag_Tx_SID = sid;
		diag_Tx_DID = data;
		diag_Tx_Data[0] = (uint8_t)PCI_SINGLE_FRAME;
		diag_Tx_Data[1] = (uint8_t)sid;
		len_val = len_val + 1;

		diag_Tx_Data[2] = (U16_LSB(data) & (uint8_t)0x0F) + (uint8_t)0x01;
		len_val = len_val + 1;
		diag_Tx_Data[4] = ((U16_LSB(data) & (uint8_t)0xF0) >> (uint8_t)4) | (uint8_t)0x10;
		len_val = len_val + 1;

		diag_Tx_Data[3] = (uint8_t)1;
		len_val = len_val + 1;

		len_val &= (uint16_t)0x0F;
		diag_Tx_Data[0] |= (uint16_t)len_val;
	}
	else
	{
		status = false;
	}
	return status;
}

static bool DIAG_Check_ECUID(void)
{
	bool status = false;

	if (diag_Rx_Len == (uint8_t)8)
	{
		if (diag_Rx_Id >= (uint32_t)0x08)
		{
			if (diag_Tx_ID == (diag_Rx_Id - (uint32_t)0x08))
			{
				status = true;
			}
			else
			{
				status = false;
			}
		}
		else
		{
			/* nothing */
		}
	}
	else
	{
		status = false;
	}
	return status;
}

static void DIAG_Init_AllData(void)
{
	BSP_FDCAN1_Reset_Rx_Queue_1();
	diag_Rx_Id = 0;
	diag_Rx_Len = 0;
	(void)memset(diag_Rx_Data, 0, 8);
	diag_Rx_DID = 0;
	diag_Rx_SID = SID_NONE;
	diag_Rx_NRC = NRC_NONE;
	diag_Tx_ID = 0;
	diag_Tx_Len = 0;
	(void)memset(diag_Tx_Data, 0, 8);
	diag_Tx_DID = 0;
	diag_Tx_SID = SID_NONE;
	diag_Tx_Rpt = 0;
	diag_Talk_Status = false;
	diag_Rx_CF_Status = false;
	diag_Rx_CF_Len = 0;
	diag_Rx_Actual_Len = 0;
	diag_Rx_CF_Sn = 0;
	diag_Data_Buf_Ptr = 0;
	(void)memset(diag_Data_Buf, 0, 70);
	diag_Extend_Session = false;
	search_Mode_SDS = false;
	search_Mode_Time = SimpleOS_GetTick();
	reset_Mode_Time = SimpleOS_GetTick();
	session_Mode_Time = SimpleOS_GetTick();
	ffd02_Mode_Time = SimpleOS_GetTick();
	commandID_Mode_Time = SimpleOS_GetTick();
	timeOut_Time = SimpleOS_GetTick();
	session_Mode_Toggle = false;
	testerPresent_Flag = false;
	testerPresent_Time = SimpleOS_GetTick();
	diag_Time = SimpleOS_GetTick();
	diag_Max_Time = 1000;
	commandID_Request = false;
	commandID_Finish = false;
	commandID_Status = STATUS_NONE;
	ffd01_Request = false;
	ffd01_Finish = false;
	ffd01_Ask_ID = 0;
	ffd01_Status = STATUS_NONE;
	ffd02_Request = false;
	ffd02_Finish = false;
	ffd02_Ask_ID = 0;
	ffd02_Status = STATUS_NONE;
	ffd03_Request = false;
	ffd03_Finish = false;
	ffd03_Ask_ID = 0;
	ffd03_Status = STATUS_NONE;

	(void)memset(cid_Vin_Data, 0, CID_VIN_LEN);
	(void)memset(cid_Ecu_Software_Number_Data, 0, CID_ECU_SOFTWARE_NUMBER_LEN);
	(void)memset(cid_Registered_Model_Data, 0, CID_REGISTERED_MODEL_LEN);
	(void)memset(cid_Ecu_Energization_Total_Time_Data, 0, CID_ECU_ENERGIZATION_TOTAL_TIME_LEN);
	(void)memset(cid_Ecu_Count_Data, 0, CID_ECU_COUNT_LEN);
	(void)memset(cid_Travel_Distance_Data, 0, CID_TRAVEL_DISTANCE_LEN);
	(void)memset(cid_Eg_Operation_Time_Data, 0, CID_EG_OPERATION_TIME_LEN);

	(void)memset(lid_h01_Data, 0x00, LID_H01_LEN);
	(void)memset(lid_h48_Data, 0x00, LID_H48_LEN);

	lid_h01_Asked = false;
	lid_h48_Asked = false;

	diag_State = DIAGSTATE_RESET;
	diag_SetState = DIAGSTATE_RESET;
	DIAG_QueueReset();
}
/*=============================== DIAG state switch ======================================================*/
static void DIAG_statusChange(DIAGState_TypeDef newStat)
{
	diag_SetState = newStat;
}

static void DIAG_enterRESET(void)
{
	if (diag_State != DIAGSTATE_RESET)
	{
		diag_State = DIAGSTATE_RESET;
		DIAG_Init_AllData();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterSEARCH(void)
{
	if (diag_State != DIAGSTATE_SEARCH)
	{
		diag_State = DIAGSTATE_SEARCH;
		DIAG_QueueReset();
		diag_Extend_Session = false;
		diag_Talk_Status = false;

		search_Mode_Time = SimpleOS_GetTick();
		timeOut_Time = SimpleOS_GetTick();
		search_Mode_SDS = true;
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterSTANDBY(void)
{
	if (diag_State != DIAGSTATE_STANDBY)
	{
		diag_State = DIAGSTATE_STANDBY;
		DIAG_QueueReset();
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterSESSION(void)
{
	if (diag_State != DIAGSTATE_SESSION)
	{
		diag_State = DIAGSTATE_SESSION;
		DIAG_QueueReset();

		session_Mode_Time = SimpleOS_GetTick();
		session_Mode_Toggle = false;
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterCOMMANDID(void)
{
	if (diag_State != DIAGSTATE_COMMANDID)
	{
		diag_State = DIAGSTATE_COMMANDID;
		DIAG_QueueReset();

		commandID_Mode_Time = SimpleOS_GetTick();

		commandID_Finish = false;
		commandID_Status = STATUS_WAIT;
		(void)memset(cid_Vin_Data, 0, CID_VIN_LEN);
		(void)memset(cid_Registered_Model_Data, 0, CID_REGISTERED_MODEL_LEN);
		(void)memset(cid_Ecu_Software_Number_Data, 0, CID_ECU_SOFTWARE_NUMBER_LEN);
		(void)memset(cid_Ecu_Energization_Total_Time_Data, 0, CID_ECU_ENERGIZATION_TOTAL_TIME_LEN);
		(void)memset(cid_Ecu_Count_Data, 0, CID_ECU_COUNT_LEN);
		(void)memset(cid_Travel_Distance_Data, 0, CID_TRAVEL_DISTANCE_LEN);
		(void)memset(cid_Eg_Operation_Time_Data, 0, CID_EG_OPERATION_TIME_LEN);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_VIN);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_REGISTERED_MODEL);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_ECU_SOFTWARE_NUMBER);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_ECU_ENERGIZATION_TOTAL_TIME);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_ECU_COUNT);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_TRAVEL_DISTANCE);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_COMMAND_ID, (uint16_t)CID_EG_OPERATION_TIME);
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterFFD01(void)
{
	if (diag_State != DIAGSTATE_FFD01)
	{
		diag_State = DIAGSTATE_FFD01;
		DIAG_QueueReset();

		timeOut_Time = SimpleOS_GetTick();

		ffd01_Finish = false;
		ffd01_Status = STATUS_WAIT;
		ffd01_Ask_ID = (uint16_t)0xFF00 | ((uint16_t)LID_FFD1_GROUP_START << (uint16_t)4) | (uint16_t)LID_FFD1_ITEM_START;
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterFFD02(void)
{
	if (diag_State != DIAGSTATE_FFD02)
	{
		diag_State = DIAGSTATE_FFD02;
		DIAG_QueueReset();

		ffd02_Mode_Time = SimpleOS_GetTick();
		timeOut_Time = SimpleOS_GetTick();

		ffd02_Finish = false;
		ffd02_Status = STATUS_WAIT;
		ffd02_Ask_ID = LID_FFD2_START;
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_enterFFD03(void)
{
	if (diag_State != DIAGSTATE_FFD03)
	{
		diag_State = DIAGSTATE_FFD03;
		DIAG_QueueReset();

		timeOut_Time = SimpleOS_GetTick();

		ffd03_Finish = false;
		ffd03_Status = STATUS_WAIT;
		ffd03_Ask_ID = LID_FFD3_START;
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_START_DIAGNOSTIC_SESSION, (uint16_t)SF_SESSION_EXTENDED);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_LOCAL_ID, (uint16_t)ffd03_Ask_ID);
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_updateRESET(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&reset_Mode_Time, (uint16_t)50);
	if (tmp_Result == true)
	{
		DIAG_statusChange(DIAGSTATE_SEARCH);
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_updateSEARCH(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&search_Mode_Time, (uint16_t)50);
	if (tmp_Result == true)
	{
		if (search_Mode_SDS == true)
		{
			search_Mode_SDS = false;
			diag_Extend_Session = false;
			DIAG_QueuePush(DIAG_STANDARD_ID, SID_START_DIAGNOSTIC_SESSION, (uint16_t)SF_SESSION_EXTENDED);
			timeOut_Time = SimpleOS_GetTick();
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

	tmp_Result = SimpleOS_CheckInterval(&timeOut_Time, (uint16_t)3000);
	if (tmp_Result == true)
	{
		search_Mode_SDS = true;
	}
	else
	{
		/* nothing */
	}

	if (diag_Extend_Session == true)
	{
		DIAG_statusChange(DIAGSTATE_STANDBY);
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_updateSTANDBY(void)
{
	bool status = BLE_Get_Connect_Status();
	if (status == true)
	{
		DIAG_statusChange(DIAGSTATE_SESSION);
	}
	else
	{
		/* ignore */
	}
}

static void DIAG_updateSESSION(void)
{
	bool tmp_Result;

	if (commandID_Request == true)
	{
		commandID_Request = false;
		DIAG_statusChange(DIAGSTATE_COMMANDID);
	}
	else if (ffd01_Request == true)
	{
		ffd01_Request = false;
		DIAG_statusChange(DIAGSTATE_FFD01);
	}
	else if (ffd02_Request == true)
	{
		ffd02_Request = false;
		DIAG_statusChange(DIAGSTATE_FFD02);
	}
	else if (ffd03_Request == true)
	{
		ffd03_Request = false;
		DIAG_statusChange(DIAGSTATE_FFD03);
	}
	else
	{
		tmp_Result = SimpleOS_CheckInterval(&session_Mode_Time, (uint16_t)50);
		if (tmp_Result == true)
		{
			if (session_Mode_Toggle == false)
			{
				if (DIAG_QueueIsEmpty() == true)
				{
					session_Mode_Toggle = true;
					DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_LOCAL_ID, (uint16_t)0x01);
				}
				else
				{
					/* ignore */
				}
			}
			else
			{
				if (DIAG_QueueIsEmpty() == true)
				{
					session_Mode_Toggle = false;
					DIAG_QueuePush(DIAG_STANDARD_ID, SID_START_DIAGNOSTIC_SESSION, (uint16_t)SF_SESSION_EXTENDED);
					DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_LOCAL_ID, (uint16_t)0x48);
				}
				else
				{
					/* ignore */
				}
			}
		}
		else
		{
			/* nothing */
		}
	}
}

static void DIAG_updateCOMMANDID(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&commandID_Mode_Time, (uint16_t)200);
	if (tmp_Result == true)
	{
		commandID_Status = STATUS_TIMEOUT;
		DIAG_statusChange(DIAGSTATE_SESSION);
	}
	else
	{
		if (commandID_Finish == true)
		{
			commandID_Finish = false;
			commandID_Status = STATUS_OK;
			DIAG_statusChange(DIAGSTATE_SESSION);
		}
		else
		{
			/* nothing */
		}
	}
}

static void DIAG_updateFFD01(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&timeOut_Time, (uint16_t)5000);
	if (tmp_Result == true)
	{
		ffd01_Status = STATUS_TIMEOUT;
		DIAG_statusChange(DIAGSTATE_SESSION);
	}
	else
	{
		if (ffd01_Finish == true)
		{
			ffd01_Finish = false;
			ffd01_Status = STATUS_OK;
			DIAG_statusChange(DIAGSTATE_SESSION);
		}
		else
		{
			/* nothing */
		}
	}

	if ((ffd01_Ask_ID & (uint16_t)0x000F) <= (uint16_t)LID_FFD1_ITEM_END)
	{
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_START_DIAGNOSTIC_SESSION, (uint16_t)SF_SESSION_EXTENDED);
		DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_FREEZE_FRAME_DATA, (uint16_t)ffd01_Ask_ID);
		ffd01_Ask_ID = ffd01_Ask_ID + 1;
	}
	else
	{
		/* nothing */
	}
}

static void DIAG_updateFFD02(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&timeOut_Time, (uint16_t)5000);
	if (tmp_Result == true)
	{
		ffd02_Status = STATUS_TIMEOUT;
		DIAG_statusChange(DIAGSTATE_SESSION);
	}
	else
	{
		if (ffd02_Finish == true)
		{
			ffd02_Finish = false;
			ffd02_Status = STATUS_OK;
			DIAG_statusChange(DIAGSTATE_SESSION);
		}
		else
		{
			/* nothing */
		}
	}

	tmp_Result = SimpleOS_CheckInterval(&ffd02_Mode_Time, (uint16_t)50);
	if (tmp_Result == true)
	{
		if (ffd02_Ask_ID <= (uint16_t)LID_FFD2_LAST)
		{
			DIAG_QueuePush(DIAG_STANDARD_ID, SID_START_DIAGNOSTIC_SESSION, (uint16_t)SF_SESSION_EXTENDED);
			DIAG_QueuePush(DIAG_STANDARD_ID, SID_READ_DATA_BY_LOCAL_ID, (uint16_t)ffd02_Ask_ID);
			ffd02_Ask_ID = ffd02_Ask_ID + 1;
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

static void DIAG_updateFFD03(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&timeOut_Time, (uint16_t)200);
	if (tmp_Result == true)
	{
		ffd03_Status = STATUS_TIMEOUT;
		DIAG_statusChange(DIAGSTATE_SESSION);
	}
	else
	{
		if (ffd03_Finish == true)
		{
			ffd03_Finish = false;
			ffd03_Status = STATUS_OK;
			DIAG_statusChange(DIAGSTATE_SESSION);
		}
		else
		{
			/* nothing */
		}
	}
}
/*========================================================================================*/

static ModuleState_TypeDef DIAGModule_state = MODULESTATE_RESET;

void DIAG_init(void)
{
	DIAGModule_state = MODULESTATE_INITIALIZED;

	DIAG_Init_AllData();
}

void DIAG_open(void)
{
	if (DIAGModule_state != MODULESTATE_OPENED)
	{
		DIAGModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/

		DIAG_Init_AllData();

		DIAG_statusChange(DIAGSTATE_SEARCH);

		vehicle_ODO = Eeprom_Get_Vehicle_ODO();
		accu_ODO = Eeprom_Get_Accumulation_ODO();
	}
	else
	{
	}
}

void DIAG_close(void)
{
	if (DIAGModule_state != MODULESTATE_CLOSED)
	{
		DIAGModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
	}
	else
	{
	}
}

void DIAG_flush(void)
{
	bool tmp_Result;

	if (DIAGModule_state == MODULESTATE_OPENED)
	{
		/* Write the module flash code here	*/

		/* Check PUD time */
		tmp_Result = SimpleOS_CheckInterval(&diag_Time, diag_Max_Time);
		if (tmp_Result == true)
		{
			/* Time Out */

			if (diag_Rx_CF_Status == true)
			{
				diag_Rx_CF_Status = false;
				(void)memset(diag_Data_Buf_Ptr, 0, 70);
				diag_Data_Buf_Ptr = 0;
				diag_Rx_CF_Sn = 0;
				diag_Rx_CF_Len = 0;
			}
			else
			{
				/* nothing */
			}

			if (testerPresent_Flag == true)
			{
				testerPresent_Flag = false;
				DIAG_Send_TesterPresent(SID_TESTER_PRESENT, (uint16_t)SF_TESTERPRESENT_NORESPOND);
				DIAG_Set_PUD_TimeOut(0);
			}
			else
			{
				if (diag_Talk_Status == true)
				{
					if (diag_Tx_Rpt > 0)
					{
						diag_Tx_Rpt--;
						DIAG_ReSend_PUD();
						DIAG_Set_PUD_TimeOut(50);
					}
					else
					{
						/* do something in time out */
						diag_Talk_Status = false;

						if (diag_Tx_SID == SID_READ_DATA_BY_LOCAL_ID)
						{
							if (diag_Tx_DID == (uint16_t)LID_FFD2_LAST)
							{
								ffd02_Finish = true;
							}
							else if (diag_Tx_DID == (uint16_t)LID_H01)
							{
								lid_h01_Asked = true;
							}
							else if (diag_Tx_DID == (uint16_t)LID_H48)
							{
								lid_h48_Asked = true;
							}
							else if (diag_Tx_DID == (uint16_t)LID_FFD3_LAST)
							{
								ffd03_Finish = true;
							}
							else
							{
								/* ignore */
							}
						}
						else if (diag_Tx_SID == SID_READ_FREEZE_FRAME_DATA)
						{
							DIAG_Add_FFD01_Group();
						}
						else
						{
							/* nothing */
						}
					}
				}
				else
				{
					/* do something in out DTC_status */
				}
			}
		}
		else
		{
			/* No Time Out */
			/* Check New Data */
			tmp_Result = DIAG_Get_FIFOElement();
			if (tmp_Result == true)
			{
				/* Check ECU ID */
				if (diag_Talk_Status == true)
				{
					tmp_Result = DIAG_Check_ECUID();
					if (tmp_Result == true)
					{
						DIAG_Analyze_PUD();
					}
					else
					{
						/* Ignore Data */
					}
				}
				else
				{
					/* Ignore Data */
				}
			}
			else
			{
				/* No New Data */
			}
		}

		if (diag_Talk_Status == false)
		{
			tmp_Result = DIAG_QueuePop(&uds_node);
			if (tmp_Result == true)
			{
				tmp_Result = DIAG_Create_PUD(uds_node->sid, uds_node->data);
				if (tmp_Result == true)
				{
					if (diag_Tx_Rpt > 0)
					{
						diag_Tx_Rpt = diag_Tx_Rpt - 1;
						DIAG_Send_PUD(uds_node->id);
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
				uds_node = 0;
			}
		}
		else
		{
			/* nothing */
		}

		if ((diag_State != DIAGSTATE_RESET) && (diag_State != DIAGSTATE_SEARCH))
		{
			tmp_Result = SimpleOS_CheckInterval(&testerPresent_Time, (uint16_t)1000);
			if (tmp_Result == true)
			{
				testerPresent_Flag = true;
			}
			else
			{
				/* nothing */
			}
		}
		else
		{
			testerPresent_Time = SimpleOS_GetTick();
			testerPresent_Flag = false;
		}

		if (diag_State != diag_SetState)
		{
			switch (diag_SetState)
			{
			case DIAGSTATE_RESET:
				DIAG_enterRESET();
				break;
			case DIAGSTATE_SEARCH:
				DIAG_enterSEARCH();
				break;
			case DIAGSTATE_STANDBY:
				DIAG_enterSTANDBY();
				break;
			case DIAGSTATE_SESSION:
				DIAG_enterSESSION();
				break;
			case DIAGSTATE_COMMANDID:
				DIAG_enterCOMMANDID();
				break;
			case DIAGSTATE_FFD01:
				DIAG_enterFFD01();
				break;
			case DIAGSTATE_FFD02:
				DIAG_enterFFD02();
				break;
			case DIAGSTATE_FFD03:
				DIAG_enterFFD03();
				break;
			}
			diag_State = diag_SetState;
		}
		else
		{
			switch (diag_State)
			{
			case DIAGSTATE_RESET:
				DIAG_updateRESET();
				break;
			case DIAGSTATE_SEARCH:
				DIAG_updateSEARCH();
				break;
			case DIAGSTATE_STANDBY:
				DIAG_updateSTANDBY();
				break;
			case DIAGSTATE_SESSION:
				DIAG_updateSESSION();
				break;
			case DIAGSTATE_COMMANDID:
				DIAG_updateCOMMANDID();
				break;
			case DIAGSTATE_FFD01:
				DIAG_updateFFD01();
				break;
			case DIAGSTATE_FFD02:
				DIAG_updateFFD02();
				break;
			case DIAGSTATE_FFD03:
				DIAG_updateFFD03();
				break;
			}
		}
	}
	else
	{
		/* nothing */
	}
}

/*========================================================================================*/

void DIAG_Reset_fifoIndex(void)
{
	BSP_FDCAN1_Reset_Rx_Queue_1();
}

void DIAG_Request_commandID(void)
{
	commandID_Request = true;
	commandID_Status = STATUS_NONE;
}

DIAGSTATUS_TypeDef DIAG_Get_commandIDStatus(void)
{
	return commandID_Status;
}

void DIAG_Request_ffd01(void)
{
	ffd01_Request = true;
	ffd01_Status = STATUS_NONE;
}

DIAGSTATUS_TypeDef DIAG_Get_ffd01Status(void)
{
	return ffd01_Status;
}

void DIAG_Request_ffd02(void)
{
	ffd02_Request = true;
	ffd02_Status = STATUS_NONE;
}

DIAGSTATUS_TypeDef DIAG_Get_ffd02Status(void)
{
	return ffd02_Status;
}

void DIAG_Request_ffd03(void)
{
	ffd03_Request = true;
	ffd03_Status = STATUS_NONE;
}

DIAGSTATUS_TypeDef DIAG_Get_ffd03Status(void)
{
	return ffd03_Status;
}

uint8_t *DIAG_Get_cid_vin_data_Ptr(void)
{
	return cid_Vin_Data;
}

uint8_t *DIAG_Get_cid_ecu_software_number_data_Ptr(void)
{
	return cid_Ecu_Software_Number_Data;
}

uint8_t *DIAG_Get_cid_registered_model_data_Ptr(void)
{
	return cid_Registered_Model_Data;
}

uint8_t *DIAG_Get_cid_ecu_energization_total_time_data_Ptr(void)
{
	return cid_Ecu_Energization_Total_Time_Data;
}

uint8_t *DIAG_Get_cid_ecu_count_data_Ptr(void)
{
	return cid_Ecu_Count_Data;
}

uint8_t *DIAG_Get_cid_travel_distance_data_Ptr(void)
{
	return cid_Travel_Distance_Data;
}

uint8_t *DIAG_Get_cid_eg_operation_time_data_Ptr(void)
{
	return cid_Eg_Operation_Time_Data;
}

uint8_t *DIAG_Get_lid_h01_data_Ptr(void)
{
	return lid_h01_Data;
}

uint8_t *DIAG_Get_lid_h48_data_Ptr(void)
{
	return lid_h48_Data;
}

bool DIAG_Get_LID_H01H48_Status(void)
{
	bool status = false;
	if ((lid_h01_Asked == true) && (lid_h48_Asked == true))
	{
		status = true;
	}
	else
	{
		status = false;
	}
	return status;
}

void DIAG_Reset_Status(void)
{
	DIAG_statusChange(DIAGSTATE_RESET);
}

uint32_t DIAG_Get_Vehicle_ODO(void)
{
	return vehicle_ODO;
}

uint32_t DIAG_Get_Accumulation_ODO(void)
{
	uint32_t tmp = accu_ODO;
	accu_ODO = 0;

	return tmp;
}
