#include "./UTF_Module.h"

#define UTF_SPLIT_CNT               6
#define UTF_LEAD_SURROGATES_START   0xD8
#define UTF_LEAD_SURROGATES_END     0xDB
#define UTF_TRAIL_SURROGATES_START  0xDC
#define UTF_TRAIL_SURROGATES_END    0xDF
#define UTF_SHADOW_BUF_LEN          70

#define ASCII_0_IND                 0x30
#define ASCII_9_IND                 0x39
#define ASCII_A_IND                 0x41
#define ASCII_F_IND                 0x46
#define ASCII_a_IND                 0x61
#define ASCII_f_IND                 0x66

uint8_t shadow_Buf[UTF_SHADOW_BUF_LEN];

void UTF_Split_Word(uint8_t *data_Buf, uint8_t data_Len, uint8_t **split_Word_Buf,
                    uint8_t *split_Word_Len, uint8_t *word_Total, uint8_t *split_Total)
{
    uint8_t split_Cnt;
    uint8_t word_Cnt;
    uint8_t *shadow_Ptr = 0;
    uint8_t *start_Ptr = 0;
    uint8_t *data_Ptr = 0;
    uint8_t data_Ind;
    uint8_t shadow_Ind;
    uint8_t byte_Cnt;

    (void)memcpy(shadow_Buf, data_Buf, data_Len);
    (void)memset(data_Buf, 0, data_Len);

    shadow_Ptr = shadow_Buf;
    data_Ptr = data_Buf;
    start_Ptr = data_Ptr;
    shadow_Ind = 0;
    data_Ind = 0;

    split_Cnt = 0;
    byte_Cnt = 0;
    word_Cnt = 0;

    while (shadow_Ind < data_Len)
    {
        shadow_Ptr = (shadow_Buf + shadow_Ind);
        data_Ptr = (data_Buf + data_Ind);

        if ((*shadow_Ptr >= (uint8_t)UTF_LEAD_SURROGATES_START) && (*shadow_Ptr <= (uint8_t)UTF_LEAD_SURROGATES_END))
        {
            /* Special UTF-16 Word */
            if ((shadow_Ind + (uint8_t)3) < data_Len)
            {
                if ((*(shadow_Ptr + (uint8_t)2) >= (uint8_t)UTF_TRAIL_SURROGATES_START) && (*(shadow_Ptr + (uint8_t)2) <= (uint8_t)UTF_TRAIL_SURROGATES_END))
                {
                    /* mark this pointer */
                    (void)memcpy(data_Ptr, shadow_Ptr, 4); /* Align Buffer */
                    data_Ind = data_Ind + 4;
                    shadow_Ind = shadow_Ind + 4;

                    word_Cnt = word_Cnt + 1;

                    if ((byte_Cnt + (uint8_t)4) > (uint8_t)UTF_SPLIT_CNT)
                    {
                        split_Word_Buf[split_Cnt] = start_Ptr;
                        split_Word_Len[split_Cnt] = byte_Cnt;
                        split_Cnt = split_Cnt + 1;

                        start_Ptr += byte_Cnt;
                        byte_Cnt = 4;
                    }
                    else if ((byte_Cnt + (uint8_t)4) == (uint8_t)UTF_SPLIT_CNT)
                    {
                        byte_Cnt = byte_Cnt + 4;
                        split_Word_Buf[split_Cnt] = start_Ptr;
                        split_Word_Len[split_Cnt] = byte_Cnt;
                        split_Cnt = split_Cnt + 1;

                        start_Ptr += byte_Cnt;
                        byte_Cnt = 0;
                    }
                    else
                    {
                        byte_Cnt = byte_Cnt + 4;
                    }
                }
                else
                {
                    /* Format Error */
                    shadow_Ind = shadow_Ind + 4;
                }
            }
            else
            {
                /* Length not enough */
                shadow_Ind = shadow_Ind + 4;
            }
        }
        else
        {
            /* Normal UTF-16 Word */
            if ((shadow_Ind + (uint8_t)1) < data_Len)
            {
                if ((*shadow_Ptr < (uint8_t)UTF_TRAIL_SURROGATES_START) || (*shadow_Ptr > (uint8_t)UTF_TRAIL_SURROGATES_END))
                {
                    if ((*shadow_Ptr != (uint8_t)0x00) || (*(shadow_Ptr + (uint8_t)1) != (uint8_t)0x00))
                    {
                        /* mark this pointer */
                        (void)memcpy(data_Ptr, shadow_Ptr, 2); /* Align Buffer */
                        data_Ind = data_Ind + 2;
                        shadow_Ind = shadow_Ind + 2;

                        word_Cnt = word_Cnt + 1;

                        if ((byte_Cnt + (uint8_t)2) > (uint8_t)UTF_SPLIT_CNT)
                        {
                            split_Word_Buf[split_Cnt] = start_Ptr;
                            split_Word_Len[split_Cnt] = byte_Cnt;
                            split_Cnt = split_Cnt + 1;

                            start_Ptr += byte_Cnt;
                            byte_Cnt = 2;
                        }
                        else if ((byte_Cnt + (uint8_t)2) == (uint8_t)UTF_SPLIT_CNT)
                        {
                            byte_Cnt = byte_Cnt + 2;
                            split_Word_Buf[split_Cnt] = start_Ptr;
                            split_Word_Len[split_Cnt] = byte_Cnt;
                            split_Cnt = split_Cnt + 1;

                            start_Ptr += byte_Cnt;
                            byte_Cnt = 0;
                        }
                        else
                        {
                            byte_Cnt = byte_Cnt + 2;
                        }
                    }
                    else
                    {
                        /* Format Error */
                        shadow_Ind = shadow_Ind + 2;
                    }
                }
                else
                {
                    /* Format Error */
                    shadow_Ind = shadow_Ind + 2;
                }
            }
            else
            {
                /* Length not enough */
                shadow_Ind = shadow_Ind + 2;
            }
        }
    }
    if (byte_Cnt != 0)
    {
        split_Word_Buf[split_Cnt] = start_Ptr;
        split_Word_Len[split_Cnt] = byte_Cnt;
        split_Cnt = split_Cnt + 1;

        byte_Cnt = 0;
    }
    *word_Total = word_Cnt;

    split_Cnt = 0;
    while (data_Ind != 0)
    {
        if (data_Ind >= 6)
        {
            data_Ind -= 6;
            split_Cnt++;
        }
        else
        {
            data_Ind = 0;
            split_Cnt++;
        }
    }

    *split_Total = split_Cnt;
}

uint8_t UTF_ASCII_2_Hex(uint8_t ascii)
{
    uint8_t hex = ascii;

    if ((ascii >= ASCII_0_IND) && (ascii <= ASCII_9_IND))
    {
        hex = hex - ASCII_0_IND;
    }
    else if ((ascii >= ASCII_A_IND) && (ascii <= ASCII_F_IND))
    {
        hex = hex - ASCII_A_IND;
        hex = hex + 10;
    }
    else if ((ascii >= ASCII_a_IND) && (ascii <= ASCII_f_IND))
    {
        hex = hex - ASCII_a_IND;
        hex = hex + 10;
    }
    else
    {
        hex = 15;
    }

    return hex;
}