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
	filename	: 	DownloadMgr.cpp
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义录像下载管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/

#include "DownloadManger.h"
#include "LockGuard.h"
#include "UserMgr.h"
#include "MediaBase.h"
#include "IVS_Trace.h"

CDownloadMgr::CDownloadMgr(void)
{
	m_RefCounter.counter = 1;
	m_pUserMgr = NULL;
	m_pHandleDownLoadMutex = VOS_CreateMutex();
}

CDownloadMgr::~CDownloadMgr(void)
{
	m_pUserMgr = NULL;
	try
	{
		HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
		while (m_HandleDownLoadMap.end() != iter)
		{
			CDownload* pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
			if (NULL != pDownLoad)
			{
				IVS_DELETE(pDownLoad);
			}
			++iter;
		}
		m_HandleDownLoadMap.clear();

		if (NULL != m_pHandleDownLoadMutex)
		{
			(void)VOS_DestroyMutex(m_pHandleDownLoadMutex);
			m_pHandleDownLoadMutex = NULL;
		}
	}
	catch (...)
	{
	}
}

void CDownloadMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}


CDownload* CDownloadMgr::GetFreeDownLoad(IVS_ULONG& ulHandle)
{
	CLockGuard lock(m_pHandleDownLoadMutex);
	CDownload *pDownLoad = NULL;

	// 查找空闲对象;
	HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
	HandleDownLoadIter end  = m_HandleDownLoadMap.end();
	for (; iter != end; iter++)
	{
		pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
		if (NULL != pDownLoad)
		{
			if (pDownLoad->GetStatus() == MEDIA_STATUS_FREE)
			{
				break;
			}
		}
		pDownLoad = NULL;
	}

	if (NULL != pDownLoad)
	{
		m_HandleDownLoadMap.erase(iter);
	}
	else
	{
		pDownLoad = IVS_NEW((CDownload*&)pDownLoad);
		if (NULL == pDownLoad)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, 
				"Get Free DownLoad", "create DownLoad error");
			return NULL;
		}
	}

	//初始化realplay对象;
	pDownLoad->SetUserMgr(m_pUserMgr);
	pDownLoad->SetStatus(MEDIA_STATUS_BUSY);

	//插入列表;
	//ulHandle = reinterpret_cast<unsigned long>(pDownLoad);
	ulHandle = atomic_inc(&m_RefCounter);//lint !e732
	(void)m_HandleDownLoadMap.insert(std::make_pair(ulHandle, pDownLoad));
	pDownLoad->SetHandle(ulHandle);
	return pDownLoad;
}


CDownload* CDownloadMgr::GetDownLoad(IVS_ULONG ulHandle)
{
	CLockGuard lock(m_pHandleDownLoadMutex);
    CDownload *pDownLoad = NULL;
    HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
    HandleDownLoadIter end  = m_HandleDownLoadMap.end();

    //查找空闲对象;
    for (; iter != end; iter++)
    {
        if (iter->first == ulHandle)
        {
            pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
            break;
        }
    }
    return pDownLoad;
}

// 根据Rtsp句柄获取下载句柄
IVS_ULONG CDownloadMgr::GetDownloadHandlebyRtspHandle(IVS_ULONG iRtspHandle)
{
	CLockGuard lock(m_pHandleDownLoadMutex);
	IVS_ULONG ulHandle = 0;
    HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
    HandleDownLoadIter end  = m_HandleDownLoadMap.end();
    
	for (; iter != end; iter++)
    {
        CDownload* pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
        if (NULL == pDownLoad)
        {
            continue;
        }

        IVS_ULONG iRtspHandleTemp = pDownLoad->GetRtspHandle();
        if (iRtspHandleTemp == iRtspHandle)
        {
            ulHandle = iter->first;
            break;
        }
    }
    return ulHandle;
}


IVS_ULONG CDownloadMgr::GetDownloadHandleByNetSourceChannel(unsigned long ulNetSourceChannel)
{
	CLockGuard lock(m_pHandleDownLoadMutex);
	IVS_ULONG ulHandle = 0;
    HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
    HandleDownLoadIter end  = m_HandleDownLoadMap.end();
    for (; iter != end; iter++)
    {
        CDownload* pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
        if (NULL == pDownLoad)
        {
            continue;
        }
        unsigned long ulNetSourceHandleTemp = pDownLoad->GetNetSourceHandle();
        if (ulNetSourceHandleTemp == ulNetSourceChannel)
        {
            ulHandle = iter->first;
            break;
        }
    }
    return ulHandle;
}


IVS_ULONG CDownloadMgr::GetDownloadHandleByPlayChannel(unsigned long ulPlayChannel)
{
	CLockGuard lock(m_pHandleDownLoadMutex);
	IVS_ULONG ulHandle = 0;
    HandleDownLoadIter iter = m_HandleDownLoadMap.begin();
    HandleDownLoadIter end  = m_HandleDownLoadMap.end();
    for (; iter != end; iter++)
    {
        CDownload* pDownLoad = dynamic_cast<CDownload*>(iter->second);//lint !e611
        if (NULL == pDownLoad)
        {
            continue;
        }
        unsigned long ulNetSourceHandleTemp = pDownLoad->GetPlayerChannel();
        if (ulNetSourceHandleTemp == ulPlayChannel)
        {
            ulHandle = iter->first;
            break;
        }
    }
    return ulHandle;
}


// 启动平台(含备份服务器)录像下载
int CDownloadMgr::StartPlatformDownLoad(const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const char* pCameraCode, 
	    const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pHandle)
{
	IVS_DEBUG_TRACE("");
    //备份服务器时pDomainCode、pNVRCode均不为NULL，平台时均为NULL，参数不做校验
	if (NULL == pCameraCode || NULL == pDownloadParam || NULL == pHandle)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "CheckParam failed");
        return IVS_PARA_INVALID;
    }

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
  	IVS_INT32 iRet     = IVS_FAIL;

    CDownload *pDownload = GetFreeDownLoad(ulHandle);
    if (NULL != pDownload)
    {
      	iRet = pDownload->StartPlatformDownLoad(pDomainCode,pNVRCode,pCameraCode, pDownloadParam);
        if (IVS_SUCCEED != iRet)
        {
            pDownload->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }
	return iRet;
}

// 停止平台录像下载
int CDownloadMgr::StopPlatformDownLoad(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->StopDownLoad();
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 启动容灾录像下载
int CDownloadMgr::StartDisasterRecoveryDownload(const char* pCameraCode, 
    const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pHandle)
{
    IVS_DEBUG_TRACE("");

    if (NULL == pCameraCode || NULL == pDownloadParam || NULL == pHandle)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Platform DownLoad", "CheckParam failed");
        return IVS_PARA_INVALID;
    }

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
    IVS_INT32 iRet     = IVS_FAIL;

    CDownload *pDownload = GetFreeDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        iRet = pDownload->StartDisasterRecoveryDownload(pCameraCode, pDownloadParam);
        if (IVS_SUCCEED != iRet)
        {
            pDownload->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }
    return iRet;
}

// 停止容灾录像下载
int CDownloadMgr::StopDisasterRecoveryDownload(IVS_ULONG ulHandle)
{
    IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->StopDisasterRecoveryDownload();
    }
    return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 暂停下载
int CDownloadMgr::DownLoadPause(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->DownLoadPause();
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;

}

// 恢复下载
int CDownloadMgr::DownLoadResume(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->DownLoadResume();
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 设置下载速度
int CDownloadMgr::SetDownLoadSpeed(IVS_ULONG ulHandle, float fSpeed)
{
	IVS_DEBUG_TRACE("");
    //IVS_INT32 iRet = IVS_FAIL;

    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->SetDownLoadSpeed(fSpeed);
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}


// 获取下载信息
int CDownloadMgr::GetDownLoadInfo(IVS_ULONG ulHandle, IVS_DOWNLOAD_INFO* pInfo)
{
	IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->GetDownLoadInfo(pInfo);
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

// 启动前端录像下载
int CDownloadMgr::StartPUDownLoad(const char* pCameraCode,
	     const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG *pHandle)
{
    if (NULL == pCameraCode || NULL == pDownloadParam || NULL == pHandle)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start PU DownLoad", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    IVS_RECORD_INFO stRecordInfo;
    eSDK_MEMSET(&stRecordInfo, 0, sizeof(stRecordInfo));
    IVS_INT32 iRet = GetPUDownLoadFileName(pCameraCode, &pDownloadParam->stTimeSpan,stRecordInfo);
    if (IVS_SUCCEED != iRet)
    {
        return iRet;
    }

    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
    CDownload *pDownload = GetFreeDownLoad(ulHandle);
    if (NULL != pDownload)
    {
         iRet = pDownload->StartPUDownLoad(pCameraCode,
                                           pDownloadParam,
                                           stRecordInfo.cRecordFileName);
        if (IVS_SUCCEED != iRet)
        {
            pDownload->RetSet();
        }
        else
        {
            *pHandle = ulHandle;
        }
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;

}

// 停止前端录像下载
int CDownloadMgr::StopPUDownLoad(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
    CDownload *pDownload  = GetDownLoad(ulHandle);
    if (NULL != pDownload)
    {
        return pDownload->StopDownLoad();
    }
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}


// 获取前端录像文件名
int CDownloadMgr::GetPUDownLoadFileName(const char* pCameraCode,
                                              const IVS_TIME_SPAN* pTimeSpan,
                                              IVS_RECORD_INFO &stRecordInfo)
{
    if (NULL == pCameraCode || NULL == pTimeSpan)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU DownLoad FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pUserMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU DownLoad FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    eSDK_MEMSET(&stRecordInfo, 0, sizeof(stRecordInfo));

    IVS_INDEX_RANGE stIndexRange;
    stIndexRange.uiFromIndex = 1;
    stIndexRange.uiToIndex = 1;
    int iSize = sizeof(IVS_RECORD_INFO_LIST) + (stIndexRange.uiToIndex-stIndexRange.uiFromIndex) * sizeof(IVS_RECORD_INFO);   //lint !e737 !e713

	char* pBuff = IVS_NEW((char*&)pBuff,iSize);//lint !e732
	CHECK_POINTER(pBuff,IVS_ALLOC_MEMORY_ERROR);
    eSDK_MEMSET(pBuff, 0, iSize);   //lint !e732

    int iRet = m_pUserMgr->GetRecordList().GetRecordList(pCameraCode, 
                                                RECORD_TYPE_PU,
                                                pTimeSpan,
                                                &stIndexRange,
                                                (IVS_RECORD_INFO_LIST*)pBuff,
                                                (IVS_UINT32)iSize);   //lint !e826


    if (IVS_SUCCEED == iRet)
    {
        IVS_RECORD_INFO_LIST *pRecordList = (IVS_RECORD_INFO_LIST*)pBuff;   //lint !e826
        (void)CToolsHelp::Memcpy(&stRecordInfo, sizeof(stRecordInfo),
                           pRecordList->stRecordInfo, sizeof(IVS_RECORD_INFO));
    }
	IVS_DELETE(pBuff,MUILI);

    return iRet;
}


int CDownloadMgr::GetDownloadRtspURL(const IVS_CHAR* pCameraCode, 
	const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
	IVS_CHAR *pRtspURL, 
	IVS_UINT32 uiBufferSize)
{
		//校验参数
		if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || strlen(pCameraCode) == 0)
		{
			BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"GetDownloadRtspURL failed", "param invalid");
			return IVS_SMU_CAMERA_CODE_NOT_EXIST;
		}
		CHECK_POINTER(pURLMediaParam, IVS_PARA_INVALID);

		IVS_INT32 iRet = IVS_FAIL;
		IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
		
		CDownload *pDownload = GetFreeDownLoad(ulHandle);
		if(NULL != pDownload)
		{
			iRet = pDownload->GetDownloadRtspURL(NULL, pURLMediaParam->cNVRCode, pCameraCode, pURLMediaParam, pRtspURL, uiBufferSize);
			if(iRet != IVS_SUCCEED)
			{
				BP_RUN_LOG_ERR(iRet, "GetDownloadRtspURL", "get download url failed");
			}
		}
		else
		{
			iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
			BP_RUN_LOG_ERR(iRet, "GetDownloadRtspURL", "get download handle failed");
		}

		return iRet;
}



int CDownloadMgr::StartPUDownLoadByChannel(IVS_INT32 uiChannel,
	const IVS_DOWNLOAD_PARAM* pDownloadParam,
	IVS_ULONG  *pHandle)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDownloadParam, IVS_PARA_INVALID);
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	
	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	IVS_INT32 iRet = m_pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);//lint !e613 !e732
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "StartPUDownLoadByChannel", "Get Uni CameraCode By Channel failed");
		return iRet;
	}

	iRet = StartPUDownLoad(pDevCode, pDownloadParam, pHandle);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "StartPUDownLoadByChannel", "Start PU Download failed");
	}
	return iRet;
}


int CDownloadMgr::StartPlatformDownLoadByChannel(
	const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
	IVS_INT32 uiChannel,
	const IVS_DOWNLOAD_PARAM* pDownloadParam,
	IVS_ULONG* pHandle
	)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDownloadParam, IVS_PARA_INVALID);
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	std::vector<IVS_DEVICE_CODE> stCameraCodeVct;
	IVS_INT32 iRet = m_pUserMgr->GetNVRChannelMgr().GetMultiCameraCodeByChannel(uiChannel, stCameraCodeVct);//lint !e613 !e732
	IVS_UINT32 uiCameraCodeNum = stCameraCodeVct.size();
	if(IVS_SUCCEED != iRet || 0 == uiCameraCodeNum)
	{
		BP_RUN_LOG_ERR(iRet, "StartPUDownLoadByChannel", "Get Multiple CameraCode By Channel failed");
		return iRet;
	}

	for(IVS_UINT32 i = 0; i < uiCameraCodeNum; i++)
	{
		iRet = StartPlatformDownLoad(NULL,pDownloadParam->cNVRCode,stCameraCodeVct[i].cDevCode, pDownloadParam, pHandle);//lint !e732
		// once playback success, return
		if(i < uiCameraCodeNum - 1 && iRet != IVS_SUCCEED)
		{
			iRet = IVS_SUCCEED;
			continue;
		}

		if(IVS_SUCCEED == iRet)
		{
			return IVS_SUCCEED;
		}	
	}
	return iRet;
}











