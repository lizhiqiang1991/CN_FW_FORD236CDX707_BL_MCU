#ifndef HAL_SILICON_INFO_H
#define HAL_SILICON_INFO_H

typedef struct 
{
    uint32_t u32SiliconID;
    uint32_t u32Family;
    uint32_t u32Rev;
    uint32_t u32FlashBootMajorVersion;
    uint32_t u32FlashBootMinorVersion;
    uint32_t u32SROM_FW_MajorVersion;
    uint32_t u32SROM_FW_MinorVersion;
    uint32_t u32SFlash_Version;
    uint64_t u64UUID;
    
}silicon_info_t;

uint8_t HAL_SiliconID_Get(silicon_info_t* ptsInfo);

#endif

