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

/*****************************************************************
 filename    :    OCXUpgradeMgr.h
 author      :    wanglei 00165153
 created     :    2012/11/19
 description :    实现设备组;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/11/13 初始版本
*****************************************************************/
#ifndef __OCX_UPGRADE_MGR_H__
#define __OCX_UPGRADE_MGR_H__

#include <afx.h>
#include "SDKDef.h"

class COCXUpgradeMgrMgr
{
private:
     COCXUpgradeMgrMgr();
    ~COCXUpgradeMgrMgr();
public:
    // 新增设备组
    static IVS_INT32 GetCUVersion(IVS_INT32 iSessionID, CXml& xmlReq, CString& strResult);
};

#endif
