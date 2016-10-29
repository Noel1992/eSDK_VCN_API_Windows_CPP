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
	filename	: 	UpgradeMgr.h
	author		:	w00165153
	created		:	2013/11/13
	description	:	升级管理类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/11/13 初始版本;
*********************************************************************/
#ifndef __UPGRADEMGR_H__
#define __UPGRADEMGR_H__
#include "hwsdk.h"
#include "ivs_error.h"


class CUserMgr;
class CUpgradeMgr
{
public:
    CUpgradeMgr(void);
    ~CUpgradeMgr(void);
public:
    // 设置用户对象类
    void SetUserMgr(CUserMgr* pUserMgr);
    IVS_INT32 GetCUVersion(const IVS_CU_INFO* pCUInfo, IVS_UPGRADE_INFO* pUpgradeInfo);
private:
    CUserMgr *m_pUserMgr; //lint !e830 //不需要//
};

#endif
