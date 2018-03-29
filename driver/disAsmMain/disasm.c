#include "disasmTable.h"

#pragma alloc_text (PAGE,DisasmCode)

ULONG     globe_DataSize;
ULONG     globe_AddrSize;
LONG      globe_SegPrefix;
ULONG     globe_HasRM;
ULONG     globe_HasSIB;
ULONG     globe_DispSize;
ULONG     globe_ImmSize;

PUCHAR    globe_Cmd;                 // 数据地址
ULONG     globe_RemainingSize;       // 指令中的剩余字节数
PDisasm   globe_Da;                  // 返回结构指针

VOID DecodeIM(ULONG constsize) {
	ULONG l;

	globe_ImmSize+=constsize;
	l=1+globe_HasRM + globe_HasSIB + globe_DispSize + (globe_ImmSize - constsize);
	if (globe_RemainingSize < l+constsize) globe_Da->error = DAE_CROSS;		
}

VOID DecodeVX(VOID) {
	ULONG l;

	globe_ImmSize+=4;

	l=1+globe_HasRM + globe_HasSIB + globe_DispSize + (globe_ImmSize - 4);
	if (globe_RemainingSize < l+4) 
	{
		globe_Da->error=DAE_CROSS;
		return; 
	}
}

void DecodeRJ(ULONG offsize) 
{
	if (globe_RemainingSize < offsize+1) 
	{
		globe_Da->error=DAE_CROSS; 
		return; 
	}
		globe_DispSize = offsize;
}


VOID DecodeMR(ULONG type) 
{
  ULONG sib;
  ULONG	c;

  if (globe_RemainingSize<2) 
  {
    globe_Da->error=DAE_CROSS; 
	return; 
  }

  globe_HasRM=1;

  c=globe_Cmd[1] & 0xC7;

  if ( (c & 0xC0) == 0xC0 ) return; 

  if (globe_AddrSize==2) 
  {
    if ( c == 0x06 ) 
	{
      globe_DispSize=2;
      if (globe_RemainingSize<4) globe_Da->error=DAE_CROSS;
	}
    else
	{
      if ((c & 0xC0)==0x40) 
	  {
        if (globe_RemainingSize<3) globe_Da->error=DAE_CROSS;
        globe_DispSize=1; 
	  }
      else if ((c & 0xC0)==0x80) 
	  {
        if (globe_RemainingSize<4) globe_Da->error=DAE_CROSS;
        globe_DispSize=2; 
	  }
    }
  } 
  else if ( c == 0x05) 
  {
    globe_DispSize=4;
    if (globe_RemainingSize<6) globe_Da->error=DAE_CROSS;  
  }
  else if ((c & 0x07)==0x04) 
  {         // SIB addresation
    sib=globe_Cmd[2]; 
	globe_HasSIB=1;

    if (c==0x04 && (sib & 0x07)==0x05) 
	{
      globe_DispSize=4;                      // Immediate address without base
      if (globe_RemainingSize<7)
        globe_Da->error=DAE_CROSS;           // Disp32 outside the memory block
	}
    else 
	{                             // Base and, eventually, displacement
      if ((c & 0xC0)==0x40) 
	  {
        globe_DispSize=1;
        if (globe_RemainingSize<4) globe_Da->error=DAE_CROSS;
      }
      else if ((c & 0xC0)==0x80) 
	  {
        globe_DispSize=4;
        if (globe_RemainingSize<7) globe_Da->error=DAE_CROSS;
      }
    }
  }
  else 
  {                               // No SIB
	  if ((c & 0xC0)==0x40) 
	  {
		  globe_DispSize=1;
		  if (globe_RemainingSize<3) globe_Da->error=DAE_CROSS;
	  }
	  else if ((c & 0xC0)==0x80) 
	  {
	      globe_DispSize=4;
		  if (globe_RemainingSize<6) globe_Da->error=DAE_CROSS;
	  }
	  
  }
}

ULONG DisasmCode(PUCHAR Src,ULONG SrcSize,PDisasm pDisasm)
{
  ULONG i,operand,arg;
  PCmdData pd;

  ULONG Code = 0;
  ULONG LockPrefix = 0;	//锁定前缀
  ULONG RepPrefix = 0;	//重复前缀
  BOOLEAN	Repeated = FALSE;
  BOOLEAN	IsPrefix;

  if (SrcSize == 0) return 0;
   
  globe_DataSize = globe_AddrSize=4;                 // 32-bit code and data segments only!
  globe_SegPrefix = SEG_UNDEF;
  globe_HasRM = globe_HasSIB = 0; 
  globe_DispSize = globe_ImmSize = 0; 
  globe_Cmd = Src; 
  globe_RemainingSize = SrcSize; 

  globe_Da=pDisasm;
  globe_Da->cmdtype=C_BAD;
  globe_Da->warnings=0;
  globe_Da->error=DAE_NOERR;

  //处理前缀
  while ( globe_RemainingSize>0 ) 
  {
    IsPrefix = TRUE;
    switch (*globe_Cmd) 
	{
      case 0x26: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_ES; else Repeated=TRUE; break;
      case 0x2E: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_CS; else Repeated=TRUE; break;
      case 0x36: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_SS; else Repeated=TRUE; break;
      case 0x3E: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_DS; else Repeated=TRUE; break;
      case 0x64: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_FS; else Repeated=TRUE; break;
      case 0x65: if (globe_SegPrefix == SEG_UNDEF) globe_SegPrefix = SEG_GS; else Repeated=TRUE; break;
      case 0x66: if (globe_DataSize == 4) globe_DataSize = 2; else Repeated = TRUE; break;
      case 0x67: if (globe_AddrSize == 4) globe_AddrSize = 2; else Repeated = TRUE; break;
      case 0xF0: if (LockPrefix == 0) LockPrefix = 0xF0; else Repeated = TRUE; break;
      case 0xF2: if (RepPrefix == 0) RepPrefix = 0xF2; else Repeated = TRUE; break;
      case 0xF3: if (RepPrefix == 0) RepPrefix = 0xF3; else Repeated = TRUE; break;

      default: 
		IsPrefix = FALSE; 
		break; 
	}
    if (IsPrefix == FALSE || Repeated == TRUE) break;

    globe_Cmd++; 
	globe_RemainingSize--; 
  }//end while
  
  //重复前缀
  if ( Repeated ) 
  {
    globe_Da->cmdtype = C_RARE;
    return 1;
  }

  //锁定前缀指示
  if ( LockPrefix != 0 ) globe_Da->warnings|=DAW_LOCK; 

  //取出指令三个字节 如果够长的话
  if (globe_RemainingSize>0) *(((PUCHAR)&Code)+0) = globe_Cmd[0];
  if (globe_RemainingSize>1) *(((PUCHAR)&Code)+1) = globe_Cmd[1];
  if (globe_RemainingSize>2) *(((PUCHAR)&Code)+2) = globe_Cmd[2];

  //如果有前缀的话 添加到双字中
  if (RepPrefix!=0) Code = (Code<<8) | RepPrefix;        // part of command.

  //查表
  for ( pd = (PCmdData)&CmdDataTable[0]; pd->mask != 0; pd++ )
  {
	  if ( ((Code^pd->code) & pd->mask) != 0 ) continue;
	  break;
  }


  //没找到
  if ( pd->mask==0 ) 
  {
    globe_Da->cmdtype=C_BAD;
    if (globe_RemainingSize<2) globe_Da->error = DAE_CROSS;  //没发现指令的话写入error
    else globe_Da->error=DAE_BADCMD; 
  }
  else //发现了指令 开始解码
  {
    globe_Da->cmdtype = pd->type;

	//特权指令
    if (pd->bits==PR) globe_Da->warnings|=DAW_PRIV;
    else if (pd->bits==WP) globe_Da->warnings|=DAW_IO;//io指令

	//因为不能出现inc esp , dec esp , add esp,imm sub esp,imm这里进行判断 <会造成堆栈不平衡>
    if (globe_Cmd[0]==0x44 || globe_Cmd[0]==0x4C ||
      (globe_RemainingSize>=3 && (globe_Cmd[0]==0x81 || globe_Cmd[0]==0x83) &&
      (globe_Cmd[1]==0xC4 || globe_Cmd[1]==0xEC) && (globe_Cmd[2] & 0x03)!=0)) 
	{
      globe_Da->warnings|=DAW_STACK;
      globe_Da->cmdtype|=C_RARE; 
	}

	//修改段寄存器指令
    if (globe_Cmd[0]==0x8E) globe_Da->warnings|=DAW_SEGMENT;

    //2字节操作码
    if (pd->len==2) 
	{
      if (globe_RemainingSize==0) globe_Da->error=DAE_CROSS;
      else 
	  {
        globe_Cmd++; 
		globe_RemainingSize--;
      }
	}

	//如果已经到达了缓冲末尾   错误信息
    if (globe_RemainingSize==0) globe_Da->error=DAE_CROSS;

    if ((pd->bits & WW)!=0 && (*globe_Cmd & WW)==0)
      globe_DataSize=1;
    else if ((pd->bits & W3)!=0 && (*globe_Cmd & W3)==0)
      globe_DataSize=1;
    else if ((pd->bits & FF)!=0)
      globe_DataSize=2;


    for (operand=0; operand<3; operand++) 
	{
      if (globe_Da->error) break;
      if (operand==0) arg = pd->arg1;
      else if (operand==1) arg=pd->arg2;
      else arg=pd->arg3;
      if (arg==NNN) break;

      switch (arg) {
		case REG: case RG4: case RMX: case R3D: case SGM:
			if (globe_RemainingSize < 2) globe_Da->error=DAE_CROSS;
			globe_HasRM=1; 
			break;

        case MRG: case MRJ: case MRD:
        case MR1: case MR2: case MR4: case MR8:
		case MMA: case MMB: case MML: case MMS: case MM6:
		case MD2: case MD4: case MD8: case MDA:
        case MB2:
		case MF4: case MF8: case MFA: case MFE: case MFS: case MFX:
		case RR4: case RR8: case RRD:
          DecodeMR(arg); 
		  break;

		case IMM:                      // Immediate data (8 or 16/32)
		case IMU:                      // Immediate unsigned data (8 or 16/32)
			if ((pd->bits & SS)!=0 && (*globe_Cmd & 0x02)!=0) DecodeIM(1);
			else DecodeIM(globe_DataSize);
			break;

		case IMA:                      // Immediate absolute near data address
			if (globe_RemainingSize < 1 + globe_AddrSize)
				globe_Da->error = DAE_CROSS;
			else
				globe_DispSize = globe_AddrSize;
			break;

		case VXD:
			DecodeVX(); 
			break;
			break;
		case JOB:
			DecodeRJ(1); 
			break;
		case JOW:
			DecodeRJ(globe_DataSize); 
			break;
		case IM2:
			DecodeIM(2);
			break;		
		case IMX: case IMS:	case IM1:
			DecodeIM(1); 
			break;

		case JMF:
			if ( globe_RemainingSize < 1+globe_AddrSize+2 )
				globe_Da->error = DAE_CROSS;
			else
			{
				globe_DispSize = globe_AddrSize;
				globe_ImmSize = 2;
			}
			break;

		case CRX:
		case DRX:
			if ((globe_Cmd[1] & 0xC0)!=0xC0) globe_Da->error=DAE_REGISTER;
			globe_HasRM = 1;
			break;

		case PRN: case PRF: case PAC: case PAH: case PFL: case PS0:	case PS1:
		case PCX: case PDI: case SCM: case C01: case RCM: case RAC:	case RAX:
		case RDX: case RCL:	case RS0: case RST:	case MSO: case MDE:	case MXL:
			break;   

        default:
          globe_Da->error=DAE_INTERN;        // Unknown argument type
        break;
      }
	 }//end for
	}//if ( (pd->mask==0)||((pd->type & C_TYPEMASK) == C_NOW) ) 

	if (globe_Da->error!=0) 
	{
		if (globe_Da->error==DAE_BADCMD && (*globe_Cmd==0x0F || *globe_Cmd==0xFF) && globe_RemainingSize>0) 
			globe_RemainingSize--;

		if (globe_RemainingSize>0) 
			globe_RemainingSize--;
	}
	else 	  
	  globe_RemainingSize -= 1+globe_HasRM+globe_HasSIB+globe_DispSize+globe_ImmSize;
  return (SrcSize-globe_RemainingSize);
}

