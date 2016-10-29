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

/********************************************************************
	filename	: 	IVSSDKNetKeyBoard.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	IVS�ṩ��������ʹ�õĽӿ�
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/
#ifndef IVS_SDK_NET_KEY_BOARD_H
#define IVS_SDK_NET_KEY_BOARD_H

/*!< �����ꡢ�ṹ��ͷ�ļ� */
#include "IVSSDKNetKeyBoardDef.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /**************************************************************************
    * name       : SetServerInfo
    * description: ���÷�����IP�Ͷ˿�
    * input      : pszIP ������IP��ַ iPort �˿�
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall  SCCSetServerInfo(const char* pszIP, int iPort);

    /**************************************************************************
    * name       : SetLoginID
    * description: ���õ�¼ID
    * input      : pszLoginID ��¼ID
    * output     : void
    * return     : RET_TRUE �ɹ���RET_FALSE ʧ��
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall SetLoginID(const char *pszLoginID);


    /**************************************************************************
    /**************************************************************************
    * name       : SetListenAddr
    * description: ���÷������˼���IP�Ͷ˿ں�
    * input      : iPort�������˼����˿ں�
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) void __stdcall IVS_SDK_KB_SetNetKeyBoardListenAddr(u_short iPort);

    /**************************************************************************
    * name       : StartListenThread
    * description: ���������߳�
    * input      : lpCallBack �ص�����
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) int __stdcall IVS_SDK_KB_StartNetKeyBoardListenThread(u_short ulNetKeyBoardVendor, u_short ulPort, LPCallBack lpCallBack);

    /**************************************************************************
    * name       : ReleaseThread
    * description: �ͷ��߳�
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    __declspec(dllexport) void __stdcall IVS_SDK_KB_ReleaseThread();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_IVS_SDK_H_
