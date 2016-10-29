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
	filename	: 	UpgradeMgrXMLProcess.h 
	author		:	wanglei
	created		:	2012/12/03	
	description	:	处理操作日志XML
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __UPGRADE_MGR_XML_PROCESS_H__
#define __UPGRADE_MGR_XML_PROCESS_H__

#include "XmlProcess.h"

class CUpgradeMgrXMLProcess
{
private:
	CUpgradeMgrXMLProcess();
public:
	~CUpgradeMgrXMLProcess();

public:
    static IVS_INT32 GetCUVersionReqXML(const IVS_CU_INFO* pCUInfo, CXml& xml);

    static IVS_INT32 ParseGetCuVersionRspXML(const std::string& strRspXML, IVS_UPGRADE_INFO* pUpgradeInfo);
};

#endif // __UPGRADE_MGR_XML_PROCESS_H__
