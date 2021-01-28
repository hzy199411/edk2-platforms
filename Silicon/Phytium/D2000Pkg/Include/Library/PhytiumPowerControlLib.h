#ifndef __POWERCONTROL__
#define __POWERCONTROL__
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ArmSmcLib.h>

void SystemOff(void);
void SystemReboot(void);
UINT32 get_s3_flag(void);
void set_s3_flag(void);
void clean_s3_flag(void);
void enable_vtt(void);
void disable_vtt(void);
void mdelay(UINT32 time);
UINT32 pm_get_s3_flag_source(void);
#endif
