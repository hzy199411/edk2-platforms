#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ArmSmcLib.h>

#if 0
#include <debug.h>                                                                                                                                                      
#include <mmio.h>
#include <platform_def.h>
#include <arch_helpers.h>
#include <string.h>
#endif

#define EcDataPort 0x20000062
#define EcCommandPort 0x20000066
//ACPI Embedded controller commands
#define SMC_READ_EC  0x80 //Read EC
#define SMC_WRITE_EC 0x81 //Write EC
#define SUCCESS 1
//Wait till EC I/P buffer is free
static UINT8 EcIbFree()
{
	UINT8 Status;
	do{
		Status=MmioRead8(EcCommandPort);
	}while(Status&2);
	return SUCCESS;
}
//Wait till EC O/P buffer is full

static UINT8 EcObFull()
{
	UINT8 Status;
	do{
		Status=MmioRead8(EcCommandPort);
	}while(!(Status&1));
	return SUCCESS;
}
//Send EC command
static UINT8 EcWriteCmd(UINT8 cmd)
{
	EcIbFree();
	MmioWrite8(EcCommandPort,cmd);
	return SUCCESS;
}
//Write Data from EC data port
static UINT8 EcWriteData(UINT8 data)
{
	EcIbFree();
	MmioWrite8(EcDataPort,data);
	return SUCCESS;
}
//Read Data from EC data Port
static UINT8 EcReadData(UINT8 *pData)
{
	//暂时修改
	*pData = MmioRead8(EcDataPort);
	EcObFull();
	*pData=MmioRead8(EcDataPort);
	return SUCCESS;
}
//Read Data from EC Memory from location pointed by Index
UINT8 EcReadMem(UINT8 Index,UINT8 *Data)
{
	UINT8 cmd =SMC_READ_EC;
	EcWriteCmd(cmd);
	EcWriteData(Index);
	EcReadData(Data);
	return SUCCESS;
}
//Write Data to EC memory at location pointed by Index
UINT8 EcWriteMem(UINT8 Index,UINT8 Data)
{
	UINT8 cmd = SMC_WRITE_EC;
	EcWriteCmd(cmd);
	EcWriteData(Index);
	EcWriteData(Data);
	return SUCCESS;
}
