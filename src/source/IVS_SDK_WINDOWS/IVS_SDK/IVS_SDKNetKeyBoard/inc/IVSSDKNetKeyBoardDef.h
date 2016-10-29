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
	filename	: 	IVSSDKNetKeyBoardDef.h
	author		:	
	created		:	2011/10/29	
	description	:	提供IVSSDKNetKeyBoardDef中需要暴露给第三方使用的数据类型（IVSSDKNetKeyBoardDef接口中需要用到的数据类型）
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
#ifndef IVS_SDK_NET_KEY_BOARD_DEF_H
#define IVS_SDK_NET_KEY_BOARD_DEF_H



typedef int (__stdcall *LPCallBack)(int iKeyValue,int iSpeedFirst,int iSpeedSecond, bool bTvWall);


#endif //IVS_SDK_NET_KEY_BOARD_DEF_H