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
filename : LoginXMLProcess.h
author : x00192614
created : 2012/12/03
description : ��¼xmlƴװ����
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 ��ʼ�汾
*********************************************************************/

#ifndef __LOGIN_XML_PROCESS_H__
#define __LOGIN_XML_PROCESS_H__

#include "SDKDef.h"

class CUserMgr;
class CUserInfo;
class CSDKSecurityClient;
class CLoginXMLProcess
{
private:
    CLoginXMLProcess();
public:
    ~CLoginXMLProcess();

public:
    /******************************************************************
     function   : GetLoginXML
     description: ��ȡ��¼����XML
     input      : const IVS_LOGIN_INFO * pLoginReqInfo ��������ṹ��
     input      : const std::string & strLoginName �û���
     input      : const std::string & strLoginPWD ����
     input      : CUserMgr * pUserObj �û�ָ��
     output      : CXml & xmlReq ����xml��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginXML(const IVS_LOGIN_INFO* pLoginReqInfo, const std::string& strLoginName, const std::string& strLoginPWD, CUserMgr *pUserObj, CXml& xmlReq );

	/******************************************************************
     function   : GetLoginByTicketXML
     description: ��ȡ�����¼����XML
     input      : const IVS_LOGIN_INFO_EX * pLoginReqInfoEx ��������ṹ��
     input      : CUserMgr * pUserObj �û�ָ��
     output     : CXml & xmlReq ����xml��;
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginByTicketXML(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, CUserMgr *pUserObj, CXml& xmlReq );

    /******************************************************************
     function   : GetReLoginXML
     description: ��ȡ�ص�½XML
     input      : const std::string & strLoginName �û���
     input      : const std::string & strLoginPWD ����
     input      : CUserInfo * pUserInfo �û���Ϣָ��
     output      : CXml & xmlReq ����XML
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetReLoginXML(const std::string& strLoginName, const std::string& strLoginPWD, CUserInfo *pUserInfo, CXml& reqXml );
	
	/******************************************************************
	 function   : GetFineMainDevXML
	 description: ��ȡ�������λXML
	 input      : const string & strDevCode ���������
	 output      : CXml & xmlReq ����xml��
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetFineMainDevXML(const std::string& strDevCode, CXml& xmlReq );

	/******************************************************************
	 function   : ParseNvrCodeXML
	 description: ����NvrCode��Ϣ
	 input      : CXml & xmlRsq ��Ӧxml��
	 output      : char * pNvrCode NVR����
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 ParseNvrCodeXML(CXml& xmlRsq, char* pNvrCode);

    /******************************************************************
     function   : ParseLoginXML
     description: ������¼��ӦXML
     input      : CXml & xmlRsq ��¼��Ӧxml��
     input      : const CSDKSecurityClient & oSecurityClient ��Կ������
     output      : ST_SRU_LOGIN_RSP & stSRULoginRsp ������Ӧ��Ϣ��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRsp);


	/******************************************************************
     function   : ParseLoginByTicketXML
     description: ���������¼��ӦXML
     input      : CXml & xmlRsq ��¼��Ӧxml��;
	 input      : const CSDKSecurityClient & oSecurityClient ��Կ������;
     output     : ST_SRU_LOGIN_RSP & stSRULoginRspEx ������Ӧ��Ϣ��;
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginByTicketXML(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, ST_SRU_LOGIN_RSP &stSRULoginRspEx);


    /******************************************************************
     function   : ParseLoginID
     description: �����ص�¼��Ӧ����ȡLoginID
     input      : CXml & xmlRsq
     input      : CSDKSecurityClient & oSecurityClient
     input      : std::string & strLoginID
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseLoginID(CXml& xmlRsq, const CSDKSecurityClient& oSecurityClient, std::string &strLoginID);

    /******************************************************************
     function   : GetLoginOmuXML
     description: �û���½OMU����XML
     input      : const string & pLoginID ��¼����SMU�ɹ�����
     output     : CXml & xmlReq ����xml��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginOmuXML(const std::string &pLoginID, CXml& xmlReq);

    /******************************************************************
     function   : GetLogoutXML
     description: �û�ע��XML
     input      : CXml & xmlReq ����xml
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLogoutXML(CXml& xmlReq);

    /******************************************************************
     function   : GetGeneralLoginSmuXML
     description: �û���½�޼�ȨSMU����XML
     input      : const char * pLoginID ��¼�ɹ����صı�ʶ
     input      : const std::string & strDomainCode �����
     output      : CXml & xmlReq ����xml��
     return     : IVS_INT32
	*******************************************************************/
    static IVS_INT32 GetGeneralLoginSmuXML(const char* pLoginID, const std::string &strDomainCode, const IVS_DOMAIN_ROUTE &stDomainRouteInfo, CXml& xmlReq);
   
    /******************************************************************
     function   : ParseUserOffLineXML
     description: �û�����֪ͨ
     input      : CXml & xmlRsq ����֪ͨxml
     input      : IVS_CHAR * pLoginID һ�ε�¼�ı�ʶ
     output      : IVS_USER_OFFLINE_INFO * stOffLineInfo ����֪ͨ�ṹ��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ParseUserOffLineXML(CXml& xmlRsq, const IVS_CHAR* /*pLoginID*/, IVS_USER_OFFLINE_INFO *stOffLineInfo);

private:

	/******************************************************************
	 function   : GetLocalIp
	 description: ��ȡ����IP��ַ
	 input      : const char * pSerIP �����IP
	 input      : int iSerPort �����PORT
	 output     : NA
	 return     : string ����IP
	*******************************************************************/
	static void GetLocalIp(const char* pSerIP, int iSerPort, std::string &strLocalIP);
};
#endif

