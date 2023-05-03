#ifndef BTN_Module_H_
#define BTN_Module_H_

#include "../Modules.h"

void BTN_Callback(uint8_t btn_signal, uint8_t dsp_signal);

uint8_t *BTN_Get_vol_Ctrl_Ptr(void);
uint8_t *BTN_Get_music_Ctrl_Ptr(void);
uint8_t *BTN_Get_tel_Ctrl_Ptr(void);

void BTN_init(void);
void BTN_open(void);
void BTN_close(void);
void BTN_flush(void);

#endif //BTN_Module_H_
