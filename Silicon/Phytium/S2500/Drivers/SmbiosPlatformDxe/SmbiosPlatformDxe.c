/** @file
  This driver installs SMBIOS information for Phytium Durian platforms.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Platform.h>
#include <IndustryStandard/SmBios.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>


// SMBIOS tables often reference each other using
// fixed constants, define a list of these constants
// for our hardcoded tables

#define TYPE0_STRINGS                                    \
  "PHYTIUM LTD\0" /* Vendor */      \
  "V2.2\0"                           /* BiosVersion */ \
  __DATE__"\0"                         /* BiosReleaseDate */

#define TYPE1_STRINGS                                   \
  "PHYTIUM LTD\0"                         /* Manufacturer */ \
  "Phytium S2500/64\0"  						  /* Product Name */ \
  "None\0"                           /* Version */      \
  "Serial Not Set\0"           				  /* SerialNumber */ \
  "SKUNumber Not Set\0"           				  /* SKUNumber */ \
  "S2500\0"           				  /* Family */ \

#define TYPE2_STRINGS                                     \
  "PHYTIUM LTD\0"                          /* Manufacturer */   \
  "Phytium S2500/64\0"  						 /* Product Name */   \
  "None\0"                             /* Version */        \
  "Serial Not Set\0"                 			/* Serial */         \
  "AssetTag Not Set\0"                			/* BaseBoardAssetTag */ \
  "Base of Chassis\0"                             /* BaseBoardChassisLocation */

#define TYPE3_STRINGS                                   \
  "PHYTIUM LTD\0"                      /* Manufacturer */ \
  "None\0"                           /* Version */      \
  "Serial Not Set\0"                  			  /* Serial  */		\
  "AssetTag Not Set\0"                 			  /* AssetTag  */

#define TYPE4_STRINGS                                               \
  "S2500\0"                          /* socket type */              \
  "PHYTIUM LTD\0"                        /* manufactuer */              \
  "S2500/64\0"                         /* processor version */  \
  "Serial Not Set\0"                   /* SerialNumber */  \
  "None \0"                     /* processor 2 description */  \
  "AssetTag Not Set"                          /* AssetTag */

#define TYPE7_STRINGS                              \
  "L1 Instruction\0"                 /* L1I  */    \
  "L1 Data\0"                        /* L1D  */    \
  "L2\0"                             /* L2   */

#define	TYPE7_L1DATA_STRINGS						\
  "L1 Data Cache\0"                             /* L1 data   */


#define	TYPE7_L1INS_STRINGS						\
  "L1 Instruction Cache\0"					/* L1 ins   */

#define	TYPE7_L2_STRINGS					\
  "L2 Cache\0"					/* L2   */

#define	TYPE7_L3_STRINGS					\
  "L3 Cache\0"					/* L3   */


#define TYPE9_STRINGS                              \
  "PCIE_SLOT0\0"                     /* Slot0 */   \
  "PCIE_SLOT1\0"                     /* Slot1 */   \
  "PCIE_SLOT2\0"                     /* Slot2 */   \
  "PCIE_SLOT3\0"                     /* Slot3 */

#define TYPE9_STRINGS_PCIE0X16                              \
  "PCIE0_X16\0"

#define TYPE9_STRINGS_PCIE0X1                             \
  "PCIE0_X1\0"

#define TYPE9_STRINGS_PCIE1X16                              \
  "PCIE1_X16\0"

#define TYPE9_STRINGS_PCIE1X1                              \
  "PCIE1_X1\0"

#define TYPE13_STRINGS                             \
  "en|US|iso8859-1\0"							\
  "zh|CN|unicode\0"


#define TYPE16_STRINGS                             \
  "\0"                               /* nothing */

#define TYPE17_STRINGS                                       \
  "RIGHT SIDE\0"                     /* location */          \
  "BANK 0\0"                         /* bank description */  \
  "Kingston\0"						 /* Manufacturer */	\
  "Serial Not Set\0"				 /* SerialNumber */			\
  "AssetTag Not Set\0"							\
  "Part Number Not Set\0"

#define TYPE19_STRINGS                             \
  "\0"                               /* nothing */

#define TYPE32_STRINGS                             \
  "\0"                               /* nothing */

#define TYPE39_STRINGS 		\
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/    \
  "Not specified\0"				/* not specified*/

#define TYPE38_STRINGS                             \
  "\0"

//
// Type definition and contents of the default SMBIOS table.
// This table covers only the minimum structures required by
// the SMBIOS specification (section 6.2, version 3.0)
//
#pragma pack(1)
typedef struct {
  SMBIOS_TABLE_TYPE0 Base;
  INT8              Strings[sizeof(TYPE0_STRINGS)];
} ARM_TYPE0;

typedef struct {
  SMBIOS_TABLE_TYPE1 Base;
  UINT8              Strings[sizeof(TYPE1_STRINGS)];
} ARM_TYPE1;

typedef struct {
  SMBIOS_TABLE_TYPE2 Base;
  UINT8              Strings[sizeof(TYPE2_STRINGS)];
} ARM_TYPE2;

typedef struct {
  SMBIOS_TABLE_TYPE3 Base;
  UINT8              Strings[sizeof(TYPE3_STRINGS)];
} ARM_TYPE3;

typedef struct {
  SMBIOS_TABLE_TYPE4 Base;
  UINT8              Strings[sizeof(TYPE4_STRINGS)];
} ARM_TYPE4;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  UINT8              Strings[sizeof(TYPE7_L1DATA_STRINGS)];
} ARM_TYPE7_L1DATA;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  UINT8              Strings[sizeof(TYPE7_L1INS_STRINGS)];
} ARM_TYPE7_L1INS;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  UINT8              Strings[sizeof(TYPE7_L2_STRINGS)];
} ARM_TYPE7_L2;

typedef struct {
  SMBIOS_TABLE_TYPE7 Base;
  UINT8              Strings[sizeof(TYPE7_L3_STRINGS)];
} ARM_TYPE7_L3;


typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings[sizeof(TYPE9_STRINGS)];
} ARM_TYPE9;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings[sizeof(TYPE9_STRINGS_PCIE0X16)];
} ARM_TYPE9_PCIE0X16;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings[sizeof(TYPE9_STRINGS_PCIE0X1)];
} ARM_TYPE9_PCIE0X1;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings[sizeof(TYPE9_STRINGS_PCIE1X16)];
} ARM_TYPE9_PCIE1X16;

typedef struct {
  SMBIOS_TABLE_TYPE9 Base;
  UINT8              Strings[sizeof(TYPE9_STRINGS_PCIE1X1)];
} ARM_TYPE9_PCIE1X1;


typedef struct {
  SMBIOS_TABLE_TYPE13 Base;
  UINT8              Strings[sizeof(TYPE13_STRINGS)];
} ARM_TYPE13;

typedef struct {
  SMBIOS_TABLE_TYPE16 Base;
  UINT8              Strings[sizeof(TYPE16_STRINGS)];
} ARM_TYPE16;

typedef struct {
  SMBIOS_TABLE_TYPE17 Base;
  UINT8              Strings[sizeof(TYPE17_STRINGS)];
} ARM_TYPE17;

typedef struct {
  SMBIOS_TABLE_TYPE19 Base;
  UINT8              Strings[sizeof(TYPE19_STRINGS)];
} ARM_TYPE19;

typedef struct {
  SMBIOS_TABLE_TYPE32 Base;
  UINT8              Strings[sizeof(TYPE32_STRINGS)];
} ARM_TYPE32;

typedef struct {
  SMBIOS_TABLE_TYPE38 Base;
  UINT8              Strings[sizeof(TYPE38_STRINGS)];
} ARM_TYPE38;

typedef struct {
  SMBIOS_TABLE_TYPE39 Base;
  UINT8              Strings[sizeof(TYPE39_STRINGS)];
} ARM_TYPE39;

enum SMBIOS_REFRENCE_HANDLES {
  SMBIOS_HANDLE_L1I = 0x1000,
  SMBIOS_HANDLE_L1D,
  SMBIOS_HANDLE_L2,
  SMBIOS_HANDLE_L3,
  SMBIOS_HANDLE_MOTHERBOARD,
  SMBIOS_HANDLE_CHASSIS,
  SMBIOS_HANDLE_CLUSTER,
  SMBIOS_HANDLE_MEMORY,
  SMBIOS_HANDLE_DIMM_0,
  SMBIOS_HANDLE_DIMM_1
};


#define SERIAL_LEN 10  //this must be less than the buffer len allocated in the type1 structure

#pragma pack()

//BIOS Information (Type 0)
ARM_TYPE0  BiosInfo_Type0 = {
    {
	    { // SMBIOS_STRUCTURE Hdr
	      EFI_SMBIOS_TYPE_BIOS_INFORMATION, // UINT8 Type
	      sizeof (SMBIOS_TABLE_TYPE0),      // UINT8 Length
	      SMBIOS_HANDLE_PI_RESERVED
	    },
	    1,                                         //Vendor
	    2.2,                                         //BiosVersion
	    0x8800,                                    //BiosSegment
	    3,                                         //BiosReleaseDate
	    0xFF,                                         //BiosSize
	    {                                          //BiosCharacteristics
	        0,                                         // Reserved                          :2
	        0,                                         // Unknown                           :1
	        0,                                         // BiosCharacteristicsNotSupported   :1
	        0,                                         // IsaIsSupported                    :1
	        0,                                         // McaIsSupported                    :1
	        0,                                         // EisaIsSupported                   :1
	        1,                                         // PciIsSupported                    :1
	        0,                                         // PcmciaIsSupported                 :1
	        0,                                         // PlugAndPlayIsSupported            :1
	        0,                                         // ApmIsSupported                    :1
	        1,                                         // BiosIsUpgradable                  :1
	        0,                                         // BiosShadowingAllowed              :1
	        0,                                         // VlVesaIsSupported                 :1
	        0,                                         // EscdSupportIsAvailable            :1
	        0,                                         // BootFromCdIsSupported             :1
	        1,                                         // SelectableBootIsSupported         :1
	        0,                                         // RomBiosIsSocketed                 :1
	        0,                                         // BootFromPcmciaIsSupported         :1
	        0,                                         // EDDSpecificationIsSupported       :1
	        0,                                         // JapaneseNecFloppyIsSupported      :1
	        0,                                         // JapaneseToshibaFloppyIsSupported  :1
	        0,                                         // Floppy525_360IsSupported          :1
	        0,                                         // Floppy525_12IsSupported           :1
	        0,                                         // Floppy35_720IsSupported           :1
	        0,                                         // Floppy35_288IsSupported           :1
	        0,                                         // PrintScreenIsSupported            :1
	        0,                                         // Keyboard8042IsSupported           :1
	        0,                                         // SerialIsSupported                 :1
	        0,                                         // PrinterIsSupported                :1
	        0,                                         // CgaMonoIsSupported                :1
	        0,                                         // NecPc98                           :1
	        0                                         // ReservedForVendor                 :3
	    },
	    {
	        0x03,                                        //BIOSCharacteristicsExtensionBytes[0]
	        //  {                                          //BiosReserved
	        //    1,                                         // AcpiIsSupported                   :1
	        //    1,                                         // UsbLegacyIsSupported              :1
	        //    0,                                         // AgpIsSupported                    :1
	        //    0,                                         // I20BootIsSupported                :1
	        //    0,                                         // Ls120BootIsSupported              :1
	        //    0,                                         // AtapiZipDriveBootIsSupported      :1
	        //    0,                                         // Boot1394IsSupported               :1
	        //    0                                          // SmartBatteryIsSupported           :1
	        //  },
	        0x0D                                         //BIOSCharacteristicsExtensionBytes[1]
	        //  {                                          //SystemReserved
	        //    1,                                         //BiosBootSpecIsSupported            :1
	        //    0,                                         //FunctionKeyNetworkBootIsSupported  :1
	        //    1,                                         //TargetContentDistributionEnabled   :1
	        //    1,                                         //UefiSpecificationSupported         :1
	        //    0,                                         //VirtualMachineSupported            :1
	        //    0                                          //ExtensionByte2Reserved             :3
	        //  },
	    },
	    0xFF,                                     //SystemBiosMajorRelease;
	    0xFF,                                     //SystemBiosMinorRelease;
	    0xFF,                                     //EmbeddedControllerFirmwareMajorRelease;
	    0xFF,                                      //EmbeddedControllerFirmwareMinorRelease;
	},
	TYPE0_STRINGS
};



//System Information (Type 1).
ARM_TYPE1 SystemInfo_Type1 = {
    {
	    {                                               // Hdr
	        EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,         // Type,
	        sizeof (SMBIOS_TABLE_TYPE1),      			// UINT8 Length
	        SMBIOS_HANDLE_PI_RESERVED                   // Handle
	    },
	    1,                                              // Manufacturer
	    2,                                              // ProductName
	    3,                                              // Version
	    4,                                              // SerialNumber
	    {                                               // Uuid
	        0x12345678, 0x1234, 0x5678, {0x90, 0xab, 0xcd, 0xde, 0xef, 0xaa, 0xbb, 0xcc}
	    },
	    SystemWakeupTypePowerSwitch,                    // SystemWakeupType
	    5,                                              // SKUNumber,
	    6                                               // Family
	},
	TYPE1_STRINGS
};


//Base Board (or Module) Information (Type 2)
ARM_TYPE2 BaseboardInfo_Type2 = {
    {
	    {                                                       // Hdr
	        EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,                // Type,
			sizeof (SMBIOS_TABLE_TYPE2),				// UINT8 Length
			SMBIOS_HANDLE_MOTHERBOARD					// Handle

	    },
	    1,                                                      // BaseBoardManufacturer
	    2,                                                      // BaseBoardProductName
	    3,                                                      // BaseBoardVersion
	    4,                                                      // BaseBoardSerialNumber
	    5,                                                      // BaseBoardAssetTag
	    {                                                       // FeatureFlag
	        1,                                                    // Motherboard           :1
	        0,                                                    // RequiresDaughterCard  :1
	        0,                                                    // Removable             :1
	        1,                                                    // Replaceable           :1
	        0,                                                    // HotSwappable          :1
	        0                                                     // Reserved              :3
	    },
	    6,                                                      // BaseBoardChassisLocation
	    0,                                                      // ChassisHandle;
	    BaseBoardTypeMotherBoard,                               // BoardType;
	    0,                                                      // NumberOfContainedObjectHandles;
	    {
	        0
	    }                                                       // ContainedObjectHandles[1];
	},
	TYPE2_STRINGS
};


//System Enclosure or Chassis (Type 3)
ARM_TYPE3 SystemEnclosure_Type3 = {
    {
	    {                                                       // Hdr
	        EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE  ,                   // Type,
			sizeof (SMBIOS_TABLE_TYPE3),				// UINT8 Length
			SMBIOS_HANDLE_CHASSIS					// Handle

	    },
	    1,                                                      // Manufactrurer
	    MiscChassisTypeMainServerChassis,                       // Type
	    2,                                                      // Version
	    3,                                                      // SerialNumber
	    4,                                                      // AssetTag
	    ChassisStateSafe,                                       // BootupState
	    ChassisStateSafe,                                       // PowerSupplyState
	    ChassisStateSafe,                                       // ThermalState
	    ChassisSecurityStatusNone,                              // SecurityState
	    {
	        0,                                                    // OemDefined[0]
	        0,                                                    // OemDefined[1]
	        0,                                                    // OemDefined[2]
	        0                                                     // OemDefined[3]
	    },
	    2,                                                      // Height
	    1,                                                      // NumberofPowerCords
	    0,                                                      // ContainedElementCount
	    0,                                                      // ContainedElementRecordLength
	    {                                                       // ContainedElements[0]
	        {
	            0,                                                    // ContainedElementType
	            0,                                                    // ContainedElementMinimum
	            0                                                     // ContainedElementMaximum
	        }
	    }
	},
	TYPE3_STRINGS
};


//System Boot Information (Type 32)
ARM_TYPE32 SystemBoot_Type32 = {
	{
		{													  // Hdr
		  EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION,				// Type,
		  sizeof (SMBIOS_TABLE_TYPE32),				  // UINT8 Length
		  SMBIOS_HANDLE_PI_RESERVED
		},
		{													  // Reserved[6]
		  0,
		  0,
		  0,
		  0,
		  0,
		  0
		},
		BootInformationStatusNoError						  // BootInformationStatus
	},
	TYPE32_STRINGS
};

//Processor Infomation (Type 4)
ARM_TYPE4 ProcessorInfo_Type4 = {
	{
		{												//Header
			EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION,		//Type
			sizeof(SMBIOS_TABLE_TYPE4), 				//Length
			SMBIOS_HANDLE_CLUSTER					    //Handle
		},
		1,												//Socket
		CentralProcessor,								//ProcessorType
		ProcessorFamilyIndicatorFamily2,				//ProcessorFamily
		2,												//ProcessorManufacture
		{												//ProcessorId
			{											//Signature
				0
			},
			{											//FeatureFlags
				0
			}
		},
		3,												//ProcessorVersion
		{												//Voltage
			0
		},
		1, 								//ExternalClock
		1,									//MaxSpeed
		0,												//CurrentSpeed
		0x41,												//Status
		ProcessorUpgradeUnknown,						//ProcessorUpgrade
		SMBIOS_HANDLE_L1D, 										//L1Ins
		SMBIOS_HANDLE_L2, 										//L1Data
		SMBIOS_HANDLE_L3, 											//L2
		4,												//SerialNumber
		5,												//AssetTag
		6,												//PartNumber

		64,												//CoreCount
		64,												//EnabledCoreCount
		1,												//ThreadCount
		0x00EC,												//ProcessorCharacteristics

		ProcessorFamilyARMv8,							  //ProcessorFamily2

		0,												//CoreCount2
		0,												//EnabledCoreCount2
		0												//ThreadCount2
	},
	TYPE4_STRINGS
};

//Cache Information (Type7) L1 DATA
ARM_TYPE7_L1DATA L1Data_Type7 = {
    {
        {                                               //Header
            EFI_SMBIOS_TYPE_CACHE_INFORMATION,          //Type
            sizeof(SMBIOS_TABLE_TYPE7),                 //Length
            SMBIOS_HANDLE_L1D							//Handle
        },
        1,                                              //SocketDesignation
        0x0180,                                         //CacheConfiguration
        0,                                         //MaximumCacheSize
        0,                                             //InstalledSize
        {                                               //SupportedSRAMType
            0,1,0,0,0,0,0,0
        },
        {                                               //CurrentSRAMType
            0,1,0,0,0,0,0,0
        },
        0,                                              //CacheSpeed
        CacheErrorSingleBit,                            //ErrorCorrectionType
        CacheTypeData,                                  //SystemCacheType
        CacheAssociativity2Way,                          //Associativity
        0x1000,
        0x1000
    },
	TYPE7_L1DATA_STRINGS
};

//Cache Information (Type7) L1 INS
ARM_TYPE7_L1INS L1Ins_Type7 = {
    {
        {                                               //Header
            EFI_SMBIOS_TYPE_CACHE_INFORMATION,          //Type
            sizeof(SMBIOS_TABLE_TYPE7),                 //Length
            SMBIOS_HANDLE_L1I							//Handle
        },
        1,                                              //SocketDesignation
        0x0180,                                              //CacheConfiguration
        0,                                              //MaximumCacheSize
        0,                                             //InstalledSize
        {                                               //SupportedSRAMType
           0,1,0,0,0,0,0,0
        },
        {                                               //CurrentSRAMType
           0,1,0,0,0,0,0,0
        },
        0,                                              //CacheSpeed
        CacheErrorParity,                            //ErrorCorrectionType
        CacheTypeInstruction,                                  //SystemCacheType
        CacheAssociativity2Way,                          //Associativity
        0x1000,
        0x1000
    },
	TYPE7_L1INS_STRINGS
};

//Cache Information (Type7) L2
ARM_TYPE7_L2 L2_Type7 = {
    {
        {                                               //Header
            EFI_SMBIOS_TYPE_CACHE_INFORMATION,          //Type
            sizeof(SMBIOS_TABLE_TYPE7),                 //Length
            SMBIOS_HANDLE_L2							//Handle
        },
        1,                                              //SocketDesignation
        0x0281,                                         //CacheConfiguration
        0,                                         //MaximumCacheSize
        0,                                             //InstalledSize
        {                                               //SupportedSRAMType
           0,1,0,0,0,0,0,0
        },
        {                                               //CurrentSRAMType
           0,1,0,0,0,0,0,0
        },
        0,                                              //CacheSpeed
        CacheErrorSingleBit,                            //ErrorCorrectionType
        CacheTypeUnified,                                  //SystemCacheType
        CacheAssociativity16Way,                          //Associativity
        0x10000,
        0x10000
    },
	TYPE7_L2_STRINGS
};

//Cache Information (Type7) L3
ARM_TYPE7_L3 L3_Type7 = {
    {
        {                                               //Header
            EFI_SMBIOS_TYPE_CACHE_INFORMATION,          //Type
            sizeof(SMBIOS_TABLE_TYPE7),                 //Length
            SMBIOS_HANDLE_L3							//Handle
        },
        1,                                              //SocketDesignation
        0x0281,                                         //CacheConfiguration
        0,                                         //MaximumCacheSize
        0,                                             //InstalledSize
        {                                               //SupportedSRAMType
           0,1,0,0,0,0,0,0
        },
        {                                               //CurrentSRAMType
           0,1,0,0,0,0,0,0
        },
        0,                                              //CacheSpeed
        CacheErrorSingleBit,                            //ErrorCorrectionType
        CacheTypeUnified,                                  //SystemCacheType
        CacheAssociativity8Way,                          //Associativity
        0x20000,
        0x20000
    },
	TYPE7_L3_STRINGS
};


//Bios Language Information (Type13)
ARM_TYPE13 BiosLangInfo_Type13 = {
	{
        { // SMBIOS_STRUCTURE Hdr
	      EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION, // UINT8 Type
	      sizeof (SMBIOS_TABLE_TYPE13),      // UINT8 Length
	      SMBIOS_HANDLE_PI_RESERVED
	    },
        2,
        0,
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        2
	},
	TYPE13_STRINGS
};

//Physical Memory Array (Type 16)
ARM_TYPE16 PhyMemArray_Type16 = {
	{
		{ // SMBIOS_STRUCTURE Hdr
	      EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY, // UINT8 Type
	      sizeof (SMBIOS_TABLE_TYPE16),      // UINT8 Length
	      SMBIOS_HANDLE_MEMORY
	    },
		MemoryArrayLocationSystemBoard,
		MemoryArrayUseSystemMemory,
		MemoryErrorCorrectionNone,
		0x04000000,
		0xFFFE,
		2
	},
	TYPE16_STRINGS
};

//Memory Device (Type17)
ARM_TYPE17 MemDev_Type17 = {
  {
    { // SMBIOS_STRUCTURE Hdr
      EFI_SMBIOS_TYPE_MEMORY_DEVICE, // UINT8 Type
      sizeof (SMBIOS_TABLE_TYPE17),  // UINT8 Length
      SMBIOS_HANDLE_DIMM_0
    },
    SMBIOS_HANDLE_MEMORY, 					 //MemoryArrayHandle
    0xFFFE,               					 //MemoryErrorInformationHandle
    76, 														 //TotalWidth  8bit error-correction
    64,                              //DataWidth
    0x4000, 												 //Size: 16GB
    0x09,                            //FormFactor:DIMM
    0,                               //DeviceSet: Not part of a set
    1,      												 //DeviceLocator
    2,      												 //BankLocator
    MemoryTypeDdr4,                  //MemoryType: DDR4
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}, //TypeDetail
    2666,                            //Speed
    3, 				                 //Manufacturer
    4,                               //SerialNumber
    5,                               //AssetTag
    6,                               //PartNumber
    0,                               //Attributes
    0x0,			                     //ExtendedSize
    2400,			                       //ConfiguredMemoryClockSpeed
    1500,			                       //MinimumVoltage
    1500,			                       //MaximumVoltage
    0,				                       //MaximumVoltage
  },
  TYPE17_STRINGS
};

//Memory Array Mapped Address (Type 19)
ARM_TYPE19 MemArrayMapAddr_Type19 = {
	{
	  {  // SMBIOS_STRUCTURE Hdr
		EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS, // UINT8 Type
		sizeof (SMBIOS_TABLE_TYPE19),				 // UINT8 Length
		SMBIOS_HANDLE_PI_RESERVED
	  },
	  0, //invalid, look at extended addr field
	  0x1FFFFFF,
	  SMBIOS_HANDLE_MEMORY, //handle
	  2,
	  0,		  //starting addr of first 2GB
	  0,		  //ending addr of first 2GB
	},
	TYPE19_STRINGS

};

//IPMI Device Infomation (Type38)
ARM_TYPE38 IPMIDevInfo_Type38 = {
	{
	  {  // SMBIOS_STRUCTURE Hdr
		EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION, // UINT8 Type
		sizeof (SMBIOS_TABLE_TYPE38),				 // UINT8 Length
		SMBIOS_HANDLE_PI_RESERVED
	  },
	  1,
	  0x20,
	  0x0,
	  0xff,
	  0x20000ca2,
	  0,
	  0
	},
	TYPE38_STRINGS
};

//System Power Supply (Type39)
ARM_TYPE39 SysPowerSupply_Type39 = {
	{
	  {  // SMBIOS_STRUCTURE Hdr
		EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY, // UINT8 Type
		sizeof (SMBIOS_TABLE_TYPE39),				 // UINT8 Length
		SMBIOS_HANDLE_PI_RESERVED
	  },
	  1,
	  1,
	  2,
	  3,
	  4,
	  5,
	  6,
	  7,
	  0x8000,
	  {0,1,0,2,3,4,0},
	  0xFFFF,
	  0xFFFF,
	  0xFFFF
	},
	TYPE39_STRINGS
};

//PCIE0_X16		(Type 9)
ARM_TYPE9_PCIE0X16 Pcie0X16_Type9 = {
	{
		{  // SMBIOS_STRUCTURE Hdr
		  EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
		  sizeof (SMBIOS_TABLE_TYPE9), 			   // UINT8 Length
		  SMBIOS_HANDLE_PI_RESERVED
		},
		  1,
		  SlotTypePciExpressGen3X16,
		  SlotDataBusWidth16X,
		  SlotUsageInUse,
		  SlotLengthLong,
		  0,
		  {0,0,1,1,0,0,0,0}, //unknown
		  {1,0,0,0,0},	//PME and SMBUS
		  0,
		  0,
		  0,
	},
    TYPE9_STRINGS_PCIE0X16
};

//PCIE0_X1		(Type 9)
ARM_TYPE9_PCIE0X1 Pcie0X1_Type9 = {
	{
		{  // SMBIOS_STRUCTURE Hdr
		  EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
		  sizeof (SMBIOS_TABLE_TYPE9), 			   // UINT8 Length
		  SMBIOS_HANDLE_PI_RESERVED
		},
		  1,
		  SlotTypePciExpressGen3X1,
		  SlotDataBusWidth1X,
		  SlotUsageAvailable,
		  SlotLengthShort,
		  1,
		  {0,0,1,1,0,0,0,0}, //unknown
		  {1,0,0,0,0},	//PME and SMBUS
		  0xFF,
		  0xFF,
		  0xFF,
	},
    TYPE9_STRINGS_PCIE0X1
};

//PCIE1_X16		(Type 9)
ARM_TYPE9_PCIE1X16 Pcie1X16_Type9 = {
	{
		{  // SMBIOS_STRUCTURE Hdr
		  EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
		  sizeof (SMBIOS_TABLE_TYPE9), 			   // UINT8 Length
		  SMBIOS_HANDLE_PI_RESERVED
		},
		  1,
		  SlotTypePciExpressGen3X16,
		  SlotDataBusWidth16X,
		  SlotUsageAvailable,
		  SlotLengthLong,
		  2,
		  {0,0,1,1,0,0,0,0}, //unknown
		  {1,0,0,0,0},	//PME and SMBUS
		  0xFF,
	      0xFF,
		  0xFF,
	},
    TYPE9_STRINGS_PCIE1X16
};

//PCIE1_X1 	(Type 9)
ARM_TYPE9_PCIE1X1 Pcie1X1_Type9 = {
	{
		{  // SMBIOS_STRUCTURE Hdr
		  EFI_SMBIOS_TYPE_SYSTEM_SLOTS, // UINT8 Type
		  sizeof (SMBIOS_TABLE_TYPE9), 			   // UINT8 Length
		  SMBIOS_HANDLE_PI_RESERVED
		},
		  1,
		  SlotTypePciExpressGen3X1,
		  SlotDataBusWidth1X,
		  SlotUsageAvailable,
		  SlotLengthShort,
		  3,
		  {0,0,1,1,0,0,0,0}, //unknown
		  {1,0,0,0,0},	//PME and SMBUS
		  0xFF,
		  0xFF,
		  0xFF,
	},
    TYPE9_STRINGS_PCIE1X1
};

VOID *DefaultCommonTables[]=
{
  &BiosInfo_Type0,
  &SystemInfo_Type1,
  &BaseboardInfo_Type2,
  &SystemEnclosure_Type3,
  &SystemBoot_Type32,
  &L1Data_Type7,
  &L1Ins_Type7,
  &L2_Type7,
  &L3_Type7,
  &ProcessorInfo_Type4,
  &BiosLangInfo_Type13,
  &SysPowerSupply_Type39,
  &Pcie0X16_Type9,
  &Pcie0X1_Type9,
  &Pcie1X16_Type9,
  &Pcie1X1_Type9,
  NULL
};


/**
   Installed a whole table worth of structructures.

   @param[in] Smbios      The Pointer of Smbios Protocol.

   @retval EFI_SUCCESS    Table data successfully installed.
   @retval Other          Table data was not installed.

**/
EFI_STATUS
InstallStructures (
  IN EFI_SMBIOS_PROTOCOL       *Smbios,
  IN VOID *DefaultTables[]
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE         SmbiosHandle;

  UINT32 TableEntry;
  for ( TableEntry=0; DefaultTables[TableEntry] != NULL; TableEntry++)
  {
    SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER*)DefaultTables[TableEntry])->Handle;
    Status = Smbios->Add (
    Smbios,
    NULL,
    &SmbiosHandle,
    (EFI_SMBIOS_TABLE_HEADER*) DefaultTables[TableEntry]
    );
    if (EFI_ERROR(Status))
      break;
    }

  return Status;
}


/**
   Installed All SMBIOS information.

   @param[in] Smbios      The Pointer of Smbios Protocol.

   @retval EFI_SUCCESS    SMBIOS information successfully installed.
   @retval Other          SMBIOS information was not installed.

**/
STATIC
EFI_STATUS
InstallAllStructures (
  IN EFI_SMBIOS_PROTOCOL       *Smbios
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = InstallStructures (Smbios, DefaultCommonTables);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
   Find the smbios protocol and installed SMBIOS information
   for ARM platforms.

   @param[in] ImageHandle     Module's image handle.
   @param[in] SystemTable     Pointer of EFI_SYSTEM_TABLE.

   @retval EFI_SUCCESS    Smbios data successfully installed.
   @retval Other          Smbios data was not installed.

**/
EFI_STATUS
EFIAPI
SmbiosTablePublishEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;

  //
  // Find the SMBIOS protocol
  //
  Status = gBS->LocateProtocol (
    &gEfiSmbiosProtocolGuid,
    NULL,
    (VOID**)&Smbios
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InstallAllStructures (Smbios);

  return Status;
}
