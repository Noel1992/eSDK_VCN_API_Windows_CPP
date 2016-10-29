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
filename : UserGroupMgr.h
author : pkf57481
created : 2012/12/03
description : �û������
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 ��ʼ�汾
*********************************************************************/


#ifndef __USERGROUP_MGR_H_
#define __USERGROUP_MGR_H_

#include "hwsdk.h"
#include "SDKDef.h"

class CUserMgr;
class CUserGroupMgr
{
public:
    CUserGroupMgr(void);
    ~CUserGroupMgr(void);

    void SetUserMgr(CUserMgr* pUserMgr);

public:
    // �����û���
    IVS_INT32 AddUserGroup(const IVS_USER_GROUP* pUserGroup, IVS_UINT32* puiUserGroupID);

    // ɾ���û���
    IVS_INT32 DeleteUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID);

    // �޸��û���
    IVS_INT32 ModifyUserGroup(const IVS_USER_GROUP* pUserGroup);

    // �ƶ��û���
    IVS_INT32 MoveUserGroup(const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetGroupID, const IVS_USER_GROUP_LIST* pUserGroupList);

    // ��ѯ�û����б�
    IVS_INT32 GetUserGroupList(const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList, IVS_UINT32 /*uiBufferSize*/);

private:
    CUserMgr* m_pUserMgr;
};

#endif
