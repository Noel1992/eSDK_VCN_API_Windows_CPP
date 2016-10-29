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
filename : OCXUserGroupMgr.h
author : s00191067
created : 2013/01/14
description : 用户组管理类
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2013/01/14 初始版本
*********************************************************************/


#ifndef __OCX_USERGROUP_MGR_H_
#define __OCX_USERGROUP_MGR_H_

#include "hwsdk.h"

class COCXUserGroupMgr
{
public:
    COCXUserGroupMgr(void);
    ~COCXUserGroupMgr(void);

public:
    // 增加用户组
    static CString AddUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 删除用户组
    static LONG DeleteUserGroup(IVS_INT32 iSessionID, LPCTSTR pDomainCode, ULONG ulUserGroupID);

    // 修改用户组
    static LONG ModifyUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 移动用户组
    static LONG MoveUserGroup(IVS_INT32 iSessionID, LPCTSTR pUserGroup);

    // 查询用户组列表
    static CString GetUserGroupList(IVS_INT32 iSessionID, LPCTSTR pReqXml);

};

#endif
