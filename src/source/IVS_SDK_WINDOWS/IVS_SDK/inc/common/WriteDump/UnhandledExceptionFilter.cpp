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
#include "ToolsHelp.h"

LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	//得到当前时间
	BP_RUN_LOG_WAR("dump happped", "NA");
	SYSTEMTIME st;
	::GetLocalTime(&st);
	std::string strPath;
	CToolsHelp::GetDLLPath("IVS_SDK.dll", strPath);
	const int PATH_LENGTH = 2048;
	char szFileName[PATH_LENGTH]={0};
	(void)sprintf_s(szFileName,PATH_LENGTH-1,("%s\\core-%04d%02d%02d-%02d%02d-%02d-%02d.dmp"),strPath.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	CreateMiniDump(pExceptionInfo, szFileName);
	exit((int)(pExceptionInfo->ExceptionRecord->ExceptionCode));
}

