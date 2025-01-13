/***************************************************************************
*\file hal_sp_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_sp_def.h"

#if defined ( __ICCARM__ )
  
  extern void * CSTACK$$Base;   /* - Linker created - */
  extern void * CSTACK$$Limit;
  
#else
  #warning "hal_sp driver is supported only by IAR compiler"
#endif
  
sp_config_t gtdSPConfig = 
{
    .pvStackStart = &CSTACK$$Base,
    .pvStackEnd   = &CSTACK$$Limit,
};

/*** End of file ***/

  