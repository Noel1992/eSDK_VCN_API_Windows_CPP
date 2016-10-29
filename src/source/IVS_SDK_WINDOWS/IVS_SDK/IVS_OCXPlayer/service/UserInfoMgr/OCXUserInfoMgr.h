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
filename : OCXUserInfoMgr.h
author : s00191067
created : 2013/01/14
description : �û�������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 ��ʼ�汾
*********************************************************************/


#ifndef __OCX_USERINFO_MGR_H_
#define __OCX_USERINFO_MGR_H_

#include "IVSCommon.h"
#include "hwsdk.h"

class COCXUserInfoMgr
{
public:
    COCXUserInfoMgr(void);
    ~COCXUserInfoMgr(void);

public:
    // ����û���Ϣ
    static CString AddUser(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ɾ���û�
    static LONG DeleteUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // �����û���Ϣ
    static LONG ModifyUser(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    // ��ѯ�û���Ϣ
    static CString GetUserInfo(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // ����/�����û�
    static LONG LockUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, ULONG ulLock);

    // ��ѯ�û��б�
    static CString GetUserList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // ��ѯ�����û��б�
    static CString GetOnlineUser(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // �޸�����
    static LONG ChangePWD(IVS_INT32 iSessionID, LPCTSTR pOldPWD, LPCTSTR pNewPWD);

    // ��������
    static LONG ResetPWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pNewPWD);

    // У������������Ƿ�����˺Ź���
    static LONG CheckPWDByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName, LPCTSTR pPWD);

    // ���ݽ�ɫ��ȡ�����˺Ź��������
    static CString GetAutoCreatePWD(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName);

    // ��ȡ����ǿ��
    static CString GetPWDStrength(LPCTSTR pPWD);

    // ��ȡ���������Ϣ
    static CString GetPWDExpireInfo(IVS_INT32 iSessionID);

    // �����û��豸�б�
    static LONG SetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pReqXml);

    //��ȡ�û��豸�б�
    static CString GetUserDeviceList(IVS_INT32 iSessionID, LPCTSTR pUnifiedQuery);

    // ǩ�������û��Ự
    static LONG SignOutOnlineUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserOnlineID);

    // �����û�IDǩ�������û��Ự
    static LONG SignOutUser(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // ���ݽ�ɫIDǩ�������û��Ự
    static LONG SignOutUserByRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

};

#endif
