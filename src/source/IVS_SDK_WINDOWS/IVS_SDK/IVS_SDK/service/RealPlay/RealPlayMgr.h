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
 filename    :    RealPlayMgr.h
 author      :    s00191067
 created     :    2012/11/19
 description :    实现所有实况业务;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*********************************************************************/
#ifndef __REALPLAY_MGR_H__
#define __REALPLAY_MGR_H__

#include "RealPlay.h"
#include "SDKDef.h"
#include "Lock.h"
#include "IAOverLayMgr.h"

class CRealPlayMgr
{
public:
	CRealPlayMgr(void);
	~CRealPlayMgr(void);

public:
	void SetUserMgr(CUserMgr* pUserMgr);

 
	/******************************************************************
	function : GetMediaBase
	description: 根据句柄获取realplay;
	input : ulHandle，句柄
	output : NA
	return : MediaBase对象
	*******************************************************************/
    CMediaBase *GetMediaBase(IVS_ULONG ulHandle);

    /******************************************************************
	function : StartRealPlay
	description: 开始实况;
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；hWnd，窗体句柄；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle);

	/******************************************************************
	function : StartRealPlayCBRaw
	description: 开始实况(裸码流);
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；fRealPlayCallBackRaw，裸码流回调；pUserData，用户信息；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData, IVS_ULONG* pHandle );

	/******************************************************************
	function : StartRealPlayCBFrame
	description: 开始实况(YUV流);
	input : pRealplayPara,实况参数信息；pCameraCode，摄像机编号；fRealPlayCallBackFrame，YUV码流回调；pUserData，用户信息；pHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle );

	/******************************************************************
	function : StopRealPlay
	description: 停止实况;
	input : ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 StopRealPlay(IVS_ULONG ulHandle);

	/******************************************************************
	function : AddPlayWindow
	description: 添加播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd);
	/******************************************************************
	function : DelPlayWindow
	description: 删除播放窗口;
	input : hWnd，窗体句柄；ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd);

	IVS_INT32 AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);
	IVS_INT32 UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);
	/******************************************************************
	function : GetPlayerChannelByHandle
	description: 获取通道号;
	input : ulHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
	IVS_UINT32 GetPlayerChannelByHandle(IVS_ULONG ulHandle);

	/******************************************************************
	function : GetCameraCodeByHandle
	description: 根据句柄获取CameraCode;
	input : strCameraCode，摄像机编号；ulHandle，句柄
	output : NA
	return : 无
	*******************************************************************/
	void GetCameraCodeByHandle(IVS_ULONG ulHandle, std::string& strCameraCode);

	/******************************************************************
	function : StopAllRealPlay
	description: 关闭所有实况;
	input : 无
	output : NA
	return : 无
	*******************************************************************/
	void StopAllRealPlay();

	/******************************************************************
	function : GetPlayHandlebyRtspHandle
	description: 获取实况句柄;
	input : iRtspHandle，句柄
	output : NA
	return : 实况句柄;
	*******************************************************************/
	IVS_ULONG GetPlayHandlebyRtspHandle(IVS_ULONG iRtspHandle);

	IVS_ULONG GetPlayHandlebyRtspHandleEx(IVS_ULONG iRtspHandle);

	/******************************************************************
	function : GetPlayHandleByNetSourceChannel
	description: 获取实况句柄;
	input : ulNetSourceChannel，句柄
	output : NA
	return : 实况句柄;
	*******************************************************************/
	IVS_ULONG GetPlayHandleByNetSourceChannel(IVS_ULONG ulNetSourceChannel);

	/******************************************************************
	function : GetPlayHandleByPlayChannel
	description: 根据播放信道获取播放句柄;
	input : ulPlayChannel，播放信道
	output : NA
	return : 实况句柄;
	*******************************************************************/
	IVS_ULONG GetPlayHandleByPlayChannel(unsigned long ulPlayChannel);

	/******************************************************************
	function : PlaySound
	description: 播放随路语音;
	input : uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 PlayAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : StopSound
	description: 停止播放随路语音;
	input : uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 StopAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : SetVolume
	description: 设置音量;
	input : uiVolumeValue，音量值；uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue);

	/******************************************************************
	function : GetVolume
	description: 获取音量;
	input : puiVolumeValue，音量值；uiPlayHandle，句柄
	output : NA
	return : 成功-0；失败-1
	*******************************************************************/
    IVS_INT32 GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue);

	CRealPlay *GetRealPlay(IVS_ULONG  ulHandle);
	CRealPlay *GetRealPlayEx(IVS_ULONG ulHandle);
	void  ReleaseRealPlay(CRealPlay *pRealPlay);

	/******************************************************************
	function			: EnablePURealplayIAOverLay
	description	: 启动智能分析轨迹叠加
	input				: pCameraCode
	input				: pNVRCode
	input				: hWnd 播放窗口句柄
	output			: NA
	return				: 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 EnablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle,
		const IVS_CHAR* pCameraCode,
		const IVS_CHAR *pNVRCode,
		HWND hWnd);

	/******************************************************************
	function			: DisablePURealplayIAOverLay
	description	: 停止智能分析轨迹叠加
	input				: pCameraCode
	input				: pNVRCode
	input				: hWnd 播放窗口句柄
	output			: NA
	return				: 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 DisablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle);

	/******************************************************************
	function			: GetRealPlayRtspURL
	description	: 获取回放url
	input				: pCameraCode
	input				: pURLMediaParam
	input				: uiBufferSize
	output			: pRtspURL
	return				: 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetRealPlayRtspURL(const IVS_CHAR* pCameraCode, 
		const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
		IVS_CHAR *pRtspURL, 
		IVS_UINT32 uiBufferSize);

private:
	/******************************************************************
	function : GetHandleByHwnd
	description: 检查窗口是否存在;
	input : hWnd，窗体句柄
	output : NA
	return : 播放句柄
	*******************************************************************/
	IVS_ULONG GetHandleByHwnd(HWND hWnd);

	/******************************************************************
	function : GetFreeRealPlay
	description: 获取空闲的实况对象;
	input : ulHandle，句柄
	output : NA
	return : 实况对象
	*******************************************************************/
	CRealPlay *GetFreeRealPlayEx(IVS_ULONG& ulHandle);
	void FreeRealPlayEx(IVS_ULONG ulHandle);

	/******************************************************************
	function			:	GetOverLayByPlayHandle
	description	:	根据实况句柄获取轨迹叠加对象，没有就创建;
	input				:	ulPlayHandle，实况句柄
	output			:	NA
	return				:	轨迹叠加对象
	*******************************************************************/
	CIAOverLayMgr *GetOverLayByPlayHandle(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function			:	GetOverLayByPlayHandle
	description	:	根据实况句柄获取轨迹叠加对象，没有就创建;
	input				:	ulPlayHandle，实况句柄
	input				:	pCameraID，摄像机编码
	input				:	pNVRCode，NVR编码
	output			:	NA
	return				:	轨迹叠加对象
	*******************************************************************/
	CIAOverLayMgr * CreateOverLayByPlayHandle(IVS_ULONG& ulPlayHandle, const IVS_CHAR *pCameraID, const IVS_CHAR *pNVRCode);

	/******************************************************************
	function			:	FreeOverLayByPlayHandle
	description	:	根据实况句柄释放轨迹叠加对象
	input				:	ulPlayHandle，实况句柄
	output			:	NA
	return				:	轨迹叠加对象
	*******************************************************************/
	IVS_VOID FreeOverLayByPlayHandle(IVS_ULONG ulPlayHandle);

private:
	CUserMgr *m_pUserMgr;
	//IVS_INT32 m_iHandle;

	//句柄和实况类的映射表;
	typedef std::map<IVS_ULONG, CRealPlay*> HandleRealPlayMap;
	typedef HandleRealPlayMap::iterator HandleRealPlayMapIter;
	HandleRealPlayMap m_HanleRealPlayMap;

	VOS_Mutex *m_pHanleRealPlayMutex;

	//实况句柄和轨迹叠加类的映射表
	typedef std::map<IVS_ULONG, CIAOverLayMgr*> HandleIAOverLayMap;
	typedef HandleIAOverLayMap::iterator HandleIAOverLayMapIter;
	HandleIAOverLayMap m_HandleIAOverLayMap;
	VOS_Mutex *m_pHandleIAOverLayMutex;

};

#endif
