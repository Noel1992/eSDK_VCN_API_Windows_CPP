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

#include "LogMgr.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "bp_log.h"
#include "UserMgr.h"
#include "ivs_xml.h"
#include "XmlProcess.h"
#include "IVS_Trace.h"

#include "LogMgrXMLProcess.h"
CLogMgr::CLogMgr(void):m_pUserMgr(NULL)
{
	
}


CLogMgr::~CLogMgr(void)
{
	m_pUserMgr = NULL;
}

// �����û�������
void CLogMgr::SetUserMgr(void *pUserMgr)
{
	m_pUserMgr = (CUserMgr *)pUserMgr;
}

// ��ѯ������־
IVS_INT32 CLogMgr::GetOperationLog(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_OPERATION_LOG_LIST* pResOperLogList,const IVS_UINT32& uiBufSize) const
{	
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResOperLogList,IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
// �ѽṹ��pUnifiedQuery��iLoginID��װ��xmlReq��
	CXml xmlReq;
	IVS_INT32 iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Get Unified Format Query XML is fail");
		//modify by zwx211831 ���Ĵ��󷵻���
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq,IVS_OPERATE_MEMORY_ERROR);
	// ����������̳�����֧�ֶ���
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID || QUERY_OPERATION_OBJECT_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, szCameraCode, szDomainCode);
		}
	}

	// ��װ����
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_OPERATION_LOG_REQ);
	sendNssMsgInfo.SetReqData(pReq);
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	
 /************************************************************************
    ** ��Կ�������̣���ʼ��->��������ID->����Կ->��Կ����(���ɵ�����ID)->
    **              ��������->ʹ����Կ�������ɵ�����ID����Cmd->������Ϣ                                                                   
    ************************************************************************/
    // ��ʼ����Կ���������,�����Կ����   
    CSDKSecurityClient oSecurityClient;
   
	string strLinkID;
	std::string strDomainCode;	
	m_pUserMgr->GetDomainCode(strDomainCode);
	int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID);
	if (IVS_SUCCEED != iMultiExchangeKey)
	{
		BP_RUN_LOG_ERR(iMultiExchangeKey, "CAlarmMgr::GetConfigInfo", "Exchange Key Failed.");
		return iMultiExchangeKey;
	}
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	
	iRet = m_pUserMgr->SendSynCmdContainsExchangeKey(sendNssMsgInfo,strLinkID,strpRsp,strSecurityTransID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Send Cmd","SendSynCmd failed");
		return iRet;
	}

    CXml xmlRsp;
    if (!xmlRsp.Parse(strpRsp.c_str()))//lint !e64 ƥ��
    {
        BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Operation Log", "xmlRsp.Parse(pRsp) is fail");
        return IVS_SMU_XML_INVALID;
    }
    // ����XML��Ӧ��ϢΪ�ṹ��
    iRet = CLogMgrXMLProcess::PraseResOperationLogXML(xmlRsp,pResOperLogList,uiBufSize,oSecurityClient);	
	
	if (iRet != IVS_SUCCEED)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Prase Response Operation Log XML is fail");
		return iRet;
	}
	return iRet;
}


//��չ��ѯ������־
IVS_INT32 CLogMgr::GetOperationLogEx(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
	IVS_OPERATION_LOG_LIST* pResOperLogList, 
	const IVS_UINT32 uiBufSize) const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery,IVS_PARA_INVALID);
	CHECK_POINTER(pOptLogQueryEx, IVS_PARA_INVALID);
	CHECK_POINTER(pResOperLogList, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("GetOperationLogEx NVRCode: %s, Module Name: %s", pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName);

	IVS_INT32 iRet = IVS_FAIL;

	//�����������С�Ƿ���ȷ
	if ( (sizeof(IVS_OPERATION_LOG_LIST) + (pUnifiedQuery->stIndex.uiToIndex - pUnifiedQuery->stIndex.uiFromIndex) * sizeof(IVS_OPERATION_LOG_INFO) )
		!= uiBufSize )
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "Get Cluster Status", "Reason:Buffer Size is Error");
		return iRet;
	}

	// �ѽṹ��pUnifiedQuery��iLoginID��װ��xmlReq��
	CXml xmlReq;
	iRet = CLogMgrXMLProcess::GetResOperationLogExXML(pUnifiedQuery, pOptLogQueryEx, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Operation Log", "Get Unified Format Query XML is fail");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq,IVS_OPERATE_MEMORY_ERROR);
	// ����������̳�����֧�ֶ���
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
	{
		if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID || QUERY_OPERATION_OBJECT_CODE == pUnifiedQuery->stQueryField[i].eFieID)
		{
			(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, szCameraCode, szDomainCode);
		}
	}

	// ��װ����
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_OPERATION_LOG_REQ);
	sendNssMsgInfo.SetReqData(pReq);
	sendNssMsgInfo.SetCameraCode(szCameraCode);
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	
 /************************************************************************
    ** ��Կ�������̣���ʼ��->��������ID->����Կ->��Կ����(���ɵ�����ID)->
    **              ��������->ʹ����Կ�������ɵ�����ID����Cmd->������Ϣ                                                                   
    ************************************************************************/
    // ��ʼ����Կ���������,�����Կ����   
    CSDKSecurityClient oSecurityClient;
   
	string strLinkID;
	std::string strDomainCode;	
	m_pUserMgr->GetDomainCode(strDomainCode);
	int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID);
	if (IVS_SUCCEED != iMultiExchangeKey)
	{
		BP_RUN_LOG_ERR(iMultiExchangeKey, "CAlarmMgr::GetConfigInfo", "Exchange Key Failed.");
		return iMultiExchangeKey;
	}
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	
	iRet = m_pUserMgr->SendSynCmdContainsExchangeKey(sendNssMsgInfo,strLinkID,strpRsp,strSecurityTransID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Send Cmd","SendSynCmd failed");
		return iRet;
	}

	// ����XML��Ӧ��Ϣ
    CXml xmlRsp;
    if (!xmlRsp.Parse(strpRsp.c_str()))//lint !e64 ƥ��
    {
        BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Operation Log", "xmlRsp.Parse(pRsp) is fail");
        return IVS_SMU_XML_INVALID;
    }

	iRet =CLogMgrXMLProcess::PraseResOperationLogXML(xmlRsp, pResOperLogList, uiBufSize, oSecurityClient);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Extending OperationLog", "Reason:Parse respond Xml failed");
		return iRet;
	}

	return iRet;
}
