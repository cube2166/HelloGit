#include "./CAN_Module.h"

#define CAN_SLEEP_TIME 3000
#define CAN_ASSY_TIME 6000

#define CAN_EMPTY_FILL 0x55

static const uint8_t SCCU_VER[] = DEF_CANFW_VER;

static uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;
static uint8_t APP_NAME_BUF[] = FLASH_APP_NAME;

static const uint8_t FIR_VER3[] = DEF_FW_VER;

static uint32_t can_Rx_Id; 								/* CAN Receive ID */
static uint8_t can_Rx_Len; 								/* CAN Receive Length */
static uint8_t can_Rx_Data[8]; 							/* CAN Receive Data */

static bool error_Busoff;

static uint32_t injection_Val = 0; 						/* Current injection_Val Value */
static uint32_t injection_PreVal = 0; 					/* Before injection_Val Value */

static volatile uint16_t sleep_Time = CAN_SLEEP_TIME; 	/* SCCU Sleep Time */
static uint16_t tmp_Sleep_Time = 0; 					/* SCCU Sleep Time temporal */
static bool sleep_Flag = false; 						/* SCCU Sleep Flag */
static volatile bool sleep_Trigger = false; 			/* SCCU Sleep Signal */
static bool tmp_Sleep_Trigger = false; 					/* SCCU Sleep Signal temporal*/

static bool whiteList_Flag = false;						/* SCCU White List Flag */

static uint16_t smartid_58A_Time = 0;					/* Smart ID H58A Time */
static uint16_t smartid_58A_counter = 0;				/* Smart ID H58A Send Counter */

static uint16_t smartid_58B_Time = 0;					/* Smart ID H58B Time */

static uint16_t navi_Info1_Time = 0;					/* Navigation information 1(H592) Time */
static uint16_t navi_Info2_Time = 0;					/* navigation information 2(H593) Time */
static uint16_t navi_Info3_Time = 0;					/* navigation information 3(H594) Time */
static uint16_t navi_Info4_Time = 0;					/* Navigation information 4(H595) Time */
static uint16_t phone_Num_Time = 0; 					/* Phone Number(H58C) Time */
static uint16_t phone_Caller_Time = 0; 					/* Phone Caller(H58D) Time */
static uint16_t sns_Title_Time = 0; 					/* SNS Title(H58E) Time */
static uint16_t sns_Text_Time = 0; 						/* SNS Text(H58F) Time */
static uint16_t sns_Sender_Time = 0;					/* SNS Sender(H591) Time */
static uint16_t music_Aritst_Time = 0;					/* Music Aritst(H59A) Time */
static uint16_t music_Album_Time = 0;					/* Music Album(H59B) Time */
static uint16_t music_Title_Time = 0;					/* Music Title(H59C) Time */
static uint16_t setting_1_Time = 0;						/* Setting 1(H59D) Time */
static uint16_t setting_2_Time = 0;						/* Setting 2(H59E) Time */
static uint16_t voice_Ctrl_Time = 0;					/* Voice Control(H598) Time */

static uint8_t tx_Buf[8]; 								/* CAN Share Buffer */

/* ==================================ASSY DATA================================== */
static uint8_t 	qrCode_Data[25]; 						/* Assy Receive QRCode Data Buffer */
static uint8_t 	qrCode_Sn; 								/* Assy Receive QRCode Continue Frame Serial Number */
static uint8_t 	qrCode_Len; 							/* Assy Receive QRCode Continue Frame Data Length */
static uint8_t 	*qrCode_Ptr; 							/* Assy QRCode Data Buffer Pointer */	
static bool 	qrCode_End; 							/* Assy Receive QRCode Continue Frame Finish */

static bool 	assy_QRCode_Trigger; 					/* Assy Send QRCode Trigger */
static bool 	assy_Firmware_Trigger; 					/* Assy Send Firmware Trigger */
static bool 	assy_Send_Flag; 						/* Assy Send Flag */
static uint8_t 	assy_Buf[50]; 							/* Assy Send Continue Frame Data Buffer */
static uint8_t 	assy_Sn; 								/* Assy Send Continue Frame Data Serial Number */
static uint8_t 	assy_Len; 								/* Assy Send Continue Frame Data Length */
static uint8_t 	*assy_Ptr; 								/* Assy Send Continue Frame Data Buffer Pointer */	
static uint16_t assy_Time; 								/* Assy Time */
/* ---------------------------------------------------------------------------- */
/* ===========================CANID DATA SCCU (V2.0)=========================== */
static uint8_t canid_meter_system_mode_data[CANID_METER_SYSTEM_MODE_LEN];
static uint8_t canid_grip_w_button_status_data[CANID_GRIP_W_BUTTON_STATUS_LEN];
static uint8_t canid_meter_setting_info_data[CANID_METER_SETTING_INFO_LEN];
static uint8_t canid_function_sw_status_data[CANID_FUNCTION_SW_STATUS_LEN];
static uint8_t canid_ecu_indicate_status_data[CANID_ECU_INDICATE_STATUS_LEN];
static uint8_t canid_vh_eg_speed_data[CANID_VH_EG_SPEED_LEN];
static uint8_t canid_ecu_system_mode_data[CANID_ECU_SYSTEM_MODE_LEN];
static uint8_t canid_aps_etv_data[CANID_APS_ETV_LEN];
static uint8_t canid_diag_coadj_egmode_data[CANID_DIAG_COADJ_EGMODE_LEN];
static uint8_t canid_eg_status_data[CANID_EG_STATUS_LEN];
static uint8_t canid_ecu_gear_pos_data[CANID_ECU_GEAR_POS_LEN];
static uint8_t canid_ecu_status_for_tcu_data[CANID_ECU_STATUS_FOR_TCU_LEN];
static uint8_t canid_ecvt_status_for_tcu_data[CANID_ECVT_STATUS_FOR_TCU_LEN];
static uint8_t canid_abs_pressure_data[CANID_ABS_PRESSURE_LEN];
static uint8_t canid_abs_status_data[CANID_ABS_STATUS_LEN];
static uint8_t canid_smart_status_data[CANID_SMART_STATUS_LEN];
static uint8_t canid_tlcu_com_control_data_data[CANID_TLCU_COM_CONTROL_DATA_LEN];
static uint8_t canid_tlcu_control_signal_data[CANID_TLCU_CONTROL_SIGNAL_LEN];
static uint8_t canid_tcu_status_for_ecu_data[CANID_TCU_STATUS_FOR_ECU_LEN];
static uint8_t canid_tbd_data[CANID_TBD_LEN];
static uint8_t canid_meter_system_mode2_data[CANID_METER_SYSTEM_MODE2_LEN];
static uint8_t canid_diag_coadj_egmode_status_data[CANID_DIAG_COADJ_EGMODE_STATUS_LEN];
static uint8_t canid_ecu_setting_info_data[CANID_ECU_SETTING_INFO_LEN];
static uint8_t canid_ecu_data_for_yccs_2_data[CANID_ECU_DATA_FOR_YCCS_2_LEN];
static uint8_t canid_meter_setting_info2_data[CANID_METER_SETTING_INFO2_LEN];
static uint8_t canid_ecu_indicate_status2_data[CANID_ECU_INDICATE_STATUS2_LEN];
static uint8_t canid_vh_eg_speed2_data[CANID_VH_EG_SPEED2_LEN];
static uint8_t canid_ecu_system_mode2_data[CANID_ECU_SYSTEM_MODE2_LEN];
static uint8_t canid_ecu_data_for_yccs_data[CANID_ECU_DATA_FOR_YCCS_LEN];
static uint8_t canid_eg_status2_data[CANID_EG_STATUS2_LEN];
static uint8_t *canid_g2_data;
static uint8_t *canid_g5_data;
static uint8_t *canid_g6_data;
static uint8_t *canid_g7_data;
static uint8_t *canid_g8_data;
static uint8_t *canid_g10_data;
static uint16_t canid_g2_id;
static uint16_t canid_g5_id;
static uint16_t canid_g6_id;
static uint16_t canid_g7_id;
static uint16_t canid_g8_id;
static uint16_t canid_g10_id;
/* ---------------------------------------------------------------------------- */
/* ================================SMARTID DATA================================ */
static uint8_t smartid_58A_Data[SMARTID_58A_LEN];
static bool smartid_58A_Flag;

static uint8_t smartid_58B_Data[SMARTID_58B_LEN];
static bool smartid_58B_Flag;
/* ---------------------------------------------------------------------------- */

/* ===============================SCCU V2.0 DATA=============================== */
static uint8_t navi_Info1_Word_Data[8];
static bool navi_Info1_Flag;

static uint8_t navi_Info2_Word_Data[8];
static bool navi_Info2_Flag;

static uint8_t navi_Info3_Word_Data[8];
static bool navi_Info3_Flag;

static uint8_t navi_Info4_Word_Data[SMARTID_NAVI_INFO4_LEN];
static uint8_t *navi_Info4_Word_Split_Ptr[15];
static uint8_t navi_Info4_Word_Len_Ptr[15];
static uint8_t navi_Info4_Word_Max;
static uint8_t navi_Info4_Packet_Max;
static uint8_t navi_Info4_Packet_Cnt;
static uint8_t navi_Info4_Seq_Number;
static bool navi_Info4_Flag;

static uint8_t phone_Num_Word_Data[SMARTID_PHONE_NUMBER_LEN];
static uint8_t *phone_Num_Word_Split_Ptr[15];
static uint8_t phone_Num_Word_Len_Ptr[15];
static uint8_t phone_Num_Word_Max;
static uint8_t phone_Num_Packet_Max;
static uint8_t phone_Num_Packet_Cnt;
static bool phone_Num_Flag;

static uint8_t phone_Caller_Word_Data[SMARTID_PHONE_CALLER_LEN];
static uint8_t *phone_Caller_Word_Split_Ptr[15];
static uint8_t phone_Caller_Word_Len_Ptr[15];
static uint8_t phone_Caller_Word_Max;
static uint8_t phone_Caller_Packet_Max;
static uint8_t phone_Caller_Packet_Cnt;
static bool phone_Caller_Flag;

static uint8_t sns_Title_Word_Data[SMARTID_SNS_TITLE_LEN];
static uint8_t *sns_Title_Word_Split_Ptr[15];
static uint8_t sns_Title_Word_Len_Ptr[15];
static uint8_t sns_Title_Word_Max;
static uint8_t sns_Title_Packet_Max;
static uint8_t sns_Title_Packet_Cnt;
static uint8_t sns_Title_Seq_Number;
static bool sns_Title_Flag;

static uint8_t sns_Text_Word_Data[SMARTID_SNS_TEXT_LEN];
static uint8_t *sns_Text_Word_Split_Ptr[15];
static uint8_t sns_Text_Word_Len_Ptr[15];
static uint8_t sns_Text_Word_Max;
static uint8_t sns_Text_Packet_Max;
static uint8_t sns_Text_Packet_Cnt;
static uint8_t sns_Text_Seq_Number;
static uint32_t sns_Text_Word_Hash;
static bool sns_Text_Flag;

static uint8_t sns_Sender_Word_Data[SMARTID_SNS_SENDER_LEN];
static uint8_t *sns_Sender_Word_Split_Ptr[15];
static uint8_t sns_Sender_Word_Len_Ptr[15];
static uint8_t sns_Sender_Word_Max;
static uint8_t sns_Sender_Packet_Max;
static uint8_t sns_Sender_Packet_Cnt;
static uint8_t sns_Sender_Seq_Number;
static uint32_t sns_Sender_Word_Hash;
static bool sns_Sender_Flag;

static uint8_t music_Aritst_Word_Data[SMARTID_MUSIC_ARITST_LEN];
static uint8_t *music_Aritst_Word_Split_Ptr[15];
static uint8_t music_Aritst_Word_Len_Ptr[15];
static uint8_t music_Aritst_Word_Max;
static uint8_t music_Aritst_Packet_Max;
static uint8_t music_Aritst_Packet_Cnt;
static uint32_t music_Aritst_Word_Hash;
static bool music_Aritst_Flag;

static uint8_t music_Album_Word_Data[SMARTID_MUSIC_ALBUM_LEN];
static uint8_t *music_Album_Word_Split_Ptr[15];
static uint8_t music_Album_Word_Len_Ptr[15];
static uint8_t music_Album_Word_Max;
static uint8_t music_Album_Packet_Max;
static uint8_t music_Album_Packet_Cnt;
static uint32_t music_Album_Word_Hash;
static bool music_Album_Flag;

static uint8_t music_Title_Word_Data[SMARTID_MUSIC_TITLE_LEN];
static uint8_t *music_Title_Word_Split_Ptr[15];
static uint8_t music_Title_Word_Len_Ptr[15];
static uint8_t music_Title_Word_Max;
static uint8_t music_Title_Packet_Max;
static uint8_t music_Title_Packet_Cnt;
static uint32_t music_Title_Word_Hash;
static bool music_Title_Flag;

static uint8_t setting_1_Word_Data[8];
static bool setting_1_Flag;

static uint8_t setting_2_Word_Data[8];
static bool setting_2_Flag;

static uint8_t voice_Ctrl_Word_Data[8];
static bool voice_Ctrl_Flag;
/* ---------------------------------------------------------------------------- */

static uint16_t error_Busoff_Time;

typedef enum
{
	CAN_NORMAL = 0,
	CAN_BUSOFF,
} CANState_TypeDef;

static CANState_TypeDef can_State; 		/* CAN_Module Current State */
static CANState_TypeDef can_SetState; 	/* CAN_Module Next State */

/*=============================== DIAG state switch ======================================================*/
static void CAN_statusChange(CANState_TypeDef newStat)
{
	can_SetState = newStat;
}

void CAN_ISR(void)
{
	tmp_Sleep_Time = sleep_Time;
	if (tmp_Sleep_Time > 0)
	{
		tmp_Sleep_Time = tmp_Sleep_Time - 1;
	}
	else
	{
		tmp_Sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		sleep_Trigger = true;
	}
	sleep_Time = tmp_Sleep_Time;
}

static bool CAN_Get_FIFOElement(void)
{
	bool status = false;

	status = BSP_FDCAN1_Get_Rx_Queue_0(&can_Rx_Id, &can_Rx_Len, can_Rx_Data);

	return status;
}

typedef enum
{
	ASSYDATA_OK 				= 0x00,
	ASSYDATA_CHECKSUM_ERR 		= 0x01,
	ASSYDATA_DATA_EMPTY 		= 0x02,
	ASSYDATA_STOP_READ_QRCODE 	= 0x03,
	ASSYDATA_COMMON_ERR 		= 0x04,
	ASSYDATA_TIMEOUT_ERR 		= 0x05,
} ASSYDATA_TypeDef;

typedef enum
{
	ASSYTYPE_QRCODE 			= 0x80,
	ASSYTYPE_FIRMWARE 			= 0x81,
	ASSYTYPE_ADV 				= 0x82,
	ASSYTYPE_CLOSE_QRCODE 		= 0x83,
} ASSYTYPE_TypeDef;

static void CAN_Send_Assy_Response(ASSYTYPE_TypeDef type, ASSYDATA_TypeDef data)
{
	uint8_t tmp[8];
	tmp[0] = 0x04;
	tmp[1] = 0x21;
	tmp[2] = (uint8_t)type;
	tmp[3] = (uint8_t)data;
	tmp[4] = tmp[1] + tmp[2] + tmp[3];
	tmp[4] = (~tmp[4]) + 1;
	tmp[5] = 0X55;
	tmp[6] = 0X55;
	tmp[7] = 0X55;
	(void)CAN_Send_DATA(C2PID, 8, tmp);
}

static void CAN_Send_Assy_Data(void)
{
	uint8_t tmp[8];
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&assy_Time, (uint16_t)50);
	if (tmp_Result == true)
	{
		if (assy_Send_Flag == false)
		{
			if (assy_QRCode_Trigger == true)
			{
				assy_QRCode_Trigger = false;
				tmp_Result = Eeprom_Get_QRCODE_Save();
				if (tmp_Result == true)
				{
					assy_Send_Flag = true;
					assy_Ptr = assy_Buf;
					(void)memcpy(assy_Ptr, Eeprom_Get_CCUID_Ptr(), 14);
					assy_Ptr += 14;
					(void)memcpy(assy_Ptr, Eeprom_Get_PassKey_Ptr(), 6);
					assy_Ptr += 6;
					(void)memcpy(assy_Ptr, Eeprom_Get_Dummy_Ptr(), 4);
					assy_Ptr += 4;
					*assy_Ptr = Eeprom_CalcChkSum(assy_Buf, 24);

					assy_Sn = 0;
					assy_Len = (uint8_t)0x19;
					assy_Ptr = assy_Buf;

					tmp[0] = (uint8_t)0x10;
					tmp[1] = assy_Len;
					(void)memcpy((tmp + 2), assy_Ptr, 6);

					assy_Sn = assy_Sn + 1;
					assy_Ptr += (uint8_t)6;
					assy_Len = assy_Len - (uint8_t)6;
					(void)CAN_Send_DATA(C2PID, 8, tmp);
				}
				else
				{
					/* ignore */
				}
			}
			else if (assy_Firmware_Trigger == true)
			{
				assy_Firmware_Trigger = false;
				assy_Send_Flag = true;

				BLEP_FirmwareTypedef *ble_firmware = BLEP_Get_BleFirmware_Ptr();

				assy_Ptr = assy_Buf;
				(void)memcpy(assy_Ptr, SCCU_VER, 6);
				assy_Ptr += 6;
				(void)memcpy(assy_Ptr, ble_firmware->Data.ALL, 7);
				assy_Ptr += 7;
				*assy_Ptr = Eeprom_CalcChkSum(assy_Buf, 13);

				assy_Sn = 0;
				assy_Len = (uint8_t)14;
				assy_Ptr = assy_Buf;

				tmp[0] = (uint8_t)0x10;
				tmp[1] = assy_Len;
				(void)memcpy((tmp + 2), assy_Ptr, 6);

				assy_Sn = assy_Sn + 1;
				assy_Ptr += (uint8_t)6;
				assy_Len = assy_Len - (uint8_t)6;
				(void)CAN_Send_DATA(C2PID, 8, tmp);
			}
			else
			{
				/* ignore */
			}
		}
		else
		{
			(void)memset(tmp, 0x55, 8);

			tmp[0] = 0x20;
			tmp[0] |= assy_Sn;
			if (assy_Len >= (uint8_t)7)
			{
				(void)memcpy((tmp + 1), assy_Ptr, 7);
				assy_Sn = assy_Sn + 1;
				assy_Sn = assy_Sn & 0xF;

				assy_Ptr += (uint8_t)7;
				assy_Len -= (uint8_t)7;
				if (assy_Len == (uint8_t)0)
				{
					assy_Send_Flag = false;
				}
				else
				{
					/* continue */
				}
			}
			else
			{
				(void)memcpy((tmp + 1), assy_Ptr, assy_Len);
				assy_Send_Flag = false;
			}
			(void)CAN_Send_DATA(C2PID, 8, tmp);
		}
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Handle_Assy(uint8_t *data)
{
	DIAGPCI_TypeDef rx_pci = (DIAGPCI_TypeDef)(data[0] & (uint8_t)0xF0);
	ASSYTYPE_TypeDef rx_type = (ASSYTYPE_TypeDef)data[2];
	uint16_t tmp = 0;
	bool tmp_Result;
	switch (rx_pci)
	{
	case PCI_SINGLE_FRAME:
		if (data[1] == (uint8_t)0x21)
		{
			tmp = data[2];
			tmp = tmp + data[3];
			if (tmp == (uint16_t)0xDF)
			{
				if (rx_type == ASSYTYPE_QRCODE) /* Read QRCode */
				{
					tmp_Result = Eeprom_Get_QRCODE_Save();
					if (tmp_Result == true)
					{
						tmp_Result = Eeprom_Get_ReadQRCODE_Close();
						if (tmp_Result == true)
						{
							/* send read error */
							CAN_Send_Assy_Response(rx_type, ASSYDATA_CHECKSUM_ERR);
						}
						else
						{
							/* send qr code */
							assy_QRCode_Trigger = true;
						}
					}
					else
					{
						/* send qrcode empty */
						CAN_Send_Assy_Response(rx_type, ASSYDATA_DATA_EMPTY);
					}
				}
				else if (rx_type == ASSYTYPE_FIRMWARE) /* Read Firmware */
				{
					/* send firmware */
					assy_Firmware_Trigger = true;
				}
				else if (rx_type == ASSYTYPE_ADV) /* Open BLE Advertising */
				{
					tmp_Result = Eeprom_Get_QRCODE_Save();
					if (tmp_Result == true)
					{
						/* send open BLE Advertising fail */
						CAN_Send_Assy_Response(rx_type, ASSYDATA_CHECKSUM_ERR);
						BLE_Set_AdvFlag(false);
					}
					else
					{
						/* open adv */
						BLE_Load_advData();
						BLE_Set_AdvFlag(true);
						CAN_Send_Assy_Response(rx_type, ASSYDATA_OK);
					}
				}
				else if (rx_type == ASSYTYPE_CLOSE_QRCODE) /* Close Read QRCode */
				{
					tmp_Result = Eeprom_Get_QRCODE_Save();
					if (tmp_Result == true)
					{
						Eeprom_ReqBackupParameterData();
						CAN_Send_Assy_Response(rx_type, ASSYDATA_OK);
					}
					else
					{
						CAN_Send_Assy_Response(rx_type, ASSYDATA_COMMON_ERR);
					}
				}
				else
				{
					CAN_Send_Assy_Response((ASSYTYPE_TypeDef)data[2], ASSYDATA_COMMON_ERR);
				}
			}
			else
			{
				CAN_Send_Assy_Response(rx_type, ASSYDATA_CHECKSUM_ERR);
			}
		}
		else
		{
			/* ignore */
		}
		break;

	case PCI_FIRST_FRAME:
		tmp = (data[0] & (uint8_t)0x0F);
		tmp <<= (uint16_t)8;
		tmp |= data[1];
		tmp_Result = Eeprom_Get_QRCODE_Save();
		if (tmp_Result == true)
		{
			CAN_Send_Assy_Response(ASSYTYPE_QRCODE, ASSYDATA_CHECKSUM_ERR);
		}
		else
		{
			if (tmp == (uint16_t)0x19) /* Write QRCode FF */
			{
				(void)memset(qrCode_Data, 0, 25);
				qrCode_Sn = 0;
				qrCode_End = false;
				qrCode_Len = (uint8_t)tmp;
				qrCode_Ptr = qrCode_Data;

				(void)memcpy(qrCode_Ptr, (data + 2), 6);
				qrCode_Sn = qrCode_Sn + 1;
				qrCode_Ptr += (uint8_t)6;
				qrCode_Len = qrCode_Len - (uint8_t)6;
			}
			else
			{
				CAN_Send_Assy_Response(ASSYTYPE_QRCODE, ASSYDATA_CHECKSUM_ERR);
			}
		}
		break;
	case PCI_CONSECUTIVE_FRAME:
		tmp = data[0] & (uint8_t)0x0F;
		if (qrCode_Sn == (uint8_t)tmp) /* Write QRCode CF */
		{
			if (qrCode_Len >= (uint8_t)7)
			{
				(void)memcpy(qrCode_Ptr, (data + 1), 7);
				qrCode_Sn = qrCode_Sn + 1;
				qrCode_Ptr += (uint8_t)7;
				qrCode_Len -= (uint8_t)7;

				if (qrCode_Len == 0)
				{
					qrCode_Ptr = 0;
					qrCode_Sn = 0;
					qrCode_End = true;
				}
				else
				{
					/* continute */
				}
			}
			else
			{
				(void)memcpy(qrCode_Ptr, (data + 1), qrCode_Len);
				qrCode_Len = 0;
				qrCode_Ptr = 0;
				qrCode_Sn = 0;
				qrCode_End = true;
			}
			if (qrCode_End == true)
			{
				qrCode_End = false;
				uint8_t tmp_ChkSum = Eeprom_CalcChkSum(qrCode_Data, 24);
				if (tmp_ChkSum == qrCode_Data[24])
				{
					Eeprom_ReqBackupQRCODE(qrCode_Data);
					BLE_Load_advData();

					/* send qr code */
					assy_QRCode_Trigger = true;
				}
				else
				{
					CAN_Send_Assy_Response(ASSYTYPE_QRCODE, ASSYDATA_CHECKSUM_ERR);
				}
			}
			else
			{
				/* ignore */
			}
		}
		else
		{
			CAN_Send_Assy_Response(ASSYTYPE_QRCODE, ASSYDATA_CHECKSUM_ERR);
		}
		break;
	case PCI_FLOW_CONTROL:
		/* ignore */
		break;
	default:
		break;
	}
}

static void CAN_Process_Data(uint32_t tmp_id, uint8_t tmp_length, uint8_t *tmp_data)
{
	if (tmp_id == PC2MCU_ID_CMD)
	{
		uint8_t tx_Buf[8];
		if (tmp_length == 8)
		{
			uint16_t U16_Val = 0;
			U16_Val = tmp_data[1] + (tmp_data[2] << 8);
			if ((tmp_data[4] == APP_NAME_BUF[0]) &&
				(tmp_data[5] == APP_NAME_BUF[1]) &&
				(tmp_data[6] == APP_NAME_BUF[2]) &&
				(tmp_data[7] == APP_NAME_BUF[3]))
			{
				tx_Buf[0] = 1;
				tx_Buf[1] = tmp_data[1];
				tx_Buf[2] = tmp_data[2];
				tx_Buf[3] = 0;
				tx_Buf[4] = APP_NAME_BUF[0];
				tx_Buf[5] = APP_NAME_BUF[1];
				tx_Buf[6] = APP_NAME_BUF[2];
				tx_Buf[7] = APP_NAME_BUF[3];

				if (U16_Val == PC2MCU_CMD_BOOT)
				{

					ramStart[0] = BTL_GUARD;
					ramStart[1] = BTL_GUARD;
					ramStart[2] = BTL_GUARD;
					ramStart[3] = BTL_GUARD;

					(void)CAN_Send_DATA(MCU2PC_ID_CMD, 8, tx_Buf);

					NVIC_SystemReset();
				}
				else if (U16_Val == PC2MCU_CMD_VERSION)
				{
					(void)memcpy(tx_Buf, FIR_VER3, 8);

					(void)CAN_Send_DATA(MCU2PC_ID_CMD, 8, tx_Buf);
				}
				else
				{
					tx_Buf[3] = 1;
					(void)CAN_Send_DATA(MCU2PC_ID_CMD, 8, tx_Buf);
				}
			}
			else if ((tmp_data[4] == 0x3F) &&
					 (tmp_data[5] == 0x3F) &&
					 (tmp_data[6] == 0x3F) &&
					 (tmp_data[7] == 0x3F))
			{
				tx_Buf[0] = 1;
				tx_Buf[1] = tmp_data[1];
				tx_Buf[2] = tmp_data[2];
				tx_Buf[3] = 0;
				tx_Buf[4] = APP_NAME_BUF[0];
				tx_Buf[5] = APP_NAME_BUF[1];
				tx_Buf[6] = APP_NAME_BUF[2];
				tx_Buf[7] = APP_NAME_BUF[3];

				if (U16_Val == PC2MCU_CMD_NAME)
				{
					(void)CAN_Send_DATA(MCU2PC_ID_CMD, 8, tx_Buf);
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
			/* ignore */
		}
	}
	else if (tmp_id == P2SID)
	{
		CAN_Handle_Assy(tmp_data);

		sleep_Time = (uint16_t)CAN_ASSY_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_EG_STATUS)
	{

		canid_eg_status_data[0] = tmp_data[2];
		canid_eg_status_data[1] = tmp_data[3];

		canid_g10_id = tmp_id;
		canid_g10_data = canid_eg_status_data;

		if (tmp_data[3] != 0 || tmp_data[2] != 0)
		{
			injection_PreVal = tmp_data[3];
			uint32_t tmp = (uint32_t)tmp_data[2] << 8;
			if (((uint32_t)0xFFFFFFFF - injection_PreVal) >= tmp)
			{
				injection_PreVal = injection_PreVal + tmp;
			}
			else
			{
				injection_PreVal = (uint32_t)0xFFFFFFFF;
			}

			if (((uint32_t)0xFFFFFFFF - injection_Val) >= injection_PreVal)
			{
				injection_Val = injection_Val + injection_PreVal;
			}
			else
			{
				injection_Val = (uint32_t)0xFFFFFFFF;
			}
		}

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_METER_SYSTEM_MODE)
	{

		canid_meter_system_mode_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_GRIP_W_BUTTON_STATUS)
	{

		canid_grip_w_button_status_data[0] = tmp_data[0];

		canid_g2_id = tmp_id;
		canid_g2_data = canid_grip_w_button_status_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_METER_SETTING_INFO)
	{

		canid_meter_setting_info_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_FUNCTION_SW_STATUS)
	{

		canid_function_sw_status_data[0] = tmp_data[0];
		canid_function_sw_status_data[1] = tmp_data[1];
		canid_function_sw_status_data[2] = tmp_data[2];

		BTN_Callback(tmp_data[0], tmp_data[1]);

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_INDICATE_STATUS)
	{

		canid_ecu_indicate_status_data[0] = tmp_data[3];
		canid_ecu_indicate_status_data[1] = tmp_data[4];
		canid_ecu_indicate_status_data[2] = tmp_data[5];

		canid_g5_id = tmp_id;
		canid_g5_data = canid_ecu_indicate_status_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_VH_EG_SPEED)
	{

		canid_vh_eg_speed_data[0] = tmp_data[7];

		canid_g6_id = tmp_id;
		canid_g6_data = canid_vh_eg_speed_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_SYSTEM_MODE)
	{

		canid_ecu_system_mode_data[0] = tmp_data[4];
		canid_ecu_system_mode_data[1] = tmp_data[6];
		canid_ecu_system_mode_data[2] = tmp_data[7];

		canid_g7_id = tmp_id;
		canid_g7_data = canid_ecu_system_mode_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_APS_ETV)
	{

		canid_aps_etv_data[0] = tmp_data[4];
		canid_aps_etv_data[1] = tmp_data[5];
		canid_aps_etv_data[2] = tmp_data[6];

		canid_g8_id = tmp_id;
		canid_g8_data = canid_aps_etv_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_DIAG_COADJ_EGMODE)
	{

		canid_diag_coadj_egmode_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_GEAR_POS)
	{

		canid_ecu_gear_pos_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_STATUS_FOR_TCU)
	{

		canid_ecu_status_for_tcu_data[0] = tmp_data[0];
		canid_ecu_status_for_tcu_data[1] = tmp_data[1];
		canid_ecu_status_for_tcu_data[2] = tmp_data[2];
		canid_ecu_status_for_tcu_data[3] = tmp_data[3];
		canid_ecu_status_for_tcu_data[4] = tmp_data[4];
		canid_ecu_status_for_tcu_data[5] = tmp_data[5];
		canid_ecu_status_for_tcu_data[6] = tmp_data[6];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECVT_STATUS_FOR_TCU)
	{

		canid_ecvt_status_for_tcu_data[0] = tmp_data[0];
		canid_ecvt_status_for_tcu_data[1] = tmp_data[1];
		canid_ecvt_status_for_tcu_data[2] = tmp_data[2];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ABS_PRESSURE)
	{

		canid_abs_pressure_data[0] = tmp_data[0];
		canid_abs_pressure_data[1] = tmp_data[1];
		canid_abs_pressure_data[2] = tmp_data[2];
		canid_abs_pressure_data[3] = tmp_data[3];
		canid_abs_pressure_data[4] = tmp_data[4];
		canid_abs_pressure_data[5] = tmp_data[5];
		canid_abs_pressure_data[6] = tmp_data[6];
		canid_abs_pressure_data[7] = tmp_data[7];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ABS_STATUS)
	{

		canid_abs_status_data[0] = tmp_data[0];
		canid_abs_status_data[1] = tmp_data[1];
		canid_abs_status_data[2] = tmp_data[4];
		canid_abs_status_data[3] = tmp_data[5];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_SMART_STATUS)
	{

		canid_smart_status_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_TLCU_COM_CONTROL_DATA)
	{

		canid_tlcu_com_control_data_data[0] = tmp_data[0];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_TLCU_CONTROL_SIGNAL)
	{

		canid_tlcu_control_signal_data[0] = tmp_data[0];
		canid_tlcu_control_signal_data[1] = tmp_data[1];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_TCU_STATUS_FOR_ECU)
	{

		canid_tcu_status_for_ecu_data[0] = tmp_data[0];
		canid_tcu_status_for_ecu_data[1] = tmp_data[1];
		canid_tcu_status_for_ecu_data[2] = tmp_data[2];
		canid_tcu_status_for_ecu_data[3] = tmp_data[3];
		canid_tcu_status_for_ecu_data[4] = tmp_data[4];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_TBD)
	{

		canid_tbd_data[0] = tmp_data[0];
		canid_tbd_data[1] = tmp_data[1];
		canid_tbd_data[2] = tmp_data[2];
		canid_tbd_data[3] = tmp_data[3];
		canid_tbd_data[4] = tmp_data[4];
		canid_tbd_data[5] = tmp_data[5];
		canid_tbd_data[6] = tmp_data[6];
		canid_tbd_data[7] = tmp_data[7];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_METER_SYSTEM_MODE2)
	{

		canid_meter_system_mode2_data[0] = tmp_data[0];
		canid_meter_system_mode2_data[1] = tmp_data[1];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_DIAG_COADJ_EGMODE_STATUS)
	{

		canid_diag_coadj_egmode_status_data[0] = tmp_data[0];
		canid_diag_coadj_egmode_status_data[1] = tmp_data[7];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_SETTING_INFO)
	{

		canid_ecu_setting_info_data[0] = tmp_data[0];
		canid_ecu_setting_info_data[1] = tmp_data[1];
		canid_ecu_setting_info_data[2] = tmp_data[2];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_DATA_FOR_YCCS_2)
	{

		canid_ecu_data_for_yccs_2_data[0] = tmp_data[3];

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_METER_SETTING_INFO2)
	{

		canid_meter_setting_info2_data[0] = tmp_data[6];

		canid_g2_id = tmp_id;
		canid_g2_data = canid_meter_setting_info2_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_INDICATE_STATUS2)
	{

		canid_ecu_indicate_status2_data[0] = tmp_data[3];
		canid_ecu_indicate_status2_data[1] = tmp_data[4];
		canid_ecu_indicate_status2_data[2] = tmp_data[5];

		canid_g5_id = tmp_id;
		canid_g5_data = canid_ecu_indicate_status2_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_VH_EG_SPEED2)
	{

		canid_vh_eg_speed2_data[0] = tmp_data[6];

		canid_g6_id = tmp_id;
		canid_g6_data = canid_vh_eg_speed2_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_SYSTEM_MODE2)
	{

		canid_ecu_system_mode2_data[0] = tmp_data[4];
		canid_ecu_system_mode2_data[1] = tmp_data[6];
		canid_ecu_system_mode2_data[2] = tmp_data[7];

		canid_g7_id = tmp_id;
		canid_g7_data = canid_ecu_system_mode2_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_ECU_DATA_FOR_YCCS)
	{

		canid_ecu_data_for_yccs_data[0] = tmp_data[3];
		canid_ecu_data_for_yccs_data[1] = tmp_data[4];
		canid_ecu_data_for_yccs_data[2] = tmp_data[5];

		canid_g8_id = tmp_id;
		canid_g8_data = canid_ecu_data_for_yccs_data;

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else if (tmp_id == CANID_EG_STATUS2)
	{

		canid_eg_status2_data[0] = tmp_data[2];
		canid_eg_status2_data[1] = tmp_data[3];

		canid_g10_id = tmp_id;
		canid_g10_data = canid_eg_status2_data;

		if (tmp_data[3] != 0 || tmp_data[2] != 0)
		{
			injection_PreVal = tmp_data[3];
			uint32_t tmp = (uint32_t)tmp_data[2] << 8;
			if (((uint32_t)0xFFFFFFFF - injection_PreVal) >= tmp)
			{
				injection_PreVal = injection_PreVal + tmp;
			}
			else
			{
				injection_PreVal = (uint32_t)0xFFFFFFFF;
			}

			if (((uint32_t)0xFFFFFFFF - injection_Val) >= injection_PreVal)
			{
				injection_Val = injection_Val + injection_PreVal;
			}
			else
			{
				injection_Val = (uint32_t)0xFFFFFFFF;
			}
		}

		sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		whiteList_Flag = true;
	}
	else
	{
		/* ignore Data */
	}
}

static void CAN_Create_H58A(void)
{
	(void)BLEP_Get_H58A(smartid_58A_Data);
}

static void CAN_Send_H58A(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&smartid_58A_Time, (uint16_t)1000);
	if (tmp_Result == true)
	{
		if (smartid_58A_counter > (uint8_t)0)
		{
			smartid_58A_Data[7] = (uint8_t)0x01 | smartid_58A_Data[7];
			smartid_58A_counter--;
		}
		else
		{
			smartid_58A_Data[7] = (uint8_t)0xFE & smartid_58A_Data[7];
		}
		(void)CAN_Send_DATA(SMARTID_58A, SMARTID_58A_LEN, smartid_58A_Data);
		(void)memset(smartid_58A_Data, 0, SMARTID_58A_LEN);
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Create_H58B(void)
{
	(void)BLEP_Get_H58B(smartid_58B_Data);
}

static void CAN_Send_H58B(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&smartid_58B_Time, (uint16_t)1000);
	if (tmp_Result == true)
	{

		smartid_58B_Data[2] = CAN_EMPTY_FILL;
		smartid_58B_Data[3] = CAN_EMPTY_FILL;
		smartid_58B_Data[4] = CAN_EMPTY_FILL;
		smartid_58B_Data[5] = CAN_EMPTY_FILL;
		smartid_58B_Data[6] = CAN_EMPTY_FILL;
		(void)CAN_Send_DATA(SMARTID_58B, SMARTID_58B_LEN, smartid_58B_Data);
		(void)memset(smartid_58B_Data, 0, SMARTID_58B_LEN);
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Create_Phone_Info(void)
{
	bool tmp_Result = false;

	if ((phone_Num_Flag == false) && (phone_Caller_Flag == false))
	{
		tmp_Result = BLEP_Get_Phone_Info(phone_Num_Word_Data, phone_Caller_Word_Data);

		if (tmp_Result == true)
		{
			UTF_Split_Word(phone_Num_Word_Data, SMARTID_PHONE_NUMBER_LEN, phone_Num_Word_Split_Ptr,
						   phone_Num_Word_Len_Ptr, &phone_Num_Word_Max, &phone_Num_Packet_Max);

			phone_Num_Packet_Cnt = 0;
			phone_Num_Flag = true;

			UTF_Split_Word(phone_Caller_Word_Data, SMARTID_PHONE_CALLER_LEN, phone_Caller_Word_Split_Ptr,
						   phone_Caller_Word_Len_Ptr, &phone_Caller_Word_Max, &phone_Caller_Packet_Max);

			phone_Caller_Packet_Cnt = 0;
			phone_Caller_Flag = true;
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

static void CAN_Send_H58C(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&phone_Num_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (phone_Num_Flag == true)
		{
			if (phone_Num_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_PHONE_NUMBER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					phone_Num_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (phone_Num_Packet_Cnt < phone_Num_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(phone_Num_Word_Data[phone_Num_Packet_Cnt * 6]), 6);
				phone_Num_Packet_Cnt++;
				tx_Buf[0] = phone_Num_Packet_Cnt;
				tx_Buf[0] |= (phone_Num_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (phone_Num_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_PHONE_NUMBER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (phone_Num_Packet_Cnt >= phone_Num_Packet_Max)
					{
						phone_Num_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (phone_Num_Packet_Cnt > 0)
					{
						phone_Num_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				phone_Num_Flag = false;
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

static void CAN_Send_H58D(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&phone_Caller_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (phone_Caller_Flag == true)
		{
			if (phone_Caller_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_PHONE_CALLER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					phone_Caller_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (phone_Caller_Packet_Cnt < phone_Caller_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(phone_Caller_Word_Data[phone_Caller_Packet_Cnt * 6]), 6);
				phone_Caller_Packet_Cnt++;
				tx_Buf[0] = phone_Caller_Packet_Cnt;
				tx_Buf[0] |= (phone_Caller_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (phone_Caller_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_PHONE_CALLER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (phone_Caller_Packet_Cnt >= phone_Caller_Packet_Max)
					{
						phone_Caller_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (phone_Caller_Packet_Cnt > 0)
					{
						phone_Caller_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				phone_Caller_Flag = false;
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

static void CAN_Create_Music_Info(void)
{
	bool tmp_Result = false;

	if ((music_Aritst_Flag == false) && (music_Album_Flag == false) && (music_Title_Flag == false))
	{
		tmp_Result = BLEP_Get_Music_Info(music_Aritst_Word_Data, music_Album_Word_Data, music_Title_Word_Data);

		if (tmp_Result == true)
		{
			UTF_Split_Word(music_Aritst_Word_Data, SMARTID_MUSIC_ARITST_LEN, music_Aritst_Word_Split_Ptr,
						   music_Aritst_Word_Len_Ptr, &music_Aritst_Word_Max, &music_Aritst_Packet_Max);

			music_Aritst_Packet_Cnt = 0;
			music_Aritst_Flag = true;

			UTF_Split_Word(music_Album_Word_Data, SMARTID_MUSIC_ALBUM_LEN, music_Album_Word_Split_Ptr,
						   music_Album_Word_Len_Ptr, &music_Album_Word_Max, &music_Album_Packet_Max);

			music_Album_Packet_Cnt = 0;
			music_Album_Flag = true;

			UTF_Split_Word(music_Title_Word_Data, SMARTID_MUSIC_TITLE_LEN, music_Title_Word_Split_Ptr,
						   music_Title_Word_Len_Ptr, &music_Title_Word_Max, &music_Title_Packet_Max);

			music_Title_Packet_Cnt = 0;
			music_Title_Flag = true;
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

static void CAN_Send_H59A(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&music_Aritst_Time, (uint16_t)500);
	if (tmp_Result == true)
	{
		if (music_Aritst_Flag == true)
		{
			if (music_Aritst_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_ARITST, 8, tx_Buf);
				if (tmp_Result == true)
				{
					music_Aritst_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (music_Aritst_Packet_Cnt < music_Aritst_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(music_Aritst_Word_Data[music_Aritst_Packet_Cnt * 6]), 6);
				music_Aritst_Packet_Cnt++;
				tx_Buf[0] = music_Aritst_Packet_Cnt;
				tx_Buf[0] |= (music_Aritst_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (music_Aritst_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_ARITST, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (music_Aritst_Packet_Cnt >= music_Aritst_Packet_Max)
					{
						music_Aritst_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (music_Aritst_Packet_Cnt > 0)
					{
						music_Aritst_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				music_Aritst_Flag = false;
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

static void CAN_Send_H59B(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&music_Album_Time, (uint16_t)500);
	if (tmp_Result == true)
	{
		if (music_Album_Flag == true)
		{
			if (music_Album_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_ALBUM, 8, tx_Buf);
				if (tmp_Result == true)
				{
					music_Album_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (music_Album_Packet_Cnt < music_Album_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(music_Album_Word_Data[music_Album_Packet_Cnt * 6]), 6);
				music_Album_Packet_Cnt++;
				tx_Buf[0] = music_Album_Packet_Cnt;
				tx_Buf[0] |= (music_Album_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (music_Album_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_ALBUM, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (music_Album_Packet_Cnt >= music_Album_Packet_Max)
					{
						music_Album_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (music_Album_Packet_Cnt > 0)
					{
						music_Album_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				music_Album_Flag = false;
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

static void CAN_Send_H59C(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&music_Title_Time, (uint16_t)500);
	if (tmp_Result == true)
	{
		if (music_Title_Flag == true)
		{
			if (music_Title_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_TITLE, 8, tx_Buf);
				if (tmp_Result == true)
				{
					music_Title_Flag = false;
				}
				else
				{
					/* code */
				}
			}
			else if (music_Title_Packet_Cnt < music_Title_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(music_Title_Word_Data[music_Title_Packet_Cnt * 6]), 6);
				music_Title_Packet_Cnt++;
				tx_Buf[0] = music_Title_Packet_Cnt;
				tx_Buf[0] |= (music_Title_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (music_Title_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_MUSIC_TITLE, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (music_Title_Packet_Cnt >= music_Title_Packet_Max)
					{
						music_Title_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (music_Title_Packet_Cnt > 0)
					{
						music_Title_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				music_Title_Flag = false;
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

static void CAN_Create_SNS_Info(void)
{
	bool tmp_Result = false;

	if ((sns_Title_Flag == false) && (sns_Text_Flag == false) && (sns_Sender_Flag == false))
	{
		tmp_Result = BLEP_Get_SNS_Info(sns_Title_Word_Data, sns_Text_Word_Data, sns_Sender_Word_Data);

		if (tmp_Result == true)
		{
			UTF_Split_Word((sns_Title_Word_Data + 1), (SMARTID_SNS_TITLE_LEN - 1), sns_Title_Word_Split_Ptr,
						   sns_Title_Word_Len_Ptr, &sns_Title_Word_Max, &sns_Title_Packet_Max);

			sns_Title_Packet_Cnt = 0;
			sns_Title_Seq_Number = sns_Title_Word_Data[0];
			sns_Title_Flag = true;

			UTF_Split_Word((sns_Text_Word_Data + 1), (SMARTID_SNS_TEXT_LEN - 1), sns_Text_Word_Split_Ptr,
						   sns_Text_Word_Len_Ptr, &sns_Text_Word_Max, &sns_Text_Packet_Max);

			sns_Text_Packet_Cnt = 0;
			sns_Text_Seq_Number = sns_Text_Word_Data[0];
			sns_Text_Flag = true;

			UTF_Split_Word((sns_Sender_Word_Data + 1), (SMARTID_SNS_SENDER_LEN - 1), sns_Sender_Word_Split_Ptr,
						   sns_Sender_Word_Len_Ptr, &sns_Sender_Word_Max, &sns_Sender_Packet_Max);

			sns_Sender_Packet_Cnt = 0;
			sns_Sender_Seq_Number = sns_Sender_Word_Data[0];
			sns_Sender_Flag = true;
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

static void CAN_Send_H58E(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&sns_Title_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (sns_Title_Flag == true)
		{
			if (sns_Title_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = (sns_Title_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_TITLE, 8, tx_Buf);
				if (tmp_Result == true)
				{
					sns_Title_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (sns_Title_Packet_Cnt < sns_Title_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(sns_Title_Word_Data[(sns_Title_Packet_Cnt * 6) + 1]), 6);
				sns_Title_Packet_Cnt++;
				tx_Buf[0] = sns_Title_Packet_Cnt;
				tx_Buf[0] |= (sns_Title_Packet_Max << (uint8_t)4);
				tx_Buf[1] = (sns_Title_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (sns_Title_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_TITLE, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (sns_Title_Packet_Cnt >= sns_Title_Packet_Max)
					{
						sns_Title_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (sns_Title_Packet_Cnt > 0)
					{
						sns_Title_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				sns_Title_Flag = false;
			}
		}
		else
		{
			/* code */
		}
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Send_H58F(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&sns_Text_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (sns_Text_Flag == true)
		{
			if (sns_Text_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = (sns_Text_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_TEXT, 8, tx_Buf);
				if (tmp_Result == true)
				{
					sns_Text_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (sns_Text_Packet_Cnt < sns_Text_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(sns_Text_Word_Data[(sns_Text_Packet_Cnt * 6) + 1]), 6);
				sns_Text_Packet_Cnt++;
				tx_Buf[0] = sns_Text_Packet_Cnt;
				tx_Buf[0] |= (sns_Text_Packet_Max << (uint8_t)4);
				tx_Buf[1] = (sns_Text_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (sns_Text_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_TEXT, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (sns_Text_Packet_Cnt >= sns_Text_Packet_Max)
					{
						sns_Text_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (sns_Text_Packet_Cnt > 0)
					{
						sns_Text_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				sns_Text_Flag = false;
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

static void CAN_Send_H591(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&sns_Sender_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (sns_Sender_Flag == true)
		{
			if (sns_Sender_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = (sns_Sender_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_SENDER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					sns_Sender_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (sns_Sender_Packet_Cnt < sns_Sender_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(sns_Sender_Word_Data[(sns_Sender_Packet_Cnt * 6) + 1]), 6);
				sns_Sender_Packet_Cnt++;
				tx_Buf[0] = sns_Sender_Packet_Cnt;
				tx_Buf[0] |= (sns_Sender_Packet_Max << (uint8_t)4);
				tx_Buf[1] = (sns_Sender_Seq_Number & (uint8_t)0xF);
				tx_Buf[1] |= (sns_Sender_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_SNS_SENDER, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (sns_Sender_Packet_Cnt >= sns_Sender_Packet_Max)
					{
						sns_Sender_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (sns_Sender_Packet_Cnt > 0)
					{
						sns_Sender_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				sns_Sender_Flag = false;
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

static void CAN_Create_Road_Info(void)
{
	bool tmp_Result = false;

	if (navi_Info4_Flag == false)
	{
		tmp_Result = BLEP_Get_Road_Info(navi_Info4_Word_Data);

		if (tmp_Result == true)
		{
			UTF_Split_Word((navi_Info4_Word_Data + 1), (SMARTID_NAVI_INFO4_LEN - 1), navi_Info4_Word_Split_Ptr,
						   navi_Info4_Word_Len_Ptr, &navi_Info4_Word_Max, &navi_Info4_Packet_Max);

			navi_Info4_Packet_Cnt = 0;
			navi_Info4_Seq_Number = navi_Info4_Word_Data[0];
			navi_Info4_Flag = true;
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

static void CAN_Send_H595(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&navi_Info4_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (navi_Info4_Flag == true)
		{
			if (navi_Info4_Packet_Max == 0)
			{
				(void)memset(tx_Buf, 0, 8);
				tx_Buf[0] = 1;
				tx_Buf[0] |= (1 << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (0 << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_NAVI_INFO4, 8, tx_Buf);
				if (tmp_Result == true)
				{
					navi_Info4_Flag = false;
				}
				else
				{
					/* nothing */
				}
			}
			else if (navi_Info4_Packet_Cnt < navi_Info4_Packet_Max)
			{
				(void)memset(tx_Buf, 0, 8);
				(void)memcpy((tx_Buf + (uint8_t)2), &(navi_Info4_Word_Data[navi_Info4_Packet_Cnt * 6]), 6);
				navi_Info4_Packet_Cnt++;
				tx_Buf[0] = navi_Info4_Packet_Cnt;
				tx_Buf[0] |= (navi_Info4_Packet_Max << (uint8_t)4);
				tx_Buf[1] = 0;
				tx_Buf[1] |= (navi_Info4_Word_Max << (uint8_t)4);
				tmp_Result = CAN_Send_DATA(SMARTID_NAVI_INFO4, 8, tx_Buf);
				if (tmp_Result == true)
				{
					if (navi_Info4_Packet_Cnt >= navi_Info4_Packet_Max)
					{
						navi_Info4_Flag = false;
					}
					else
					{
						/* nothing */
					}
				}
				else
				{
					if (navi_Info4_Packet_Cnt > 0)
					{
						navi_Info4_Packet_Cnt--;
					}
					else
					{
						/* nothing */
					}
				}
			}
			else
			{
				navi_Info4_Flag = false;
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

static void CAN_Create_H592(void)
{
	bool tmp_Result = false;

	if (navi_Info1_Flag == false)
	{
		tmp_Result = BLEP_Get_Navi_Info1(navi_Info1_Word_Data);

		if (tmp_Result == true)
		{
			navi_Info1_Flag = true;
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

static void CAN_Send_H592(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&navi_Info1_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (navi_Info1_Flag == true)
		{
			navi_Info1_Flag = false;

			(void)CAN_Send_DATA(SMARTID_NAVI_INFO1, (uint8_t)8, navi_Info1_Word_Data);
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

static void CAN_Create_H593(void)
{
	bool tmp_Result = false;

	if (navi_Info2_Flag == false)
	{
		tmp_Result = BLEP_Get_Navi_Info2(navi_Info2_Word_Data);

		if (tmp_Result == true)
		{
			navi_Info2_Flag = true;
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

static void CAN_Send_H593(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&navi_Info2_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (navi_Info2_Flag == true)
		{
			navi_Info2_Flag = false;

			(void)CAN_Send_DATA(SMARTID_NAVI_INFO2, (uint8_t)8, navi_Info2_Word_Data);
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

static void CAN_Create_H594(void)
{
	bool tmp_Result = false;

	if (navi_Info3_Flag == false)
	{
		tmp_Result = BLEP_Get_Navi_Info3(navi_Info3_Word_Data);

		if (tmp_Result == true)
		{
			navi_Info3_Flag = true;
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

static void CAN_Send_H594(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&navi_Info3_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (navi_Info3_Flag == true)
		{
			navi_Info3_Flag = false;

			navi_Info3_Word_Data[0] = navi_Info3_Word_Data[0] & (uint8_t)0xFC;
			navi_Info3_Word_Data[2] = navi_Info3_Word_Data[2] & (uint8_t)0xF0;
			navi_Info3_Word_Data[3] = navi_Info3_Word_Data[3] & (uint8_t)0xFE;

			navi_Info3_Word_Data[4] = CAN_EMPTY_FILL;
			navi_Info3_Word_Data[5] = CAN_EMPTY_FILL;
			navi_Info3_Word_Data[6] = CAN_EMPTY_FILL;
			navi_Info3_Word_Data[7] = CAN_EMPTY_FILL;

			(void)CAN_Send_DATA(SMARTID_NAVI_INFO3, (uint8_t)8, navi_Info3_Word_Data);
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

static void CAN_Create_H59D(void)
{
	bool tmp_Result = false;

	tmp_Result = BLEP_Get_Setting_1(setting_1_Word_Data);

	if (tmp_Result == true)
	{
		setting_1_Flag = true;
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Send_H59D(void)
{
	bool tmp_Result;
	uint8_t tmp_Data;
	uint8_t *qrcode = 0;

	tmp_Result = SimpleOS_CheckInterval(&setting_1_Time, (uint16_t)1000);
	if (tmp_Result == true)
	{
		setting_1_Flag = false;

		setting_1_Word_Data[3] = CAN_EMPTY_FILL;

		tmp_Result = Eeprom_Get_QRCODE_Save();
		if (tmp_Result == true)
		{
			qrcode = Eeprom_Get_QRcode_Ptr();

			tmp_Data = UTF_ASCII_2_Hex(qrcode[0]);
			setting_1_Word_Data[4] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[1]);
			setting_1_Word_Data[4] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[2]);
			setting_1_Word_Data[5] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[3]);
			setting_1_Word_Data[5] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[4]);
			setting_1_Word_Data[6] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[5]);
			setting_1_Word_Data[6] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[6]);
			setting_1_Word_Data[7] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[7]);
			setting_1_Word_Data[7] |= (tmp_Data << 0);
		}
		else
		{
			setting_1_Word_Data[4] = 0xFF;
			setting_1_Word_Data[5] = 0xFF;
			setting_1_Word_Data[6] = 0xFF;
			setting_1_Word_Data[7] = 0xFF;
		}

		(void)CAN_Send_DATA(SMARTID_SETTING_1, (uint8_t)8, setting_1_Word_Data);
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Create_H59E(void)
{
	bool tmp_Result = false;

	tmp_Result = BLEP_Get_Setting_2(setting_2_Word_Data);

	if (tmp_Result == true)
	{
		setting_2_Flag = true;
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Send_H59E(void)
{
	bool tmp_Result;
	uint8_t tmp_Data;
	uint8_t *qrcode = 0;

	tmp_Result = SimpleOS_CheckInterval(&setting_2_Time, (uint16_t)1000);
	if (tmp_Result == true)
	{
		setting_2_Flag = false;

		tmp_Result = Eeprom_Get_QRCODE_Save();
		if (tmp_Result == true)
		{
			qrcode = Eeprom_Get_QRcode_Ptr();

			tmp_Data = UTF_ASCII_2_Hex(qrcode[8]);
			setting_2_Word_Data[0] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[9]);
			setting_2_Word_Data[0] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[10]);
			setting_2_Word_Data[1] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[11]);
			setting_2_Word_Data[1] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[12]);
			setting_2_Word_Data[2] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[13]);
			setting_2_Word_Data[2] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[14]);
			setting_2_Word_Data[3] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[15]);
			setting_2_Word_Data[3] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[16]);
			setting_2_Word_Data[4] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[17]);
			setting_2_Word_Data[4] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[18]);
			setting_2_Word_Data[5] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[19]);
			setting_2_Word_Data[5] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[20]);
			setting_2_Word_Data[6] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[21]);
			setting_2_Word_Data[6] |= (tmp_Data << 0);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[22]);
			setting_2_Word_Data[7] = (tmp_Data << 4);
			tmp_Data = UTF_ASCII_2_Hex(qrcode[23]);
			setting_2_Word_Data[7] |= (tmp_Data << 0);
		}
		else
		{
			(void)memset(setting_2_Word_Data, 0xFF, 8);
		}

		(void)CAN_Send_DATA(SMARTID_SETTING_2, (uint8_t)8, setting_2_Word_Data);
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Create_H598(void)
{
	bool tmp_Result = false;

	tmp_Result = BLEP_Get_Voice_Ctrl(voice_Ctrl_Word_Data);

	if (tmp_Result == true)
	{
		voice_Ctrl_Flag = true;
	}
	else
	{
		/* nothing */
	}
}

static void CAN_Send_H598(void)
{
	bool tmp_Result;

	tmp_Result = SimpleOS_CheckInterval(&voice_Ctrl_Time, (uint16_t)100);
	if (tmp_Result == true)
	{
		if (voice_Ctrl_Flag == true)
		{
			voice_Ctrl_Flag = false;

			(void)memset(voice_Ctrl_Word_Data, CAN_EMPTY_FILL, 8);
			(void)CAN_Send_DATA(SMARTID_VOICE_CTRL, (uint8_t)8, voice_Ctrl_Word_Data);
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

static void CAN_enterNORMAL(void)
{
	if (can_State != CAN_NORMAL)
	{
		can_State = CAN_NORMAL;
		DIAG_Reset_fifoIndex();
		DIAG_Reset_Status();
		BSP_FDCAN1_Reset_Rx_Queue_0();
		BSP_FDCAN1_Reset_Tx_Queue();
		BSP_FDCAN1_Change_Mode(0);
		CAN_Set_PutVal(true);

		smartid_58A_Time = SimpleOS_GetTick();
		smartid_58B_Time = SimpleOS_GetTick();

		navi_Info1_Time = SimpleOS_GetTick();
		navi_Info2_Time = SimpleOS_GetTick();
		navi_Info3_Time = SimpleOS_GetTick();
		navi_Info4_Time = SimpleOS_GetTick();
		phone_Num_Time = SimpleOS_GetTick();
		phone_Caller_Time = SimpleOS_GetTick();
		sns_Title_Time = SimpleOS_GetTick();
		sns_Text_Time = SimpleOS_GetTick();
		sns_Sender_Time = SimpleOS_GetTick();
		music_Aritst_Time = SimpleOS_GetTick();
		music_Album_Time = SimpleOS_GetTick();
		music_Title_Time = SimpleOS_GetTick();
		setting_1_Time = SimpleOS_GetTick();
		setting_2_Time = SimpleOS_GetTick();
		voice_Ctrl_Time = SimpleOS_GetTick();

		navi_Info4_Packet_Cnt = 0;
		phone_Num_Packet_Cnt = 0;
		phone_Caller_Packet_Cnt = 0;
		sns_Title_Packet_Cnt = 0;
		sns_Text_Packet_Cnt = 0;
		sns_Sender_Packet_Cnt = 0;
		music_Aritst_Packet_Cnt = 0;
		music_Album_Packet_Cnt = 0;
		music_Title_Packet_Cnt = 0;
	}
	else
	{
		/* ignore */
	}
}

static void CAN_enterBUSOFF(void)
{
	if (can_State != CAN_BUSOFF)
	{
		can_State = CAN_BUSOFF;
		DIAG_Reset_fifoIndex();
		DIAG_Reset_Status();
		BSP_FDCAN1_Reset_Rx_Queue_0();
		BSP_FDCAN1_Reset_Tx_Queue();
		BSP_FDCAN1_Change_Mode(1);
		CAN_Set_PutVal(false);
		error_Busoff_Time = SimpleOS_GetTick();
	}
	else
	{
		/* ignore */
	}
}

static void CAN_updateNORMAL(void)
{
	bool tmp_Result;

	error_Busoff = BSP_FDCAN1_Get_BusOff();
	if (error_Busoff == true)
	{
		CAN_statusChange(CAN_BUSOFF);
	}
	else
	{
		tmp_Result = CAN_Get_FIFOElement();
		if (tmp_Result == true)
		{
			CAN_Process_Data(can_Rx_Id, can_Rx_Len, can_Rx_Data);
		}
		else
		{
			/* No New Data */
		}

		CAN_Create_H58A();
		CAN_Create_H58B();
		CAN_Create_H592();
		CAN_Create_H593();
		CAN_Create_H594();
		CAN_Create_H598();
		CAN_Create_Phone_Info();
		CAN_Create_SNS_Info();
		CAN_Create_Music_Info();
		CAN_Create_Road_Info();
		CAN_Create_H59D();
		CAN_Create_H59E();

		CAN_Send_H58A();
		CAN_Send_H58B();
		CAN_Send_H58C();
		CAN_Send_H58D();
		CAN_Send_H58E();
		CAN_Send_H58F();
		CAN_Send_H591();
		CAN_Send_H592();
		CAN_Send_H593();
		CAN_Send_H594();
		CAN_Send_H595();
		CAN_Send_H598();
		CAN_Send_H59A();
		CAN_Send_H59B();
		CAN_Send_H59C();
		CAN_Send_H59D();
		CAN_Send_H59E();

		CAN_Send_Assy_Data();

		BSP_FDCAN1_Tx_Flush();
	}
}

static void CAN_updateBUSOFF(void)
{
	bool tmp_Result;
	tmp_Result = SimpleOS_CheckInterval(&error_Busoff_Time, (uint16_t)1000);
	if (tmp_Result == true)
	{
		CAN_statusChange(CAN_NORMAL);
	}
	else
	{
		/* nothing */
	}
}

/*========================================================================================*/

static ModuleState_TypeDef CANModule_state = MODULESTATE_RESET;

void CAN_init(void)
{
	CANModule_state = MODULESTATE_INITIALIZED;

	/* Write the module initialize code here	*/
	BSP_FDCAN1_Init();
	CAN_Set_PutVal(true);
	DIAG_Reset_fifoIndex();
}

void CAN_open(void)
{
	if (CANModule_state != MODULESTATE_OPENED)
	{
		CANModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		BSP_FDCAN1_Init();
		BSP_FDCAN1_Open();
		CAN_Set_PutVal(true);
		DIAG_Reset_fifoIndex();

		BSP_FDCAN1_Reset_Rx_Queue_0();
		BSP_FDCAN1_Reset_Tx_Queue();

		(void)memset(canid_meter_system_mode_data, 0xFE, CANID_METER_SYSTEM_MODE_LEN);
		(void)memset(canid_grip_w_button_status_data, 0xFE, CANID_GRIP_W_BUTTON_STATUS_LEN);
		(void)memset(canid_meter_setting_info_data, 0xFE, CANID_METER_SETTING_INFO_LEN);
		(void)memset(canid_function_sw_status_data, 0xFE, CANID_FUNCTION_SW_STATUS_LEN);
		(void)memset(canid_ecu_indicate_status_data, 0xFE, CANID_ECU_INDICATE_STATUS_LEN);
		(void)memset(canid_vh_eg_speed_data, 0xFE, CANID_VH_EG_SPEED_LEN);
		(void)memset(canid_ecu_system_mode_data, 0xFE, CANID_ECU_SYSTEM_MODE_LEN);
		(void)memset(canid_aps_etv_data, 0xFE, CANID_APS_ETV_LEN);
		(void)memset(canid_diag_coadj_egmode_data, 0xFE, CANID_DIAG_COADJ_EGMODE_LEN);
		(void)memset(canid_eg_status_data, 0xFE, CANID_EG_STATUS_LEN);
		(void)memset(canid_ecu_gear_pos_data, 0xFE, CANID_ECU_GEAR_POS_LEN);
		(void)memset(canid_ecu_status_for_tcu_data, 0xFE, CANID_ECU_STATUS_FOR_TCU_LEN);
		(void)memset(canid_ecvt_status_for_tcu_data, 0xFE, CANID_ECVT_STATUS_FOR_TCU_LEN);
		(void)memset(canid_abs_pressure_data, 0xFE, CANID_ABS_PRESSURE_LEN);
		(void)memset(canid_abs_status_data, 0xFE, CANID_ABS_STATUS_LEN);
		(void)memset(canid_smart_status_data, 0xFE, CANID_SMART_STATUS_LEN);
		(void)memset(canid_tlcu_com_control_data_data, 0xFE, CANID_TLCU_COM_CONTROL_DATA_LEN);
		(void)memset(canid_tlcu_control_signal_data, 0xFE, CANID_TLCU_CONTROL_SIGNAL_LEN);
		(void)memset(canid_tcu_status_for_ecu_data, 0xFE, CANID_TCU_STATUS_FOR_ECU_LEN);
		(void)memset(canid_tbd_data, 0xFE, CANID_TBD_LEN);
		(void)memset(canid_meter_system_mode2_data, 0xFE, CANID_METER_SYSTEM_MODE2_LEN);
		(void)memset(canid_diag_coadj_egmode_status_data, 0xFE, CANID_DIAG_COADJ_EGMODE_STATUS_LEN);
		(void)memset(canid_ecu_setting_info_data, 0xFE, CANID_ECU_SETTING_INFO_LEN);
		(void)memset(canid_ecu_data_for_yccs_2_data, 0xFE, CANID_ECU_DATA_FOR_YCCS_2_LEN);

		(void)memset(canid_meter_setting_info2_data, 0xFE, CANID_METER_SETTING_INFO2_LEN);
		(void)memset(canid_ecu_indicate_status2_data, 0xFE, CANID_ECU_INDICATE_STATUS2_LEN);
		(void)memset(canid_vh_eg_speed2_data, 0xFE, CANID_VH_EG_SPEED2_LEN);
		(void)memset(canid_ecu_system_mode2_data, 0xFE, CANID_ECU_SYSTEM_MODE2_LEN);
		(void)memset(canid_ecu_data_for_yccs_data, 0xFE, CANID_ECU_DATA_FOR_YCCS_LEN);
		(void)memset(canid_eg_status2_data, 0xFE, CANID_EG_STATUS2_LEN);

		(void)memset(smartid_58A_Data, 0x00, 8);
		(void)memset(smartid_58B_Data, 0x00, 8);
		(void)memset(setting_1_Word_Data, 0x00, 8);
		(void)memset(setting_2_Word_Data, 0x55, 8);

		can_State = CAN_NORMAL;
		can_SetState = CAN_NORMAL;

		injection_Val = Eeprom_Get_Injection_Val();
		smartid_58A_counter = 3;
		smartid_58A_Time = SimpleOS_GetTick();
		smartid_58A_Flag = false;

		smartid_58B_Time = SimpleOS_GetTick();
		smartid_58B_Flag = false;

		navi_Info1_Time = SimpleOS_GetTick();
		navi_Info1_Flag = false;

		navi_Info2_Time = SimpleOS_GetTick();
		navi_Info2_Flag = false;

		navi_Info3_Time = SimpleOS_GetTick();
		navi_Info3_Flag = false;

		navi_Info4_Time = SimpleOS_GetTick();
		navi_Info4_Word_Max = 0;
		navi_Info4_Packet_Max = 0;
		navi_Info4_Packet_Cnt = 0;
		navi_Info4_Seq_Number = 0;
		navi_Info4_Flag = false;

		phone_Num_Time = SimpleOS_GetTick();
		phone_Num_Word_Max = 0;
		phone_Num_Packet_Max = 0;
		phone_Num_Packet_Cnt = 0;
		phone_Num_Flag = false;

		phone_Caller_Time = SimpleOS_GetTick();
		phone_Caller_Word_Max = 0;
		phone_Caller_Packet_Max = 0;
		phone_Caller_Packet_Cnt = 0;
		phone_Caller_Flag = false;

		sns_Title_Time = SimpleOS_GetTick();
		sns_Title_Word_Max = 0;
		sns_Title_Packet_Max = 0;
		sns_Title_Packet_Cnt = 0;
		sns_Title_Seq_Number = 0;
		sns_Title_Flag = false;

		sns_Text_Time = SimpleOS_GetTick();
		sns_Text_Word_Max = 0;
		sns_Text_Packet_Max = 0;
		sns_Text_Packet_Cnt = 0;
		sns_Text_Seq_Number = 0;
		sns_Text_Word_Hash = 0;
		sns_Text_Flag = false;

		sns_Sender_Time = SimpleOS_GetTick();
		sns_Sender_Word_Max = 0;
		sns_Sender_Packet_Max = 0;
		sns_Sender_Packet_Cnt = 0;
		sns_Sender_Seq_Number = 0;
		sns_Sender_Word_Hash = 0;
		sns_Sender_Flag = false;

		music_Aritst_Time = SimpleOS_GetTick();
		music_Aritst_Word_Max = 0;
		music_Aritst_Packet_Max = 0;
		music_Aritst_Packet_Cnt = 0;
		music_Aritst_Word_Hash = 0;
		music_Aritst_Flag = false;

		music_Album_Time = SimpleOS_GetTick();
		music_Album_Word_Max = 0;
		music_Album_Packet_Max = 0;
		music_Album_Packet_Cnt = 0;
		music_Album_Word_Hash = 0;
		music_Album_Flag = false;

		music_Title_Time = SimpleOS_GetTick();
		music_Title_Word_Max = 0;
		music_Title_Packet_Max = 0;
		music_Title_Packet_Cnt = 0;
		music_Title_Word_Hash = 0;
		music_Title_Flag = false;

		setting_1_Time = SimpleOS_GetTick();
		setting_1_Flag = false;

		setting_2_Time = SimpleOS_GetTick();
		setting_2_Flag = false;

		voice_Ctrl_Time = SimpleOS_GetTick();
		voice_Ctrl_Flag = false;

		sleep_Trigger = false;
		sleep_Flag = false;
		sleep_Time = (uint16_t)CAN_SLEEP_TIME;

		assy_QRCode_Trigger = false;
		assy_Firmware_Trigger = false;
		assy_Send_Flag = false;
		assy_Sn = 0;
		assy_Len = 0;
		assy_Ptr = 0;
		assy_Time = SimpleOS_GetTick();

		canid_g2_id = CANID_GRIP_W_BUTTON_STATUS;
		canid_g5_id = CANID_ECU_INDICATE_STATUS;
		canid_g6_id = CANID_VH_EG_SPEED;
		canid_g7_id = CANID_ECU_SYSTEM_MODE;
		canid_g8_id = CANID_APS_ETV;
		canid_g10_id = CANID_EG_STATUS;

		canid_g2_data = canid_grip_w_button_status_data;
		canid_g5_data = canid_ecu_indicate_status_data;
		canid_g6_data = canid_vh_eg_speed_data;
		canid_g7_data = canid_ecu_system_mode_data;
		canid_g8_data = canid_aps_etv_data;
		canid_g10_data = canid_eg_status_data;
	}
	else
	{
		/* nothing */
	}
}

void CAN_close(void)
{
	if (CANModule_state != MODULESTATE_CLOSED)
	{
		CANModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
		CAN_Set_PutVal(false);
		whiteList_Flag = false;

		BSP_FDCAN1_Close();
	}
	else
	{
		/* nothing */
	}
}

void CAN_flush(void)
{
	if (CANModule_state == MODULESTATE_OPENED)
	{
		tmp_Sleep_Trigger = sleep_Trigger;
		if (tmp_Sleep_Trigger == true)
		{
			tmp_Sleep_Trigger = false;
			sleep_Trigger = tmp_Sleep_Trigger;
			sleep_Flag = true;
			sleep_Time = (uint16_t)CAN_SLEEP_TIME;
		}
		else
		{
			/* nothing */
		}

		if (can_State != can_SetState)
		{
			switch (can_SetState)
			{
			case CAN_NORMAL:
				CAN_enterNORMAL();
				break;
			case CAN_BUSOFF:
				CAN_enterBUSOFF();
				break;
			}
			can_State = can_SetState;
		}
		else
		{
			switch (can_State)
			{
			case CAN_NORMAL:
				CAN_updateNORMAL();
				break;
			case CAN_BUSOFF:
				CAN_updateBUSOFF();
				break;
			}
		}
	}
	else
	{
		/* nothing */
	}
}

/*-------- Data APIs------------------------------*/
bool CAN_Send_DATA(const uint32_t id, const uint8_t length, uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_data = data;
	uint32_t tmp_id = id;
	uint8_t tmp_len = length;
	if (can_State == CAN_NORMAL)
	{
		tmp_Result = BSP_FDCAN1_Send_Data(&tmp_id, &tmp_len, tmp_data, false);
	}
	else
	{
		tmp_Result = false;
	}
	return tmp_Result;
}

uint32_t CAN_Get_Injection_Val(void)
{
	uint32_t tmp = injection_Val;
	injection_Val = 0;

	return tmp;
}

uint8_t *CAN_Get_canid_meter_system_mode_data_Ptr(void)
{
	return canid_meter_system_mode_data;
}
uint8_t *CAN_Get_canid_g2_data_Ptr(void)
{
	return canid_g2_data;
}
uint8_t *CAN_Get_canid_meter_setting_info_data_Ptr(void)
{
	return canid_meter_setting_info_data;
}
uint8_t *CAN_Get_canid_function_sw_status_data_Ptr(void)
{
	return canid_function_sw_status_data;
}
uint8_t *CAN_Get_canid_g5_data_Ptr(void)
{
	return canid_g5_data;
}
uint8_t *CAN_Get_canid_g6_data_Ptr(void)
{
	return canid_g6_data;
}
uint8_t *CAN_Get_canid_g7_data_Ptr(void)
{
	return canid_g7_data;
}
uint8_t *CAN_Get_canid_g8_data_Ptr(void)
{
	return canid_g8_data;
}
uint8_t *CAN_Get_canid_diag_coadj_egmode_data_Ptr(void)
{
	return canid_diag_coadj_egmode_data;
}
uint8_t *CAN_Get_canid_g10_data_Ptr(void)
{
	return canid_g10_data;
}
uint8_t *CAN_Get_canid_ecu_gear_pos_data_Ptr(void)
{
	return canid_ecu_gear_pos_data;
}
uint8_t *CAN_Get_canid_ecu_status_for_tcu_data_Ptr(void)
{
	return canid_ecu_status_for_tcu_data;
}
uint8_t *CAN_Get_canid_ecvt_status_for_tcu_data_Ptr(void)
{
	return canid_ecvt_status_for_tcu_data;
}
uint8_t *CAN_Get_canid_abs_pressure_data_Ptr(void)
{
	return canid_abs_pressure_data;
}
uint8_t *CAN_Get_canid_abs_status_data_Ptr(void)
{
	return canid_abs_status_data;
}
uint8_t *CAN_Get_canid_smart_status_data_Ptr(void)
{
	return canid_smart_status_data;
}
uint8_t *CAN_Get_canid_tlcu_com_control_data_data_Ptr(void)
{
	return canid_tlcu_com_control_data_data;
}
uint8_t *CAN_Get_canid_tlcu_control_signal_data_Ptr(void)
{
	return canid_tlcu_control_signal_data;
}
uint8_t *CAN_Get_canid_tcu_status_for_ecu_data_Ptr(void)
{
	return canid_tcu_status_for_ecu_data;
}
uint8_t *CAN_Get_canid_tbd_data_Ptr(void)
{
	return canid_tbd_data;
}
uint8_t *CAN_Get_canid_meter_system_mode2_data_Ptr(void)
{
	return canid_meter_system_mode2_data;
}
uint8_t *CAN_Get_canid_diag_coadj_egmode_status_data_Ptr(void)
{
	return canid_diag_coadj_egmode_status_data;
}
uint8_t *CAN_Get_canid_ecu_setting_info_data_Ptr(void)
{
	return canid_ecu_setting_info_data;
}
uint8_t *CAN_Get_canid_ecu_data_for_yccs_2_data_Ptr(void)
{
	return canid_ecu_data_for_yccs_2_data;
}
uint16_t CAN_Get_canid_g2_ID(void)
{
	return canid_g2_id;
}
uint16_t CAN_Get_canid_g5_ID(void)
{
	return canid_g5_id;
}
uint16_t CAN_Get_canid_g6_ID(void)
{
	return canid_g6_id;
}
uint16_t CAN_Get_canid_g7_ID(void)
{
	return canid_g7_id;
}
uint16_t CAN_Get_canid_g8_ID(void)
{
	return canid_g8_id;
}
uint16_t CAN_Get_canid_g10_ID(void)
{
	return canid_g10_id;
}

void CAN_Send_test(uint32_t data)
{
	uint8_t tmp[8];
	(void)memset(tmp, 0, 8);
	tmp[0] = (uint8_t)(data >> 24);
	tmp[1] = (uint8_t)(data >> 16);
	tmp[2] = (uint8_t)(data >> 8);
	tmp[3] = (uint8_t)(data >> 0);
	(void)CAN_Send_DATA(0x666, 8, tmp);
}

void CAN_Reset_SleepTime(void)
{
	sleep_Time = (uint16_t)CAN_SLEEP_TIME;
}

bool CAN_Get_SleepFlag(void)
{
	return sleep_Flag;
}

bool CAN_Get_WhiteListFlag(void)
{
	return whiteList_Flag;
}
