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
 filename		:	IAMessageMgr.h
 author			:	zwx211831
 created			:	2015/1/13
 description	:	IA消息管理类，管理轨迹消息与Panel之间的对应关系;
 copyright		:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history			:	2015/1/13 初始版本
*****************************************************************/

#ifndef __IA_MESSAGE_MGR_H__
#define __IA_MESSAGE_MGR_H__

#include "vos.h"
#include "SDKDef.h"
#include "vos_atomic.h"
#include "IVS_IATrackAPI.h"
#include "IAOverLayMgr.h"
#include "IAObject.h"

class CIAObject;

//IA message map, key is requestID, value is the pointer of CIAObject obj
typedef std::map<IVS_ULONG, CIAObject*>	IAMESSAGE_MAP;
typedef IAMESSAGE_MAP::iterator			IAMESSAGE_MAP_ITER;

/****************************************************************************/
/*								CIAMessageMgr																			*/
/*								管理轨迹消息与Panel之间的对应关系										*/
/****************************************************************************/

class CIAMessageMgr
{
public:
	static CIAMessageMgr &Instance();
	~CIAMessageMgr();

	//start overlay. If successed, return questID, else, return IA_INVALID_REQUEST_ID.
	IVS_ULONG StartOverlay(CIAOverLayMgr *pVideoPane);

	//process the callback message of IA overlay
	IVS_INT32 IAPushAlarmLocus(const IVS_CHAR *pIAMessage);

	//stop overlay
	IVS_INT32 StopOverlay(const CIAOverLayMgr *pVideoPane, IVS_ULONG &ulRequestID);

	//try share stream
	IVS_ULONG TryRealStreamShare(CIAOverLayMgr *pVideoPane, const IA_REQ_PARAM &IAReqParam);

	//alloc RequestID
	IVS_ULONG AllocRequestID();

	//start realtime overlay. If successed, return requestID. If share stream, don't send request message again.
	IVS_ULONG StartRealtimeOverlay(CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam);

	//send request message
	IVS_INT32 SendenableMessage(IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID)const;

	//set play delay time
	IVS_INT32 SetPlayDelayTime(const CIAOverLayMgr *pVideoPane) const;

	//Insert the new IAObject into map
	IVS_INT32 InsertIAObject(CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam);

	// 插入轨迹信息
	IVS_INT32 InsertTrackInfo(const IA_REQ_PARAM &IAReqParameters, const IA_LAYOUT_MESSAGE &IALayoutMessage);

private:
	CIAMessageMgr();

	//get request param.
	IVS_INT32 GetReqParam(const CIAOverLayMgr *pVideoPane, IA_REQ_PARAM &IAReqParam)const;

	//send stop request message
	IVS_INT32 SendMessageStopXML(const CIAOverLayMgr *pVideoPane, IVS_ULONG ulRequestID, IVS_CHAR *pTransID)const;


	IVS_VOID DeletePanelInMap(IVS_ULONG ulRequestID, const CIAOverLayMgr *pVideoPane);

private:
	bool m_bInited;
	IVS_ULONG m_ulRequestID;
	IVS_INT32 m_uiPUIAVideoDelayValue;

	IA_REQ_PARAM m_IAReqParam;
	IAMESSAGE_MAP m_IAMessageMap;

	VOS_Mutex *m_MutexMap;

public:
	IVS_VOID SetPUIAVideoDelayValue(const IVS_INT32 val) {m_uiPUIAVideoDelayValue = val;}
	IVS_INT32 GetPUIAVideoDelayValue() const {return m_uiPUIAVideoDelayValue;}
};

#endif
