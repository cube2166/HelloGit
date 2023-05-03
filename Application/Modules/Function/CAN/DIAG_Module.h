#ifndef DIAG_Module_H_
#define DIAG_Module_H_

#include "../../Modules.h"

typedef enum
{
	SID_NONE 								 = 0x00,
	SID_START_DIAGNOSTIC_SESSION			 = 0x10,
	SID_ECU_RESET							 = 0x11,
	SID_READ_FREEZE_FRAME_DATA				 = 0x12,
	SID_READ_DIAGNOSTIC_TROUBLE_CODES		 = 0x13,
	SID_CLEAR_DIAGNOSTIC_INFORMATION		 = 0x14,
	SID_READ_DIAGNOSTIC_TROUBLE_STATUS 		 = 0x17,
	SID_READ_DTC_BY_STATUS					 = 0x18,
	SID_READ_DTC_INFORMATION 				 = 0x19,
	SID_READ_ECU_ID 						 = 0x1A,
	SID_STOP_DIAGNOSTIC_SESSION 			 = 0x20,
	SID_READ_DATA_BY_LOCAL_ID 				 = 0x21,
	SID_READ_DATA_BY_COMMAND_ID 			 = 0x22,
	SID_READ_MEMORY_BY_ADDRESS 				 = 0X23,
	SID_READ_SCALING_DATA_BY_IDENTIFIER 	 = 0x24,
	SID_STOP_REPEATED_DATA_TRANSMISSION 	 = 0x25,
	SID_SET_DATA_RATES 						 = 0X26,
	SID_SECURITY_ACCESS 					 = 0X27,
	SID_COMMUNICATION_CONTROL 				 = 0x28,
	SID_READ_DATA_BY_PERIODIC_IDENTIFIER 	 = 0x29,
	SID_READ_DATA_BY_ID_PERIODIC 			 = 0x2A,
	SID_DYNAMICALLY_DEFINE_LOCAL_ID 		 = 0x2C,
	SID_DEFINE_PID_BY_MEMORY_ADDRESS 		 = 0x2D,
	SID_WRITE_DATA_BY_COMMAND_ID 			 = 0x2E,
	SID_INPUT_OUTPUT_CONTROL_BY_COMMAND_ID 	 = 0x2F,
	SID_INPUT_OUTPUT_CONTROL_BY_LOCAL_ID 	 = 0x30,
	SID_START_ROUTINE_BY_LOCAL_ID 			 = 0x31,
	SID_STOP_ROUTINE_BY_LOCAL_ID 			 = 0x32,
	SID_REQUEST_ROUTINE_RESULTS_BY_LOCAL_ID  = 0x33,
	SID_REQUEST_DOWNLOAD 					 = 0x34,
	SID_REQUEST_UPLOAD 						 = 0x35,
	SID_TRANSFER_DATA 						 = 0x36,
	SID_REQUEST_TRANSFER_EXIT 				 = 0x37,
	SID_START_ROUTINE_BY_ADDRESS 			 = 0x38,
	SID_STOP_ROUTINE_BY_ADDRESS 			 = 0x39,
	SID_REQUEST_ROUTINE_RESULTS_ADDRESS 	 = 0x3A,
	SID_WRITE_DATA_BY_LOCAL_ID 				 = 0x3B,
	SID_WRITE_MEMORY_BY_ADDRESS 			 = 0x3D,
	SID_TESTER_PRESENT 						 = 0x3E,
	SID_NEGATIVE_RESPONSE 					 = 0x7F,/* NR */
	SID_ESC_CODE 							 = 0x80,
	SID_START_COMMUNICATION 				 = 0x81,
	SID_STOP_COMMUNICATION 					 = 0x82,
	SID_ACCESS_TIMING 						 = 0x83,
	SID_SECURED_DATA_TRANS 					 = 0x84,
	SID_CONTROL_DTC_SETTINGS 				 = 0x85,
	SID_RESPONSE_ON_EVENT 					 = 0x86,
	SID_LINK_CONTROL						 = 0x87,
} DIAGSID_TypeDef;

typedef enum
{
	SF_SESSION_DEFAULT 			= 0x01,
	SF_SESSION_PROGRAM 			= 0x02,
	SF_SESSION_EXTENDED 		= 0x03,
	SF_TESTERPRESENT_RESPOND 	= 0x00,
	SF_TESTERPRESENT_NORESPOND 	= 0x80,
} DIAGSF_TypeDef;

typedef enum
{
	PCI_SINGLE_FRAME		= 0x00,
	PCI_FIRST_FRAME			= 0x10,
	PCI_CONSECUTIVE_FRAME	= 0x20,
	PCI_FLOW_CONTROL		= 0x30,
} DIAGPCI_TypeDef;

typedef enum
{
	NRC_NONE 										= 0x00,
	NRC_GENERAL_REJECT								= 0x10,
	NRC_SERVICE_NOT_SUPPORTED						= 0x11,
	NRC_SUB_FUNCTION_NOT_SUPPORTED					= 0x12,
	NRC_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT	= 0x13,
	NRC_REQUEST_OUT_OF_RANGE						= 0x31,
	NRC_BUSY_RESPECT_REQUEST						= 0x21,
	NRC_CONDITIONS_NOT_CORRECT						= 0x22,
	NRC_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING	= 0x78,
} DIAGNRC_TypeDef;

typedef enum
{
	FC_FS_BS 	= 0x0,
	FC_FS_CF 	= 0x1,
	FC_FS_FFDL 	= 0x2,
} DIAGFC_TypeDef;

/* COMMAND ID */
#define CID_VIN 							0xF190
#define CID_VIN_LEN 						17
#define CID_ECU_SOFTWARE_NUMBER 			0xF194
#define CID_ECU_SOFTWARE_NUMBER_LEN 		4
#define CID_REGISTERED_MODEL 				0xF197
#define CID_REGISTERED_MODEL_LEN 			4
#define CID_ECU_ENERGIZATION_TOTAL_TIME 	0xF1A1
#define CID_ECU_ENERGIZATION_TOTAL_TIME_LEN 4
#define CID_ECU_COUNT 						0xF1A2
#define CID_ECU_COUNT_LEN 					2
#define CID_TRAVEL_DISTANCE 				0xF421
#define CID_TRAVEL_DISTANCE_LEN 			2
#define CID_EG_OPERATION_TIME 				0xF44D
#define CID_EG_OPERATION_TIME_LEN 			2

/* LOCAL ID */
#define LID_H01 							0x01
#define LID_H01_LEN 						33
#define LID_H48 							0x48
#define LID_H48_LEN 						4
#define LID_FFD1_GROUP_START				0x1
#define LID_FFD1_GROUP_END 					0x5
#define LID_FFD1_ITEM_START					0x0
#define LID_FFD1_ITEM_END 					0x9
#define LID_FFD2_START						0x60
#define LID_FFD2_LAST						0x7F
#define LID_FFD3_START						0x09
#define LID_FFD3_LAST						0x09
/* FFD2 ID */

typedef enum
{
	STATUS_NONE = 0,
	STATUS_WAIT,
	STATUS_OK,
	STATUS_TIMEOUT,
} DIAGSTATUS_TypeDef;

void DIAG_Request_commandID(void);
void DIAG_Request_ffd01(void);
void DIAG_Request_ffd02(void);
void DIAG_Request_ffd03(void);

DIAGSTATUS_TypeDef DIAG_Get_commandIDStatus(void);
DIAGSTATUS_TypeDef DIAG_Get_ffd01Status(void);
DIAGSTATUS_TypeDef DIAG_Get_ffd02Status(void);
DIAGSTATUS_TypeDef DIAG_Get_ffd03Status(void);

uint8_t *DIAG_Get_cid_vin_data_Ptr(void);
uint8_t *DIAG_Get_cid_ecu_software_number_data_Ptr(void);
uint8_t *DIAG_Get_cid_registered_model_data_Ptr(void);
uint8_t *DIAG_Get_cid_ecu_energization_total_time_data_Ptr(void);
uint8_t *DIAG_Get_cid_ecu_count_data_Ptr(void);
uint8_t *DIAG_Get_cid_travel_distance_data_Ptr(void);
uint8_t *DIAG_Get_cid_eg_operation_time_data_Ptr(void);

uint8_t *DIAG_Get_lid_h01_data_Ptr(void);
uint8_t *DIAG_Get_lid_h48_data_Ptr(void);

void DIAG_Reset_fifoIndex(void);
void DIAG_Reset_Status(void);

bool DIAG_Get_LID_H01H48_Status(void);

uint32_t DIAG_Get_Vehicle_ODO(void);
uint32_t DIAG_Get_Accumulation_ODO(void);

void DIAG_init(void);
void DIAG_open(void);
void DIAG_close(void);
void DIAG_flush(void);

#endif //DIAG_Module_H_
