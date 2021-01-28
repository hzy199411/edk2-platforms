#/** @file
#  Phytium RTC Drivers.
#
#  Copyright (C) 2020, Phytium Technology Co,Ltd. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#**/

#include "GetSetRtcDxe.h"

int rtc_get (rtc_time *tmp);
int rtc_set (EFI_TIME *tmp, rtc_time *current);
void rtc_reset (void);

BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  STATIC CONST INTN DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (Time->Day < 1 ||
      Time->Day > DayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
     ) {
    return FALSE;
  }

  return TRUE;
}

EFI_STATUS
EFIAPI
GetRtcTime (
  OUT EFI_TIME                *Time,
  OUT EFI_TIME_CAPABILITIES   *Capabilities
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  rtc_time    RtcTime;
  int         ret;

  // Ensure Time is a valid pointer
  if (Time == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }


  ret = rtc_get(&RtcTime);
  if (ret == 0) {
    Time->Year       = (UINT16)RtcTime.tm_year;
    Time->Month      = (UINT8)RtcTime.tm_mon;
    Time->Day        = (UINT8)RtcTime.tm_mday;
    Time->Hour       = (UINT8)RtcTime.tm_hour;
    Time->Minute     = (UINT8)RtcTime.tm_min;
    Time->Second     = (UINT8)RtcTime.tm_sec;
    Time->Nanosecond = 0;
  } else {
    //DEBUG((EFI_D_ERROR, " LibGetTime-error\n",Status));
    Status = EFI_UNSUPPORTED;
    }
  EXIT:
  return Status;
}

EFI_STATUS
EFIAPI
SetRtcTime (
  IN  EFI_TIME                *Time
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  rtc_time    RtcTime;
  int         ret;

  // Check the input parameters are within the range specified by UEFI
  if ((Time->Year   < 1900) ||
      (Time->Year   > 9999) ||
      (Time->Month  < 1   ) ||
      (Time->Month  > 12  ) ||
      (!DayValid (Time)   ) ||
      (Time->Hour   > 23  ) ||
      (Time->Minute > 59  ) ||
      (Time->Second > 59  ) ||
      (Time->Nanosecond > 999999999) ||
      (!((Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE) || ((Time->TimeZone >= -1440) && (Time->TimeZone <= 1440)))) ||
      (Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT)))
    ) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }

  // Because the PL031 is a 32-bit counter counting seconds,
  // the maximum time span is just over 136 years.
  // Time is stored in Unix Epoch format, so it starts in 1970,
  // Therefore it can not exceed the year 2106.
  if ((Time->Year < 1970) || (Time->Year >= 2106)) {
    Status = EFI_UNSUPPORTED;
    goto EXIT;
  }

  ret = rtc_get (&RtcTime);
  if (ret == 0) {
    rtc_set(Time, &RtcTime);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  EXIT:
  return Status;
}

EFI_GET_SET_RTC_PROTOCOL mGetSetRtc = {
    GetRtcTime,
    SetRtcTime,
};

extern void *i2c_regs_p;


VOID
EFIAPI
VirtualNotifyEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EfiConvertPointer (0x0, (VOID **) &i2c_regs_p);

  return;
}

EFI_STATUS
EFIAPI
GetSetRtcEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  UINT64        I2CBaseAddress;
  UINT32        I2CSpeed;
  UINT32        I2CSlaveAddress;
  EFI_EVENT     Event;

  I2CBaseAddress = PcdGet64 (PcdRtcI2cControllerBaseAddress);
  I2CSpeed = PcdGet32 (PcdRtcI2cControllerSpeed);
  I2CSlaveAddress = PcdGet32 (PcdRtcI2cControllerSlaveAddress);

  // Initialize I2C Bus which contain RTC chip
  i2c_init(I2CBaseAddress, I2CSpeed, I2CSlaveAddress);

  // Declare the controller as EFI_MEMORY_RUNTIME
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  I2CBaseAddress,
                  SIZE_4KB,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gDS->SetMemorySpaceAttributes (I2CBaseAddress, SIZE_4KB, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->InstallProtocolInterface(
                  &ImageHandle,
                  &gEfiGetSetRtcProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mGetSetRtc
                  );
  ASSERT_EFI_ERROR (Status);


  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
