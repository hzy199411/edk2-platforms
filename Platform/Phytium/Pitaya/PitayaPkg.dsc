## @file
# This package provides common open source Phytium Platform modules.
#
# Copyright (C) 2020, Phytium Technology Co, Ltd. All rights reserved.
#
# SPDX-License-Identifier:BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = PitayaPkg
  PLATFORM_GUID                  = 1ed3bf41-6294-4ff3-8596-930a24ae47ef
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Platform/Phytium/Pitaya/PitayaPkg.fdf

!include Silicon/Phytium/PhytiumCommonPkg/PhytiumCommonPkg.dsc.inc

[LibraryClasses.common]
  # Phytium Platform library
  ArmPlatformLib|Silicon/Phytium/S2500/Library/PlatformLib/PlatformLib.inf
  LogoLib|Silicon/Phytium/PhytiumCommonPkg/Library/LogoLib/LogoLib.inf

  #S2500 RTC Driver
  RealTimeClockLib|Silicon/Phytium/S2500/Library/Ds1339_RtcLib/Ds1339_RtcLib.inf
  I2CLib|Silicon/Phytium/S2500/Library/Dw_I2CLib/Dw_I2CLib.inf
  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf

  # PL011 UART Driver and Dependency Libraries
  SerialPortLib|ArmPlatformPkg/Library/PL011SerialPortLib/PL011SerialPortLib.inf
  PL011UartClockLib|ArmPlatformPkg/Library/PL011UartClockLib/PL011UartClockLib.inf
  PL011UartLib|ArmPlatformPkg/Library/PL011UartLib/PL011UartLib.inf

[LibraryClasses.common.DXE_DRIVER]
  # Pci dependencies
  PciSegmentLib|Silicon/Phytium/S2500/Library/PciSegmentLib/PciSegmentLib.inf
  PciHostBridgeLib|Silicon/Phytium/S2500/Library/PciHostBridgeLib/PciHostBridgeLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Pitaya Platform"

  gArmTokenSpaceGuid.PcdVFPEnabled|1
  gArmTokenSpaceGuid.PcdArmPrimaryCoreMask|0x303

  gArmPlatformTokenSpaceGuid.PcdCoreCount|4
  gArmPlatformTokenSpaceGuid.PcdClusterCount|2

  #
  # NV Storage PCDs.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0xe00000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableSize|0x00020000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64|0xe20000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingSize|0x00010000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64|0xe30000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareSize|0x00020000

  # Size of the region used by UEFI in permanent memory (Reserved 64MB)
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x04000000

  #
  # PL011 - Serial Terminal
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x28001000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultReceiveFifoDepth|0
  gArmPlatformTokenSpaceGuid.PL011UartClkInHz|48000000
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200


  #
  # ARM General Interrupt Controller
  #
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x2a000000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x2b000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x29c00000

  # System IO space
  gPhytiumPlatformTokenSpaceGuid.PcdSystemIoBase|0x0
  gPhytiumPlatformTokenSpaceGuid.PcdSystemIoSize|0x40000000

  # System Memory (2GB ~ 4GB - 64MB), the top 64MB is reserved for PBF
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000
  gArmTokenSpaceGuid.PcdSystemMemorySize|0x4000000

  #
  # Designware PCI Root Complex
  #
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0x40000000
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|44
  gPhytiumPlatformTokenSpaceGuid.PcdPciConfigBase|0x40000000
  gPhytiumPlatformTokenSpaceGuid.PcdPciConfigSize|0x10000000
  gArmTokenSpaceGuid.PcdPciBusMin|0
  gArmTokenSpaceGuid.PcdPciBusMax|128
  gArmTokenSpaceGuid.PcdPciIoBase|0x01000
  gArmTokenSpaceGuid.PcdPciIoSize|0xf00000
  gArmTokenSpaceGuid.PcdPciIoTranslation|0x50000000
  gArmTokenSpaceGuid.PcdPciMmio32Base|0x58000000
  gArmTokenSpaceGuid.PcdPciMmio32Size|0x28000000
  gArmTokenSpaceGuid.PcdPciMmio32Translation|0x0
  gArmTokenSpaceGuid.PcdPciMmio64Base|0x00000000
  gArmTokenSpaceGuid.PcdPciMmio64Size|0x1000000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciDisableBusEnumeration|FALSE

  #
  # SPI Flash Control Register Base Address and Size
  #
  gPhytiumPlatformTokenSpaceGuid.PcdSpiFlashBase|0x0
  gPhytiumPlatformTokenSpaceGuid.PcdSpiFlashSize|0x1000000
  gPhytiumPlatformTokenSpaceGuid.PcdSpiControllerBase|0x1fffff00
  gPhytiumPlatformTokenSpaceGuid.PcdSpiControllerSize|0x100

  #
  # RTC I2C Controller Register Base Address and Speed
  #
  gPhytiumPlatformTokenSpaceGuid.PcdRtcI2cControllerBaseAddress|0x28002000
  gPhytiumPlatformTokenSpaceGuid.PcdRtcI2cControllerSpeed|100000
  gPhytiumPlatformTokenSpaceGuid.PcdRtcI2cControllerSlaveAddress|0x68
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"V1.0"

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # PCD database
  #
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

  ShellPkg/DynamicCommand/TftpDynamicCommand/TftpDynamicCommand.inf
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellAcpiViewCommandLib/UefiShellAcpiViewCommandLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
  }

  ArmPlatformPkg/PrePi/PeiMPCore.inf {
    <LibraryClasses>
      ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  }

  #
  #Dxe core entry
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
  }

  #DXE driver
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
  }
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf

  #
  # Common Arm Timer and Gic Components
  #
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf

  #
  # security system
  #
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
  }

  #
  #network,  mod for https boot.
  #
  NetworkPkg/SnpDxe/SnpDxe.inf
  NetworkPkg/DpcDxe/DpcDxe.inf
  NetworkPkg/MnpDxe/MnpDxe.inf
  NetworkPkg/ArpDxe/ArpDxe.inf
  NetworkPkg/Dhcp4Dxe/Dhcp4Dxe.inf
  NetworkPkg/Ip4Dxe/Ip4Dxe.inf
  NetworkPkg/Mtftp4Dxe/Mtftp4Dxe.inf
  NetworkPkg/Udp4Dxe/Udp4Dxe.inf
  NetworkPkg/VlanConfigDxe/VlanConfigDxe.inf

  NetworkPkg/Ip6Dxe/Ip6Dxe.inf
  NetworkPkg/Udp6Dxe/Udp6Dxe.inf
  NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
  NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  NetworkPkg/TcpDxe/TcpDxe.inf

  NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf

  NetworkPkg/DnsDxe/DnsDxe.inf
  NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  NetworkPkg/HttpDxe/HttpDxe.inf

  # FV Filesystem
  MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf

  #
  # Common Console Components
  #
  # ConIn,ConOut,StdErr
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf

  #
  # Hii database init
  #
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf

  #
  # Generic Watchdog Timer
  #
  ArmPkg/Drivers/GenericWatchdogDxe/GenericWatchdogDxe.inf

  #
  # Spi driver
  #
  Silicon/Phytium/S2500/Drivers/SpiDxe/SpiDxe.inf

  #
  # NOR Flash driver
  #
  Silicon/Phytium/S2500/Drivers/SpiNorFlashDxe/SpiNorFlashDxe.inf
  Silicon/Phytium/PhytiumCommonPkg/Drivers/FlashFvbDxe/FlashFvbDxe.inf
  
  # Rtc driver
  Silicon/Phytium/S2500/Drivers/GetSetRtc/GetSetRtc.inf
  
  #
  # Usb Support
  #
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  #
  # IDE/AHCI Support
  #
  MdeModulePkg/Bus/Pci/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf

  #
  # PCI Support
  #
  ArmPkg/Drivers/ArmPciCpuIo2Dxe/ArmPciCpuIo2Dxe.inf
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  MdeModulePkg/Bus/Pci/NonDiscoverablePciDeviceDxe/NonDiscoverablePciDeviceDxe.inf

  #
  # The following 2 module perform the same work except one operate variable.
  # Only one of both should be put into fdf.
  #
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf

  #
  # NVME Support
  #
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

  #
  # ACPI Support
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  Silicon/Phytium/S2500/Drivers/AcpiTables/AcpiTables.inf
  Silicon/Phytium/PhytiumCommonPkg/Drivers/AcpiPlatformDxe/AcpiPlatformDxe.inf

  #
  # SMBIOS
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  Silicon/Phytium/S2500/Drivers/SmbiosPlatformDxe/SmbiosPlatformDxe.inf

  #
  # Bds
  #
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  MdeModulePkg/Universal/DriverSampleDxe/DriverSampleDxe.inf
  MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }
  MdeModulePkg/Application/BootManagerMenuApp/BootManagerMenuApp.inf

