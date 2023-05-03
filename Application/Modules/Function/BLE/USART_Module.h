#ifndef USART_Module_H_
#define USART_Module_H_

#include "../../Modules.h"

void USART_QueueReset(void);

// void USART_SendData(BLEP_FUNC_PTR funcPtr, BLEP_PackageInfoTypedef *infoPtr);
bool USART_ReadData(uint8_t *data);
void USART_SoftwareReset(void);

void USART_init(void);
void USART_open(void);
void USART_close(void);
void USART_flush(void);

#endif //USART_Module_H_
