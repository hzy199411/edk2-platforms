/** @file
  Phytium ACPI ASL Sources.

  Copyright (C) 2020, Phytium Technology Co Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope(_SB)
{
    Device (CLU0) {
      Name(_HID, "ACPI0010")
      Name(_UID, 0)
      Method (_STA, 0, NotSerialized) {
        Return(0x0F)
      }
      Device(CPU0) {
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c0"},
          Package () {"clock-domain",0},
          }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
        }
     }

      Device(CPU1) {
        Name(_HID, "ACPI0007")
        Name(_UID, 1)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
            Package () {"clock-name","c0"},
            Package () {"clock-domain",0},
          }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
        }
      }
    }

    Device (CLU1) {
      Name(_HID, "ACPI0010")
      Name(_UID, 1)
      Method (_STA, 0, NotSerialized) {
        Return(0x0F)
      }
      Device(CPU2) {
        Name(_HID, "ACPI0007")
        Name(_UID, 2)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c1"},
          Package () {"clock-domain",1},
        }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
        }
      }

      Device(CPU3) {
        Name(_HID, "ACPI0007")
        Name(_UID, 3)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c1"},
          Package () {"clock-domain",1},
          }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
       }
      }
    }

    Device (CLU2) {
      Name(_HID, "ACPI0010")
      Name(_UID, 2)
      Method (_STA, 0, NotSerialized) {
        Return(0x0F)
      }
      Device(CPU4) {
        Name(_HID, "ACPI0007")
        Name(_UID, 4)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c2"},
          Package () {"clock-domain",2},
        }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
        }
      }

      Device(CPU5) {
        Name(_HID, "ACPI0007")
        Name(_UID, 5)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c2"},
          Package () {"clock-domain",2},
          }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
       }
      }
    }

    Device (CLU3) {
      Name(_HID, "ACPI0010")
      Name(_UID, 3)
      Method (_STA, 0, NotSerialized) {
        Return(0x0F)
      }
      Device(CPU6) {
        Name(_HID, "ACPI0007")
        Name(_UID, 6)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c3"},
          Package () {"clock-domain",3},
        }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
        }
      }

      Device(CPU7) {
        Name(_HID, "ACPI0007")
        Name(_UID, 7)
        Name (_DSD, Package () {
          ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
          Package () {
          Package () {"clock-name","c3"},
          Package () {"clock-domain",3},
          }
        })
        Method (_STA, 0, NotSerialized) {
          Return(0x0F)
       }
      }
    }
}
