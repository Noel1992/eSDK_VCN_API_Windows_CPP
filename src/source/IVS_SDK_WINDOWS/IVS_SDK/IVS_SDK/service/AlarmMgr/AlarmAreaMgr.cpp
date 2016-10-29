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

#include "AlarmAreaMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "ivs_error.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "AlarmMgrXMLProcess.h"
#include "IVS_Trace.h"
CAlarmAreaMgr::CAlarmAreaMgr(void)
	:m_pUserMgr(NULL)
{
}
CAlarmAreaMgr::~CAlarmAreaMgr(void)
{
	m_pUserMgr = NULL;
}

void CAlarmAreaMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}



// 新增防区
IVS_INT32 CAlarmAreaMgr::AddAlarmArea(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

	if (IVS_SUCCEED != AddAlarmAreaShadowDevCheck(xmlReq))
	{
		BP_RUN_LOG_ERR(IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV, "AddAlarmArea: Shadow Dev Not Support", "");
		return IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV;
	}

	std::string strDomainCode;
    
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	// 拆分AlarmInCode，添加DevDomainCode节点(可以增加空防区，即可能没有告警源设备信息信息)
	IVS_INT32 iRet = CAlarmMgrXMLProcess::SetAlarmInCode4AlarmAreaReq(xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Alarm In Code fail", "NA");
		return iRet;
	}
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	iRet = CAlarmMgrXMLProcess::GetRsqXML(strpRsp.c_str(), *pRspXml);//lint !e64 匹配
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Add AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

//修改防区
IVS_INT32 CAlarmAreaMgr::ModifyAlarmArea(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	// 获取域编码
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

	if (IVS_SUCCEED != AddAlarmAreaShadowDevCheck(xmlReq))
	{
		BP_RUN_LOG_ERR(IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV, "ModifyAlarmArea: Shadow Dev Not Support", "");
		return IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV;
	}

    std::string strDomainCode;

    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	// 拆分AlarmInCode，添加DevDomainCode节点
	// 只修改防区名称没有AlarmInList节点；修改防区的设备带AlarmInList节点；如果只有AlarmInList节点，但是没有AlarmInInfo节点，表示删除防区下的所有设备
	IVS_INT32 iRet = CAlarmMgrXMLProcess::SetAlarmInCode4AlarmAreaReq(xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Alarm In Code of Modify AlarmArea fail", "NA");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqMsg = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqMsg, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pReqMsg);
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Modify AlarmArea fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 删除防区
IVS_INT32 CAlarmAreaMgr::DeleteAlarmArea(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	unsigned int xmlLen = 0;
	const IVS_CHAR * pAlarmArea = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pAlarmArea, IVS_XML_PACK_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_ALARM_AREA_REQ);
	sendNssMsgInfo.SetReqData(pAlarmArea);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Delete Alarm Area fail","Send Cmd failed");
		return iRet;
	}

	return iRet;
}

// 查看防区列表
IVS_INT32 CAlarmAreaMgr::GetAlarmAreaList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 获取域编码，拆分AlarmInCode
	std::string strDomainCode;
	CXml xml;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetDomainCodeANDParseAlarmInCode(pReqXml, strDomainCode, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainCodeANDParseAlarmInCode fail", "NA");
		return iRet;
	}

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_ALARM_AREA_LIST_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Alarm Area List fail","Send Cmd failed");
		return iRet;
	}

	// 响应消息中的DevDomainCode拼回给AlarmInCode
	CXml xmlRsq;
	if (!xmlRsq.Parse(strpRsp.c_str()))//lint !e64 匹配
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.Parse(strpRsp.c_str()) fail", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (xmlRsq.FindElemEx("Content/AlarmAreaList"))
	{
		BP_RUN_LOG_INF("FindElemEx(Content/AlarmAreaList)", "NA");
		if (!xmlRsq.FindElem("AlarmAreaInfo"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
			return IVS_SMU_XML_INVALID;
		}
		do 
		{
			xmlRsq.IntoElem();
			if (xmlRsq.FindElem("AlarmInList"))
			{
				xmlRsq.IntoElem();
				if (!xmlRsq.FindElem("AlarmInInfo"))
				{
					BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
					return IVS_SMU_XML_INVALID;
				}
				do 
				{
					xmlRsq.IntoElem();
					const IVS_CHAR* szElemValue = NULL;
					IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
					IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
					GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, szDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsq);//lint !e838
					GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, szAlarmInCode, IVS_ALARM_CODE_LEN, xmlRsq);
					std::string strAlarmInCode = szAlarmInCode;
					if (!strAlarmInCode.empty())
					{
						strAlarmInCode.append("#").append(szDomainCode);
						xmlRsq.ModifyElemValue(strAlarmInCode.c_str());//lint !e64 匹配
					}
					xmlRsq.OutOfElem();
				} while (xmlRsq.NextElem());
				xmlRsq.OutOfElem();
			}
			xmlRsq.OutOfElem();
		} while (xmlRsq.NextElem());
	}	

	unsigned int xmlLen = 0;
	const IVS_CHAR* pRsq = xmlRsq.GetXMLStream(xmlLen);
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);

	iRet = CAlarmMgrXMLProcess::GetRsqXML(pRsq, *pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Get AlarmArea List fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 查看防区详情
IVS_INT32 CAlarmAreaMgr::GetAlarmAreaInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId,IVS_CHAR** pAlarmArea)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmArea, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId,xmlReq);	

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_ALARM_AREA_INFO_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get AlarmArea Info fail","Send Cmd failed");
		return iRet;
	}

	// 响应消息中的DevDomainCode拼回给AlarmInCode
	CXml xmlRsq;
	if (!xmlRsq.Parse(strpRsp.c_str()))//lint !e64 匹配
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.Parse(strpRsp.c_str()) fail", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (xmlRsq.FindElemEx("Content/AlarmAreaInfo/AlarmInList"))
	{
		if (!xmlRsq.FindElem("AlarmInInfo"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xmlRsq.FindElem(\"AlarmInInfo\") fail", "NA");
			return IVS_SMU_XML_INVALID;
		}
		do 
		{
			xmlRsq.IntoElem();
			const IVS_CHAR* szElemValue = NULL;
			IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
			IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
			GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, szDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsq);//lint !e838
			GET_ELEM_VALUE_CHAR("AlarmInCode", szElemValue, szAlarmInCode, IVS_ALARM_CODE_LEN, xmlRsq);
			std::string strAlarmInCode = szAlarmInCode;
			if (!strAlarmInCode.empty())
			{
				strAlarmInCode.append("#").append(szDomainCode);
				xmlRsq.ModifyElemValue(strAlarmInCode.c_str());//lint !e64 匹配
			}

			xmlRsq.OutOfElem();
		} while (xmlRsq.NextElem());
	}	

	const IVS_CHAR* pRsq = xmlRsq.GetXMLStream(xmlLen);
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);

	iRet = CAlarmMgrXMLProcess::GetRsqXML(pRsq, *pAlarmArea);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmAreaInfo fail", "NA");
		return iRet;
	}

	return iRet;
}

// 手动布防
IVS_INT32 CAlarmAreaMgr::StartAlarmAreaGuard(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_START_AREA_GUARD_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start AlarmArea Guard fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 手动撤防
IVS_INT32 CAlarmAreaMgr::StopAlarmAreaGuard(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::AlarmAreaGetXML(pDomainCode, uiAlarmAreaId, xmlReq);
	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(uiXmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_STOP_AREA_GUARD_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Stop AlarmArea Guard fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 新增布防计划
IVS_INT32 CAlarmAreaMgr::AllocAreaGuardPlan(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}


	std::string strDomainCode;
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ALLOC_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Alloc AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 修改布防计划
IVS_INT32 CAlarmAreaMgr::ModifyAreaGuardPlan(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/DomainCode\") fail", "NA");
		return IVS_XML_INVALID;
	}

	std::string strDomainCode;
    const IVS_CHAR* pDomainCode = xmlReq.GetElemValue();
    if (NULL != pDomainCode)
    {
        strDomainCode = pDomainCode;
    }

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Modify AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 删除布防计划
IVS_INT32 CAlarmAreaMgr::DeleteAreaGuardPlan(const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::GetAlarmAreaPlanXML(pDomainCode, uiAlarmAreaId,xmlReq);

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Delete Area Guard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}

// 查看布防计划
IVS_INT32 CAlarmAreaMgr::GetAreaGuardPlan(const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	(void)CAlarmMgrXMLProcess::GetAlarmAreaPlanXML(pDomainCode, uiAlarmAreaId,xmlReq);
	
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReqXml = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_AREA_GUARD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Area Guard Plan fail","Send Cmd failed");
		return iRet;
	}

	iRet = CAlarmMgrXMLProcess::GetRsqXML(strpRsp.c_str(), *pRspXml);//lint !e64 匹配
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Rsq XML Get AreaGuard Plan fail","Send Cmd failed");
		return iRet;
	}
	return iRet;
}



//增加防区消息的影子IPC校验
IVS_INT32 CAlarmAreaMgr::AddAlarmAreaShadowDevCheck(CXml& reqXml)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CDeviceMgr& deviceMgr = m_pUserMgr->GetDeviceMgr();

	//查找是否有Content/AlarmAreaInfo/AlarmInList元素
	if (!reqXml.FindElemEx("Content/AlarmAreaInfo/AlarmInList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.FindElemEx('Content/DevList'')", "NA");
		return IVS_SUCCEED;//这里异常分支都不做异常处理
	}

	if (!reqXml.FindElem("AlarmInInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == reqXml.FindElem(DevInfo)", "NA");
		return IVS_SUCCEED;
	}


	const IVS_CHAR* pTemp = NULL;
	IVS_CHAR szDevCode[IVS_DEV_CODE_LEN + 1];
	do 
	{
		if (!reqXml.FindElemValue("AlarmInInfo") && !reqXml.NextElem())
		{
			break;
		}
		reqXml.IntoElem();  

		//设备编码
		eSDK_MEMSET(szDevCode, 0, IVS_DEV_CODE_LEN + 1);
		GET_ELEM_VALUE_CHAR("AlarmInCode", pTemp, szDevCode, IVS_DEV_CODE_LEN, reqXml);

		if (deviceMgr.IsShadowDev(szDevCode))
		{
			BP_RUN_LOG_ERR(IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV, "AddAlarmAreaShadowDevCheck: Shadow Dev Not Support", 
				"szDevCode: %s", szDevCode);
			return IVS_SDK_CUR_SES_NO_SUPPORT_SHADOW_DEV;
		}

		reqXml.OutOfElem();

	} while (reqXml.NextElem());  

	return IVS_SUCCEED;
}





