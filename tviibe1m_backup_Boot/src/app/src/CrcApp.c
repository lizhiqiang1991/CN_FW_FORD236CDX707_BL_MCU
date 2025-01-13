/******************************************************************************
;       Program		: CrcApp.c
;       Function	: CRC Calculation Functions 
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 12
;       Author		: Joel Wang
;       Describe	: 1. The CRC32 polynomial is 0x04C11DB7.
;                  	  2. The CRC16 polynomial is 0x1021.
;					  3. Total XOR checksum calculation.
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"

#define POLYNOMIAL_CRC32    (0xEDB88320)
#define POLYNOMIAL_CRC16    (0x1021)
#define MASK_CRC32          (0x80000000)
#define MASK_CRC16          (0x8000)
#define WIDTH    (8 * sizeof(uint32_t))
#define TOPBIT   (((uint32_t)1) << (WIDTH - 1))


static uint16_t u16checksum_Current = 0;

/*---------------------------- Declare Variable ------------------------------*/
const uint32_t gu32Crc32Table[SIZE_CRC32] = 
{
    0x00000000u, 0x77073096u, 0xee0e612cu, 0x990951bau, 0x076dc419u, 0x706af48fu,
    0xe963a535u, 0x9e6495a3u, 0x0edb8832u, 0x79dcb8a4u, 0xe0d5e91eu, 0x97d2d988u,
    0x09b64c2bu, 0x7eb17cbdu, 0xe7b82d07u, 0x90bf1d91u, 0x1db71064u, 0x6ab020f2u,
    0xf3b97148u, 0x84be41deu, 0x1adad47du, 0x6ddde4ebu, 0xf4d4b551u, 0x83d385c7u,
    0x136c9856u, 0x646ba8c0u, 0xfd62f97au, 0x8a65c9ecu, 0x14015c4fu, 0x63066cd9u,
    0xfa0f3d63u, 0x8d080df5u, 0x3b6e20c8u, 0x4c69105eu, 0xd56041e4u, 0xa2677172u,
    0x3c03e4d1u, 0x4b04d447u, 0xd20d85fdu, 0xa50ab56bu, 0x35b5a8fau, 0x42b2986cu,
    0xdbbbc9d6u, 0xacbcf940u, 0x32d86ce3u, 0x45df5c75u, 0xdcd60dcfu, 0xabd13d59u,
    0x26d930acu, 0x51de003au, 0xc8d75180u, 0xbfd06116u, 0x21b4f4b5u, 0x56b3c423u,
    0xcfba9599u, 0xb8bda50fu, 0x2802b89eu, 0x5f058808u, 0xc60cd9b2u, 0xb10be924u,
    0x2f6f7c87u, 0x58684c11u, 0xc1611dabu, 0xb6662d3du, 0x76dc4190u, 0x01db7106u,
    0x98d220bcu, 0xefd5102au, 0x71b18589u, 0x06b6b51fu, 0x9fbfe4a5u, 0xe8b8d433u,
    0x7807c9a2u, 0x0f00f934u, 0x9609a88eu, 0xe10e9818u, 0x7f6a0dbbu, 0x086d3d2du,
    0x91646c97u, 0xe6635c01u, 0x6b6b51f4u, 0x1c6c6162u, 0x856530d8u, 0xf262004eu,
    0x6c0695edu, 0x1b01a57bu, 0x8208f4c1u, 0xf50fc457u, 0x65b0d9c6u, 0x12b7e950u,
    0x8bbeb8eau, 0xfcb9887cu, 0x62dd1ddfu, 0x15da2d49u, 0x8cd37cf3u, 0xfbd44c65u,
    0x4db26158u, 0x3ab551ceu, 0xa3bc0074u, 0xd4bb30e2u, 0x4adfa541u, 0x3dd895d7u,
    0xa4d1c46du, 0xd3d6f4fbu, 0x4369e96au, 0x346ed9fcu, 0xad678846u, 0xda60b8d0u,
    0x44042d73u, 0x33031de5u, 0xaa0a4c5fu, 0xdd0d7cc9u, 0x5005713cu, 0x270241aau,
    0xbe0b1010u, 0xc90c2086u, 0x5768b525u, 0x206f85b3u, 0xb966d409u, 0xce61e49fu,
    0x5edef90eu, 0x29d9c998u, 0xb0d09822u, 0xc7d7a8b4u, 0x59b33d17u, 0x2eb40d81u,
    0xb7bd5c3bu, 0xc0ba6cadu, 0xedb88320u, 0x9abfb3b6u, 0x03b6e20cu, 0x74b1d29au,
    0xead54739u, 0x9dd277afu, 0x04db2615u, 0x73dc1683u, 0xe3630b12u, 0x94643b84u,
    0x0d6d6a3eu, 0x7a6a5aa8u, 0xe40ecf0bu, 0x9309ff9du, 0x0a00ae27u, 0x7d079eb1u,
    0xf00f9344u, 0x8708a3d2u, 0x1e01f268u, 0x6906c2feu, 0xf762575du, 0x806567cbu,
    0x196c3671u, 0x6e6b06e7u, 0xfed41b76u, 0x89d32be0u, 0x10da7a5au, 0x67dd4accu,
    0xf9b9df6fu, 0x8ebeeff9u, 0x17b7be43u, 0x60b08ed5u, 0xd6d6a3e8u, 0xa1d1937eu,
    0x38d8c2c4u, 0x4fdff252u, 0xd1bb67f1u, 0xa6bc5767u, 0x3fb506ddu, 0x48b2364bu,
    0xd80d2bdau, 0xaf0a1b4cu, 0x36034af6u, 0x41047a60u, 0xdf60efc3u, 0xa867df55u,
    0x316e8eefu, 0x4669be79u, 0xcb61b38cu, 0xbc66831au, 0x256fd2a0u, 0x5268e236u,
    0xcc0c7795u, 0xbb0b4703u, 0x220216b9u, 0x5505262fu, 0xc5ba3bbeu, 0xb2bd0b28u,
    0x2bb45a92u, 0x5cb36a04u, 0xc2d7ffa7u, 0xb5d0cf31u, 0x2cd99e8bu, 0x5bdeae1du,
    0x9b64c2b0u, 0xec63f226u, 0x756aa39cu, 0x026d930au, 0x9c0906a9u, 0xeb0e363fu,
    0x72076785u, 0x05005713u, 0x95bf4a82u, 0xe2b87a14u, 0x7bb12baeu, 0x0cb61b38u,
    0x92d28e9bu, 0xe5d5be0du, 0x7cdcefb7u, 0x0bdbdf21u, 0x86d3d2d4u, 0xf1d4e242u,
    0x68ddb3f8u, 0x1fda836eu, 0x81be16cdu, 0xf6b9265bu, 0x6fb077e1u, 0x18b74777u,
    0x88085ae6u, 0xff0f6a70u, 0x66063bcau, 0x11010b5cu, 0x8f659effu, 0xf862ae69u,
    0x616bffd3u, 0x166ccf45u, 0xa00ae278u, 0xd70dd2eeu, 0x4e048354u, 0x3903b3c2u,
    0xa7672661u, 0xd06016f7u, 0x4969474du, 0x3e6e77dbu, 0xaed16a4au, 0xd9d65adcu,
    0x40df0b66u, 0x37d83bf0u, 0xa9bcae53u, 0xdebb9ec5u, 0x47b2cf7fu, 0x30b5ffe9u,
    0xbdbdf21cu, 0xcabac28au, 0x53b39330u, 0x24b4a3a6u, 0xbad03605u, 0xcdd70693u,
    0x54de5729u, 0x23d967bfu, 0xb3667a2eu, 0xc4614ab8u, 0x5d681b02u, 0x2a6f2b94u,
    0xb40bbe37u, 0xc30c8ea1u, 0x5a05df1bu, 0x2d02ef8du
};

const uint16_t gu16Crc16CcittTable[SIZE_CRC16] =
{
    0x0000u, 0x1021u, 0x2042u, 0x3063u, 0x4084u, 0x50A5u, 0x60C6u, 0x70E7u,
    0x8108u, 0x9129u, 0xA14Au, 0xB16Bu, 0xC18Cu, 0xD1ADu, 0xE1CEu, 0xF1EFu,
    0x1231u, 0x0210u, 0x3273u, 0x2252u, 0x52B5u, 0x4294u, 0x72F7u, 0x62D6u,
    0x9339u, 0x8318u, 0xB37Bu, 0xA35Au, 0xD3BDu, 0xC39Cu, 0xF3FFu, 0xE3DEu,
    0x2462u, 0x3443u, 0x0420u, 0x1401u, 0x64E6u, 0x74C7u, 0x44A4u, 0x5485u,
    0xA56Au, 0xB54Bu, 0x8528u, 0x9509u, 0xE5EEu, 0xF5CFu, 0xC5ACu, 0xD58Du,
    0x3653u, 0x2672u, 0x1611u, 0x0630u, 0x76D7u, 0x66F6u, 0x5695u, 0x46B4u,
    0xB75Bu, 0xA77Au, 0x9719u, 0x8738u, 0xF7DFu, 0xE7FEu, 0xD79Du, 0xC7BCu,
    0x48C4u, 0x58E5u, 0x6886u, 0x78A7u, 0x0840u, 0x1861u, 0x2802u, 0x3823u,
    0xC9CCu, 0xD9EDu, 0xE98Eu, 0xF9AFu, 0x8948u, 0x9969u, 0xA90Au, 0xB92Bu,
    0x5AF5u, 0x4AD4u, 0x7AB7u, 0x6A96u, 0x1A71u, 0x0A50u, 0x3A33u, 0x2A12u,
    0xDBFDu, 0xCBDCu, 0xFBBFu, 0xEB9Eu, 0x9B79u, 0x8B58u, 0xBB3Bu, 0xAB1Au,
    0x6CA6u, 0x7C87u, 0x4CE4u, 0x5CC5u, 0x2C22u, 0x3C03u, 0x0C60u, 0x1C41u,
    0xEDAEu, 0xFD8Fu, 0xCDECu, 0xDDCDu, 0xAD2Au, 0xBD0Bu, 0x8D68u, 0x9D49u,
    0x7E97u, 0x6EB6u, 0x5ED5u, 0x4EF4u, 0x3E13u, 0x2E32u, 0x1E51u, 0x0E70u,
    0xFF9Fu, 0xEFBEu, 0xDFDDu, 0xCFFCu, 0xBF1Bu, 0xAF3Au, 0x9F59u, 0x8F78u,
    0x9188u, 0x81A9u, 0xB1CAu, 0xA1EBu, 0xD10Cu, 0xC12Du, 0xF14Eu, 0xE16Fu,
    0x1080u, 0x00A1u, 0x30C2u, 0x20E3u, 0x5004u, 0x4025u, 0x7046u, 0x6067u,
    0x83B9u, 0x9398u, 0xA3FBu, 0xB3DAu, 0xC33Du, 0xD31Cu, 0xE37Fu, 0xF35Eu,
    0x02B1u, 0x1290u, 0x22F3u, 0x32D2u, 0x4235u, 0x5214u, 0x6277u, 0x7256u,
    0xB5EAu, 0xA5CBu, 0x95A8u, 0x8589u, 0xF56Eu, 0xE54Fu, 0xD52Cu, 0xC50Du,
    0x34E2u, 0x24C3u, 0x14A0u, 0x0481u, 0x7466u, 0x6447u, 0x5424u, 0x4405u,
    0xA7DBu, 0xB7FAu, 0x8799u, 0x97B8u, 0xE75Fu, 0xF77Eu, 0xC71Du, 0xD73Cu,
    0x26D3u, 0x36F2u, 0x0691u, 0x16B0u, 0x6657u, 0x7676u, 0x4615u, 0x5634u,
    0xD94Cu, 0xC96Du, 0xF90Eu, 0xE92Fu, 0x99C8u, 0x89E9u, 0xB98Au, 0xA9ABu,
    0x5844u, 0x4865u, 0x7806u, 0x6827u, 0x18C0u, 0x08E1u, 0x3882u, 0x28A3u,
    0xCB7Du, 0xDB5Cu, 0xEB3Fu, 0xFB1Eu, 0x8BF9u, 0x9BD8u, 0xABBBu, 0xBB9Au,
    0x4A75u, 0x5A54u, 0x6A37u, 0x7A16u, 0x0AF1u, 0x1AD0u, 0x2AB3u, 0x3A92u,
    0xFD2Eu, 0xED0Fu, 0xDD6Cu, 0xCD4Du, 0xBDAAu, 0xAD8Bu, 0x9DE8u, 0x8DC9u,
    0x7C26u, 0x6C07u, 0x5C64u, 0x4C45u, 0x3CA2u, 0x2C83u, 0x1CE0u, 0x0CC1u,
    0xEF1Fu, 0xFF3Eu, 0xCF5Du, 0xDF7Cu, 0xAF9Bu, 0xBFBAu, 0x8FD9u, 0x9FF8u,
    0x6E17u, 0x7E36u, 0x4E55u, 0x5E74u, 0x2E93u, 0x3EB2u, 0x0ED1u, 0x1EF0u
};

/*---------------------------- Start Program ---------------------------------*/
/******************************************************************************
;	Function Name			:	uint32_t CrcApp_Crc32_Calculation(uint8_t *pData, uint32_t u32Len, uint32_t u32Crc32)
;	Function Description	:	Calcuate CRC32 values.
;	Parameters				:	[*pData] - Input data.
;								[u32Len] - Set data length.
;								[u32Len] - CRC32 value.
;	Return Values			:	Return CRC32 value.
;	Source ID				:
******************************************************************************/
uint32_t CrcApp_Crc32_Calculation(uint8_t *pData, uint32_t u32Len, uint32_t u32Crc32)
{
	/*  First, the polynomial itself and its table of feedback terms.  The
	  	polynomial is X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 
	  	Polynomial = 0x04C11DB7 */

	uint32_t u32Index, u32TableIndex;

	for(u32Index=0U; u32Index<u32Len; u32Index++)
	{
		u32TableIndex = (u32Crc32 ^ (uint32_t)(*(pData + u32Index))) & (uint32_t)(0xFFU);		
		u32Crc32 = ((u32Crc32>>8U) & (uint32_t)(0x00FFFFFFU)) ^ gu32Crc32Table[u32TableIndex];
	}

	(void)(*pData);
	(void)(u32Len);
	return u32Crc32;
}

/******************************************************************************
;	Function Name			:	uint16_t CrcApp_Crc16_Calculation(uint8_t *pData, uint32_t u32Len)
;	Function Description	:	Calcuate CRC16 values.
;	Parameters				:	[*pData] - Input data.
;								[u32Len] - Set data length.
;	Return Values			:	Return CRC16 value.
;	Source ID				:
******************************************************************************/
uint16_t CrcApp_Crc16_Calculation(uint8_t *pData, uint32_t u32Len)
{	
	/* Polynomial = 0x1021 */
	uint16_t u16Crc16 = 0x0000U;
	uint16_t u16Crc16H, u16Crc16L, u16TableIndex;
	uint32_t u32Index;

	for(u32Index=0U; u32Index<u32Len; u32Index++)
	{
		u16Crc16H = (u16Crc16 >> 8U);
		u16Crc16L = (u16Crc16 << 8U);
		u16TableIndex = u16Crc16H ^ (uint16_t)(*(pData + u32Index));
		u16Crc16 = u16Crc16L ^ gu16Crc16CcittTable[u16TableIndex];		
	}	

	(void)(*pData);
	(void)(u32Len);
	return u16Crc16;
}

/******************************************************************************
;	Function Name			:	uint8_t CrcApp_CalculateSum8(uint8_t *Buffer, uint8_t Length)
;	Function Description	:
;	Parameters				:
;	Return Values			:	
;	Source ID				:
******************************************************************************/
uint8_t CrcApp_CalculateSum8(uint8_t *Buffer, uint8_t Length)
{
    uint8_t Sum;
    uint8_t Count;

    for(Sum = 0, Count = 0; Count < Length; Count++)
    {
        Sum = (uint8_t) (Sum + *(Buffer + Count));
    }

    return Sum;
}
    
/******************************************************************************
;	Function Name			:	uint16_t CrcApp_CalculateSum16(uint8_t *Buffer, uint8_t Length)
;	Function Description	:
;	Parameters				:
;	Return Values			:	
;	Source ID				:
******************************************************************************/
uint16_t CrcApp_CalculateSum16(uint8_t *Buffer, uint8_t Length)
{
    uint16_t u16Sum;
    uint8_t u8Count;

    for(u16Sum = 0, u8Count = 0; u8Count < Length; u8Count++)
    {
        u16Sum = (uint16_t) (u16Sum + *(Buffer + u8Count));
    }

    return u16Sum;
}

/******************************************************************************
;	Function Name			:	uint8_t CrcApp_CalculateCheckSum8(uint8_t *Buffer, uint8_t Length)
;	Function Description	:
;	Parameters				:
;	Return Values			:	
;	Source ID				:
******************************************************************************/
uint8_t CrcApp_CalculateCheckSum8(uint8_t *Buffer, uint8_t Length)
{
  uint8_t CheckSum;
  
  CheckSum = CrcApp_CalculateSum8(Buffer, Length);

  return CheckSum+1;
}

/******************************************************************************
;	Function Name			:	void CrcApp_CalculateDataCheckSum8(uint8_t *Buffer, uint8_t Length, uint8_t reset)
;	Function Description	:
;	Parameters				:
;	Return Values			:	
;	Source ID				:
******************************************************************************/
void CrcApp_CalculateAllDataCheckSum16(uint8_t *Buffer, uint8_t Length, uint8_t reset)
{
  uint16_t u16CheckSum;
  
  // caculate data byte first
  u16CheckSum = CrcApp_CalculateSum16 ((uint8_t  *)(Buffer+4), Length);
  // add checksum to previous for total sum
  if(1==reset)
  {
        u16checksum_Current = u16CheckSum;
  }
  else
  {
        u16checksum_Current += u16CheckSum;
  }
}

/******************************************************************************
;	Function Name			:	uint16_t CrcApp_GetCheckSum16(void)
;	Function Description	:
;	Parameters				:
;	Return Values			:	
;	Source ID				:
******************************************************************************/
uint16_t CrcApp_GetCheckSum16(void)
{
  return u16checksum_Current;
}
/*------------------------------------------------------------------------*/
