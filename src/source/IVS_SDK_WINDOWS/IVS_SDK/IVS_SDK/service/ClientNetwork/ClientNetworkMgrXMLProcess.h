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
filename : ClientNetworkMgrXMLProcess.h
author : pkf57481
created : 2013/01/22
description : �ͻ�������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/22 ��ʼ�汾
*********************************************************************/


#ifndef __CLIENTNETWORK_MGR_XML_PROCESS_H__
#define __CLIENTNETWORK_MGR_XML_PROCESS_H__

#include "ToolsHelp.h"
#include "XmlProcess.h"
#include <string.h>
#include "QueryFiled.h"
#include "hwsdk.h"

class CClientNetworkMgrXMLProcess
{
private:
    CClientNetworkMgrXMLProcess();
public:
    ~CClientNetworkMgrXMLProcess();

public:
	/***********************************************************************************
    * name       : GetCPUMemoryGetXML
    * description: �ͻ��˲�ѯCPU/�ڴ�����ƴ��xml
    * input      : pNodeCode:��Ԫ����; xml:����XML
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 NodeCodeGetXML(const IVS_CHAR* pNodeCode, CXml& xml);

};

#endif

