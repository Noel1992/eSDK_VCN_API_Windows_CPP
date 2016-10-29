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
 filename    :    OCXDeviceParaConfig.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __OCX_DEVICE_PARA_CONFIG_H__
#define __OCX_DEVICE_PARA_CONFIG_H__

#include <afx.h>
#include "SDKDef.h"
#include "OCXDeviceParaConfigXMLProcess.h"

class COCXDeviceParaConfig
{
public:
	COCXDeviceParaConfig();
    ~COCXDeviceParaConfig();
public:
	// 设置设备参数
    static IVS_INT32 SetDeviceConfig(IVS_INT32 iSessionID, LONG lConfigType, const IVS_CHAR *pchDevCode, CXml &xml);

	// 查询设备参数
    static IVS_INT32 GetDeviceConfig(IVS_INT32 iSessionID, LONG lConfigType, const IVS_CHAR *pchDevCode, CXml &xml);

	// 设备参数统一分配内存
    static IVS_INT32 GetCommDevConfig(IVS_UINT32 &uiBufferSize, LONG lConfigType, IVS_VOID* &pBuffer);

private:

};

#endif
