/** @file
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <PiPei.h>

//
// The protocols, PPI and GUID definitions for this module
//
#include <Ppi/MasterBootMode.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/GuidedSectionExtraction.h>
//
// The Library classes this module consumes
//
#include <Library/ArmPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/ArmSmcLib.h>
#include <Ppi/ReadOnlyVariable2.h>
//#include <OemConfigData.h>
#include "PlatformPeim.h"
#include "mcu_info.h"
#include <Library/PhytiumPowerControlLib.h>

EFI_STATUS
EFIAPI
InitializePlatformPeim (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
PlatformPeim (
  VOID
  );

//
// Module globals
//
CONST EFI_PEI_PPI_DESCRIPTOR  mPpiListBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

CONST EFI_PEI_PPI_DESCRIPTOR  mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

#if 0
EFI_STATUS
EFIAPI
TestVariable()
{
  EFI_STATUS Status;

  OEM_CONFIG_DATA  OemConfigData;
  UINTN VariableSize = sizeof(OEM_CONFIG_DATA);

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
  // // Lookup DXE IPL PPI //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariablePpi
             );
  ASSERT_EFI_ERROR (Status);

  //PeiServicesLocatePpi (
  Status = VariablePpi->GetVariable(
				VariablePpi,
                OEM_CONFIG_NAME,
                &gOemDataSetupGuid,
				NULL,
				&VariableSize,
                &OemConfigData
          );

  if(EFI_ERROR(Status)) {
      if(EFI_NOT_FOUND == Status) {
          DEBUG((EFI_D_INFO, "%a() Line=%d\n", __FUNCTION__, __LINE__));
      } else {
        ASSERT_EFI_ERROR(Status);
      }
  } else {
      DEBUG((EFI_D_INFO, "%a() Line=%d\n", __FUNCTION__, __LINE__));
  }

  return EFI_SUCCESS;
}
#endif

UINT32 ReadDeviceId()
{
  UINT32 Reg = 0;

  MmioWrite32(0x28014010, 0x9F002040);
  Reg = MmioRead32(0x2801401C);
  return Reg;
}

VOID QspiSet(void)
{
  UINT32 DeviceId = 0;

  DeviceId = ReadDeviceId();
  DEBUG((EFI_D_INFO, "JEDEC Device ID:%x\n", DeviceId));

  //WriteEnable
  MmioWrite32(0x28014010, 0x06000000);
  MmioRead32(0x2801401C);
  //Set CR1V[1] to 1, CR1V Adress is 00800002
  MmioWrite32(0x28014010, 0x7100A000);
  MmioWrite32(0x28014014, 0x800002);
  MmioWrite32(0x2801401C, 0x2);
  //Set Flash Mode
  MmioWrite32(0x2801402C, 0xF0A0);
  //
  MmioWrite32(0x28014004, 0xEC4F407E);

  DEBUG((EFI_D_INFO, "%a() Line=%d, Reg=%x\n", __FUNCTION__, __LINE__, MmioRead32(0x28014004)));
}


/*++

Routine Description:



Arguments:

  FileHandle  - Handle of the file being invoked.
  PeiServices - Describes the list of possible PEI Services.

Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
EFI_STATUS
EFIAPI
InitializePlatformPeim (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  ARM_SMC_ARGS                  ArmSmcArgs;
  EFI_STATUS                    Status;
  EFI_BOOT_MODE                 BootMode;

  DEBUG ((EFI_D_LOAD | EFI_D_INFO, "Platform PEIM Loaded\n"));

  //TestVariable();
  Status = PeiServicesSetBootMode (ArmPlatformGetBootMode ());
  ASSERT_EFI_ERROR (Status);

  PlatformPeim ();

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesInstallPpi (&mPpiListBootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    Status = PeiServicesInstallPpi (&mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG((EFI_D_INFO, "call 0xc2000f01\n"));
  ArmSmcArgs.Arg0 = 0xc2000f01;
  ArmCallSmc (&ArmSmcArgs);
  DEBUG((EFI_D_INFO, "reset source=%lx\n", ArmSmcArgs.Arg0));
  if(0x1 == ArmSmcArgs.Arg0) {
	//PLL
    DEBUG((EFI_D_INFO, "call 0xc2000f02\n"));
	DEBUG((EFI_D_INFO, "cpu_base_info addr=%p\n", &cpu_base_info));
    ArmSmcArgs.Arg0 = 0xc2000f02;
    ArmSmcArgs.Arg1 = 0;
	ArmSmcArgs.Arg2 = (UINTN)&cpu_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%x\n", ArmSmcArgs.Arg0));
		while(1);
	}
  } else if(0x4 != ArmSmcArgs.Arg0){
    DEBUG((EFI_D_INFO, "Other reset source!\n"));
    while(1);
  }

 	//PEU
    DEBUG((EFI_D_INFO, "call 0xc2000f03\n"));
    ArmSmcArgs.Arg0 = 0xc2000f03;
    ArmSmcArgs.Arg1 = 0;
	ArmSmcArgs.Arg2 = (UINTN)&peu_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
		while(1);
	}

	//DDR
    /*uint32_t s3_flag = gpio_get_s3_flag();*/
    uint32_t S3Source = pm_get_s3_flag_source();
    uint32_t s3_flag = get_s3_flag();
    clean_s3_flag();
    /*uint32_t s3_flag = 0;*/
	mcu_config_t mcu_base_info;
    get_mcu_param_info(&mcu_base_info);
    if(s3_flag) {
        DEBUG((EFI_D_INFO, "Run in S3\n"));
        mcu_base_info.train_recover = 0x1;
    }
    //ec disabe/enabel vtt don't implement
    if(S3Source == 1) {
      //gpio mode
      DEBUG((EFI_D_INFO, "call 0xc2000f04\n"));
      ArmSmcArgs.Arg0 = 0xc2000f04;
      ArmSmcArgs.Arg1 = 0x0;
      ArmSmcArgs.Arg2 = (UINTN)&mcu_base_info;
      ArmCallSmc (&ArmSmcArgs);
	  if(0 != ArmSmcArgs.Arg0) {
	  	DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
	  	while(1);
      } else {
          if(s3_flag) {
            mdelay(10);
            enable_vtt();
            mdelay(40);
            ArmSmcArgs.Arg0 = 0xc2000f04;
            ArmSmcArgs.Arg1 = 0x3;
            ArmCallSmc (&ArmSmcArgs);
          }
      }
    } else {
      //ec mode
      DEBUG((EFI_D_INFO, "call 0xc2000f04\n"));
      ArmSmcArgs.Arg0 = 0xc2000f04;
      ArmSmcArgs.Arg1 = 0x0;
      ArmSmcArgs.Arg2 = (UINTN)&mcu_base_info;
      ArmCallSmc (&ArmSmcArgs);
	  if(0 != ArmSmcArgs.Arg0) {
	  	DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
	  	while(1);
      } else {
        if(s3_flag) {
          ArmSmcArgs.Arg0 = 0xc2000f04;
          ArmSmcArgs.Arg1 = 0x3;
          ArmCallSmc (&ArmSmcArgs);
        }
      }
    }

	//SEC
    DEBUG((EFI_D_INFO, "call 0xc2000f05\n"));
	uint32_t s3_fun = 0;
    if(s3_flag) {
        DEBUG((EFI_D_INFO, "Run in S3\n"));
        s3_fun = 1;
    }

    ArmSmcArgs.Arg0 = 0xc2000f05;
    ArmSmcArgs.Arg1 = s3_fun;
    ArmSmcArgs.Arg2 = (UINTN)&common_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
		while(1);
	}


  return Status;
}
