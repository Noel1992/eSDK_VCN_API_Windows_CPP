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

/*****************************************************************
 filename    :    IVS_OCXPlayer.h
 author      :    ykf75928
 created     :    2013/01/16
 description :    ocx程序头文件
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/16 初始版本
*****************************************************************/
#ifndef _IVS_OCXPLAYER_H_
#define _IVS_OCXPLAYER_H_

// IVS_OCXPlayer.h : main header file for IVS_OCXPlayer.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

// CIVS_OCXPlayerApp : See IVS_OCXPlayer.cpp for implementation.

class CIVS_OCXPlayerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};
//lint -e526
extern const GUID CDECL _tlid;//lint !e526
extern const WORD _wVerMajor;//lint !e526
extern const WORD _wVerMinor;//lint !e526

#endif	//_IVS_OCXPLAYER_H_
