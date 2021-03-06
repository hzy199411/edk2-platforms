/** @file
  Phytium NorFlash Drivers Header.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SPI_NORFALSH_DXE_H_
#define SPI_NORFALSH_DXE_H_

#include <Guid/EventGroup.h>
#include <Guid/GlobalVariable.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <PiDxe.h>
#include <Protocol/SpiProtocol.h>
#include <Protocol/SpiNorFlashProtocol.h>
#include <SystemServiceInterface.h>
#include <Uefi/UefiBaseType.h>

//
//  Norflash registers
//
#define REG_FLASH_CAP 0x000
#define REG_RD_CFG    0x004
#define REG_WR_CFG    0x008
#define REG_FLUSH_REG 0x00C
#define REG_CMD_PORT  0x010
#define REG_ADDR_PORT 0x014
#define REG_HD_PORT   0x018
#define REG_LD_PORT   0x01C
#define REG_CS_CFG    0x020
#define REG_WIP_CFG   0x024
#define REG_WP_REG    0x028

#define NORFLASH_SIGNATURE     SIGNATURE_32 ('F', 'T', 'S', 'F')

//
// Platform Nor Flash Functions
//
EFI_STATUS
EFIAPI
NorFlashPlatformEraseSingleBlock (
  IN UINTN                BlockAddress
  );

EFI_STATUS
EFIAPI
NorFlashPlatformErase (
  IN UINT64                  Offset,
  IN UINT64                  Length
  );

EFI_STATUS
EFIAPI
NorFlashPlatformRead (
  IN UINTN                Address,
  IN VOID                 *Buffer,
  OUT UINT32              Len
  );

EFI_STATUS
EFIAPI
NorFlashPlatformWrite (
  IN UINTN                Address,
  IN VOID                 *Buffer,
  IN UINT32               Len
  );

EFI_STATUS
EFIAPI
NorFlashPlatformGetDevices (
  OUT NOR_FLASH_DEVICE_DESCRIPTION *NorFlashDevices
  );

EFI_STATUS
EFIAPI
NorFlashPlatformInitialization (
  VOID
  );

EFI_STATUS
EFIAPI
NorFlashPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

typedef struct {
  EFI_NORFLASH_DRV_PROTOCOL FlashProtocol;
  UINTN                   Signature;
  EFI_HANDLE              Handle;
} NorFlash_Device;

#endif // SPI_NORFALSH_DXE_H_
