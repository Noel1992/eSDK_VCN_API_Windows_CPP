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
filename 	: IAXMLProcess.h
author   	: z00227377
created 	: 2012/12/22
description : 轨迹推送相关XML
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/22 初始版本
*********************************************************************/


#ifndef __OCX_IA_XML_PROCESS_H__
#define __OCX_IA_XML_PROCESS_H__

#include "ivs_xml.h"
#include "hwsdk.h"
#include "IVS_IATrackAPI.h"
#include "GlobalIAMessageMgr.h"



class CIAXMLProcess
{
private:
	CIAXMLProcess();
public:
	~CIAXMLProcess();

public:

	// 构建智能分析请求轨迹报文
	static IVS_INT32 GetAnalyzeResultBuildXML(const IA_REQ_PARAM &IAReqParam, CXml &ReqXML);

	// 解析智能分析请求轨迹报文
	static IVS_INT32 GetAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode, IA_REQ_PARAM &IAReqParam);

	// 解析智能分析轨迹推送报文
	static IVS_INT32 TrackInfoContentPraseXML(LPCTSTR pTrackXml, IA_LAYOUT_MESSAGE &pIALayoutMessage, IA_REQ_PARAM &IAReqParameters);
	
	// 构建停止智能分析请求轨迹报文
	static IVS_INT32 StopAnalyzeResultBuildXML(IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, CXml &ReqXML);
	
	// 解析停止智能分析请求轨迹报文
	static IVS_INT32 StopAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode);

private:

	// 解析分辨率长宽字段
	static IVS_INT32 PraseFrameSize(char *tempBuf, IA_LAYOUT_MESSAGE &pIALayoutMessage);

	// 解析颜色字段
	static IVS_INT32 PraseColor(char *tempBuf, COLORREF &ulColor);

	// 解析点坐标字段
	static IVS_INT32 PrasePoint(char *tempBuf, POINT &point);

	// 解析点列表字段
	static IVS_INT32 PrasePointList(char *tempBuf, POINT *PointList, ULONG &ulListPointsNum);

};


#endif // __OCX_IA_XML_PROCESS_H__
