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
filename	: 	AlarmMgr.h
author		:	pkf57481
created		:	2012/12/7
description	:	�澯����;
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/7 ��ʼ�汾;
*********************************************************************/

#ifndef __ALARMLINKAGE_MGR_H__
#define __ALARMLINKAGE_MGR_H__

#include "hwsdk.h"

class CUserMgr;
class CAlarmLinkageMgr
{
public:
	CAlarmLinkageMgr();
	~CAlarmLinkageMgr();

	void SetUserMgr(CUserMgr *pUserMgr);
public:
	// �����澯��������
	IVS_INT32 AddAlarmLinkage(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	// �޸ĸ澯��������
	IVS_INT32 ModifyAlarmLinkage(const IVS_CHAR* pReqXml);

	// ɾ���澯��������
	IVS_INT32 DeleteAlarmLinkage(const IVS_CHAR* pReqXml);

	// �鿴�澯���������б�
	IVS_INT32 GetAlarmlinkageList(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);

	// ��ѯ�澯��������
	IVS_INT32 GetAlarmLinkage(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml);

	// ��ѯ�澯������������
	IVS_INT32 GetAlarmLinkageAction(const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml);
private:
	CUserMgr *m_pUserMgr;
};

#endif //__ALARMLINKAGE_MGR_H__
