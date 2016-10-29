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
	filename	: 	LogMgrXMLProcess.h
	author		:	wulixia
	created		:	2012/12/03	
	description	:	���������־XML
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/03 ��ʼ�汾
*********************************************************************/

#ifndef __LOG_MGR_XML_PROCESS_H__
#define __LOG_MGR_XML_PROCESS_H__

#include "XmlProcess.h" //lint !e537
#include "SDKSecurityClient.h"

class CLogMgrXMLProcess
{
private:
	CLogMgrXMLProcess();
public:
	~CLogMgrXMLProcess();

public:
	/*****************************************************************
     function   : PraseResOperationLogXML
     description: ��ѯ������־������Ӧ
	 intput     : pResOperationLog����Ӧ��Ϣ�ṹ��
	              iBufSize�������С
				  oSecurityClient���ܶ���
     output     : xml�������Ŀ��xml
     return     : �ɹ�����0��ʧ�ܷ��ش�����;
     *****************************************************************/

	static IVS_INT32 PraseResOperationLogXML(CXml& xml,IVS_OPERATION_LOG_LIST* pResOperationLog,const IVS_UINT32& uiBufSize, const CSDKSecurityClient& oSecurityClient);

	/*****************************************************************
     function   : GetResOperationLogExXML
     description: ƴװ��ѯ������־����Xml
	 intput     : pUnifiedQuery ͨ�÷�ҳ��ѯ�ṹ��
	 intput     : pOptLogQueryEx ��չ��ѯ������־����
     output    : xml�������Ŀ��xml
     return     : �ɹ�����0��ʧ�ܷ��ش�����;
     *****************************************************************/
	static IVS_INT32 GetResOperationLogExXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
		CXml &xml);
};

#endif //__LOG_MGR_XML_PROCESS_H__

