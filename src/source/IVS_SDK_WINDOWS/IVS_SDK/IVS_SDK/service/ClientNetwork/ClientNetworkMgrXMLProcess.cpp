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

#include "ClientNetworkMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"


CClientNetworkMgrXMLProcess::CClientNetworkMgrXMLProcess(void)
{
}

CClientNetworkMgrXMLProcess::~CClientNetworkMgrXMLProcess(void)
{
}

// 客户端查询CPU/内存性能拼接xml
IVS_INT32 CClientNetworkMgrXMLProcess::NodeCodeGetXML(const IVS_CHAR* pNodeCode, CXml& xml)
{
	CHECK_POINTER(pNodeCode, IVS_SDK_RET_INIT_RTSPCLIENT_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("NodeCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pNodeCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
