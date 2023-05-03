#include "./BLE_Package.h"

#define BLEP_BUFFER_LEN 200

static uint8_t 	blep_Rx_Data; 					/* Bluetooth Receive Package Data */
static uint8_t 	blep_Rx_Buf[BLEP_BUFFER_LEN]; 	/* Bluetooth Receive Package Buffer */
static uint8_t 	blep_Rx_Ind; 					/* Bluetooth Receive Package Data Index */
static uint16_t blep_Rx_Reset_Time; 			/* Bluetooth Receive Package Align Time */
static uint8_t 	blep_Rx_Checksum; 				/* Bluetooth Receive Package Checksum */
static uint8_t 	blep_Rx_Tail_Cnt; 				/* Bluetooth Receive Package Tail Counter */
static uint8_t 	blep_Rx_Head; 					/* Bluetooth Receive Package Head */
static uint8_t 	blep_Rx_Group; 					/* Bluetooth Receive Package Group */
static uint8_t 	blep_Rx_GroupMax; 				/* Bluetooth Receive Package Max Group */
static uint8_t 	blep_Rx_Group_Len[20]; 			/* Bluetooth Receive Package Group Data Length */
static uint8_t 	blep_Rx_Group_Len_Cnt; 			/* Bluetooth Receive Package Group Data Counter */
static uint16_t blep_Rx_Group_ID[20]; 			/* Bluetooth Receive Package Group ID */
static uint8_t 	blep_Rx_Group_ID_Cnt; 			/* Bluetooth Receive Package Group ID Counter */
static uint8_t 	*blep_Rx_Group_Buf_Ptr[20]; 	/* Bluetooth Receive Package Group Buffer Pointer */

static uint8_t 	blep_Tx_Buf[BLEP_BUFFER_LEN]; 	/* Bluetooth Transmit Package Buffer */
static uint8_t 	blep_Tx_Ind; 					/* Bluetooth Transmit Package Data Index */

static uint8_t  advPackage_crc; 				/* Bluetooth Transmit Advertising Package CRC */
static uint8_t  canIDPackage_crc; 				/* Bluetooth Transmit CANID Package CRC */
static uint8_t  commandIDPackage_crc; 			/* Bluetooth Transmit CommanID Package CRC */
static uint8_t  firmwarePackage_crc; 			/* Bluetooth Transmit Firmware Package CRC */
static uint8_t  shutdownPackage_crc; 			/* Bluetooth Transmit Shutdown Package CRC */
static uint8_t  processPackage_crc; 			/* Bluetooth Transmit Process Package CRC */
static uint8_t  h0148Package_crc; 				/* Bluetooth Transmit H0148 Package CRC */
static uint8_t  ffd01Package_crc; 				/* Bluetooth Transmit FFD01 Package CRC */
static uint8_t  ffd02Package_crc; 				/* Bluetooth Transmit FFD02 Package CRC */
static uint8_t  ffd03Package_crc; 				/* Bluetooth Transmit FFD03 Package CRC */

static void BLEP_Reset_Rx_Package(void)
{
	blep_Rx_Ind = 0;
	blep_Rx_Reset_Time = SimpleOS_GetTick();
	blep_Rx_Checksum = 0;
	blep_Rx_Tail_Cnt = 0;
	blep_Rx_Head = 0;
	blep_Rx_Group = 0;
	blep_Rx_GroupMax = 0;
	blep_Rx_Group_Len_Cnt = 0;
	blep_Rx_Group_ID_Cnt = 0;
}

/* ================================SCCU1.0 DATA================================ */
static BLEP_FirmwareTypedef blep_Firmware_Package;
static BLEP_FFDTypedef blep_Shutdown_Package;
static BLEP_FFDTypedef blep_FF01_Package;
static BLEP_FFDTypedef blep_FF02_Package;
static BLEP_FFDTypedef blep_FF03_Package;
/* ---------------------------------------------------------------------------- */

/* ================================SCCU2.0 DATA================================ */

typedef struct
{
	uint8_t data[(uint8_t)8];
} BLEP_PassThroughPackageTypedef;
static BLEP_PassThroughPackageTypedef blep_H58A_Package[1];
static BLEP_PassThroughPackageTypedef blep_H58B_Package[1];
static BLEP_PassThroughPackageTypedef blep_Navi_Info1_Package[1];
static BLEP_PassThroughPackageTypedef blep_Navi_Info2_Package[1];
static BLEP_PassThroughPackageTypedef blep_Navi_Info3_Package[1];
static BLEP_PassThroughPackageTypedef blep_Voice_Ctrl_Package[1];
static BLEP_PassThroughPackageTypedef blep_Setting_1_Package[1];
static BLEP_PassThroughPackageTypedef blep_Setting_2_Package[1];

typedef struct
{
	uint8_t localName[(uint8_t)BLE2SCCU_AUTH_LOCALNAME_LEN];
	uint8_t passKey[(uint8_t)BLE2SCCU_AUTH_PASSKEY_LEN];
	uint8_t uuid[(uint8_t)BLE2SCCU_AUTH_UUID_LEN];
	uint8_t bondFlag[(uint8_t)BLE2SCCU_AUTH_BONDFLAG_LEN];
} BLEP_AuthenticationPackageTypedef;
static BLEP_AuthenticationPackageTypedef blep_Authentication_Package[1];

typedef struct
{
	uint8_t instruction[(uint8_t)BLE2SCCU_NAVI_INFO4_LEN];
} BLEP_RoadPackageTypedef;
static BLEP_RoadPackageTypedef blep_Road_Package[1];

typedef struct
{
	uint8_t number[(uint8_t)BLE2SCCU_PHONE_NUMBER_LEN];
	uint8_t caller[(uint8_t)BLE2SCCU_PHONE_CALLER_LEN];
} BLEP_PhonePackageTypedef;
static BLEP_PhonePackageTypedef blep_Phone_Package[1];

typedef struct
{
	uint8_t title[(uint8_t)BLE2SCCU_SNS_TITLE_LEN];
	uint8_t text[(uint8_t)BLE2SCCU_SNS_TEXT_LEN];
	uint8_t sender[(uint8_t)BLE2SCCU_SNS_SENDER_LEN];
} BLEP_SNSPackageTypedef;
static BLEP_SNSPackageTypedef blep_SNS_Package[2];

typedef struct
{
	uint8_t aritst[(uint8_t)BLE2SCCU_MUSIC_ARITST_LEN];
	uint8_t album[(uint8_t)BLE2SCCU_MUSIC_ALBUM_LEN];
	uint8_t title[(uint8_t)BLE2SCCU_MUSIC_TITLE_LEN];
} BLEP_MusicPackageTypedef;
static BLEP_MusicPackageTypedef blep_Music_Package[1];

static uint8_t *blep_Authentication_DataPtr[BLE2SCCU_AUTH_COUNT];
static uint8_t blep_Authentication_Len[BLE2SCCU_AUTH_COUNT];
static uint8_t blep_Authentication_Offset;

static uint8_t *blep_PassThrough_DataPtr[1];
static uint8_t blep_PassThrough_Len[1];

static uint8_t *blep_SNS_DataPtr[BLE2SCCU_SNS_COUNT];
static uint8_t blep_SNS_Len[BLE2SCCU_SNS_COUNT];
static uint8_t blep_SNS_Cnt = 0;
static uint8_t blep_SNS_Sequence = 0;

static uint8_t *blep_Music_DataPtr[BLE2SCCU_MUSIC_COUNT];
static uint8_t blep_Music_Len[BLE2SCCU_MUSIC_COUNT];
static uint8_t blep_Music_Cnt = 0;

static uint8_t *blep_Phone_DataPtr[BLE2SCCU_PHONE_COUNT];
static uint8_t blep_Phone_Len[BLE2SCCU_PHONE_COUNT];
static uint8_t blep_Phone_Cnt = 0;

static uint8_t *blep_Road_DataPtr[BLE2SCCU_ROAD_COUNT];
static uint8_t blep_Road_Len[BLE2SCCU_ROAD_COUNT];
static uint8_t blep_Road_Cnt = 0;

/* ---------------------------------------------------------------------------- */

/* ================================BLEP_QUEUE================================ */

typedef void (*BLEP_QUEUE_PTR)(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt);

typedef struct
{
	void *storage_Ptr;
	uint16_t storage_Size;
	uint16_t front;
	uint16_t back;
	bool isEmpty;
	bool canOverwrite;
	bool canBlocked;
	bool canCheckHash;
	bool isBlocked;
	BLEP_QUEUE_PTR push_Func;
	BLEP_QUEUE_PTR pop_Func;
	uint32_t last_Hash;
} BLEP_MessageQueueTypedef;

static BLEP_MessageQueueTypedef blep_Authentication_Queue;

static BLEP_MessageQueueTypedef blep_H58A_Queue;
static BLEP_MessageQueueTypedef blep_H58B_Queue;
static BLEP_MessageQueueTypedef blep_Navi_Info1_Queue;
static BLEP_MessageQueueTypedef blep_Navi_Info2_Queue;
static BLEP_MessageQueueTypedef blep_Navi_Info3_Queue;
static BLEP_MessageQueueTypedef blep_Voice_Ctrl_Queue;
static BLEP_MessageQueueTypedef blep_Setting_1_Queue;
static BLEP_MessageQueueTypedef blep_Setting_2_Queue;

static BLEP_MessageQueueTypedef blep_Road_Queue;
static BLEP_MessageQueueTypedef blep_Phone_Queue;
static BLEP_MessageQueueTypedef blep_SNS_Queue;
static BLEP_MessageQueueTypedef blep_Music_Queue;

static BLEP_MessageQueueTypedef *blep_PassThrough_QueuePtr;

static void BLEP_QueueInitial(BLEP_MessageQueueTypedef *queue, void *storage_Ptr, uint8_t storage_Size, BLEP_QUEUE_PTR push_Func, BLEP_QUEUE_PTR pop_Func, uint32_t hash, bool canOverwrite, bool canBlocked, bool canCheckHash)
{
	queue->storage_Ptr = storage_Ptr;
	queue->storage_Size = storage_Size;
	queue->front = 0;
	queue->back = 0;
	queue->isEmpty = true;
	queue->canOverwrite = canOverwrite;
	queue->canBlocked = canBlocked;
	queue->canCheckHash = canCheckHash;
	queue->isBlocked = false;
	queue->push_Func = push_Func;
	queue->pop_Func = pop_Func;
	queue->last_Hash = hash;
}

static void BLEP_QueueReset(BLEP_MessageQueueTypedef *queue)
{
	queue->front = 0;
	queue->back = 0;
	queue->isEmpty = true;
	queue->isBlocked = false;
	queue->last_Hash = 0;
}

static bool BLEP_QueueIsEmpty(BLEP_MessageQueueTypedef *queue)
{
	bool tmp_Result = false;

	if ((queue->back == queue->front) && (queue->isEmpty == true))
	{
		tmp_Result = true;
	}
	else
	{
		tmp_Result = false;
	}
	return tmp_Result;
}

static bool BLEP_QueueIsFull(BLEP_MessageQueueTypedef *queue)
{
	bool tmp_Result = false;

	if ((queue->back == queue->front) && (queue->isEmpty == false))
	{
		tmp_Result = true;
	}
	else
	{
		tmp_Result = false;
	}
	return tmp_Result;
}

static bool BLEP_QueuePush(BLEP_MessageQueueTypedef *queue, uint8_t **data_Ptr, uint8_t *data_Len, uint32_t hash)
{
	bool tmp_Result = false;
	uint16_t tmp_Position;

	if ((queue->storage_Size != 0) && (queue->storage_Ptr != 0) && (queue->push_Func != 0))
	{
		if (BLEP_QueueIsFull(queue) == true)
		{
			if ((queue->last_Hash != hash) || (queue->canCheckHash == false))
			{
				if ((queue->canOverwrite == true) && (queue->isBlocked == false))
				{
					/* overwrite last node */
					tmp_Result = true;

					queue->push_Func(data_Ptr, data_Len, queue->storage_Ptr, queue->back);

					queue->last_Hash = hash;
				}
				else
				{
					/* can't push node from queue */
					tmp_Result = false;
				}
			}
			else
			{
				/* can't push node from queue */
				tmp_Result = false;
			}
		}
		else
		{
			if ((queue->last_Hash != hash) || (queue->canCheckHash == false))
			{
				if (queue->isBlocked == false)
				{
					tmp_Result = true;

					tmp_Position = queue->back + 1;
					tmp_Position %= queue->storage_Size;
					queue->back = tmp_Position;

					queue->push_Func(data_Ptr, data_Len, queue->storage_Ptr, queue->back);

					if (queue->back == queue->front)
					{
						queue->isEmpty = false;
						if (queue->canBlocked == true)
						{
							queue->isBlocked = true;
						}
						else
						{
							/* nothing */
						}
					}
					else
					{
						/* nothing */
					}

					queue->last_Hash = hash;
				}
				else
				{
					/* can't push node from queue */
					tmp_Result = false;
				}
			}
			else
			{
				/* can't push node from queue */
				tmp_Result = false;
			}
		}
	}
	else
	{
		/* can't push node from queue */
		tmp_Result = false;
	}

	return tmp_Result;
}

static bool BLEP_QueuePop(BLEP_MessageQueueTypedef *queue, uint8_t **data_Ptr, uint8_t *data_Len)
{
	bool tmp_Result = false;
	uint16_t tmp_Position;

	if ((queue->storage_Size != 0) && (queue->storage_Ptr != 0) && (queue->pop_Func != 0))
	{
		if (BLEP_QueueIsEmpty(queue) == true)
		{
			/* can't pop node from queue */
			tmp_Result = false;
		}
		else
		{
			tmp_Result = true;

			tmp_Position = queue->front + 1;
			tmp_Position %= queue->storage_Size;
			queue->front = tmp_Position;

			queue->pop_Func(data_Ptr, data_Len, queue->storage_Ptr, queue->front);

			if (queue->back == queue->front)
			{
				queue->isEmpty = true;
				queue->isBlocked = false;
			}
			else
			{
				/* nothing */
			}
		}
	}
	else
	{
		/* can't pop node from queue */
		tmp_Result = false;
	}

	return tmp_Result;
}

/* ---------------------------------------------------------------------------- */

static uint8_t BLEP_Calc_Checksum(uint8_t *ptr, uint8_t len)
{
	uint8_t ii = 0;
	uint8_t chkSum = 0;

	for (ii = 0; ii < len; ii++)
	{
		chkSum = chkSum + *ptr;
		ptr++;
	}
	chkSum = (~chkSum) + (uint8_t)1;
	return chkSum;
}

static uint32_t BLEP_Hash_Combine(uint32_t hash_A, uint32_t hash_B)
{
	uint32_t hash = 0;

	hash = hash_A << 1;
	hash = hash + hash_A;
	hash = hash + hash_B;

	return hash;
}

static uint32_t BLEP_Calc_Hash(uint8_t *ptr, uint8_t len)
{
	uint32_t c1 = 0xcc9e2d51;
	uint32_t c2 = 0x1b873593;
	uint32_t r1 = 15;
	uint32_t r2 = 13;
	uint32_t m = 5;
	uint32_t n = 0xe6546b64;
	uint32_t hash = 0;
	uint32_t k = 0;
	uint8_t *data = ptr; /* 32 bit extract from `key' */
	uint8_t *tail = 0;	 /* tail - last 8 bytes */
	uint8_t ind = 0;
	uint8_t length = len / 4; /* chunk length */
	uint8_t offset = 0;

	hash = 0;

	tail = (uint8_t *)&(data[0]); /* Last 8 byte chunk of `key' */

	/* For each 4 byte chunk of `key' */
	for (ind = 0; ind < length; ind++)
	{
		/* Next 4 byte chunk of `key' */
		offset = ind * 4;
		k = data[offset + 0];
		k <<= 8;
		k |= data[offset + 1];
		k <<= 8;
		k |= data[offset + 2];
		k <<= 8;
		k |= data[offset + 3];

		/* Encode next 4 byte chunk of `key' */
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		/* Append to hash */
		hash ^= k;
		hash = (hash << r2) | (hash >> (32 - r2));
		hash = hash * m + n;
	}

	k = 0;

	/* remainder */
	switch (len & 3) /* len % 4 */
	{
	case 3:
		k ^= (tail[(len - 3)] << 16);
	case 2:
		k ^= (tail[(len - 2)] << 8);
	case 1:
		k ^= tail[(len - 1)];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
		hash ^= k;
		break;
	}

	hash ^= len;

	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}

static uint8_t BLEP_Set_Package(uint8_t startByte, uint8_t group, uint16_t *group_id, uint8_t *group_len,
							   uint8_t *group_dataPtr[], uint8_t crc)
{
	uint8_t groupCnt = 0;
	blep_Tx_Ind = 0;
	blep_Tx_Buf[blep_Tx_Ind] = startByte;
	blep_Tx_Ind = blep_Tx_Ind + 1;
	blep_Tx_Buf[blep_Tx_Ind] = group;
	blep_Tx_Ind = blep_Tx_Ind + 1;
	while (groupCnt < group)
	{
		blep_Tx_Buf[blep_Tx_Ind] = (uint8_t)(group_id[groupCnt] >> (uint16_t)8); //ID_1
		blep_Tx_Ind = blep_Tx_Ind + 1;
		blep_Tx_Buf[blep_Tx_Ind] = (uint8_t)(group_id[groupCnt] & (uint16_t)0xFF); //ID_2
		blep_Tx_Ind = blep_Tx_Ind + 1;
		blep_Tx_Buf[blep_Tx_Ind] = group_len[groupCnt]; //LEN
		blep_Tx_Ind = blep_Tx_Ind + 1;
		if (group_len[groupCnt] > (uint8_t)0)
		{
			(void)memcpy(&blep_Tx_Buf[blep_Tx_Ind], group_dataPtr[groupCnt], group_len[groupCnt]); //DATA
		}
		else
		{
			/* nothing */
		}
		blep_Tx_Ind = blep_Tx_Ind + group_len[groupCnt];
		groupCnt++;
	}
	blep_Tx_Buf[blep_Tx_Ind] = crc;
	blep_Tx_Ind = blep_Tx_Ind + 1;
	blep_Tx_Buf[blep_Tx_Ind] = BLEP_Calc_Checksum(blep_Tx_Buf, blep_Tx_Ind);
	blep_Tx_Ind = blep_Tx_Ind + 1;
	return blep_Tx_Ind;
}

static bool BLEP_Analyze_Package(uint8_t data)
{
	bool status = false;
	blep_Rx_Buf[blep_Rx_Ind] = data;
	blep_Rx_Checksum = blep_Rx_Checksum + data;
	blep_Rx_Ind = blep_Rx_Ind + 1;
	if (blep_Rx_Ind >= (uint8_t)BLEP_BUFFER_LEN)
	{
		blep_Rx_Head = 0;
		blep_Rx_Ind = 0;
		blep_Rx_Checksum = 0;
	}
	else if (blep_Rx_Ind == (uint8_t)1)
	{
		if ((data == (uint8_t)BLE2SCCU_FFD01_HEAD) || (data == (uint8_t)BLE2SCCU_H58A_HEAD) || (data == (uint8_t)BLE2SCCU_READFW_HEAD))
		{
			blep_Rx_Head = data;
		}
		else if ((data == (uint8_t)BLE2SCCU_NAVI_HEAD) || (data == (uint8_t)BLE2SCCU_TEL_MASSAGE_HEAD) || (data == (uint8_t)BLE2SCCU_MEDIA_HEAD))
		{
			blep_Rx_Head = data;
		}
		else
		{
			blep_Rx_Ind = 0;
			blep_Rx_Checksum = 0;
		}
	}
	else if (blep_Rx_Ind == (uint8_t)2)
	{
		blep_Rx_GroupMax = data;
		blep_Rx_Tail_Cnt = 0;
		blep_Rx_Group_ID_Cnt = 0;
		blep_Rx_Group = 0;
		blep_Rx_Group_Len_Cnt = 0;
		blep_Rx_Group_Len[blep_Rx_Group] = 0xFF;
	}
	else
	{
		if (blep_Rx_Group < blep_Rx_GroupMax)
		{
			if (blep_Rx_Group_Len[blep_Rx_Group] == (uint8_t)0xFF)
			{
				if (blep_Rx_Group_ID_Cnt == (uint8_t)0)
				{
					blep_Rx_Group_ID[blep_Rx_Group] = ((uint16_t)data << (uint16_t)8);
					blep_Rx_Group_ID_Cnt = blep_Rx_Group_ID_Cnt + 1;
				}
				else if (blep_Rx_Group_ID_Cnt == (uint8_t)1)
				{
					blep_Rx_Group_ID[blep_Rx_Group] |= ((uint16_t)data);
					blep_Rx_Group_ID_Cnt = blep_Rx_Group_ID_Cnt + 1;
				}
				else
				{
					blep_Rx_Group_Len[blep_Rx_Group] = data;
				}
			}
			else
			{
				if (blep_Rx_Group_Len_Cnt == (uint8_t)0)
				{
					blep_Rx_Group_Buf_Ptr[blep_Rx_Group] = &blep_Rx_Buf[blep_Rx_Ind - 1];
				}
				else
				{
				}
				blep_Rx_Group_Len_Cnt = blep_Rx_Group_Len_Cnt + 1;
				if (blep_Rx_Group_Len_Cnt == blep_Rx_Group_Len[blep_Rx_Group])
				{
					blep_Rx_Group_ID_Cnt = 0;
					blep_Rx_Group = blep_Rx_Group + 1;
					blep_Rx_Group_Len_Cnt = 0;
					blep_Rx_Group_Len[blep_Rx_Group] = 0xFF;
				}
				else if (blep_Rx_Group_Len[blep_Rx_Group] == (uint8_t)0)
				{
					blep_Rx_Group_ID_Cnt = 0;
					blep_Rx_Group = blep_Rx_Group + 1;
					blep_Rx_Group_Len_Cnt = 0;
					blep_Rx_Group_Len[blep_Rx_Group] = 0xFF;
					blep_Rx_Tail_Cnt = blep_Rx_Tail_Cnt + 1;
				}
				else
				{
					/* nothing */
				}
			}
		}
		else
		{
			blep_Rx_Tail_Cnt = blep_Rx_Tail_Cnt + 1;
			if (blep_Rx_Tail_Cnt == (uint8_t)2)
			{
				if (blep_Rx_Checksum == (uint8_t)0)
				{
					status = true;
#ifdef RTT_OUT
					uint8_t ind = 0;
					uint16_t sysms = SimpleOS_GetTick();
					if (sysms < 10)
					{
						RTT_PRINT_1("(0000%d) ", sysms);
					}
					else if (sysms < 100)
					{
						RTT_PRINT_1("(000%d) ", sysms);
					}
					else if (sysms < 1000)
					{
						RTT_PRINT_1("(00%d) ", sysms);
					}
					else if (sysms < 10000)
					{
						RTT_PRINT_1("(0%d) ", sysms);
					}
					else
					{
						RTT_PRINT_1("(%d) ", sysms);
					}
					RTT_PRINT_1("Rx: ");
					for (ind = 0; ind < blep_Rx_Ind; ind++)
					{
						if (blep_Rx_Buf[ind] < 0x10)
						{
							RTT_PRINT_1("0%x ", blep_Rx_Buf[ind]);
						}
						else
						{
							RTT_PRINT_1("%x ", blep_Rx_Buf[ind]);
						}
					}
					RTT_PRINT_1("\n");
#endif
				}
				else
				{
					status = false;
				}
				blep_Rx_Ind = 0;
				blep_Rx_Checksum = 0;
			}
			else
			{
				/* nothing */
			}
		}
	}
	return status;
}

void BLEP_Create_AdvPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_LOCALNAME_ID};
	uint8_t len[] = {SCCU2BLE_LOCALNAME_LEN};
	uint8_t *dataPtr[] = {SCCU2BLE_LOCALNAME_PTR};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_LOCALNAME_HEAD, 0x01, id, len, dataPtr, advPackage_crc);
	*buffer = blep_Tx_Buf;
	advPackage_crc = advPackage_crc + 1;
}

void BLEP_Create_CanIDPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_CANID_G1_ID,  SCCU2BLE_CANID_G2_ID,  SCCU2BLE_CANID_G3_ID,  SCCU2BLE_CANID_G4_ID,
					 SCCU2BLE_CANID_G5_ID,  SCCU2BLE_CANID_G6_ID,  SCCU2BLE_CANID_G7_ID,  SCCU2BLE_CANID_G8_ID,
					 SCCU2BLE_CANID_G9_ID,  SCCU2BLE_CANID_G10_ID, SCCU2BLE_CANID_G11_ID, SCCU2BLE_CANID_G12_ID,
					 SCCU2BLE_CANID_G13_ID, SCCU2BLE_CANID_G14_ID, SCCU2BLE_CANID_G15_ID, SCCU2BLE_CANID_G16_ID,
					 SCCU2BLE_CANID_G17_ID, SCCU2BLE_CANID_G18_ID, SCCU2BLE_CANID_G19_ID, SCCU2BLE_CANID_G20_ID,
					 SCCU2BLE_CANID_G21_ID, SCCU2BLE_CANID_G22_ID, SCCU2BLE_CANID_G23_ID, SCCU2BLE_CANID_G24_ID};
	uint8_t len[] = {SCCU2BLE_CANID_G1_LEN,  SCCU2BLE_CANID_G2_LEN,  SCCU2BLE_CANID_G3_LEN,  SCCU2BLE_CANID_G4_LEN,
					 SCCU2BLE_CANID_G5_LEN,  SCCU2BLE_CANID_G6_LEN,  SCCU2BLE_CANID_G7_LEN,  SCCU2BLE_CANID_G8_LEN,
					 SCCU2BLE_CANID_G9_LEN,  SCCU2BLE_CANID_G10_LEN, SCCU2BLE_CANID_G11_LEN, SCCU2BLE_CANID_G12_LEN,
					 SCCU2BLE_CANID_G13_LEN, SCCU2BLE_CANID_G14_LEN, SCCU2BLE_CANID_G15_LEN, SCCU2BLE_CANID_G16_LEN,
					 SCCU2BLE_CANID_G17_LEN, SCCU2BLE_CANID_G18_LEN, SCCU2BLE_CANID_G19_LEN, SCCU2BLE_CANID_G20_LEN,
					 SCCU2BLE_CANID_G21_LEN, SCCU2BLE_CANID_G22_LEN, SCCU2BLE_CANID_G23_LEN, SCCU2BLE_CANID_G24_LEN};
	uint8_t *dataPtr[] = {SCCU2BLE_CANID_G1_PTR,  SCCU2BLE_CANID_G2_PTR,  SCCU2BLE_CANID_G3_PTR,  SCCU2BLE_CANID_G4_PTR,
						  SCCU2BLE_CANID_G5_PTR,  SCCU2BLE_CANID_G6_PTR,  SCCU2BLE_CANID_G7_PTR,  SCCU2BLE_CANID_G8_PTR,
						  SCCU2BLE_CANID_G9_PTR,  SCCU2BLE_CANID_G10_PTR, SCCU2BLE_CANID_G11_PTR, SCCU2BLE_CANID_G12_PTR,
						  SCCU2BLE_CANID_G13_PTR, SCCU2BLE_CANID_G14_PTR, SCCU2BLE_CANID_G15_PTR, SCCU2BLE_CANID_G16_PTR,
						  SCCU2BLE_CANID_G17_PTR, SCCU2BLE_CANID_G18_PTR, SCCU2BLE_CANID_G19_PTR, SCCU2BLE_CANID_G20_PTR,
						  SCCU2BLE_CANID_G21_PTR, SCCU2BLE_CANID_G22_PTR, SCCU2BLE_CANID_G23_PTR, SCCU2BLE_CANID_G24_PTR};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_CANID_HEAD, 24, id, len, dataPtr, canIDPackage_crc);
	*buffer = blep_Tx_Buf;
	canIDPackage_crc = canIDPackage_crc + 1;
}

void BLEP_Create_CommondIDPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint32_t injection_Val = CAN_Get_Injection_Val();
	uint8_t injection_Data[4];
	injection_Data[0] = (uint8_t)((injection_Val & (uint32_t)0xFF000000) >> (uint32_t)24);
	injection_Data[1] = (uint8_t)((injection_Val & (uint32_t)0x00FF0000) >> (uint32_t)16);
	injection_Data[2] = (uint8_t)((injection_Val & (uint32_t)0x0000FF00) >> (uint32_t)8);
	injection_Data[3] = (uint8_t)(injection_Val & (uint32_t)0x000000FF);

	uint32_t diff_ODO_Val = DIAG_Get_Accumulation_ODO();
	uint8_t diff_ODO_Data[4];

	Eeprom_ReqBackupData();

	diff_ODO_Data[0] = (uint8_t)((diff_ODO_Val & (uint32_t)0xFF000000) >> (uint32_t)24);
	diff_ODO_Data[1] = (uint8_t)((diff_ODO_Val & (uint32_t)0x00FF0000) >> (uint32_t)16);
	diff_ODO_Data[2] = (uint8_t)((diff_ODO_Val & (uint32_t)0x0000FF00) >> (uint32_t)8);
	diff_ODO_Data[3] = (uint8_t)(diff_ODO_Val & (uint32_t)0x000000FF);

	uint16_t id[] = {SCCU2BLE_COMMONDID_G1_ID, SCCU2BLE_COMMONDID_G2_ID,
					 SCCU2BLE_COMMONDID_G3_ID, SCCU2BLE_COMMONDID_G4_ID,
					 SCCU2BLE_COMMONDID_G5_ID, SCCU2BLE_COMMONDID_G6_ID,
					 SCCU2BLE_COMMONDID_G7_ID, SCCU2BLE_COMMONDID_G8_ID,
					 SCCU2BLE_COMMONDID_G9_ID};
	uint8_t len[] = {SCCU2BLE_COMMONDID_G1_LEN, SCCU2BLE_COMMONDID_G2_LEN,
					 SCCU2BLE_COMMONDID_G3_LEN, SCCU2BLE_COMMONDID_G4_LEN,
					 SCCU2BLE_COMMONDID_G5_LEN, SCCU2BLE_COMMONDID_G6_LEN,
					 SCCU2BLE_COMMONDID_G7_LEN, SCCU2BLE_COMMONDID_G8_LEN,
					 SCCU2BLE_COMMONDID_G9_LEN};
	uint8_t *dataPtr[] = {SCCU2BLE_COMMONDID_G1_PTR, SCCU2BLE_COMMONDID_G2_PTR,
						  SCCU2BLE_COMMONDID_G3_PTR, SCCU2BLE_COMMONDID_G4_PTR,
						  SCCU2BLE_COMMONDID_G5_PTR, SCCU2BLE_COMMONDID_G6_PTR,
						  SCCU2BLE_COMMONDID_G7_PTR, injection_Data,
						  diff_ODO_Data};

	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_COMMONDID_HEAD, 0x09, id, len, dataPtr, commandIDPackage_crc);
	*buffer = blep_Tx_Buf;
	commandIDPackage_crc = commandIDPackage_crc + 1;
}

void BLEP_Create_FirmwarePackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_READFW_ID};
	uint8_t len[] = {0};
	uint8_t data[] = {0};
	uint8_t *dataPtr[] = {data};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_READFW_HEAD, 0x01, id, len, dataPtr, firmwarePackage_crc);
	*buffer = blep_Tx_Buf;
	firmwarePackage_crc = firmwarePackage_crc + 1;
}

void BLEP_Create_ProcessPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_PROCESS_ID};
	uint8_t len[] = {SCCU2BLE_PROCESS_LEN};
	uint8_t *dataPtr[] = {SCCU2BLE_PROCESS_PTR};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_PROCESS_HEAD, 0x01, id, len, dataPtr, processPackage_crc);
	*buffer = blep_Tx_Buf;
	processPackage_crc = processPackage_crc + 1;
}

void BLEP_Create_H0148Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_H0148_01_ID, SCCU2BLE_H0148_48_ID,
					 SCCU2BLE_H0148_Op1_ID, SCCU2BLE_H0148_Op2_ID,
					 SCCU2BLE_H0148_Op3_ID};
	uint8_t len[] = {SCCU2BLE_H0148_01_LEN, SCCU2BLE_H0148_48_LEN,
					 SCCU2BLE_H0148_Op1_LEN, SCCU2BLE_H0148_Op2_LEN,
					 SCCU2BLE_H0148_Op3_LEN};
	uint8_t *dataPtr[] = {SCCU2BLE_H0148_01_PTR, SCCU2BLE_H0148_48_PTR,
						  SCCU2BLE_H0148_Op1_PTR, SCCU2BLE_H0148_Op2_PTR,
						  SCCU2BLE_H0148_Op3_PTR};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_H0148_HEAD, 0x05, id, len, dataPtr, h0148Package_crc);
	*buffer = blep_Tx_Buf;
	*dataPtr[2] = 0xFF;
	*dataPtr[3] = 0xFF;
	*dataPtr[4] = 0xFF;
	h0148Package_crc = h0148Package_crc + 1;
}

void BLEP_Create_ShutdownPackage(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {SCCU2BLE_SHUTDOWN_ID};
	uint8_t len[] = {0};
	uint8_t data[] = {0};
	uint8_t *dataPtr[] = {data};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_SHUTDOWN_HEAD, 0x01, id, len, dataPtr, shutdownPackage_crc);
	*buffer = blep_Tx_Buf;
	shutdownPackage_crc = shutdownPackage_crc + 1;
}

void BLEP_Create_FFD01Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {info->id};
	uint8_t len[] = {info->len};
	uint8_t *dataPtr[] = {info->dataPtr};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_FFD_HEAD, 0x01, id, len, dataPtr, ffd01Package_crc);
	*buffer = blep_Tx_Buf;
	ffd01Package_crc = ffd01Package_crc + 1;
}

void BLEP_Create_FFD02Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {info->id};
	uint8_t len[] = {info->len};
	uint8_t *dataPtr[] = {info->dataPtr};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_FFD_HEAD, 0x01, id, len, dataPtr, ffd02Package_crc);
	*buffer = blep_Tx_Buf;
	ffd02Package_crc = ffd02Package_crc + 1;
}

void BLEP_Create_FFD03Package(uint8_t **buffer, uint8_t *size, BLEP_PackageInfoTypedef *info)
{
	uint16_t id[] = {info->id};
	uint8_t len[] = {info->len};
	uint8_t *dataPtr[] = {info->dataPtr};
	*size = BLEP_Set_Package((uint8_t)SCCU2BLE_FFD_HEAD, 0x01, id, len, dataPtr, ffd03Package_crc);
	*buffer = blep_Tx_Buf;
	ffd03Package_crc = ffd03Package_crc + 1;
}

void BLEP_Reset_Tx_Package(void)
{
	blep_Firmware_Package.newData = false;
	blep_Shutdown_Package.newData = false;
	blep_FF01_Package.newData = false;
	blep_FF02_Package.newData = false;
	blep_FF03_Package.newData = false;
}

void BLEP_Reset_Shutdown_Routine(void)
{
	blep_Shutdown_Package.newData = false;
}

static void BLEP_Handle_Road_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_RoadPackageTypedef *ptr = (BLEP_RoadPackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].instruction, data_Ptr[BLE2SCCU_NAVI_INFO4_INDEX], data_Len[BLE2SCCU_NAVI_INFO4_INDEX]);
}

static void BLEP_Handle_Road_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_RoadPackageTypedef *ptr = (BLEP_RoadPackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[BLE2SCCU_NAVI_INFO4_INDEX], ptr[storage_Cnt].instruction, data_Len[BLE2SCCU_NAVI_INFO4_INDEX]);
}

static void BLEP_Handle_SNS_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_SNSPackageTypedef *ptr = (BLEP_SNSPackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].title, data_Ptr[BLE2SCCU_SNS_TITLE_INDEX], data_Len[BLE2SCCU_SNS_TITLE_INDEX]);
	memcpy(ptr[storage_Cnt].text, data_Ptr[BLE2SCCU_SNS_TEXT_INDEX], data_Len[BLE2SCCU_SNS_TEXT_INDEX]);
	memcpy(ptr[storage_Cnt].sender, data_Ptr[BLE2SCCU_SNS_SENDER_INDEX], data_Len[BLE2SCCU_SNS_SENDER_INDEX]);
}

static void BLEP_Handle_SNS_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_SNSPackageTypedef *ptr = (BLEP_SNSPackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[BLE2SCCU_SNS_TITLE_INDEX], ptr[storage_Cnt].title, data_Len[BLE2SCCU_SNS_TITLE_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_SNS_TEXT_INDEX], ptr[storage_Cnt].text, data_Len[BLE2SCCU_SNS_TEXT_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_SNS_SENDER_INDEX], ptr[storage_Cnt].sender, data_Len[BLE2SCCU_SNS_SENDER_INDEX]);
}

static void BLEP_Handle_Music_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_MusicPackageTypedef *ptr = (BLEP_MusicPackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].aritst, data_Ptr[BLE2SCCU_MUSIC_ARITST_INDEX], data_Len[BLE2SCCU_MUSIC_ARITST_INDEX]);
	memcpy(ptr[storage_Cnt].album, data_Ptr[BLE2SCCU_MUSIC_ALBUM_INDEX], data_Len[BLE2SCCU_MUSIC_ALBUM_INDEX]);
	memcpy(ptr[storage_Cnt].title, data_Ptr[BLE2SCCU_MUSIC_TITLE_INDEX], data_Len[BLE2SCCU_MUSIC_TITLE_INDEX]);
}

static void BLEP_Handle_Music_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_MusicPackageTypedef *ptr = (BLEP_MusicPackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[BLE2SCCU_MUSIC_ARITST_INDEX], ptr[storage_Cnt].aritst, data_Len[BLE2SCCU_MUSIC_ARITST_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_MUSIC_ALBUM_INDEX], ptr[storage_Cnt].album, data_Len[BLE2SCCU_MUSIC_ALBUM_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_MUSIC_TITLE_INDEX], ptr[storage_Cnt].title, data_Len[BLE2SCCU_MUSIC_TITLE_INDEX]);
}

static void BLEP_Handle_Phone_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_PhonePackageTypedef *ptr = (BLEP_PhonePackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].number, data_Ptr[BLE2SCCU_PHONE_NUMBER_INDEX], data_Len[BLE2SCCU_PHONE_NUMBER_INDEX]);
	memcpy(ptr[storage_Cnt].caller, data_Ptr[BLE2SCCU_PHONE_CALLER_INDEX], data_Len[BLE2SCCU_PHONE_CALLER_INDEX]);
}

static void BLEP_Handle_Phone_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_PhonePackageTypedef *ptr = (BLEP_PhonePackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[BLE2SCCU_PHONE_NUMBER_INDEX], ptr[storage_Cnt].number, data_Len[BLE2SCCU_PHONE_NUMBER_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_PHONE_CALLER_INDEX], ptr[storage_Cnt].caller, data_Len[BLE2SCCU_PHONE_CALLER_INDEX]);
}

static void BLEP_Handle_PassThrough_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_PassThroughPackageTypedef *ptr = (BLEP_PassThroughPackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].data, data_Ptr[0], data_Len[0]);
}

static void BLEP_Handle_PassThrough_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_PassThroughPackageTypedef *ptr = (BLEP_PassThroughPackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[0], ptr[storage_Cnt].data, data_Len[0]);
}

static void BLEP_Handle_Authentication_Queue_Push(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_AuthenticationPackageTypedef *ptr = (BLEP_AuthenticationPackageTypedef *)storage_Ptr;

	memcpy(ptr[storage_Cnt].localName, data_Ptr[BLE2SCCU_AUTH_LOCALNAME_INDEX], data_Len[BLE2SCCU_AUTH_LOCALNAME_INDEX]);
	memcpy(ptr[storage_Cnt].passKey, data_Ptr[BLE2SCCU_AUTH_PASSKEY_INDEX], data_Len[BLE2SCCU_AUTH_PASSKEY_INDEX]);
	memcpy(ptr[storage_Cnt].uuid, data_Ptr[BLE2SCCU_AUTH_UUID_INDEX], data_Len[BLE2SCCU_AUTH_UUID_INDEX]);
	memcpy(ptr[storage_Cnt].bondFlag, data_Ptr[BLE2SCCU_AUTH_BONDFLAG_INDEX], data_Len[BLE2SCCU_AUTH_BONDFLAG_INDEX]);
}

static void BLEP_Handle_Authentication_Queue_Pop(uint8_t **data_Ptr, uint8_t *data_Len, void *storage_Ptr, uint8_t storage_Cnt)
{
	BLEP_AuthenticationPackageTypedef *ptr = (BLEP_AuthenticationPackageTypedef *)storage_Ptr;

	memcpy(data_Ptr[BLE2SCCU_AUTH_LOCALNAME_INDEX], ptr[storage_Cnt].localName, data_Len[BLE2SCCU_AUTH_LOCALNAME_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_AUTH_PASSKEY_INDEX], ptr[storage_Cnt].passKey, data_Len[BLE2SCCU_AUTH_PASSKEY_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_AUTH_UUID_INDEX], ptr[storage_Cnt].uuid, data_Len[BLE2SCCU_AUTH_UUID_INDEX]);
	memcpy(data_Ptr[BLE2SCCU_AUTH_BONDFLAG_INDEX], ptr[storage_Cnt].bondFlag, data_Len[BLE2SCCU_AUTH_BONDFLAG_INDEX]);
}

/// @brief Procse id 0x58E(title), 0x58F(text), 0x591(sender)
/// @param data_Array Data point array
/// @param len_Array Len array
static void BLEP_Handle_SNS_Info(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t hash_Tmp = 0;
	uint8_t len_Tmp = 0;
	uint8_t data_Cnt = 0;

	while (data_Cnt < BLE2SCCU_SNS_COUNT)
	{
		switch (data_Cnt)
		{
		case BLE2SCCU_SNS_TITLE_INDEX:
			len_Tmp = BLE2SCCU_SNS_TITLE_LEN;
			break;
		case BLE2SCCU_SNS_TEXT_INDEX:
			len_Tmp = BLE2SCCU_SNS_TEXT_LEN;
			break;
		case BLE2SCCU_SNS_SENDER_INDEX:
			len_Tmp = BLE2SCCU_SNS_SENDER_LEN;
			break;
		default:
			len_Tmp = BLE2SCCU_SNS_TITLE_LEN;
			break;
		}

		if (data_Len[data_Cnt] == len_Tmp)
		{
			hash_Tmp = BLEP_Calc_Hash(data_Ptr[data_Cnt], data_Len[data_Cnt]);
			hash = BLEP_Hash_Combine(hash, hash_Tmp);
			data_Cnt++;
		}
		else
		{
			break;
		}
	}

	if (data_Cnt == BLE2SCCU_SNS_COUNT)
	{
		(void)BLEP_QueuePush(&blep_SNS_Queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse id 0x59A(aritst), 0x59B(album), 0x59C(title)
/// @param data_Array Data point array
/// @param len_Array Len array
static void BLEP_Handle_Music_Info(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t hash_Tmp = 0;
	uint8_t len_Tmp = 0;
	uint8_t data_Cnt = 0;

	while (data_Cnt < BLE2SCCU_MUSIC_COUNT)
	{
		switch (data_Cnt)
		{
		case BLE2SCCU_MUSIC_ARITST_INDEX:
			len_Tmp = BLE2SCCU_MUSIC_ARITST_LEN;
			break;
		case BLE2SCCU_MUSIC_ALBUM_INDEX:
			len_Tmp = BLE2SCCU_MUSIC_ALBUM_LEN;
			break;
		case BLE2SCCU_MUSIC_TITLE_INDEX:
			len_Tmp = BLE2SCCU_MUSIC_TITLE_LEN;
			break;
		default:
			len_Tmp = BLE2SCCU_MUSIC_ARITST_LEN;
			break;
		}

		if (data_Len[data_Cnt] == len_Tmp)
		{
			hash_Tmp = BLEP_Calc_Hash(data_Ptr[data_Cnt], data_Len[data_Cnt]);
			hash = BLEP_Hash_Combine(hash, hash_Tmp);
			data_Cnt++;
		}
		else
		{
			break;
		}
	}

	if (data_Cnt == BLE2SCCU_MUSIC_COUNT)
	{
		(void)BLEP_QueuePush(&blep_Music_Queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse id 0x58C(number), 0x58D(caller)
/// @param data_Array Data point array
/// @param len_Array Len array
static void BLEP_Handle_Phone_Info(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t hash_Tmp = 0;
	uint8_t len_Tmp = 0;
	uint8_t data_Cnt = 0;

	while (data_Cnt < BLE2SCCU_PHONE_COUNT)
	{
		switch (data_Cnt)
		{
		case BLE2SCCU_PHONE_NUMBER_INDEX:
			len_Tmp = BLE2SCCU_PHONE_NUMBER_LEN;
			break;
		case BLE2SCCU_PHONE_CALLER_INDEX:
			len_Tmp = BLE2SCCU_PHONE_CALLER_LEN;
			break;
		default:
			len_Tmp = BLE2SCCU_PHONE_NUMBER_LEN;
			break;
		}

		if (data_Len[data_Cnt] == len_Tmp)
		{
			hash_Tmp = BLEP_Calc_Hash(data_Ptr[data_Cnt], data_Len[data_Cnt]);
			hash = BLEP_Hash_Combine(hash, hash_Tmp);
			data_Cnt++;
		}
		else
		{
			break;
		}
	}

	if (data_Cnt == BLE2SCCU_PHONE_COUNT)
	{
		(void)BLEP_QueuePush(&blep_Phone_Queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse id 0x592(navi_index), 0x595(instruction)
/// @param data_Ptr Data point array
/// @param data_Len Len array
static void BLEP_Handle_Road_Info(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t tmp_hash = 0;
	uint8_t tmp_Len = 0;
	uint8_t data_Cnt = 0;

	while (data_Cnt < BLE2SCCU_ROAD_COUNT)
	{
		switch (data_Cnt)
		{
		case BLE2SCCU_NAVI_INFO1_INDEX:
			tmp_Len = 1;
			break;
		case BLE2SCCU_NAVI_INFO4_INDEX:
			tmp_Len = BLE2SCCU_NAVI_INFO4_LEN;
			break;
		default:
			tmp_Len = 1;
			break;
		}

		if (data_Len[data_Cnt] == tmp_Len)
		{
			tmp_hash = BLEP_Calc_Hash(data_Ptr[data_Cnt], data_Len[data_Cnt]);
			hash = BLEP_Hash_Combine(hash, tmp_hash);
			data_Cnt++;
		}
		else
		{
			break;
		}
	}

	if (data_Cnt == BLE2SCCU_ROAD_COUNT)
	{
		(void)BLEP_QueuePush(&blep_Road_Queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse PassThrough data
/// @param data_Ptr Data point array
/// @param data_Len Len array
static void BLEP_Handle_PassThrough_Info(uint8_t **data_Ptr, uint8_t *data_Len, BLEP_MessageQueueTypedef *queue)
{
	uint32_t hash = 0;
	uint32_t tmp_hash = 0;

	if (data_Len[0] == 8)
	{
		tmp_hash = BLEP_Calc_Hash(data_Ptr[0], data_Len[0]);
		hash = BLEP_Hash_Combine(hash, tmp_hash);

		(void)BLEP_QueuePush(queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse Authentication data
/// @param data_Ptr Data point array
/// @param data_Len Len array
static void BLEP_Handle_Authentication_Info(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t tmp_hash = 0;
	uint8_t tmp_Len = 0;
	uint8_t data_Cnt = 0;

	while (data_Cnt < BLE2SCCU_AUTH_COUNT)
	{
		switch (data_Cnt)
		{
		case BLE2SCCU_AUTH_LOCALNAME_INDEX:
			tmp_Len = BLE2SCCU_AUTH_LOCALNAME_LEN;
			break;
		case BLE2SCCU_AUTH_PASSKEY_INDEX:
			tmp_Len = BLE2SCCU_AUTH_PASSKEY_LEN;
			break;
		case BLE2SCCU_AUTH_UUID_INDEX:
			tmp_Len = BLE2SCCU_AUTH_UUID_LEN;
			break;
		case BLE2SCCU_AUTH_BONDFLAG_INDEX:
			tmp_Len = BLE2SCCU_AUTH_BONDFLAG_LEN;
			break;
		default:
			tmp_Len = BLE2SCCU_AUTH_LOCALNAME_LEN;
			break;
		}

		if (data_Len[data_Cnt] == tmp_Len)
		{
			tmp_hash = BLEP_Calc_Hash(data_Ptr[data_Cnt], data_Len[data_Cnt]);
			hash = BLEP_Hash_Combine(hash, tmp_hash);
			data_Cnt++;
		}
		else
		{
			break;
		}
	}

	if (data_Cnt == BLE2SCCU_AUTH_COUNT)
	{
		(void)BLEP_QueuePush(&blep_Authentication_Queue, data_Ptr, data_Len, hash);
	}
	else
	{
		/* nothing */
	}
}

static void BLEP_Handle_7F03(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)BLE2SCCU_READFW_LEN)
	{
		(void)memcpy(blep_Firmware_Package.Data.ALL, data, (size_t)BLE2SCCU_READFW_LEN);
		blep_Firmware_Package.newData = true;
	}
	else
	{
		/* ignore */
	}
}

static void BLEP_Handle_6FFF(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)BLE2SCCU_SHUTDOWN_LEN)
	{
		blep_Shutdown_Package.newData = true;
	}
	else
	{
		/* ignore */
	}
}

/// @brief Procse id 0x58A(time) data
/// @param data_Ptr Data point array
/// @param data_Len Len array
static void BLEP_Handle_058A(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t tmp_hash = 0;

	if (data_Len[0] == 8)
	{
		tmp_hash = BLEP_Calc_Hash(data_Ptr[0], data_Len[0]);
		hash = BLEP_Hash_Combine(hash, tmp_hash);

		if (hash != 0x63852afc)
		{
			(void)BLEP_QueuePush(&blep_H58A_Queue, data_Ptr, data_Len, hash);
		}
		else
		{
			/* nothing */
		}
	}
	else
	{
		/* nothing */
	}
}

/// @brief Procse id 0x58B(status) data
/// @param data_Ptr Data point array
/// @param data_Len Len array
static void BLEP_Handle_058B(uint8_t **data_Ptr, uint8_t *data_Len)
{
	uint32_t hash = 0;
	uint32_t tmp_hash = 0;

	if (data_Len[0] == 8)
	{
		tmp_hash = BLEP_Calc_Hash(data_Ptr[0], data_Len[0]);
		hash = BLEP_Hash_Combine(hash, tmp_hash);

		if (hash != 0x63852afc)
		{
			(void)BLEP_QueuePush(&blep_H58B_Queue, data_Ptr, data_Len, hash);
		}
		else
		{
			/* nothing */
		}
	}
	else
	{
		/* nothing */
	}
}

static void BLEP_Handle_FF01(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)BLE2SCCU_FFD01_LEN)
	{
		blep_FF01_Package.newData = true;
	}
	else
	{
		/* ignore */
	}
}

static void BLEP_Handle_FF02(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)BLE2SCCU_FFD02_LEN)
	{
		blep_FF02_Package.newData = true;
	}
	else
	{
		/* ignore */
	}
}

static void BLEP_Handle_FF03(uint8_t *data, uint8_t len)
{
	if (len == (uint8_t)BLE2SCCU_FFD03_LEN)
	{
		blep_FF03_Package.newData = true;
	}
	else
	{
		/* ignore */
	}
}

/*========================================================================================*/

static ModuleState_TypeDef BLEPModule_state = MODULESTATE_RESET;

void BLEP_init(void)
{
	BLEPModule_state = MODULESTATE_INITIALIZED;

	/* Write the module initialize code here	*/
	BLEP_Reset_Tx_Package();
	BLEP_Reset_Rx_Package();

	BLEP_QueueInitial(&blep_Authentication_Queue, &blep_Authentication_Package, 1, BLEP_Handle_Authentication_Queue_Push, BLEP_Handle_Authentication_Queue_Pop, 0, true, false, false);

	BLEP_QueueInitial(&blep_H58A_Queue, &blep_H58A_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
	BLEP_QueueInitial(&blep_H58B_Queue, &blep_H58B_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
	BLEP_QueueInitial(&blep_Navi_Info1_Queue, &blep_Navi_Info1_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
	BLEP_QueueInitial(&blep_Navi_Info2_Queue, &blep_Navi_Info2_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
	BLEP_QueueInitial(&blep_Navi_Info3_Queue, &blep_Navi_Info3_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
	BLEP_QueueInitial(&blep_Voice_Ctrl_Queue, &blep_Voice_Ctrl_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
	BLEP_QueueInitial(&blep_Setting_1_Queue, &blep_Setting_1_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
	BLEP_QueueInitial(&blep_Setting_2_Queue, &blep_Setting_2_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);

	BLEP_QueueInitial(&blep_Road_Queue, &blep_Road_Package, 1, BLEP_Handle_Road_Queue_Push, BLEP_Handle_Road_Queue_Pop, 0, true, false, true);
	BLEP_QueueInitial(&blep_SNS_Queue, &blep_SNS_Package, 2, BLEP_Handle_SNS_Queue_Push, BLEP_Handle_SNS_Queue_Pop, 0, false, true, true);
	BLEP_QueueInitial(&blep_Phone_Queue, &blep_Phone_Package, 1, BLEP_Handle_Phone_Queue_Push, BLEP_Handle_Phone_Queue_Pop, 0, true, false, true);
	BLEP_QueueInitial(&blep_Music_Queue, &blep_Music_Package, 1, BLEP_Handle_Music_Queue_Push, BLEP_Handle_Music_Queue_Pop, 0, true, false, true);
}

void BLEP_open(void)
{
	if (BLEPModule_state != MODULESTATE_OPENED)
	{
		BLEPModule_state = MODULESTATE_OPENED;
		/* Write the module open code here	*/
		BLEP_Reset_Tx_Package();
		BLEP_Reset_Rx_Package();
		advPackage_crc = 0;
		canIDPackage_crc = 0;
		commandIDPackage_crc = 0;
		firmwarePackage_crc = 0;
		shutdownPackage_crc = 0;
		processPackage_crc = 0;
		h0148Package_crc = 0;
		ffd01Package_crc = 0;
		ffd02Package_crc = 0;
		ffd03Package_crc = 0;

		BLEP_QueueInitial(&blep_Authentication_Queue, &blep_Authentication_Package, 1, BLEP_Handle_Authentication_Queue_Push, BLEP_Handle_Authentication_Queue_Pop, 0, true, false, false);

		BLEP_QueueInitial(&blep_H58A_Queue, &blep_H58A_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
		BLEP_QueueInitial(&blep_H58B_Queue, &blep_H58B_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
		BLEP_QueueInitial(&blep_Navi_Info1_Queue, &blep_Navi_Info1_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
		BLEP_QueueInitial(&blep_Navi_Info2_Queue, &blep_Navi_Info2_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
		BLEP_QueueInitial(&blep_Navi_Info3_Queue, &blep_Navi_Info3_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
		BLEP_QueueInitial(&blep_Voice_Ctrl_Queue, &blep_Voice_Ctrl_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, true);
		BLEP_QueueInitial(&blep_Setting_1_Queue, &blep_Setting_1_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);
		BLEP_QueueInitial(&blep_Setting_2_Queue, &blep_Setting_2_Package, 1, BLEP_Handle_PassThrough_Queue_Push, BLEP_Handle_PassThrough_Queue_Pop, 0x63852afc, true, false, false);

		BLEP_QueueInitial(&blep_Road_Queue, &blep_Road_Package, 1, BLEP_Handle_Road_Queue_Push, BLEP_Handle_Road_Queue_Pop, 0, true, false, true);
		BLEP_QueueInitial(&blep_SNS_Queue, &blep_SNS_Package, 2, BLEP_Handle_SNS_Queue_Push, BLEP_Handle_SNS_Queue_Pop, 0, false, true, true);
		BLEP_QueueInitial(&blep_Phone_Queue, &blep_Phone_Package, 1, BLEP_Handle_Phone_Queue_Push, BLEP_Handle_Phone_Queue_Pop, 0, true, false, true);
		BLEP_QueueInitial(&blep_Music_Queue, &blep_Music_Package, 1, BLEP_Handle_Music_Queue_Push, BLEP_Handle_Music_Queue_Pop, 0, true, false, true);
	}
	else
	{
	}
}

void BLEP_close(void)
{
	if (BLEPModule_state != MODULESTATE_CLOSED)
	{
		BLEPModule_state = MODULESTATE_CLOSED;
		/* Write the module close code here	*/
		BLEP_Reset_Tx_Package();
		BLEP_Reset_Rx_Package();

		BLEP_QueueReset(&blep_Authentication_Queue);

		BLEP_QueueReset(&blep_H58A_Queue);
		BLEP_QueueReset(&blep_H58B_Queue);
		BLEP_QueueReset(&blep_Navi_Info1_Queue);
		BLEP_QueueReset(&blep_Navi_Info2_Queue);
		BLEP_QueueReset(&blep_Navi_Info3_Queue);
		BLEP_QueueReset(&blep_Voice_Ctrl_Queue);
		BLEP_QueueReset(&blep_Setting_1_Queue);
		BLEP_QueueReset(&blep_Setting_2_Queue);

		BLEP_QueueReset(&blep_Road_Queue);
		BLEP_QueueReset(&blep_SNS_Queue);
		BLEP_QueueReset(&blep_Phone_Queue);
		BLEP_QueueReset(&blep_Music_Queue);
	}
	else
	{
	}
}

void BLEP_flush(void)
{
	bool tmp_Result;
	uint8_t ii = 0;
	if (BLEPModule_state == MODULESTATE_OPENED)
	{
		tmp_Result = USART_ReadData(&blep_Rx_Data);
		if (tmp_Result == true)
		{
			blep_Rx_Reset_Time = SimpleOS_GetTick();
			tmp_Result = BLEP_Analyze_Package(blep_Rx_Data);
			if (tmp_Result == true)
			{
				blep_SNS_Cnt = 0;
				blep_SNS_Sequence = 0;
				blep_Music_Cnt = 0;
				blep_Phone_Cnt = 0;
				blep_Road_Cnt = 0;

				for (ii = 0; ii < blep_Rx_GroupMax; ii++)
				{
					if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_READFW_ID)
					{
						BLEP_Handle_7F03(blep_Rx_Group_Buf_Ptr[ii], blep_Rx_Group_Len[ii]);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SHUTDOWN_ID)
					{
						BLEP_Handle_6FFF(blep_Rx_Group_Buf_Ptr[ii], blep_Rx_Group_Len[ii]);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_AUTHENTICATION_ID)
					{
						if (blep_Rx_Group_Len[ii] == BLE2SCCU_AUTHENTICATION_LEN)
						{
							blep_Authentication_Offset = 0;

							blep_Authentication_DataPtr[BLE2SCCU_AUTH_LOCALNAME_INDEX] = (uint8_t *)(blep_Rx_Group_Buf_Ptr[ii] + blep_Authentication_Offset);
							blep_Authentication_Len[BLE2SCCU_AUTH_LOCALNAME_INDEX] = BLE2SCCU_AUTH_LOCALNAME_LEN;
							blep_Authentication_Offset += BLE2SCCU_AUTH_LOCALNAME_LEN;

							blep_Authentication_DataPtr[BLE2SCCU_AUTH_PASSKEY_INDEX] = (uint8_t *)(blep_Rx_Group_Buf_Ptr[ii] + blep_Authentication_Offset);
							blep_Authentication_Len[BLE2SCCU_AUTH_PASSKEY_INDEX] = BLE2SCCU_AUTH_PASSKEY_LEN;
							blep_Authentication_Offset += BLE2SCCU_AUTH_PASSKEY_LEN;

							blep_Authentication_DataPtr[BLE2SCCU_AUTH_UUID_INDEX] = (uint8_t *)(blep_Rx_Group_Buf_Ptr[ii] + blep_Authentication_Offset);
							blep_Authentication_Len[BLE2SCCU_AUTH_UUID_INDEX] = BLE2SCCU_AUTH_UUID_LEN;
							blep_Authentication_Offset += BLE2SCCU_AUTH_UUID_LEN;

							blep_Authentication_DataPtr[BLE2SCCU_AUTH_BONDFLAG_INDEX] = (uint8_t *)(blep_Rx_Group_Buf_Ptr[ii] + blep_Authentication_Offset);
							blep_Authentication_Len[BLE2SCCU_AUTH_BONDFLAG_INDEX] = BLE2SCCU_AUTH_BONDFLAG_LEN;

							BLEP_Handle_Authentication_Info(blep_Authentication_DataPtr, blep_Authentication_Len);
						}
						else
						{
							/* nothing */
						}
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_H58A_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						BLEP_Handle_058A(blep_PassThrough_DataPtr, blep_PassThrough_Len);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_H58B_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						BLEP_Handle_058B(blep_PassThrough_DataPtr, blep_PassThrough_Len);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_FFD01_ID)
					{
						BLEP_Handle_FF01(blep_Rx_Group_Buf_Ptr[ii], blep_Rx_Group_Len[ii]);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_FFD02_ID)
					{
						BLEP_Handle_FF02(blep_Rx_Group_Buf_Ptr[ii], blep_Rx_Group_Len[ii]);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_FFD03_ID)
					{
						BLEP_Handle_FF03(blep_Rx_Group_Buf_Ptr[ii], blep_Rx_Group_Len[ii]);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_NAVI_INFO1_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Navi_Info1_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);

						blep_Road_DataPtr[BLE2SCCU_NAVI_INFO1_INDEX] = (uint8_t *)(blep_Rx_Group_Buf_Ptr[ii] + 1);
						blep_Road_Len[BLE2SCCU_NAVI_INFO1_INDEX] = 1;
						blep_Road_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_NAVI_INFO2_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Navi_Info2_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_NAVI_INFO3_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Navi_Info3_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_NAVI_INFO4_ID)
					{
						blep_Road_DataPtr[BLE2SCCU_NAVI_INFO4_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Road_Len[BLE2SCCU_NAVI_INFO4_INDEX] = blep_Rx_Group_Len[ii];
						blep_Road_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_PHONE_NUMBER_ID)
					{
						blep_Phone_DataPtr[BLE2SCCU_PHONE_NUMBER_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Phone_Len[BLE2SCCU_PHONE_NUMBER_INDEX] = blep_Rx_Group_Len[ii];
						blep_Phone_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_PHONE_CALLER_ID)
					{
						blep_Phone_DataPtr[BLE2SCCU_PHONE_CALLER_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Phone_Len[BLE2SCCU_PHONE_CALLER_INDEX] = blep_Rx_Group_Len[ii];
						blep_Phone_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SNS_TITLE_ID)
					{
						blep_SNS_DataPtr[BLE2SCCU_SNS_TITLE_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_SNS_Len[BLE2SCCU_SNS_TITLE_INDEX] = blep_Rx_Group_Len[ii];
						if (blep_SNS_Cnt == 0)
						{
							blep_SNS_Sequence = *(blep_Rx_Group_Buf_Ptr[ii] + 0);
							blep_SNS_Cnt++;
						}
						else
						{
							if (blep_SNS_Sequence != *(blep_Rx_Group_Buf_Ptr[ii] + 0))
							{
								blep_SNS_Cnt = 0;
							}
							else
							{
								blep_SNS_Cnt++;
							}
						}
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SNS_TEXT_ID)
					{
						blep_SNS_DataPtr[BLE2SCCU_SNS_TEXT_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_SNS_Len[BLE2SCCU_SNS_TEXT_INDEX] = blep_Rx_Group_Len[ii];
						if (blep_SNS_Cnt == 0)
						{
							blep_SNS_Sequence = *(blep_Rx_Group_Buf_Ptr[ii] + 0);
							blep_SNS_Cnt++;
						}
						else
						{
							if (blep_SNS_Sequence != *(blep_Rx_Group_Buf_Ptr[ii] + 0))
							{
								blep_SNS_Cnt = 0;
							}
							else
							{
								blep_SNS_Cnt++;
							}
						}
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SNS_SENDER_ID)
					{
						blep_SNS_DataPtr[BLE2SCCU_SNS_SENDER_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_SNS_Len[BLE2SCCU_SNS_SENDER_INDEX] = blep_Rx_Group_Len[ii];
						if (blep_SNS_Cnt == 0)
						{
							blep_SNS_Sequence = *(blep_Rx_Group_Buf_Ptr[ii] + 0);
							blep_SNS_Cnt++;
						}
						else
						{
							if (blep_SNS_Sequence != *(blep_Rx_Group_Buf_Ptr[ii] + 0))
							{
								blep_SNS_Cnt = 0;
							}
							else
							{
								blep_SNS_Cnt++;
							}
						}
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_MUSIC_ARITST_ID)
					{
						blep_Music_DataPtr[BLE2SCCU_MUSIC_ARITST_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Music_Len[BLE2SCCU_MUSIC_ARITST_INDEX] = blep_Rx_Group_Len[ii];
						blep_Music_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_MUSIC_ALBUM_ID)
					{
						blep_Music_DataPtr[BLE2SCCU_MUSIC_ALBUM_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Music_Len[BLE2SCCU_MUSIC_ALBUM_INDEX] = blep_Rx_Group_Len[ii];
						blep_Music_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_MUSIC_TITLE_ID)
					{
						blep_Music_DataPtr[BLE2SCCU_MUSIC_TITLE_INDEX] = blep_Rx_Group_Buf_Ptr[ii];
						blep_Music_Len[BLE2SCCU_MUSIC_TITLE_INDEX] = blep_Rx_Group_Len[ii];
						blep_Music_Cnt++;
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_VOICE_CTRL_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Voice_Ctrl_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SETTING_1_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Setting_1_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);
					}
					else if (blep_Rx_Group_ID[ii] == (uint16_t)BLE2SCCU_SETTING_2_ID)
					{
						blep_PassThrough_DataPtr[0] = blep_Rx_Group_Buf_Ptr[ii];
						blep_PassThrough_Len[0] = blep_Rx_Group_Len[ii];
						blep_PassThrough_QueuePtr = &blep_Setting_2_Queue;
						BLEP_Handle_PassThrough_Info(blep_PassThrough_DataPtr, blep_PassThrough_Len, blep_PassThrough_QueuePtr);
					}
					else
					{
						/* nothing */
					}
				}

				if (blep_SNS_Cnt == BLE2SCCU_SNS_COUNT)
				{
					BLEP_Handle_SNS_Info(blep_SNS_DataPtr, blep_SNS_Len);
				}
				else
				{
					/* nothing */
				}

				if (blep_Music_Cnt == BLE2SCCU_MUSIC_COUNT)
				{
					BLEP_Handle_Music_Info(blep_Music_DataPtr, blep_Music_Len);
				}
				else
				{
					/* nothing */
				}

				if (blep_Phone_Cnt == BLE2SCCU_PHONE_COUNT)
				{
					BLEP_Handle_Phone_Info(blep_Phone_DataPtr, blep_Phone_Len);
				}
				else
				{
					/* nothing */
				}

				if (blep_Road_Cnt == BLE2SCCU_ROAD_COUNT)
				{
					BLEP_Handle_Road_Info(blep_Road_DataPtr, blep_Road_Len);
				}
				else
				{
					/* nothing */
				}
			}
		}
		else
		{
			tmp_Result = SimpleOS_CheckInterval(&blep_Rx_Reset_Time, (uint16_t)80);
			if (tmp_Result == true)
			{
				BLEP_Reset_Rx_Package();
			}
		}
	}
	else
	{
	}
}

bool BLEP_Get_Authentication(uint8_t *localName, uint8_t *passKey, uint8_t *uuid, uint8_t *bondFlag)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[BLE2SCCU_AUTH_COUNT];
	uint8_t tmp_Len[BLE2SCCU_AUTH_COUNT];

	if ((localName != 0) && (passKey != 0) && (uuid != 0) && (bondFlag != 0))
	{
		tmp_Ptr[BLE2SCCU_AUTH_LOCALNAME_INDEX] = localName;
		tmp_Len[BLE2SCCU_AUTH_LOCALNAME_INDEX] = BLE2SCCU_AUTH_LOCALNAME_LEN;

		tmp_Ptr[BLE2SCCU_AUTH_PASSKEY_INDEX] = passKey;
		tmp_Len[BLE2SCCU_AUTH_PASSKEY_INDEX] = BLE2SCCU_AUTH_PASSKEY_LEN;

		tmp_Ptr[BLE2SCCU_AUTH_UUID_INDEX] = uuid;
		tmp_Len[BLE2SCCU_AUTH_UUID_INDEX] = BLE2SCCU_AUTH_UUID_LEN;

		tmp_Ptr[BLE2SCCU_AUTH_BONDFLAG_INDEX] = bondFlag;
		tmp_Len[BLE2SCCU_AUTH_BONDFLAG_INDEX] = BLE2SCCU_AUTH_BONDFLAG_LEN;

		tmp_Result = BLEP_QueuePop(&blep_Authentication_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

BLEP_FirmwareTypedef *BLEP_Get_BleFirmware_Ptr(void)
{
	return &blep_Firmware_Package;
}

bool BLEP_Get_H58A(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_H58A_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_H58B(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_H58B_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Navi_Info1(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Navi_Info1_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Navi_Info2(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Navi_Info2_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Navi_Info3(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Navi_Info3_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Voice_Ctrl(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Voice_Ctrl_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Setting_1(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Setting_1_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Setting_2(uint8_t *data)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[1];
	uint8_t tmp_Len[1];

	if (data != 0)
	{
		tmp_Ptr[0] = data;
		tmp_Len[0] = 8;

		tmp_Result = BLEP_QueuePop(&blep_Setting_2_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Phone_Info(uint8_t *number, uint8_t *caller)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[BLE2SCCU_PHONE_COUNT];
	uint8_t tmp_Len[BLE2SCCU_PHONE_COUNT];

	if ((number != 0) && (caller != 0))
	{
		tmp_Ptr[BLE2SCCU_PHONE_NUMBER_INDEX] = number;
		tmp_Len[BLE2SCCU_PHONE_NUMBER_INDEX] = BLE2SCCU_PHONE_NUMBER_LEN;

		tmp_Ptr[BLE2SCCU_PHONE_CALLER_INDEX] = caller;
		tmp_Len[BLE2SCCU_PHONE_CALLER_INDEX] = BLE2SCCU_PHONE_CALLER_LEN;

		tmp_Result = BLEP_QueuePop(&blep_Phone_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Music_Info(uint8_t *aritst, uint8_t *album, uint8_t *title)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[BLE2SCCU_MUSIC_COUNT];
	uint8_t tmp_Len[BLE2SCCU_MUSIC_COUNT];

	if ((aritst != 0) && (album != 0) && (title != 0))
	{
		tmp_Ptr[BLE2SCCU_MUSIC_ARITST_INDEX] = aritst;
		tmp_Len[BLE2SCCU_MUSIC_ARITST_INDEX] = BLE2SCCU_MUSIC_ARITST_LEN;

		tmp_Ptr[BLE2SCCU_MUSIC_ALBUM_INDEX] = album;
		tmp_Len[BLE2SCCU_MUSIC_ALBUM_INDEX] = BLE2SCCU_MUSIC_ALBUM_LEN;

		tmp_Ptr[BLE2SCCU_MUSIC_TITLE_INDEX] = title;
		tmp_Len[BLE2SCCU_MUSIC_TITLE_INDEX] = BLE2SCCU_MUSIC_TITLE_LEN;

		tmp_Result = BLEP_QueuePop(&blep_Music_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_SNS_Info(uint8_t *title, uint8_t *text, uint8_t *sender)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[BLE2SCCU_SNS_COUNT];
	uint8_t tmp_Len[BLE2SCCU_SNS_COUNT];

	if ((title != 0) && (text != 0) && (sender != 0))
	{
		tmp_Ptr[BLE2SCCU_SNS_TITLE_INDEX] = title;
		tmp_Len[BLE2SCCU_SNS_TITLE_INDEX] = BLE2SCCU_SNS_TITLE_LEN;

		tmp_Ptr[BLE2SCCU_SNS_TEXT_INDEX] = text;
		tmp_Len[BLE2SCCU_SNS_TEXT_INDEX] = BLE2SCCU_SNS_TEXT_LEN;

		tmp_Ptr[BLE2SCCU_SNS_SENDER_INDEX] = sender;
		tmp_Len[BLE2SCCU_SNS_SENDER_INDEX] = BLE2SCCU_SNS_SENDER_LEN;

		tmp_Result = BLEP_QueuePop(&blep_SNS_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

bool BLEP_Get_Road_Info(uint8_t *instruction)
{
	bool tmp_Result = false;
	uint8_t *tmp_Ptr[BLE2SCCU_ROAD_COUNT];
	uint8_t tmp_Len[BLE2SCCU_ROAD_COUNT];

	if (instruction != 0)
	{
		tmp_Ptr[BLE2SCCU_NAVI_INFO4_INDEX] = instruction;
		tmp_Len[BLE2SCCU_NAVI_INFO4_INDEX] = BLE2SCCU_NAVI_INFO4_LEN;

		tmp_Result = BLEP_QueuePop(&blep_Road_Queue, tmp_Ptr, tmp_Len);
	}
	else
	{
		tmp_Result = false;
	}

	return tmp_Result;
}

BLEP_FFDTypedef *BLEP_Get_FF01_Ptr(void)
{
	return &blep_FF01_Package;
}

BLEP_FFDTypedef *BLEP_Get_FF02_Ptr(void)
{
	return &blep_FF02_Package;
}

BLEP_FFDTypedef *BLEP_Get_FF03_Ptr(void)
{
	return &blep_FF03_Package;
}

bool BLEP_Get_ShutdownFlag(void)
{
	return blep_Shutdown_Package.newData;
}
