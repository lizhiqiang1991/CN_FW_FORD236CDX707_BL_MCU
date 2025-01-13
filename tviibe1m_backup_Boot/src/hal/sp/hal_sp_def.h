/***************************************************************************
*\file hal_sp_def.h 
****************************************************************************/

#ifndef HAL_SP_DEF_H
#define HAL_SP_DEF_H

typedef struct 
{
  void* pvStackStart;
  void* pvStackEnd;
}sp_config_t;

extern sp_config_t gtdSPConfig;

#endif

  