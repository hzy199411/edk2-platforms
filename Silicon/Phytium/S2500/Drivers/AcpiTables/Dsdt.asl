/** @file
  Phytium ACPI ASL Sources.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Platform.h>


DefinitionBlock("DsdtTable.aml", "DSDT", 2, "ARMLTD", "PHYTIUM ", EFI_ACPI_PHYTIUM_OEM_REVISION) {
  Scope(_SB) {
    Method (_OSC, 4, Serialized)  { // _OSC: Operating System Capabilities
      CreateDWordField (Arg3, 0x00, STS0)
      CreateDWordField (Arg3, 0x04, CAP0)
      If ((Arg0 == ToUUID ("0811b06e-4a27-44f9-8d60-3cbbc22e7b48") /* Platform-wide Capabilities */)) {
      DBGC(2, "_OSC OSPM Arg1:",Arg1)
      DBGC(2, "CAP0:", CAP0)
      DBGC(2, "STS0:", STS0)
        If (!(Arg1 == One)) {
          STS0 &= ~0x1F
          STS0 |= 0x0A
        } Else {
          If ((CAP0 & 0x100)) {
            CAP0 &= ~0x100 /* No support for OS Initiated LPI */
            STS0 &= ~0x1F
            STS0 |= 0x12
          }
        }
      } Else {
        STS0 &= ~0x1F
        STS0 |= 0x06
      }
      Return (Arg3)
    }

    Device (CLU0) { // Cluster0 state
      Name(_HID, "ACPI0010")
      Name(_UID, 1)

      Device(CPU0) { //  Cluster 0, Cpu 0
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
       
      }
      Device(CPU1) { //  Cluster 0, Cpu 1
        Name(_HID, "ACPI0007")
        Name(_UID, 1)
        
      }
      Device(CPU2) { //  Cluster 0, Cpu 2
        Name(_HID, "ACPI0007")
        Name(_UID, 2)
       
      }
      Device(CPU3) { // Cluster 0, Cpu 3
        Name(_HID, "ACPI0007")
        Name(_UID, 3)
       
      }
    }

    Device (CLU1) { // Cluster1 state
      Name(_HID, "ACPI0010")
      Name(_UID, 2)
      
      Device(CPU4) { // Cluster 1, Cpu 0
        Name(_HID, "ACPI0007")
        Name(_UID, 4)
       
      }
      Device(CPU5) { // Cluster 1, Cpu 1
        Name(_HID, "ACPI0007")
        Name(_UID, 5)
       
      }
      Device(CPU6) { // Cluster 1, Cpu 2
        Name(_HID, "ACPI0007")
        Name(_UID, 6)
       
      }
      Device(CPU7) { // Cluster 1, Cpu 3
        Name(_HID, "ACPI0007")
        Name(_UID, 7)
       
      }
    }

    Device (CLU2) { // Cluster2 state
      Name(_HID, "ACPI0010")
      Name(_UID, 3)
      
      Device(CPU8) { // Cluster 2, Cpu 0
        Name(_HID, "ACPI0007")
        Name(_UID, 8)
       
      }
      Device(CPU9) { // Cluster 2, Cpu 1
        Name(_HID, "ACPI0007")
        Name(_UID, 9)
       
      }
      Device(CP10) { // Cluster 2, Cpu 2
        Name(_HID, "ACPI0007")
        Name(_UID, 10)
       
      }
      Device(CP11) { // Cluster 2, Cpu 3
        Name(_HID, "ACPI0007")
        Name(_UID, 11)
       
      }
    }

    Device (CLU3) { // Cluster3 state
      Name(_HID, "ACPI0010")
      Name(_UID, 4)
      
      Device(CP12) { // Cluster 3, Cpu 0
        Name(_HID, "ACPI0007")
        Name(_UID, 12)
       
      }
      Device(CP13) { // Cluster 3, Cpu 1
        Name(_HID, "ACPI0007")
        Name(_UID, 13)
      
      }
      Device(CP14) { // Cluster 3, Cpu 2
        Name(_HID, "ACPI0007")
        Name(_UID, 14)
       
      }
      Device(CP15) { // Cluster 3, Cpu 3
        Name(_HID, "ACPI0007")
        Name(_UID, 15)
       
      }
    }

    Device (CLU4) { // Cluster4 state
      Name(_HID, "ACPI0010")
      Name(_UID, 5)
      
      Device(CP16) {
        Name(_HID, "ACPI0007")
        Name(_UID, 16)
       
      }
      Device(CP17) {
        Name(_HID, "ACPI0007")
        Name(_UID, 17)
        
      }
      Device(CP18) {
        Name(_HID, "ACPI0007")
        Name(_UID, 18)
        
      }
      Device(CP19) {
        Name(_HID, "ACPI0007")
        Name(_UID, 19)
        
      }
    }

    Device (CLU5) { // Cluster5 state
      Name(_HID, "ACPI0010")
      Name(_UID, 6)
      
      
      
      Device(CP20) {
        Name(_HID, "ACPI0007")
        Name(_UID, 20)
        
      }
      Device(CP21) {
        Name(_HID, "ACPI0007")
        Name(_UID, 21)
        
      }
      Device(CP22) {
        Name(_HID, "ACPI0007")
        Name(_UID, 22)
        
      }
      Device(CP23) {
        Name(_HID, "ACPI0007")
        Name(_UID, 23)
        
      }
    }

    Device (CLU6) { // Cluster6 state
      Name(_HID, "ACPI0010")
      Name(_UID, 7)
      
      
      
      Device(CP24) {
        Name(_HID, "ACPI0007")
        Name(_UID, 24)
        
      }
      Device(CP25) {
        Name(_HID, "ACPI0007")
        Name(_UID, 25)
        
      }
      Device(CP26) {
        Name(_HID, "ACPI0007")
        Name(_UID, 26)
        
      }
      Device(CP27) {
        Name(_HID, "ACPI0007")
        Name(_UID, 27)
        
      }
    }

    Device (CLU7) { // Cluster7 state
      Name(_HID, "ACPI0010")
      Name(_UID, 8)
      
      
      
      Device(CP28) {
        Name(_HID, "ACPI0007")
        Name(_UID, 28)
        
      }
      Device(CP29) {
        Name(_HID, "ACPI0007")
        Name(_UID, 29)
        
      }
      Device(CP30) {
        Name(_HID, "ACPI0007")
        Name(_UID, 30)
        
      }
      Device(CP31) {
        Name(_HID, "ACPI0007")
        Name(_UID, 31)
        
      }
    }

    Device (CLU8) { // Cluster8 state
      Name(_HID, "ACPI0010")
      Name(_UID, 9)
      
      
      
      Device(CP32) {
        Name(_HID, "ACPI0007")
        Name(_UID, 32)
        
      }
      Device(CP33) {
        Name(_HID, "ACPI0007")
        Name(_UID, 33)
        
      }
      Device(CP34) {
        Name(_HID, "ACPI0007")
        Name(_UID, 34)
        
      }
      Device(CP35) {
        Name(_HID, "ACPI0007")
        Name(_UID, 35)
        
      }
    }

    Device (CLU9) { // Cluster9 state
      Name(_HID, "ACPI0010")
      Name(_UID, 10)
      
      
      
      Device(CP36) {
        Name(_HID, "ACPI0007")
        Name(_UID, 36)
        
      }
      Device(CP37) {
        Name(_HID, "ACPI0007")
        Name(_UID, 37)
        
      }
      Device(CP38) {
        Name(_HID, "ACPI0007")
        Name(_UID, 38)
        
      }
      Device(CP39) {
        Name(_HID, "ACPI0007")
        Name(_UID, 39)
        
      }
    }

    Device (CLUA) { // ClusterA state
      Name(_HID, "ACPI0010")
      Name(_UID, 11)
      
      
      
      Device(CP40) {
        Name(_HID, "ACPI0007")
        Name(_UID, 40)
        
      }
      Device(CP41) {
        Name(_HID, "ACPI0007")
        Name(_UID, 41)
        
      }
      Device(CP42) {
        Name(_HID, "ACPI0007")
        Name(_UID, 42)
        
      }
      Device(CP43) {
        Name(_HID, "ACPI0007")
        Name(_UID, 43)
        
      }
    }
    
    Device (CLUB) { // ClusterB state
      Name(_HID, "ACPI0010")
      Name(_UID, 12)
      
      
      
      Device(CP44) {
        Name(_HID, "ACPI0007")
        Name(_UID, 44)
        
      }
      Device(CP45) {
        Name(_HID, "ACPI0007")
        Name(_UID, 45)
        
      }
      Device(CP46) {
        Name(_HID, "ACPI0007")
        Name(_UID, 46)
        
      }
      Device(CP47) {
        Name(_HID, "ACPI0007")
        Name(_UID, 47)
        
      }
    }
    
    Device (CLUC) { // ClusterC state
      Name(_HID, "ACPI0010")
      Name(_UID, 13)
      
      
      
      Device(CP48) {
        Name(_HID, "ACPI0007")
        Name(_UID, 48)
        
      }
      Device(CP49) {
        Name(_HID, "ACPI0007")
        Name(_UID, 49)
        
      }
      Device(CP50) {
        Name(_HID, "ACPI0007")
        Name(_UID, 50)
        
      }
      Device(CP51) {
        Name(_HID, "ACPI0007")
        Name(_UID, 51)
        
      }
    }
    
    Device (CLUD) { // ClusterD state
      Name(_HID, "ACPI0010")
      Name(_UID, 14)
      
      
      
      Device(CP52) {
        Name(_HID, "ACPI0007")
        Name(_UID, 52)
        
      }
      Device(CP53) {
        Name(_HID, "ACPI0007")
        Name(_UID, 53)
        
      }
      Device(CP54) {
        Name(_HID, "ACPI0007")
        Name(_UID, 54)
        
      }
      Device(CP55) {
        Name(_HID, "ACPI0007")
        Name(_UID, 55)
        
      }
    }
    
    Device (CLUE) { // ClusterE state
      Name(_HID, "ACPI0010")
      Name(_UID, 15)
      
      
      
      Device(CP56) {
        Name(_HID, "ACPI0007")
        Name(_UID, 56)
        
      }
      Device(CP57) {
        Name(_HID, "ACPI0007")
        Name(_UID, 57)
        
      }
      Device(CP58) {
        Name(_HID, "ACPI0007")
        Name(_UID, 58)
        
      }
      Device(CP59) {
        Name(_HID, "ACPI0007")
        Name(_UID, 59)
        
      }
    }

    Device (CLUF) { // ClusterF state
      Name(_HID, "ACPI0010")
      Name(_UID, 16)
      
      
      
      Device(CP60) {
        Name(_HID, "ACPI0007")
        Name(_UID, 60)
        
      }
      Device(CP61) {
        Name(_HID, "ACPI0007")
        Name(_UID, 61)
        
      }
      Device(CP62) {
        Name(_HID, "ACPI0007")
        Name(_UID, 62)
        
      }
      Device(CP63) {
        Name(_HID, "ACPI0007")
        Name(_UID, 63)
        
      }
    }

    Device (CLUG) { // ClusterF state
      Name(_HID, "ACPI0010")
      Name(_UID, 17)
      
      
      
      Device(CP64) {
        Name(_HID, "ACPI0007")
        Name(_UID, 64)
        
      }
#if 0
      Device(CP61) {
        Name(_HID, "ACPI0007")
        Name(_UID, 61)
        
      }
      Device(CP62) {
        Name(_HID, "ACPI0007")
        Name(_UID, 62)
        
      }
      Device(CP63) {
        Name(_HID, "ACPI0007")
        Name(_UID, 63)
        
      }
#endif
    }

    // UART 8250dw
    Device(COM0) {
      Name(_HID, "BRCM2032")
      Name(_UID, Zero)
      Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
        Name (SER0, ResourceTemplate () {
          QWordMemory (
            ResourceProducer, PosDecode,
            MinFixed, MaxFixed,
            NonCacheable, ReadWrite,
            0x00000000,               // Granularity
            0x28001000,            // Min Base Address
            0x28001fff,            // Max Base Address
            0x00000000,               // Translate
            0x1000,                   // Length
            , , BUFF
          )
          Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 72 }
        })
        CreateQWordField (SER0, \_SB.COM0._CRS.BUFF._MIN, U0BL)
        CreateQWordField (SER0, \_SB.COM0._CRS.BUFF._MAX, U0BH)
        Store (FixedPcdGet64 (PcdSerialRegisterBase), U0BL)
        Store (FixedPcdGet64 (PcdSerialRegisterBase), U0BH)
        Add (U0BH, 0xfff, U0BH)

        Return (SER0)
      }
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"clock-frequency", 0x2faf080},
          Package () {"reg-io-width", 4},
          Package () {"reg-shift", 2},
        }
      })
      Method (_STA, 0, NotSerialized) {
        Return(0x0F)
      }
    }
#if 1
    Device (I2C0) {
      Name (_HID, "PHYT0003")
      Name (_UID, Zero)
      Name (_CRS, ResourceTemplate () {
        QWordMemory (
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          NonCacheable, ReadWrite,
          0x00000000,               // Granularity
          0x28002000,            // Min Base Address
          0x28002fff,            // Max Base Address
          0x00000000,               // Translate
          0x1000,                   // Length
        )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 80 }
      })
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"clock-frequency", 0x186a0},
          Package () {"clocks", 0x44},
        }
      })
      
      Device (RTC0) {
        Name (_HID, "DS139900")
        Name (_CID, "DS1399")
        Name (_UID, Zero)
        Name (_CRS, ResourceTemplate () {
          I2CSerialBusV2 (0x68, ControllerInitiated, 100000, 
                          AddressingMode7Bit, "\\_SB.I2C0", 0, ResourceConsumer, ,)
        })
      }
    }

    Device (I2C1) {
      Name (_HID, "PHYT0003")
      Name (_UID, ONE)
      Name (_CRS, ResourceTemplate () {
        QWordMemory (
          ResourceProducer, PosDecode,
          MinFixed, MaxFixed,
          NonCacheable, ReadWrite,
          0x00000000,               // Granularity
          0x28003000,            // Min Base Address
          0x28003fff,            // Max Base Address
          0x00000000,               // Translate
          0x1000,                   // Length
        )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 88 }
      })
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package () {
          Package () {"clock-frequency", 0x186a0},
          Package () {"clocks", 0x44},
        }
      })
    }
#endif 
	
	Device (IPI0) {
		Name (_HID, "IPI0001")
		Name (_UID, 0)
		Name (_STR, Unicode("IPMI_KCS"))
		Name(_CCA, 1)
		//Name (_CID, "IPI0001")
		Method (_IFT) {
			Return (0x01)
  
		}
		Method (_SRV) {
			Return (0x0200)   // IPMI Spec Revision 2.0
  
		}
		Name (_CRS, ResourceTemplate () { // _CRS: Current Resource Settings
			QWordMemory ( // BMC memory region
			ResourceConsumer,
			PosDecode,
			MinFixed,
			MaxFixed,
			Cacheable,
			ReadWrite,
			0x0, // Granularity
			0x20000ca2, // Min Base Address
			0x20000ca3, // Max Base Address
			0x0, // Translate
			0x2 // Length
			)
			Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 56  }
		})
	}
	
    Method(TCOM, 1, Serialized) {
      OperationRegion(COM0, SystemMemory, FixedPcdGet64 (PcdSerialRegisterBase), 0x1000)
      Field(COM0, DWordAcc, NoLock, Preserve) {
        DAT8, 8,
        Offset(5),
            , 5,
        TRDY, 1,
      }

      Add(SizeOf(Arg0), One, Local0)
      Name(BUF0, Buffer(Local0){})
      Store(Arg0, BUF0)
      store(0, Local1)
      Decrement(Local0)
      While(LNotEqual(Local1, Local0)){
        while(LEqual(TRDY, ONE)){}
        Store(DerefOf(Index(BUF0, Local1)), DAT8)
        Increment(Local1)
      }
    }

    Method(DBGC, 3, Serialized) {      // DBGC(count, string, int)
      Name(CRLF, Buffer(2){0x0D, 0x0A})
      TCOM(Arg1)
      if(LEqual(Arg0, 2)){
        TCOM(ToHexString(Arg2))
      }
      TCOM(CRLF)
    }

  // #include "EC.asl"

  } // Scope(_SB)
}
