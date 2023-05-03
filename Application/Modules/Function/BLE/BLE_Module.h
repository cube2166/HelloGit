#ifndef BLE_Module_H_
#define BLE_Module_H_

#include "../../Modules.h"

void BLE_Send_FFDdata(uint16_t id, uint8_t *dataPtr, uint8_t index, uint8_t ffd);

uint8_t *BLE_Get_ccuid_adv_data_Ptr(void);
uint8_t *BLE_Get_process_data_Ptr(void);
void BLE_Load_advData(void);
void BLE_Set_AdvFlag(bool flag);
bool BLE_Get_Connect_Status(void);

void BLE_init(void);
void BLE_open(void);
void BLE_close(void);
void BLE_flush(void);

#endif //BLE_Module_H_
