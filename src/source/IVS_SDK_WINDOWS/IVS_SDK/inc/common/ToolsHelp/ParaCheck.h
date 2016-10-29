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
	filename	: 	ParaCheck.h
	author		:	z00201790
	created		:	2013/01/17
	description	:	对参数进行校验的类
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/1/17 初始版本
*********************************************************************/

#ifndef __PARACHECK_H__
#define __PARACHECK_H__

#include "hwsdk.h"
class CParaCheck
{
public:

    /***********************************************************************************
    * name       : CheckIsIvsBool
    * description: 检查输入的参数是否为0/1型的IVS_BOOL
    * input      : bIvsBool:
    * output     : NA
    * return     : 如果在返回内返回IVS_SUCCEED,失败返回IVS_PARA_INVALID
    * remark     : NA
    ***********************************************************************************/
    static IVS_INT32 CheckIsIvsBool(IVS_BOOL bIvsBool);
};

#endif

