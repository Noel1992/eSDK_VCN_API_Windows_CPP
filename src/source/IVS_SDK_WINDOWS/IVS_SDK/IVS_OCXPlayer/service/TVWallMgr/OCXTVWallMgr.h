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
#ifndef _OCX_TVWALL_MGR_H_
#define _OCX_TVWALL_MGR_H_

#include <afx.h>
#include "SDKDef.h"
#include "IVS_SDKTVWall.h"
#include "OCXTVWallXMLProcess.h"
class COCXTVWallMgr
{
public:
	COCXTVWallMgr(void);
	~COCXTVWallMgr(void);
	// ��ʼ������ǽ
	static IVS_INT32 InitTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// �ͷŵ���ǽ
	static IVS_INT32 ReleaseTVWall(IVS_INT32 iSessionID);
	// ��ȡ����ǽ�б�
	static IVS_VOID GetTVWallList(CString &strResult);
	// ��ȡ����ǽ��Ϣ
	static IVS_VOID GetTVWallByID(IVS_UINT32 uiTVWallID, CString &strResult);
	// ���ӵ���ǽ
	static CString AddTVWall(const CString& strTVWall);
	// ɾ������ǽ
	static IVS_INT32 DeleteTVWall(IVS_ULONG uiTVWallID);
	// ���ĵ���ǽ��Ϣ
	static IVS_INT32 ModifyTVWallInfo(const CString& strResult);
	// ���ĵ���ǽ����
	static IVS_INT32 ModifyTVWallLayout(const CString& strResult);
	// ��������ǽʵʱ���
	static IVS_INT32 StartRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
                                        const CString& strRealplayParam, const TVWall_VPANEID& vPaneID);
	// ֹͣ����ǽʵʱ���
	static IVS_INT32 StopRealPlayTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ��ʼ����ǽƽ̨¼��ط�
	static IVS_INT32 StartPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
                                                const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// ֹͣ����ǽƽ̨¼��ط�
	static IVS_INT32 StopPlatformPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ��ʼ����ǽǰ��¼��ط�
	static IVS_INT32 StartPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strCameraCode, 
		const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// ֹͣ����ǽǰ��¼��ط�
	static IVS_INT32 StopPUPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ��ʼ����ǽ����¼��ط�
	static IVS_INT32 StartBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const CString& strDomainCode, 
												const CString& strNVRCode, const CString& strCameraCode, 
												const CString& pPlaybackParam, const TVWall_VPANEID& vPaneID);
	// ֹͣ����ǽ����¼��ط�
	static IVS_INT32 StopBackupPlayBackTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ��ͣ����ǽ¼��ط�
	static IVS_INT32 PlayBackPauseTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// �ָ�����ǽ¼��ط�
	static IVS_INT32 PlayBackResumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ��֡����
	static IVS_INT32 PlaybackFrameStepForwardTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID);
	// ���ݽ�����ID��������ͨ���͵���ǽID ��ȡ����ǽ����ID
	static IVS_INT32 GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CUMW_NOTIFY_REMOTE_DECODER_STATUS& decoder,TVWall_VPANEID_LIST& VPaneIDList);
	static IVS_INT32 GetTVWallWndIDByDecoderIDChannel(const IVS_ULONG& ulTVWallID,const CString& strDecoderID,IVS_UINT32 uiChannel,TVWall_VPANEID& stPaneID);
	// ���ݽ�����ID�͵���ǽID ��ȡ����ǽID
	static IVS_INT32 GetTVWallWndIDByDecoderID(const IVS_ULONG& ulTVWallID,const CString& cstrDecoderID,std::vector<IVS_ULONG>& ulWndIDVec);
	// ������ע����Ϣ�б�
	static IVS_VOID GetDecoderList(IVS_INT32 iSessionID, CString &strResult);
	// ��ȡ���߽�����ID�б�
	static IVS_INT32 GetOnlineDecoderIDList(IVS_INT32 iSessionID,std::list<CUMW_NOTIFY_REMOTE_DECODER_STATUS>& decoderIDList);

	// ��������
	static IVS_INT32 PlaySoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID);
	// ֹͣ����
	static IVS_INT32 StopSoundTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID);
	// ��������
	static IVS_INT32 SetVolumeTVWall(IVS_INT32 iSessionID, IVS_ULONG ulTVWallID, const TVWall_VPANEID& stPaneID, IVS_UINT32 uiVolumeValue);
};

#endif	//_OCX_TVWALL_MGR_H_
