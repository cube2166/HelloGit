#ifndef CAN_Module_H_
#define CAN_Module_H_

#include "../../Modules.h"

#define FLASH_APP_NAME                  "BWP0"
#define BTL_GUARD                       (0x5048434DUL)
#define BTL_TRIGGER_RAM_START           0x20000000
#define PC2MCU_ID_CMD                   (0x07CAUL)
#define PC2MCU_ID_DATA                  (0x07DAUL)
#define MCU2PC_ID_CMD                   (0x07CBUL)

#define PC2MCU_CMD_BOOT                 (0x7E00UL)
#define PC2MCU_CMD_VERSION              (0x7E01UL)
#define PC2MCU_CMD_EDIT                 (0x7E02UL)
#define PC2MCU_CMD_NAME                 (0x7EFEUL)
#define PC2MCU_SUBCMD_EDIT_ODO          (0x00U)
#define PC2MCU_SUBCMD_EDIT_TRIP1        (0x01U)
#define PC2MCU_SUBCMD_EDIT_TRIP2        (0x02U)
#define PC2MCU_SUBCMD_EDIT_OIL          (0x03U)
/* 
SGCU→SCCU (V1.0)
#define CANID_APS_ETV           0X216
#define CANID_APS_ETV_LEN       4
#define CANID_EG_SPEED          0x20A
#define CANID_EG_SPEED_LEN      2
#define CANID_DIAG_STATUS       0x23A
#define CANID_DIAG_STATUS_LEN   2
#define CANID_EG_STATUS         0x23E
#define CANID_EG_STATUS_LEN     2
#define CANID_ECU_GEAR_POS      0x245
#define CANID_ECU_GEAR_POS_LEN  1
 */

/* SGCU→SCCU (V2.0) */
#define CANID_METER_SYSTEM_MODE                 0x226
#define CANID_METER_SYSTEM_MODE_LEN             1
#define CANID_GRIP_W_BUTTON_STATUS              0x22E
#define CANID_GRIP_W_BUTTON_STATUS_LEN          1
#define CANID_METER_SETTING_INFO                0x590
#define CANID_METER_SETTING_INFO_LEN            1
#define CANID_FUNCTION_SW_STATUS                0x2A3
#define CANID_FUNCTION_SW_STATUS_LEN            3
#define CANID_ECU_INDICATE_STATUS               0x209
#define CANID_ECU_INDICATE_STATUS_LEN           3
#define CANID_VH_EG_SPEED                       0x20A
#define CANID_VH_EG_SPEED_LEN                   1
#define CANID_ECU_SYSTEM_MODE                   0x215
#define CANID_ECU_SYSTEM_MODE_LEN               3
#define CANID_APS_ETV                           0x216
#define CANID_APS_ETV_LEN                       3
#define CANID_DIAG_COADJ_EGMODE                 0x23A
#define CANID_DIAG_COADJ_EGMODE_LEN             1
#define CANID_EG_STATUS                         0x23E
#define CANID_EG_STATUS_LEN                     2
#define CANID_ECU_GEAR_POS                      0x245
#define CANID_ECU_GEAR_POS_LEN                  1
#define CANID_ECU_STATUS_FOR_TCU                0x2AA
#define CANID_ECU_STATUS_FOR_TCU_LEN            7
#define CANID_ECVT_STATUS_FOR_TCU               0x2AB
#define CANID_ECVT_STATUS_FOR_TCU_LEN           3
#define CANID_ABS_PRESSURE                      0x260
#define CANID_ABS_PRESSURE_LEN                  8
#define CANID_ABS_STATUS                        0x268
#define CANID_ABS_STATUS_LEN                    4
#define CANID_SMART_STATUS                      0x555
#define CANID_SMART_STATUS_LEN                  1
#define CANID_TLCU_COM_CONTROL_DATA             0x404
#define CANID_TLCU_COM_CONTROL_DATA_LEN         1
#define CANID_TLCU_CONTROL_SIGNAL               0x2B1
#define CANID_TLCU_CONTROL_SIGNAL_LEN           2
#define CANID_TCU_STATUS_FOR_ECU                0x2BA
#define CANID_TCU_STATUS_FOR_ECU_LEN            5
#define CANID_TBD                               0x3BA
#define CANID_TBD_LEN                           8
#define CANID_METER_SYSTEM_MODE2                0x27A
#define CANID_METER_SYSTEM_MODE2_LEN            2
#define CANID_DIAG_COADJ_EGMODE_STATUS          0x383
#define CANID_DIAG_COADJ_EGMODE_STATUS_LEN      2
#define CANID_ECU_SETTING_INFO                  0x2BC
#define CANID_ECU_SETTING_INFO_LEN              3
#define CANID_ECU_DATA_FOR_YCCS_2               0x204
#define CANID_ECU_DATA_FOR_YCCS_2_LEN           1

#define CANID_METER_SETTING_INFO2               0x382
#define CANID_METER_SETTING_INFO2_LEN           1
#define CANID_ECU_INDICATE_STATUS2              0x27C
#define CANID_ECU_INDICATE_STATUS2_LEN          3
#define CANID_VH_EG_SPEED2                      0x278
#define CANID_VH_EG_SPEED2_LEN                  1
#define CANID_ECU_SYSTEM_MODE2                  0x279
#define CANID_ECU_SYSTEM_MODE2_LEN              3
#define CANID_ECU_DATA_FOR_YCCS                 0x27F
#define CANID_ECU_DATA_FOR_YCCS_LEN             3
#define CANID_EG_STATUS2                        0x512
#define CANID_EG_STATUS2_LEN                    2


/* SCCU→SGCU (V1.0)*/
#define SMARTID_58A             0x58A
#define SMARTID_58A_LEN         8
#define SMARTID_58B             0x58B
#define SMARTID_58B_LEN         8
#define SMARTID_58C             0x58C
#define SMARTID_58C_LEN         8
#define SMARTID_58D             0x58D
#define SMARTID_58D_LEN         8
#define SMARTID_58E             0x58E
#define SMARTID_58E_LEN         8
#define SMARTID_58F             0x58F
#define SMARTID_58F_LEN         8

/* SCCU→SGCU (V2.0)*/
#define SMARTID_NAVI_INFO1          BLE2SCCU_NAVI_INFO1_ID
#define SMARTID_NAVI_INFO2          BLE2SCCU_NAVI_INFO2_ID
#define SMARTID_NAVI_INFO3          BLE2SCCU_NAVI_INFO3_ID
#define SMARTID_NAVI_INFO4          BLE2SCCU_NAVI_INFO4_ID
#define SMARTID_NAVI_INFO4_LEN      BLE2SCCU_NAVI_INFO4_LEN
#define SMARTID_NAVI_INFO5          BLE2SCCU_NAVI_INFO5_ID
#define SMARTID_NAVI_INFO5_LEN      BLE2SCCU_NAVI_INFO5_LEN
#define SMARTID_VOICE_CTRL          BLE2SCCU_VOICE_CTRL_ID
#define SMARTID_VOICE_CTRL_LEN      BLE2SCCU_VOICE_CTRL_LEN

#define SMARTID_PHONE_NUMBER        BLE2SCCU_PHONE_NUMBER_ID
#define SMARTID_PHONE_NUMBER_LEN    BLE2SCCU_PHONE_NUMBER_LEN
#define SMARTID_PHONE_CALLER        BLE2SCCU_PHONE_CALLER_ID
#define SMARTID_PHONE_CALLER_LEN    BLE2SCCU_PHONE_CALLER_LEN
#define SMARTID_SNS_TITLE           BLE2SCCU_SNS_TITLE_ID
#define SMARTID_SNS_TITLE_LEN       BLE2SCCU_SNS_TITLE_LEN
#define SMARTID_SNS_TEXT            BLE2SCCU_SNS_TEXT_ID
#define SMARTID_SNS_TEXT_LEN        BLE2SCCU_SNS_TEXT_LEN
#define SMARTID_SNS_SENDER          BLE2SCCU_SNS_SENDER_ID
#define SMARTID_SNS_SENDER_LEN      BLE2SCCU_SNS_SENDER_LEN

#define SMARTID_MUSIC_ARITST        BLE2SCCU_MUSIC_ARITST_ID
#define SMARTID_MUSIC_ARITST_LEN    BLE2SCCU_MUSIC_ARITST_LEN
#define SMARTID_MUSIC_ALBUM         BLE2SCCU_MUSIC_ALBUM_ID
#define SMARTID_MUSIC_ALBUM_LEN     BLE2SCCU_MUSIC_ALBUM_LEN
#define SMARTID_MUSIC_TITLE         BLE2SCCU_MUSIC_TITLE_ID
#define SMARTID_MUSIC_TITLE_LEN     BLE2SCCU_MUSIC_TITLE_LEN
#define SMARTID_SETTING_1           BLE2SCCU_SETTING_1_ID
#define SMARTID_SETTING_1_LEN       BLE2SCCU_SETTING_1_LEN
#define SMARTID_SETTING_2           BLE2SCCU_SETTING_2_ID
#define SMARTID_SETTING_2_LEN       BLE2SCCU_SETTING_2_LEN

/* PC→SGCU */
#define P2SID                  0x6FF

/* SGCU->PC */
#define C2PID                  0x6FE


void CAN_ISR(void);


bool CAN_Send_DATA(const uint32_t id, const uint8_t length, uint8_t *data);
void CAN_Send_test(uint32_t data);

uint32_t CAN_Get_Injection_Val(void);

uint8_t *CAN_Get_canid_meter_system_mode_data_Ptr(void);
uint8_t *CAN_Get_canid_g2_data_Ptr(void);
uint8_t *CAN_Get_canid_meter_setting_info_data_Ptr(void);
uint8_t *CAN_Get_canid_function_sw_status_data_Ptr(void);
uint8_t *CAN_Get_canid_g5_data_Ptr(void);
uint8_t *CAN_Get_canid_g6_data_Ptr(void);
uint8_t *CAN_Get_canid_g7_data_Ptr(void);
uint8_t *CAN_Get_canid_g8_data_Ptr(void);
uint8_t *CAN_Get_canid_diag_coadj_egmode_data_Ptr(void);
uint8_t *CAN_Get_canid_g10_data_Ptr(void);
uint8_t *CAN_Get_canid_ecu_gear_pos_data_Ptr(void);
uint8_t *CAN_Get_canid_ecu_status_for_tcu_data_Ptr(void);
uint8_t *CAN_Get_canid_ecvt_status_for_tcu_data_Ptr(void);
uint8_t *CAN_Get_canid_abs_pressure_data_Ptr(void);
uint8_t *CAN_Get_canid_abs_status_data_Ptr(void);
uint8_t *CAN_Get_canid_smart_status_data_Ptr(void);
uint8_t *CAN_Get_canid_tlcu_com_control_data_data_Ptr(void);
uint8_t *CAN_Get_canid_tlcu_control_signal_data_Ptr(void);
uint8_t *CAN_Get_canid_tcu_status_for_ecu_data_Ptr(void);
uint8_t *CAN_Get_canid_tbd_data_Ptr(void);
uint8_t *CAN_Get_canid_meter_system_mode2_data_Ptr(void);
uint8_t *CAN_Get_canid_diag_coadj_egmode_status_data_Ptr(void);
uint8_t *CAN_Get_canid_ecu_setting_info_data_Ptr(void);
uint8_t *CAN_Get_canid_ecu_data_for_yccs_2_data_Ptr(void);
uint16_t CAN_Get_canid_g2_ID(void);
uint16_t CAN_Get_canid_g5_ID(void);
uint16_t CAN_Get_canid_g6_ID(void);
uint16_t CAN_Get_canid_g7_ID(void);
uint16_t CAN_Get_canid_g8_ID(void);
uint16_t CAN_Get_canid_g10_ID(void);

bool CAN_Get_SleepFlag(void);
void CAN_Reset_SleepTime(void);
bool CAN_Get_WhiteListFlag(void);

void CAN_init(void);
void CAN_open(void);
void CAN_close(void);
void CAN_flush(void);

#endif //CAN_Module_H_
