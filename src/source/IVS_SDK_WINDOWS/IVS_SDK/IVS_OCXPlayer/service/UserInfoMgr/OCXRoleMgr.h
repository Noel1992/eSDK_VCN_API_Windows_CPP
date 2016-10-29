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
description : 角色管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
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
    // 增加角色
    static CString AddRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo);

    // 删除角色
    static LONG DeleteRole(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

    // 修改角色
    static LONG ModifyRole(IVS_INT32 iSessionID, LPCTSTR pRoleInfo);

    // 查询角色列表
    static CString GetRoleList(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

    // 根据角色ID获取角色详细信息
    static CString GetRoleInfoByRoleID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulRoleID);

    // 根据用户ID获取角色详细信息
    static CString GetRoleInfoByUserID(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserID);

    // 获取默认账号规则
    static CString GetDefaultRoleAccountRule(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

    // 获取用户权限信息
    static CString GetOperationRightGroup(IVS_INT32 iSessionID, LPCTSTR pDomainCode);

};

#endif
