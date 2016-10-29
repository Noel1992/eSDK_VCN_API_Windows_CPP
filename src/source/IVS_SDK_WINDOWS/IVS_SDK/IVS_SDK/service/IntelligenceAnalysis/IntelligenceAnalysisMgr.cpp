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

#include "UserMgr.h"
#include "bp_log.h"
#include "Cmd.h"
#include "NSSOperator.h"
#include "IntelligenceAnalysisMgr.h"
#include "IVS_Trace.h"

CIntelligenceAnalysisMgr::CIntelligenceAnalysisMgr(void)
    : m_pUserMgr(NULL)
{
    
}

CIntelligenceAnalysisMgr::~CIntelligenceAnalysisMgr(void)
{
    m_pUserMgr = NULL;
}

void CIntelligenceAnalysisMgr::SetUserMgr( CUserMgr *pUserMgr)
{
    m_pUserMgr = pUserMgr;
}
// 智能分析统一透传接口
IVS_INT32 CIntelligenceAnalysisMgr::IntelligenceAnalysis(IVS_UINT32 uiInterfaceType, IVS_CHAR* pTransID, IVS_UINT32 uiTransIDLen, 
															const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
//	***********pReqXml可以为空*********************
//  CHECK_POINTER(pReqXml, NULL);
//	***********pTransID可以为空*********************
//  CHECK_POINTER(pTransID, NULL);
	IVS_DEBUG_TRACE("InterfaceType: %u", uiInterfaceType);

	CCmd* pCmd = NULL;
    // 获取本域SMU连接，连接状态为断开时，返回连接错误
    std::string strLinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strLinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	if (NULL == pTransID) //通用智能分析消息 IVS_SDK_IA
	{
		pCmd = CNSSOperator::instance().BuildSMUCmd((NSS_MSG_TYPE_E)uiInterfaceType, pReqXml, strLinkID);
	} 
	else if ('\0' == pTransID[0]) // 第一次请求录像轨迹叠加，需要返回生成的TransID
	{
		std::string strTransID = CMKTransID::Instance().GenerateTransID();
		if (!CToolsHelp::Strncpy(pTransID, uiTransIDLen, strTransID.c_str(), strlen(strTransID.c_str())))//lint !e64 匹配
		{
			BP_RUN_LOG_INF("Intelligence Analysis", "Get TransID %s", strTransID.c_str());
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Intelligence Analysis", "Get TransID Failed");
			return IVS_OPERATE_MEMORY_ERROR;
		}
		BP_RUN_LOG_INF("Intelligence Analysis", "Get TransID %s", pTransID);
		//std::string strLinkID = CNSSOperator::instance().GetSMULinkID();
		pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, (NSS_MSG_TYPE_E)uiInterfaceType, pReqXml, strTransID);//lint !e64 匹配
	} 
	else // 存在TransID时以该TransID发送命令
	{
		std::string strTransID = pTransID;
		//std::string strLinkID = CNSSOperator::instance().GetSMULinkID();
		pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, (NSS_MSG_TYPE_E)uiInterfaceType, pReqXml, strTransID);//lint !e64 匹配
	}
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	if (NSS_BA_ADD_VIDEO_SOURCE_REQ == uiInterfaceType)
	{
		BP_RUN_LOG_INF("NSS_BA_ADD_VIDEO_SOURCE_REQ","na");
		// 同步发送消息
		CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd, 60, 1);
		CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
		IVS_INT32 iRet = IVS_SUCCEED;
		*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
		HW_DELETE(pCmdRsp);

		return iRet;
	}
	else
	{
		std::string strCameraCode;
		std::string strDomainCode;

		CXml xmlReq;
		if (!xmlReq.Parse(pReqXml))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IntelligenceAnalysis prase xml fail", "NA");
			return IVS_XML_INVALID;
		}

		const char* pTmpCamera = NULL;
		const char* pTmpDomain = NULL;
		//设置前端智能分析规则计划和获取前端智能分析规则计划
		if (NSS_GET_PU_CAMERA_RULE_REQ == uiInterfaceType)
		{
			pTmpCamera = xmlReq.GetElemValueEx("MESSAGE/CV_CONTENT/CameraID");
			if (NULL != pTmpCamera)
			{
				strCameraCode = pTmpCamera;
			}
		}
		else if (NSS_SET_PU_CAMERA_RULE_REQ == uiInterfaceType)
		{
			pTmpCamera = xmlReq.GetElemValueEx("MESSAGE/CV_CONTENT/CommonParam/CameraID");
			if (NULL != pTmpCamera)
			{
				strCameraCode = pTmpCamera;
			}

		}
		else if ((NSS_GET_ANALYZE_RESULT_REQ == uiInterfaceType) || (NSS_STOP_PUSH_ANALYZE_RESULT_REQ == uiInterfaceType))
		{
			pTmpCamera = xmlReq.GetElemValueEx("MESSAGE/CV_CONTENT/REQ_INFO/CameraID");
			if (NULL != pTmpCamera)
			{
				strCameraCode = pTmpCamera;
			}
		}
		else if (NSS_GET_QD_PLAN_REQ == uiInterfaceType)
		{
			pTmpCamera = xmlReq.GetElemValueEx("Content/CameraID");
			if (NULL != pTmpCamera)
			{
				strCameraCode = pTmpCamera;
			}
			pTmpDomain = xmlReq.GetElemValueEx("Content/DomainCode");
			if (NULL != pTmpDomain)
			{
				strDomainCode = pTmpDomain;
			}
		}
		else if (NSS_SET_QD_PLAN_REQ == uiInterfaceType)
		{
			pTmpCamera = xmlReq.GetElemValueEx("Content/PlanList/PlanInfo/CameraID");
			if (NULL != pTmpCamera)
			{
				strCameraCode = pTmpCamera;
			}
			pTmpDomain = xmlReq.GetElemValueEx("Content/DomainCode");
			if (NULL != pTmpDomain)
			{
				strDomainCode = pTmpDomain;
			}
		}
		else
		{
			// 同步发送消息
			CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
			CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
			IVS_INT32 iRet = IVS_SUCCEED;
			*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
			HW_DELETE(pCmdRsp);

			return iRet;
		}
	
		//解析DomainCode和CameraCode
		IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
		IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
		IVS_INT32 iRet = CXmlProcess::ParseDevCode(strCameraCode.c_str(), chDevCode, chDomaCode);//lint !e64 匹配
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "IntelligenceAnalysis ", "(ParseDevCode)iRet=%d", iRet);
			return iRet;
		}

		if ( (xmlReq.FindElemEx("MESSAGE/CV_CONTENT/CameraID")) 
			|| (xmlReq.FindElemEx("MESSAGE/CV_CONTENT/CommonParam/CameraID")) 
			|| (xmlReq.FindElemEx("MESSAGE/CV_CONTENT/REQ_INFO/CameraID")))
		{
			xmlReq.ModifyElemValue(chDevCode);

		}
			
		unsigned int uiLen = 0;
		const char* pXmlReq = xmlReq.GetXMLStream(uiLen);

		if (0 == strlen(chDomaCode) && 0 == strDomainCode.length())
		{
			// 同步发送消息
			CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
			CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
			IVS_INT32 iRes = IVS_SUCCEED;
			*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRes);
			HW_DELETE(pCmdRsp);

			return iRes;
		}
		
		std::string strRsp;
		if(0 != strlen(chDomaCode))
		{
			iRet = m_pUserMgr->SendMsg(chDomaCode, uiInterfaceType, pXmlReq, strRsp, chDevCode);//lint !e734
		}
		else
		{
			iRet = m_pUserMgr->SendMsg(strDomainCode.c_str(), uiInterfaceType, pXmlReq, strRsp, chDevCode);//lint !e734 !e64 匹配
		}

		unsigned int uiRspLen = strRsp.length();
		char* xmlBuf = IVS_NEW(xmlBuf, uiRspLen+1);
		if (NULL != xmlBuf)
		{
			(void)memcpy_s(xmlBuf, uiRspLen, strRsp.c_str(), uiRspLen);
			xmlBuf[uiRspLen] = '\0'; 
			*pRspXml = xmlBuf;

			return iRet;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "new Rsp buffer fail.", "InterfaceType[%u]", uiInterfaceType);
			return IVS_OPERATE_MEMORY_ERROR;
		}
	}
}

