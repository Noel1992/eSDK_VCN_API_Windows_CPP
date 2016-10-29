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

#include "IAObject.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"
#include "IAOverLayMgr.h"

/****************************************************************************/
/*								CIAObject																						*/
/*								与RequestID对应的Panel列表及轨迹请求参数						*/
/****************************************************************************/
CIAObject::CIAObject(IVS_ULONG	ulRuleType, IVS_ULONG ulQueryType)
	:m_ulRuleType(ulRuleType)
	,m_ulQueryType(ulQueryType)
{
	eSDK_MEMSET(m_szCameraID, 0x0, sizeof(m_szCameraID));
	eSDK_MEMSET(m_szNVRCode, 0x0, sizeof(m_szNVRCode));
	eSDK_MEMSET(m_szStartTime, 0x0, sizeof(m_szStartTime));
}

CIAObject::~CIAObject()
{
	eSDK_MEMSET(m_szCameraID, 0x0, sizeof(m_szCameraID));
	eSDK_MEMSET(m_szNVRCode, 0x0, sizeof(m_szNVRCode));
	eSDK_MEMSET(m_szStartTime, 0x0, sizeof(m_szStartTime));
}

IVS_INT32 CIAObject::InsertPanel(CIAOverLayMgr *pPanel)
{
	CHECK_POINTER(pPanel, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, PlayHandle: %lu", pPanel->GetCameraID(), pPanel->GetNVRCode(), pPanel->GetPlayHandle());

	PANEL_ITER iterPanel = m_VideoPaneList.begin();
	PANEL_ITER iterPanelEnd = m_VideoPaneList.end();
	while (iterPanel != iterPanelEnd)
	{
		CIAOverLayMgr* pVideoPanel = *iterPanel++;
		if (pVideoPanel == pPanel)
		{
			return IVS_SUCCEED;
		}
	}
	m_VideoPaneList.push_back(pPanel);
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::DeletePanel(const CIAOverLayMgr *pPanel)
{
	CHECK_POINTER(pPanel, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraCode: %s, NVRCode: %s, PlayHandle: %lu", pPanel->GetCameraID(), pPanel->GetNVRCode(), pPanel->GetPlayHandle());

	PANEL_ITER iterPanel = m_VideoPaneList.begin();
	PANEL_ITER iterPanelEnd = m_VideoPaneList.end();
	while (iterPanel != iterPanelEnd)
	{
		CIAOverLayMgr* pVideoPanel = *iterPanel;
		if (pVideoPanel == pPanel)
		{
			// Panel不能释放
			m_VideoPaneList.erase(iterPanel);
			break;
		}
		iterPanel++;
	}
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::SetCameraID(const IVS_CHAR *szCamID)
{
	CHECK_POINTER(szCamID, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("CameraCode: %s", szCamID);

	CHECK_SUCCESS(CToolsHelp::Strncpy(m_szCameraID, sizeof(m_szCameraID), szCamID, IVS_DEV_CODE_LEN));
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::SetNVRCode(const IVS_CHAR *szNVRCode)
{
	CHECK_POINTER(szNVRCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("NVRCode: %s", szNVRCode);

	CHECK_SUCCESS(CToolsHelp::Strncpy(m_szNVRCode, sizeof(m_szNVRCode), szNVRCode, IVS_NVR_CODE_LEN));
	return IVS_SUCCEED;
}

IVS_INT32 CIAObject::SetStartTime(const IVS_CHAR *szStartTime)
{
	CHECK_POINTER(szStartTime, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("StartTime: %s", szStartTime);

	CHECK_SUCCESS(CToolsHelp::Strncpy(m_szStartTime, sizeof(m_szStartTime), szStartTime, IVS_TIME_LEN));
	return IVS_SUCCEED;
}

