#ifndef BLE_Package_H_
#define BLE_Package_H_

#include "../../Modules.h"

/* SCCU→BLE */
#define SCCU2BLE_PROCESS_HEAD           0x5A
#define SCCU2BLE_PROCESS_ID             0x7F01
#define SCCU2BLE_PROCESS_LEN            1
#define SCCU2BLE_PROCESS_PTR            BLE_Get_process_data_Ptr()

#define SCCU2BLE_LOCALNAME_HEAD         0x5A
#define SCCU2BLE_LOCALNAME_ID           0x7F02
#define SCCU2BLE_LOCALNAME_LEN          20
#define SCCU2BLE_LOCALNAME_PTR          BLE_Get_ccuid_adv_data_Ptr()

#define SCCU2BLE_READFW_HEAD            0x5A
#define SCCU2BLE_READFW_ID              0x7F03

#define SCCU2BLE_SHUTDOWN_HEAD          0x5A
#define SCCU2BLE_SHUTDOWN_ID            0x6FFF
/* 
SCCU (V1.0) CANID
#define SCCU2BLE_CANID_HEAD             0x56
#define SCCU2BLE_CANID_G1_ID            CANID_EG_SPEED
#define SCCU2BLE_CANID_G1_LEN           CANID_EG_SPEED_LEN
#define SCCU2BLE_CANID_G1_PTR           CAN_Get_canid_eg_speed_data_Ptr()
#define SCCU2BLE_CANID_G2_ID            CANID_APS_ETV
#define SCCU2BLE_CANID_G2_LEN           CANID_APS_ETV_LEN
#define SCCU2BLE_CANID_G2_PTR           CAN_Get_canid_aps_evt_data_Ptr()
#define SCCU2BLE_CANID_G3_ID            CANID_DIAG_STATUS
#define SCCU2BLE_CANID_G3_LEN           CANID_DIAG_STATUS_LEN
#define SCCU2BLE_CANID_G3_PTR           CAN_Get_canid_diag_status_data_Ptr()
#define SCCU2BLE_CANID_G4_ID            CANID_EG_STATUS
#define SCCU2BLE_CANID_G4_LEN           CANID_EG_STATUS_LEN
#define SCCU2BLE_CANID_G4_PTR           CAN_Get_canid_g10_data_Ptr()
#define SCCU2BLE_CANID_G5_ID            CANID_ECU_GEAR_POS
#define SCCU2BLE_CANID_G5_LEN           CANID_ECU_GEAR_POS_LEN
#define SCCU2BLE_CANID_G5_PTR           CAN_Get_canid_ecu_gear_pos_data_Ptr()
 */

#define SCCU2BLE_CANID_HEAD             0x6C
#define SCCU2BLE_CANID_G1_ID            CANID_METER_SYSTEM_MODE
#define SCCU2BLE_CANID_G1_LEN           CANID_METER_SYSTEM_MODE_LEN
#define SCCU2BLE_CANID_G1_PTR           CAN_Get_canid_meter_system_mode_data_Ptr()
#define SCCU2BLE_CANID_G2_ID            CAN_Get_canid_g2_ID()
#define SCCU2BLE_CANID_G2_LEN           CANID_GRIP_W_BUTTON_STATUS_LEN
#define SCCU2BLE_CANID_G2_PTR           CAN_Get_canid_g2_data_Ptr()
#define SCCU2BLE_CANID_G3_ID            CANID_FUNCTION_SW_STATUS
#define SCCU2BLE_CANID_G3_LEN           CANID_FUNCTION_SW_STATUS_LEN
#define SCCU2BLE_CANID_G3_PTR           CAN_Get_canid_function_sw_status_data_Ptr()
#define SCCU2BLE_CANID_G4_ID            CANID_METER_SETTING_INFO
#define SCCU2BLE_CANID_G4_LEN           CANID_METER_SETTING_INFO_LEN
#define SCCU2BLE_CANID_G4_PTR           CAN_Get_canid_meter_setting_info_data_Ptr()
#define SCCU2BLE_CANID_G5_ID            CAN_Get_canid_g5_ID()
#define SCCU2BLE_CANID_G5_LEN           CANID_ECU_INDICATE_STATUS_LEN
#define SCCU2BLE_CANID_G5_PTR           CAN_Get_canid_g5_data_Ptr()
#define SCCU2BLE_CANID_G6_ID            CAN_Get_canid_g6_ID()
#define SCCU2BLE_CANID_G6_LEN           CANID_VH_EG_SPEED_LEN
#define SCCU2BLE_CANID_G6_PTR           CAN_Get_canid_g6_data_Ptr()
#define SCCU2BLE_CANID_G7_ID            CAN_Get_canid_g7_ID()
#define SCCU2BLE_CANID_G7_LEN           CANID_ECU_SYSTEM_MODE_LEN
#define SCCU2BLE_CANID_G7_PTR           CAN_Get_canid_g7_data_Ptr()
#define SCCU2BLE_CANID_G8_ID            CAN_Get_canid_g8_ID()
#define SCCU2BLE_CANID_G8_LEN           CANID_APS_ETV_LEN
#define SCCU2BLE_CANID_G8_PTR           CAN_Get_canid_g8_data_Ptr()
#define SCCU2BLE_CANID_G9_ID            CANID_DIAG_COADJ_EGMODE
#define SCCU2BLE_CANID_G9_LEN           CANID_DIAG_COADJ_EGMODE_LEN
#define SCCU2BLE_CANID_G9_PTR           CAN_Get_canid_diag_coadj_egmode_data_Ptr()
#define SCCU2BLE_CANID_G10_ID           CAN_Get_canid_g10_ID()
#define SCCU2BLE_CANID_G10_LEN          CANID_EG_STATUS_LEN
#define SCCU2BLE_CANID_G10_PTR          CAN_Get_canid_g10_data_Ptr()
#define SCCU2BLE_CANID_G11_ID           CANID_ECU_GEAR_POS
#define SCCU2BLE_CANID_G11_LEN          CANID_ECU_GEAR_POS_LEN
#define SCCU2BLE_CANID_G11_PTR          CAN_Get_canid_ecu_gear_pos_data_Ptr()
#define SCCU2BLE_CANID_G12_ID           CANID_ECU_STATUS_FOR_TCU
#define SCCU2BLE_CANID_G12_LEN          CANID_ECU_STATUS_FOR_TCU_LEN
#define SCCU2BLE_CANID_G12_PTR          CAN_Get_canid_ecu_status_for_tcu_data_Ptr()
#define SCCU2BLE_CANID_G13_ID           CANID_ECVT_STATUS_FOR_TCU
#define SCCU2BLE_CANID_G13_LEN          CANID_ECVT_STATUS_FOR_TCU_LEN
#define SCCU2BLE_CANID_G13_PTR          CAN_Get_canid_ecvt_status_for_tcu_data_Ptr()
#define SCCU2BLE_CANID_G14_ID           CANID_ABS_PRESSURE
#define SCCU2BLE_CANID_G14_LEN          CANID_ABS_PRESSURE_LEN
#define SCCU2BLE_CANID_G14_PTR          CAN_Get_canid_abs_pressure_data_Ptr()
#define SCCU2BLE_CANID_G15_ID           CANID_ABS_STATUS
#define SCCU2BLE_CANID_G15_LEN          CANID_ABS_STATUS_LEN
#define SCCU2BLE_CANID_G15_PTR          CAN_Get_canid_abs_status_data_Ptr()
#define SCCU2BLE_CANID_G16_ID           CANID_SMART_STATUS
#define SCCU2BLE_CANID_G16_LEN          CANID_SMART_STATUS_LEN
#define SCCU2BLE_CANID_G16_PTR          CAN_Get_canid_smart_status_data_Ptr()
#define SCCU2BLE_CANID_G17_ID           CANID_TLCU_COM_CONTROL_DATA
#define SCCU2BLE_CANID_G17_LEN          CANID_TLCU_COM_CONTROL_DATA_LEN
#define SCCU2BLE_CANID_G17_PTR          CAN_Get_canid_tlcu_com_control_data_data_Ptr()
#define SCCU2BLE_CANID_G18_ID           CANID_TLCU_CONTROL_SIGNAL
#define SCCU2BLE_CANID_G18_LEN          CANID_TLCU_CONTROL_SIGNAL_LEN
#define SCCU2BLE_CANID_G18_PTR          CAN_Get_canid_tlcu_control_signal_data_Ptr()
#define SCCU2BLE_CANID_G19_ID           CANID_TCU_STATUS_FOR_ECU
#define SCCU2BLE_CANID_G19_LEN          CANID_TCU_STATUS_FOR_ECU_LEN
#define SCCU2BLE_CANID_G19_PTR          CAN_Get_canid_tcu_status_for_ecu_data_Ptr()
#define SCCU2BLE_CANID_G20_ID           CANID_TBD
#define SCCU2BLE_CANID_G20_LEN          CANID_TBD_LEN
#define SCCU2BLE_CANID_G20_PTR          CAN_Get_canid_tbd_data_Ptr()
#define SCCU2BLE_CANID_G21_ID           CANID_METER_SYSTEM_MODE2
#define SCCU2BLE_CANID_G21_LEN          CANID_METER_SYSTEM_MODE2_LEN
#define SCCU2BLE_CANID_G21_PTR          CAN_Get_canid_meter_system_mode2_data_Ptr()
#define SCCU2BLE_CANID_G22_ID           CANID_DIAG_COADJ_EGMODE_STATUS
#define SCCU2BLE_CANID_G22_LEN          CANID_DIAG_COADJ_EGMODE_STATUS_LEN
#define SCCU2BLE_CANID_G22_PTR          CAN_Get_canid_diag_coadj_egmode_status_data_Ptr()
#define SCCU2BLE_CANID_G23_ID           CANID_ECU_SETTING_INFO
#define SCCU2BLE_CANID_G23_LEN          CANID_ECU_SETTING_INFO_LEN
#define SCCU2BLE_CANID_G23_PTR          CAN_Get_canid_ecu_setting_info_data_Ptr()
#define SCCU2BLE_CANID_G24_ID           CANID_ECU_DATA_FOR_YCCS_2
#define SCCU2BLE_CANID_G24_LEN          CANID_ECU_DATA_FOR_YCCS_2_LEN
#define SCCU2BLE_CANID_G24_PTR          CAN_Get_canid_ecu_data_for_yccs_2_data_Ptr()

/* 
SCCU (V1.0) COMMONDID
#define SCCU2BLE_COMMONDID_HEAD         0x5B
#define SCCU2BLE_COMMONDID_G1_ID        CID_VIN
#define SCCU2BLE_COMMONDID_G1_LEN       CID_VIN_LEN
#define SCCU2BLE_COMMONDID_G1_PTR       DIAG_Get_cid_vin_data_Ptr()
#define SCCU2BLE_COMMONDID_G2_ID        CID_ECU_SOFTWARE_NUMBER
#define SCCU2BLE_COMMONDID_G2_LEN       CID_ECU_SOFTWARE_NUMBER_LEN
#define SCCU2BLE_COMMONDID_G2_PTR       DIAG_Get_cid_ecu_software_number_data_Ptr()
#define SCCU2BLE_COMMONDID_G3_ID        CID_REGISTERED_MODEL
#define SCCU2BLE_COMMONDID_G3_LEN       CID_REGISTERED_MODEL_LEN
#define SCCU2BLE_COMMONDID_G3_PTR       DIAG_Get_cid_registered_model_data_Ptr()
#define SCCU2BLE_COMMONDID_G4_ID        CID_ECU_ENERGIZATION_TOTAL_TIME
#define SCCU2BLE_COMMONDID_G4_LEN       CID_ECU_ENERGIZATION_TOTAL_TIME_LEN
#define SCCU2BLE_COMMONDID_G4_PTR       DIAG_Get_cid_ecu_energization_total_time_data_Ptr()
#define SCCU2BLE_COMMONDID_G5_ID        CID_ECU_COUNT
#define SCCU2BLE_COMMONDID_G5_LEN       CID_ECU_COUNT_LEN
#define SCCU2BLE_COMMONDID_G5_PTR       DIAG_Get_cid_ecu_count_data_Ptr()
#define SCCU2BLE_COMMONDID_G6_ID        CID_TRAVEL_DISTANCE
#define SCCU2BLE_COMMONDID_G6_LEN       CID_TRAVEL_DISTANCE_LEN
#define SCCU2BLE_COMMONDID_G6_PTR       DIAG_Get_cid_travel_distance_data_Ptr()
#define SCCU2BLE_COMMONDID_G7_ID        CID_EG_OPERATION_TIME
#define SCCU2BLE_COMMONDID_G7_LEN       CID_EG_OPERATION_TIME_LEN
#define SCCU2BLE_COMMONDID_G7_PTR       DIAG_Get_cid_eg_operation_time_data_Ptr()
#define SCCU2BLE_COMMONDID_G8_ID        0xF701
#define SCCU2BLE_COMMONDID_G8_LEN       4
 */

#define SCCU2BLE_COMMONDID_HEAD         0x6A
#define SCCU2BLE_COMMONDID_G1_ID        CID_VIN
#define SCCU2BLE_COMMONDID_G1_LEN       CID_VIN_LEN
#define SCCU2BLE_COMMONDID_G1_PTR       DIAG_Get_cid_vin_data_Ptr()
#define SCCU2BLE_COMMONDID_G2_ID        CID_ECU_SOFTWARE_NUMBER
#define SCCU2BLE_COMMONDID_G2_LEN       CID_ECU_SOFTWARE_NUMBER_LEN
#define SCCU2BLE_COMMONDID_G2_PTR       DIAG_Get_cid_ecu_software_number_data_Ptr()
#define SCCU2BLE_COMMONDID_G3_ID        CID_REGISTERED_MODEL
#define SCCU2BLE_COMMONDID_G3_LEN       CID_REGISTERED_MODEL_LEN
#define SCCU2BLE_COMMONDID_G3_PTR       DIAG_Get_cid_registered_model_data_Ptr()
#define SCCU2BLE_COMMONDID_G4_ID        CID_ECU_ENERGIZATION_TOTAL_TIME
#define SCCU2BLE_COMMONDID_G4_LEN       CID_ECU_ENERGIZATION_TOTAL_TIME_LEN
#define SCCU2BLE_COMMONDID_G4_PTR       DIAG_Get_cid_ecu_energization_total_time_data_Ptr()
#define SCCU2BLE_COMMONDID_G5_ID        CID_ECU_COUNT
#define SCCU2BLE_COMMONDID_G5_LEN       CID_ECU_COUNT_LEN
#define SCCU2BLE_COMMONDID_G5_PTR       DIAG_Get_cid_ecu_count_data_Ptr()
#define SCCU2BLE_COMMONDID_G6_ID        CID_TRAVEL_DISTANCE
#define SCCU2BLE_COMMONDID_G6_LEN       CID_TRAVEL_DISTANCE_LEN
#define SCCU2BLE_COMMONDID_G6_PTR       DIAG_Get_cid_travel_distance_data_Ptr()
#define SCCU2BLE_COMMONDID_G7_ID        CID_EG_OPERATION_TIME
#define SCCU2BLE_COMMONDID_G7_LEN       CID_EG_OPERATION_TIME_LEN
#define SCCU2BLE_COMMONDID_G7_PTR       DIAG_Get_cid_eg_operation_time_data_Ptr()
#define SCCU2BLE_COMMONDID_G8_ID        0xF701
#define SCCU2BLE_COMMONDID_G8_LEN       4
#define SCCU2BLE_COMMONDID_G9_ID        0xF702  /* SCCU (V2.0) */
#define SCCU2BLE_COMMONDID_G9_LEN       4       /* SCCU (V2.0) */

#define SCCU2BLE_H0148_HEAD             0x6B
#define SCCU2BLE_H0148_01_ID            LID_H01
#define SCCU2BLE_H0148_01_LEN           LID_H01_LEN
#define SCCU2BLE_H0148_01_PTR           DIAG_Get_lid_h01_data_Ptr()
#define SCCU2BLE_H0148_48_ID            LID_H48
#define SCCU2BLE_H0148_48_LEN           LID_H48_LEN
#define SCCU2BLE_H0148_48_PTR           DIAG_Get_lid_h48_data_Ptr()
#define SCCU2BLE_H0148_Op1_ID           0x011D
#define SCCU2BLE_H0148_Op1_LEN          1
#define SCCU2BLE_H0148_Op1_PTR          BTN_Get_vol_Ctrl_Ptr()
#define SCCU2BLE_H0148_Op2_ID           0x0117
#define SCCU2BLE_H0148_Op2_LEN          1
#define SCCU2BLE_H0148_Op2_PTR          BTN_Get_music_Ctrl_Ptr()
#define SCCU2BLE_H0148_Op3_ID           0x0119
#define SCCU2BLE_H0148_Op3_LEN          1
#define SCCU2BLE_H0148_Op3_PTR          BTN_Get_tel_Ctrl_Ptr()

#define SCCU2BLE_FFD_HEAD               0x59
#define SCCU2BLE_FFD_LEN                70

/* BLE→SCCU (V1.0)*/
#define BLE2SCCU_AUTHENTICATION_HEAD    0xAA
#define BLE2SCCU_AUTHENTICATION_ID      0x7F00
#define BLE2SCCU_AUTHENTICATION_LEN     53
#define BLE2SCCU_AUTH_LOCALNAME_LEN     14
#define BLE2SCCU_AUTH_LOCALNAME_INDEX   0
#define BLE2SCCU_AUTH_PASSKEY_LEN       6
#define BLE2SCCU_AUTH_PASSKEY_INDEX     (BLE2SCCU_AUTH_LOCALNAME_INDEX + 1)
#define BLE2SCCU_AUTH_UUID_LEN          32
#define BLE2SCCU_AUTH_UUID_INDEX        (BLE2SCCU_AUTH_PASSKEY_INDEX + 1)
#define BLE2SCCU_AUTH_BONDFLAG_LEN      1
#define BLE2SCCU_AUTH_BONDFLAG_INDEX    (BLE2SCCU_AUTH_UUID_INDEX + 1)
#define BLE2SCCU_AUTH_COUNT             (BLE2SCCU_AUTH_BONDFLAG_INDEX + 1)

#define BLE2SCCU_READFW_HEAD            0xAA
#define BLE2SCCU_READFW_ID              0x7F03
#define BLE2SCCU_READFW_LEN             7

#define BLE2SCCU_SHUTDOWN_HEAD          0xAA
#define BLE2SCCU_SHUTDOWN_ID            0x6FFF
#define BLE2SCCU_SHUTDOWN_LEN           0

#define BLE2SCCU_H58A_HEAD              0xA6
#define BLE2SCCU_H58A_ID                0x058A
#define BLE2SCCU_H58A_LEN               8

#define BLE2SCCU_H58B_HEAD              0xA6
#define BLE2SCCU_H58B_ID                0x058B
#define BLE2SCCU_H58B_LEN               2

#define BLE2SCCU_FFD01_HEAD             0xA9
#define BLE2SCCU_FFD01_ID               0xFF01
#define BLE2SCCU_FFD01_LEN              0

#define BLE2SCCU_FFD02_HEAD             0xA9
#define BLE2SCCU_FFD02_ID               0xFF02
#define BLE2SCCU_FFD02_LEN              0

#define BLE2SCCU_FFD03_HEAD             0xA9
#define BLE2SCCU_FFD03_ID               0xFF03
#define BLE2SCCU_FFD03_LEN              0

/* BLE→SCCU (V2.0)*/
#define BLE2SCCU_NAVI_HEAD              0x60
#define BLE2SCCU_NAVI_INFO1_ID          0x592
#define BLE2SCCU_NAVI_INFO1_LEN         8
#define BLE2SCCU_NAVI_INFO1_INDEX       0
#define BLE2SCCU_NAVI_INFO2_ID          0x593
#define BLE2SCCU_NAVI_INFO2_LEN         8
#define BLE2SCCU_NAVI_INFO3_ID          0x594
#define BLE2SCCU_NAVI_INFO3_LEN         8
#define BLE2SCCU_NAVI_INFO4_ID          0x595
#define BLE2SCCU_NAVI_INFO4_LEN         61
#define BLE2SCCU_NAVI_INFO4_INDEX       (BLE2SCCU_NAVI_INFO1_INDEX + 1)
#define BLE2SCCU_ROAD_COUNT             (BLE2SCCU_NAVI_INFO4_INDEX + 1)
#define BLE2SCCU_NAVI_INFO5_ID          0x596
#define BLE2SCCU_NAVI_INFO5_LEN         0
#define BLE2SCCU_VOICE_CTRL_ID          0x598
#define BLE2SCCU_VOICE_CTRL_LEN         8

#define BLE2SCCU_TEL_MASSAGE_HEAD       0x61
#define BLE2SCCU_PHONE_NUMBER_ID        0x58C
#define BLE2SCCU_PHONE_NUMBER_LEN       30
#define BLE2SCCU_PHONE_NUMBER_INDEX     0
#define BLE2SCCU_PHONE_CALLER_ID        0x58D
#define BLE2SCCU_PHONE_CALLER_LEN       30
#define BLE2SCCU_PHONE_CALLER_INDEX     (BLE2SCCU_PHONE_NUMBER_INDEX + 1)
#define BLE2SCCU_PHONE_COUNT            (BLE2SCCU_PHONE_CALLER_INDEX + 1)
#define BLE2SCCU_SNS_TITLE_ID           0x58E
#define BLE2SCCU_SNS_TITLE_LEN          31
#define BLE2SCCU_SNS_TITLE_INDEX        0
#define BLE2SCCU_SNS_TEXT_ID            0x58F
#define BLE2SCCU_SNS_TEXT_LEN           31
#define BLE2SCCU_SNS_TEXT_INDEX         (BLE2SCCU_SNS_TITLE_INDEX + 1)
#define BLE2SCCU_SNS_SENDER_ID          0x591
#define BLE2SCCU_SNS_SENDER_LEN         31
#define BLE2SCCU_SNS_SENDER_INDEX       (BLE2SCCU_SNS_TEXT_INDEX + 1)
#define BLE2SCCU_SNS_COUNT              (BLE2SCCU_SNS_SENDER_INDEX + 1)

#define BLE2SCCU_MEDIA_HEAD             0x62
#define BLE2SCCU_CURRENT_STATE_ID       0x58A
#define BLE2SCCU_CURRENT_STATE_LEN      8
#define BLE2SCCU_NOTIFICATION_ID        0x58B
#define BLE2SCCU_NOTIFICATION_LEN       8
#define BLE2SCCU_MUSIC_ARITST_ID        0x59A
#define BLE2SCCU_MUSIC_ARITST_LEN       30
#define BLE2SCCU_MUSIC_ARITST_INDEX     0
#define BLE2SCCU_MUSIC_ALBUM_ID         0x59B
#define BLE2SCCU_MUSIC_ALBUM_LEN        30
#define BLE2SCCU_MUSIC_ALBUM_INDEX      (BLE2SCCU_MUSIC_ARITST_INDEX + 1)
#define BLE2SCCU_MUSIC_TITLE_ID         0x59C
#define BLE2SCCU_MUSIC_TITLE_LEN        30
#define BLE2SCCU_MUSIC_TITLE_INDEX      (BLE2SCCU_MUSIC_ALBUM_INDEX + 1)
#define BLE2SCCU_MUSIC_COUNT            (BLE2SCCU_MUSIC_TITLE_INDEX + 1)
#define BLE2SCCU_SETTING_1_ID           0x59D
#define BLE2SCCU_SETTING_1_LEN          8
#define BLE2SCCU_SETTING_2_ID           0x59E
#define BLE2SCCU_SETTING_2_LEN          8

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    uint16_t id;
    uint8_t len;
    uint8_t *dataPtr;
} BLEP_PackageInfoTypedef;

typedef struct
{
    bool newData;
    union
    {
        uint8_t ALL[(uint8_t)BLE2SCCU_READFW_LEN];
        struct
        {
            uint8_t version[(uint8_t)BLE2SCCU_READFW_LEN];
        } Bytes;
    } Data;
} BLEP_FirmwareTypedef;

typedef struct
{
    bool newData;
} BLEP_FFDTypedef;

#pragma pack(pop)

typedef void (*BLEP_FUNC_PTR)(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);

void BLEP_Create_FFD01Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_FFD02Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_FFD03Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_CanIDPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_H0148Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_FirmwarePackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_AdvPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_ProcessPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_CommondIDPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);
void BLEP_Create_ShutdownPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info);

BLEP_FirmwareTypedef *BLEP_Get_BleFirmware_Ptr(void);
BLEP_FFDTypedef *BLEP_Get_FF01_Ptr(void);
BLEP_FFDTypedef *BLEP_Get_FF02_Ptr(void);
BLEP_FFDTypedef *BLEP_Get_FF03_Ptr(void);

bool BLEP_Get_Authentication(uint8_t *localName, uint8_t *passKey, uint8_t *uuid, uint8_t *bondFlag);
bool BLEP_Get_H58A(uint8_t *data);
bool BLEP_Get_H58B(uint8_t *data);
bool BLEP_Get_Navi_Info1(uint8_t *data);
bool BLEP_Get_Navi_Info2(uint8_t *data);
bool BLEP_Get_Navi_Info3(uint8_t *data);
bool BLEP_Get_Setting_1(uint8_t *data);
bool BLEP_Get_Setting_2(uint8_t *data);
bool BLEP_Get_Voice_Ctrl(uint8_t *data);

bool BLEP_Get_Phone_Info(uint8_t *number, uint8_t *caller);
bool BLEP_Get_Music_Info(uint8_t *aritst, uint8_t *album, uint8_t *title);
bool BLEP_Get_SNS_Info(uint8_t *title, uint8_t *text, uint8_t *sender);
bool BLEP_Get_Road_Info(uint8_t *instruction);

bool BLEP_Get_ShutdownFlag(void);

void BLEP_Reset_Shutdown_Routine(void);

void BLEP_Reset_Tx_Package(void);

void BLEP_init(void);
void BLEP_open(void);
void BLEP_close(void);
void BLEP_flush(void);

void USART_SendData(BLEP_FUNC_PTR funcPtr, BLEP_PackageInfoTypedef *infoPtr);

#endif //BLE_Package_H_
