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
filename 	: OCXDrawXMLProcess.h
author   	: z00227377
created 	: 2012/12/25
description : ��ͼXML
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/25 ��ʼ�汾
*********************************************************************/


#ifndef __OCX_DRAW_XML_PROCESS_H__ 
#define __OCX_DRAW_XML_PROCESS_H__

#include "SDKDef.h"
#include "IVS_IADrawAPI.h"




class COCXDrawXMLProcess
{
private:
	COCXDrawXMLProcess();
public:
	~COCXDrawXMLProcess();

public:
	// ��������ͼ�ε�XML
	static IVS_INT32 SetGraphicsPraseXML(IA_GRAPHICS *pGraphicsArr, ULONG &ulNum, LPCTSTR pGraphicsXml, const SIZE &szGraphic, BOOL bPersent);

	// ������ȡͼ�ε�XML
	static IVS_INT32 GetGraphicsGetXML(const IA_GRAPHICS *pGraphicsArr, IVS_INT32 iResultCode, ULONG ulNum, CXml& xmlGraphics, const SIZE &szGraphic);

	// ������ȡͼ��ʧ�ܵ�XML
	static IVS_INT32 GetGraphicsGetErrXML(IVS_INT32 iResultCode, CXml& xmlGraphics);
	
	// ������ͼǰͼ����������XML
	static IVS_INT32 DrawGraphicsPraseXML(IA_GRAPHICS &GraphicAttribute, ULONG &ulGroupDrawMaxNum, LPCTSTR pGraphicsXml);

	// ����ѡ��ͼ����ϢXML
	static IVS_INT32 SelectGraphicGetXML(CString &xmlToCU, IVS_ULONG ulGraphicGroup);
	
	// ����ȥѡ��ͼ����ϢXML
	static IVS_INT32 UnSelectGraphicGetXML(CString &xmlToCU);

};
#endif //__OCX_DRAW_XML_PROCESS_H__
