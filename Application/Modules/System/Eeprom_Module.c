#include "./Eeprom_Module.h"

static const uint32_t BSP_FLASH_ALL_SIZE        = (128 << 10);
static const uint32_t BSP_FLASH_TOTAL_SIZE      = (FLASH_PAGE_SIZE * 3);
static const uint32_t BSP_FLASH_BASE_ADDRESS    = ((FLASH_BASE + BSP_FLASH_ALL_SIZE) - BSP_FLASH_TOTAL_SIZE);
static const uint32_t BSP_FLASH_BANK0_ADDRESS   = (BSP_FLASH_BASE_ADDRESS);
static const uint32_t BSP_FLASH_BANK1_ADDRESS   = (BSP_FLASH_BANK0_ADDRESS + FLASH_PAGE_SIZE);
static const uint32_t BSP_FLASH_BANK2_ADDRESS   = (BSP_FLASH_BANK1_ADDRESS + FLASH_PAGE_SIZE);

#define BSP_FLASH_PROG_SIZE                8U

#define BSP_FLASH_CONFIG_REGION_SIZE       64U
#define BSP_FLASH_DATA_REGION_SIZE         64U

#define BSP_FLASH_CONFIG_VERSION_IND       0U   /* Bank0 */
#define BSP_FLASH_CONFIG_QRCODE_IND        1U   /* Bank0 */
#define BSP_FLASH_CONFIG_PARAM_IND         2U   /* Bank0 */

#define BSP_FLASH_DATA_MAX_CNT             64

#define BSP_FLASH_GET_U8(addr)             *((uint8_t *)(addr))
#define BSP_FLASH_GET_U16(addr)            *((uint16_t *)(addr))
#define BSP_FLASH_GET_U32(addr)            *((uint32_t *)(addr))
#define BSP_FLASH_GET_U64(addr)            *((uint64_t *)(addr))

#pragma pack(push)
#pragma pack(1)
typedef union
{
    uint8_t u8_all[BSP_FLASH_CONFIG_REGION_SIZE];
    uint32_t u32_all[BSP_FLASH_CONFIG_REGION_SIZE / sizeof(uint32_t)];
    uint64_t u64_all[BSP_FLASH_CONFIG_REGION_SIZE / sizeof(uint64_t)];
} PAGE_DATA;
static PAGE_DATA flash_tmp_data;
#pragma pack(pop)
// static uint8_t flash_tmp_data[BSP_FLASH_CONFIG_REGION_SIZE];

/*-------- Local variables -------------------------------*/
static FLASH_DataCond_TypeDef eepCond;
/*--------------------------------------------------------*/

#pragma pack(push)
#pragma pack(1)
typedef union
{
    uint8_t all[46];
    struct
    {
        uint8_t index;
        uint32_t injection_Val;
        uint32_t accumulation_ODO;
        uint32_t vehicle_ODO;
        uint8_t UUID[32];
        uint8_t chkSum;
    } Data;
} FLASH_DATA;
static FLASH_DATA Eeprom_DataStruct;
#pragma pack(pop)

static void Eeprom_AcqStoredData(void)
{
    Eeprom_DataStruct.Data.injection_Val = CAN_Get_Injection_Val();
    Eeprom_DataStruct.Data.accumulation_ODO = DIAG_Get_Accumulation_ODO();
    Eeprom_DataStruct.Data.vehicle_ODO = DIAG_Get_Vehicle_ODO();
}

static void Eeprom_SetStoreDataDefault(void)
{
    Eeprom_DataStruct.Data.injection_Val = 0;
    Eeprom_DataStruct.Data.accumulation_ODO = 0;
    Eeprom_DataStruct.Data.vehicle_ODO = 0;
}

#pragma pack(push)
#pragma pack(1)
typedef union
{
    uint8_t all[1];
    struct
    {
        uint8_t close_QR;
    } Data;
} FLASHPARAM_DATA;
static FLASHPARAM_DATA Eepparam_DataStruct;
#pragma pack(pop)

static void Eeprom_SetParameterDataDefault(void)
{
    Eepparam_DataStruct.Data.close_QR = 0;
}
static void Eeprom_AcqParameterData(void)
{
    Eepparam_DataStruct.Data.close_QR = 1;
}

// #pragma pack(push)
// #pragma pack(1)
// typedef struct
// {
//     uint8_t saved;
//     uint8_t UUID[32];
//     uint8_t chkSum;
// } FLASHUUID_DATA;
// static FLASHUUID_DATA Eepuuid_DataStruct;
// #pragma pack(pop)

static void Eeprom_SetUUIDDefault(void)
{
    (void)memset(Eeprom_DataStruct.Data.UUID, 0, 32);
}

static void Eeprom_AcqUUID(uint8_t *ptr)
{
    (void)memcpy(Eeprom_DataStruct.Data.UUID, ptr, 32);
}

#pragma pack(push)
#pragma pack(1)
typedef union
{
    uint8_t all[26];
    struct
    {
        uint8_t saved;
        union
        {
            uint8_t Data[24];
            struct
            {
                uint8_t CCUID[14];
                uint8_t PassKey[6];
                uint8_t Dummy[4];
            } Bytes;
        } QRCODE;
        uint8_t chkSum;
    } Data;
} FLASHQRCODE_DATA;
static FLASHQRCODE_DATA Eepqrcode_DataStruct;
#pragma pack(pop)

static uint8_t qrcode_encoded[24];

static void Eeprom_SetQRCodeTest(void)
{
    Eepqrcode_DataStruct.Data.saved = 1;
    (void)memset(Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID, 0x30, 14);
    Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID[3] = DEF_DEVICE_CODE;
    Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID[13] = 0x31;
    (void)memset(Eepqrcode_DataStruct.Data.QRCODE.Bytes.PassKey, 0x30, 6);
    Eepqrcode_DataStruct.Data.QRCODE.Bytes.PassKey[5] = 0x31;
}
static void Eeprom_SetQRCodeDefault(void)
{
    Eepqrcode_DataStruct.Data.saved = 0;
    (void)memset(Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID, 0x30, 14);
    (void)memset(Eepqrcode_DataStruct.Data.QRCODE.Bytes.PassKey, 0x30, 6);
}

static void Eeprom_AcqQRCode(uint8_t *ptr)
{
    Eepqrcode_DataStruct.Data.saved = 1;
    (void)memcpy(Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID, ptr, 14);
    ptr += 14;
    (void)memcpy(Eepqrcode_DataStruct.Data.QRCODE.Bytes.PassKey, ptr, 6);
    ptr += 6;
    (void)memcpy(Eepqrcode_DataStruct.Data.QRCODE.Bytes.Dummy, ptr, 4);
}

/*-------------------------------------------------*/

const uint8_t FIR_VER[] = DEF_FW_VER;
const uint8_t FAC_NUM[] = DEF_FAC_NUM;

static void FLASH_Read_Config_Region(uint8_t ind, void *src)
{
    uint8_t *ptr = (uint8_t *)src;
    uint8_t ii = 0;
    uint32_t adderss = BSP_FLASH_BANK0_ADDRESS;
    uint32_t offset = BSP_FLASH_CONFIG_REGION_SIZE;

    if (((uint32_t)0xFFFFFFFF / offset) >= ind)
    {
        offset = offset * ind;
    }
    else
    {
        offset = (uint32_t)0xFFFFFFFF;
    }

    if (offset < FLASH_PAGE_SIZE)
    {
        if (((uint32_t)0xFFFFFFFF - adderss) >= offset)
        {
            adderss = adderss + offset;

            for (ii = 0; ii < BSP_FLASH_CONFIG_REGION_SIZE; ii++)
            {
                *ptr = BSP_FLASH_GET_U8(adderss);
                if ((uint32_t)0xFFFFFFFF > adderss)
                {
                    adderss++;
                    ptr++;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            /* code */
        }
    }
    else
    {
        /* nothing */
    }
}

static void FLASH_Read_Data_Region(uint8_t ind, void *src)
{
    uint8_t *ptr = (uint8_t *)src;
    uint8_t ii = 0;
    uint32_t adderss = BSP_FLASH_BANK1_ADDRESS;
    uint32_t offset = BSP_FLASH_DATA_REGION_SIZE;

    if (((uint32_t)0xFFFFFFFF / offset) >= ind)
    {
        offset = offset * ind;
    }
    else
    {
        offset = (uint32_t)0xFFFFFFFF;
    }

    if (offset < (FLASH_PAGE_SIZE * 2))
    {
        if (((uint32_t)0xFFFFFFFF - adderss) >= offset)
        {
            adderss = adderss + offset;

            for (ii = 0; ii < BSP_FLASH_DATA_REGION_SIZE; ii++)
            {
                *ptr = BSP_FLASH_GET_U8(adderss);
                if ((uint32_t)0xFFFFFFFF > adderss)
                {
                    adderss++;
                    ptr++;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            /* code */
        }
    }
    else
    {
        /* nothing */
    }
}

static bool FLASH_Write_Config_Region(uint8_t ind, void *src, uint8_t len)
{
    uint64_t *ptr = flash_tmp_data.u64_all;
    uint8_t ii = 0;
    bool result = true;
    HAL_StatusTypeDef status;
    uint32_t adderss = BSP_FLASH_BANK0_ADDRESS;
    uint32_t offset = BSP_FLASH_CONFIG_REGION_SIZE;

    if (((uint32_t)0xFFFFFFFF / offset) >= ind)
    {
        offset = offset * ind;
    }
    else
    {
        offset = (uint32_t)0xFFFFFFFF;
    }

    (void)memset(flash_tmp_data.u8_all, 0xFF, BSP_FLASH_CONFIG_REGION_SIZE);
    (void)memcpy(flash_tmp_data.u8_all, src, len);

    if (offset < FLASH_PAGE_SIZE)
    {
        if (((uint32_t)0xFFFFFFFF - adderss) >= offset)
        {
            adderss = adderss + offset;

            do
            {
                status = HAL_FLASH_Unlock();
            } while (status == HAL_ERROR);

            for (ii = 0; ii < BSP_FLASH_CONFIG_REGION_SIZE; ii += BSP_FLASH_PROG_SIZE)
            {
                status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, adderss, *ptr);
                if (status != HAL_OK)
                {
                    result = false;
                    break;
                }
                else
                {
                    if (((uint32_t)0xFFFFFFFF - adderss) >= BSP_FLASH_PROG_SIZE)
                    {
                        adderss = adderss + BSP_FLASH_PROG_SIZE;
                        ptr++;
                    }
                    else
                    {
                        result = false;
                        break;
                    }
                }
            }

            do
            {
                status = HAL_FLASH_Lock();
            } while (status == HAL_ERROR);
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }

    return result;
}

static bool FLASH_Write_Data_Region(uint8_t ind, void *src, uint8_t len)
{
    uint64_t *ptr = flash_tmp_data.u64_all;
    uint8_t ii = 0;
    bool result = true;
    HAL_StatusTypeDef status;
    uint32_t adderss = BSP_FLASH_BANK1_ADDRESS;
    uint32_t offset = BSP_FLASH_DATA_REGION_SIZE;

    if (((uint32_t)0xFFFFFFFF / offset) >= ind)
    {
        offset = offset * ind;
    }
    else
    {
        offset = (uint32_t)0xFFFFFFFF;
    }

    (void)memset(flash_tmp_data.u8_all, 0xFF, BSP_FLASH_DATA_REGION_SIZE);
    (void)memcpy(flash_tmp_data.u8_all, src, len);

    if (offset < (FLASH_PAGE_SIZE * 2))
    {
        if (((uint32_t)0xFFFFFFFF - adderss) >= offset)
        {
            adderss = adderss + offset;

            do
            {
                status = HAL_FLASH_Unlock();
            } while (status == HAL_ERROR);

            for (ii = 0; ii < BSP_FLASH_DATA_REGION_SIZE; ii += BSP_FLASH_PROG_SIZE)
            {
                status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, adderss, *ptr);
                if (status != HAL_OK)
                {
                    result = false;
                    break;
                }
                else
                {
                    if (((uint32_t)0xFFFFFFFF - adderss) >= BSP_FLASH_PROG_SIZE)
                    {
                        adderss = adderss + BSP_FLASH_PROG_SIZE;
                        ptr++;
                    }
                    else
                    {
                        result = false;
                        break;
                    }
                }
            }

            do
            {
                status = HAL_FLASH_Lock();
            } while (status == HAL_ERROR);
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }

    return result;
}

static void FLASH_Erase_All(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t EraseError = 0;
    uint32_t page_ind = 0;
    HAL_StatusTypeDef status;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.NbPages = 3;

    if (BSP_FLASH_BASE_ADDRESS >= FLASH_BASE)
    {
        page_ind = BSP_FLASH_BASE_ADDRESS - FLASH_BASE;
        page_ind = page_ind / FLASH_PAGE_SIZE;

        EraseInitStruct.Page = page_ind;

        do
        {
            status = HAL_FLASH_Unlock();
        } while (status == HAL_ERROR);

        status = HAL_FLASHEx_Erase(&EraseInitStruct, &EraseError);

        do
        {
            status = HAL_FLASH_Lock();
        } while (status == HAL_ERROR);
    }
    else
    {
        /* nothing */
    }
}

static void FLASH_Erase_Page(uint32_t tmp_address)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t EraseError = 0;
    uint32_t page_ind = 0;
    HAL_StatusTypeDef status;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.NbPages = 1;

    if (tmp_address >= FLASH_BASE)
    {
        page_ind = tmp_address - FLASH_BASE;
        page_ind = page_ind / FLASH_PAGE_SIZE;

        EraseInitStruct.Page = page_ind;

        do
        {
            status = HAL_FLASH_Unlock();
        } while (status == HAL_ERROR);

        status = HAL_FLASHEx_Erase(&EraseInitStruct, &EraseError);

        do
        {
            status = HAL_FLASH_Lock();
        } while (status == HAL_ERROR);
    }
    else
    {
        /* nothing */
    }
}

static bool Eeprom_CHK(void)
{

    uint8_t ind, chk;
    bool returnVal = false;
    uint8_t *pFIR_VER = 0;
    uint8_t *pFAC_VER = 0;

    FLASH_Read_Config_Region(BSP_FLASH_CONFIG_VERSION_IND, flash_tmp_data.u8_all);
    pFIR_VER = &flash_tmp_data.u8_all[0];
    pFAC_VER = &flash_tmp_data.u8_all[DEF_FIR_VER_LEN];

    chk = 0;
    for (ind = 0; ind < DEF_FIR_VER_LEN; ind++) // Check the firmware version in the EEPROM
    {
        if (FIR_VER[ind] != pFIR_VER[ind])
        {
            chk = chk + 1;
            break;
        }
    }
    for (ind = 0; ind < DEF_FAC_NUM_LEN; ind++) // Check the Factory number in the EEPROM
    {
        if (FAC_NUM[ind] != pFAC_VER[ind])
        {
            chk = chk + 1;
            break;
        }
    }

    if (chk == 0)
    {
        returnVal = true;
    }
    else
    {
        returnVal = false;
    }

    return returnVal;
}

/*-----------------------------------------------------*/
static void Eeprom_PARAM_Read(void)
{
    uint8_t *ptr, len;

    ptr = Eepparam_DataStruct.all;
    len = (uint8_t)(sizeof(FLASHPARAM_DATA));

    FLASH_Read_Config_Region(BSP_FLASH_CONFIG_PARAM_IND, flash_tmp_data.u8_all);
    (void)memcpy(ptr, flash_tmp_data.u8_all, len);
}
static void Eeprom_PARAM_Write(void)
{
    uint8_t *ptr, len;

    ptr = Eepparam_DataStruct.all;
    len = (uint8_t)(sizeof(FLASHPARAM_DATA));

    (void)FLASH_Write_Config_Region(BSP_FLASH_CONFIG_PARAM_IND, ptr, len);
}
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
static void Eeprom_QRCode_Read(void)
{
    uint8_t *ptr, len;

    ptr = Eepqrcode_DataStruct.all;
    len = (uint8_t)(sizeof(FLASHQRCODE_DATA));

    FLASH_Read_Config_Region(BSP_FLASH_CONFIG_QRCODE_IND, flash_tmp_data.u8_all);
    (void)memcpy(ptr, flash_tmp_data.u8_all, len);
}
static void Eeprom_QRCode_Write(void)
{
    uint8_t *ptr, len;
    uint8_t calCheckSum = 0;

    ptr = Eepqrcode_DataStruct.all;
    len = (uint8_t)(sizeof(Eepqrcode_DataStruct.all));

    calCheckSum = Eeprom_CalcChkSum(ptr, (len - (uint8_t)1));
    Eepqrcode_DataStruct.Data.chkSum = calCheckSum;
    (void)FLASH_Write_Config_Region(BSP_FLASH_CONFIG_QRCODE_IND, ptr, len);
}
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/

static void Eeprom_PrepareStoreData(void)
{
    Eeprom_DataStruct.Data.index = Eeprom_DataStruct.Data.index + 1;
    if (Eeprom_DataStruct.Data.index == 0xFF)
    {
        Eeprom_DataStruct.Data.index = 0;
    }
    else
    {
    }

    Eeprom_DataStruct.Data.chkSum = Eeprom_CalcChkSum(Eeprom_DataStruct.all, (uint8_t)(sizeof(Eeprom_DataStruct.all) - 1));
}

static void Eeprom_WriteData(void)
{
    uint8_t *ptr, len;
    uint16_t tmpInd;

    Eeprom_PrepareStoreData();

    ptr = Eeprom_DataStruct.all;
    len = (uint8_t)(sizeof(Eeprom_DataStruct.all));
    tmpInd = Eeprom_DataStruct.Data.index;
    tmpInd &= (uint8_t)0x3F;

    if (tmpInd == 0)
    {
        FLASH_Erase_Page(BSP_FLASH_BANK1_ADDRESS);
    }
    else if (tmpInd == 32)
    {
        FLASH_Erase_Page(BSP_FLASH_BANK2_ADDRESS);
    }
    else
    {
        /* nothing */
    }

    (void)FLASH_Write_Data_Region(tmpInd, ptr, len);
}

static uint8_t bank_index[BSP_FLASH_DATA_MAX_CNT];
static uint8_t bank_index_compare;
static uint8_t bank_index_select;

static bool Eeprom_ReadData(void)
{
    uint8_t *ptr, len, chkSum;
    uint16_t ind;
    bool returnVal = false;
    uint16_t tmpInd;
    bool readFail = false;

    for (ind = 0; ind < BSP_FLASH_DATA_MAX_CNT; ind++)
    {
        tmpInd = ind;
        FLASH_Read_Data_Region(tmpInd, flash_tmp_data.u8_all);
        bank_index[ind] = flash_tmp_data.u8_all[0];
    }

    bank_index_compare = (uint8_t)0xFF;
    bank_index_select = (uint8_t)0xFF;
    for (ind = 0; ind < BSP_FLASH_DATA_MAX_CNT; ind++)
    {
        if (bank_index[ind] != (uint8_t)0xFF)
        {
            if (bank_index_compare == (uint8_t)0xFF)
            {
                bank_index_compare = bank_index[ind];
                bank_index_select = ind;
            }
            else
            {
                if (bank_index_compare < bank_index[ind])
                {
                    if ((bank_index[ind] - bank_index_compare) < BSP_FLASH_DATA_MAX_CNT)
                    {
                        bank_index_compare = bank_index[ind];
                        bank_index_select = ind;
                    }
                    else
                    {
                        /* nothing */
                    }
                }
                else
                {
                    // if ((bank_index_compare - bank_index[ind]) > BSP_FLASH_DATA_MAX_CNT)
                    // {
                    //     bank_index_compare = bank_index[ind];
                    //     bank_index_select = ind;
                    // }
                    // else
                    // {
                    //     /* nothing */
                    // }
                }
            }
        }
        else
        {
            /* nothing */
        }
    }
    if (bank_index_select == (uint8_t)0xFF)
    {
        returnVal = false;
    }
    else
    {

        ptr = Eeprom_DataStruct.all;
        len = (uint8_t)(sizeof(Eeprom_DataStruct.all));

        for (ind = 0; ind < BSP_FLASH_DATA_MAX_CNT; ind++)
        {

            tmpInd = bank_index_select;
            FLASH_Read_Data_Region(tmpInd, flash_tmp_data.u8_all);
            (void)memcpy(ptr, flash_tmp_data.u8_all, len);
            chkSum = Eeprom_CalcChkSum(ptr, (len - (uint8_t)1));
            if (chkSum == Eeprom_DataStruct.Data.chkSum)
            {
                returnVal = true;
                if (readFail == true)
                {
                    Eeprom_DataStruct.Data.index = 0xFE;
                    FLASH_Erase_Page(BSP_FLASH_BANK2_ADDRESS);
                    Eeprom_WriteData();
                    FLASH_Read_Data_Region(0, flash_tmp_data.u8_all);
                    (void)memcpy(ptr, flash_tmp_data.u8_all, len);
                }
                break;
            }
            else
            {
                readFail = true;
            }

            if (bank_index_select > 0)
            {
                bank_index_select = bank_index_select - 1;
            }
            else
            {
                bank_index_select = (BSP_FLASH_DATA_MAX_CNT - 1);
            }
        }
    }
    return returnVal;
}

/*-----------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
static ModuleState_TypeDef EepromModule_state = MODULESTATE_RESET;

void Eeprom_init(void)
{
    bool tmp_Check;

    EepromModule_state = MODULESTATE_INITIALIZED;
    eepCond = FLASH_DataNotInitialized;
    Eeprom_DataStruct.Data.index = 0xFE;

    (void)memset(flash_tmp_data.u8_all, 0xFF, BSP_FLASH_CONFIG_REGION_SIZE);

    eepCond = FLASH_DataValid;
    /* Write the module initialize code here	*/
    tmp_Check = Eeprom_CHK();
    if (tmp_Check == true)
    {
        Eeprom_PARAM_Read();
        tmp_Check = Eeprom_ReadData();
        if (tmp_Check == true)
        {
            eepCond = FLASH_DataValid;
        }
        else
        {
            eepCond = FLASH_DataFailure;
        }
    }
    else
    {
        /* TODO: Perform the Assembly check function here	*/
        // Assy_Check();
        FLASH_Erase_All();

        /*TODO:  Write the Calibrated parameter here	*/

        /* Write factory number to EEPROM	*/
        uint8_t tmpArr[BSP_FLASH_CONFIG_REGION_SIZE];
        (void)memset(tmpArr, 0, BSP_FLASH_CONFIG_REGION_SIZE);
        (void)memcpy(tmpArr, FIR_VER, DEF_FIR_VER_LEN);
        (void)memcpy((tmpArr + DEF_FIR_VER_LEN), FAC_NUM, DEF_FAC_NUM_LEN);

        (void)FLASH_Write_Config_Region(BSP_FLASH_CONFIG_VERSION_IND, tmpArr, (DEF_FIR_VER_LEN + DEF_FAC_NUM_LEN));

        Eeprom_SetParameterDataDefault();
        // Eeprom_PARAM_Write();

        /* Reset the EEPROM data to factory default	*/
        Eeprom_SetStoreDataDefault();
        Eeprom_WriteData();

        Eeprom_SetUUIDDefault();
        Eeprom_WriteData();
        // Eeprom_UUID_Write();

        Eeprom_SetQRCodeTest();

        Eeprom_SetQRCodeDefault();
        // Eeprom_QRCode_Write();

        Eeprom_PARAM_Read();
        tmp_Check = Eeprom_ReadData();
        if (tmp_Check == true)
        {
            eepCond = FLASH_DataValid;
        }
        else
        {
            eepCond = FLASH_DataFailure;
        }
    }
}

void Eeprom_open(void)
{
    if (EepromModule_state == MODULESTATE_OPENED)
        return;
    EepromModule_state = MODULESTATE_OPENED;

    /* Write the module open code here	*/

    (void)Eeprom_ReadData();
    Eeprom_PARAM_Read();
    Eeprom_QRCode_Read();

    if (Eepqrcode_DataStruct.Data.saved != (uint8_t)1)
    {
        (void)memset(qrcode_encoded, 0xFF, 24);
    }
    else
    {
        qrcode_encoded[0]   = Eepqrcode_DataStruct.Data.QRCODE.Data[3];
        qrcode_encoded[1]   = Eepqrcode_DataStruct.Data.QRCODE.Data[11];
        qrcode_encoded[2]   = Eepqrcode_DataStruct.Data.QRCODE.Data[8];
        qrcode_encoded[3]   = Eepqrcode_DataStruct.Data.QRCODE.Data[19];
        qrcode_encoded[4]   = Eepqrcode_DataStruct.Data.QRCODE.Data[22];
        qrcode_encoded[5]   = Eepqrcode_DataStruct.Data.QRCODE.Data[17];
        qrcode_encoded[6]   = Eepqrcode_DataStruct.Data.QRCODE.Data[6];
        qrcode_encoded[7]   = Eepqrcode_DataStruct.Data.QRCODE.Data[0];
        qrcode_encoded[8]   = Eepqrcode_DataStruct.Data.QRCODE.Data[14];
        qrcode_encoded[9]   = Eepqrcode_DataStruct.Data.QRCODE.Data[20];
        qrcode_encoded[10]  = Eepqrcode_DataStruct.Data.QRCODE.Data[10];
        qrcode_encoded[11]  = Eepqrcode_DataStruct.Data.QRCODE.Data[23];
        qrcode_encoded[12]  = Eepqrcode_DataStruct.Data.QRCODE.Data[4];
        qrcode_encoded[13]  = Eepqrcode_DataStruct.Data.QRCODE.Data[9];
        qrcode_encoded[14]  = Eepqrcode_DataStruct.Data.QRCODE.Data[1];
        qrcode_encoded[15]  = Eepqrcode_DataStruct.Data.QRCODE.Data[18];
        qrcode_encoded[16]  = Eepqrcode_DataStruct.Data.QRCODE.Data[12];
        qrcode_encoded[17]  = Eepqrcode_DataStruct.Data.QRCODE.Data[5];
        qrcode_encoded[18]  = Eepqrcode_DataStruct.Data.QRCODE.Data[21];
        qrcode_encoded[19]  = Eepqrcode_DataStruct.Data.QRCODE.Data[13];
        qrcode_encoded[20]  = Eepqrcode_DataStruct.Data.QRCODE.Data[2];
        qrcode_encoded[21]  = Eepqrcode_DataStruct.Data.QRCODE.Data[15];
        qrcode_encoded[22]  = Eepqrcode_DataStruct.Data.QRCODE.Data[7];
        qrcode_encoded[23]  = Eepqrcode_DataStruct.Data.QRCODE.Data[16];
    }
}

void Eeprom_close(void)
{
    if (EepromModule_state == MODULESTATE_CLOSED)
        return;

    /* Write the module close code here	*/
    if (EepromModule_state == MODULESTATE_OPENED)
    {
        Eeprom_AcqStoredData();
        Eeprom_WriteData();
    }

    EepromModule_state = MODULESTATE_CLOSED;
}

void Eeprom_flush(void)
{
    if (EepromModule_state != MODULESTATE_OPENED)
        return;

    if (Eeprom_GetDataCond() == FLASH_DataFailure)
        return;
}

/*----------------------------------------------------*/
void Eeprom_ReqBackupData(void)
{
    Eeprom_AcqStoredData();
    Eeprom_WriteData();
}

void Eeprom_ReqBackupParameterData(void)
{
    if ((Eepparam_DataStruct.Data.close_QR == 0) || (Eepparam_DataStruct.Data.close_QR == 0xFF))
    {
        Eeprom_AcqParameterData();
        Eeprom_PARAM_Write();
    }
    else
    {
        /* nothing */
    }
}

void Eeprom_ReqBackupUUID(uint8_t *ptr)
{
    Eeprom_AcqUUID(ptr);
    Eeprom_WriteData();
}

void Eeprom_ReqBackupQRCODE(uint8_t *ptr)
{
    Eeprom_AcqQRCode(ptr);
    Eeprom_QRCode_Write();
    qrcode_encoded[0]   = Eepqrcode_DataStruct.Data.QRCODE.Data[3];
    qrcode_encoded[1]   = Eepqrcode_DataStruct.Data.QRCODE.Data[11];
    qrcode_encoded[2]   = Eepqrcode_DataStruct.Data.QRCODE.Data[8];
    qrcode_encoded[3]   = Eepqrcode_DataStruct.Data.QRCODE.Data[19];
    qrcode_encoded[4]   = Eepqrcode_DataStruct.Data.QRCODE.Data[22];
    qrcode_encoded[5]   = Eepqrcode_DataStruct.Data.QRCODE.Data[17];
    qrcode_encoded[6]   = Eepqrcode_DataStruct.Data.QRCODE.Data[6];
    qrcode_encoded[7]   = Eepqrcode_DataStruct.Data.QRCODE.Data[0];
    qrcode_encoded[8]   = Eepqrcode_DataStruct.Data.QRCODE.Data[14];
    qrcode_encoded[9]   = Eepqrcode_DataStruct.Data.QRCODE.Data[20];
    qrcode_encoded[10]  = Eepqrcode_DataStruct.Data.QRCODE.Data[10];
    qrcode_encoded[11]  = Eepqrcode_DataStruct.Data.QRCODE.Data[23];
    qrcode_encoded[12]  = Eepqrcode_DataStruct.Data.QRCODE.Data[4];
    qrcode_encoded[13]  = Eepqrcode_DataStruct.Data.QRCODE.Data[9];
    qrcode_encoded[14]  = Eepqrcode_DataStruct.Data.QRCODE.Data[1];
    qrcode_encoded[15]  = Eepqrcode_DataStruct.Data.QRCODE.Data[18];
    qrcode_encoded[16]  = Eepqrcode_DataStruct.Data.QRCODE.Data[12];
    qrcode_encoded[17]  = Eepqrcode_DataStruct.Data.QRCODE.Data[5];
    qrcode_encoded[18]  = Eepqrcode_DataStruct.Data.QRCODE.Data[21];
    qrcode_encoded[19]  = Eepqrcode_DataStruct.Data.QRCODE.Data[13];
    qrcode_encoded[20]  = Eepqrcode_DataStruct.Data.QRCODE.Data[2];
    qrcode_encoded[21]  = Eepqrcode_DataStruct.Data.QRCODE.Data[15];
    qrcode_encoded[22]  = Eepqrcode_DataStruct.Data.QRCODE.Data[7];
    qrcode_encoded[23]  = Eepqrcode_DataStruct.Data.QRCODE.Data[16];
}

FLASH_DataCond_TypeDef Eeprom_GetDataCond(void)
{
    return eepCond;
}

uint8_t Eeprom_CalcChkSum(void *src, uint8_t len)
{
    uint8_t ind, sum, *ptr;

    ptr = (uint8_t *)src;

    sum = 0;
    for (ind = 0; ind < len; ind++)
    {
        sum = sum + *ptr;
        ptr++;
    }

    sum = (~sum) + 1;
    return sum;
}
/*-------- Data APIs------------------------------*/
/* TODO Modify the following function ADD or DELETE function according to required application */

uint32_t Eeprom_Get_Injection_Val(void)
{
    return Eeprom_DataStruct.Data.injection_Val;
}

uint32_t Eeprom_Get_Vehicle_ODO(void)
{
    return Eeprom_DataStruct.Data.vehicle_ODO;
}

uint32_t Eeprom_Get_Accumulation_ODO(void)
{
    return Eeprom_DataStruct.Data.accumulation_ODO;
}

bool Eeprom_Get_QRCODE_Save(void)
{
    bool status = false;
    if (Eepqrcode_DataStruct.Data.saved == (uint8_t)1)
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}
bool Eeprom_Get_ReadQRCODE_Close(void)
{
    bool status = false;
    if (Eepparam_DataStruct.Data.close_QR == (uint8_t)1)
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}
uint8_t *Eeprom_Get_CCUID_Ptr(void)
{
    uint8_t *ptr = 0;
    if (Eepqrcode_DataStruct.Data.saved != (uint8_t)1)
    {
        ptr = 0;
    }
    else
    {
        ptr = Eepqrcode_DataStruct.Data.QRCODE.Bytes.CCUID;
    }

    return ptr;
}
uint8_t *Eeprom_Get_PassKey_Ptr(void)
{
    uint8_t *ptr = 0;
    if (Eepqrcode_DataStruct.Data.saved != (uint8_t)1)
    {
        ptr = 0;
    }
    else
    {
        ptr = Eepqrcode_DataStruct.Data.QRCODE.Bytes.PassKey;
    }

    return ptr;
}
uint8_t *Eeprom_Get_Dummy_Ptr(void)
{
    uint8_t *ptr = 0;
    if (Eepqrcode_DataStruct.Data.saved != (uint8_t)1)
    {
        ptr = 0;
    }
    else
    {
        ptr = Eepqrcode_DataStruct.Data.QRCODE.Bytes.Dummy;
    }

    return ptr;
}
uint8_t *Eeprom_Get_QRcode_Ptr(void)
{
    uint8_t *ptr = 0;

    ptr = qrcode_encoded;

    return ptr;
}
uint8_t *Eeprom_Get_UUID_Ptr(void)
{
    uint8_t *ptr = 0;

    ptr = Eeprom_DataStruct.Data.UUID;

    return ptr;
}
/* END Eeprom_Module */
