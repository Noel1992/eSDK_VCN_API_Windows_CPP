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
 filename		:	IAXMLProcess.h
 author			:	zwx211831
 created			:	2015/1/14
 description	:	�����ͽ���XML����;
 copyright		:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history			:	2015/1/13 ��ʼ�汾
*****************************************************************/

#ifndef __IA_XML_PROCESS_H__
#define __IA_XML_PROCESS_H__

#include "vos.h"
#include "SDKDef.h"
#include "vos_atomic.h"
#include "IVS_IATrackAPI.h"
#include "ivs_xml.h"

class CIAXMLProcess
{
private:
	CIAXMLProcess();
public:
	~CIAXMLProcess();

public:
	// �������ܷ�������켣����
	static IVS_INT32 GetAnalyzeResultBuildXML(const IA_REQ_PARAM &IAReqParam, CXml &ReqXML);

	// �������ܷ�������켣����
	static IVS_INT32 GetAnalyzeResultPraseXML(IVS_CHAR *szRspXML, IVS_INT32 &ulResultCode, IA_REQ_PARAM &IAReqParam);

	// �������ܷ����켣���ͱ���
	static IVS_INT32 TrackInfoContentPraseXML(const IVS_CHAR * pTrackXml, IA_LAYOUT_MESSAGE &pIALayoutMessage, IA_REQ_PARAM &IAReqParameters);

	// ����ֹͣ���ܷ�������켣����
	static IVS_INT32 StopAnalyzeResultBuildXML(IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, CXml &ReqXML);

private:
	// �����ֱ��ʳ����ֶ�
	static IVS_INT32 PraseFrameSize(char *tempBuf, IA_LAYOUT_MESSAGE &pIALayoutMessage);
	// ������ɫ�ֶ�
	static IVS_INT32 PraseColor(char *tempBuf, COLORREF &ulColor);
	// �����������ֶ�
	static IVS_INT32 PrasePoint(char *tempBuf, POINT &point);
	// �������б��ֶ�
	static IVS_INT32 PrasePointList(char *tempBuf, POINT *PointList, ULONG &ulListPointsNum);
};

#endif
