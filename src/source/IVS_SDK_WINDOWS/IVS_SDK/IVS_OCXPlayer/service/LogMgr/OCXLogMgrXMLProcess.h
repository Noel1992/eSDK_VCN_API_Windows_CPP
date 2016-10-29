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
	filename	: 	OCXLogMgrXMLProcess.h
	author		:	wulixia
	created		:	2012/12/03	
	description	:	����澯XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 ��ʼ�汾
*********************************************************************/

#ifndef __OCX_LOG_MGR_XML_PROCESS_H__
#define __OCX_LOG_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXLogMgrXMLProcess
{
private:
	COCXLogMgrXMLProcess();
public:
	~COCXLogMgrXMLProcess();

public:	

	/**************************************************************************
	* name       : GetResOperationLogXML()
	* description: ƴװ��Ӧ��Ϣ��xml
	* input      : pQueryUnifiedFormat        ������ڴ�ṹ��	
	*              iFieNum    ��ѯ�ֶεĸ���
	*              pUserData		 �û�����;
	* output     : xmlReq
	* return     : �ɹ�����0 �����󷵻ش�����
	* remark     : NA
	**************************************************************************/
	static IVS_INT32 GetResOperationLogXML(IVS_OPERATION_LOG_LIST* pOperationLogList, CXml& xmlRes);
};

#endif //__OCX_LOG_MGR_XML_PROCESS_H__
