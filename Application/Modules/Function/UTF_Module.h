#ifndef UTF_Module_H_
#define UTF_Module_H_

#include "../Modules.h"

void UTF_Split_Word(uint8_t *data_Buf, uint8_t data_Len, uint8_t **split_Word_Buf,
                    uint8_t *split_Word_Len, uint8_t *word_Total, uint8_t *split_Total);
uint8_t UTF_ASCII_2_Hex(uint8_t ascii);

#endif //UTF_Module_H_
