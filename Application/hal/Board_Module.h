#ifndef BOARD_H_
#define BOARD_H_

#include "../Modules/Modules.h"
#include <stdbool.h>

bool LINK_GetVal(void);

void PWR_Ctrl_PutVal(bool onoff);
void BT_Reset_PutVal(bool onoff);
void CAN_Set_PutVal(bool onoff);

void Board_init(void);
void Board_open(void);
void Board_close(void);
void Board_flush(void);

#endif //BOARD_H_
