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
#include <OemConfigData.h>
#include "PlatformPeim.h"
#include "mcu_info.h"

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

EFI_STATUS
EFIAPI
TestVariable()
{
  EFI_STATUS Status;
  OEM_CONFIG_DATA  OemConfigData;
  UINTN VariableSize = sizeof(OEM_CONFIG_DATA);
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
  //
  // Lookup DXE IPL PPI
  //
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

/*GPIO*/
typedef struct pad_info{
    uint32_t pad_sel_reg;
    uint32_t bit_offset;
    uint32_t gpio_swport_ddr;
    uint32_t gpio_ext_port;
    uint32_t port;
}pad_info_t;

static pad_info_t s3_flag_gpio = {
    .pad_sel_reg = 0x28180200,
    .bit_offset = 20,
    .gpio_swport_ddr =GPIO0_BASE + SWPORTA_DDR_OFFSET,
    .gpio_ext_port = GPIO0_BASE + EXT_PORTA_OFFSET,
    .port = 1,
};


void mdelay(uint32_t msec)
{

}
//复用gpio0_a1位gpio，且为输入模式（默认为输入模式），读取对应状态
uint32_t gpio_get_s3_flag(void)
{
    printf("s3 flag form gpio\n");
    uint32_t temp, temp1, ret;

    uint32_t pad_sel_reg = s3_flag_gpio.pad_sel_reg;
    uint32_t bit_offset = s3_flag_gpio.bit_offset;

    uint32_t gpio_swport_ddr = s3_flag_gpio.gpio_swport_ddr;
    uint32_t gpio_ext_port = s3_flag_gpio.gpio_ext_port;
    uint32_t port = s3_flag_gpio.port;

    //读取之前配置方向寄存器
    temp1 = readl((u64)gpio_swport_ddr);
    //配置为输入模式
    writel(temp1 & (1 << port), (u64)gpio_swport_ddr);

    temp = readl((u64)pad_sel_reg);
    writel((temp & (~(0xf << bit_offset))) | (0x1 << bit_offset), (u64)pad_sel_reg);
    mdelay(10);
    //读取当前控io寄存器的值
    ret = readl((u64)gpio_ext_port);
    //恢复配置
    writel(temp1, (u64)gpio_swport_ddr);
    writel(temp, (u64)pad_sel_reg);

    //判断对应io引脚是否为高电平
    if((1 << port) == ret)
        return  1;
    else
        return  0;
}


 void send_cpld_ctr(uint32_t cmd)
 {
     uint32_t i;

     mdelay(1);
     printf("u-boot : send cmd to cpld : %d \n", cmd);
     //start
     writel(0x1, PWR_CTR0_REG);
     mdelay(2);
     for(i = 0; i < cmd; i++){
         writel(0x1, PWR_CTR1_REG);
         mdelay(1);
         writel(0x0, PWR_CTR1_REG);
         mdelay(1);
     }
     //end
     mdelay(2);
     writel(0x0, PWR_CTR0_REG);

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
  UINT8		Buff[0x100];

  DEBUG ((EFI_D_LOAD | EFI_D_INFO, "Platform PEIM Loaded\n"));

  TestVariable();
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
#if 1
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
  } else if(0xffffffffffffffff == ArmSmcArgs.Arg0){
    DEBUG((EFI_D_INFO, "Done!\n"));
  } else {
	//PEU
    DEBUG((EFI_D_INFO, "call 0xc2000f03\n"));
    ArmSmcArgs.Arg0 = 0xc2000f03;
	ArmSmcArgs.Arg2 = (UINTN)&peu_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
		while(1);
	}

	//DDR
    uint32_t s3_flag = gpio_get_s3_flag();
    send_cpld_ctr(1);
	mcu_config_t mcu_base_info;
    get_mcu_param_info(&mcu_base_info);
	if(s3_flag)
        mcu_base_info.train_recover = 0x1;
    DEBUG((EFI_D_INFO, "call 0xc2000f04\n"));
    ArmSmcArgs.Arg0 = 0xc2000f04;
    ArmSmcArgs.Arg2 = (UINTN)&mcu_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
		while(1);
	}

	//SEC
    DEBUG((EFI_D_INFO, "call 0xc2000f05\n"));
	uint32_t s3_fun = 0;
    if(s3_flag)
        s3_fun = 1;

    ArmSmcArgs.Arg0 = 0xc2000f05;
    ArmSmcArgs.Arg1 = s3_fun;
    ArmSmcArgs.Arg2 = (UINTN)&common_base_info;
    ArmCallSmc (&ArmSmcArgs);
	if(0 != ArmSmcArgs.Arg0) {
		DEBUG((EFI_D_INFO, "Error ret=%d\n", ArmSmcArgs.Arg0));
		while(1);
	}

  }
#endif
  return Status;
}
