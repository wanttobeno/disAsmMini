#include "disAsmMain.h"

#pragma alloc_text (INIT,DriverEntry)
#pragma alloc_text (PAGE,DispatchDeviceIoControl)
#pragma alloc_text (PAGE,Unload)
#pragma alloc_text (PAGE,DispatchCompletion)

#define DEVICE_NAME L"\\Device\\disAsm"
#define DEVICE_SYMBOLICLINK_NAME L"\\DosDevices\\disAsmSymLink"

#define IOCTL_DISASM_SPECIAL_FUNCTION (CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,FILE_ANY_ACCESS,METHOD_BUFFERED))

void DisassemblerNow(pDisAsmData pFuncInfo, PVOID pFuncAddress)
{
	ULONG   dw;
	Disasm  dis;
	ULONG   DecodedLength = 0;
	while (DecodedLength < pFuncInfo->Length)
	{
		dw = DisasmCode((PUCHAR)((ULONG)pFuncAddress + DecodedLength), pFuncInfo->Length - DecodedLength, &dis);
		DecodedLength = DecodedLength + dw;

		switch (dis.cmdtype)
		{
		case C_NOP: KdPrint(("C_NOP  %d", dw)); break;
		case C_TST: KdPrint(("C_TST  %d", dw)); break;
		case C_CMP: KdPrint(("C_CMP  %d", dw)); break;
		case C_CMPS: KdPrint(("C_CMPS  %d", dw)); break;
		case C_CMPSB: KdPrint(("C_CMPSB  %d", dw)); break;
		case C_CMPSW: KdPrint(("C_CMPSW  %d", dw)); break;
		case C_CMPSD: KdPrint(("C_CMPSD  %d", dw)); break;

		case C_MOV: KdPrint(("C_MOV  %d", dw)); break;
		case C_MOVS: KdPrint(("C_MOVS  %d", dw)); break;
		case C_MOVSX: KdPrint(("C_MOVSX  %d", dw)); break;
		case C_MOVSB: KdPrint(("C_MOVSB  %d", dw)); break;
		case C_MOVSW: KdPrint(("C_MOVSW  %d", dw)); break;
		case C_MOVSD: KdPrint(("C_MOVSD  %d", dw)); break;
		case C_MOVZX: KdPrint(("C_MOVZX  %d", dw)); break;

		case C_JMP: KdPrint(("C_JMP  %d", dw)); break;
		case C_JMC: KdPrint(("C_JMC  %d", dw)); break;
		case C_CAL: KdPrint(("C_CAL  %d", dw)); break;
		case C_RET: KdPrint(("C_RET  %d", dw)); break;

		case C_XOR: KdPrint(("C_XOR  %d", dw)); break;
		case C_AND: KdPrint(("C_AND  %d", dw)); break;
		case C_OR: KdPrint(("C_OR  %d", dw)); break;
		case C_NOT: KdPrint(("C_NOT  %d", dw)); break;

		case C_PSH: KdPrint(("C_PSH  %d", dw)); break;
		case C_POP: KdPrint(("C_POP  %d", dw)); break;
		case C_PSHA: KdPrint(("C_PSHA  %d", dw)); break;
		case C_POPA: KdPrint(("C_POPA  %d", dw)); break;
		case C_PSHF: KdPrint(("C_PSHF  %d", dw)); break;
		case C_POPF: KdPrint(("C_POPF  %d", dw)); break;

		case C_ADD: KdPrint(("C_ADD  %d", dw)); break;
		case C_ADC: KdPrint(("C_ADC  %d", dw)); break;
		case C_SBB: KdPrint(("C_SBB  %d", dw)); break;
		case C_SUB: KdPrint(("C_SUB  %d", dw)); break;
		case C_INC: KdPrint(("C_INC  %d", dw)); break;
		case C_DEC: KdPrint(("C_DEC  %d", dw)); break;
		case C_MUL: KdPrint(("C_MUL  %d", dw)); break;
		case C_IMUL: KdPrint(("C_IMUL  %d", dw)); break;
		case C_DIV: KdPrint(("C_DIV  %d", dw)); break;
		case C_IDIV: KdPrint(("C_IDIV  %d", dw)); break;

		case C_NEG: KdPrint(("C_NEG  %d", dw)); break;
		case C_LEA: KdPrint(("C_LEA  %d", dw)); break;
		case C_XCHG: KdPrint(("C_XCHG  %d", dw)); break;

		case C_SHR: KdPrint(("C_SHR  %d", dw)); break;
		case C_SHL: KdPrint(("C_SHL  %d", dw)); break;
		case C_ROL: KdPrint(("C_ROL  %d", dw)); break;
		case C_ROR: KdPrint(("C_ROR  %d", dw)); break;
		case C_RCL: KdPrint(("C_RCL  %d", dw)); break;
		case C_RCR: KdPrint(("C_RCR  %d", dw)); break;
		case C_SAR: KdPrint(("C_SAR  %d", dw)); break;
		case C_SAL: KdPrint(("C_SAK  %d", dw)); break;

		case C_CLI: KdPrint(("C_CLI  %d", dw)); break;
		case C_STI: KdPrint(("C_STI  %d", dw)); break;
		case C_ENTER: KdPrint(("C_ENTER  %d", dw)); break;
		case C_LEAVE: KdPrint(("C_LEAVE  %d", dw)); break;
		case C_INT: KdPrint(("C_INT  %d", dw)); break;
		case C_INT3: KdPrint(("C_INT3  %d", dw)); break;
		case C_SYSENTER: KdPrint(("C_SYSENTER  %d", dw)); break;
		case C_SYSEXIT: KdPrint(("C_SYSEXIT  %d", dw)); break;

		default:
			KdPrint(("C_UNKNOW  %d", dw)); break;
		}
	}
}


NTSTATUS
DispatchDeviceIoControl(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	ULONG                ulIoControlCode;     //控制代码
	PVOID                pvBuf = NULL;
	PIO_STACK_LOCATION   pIrpStatck = NULL;
	NTSTATUS             status = STATUS_UNSUCCESSFUL;

	pDisAsmData            pFuncInfo;
	PVOID                pFuncAddress = NULL;
	UNICODE_STRING       destString;

	pIrpStatck = IoGetCurrentIrpStackLocation(pIrp);
	ulIoControlCode = pIrpStatck->Parameters.DeviceIoControl.IoControlCode;
	pvBuf = pIrp->AssociatedIrp.SystemBuffer;
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	pFuncInfo = (pDisAsmData)pvBuf;
	KdPrint(("目标函数:%S\n", pFuncInfo->szFuncName));

	switch (ulIoControlCode)
	{
	case IOCTL_DISASM_SPECIAL_FUNCTION:
	{
		if (pFuncInfo->Length == 0)
			break;
		RtlInitUnicodeString(&destString, (PWCHAR)pFuncInfo->szFuncName);

		pFuncAddress = MmGetSystemRoutineAddress(&destString);
		if (pFuncAddress == NULL)
		{
			KdPrint(("未发现指定函数"));
			break;
		}
		DisassemblerNow(pFuncInfo, pFuncAddress);

		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = status = STATUS_SUCCESS;

		KdPrint(("打完收工"));

	}
	break;
	default:
	{
		KdPrint(("接收到非法命令\n"));
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = status = STATUS_INVALID_DEVICE_REQUEST;
	}
		break;
	}

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}


VOID
Unload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING      SymLinkName;

	RtlInitUnicodeString(&SymLinkName, DEVICE_SYMBOLICLINK_NAME);
	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
}

NTSTATUS
DispatchCompletion(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	PDEVICE_OBJECT     pMyDeviceObject = NULL;
	UNICODE_STRING     SymLinkName;
	UNICODE_STRING     MyDeviceName;
	NTSTATUS           status = STATUS_SUCCESS;
	ULONG              i;

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = DispatchCompletion;

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceIoControl;
	pDriverObject->DriverUnload = Unload;

	//创建设备对象
	RtlInitUnicodeString(&MyDeviceName, DEVICE_NAME);
	status = IoCreateDevice(pDriverObject, 0, &MyDeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pMyDeviceObject);
	if (!NT_SUCCESS(status)){
		KdPrint(("创建设备对象失败\n"));
		return status;
	}

	//创建符号链接
	RtlInitUnicodeString(&SymLinkName, DEVICE_SYMBOLICLINK_NAME);
	status = IoCreateSymbolicLink(&SymLinkName, &MyDeviceName);
	if (!NT_SUCCESS(status)){
		IoDeleteDevice(pMyDeviceObject);
		KdPrint(("创建符号链接失败\n"));
		return status;
	}

	pMyDeviceObject->Flags |= DO_BUFFERED_IO;
	pMyDeviceObject->Flags = pMyDeviceObject->Flags & ~DO_DEVICE_INITIALIZING;

	return status;
}
