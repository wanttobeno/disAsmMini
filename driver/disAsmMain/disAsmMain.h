#ifndef DISASMMAIN_H
#define DISASMMAIN_H
#endif
 
#include <ntddk.h>
#include "disasm.h"
 
//与RING3层通讯结构
typedef struct _disAsmData{
    ULONG  Length;          //反汇编长度
	wchar_t*  szFuncName; //要反汇编的函数名称
}DisAsmData,*pDisAsmData;
 
DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath);
 
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DispatchDeviceIoControl;
NTSTATUS DispatchDeviceIoControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
 
__drv_dispatchType_other
DRIVER_DISPATCH DispatchCompletion;
NTSTATUS DispatchCompletion(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
 
DRIVER_UNLOAD Unload;
VOID Unload(PDRIVER_OBJECT pDriverObject);