#ifndef _DISASM_H
#define _DISASM_H
#endif

#pragma once
#include <ntddk.h>

#define PSEUDOOP       128

#define WW             0x01
#define SS             0x02
#define WS             0x03
#define W3             0x08
#define CC             0x10
#define FF             0x20
#define LL             0x40
#define PR             0x80
#define WP             0x81

#define NNN            0
#define REG            1
#define RCM            2
#define RG4            3
#define RAC            4
#define RAX            5
#define RDX            6
#define RCL            7
#define RS0            8
#define RST            9
#define RMX            10
#define R3D            11
#define MRG            12
#define MR1            13
#define MR2            14
#define MR4            15
#define RR4            16
#define MR8            17
#define RR8            18
#define MRD            19
#define RRD            20
#define MRJ            21
#define MMA            22
#define MML            23
#define MMS            24
#define MM6            25
#define MMB            26
#define MD2            27
#define MB2            28
#define MD4            29
#define MD8            30
#define MDA            31
#define MF4            32
#define MF8            33
#define MFA            34
#define MFE            35
#define MFS            36
#define MFX            37
#define MSO            38
#define MDE            39
#define MXL            40
#define IMM            41
#define IMU            42
#define VXD            43
#define IMX            44
#define C01            45
#define IMS            46
#define IM1            47
#define IM2            48
#define IMA            49
#define JOB            50
#define JOW            51
#define JMF            52
#define SGM            53
#define SCM            54
#define CRX            55
#define DRX            56

#define PRN            (PSEUDOOP+0)
#define PRF            (PSEUDOOP+1)
#define PAC            (PSEUDOOP+2)
#define PAH            (PSEUDOOP+3)
#define PFL            (PSEUDOOP+4)
#define PS0            (PSEUDOOP+5)
#define PS1            (PSEUDOOP+6)
#define PCX            (PSEUDOOP+7)
#define PDI            (PSEUDOOP+8)

#define DAE_NOERR      0               // No error
#define DAE_BADCMD     1               // 无法识别指令
#define DAE_CROSS      2               // 内存块越界
#define DAE_BADSEG     3               // 无法识别的寄存器
#define DAE_MEMORY     4               // Register where only memory allowed
#define DAE_REGISTER   5               // Memory where only register allowed
#define DAE_INTERN     6               // Internal error

#define REG_EAX        0   
#define REG_ECX        1          
#define REG_EDX        2
#define REG_EBX        3
#define REG_ESP        4
#define REG_EBP        5
#define REG_ESI        6
#define REG_EDI        7

#define SEG_UNDEF     -1
#define SEG_ES         0    
#define SEG_CS         1
#define SEG_SS         2
#define SEG_DS         3
#define SEG_FS         4
#define SEG_GS         5

// Warnings
#define DAW_SEGMENT    0x0002
#define DAW_PRIV       0x0004
#define DAW_IO         0x0008
#define DAW_LOCK       0x0040
#define DAW_STACK      0x0080

//自定义
#define   C_CMD    0x00
#define   C_NOP    0x90

#define   C_TST    0x01		//比较
#define   C_CMP    0x02
#define   C_CMPS   0x03
#define   C_CMPSB  0X04
#define   C_CMPSW  0X05
#define   C_CMPSD  0X06

#define   C_MOV    0x11
#define   C_MOVS   0x12
#define	  C_MOVSX  0x13
#define   C_MOVSB  0x14
#define   C_MOVSW  0x15
#define   C_MOVSD  0x16
#define   C_MOVZX  0x17


#define   C_JMP    0x21		//转向
#define   C_JMC    0x22
#define   C_CAL    0x23
#define   C_RET    0x24

#define   C_XOR    0x31		//位操作
#define   C_AND    0x32
#define   C_OR     0x33
#define   C_NOT    0x34


#define   C_PSH    0x41		//进出栈操作
#define   C_POP    0x42
#define   C_PSHA   0x43
#define   C_POPA   0x44
#define   C_PSHF   0x45
#define   C_POPF   0x46

#define   C_ADD    0x51		//算术计算
#define   C_ADC    0x52
#define   C_SBB    0x53
#define   C_SUB    0x54
#define   C_INC    0x55
#define   C_DEC    0x56
#define   C_MUL    0x57
#define   C_IMUL   0x58
#define   C_DIV    0x59
#define   C_IDIV   0x5A

#define   C_NEG    0x61		//取反
#define   C_LEA    0x62		//取址
#define   C_XCHG   0x63

#define   C_SHR    0x71		//位移
#define   C_SHL    0x72
#define   C_ROL    0x73
#define   C_ROR    0x74
#define   C_RCL    0x75
#define   C_RCR    0x76
#define   C_SAR    0x77
#define   C_SAL    0x78

#define   C_CLI    0x81		//特殊指令
#define   C_STI    0x82
#define   C_ENTER  0x83
#define   C_LEAVE  0x84
#define   C_INT    0x85
#define   C_INT3   0x86
#define   C_SYSENTER    0x87
#define   C_SYSEXIT     0x88

#define	  C_SETNE  0xA1



#define   C_MMX    0xF1		//占位
#define   C_FLT    0xF2
#define   C_FLG    0xF3
#define   C_RTF    0xF4
#define   C_REP    0xF5
#define   C_DAT    0xF6
#define   C_EXPL   0xF7
#define   C_SIZEMASK 0xF8

#define   C_NOW    0xF9    //3d now
#define   C_PRI    0xFA    //特权指令     <HLT>

#define   C_RARE   0xFB    //罕见指令
#define   C_BAD    0xFC    //未识别指令

typedef struct _CmdData {
	ULONG          mask;
	ULONG          code;
	UCHAR          len;
	UCHAR          bits;
	UCHAR          arg1,arg2,arg3;
	UCHAR          type;
}CmdData, *PCmdData;



typedef struct _Disasm {
	ULONG  cmdtype;
	ULONG  error;
	ULONG  warnings;
}Disasm,*PDisasm;

ULONG DisasmCode(PUCHAR Src,ULONG SrcSize,PDisasm pDisasm);