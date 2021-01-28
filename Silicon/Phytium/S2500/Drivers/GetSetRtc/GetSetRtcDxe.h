/** @file
  Phytium RTC Header

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DXE_EFI_GET_SET_RTC_H_
#define _DXE_EFI_GET_SET_RTC_H_

#include <Guid/EventGroup.h>
#include <Protocol/PhytiumGetSetRtc.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>

/* I2C related */
#define CONFIG_DW_I2C
#define CONFIG_CMD_I2C
#define CONFIG_HARD_I2C

/* RTC related */
#define CONFIG_CMD_DATE
#define CONFIG_RTC_DS1337
#define CONFIG_RTC_DS1339
#define CONFIG_SYS_I2C_RTC_ADDR  0x68

typedef UINT64 u64;
typedef UINT32 u32;
typedef UINT16 u16;
typedef UINT8  u8;

typedef unsigned char uchar;

typedef struct {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
} rtc_time;

typedef struct {
  char min;
  char hour;
  char day;
  char mon;
  char year;
} rtc_time_valid;

#endif
