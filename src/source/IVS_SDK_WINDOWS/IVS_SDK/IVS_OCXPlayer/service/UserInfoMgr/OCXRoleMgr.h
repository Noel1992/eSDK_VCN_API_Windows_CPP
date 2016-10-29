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
filename : OCXRoleMgr.h
author : s00191067
created : 2013/01/14
description : ��ɫ������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 ��ʼ�汾
*********************************************************************/


#ifndef __OCX_ROLE_MGR_H_
#define __OCX_ROLE_MGR_H_

#include "hwsdk.h"
#include "IVSCommon.h"

class COCXRoleMgr
{
public:
    COCXRoleMgr(void);
    ~COCXRoleMgr(void);

public:
    // ���ӽ�ɫ
    static CString AddRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo);

    // ɾ����ɫ
    static LONG DeleteRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

    // �޸Ľ�ɫ
    static LONG ModifyRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo);

    // ��ѯ��ɫ�б�
    static CString GetRoleList(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

    // ���ݽ�ɫID��ȡ��ɫ��ϸ��Ϣ
    static CString GetRoleInfoByRoleID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

    // �����û�ID��ȡ��ɫ��ϸ��Ϣ
    static CString GetRoleInfoByUserID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // ��ȡĬ���˺Ź���
    static CString GetDefaultRoleAccountRule(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

    // ��ȡ�û�Ȩ����Ϣ
    static CString GetOperationRightGroup(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

};

#endif
