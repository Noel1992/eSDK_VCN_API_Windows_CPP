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
 description :    ʵ������ʵ��ҵ��;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 ��ʼ�汾
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
	description: ���ݾ����ȡrealplay;
	input : ulHandle�����
	output : NA
	return : MediaBase����
	*******************************************************************/
    CMediaBase *GetMediaBase(IVS_ULONG ulHandle);

    /******************************************************************
	function : StartRealPlay
	description: ��ʼʵ��;
	input : pRealplayPara,ʵ��������Ϣ��pCameraCode���������ţ�hWnd����������pHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle);

	/******************************************************************
	function : StartRealPlayCBRaw
	description: ��ʼʵ��(������);
	input : pRealplayPara,ʵ��������Ϣ��pCameraCode���������ţ�fRealPlayCallBackRaw���������ص���pUserData���û���Ϣ��pHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData, IVS_ULONG* pHandle );

	/******************************************************************
	function : StartRealPlayCBFrame
	description: ��ʼʵ��(YUV��);
	input : pRealplayPara,ʵ��������Ϣ��pCameraCode���������ţ�fRealPlayCallBackFrame��YUV�����ص���pUserData���û���Ϣ��pHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle );

	/******************************************************************
	function : StopRealPlay
	description: ֹͣʵ��;
	input : ulHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StopRealPlay(IVS_ULONG ulHandle);

	/******************************************************************
	function : AddPlayWindow
	description: ��Ӳ��Ŵ���;
	input : hWnd����������ulHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd);
	/******************************************************************
	function : DelPlayWindow
	description: ɾ�����Ŵ���;
	input : hWnd����������ulHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd);

	IVS_INT32 AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);
	IVS_INT32 UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial);
	/******************************************************************
	function : GetPlayerChannelByHandle
	description: ��ȡͨ����;
	input : ulHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_UINT32 GetPlayerChannelByHandle(IVS_ULONG ulHandle);

	/******************************************************************
	function : GetCameraCodeByHandle
	description: ���ݾ����ȡCameraCode;
	input : strCameraCode���������ţ�ulHandle�����
	output : NA
	return : ��
	*******************************************************************/
	void GetCameraCodeByHandle(IVS_ULONG ulHandle, std::string& strCameraCode);

	/******************************************************************
	function : StopAllRealPlay
	description: �ر�����ʵ��;
	input : ��
	output : NA
	return : ��
	*******************************************************************/
	void StopAllRealPlay();

	/******************************************************************
	function : GetPlayHandlebyRtspHandle
	description: ��ȡʵ�����;
	input : iRtspHandle�����
	output : NA
	return : ʵ�����;
	*******************************************************************/
	IVS_ULONG GetPlayHandlebyRtspHandle(IVS_ULONG iRtspHandle);

	IVS_ULONG GetPlayHandlebyRtspHandleEx(IVS_ULONG iRtspHandle);

	/******************************************************************
	function : GetPlayHandleByNetSourceChannel
	description: ��ȡʵ�����;
	input : ulNetSourceChannel�����
	output : NA
	return : ʵ�����;
	*******************************************************************/
	IVS_ULONG GetPlayHandleByNetSourceChannel(IVS_ULONG ulNetSourceChannel);

	/******************************************************************
	function : GetPlayHandleByPlayChannel
	description: ���ݲ����ŵ���ȡ���ž��;
	input : ulPlayChannel�������ŵ�
	output : NA
	return : ʵ�����;
	*******************************************************************/
	IVS_ULONG GetPlayHandleByPlayChannel(unsigned long ulPlayChannel);

	/******************************************************************
	function : PlaySound
	description: ������·����;
	input : uiPlayHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 PlayAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : StopSound
	description: ֹͣ������·����;
	input : uiPlayHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 StopAudioSound(unsigned int uiPlayHandle);

	/******************************************************************
	function : SetVolume
	description: ��������;
	input : uiVolumeValue������ֵ��uiPlayHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue);

	/******************************************************************
	function : GetVolume
	description: ��ȡ����;
	input : puiVolumeValue������ֵ��uiPlayHandle�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue);

	CRealPlay *GetRealPlay(IVS_ULONG  ulHandle);
	CRealPlay *GetRealPlayEx(IVS_ULONG ulHandle);
	void  ReleaseRealPlay(CRealPlay *pRealPlay);

	/******************************************************************
	function			: EnablePURealplayIAOverLay
	description	: �������ܷ����켣����
	input				: pCameraCode
	input				: pNVRCode
	input				: hWnd ���Ŵ��ھ��
	output			: NA
	return				: �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 EnablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle,
		const IVS_CHAR* pCameraCode,
		const IVS_CHAR *pNVRCode,
		HWND hWnd);

	/******************************************************************
	function			: DisablePURealplayIAOverLay
	description	: ֹͣ���ܷ����켣����
	input				: pCameraCode
	input				: pNVRCode
	input				: hWnd ���Ŵ��ھ��
	output			: NA
	return				: �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 DisablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle);

	/******************************************************************
	function			: GetRealPlayRtspURL
	description	: ��ȡ�ط�url
	input				: pCameraCode
	input				: pURLMediaParam
	input				: uiBufferSize
	output			: pRtspURL
	return				: �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 GetRealPlayRtspURL(const IVS_CHAR* pCameraCode, 
		const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
		IVS_CHAR *pRtspURL, 
		IVS_UINT32 uiBufferSize);

private:
	/******************************************************************
	function : GetHandleByHwnd
	description: ��鴰���Ƿ����;
	input : hWnd��������
	output : NA
	return : ���ž��
	*******************************************************************/
	IVS_ULONG GetHandleByHwnd(HWND hWnd);

	/******************************************************************
	function : GetFreeRealPlay
	description: ��ȡ���е�ʵ������;
	input : ulHandle�����
	output : NA
	return : ʵ������
	*******************************************************************/
	CRealPlay *GetFreeRealPlayEx(IVS_ULONG& ulHandle);
	void FreeRealPlayEx(IVS_ULONG ulHandle);

	/******************************************************************
	function			:	GetOverLayByPlayHandle
	description	:	����ʵ�������ȡ�켣���Ӷ���û�оʹ���;
	input				:	ulPlayHandle��ʵ�����
	output			:	NA
	return				:	�켣���Ӷ���
	*******************************************************************/
	CIAOverLayMgr *GetOverLayByPlayHandle(IVS_ULONG ulPlayHandle);

	/******************************************************************
	function			:	GetOverLayByPlayHandle
	description	:	����ʵ�������ȡ�켣���Ӷ���û�оʹ���;
	input				:	ulPlayHandle��ʵ�����
	input				:	pCameraID�����������
	input				:	pNVRCode��NVR����
	output			:	NA
	return				:	�켣���Ӷ���
	*******************************************************************/
	CIAOverLayMgr * CreateOverLayByPlayHandle(IVS_ULONG& ulPlayHandle, const IVS_CHAR *pCameraID, const IVS_CHAR *pNVRCode);

	/******************************************************************
	function			:	FreeOverLayByPlayHandle
	description	:	����ʵ������ͷŹ켣���Ӷ���
	input				:	ulPlayHandle��ʵ�����
	output			:	NA
	return				:	�켣���Ӷ���
	*******************************************************************/
	IVS_VOID FreeOverLayByPlayHandle(IVS_ULONG ulPlayHandle);

private:
	CUserMgr *m_pUserMgr;
	//IVS_INT32 m_iHandle;

	//�����ʵ�����ӳ���;
	typedef std::map<IVS_ULONG, CRealPlay*> HandleRealPlayMap;
	typedef HandleRealPlayMap::iterator HandleRealPlayMapIter;
	HandleRealPlayMap m_HanleRealPlayMap;

	VOS_Mutex *m_pHanleRealPlayMutex;

	//ʵ������͹켣�������ӳ���
	typedef std::map<IVS_ULONG, CIAOverLayMgr*> HandleIAOverLayMap;
	typedef HandleIAOverLayMap::iterator HandleIAOverLayMapIter;
	HandleIAOverLayMap m_HandleIAOverLayMap;
	VOS_Mutex *m_pHandleIAOverLayMutex;

};

#endif
