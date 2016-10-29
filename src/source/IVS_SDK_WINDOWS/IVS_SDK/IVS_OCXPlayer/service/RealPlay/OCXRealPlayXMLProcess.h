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
	filename	: 	OCXRealPlayXMLProcess.h
	author		:	songjianfeng(00193168)
	created		:	2012/12/03	
	description	:	����ý����Ƶ�طŵ�XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 ��ʼ�汾
*********************************************************************/
#ifndef __OCX_REALPLAY_XML_PROCESS_H__ 
#define __OCX_REALPLAY_XML_PROCESS_H__

#include "OCXXmlProcess.h"
#include "OCXDatatype.h"
#include "SDKDef.h"

class COCXRealPlayXMLProcess
{
private:
	COCXRealPlayXMLProcess();
public:
	~COCXRealPlayXMLProcess();

public:
	/******************************************************************
	function : GetMediaParamXML
	description: ��ȡý�������Ϣ;
	input : pMediaParamXml,ý�������ϢXML��mediaParam��ý�������Ϣ
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	static LONG GetMediaParamXML(LPCTSTR pRealPlayParamXml, IVS_REALPLAY_PARAM &realPlayParam);

    /******************************************************************
	function : EnableGPUParseClientConfig
	description: ��ȡCU�����ļ��е�ý�����;
	input : pMediaParamXml,ý�������ϢXML��mediaParam��ý�������Ϣ
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	static LONG EnableGPUParseClientConfig(LPCTSTR pParamXml, BOOL& bEnableGPU);


	static LONG BuildAsyncRealPlayXml(CXml &xml, long winID, long result, unsigned long transid);



};
#endif

