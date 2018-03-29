#include "SystemHelp.h"
#include <Windows.h>
#include <tchar.h>

bool IsWow64()
{
	static bool bGet = false;
	static BOOL bIsWow64 = FALSE;

	if (!bGet)
	{
		if (sizeof(PVOID) == 8)
			bIsWow64 = TRUE;
		else
		{
			typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

			LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
			fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "IsWow64Process");
			if (fnIsWow64Process)
			{
				if (!fnIsWow64Process(::GetCurrentProcess(), &bIsWow64))
				{
					//LOG_ERROR("error IsWow64Process");
				}
			}
		}

		//LOG("bIsWow64[%d]",bIsWow64);
		bGet = TRUE;
	}
	return bIsWow64 ? true : false;
}


bool IsApp64()
{
	int nNum = 0;
	int nSize = sizeof(&nNum);
	if (nSize == 8)
	{
		return TRUE;
	}
	else
	{
		return false;
	}
}