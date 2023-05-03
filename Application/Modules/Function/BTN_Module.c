#include "./BTN_Module.h"

#define BTN_UPDATE_PER 10

#define BTN_SHORT_PER           (100 / BTN_UPDATE_PER)
#define BTN_LONG_PER_1S         (1000 / BTN_UPDATE_PER)

#define CMD_DEFAULT         0xFF
#define CMD_TEL_ENDCALL     0
#define CMD_TEL_DECLINE     0
#define CMD_TEL_ANSWER      1
#define CMD_NEXT_TRACK      1
#define CMD_PREVIOUS_TRACK  2
#define CMD_MUSIC_PLAY      3
#define CMD_MUSIC_PAUSE     4
#define CMD_VOL_UP          5
#define CMD_VOL_DOWN        6

typedef enum
{
    Btn_signal_RELEASE = 0,
    Btn_signal_PRESS,
    Btn_signal_NONE,
} Btn_signal_TypeDef;

typedef enum
{
    Dsp_signal_OTHER = 0,
    Dsp_signal_MUSIC_PLAY,
    Dsp_signal_MUSIC_PAUSE,
    Dsp_signal_TEL_INCOMING,
    Dsp_signal_TEL_DURING_ENDPOSI,
    Dsp_signal_TEL_DURING_ENDNEGA,
    Dsp_signal_RESERVE,
} Dsp_signal_TypeDef;

typedef enum
{
    Btn_state_NONE = 0,
    Btn_state_SHORT,
    Btn_state_HIT,
    Btn_state_LONG_1S,
} Btn_state_TypeDef;

typedef struct
{
    uint16_t persist;
    Btn_signal_TypeDef *btn_signal;
    Btn_state_TypeDef btn_cond;
} Btn_Status_TypeDef;

static Btn_Status_TypeDef btn_UP;
static Btn_Status_TypeDef btn_DOWN;
static Btn_Status_TypeDef btn_RIGHT;
static Btn_Status_TypeDef btn_LEFT;
static Btn_Status_TypeDef btn_SELECT;

static Btn_signal_TypeDef btn_signal_UP         = Btn_signal_NONE;
static Btn_signal_TypeDef btn_signal_DOWN       = Btn_signal_NONE;
static Btn_signal_TypeDef btn_signal_RIGHT      = Btn_signal_NONE;
static Btn_signal_TypeDef btn_signal_LEFT       = Btn_signal_NONE;
static Btn_signal_TypeDef btn_signal_SELECT     = Btn_signal_NONE;

static Dsp_signal_TypeDef dsp_signal_Current    = Dsp_signal_OTHER;
static Dsp_signal_TypeDef dsp_signal_Before     = Dsp_signal_OTHER;

static volatile uint16_t btn_Timeout = 0;
static uint16_t tmp_btn_Timeout = 0;

static uint8_t vol_Ctrl         = CMD_DEFAULT;
static uint8_t music_Ctrl       = CMD_DEFAULT;
static uint8_t tel_Ctrl         = CMD_DEFAULT;
static uint8_t vol_Ctrl_tmp     = CMD_DEFAULT;
static uint8_t music_Ctrl_tmp   = CMD_DEFAULT;
static uint8_t tel_Ctrl_tmp     = CMD_DEFAULT;

void BTN_Callback(uint8_t btn_signal, uint8_t dsp_signal)
{
    btn_signal_UP       = (Btn_signal_TypeDef)((btn_signal & (uint8_t)0b10000000) >> (uint8_t)7);
    btn_signal_DOWN     = (Btn_signal_TypeDef)((btn_signal & (uint8_t)0b01000000) >> (uint8_t)6);
    btn_signal_RIGHT    = (Btn_signal_TypeDef)((btn_signal & (uint8_t)0b00100000) >> (uint8_t)5);
    btn_signal_LEFT     = (Btn_signal_TypeDef)((btn_signal & (uint8_t)0b00010000) >> (uint8_t)4);
    btn_signal_SELECT   = (Btn_signal_TypeDef)((btn_signal & (uint8_t)0b00001000) >> (uint8_t)3);
    btn_Timeout         = (uint16_t)((uint8_t)50 / (uint8_t)BTN_UPDATE_PER);

    dsp_signal &= (uint8_t)0xF0;
    dsp_signal >>= (uint8_t)4;
    if (dsp_signal >= 6)
    {
        dsp_signal_Current = Dsp_signal_RESERVE;
    }
    else
    {
        dsp_signal_Current = (Dsp_signal_TypeDef)dsp_signal;
    }
}

static bool BTN_check(Btn_Status_TypeDef *pBtn)
{
    bool tmp_Val = false;
    if (pBtn != 0)
    {
        if (pBtn->btn_signal != 0)
        {
            switch (*(pBtn->btn_signal))
            {
            case Btn_signal_PRESS:
                pBtn->persist = pBtn->persist + 1;
                if (pBtn->persist >= (uint16_t)0xFFFE)
                {
                    pBtn->persist = (uint16_t)0xFFFE;
                    pBtn->btn_cond = Btn_state_NONE;
                }
                else
                {
                    if (pBtn->persist == BTN_SHORT_PER)
                    {
                        pBtn->btn_cond = Btn_state_SHORT;
                    }
                    else if (pBtn->persist == BTN_LONG_PER_1S)
                    {
                        pBtn->btn_cond = Btn_state_LONG_1S;
                    }
                    else
                    {
                        /* nothing */
                    }
                }
                tmp_Val = true;
                break;
            case Btn_signal_RELEASE:
                pBtn->persist = 0;
                if (pBtn->btn_cond == Btn_state_SHORT)
                {
                    pBtn->btn_cond = Btn_state_HIT;
                }
                else
                {
                    pBtn->btn_cond = Btn_state_NONE;
                }
                break;
            default:
                pBtn->persist = (uint16_t)0xFFFE;
                pBtn->btn_cond = Btn_state_NONE;
                break;
            }
        }
        else
        {
            pBtn->persist = (uint16_t)0xFFFE;
            pBtn->btn_cond = Btn_state_NONE;
        }
    }
    else
    {
        /* ignore */
    }
    return tmp_Val;
}

/*========================================================================================*/

static ModuleState_TypeDef BTNModule_state = MODULESTATE_RESET;

void BTN_init(void)
{
    BTNModule_state = MODULESTATE_INITIALIZED;

    btn_signal_UP           = Btn_signal_NONE;
    btn_signal_DOWN         = Btn_signal_NONE;
    btn_signal_RIGHT        = Btn_signal_NONE;
    btn_signal_LEFT         = Btn_signal_NONE;
    btn_signal_SELECT       = Btn_signal_NONE;

    dsp_signal_Current      = Dsp_signal_OTHER;
    dsp_signal_Before       = Dsp_signal_OTHER;

    btn_UP.persist          = (uint16_t)0xFFFE;
    btn_UP.btn_signal       = &btn_signal_UP;
    btn_UP.btn_cond         = Btn_state_NONE;

    btn_DOWN.persist        = (uint16_t)0xFFFE;
    btn_DOWN.btn_signal     = &btn_signal_DOWN;
    btn_DOWN.btn_cond       = Btn_state_NONE;

    btn_RIGHT.persist        = (uint16_t)0xFFFE;
    btn_RIGHT.btn_signal     = &btn_signal_RIGHT;
    btn_RIGHT.btn_cond       = Btn_state_NONE;

    btn_LEFT.persist        = (uint16_t)0xFFFE;
    btn_LEFT.btn_signal     = &btn_signal_LEFT;
    btn_LEFT.btn_cond       = Btn_state_NONE;

    btn_SELECT.persist      = (uint16_t)0xFFFE;
    btn_SELECT.btn_signal   = &btn_signal_SELECT;
    btn_SELECT.btn_cond     = Btn_state_NONE;

    vol_Ctrl                = CMD_DEFAULT;
    music_Ctrl              = CMD_DEFAULT;
    tel_Ctrl                = CMD_DEFAULT;
    vol_Ctrl_tmp            = CMD_DEFAULT;
    music_Ctrl_tmp          = CMD_DEFAULT;
    tel_Ctrl_tmp            = CMD_DEFAULT;

    btn_Timeout             = 0;
}

void BTN_open(void)
{
    if (BTNModule_state != MODULESTATE_OPENED)
    {
        BTNModule_state = MODULESTATE_OPENED;
        /* Write the module open code here	*/

        btn_signal_UP           = Btn_signal_NONE;
        btn_signal_DOWN         = Btn_signal_NONE;
        btn_signal_RIGHT        = Btn_signal_NONE;
        btn_signal_LEFT         = Btn_signal_NONE;
        btn_signal_SELECT       = Btn_signal_NONE;

        dsp_signal_Current      = Dsp_signal_OTHER;
        dsp_signal_Before       = Dsp_signal_OTHER;

        btn_UP.persist          = (uint16_t)0xFFFE;
        btn_UP.btn_signal       = &btn_signal_UP;
        btn_UP.btn_cond         = Btn_state_NONE;

        btn_DOWN.persist        = (uint16_t)0xFFFE;
        btn_DOWN.btn_signal     = &btn_signal_DOWN;
        btn_DOWN.btn_cond       = Btn_state_NONE;

        btn_RIGHT.persist       = (uint16_t)0xFFFE;
        btn_RIGHT.btn_signal    = &btn_signal_RIGHT;
        btn_RIGHT.btn_cond      = Btn_state_NONE;

        btn_LEFT.persist        = (uint16_t)0xFFFE;
        btn_LEFT.btn_signal     = &btn_signal_LEFT;
        btn_LEFT.btn_cond       = Btn_state_NONE;

        btn_SELECT.persist      = (uint16_t)0xFFFE;
        btn_SELECT.btn_signal   = &btn_signal_SELECT;
        btn_SELECT.btn_cond     = Btn_state_NONE;

        vol_Ctrl                = CMD_DEFAULT;
        music_Ctrl              = CMD_DEFAULT;
        tel_Ctrl                = CMD_DEFAULT;

        vol_Ctrl_tmp            = CMD_DEFAULT;
        music_Ctrl_tmp          = CMD_DEFAULT;
        tel_Ctrl_tmp            = CMD_DEFAULT;

        btn_Timeout             = 0;
    }
    else
    {
        /* nothing */
    }
}

void BTN_close(void)
{
    if (BTNModule_state != MODULESTATE_CLOSED)
    {
        BTNModule_state = MODULESTATE_CLOSED;
        /* Write the module close code here	*/
    }
    else
    {
        /* nothing */
    }
}

void BTN_flush(void)
{
    if (BTNModule_state == MODULESTATE_OPENED)
    {
        uint8_t check_Cnt = 0;
        bool tmp_Result;
        tmp_btn_Timeout = btn_Timeout;
        if (tmp_btn_Timeout > 0)
        {
            tmp_btn_Timeout--;

            tmp_Result = BTN_check(&btn_UP);
            if (tmp_Result == true)
            {
                check_Cnt = check_Cnt + 1;
            }
            else
            {
                /* nothing */
            }

            tmp_Result = BTN_check(&btn_DOWN);
            if (tmp_Result == true)
            {
                check_Cnt = check_Cnt + 1;
            }
            else
            {
                /* nothing */
            }

            tmp_Result = BTN_check(&btn_RIGHT);
            if (tmp_Result == true)
            {
                check_Cnt = check_Cnt + 1;
            }
            else
            {
                /* nothing */
            }

            tmp_Result = BTN_check(&btn_LEFT);
            if (tmp_Result == true)
            {
                check_Cnt = check_Cnt + 1;
            }
            else
            {
                /* nothing */
            }

            tmp_Result = BTN_check(&btn_SELECT);
            if (tmp_Result == true)
            {
                check_Cnt = check_Cnt + 1;
            }
            else
            {
                /* nothing */
            }

            if ((dsp_signal_Before != dsp_signal_Current) || (check_Cnt > 1))
            {
                dsp_signal_Before = dsp_signal_Current;

                btn_UP.persist          = (uint16_t)0xFFFE;
                btn_UP.btn_cond         = Btn_state_NONE;

                btn_DOWN.persist        = (uint16_t)0xFFFE;
                btn_DOWN.btn_cond       = Btn_state_NONE;

                btn_RIGHT.persist       = (uint16_t)0xFFFE;
                btn_RIGHT.btn_cond      = Btn_state_NONE;

                btn_LEFT.persist        = (uint16_t)0xFFFE;
                btn_LEFT.btn_cond       = Btn_state_NONE;

                btn_SELECT.persist      = (uint16_t)0xFFFE;
                btn_SELECT.btn_cond     = Btn_state_NONE;

                vol_Ctrl                = CMD_DEFAULT;
                music_Ctrl              = CMD_DEFAULT;
                tel_Ctrl                = CMD_DEFAULT;

                vol_Ctrl_tmp            = CMD_DEFAULT;
                music_Ctrl_tmp          = CMD_DEFAULT;
                tel_Ctrl_tmp            = CMD_DEFAULT;
            }
            else
            {
                check_Cnt = 0;
                vol_Ctrl_tmp = CMD_DEFAULT;
                music_Ctrl_tmp = CMD_DEFAULT;
                tel_Ctrl_tmp = CMD_DEFAULT;
                switch (dsp_signal_Before)
                {
                case Dsp_signal_OTHER:
                    /* nothing */
                    break;
                case Dsp_signal_MUSIC_PLAY:
                    if (btn_UP.btn_cond == Btn_state_HIT)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                        vol_Ctrl_tmp = CMD_VOL_UP;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_UP.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_NEXT_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_DOWN.btn_cond == Btn_state_HIT)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        vol_Ctrl_tmp = CMD_VOL_DOWN;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_DOWN.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_PREVIOUS_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_SELECT.btn_cond == Btn_state_HIT)
                    {
                        btn_SELECT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_MUSIC_PAUSE;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_SELECT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_SELECT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_LEFT.btn_cond == Btn_state_HIT)
                    {
                        btn_LEFT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_PREVIOUS_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_LEFT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_LEFT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_RIGHT.btn_cond == Btn_state_HIT)
                    {
                        btn_RIGHT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_NEXT_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_RIGHT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_RIGHT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (check_Cnt > 1)
                    {
                        vol_Ctrl_tmp = CMD_DEFAULT;
                        music_Ctrl_tmp = CMD_DEFAULT;
                    }
                    else
                    {
                        /* nothing */
                    }
                    break;
                case Dsp_signal_MUSIC_PAUSE:
                    if (btn_UP.btn_cond == Btn_state_HIT)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                        vol_Ctrl_tmp = CMD_VOL_UP;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_UP.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_NEXT_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_DOWN.btn_cond == Btn_state_HIT)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        vol_Ctrl_tmp = CMD_VOL_DOWN;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_DOWN.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_PREVIOUS_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_SELECT.btn_cond == Btn_state_HIT)
                    {
                        btn_SELECT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_MUSIC_PLAY;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_SELECT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_SELECT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_LEFT.btn_cond == Btn_state_HIT)
                    {
                        btn_LEFT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_PREVIOUS_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_LEFT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_LEFT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_RIGHT.btn_cond == Btn_state_HIT)
                    {
                        btn_RIGHT.btn_cond = Btn_state_NONE;
                        music_Ctrl_tmp = CMD_NEXT_TRACK;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_RIGHT.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_RIGHT.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (check_Cnt > 1)
                    {
                        vol_Ctrl_tmp = CMD_DEFAULT;
                        music_Ctrl_tmp = CMD_DEFAULT;
                    }
                    else
                    {
                        /* nothing */
                    }
                    break;
                case Dsp_signal_TEL_INCOMING:
                    if (btn_UP.btn_cond == Btn_state_HIT)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                        tel_Ctrl_tmp = CMD_TEL_ANSWER;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_UP.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_UP.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (btn_DOWN.btn_cond == Btn_state_HIT)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        tel_Ctrl_tmp = CMD_TEL_DECLINE;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_DOWN.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (check_Cnt > 1)
                    {
                        tel_Ctrl_tmp = CMD_DEFAULT;
                    }
                    else
                    {
                        /* nothing */
                    }
                    break;
                case Dsp_signal_TEL_DURING_ENDPOSI:
                    if (btn_DOWN.btn_cond == Btn_state_HIT)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                        tel_Ctrl_tmp = CMD_TEL_ENDCALL;
                        check_Cnt = check_Cnt + 1;
                    }
                    else if (btn_DOWN.btn_cond == Btn_state_LONG_1S)
                    {
                        btn_DOWN.btn_cond = Btn_state_NONE;
                    }
                    else
                    {
                        /* nothing */
                    }

                    if (check_Cnt > 1)
                    {
                        tel_Ctrl_tmp = CMD_DEFAULT;
                    }
                    else
                    {
                        /* nothing */
                    }
                    break;
                case Dsp_signal_TEL_DURING_ENDNEGA:
                    /* nothing */
                    break;
                case Dsp_signal_RESERVE:
                    /* nothing */
                    break;
                default:
                    break;
                }

                if (vol_Ctrl == CMD_DEFAULT)
                {
                    vol_Ctrl = vol_Ctrl_tmp;
                }
                else
                {
                    /* nothing */
                }

                if (music_Ctrl == CMD_DEFAULT)
                {
                    music_Ctrl = music_Ctrl_tmp;
                }
                else
                {
                    /* nothing */
                }

                if (tel_Ctrl == CMD_DEFAULT)
                {
                    tel_Ctrl = tel_Ctrl_tmp;
                }
                else
                {
                    /* nothing */
                }
            }
        }
        else
        {
            btn_signal_UP           = Btn_signal_NONE;
            btn_signal_DOWN         = Btn_signal_NONE;
            btn_signal_RIGHT        = Btn_signal_NONE;
            btn_signal_LEFT         = Btn_signal_NONE;
            btn_signal_SELECT       = Btn_signal_NONE;

            dsp_signal_Current      = Dsp_signal_OTHER;
            dsp_signal_Before       = Dsp_signal_OTHER;

            btn_UP.persist          = (uint16_t)0xFFFE;
            btn_UP.btn_cond         = Btn_state_NONE;

            btn_DOWN.persist        = (uint16_t)0xFFFE;
            btn_DOWN.btn_cond       = Btn_state_NONE;

            btn_RIGHT.persist       = (uint16_t)0xFFFE;
            btn_RIGHT.btn_cond      = Btn_state_NONE;

            btn_LEFT.persist        = (uint16_t)0xFFFE;
            btn_LEFT.btn_cond       = Btn_state_NONE;

            btn_SELECT.persist      = (uint16_t)0xFFFE;
            btn_SELECT.btn_cond     = Btn_state_NONE;

            vol_Ctrl                = CMD_DEFAULT;
            music_Ctrl              = CMD_DEFAULT;
            tel_Ctrl                = CMD_DEFAULT;

            vol_Ctrl_tmp            = CMD_DEFAULT;
            music_Ctrl_tmp          = CMD_DEFAULT;
            tel_Ctrl_tmp            = CMD_DEFAULT;
        }
        btn_Timeout = tmp_btn_Timeout;
    }
    else
    {
        /* nothing */
    }
}

uint8_t *BTN_Get_vol_Ctrl_Ptr(void)
{
    return &vol_Ctrl;
}
uint8_t *BTN_Get_music_Ctrl_Ptr(void)
{
    return &music_Ctrl;
}
uint8_t *BTN_Get_tel_Ctrl_Ptr(void)
{
    return &tel_Ctrl;
}
