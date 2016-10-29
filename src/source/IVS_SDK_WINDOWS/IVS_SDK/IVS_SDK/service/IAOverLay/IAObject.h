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
 filename		:	IAObject.h
 author			:	zwx211831
 created			:	2015/7/14
 description	:	管理与RequestID对应的Panel列表及轨迹请求参数;
 copyright		:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history			:	2015/7/14初始版本
*****************************************************************/

#ifndef __IA_OBJECT_H__
#define __IA_OBJECT_H__

#include "SDKDef.h"
#include "IAMessageMgr.h"
#include "IAOverLayMgr.h"

typedef std::list<CIAOverLayMgr*> PANEL_LIST;
typedef PANEL_LIST::iterator   PANEL_ITER;

class CIAMessageMgr;

class CIAObject
{
	friend CIAMessageMgr;

public:
	CIAObject(IVS_ULONG ulRuleType, IVS_ULONG ulQueryType);
	~CIAObject();

	IVS_INT32 InsertPanel(CIAOverLayMgr *pPanel);
	IVS_INT32 DeletePanel(const CIAOverLayMgr *pPanel);
	IVS_INT32 SetCameraID(const IVS_CHAR *szCamID);
	IVS_INT32 SetNVRCode(const IVS_CHAR *szNVRCode);
	IVS_INT32 SetStartTime(const IVS_CHAR *szStartTime);
	bool CheckPanelListNull()const { return m_VideoPaneList.empty(); }
	IVS_UINT32 GetPanelNum() const {return m_VideoPaneList.size();};

private:
	CIAObject();

private:
	IVS_ULONG m_ulRuleType;
	IVS_ULONG	m_ulQueryType;
	IVS_CHAR	m_szCameraID[IVS_DEV_CODE_LEN + 1];
	IVS_CHAR m_szNVRCode[IVS_NVR_CODE_LEN + 1];
	IVS_CHAR m_szStartTime[IVS_TIME_LEN + 1];
	PANEL_LIST	m_VideoPaneList;
};

#endif

