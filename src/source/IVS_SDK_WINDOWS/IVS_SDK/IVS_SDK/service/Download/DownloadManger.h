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

#ifndef __DOWMLOAD_MGR_H__
#define __DOWMLOAD_MGR_H__

#include "Download.h"
#include "vos_atomic.h"
#include "UserInfo.h"
#include "hwsdk.h"
#include "SDKDef.h"

class CDownloadMgr
{
public:
	CDownloadMgr(void);
	~CDownloadMgr(void);

public:
	void SetUserMgr(CUserMgr* pUserMgr);
	
	int StartPlatformDownLoad(
		    const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		    const char* pCameraCode,
		    const IVS_DOWNLOAD_PARAM* pDownloadParam,
		    IVS_ULONG* pHandle
		  );

	int StartPlatformDownLoadByChannel(
		const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		IVS_INT32 uiChannel,
		const IVS_DOWNLOAD_PARAM* pDownloadParam,
		IVS_ULONG* pHandle
		);
	
	int StopPlatformDownLoad(IVS_ULONG ulHandle);

    // 启动容灾录像下载
    int StartDisasterRecoveryDownload(const char* pCameraCode, 
        const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pHandle);
    // 停止容灾录像下载
    int StopDisasterRecoveryDownload(IVS_ULONG ulHandle);

	int DownLoadPause(IVS_ULONG ulHandle);
	int DownLoadResume(IVS_ULONG ulHandle);

	int SetDownLoadSpeed(IVS_ULONG ulHandle, float fSpeed);
	int GetDownLoadInfo(IVS_ULONG ulHandle, IVS_DOWNLOAD_INFO* pInfo);

	int StartPUDownLoad(
			const char* pCameraCode,
			const IVS_DOWNLOAD_PARAM* pDownloadParam,
			IVS_ULONG  *pHandle
		);

	int StartPUDownLoadByChannel(
		IVS_INT32 uiChannel,
		const IVS_DOWNLOAD_PARAM* pDownloadParam,
		IVS_ULONG  *pHandle
		);

	int StopPUDownLoad(IVS_ULONG ulHandle);

	CDownload* GetFreeDownLoad(IVS_ULONG& ulHandle);
    CDownload* GetDownLoad(IVS_ULONG ulHandle);

	// 根据rtsp handle获取下载句柄
    IVS_ULONG  GetDownloadHandlebyRtspHandle(IVS_ULONG iRtspHandle);

    // 根据netSourceHandle获取下载句柄
    IVS_ULONG GetDownloadHandleByNetSourceChannel(unsigned long ulNetSourceChannel);

	// 根据播放通道号获取下载句柄
    IVS_ULONG GetDownloadHandleByPlayChannel(unsigned long ulPlayChannel);


    // 获取前端录像文件名
	int GetPUDownLoadFileName(const char* pCameraCode,
                                              const IVS_TIME_SPAN* pTimeSpan,
                                              IVS_RECORD_INFO &stRecordInfo);
	// 获取rtsp url
	int GetDownloadRtspURL(const IVS_CHAR* pCameraCode, 
		const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
		IVS_CHAR *pRtspURL, 
		IVS_UINT32 uiBufferSize);

private:
	CUserMgr *m_pUserMgr;
	typedef std::map<unsigned long, CDownload*> HandleDownLoadMap;
	typedef HandleDownLoadMap::iterator HandleDownLoadIter;
	
	HandleDownLoadMap m_HandleDownLoadMap;
	VOS_Mutex* m_pHandleDownLoadMutex;	
	vos_atomic_t        m_RefCounter;
};

#endif
