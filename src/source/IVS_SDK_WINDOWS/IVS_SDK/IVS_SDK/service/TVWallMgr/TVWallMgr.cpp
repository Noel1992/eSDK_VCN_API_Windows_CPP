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

#include "TVWallMgr.h"
#include "bp_def.h"
#include "bp_log.h"
#include "IVS_SDKTVWall.h"
#include "IVS_TVWall.h"
#include "XmlProcess.h"
#include "ivs_xml.h"
#include "ToolsHelp.h"
#include "PlayThread.h"
#include "AutoResumeTrigger.h"
#include "RDAChannel.h"
#include "SDKChannel.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
#include "MediaBase.h"
#include "RecordList.h"
#include "UserMgr.h"
#include "HikDecoderSDK.h"
#include "Aes256Operator.h"
#include "DecoderSDKFactory.h"

CTVWallMgr::CTVWallMgr(void) : m_pUserMgr(NULL)
								, m_bTimerInit(false)
								, m_ulAsyncSeq(1)
{
	m_pInitDecoderMapMutex = VOS_CreateMutex();
	m_pDecChMapMutex = VOS_CreateMutex();
    m_pOnlineDecoderMapMutex = VOS_CreateMutex();
    m_pAutoResumeMapMutex = VOS_CreateMutex();
	m_pCallBackMapMutex = VOS_CreateMutex();
	m_AutoResumeTigger.SetTVWallMgr(this);
    m_pIdleChMapMutex = VOS_CreateMutex();
    m_pHandleMutex = VOS_CreateMutex();
	m_pCapabilityVecMutex = VOS_CreateMutex();
	m_pStartDecoderMapMutex = VOS_CreateMutex();
	m_pDecoderNameMutex = VOS_CreateMutex();
}

CTVWallMgr::~CTVWallMgr(void)
{
	m_pUserMgr = NULL;
    try
    {
        (void)m_timer.cancelTimer(&m_AutoResumeTigger);
        m_timer.clearTimer();
        m_timer.exit();
		m_bTimerInit = false;
    }
    catch (...)
    {
    }
	//通道
	(void)VOS_MutexLock(m_pDecChMapMutex);
	try
	{
		for(TVWALL_DEC_CH_MAP::iterator ite=m_tvwallDecChMap.begin();ite!=m_tvwallDecChMap.end();ite++)
		{
			CDecoderChannel* pCh = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
			IVS_DELETE(pCh);
		}
	}
	catch (...)
	{
	}
	(void)VOS_MutexUnlock(m_pDecChMapMutex);
	//初始化信息
	(void)VOS_MutexLock(m_pInitDecoderMapMutex);
	try
	{
		for(REMOTE_DECODER_INIT_INFO_MAP::iterator ite=m_remoteDecodeInitInfoMap.begin();ite!=m_remoteDecodeInitInfoMap.end();ite++)
		{
			CUMW_NOTIFY_INIT_DECODER_INFO* pInitInfo = dynamic_cast<CUMW_NOTIFY_INIT_DECODER_INFO*>(ite->second);//lint !e611
			IVS_DELETE(pInitInfo);
		}
	}
	catch (...)
	{
	}
	(void)VOS_MutexUnlock(m_pInitDecoderMapMutex);
	//在线信息
	(void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
	try
	{
		for(REMOTE_DECODER_MAP::iterator ite=m_onlineDecoderMap.begin();ite!=m_onlineDecoderMap.end();ite++)
		{
			CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoder = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(ite->second);//lint !e611
			IVS_DELETE(pDecoder);
		}
	}
	catch (...)
	{
	}
	(void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);
	//自动恢复
	(void)VOS_MutexLock(m_pAutoResumeMapMutex);
	try
	{
		for(AUTO_RESUME_MAP::iterator ite=m_autoResumeMap.begin();ite!=m_autoResumeMap.end();ite++)
		{
			AUTO_RESUME_PARAM* pAutoResume = dynamic_cast<AUTO_RESUME_PARAM*>(ite->second);//lint !e611
			IVS_DELETE(pAutoResume);
		}
	}
	catch (...)
	{
	}
	(void)VOS_MutexUnlock(m_pAutoResumeMapMutex);

	(void)VOS_DestroyMutex(m_pInitDecoderMapMutex);
	(void)VOS_DestroyMutex(m_pDecChMapMutex);
    (void)VOS_DestroyMutex(m_pOnlineDecoderMapMutex);
    (void)VOS_DestroyMutex(m_pAutoResumeMapMutex);
	(void)VOS_DestroyMutex(m_pCallBackMapMutex);
	(void)VOS_DestroyMutex(m_pHandleMutex);
	
	try
	{
		if(NULL != m_pStartDecoderMapMutex)
		{
			(void)VOS_DestroyMutex(m_pStartDecoderMapMutex);
			m_pStartDecoderMapMutex = NULL;
		}	
	}   
	catch(...)
	{//lint !e1775

	}

	try
	{
		if (NULL != m_pDecoderNameMutex)
		{
			(void)VOS_DestroyMutex(m_pDecoderNameMutex);
			m_pDecoderNameMutex = NULL;
		}
	}
	catch(...)
	{//lint !e1775

	}

    try
    {
        if (NULL != m_pIdleChMapMutex)
        {
            VOS_DestroyMutex(m_pIdleChMapMutex);
            m_pIdleChMapMutex = NULL;
        }
    }
    catch(...)
    {//lint !e1775
    }

	m_pInitDecoderMapMutex = NULL;
	m_pDecChMapMutex = NULL;
    m_pOnlineDecoderMapMutex = NULL;
    m_pAutoResumeMapMutex = NULL;
	m_pCallBackMapMutex = NULL;
    m_pHandleMutex = NULL;

	(void)VOS_MutexLock(m_pCapabilityVecMutex);
	try
	{
		m_CapabilityVec.clear();
	}
	catch(...)
	{
		(void)VOS_MutexUnlock(m_pCapabilityVecMutex);
	}
	(void)VOS_MutexUnlock(m_pCapabilityVecMutex);
	(void)VOS_DestroyMutex(m_pCapabilityVecMutex);
	m_pCapabilityVecMutex = NULL;
}

/**************************************************************************
* name       : InitTVWallService
* description: 初始化电视上墙模块
* input      : pParaInfo 初始化参数
* output     : void
* return     : long.       返回码
* remark     : NA
**************************************************************************/
IVS_INT32 CTVWallMgr::InitTVWall(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)

{
	IVS_DEBUG_TRACE("this=0x%p", this);
	CHECK_POINTER(pFunNotifyCallBack, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	(void)AddCallBack(pFunNotifyCallBack, pUserData);
	IVS_INT32 iRet = IVS_TVWall_Init(HandleNotifyCallBack, this);
	if(IVS_SUCCEED!=iRet)
	{
		return iRet;
	}
//////////////////////////////////////////////////////////////////////////
	//2013-1-24 高书明 临时屏蔽 迭代三不做
	//iRet = m_SDKDecoderMgr.InitSDK(HandleNotifyCallBack, this);
	//if(IVS_SUCCEED!=iRet)
	//{
	//	IVS_TVWall_Release();
	//	return iRet;
	//}
//////////////////////////////////////////////////////////////////////////

	
	iRet = InitHik();
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Init TVWall Hik Failed!", "");
		(void)IVS_TVWall_Release();
		return iRet;
	}
	
	if(!m_bTimerInit)
	{
	    m_timer.init(1000);
	    (void)m_timer.run();
	    //启动定时器 15S
	    m_timer.registerTimer(&m_AutoResumeTigger, NULL, 15, enRepeated);
	    m_bTimerInit = true;
	}
	return iRet;
}

/**************************************************************************
* name       : IVS_SDK_CleanupTVWall
* description: 释放电视上墙模块
* input      : NA
* output     : NA
* return     : RET_OK-成功     RET_FAIL-失败
* remark     : NA
**************************************************************************/
IVS_INT32 CTVWallMgr::CleanupTVWall()
{
	(void)VOS_MutexLock(m_pCallBackMapMutex);
	m_callbackMap.clear();
	(void)VOS_MutexUnlock(m_pCallBackMapMutex);

    (void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
	REMOTE_DECODER_MAP::iterator ite = m_onlineDecoderMap.begin();
	while(m_onlineDecoderMap.end()!=ite)
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoder = ite->second;
		ite = m_onlineDecoderMap.erase(ite);
		IVS_DELETE(pDecoder);
	}
	m_onlineDecoderMap.clear();
    (void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);
	//释放第三方解码器数据
	DecoderRelease();
	if(m_bTimerInit)
	{
		(void)m_timer.cancelTimer(&m_AutoResumeTigger);
		m_timer.clearTimer();
		m_timer.exit();
		m_bTimerInit = false;
	}
	IVS_INT32 iRet = IVS_TVWall_Release();
	//////////////////////////////////////////////////////////////////////////
	//2013-1-24 高书明 临时屏蔽 迭代三不做
	//if(iRet!=IVS_SUCCEED)
	//{
	//	(void)m_SDKDecoderMgr.CleanupSDK();
	//}
	//else
	//{
	//	iRet = m_SDKDecoderMgr.CleanupSDK();
	//}
	//////////////////////////////////////////////////////////////////////////
	return iRet;
}

IVS_INT32 CTVWallMgr::GetDecoderList(const IVS_INDEX_RANGE* pIndexRange,
										CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
										IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDecoderList, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("this=0x%p FromIndex=%u, ToIndex=%u, BufferSize=%u", 
					this, pIndexRange->uiFromIndex, pIndexRange->uiToIndex, uiBufferSize);

	REMOTE_DECODER_MAP stRemoteDecInfoMap;	// 解码器列表临时Map。先获取到map中，然后拷贝到用户内存中
	stRemoteDecInfoMap.clear();

	// 获取海康解码器列表
	(void)GetInfoFromHikDecoder(stRemoteDecInfoMap);
	// 获取海康解码器列表时开辟的内存空间头指针。用于回收该段内存
	CUMW_NOTIFY_REMOTE_DECODER_STATUS *pHidDecBufHead = NULL;	
	if (stRemoteDecInfoMap.size() > 0)
	{
		REMOTE_DECODER_MAP::iterator iterBeg = stRemoteDecInfoMap.begin();
		pHidDecBufHead = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(iterBeg->second);
	}

	// 获取华为解码器列表
	(void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
	IVS_UINT32 uiOnlineDecoderSize = m_onlineDecoderMap.size();
	IVS_CHAR* pstDecoder = IVS_NEW((IVS_CHAR*&)pstDecoder, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)*uiOnlineDecoderSize);
	if (NULL == pstDecoder)
	{
		(void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "new failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	eSDK_MEMSET(pstDecoder, 0, uiOnlineDecoderSize * sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));
	CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoderStatus = reinterpret_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(pstDecoder);//lint !e826

	IVS_UINT32 uiIndex = 0;
	REMOTE_DECODER_MAP::iterator BeginItor = m_onlineDecoderMap.begin();
	REMOTE_DECODER_MAP::iterator EndItor = m_onlineDecoderMap.end();
	for(; BeginItor != EndItor; ++BeginItor)
	{
		std::string stDecoderID = BeginItor->first;
		eSDK_MEMCPY(&pDecoderStatus[uiIndex], sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS), BeginItor->second, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));
		(void)stRemoteDecInfoMap.insert(std::make_pair(stDecoderID, &pDecoderStatus[uiIndex]));
		uiIndex++;
	}
	(void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);

	// 解码器总数
	pDecoderList->uiTotal = stRemoteDecInfoMap.size();
	//没有，返回成功
	if(0==pDecoderList->uiTotal)
	{
		pDecoderList->stIndexRange.uiFromIndex = 0;
		pDecoderList->stIndexRange.uiToIndex = 0;
		return IVS_SUCCEED;
	}
	if(pIndexRange->uiToIndex < pIndexRange->uiFromIndex ||
		pIndexRange->uiFromIndex > pDecoderList->uiTotal ||
		0==pIndexRange->uiFromIndex)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "IndexRange invalid");
		return IVS_PARA_INVALID;
	}
	pDecoderList->stIndexRange.uiFromIndex = pIndexRange->uiFromIndex;
	pDecoderList->stIndexRange.uiToIndex = 0;

	// 连一个解码器都放不下
	if(sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST) > uiBufferSize)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "Buffer is not enough");
		return IVS_PARA_INVALID;
	}

	IVS_UINT32 uiLen = sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST) - sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS);
	IVS_UINT32 uiNum = 0;
	REMOTE_DECODER_MAP::iterator ite = stRemoteDecInfoMap.begin();
	for(; ite!=stRemoteDecInfoMap.end(); ite++)
	{
		// 如果用户内存不够了，退出循环
		if(uiBufferSize < (uiLen + sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)))
		{
			break;
		}

		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoder = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(ite->second);
		if(NULL != pDecoder)
		{
			pDecoderList->stIndexRange.uiToIndex++;
			if(pDecoderList->stIndexRange.uiToIndex < pDecoderList->stIndexRange.uiFromIndex)
			{
				continue;
			}
			if(!CToolsHelp::Memcpy(&pDecoderList->stDecoderList[uiNum], 
									sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS), pDecoder, 
									sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder List", "Memcpy Failed");
				IVS_DELETE(pDecoderStatus, MUILI);
				IVS_DELETE(pHidDecBufHead, MUILI);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			uiNum++;
			uiLen += sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS);
			
			if(pDecoderList->stIndexRange.uiToIndex>=pIndexRange->uiToIndex)
			{
				break;
			}
		}
	}

	IVS_DELETE(pDecoderStatus, MUILI);
	IVS_DELETE(pHidDecBufHead, MUILI);
	BP_DBG_LOG("CTVWallMgr::GetDecoderList Total=%u, FromIndex=%u, ToIndex=%u", 
				pDecoderList->uiTotal,
				pDecoderList->stIndexRange.uiFromIndex, 
				pDecoderList->stIndexRange.uiToIndex);
	return IVS_SUCCEED;
}

IVS_INT32 CTVWallMgr::StartRealPlayTVWall(const IVS_CHAR* pCameraCode,
											const IVS_REALPLAY_PARAM* pRealplayParam,
											const IVS_TVWALL_PARAM* pTVWallParam,
											IVS_BOOL bAutoResume)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRealplayParam, IVS_PARA_INVALID);
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	BP_DBG_LOG("TVWall Start Realplay this=0x%p, CameraCode=%s, DecoderID=%s, Channel=%u, StreamType=%u, \
				ProtocolType=%u, DirectFirst=%d, MultiCast=%d, AutoResume=%s", 
				this, pCameraCode, pTVWallParam->cDecoderCode, pTVWallParam->uiChannel,
				pRealplayParam->uiStreamType, pRealplayParam->uiProtocolType,
				pRealplayParam->bDirectFirst, pRealplayParam->bMultiCast, 
				bAutoResume ? "True":"False");

	if(0==strnlen(pTVWallParam->cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) /*|| pTVWallParam->uiChannel>65435*/)
	{
		BP_RUN_LOG_ERR(IVS_CODE_INVALID, "TVWall Start Realplay", "TVWallParam Invalid");
		return IVS_CODE_INVALID;
	}
	if(pRealplayParam->uiStreamType>STREAM_TYPE_SUB2
		|| pRealplayParam->uiProtocolType<1
		|| pRealplayParam->uiProtocolType>2
		|| pRealplayParam->bDirectFirst<0
		|| pRealplayParam->bDirectFirst>1
		|| pRealplayParam->bMultiCast)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Start Realplay", "RealPlayParam Invalid");
		return IVS_PARA_INVALID;
	}

	//非自动恢复调用该函数，将相同解码器通道的自动恢复移除
	if(!bAutoResume)
	{
		//std::string key = MakeKey(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
		(void)RemoveAutoResume(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	}

	// 检查接入解码器的有效性
	IVS_INT32 iRet = CheckDecoderEx(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start Realplay", "failed");
		return iRet;
	}
	CDecoderChannel* pWnd = GetDecoderChannel(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	CHECK_POINTER(pWnd, IVS_ALLOC_MEMORY_ERROR);
    //////////////////////////////////////同步////////////////////////////////////
	pWnd->SetUserMgr(m_pUserMgr);
    iRet = pWnd->StartRealPlay(pCameraCode, *pRealplayParam, *pTVWallParam);
    //////////////////////////////////////异步////////////////////////////////////
	//CHECK_POINTER(pWnd, IVS_SDK_RET_INTRINSIC_PTR_ERROR);
 //   CRealPlayThread* pRealPlay = NULL;//该类可以自删除
 //   if(NULL==IVS_NEW(pRealPlay))
 //   {
 //       return IVS_ALLOC_MEMORY_ERROR;
 //   }
 //   pRealPlay->SetDecoderChannel(pWnd);
 //   pRealPlay->SetCameraCode(pCameraCode);
 //   pRealPlay->SetTVWallParam(pTVWallParam);
 //   pRealPlay->SetRealPlayParam(pRealplayParam);
 //   iRet = pRealPlay->run();
    //////////////////////////////////////////////////////////////////////////
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start Realplay", "failed, Decoder=%s, Channel=%u", 
							pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	}
	else
	{
        (void)VOS_MutexLock(m_pHandleMutex);
		m_mapPlayChannel.insert(std::make_pair(pWnd->GetHandle(),pWnd));//lint !e534
        (void)VOS_MutexUnlock(m_pHandleMutex);
	}

	pWnd->SetSwitchPlay(false);//lint !e730
	return iRet;
}

IVS_INT32 CTVWallMgr::SwitchRealPlayTVWall(const IVS_CHAR* pCameraCode,
	const IVS_REALPLAY_PARAM* pRealplayParam,
	const IVS_TVWALL_PARAM* pTVWallParam,
	IVS_BOOL bAutoResume)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRealplayParam, IVS_PARA_INVALID);
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	BP_DBG_LOG("TVWall Switch Realplay this=0x%p, CameraCode=%s, DecoderID=%s, Channel=%u, StreamType=%u, \
			   ProtocolType=%u, DirectFirst=%d, MultiCast=%d, AutoResume=%s", 
			   this, pCameraCode, pTVWallParam->cDecoderCode, pTVWallParam->uiChannel,
			   pRealplayParam->uiStreamType, pRealplayParam->uiProtocolType,
			   pRealplayParam->bDirectFirst, pRealplayParam->bMultiCast, 
			   bAutoResume ? "True":"False");

	if(0==strnlen(pTVWallParam->cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || pTVWallParam->uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_CODE_INVALID, "TVWall Switch Realplay", "TVWallParam Invalid");
		return IVS_CODE_INVALID;
	}
	if(pRealplayParam->uiStreamType>STREAM_TYPE_SUB2
		|| pRealplayParam->uiProtocolType<1
		|| pRealplayParam->uiProtocolType>2
		|| pRealplayParam->bDirectFirst<0
		|| pRealplayParam->bDirectFirst>1
		|| pRealplayParam->bMultiCast)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Switch Realplay", "RealPlayParam Invalid");
		return IVS_PARA_INVALID;
	}

	if (!IsCapabilityVecExist(pTVWallParam->cDecoderCode))
	{
		BP_RUN_LOG_ERR(IVS_TVWALL_DECODER_NO_CAPABILITY, "TVWall Switch Realplay", "Decoder : %s does not have capability", pTVWallParam->cDecoderCode);
		return IVS_TVWALL_DECODER_NO_CAPABILITY;
	}

	//非自动恢复调用该函数，将相同解码器通道的自动恢复移除
	if(!bAutoResume)
	{
		(void)RemoveAutoResume(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	}

	CDecoderChannel* pWnd = GetDecoderChannel(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	CHECK_POINTER(pWnd, IVS_ALLOC_MEMORY_ERROR);
	//////////////////////////////////////同步////////////////////////////////////
	IVS_INT32 iRet = pWnd->SwitchRealPlay(pCameraCode, *pRealplayParam, *pTVWallParam);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Switch Realplay", "failed, Decoder=%s, Channel=%u", 
			pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	}
	else
	{
		//设置切换前CDecoderChannel的标识为被切换

		(void)VOS_MutexLock(m_pHandleMutex);
		m_mapPlayChannel.insert(std::make_pair(pWnd->GetHandle(),pWnd));//lint !e534
		(void)VOS_MutexUnlock(m_pHandleMutex);
	}

	pWnd->SetSwitchPlay(true);//lint !e730
	return iRet;
}

IVS_INT32 CTVWallMgr::StopRealPlayTVWall(const IVS_TVWALL_PARAM* pTVWallParam)
{
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	BP_DBG_LOG("TVWall Stop Realplay DecoderID=%s, Channel=%u", 
			   pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);

	if(0==strnlen(pTVWallParam->cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN)/* || pTVWallParam->uiChannel>65435*/)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Stop Realplay", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}

    //处理有自动重连的情况.(在关闭自动重连后删除m_mapPlayChannel中相应句柄)
    if (RemoveAutoResume(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel))
    {
        BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall: This wnd have auto resume, stop it.");
        return IVS_SUCCEED;
    }

    IVS_INT32 iRet = IVS_FAIL;
	iRet = CheckDecContType(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);//lint !e838
	switch ((CUMW_DECODER_CONNECT_TYPE)iRet)
	{
		case CUMW_DECODER_CONNECT_HIK:
			{
				CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(GetDecoderSDKHandle().GetDecoder(CUMW_DECODER_CONNECT_HIK));//lint !e1705
				CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
				pHikDecoderSDK->m_bHikDecPlay = false;
				IVS_LONG lSessionID = -1;
				CUMW_DECODER_CHANNEL_INFO stDecoderChannelInfo;
				eSDK_MEMSET(&stDecoderChannelInfo, 0, sizeof(CUMW_DECODER_CHANNEL_INFO));
				iRet = GetDecoderChannelInfo(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel, lSessionID, stDecoderChannelInfo);
				if(iRet != IVS_SUCCEED)
				{
					BP_RUN_LOG_ERR(IVS_FAIL, "Stop Play Req", "get channel info failed!");
					return IVS_FAIL;
				}
				iRet = pHikDecoderSDK->StopPlayURL(lSessionID, &stDecoderChannelInfo);
				if(iRet != IVS_SUCCEED)
				{
					BP_RUN_LOG_ERR(IVS_FAIL, "Stop Play Req", "get channel info failed!");
					return IVS_FAIL;
				}
			}
			break;
		case CUMW_DECODER_CONNECT_HW:
			{
				CDecoderChannel* pWnd = GetDecoderChannel(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
				if(NULL != pWnd)
				{
					BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall: to stop.");
					iRet = pWnd->StopPlay();
					BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall: stop end.");
				}
				if(IVS_SUCCEED!=iRet && IVS_SDK_NOT_PLAY_TVWALL != iRet)
				{
					BP_RUN_LOG_ERR(iRet, "TVWall Stop Realplay", "Realplay stop failed, Decoder=%s, Channel=%u", 
						pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
				}
				else
				{
					if (NULL != pWnd)
					{
						BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall: to erase.");
						(void)VOS_MutexLock(m_pHandleMutex);
						m_mapPlayChannel.erase(pWnd->GetHandle());
						(void)VOS_MutexUnlock(m_pHandleMutex);
						BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall: erase end.");
					}
				}
			}
			break;
		default:
			{
				BP_RUN_LOG_ERR(IVS_FAIL,"Start Play Req", "unknown connect type");
				iRet = IVS_FAIL;
			}
			break;
			
	}
    BP_DBG_LOG("CTVWallMgr::StopRealPlayTVWall:  end.");
	return iRet;
}

IVS_INT32 CTVWallMgr::StartPlayBackTVWall(const IVS_CHAR* pCameraCode,
                                                    const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                    const IVS_TVWALL_PARAM* pTVWallParam, 
													REPLAY_TYPE iReplayType,
													const IVS_CHAR* pDomainCode,
													const IVS_CHAR* pNVRCode)
{
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
    CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
    CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	BP_DBG_LOG("TVWall Start PlayBack DecoderID=%s, Channel=%d, ProtocolType=%u, \
			   StartTime=%s, EndTime=%s, Speed=%0.1f", 
			   pTVWallParam->cDecoderCode, pTVWallParam->uiChannel,
			   pPlaybackParam->uiProtocolType, pPlaybackParam->stTimeSpan.cStart,
			   pPlaybackParam->stTimeSpan.cEnd, pPlaybackParam->fSpeed);

	if(0==strnlen(pTVWallParam->cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || pTVWallParam->uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Start PlayBack", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	if(pPlaybackParam->uiProtocolType<1
		|| pPlaybackParam->uiProtocolType>2)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Start PlayBack", "PlaybackParam Invalid");
		return IVS_PARA_INVALID;
	}

    //如果是前端回放，查找录像文件名称
    IVS_RECORD_INFO stRecordInfo;
    eSDK_MEMSET(&stRecordInfo, 0, sizeof(stRecordInfo));
    if (REPLAY_TYPE_PU == iReplayType)
    {
        IVS_INT32 iRet = GetPUPlayBackFileName(pCameraCode, &pPlaybackParam->stTimeSpan, stRecordInfo);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start PU Play Back TVWALL",
                "Get PU playBack fileName failed with %d.", iRet);
            return iRet;
        }

        BP_DBG_LOG("StartPlayBackTVWall: Get PU playBack fileName %s", stRecordInfo.cRecordFileName);
    }

	IVS_INT32 iRet = CheckDecoder(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start PlayBack", "failed");
		return iRet;
	}
    CDecoderChannel* pWnd = GetDecoderChannel(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	CHECK_POINTER(pWnd, IVS_ALLOC_MEMORY_ERROR);
	pWnd->SetUserMgr(m_pUserMgr);
    iRet = pWnd->StartPlayBack(pCameraCode, *pPlaybackParam, *pTVWallParam, iReplayType,pDomainCode,pNVRCode,stRecordInfo.cRecordFileName);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "TVWall Start PlayBack", "failed, Decoder=%s, Channel=%u", 
            pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
    }
	else
	{
        (void)VOS_MutexLock(m_pHandleMutex);
		(void)m_mapPlayChannel.insert(std::make_pair(pWnd->GetHandle(),pWnd));
        (void)VOS_MutexUnlock(m_pHandleMutex);
	}
	pWnd->SetSwitchPlay(false);//lint !e730
    return iRet;
}

IVS_INT32 CTVWallMgr::GetPUPlayBackFileName(const char* pCameraCode, const IVS_TIME_SPAN* pTimeSpan, IVS_RECORD_INFO &stRecordInfo)
{
    if (NULL == pCameraCode || NULL == pTimeSpan)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU PlayBack FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pUserMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU PlayBack FileName", "CheckParam failed.");
        return IVS_PARA_INVALID;
    }

    eSDK_MEMSET(&stRecordInfo,0,sizeof(stRecordInfo));

    IVS_INDEX_RANGE stIndexRange;
    stIndexRange.uiFromIndex = 1;
    stIndexRange.uiToIndex = 1;
    int iSize = sizeof(IVS_RECORD_INFO_LIST) + (stIndexRange.uiToIndex-stIndexRange.uiFromIndex) * sizeof(IVS_RECORD_INFO);   //lint !e737 !e713

    char *pBuff = IVS_NEW((char* &)pBuff, (unsigned long)iSize);
    if (NULL == pBuff)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get PU PlayBack FileName", "New Mem Failed.");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    eSDK_MEMSET(pBuff, 0, iSize);   //lint !e732

    CRecordList& recordList = m_pUserMgr->GetRecordList();//lint !e1013 !e1055 !e1058 !e1514 !e64
    int iRet = recordList.GetRecordList(pCameraCode, 
        RECORD_TYPE_PU,
        pTimeSpan,
        &stIndexRange,
        (IVS_RECORD_INFO_LIST*)pBuff,
        (IVS_UINT32)iSize);   //lint !e826

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get PU PlayBack FileName", "Get Record List failed.");
    }
    else
    {
        IVS_RECORD_INFO_LIST *pRecordList = (IVS_RECORD_INFO_LIST*)pBuff;   //lint !e826
        (void)CToolsHelp::Memcpy(&stRecordInfo, sizeof(stRecordInfo),
            pRecordList->stRecordInfo, sizeof(IVS_RECORD_INFO));
    }

    IVS_DELETE(pBuff, MUILI);
    return iRet;
}

IVS_INT32 CTVWallMgr::StopPlayBackTVWall(const IVS_TVWALL_PARAM* pTVWallParam)
{
    CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	BP_DBG_LOG("TVWall Stop PlayBack.DecoderID=%s, Channel=%d", 
		pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	if(0==strnlen(pTVWallParam->cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || pTVWallParam->uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Stop PlayBack", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
    CDecoderChannel* pWnd = GetDecoderChannel(pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
    IVS_INT32 iRet = IVS_FAIL;
    if(NULL != pWnd)
    {
        iRet = pWnd->StopPlay();
    }
    if(IVS_SUCCEED!=iRet && IVS_SDK_NOT_PLAY_TVWALL != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "TVWall Stop PlayBack", "failed, Decoder=%s, Channel=%u", 
            pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
    }
	else
	{
		if(NULL != pWnd)
		{
            (void)VOS_MutexLock(m_pHandleMutex);
			m_mapPlayChannel.erase(pWnd->GetHandle());
            (void)VOS_MutexUnlock(m_pHandleMutex);
		}
	}
    return iRet;
}


IVS_INT32 CTVWallMgr::PlayBackPauseTVWall(const IVS_TVWALL_PARAM& stTVWallParam)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Pause Playback", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	CDecoderChannel* pChannel = GetDecoderChannel(MakeKey(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel));
	IVS_INT32 iRet = IVS_PARA_INVALID;
	if(NULL != pChannel)
	{
		iRet = pChannel->PausePlayback();
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Pause Playback", "failed, Decoder=%s, Channel=%u", 
			stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}

IVS_INT32 CTVWallMgr::PlayBackResumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Resume Playback", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	CDecoderChannel* pChannel = GetDecoderChannel(MakeKey(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel));
	IVS_INT32 iRet = IVS_PARA_INVALID;
	if(NULL != pChannel)
	{
		iRet = pChannel->ResumePlayback();
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Resume Playback", "failed, Decoder=%s, Channel=%u", 
			stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}


// 单帧快进
IVS_INT32 CTVWallMgr::PlaybackFrameStepForwardTVWall(const IVS_TVWALL_PARAM& stTVWallParam)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Frame Step", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	CDecoderChannel* pChannel = GetDecoderChannel(MakeKey(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel));
	IVS_INT32 iRet = IVS_PARA_INVALID;
	if(NULL != pChannel)
	{
		iRet = pChannel->PlaybackFrameStepForward();
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Frame Step", "failed, Decoder=%s, Channel=%u", 
						stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}

IVS_INT32 CTVWallMgr::PlaySoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Start Sound", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	CDecoderChannel* pChannel = GetDecoderChannel(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	IVS_INT32 iRet = IVS_FAIL;
	if(NULL != pChannel)
	{
		iRet = pChannel->StartSound();
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Start Sound", "failed, Decoder=%s, Channel=%u", 
						stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}

IVS_INT32 CTVWallMgr::StopSoundTVWall(const IVS_TVWALL_PARAM& stTVWallParam)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Stop Sound", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	CDecoderChannel* pChannel = GetDecoderChannel(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	IVS_INT32 iRet = IVS_FAIL;
	if(NULL != pChannel)
	{
		iRet = pChannel->StopSound();
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Stop Sound", "failed, Decoder=%s, Channel=%u", 
			stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}

IVS_INT32 CTVWallMgr::SetVolumeTVWall(const IVS_TVWALL_PARAM& stTVWallParam, IVS_UINT32 uiVolumeValue)
{
	if(0==strnlen(stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN) || stTVWallParam.uiChannel>65435)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Stop Sound", "TVWallParam Invalid");
		return IVS_PARA_INVALID;
	}
	if(uiVolumeValue>100)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "TVWall Stop Sound", "Volume(%u) Invalid", uiVolumeValue);
		return IVS_PARA_INVALID;
	}

	CDecoderChannel* pChannel = GetDecoderChannel(stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	IVS_INT32 iRet = IVS_FAIL;
	if(NULL != pChannel)
	{
		iRet = pChannel->SetVolume(uiVolumeValue);
	}
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "TVWall Stop Sound", "failed, Decoder=%s, Channel=%u", 
			stTVWallParam.cDecoderCode, stTVWallParam.uiChannel);
	}
	return iRet;
}

IVS_INT32 CTVWallMgr::AddCallBack(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
	CHECK_POINTER(pFunNotifyCallBack, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	CLockGuard autoLock(m_pCallBackMapMutex);
	IVS_UINT64 key = (IVS_UINT64)pUserData;
	TVWALL_CALLBACK_MAP::iterator ite = m_callbackMap.find(key);
	if(m_callbackMap.end()!=ite)
	{
		ite->second.pCallBackFunc = pFunNotifyCallBack;
	}
	else
	{
		TVWALL_CALLBACK_INFO info = {0};
		info.pCallBackFunc = pFunNotifyCallBack;
		info.pUserData = pUserData;
		m_callbackMap.insert(TVWALL_CALLBACK_MAP::value_type(key, info));
	}
	return IVS_SUCCEED;
}

IVS_INT32 CTVWallMgr::RemoveCallBack(void* pUserData)
{
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	CLockGuard autoLock(m_pCallBackMapMutex);
	IVS_UINT64 key = (IVS_UINT64)pUserData;
	m_callbackMap.erase(key);
	return IVS_SUCCEED;
}


void CTVWallMgr::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr =  pUserMgr;
}

//获取解码器通道初始化信息
IVS_INT32 CTVWallMgr::GetDecoderInitInfo(IVS_ULONG ulWndID, CUMW_NOTIFY_INIT_DECODER_INFO* pInitDecoderInfo)
{
	CHECK_POINTER(pInitDecoderInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	(void)VOS_MutexLock(m_pInitDecoderMapMutex);
	REMOTE_DECODER_INIT_INFO_MAP::iterator ite = m_remoteDecodeInitInfoMap.find(ulWndID);
	if(ite!=m_remoteDecodeInitInfoMap.end())
	{
		CUMW_NOTIFY_INIT_DECODER_INFO* pInitInfo = dynamic_cast<CUMW_NOTIFY_INIT_DECODER_INFO*>(ite->second);//lint !e611
		if(NULL!=pInitInfo)
		{
			if(CToolsHelp::Memcpy(pInitDecoderInfo, sizeof(CUMW_NOTIFY_INIT_DECODER_INFO), pInitInfo, sizeof(CUMW_NOTIFY_INIT_DECODER_INFO)))//lint !e64
			{
				iRet = IVS_SUCCEED;
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder InitInfo", "Memcpy Failed");
				iRet = IVS_ALLOC_MEMORY_ERROR;
			}

            m_remoteDecodeInitInfoMap.erase(ulWndID);
            IVS_DELETE(pInitInfo);
		}
		else
		{
			m_remoteDecodeInitInfoMap.erase(ulWndID);
			BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder InitInfo", "failed");
		}
	}
	(void)VOS_MutexUnlock(m_pInitDecoderMapMutex);
	return iRet;
}

void CTVWallMgr::AddDecoderInitInfo(IVS_ULONG ulWndID, const CUMW_NOTIFY_INIT_DECODER_INFO& initDecoderInfo)
{
	CUMW_NOTIFY_INIT_DECODER_INFO* pInitInfo = IVS_NEW(pInitInfo);
	if(NULL==pInitInfo)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Decoder InitInfo", "Create Mem Failed");
		return;
	}
	if(!CToolsHelp::Memcpy(pInitInfo, sizeof(CUMW_NOTIFY_INIT_DECODER_INFO), &initDecoderInfo, sizeof(CUMW_NOTIFY_INIT_DECODER_INFO)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Decoder InitInfo", "Memcpy Failed");
		return ;
	}
	(void)VOS_MutexLock(m_pInitDecoderMapMutex);
    REMOTE_DECODER_INIT_INFO_MAP::iterator ite = m_remoteDecodeInitInfoMap.find(ulWndID);
    if(ite!=m_remoteDecodeInitInfoMap.end())
    {
		CUMW_NOTIFY_INIT_DECODER_INFO* pTmp = dynamic_cast<CUMW_NOTIFY_INIT_DECODER_INFO*>(ite->second);//lint !e611
        m_remoteDecodeInitInfoMap.erase(ulWndID);
		IVS_DELETE(pTmp);
    }
	m_remoteDecodeInitInfoMap.insert(REMOTE_DECODER_INIT_INFO_MAP::value_type(ulWndID, pInitInfo));
	(void)VOS_MutexUnlock(m_pInitDecoderMapMutex);
}

//删除指定窗格的解码器通道初始化信息
void CTVWallMgr::DeleteDecoderInitInfo(IVS_ULONG ulWndID)
{
	(void)VOS_MutexLock(m_pInitDecoderMapMutex);
	REMOTE_DECODER_INIT_INFO_MAP::iterator ite = m_remoteDecodeInitInfoMap.find(ulWndID);
	if(ite!=m_remoteDecodeInitInfoMap.end())
	{
		CUMW_NOTIFY_INIT_DECODER_INFO* pTmp = dynamic_cast<CUMW_NOTIFY_INIT_DECODER_INFO*>(ite->second);//lint !e611
		m_remoteDecodeInitInfoMap.erase(ulWndID);
		IVS_DELETE(pTmp);
	}
	(void)VOS_MutexUnlock(m_pInitDecoderMapMutex);
}

//自动恢复处理
void CTVWallMgr::HandleAutoResume()
{
    typedef std::map<std::string, AUTO_RESUME_PARAM> TEMP_AUTO_RESUME_MAP;//自动恢复参数MAP
    TEMP_AUTO_RESUME_MAP TempMap;

    //复制一份重连map的数据
    (void)VOS_MutexLock(m_pAutoResumeMapMutex);

    BP_DBG_LOG("Enter CTVWallMgr::HandleAutoResume(): m_autoResumeMap size = %d.", m_autoResumeMap.size());

    AUTO_RESUME_MAP::iterator it = m_autoResumeMap.begin();
    while(m_autoResumeMap.end() != it)
    {
        AUTO_RESUME_PARAM* pResumeParam = it->second;
        if(NULL==pResumeParam)
        {
            m_autoResumeMap.erase(it++);
            continue;
        }

        AUTO_RESUME_PARAM stParam = {0};
        eSDK_MEMCPY((void*)&stParam, sizeof(stParam), (void*)pResumeParam, sizeof(AUTO_RESUME_PARAM));

        TempMap.insert(TEMP_AUTO_RESUME_MAP::value_type(it->first, stParam));

        it++;
    }

    (void)VOS_MutexUnlock(m_pAutoResumeMapMutex);


    TEMP_AUTO_RESUME_MAP::iterator ite = TempMap.begin();
    while(TempMap.end()!=ite)
    {
        AUTO_RESUME_PARAM ResumeParam = ite->second;

        //检查m_autoResumeMap里是否还有这个重连任务，没有的话就不进行重连了
        if (!IsAutoResumeParaExist(MakeKey(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel)))
        {
            BP_DBG_LOG("CTVWallMgr::HandleAutoResume: no para exist, not need go on. cDecoderCode = %s, uiChannel = %d, szCameraCode = %s.", 
                ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel, ResumeParam.szCameraCode);

            ite++;
            continue;
        }

        if(!ResumeParam.bAutoResuming)
        {
            //先判断解码器是否在线
            if(!IsOnline(ResumeParam.stTVWallParam.cDecoderCode))
            {//解码器不在线，跳过

                BP_DBG_LOG("CTVWallMgr::HandleAutoResume: Decoder offline. cDecoderCode = %s.", 
                    ResumeParam.stTVWallParam.cDecoderCode);

                ite++;
                continue;
            }
            CDecoderChannel* pCh = GetDecoderChannel(MakeKey(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel));
            if(NULL != pCh)
            {
                const IVS_CHAR* pCameraCode = pCh->GetCameraCode();
                if(pCh->IsPlaying() && strncmp(const_cast<IVS_CHAR *>(pCameraCode), ResumeParam.szCameraCode, IVS_DEV_CODE_LEN)!=0)
                {//正在播放，且摄像头非同一个，说明已经重新上墙，自动恢复取消
     
                    BP_DBG_LOG("CTVWallMgr::HandleAutoResume: is play new. cDecoderCode = %s, uiChannel = %d, szCameraCode = %s.", 
                        ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel, ResumeParam.szCameraCode);

                    DelAutoResumePara(MakeKey(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel));
                    ite++;

                    continue;
                }
                if(pCh->IsStarting())
                {//正在启动，不需要恢复
     
                    BP_DBG_LOG("CTVWallMgr::HandleAutoResume: is starting. cDecoderCode = %s, uiChannel = %d, szCameraCode = %s.", 
                        ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel, ResumeParam.szCameraCode);

                    DelAutoResumePara(MakeKey(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel));
                    ite++;

                    continue;
                }
            }

            //需要恢复
            if(PLAY_TYPE_REAL==ResumeParam.iPlayType)
            {
                //上报正在自动恢复事件
                ReportAutoResumeEvnet(&ResumeParam);

                //这个CDecoderChannel对象是刚刚创建的,在启动上墙过程中肯定不可能去关闭之前的RTSP,所以需要特意关闭RTSP.
                if (NULL == pCh)
                {
                    BP_DBG_LOG("CTVWallMgr::HandleAutoResume: szScuUrl = %s, iHandle = %d iPlayType = %d.",
                        ResumeParam.szScuUrl, ResumeParam.iHandle, ResumeParam.iPlayType);

                    pCh = GetDecoderChannel(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel);
                    if(NULL != pCh)
                    {
                        pCh->m_wndConnectInfo.iPlayType = ResumeParam.iPlayType;

                        eSDK_MEMSET(pCh->m_wndConnectInfo.szScuUrl, 0, URL_LEN+1);
                        (void)CToolsHelp::Strncpy(pCh->m_wndConnectInfo.szScuUrl, URL_LEN+1, ResumeParam.szScuUrl, strlen(ResumeParam.szScuUrl));
                        pCh->m_wndConnectInfo.iHandle = ResumeParam.iHandle;

                        (void)pCh->StopPlay();//lint !e830

                        pCh->m_wndStatus = WND_STATUS_IDLE;
                        pCh->m_wndConnectInfo.iPlayType = PLAY_TYPE_NONE;

                        (void)VOS_MutexLock(m_pHandleMutex);
                        m_mapPlayChannel.erase(pCh->GetHandle());
                        (void)VOS_MutexUnlock(m_pHandleMutex);
                    }//lint !e529
                }

                IVS_INT32 iRet = AutoResumeEx(&ResumeParam);
                if(IVS_SUCCEED==iRet)
                {//自动恢复成功，删除
                    //上报自动恢复成功事件
                    /*ReportAutoResumeEvnet(&ResumeParam, IVS_SUCCEED, FALSE);
                    BP_DBG_LOG("TVWall AutoResume Realplay Success！CameraCode=%s, DecoderID=%s, Channel=%u", 
                        ResumeParam.szCameraCode, ResumeParam.stTVWallParam.cDecoderCode, 
                        ResumeParam.stTVWallParam.uiChannel);

                    DelAutoResumePara(MakeKey(ResumeParam.stTVWallParam.cDecoderCode, ResumeParam.stTVWallParam.uiChannel));*/
                    ite++;

                    continue;
                }
                else
                {
                    BP_RUN_LOG_ERR(iRet, "TVWall AutoResume Realplay", "Failed！CameraCode=%s", ResumeParam.szCameraCode);
                }
            }
            else
            {//回放不需要恢复，删除
                AUTO_RESUME_PARAM Temp = ite->second;

                DelAutoResumePara(MakeKey(Temp.stTVWallParam.cDecoderCode, Temp.stTVWallParam.uiChannel));
                ite++;

                BP_DBG_LOG("TVWall AutoResume Playback Remove！");
                continue;
            }
        }
        ite++;
    }

    BP_DBG_LOG("Out CTVWallMgr::HandleAutoResume()");
}

//自动恢复
IVS_INT32 CTVWallMgr::AutoResumeEx(const AUTO_RESUME_PARAM* pResumeParam)
{
    BP_DBG_LOG("Enter AutoResumeEx");

    CHECK_POINTER(pResumeParam, IVS_PARA_INVALID);

    AUTO_RESUME_TREAD_PARAM* pPara = IVS_NEW(pPara);
    CHECK_POINTER(pPara, IVS_ALLOC_MEMORY_ERROR);
    eSDK_MEMSET(pPara, 0, sizeof(AUTO_RESUME_TREAD_PARAM));

    pPara->pTVWallMgr = this;
    eSDK_MEMCPY((void*)&pPara->stAutoResumePara, sizeof(pPara->stAutoResumePara), static_cast<void *>(const_cast<AUTO_RESUME_PARAM *>(pResumeParam)), sizeof(AUTO_RESUME_PARAM));

    int nRet = (int)_beginthread(AutoResumeThreadFun, 0, pPara);
    if (-1 == nRet) //-1为失败，其他值成功
    {
        IVS_DELETE(pPara);

        BP_RUN_LOG_ERR(IVS_FAIL, "start real play error, create thread fail!","");
        return IVS_FAIL;
    }

    BP_DBG_LOG("Leave AutoResumeEx");
    return IVS_SUCCEED;
}

//开始自动恢复线程函数
void __cdecl CTVWallMgr::AutoResumeThreadFun(LPVOID lpvoid)
{
    BP_DBG_LOG("Enter StartRealPlayTVWallThreadFun");
    CHECK_POINTER_VOID(lpvoid);

    AUTO_RESUME_TREAD_PARAM* pRealPlay = (AUTO_RESUME_TREAD_PARAM*)lpvoid;

    AUTO_RESUME_TREAD_PARAM stRealPlay;
    eSDK_MEMSET(&stRealPlay, 0, sizeof(AUTO_RESUME_TREAD_PARAM));
    eSDK_MEMCPY(&stRealPlay, sizeof(stRealPlay), pRealPlay, sizeof(AUTO_RESUME_TREAD_PARAM));

    IVS_DELETE(pRealPlay);
    //pRealPlay = NULL;

    IVS_INT32 iRet = stRealPlay.pTVWallMgr->StartRealPlayTVWall(stRealPlay.stAutoResumePara.szCameraCode, 
        &stRealPlay.stAutoResumePara.stRealplayParam, &stRealPlay.stAutoResumePara.stTVWallParam, TRUE);
    if(IVS_SUCCEED == iRet)
    {//自动恢复成功，删除
        //上报自动恢复成功事件
        stRealPlay.pTVWallMgr->ReportAutoResumeEvnet(&stRealPlay.stAutoResumePara, IVS_SUCCEED, FALSE);
        BP_DBG_LOG("TVWall AutoResume Realplay Success！CameraCode=%s, DecoderID=%s, Channel=%u", 
            stRealPlay.stAutoResumePara.szCameraCode, stRealPlay.stAutoResumePara.stTVWallParam.cDecoderCode, 
            stRealPlay.stAutoResumePara.stTVWallParam.uiChannel);

        stRealPlay.pTVWallMgr->DelAutoResumePara(stRealPlay.pTVWallMgr->MakeKey(stRealPlay.stAutoResumePara.stTVWallParam.cDecoderCode, stRealPlay.stAutoResumePara.stTVWallParam.uiChannel));
    }

    BP_DBG_LOG("Leave StartRealPlayTVWallThreadFun");
    _endthread();
}

bool CTVWallMgr::IsAutoResumeParaExist(const std::string& key)
{
    bool bHave = false;

    (void)VOS_MutexLock(m_pAutoResumeMapMutex);

    AUTO_RESUME_MAP::iterator ite = m_autoResumeMap.find(key);
    if(ite != m_autoResumeMap.end())
    {
        bHave = true;
    }

    (void)VOS_MutexUnlock(m_pAutoResumeMapMutex);

    return bHave;
}

void CTVWallMgr::DelAutoResumePara(const std::string& key)
{
    (void)VOS_MutexLock(m_pAutoResumeMapMutex);

    AUTO_RESUME_MAP::iterator ite = m_autoResumeMap.find(key);
    if(ite != m_autoResumeMap.end())
    {
        AUTO_RESUME_PARAM* pTemp = ite->second;
        m_autoResumeMap.erase(ite);
        if (NULL != pTemp)
        {
            IVS_DELETE(pTemp);
        }
    }

    (void)VOS_MutexUnlock(m_pAutoResumeMapMutex);
}

//异步操作返回通知
void CTVWallMgr::NotifyAsyncReturn(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet)
{
    CUMW_NOTIFY_INFO stNotifyInfo = {0};
    stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_ASYNC_RET;
    stNotifyInfo.pNotifyInfo = static_cast<void*>(const_cast<CUMW_NOTIFY_ASYNC_RET_EX *>(&stAsyncRet));
    stNotifyInfo.ulNotifyInfoLen = sizeof(stAsyncRet);
    NotifyEvent(&stNotifyInfo);
}

void CTVWallMgr::NotifyEvent(CUMW_NOTIFY_INFO* pstNotifyInfo)
{
	// 规避死锁问题;
	//CLockGuard autoLock(m_pCallBackMapMutex);
	for(TVWALL_CALLBACK_MAP::iterator ite=m_callbackMap.begin();ite!=m_callbackMap.end();ite++)
	{
		if(NULL!=ite->second.pCallBackFunc && NULL!=ite->second.pUserData)
		{
			(void)ite->second.pCallBackFunc(pstNotifyInfo, ite->second.pUserData);
		}
	}
}

//电视墙回调函数
IVS_INT32 __stdcall CTVWallMgr::HandleNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData)
{
	CHECK_POINTER(pstNotifyInfo,IVS_TVWALL_MEMORY_NULL);

	CTVWallMgr* pTVWallMgr = (CTVWallMgr*)pUserData;
	CHECK_POINTER(pTVWallMgr, IVS_TVWALL_MEMORY_NULL);

    BP_DBG_LOG("CTVWallMgr::HandleNotifyCallBack, NotifyType=0x%04X, User Address=0x%p", 
        pstNotifyInfo->ulNotifyType, pUserData);

	switch (pstNotifyInfo->ulNotifyType)
	{
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER:
		{
			CUMW_NOTIFY_REMOTE_DECODER_STATUS* pstInfo	= (CUMW_NOTIFY_REMOTE_DECODER_STATUS*)pstNotifyInfo->pNotifyInfo;

			CUMW_NOTIFY_REMOTE_DECODER_STATUS stRegInfo = {0};
			if(!CToolsHelp::Memcpy(&stRegInfo, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS), pstInfo, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TVWall CallBack", "Memcpy RegInfo failed");
			}
			pTVWallMgr->AddRemoteDecoder(stRegInfo);
			pTVWallMgr->NotifyEvent(pstNotifyInfo);
		}
		break;
	case CUMW_NOTIFY_TYPE_INIT_DECODER:
		{
			pTVWallMgr->InitDecoderNotify(pstNotifyInfo, pUserData);
		}
		break;
	case CUMW_NOTIFY_TYPE_START_DECODER:
	case CUMW_NOTIFY_TYPE_STOP_DECODER:
	case CUMW_NOTIFY_TYPE_AUDIO_CONTROL:             //播放/停止播放音频消息通知
	case CUMW_NOTIFY_TYPE_SET_VOLUME:                //音量调节消息通知
		{

		}
		break;
	case CUMW_NOTIFY_TYPE_DECODER_ERROR:
		{
			CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO* pstErrInfo = (CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO*)pstNotifyInfo->pNotifyInfo; 
			BP_RUN_LOG_INF("TVWall CallBack", "Decoder=%s Report Error, errCode=%lu, errDesc=%s",
							pstErrInfo->szDecoderID, pstErrInfo->stErrorCode.ulErrorCode, 
							pstErrInfo->stErrorCode.szErrorDescription);
			if(0x2==pstErrInfo->stErrorCode.ulErrorCode)
			{//断流
				(void)pTVWallMgr->DecoderStreamError(pstErrInfo->szDecoderID, pstErrInfo->usChannelNo);
				pTVWallMgr->NotifyEvent(pstNotifyInfo);
				CDecoderChannel* pCh = pTVWallMgr->GetDecoderChannel(pstErrInfo->szDecoderID, pstErrInfo->usChannelNo);
				if(NULL!=pCh)
				{//加入到自动恢复中
					//pTVWallMgr->AddAutoResume(pCh, CUMW_BREAKDOWN_REASON_NO_VIDEO);
                    pTVWallMgr->DecoderStreamError(pCh, CUMW_BREAKDOWN_REASON_NO_VIDEO);
				}
			}
		}
        break;
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER:
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT:
		{
            CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstInitInfo = (CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO*)pstNotifyInfo->pNotifyInfo;
            pTVWallMgr->RemoveRemoteDecoder(pstInitInfo->szDecoderID);
            pTVWallMgr->NotifyEvent(pstNotifyInfo);
		}
		break;
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_CAPABILITY:
		{
			CUMW_NOTIFY_TYPE_DECODER_CAPABILITY_INFO* pstCapabilityInfo = reinterpret_cast<CUMW_NOTIFY_TYPE_DECODER_CAPABILITY_INFO *>(pstNotifyInfo->pNotifyInfo);
			if (!pTVWallMgr->IsCapabilityVecExist(pstCapabilityInfo->szDecoderID))
			{
				pTVWallMgr->PushBackCapabilityVec(pstCapabilityInfo->szDecoderID);
				BP_DBG_LOG("CTVWallMgr::HandleNotifyCallBack,Capability Report, DecoderID=%s", pstCapabilityInfo->szDecoderID);
			}
			pTVWallMgr->NotifyEvent(pstNotifyInfo);
		}
		break;
	default:
		{
			BP_RUN_LOG_INF("TVWall CallBack", "Unsupported NotifyType(0x%04x)", pstNotifyInfo->ulNotifyType);
		}
		break;
	}

	return IVS_SUCCEED;
}//lint !e818

void CTVWallMgr::DecoderStreamError(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON emReason)
{
    CHECK_POINTER_VOID(pCh);

    TVWALL_ADD_AUTO_RESUME_INFO* pstInfo = IVS_NEW(pstInfo);
    CHECK_POINTER_VOID(pstInfo);
    eSDK_MEMSET(pstInfo, 0, sizeof(TVWALL_ADD_AUTO_RESUME_INFO));

    pstInfo->pCh = pCh;
    pstInfo->emReason = emReason;
    pstInfo->pTVWallMgr = this;

    IVS_INT32 iRet = (IVS_INT32)_beginthread(AddAutoResumeThread, 0, pstInfo);
    if (-1 == iRet) //-1为失败，其他值成功
    {
        IVS_DELETE(pstInfo);
        BP_RUN_LOG_ERR(iRet, "start real play error, create thread fail!", "");
        return;
    }

    return;
}

void __cdecl CTVWallMgr::AddAutoResumeThread(LPVOID lpvoid)
{
    CHECK_POINTER_VOID(lpvoid);

    TVWALL_ADD_AUTO_RESUME_INFO* pstInfo = (TVWALL_ADD_AUTO_RESUME_INFO*)lpvoid;
    CHECK_POINTER_VOID(pstInfo->pCh);
    CHECK_POINTER_VOID(pstInfo->pTVWallMgr);

    pstInfo->pTVWallMgr->AddAutoResume(pstInfo->pCh, pstInfo->emReason);

    IVS_DELETE(pstInfo);
}

//处理初始化解码器通道的返回数据
void CTVWallMgr::InitDecoderNotify(const CUMW_NOTIFY_INFO* pstNotifyInfo, const void* pUserData)
{
	if (NULL == pstNotifyInfo )
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "InitDecoder Notify", "input pstNotifyInfo is NULL!")
		return;
	}

	CUMW_NOTIFY_INIT_DECODER_INFO* pstInfo
		= (CUMW_NOTIFY_INIT_DECODER_INFO*)pstNotifyInfo->pNotifyInfo;

    AddDecoderInitInfo(pstInfo->ulAsyncSeq, *pstInfo);

	if (0==pstInfo->stErrorCode.ulErrorCode)
	{       
		//AddDecoderInitInfo(pstInfo->ulAsyncSeq, *pstInfo);
	}
	else
	{//初始化失败，通知停止上墙
        BP_DBG_LOG("CTVWallMgr::InitDecoderNotify:  Init failed. ulErrorCode = %d, ulNotifyType = %d, szErrorDescription = %s.",
            pstInfo->stErrorCode.ulErrorCode, pstNotifyInfo->ulNotifyType, pstInfo->stErrorCode.szErrorDescription);
		//CDecoderChannel* pCh = GetDecoderChannel(pstInfo->ulAsyncSeq);
		//CHECK_POINTER_VOID(pCh);
		//(void)pCh->StopPlay();
	}

	return;
}//lint !e715

CDecoderChannel* CTVWallMgr::GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel)
{
    BP_DBG_LOG("Enter CTVWallMgr::GetDecoderChannel");

	CUMW_DECODER_TYPE decType = CUMW_DECODER_TYPE_SDK;
	(void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
	REMOTE_DECODER_MAP::iterator iteInfo=m_onlineDecoderMap.find(pDecoderID);
	if(iteInfo!=m_onlineDecoderMap.end())
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pInfo = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(iteInfo->second);
		if(NULL!=pInfo)
		{
			decType = (CUMW_DECODER_TYPE)pInfo->ucDecoderType;
		}
	}
	(void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);

    std::string key = MakeKey(pDecoderID, uiChannel);
	(void)VOS_MutexLock(m_pDecChMapMutex);
	TVWALL_DEC_CH_MAP::iterator ite = m_tvwallDecChMap.find(key);
	CDecoderChannel* pDecCh  = NULL;
	if(ite!=m_tvwallDecChMap.end())
	{
		pDecCh = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
	}
	if(NULL==pDecCh)
	{
		try
		{
            IVS_ULONG ulAsyncSeq = m_ulAsyncSeq++;
			if(CUMW_DECODER_TYPE_RDA==decType)
			{
                BP_DBG_LOG("CTVWallMgr::GetDecoderChannel: create CRDAChannel object.");
				pDecCh = new CRDAChannel(ulAsyncSeq, pDecoderID, uiChannel, this);
				m_tvwallDecChMap.insert(TVWALL_DEC_CH_MAP::value_type(key, pDecCh));
			}
			else if(CUMW_DECODER_TYPE_SDK==decType)
			{
                BP_DBG_LOG("CTVWallMgr::GetDecoderChannel: create CSDKChannel object.");
				pDecCh = new CSDKChannel(ulAsyncSeq, pDecoderID, uiChannel, this);
				m_tvwallDecChMap.insert(TVWALL_DEC_CH_MAP::value_type(key, pDecCh));
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Channel", "unknown Decoder Type=%d", decType);
			}
		}
		catch(...)
		{
			pDecCh = NULL;
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder Channel", "new failed");
		}
	}
	(void)VOS_MutexUnlock(m_pDecChMapMutex);

    BP_DBG_LOG("Out CTVWallMgr::GetDecoderChannel");

	return pDecCh;
}

CDecoderChannel* CTVWallMgr::GetDecoderChannel(const std::string& key)
{
    (void)VOS_MutexLock(m_pDecChMapMutex);
    TVWALL_DEC_CH_MAP::iterator ite = m_tvwallDecChMap.find(key);
    CDecoderChannel* pWnd  = NULL;;
    if(ite!=m_tvwallDecChMap.end())
    {
        pWnd = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
    }
    (void)VOS_MutexUnlock(m_pDecChMapMutex);
    return pWnd;
}

CDecoderChannel* CTVWallMgr::GetDecoderChannel(IVS_ULONG ulAsyncSeq)
{
	(void)VOS_MutexLock(m_pDecChMapMutex);
	CDecoderChannel* pCh = NULL;
	for(TVWALL_DEC_CH_MAP::iterator ite=m_tvwallDecChMap.begin();ite!=m_tvwallDecChMap.end();ite++)
	{
        pCh = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
		if(pCh!= NULL && pCh->GetAsyncSeq()==ulAsyncSeq)
		{
			break;
		}
        pCh = NULL;
	}
	(void)VOS_MutexUnlock(m_pDecChMapMutex);
	return pCh;
}


std::string CTVWallMgr::MakeKey(const char* pDecoderID, IVS_UINT32 uiChannel) const
{
	std::string str = pDecoderID;
	str += "#";
	str += CToolsHelp::Int2Str(static_cast<int>(uiChannel));
	return str; //lint !e1036
}

//增加一个解码器信息
void CTVWallMgr::AddRemoteDecoder(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& remoteDecoder)
{
	CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoder = IVS_NEW(pDecoder);
	if(NULL==pDecoder)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Remote Decoder", "new failed!");
		return;
	}
	if(!CToolsHelp::Memcpy(pDecoder, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS), &remoteDecoder, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add Remote Decoder", "Memcpy failed!");
		return ;
	}
    (void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
    std::string key = remoteDecoder.szDecoderID;
    REMOTE_DECODER_MAP::iterator ite = m_onlineDecoderMap.find(key);
    if(ite!=m_onlineDecoderMap.end())
    {
		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pTemp = ite->second;
        m_onlineDecoderMap.erase(ite);
		IVS_DELETE(pTemp);
    }
    m_onlineDecoderMap.insert(REMOTE_DECODER_MAP::value_type(key, pDecoder));
    (void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);
}

//移除一个解码器
void CTVWallMgr::RemoveRemoteDecoder(const IVS_CHAR* remoteDecoderID)
{
    BP_DBG_LOG("Enter CTVWallMgr::RemoveRemoteDecoder");

    IVS_CHAR decoderId[100] = {0};
    if(!CToolsHelp::Strncpy(decoderId, sizeof(decoderId), remoteDecoderID, CUMW_DECODER_ID_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Remove Remote Decoder Copy DecoderID", "failed");
		return;
	}
	//清除在线信息
    (void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
    std::string key = decoderId;
	REMOTE_DECODER_MAP::iterator iteOnline = m_onlineDecoderMap.find(key);
	if(iteOnline!=m_onlineDecoderMap.end())
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pTemp = iteOnline->second;
		m_onlineDecoderMap.erase(iteOnline);
		IVS_DELETE(pTemp);
	}
    (void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);

	//清除初始化信息
	(void)VOS_MutexLock(m_pInitDecoderMapMutex);
	REMOTE_DECODER_INIT_INFO_MAP::iterator iteInit = m_remoteDecodeInitInfoMap.begin();
	while(iteInit!=m_remoteDecodeInitInfoMap.end())
	{
		CUMW_NOTIFY_INIT_DECODER_INFO* pTemp = dynamic_cast<CUMW_NOTIFY_INIT_DECODER_INFO*>(iteInit->second);//lint !e611
		if(NULL!=pTemp && strcmp(pTemp->szDecoderID, decoderId)==0)
		{
			iteInit = m_remoteDecodeInitInfoMap.erase(iteInit);
			IVS_DELETE(pTemp);
			continue;
		}
		iteInit++;
	}
	(void)VOS_MutexUnlock(m_pInitDecoderMapMutex);

    //解码器掉线，将正在播放的放到自动恢复队列中
    (void)VOS_MutexLock(m_pDecChMapMutex);
    TVWALL_DEC_CH_MAP::iterator ite=m_tvwallDecChMap.begin();
    while(ite!=m_tvwallDecChMap.end())
    {
        CDecoderChannel* pCh = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
        if(NULL!=pCh && strcmp(pCh->GetDecoderID(), decoderId)==0)
        {
			AddAutoResume(pCh, CUMW_BREAKDOWN_REASON_DECODER_OFFLINE); //DTS2013051708819
            ite = m_tvwallDecChMap.erase(ite);
			//add by zwx211831，需要删除实况列表中的对应节点。因为该pCh内存五分钟后会被释放
			IVS_ULONG ulHandle = pCh->GetHandle();
			(void)VOS_MutexLock(m_pHandleMutex);
			CHANNEL_MAP_ITER iter = m_mapPlayChannel.find(ulHandle);
			if(iter != m_mapPlayChannel.end())
			{
				m_mapPlayChannel.erase(iter);
				BP_RUN_LOG_INF("Play Channel Map erase, "," play handle is %d", ulHandle);
			}
			(void)VOS_MutexUnlock(m_pHandleMutex);
			//end
            AddToIdleList(pCh);//IVS_DELETE(pCh);
            continue;
        }
        ite++;
    }
    (void)VOS_MutexUnlock(m_pDecChMapMutex);

    BP_DBG_LOG("Out CTVWallMgr::RemoveRemoteDecoder");
}

//增加自动恢复
void  CTVWallMgr::AddAutoResume(CDecoderChannel* pCh, CUMW_BREAKDOWN_REASON iReason)
{
    BP_DBG_LOG("Enter CTVWallMgr::AddAutoResume");

	if(NULL!=pCh && pCh->IsPlaying())
	{//如果在播放过程中解码器掉线，上线后需要自动恢复播放

        std::string key = MakeKey(pCh->GetDecoderID(), pCh->GetChannel());

        CLockGuard autoLock(m_pAutoResumeMapMutex);

        AUTO_RESUME_MAP::iterator ite = m_autoResumeMap.find(key);
        if(m_autoResumeMap.end() != ite)
        {
            BP_DBG_LOG("CTVWallMgr::AddAutoResume: Have same key(%s) in resume list, not need add.", key.c_str());
            return;
        }

        BP_DBG_LOG("CTVWallMgr::AddAutoResume: Create resume data, key is %s.", key.c_str());

		AUTO_RESUME_PARAM* pResumeParam = IVS_NEW(pResumeParam);
		if(NULL!=pResumeParam)
		{
            eSDK_MEMSET(pResumeParam, 0, sizeof(AUTO_RESUME_PARAM));

			pResumeParam->iReson = iReason;
			pCh->SetException(true);
			pCh->GetAutoResumeParam(pResumeParam);
			m_autoResumeMap.insert(AUTO_RESUME_MAP::value_type(key, pResumeParam));
		}
	}
}

//检查是否有自动重连数据。有，关闭RTSP，返回true;无,返回false.
bool CTVWallMgr::RemoveAutoResume(const char* szDecoderCode, unsigned int uiChannel)
{
    BP_DBG_LOG("Enter CTVWallMgr::RemoveAutoResume");

    if (NULL == szDecoderCode)
    {
        return false;
    }

    std::string key = MakeKey(szDecoderCode, uiChannel);

    CLockGuard autoLock(m_pAutoResumeMapMutex);

    BP_DBG_LOG("CTVWallMgr::RemoveAutoResume: m_autoResumeMap size = %d.", m_autoResumeMap.size());

    AUTO_RESUME_MAP::iterator ite = m_autoResumeMap.find(key);
    if(m_autoResumeMap.end()!=ite)
    {
        AUTO_RESUME_PARAM* pResumeParam = ite->second;
        m_autoResumeMap.erase(ite);
        if(NULL!=pResumeParam)
        {
            CDecoderChannel* pWnd = GetDecoderChannel(szDecoderCode, uiChannel);
            //IVS_INT32 iRet = IVS_FAIL;//lint !e830
            if(NULL != pWnd)
            {
                BP_DBG_LOG("CTVWallMgr::RemoveAutoResume: szScuUrl = %s, iHandle = %d iPlayType = %d.",
                    pResumeParam->szScuUrl, pResumeParam->iHandle, pResumeParam->iPlayType);

                pWnd->m_wndConnectInfo.iPlayType = pResumeParam->iPlayType;

                eSDK_MEMSET(pWnd->m_wndConnectInfo.szScuUrl, 0, URL_LEN+1);
                (void)CToolsHelp::Strncpy(pWnd->m_wndConnectInfo.szScuUrl, URL_LEN+1, pResumeParam->szScuUrl, strlen(pResumeParam->szScuUrl));
                pWnd->m_wndConnectInfo.iHandle = pResumeParam->iHandle;

                (void)pWnd->StopPlay();

                pWnd->m_wndStatus = WND_STATUS_IDLE;
                pWnd->m_wndConnectInfo.iPlayType = PLAY_TYPE_NONE;

                (void)VOS_MutexLock(m_pHandleMutex);
                m_mapPlayChannel.erase(pWnd->GetHandle());
                (void)VOS_MutexUnlock(m_pHandleMutex);
            }
            else
            {
                BP_DBG_LOG("CTVWallMgr::RemoveAutoResume: Get decoder channel failed.szScuUrl = %s, iHandle = %d iPlayType = %d.",
                    pResumeParam->szScuUrl, pResumeParam->iHandle, pResumeParam->iPlayType);
            }

            IVS_DELETE(pResumeParam);

            return true;
        }//lint !e550
    }

    BP_DBG_LOG("Out CTVWallMgr::RemoveAutoResume");
    return false;
}

IVS_INT32 CTVWallMgr::GetURL(const char *pszAuthHeader, ST_URL_MEDIA_INFO& stUrlMediaInfo, ST_MEDIA_RSP& stMediaRsp)
{
	bool bAudio;
	return CMediaBase::GetURL(pszAuthHeader, m_pUserMgr, stUrlMediaInfo, stMediaRsp, bAudio);
}

//查询指定的解码器是否在线
bool CTVWallMgr::IsOnline(const char* pDecoderID)
{
    bool bFind = false;
    std::string strID = pDecoderID;
    (void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
    REMOTE_DECODER_MAP::iterator ite=m_onlineDecoderMap.find(strID);
    if(ite!=m_onlineDecoderMap.end())
    {
        bFind = true;
    }
    (void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);
    return bFind;
}

IVS_INT32 CTVWallMgr::CheckDecoder(const char* pDecoderID, IVS_UINT32 uiChannel)
{
	bool bOnline = false;
	bool bFind = false;
	std::string strID = pDecoderID;
	CLockGuard autoLock(m_pOnlineDecoderMapMutex);
	REMOTE_DECODER_MAP::iterator ite=m_onlineDecoderMap.find(strID);
	if(ite!=m_onlineDecoderMap.end())
	{
		bOnline = true;
		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoder = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(ite->second);
		if(NULL!=pDecoder)
		{
			for(IVS_ULONG i=0;i<pDecoder->ulChannelNum;i++)
			{
				if(pDecoder->stChannelInfoArray[i].ulChannelID==uiChannel)
				{
					bFind = true;
					break;
				}
			}
		}
	}
	if(!bOnline)
	{
		return IVS_TVWALL_DECODER_OFFLINE;
	}
	if(!bFind)
	{
		return IVS_TVWALL_DECODER_NO_CHANNEL;
	}
	return IVS_SUCCEED;
}

IVS_INT32 CTVWallMgr::DecoderStreamError(const char* pDecoderID, IVS_UINT32 uiChannel)
{
	CDecoderChannel* pCh = GetDecoderChannel(MakeKey(pDecoderID, uiChannel));
	if(NULL!=pCh)
	{
		pCh->SetException(true);
	}
	return IVS_SUCCEED;
}

void CTVWallMgr::ReportAutoResumeEvnet(const AUTO_RESUME_PARAM* pResumeParam, IVS_INT32 iErrCode, IVS_BOOL bStartResume)
{
	if(NULL==pResumeParam)
	{
		return;
	}
	CUMW_NOTIFY_INFO stNotifyInfo = {0};
	CUMW_NOTIFY_ASYNC_RET_EX stAsyncInfo = {0};
	stNotifyInfo.pNotifyInfo = &stAsyncInfo;
	stNotifyInfo.ulNotifyInfoLen = sizeof(CUMW_NOTIFY_ASYNC_RET_EX);
	if(bStartResume)
	{
		stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO;
	}
	else
	{
		stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_AUTO_RESUME_RES;
	}
	stAsyncInfo.iRetCode = iErrCode;
	bool bCopy = CToolsHelp::Strncpy(stAsyncInfo.szCameraCode, sizeof(stAsyncInfo.szCameraCode), pResumeParam->szCameraCode, IVS_DEV_CODE_LEN);
	if(!bCopy)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TVWall AutoResume", "Copy CameraCode failed");
		return;
	}
	bCopy = CToolsHelp::Strncpy(stAsyncInfo.szDecoderID, sizeof(stAsyncInfo.szDecoderID), pResumeParam->stTVWallParam.cDecoderCode, IVS_TVWALL_DECODER_CODE_LEN);
	if(!bCopy)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TVWall AutoResume", "Copy DecoderID failed");
		return;
	}
	stAsyncInfo.uiChannel = pResumeParam->stTVWallParam.uiChannel;
	stAsyncInfo.iReason = pResumeParam->iReson;
	NotifyEvent(&stNotifyInfo);
}

CDecoderChannel* CTVWallMgr::GetChannel(IVS_ULONG ulRtspHandle)
{
    (void)VOS_MutexLock(m_pHandleMutex);
	CHANNEL_MAP_ITER iter = m_mapPlayChannel.find(ulRtspHandle);
	if(iter == m_mapPlayChannel.end())
	{
        (void)VOS_MutexUnlock(m_pHandleMutex);
		return NULL;
	}

	CDecoderChannel* pChannel = iter->second;
    (void)VOS_MutexUnlock(m_pHandleMutex);
	return pChannel;	
}

IVS_ULONG CTVWallMgr::GetTVWallHandlebyRtspHandle(IVS_ULONG ulRtspHandle)
{
    (void)VOS_MutexLock(m_pHandleMutex);
	CHANNEL_MAP_ITER iter = m_mapPlayChannel.find(ulRtspHandle);
	if(iter == m_mapPlayChannel.end())
	{
        (void)VOS_MutexUnlock(m_pHandleMutex);
		return 0;
	}

	IVS_ULONG ulhandle = iter->first;
    (void)VOS_MutexUnlock(m_pHandleMutex);
	return ulhandle;
}

void CTVWallMgr::AddToIdleList(CDecoderChannel* pDecoderChannel)
{
    BP_DBG_LOG("Enter CTVWallMgr::AddToIdleList");

    if (NULL == pDecoderChannel)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add To Idle List", "Pointer Is NULL.");
        return;
    }

    CLockGuard autoLock(m_pIdleChMapMutex);

    TVWALL_IDLE_CH_MAP::iterator ite = m_IdleChMap.begin();
    for (; ite != m_IdleChMap.end(); ite++)
    {
        CDecoderChannel* pTemp = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
        if (NULL == pTemp)
        {
            continue;
        }

        if (pDecoderChannel == pTemp)
        {
            BP_DBG_LOG("Out CTVWallMgr::AddToIdleList: Find same object.");
            break;
        }
    }

    if(ite != m_IdleChMap.end())
    {
        BP_DBG_LOG("Out CTVWallMgr::AddToIdleList, already has decoderchannel[%lu]", pDecoderChannel);
        return;
    }

    m_IdleChMap.insert(std::make_pair(GetCurrentTime(), pDecoderChannel));//lint !e534

    BP_DBG_LOG("Out CTVWallMgr::AddToIdleList");
}

void CTVWallMgr::DelIdleCh()
{
    BP_DBG_LOG("Enter CTVWallMgr::DelIdleCh");

    time_t tCurTime = GetCurrentTime();

    CLockGuard autoLock(m_pIdleChMapMutex);

    TVWALL_IDLE_CH_MAP::iterator ite = m_IdleChMap.begin();
    while (ite != m_IdleChMap.end()) 
    {
        CDecoderChannel* pTemp = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
        if (NULL == pTemp)
        {
            ite = m_IdleChMap.erase(ite);
            continue;
        }

        time_t tInterval = tCurTime - ite->first;
        if ((tInterval < -(5 * 60))  ||  (tInterval > (5 * 60))) //时间间隔为5分钟
        {
			//Free掉该Channel下的所有RTSP通道
			pTemp->FreeAllRTSPHandle();
            ite = m_IdleChMap.erase(ite);
            IVS_DELETE(pTemp);
            continue;
        }

        ite++;
    }

    BP_DBG_LOG("Out CTVWallMgr::DelIdleCh");
}

//取得当前系统时间，以秒为单位
time_t CTVWallMgr::GetCurrentTime() const
{
    time_t tCurrentTime = 0;
    (void)BP_OS::time(&tCurrentTime);

    return tCurrentTime;
}

void CTVWallMgr::DeleteChannel(IVS_ULONG ulHandle)
{
	(void)VOS_MutexLock(m_pHandleMutex);
	m_mapPlayChannel.erase(ulHandle); 
	(void)VOS_MutexUnlock(m_pHandleMutex);
}

IVS_INT32 CTVWallMgr::SetPrePlaySwitched(const char* pDecoderID, IVS_UINT32 uiChannel)
{
	IVS_DEBUG_TRACE("DecoderID: %s, Channel: %u", pDecoderID, uiChannel);

	std::string key = MakeKey(pDecoderID, uiChannel);
	(void)VOS_MutexLock(m_pDecChMapMutex);
	TVWALL_DEC_CH_MAP::iterator ite = m_tvwallDecChMap.find(key);
	CDecoderChannel* pDecCh  = NULL;
	if(ite!=m_tvwallDecChMap.end())
	{
		pDecCh = dynamic_cast<CDecoderChannel*>(ite->second);
	}
	(void)VOS_MutexUnlock(m_pDecChMapMutex);
	CHECK_POINTER(pDecCh, IVS_ALLOC_MEMORY_ERROR);

	return IVS_SUCCEED;
}

bool CTVWallMgr::IsCapabilityVecExist(const std::string& key)
{
	bool bHave = false;

	(void)VOS_MutexLock(m_pCapabilityVecMutex);

	TVWALL_CAPABILITY_VEC::iterator iter = m_CapabilityVec.begin();
	TVWALL_CAPABILITY_VEC::iterator iterEnd = m_CapabilityVec.end();
	for (; iter != iterEnd; iter++)
	{
		if (key == *iter)
		{
			bHave = true;
			break;
		}
	}

	(void)VOS_MutexUnlock(m_pCapabilityVecMutex);

	return bHave;
}


void CTVWallMgr::PushBackCapabilityVec(const std::string& key)
{
	(void)VOS_MutexLock(m_pCapabilityVecMutex);
	m_CapabilityVec.push_back(key);
	(void)VOS_MutexUnlock(m_pCapabilityVecMutex);

	return;
}

IVS_INT32 CTVWallMgr::InitHik()
{
	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK);
	CHECK_POINTER(pDecoderSDK, IVS_PARA_INVALID);
	int iRet = pDecoderSDK->InitHik(HandleSdkCallBack, this);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"InitHik","failed");
		return iRet;
	}

	return iRet;
}


IVS_INT32 CTVWallMgr::AddDecoder(DECODER_DEV *pDecoder)
{
	BP_DBG_LOG("Enter CTVWallMgr::AddDecoder: DecoderID= %s.", (pDecoder)->cDecoderID);

	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); //现在暂时默认
	CHECK_POINTER(pDecoderSDK, IVS_PARA_INVALID);

	IVS_LONG lSession = 0;
	int iRet = pDecoderSDK->Login(lSession,pDecoder,pDecoder->cPassword);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"AddDecoder","Login SDK failed");
		return iRet;
	}

	iRet = pDecoderSDK->GetChannels(lSession,pDecoder);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"AddDecoder","Get channels failed");
		return iRet;
	}

	BP_DBG_LOG("Out CTVWallMgr::AddDecoder");

	return IVS_SUCCEED;
}//lint !e1762


IVS_INT32 CTVWallMgr::DelDecoder(const IVS_CHAR* pszDecoderID)
{
	CHECK_POINTER(pszDecoderID, IVS_PARA_INVALID);

	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); //现在暂时默认
	CHECK_POINTER(pDecoderSDK, IVS_PARA_INVALID);

	IVS_LONG lUserID = 0;
	IVS_INT32 lRes = pDecoderSDK->GetUserIDbyDecorderID(pszDecoderID, lUserID);
	if (IVS_SUCCEED != lRes)
	{
		BP_RUN_LOG_ERR(lRes, "StartPlay fail, Can't Find UserID.", "");
		return lRes;
	}

	return pDecoderSDK->Logout(lUserID);
}//lint !e1762


IVS_INT32 CTVWallMgr::ValidateDecoder(DECODER_DEV *pDecoder)
{
	BP_DBG_LOG("CTVWallMgr::ValidateDecoder: DecoderID = %s, cUser = %s, iPort = %d, cDevIP = %s.",
		pDecoder->cDecoderID, pDecoder->cUser, pDecoder->iPort, pDecoder->cDevIP);

	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); //现在暂时默认
	CHECK_POINTER(pDecoderSDK, IVS_PARA_INVALID);

	(void)InitHik();

	IVS_LONG lSession = 0;
	int iRet = pDecoderSDK->Login(lSession,pDecoder,pDecoder->cPassword);
	eSDK_MEMSET(pDecoder->cPassword,0,IVS_PWD_LEN+1);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_INF("Validate decoder","Login SDK failed,%d",iRet);
	}else
	{
		iRet = pDecoderSDK->Logout(lSession);
	}
	return iRet;
}


bool CTVWallMgr::IsHikDecOnline(const char* pszDecoderID)
{
	CHECK_POINTER(pszDecoderID, false);

	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); //现在暂时默认
	CHECK_POINTER(pDecoderSDK, false);

	return pDecoderSDK->IsHikDecOnline(pszDecoderID);
}//lint !e1762


//SDK方式接入的解码器的回调函数
IVS_INT32 __stdcall CTVWallMgr::HandleSdkCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData)
{
	CHECK_POINTER(pstNotifyInfo,IVS_TVWALL_MEMORY_NULL);

	CTVWallMgr* pTVWallMgr = (CTVWallMgr*)pUserData; //lint !e1924
	CHECK_POINTER(pTVWallMgr, IVS_TVWALL_MEMORY_NULL);

	BP_DBG_LOG("CTVWallMgr::HandleSdkCallBack, NotifyType=0x%04X, User Address=0x%p", 
		pstNotifyInfo->ulNotifyType, pUserData);

	switch (pstNotifyInfo->ulNotifyType)
	{
	case CUMW_NOTIFY_TYPE_DECODER_ERROR:
		{
			CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO_EX* pstErrInfo = (CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO_EX*)pstNotifyInfo->pNotifyInfo;  //lint !e1924

			BP_RUN_LOG_INF("TVWall Sdk CallBack", "Decoder=%s Report Error, errCode=%lu.",
				pstErrInfo->szDecoderID, pstErrInfo->stErrorCode.ulErrorCode);

			if(0x02 == pstErrInfo->stErrorCode.ulErrorCode) //断流
			{
				(void)pTVWallMgr->DecoderStreamError(pstErrInfo->szDecoderID, pstErrInfo->usChannelNo);
				pTVWallMgr->NotifyEvent(pstNotifyInfo);
				CDecoderChannel* pCh = pTVWallMgr->GetDecoderChannel(pstErrInfo->szDecoderID, pstErrInfo->usChannelNo, 1);
				if(NULL != pCh)
				{
					BP_DBG_LOG("CTVWallMgr::HandleSdkCallBack, pCh=0x%p", pCh);

					//加入到自动恢复中
					pTVWallMgr->DecoderStreamError(pCh, CUMW_BREAKDOWN_REASON_NO_VIDEO);
				}
			}
		}
		break;
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER:
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT:
		{
			//const CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstInitInfo = (CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO*)pstNotifyInfo->pNotifyInfo; //lint !e1924 !e529
			pTVWallMgr->NotifyEvent(pstNotifyInfo);
		}
		break;
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_RESUMECONNECT:
		{
			//const CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstInitInfo = (CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO*)pstNotifyInfo->pNotifyInfo; //lint !e1924 !e529
			pTVWallMgr->NotifyEvent(pstNotifyInfo);
		}
		break;
	default:
		{
			BP_RUN_LOG_INF("TVWall CallBack", "Unsupported NotifyType(0x%04x)", pstNotifyInfo->ulNotifyType);
		}
		break;
	}

	return IVS_SUCCEED;
}


CDecoderChannel* CTVWallMgr::GetDecoderChannel(const char* pDecoderID, IVS_UINT32 uiChannel, IVS_BOOL bSDKConnect)
{
	BP_DBG_LOG("Enter CTVWallMgr::GetDecoderChannel");
	CUMW_DECODER_TYPE decType = CUMW_DECODER_TYPE_RDA;
	if (1 == bSDKConnect)
	{
		decType = CUMW_DECODER_TYPE_SDK;
	}

	(void)VOS_MutexLock(m_pOnlineDecoderMapMutex);
	REMOTE_DECODER_MAP::iterator iteInfo=m_onlineDecoderMap.find(pDecoderID);
	if(iteInfo!=m_onlineDecoderMap.end())
	{
		const CUMW_NOTIFY_REMOTE_DECODER_STATUS* pInfo = dynamic_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(iteInfo->second);
		if(NULL!=pInfo)
		{ 
			decType = (CUMW_DECODER_TYPE)pInfo->ucDecoderType; //lint !e1924
		}
	}
	(void)VOS_MutexUnlock(m_pOnlineDecoderMapMutex);

	std::string key = MakeKey(pDecoderID, uiChannel);
	(void)VOS_MutexLock(m_pDecChMapMutex);
	TVWALL_DEC_CH_MAP::iterator ite = m_tvwallDecChMap.find(key);
	CDecoderChannel* pDecCh  = NULL;;
	if(ite!=m_tvwallDecChMap.end())
	{
		pDecCh = dynamic_cast<CDecoderChannel*>(ite->second);//lint !e611
	}
	if(NULL==pDecCh)
	{
		try
		{
			IVS_ULONG ulAsyncSeq = m_ulAsyncSeq++;
			if(CUMW_DECODER_TYPE_RDA==decType)
			{
				BP_DBG_LOG("CTVWallMgr::GetDecoderChannel: create CRDAChannel object.");
				pDecCh = new CRDAChannel(ulAsyncSeq, pDecoderID, uiChannel, this);
				m_tvwallDecChMap.insert(TVWALL_DEC_CH_MAP::value_type(key, pDecCh));
			}
			else if(CUMW_DECODER_TYPE_SDK==decType)
			{
				BP_DBG_LOG("CTVWallMgr::GetDecoderChannel: create CSDKChannel object.");
				pDecCh = new CSDKChannel(ulAsyncSeq, pDecoderID, uiChannel, this);
				m_tvwallDecChMap.insert(TVWALL_DEC_CH_MAP::value_type(key, pDecCh));
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Channel", "unknown Decoder Type=%d", decType);
			}
		}
		catch(...)
		{
			pDecCh = NULL;
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Decoder Channel", "new failed");
		}
	}
	(void)VOS_MutexUnlock(m_pDecChMapMutex);
	BP_DBG_LOG("Out CTVWallMgr::GetDecoderChannel");

	return pDecCh;
}



IVS_INT32 CTVWallMgr::AddChannel(const PORTS_CONFIG& stPortCfgs, std::list<unsigned short>& listTotalChannel)const
{
	for (unsigned int i = 0; i < stPortCfgs.usPortNum; i++)
	{
		for (unsigned int j = 0; j < stPortCfgs.stPortConfig[i].stChannelsInfo.usChannelNum; j++)
		{
			unsigned short usChannelNo = stPortCfgs.stPortConfig[i].stChannelsInfo.stChannelInfo[j].usChannelNo;
			std::list<unsigned short>::iterator iter = find(listTotalChannel.begin(), listTotalChannel.end(), usChannelNo);//lint !e864
			if(iter == listTotalChannel.end())
			{
				listTotalChannel.push_back(usChannelNo);
				continue;
			}
			BP_RUN_LOG_ERR(IVS_SDK_HAVE_SAME_CHANNEL, "Add Channel Failed!", "Have Same Channel %d.", usChannelNo);
			return IVS_SDK_HAVE_SAME_CHANNEL; //lint !e40
		}
	}

	return IVS_SUCCEED;
}



IVS_INT32 CTVWallMgr::CheckChannel(const IPI_SET_PORTS_CONFIG_REQ* pInfo)const
{
	CHECK_POINTER(pInfo, IVS_PARA_INVALID);
	std::list<unsigned short> listTotalChannel;
	IVS_INT32 iRet = AddChannel(pInfo->stPortsInfo.stBNCPortCfgs, listTotalChannel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add Channel Failed!", "BNC Channel.");
		return iRet;
	}

	iRet = AddChannel(pInfo->stPortsInfo.stVGAPortCfgs, listTotalChannel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add Channel Failed!", "VGA Channel.");
		return iRet;
	}

	iRet = AddChannel(pInfo->stPortsInfo.stHDMIPortCfgs, listTotalChannel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add Channel Failed!", "HDMI Channel.");
		return iRet;
	}

	iRet = AddChannel(pInfo->stPortsInfo.stDVIPortCfgs, listTotalChannel);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add Channel Failed!", "DVI Channel.");
		return iRet;
	}

	return IVS_SUCCEED;
}


IVS_INT32 CTVWallMgr::DecoderLogin(const CUMW_DECODER_CONNECT_TYPE emConnectType, const SDK_DECODER_LOGIN_INFO *pSDKDecoderLoginInfo)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pSDKDecoderLoginInfo, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_FAIL;
	if(emConnectType <=  CUMW_DECODER_CONNECT_NONE || CUMW_DECODER_CONNECT_MAX <= emConnectType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Decoder Login failed", "Connect Type invalid");
		return IVS_PARA_INVALID;
	}

	// 判断该解码器名是否已被添加
	if (IsDecoderNameExist(pSDKDecoderLoginInfo->cDecoderName))
	{
		BP_RUN_LOG_ERR(IVS_TVWALL_HAS_SAME_NAME_DEC, "DecoderLogin", "decoder name has exist");
		return IVS_TVWALL_HAS_SAME_NAME_DEC;
	}

	if(CUMW_DECODER_CONNECT_HIK == emConnectType)
	{
		IVS_LONG lSessionID = -1;
		string strDecoderID;
		DECODER_DEV stDecoder;
		eSDK_MEMSET(&stDecoder, 0, sizeof(DECODER_DEV));
		stDecoder.iPort = pSDKDecoderLoginInfo->uiPort;//lint !e713
		eSDK_MEMCPY(stDecoder.cDevIP, IVS_IP_LEN + 1, pSDKDecoderLoginInfo->stIP.cIP, strlen(pSDKDecoderLoginInfo->stIP.cIP));
		eSDK_MEMCPY(stDecoder.cDecoderName, CUMW_DECODER_NAME_LEN+1, pSDKDecoderLoginInfo->cDecoderName, CUMW_DECODER_NAME_LEN);
		eSDK_MEMCPY(stDecoder.cUser, IVS_NAME_LEN+1, pSDKDecoderLoginInfo->cUserName, IVS_NAME_LEN);

		//1. login hik decoder
		CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); 
		CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(pDecoderSDK);
		CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
		iRet = pHikDecoderSDK->Login(lSessionID, &stDecoder, pSDKDecoderLoginInfo->pPWD);
		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "DecoderLogin", "HIK decoder login failed.");
			return iRet;
		}

		// 2. get decoder info & create decoderid
		(void)pHikDecoderSDK->CreateHikDecoderID(strDecoderID);
		eSDK_MEMCPY(stDecoder.cDecoderID, CUMW_DECODER_ID_LEN+1, strDecoderID.c_str(), strDecoderID.size());
		iRet = pHikDecoderSDK->GetChannels(lSessionID, &stDecoder);
		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "DecoderLogin", "HIK decoder get channels failed.");
			return iRet;
		}

		// 3. add decoder info into local
		iRet = pHikDecoderSDK->AddDecoderInfo(lSessionID, strDecoderID.c_str(), stDecoder);
		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "DecoderLogin", "add HIK decoder info failed.");
			return iRet;
		}		
	}

	// 将解码器名加入到list中，用于后续判断是否已经添加了该解码器名
	iRet = AddDecoderNameToList(pSDKDecoderLoginInfo->cDecoderName);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DecoderLogin", "add decoder name failed.");
		return iRet;
	}

	return iRet;
}


IVS_INT32 CTVWallMgr::DecoderLogout(const CUMW_DECODER_CONNECT_TYPE emConnectType, const IVS_CHAR* pDecoderID)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pDecoderID, IVS_PARA_INVALID);
	CHECK_EQUAL(strlen(pDecoderID), 0, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	IVS_CHAR *pDecoderName = NULL;

	if(emConnectType <=  CUMW_DECODER_CONNECT_NONE || CUMW_DECODER_CONNECT_MAX <= emConnectType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Decoder Logout failed", "Connect Type invalid");
		return IVS_PARA_INVALID;
	}

	if(CUMW_DECODER_CONNECT_HIK == emConnectType)
	{
		CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); 
		CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(pDecoderSDK);
		CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
		DECODER_REG_INFO_MAP TempMap;
		IVS_LONG lSessionID = SDK_SESSIONID_INIT;
		pHikDecoderSDK->GetDecInfo2Map(TempMap);
		DECODER_REG_INFO_MAP::iterator BeginIter = TempMap.begin();
		DECODER_REG_INFO_MAP::iterator EndIter = TempMap.end();
		for(; BeginIter != EndIter; ++BeginIter)
		{
			if(0 == strcmp(pDecoderID, BeginIter->second.cDecoderID))
			{
				lSessionID = BeginIter->first;
				pDecoderName = BeginIter->second.cDecoderName;
			}
		}
		if(SDK_SESSIONID_INIT == lSessionID)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "DecoderLogout", "decoderid not match");
			return IVS_FAIL;
		}
		iRet = pHikDecoderSDK->Logout(lSessionID);
		if(IVS_SUCCEED != iRet)
		{
			return iRet;
		}
	}

	// 删除解码器名
	iRet = DelDecoderNameFromList(pDecoderName);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Decoder Logout failed", "delete decoder name = %s, failed", pDecoderName);
		return iRet;
	}

	return iRet;
}


IVS_INT32 CTVWallMgr::GetInfoFromHikDecoder(REMOTE_DECODER_MAP& stRemoteDecInfoMap)
{
	IVS_INFO_TRACE("");
	CDecoderSDK* pDecoderSDK = CDecoderSDKFactory::GetDecoder(CUMW_DECODER_CONNECT_HIK); 
	CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(pDecoderSDK);
	CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);

	std::string stDecoderID;
	DECODER_REG_INFO_MAP TempMap;
	pHikDecoderSDK->GetDecInfo2Map(TempMap);
	stRemoteDecInfoMap.clear();

	IVS_UINT32 uiDecoderSize = TempMap.size();
	IVS_CHAR* pDecoder = IVS_NEW((IVS_CHAR*&)pDecoder, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)*uiDecoderSize);
	CHECK_POINTER(pDecoder, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(pDecoder, 0, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS)*uiDecoderSize);
	CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoderStatus = reinterpret_cast<CUMW_NOTIFY_REMOTE_DECODER_STATUS*>(pDecoder);//lint !e826
	IVS_UINT32 uiIndex = 0;

	DECODER_REG_INFO_MAP::iterator BeginIter = TempMap.begin();
	DECODER_REG_INFO_MAP::iterator EndIter = TempMap.end();
	for(; BeginIter != EndIter; ++BeginIter)
	{	
		stDecoderID = BeginIter->second.cDecoderID;
		eSDK_MEMCPY(pDecoderStatus[uiIndex].szDecoderID, CUMW_DECODER_ID_LEN + 1, BeginIter->second.cDecoderID, CUMW_DECODER_ID_LEN + 1);
		eSDK_MEMCPY(pDecoderStatus[uiIndex].szDecoderName, CUMW_DECODER_NAME_LEN + 1, BeginIter->second.cDecoderName, CUMW_DECODER_NAME_LEN + 1);
		eSDK_MEMCPY(pDecoderStatus[uiIndex].szDecoderIP, IVS_IP_LEN, BeginIter->second.cDevIP, IVS_IP_LEN);
		pDecoderStatus[uiIndex].ulChannelNum = (unsigned long)BeginIter->second.iChannelNum; 
		IVS_UINT32 uiChannlesSize = sizeof(CUMW_DECODER_CHANNEL_INFO)*(BeginIter->second.iChannelNum);//lint !e737
		eSDK_MEMCPY(&pDecoderStatus[uiIndex].stChannelInfoArray[0], uiChannlesSize, &BeginIter->second.stChannels[0], uiChannlesSize);
		(void)stRemoteDecInfoMap.insert(std::make_pair(stDecoderID, &pDecoderStatus[uiIndex]));
		uiIndex++;
	}

	return IVS_SUCCEED;
}//lint !e1762


IVS_INT32 CTVWallMgr::GetDecoderChannelInfo(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel, IVS_LONG& lUserID, CUMW_DECODER_CHANNEL_INFO& stDecoderChannelInfo)
{
	IVS_INFO_TRACE("");
	std::string stdecoderID = pDecoderID;
	DECODER_REG_INFO_MAP stRegInfoMap;
	CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(GetDecoderSDKHandle().GetDecoder(CUMW_DECODER_CONNECT_HIK));//lint !e1705
	CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
	pHikDecoderSDK->GetDecInfo2Map(stRegInfoMap);
	DECODER_REG_INFO_MAP::iterator BeginIter = stRegInfoMap.begin();
	DECODER_REG_INFO_MAP::iterator EndIter = stRegInfoMap.end();
	for(; BeginIter != EndIter; ++BeginIter)
	{
		if(0 == stdecoderID.compare(BeginIter->second.cDecoderID))
		{
			for(IVS_INT32 i = 0; i < BeginIter->second.iChannelNum; ++i)
			{
				if((unsigned long)uiChannel == BeginIter->second.stChannels[i].ulChannelID)
				{
					lUserID = (IVS_LONG)BeginIter->first;
					eSDK_MEMCPY(BeginIter->second.stChannels[i].szDecoderID,  IVS_TVWALL_DECODER_CODE_LEN + 1, BeginIter->second.cDecoderID, IVS_TVWALL_DECODER_CODE_LEN + 1);
					eSDK_MEMCPY(&stDecoderChannelInfo, sizeof(CUMW_DECODER_CHANNEL_INFO), &BeginIter->second.stChannels[i], sizeof(CUMW_DECODER_CHANNEL_INFO));
					BP_RUN_LOG_INF("GetDecoderChannelInfo", "Get Channel Info, DecoderID: %s, ChannelID: %lu", BeginIter->second.stChannels[i].szDecoderID, BeginIter->second.stChannels[i].ulChannelID);
					return IVS_SUCCEED;
				}
			}
		}
	}

	return IVS_FAIL;
}


IVS_INT32 CTVWallMgr::CheckDecoderEx(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pDecoderID, IVS_ALLOC_MEMORY_ERROR);
	BP_DBG_LOG("CheckDecContType", "DecoderID: %s, Channel: %d", pDecoderID, uiChannel);
	std::string stDecoderID;
	stDecoderID = pDecoderID;
	IVS_INT32 iRet = -1;
	iRet = CheckDecoder(pDecoderID, uiChannel);//lint !e838
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("CheckDecoderEx","Get HuaWei RDA decoder");
		return IVS_SUCCEED;
	}

	CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(GetDecoderSDKHandle().GetDecoder(CUMW_DECODER_CONNECT_HIK));//lint !e1705
	CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
	iRet = pHikDecoderSDK->CheckDecoderChannel(pDecoderID, uiChannel);
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("CheckDecoderEx","Get HIK decoder");
		return IVS_SUCCEED;
	}

	return iRet;

}


IVS_INT32 CTVWallMgr::CheckDecContType(const IVS_CHAR* pDecoderID, IVS_UINT32 uiChannel)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pDecoderID, IVS_ALLOC_MEMORY_ERROR);
	BP_DBG_LOG("CheckDecContType", "DecoderID: %s, Channel: %d", pDecoderID, uiChannel);
	std::string stDecoderID;
	stDecoderID = pDecoderID;
	IVS_INT32 iRet = -1;
	iRet = CheckDecoder(pDecoderID, uiChannel);//lint !e838
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("CheckDecContType","Get decoder connect type: HuaWei RDA");
		return CUMW_DECODER_CONNECT_HW;
	}

	CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(GetDecoderSDKHandle().GetDecoder(CUMW_DECODER_CONNECT_HIK));//lint !e1705
	CHECK_POINTER(pHikDecoderSDK, IVS_ALLOC_MEMORY_ERROR);
	iRet = pHikDecoderSDK->CheckDecoderChannel(pDecoderID, uiChannel);
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("CheckDecContType","Get decoder connect type: HIK");
		return CUMW_DECODER_CONNECT_HIK;
	}

	return iRet;

}


void CTVWallMgr::DecoderRelease()
{
	IVS_INFO_TRACE("");
	CHikDecoderSDK* pHikDecoderSDK = dynamic_cast<CHikDecoderSDK*>(GetDecoderSDKHandle().GetDecoder(CUMW_DECODER_CONNECT_HIK));//lint !e1705
	if(NULL == pHikDecoderSDK)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "DecoderRelease","get decoder handle failed");
		return;
	}
	pHikDecoderSDK->DecoderRelease();

	// 删除所有的解码器名
	DelAllDecoderNameFromList();

	return;
}

IVS_INT32 CTVWallMgr::AddDecoderNameToList(const IVS_CHAR* pDecoderName)
{
	CHECK_POINTER(pDecoderName, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("Decoder name = %s", pDecoderName);

	IVS_CHAR cDecoderName[CUMW_DECODER_NAME_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cDecoderName, sizeof(cDecoderName), pDecoderName, CUMW_DECODER_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Decoder Login failed", "copy decoder name failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strDecoderName = cDecoderName;
	(void)VOS_MutexLock(m_pDecoderNameMutex);
	m_listDecoderName.push_back(strDecoderName);
	(void)VOS_MutexUnlock(m_pDecoderNameMutex);

	return IVS_SUCCEED;
}

IVS_INT32 CTVWallMgr::DelDecoderNameFromList(const IVS_CHAR* pDecoderName)
{
	CHECK_POINTER(pDecoderName, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("Decoder name = %s", pDecoderName);

	IVS_CHAR cDecoderName[CUMW_DECODER_NAME_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cDecoderName, sizeof(cDecoderName), pDecoderName, CUMW_DECODER_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Decoder Login failed", "copy decoder name failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	std::string strDecoderName = cDecoderName;

	(void)VOS_MutexLock(m_pDecoderNameMutex);
	TVWALL_DEC_NAME_LIST::iterator Iter = m_listDecoderName.begin();
	TVWALL_DEC_NAME_LIST::iterator IterEnd = m_listDecoderName.end();
	for(; Iter != IterEnd; Iter++)
	{
		if (!strDecoderName.compare(*Iter))
		{
			BP_RUN_LOG_INF("delete decoder name = %s", strDecoderName.c_str());
			m_listDecoderName.erase(Iter);
			(void)VOS_MutexUnlock(m_pDecoderNameMutex);
			return IVS_SUCCEED;
		}
	}
	(void)VOS_MutexUnlock(m_pDecoderNameMutex);
	BP_RUN_LOG_WAR("not find the decoder name = %s", strDecoderName.c_str());
	
	return IVS_PARA_INVALID;
}

void CTVWallMgr::DelAllDecoderNameFromList()
{
	IVS_DEBUG_TRACE("");

	(void)VOS_MutexLock(m_pDecoderNameMutex);
	m_listDecoderName.clear();
	(void)VOS_MutexUnlock(m_pDecoderNameMutex);

	return;
}

IVS_BOOL CTVWallMgr::IsDecoderNameExist(const IVS_CHAR* pDecoderName)
{
	CHECK_POINTER(pDecoderName, FALSE);
	IVS_DEBUG_TRACE("Decoder name = %s", pDecoderName);

	IVS_CHAR cDecoderName[CUMW_DECODER_NAME_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cDecoderName, sizeof(cDecoderName), pDecoderName, CUMW_DECODER_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Decoder Login failed", "copy decoder name failed");
		return FALSE;
	}
	std::string strDecoderName = cDecoderName;

	(void)VOS_MutexLock(m_pDecoderNameMutex);
	TVWALL_DEC_NAME_LIST::iterator Iter = m_listDecoderName.begin();
	TVWALL_DEC_NAME_LIST::iterator IterEnd = m_listDecoderName.end();
	for(; Iter != IterEnd; Iter++)
	{
		if (!strDecoderName.compare(*Iter))
		{
			(void)VOS_MutexUnlock(m_pDecoderNameMutex);
			return TRUE;
		}
	}
	(void)VOS_MutexUnlock(m_pDecoderNameMutex);

	return FALSE;
}




