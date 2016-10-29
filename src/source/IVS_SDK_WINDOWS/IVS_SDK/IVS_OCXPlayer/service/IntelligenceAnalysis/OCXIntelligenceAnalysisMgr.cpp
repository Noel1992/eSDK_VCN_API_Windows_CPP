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

#include "OCXIntelligenceAnalysisMgr.h"
#include "stdafx.h"
#include "SDKDef.h"
#include "ToolsHelp.h"
#include "ivs_error.h"
#include "IVS_OCXPlayerCtrl.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"

COcxIntelligenceAnalysisMgr::COcxIntelligenceAnalysisMgr(void)
{
    
}

COcxIntelligenceAnalysisMgr::~COcxIntelligenceAnalysisMgr(void)
{
    
}

// 构建返回消息
void COcxIntelligenceAnalysisMgr::RetToCUMsg(CString &strResult, IVS_INT32 iRet, IVS_CHAR* pRspXml)
{
	IVS_DEBUG_TRACE("iRet: %d", iRet);

	CXml xmlRsp;
   
    if ((IVS_SUCCEED != iRet) || (NULL == pRspXml))
    {
		(void)xmlRsp.AddDeclaration("1.0","UTF-8","");
		(void)xmlRsp.AddElem("MESSAGE");
		(void)xmlRsp.AddChildElem("CV_CONTENT");
		(void)xmlRsp.IntoElem();
		(void)xmlRsp.AddChildElem("RESULT_CODE");
		(void)xmlRsp.IntoElem();
		(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		xmlRsp.OutOfElem();
		xmlRsp.OutOfElem();
		IVS_UINT32 uiLen = 0;
		strResult = xmlRsp.GetXMLStream(uiLen);
    } else {
        strResult = pRspXml;
    }
    (void)IVS_SDK_ReleaseBuffer(pRspXml);

    return;
}


// 智能分析统一透传接口
void COcxIntelligenceAnalysisMgr::IntelligenceAnalysis(CString &strResult, IVS_INT32 iSessionID, ULONG ulInterfaceType, const IVS_CHAR* pReqXml)
{
//	***********pReqXml可以为空*********************
	return;
//  CHECK_POINTER(pReqXml, NULL);
	//IVS_DEBUG_TRACE("SessionID: %d, InterfaceType: %lu", iSessionID, ulInterfaceType);

	//IVS_CHAR* pRspXml = NULL;
	//IVS_INT32 iRet = IVS_SDK_IA(iSessionID, ulInterfaceType, pReqXml, &pRspXml);
	//if (IVS_SUCCEED != iRet)
	//{
	//	BP_RUN_LOG_ERR(iRet, "IVS SDK IA failed", "SessionID: %d, InterfaceType: %lu, ReqXml: %s", iSessionID, ulInterfaceType, pReqXml);
	//}
	//RetToCUMsg(strResult, iRet, pRspXml);
}


