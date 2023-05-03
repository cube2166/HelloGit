#include "./BLE_Module.h"

#define BLE_UPTADE_PER 50

static uint8_t 		ble_Cts_Cnt; 			/* BLE Module CTS Pin Voltage time */
static bool 		ble_Cts_PreStatus; 		/* BLE Module CTS Pin Before Voltage */
static bool 		ble_Cts_Status; 		/* BLE Module CTS Pin Current Voltage */

static uint8_t 		ble_Link_Cnt; 			/* BLE Module Link Pin Voltage time */
static bool 		ble_Link_PreStatus; 	/* BLE Module Link Pin Before Voltage */
static bool 		ble_Link_Status; 		/* BLE Module Link Pin Current Voltage */

static uint16_t 	reBoot_Cnt; 			/* BLE Module Reboot Time */

static uint8_t 		reset_Mode_Cnt; 		/* Search Mode Time */
static uint8_t 		adv_Mode_Cnt; 			/* Advertising Mode Time */
static uint8_t 		connect_Mode_Cnt; 		/* Connect Mode Time */
static uint8_t 		approve_Mode_Cnt; 		/* Approve Mode Time */
static uint8_t 		ffd_Mode_Cnt; 			/* FFD Mode Time */
static uint8_t 		command_Mode_Cnt; 		/* Command Mode Time */

static bool 		ble_Connected; 			/* BLE Module Connected Flag */
static bool 		ble_advFlag; 			/* BLE Module Fake Advertising Flag */

static BLEP_PackageInfoTypedef ffdInfo_1, ffdInfo_2, ffdInfo_3, ffdInfo_4, ffdInfo_5;

/* ==================================DATA====================================== */
static uint8_t ccuid_adv_data[20] = {0x59, 0x53, 0x43, 0x43, 0x55, 0x5F};
static uint8_t process_data[1] = {0x00};
/* ---------------------------------------------------------------------------- */

typedef enum
{
	BLESTATE_RESET = 0,
	BLESTATE_ADV,
	BLESTATE_CONNECT,
	BLESTATE_APPROVE,
	BLESTATE_COMMANDID,
	BLESTATE_FFD01,
	BLESTATE_FFD02,
	BLESTATE_FFD03,
} BLEState_TypeDef;

static BLEState_TypeDef ble_State = BLESTATE_RESET;
static BLEState_TypeDef ble_SetState = BLESTATE_RESET;
static void BLE_statusChange(BLEState_TypeDef newStat)
{
	ble_SetState = newStat;
}

/*=============================== BLE function ======================================================*/
static void BLE_Send_LocalName(void)
{
	adv_Mode_Cnt = adv_Mode_Cnt + 1;
	if (adv_Mode_Cnt >= (uint8_t)((uint16_t)300 / (uint16_t)BLE_UPTADE_PER))
	{
		adv_Mode_Cnt = (uint8_t)0;
		USART_SendData(BLEP_Create_AdvPackage, 0);
	}
	else
	{
		/* nothing */
	}
}

static void BLE_Check_Authentication(void)
{
	bool authentication_status = false;
	uint8_t ii = 0;
	bool tmp_Result;

	uint8_t *ccuIDPtr = 0;
	uint8_t *passKeyPtr = 0;
	uint8_t *uuidPtr = 0;

	uint8_t localName[(uint8_t)BLE2SCCU_AUTH_LOCALNAME_LEN];
	uint8_t passKey[(uint8_t)BLE2SCCU_AUTH_PASSKEY_LEN];
	uint8_t uuid[(uint8_t)BLE2SCCU_AUTH_UUID_LEN];
	uint8_t bondFlag[(uint8_t)BLE2SCCU_AUTH_BONDFLAG_LEN];

	tmp_Result = Eeprom_Get_QRCODE_Save();
	if (tmp_Result == true)
	{
		ccuIDPtr = Eeprom_Get_CCUID_Ptr();
		passKeyPtr = Eeprom_Get_PassKey_Ptr();
		uuidPtr = Eeprom_Get_UUID_Ptr();
		tmp_Result = BLEP_Get_Authentication(localName, passKey, uuid, bondFlag);
		if (tmp_Result == true)
		{
			if (bondFlag[0] == 1)
			{
				// First bonding, check name, passkey; save uuid
				for (ii = 0; ii < BLE2SCCU_AUTH_LOCALNAME_LEN; ii++)
				{
					/* check name */
					if (ccuIDPtr[ii] != localName[ii])
					{
						authentication_status = false;
						break;
					}
					else
					{
						authentication_status = true;
					}
				}
				if (authentication_status == true)
				{
					for (ii = 0; ii < BLE2SCCU_AUTH_PASSKEY_LEN; ii++)
					{
						/* check passkey */
						if (passKeyPtr[ii] != passKey[ii])
						{
							authentication_status = false;
							break;
						}
						else
						{
							authentication_status = true;
						}
					}
					if (authentication_status == true)
					{
						Eeprom_ReqBackupUUID(uuid);
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
				// No first bonding, check uuid
				for (ii = 0; ii < BLE2SCCU_AUTH_UUID_LEN; ii++)
				{
					if (uuidPtr[ii] != uuid[ii])
					{
						authentication_status = false;
						break;
					}
					else
					{
						authentication_status = true;
					}
				}
			}

			if (authentication_status == true)
			{
				process_data[0] = 1;
				USART_QueueReset();
				USART_SendData(BLEP_Create_ProcessPackage, 0);
				BLE_statusChange(BLESTATE_COMMANDID);
			}
			else
			{
				process_data[0] = 0;
				USART_SendData(BLEP_Create_ProcessPackage, 0);
			}
		}
		else
		{
			/* nothing */
		}
	}
	else
	{
		BLE_statusChange(BLESTATE_RESET);
		RTT_PRINT_0("BLE noQRCode RESET\n");
	}
}

static void BLE_Check_CTS(void)
{
	GPIO_PinState tmp = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

	if (tmp == GPIO_PIN_SET)
	{
		ble_Cts_Status = true;
	}
	else
	{
		ble_Cts_Status = false;
	}

	if (ble_Cts_Status != ble_Cts_PreStatus)
	{
		ble_Cts_PreStatus = ble_Cts_Status;
		ble_Cts_Cnt = 0;
	}

	if (ble_Cts_Status == true)
	{
		ble_Cts_Cnt = ble_Cts_Cnt + 1;
		if (ble_Cts_Cnt >= (uint8_t)((uint16_t)1000 / (uint16_t)BLE_UPTADE_PER))
		{
			ble_Cts_Cnt = 0;
			ble_Connected = false;
			BLE_statusChange(BLESTATE_RESET);
			RTT_PRINT_0("BLE CTS RESET\n");
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

static void BLE_Check_Link(void)
{
	ble_Link_Status = LINK_GetVal();

	if (ble_Link_Status != ble_Link_PreStatus)
	{
		ble_Link_PreStatus = ble_Link_Status;
		ble_Link_Cnt = 0;
		reBoot_Cnt = 0;
	}

	if (ble_Link_Status == true)
	{
		ble_Link_Cnt = ble_Link_Cnt + 1;
		reBoot_Cnt = reBoot_Cnt + 1;
		if (ble_Link_Cnt >= (uint8_t)((uint16_t)300 / (uint16_t)BLE_UPTADE_PER))
		{
			ble_Link_Cnt = 0;
			BLE_statusChange(BLESTATE_ADV);
			if (ble_Connected == true)
			{
				ble_Connected = false;
				BLE_statusChange(BLESTATE_RESET);
				RTT_PRINT_0("BLE disConnect RESET\n");
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
		if (reBoot_Cnt >= (uint16_t)((uint16_t)40000 / (uint16_t)BLE_UPTADE_PER))
		{
			reBoot_Cnt = 0;
			BLE_statusChange(BLESTATE_RESET);
			RTT_PRINT_0("BLE noConnect RESET\n");
		}
		else
		{
			/* nothing */
		}
	}
	else
	{
		ble_Link_Cnt = ble_Link_Cnt + 1;
		if (ble_Link_Cnt >= (uint8_t)((uint16_t)200 / (uint16_t)BLE_UPTADE_PER))
		{
			ble_Link_Cnt = 0;
			ble_Connected = true;
			if (ble_State == BLESTATE_ADV)
			{
				BLE_statusChange(BLESTATE_CONNECT);
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
}

static void BLE_Init_AllData(void)
{
	ble_Cts_Cnt = 0;
	ble_Cts_PreStatus = false;
	ble_Cts_Status = false;
	ble_Link_Cnt = 0;
	ble_Link_PreStatus = false;
	ble_Link_Status = false;
	reBoot_Cnt = 0;
	reset_Mode_Cnt = 0;
	adv_Mode_Cnt = 0;
	connect_Mode_Cnt = 0;
	approve_Mode_Cnt = 0;
	ffd_Mode_Cnt = 0;
	command_Mode_Cnt = 0;
	ble_Connected = false;
	ble_advFlag = false;
	ble_State = BLESTATE_RESET;
	ble_SetState = BLESTATE_RESET;
	ccuid_adv_data[0] = 0x59;
	ccuid_adv_data[1] = 0x53;
	ccuid_adv_data[2] = 0x43;
	ccuid_adv_data[3] = 0x43;
	ccuid_adv_data[4] = 0x55;
	ccuid_adv_data[5] = 0x5F;
}
/*=============================== BLE state switch ======================================================*/
static void BLE_enterADV(void)
{
	if (ble_State != BLESTATE_ADV)
	{
		ble_State = BLESTATE_ADV;
		adv_Mode_Cnt = 0;
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterRESET(void)
{
	if (ble_State != BLESTATE_RESET)
	{
		ble_State = BLESTATE_RESET;
		reset_Mode_Cnt = 0;
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterCONNECT(void)
{
	if (ble_State != BLESTATE_CONNECT)
	{
		ble_State = BLESTATE_CONNECT;
		connect_Mode_Cnt = 0;
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterAPPROVE(void)
{
	if (ble_State != BLESTATE_APPROVE)
	{

		ble_State = BLESTATE_APPROVE;
		USART_QueueReset();

		approve_Mode_Cnt = 0;
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterCOMMANDID(void)
{
	if (ble_State != BLESTATE_COMMANDID)
	{

		ble_State = BLESTATE_COMMANDID;
		USART_QueueReset();

		command_Mode_Cnt = 0;
		DIAG_Request_commandID();
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterFFD01(void)
{
	if (ble_State != BLESTATE_FFD01)
	{
		ble_State = BLESTATE_FFD01;
		USART_QueueReset();

		ffd_Mode_Cnt = 0;
		DIAG_Request_ffd01();
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterFFD02(void)
{
	if (ble_State != BLESTATE_FFD02)
	{
		ble_State = BLESTATE_FFD02;
		USART_QueueReset();

		ffd_Mode_Cnt = 0;
		DIAG_Request_ffd02();
	}
	else
	{
		/* nothing */
	}
}

static void BLE_enterFFD03(void)
{
	if (ble_State != BLESTATE_FFD03)
	{
		ble_State = BLESTATE_FFD03;
		USART_QueueReset();

		ffd_Mode_Cnt = 0;
		DIAG_Request_ffd03();
	}
	else
	{
		/* nothing */
	}
}

static void BLE_updateRESET(void)
{
	reset_Mode_Cnt = reset_Mode_Cnt + 1;
	if (reset_Mode_Cnt <= (uint8_t)((uint16_t)50 / (uint16_t)BLE_UPTADE_PER))
	{
		USART_SoftwareReset();
		BLEP_Reset_Tx_Package();
	}
	else if (reset_Mode_Cnt <= (uint8_t)((uint16_t)100 / (uint16_t)BLE_UPTADE_PER))
	{
		PWR_Ctrl_PutVal(false);
	}
	else if (reset_Mode_Cnt <= (uint8_t)((uint16_t)150 / (uint16_t)BLE_UPTADE_PER))
	{
		PWR_Ctrl_PutVal(true);
	}
	else
	{
		reset_Mode_Cnt = (uint8_t)((uint16_t)200 / (uint16_t)BLE_UPTADE_PER);
		USART_open();
		USART_SendData(BLEP_Create_FirmwarePackage, 0);
		BLE_statusChange(BLESTATE_ADV);
	}
}

static void BLE_updateADV(void)
{
	bool tmp_Result;
	BLE_Check_Link();

	tmp_Result = Eeprom_Get_QRCODE_Save();

	if (tmp_Result == true)
	{
		BLE_Send_LocalName();
	}
	else if (ble_advFlag == true)
	{
		BLE_Send_LocalName();
	}
	else
	{
		/* ignore */
	}
}

static void BLE_updateCONNECT(void)
{
	BLE_Check_Authentication();
	BLE_Check_Link();
}

static void BLE_updateAPPROVE(void)
{
	BLEP_FFDTypedef *ffd_01, *ffd_02, *ffd_03;
	bool tmp_Result;
	ffd_01 = BLEP_Get_FF01_Ptr();
	ffd_02 = BLEP_Get_FF02_Ptr();
	ffd_03 = BLEP_Get_FF03_Ptr();

	if (ffd_01->newData == true)
	{
		ffd_01->newData = false;
		BLE_statusChange(BLESTATE_FFD01);
	}
	else if (ffd_02->newData == true)
	{
		ffd_02->newData = false;
		BLE_statusChange(BLESTATE_FFD02);
	}
	else if (ffd_03->newData == true)
	{
		ffd_03->newData = false;
		BLE_statusChange(BLESTATE_FFD03);
	}
	else
	{
		approve_Mode_Cnt = approve_Mode_Cnt + 1;
		if (approve_Mode_Cnt <= (uint8_t)((uint16_t)50 / (uint16_t)BLE_UPTADE_PER))
		{
			tmp_Result = DIAG_Get_LID_H01H48_Status();
			if (tmp_Result == true)
			{
				USART_SendData(BLEP_Create_H0148Package, 0);
			}
			else
			{
				/* nothing */
			}
		}
		else if (approve_Mode_Cnt <= (uint8_t)((uint16_t)100 / (uint16_t)BLE_UPTADE_PER))
		{
			approve_Mode_Cnt = 0;
			USART_SendData(BLEP_Create_CanIDPackage, 0);
		}
	}
	BLE_Check_Authentication();
	BLE_Check_Link();
}

static void BLE_updateCOMMANDID(void)
{
	DIAGSTATUS_TypeDef status;

	status = DIAG_Get_commandIDStatus();
	if ((status == STATUS_OK) || (status == STATUS_TIMEOUT))
	{
		USART_SendData(BLEP_Create_CommondIDPackage, 0);
		BLE_statusChange(BLESTATE_FFD02);
	}
	else
	{
		/* nothing */
	}
	BLE_Check_Authentication();
	BLE_Check_Link();
}

static void BLE_updateFFD01(void)
{
	DIAGSTATUS_TypeDef status;

	status = DIAG_Get_ffd01Status();
	if ((status == STATUS_OK) || (status == STATUS_TIMEOUT))
	{
		BLE_statusChange(BLESTATE_APPROVE);
	}
	else
	{
		/* nothing */
	}
	BLE_Check_Link();
}

static void BLE_updateFFD02(void)
{
	DIAGSTATUS_TypeDef status;

	status = DIAG_Get_ffd02Status();
	if ((status == STATUS_OK) || (status == STATUS_TIMEOUT))
	{
		BLE_statusChange(BLESTATE_APPROVE);
	}
	else
	{
		/* nothing */
	}
	BLE_Check_Link();
}

static void BLE_updateFFD03(void)
{
	DIAGSTATUS_TypeDef status;

	status = DIAG_Get_ffd03Status();
	if ((status == STATUS_OK) || (status == STATUS_TIMEOUT))
	{
		BLE_statusChange(BLESTATE_APPROVE);
	}
	else
	{
		/* nothing */
	}
	BLE_Check_Link();
}

/*-------------------------------------------------------------------------------------------------------*/

/*========================================================================================*/

static ModuleState_TypeDef BLEModule_state = MODULESTATE_RESET;

void BLE_init(void)
{
	BLEModule_state = MODULESTATE_INITIALIZED;
	BLE_Init_AllData();
}

void BLE_open(void)
{
	if (BLEModule_state != MODULESTATE_OPENED)
	{
		BLEModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		BLE_Init_AllData();
		BLE_Load_advData();
	}
	else
	{
		/* nothing */
	}
}

void BLE_close(void)
{
	if (BLEModule_state != MODULESTATE_CLOSED)
	{
		BLEModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
		USART_QueueReset();
		USART_SendData(BLEP_Create_ShutdownPackage, 0);
		BLEP_Reset_Shutdown_Routine();
	}
	else
	{
		/* nothing */
	}
}

void BLE_flush(void)
{
	if (BLEModule_state == MODULESTATE_OPENED)
	{
		/* Write the module flash code here	*/
		// BLE_Check_Link();
		BLE_Check_CTS();

		if (ble_State != ble_SetState)
		{
			switch (ble_SetState)
			{
			case BLESTATE_RESET:
				BLE_enterRESET();
				break;
			case BLESTATE_ADV:
				BLE_enterADV();
				break;
			case BLESTATE_CONNECT:
				BLE_enterCONNECT();
				break;
			case BLESTATE_APPROVE:
				BLE_enterAPPROVE();
				break;
			case BLESTATE_COMMANDID:
				BLE_enterCOMMANDID();
				break;
			case BLESTATE_FFD01:
				BLE_enterFFD01();
				break;
			case BLESTATE_FFD02:
				BLE_enterFFD02();
				break;
			case BLESTATE_FFD03:
				BLE_enterFFD03();
				break;
			}
			ble_State = ble_SetState;
		}
		else
		{
			switch (ble_State)
			{
			case BLESTATE_RESET:
				BLE_updateRESET();
				break;
			case BLESTATE_ADV:
				BLE_updateADV();
				break;
			case BLESTATE_CONNECT:
				BLE_updateCONNECT();
				break;
			case BLESTATE_APPROVE:
				BLE_updateAPPROVE();
				break;
			case BLESTATE_COMMANDID:
				BLE_updateCOMMANDID();
				break;
			case BLESTATE_FFD01:
				BLE_updateFFD01();
				break;
			case BLESTATE_FFD02:
				BLE_updateFFD02();
				break;
			case BLESTATE_FFD03:
				BLE_updateFFD03();
				break;
			}
		}
	}
	else
	{
		/* nothing */
	}
}

void BLE_Load_advData(void)
{
	bool tmp_Result;

	tmp_Result = Eeprom_Get_QRCODE_Save();
	if (tmp_Result == true)
	{
		(void)memcpy((ccuid_adv_data + 6), Eeprom_Get_CCUID_Ptr(), BLE2SCCU_AUTH_LOCALNAME_LEN);
	}
	else
	{
		(void)memset((ccuid_adv_data + 6), 0x30, BLE2SCCU_AUTH_LOCALNAME_LEN);
		ccuid_adv_data[6 + 3] = DEF_DEVICE_CODE;
	}
}

void BLE_Send_FFDdata(uint16_t id, uint8_t *dataPtr, uint8_t index, uint8_t ffd)
{
	BLEP_PackageInfoTypedef *ptr = 0;

	index %= 5;

	switch (index)
	{
	case 0:
		ptr = &ffdInfo_1;
		break;
	case 1:
		ptr = &ffdInfo_2;
		break;
	case 2:
		ptr = &ffdInfo_3;
		break;
	case 3:
		ptr = &ffdInfo_4;
		break;
	case 4:
		ptr = &ffdInfo_5;
		break;
	default:
		ptr = &ffdInfo_5;
		break;
	}

	ptr->id = id;
	ptr->len = SCCU2BLE_FFD_LEN;
	ptr->dataPtr = dataPtr;
	if (ffd == (uint8_t)1)
	{
		USART_SendData(BLEP_Create_FFD01Package, ptr);
	}
	else if (ffd == (uint8_t)2)
	{
		USART_SendData(BLEP_Create_FFD02Package, ptr);
	}
	else
	{
		USART_SendData(BLEP_Create_FFD03Package, ptr);
	}
}

void BLE_Set_AdvFlag(bool flag)
{
	ble_advFlag = flag;
}

uint8_t *BLE_Get_ccuid_adv_data_Ptr(void)
{
	return ccuid_adv_data;
}

uint8_t *BLE_Get_process_data_Ptr(void)
{
	return process_data;
}

bool BLE_Get_Connect_Status(void)
{
	bool status = false;
	uint8_t tmp = (uint8_t)ble_State;
	if (tmp >= (uint8_t)BLESTATE_CONNECT)
	{
		status = true;
	}
	else
	{
		status = false;
	}
	return status;
}
