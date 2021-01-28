/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __EC_H__
#define __EC_H__
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

UINT8 EcWriteMem(UINT8 Index,UINT8 Data);
UINT8 EcReadMem(UINT8 Index,UINT8 *Data);

#endif
