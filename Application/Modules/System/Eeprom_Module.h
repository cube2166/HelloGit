#ifndef Eeprom_Module_H_
#define Eeprom_Module_H_

#include "../Modules.h"

typedef enum
{
    FLASH_DataNotInitialized = 0,
    FLASH_DataFailure,
    FLASH_DataValid,
} FLASH_DataCond_TypeDef;

void Eeprom_ReqBackupData(void);
void Eeprom_ReqBackupParameterData(void);
void Eeprom_ReqBackupUUID(uint8_t *ptr);
void Eeprom_ReqBackupQRCODE(uint8_t *ptr);

FLASH_DataCond_TypeDef Eeprom_GetDataCond(void);
uint8_t Eeprom_CalcChkSum(void *src, uint8_t len);

uint32_t Eeprom_Get_Injection_Val(void);
uint32_t Eeprom_Get_Vehicle_ODO(void);
uint32_t Eeprom_Get_Accumulation_ODO(void);
bool Eeprom_Get_QRCODE_Save(void);
bool Eeprom_Get_ReadQRCODE_Close(void);
uint8_t *Eeprom_Get_CCUID_Ptr(void);
uint8_t *Eeprom_Get_PassKey_Ptr(void);
uint8_t *Eeprom_Get_Dummy_Ptr(void);
uint8_t *Eeprom_Get_QRcode_Ptr(void);
uint8_t *Eeprom_Get_UUID_Ptr(void);
/*---------- Control APIs -----------------------*/
void Eeprom_init(void);
void Eeprom_open(void);
void Eeprom_close(void);
void Eeprom_flush(void);

/* END Eeprom_Module */

#endif //Eeprom_Module_H_
