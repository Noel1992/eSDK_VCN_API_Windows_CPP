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
	filename	: 	OCXUpgradeMgrXMLProcess.h
	author		:	wanglei 00165153
	created		:	2013/11/13	
	description	:	处理告警XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2013/11/13 初始版本
*********************************************************************/

#ifndef __OCX_UPGRADE_MGR_XML_PROCESS_H__
#define __OCX_UPGRADE_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXUpgradeMgrXMLProcess
{
private:
    COCXUpgradeMgrXMLProcess();
    ~COCXUpgradeMgrXMLProcess();

public:	
    static IVS_INT32 GetCUVersionParseReqXML(CXml& xmlReq,IVS_CU_INFO& stCuInfo);
    static IVS_INT32 GetCUVersionRspXML(IVS_UPGRADE_INFO& stUpgradeInfo,IVS_INT32 iRetCord,CXml& xmlRsp);
};

#endif
