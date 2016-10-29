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
	filename	: 	CSDKMainctrl.h
	author		:	z00193167
	created		:	2012/11/01	
	description	:	BP主线程
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/01 初始版本
*********************************************************************/

#ifndef SDK_MAIN_CTRL_H
#define SDK_MAIN_CTRL_H

#include "MainCtrl.h"
#include "Cmd.h"
#include "SDKDef.h"

USING_NAMESPACE_BP

class CSDKMainctrl : public CMainCtrl
{
public:
	CSDKMainctrl();
	~CSDKMainctrl();

	virtual int HandleCmd(CCmd* pCmd);

	virtual uint32_t Routing(void); //lint !e601 !e10 //定期检查;  

	virtual int Init();

	//virtual void UnInit();
protected:
	virtual void OnLinkOpened(const std::string& strDevName, int iNetElementType, const std::string& strLinkID, const std::string& strIP, int iPort);

	virtual void OnLinkClosed(const std::string& strDevName, int iNetElementType, const std::string& strLinkID, const std::string& strIP, int iPort);

private:
	// 处理告警上报消息
	int OnAlarmNotify(CCmd* pCmd)const;
	// 处理智能分析
	int HandleMAUCmd(CCmd* pCmd) const;
	// 处理告警状态
	int OnAlarmStatusNotify(CCmd* pCmd)const;
	// 用户离线通知
    int OnUserOffLineNotify(CCmd* pCmd)const;
	// 处理联动动作通知
	int DoLinkageAction(CCmd* pCmd)const;
    // 处理联动动作通知
	int StopLinkageAction(CCmd* pCmd)const;
	//对XML中的设备编码拼装与编码
	int ModifyDevCode(CXml& xml, IVS_CHAR* pData)const;
	// 推送发现到的前端设备
	int OnDisCoveredDevNotify(CCmd* pCmd)const;
	// 设备告警上报OMU
	int OnDevAlarmNotify(CCmd* pCmd)const;
	// 处理手动录像状态
	int OnManualRecordStateNotify(CCmd* pCmd)const;
	// 模式轨迹录制结束通知
	int OnModeCruiseRecordOver(CCmd* pCmd)const;
	// 子设备（设备树）消息异步响应处理
    int HandleGetUserChannelListRsp(CCmd* pCmd)const;
	// add by zwx211831, Date:20140605 处理客户端关闭视频业务通知
	int OnShutdownStreamNotify(CCmd* pCmd)const;

private:
	typedef int (__thiscall CSDKMainctrl::*FnProcessCmd)(CCmd* pCmd)const; 

	typedef struct
	{
		uint32_t reqID;
		FnProcessCmd fnProcessCmd;
	} REQUEST_NAME;
	static REQUEST_NAME m_reqNames[];
	FnProcessCmd GetFnProcessCmd(uint32_t enReqID) const;
};

#endif //SDK_MAIN_CTRL_H
