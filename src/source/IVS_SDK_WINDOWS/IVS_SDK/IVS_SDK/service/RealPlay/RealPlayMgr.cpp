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

#include "RealPlayMgr.h"
#include "UserMgr.h"
#include "bp_log.h"
#include "IVS_Trace.h"

CRealPlayMgr::CRealPlayMgr(void)
{
	m_pUserMgr = NULL;
	m_pHanleRealPlayMutex = VOS_CreateMutex();
	m_pHandleIAOverLayMutex = VOS_CreateMutex();
}


CRealPlayMgr::~CRealPlayMgr(void)
{
	m_pUserMgr = NULL;
	try
	{
		HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
		while(m_HanleRealPlayMap.end() != iter)
		{
			CPlayback* pPlayback = dynamic_cast<CPlayback*>(iter->second);//lint !e611
			if (NULL != pPlayback)
			{
				IVS_DELETE(pPlayback);
			}
			++iter;
		}
		m_HanleRealPlayMap.clear();

		if (NULL != m_pHanleRealPlayMutex)
		{
			(void)VOS_DestroyMutex(m_pHanleRealPlayMutex);
			m_pHanleRealPlayMutex = NULL;
		}
	}
	catch (...)
	{
	}

	try
	{
		HandleIAOverLayMapIter iter = m_HandleIAOverLayMap.begin();
		while(m_HandleIAOverLayMap.end() != iter)
		{
			CIAOverLayMgr* pRealPlayIAOverLay = dynamic_cast<CIAOverLayMgr*>(iter->second);//lint !e611
			if (NULL != pRealPlayIAOverLay)
			{
				IVS_DELETE(pRealPlayIAOverLay);
			}
			++iter;
		}
		m_HandleIAOverLayMap.clear();

		if (NULL != m_pHandleIAOverLayMutex)
		{
			(void)VOS_DestroyMutex(m_pHandleIAOverLayMutex);
			m_pHandleIAOverLayMutex = NULL;
		}
	}
	catch (...)
	{
	}

}//lint !e1740


void CRealPlayMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}


CRealPlay *CRealPlayMgr::GetFreeRealPlayEx(IVS_ULONG& ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	pRealPlay = IVS_NEW((CRealPlay*&)pRealPlay);
	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Free RealPlayEx", "create real play error");
		return NULL;
	}

	BP_DBG_LOG("Get Free RealPlayEx", "CreateHandle %u", (unsigned long)pRealPlay);
	
	ulHandle = (unsigned long)pRealPlay;
	pRealPlay->SetHandle(ulHandle);
	pRealPlay->SetUserMgr(m_pUserMgr);
	//2015-2-7 高书明 add
	//设置状态，否则注销时不能停止
	pRealPlay->SetStatus(MEDIA_STATUS_BUSY);


	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	(void)m_HanleRealPlayMap.insert(std::make_pair(ulHandle, pRealPlay));
	(void)pRealPlay->GetRef();
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return pRealPlay;
}


//开始实况
IVS_INT32 CRealPlayMgr::StartRealPlay(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle)
{
	if (NULL == pRealplayPara || NULL == pHandle || NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Start Real Play", "Para Invalid");
		return IVS_PARA_INVALID;
	}

	BP_RUN_LOG_INF("Enter CRealPlayMgr::StartRealPlay", "pCameraCode = %s, StreamType = %u, ProtocolType = %u, DirectFirst = %d, MultiCast = %d, Wnd = %d.",
		pCameraCode, pRealplayPara->uiStreamType, pRealplayPara->uiProtocolType, pRealplayPara->bDirectFirst, pRealplayPara->bMultiCast, (int)hWnd); //lint !e1924

	IVS_INT32 iRet = IVS_FAIL;
	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
    
	//初始化realplay对象;
	CRealPlay *pRealPlay = GetFreeRealPlayEx(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlay(pRealplayPara, pCameraCode, hWnd);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlayEx(ulHandle);
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

	BP_RUN_LOG_INF("Out CRealPlayMgr::StartRealPlay", "ulHandle = %d, iRet = %d.", ulHandle, iRet);

    return iRet;
}

//根据句柄获取realplay;
CMediaBase *CRealPlayMgr::GetMediaBase(IVS_ULONG ulHandle)
{
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CMediaBase *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
    HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (; realplayIter != realplayIterEnd; realplayIter++)
	{
		if (realplayIter->first == ulHandle)
		{
            pRealPlay = realplayIter->second;
			break;
		}
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}
//停止实况
IVS_INT32 CRealPlayMgr::StopRealPlay(IVS_ULONG ulHandle)
{
	BP_RUN_LOG_INF("Enter CRealPlayMgr::StopRealPlay", "ulHandle = %d.", ulHandle);

	IVS_INT32 iRet = IVS_FAIL;

	if (NULL != m_pUserMgr)
	{
		m_pUserMgr->GetAutoResume().RemoveStopPlayHandle(ulHandle);
	}

	CIAOverLayMgr *pIAOverLay = GetOverLayByPlayHandle(ulHandle);
	if (NULL != pIAOverLay)
	{
		iRet = pIAOverLay->DisableOverlay();
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "DisablePURealplayIAOverLay", "DisableOverlay failed, RealplayHandle: %lu", ulHandle);
			return iRet;
		}
		FreeOverLayByPlayHandle(ulHandle);
	}

	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StopRealPlay();
    	FreeRealPlayEx(ulHandle);
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
	}

	BP_RUN_LOG_INF("Out CRealPlayMgr::StopRealPlay", "ulHandle = %d, iRet = %d.",
		ulHandle, iRet);

	return iRet;
}

//添加播放窗口
IVS_INT32 CRealPlayMgr::AddPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->AddPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}
//删除播放窗口
IVS_INT32 CRealPlayMgr::DelPlayWindow(IVS_ULONG ulHandle, HWND hWnd)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->DelPlayWindow(hWnd);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//增加局部图像播放窗口
IVS_INT32 CRealPlayMgr::AddPlayWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->AddPlayWindowPartial(hWnd, pPartial);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//更新局部放大源矩形
IVS_INT32 CRealPlayMgr::UpdateWindowPartial(IVS_ULONG ulHandle, HWND hWnd, void* pPartial)
{
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(ulHandle));
	if (NULL != pRealPlay)
	{
		return pRealPlay->UpdateWindowPartial(hWnd, pPartial);
	}
	return IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
}

//获取通道号(如果失败返回-1);
IVS_UINT32 CRealPlayMgr::GetPlayerChannelByHandle(IVS_ULONG ulHandle)
{
	IVS_DEBUG_TRACE("");
	IVS_UINT32 uiChannel = SDK_INVALID_HANDLE;

	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	const CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter;

	//查找空闲对象;
	for (realplayIter = m_HanleRealPlayMap.begin(); realplayIter != m_HanleRealPlayMap.end(); realplayIter++)
	{
		if (realplayIter->first == ulHandle)
		{
            pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
			if (NULL != pRealPlay)
			{
				uiChannel = pRealPlay->GetPlayerChannel();
			}
			break;
		}
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return uiChannel;
}

//根据句柄获取CameraCode;
void CRealPlayMgr::GetCameraCodeByHandle(IVS_ULONG ulHandle, std::string& strCameraCode)
{
	//char *pCameraCode = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.find(ulHandle);
	if (realplayIter != m_HanleRealPlayMap.end())
	{
		pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second); //lint !e611
		// mod by z00193167 未释放锁
		if (NULL == pRealPlay)
		{
			(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
			return;
		}
		strCameraCode = pRealPlay->GetCameraCode();
		(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
		return;
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
}

//检查窗口是否存在;
IVS_ULONG CRealPlayMgr::GetHandleByHwnd(HWND hWnd)
{
    IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
    HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (;realplayIter != realplayIterEnd; realplayIter++)
	{
        pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
		if (NULL != pRealPlay && pRealPlay->IsExistHWnd(hWnd))
		{
			ulHandle = realplayIter->first;
			break;
		}
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	return ulHandle;
}//lint !e818

//开始实况(裸码流);
IVS_INT32 CRealPlayMgr::StartRealPlayCBRaw(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackRaw fRealPlayCallBackRaw, void* pUserData, IVS_ULONG* pHandle )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_FAIL;

	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
	CRealPlay *pRealPlay = GetFreeRealPlayEx(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlayCBRaw(pRealplayPara, pCameraCode, fRealPlayCallBackRaw, pUserData);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlayEx(ulHandle);
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

//开始实况(YUV流);
IVS_INT32 CRealPlayMgr::StartRealPlayCBFrame(IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle )
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);

	IVS_INT32 iRet = IVS_FAIL;
	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;
	CRealPlay *pRealPlay = GetFreeRealPlayEx(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->StartRealPlayCBFrame(pRealplayPara, pCameraCode, fRealPlayCallBackFrame, pUserData);
		if (IVS_SUCCEED != iRet)
		{
			FreeRealPlayEx(ulHandle);
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

//关闭所有实况;
void CRealPlayMgr::StopAllRealPlay()
{
	IVS_DEBUG_TRACE("");
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);

	CRealPlay *pRealPlay = NULL;
	HandleRealPlayMapIter realplayIter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter realplayIterEnd = m_HanleRealPlayMap.end();

	//查找空闲对象;
	for (;realplayIter != realplayIterEnd; realplayIter++)
	{

		CIAOverLayMgr *pIAOverLay = GetOverLayByPlayHandle(realplayIter->first);
		if (NULL != pIAOverLay)
		{
			IVS_INT32 iRet = pIAOverLay->DisableOverlay();
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "DisablePURealplayIAOverLay", "DisableOverlay failed, RealplayHandle: %lu", realplayIter->first);
			}
			else
			{
				FreeOverLayByPlayHandle(realplayIter->first);
			}
		}

		pRealPlay = dynamic_cast<CRealPlay*>(realplayIter->second);//lint !e611
		if (NULL != pRealPlay)
		{
			try
			{
				if (pRealPlay->GetStatus() == MEDIA_STATUS_BUSY)
				{
					(void)pRealPlay->StopRealPlay();
				}
			}
			catch(...)
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "StopAllRealPlay", "Delete realplay[%lu] failed.", pRealPlay);
			}

			try
			{
				IVS_DELETE(pRealPlay);
			}
			catch(...)
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "StopAllRealPlay", "Delete realplay[%lu] failed.", pRealPlay);
			}
			
		}
		pRealPlay = NULL;
	}
	m_HanleRealPlayMap.clear();
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
}

//获取实况句柄
IVS_ULONG CRealPlayMgr::GetPlayHandlebyRtspHandle(IVS_ULONG iRtspHandle)
{
	IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		const CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		IVS_ULONG iRtspHandleTemp = pRealPlay->GetRtspHandle();
		if (iRtspHandleTemp == iRtspHandle)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}
	
	return ulPlayHandle;
}


IVS_ULONG CRealPlayMgr::GetPlayHandlebyRtspHandleEx(IVS_ULONG iRtspHandle)
{
	IVS_ULONG ulPlayHandle = 0;

    (void)VOS_MutexLock(m_pHanleRealPlayMutex);
	
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		CRealPlay* pRealPlay = iter->second;
		if (NULL == pRealPlay)
		{
			continue;
		}

		if (iRtspHandle == pRealPlay->GetRtspHandle())
		{
			ulPlayHandle = iter->first;
			(void)pRealPlay->GetRef();
			break;
		}
	}

	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return ulPlayHandle;
}



//获取实况句柄
IVS_ULONG CRealPlayMgr::GetPlayHandleByNetSourceChannel(IVS_ULONG ulNetSourceChannel)
{
    IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		const CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		IVS_ULONG ulNetSourceHandleTemp = pRealPlay->GetNetSourceHandle();
		if (ulNetSourceHandleTemp == ulNetSourceChannel)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}

	return ulPlayHandle;
}
//根据播放信道获取播放句柄
IVS_ULONG CRealPlayMgr::GetPlayHandleByPlayChannel(unsigned long ulPlayChannel)
{
	IVS_ULONG ulPlayHandle = 0;
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.begin();
	HandleRealPlayMapIter iterEnd = m_HanleRealPlayMap.end();
	for (; iter != iterEnd; iter++)
	{
		const CRealPlay* pRealPlay = dynamic_cast<CRealPlay*>(iter->second);//lint !e611
		if (NULL == pRealPlay)
		{
			continue;
		}
		unsigned long ulNetSourceHandleTemp = pRealPlay->GetPlayerChannel();
		if (ulNetSourceHandleTemp == ulPlayChannel)
		{
			ulPlayHandle = iter->first;
			break;
		}
	}

	return ulPlayHandle;
}

// 播放随路语音
IVS_INT32 CRealPlayMgr::PlayAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->PlaySound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//停止播放随路语音
IVS_INT32 CRealPlayMgr::StopAudioSound(unsigned int uiPlayHandle)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->StopSound();
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//设置音量
IVS_INT32 CRealPlayMgr::SetVolume(unsigned int uiPlayHandle, unsigned int uiVolumeValue)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->SetVolume(uiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}

//获取音量
IVS_INT32 CRealPlayMgr::GetVolume(unsigned int uiPlayHandle, unsigned int* puiVolumeValue)
{	
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(GetMediaBase(uiPlayHandle));
    if (NULL != pRealPlay)
    {
        iRet = pRealPlay->GetVolume(puiVolumeValue);
    }
    else
    {
        iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
    }

    return iRet;
}


void CRealPlayMgr::FreeRealPlayEx(IVS_ULONG ulHandle)
{
    BP_DBG_LOG("Free Real PlayEx", "Free Handle %u", ulHandle);

	CRealPlay *pRealPlay = NULL;
	
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
		m_HanleRealPlayMap.erase(iter++);
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);

	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "FreeRealPlayEx", "pRealPlay = NULL");
		return;
	}

	if (0 == pRealPlay->PutRef())
	{
		BP_DBG_LOG("Free Real PlayEx", "Delete Handle %u", ulHandle);
	    IVS_DELETE(pRealPlay);
	}
}


CRealPlay *CRealPlayMgr::GetRealPlay(IVS_ULONG ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}



CRealPlay *CRealPlayMgr::GetRealPlayEx(IVS_ULONG ulHandle)
{
	CRealPlay *pRealPlay = NULL;
	(void)VOS_MutexLock(m_pHanleRealPlayMutex);
	HandleRealPlayMapIter iter = m_HanleRealPlayMap.find(ulHandle);
	if (iter != m_HanleRealPlayMap.end())
	{
		pRealPlay = iter->second;
		(void)pRealPlay->GetRef();
	}
	(void)VOS_MutexUnlock(m_pHanleRealPlayMutex);
	return pRealPlay;
}

void CRealPlayMgr::ReleaseRealPlay(CRealPlay * pRealPlay)
{
	if (pRealPlay && 0 == pRealPlay->PutRef())
	{
		BP_RUN_LOG_INF("CRealPlayMgr::ReleaseRealPlay", "Del RealPlay: ulHandle = %d.", 
			(unsigned long)pRealPlay); //lint !e1924
		IVS_DELETE(pRealPlay);
	}
}//lint !e1762 暂不列为const


//启动智能分析轨迹叠加
IVS_INT32 CRealPlayMgr::EnablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle,
	const IVS_CHAR* pCameraCode,
	const IVS_CHAR *pNVRCode,
	HWND hWnd)
{	
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(hWnd, IVS_PARA_INVALID);

	//Get Playback Object by playbackHandle
	CRealPlay* pRealPlay = GetRealPlay(ulRealPlayHandle);
	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "EnablePURealplayIAOverLay", "Get RealPlay Object by RealPlayHandle failed, RealPlayHandle: %lu Invalid", ulRealPlayHandle);
		return IVS_PARA_INVALID;
	}

	//获取IAOverLay对象
	CIAOverLayMgr* pRealPlayIAOverLay = CreateOverLayByPlayHandle(ulRealPlayHandle, pCameraCode, pNVRCode);
	if (NULL == pRealPlayIAOverLay)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "EnablePURealplayIAOverLay", "GetOverLayByPlayHandle failed");
		return IVS_FAIL;
	}

	return pRealPlayIAOverLay->EnableOverlay(IVS_ABILITY_IA_QD, hWnd, IA_QUERY_TYPE_REALTIME);
}

//停止智能分析轨迹叠加
IVS_INT32 CRealPlayMgr::DisablePURealplayIAOverLay(IVS_ULONG ulRealPlayHandle)
{	
	IVS_DEBUG_TRACE("");

	//Get Playback Object by playbackHandle
	CRealPlay* pRealPlay = GetRealPlay(ulRealPlayHandle);
	if (NULL == pRealPlay)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "EnablePURealplayIAOverLay", "Get RealPlay Object by RealPlayHandle failed, RealPlayHandle: %lu Invalid", ulRealPlayHandle);
		return IVS_PARA_INVALID;
	}

	//获取IAOverLay对象
	CIAOverLayMgr* pRealPlayIAOverLay = GetOverLayByPlayHandle(ulRealPlayHandle);
	if (NULL == pRealPlayIAOverLay)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "EnablePURealplayIAOverLay", "the OverLay Object of RealPlayHandle: %lu is not exist", ulRealPlayHandle);
		return IVS_PARA_INVALID;
	}

	IVS_INT32 iRet = pRealPlayIAOverLay->DisableOverlay();
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DisablePURealplayIAOverLay", "CRealPlayIAOverLay::DisableOverlay failed");
	}
	FreeOverLayByPlayHandle(ulRealPlayHandle);

	return iRet;
}


CIAOverLayMgr *CRealPlayMgr::GetOverLayByPlayHandle(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("PlayHandle : %lu", ulPlayHandle);
	(void)VOS_MutexLock(m_pHandleIAOverLayMutex);
	CIAOverLayMgr *pRealPlayIAOverLay = NULL;

	HandleIAOverLayMapIter IAOverLayIter = m_HandleIAOverLayMap.find(ulPlayHandle);
	//found this key, existed
	if (IAOverLayIter != m_HandleIAOverLayMap.end())
	{
		pRealPlayIAOverLay = IAOverLayIter->second;
		//erase invalid node
		if (NULL == pRealPlayIAOverLay)
		{
			m_HandleIAOverLayMap.erase(ulPlayHandle);
		}
	}

	(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
	return pRealPlayIAOverLay;
}


CIAOverLayMgr *CRealPlayMgr::CreateOverLayByPlayHandle(IVS_ULONG& ulPlayHandle, const IVS_CHAR *pCameraID, const IVS_CHAR *pNVRCode)
{
	IVS_DEBUG_TRACE("PlayHandle : %lu", ulPlayHandle);
	(void)VOS_MutexLock(m_pHandleIAOverLayMutex);
	CIAOverLayMgr *pRealPlayIAOverLay = NULL;

	HandleIAOverLayMapIter IAOverLayIter = m_HandleIAOverLayMap.find(ulPlayHandle);
	if (IAOverLayIter != m_HandleIAOverLayMap.end())	//已经存在
	{
		pRealPlayIAOverLay = IAOverLayIter->second;
		if (NULL != pRealPlayIAOverLay)
		{
			(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
			return pRealPlayIAOverLay;
		}
		else	//无效的空对象，删除该节点后重新创建
		{
			m_HandleIAOverLayMap.erase(ulPlayHandle);
		}
	}

	if (NULL == pCameraID)
	{
		(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetOverLayByPlayHandle", "Camera ID is NULL");
		return NULL;
	}
	if (NULL == pNVRCode)
	{
		(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetOverLayByPlayHandle", "NVR Code is NULL");
		return NULL;
	}
	if (NULL == m_pUserMgr)
	{
		(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetOverLayByPlayHandle", "m_pUserMgr is NULL");
		return NULL;
	}
	//没有就创建
	pRealPlayIAOverLay = IVS_NEW((CIAOverLayMgr *&)pRealPlayIAOverLay);
	if (NULL == pRealPlayIAOverLay)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"GetOverLayByPlayHandle", "create object error");
		(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
		return NULL;
	}

	//初始化RealPlayIAOverLay对象
	pRealPlayIAOverLay->SetSessionID(m_pUserMgr->GetSessionID());
	pRealPlayIAOverLay->SetPlayHandle(ulPlayHandle);
	(void)pRealPlayIAOverLay->SetCameraID(pCameraID);
	(void)pRealPlayIAOverLay->SetNVRCode(pNVRCode);

	//插入列表;
	(void)m_HandleIAOverLayMap.insert(std::make_pair(ulPlayHandle, pRealPlayIAOverLay));

	(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);
	return pRealPlayIAOverLay;
}

IVS_VOID CRealPlayMgr::FreeOverLayByPlayHandle(IVS_ULONG ulPlayHandle)
{
	IVS_DEBUG_TRACE("PlayHandle :%lu", ulPlayHandle);
	CIAOverLayMgr *pIAOverLay = NULL;

	(void)VOS_MutexLock(m_pHandleIAOverLayMutex);
	HandleIAOverLayMapIter iter = m_HandleIAOverLayMap.find(ulPlayHandle);
	if (iter != m_HandleIAOverLayMap.end())
	{
		pIAOverLay = iter->second;
		m_HandleIAOverLayMap.erase(iter);
	}
	(void)VOS_MutexUnlock(m_pHandleIAOverLayMutex);

	if (NULL == pIAOverLay)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "FreeOverLayByPlayHandle", "pIAOverLay is NULL");
		return;
	}

	BP_DBG_LOG("FreeOverLayByPlayHandle", "Delete Handle %u", ulPlayHandle);
	IVS_DELETE(pIAOverLay);
}


IVS_INT32 CRealPlayMgr::GetRealPlayRtspURL(const IVS_CHAR* pCameraCode, 
	const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
	IVS_CHAR *pRealPlayURL, 
	IVS_UINT32 uiBufferSize)
{
	//校验参数
	if (NULL == pCameraCode || strlen(pCameraCode) > (CODE_LEN-1) || strlen(pCameraCode) == 0)
	{
		BP_RUN_LOG_ERR(IVS_SMU_CAMERA_CODE_NOT_EXIST,"GetRealPlayRtspURL failed", "param invalid");
		return IVS_SMU_CAMERA_CODE_NOT_EXIST;
	}
	CHECK_POINTER(pURLMediaParam, IVS_PARA_INVALID);

	IVS_INT32 iRet = IVS_FAIL;
	IVS_ULONG ulHandle = SDK_INVALID_HANDLE;

	//初始化realplay对象;
	CRealPlay *pRealPlay = GetFreeRealPlayEx(ulHandle);
	if (NULL != pRealPlay)
	{
		iRet = pRealPlay->GetRealPlayRtspURL(pCameraCode, pURLMediaParam, pRealPlayURL, uiBufferSize);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get RealPlay URL failed", "Get RealPlay URL failed");
		}
	}
	else
	{
		iRet = IVS_SDK_ERR_FINDVIDEOHANDLE_FAILED;
		BP_RUN_LOG_ERR(iRet, "Get RealPlay handle failed", "Get RealPlay handle failed");
	}

	FreeRealPlayEx(ulHandle);

	BP_RUN_LOG_INF("Get RealPlay URL", "success");

	return iRet;
}





