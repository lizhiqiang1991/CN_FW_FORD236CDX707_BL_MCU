/***************************************************************************
*\file hal_sp.h 
****************************************************************************/

#ifndef HAL_SP_H
#define HAL_SP_H

uint8_t HAL_SP_Init           (sp_config_t *tsConfig);
uint8_t HAL_SP_Overflow_Check (sp_config_t *tsConfig);
uint8_t HAL_SP_Usage_Get      (sp_config_t *tsConfig,uint8_t* pReturn);

#endif

  