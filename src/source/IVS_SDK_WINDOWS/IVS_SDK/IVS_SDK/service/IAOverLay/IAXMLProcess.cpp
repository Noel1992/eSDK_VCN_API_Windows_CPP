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

#include "IAXMLProcess.h"
#include "ToolHelp.h"
#include "IVS_Trace.h"
#include "XmlProcess.h"
#include "IAMessageMgr.h"

CIAXMLProcess::CIAXMLProcess()
{

}

CIAXMLProcess::~CIAXMLProcess()
{

}

IVS_INT32 CIAXMLProcess::GetAnalyzeResultBuildXML(const IA_REQ_PARAM &IAReqParam, CXml &ReqXML)
{
	CHECK_SUCCESS(ReqXML.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(ReqXML.AddElem("MESSAGE"));
	CHECK_SUCCESS(ReqXML.AddChildElem("CV_CONTENT"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.AddChildElem("REQ_INFO"));
	CHECK_SUCCESS(ReqXML.IntoElem());

	CHECK_SUCCESS(ReqXML.AddChildElem("RequestID"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(IAReqParam.ulRequestID)).c_str()));

	CHECK_SUCCESS(ReqXML.AddElem("NVRCode"));
	CHECK_SUCCESS(ReqXML.SetElemValue(IAReqParam.NVRCode));

	CHECK_SUCCESS(ReqXML.AddElem("CameraID"));
	CHECK_SUCCESS(ReqXML.SetElemValue(IAReqParam.CameraID));

	CHECK_SUCCESS(ReqXML.AddElem("RuleType"));
	CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(IAReqParam.ulRuleType)).c_str()));

	CHECK_SUCCESS(ReqXML.AddElem("QueryType"));
	CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(IAReqParam.ulQueryType)).c_str()));

	CHECK_SUCCESS(ReqXML.AddElem("StartTime"));
	CHECK_SUCCESS(ReqXML.SetElemValue(IAReqParam.StartTime));

	CHECK_SUCCESS(ReqXML.AddElem("StopTime"));
	CHECK_SUCCESS(ReqXML.SetElemValue(IAReqParam.StopTime));

	ReqXML.OutOfElem();
	ReqXML.OutOfElem();
	ReqXML.OutOfElem();

	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::GetAnalyzeResultPraseXML(IVS_CHAR *szRspXML, IVS_INT32 &ulResultCode, IA_REQ_PARAM &IAReqParam)
{
	CHECK_POINTER(szRspXML, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("RspXML: %s",szRspXML);

	CXml xml;
	if (!xml.Parse((char *)szRspXML))
	{
		return IVS_FAIL;
	}

	CHECK_SUCCESS(xml.FindElem("MESSAGE"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("CV_CONTENT"));
	CHECK_SUCCESS(xml.IntoElem());
	const char* AttriValue = NULL;
	GET_ELEM_VALUE_NUM("RESULT_CODE", AttriValue, ulResultCode, xml);

	if (IA_QUERY_TYPE_RECORD == IAReqParam.ulQueryType)
	{
		GET_ELEM_VALUE_CHAR("LastRecordStopTime", AttriValue, IAReqParam.StartTime, IVS_TIME_LEN, xml);
	}
	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::TrackInfoContentPraseXML(const IVS_CHAR * pTrackXml, IA_LAYOUT_MESSAGE &pIALayoutMessage, IA_REQ_PARAM &IAReqParameters)
{
	CHECK_POINTER(pTrackXml, IVS_OPERATE_MEMORY_ERROR);
	BP_DBG_LOG("TrackXml: %s",pTrackXml);

	eSDK_MEMSET(&pIALayoutMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	CXml xmlMessage;
	if (!xmlMessage.Parse(pTrackXml))
	{
		return IVS_FAIL;
	}
	const char* AttriValue = NULL;
	CHECK_SUCCESS(xmlMessage.FindElem("MESSAGE"));
	CHECK_SUCCESS(xmlMessage.IntoElem());
	CHECK_SUCCESS(xmlMessage.FindElem("CV_CONTENT"));
	CHECK_SUCCESS(xmlMessage.IntoElem());

	// 解析DeviceID字段
	GET_ELEM_VALUE_CHAR("DeviceID", AttriValue, IAReqParameters.CameraID, IVS_DEV_CODE_LEN + 1, xmlMessage);	//lint !e838

	// 解析VideoType字段
	GET_ELEM_VALUE_NUM_FOR_ULONG("VideoType", AttriValue, IAReqParameters.ulQueryType, xmlMessage);

	// 解析RuleType字段
	GET_ELEM_VALUE_NUM_FOR_ULONG("RuleType", AttriValue, IAReqParameters.ulRuleType, xmlMessage);

	// 解析FrameID字段
	GET_ELEM_VALUE_NUM_FOR_UINT64("FrameID", AttriValue, pIALayoutMessage.ulFrameID, xmlMessage);

	if (IA_QUERY_TYPE_RECORD == IAReqParameters.ulQueryType)
	{
		// 解析RequestID字段
		GET_ELEM_VALUE_NUM_FOR_ULONG("RequestID", AttriValue, IAReqParameters.ulRequestID, xmlMessage);
	}

	const char *pLayoutInfo;	
	CHECK_SUCCESS(xmlMessage.FindElem("LayoutInfo"));
	CHECK_SUCCESS(NULL != (pLayoutInfo = xmlMessage.GetElemValue()));			

	CXml xml;
	if (!xml.Parse(pLayoutInfo))
	{
		return IVS_FAIL;
	}


	CHECK_SUCCESS(xml.FindElem("XMLLayoutMessage"));
	CHECK_SUCCESS(xml.IntoElem());

	char tempBuf[TEMP_BUF_MAX_LENGTH] = {0};


	// 解析FrameSize字段
	GET_ELEM_VALUE_CHAR("FrameSize", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
	CHECK_SUCCESS(IVS_SUCCEED == PraseFrameSize(tempBuf, pIALayoutMessage));


	CHECK_SUCCESS(xml.FindElem("LayoutElementList"));
	CHECK_SUCCESS(xml.IntoElem());

	ULONG ulElementNum = 0;
	bool elements_more_than_max = false;	//需要叠加的轨迹是否超过最大数，如果大于60条轨迹信息，就先插入前60条，多余的再次处理
	do
	{
		if (!ulElementNum && !elements_more_than_max)
		{
			CHECK_SUCCESS(xml.FindElem("LayoutElement"));
		}
		CHECK_SUCCESS(xml.IntoElem());

		// 解析Layer字段
		eSDK_MEMSET(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEM_VALUE_CHAR("Layer", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
		if (!strcmp("Alarmed", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulLayer = IA_LAYER_TYPE_ALARM;
		}
		if (!strcmp("ROI", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulLayer = IA_LAYER_TYPE_ROI;
		}

		// 解析Color字段
		eSDK_MEMSET(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEM_VALUE_CHAR("Color", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
		CHECK_SUCCESS(IVS_SUCCEED == PraseColor(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].ulColor));

		// 解析ElementType字段
		eSDK_MEMSET(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEM_VALUE_CHAR("ElementType", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
		if (!strcmp("Text", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulElementType = IA_ELEMENT_TYPE_TEXT;

			// 类型为Text时，解析TheText字段
			CHECK_SUCCESS(xml.FindElem("TheText"));
			CHECK_SUCCESS(xml.IntoElem());

			char *text = pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.TextContent;
			GET_ELEM_VALUE_CHAR("Text", AttriValue, text, IA_TEXT_LENGTH_MAX, xml);
			pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulTextLenth = strlen(text);

			GET_ELEM_VALUE_NUM_FOR_ULONG("Scale", AttriValue, pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulTextScale, xml);

			eSDK_MEMSET(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
			GET_ELEM_VALUE_CHAR("PointToDraw", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
			CHECK_SUCCESS(IVS_SUCCEED == PrasePoint(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.TextPoint));

			GET_ELEM_VALUE_NUM_FOR_ULONG("PercentPoint", AttriValue, pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulIsPercent, xml);
			BP_RUN_LOG_INF("Prase Track", "Is Percent: %lu", pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent);

			xml.OutOfElem();
		}
		if (!strcmp("Polyline", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulElementType = IA_ELEMENT_TYPE_POLYLINE;

			// 类型为Polyline时，解析ThePoints字段
			CHECK_SUCCESS(xml.FindElem("ThePoints"));
			CHECK_SUCCESS(xml.IntoElem());

			ULONG ulPointsNum = 0;
			GET_ELEM_VALUE_NUM_FOR_ULONG("NumberOfPoints", AttriValue, ulPointsNum, xml);	//lint !e838
			pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulPointsNum = ulPointsNum;

			GET_ELEM_VALUE_NUM_FOR_ULONG("PercentPoint", AttriValue, pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent, xml);
			BP_RUN_LOG_INF("Prase Track", "Is Percent: %lu", pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent);

			ULONG ulListPointsNum = 0;
			eSDK_MEMSET(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
			GET_ELEM_VALUE_CHAR("ElementPoints", AttriValue, tempBuf, TEMP_BUF_MAX_LENGTH, xml);
			CHECK_SUCCESS(IVS_SUCCEED == PrasePointList(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.Points, ulListPointsNum));

			//拌线需要增加方向，不是拌线没有该字段
			GET_ELEM_VALUE_NUM_FOR_ULONG("TripWireDirection", AttriValue, pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulDirection, xml);

			xml.OutOfElem();

			// 判断解析点列表获得的点数 与 传入的点数是否一致
			CHECK_SUCCESS(ulPointsNum == ulListPointsNum);

		}

		ulElementNum++;
		xml.OutOfElem();//LayoutElement

		//如果大于60条轨迹信息，就先插入前60条，多余的再次处理
		if (ulElementNum >= IA_ELEMENT_NUM_MAX)
		{
			pIALayoutMessage.ulLayoutElementNum = ulElementNum;
			(void)CIAMessageMgr::Instance().InsertTrackInfo(IAReqParameters, pIALayoutMessage);
			eSDK_MEMSET(pIALayoutMessage.LayoutElement, 0, sizeof(IA_LAYOUT_ELEMENT)*IA_ELEMENT_NUM_MAX);
			ulElementNum = 0;
			elements_more_than_max = true;
		}

	}while (xml.NextElem());
	pIALayoutMessage.ulLayoutElementNum = ulElementNum;

	xml.OutOfElem();//LayoutElementList
	xml.OutOfElem();//XMLLayoutMessage

	xmlMessage.OutOfElem();//CV_CONTENT
	xmlMessage.OutOfElem();//MESSAGE

	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::StopAnalyzeResultBuildXML(IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, CXml &ReqXML)
{
	CHECK_POINTER(CameraID, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("RequestID: %lu, RuleType: %lu, QueryType: %lu, CameraID: %s", ulRequestID, ulRuleType, ulQueryType, CameraID);

	CHECK_SUCCESS(ReqXML.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(ReqXML.AddElem("MESSAGE"));
	CHECK_SUCCESS(ReqXML.AddChildElem("CV_CONTENT"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.AddChildElem("REQ_INFO"));
	CHECK_SUCCESS(ReqXML.IntoElem());

	CHECK_SUCCESS(ReqXML.AddChildElem("CameraID"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.SetElemValue(CameraID));

	CHECK_SUCCESS(ReqXML.AddElem("RuleType"));
	CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(ulRuleType)).c_str()));

	CHECK_SUCCESS(ReqXML.AddElem("VideoType"));
	CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(ulQueryType)).c_str()));

	if (ulRequestID)
	{
		CHECK_SUCCESS(ReqXML.AddElem("RequestID"));
		CHECK_SUCCESS(ReqXML.SetElemValue((CToolsHelp::ULong2Str(ulRequestID)).c_str()));
	}

	ReqXML.OutOfElem();
	ReqXML.OutOfElem();
	ReqXML.OutOfElem();

	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::PraseFrameSize(char *tempBuf, IA_LAYOUT_MESSAGE &pIALayoutMessage) 
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(tempBuf, IVS_PARA_INVALID);

	char *outer_ptr = NULL;  
	char *token = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), ulFrameWidth);	//lint !e838
	pIALayoutMessage.ulFrameWidth = strtoul(token, NULL, 10);
	CHECK_STRTOK_POINTER(outer_ptr, ulFrameHeight);
	pIALayoutMessage.ulFrameHeight = strtoul(outer_ptr, NULL, 10);
	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::PraseColor(char *tempBuf, COLORREF &ulColor)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(tempBuf, IVS_PARA_INVALID);

	char *token = NULL;
	char *outer_ptr = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), Red);	//lint !e838
	unsigned char cRed = (unsigned char)atoi(token);
	CHECK_STRTOK_POINTER(token = strtok_s(NULL, ",", &outer_ptr), Green);
	unsigned char cGreen = (unsigned char)atoi(token);
	CHECK_STRTOK_POINTER(outer_ptr, Blue);
	unsigned char cBlue = (unsigned char)atoi(outer_ptr);
	ulColor = RGB(cRed, cGreen, cBlue);

	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::PrasePoint(char *tempBuf, POINT &point) 
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(tempBuf, IVS_PARA_INVALID);

	char *token = NULL;
	char *outer_ptr = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), point.x);	//lint !e838
	point.x = atol(token);
	CHECK_STRTOK_POINTER(outer_ptr, point.y);
	point.y = atol(outer_ptr);
	return IVS_SUCCEED;
}

IVS_INT32 CIAXMLProcess::PrasePointList(char *tempBuf, POINT *PointList, ULONG &ulListPointsNum) 
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(tempBuf, IVS_PARA_INVALID);
	CHECK_POINTER(PointList, IVS_PARA_INVALID);

	char *outer_ptr = NULL;
	char *PointToken = NULL;
	char *buf = tempBuf;
	ULONG   tot = 0;

	while((PointToken = strtok_s(buf, ";", &outer_ptr)) != NULL) 	//lint !e838
	{  
		buf = PointToken;
		if (IVS_SUCCEED != CIAXMLProcess::PrasePoint(buf, PointList[tot]))
		{
			return IVS_FAIL;
		}
		tot++;
		buf = NULL;   
	}  

	ulListPointsNum = tot;

	return IVS_SUCCEED;
}

