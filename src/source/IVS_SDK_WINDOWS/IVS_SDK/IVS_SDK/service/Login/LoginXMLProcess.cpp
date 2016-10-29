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
filename : LoginXMLProcess.cpp
author : x00192614
created : 2012/12/03
description : ��¼xmlƴװ����
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 ��ʼ�汾
*********************************************************************/
#include "LoginXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "SDKSecurityClient.h"
#include "IVS_Trace.h"
/*!< socketͷ�ļ� */
#ifdef WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#else
#include <iconv.h>
#define _XOPEN_SOURCE 500
#include <signal.h>
#endif

#if VOS_APP_OS == VOS_OS_LINUX
void u_alarm_handler(int i)
{
	BP_RUN_LOG_ERR(IVS_FAIL, "connect timeout", "NA");
}
#endif


CLoginXMLProcess::CLoginXMLProcess(void)
{
}

CLoginXMLProcess::~CLoginXMLProcess(void)
{
}

// ��ȡ��¼XML
IVS_INT32 CLoginXMLProcess::GetLoginXML(const IVS_LOGIN_INFO* pLoginReqInfo, const std::string& strLoginName, const std::string& strLoginPWD,CUserMgr *pUserObj, CXml& xmlReq )
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserObj, IVS_OPERATE_MEMORY_ERROR);
	std::string strServerIp = pLoginReqInfo->stIP.cIP;
	std::string strLocalIp;
	GetLocalIp(strServerIp.c_str(), static_cast<int>(pLoginReqInfo->uiPort), strLocalIp);
	if (strLocalIp.empty())
	{
		BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "CToolsHelp::GetLocalIp fail");
		return IVS_NET_RECV_TIMEOUT;
	}

     //���汾��IP��������ʵ������ʹ��
    CUserInfo *userInfo = pUserObj->GetUserInfoEx();
    CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
    userInfo->SetUserLocalIp(strLocalIp);   

    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    (void)xmlReq.IntoElem();

    //xmlReq.AddChildElem("LoginID");
    //xmlReq.IntoElem();
    //xmlReq.SetElemValue("0");

    //xmlReq.AddElem("LoginName");
    //xmlReq.SetElemValue(sLoginParas.cUserName);

    (void)xmlReq.AddChildElem("LoginName");
    (void)xmlReq.IntoElem();

    (void)xmlReq.SetElemValue(strLoginName.c_str());

	(void)xmlReq.AddElem("LoginPWD");

	(void)xmlReq.SetElemValue(strLoginPWD.c_str());

	(void)xmlReq.AddElem("LoginIP");
	(void)xmlReq.SetElemValue(strLocalIp.c_str()); // ����IP

	(void)xmlReq.AddElem("LoginPort");            // ���ض˿�
	(void)xmlReq.SetElemValue("0");

	(void)xmlReq.AddElem("LoginType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(pLoginReqInfo->uiLoginType)).c_str());

	(void)xmlReq.AddElem("DomainName");
    char cTempDomainName[IVS_DOMAIN_LEN + 1] = {0};
    eSDK_MEMCPY(cTempDomainName, IVS_DOMAIN_LEN + 1, pLoginReqInfo->cDomainName, IVS_DOMAIN_LEN);
	(void)xmlReq.SetElemValue(cTempDomainName);

	(void)xmlReq.AddElem("MachineName");
    char cTempMachineName[IVS_MACHINE_NAME_LEN + 1] = {0};
    eSDK_MEMCPY(cTempMachineName, IVS_MACHINE_NAME_LEN + 1, pLoginReqInfo->cMachineName, IVS_MACHINE_NAME_LEN);
	(void)xmlReq.SetElemValue(cTempMachineName);

    (void)xmlReq.AddElem("ClientType");
    (void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfo->uiClientType).c_str());

    (void)xmlReq.AddElem("ServerIP");
    char cTempSerIp[IVS_IP_LEN + 1] = {0};
    eSDK_MEMCPY(cTempSerIp, IVS_IP_LEN + 1, pLoginReqInfo->stIP.cIP, IVS_IP_LEN);
    (void)xmlReq.SetElemValue(cTempSerIp);

    (void)xmlReq.AddElem("ServerPort");
    (void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfo->uiPort).c_str());

	xmlReq.OutOfElem();

	BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "ok");
	return IVS_SUCCEED;
}


//��ȡ�����¼����xml;
IVS_INT32 CLoginXMLProcess::GetLoginByTicketXML(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, CUserMgr *pUserObj, CXml& xmlReq )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginReqInfoEx, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUserObj, IVS_OPERATE_MEMORY_ERROR);

	std::string strLocalIp;
	GetLocalIp(pLoginReqInfoEx->stIP.cIP, static_cast<int>(pLoginReqInfoEx->uiPort), strLocalIp);
	if (strLocalIp.empty())
	{
		BP_RUN_LOG_INF("Leave CLogin::GetLoginXML", "CToolsHelp::GetLocalIp fail");
		return IVS_NET_RECV_TIMEOUT;
	}

	//���汾��IP��������ʵ������ʹ��
	CUserInfo *userInfo = pUserObj->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	userInfo->SetUserLocalIp(strLocalIp);   

	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.IntoElem();

	(void)xmlReq.AddChildElem("TicketID");
	(void)xmlReq.IntoElem();
	IVS_CHAR cTmpTicketID[IVS_TICKET_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cTmpTicketID, IVS_TICKET_LEN + 1, pLoginReqInfoEx->cTicketID, IVS_TICKET_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetLoginByTicketXML", "Copy TicketID Failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xmlReq.SetElemValue(cTmpTicketID);

	(void)xmlReq.AddElem("LoginIP");
	(void)xmlReq.SetElemValue(strLocalIp.c_str()); // ����IP

	(void)xmlReq.AddElem("LoginPort");            // ���ض˿�;
	(void)xmlReq.SetElemValue("0");

	(void)xmlReq.AddElem("LoginType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(pLoginReqInfoEx->uiLoginType)).c_str());

	(void)xmlReq.AddElem("DomainName");
	char cTempDomainName[IVS_DOMAIN_LEN + 1] = {0};
	eSDK_MEMCPY(cTempDomainName, IVS_DOMAIN_LEN + 1, pLoginReqInfoEx->cDomainName, IVS_DOMAIN_LEN);
	(void)xmlReq.SetElemValue(cTempDomainName);

	(void)xmlReq.AddElem("MachineName");
	char cTempMachineName[IVS_MACHINE_NAME_LEN + 1] = {0};
	eSDK_MEMCPY(cTempMachineName, IVS_MACHINE_NAME_LEN + 1, pLoginReqInfoEx->cMachineName, IVS_MACHINE_NAME_LEN);
	(void)xmlReq.SetElemValue(cTempMachineName);

	(void)xmlReq.AddElem("ClientType");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfoEx->uiClientType).c_str());

	(void)xmlReq.AddElem("ServerIP");
	char cTempSerIp[IVS_IP_LEN + 1] = {0};
	eSDK_MEMCPY(cTempSerIp, IVS_IP_LEN + 1, pLoginReqInfoEx->stIP.cIP, IVS_IP_LEN);
	(void)xmlReq.SetElemValue(cTempSerIp);

	(void)xmlReq.AddElem("ServerPort");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str((int)pLoginReqInfoEx->uiPort).c_str());

	xmlReq.OutOfElem();

	BP_RUN_LOG_INF("Leave CLogin::GetLoginByTicketXML", "ok");
	return IVS_SUCCEED;
}


IVS_INT32 CLoginXMLProcess::GetReLoginXML(const std::string& strLoginName, const std::string& strLoginPWD, CUserInfo *pUserInfo, CXml& reqXml )
{
    IVS_DEBUG_TRACE("");
    CHECK_POINTER(pUserInfo, IVS_OPERATE_MEMORY_ERROR);
    std::string strLocalIp = pUserInfo->GetUserLocalIp();

    (void)reqXml.AddDeclaration("1.0","UTF-8","");
    (void)reqXml.AddElem("Content");
    (void)reqXml.IntoElem();

    //xmlReq.AddChildElem("LoginID");
    //xmlReq.IntoElem();
    //xmlReq.SetElemValue("0");

    //xmlReq.AddElem("LoginName");
    //xmlReq.SetElemValue(sLoginParas.cUserName);

    (void)reqXml.AddChildElem("LoginName");
    (void)reqXml.IntoElem();
    (void)reqXml.SetElemValue(strLoginName.c_str());

    (void)reqXml.AddElem("LoginPWD");
    (void)reqXml.SetElemValue(strLoginPWD.c_str());

    (void)reqXml.AddElem("LoginIP");
    (void)reqXml.SetElemValue(strLocalIp.c_str()); // ����IP

    (void)reqXml.AddElem("LoginPort");            // ���ض˿�
    (void)reqXml.SetElemValue("0");

    (void)reqXml.AddElem("LoginType");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetLoginType()).c_str());

    (void)reqXml.AddElem("DomainName");
    (void)reqXml.SetElemValue(pUserInfo->GetDomainName().c_str());

    (void)reqXml.AddElem("MachineName");
    (void)reqXml.SetElemValue(pUserInfo->GetMachineName().c_str());

    (void)reqXml.AddElem("ClientType");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetClientType()).c_str());

    (void)reqXml.AddElem("ServerIP");
    (void)reqXml.SetElemValue(pUserInfo->GetServerIP().c_str());

    (void)reqXml.AddElem("ServerPort");
    (void)reqXml.SetElemValue(CToolsHelp::Int2Str(pUserInfo->GetServerPort()).c_str());

    reqXml.OutOfElem();
    return IVS_SUCCEED;
}
// ��ȡ�������λXML
IVS_INT32 CLoginXMLProcess::GetFineMainDevXML(const std::string& strDevCode, CXml& xmlReq )
{
	IVS_DEBUG_TRACE("");
	if (strDevCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get FineMain Dev XML", "DevCode is empty");
		return IVS_FAIL;
	}
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.AddChildElem("CameraInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("CameraCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(strDevCode.c_str());
	xmlReq.OutOfElem();
	return IVS_SUCCEED;
}

// ����NvrCode��Ϣ
IVS_INT32 CLoginXMLProcess::ParseNvrCodeXML(CXml& xmlRsq, char* pNvrCode)
{
	const char*AttriValue = NULL;   //��ʱ�洢�����ڵ�ֵ
	 if (xmlRsq.FindElemEx("Content/NVRInfo"))
	 {
		 (void)xmlRsq.IntoElem();
		 GET_ELEM_VALUE_CHAR("NVRCode",AttriValue, pNvrCode,IVS_NVR_CODE_LEN,xmlRsq);
		 return IVS_SUCCEED;
	 }
// 	if (!xmlRsq.FindElemEx("Content"))
// 	{
// 		BP_RUN_LOG_INF("Leave CLoginXMLProcess::ParseNvrCodeXML", "Content is null");
// 		return IVS_FAIL;
// 	}
// 	(void)xmlRsq.IntoElem();
// 	if (!xmlRsq.FindElemEx("IPCInfo"))
// 	{
// 		BP_RUN_LOG_INF("Leave CLoginXMLProcess::ParseNvrCodeXML", "IPCInfo is null");
// 		return IVS_FAIL;
// 	}
// 	(void)xmlRsq.IntoElem();
// 	GET_ELEM_VALUE_CHAR("IPCCode",AttriValue,pNvrCode,IVS_DEV_CODE_LEN,xmlRsq);
	 BP_RUN_LOG_ERR(IVS_FAIL, "Parse NvrCode XML", "Get NVRCode error");
	return IVS_FAIL;
}

// �û�����֪ͨ
IVS_INT32 CLoginXMLProcess::ParseUserOffLineXML(CXml& xmlRsq, const IVS_CHAR* /*pLoginID*/, IVS_USER_OFFLINE_INFO *stOffLineInfo)
{
    //eSDK_MEMSET(&stOffLineInfo, 0, sizeof(stOffLineInfo));
    IVS_DEBUG_TRACE("");
    const char *AttriValue = NULL;   //��ʱ�洢�����ڵ�ֵ

    if (!xmlRsq.FindElemEx("/"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CLoginXMLProcess::ParseUserOffLineXML", "Content is null");
        return IVS_XML_INVALID;
    }
    //GET_ELEM_VALUE_CHAR("LoginID", AttriValue, pLoginID, LOGIN_ID_LEN, xmlRsq);
    GET_ELEM_VALUE_NUM_FOR_UINT("OfflineType", AttriValue, stOffLineInfo->uiOfflineType, xmlRsq);
    return IVS_SUCCEED;
}

// ������¼��ӦXML
IVS_INT32 CLoginXMLProcess::ParseLoginXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRsp)
{
	IVS_DEBUG_TRACE("");
	eSDK_MEMSET(&stSRULoginRsp, 0, sizeof(ST_SRU_LOGIN_RSP));
	if (!xmlRsq.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "Node Content not found");
		return IVS_FAIL;
	}
	(void)xmlRsq.IntoElem();
    if (!xmlRsq.FindElem("LoginID"))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "Node LoginID not found");
        return IVS_FAIL;
    }
    std::string strLoginID;
    const char* AttriValue = xmlRsq.GetElemValue();
    if (NULL == AttriValue)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "LoginID is NUll");
        return IVS_FAIL;
    }
    if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "DecryptString LoginID failed");
        return IVS_FAIL;
    }
    if(!CToolsHelp::Memcpy(stSRULoginRsp.szLoginID, LOGIN_ID_LEN, strLoginID.c_str(), strLoginID.length()))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse LoginXML", "Memcpy LoginID failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
    //GET_ELEM_VALUE_CHAR("LoginID",AttriValue,stSRULoginRsp.szLoginID,LOGIN_ID_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("SessionID",AttriValue,stSRULoginRsp.szSessionID,SESSION_ID_LEN,xmlRsq);
    GET_ELEM_VALUE_NUM("KeepAliveTime",AttriValue,stSRULoginRsp.iKeepAliveTime,xmlRsq);
    GET_ELEM_VALUE_NUM("KeepAliveCnt",AttriValue,stSRULoginRsp.iKeepAliveCount,xmlRsq);
    GET_ELEM_VALUE_CHAR("TimeZone",AttriValue,stSRULoginRsp.szTimeZone,TIME_ZONE_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("Time",AttriValue,stSRULoginRsp.szTime,TIME_LEN,xmlRsq);
    GET_ELEM_VALUE_CHAR("PlayType",AttriValue,stSRULoginRsp.cPlatformVersion,PLATFORM_VERSION_LEN,xmlRsq);
    GET_ELEM_VALUE_NUM("OMUPort",AttriValue,stSRULoginRsp.iOmuPort,xmlRsq);
    GET_ELEM_VALUE_NUM_FOR_UINT("UserID",AttriValue,stSRULoginRsp.uiUserID,xmlRsq);
	return IVS_SUCCEED;
}


//���������¼��Ӧxml;
IVS_INT32 CLoginXMLProcess::ParseLoginByTicketXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRspEx)
{
	IVS_DEBUG_TRACE("");
	eSDK_MEMSET(&stSRULoginRspEx, 0, sizeof(ST_SRU_LOGIN_RSP));

	if (!xmlRsq.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "Node Content not found");
		return IVS_FAIL;
	}
	(void)xmlRsq.IntoElem();
	if (!xmlRsq.FindElem("LoginID"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "Node LoginID not found");
		return IVS_FAIL;
	}
	std::string strLoginID;
	const char* AttriValue = xmlRsq.GetElemValue();
	if (NULL == AttriValue)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "LoginID is NUll");
		return IVS_FAIL;
	}
	if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML By Ticket", "DecryptString LoginID failed");
		return IVS_FAIL;
	}
	if(!CToolsHelp::Memcpy(stSRULoginRspEx.szLoginID, LOGIN_ID_LEN, strLoginID.c_str(), strLoginID.length()))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse LoginXML By Ticket", "Memcpy LoginID failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	GET_ELEM_VALUE_CHAR("SessionID",AttriValue,stSRULoginRspEx.szSessionID,SESSION_ID_LEN,xmlRsq);
	GET_ELEM_VALUE_NUM_FOR_UINT("UserID",AttriValue,stSRULoginRspEx.uiUserID,xmlRsq);
	GET_ELEM_VALUE_NUM("KeepAliveTime",AttriValue,stSRULoginRspEx.iKeepAliveTime,xmlRsq);
	GET_ELEM_VALUE_NUM("KeepAliveCnt",AttriValue,stSRULoginRspEx.iKeepAliveCount,xmlRsq);
	GET_ELEM_VALUE_CHAR("TimeZone",AttriValue,stSRULoginRspEx.szTimeZone,TIME_ZONE_LEN,xmlRsq);
	GET_ELEM_VALUE_CHAR("Time",AttriValue,stSRULoginRspEx.szTime,TIME_LEN,xmlRsq);
	GET_ELEM_VALUE_CHAR("PlayType",AttriValue,stSRULoginRspEx.cPlatformVersion,PLATFORM_VERSION_LEN,xmlRsq);
	GET_ELEM_VALUE_NUM("OMUPort",AttriValue,stSRULoginRspEx.iOmuPort,xmlRsq);
	
	return IVS_SUCCEED;
}


// ������Ӧ�����ܻ�ȡLoginID����
IVS_INT32 CLoginXMLProcess::ParseLoginID(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, std::string &strLoginID)
{
    IVS_DEBUG_TRACE("");

    if (!xmlRsq.FindElemEx("Content/LoginID"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse LoginXML", "Node LoginID not found");
        return IVS_XML_INVALID;
    }
    (void)xmlRsq.IntoElem();
    const char* AttriValue = xmlRsq.GetElemValue();
    if (NULL == AttriValue)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse LoginXML", "LoginID is NUll");
        return IVS_XML_INVALID;
    }
    if (IVS_SUCCEED != oSecurityClient.DecryptString(AttriValue, strLoginID))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Parse LoginXML", "DecryptString LoginID failed");
        return IVS_FAIL;
    }
    return IVS_SUCCEED;
}

// ��ȡ�û�ע��XML
IVS_INT32 CLoginXMLProcess::GetLogoutXML(CXml& xmlReq)
{
    IVS_DEBUG_TRACE("");
    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    return IVS_SUCCEED;
}

// �û���½OMU����XML������step1Ҳʹ��,�û���½SCU����XML
IVS_INT32 CLoginXMLProcess::GetLoginOmuXML(const std::string &strLoginID, CXml& xmlReq)
{
    if (strLoginID.empty())
    {
        return IVS_FAIL;
    }
    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    (void)xmlReq.IntoElem();
    (void)xmlReq.AddChildElem("LoginID");
    (void)xmlReq.IntoElem();
    (void)xmlReq.SetElemValue(strLoginID.c_str());
    xmlReq.OutOfElem();
    return IVS_SUCCEED;
}

// �û���½�޼�ȨSMU����XML
IVS_INT32 CLoginXMLProcess::GetGeneralLoginSmuXML(const char* pLoginID, const std::string &strDomainCode, const IVS_DOMAIN_ROUTE &stDomainRouteInfo, CXml& xmlReq)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pLoginID, IVS_OPERATE_MEMORY_ERROR);
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("LoginID");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(pLoginID); // TODO ������
	(void)xmlReq.AddElem("DomainCode");
	(void)xmlReq.SetElemValue(strDomainCode.c_str());

	(void)xmlReq.AddElem("ServerIP");
	IVS_CHAR cServerIP [IVS_IP_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cServerIP, sizeof(cServerIP), stDomainRouteInfo.stIP.cIP, IVS_IP_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get General Login Smu XML failed", "strncpy failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xmlReq.SetElemValue(cServerIP);
	(void)xmlReq.AddElem("ServerPort");
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str(stDomainRouteInfo.uiPort).c_str());//lint !e713

	xmlReq.OutOfElem();
	return IVS_SUCCEED;
}

// ��ȡ��������������ӿ��õ�IP��ַ;
void CLoginXMLProcess::GetLocalIp(const char* pSerIP, int iSerPort, std::string &strLocalIP)
{
    IVS_DEBUG_TRACE("pSerIP :%s, pSerIPAddr: %p, iSerPort :%d", pSerIP, pSerIP, iSerPort);

	strLocalIP = "";
    char szLocalIp[IVS_IP_LEN] = {0};//����IP
	int iLastError;
	int iRet = -1;
	bool bRet;
	struct sockaddr_in peerAddr;	//��������ַ
	sockaddr_in stAddr;				//���ص�ַ
	char* pszLocalIp = NULL;		//����IP
	socklen_t  nAddrLen;			//���ص�ַ����
	struct timeval timeout;	//��ʱʱ��
	fd_set r;	//�ɶ��ļ���

    long lSockFd = (long)socket(AF_INET, SOCK_STREAM, 0);
    if (lSockFd < 0)
    {
		iLastError = errno;
		BP_RUN_LOG_ERR(iLastError, "socket failed", "socket :%ld. ErrorCode : %d", lSockFd, iLastError);
        return;
    }

    unsigned long iSerIP = inet_addr(pSerIP);
    if (INADDR_NONE == iSerIP)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "inet_addr failed", "server IP :%s", pSerIP);
		goto ERR_CLOSE;
    }

	//��ʼ����������ַ
    eSDK_MEMSET((char *)&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_addr.s_addr = iSerIP;
    peerAddr.sin_port = htons((u_short)iSerPort);

	//linuxϵͳ��������ʽ���ӣ�����Linuxϵͳ��������
#if VOS_APP_OS == VOS_OS_LINUX
	//����һ���ж��źŴ�����
	(void)sigset(SIGALRM, u_alarm_handler);
	//ע��һ��2��Ķ�ʱ��
	(void)alarm(2);
	iRet = connect(lSockFd, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
	(void)alarm(0);
	(void)sigrelse(SIGALRM);
	if (0 != iRet)
	{
		iLastError = errno;
		BP_RUN_LOG_ERR(iLastError, "connect failed", "socket = %ld. ErrorCode = %d.", lSockFd, iLastError);
		goto ERR_CLOSE;
	}
	
#elif VOS_APP_OS == VOS_OS_WIN32
	//windows�ѷ�������ʽ��������
	unsigned long ul = 1;
	if(SOCKET_ERROR == ioctlsocket((SOCKET)lSockFd, (long)FIONBIO, (unsigned long*)&ul))
	{
		iLastError = SOCKET_ERROR;
		BP_RUN_LOG_ERR(iLastError, "ioctlsocket failed", "ErrorCode = %d.", iLastError);
		goto ERR_CLOSE;  
	}

	(void)::connect((unsigned int)lSockFd, (struct sockaddr*)&peerAddr, sizeof(peerAddr));

	//��ʼ����ʱʱ��
	timeout.tv_sec = CONNET_TIME_OUT;   // ���ӳ�ʱ8��
	timeout.tv_usec =0;

	//���ÿɶ��ļ���
	FD_ZERO(&r);
	FD_SET((unsigned int)lSockFd, &r);
	
	//select�ĵ�һ��������0��ΪlSockFd + 1��WIN32�»���Ըò�������LINUX����
	iRet = select(lSockFd + 1, 0, &r, 0, &timeout);
	if(iRet <= 0)   
	{
		iLastError = errno;
		BP_RUN_LOG_ERR(iLastError, "ioctlsocket failed", "ErrorCode = %d.", iLastError);
		goto ERR_CLOSE;
	}
#endif

	//���»�ȡ������ַ
	eSDK_MEMSET(&stAddr, 0, sizeof(stAddr));
	nAddrLen =  sizeof(stAddr);
	iRet = getsockname((int)lSockFd, (struct sockaddr *)&stAddr, &nAddrLen);	//lint !e732

	//�ر�socket
#if VOS_APP_OS == VOS_OS_WIN32
	(void)::closesocket((SOCKET)lSockFd);
#elif VOS_APP_OS == VOS_OS_LINUX
	(void)close(lSockFd);
#endif

	if (0 != iRet)
	{
		iLastError = errno;
		BP_RUN_LOG_ERR(iLastError, "getsockname failed", "socket :%ld. ErrorCode : %d", lSockFd, iLastError);
		return;
	}

	pszLocalIp = inet_ntoa(stAddr.sin_addr);
	if (NULL == pszLocalIp)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "inet_ntoa failed", "pszLocalIp is NULL");
		return;
	}

	//���沢����
	bRet = CToolsHelp::Strncpy(szLocalIp, sizeof(szLocalIp), pszLocalIp, sizeof(szLocalIp)-1);
	if (false == bRet)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Strncpy failed", "NA");
		return;
	}
	strLocalIP = szLocalIp;

	return;

	//�쳣����¹ر�socket
ERR_CLOSE:
#if VOS_APP_OS == VOS_OS_WIN32
	(void)::closesocket((SOCKET)lSockFd);
#elif VOS_APP_OS == VOS_OS_LINUX
	(void)close(lSockFd);
#endif

	return;

}
