/** @file
  Phytium ACPI ASL Sources.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Platform.h>

#define LNK_DEVICE(Unique_Id, Link_Name, irq)                                 \
  Device(Link_Name) {                                                         \
    Name(_HID, EISAID("PNP0C0F"))                                             \
    Name(_UID, Unique_Id)                                                     \
    Name(_PRS, ResourceTemplate() {                                           \
        Interrupt(ResourceProducer, Level, ActiveHigh, Exclusive) { irq }     \
    })                                                                        \
    Method (_CRS, 0) { Return (_PRS) }                                        \
    Method (_SRS, 1) { }                                                      \
    Method (_DIS) { }                                                         \
  }

#define PRT_ENTRY(Address, Pin, Link)                  \
        Package (4) {                                                                             \
            Address,    /* uses the same format as _ADR */                                        \
            Pin,        /* The PCI pin number of the device (0-INTA, 1-INTB, 2-INTC, 3-INTD). */  \
            Link,       /* Interrupt allocated via Link device. */                             \
            Zero        /* global system interrupt number (no used) */          \
          }

/*
  See Reference [1] 6.1.1
  "High word–Device #, Low word–Function #. (for example, device 3, function 2 is
   0x00030002). To refer to all the functions on a device #, use a function number of FFFF)."
*/
#define ROOT_PRT_ENTRY(Dev, Pin, Link)   PRT_ENTRY(Dev * 0x10000 + 0xFFFF, Pin, Link)


DefinitionBlock("SsdtPci.aml", "SSDT", 1, "ARMLTD", "PHYTIUM ", EFI_ACPI_PHYTIUM_OEM_REVISION) {
  Scope(_SB) {
  
  External (\_SB.DBGC, MethodObj)
  //
  // PCI Root Complex
  //
  LNK_DEVICE(1, LNKA, 0x98)
  LNK_DEVICE(2, LNKB, 0xA0)
  LNK_DEVICE(3, LNKC, 0xA8)
  LNK_DEVICE(4, LNKD, 0xB0)

  LNK_DEVICE(5, LNKE, 0x99)
  LNK_DEVICE(6, LNKF, 0xA1)
  LNK_DEVICE(7, LNKG, 0xA9)
  LNK_DEVICE(8, LNKH, 0xB1)

  LNK_DEVICE(5, LNKI, 0x9a)
  LNK_DEVICE(6, LNKJ, 0xA2)
  LNK_DEVICE(7, LNKK, 0xAa)
  LNK_DEVICE(8, LNKL, 0xB1)

  LNK_DEVICE(5, LNKM, 0x9b)
  LNK_DEVICE(6, LNKN, 0xA3)
  LNK_DEVICE(7, LNKO, 0xAb)
  LNK_DEVICE(8, LNKP, 0xB3)
  // reserve ECAM memory range
  Device(RES0)
  {
     Name(_HID, EISAID("PNP0C02"))
     Name(_UID, 0)
     Name(_CRS, ResourceTemplate() { 

      QWordMemory (ResourceConsumer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
          0x0, // Granularity
          0x40000000, // Range Minimum
          0x4FFFFFFF, // Range Maximum
          0, // Translation Offset
          0x10000000, // Length
          ,,)
    })
  }

  Device(RES1)
  {
     Name(_HID, EISAID("PNP0C02"))
     Name(_UID, 1)
     Name(_CRS, ResourceTemplate() { 

      QWordMemory (ResourceConsumer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
          0x0, // Granularity
          0x20040000000, // Range Minimum
          0x2004FFFFFFF, // Range Maximum
          0, // Translation Offset
          0x10000000, // Length
          ,,)
    })
  }

  Device(RES2)
  {
     Name(_HID, EISAID("PNP0C02"))
     Name(_UID, 2)
     Name(_CRS, ResourceTemplate() { 

      QWordMemory (ResourceConsumer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
          0x0, // Granularity
          0x40040000000, // Range Minimum
          0x4004FFFFFFF, // Range Maximum
          0, // Translation Offset
          0x10000000, // Length
          ,,)
    })
  }

  Device(RES3)
  {
     Name(_HID, EISAID("PNP0C02"))
     Name(_UID, 3)
     Name(_CRS, ResourceTemplate() { 

      QWordMemory (ResourceConsumer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
          0x0, // Granularity
          0x60040000000, // Range Minimum
          0x6004FFFFFFF, // Range Maximum
          0, // Translation Offset
          0x10000000, // Length
          ,,)
    })
  }

  Device(PCI0)
  {
      Name(_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name(_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name(_SEG, Zero) // PCI Segment Group number
      Name(_BBN, 0)    // PCI Base Bus Number
      Name(_CCA, 1)    // Initially mark the PCI coherent (for JunoR1)
      Name(_PXM, 0)

      // Root Complex
      Device (RP0) {
          Name(_ADR, 0x00000000)    // Dev 0, Func 0
      }
    // PCI Routing Table
    Name(_PRT, Package() {
      ROOT_PRT_ENTRY(0, 0, LNKA),   // INTA
      ROOT_PRT_ENTRY(0, 1, LNKB),   // INTB
      ROOT_PRT_ENTRY(0, 2, LNKC),   // INTC
      ROOT_PRT_ENTRY(0, 3, LNKD),   // INTD

      ROOT_PRT_ENTRY(1, 0, LNKA),   // INTA
      ROOT_PRT_ENTRY(1, 1, LNKB),   // INTB
      ROOT_PRT_ENTRY(1, 2, LNKC),   // INTC
      ROOT_PRT_ENTRY(1, 3, LNKD),   // INTD

      ROOT_PRT_ENTRY(2, 0, LNKA),   // INTA
      ROOT_PRT_ENTRY(2, 1, LNKB),   // INTB
      ROOT_PRT_ENTRY(2, 2, LNKC),   // INTC
      ROOT_PRT_ENTRY(2, 3, LNKD),   // INTD

    })

        // Root complex resources
    Method (_CRS, 0, Serialized) {
      Name (RBUF, ResourceTemplate () {
        WordBusNumber ( // Bus numbers assigned to this root
          ResourceProducer,
          MinFixed, MaxFixed, PosDecode,
          0,   // AddressGranularity
          0,   // AddressMinimum - Minimum Bus Number
          255, // AddressMaximum - Maximum Bus Number
          0,   // AddressTranslation - Set to 0
          256  // RangeLength - Number of Busses
        )

        DWordMemory ( // 32-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x60000000,               // Min Base Address
          0x7FFFFFFF,               // Max Base Address
          0x00000000,               // Translate
          0x20000000                // Length
        )

        QWordMemory ( // 64-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x8000000000,               // Min Base Address
          0xFFFFFFFFFF,               // Max Base Address
          0x0000000000,               // Translate
          0x8000000000                // Length
        )

        DWordIo ( // IO window
          ResourceProducer,
          MinFixed,
          MaxFixed,
          PosDecode,
          EntireRange,
          0x00000000,               // Granularity
          0x00000000,               // Min Base Address
          0x00efffff,                // Max Base Address
          0x50000000,               // Translate
          0x00f00000,                // Length
          ,,,TypeTranslation
        )
      }) // Name(RBUF)

      Return (RBUF)
    } // Method(_CRS)

  } 
  
  Device(PCI1)
  {
      Name(_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name(_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name(_SEG, 1) // PCI Segment Group number
      Name(_BBN, 0)    // PCI Base Bus Number
      Name(_CCA, 1)    // Initially mark the PCI coherent (for JunoR1)
      Name(_PXM, 8)

      // Root Complex
      //Device (RP0) {
       //   Name(_ADR, 0x00000000)    // Dev 0, Func 0
     // }
    // PCI Routing Table
    Name(_PRT, Package() {
      ROOT_PRT_ENTRY(0, 0, LNKE),   // INTA
      ROOT_PRT_ENTRY(0, 1, LNKF),   // INTB
      ROOT_PRT_ENTRY(0, 2, LNKG),   // INTC
      ROOT_PRT_ENTRY(0, 3, LNKH),   // INTD

      ROOT_PRT_ENTRY(1, 0, LNKE),   // INTA
      ROOT_PRT_ENTRY(1, 1, LNKF),   // INTB
      ROOT_PRT_ENTRY(1, 2, LNKG),   // INTC
      ROOT_PRT_ENTRY(1, 3, LNKH),   // INTD

      ROOT_PRT_ENTRY(2, 0, LNKE),   // INTA
      ROOT_PRT_ENTRY(2, 1, LNKF),   // INTB
      ROOT_PRT_ENTRY(2, 2, LNKG),   // INTC
      ROOT_PRT_ENTRY(2, 3, LNKH),   // INTD

    })

        // Root complex resources
    Method (_CRS, 0, Serialized) {
      Name (RBUF, ResourceTemplate () {
        WordBusNumber ( // Bus numbers assigned to this root
          ResourceProducer,
          MinFixed, MaxFixed, PosDecode,
          0,   // AddressGranularity
          0,   // AddressMinimum - Minimum Bus Number
          255, // AddressMaximum - Maximum Bus Number
          0,   // AddressTranslation - Set to 0
          256  // RangeLength - Number of Busses
        )

        QWordMemory ( // 32-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x60000000,               // Min Base Address
          0x7FFFFFFF,               // Max Base Address
          0x20000000000,               // Translate
          0x20000000                // Length
        )

        QWordMemory ( // 64-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x8000000000,               // Min Base Address
          0xFFFFFFFFFF,               // Max Base Address
          0x20000000000,               // Translate
          0x8000000000                // Length
        )

        QWordIo ( // IO window
          ResourceProducer,
          MinFixed,
          MaxFixed,
          PosDecode,
          EntireRange,
          0x00000000,               // Granularity
          0x00000000,               // Min Base Address
          0x00efffff,                // Max Base Address
          0x20050000000,               // Translate
          0x00f00000,                // Length
          ,,,TypeTranslation
        )
      }) // Name(RBUF)

      Return (RBUF)
    } // Method(_CRS)



    //
    // OS Control Handoff
    //
    Name(SUPP, Zero) // PCI _OSC Support Field value
    Name(CTRL, Zero) // PCI _OSC Control Field value

    /*
    See [1] 6.2.10, [2] 4.5
    */
    Method(_OSC,4) {
      // Check for proper UUID
      If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {
        // Create DWord-adressable fields from the Capabilities Buffer
        CreateDWordField(Arg3,0,CDW1)
        CreateDWordField(Arg3,4,CDW2)
        CreateDWordField(Arg3,8,CDW3)

        // Save Capabilities DWord2 & 3
        Store(CDW2,SUPP)
        Store(CDW3,CTRL)

        // Only allow native hot plug control if OS supports:
        // * ASPM
        // * Clock PM
        // * MSI/MSI-X
        If(LNotEqual(And(SUPP, 0x16), 0x16)) {
          And(CTRL,0x1E,CTRL) // Mask bit 0 (and undefined bits)
        }

        // Do not allow native PME, AER (no dependencies)
        // Never allow SHPC (no SHPC controller in this system)
        And(CTRL,0x10,CTRL)

        If(LNotEqual(Arg1,One)) {  // Unknown revision
          Or(CDW1,0x08,CDW1)
        }

        If(LNotEqual(CDW3,CTRL)) {  // Capabilities bits were masked
          Or(CDW1,0x10,CDW1)
        }
        // Update DWORD3 in the buffer
        Store(CTRL,CDW3)
        //DBGC(1, "_OSC PCI",0)
        //DBGC(2, "CDW1:", CDW1)
        //DBGC(2, "CDW2:", CDW2)
        //DBGC(2, "CDW3:", CDW3)
        Return(Arg3)
      } Else {
        Or(CDW1,4,CDW1) // Unrecognized UUID
        Return(Arg3)
      }
    } // End _OSC
    } // PCI1

    Device(PCI2)
    {
      Name(_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name(_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name(_SEG, 2) // PCI Segment Group number
      Name(_BBN, 0)    // PCI Base Bus Number
      Name(_CCA, 1)    // Initially mark the PCI coherent (for JunoR1)

      // Root Complex
      //Device (RP0) {
       //   Name(_ADR, 0x00000000)    // Dev 0, Func 0
     // }
    // PCI Routing Table
    Name(_PRT, Package() {
      ROOT_PRT_ENTRY(0, 0, LNKI),   // INTA
      ROOT_PRT_ENTRY(0, 1, LNKJ),   // INTB
      ROOT_PRT_ENTRY(0, 2, LNKK),   // INTC
      ROOT_PRT_ENTRY(0, 3, LNKL),   // INTD

      ROOT_PRT_ENTRY(1, 0, LNKI),   // INTA
      ROOT_PRT_ENTRY(1, 1, LNKJ),   // INTB
      ROOT_PRT_ENTRY(1, 2, LNKK),   // INTC
      ROOT_PRT_ENTRY(1, 3, LNKL),   // INTD

      ROOT_PRT_ENTRY(2, 0, LNKI),   // INTA
      ROOT_PRT_ENTRY(2, 1, LNKJ),   // INTB
      ROOT_PRT_ENTRY(2, 2, LNKK),   // INTC
      ROOT_PRT_ENTRY(2, 3, LNKL),   // INTD

    })

    // Root complex resources
    Method (_CRS, 0, Serialized) {
      Name (RBUF, ResourceTemplate () {
        WordBusNumber ( // Bus numbers assigned to this root
          ResourceProducer,
          MinFixed, MaxFixed, PosDecode,
          0,   // AddressGranularity
          0,   // AddressMinimum - Minimum Bus Number
          255, // AddressMaximum - Maximum Bus Number
          0,   // AddressTranslation - Set to 0
          256  // RangeLength - Number of Busses
        )

        QWordMemory ( // 32-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x60000000,               // Min Base Address
          0x7FFFFFFF,               // Max Base Address
          0x40000000000,               // Translate
          0x20000000                // Length
        )

        QWordMemory ( // 64-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x8000000000,               // Min Base Address
          0xFFFFFFFFFF,               // Max Base Address
          0x40000000000,               // Translate
          0x8000000000                // Length
        )

        QWordIo ( // IO window
          ResourceProducer,
          MinFixed,
          MaxFixed,
          PosDecode,
          EntireRange,
          0x00000000,               // Granularity
          0x00000000,               // Min Base Address
          0x00efffff,                // Max Base Address
          0x40050000000,               // Translate
          0x00f00000,                // Length
          ,,,TypeTranslation
        )
      }) // Name(RBUF)

      Return (RBUF)
    } // Method(_CRS)



    //
    // OS Control Handoff
    //
    Name(SUPP, Zero) // PCI _OSC Support Field value
    Name(CTRL, Zero) // PCI _OSC Control Field value

    /*
    See [1] 6.2.10, [2] 4.5
    */
    Method(_OSC,4) {
      // Check for proper UUID
      If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {
        // Create DWord-adressable fields from the Capabilities Buffer
        CreateDWordField(Arg3,0,CDW1)
        CreateDWordField(Arg3,4,CDW2)
        CreateDWordField(Arg3,8,CDW3)

        // Save Capabilities DWord2 & 3
        Store(CDW2,SUPP)
        Store(CDW3,CTRL)

        // Only allow native hot plug control if OS supports:
        // * ASPM
        // * Clock PM
        // * MSI/MSI-X
        If(LNotEqual(And(SUPP, 0x16), 0x16)) {
          And(CTRL,0x1E,CTRL) // Mask bit 0 (and undefined bits)
        }

        // Do not allow native PME, AER (no dependencies)
        // Never allow SHPC (no SHPC controller in this system)
        And(CTRL,0x10,CTRL)

        If(LNotEqual(Arg1,One)) {  // Unknown revision
          Or(CDW1,0x08,CDW1)
        }

        If(LNotEqual(CDW3,CTRL)) {  // Capabilities bits were masked
          Or(CDW1,0x10,CDW1)
        }
        // Update DWORD3 in the buffer
        Store(CTRL,CDW3)
        //DBGC(1, "_OSC PCI",0)
        //DBGC(2, "CDW1:", CDW1)
        //DBGC(2, "CDW2:", CDW2)
        //DBGC(2, "CDW3:", CDW3)
        Return(Arg3)
      } Else {
        Or(CDW1,4,CDW1) // Unrecognized UUID
        Return(Arg3)
      }
    } // End _OSC
    }

    Device(PCI3)
    {
      Name(_HID, EISAID("PNP0A08")) // PCI Express Root Bridge
      Name(_CID, EISAID("PNP0A03")) // Compatible PCI Root Bridge
      Name(_SEG, 3) // PCI Segment Group number
      Name(_BBN, 0)    // PCI Base Bus Number
      Name(_CCA, 1)    // Initially mark the PCI coherent (for JunoR1)

      // Root Complex
      //Device (RP0) {
       //   Name(_ADR, 0x00000000)    // Dev 0, Func 0
     // }
    // PCI Routing Table
    Name(_PRT, Package() {
      ROOT_PRT_ENTRY(0, 0, LNKM),   // INTA
      ROOT_PRT_ENTRY(0, 1, LNKN),   // INTB
      ROOT_PRT_ENTRY(0, 2, LNKO),   // INTC
      ROOT_PRT_ENTRY(0, 3, LNKP),   // INTD

      ROOT_PRT_ENTRY(1, 0, LNKM),   // INTA
      ROOT_PRT_ENTRY(1, 1, LNKN),   // INTB
      ROOT_PRT_ENTRY(1, 2, LNKO),   // INTC
      ROOT_PRT_ENTRY(1, 3, LNKP),   // INTD

      ROOT_PRT_ENTRY(2, 0, LNKM),   // INTA
      ROOT_PRT_ENTRY(2, 1, LNKN),   // INTB
      ROOT_PRT_ENTRY(2, 2, LNKO),   // INTC
      ROOT_PRT_ENTRY(2, 3, LNKP),   // INTD

    })

    // Root complex resources
    Method (_CRS, 0, Serialized) {
      Name (RBUF, ResourceTemplate () {
        WordBusNumber ( // Bus numbers assigned to this root
          ResourceProducer,
          MinFixed, MaxFixed, PosDecode,
          0,   // AddressGranularity
          0,   // AddressMinimum - Minimum Bus Number
          255, // AddressMaximum - Maximum Bus Number
          0,   // AddressTranslation - Set to 0
          256  // RangeLength - Number of Busses
        )

        QWordMemory ( // 32-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x60000000,               // Min Base Address
          0x7FFFFFFF,               // Max Base Address
          0x60000000000,               // Translate
          0x20000000                // Length
        )

        QWordMemory ( // 64-bit BAR Windows
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          Cacheable, ReadWrite,
          0x00000000,               // Granularity
          0x8000000000,               // Min Base Address
          0xFFFFFFFFFF,               // Max Base Address
          0x60000000000,               // Translate
          0x8000000000                // Length
        )

        QWordIo ( // IO window
          ResourceProducer,
          MinFixed,
          MaxFixed,
          PosDecode,
          EntireRange,
          0x00000000,               // Granularity
          0x00000000,               // Min Base Address
          0x00efffff,                // Max Base Address
          0x60050000000,               // Translate
          0x00f00000,                // Length
          ,,,TypeTranslation
        )
      }) // Name(RBUF)

      Return (RBUF)
    } // Method(_CRS)



    //
    // OS Control Handoff
    //
    Name(SUPP, Zero) // PCI _OSC Support Field value
    Name(CTRL, Zero) // PCI _OSC Control Field value

    /*
    See [1] 6.2.10, [2] 4.5
    */
    Method(_OSC,4) {
      // Check for proper UUID
      If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {
        // Create DWord-adressable fields from the Capabilities Buffer
        CreateDWordField(Arg3,0,CDW1)
        CreateDWordField(Arg3,4,CDW2)
        CreateDWordField(Arg3,8,CDW3)

        // Save Capabilities DWord2 & 3
        Store(CDW2,SUPP)
        Store(CDW3,CTRL)

        // Only allow native hot plug control if OS supports:
        // * ASPM
        // * Clock PM
        // * MSI/MSI-X
        If(LNotEqual(And(SUPP, 0x16), 0x16)) {
          And(CTRL,0x1E,CTRL) // Mask bit 0 (and undefined bits)
        }

        // Do not allow native PME, AER (no dependencies)
        // Never allow SHPC (no SHPC controller in this system)
        And(CTRL,0x10,CTRL)

        If(LNotEqual(Arg1,One)) {  // Unknown revision
          Or(CDW1,0x08,CDW1)
        }

        If(LNotEqual(CDW3,CTRL)) {  // Capabilities bits were masked
          Or(CDW1,0x10,CDW1)
        }
        // Update DWORD3 in the buffer
        Store(CTRL,CDW3)
        //DBGC(1, "_OSC PCI",0)
        //DBGC(2, "CDW1:", CDW1)
        //DBGC(2, "CDW2:", CDW2)
        //DBGC(2, "CDW3:", CDW3)
        Return(Arg3)
      } Else {
        Or(CDW1,4,CDW1) // Unrecognized UUID
        Return(Arg3)
      }
    } // End _OSC
    }
  }
}
