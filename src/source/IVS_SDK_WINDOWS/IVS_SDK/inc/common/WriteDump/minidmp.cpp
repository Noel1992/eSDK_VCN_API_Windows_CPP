/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "stdafx.h"
#include "minidmp.h"

//lint -e818
BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if(0 == pModuleName)
	{
		return FALSE;
	}

	WCHAR szFileName[_MAX_FNAME] = L"";
	(void)_wsplitpath_s(pModuleName, NULL,0, NULL, 0,szFileName,_MAX_FNAME, NULL,0);
	if(_wcsicmp(szFileName, L"ntdll") == 0)
	{
		return TRUE;
	}
	return FALSE; 
}

BOOL CALLBACK MiniDumpCallback(PVOID                            /*pParam*/, 
									  const PMINIDUMP_CALLBACK_INPUT   pInput, 
									  PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if(0 == pInput||0 == pOutput)
	{
		return FALSE;
	}

	switch(pInput->CallbackType)
	{
	case ModuleCallback: 
		{
			if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg) 
			{
				if(!IsDataSectionNeeded(pInput->Module.FullPath)) 
				{
					pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg); 
				}
			}
			return TRUE;
		}
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		{
			return TRUE;
		}		
	default:
		{
			return FALSE;
		}
	}

}

void CreateMiniDump(EXCEPTION_POINTERS* pep, std::string strFileName)
{
	HANDLE hFile = CreateFileA(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if((NULL != hFile) && (INVALID_HANDLE_VALUE != hFile))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId           = GetCurrentThreadId();
		mdei.ExceptionPointers  = pep;
		mdei.ClientPointers     = FALSE;
		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam       = 0;
		MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | 
			MiniDumpWithDataSegs | 
			MiniDumpWithHandleData |
			0x00000800 /*MiniDumpWithFullMemoryInfo*/ | 
			0x00001000 /*MiniDumpWithThreadInfo*/ | 
			MiniDumpWithUnloadedModules);
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);
		CloseHandle(hFile); 
	}
}
