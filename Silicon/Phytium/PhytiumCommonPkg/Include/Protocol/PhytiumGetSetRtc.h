/** @file
  The Header of Protocol For RTC.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PHY_GET_SET_RTC_H_
#define _PHY_GET_SET_RTC_H_

#define EFI_GET_SET_RTC_PROTOCOL_GUID \
  { \
    0x40b1dd4e, 0x5653, 0x4457, 0xb2, 0x37, 0x61, 0xda, 0xdc, 0xe, 0xa4, 0xcb \
  }

extern EFI_GUID					gEfiGetSetRtcProtocolGuid;

typedef struct _EFI_GET_SET_RTC_PROTOCOL  EFI_GET_SET_RTC_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *RtcGetTime) (
  OUT   EFI_TIME				*Time,
  OUT   EFI_TIME_CAPABILITIES	*Capabilities
);

typedef
EFI_STATUS
(EFIAPI *RtcSetTime) (
  IN    EFI_TIME                *Time
);

struct _EFI_GET_SET_RTC_PROTOCOL {
  RtcGetTime        GetTime;
  RtcSetTime        SetTime;
};

extern void *i2c_regs_p;

int i2c_read(unsigned char Chip, unsigned int Addr, int Alen, unsigned char *Buffer, int Len);
int i2c_write(unsigned char Chip, unsigned int Addr, int Alen, unsigned char *Buffer, int Len);
void i2c_init(unsigned long BusAddress, unsigned int Speed, int SlaveAddress);
#endif
