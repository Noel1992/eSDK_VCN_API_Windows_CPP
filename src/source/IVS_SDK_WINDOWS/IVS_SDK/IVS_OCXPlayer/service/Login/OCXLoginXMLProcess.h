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
filename    :    OCXLoginXMLProcess.h
author      :    x00192614
created     :    2012/12/3
description :    ivs �����붨��
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/12/3 ��ʼ�汾
*********************************************************************/
#ifndef __OCX_LOGIN_XML_PROCESS_H__
#define __OCX_LOGIN_XML_PROCESS_H__
#include "OCXXmlProcess.h"

class COCXLoginXMLProcess
{
private:
    COCXLoginXMLProcess();
public:
    ~COCXLoginXMLProcess();

    /******************************************************************
     function   : GetLoginReqInfo
     description: ���ݵ�½����XML����ȡ��½��Ϣ�ṹ��
     input      : LPCTSTR pUserName             �û���
     input      : LPCTSTR pPassward             ����
     input      : LPCTSTR pServerIP             �����IP
     input      : LPCTSTR pPort                 �����PORT
     input      : LONG uiClientType             �ͻ�������
     output      : IVS_LOGIN_INFO & pLoginInfo   ��νṹ��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginReqInfo(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP,LPCTSTR pPort, ULONG ulClientType, IVS_LOGIN_INFO &pLoginInfo);


	/******************************************************************
     function   : GetLoginByTicketReqInfo
     description: ���ݵ����½����XML����ȡ�����½��Ϣ�ṹ��
     input      : LPCTSTR pTicketID             TicketID
     input      : LPCTSTR pServerIP             �����IP
     input      : LPCTSTR pPort                 �����PORT
     input      : LONG uiClientType             �ͻ�������
     output      : IVS_LOGIN_INFO_EX & pLoginInfoEx   ��νṹ��
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetLoginByTicketReqInfo(LPCTSTR pTicketID, 
		LPCTSTR pServerIP,
		LPCTSTR pPort, 
		ULONG ulClientType, 
		IVS_LOGIN_INFO_EX &pLoginInfoEx);


    /******************************************************************
     function   : UserOfflineNotifyGetXML
     description: �û�����֪ͨ����xml
     input      : const IVS_USER_OFFLINE_INFO * pOfflineNotify ����֪ͨ�ṹ��
     output      : CXml & xml ����֪ͨxml
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 UserOfflineNotifyGetXML(const IVS_USER_OFFLINE_INFO* pOfflineNotify, CXml &xml);    
    
    /******************************************************************
     function   : GetUserIDRspXML
     description: ƴװGetUserID��Ӧ��Ϣ
     input      : IVS_INT32 iRet ��ȡUserID������
     input      : IVS_UINT32 uiUserID �û�ID
     output      : CXml & xml ��Ӧxml
     return     : void
    *******************************************************************/
    static void GetUserIDRspXML(IVS_INT32 iRet, IVS_UINT32 uiUserID, CXml &xml);
};

#endif // __OCX_LOGIN_XML_PROCESS_H__