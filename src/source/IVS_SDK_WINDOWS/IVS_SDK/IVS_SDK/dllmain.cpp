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

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

/**************************************************************************
* name       : FreeSock
* description: �ͷ�SocketͨѶ����Դ;
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void FreeSock()
{
	(void)WSACleanup();
}

/**************************************************************************
* name       : InitSock
* description: ��ʼ��SocketͨѶ��
* input      : NA
* output     : NA
* return     : TRUE �ɹ���FALSE ʧ��
* remark     : NA
**************************************************************************/
BOOL InitSock()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2,2);

	//��socketͨѶ��;
	if (0 != WSAStartup(wVersionRequested,&wsaData))
	{
		return FALSE;
	}

	//���汾���Ƿ�Ϊ2.2;
	if ((2 != LOBYTE(wsaData.wVersion))||(2 != HIBYTE(wsaData.wVersion)))
	{
		FreeSock();
		return FALSE;
	}

	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (FALSE == InitSock())
		{
			return FALSE;
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
        FreeSock();
		break;
	default:
		break;
	}
	return TRUE;
}

