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
 filename    :    OCXPtzControlXMLProcess.h
 author      :    z00193167
 created     :    2013/1/22
 description :    ʵ����̨����XML��ƴװ�ͽ���;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/22 ��ʼ�汾
*****************************************************************/
#ifndef __OCX_PTZ_CONTROL_PROCESS_H__
#define __OCX_PTZ_CONTROL_PROCESS_H__
#include "stdafx.h"
#include "SDKDef.h"

class COCXPtzControlXMLProcess
{
public:
    ~COCXPtzControlXMLProcess();
	// ����Ԥ��λƴװxml
	static IVS_INT32 AddPTZPresetGetXML(std::string& strCameraCode, IVS_UINT32 uiPresetIndex, IVS_INT32 iRet, CString& strResult);
	// ��ѯԤ��λ�б�ƴװxml
	static IVS_INT32 GetPTZPresetListGetXML(std::string& strCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiPTZPresetNum, IVS_INT32 iRet, CString& strResult);
	// ���ÿ���λ����xml
	static IVS_INT32 SetGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);
	// ɾ������λ����xml
	static IVS_INT32 DelGuardPosParseXML(std::string& strGuardPosInfo, std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo);
    // ��ȡ����λƴװxml
	static IVS_INT32 GetGuardPosGetXML(std::string& strCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo, IVS_INT32 iRet,CString& strResult);
	// ����Ѳ���켣����xml
	static IVS_INT32 AddCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo);
	// ����Ѳ���켣ƴװxml
	static IVS_INT32 AddCruiseTrackGetXML(IVS_UINT32 uiTrackIndex, IVS_INT32 iRet, CString& strResult);
	// �޸�Ѳ���켣����xml
	static IVS_INT32 ModCruiseTrackParseXML(std::string& strCuriseTrackInfo, std::string& strCameraCode, IVS_CRUISE_TRACK* pCuriseTrackInfo);
	// ��ѯѲ���켣ƴװxml
	static IVS_INT32 GetCruiseTrackGetXML(std::string& strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackInfo, IVS_INT32 iRet, CString& strResult);
	// ��ѯѲ���켣��Ϣ�б�ƴװxml
	static IVS_INT32 GetCruiseTrackListGetXML(std::string&strCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiCruiseTrackNum, IVS_INT32 iRet, CString& strResult);
	// ����Ѳ���ƻ�����xml
	static IVS_INT32 SetCruisePlanParseXML(std::string& strCruisePlan, std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan);
	// ��ȡѲ���ƻ�ƴװxml
	static IVS_INT32 GetCruisePlanGetXML(std::string& strCameraCode, IVS_CRUISE_PLAN* pCruisePlan, IVS_INT32 iRet, CString& strResult);
    // PTZ����ƴװxml
    static IVS_INT32 GetPTZControlRspXML(IVS_PTZ_CONTROL_INFO& stPtzControlInfo, IVS_INT32 iRet, CString& strResult);

private:
    COCXPtzControlXMLProcess();
    
};

#endif // __OCX_PTZ_CONTROL_PROCESS_H__
