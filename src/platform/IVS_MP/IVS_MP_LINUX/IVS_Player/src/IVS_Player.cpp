// IVS_Player.cpp : Defines the exported functions for the DLL application.
//
#include "IVS_Player.h"
#include "MediaChannel.h"
#include "MediaChannelMgr.h"
#include "ivs_log.h"
#include "ivs_timer.h"
#ifdef WIN32
#include "IntelDecoder.h"  //intel 硬解能力支持
#include "NvdiaDecoder.h"
#endif

// 日志参数配置
#define LOG_CONF_PATH   "../log"          // 设置当前目录
#define LOG_CONF		"mp_log.conf"// 配置文件名称
#define LOG_FILE_NAME   "IVS_Player"
#define MOUDLE_TYPE     "player"      // 模块名称
#define MOUDLE_ID       101           // 模块编号

//语音对讲开启标志
bool g_bIsStartVoiceTalkback = false;

//语音广播开启标志
bool g_bIsStartVoiceBroadcast = false;


/*
 * PlayerExceptionCallBack
 * 全局异常函数
 */
static IVS_VOID __SDK_CALLBACK PlayerExceptionCallBack(IVS_ULONG /*ulChannel*/, 
	    IVS_INT32 iMsgType, IVS_VOID*  pParam, IVS_VOID*  pUser)
{
    CMediaChannelMgr::instance().DoExceptionCallBack(iMsgType, pParam, pUser);
}

/*
 * PlayerEventCallBack
 * 全局异常函数
 */
static IVS_VOID __SDK_CALLBACK PlayerEventCallBack(IVS_ULONG /*ulChannel*/, 
	    IVS_INT32 iMsgType, IVS_VOID*  pParam, IVS_VOID*  pUser)
{
    CMediaChannelMgr::instance().DoEventCallBack(iMsgType, pParam, pUser);
}

#ifndef WIN32
/*
 * GetModuleFilePath
 * 获取指定库文件所在目录的上级目录
 * sModuleName [in] 库文件名称，如"libIVS_Player.so"
 * sPath [out] 返回目录
 * unSize [int] sPath的长度
 * added by zhangrui/233055
 */
int GetModuleFilePath(const char* sModuleName, char* sPath, unsigned int unSize)
{
	int iRet = IVS_FAIL;
	char sLine[1024] = {0};
	FILE *fp = NULL;
	char *pTmpFullDir = NULL;
	char *pTmpModuleDir = NULL;

	fp = fopen ("/proc/self/maps", "r");
	if (NULL == fp)
	{
		return iRet;
	}

	while (0 == feof(fp))
	{
		if (NULL == fgets(sLine, sizeof(sLine), fp))
		{
			continue;
		}

		pTmpFullDir = strchr(sLine, '/');
		if (NULL == strstr(sLine, "r-xp") ||  NULL == pTmpFullDir || NULL == strstr(sLine, sModuleName))//r-xp:私有只读可执行属性
		{
			continue;
		}

		//如果查找的模块直接放在根目录下，返回失败
		pTmpModuleDir = strrchr(pTmpFullDir, '/');            
		if (pTmpModuleDir == pTmpFullDir)
		{
			break;
		}

		*pTmpModuleDir = '\0';
		//查找的模块必须放在lib目录下，即lib/libXXX.so
		pTmpModuleDir = strrchr(pTmpFullDir, '/'); 
		if (pTmpModuleDir == pTmpFullDir)
		{
			iRet = IVS_SUCCEED;
			strcpy(sPath, "/");
			break;
		}

		*pTmpModuleDir = '\0';            

		if (strlen(pTmpFullDir) >= unSize)
		{
			break;
		}

		iRet = IVS_SUCCEED;
		strncpy(sPath, pTmpFullDir, strlen(pTmpFullDir) + 1);
		break;
	}

	return iRet;

}
#endif

/*
 * IVS_PLAY_Init
 * 初始化解码播放库
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_Init(const char* pLogPath)
{
    // 指定目录（多进程场景）
#ifdef WIN32 
	std::string strLogPath = LOG_CONF_PATH;
	std::string strLogConf = LOG_CONF;
#else
	char pModulePath[1024] = {0};
	int iResult = GetModuleFilePath("libIVS_Player.so", pModulePath, 1024);
	if (IVS_SUCCEED != iResult)
	{
		IVS_LOG(iResult, "GetModuleFileName Failed ", "failed.");
		return iResult;
	}
	std::string strLogPath = pModulePath;
	std::string strLogConf = pModulePath;
	strLogPath += "/log";
	strLogConf += "/config/";
	strLogConf += LOG_CONF;
#endif
    if (NULL !=pLogPath && 0 != strlen(pLogPath))
    {
        strLogPath = pLogPath;
    }
    IVS_INT32 iInitLogRet = IVS_LogInit(strLogPath.c_str(), strLogConf.c_str(), LOG_FILE_NAME, MOUDLE_TYPE, MOUDLE_ID, NULL);
    if (IVS_SUCCEED != iInitLogRet) //lint !e1776
	{
	    return IVS_FAIL;	
	}
#ifdef WIN32
	//防止系统关闭显示器或进入待机状态
    (void)SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_DISPLAY_REQUIRED);

    //防止系统进入屏保
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
#endif

	(void)IVS_StarTimer();

    CMediaChannelMgr::instance().Init();
#ifdef WIN32
    if (CNvidiaDecoder::HasHwDecAbility())
    {
        int iRet = CNvidiaDecoder::InitCuda();
        if (IVS_SUCCEED != iRet)
        {
            IVS_LOG(IVS_LOG_CRIT, "Init cuda, can't use nvidia decode", "errno=%d", iRet); //lint !e1551
        }
    }
#endif
	return IVS_SUCCEED;
}

/*
 * IVS_PLAY_UnInit
 * 清理解码播放库
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_UnInit()
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_UnInit");
    CMediaChannelMgr::instance().UnInit();
#ifdef WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);
#endif

	IVS_StopTimer();

#ifdef WIN32	
    if (CNvidiaDecoder::HasHwDecAbility())
    {
        CNvidiaDecoder::DestoryCuda();
    }
#endif
    IVS_LogClean();  // 关闭日志需放在最后
}

/*
 * IVS_PLAY_GetChannel
 * 获取播放通道
 */ 
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetChannel(IVS_ULONG* pChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_GetChannel in");
    IVS_ASSERT(NULL != pChannel);
	if (NULL == pChannel)
	{
        return IVS_FAIL;
	}
	int iRet = CMediaChannelMgr::instance().GetChannel(*pChannel);   
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_GetChannel out, Channel[%lu]", *pChannel);
	return iRet;
}

/*
 * IVS_PLAY_FreeChannel
 * 释放播放通道
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_FreeChannel(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_FreeChannel in");
    int iRet = CMediaChannelMgr::instance().FreeChannel(ulChannel);
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_FreeChannel out, Channel[%lu]", ulChannel);
	return iRet;
}

/*
 * IVS_PLAY_SetExceptionCallBack
 * 设置异常回调
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK cbException, void* pUser)
{
    CMediaChannelMgr::instance().SetExceptionCallBack(cbException, pUser);
}

/*
 * IVS_PLAY_SetVideoDecodeType
 * 设置解码器类型
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVideoDecodeType(IVS_ULONG ulChannel, IVS_UINT32 uiVideoDecoderType)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		pMediaChannel->SetVideoDecodeType((IVS_VIDEO_DEC_TYPE)uiVideoDecoderType);
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_GetVideoDecodeType
 * 获取解码器类型
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetVideoDecodeType(IVS_ULONG ulChannel, IVS_UINT32* pVideoDecoderType)
{
	IVS_ASSERT(NULL != pVideoDecoderType);
	if (NULL == pVideoDecoderType)
	{
		return IVS_PARA_INVALID;
	}
	
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		IVS_VIDEO_DEC_TYPE enVideoDecoderType = VIDEO_DEC_H264;
		pMediaChannel->GetVideoDecodeType(enVideoDecoderType);
		*pVideoDecoderType = (unsigned int)enVideoDecoderType;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_SetAudioDecodeType
 * 设置音频解码器类型
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetAudioDecodeType(IVS_ULONG ulChannel, IVS_UINT32 uiAudioDecoderType)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		pMediaChannel->SetAudioDecodeType((IVS_AUDIO_DEC_TYPE)uiAudioDecoderType);
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_GetAudioDecodeType
 * 获取音频解码器类型
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetAudioDecodeType(IVS_ULONG ulChannel, IVS_UINT32* pAudioDecoderType)
{
	IVS_ASSERT(NULL != pAudioDecoderType);
	if (NULL == pAudioDecoderType)
	{
		return IVS_PARA_INVALID;
	}

	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		IVS_AUDIO_DEC_TYPE enAudioDecType = AUDIO_DEC_G711U;
		pMediaChannel->GetAudioDecodeType(enAudioDecType);
		*pAudioDecoderType = (unsigned int)enAudioDecType;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_OpenStream
 * 打开视频流
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_OpenStream(IVS_ULONG ulChannel, IVS_UINT32 uiPlayeMode,
	                                                   IVS_UINT32 uiSourceType)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_OpenStream in, Channel[%lu] PlayeMode[%u] SourceType[%u]", ulChannel, uiPlayeMode, uiSourceType);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
        // 设置媒体缓冲区的上下限;
        IVS_UINT32 uiLowerLimit = 0;
        IVS_UINT32 uiUpperLimit = 0;
        CMediaChannelMgr::instance().GetMediaBufferLimit(uiLowerLimit, uiUpperLimit);

        OPEN_STREAM_PARAM stOpenStreamParam;
        memset(&stOpenStreamParam, 0, sizeof(OPEN_STREAM_PARAM));
        stOpenStreamParam.enServiceType = (IVS_SERVICE_TYPE)uiPlayeMode;
        stOpenStreamParam.enSourceType = (SOURCE_TYPE)uiSourceType;
        stOpenStreamParam.fExceptionCallback = PlayerExceptionCallBack;
        stOpenStreamParam.fEventCallback = PlayerEventCallBack;
        stOpenStreamParam.pUser = (void*)ulChannel;
        stOpenStreamParam.uiLowerLimit = uiLowerLimit;
        stOpenStreamParam.uiUpperLimit = uiUpperLimit;
		iRet = pMediaChannel->OpenStream(stOpenStreamParam);
	}

	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_OpenStream out, Ret[%d]", iRet);
	return iRet;

}

/*
 * IVS_PLAY_CloseStream
 * 关闭视频流
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_CloseStream(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_CloseStream in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->CloseStream();
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_CloseStream out, Ret[%d]", iRet);
	return iRet;
}

/*
 * IVS_PLAY_StartPlay
 * 启动解码播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StartPlay(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StartPlay in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->StartPlay();
	}

	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StartPlay out, Ret[%d]", iRet);
	return iRet;
}

/*
 * IVS_PLAY_StopPlay
 * 停止解码播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopPlay(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StopPlay in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->StopPlay();
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StopPlay out.");
	return iRet;
}

/*
 * IVS_PLAY_PausePlay
 * 暂停播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_PausePlay(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_PausePlay in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
    CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->PausePlay();
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_PausePlay out.");
	return iRet;
}

/*
 * IVS_PLAY_ResumePlay
 * 恢复播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_ResumePlay(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_ResumePlay in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->ResumePlay();
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_ResumePlay out.");
	return iRet;
}

/*
 * IVS_PLAY_SetPlaySpeed
 * 设置播放速度
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetPlaySpeed(IVS_ULONG ulChannel, double dSpeed)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_SetPlaySpeed in, Channel[%lu] Speed[%f]", ulChannel, (float)dSpeed);
    int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		pMediaChannel->SetPlaySpeed(dSpeed);
		iRet = IVS_SUCCEED;
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_SetPlaySpeed out.");
	return iRet;
}

/*
 * IVS_PLAY_StepPlayForward
 * 单步播放前进
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StepPlayForward(IVS_ULONG ulChannel, IVS_ULONG *pRemain)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		unsigned int remain = 0;
		if (IVS_SUCCEED == pMediaChannel->StepForward(remain))
		{
			if (NULL != pRemain)
			{
				*pRemain = remain;
			}
        	return IVS_SUCCEED;
		}
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_StepPlayBackward
 * 单步播放后退
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StepPlayBackward(IVS_ULONG ulChannel, IVS_ULONG *pRemain)
{
	if (NULL == pRemain)
	{
		IVS_LOG(IVS_LOG_ERR, "Step Play Forward", "Remian = NULL");
		return IVS_PARA_INVALID;
	}

	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		unsigned int remain = 0;
		if (IVS_SUCCEED == pMediaChannel->StepBackward(remain))
		{
			*pRemain = remain;
			return IVS_SUCCEED;
		}
	}
	return IVS_FAIL;
}


/*
 * IVS_PLAY_PlaySound
 * 启动音频播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_PlaySound(IVS_ULONG ulChannel)
{

    // add by wanglei 00165153:2013.6.9 DTS2013060606556
    if (g_bIsStartVoiceTalkback)  //语音对讲开启标志
    {
        return IVS_PLAYER_RET_TALKBACK_OPENED_ERROR;
    }

	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->PlaySound();
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_StopSound
 * 停止音频播放
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopSound(IVS_ULONG ulChannel)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->StopSound();
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_SetVolume
 * 设置音量
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVolume(IVS_ULONG ulChannel,
                                                    IVS_UINT32 uiVolumeValue)
{
    CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
    if (NULL != pMediaChannel)
    {
        return pMediaChannel->SetVolume(uiVolumeValue);
    }
    return IVS_FAIL;
}

/*
 * IVS_PLAY_GetVolume
 * 获取音量
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetVolume(IVS_ULONG ulChannel,
                                                    IVS_UINT32* puiVolumeValue)
{
    CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
    if (NULL != pMediaChannel)
    {
        return pMediaChannel->GetVolume(puiVolumeValue);
    }
    return IVS_FAIL;
}

/*
 * IVS_PLAY_SetRawFrameCallBack
 * 设置未解码帧数据回调
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetRawFrameCallBack(IVS_ULONG ulChannel,
	                                   REASSM_CALLBACK cbReassmCallBack, void *pUser)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetRawFrameCallBack(cbReassmCallBack, pUser);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_SetFrameCallBack
 * 设置解码后帧数据回调
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetFrameCallBack(IVS_ULONG ulChannel,
	                                   DECODER_CALLBACK fnDecoderCB, void *pUser)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetFrameCallBack(fnDecoderCB, pUser);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_SetPlayWindow
 * 设置播放窗口
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetPlayWindow(IVS_ULONG ulChannel, HWND hWnd)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetPlayWindow(hWnd);
	}
	return IVS_FAIL;
}
#endif

/*
 * IVS_PLAY_AddPlayWindow
 * 增加播放窗口
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_AddPlayWindow(IVS_ULONG ulChannel, 
	                                    HWND hWnd, int nType, void *pLayout)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->AddPlayWindow(hWnd, nType, pLayout);
	}
	return IVS_FAIL;
}
#endif

/*
 * IVS_PLAY_AddPlayWindowPartial
 * 加入局部播放窗口
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_AddPlayWindowPartial(IVS_ULONG ulChannel, 
	             HWND hWnd, LAYOUT_DRAW_PARTIAL *pPartial)
{
	CMediaChannel *pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->AddPlayWindow(hWnd, DRAW_PARTIAL, pPartial);
	}
	return IVS_FAIL;
}
#endif

/*
 * Render_UpdateWindowLayout
 * 更新局部放大窗口布局
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_UpdateWindowPartial(IVS_ULONG ulChannel,
	                  HWND hWnd, LAYOUT_DRAW_PARTIAL *pPartial)
{
	CMediaChannel *pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->UpdateWindowLayout(hWnd, DRAW_PARTIAL, pPartial);
	}
	return IVS_FAIL;
}
#endif


/*
 * IVS_PLAY_DelPlayWindow
 * 删除播放窗口
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_DelPlayWindow(IVS_ULONG ulChannel, 
	                                                      HWND hWnd)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->DelPlayWindow(hWnd);
	}
	return IVS_FAIL;
}
#endif

/*
 * IVS_PLAY_SetDisplayAspectRatio
 * 设置播放窗口显示比例
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDisplayAspectRatio(IVS_ULONG ulChannel, IVS_UINT32 uiScale)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetDisplayAspectRatio(uiScale);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_InputRtpData
 * 输入RTP包数据
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_InputRtpData(IVS_ULONG ulChannel, IVS_CHAR *pBuf, IVS_UINT32 uiSize)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->InputRtpData(pBuf, uiSize);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_InputFrameData
 * 获取视频播放参数
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_InputFrameData(IVS_ULONG ulChannel, 
	     IVS_RAW_FRAME_INFO* pstRawFrameInfo, IVS_CHAR* pFrame, IVS_UINT32 uFrameSize)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->ProcessRawFrame(pstRawFrameInfo, pFrame, uFrameSize);
	}
	return IVS_FAIL;
}

#ifdef WIN32
/*
 * IVS_PLAY_OpenFile
 * 打开视频文件
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_OpenFile(IVS_ULONG ulChannel, const IVS_CHAR* pFileName)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->OpenFile(pFileName, PlayerExceptionCallBack, (void*)ulChannel);
	}
	return IVS_FAIL;
}
#endif

/*
 * IVS_PLAY_SetFileTimeRange
 * 设置文件播放时间范围
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetFileTimeRange(IVS_ULONG ulChannel, 
	                           unsigned int beg, unsigned int end)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetFileTimeRange(beg, end);
	}
	return IVS_FAIL;
}







/*
 * IVS_PLAY_SetWaterMarkVerify
 * 开启\关闭水印校验
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetWaterMarkVerify(IVS_ULONG ulChannel, IVS_INT32 iStart)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
        BOOL bStart = FALSE;
        if (0 == iStart)
        {
            bStart = TRUE;
        }
		return pMediaChannel->SetWaterMarkState(bStart);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_GetVideoQualityPara
 * 设置播放流畅度参数
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetMediaInfo(IVS_ULONG ulChannel,IVS_MEDIA_INFO* pMediaInfo)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->GetMediaInfo(pMediaInfo);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_GetPlayedTime
 * 获取播放时间(文件)
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetPlayedTime(IVS_ULONG ulChannel, IVS_UINT64* puPlayTime)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->GetPlayedTime(puPlayTime);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_GetCurrentFrameTick
 * 获取当前播放帧时间戳
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetCurrentFrameTick(IVS_ULONG ulChannel, IVS_UINT64 *pTick)
{                                  
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->GetCurrentFrameTick(pTick);
	}
	return IVS_FAIL;
}


/*
 * IVS_PLAY_SetFrameTickBase
 * 设置播放时间计算基准
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetPlayTime(IVS_ULONG ulChannel, time_t tm)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetPlayTime(tm);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_SetDownLoadTimeSpan
 * 录像下载模式 设置录像起始时间和结束时间
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDownLoadTimeSpan(IVS_ULONG ulChannel, time_t tmBeg, time_t tmEnd, IVS_BOOL bReportFin)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetDownLoadTimeSpan(tmBeg, tmEnd, !!bReportFin);
	}
	return IVS_FAIL;
}


/*
 * IVS_PLAY_SetPlayQualityMode
 * 设置视频流畅度、清晰模式
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetPlayQualityMode(IVS_ULONG ulChannel,IVS_INT32 iPlayQualityMode)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetPlayQualityMode(iPlayQualityMode);
	}
	return IVS_FAIL;
}
#ifdef WIN32
/*
 * IVS_PLAY_GetDecodeAbility
 * 获取硬件解码能力
 */
IVS_PLAYER_API VIDEO_DECODE_MODE __SDK_CALL IVS_PLAY_GetDecodeAbility()
{
    if (CNvidiaDecoder::HasHwDecAbility())
    {
        return DECODE_HW_NVDIA;
    }
    else if (CIntelDecoder::HasHwDecAbility())
    {
        return DECODE_HW_INTEL;
    }
    else
    {
        // TODO: NVDIA硬解
        return DECODE_SW_HICILICON;
    }
}
#endif

/*
 * IVS_PLAY_GetDecodeAbility
 * 设置硬解标志
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetHardwareDec(IVS_BOOL bHardwareDec)
{
    CMediaChannel::m_bHardwareDec = bHardwareDec;
}

/*
 * IVS_PLAY_ShowPlayStatis
 * 使能显示播放统计
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_ShowPlayStatis(IVS_ULONG ulChannel, IVS_BOOL enable)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->ShowPlayStatis(enable != 0);
	}
	return IVS_FAIL;
}
#endif

IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetResetFlag(IVS_ULONG ulChannel, bool bIsReset)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_SetResetFlag in, Channel[%lu]", ulChannel);
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_SetResetFlag out, Channel[%lu]", ulChannel);
		pMediaChannel->SetResetFlag(bIsReset);
	}
	else
	{
		IVS_LOG(IVS_LOG_ERR, "","IVS_PLAY_SetResetFlag out, Channel[%lu] is null", ulChannel);
	}
}
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_ResetData(IVS_ULONG ulChannel)
{
    IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_ResetData in, Channel[%lu]", ulChannel);
    CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
    if (NULL != pMediaChannel)
    {
        IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_ResetData out, Channel[%lu]", ulChannel);
        pMediaChannel->ResetData();
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "","IVS_PLAY_ResetData out, Channel[%lu] is null", ulChannel);
    }
}

/*
 * IVS_PLAY_StartRecord
 * 启动本地录像
 */
IVS_PLAYER_API IVS_INT32 IVS_PLAY_StartRecord(IVS_ULONG ulChannel, const char *pCamCode, const char *pCamName, 
	const char *pName, const IVS_LOCAL_RECORD_PARAM *pParm, bool bAssociatedAudio)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StartRecord in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		iRet = pMediaChannel->StartRecord(pCamCode, pCamName, pName, pParm,
			   PlayerExceptionCallBack, (void*)ulChannel, bAssociatedAudio);
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StartRecord out, Ret[%d]", iRet);
	return iRet;
}

/*
 * IVS_PLAY_StopRecord
 * 停止本地录像
 */
IVS_PLAYER_API IVS_INT32 IVS_PLAY_StopRecord(IVS_ULONG ulChannel)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StopRecord in, Channel[%lu]", ulChannel);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		pMediaChannel->StopRecord();
	    iRet = IVS_SUCCEED;
	}
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_StopRecord out, Ret[%d]", iRet);
	return iRet;
}

/*
 * IVS_PLAY_GetRecordTime
 * 获取已录像时间
 */
IVS_PLAYER_API IVS_INT32 IVS_PLAY_GetRecordStatis(IVS_ULONG ulChannel, IVS_UINT32 *pTime, IVS_UINT32 *pSpeed, IVS_UINT64 *pTotalSz)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->GetRecordStatis(pTime, pSpeed, pTotalSz);

	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_StopDecryptData
 * 停止视频解密
 */
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_StopDecryptData(IVS_ULONG ulChannel)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->StopDecryptData();
	}
	IVS_LOG(IVS_LOG_ERR, "IVS Stop DecryptData", "call media channel stop decrypt data fail.");
	return IVS_FAIL;
}


/*
 * IVS_PLAY_SetSecretKey
 * 设置视频解密密钥
 */
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_SetSecretKey(IVS_ULONG ulChannel,IVS_INT32 uiDecType,const IVS_CHAR* pSecretKey)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_SetSecretKey in, Channel[%lu]", ulChannel);
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	//if (NULL != pMediaChannel && NULL != pSecretKey)
    if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetSecretKey(uiDecType, pSecretKey);
	}
	IVS_LOG(IVS_LOG_ERR, "IVS PLAY SetSecretKey failed", "pMediaChannel is NULL.");
	return IVS_FAIL;
}

#ifdef WIN32
/*
 * IVS_PLAY_LocalSnapshot
 * 本地抓拍单张图片
 */
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_LocalSnapshot(IVS_ULONG ulChannel, const char *pSavePath, IVS_ULONG ulSnapshotFormat)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->LocalSnapshot(pSavePath, ulSnapshotFormat, NULL);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_LocalSnapshotRect
 * 本地局部放大抓拍单张图片
 */
IVS_PLAYER_API  IVS_INT32 IVS_PLAY_LocalSnapshotRect(IVS_ULONG ulChannel, const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT *Rct)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->LocalSnapshot(pSavePath, ulSnapshotFormat, Rct);
	}
	return IVS_FAIL;
}
#endif

/*
 * IVS_PLAY_StepPlay
 * 单步播放控制
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StepPlay(IVS_ULONG ulChannel, IVS_UINT32 *pPicInPlayer)
{
	IVS_ASSERT(NULL != pPicInPlayer);
	if (NULL == pPicInPlayer)
	{
		return IVS_PARA_INVALID;
	}
	
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{   
	    unsigned int PicInPlayer = 0;
		pMediaChannel->StepForward(PicInPlayer);
		*pPicInPlayer = PicInPlayer;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

/*
 *  IVS_PLAY_SetDrawCallBack
 *  设置绘图回调函数
 */
#ifdef WIN32
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDrawCallBack(IVS_ULONG ulChannel, DRAW_CALLBACK  callback, void *pUser)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetDrawCallBack(callback, pUser);
	}
	return IVS_FAIL;

}

/*
 *  IVS_PLAY_SetDrawCallBack
 *  设置绘图回调函数
 */

IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDrawCallBackEx(IVS_ULONG ulChannel, HWND hWnd, DRAW_CALLBACK callback, void *pUser)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetDrawCallBackEx(hWnd, callback, pUser);
	}
	return IVS_FAIL;

}
#endif

/*****************************************************************************
 函数名称：IVS_PLAY_SetDelayFrameNum
 功能描述：设置延时解码的帧数
 输入参数：ulChannel      通道号;
           uDelayFrameNum 延时解码的帧数;
 输出参数：NA
 返 回 值：成功:IVS_SUCCEED;失败:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDelayFrameNum(IVS_ULONG ulChannel, 
	                                                IVS_UINT32 uDelayFrameNum);

/*
 *  IVS_PLAY_SetDelayFrameNum
 *  设置延时解码的帧数
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDelayFrameNum(IVS_ULONG ulChannel, IVS_UINT32 uDelayFrameNum)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetDelayFrameNum(uDelayFrameNum);
	}

	return IVS_FAIL;
}

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceVolume()
 功能描述：设置语音对讲的音量
 输入参数：ulVolumeValue       音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceVolume(IVS_ULONG ulVolumeValue)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Voice Volume",
            "Set voice volume begin, volume value = %u.", ulVolumeValue);

	//入参检查
    if (PLAYER_AUDIO_MAX_VOLUM < ulVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Voice Volume",
                "Set voice volume error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

#ifdef WIN32
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL != pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Voice Volume",
                "Set voice volume error, get voice channel fail.");
        return IVS_FAIL;
    }

    return pAudioChannel->SetVoiceVolume(ulVolumeValue);  //lint !e413
#else
    return IVS_SUCCEED;
#endif
}

/*****************************************************************************
 函数名称：IVS_PLAY_GetVoiceVolume()
 功能描述：获取语音对讲的音量
 输入参数：无
 输出参数：ulVolumeValue       音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetVoiceVolume(IVS_UINT32* pVolumeValue)
{    
	//入参检查
    if (NULL == pVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Voice Volume",
                "Get voice volume error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

#ifdef WIN32
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Voice Volume",
                "Get voice volume error, get voice channel fail.");
        return IVS_FAIL;
    }

    return pAudioChannel->GetVoiceVolume(pVolumeValue);
#else
    return IVS_SUCCEED;
#endif
}

/*****************************************************************************
 函数名称：IVS_PLAY_SetMicVolume()
 功能描述：设置麦克风的音量
 输入参数：ulVolumeValue       音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetMicVolume(IVS_UINT32 uVolumeValue)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Mic Volume",
            "IVS dll:Set mic volume begin, volume value = %u.", uVolumeValue);

	//入参检查
    if (PLAYER_AUDIO_MAX_VOLUM < uVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Mic Volume",
                "Set Mic volume error, parameter is invalid.");
        //TODO 定义错误码
        return IVS_PARA_INVALID;
    }
    long ulRet = IVS_SUCCEED;
#ifdef WIN32
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Mic Volume",
                "Set Mic volume error, get voice channel fail.");
        return IVS_FAIL;
    }

	//初始化音量控制
 	ulRet = pAudioChannel->InitVoiceControl();        //lint !e838
 
 	if (IVS_SUCCEED == ulRet)
 	{
		ulRet = pAudioChannel->SetMicrophoneVolume(uVolumeValue);

		(void)pAudioChannel->ReleaseVoiceControl();
	}
#endif

	return ulRet;
}

/*****************************************************************************
 函数名称：IVS_PLAY_GetMicVolume
 功能描述：获取麦克风的音量
 输入参数：pVolumeValue       音量值，范围从 MIN_WAVE_COEF(0) 到 MAX_WAVE_COEF(100)
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetMicVolume(IVS_UINT32* pVolumeValue)
{    
	//入参检查
	if (NULL == pVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Mic Volume",
                "Get Mic volume error, parameter is invalide.");
        //TODO 定义错误码
        return IVS_PARA_INVALID;
    }
    long ulRet = IVS_SUCCEED;
#ifdef WIN32
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Mic Volume",
                "Get mic volume error, get voice channel fail.");
        return IVS_FAIL;
    }

	//初始化音量控制
	ulRet = pAudioChannel->InitVoiceControl();        //lint !e838

	if (IVS_SUCCEED == ulRet)
	{
		ulRet = pAudioChannel->GetMicrophoneVolume(*pVolumeValue);

		(void)pAudioChannel->ReleaseVoiceControl();
	}
#endif

	return ulRet;
}

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceProtocolType()
 功能描述：设置语音传输协议
 输入参数：ulVoiceProtocolType             语音传输协议
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceProtocolType(IVS_UINT32 uVoiceProtocolType)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Get Mic Volume",
            "IVS dll:Set voice protocol type begin, voice protocol type = %u.",
		    uVoiceProtocolType);

    //入参检查
    if ((PROTOCOL_RTP_OVER_UDP != uVoiceProtocolType) 
		&& (PROTOCOL_RTP_OVER_TCP != uVoiceProtocolType))
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Mic Volume",
                "Set voice protocol type error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

#ifdef WIN32
    CMediaChannelMgr::instance().SetVoiceProtocolType(uVoiceProtocolType);
#endif

    return IVS_SUCCEED;
}

// /*****************************************************************************
//  函数名称：IVS_PLAY_GetRecvVoiceAddr()
//  功能描述：获取接收语音的地址（IP和端口号）
//  输入参数：无
//  输出参数：pRecvVoicePort  接收语音的端口号
//  返 回 值： ULONG
// *****************************************************************************/
// IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetRecvVoiceAddr(IVS_ULONG* pRecvVoicePort)
// {
//     //入参检查
//     if (NULL == pRecvVoicePort)
//     {
//         IVS_LOG(IVS_LOG_ERR, "IVS Get Recv Voice Addr",
//                 "Get receive voice address error, parameter is invalid.");
//         return IVS_PARA_INVALID;
//     }
// 
// #ifdef WIN32
//     return CMediaChannelMgr::instance().GetRecvVoiceAddr(pRecvVoicePort);
// #else
//     return IVS_SUCCEED;
// #endif
// }

// /*****************************************************************************
//  函数名称：IVS_PLAY_FreeRecvVoiceAddr()
//  功能描述：释放接收语音的地址
//  输入参数：ulPort             通道号
//  输出参数：无
//  返 回 值： ULONG
// *****************************************************************************/
// IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_FreeRecvVoiceAddr()
// {
// #ifdef WIN32
//     CMediaChannelMgr::instance().FreeRecvVoiceAddr();
//     CMediaChannelMgr::instance().DestroyVoiceChannel();
// #endif
//     return IVS_SUCCEED;
// }

/*****************************************************************************
 函数名称：IVS_PLAY_SetVoiceDecoderType()
 功能描述：设置语音解码类型
 输入参数：ulVoiceDecoderType             语音解码类型
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceDecoderType(IVS_ULONG ulVoiceDecoderType)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Voice Decoder Type",
            "Set voice decoder type begin, voice decoder type = %u.",
		    ulVoiceDecoderType);

    //入参检查
    if ((PAY_LOAD_TYPE_PCMU != ulVoiceDecoderType) 
		&& (PAY_LOAD_TYPE_PCMA != ulVoiceDecoderType))
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Voice Decoder Type",
                "Set decoder type error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

#ifdef WIN32
    CMediaChannelMgr::instance().SetVoiceDecoderType(ulVoiceDecoderType);
#endif

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：IVS_PLAY_StartVoiceTalkback()
 功能描述：开始语音对讲         
 输入参数：pLocalIp    本地语音的IP地址
           ulLocalPort  本地语音的端口
           pRemoteIp   远端语音的IP地址
           ulRemotePort 远端语音的端口
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StartVoiceTalkback(
                                                        IVS_CHAR* pLocalIp,
                                                        IVS_ULONG ulLocalPort,
                                                        IVS_CHAR* pRemoteIp,
                                                        IVS_ULONG ulRemotePort)
{
	//入参检查
    if ((NULL == pLocalIp) || (NULL == pRemoteIp) 
		|| (0 == strlen(pLocalIp)) || (0 == strlen(pRemoteIp)))
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error, parameter is invalid.");
        return IVS_PARA_INVALID;
    }

    IVS_LOG(IVS_LOG_INFO, "IVS Start Voice Talkback",
		    "IVS dll:Start voice talkback begin, local ip = %s, local port = %u. remote ip = %s, remote port = %u", 
			pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);

    //判断是否已经开启语音对讲
    if (g_bIsStartVoiceTalkback)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error, voice talkback has opened.");
        return IVS_PLAYER_RET_VOICE_TALKBACK_STATUS_ERROR;
    }

#ifdef WIN32
    //创建语音对讲通道
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().CreateVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error,create voice channel fail");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }
    
    CMediaChannelMgr::instance().SetRecvStreamIp(pLocalIp);  //保存到Mgr中

    //设置通道属性
    pAudioChannel->m_strLocalIp = pLocalIp;    //lint !e1705
    pAudioChannel->m_ulLocalAudioRtpPort = ulLocalPort;//lint !e1705

    pAudioChannel->m_strSourceIp = pRemoteIp;
    pAudioChannel->m_ulSourceAudioRtpPort = ulRemotePort;

	pAudioChannel->SetOperationType(VOICE_TALKBACK); //设置业务类型

    //初始化通道
    long ulRet = pAudioChannel->InitChannel();
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error,initial audio channel fail.");
		CMediaChannelMgr::instance().DestroyVoiceChannel();
        return ulRet;
    }

    //开启接受语音对讲
    ulRet = pAudioChannel->StartReceive();
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error, start receive voice data failed, error code is %u.",
                ulRet);
        pAudioChannel->ReleaseChannel();
		CMediaChannelMgr::instance().DestroyVoiceChannel();
        return ulRet;
    }

    //开启发送语音对讲
    ulRet = pAudioChannel->StartSend(VOICE_TALKBACK);
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error, start send voice data failed, error code is %u.",
                ulRet);
        (void)pAudioChannel->StopReceive();
        pAudioChannel->ReleaseChannel();
		CMediaChannelMgr::instance().DestroyVoiceChannel();
        return ulRet;
    }
    
    g_bIsStartVoiceTalkback = true;

    IVS_LOG(IVS_LOG_INFO, "IVS Start Voice Talkback",
            "IVS dll:Start voice talk back end.");
#endif

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：IVS_PLAY_StopVoiceTalkback()
 功能描述：开始语音对讲         
 输入参数：无
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopVoiceTalkback()
{
    IVS_LOG(IVS_LOG_INFO, "IVS Stop Voice Talkback",
            "IVS dll:Stop voice talkback begin.");

#ifdef WIN32
    //指向语音通道类
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop voice talkback error, voice channel is null.");
        return IVS_FAIL;
    }

	//停止发送音频数据
	long ulRet = pAudioChannel->StopSend();
	if (IVS_SUCCEED != ulRet)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop send audio data failed, Result code is %u.", ulRet);
	}
    
    //停止接受音频数据
    ulRet = pAudioChannel->StopReceive();
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop audio revcive failed, Result code is %u.", ulRet);
        //失败继续执行
    }
     
    //释放音频通道资源
    pAudioChannel->ReleaseChannel();
	//CMediaChannelMgr::instance().DestroyVoiceChannel();//只会返回IVS_SUCCEED

    g_bIsStartVoiceTalkback = false;

#endif

    IVS_LOG(IVS_LOG_INFO, "IVS Stop Voice Talkback",
            "IVS dll:Stop voice talkback end.");

	return IVS_SUCCEED;
}


/*****************************************************************************
 函数名称：IVS_PLAY_StartVoiceBroadcast()
 功能描述：开始语音广播        
 输入参数：pLocalIp        本地语音的IP地址
           ulLocalPort     本地语音的端口
           pDeviceList     语音广播设备列表
           ulBroadcastType 语音广播类型(0为文件广播，1为实时广播)
           pFileName       语音广播文件名(绝对路径)
           bCycle          是否循环播放(0不循环播放 1循环播放,实时广播不支持循环)
 输出参数：无
 返 回 值：ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StartVoiceBroadcast(
                                                    const IVS_CHAR* pLocalIp,
                                                    IVS_ULONG ulLocalPort, 
                                                    const IVS_CHAR* pRemoteIp,
                                                    IVS_ULONG ulRemotePort,
                                                    IVS_ULONG ulBroadcastType,
                                                    const IVS_CHAR* pFileName,
                                                    bool bCycle)
{
    //入参检查
	if ((NULL == pLocalIp) || (NULL == pRemoteIp)) 
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, parameter is invalid.");
		return IVS_PARA_INVALID;
	}

#ifdef WIN32
	//先判断是否已经开启语音对讲
	if (g_bIsStartVoiceTalkback)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
            "Start voice broadcast error, because the voice talk has opened.");
		return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
	}

	IVS_LOG(IVS_LOG_INFO, "IVS PLAY StartVoiceBroadcast",
		    "Start voice broadcast begin, local ip = %s, local port = %u.,RemoteIp = %s, RemotePort = %u.", 
			pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);


	//判断是否已经开启语音广播，若已经开启广播，调用AddBroadcast添加广播设备列表	
	if (g_bIsStartVoiceBroadcast)
	{
		CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
		if (NULL == pAudioChannel)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error,the broadcast resource is not exist.");
			return IVS_PLAYER_RET_CHANNEL_NOT_EXIST_ERROR;
		}

		long ulRet = pAudioChannel->AddBroadcastDevice(pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Add voice broadcast device error.");
			return ulRet;
		}

		return IVS_SUCCEED;
	}

	//若没有开启语音广播，需要创建语音广播通道，第一次广播首先需要添加广播设备列表
	CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().CreateVoiceChannel();
	if (NULL == pAudioChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
            "Start voice broadcast error, create voice broadcsat channel fail.");
		return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
	}

	//设置通道属性
	pAudioChannel->m_strLocalIp = pLocalIp;    //lint !e1705
	pAudioChannel->m_ulLocalAudioRtpPort = ulLocalPort;//lint !e1705

	pAudioChannel->SetOperationType(VOICE_BROADCAST); //设置业务类型

	// 文件广播,支持循环
	if (VOICE_FILE_BROADCAST == ulBroadcastType)
	{
		//设置广播类型
		pAudioChannel->SetBroadcastType(VOICE_FILE_BROADCAST);
		pAudioChannel->SetBroadcastFileName(pFileName);
        pAudioChannel->SetBroadcastCycle(bCycle);

		//初始化通道
		long ulRet = pAudioChannel->InitChannel();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Start voice broadcast error,initial audio channel fail.");
			//释放音频通道资源			
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}

		//添加广播设备列表
		ulRet = pAudioChannel->AddBroadcastDevice(pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Add voice broadcast Device error.");
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;  //将广播状态置为未开启
			return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
		}

        //开启接受语音,主要为了接收NAT穿越包
        ulRet = pAudioChannel->StartReceive();
        if (IVS_SUCCEED != ulRet)
        {
            IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, start receive voice data failed, error code is %u.", ulRet);
            pAudioChannel->ReleaseChannel();
            CMediaChannelMgr::instance().DestroyVoiceChannel();
            g_bIsStartVoiceBroadcast = false;
            return ulRet;
        }

		//开始文件广播
		ulRet = pAudioChannel->StartFileSend();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, start send voice data failed, error code is %u.", ulRet);
			
			//释放音频通道资源
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}
	}
	else   //实时广播,不支持循环
	{
		//设置广播类型
		pAudioChannel->SetBroadcastType(VOICE_REAL_BROADCAST);		

		//初始化通道
		long ulRet = pAudioChannel->InitChannel();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Start voice broadcast error,initial audio channel fail.");

			//释放音频通道资源			
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}

		//添加广播设备列表
		ulRet = pAudioChannel->AddBroadcastDevice(pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Add voice broadcast Device error.");
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;  //将广播状态置为未开启
			return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
		}

        //开启接受语音,主要为了接收NAT穿越包
        ulRet = pAudioChannel->StartReceive();
        if (IVS_SUCCEED != ulRet)
        {
            IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, start receive voice data failed, error code is %u.", ulRet);
            pAudioChannel->ReleaseChannel();
            CMediaChannelMgr::instance().DestroyVoiceChannel();
            g_bIsStartVoiceBroadcast = false;
            return ulRet;
        }

		//开始发送实时广播
		ulRet = pAudioChannel->StartSend(VOICE_BROADCAST);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, start send voice data failed, error code is %u.", ulRet);
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}		
	}
#endif
	g_bIsStartVoiceBroadcast = true;  //设置语音广播开启标志

	IVS_LOG(IVS_LOG_INFO, "IVS PLAY StartVoiceBroadcast",
            "IVS dll:Start voice broadcast end.");

    return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：IVS_PLAY_StopVoiceBroadcast()
 功能描述：停止语音广播         
 输入参数：pRemoteIp    语音设备IP
         ulRemotePort 语音设备端口号
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopVoiceBroadcast(
                                                        IVS_CHAR* pRemoteIp,
                                                        IVS_ULONG ulRemotePort)
{
    IVS_LOG(IVS_LOG_INFO, "IVS PLAY StopVoiceBroadcast",
            "IVS dll:Stop voice broadcast begin.");

	//入参检查
	if ((NULL == pRemoteIp)
		|| 0 == strlen(pRemoteIp))
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, parameter is invalid.");
		return IVS_PARA_INVALID;
	}
#ifdef WIN32
	//获取指向语音广播通道类
	CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
	if (NULL == pAudioChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, voice channel is null.");
		return IVS_PLAYER_RET_CHANNEL_NOT_EXIST_ERROR;
	}

	//删除广播设备信息
	long ulRet = pAudioChannel->DelBroadcastDevice(pRemoteIp, ulRemotePort);
	if (IVS_SUCCEED != ulRet)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, delete the broadcast device false.");
		return ulRet;
	}

	int nDeviceNum = pAudioChannel->GetBroadcastNum();
	//判断失败列表是否为空，如果为空，则要释放通道资源，停止广播
	if (0 == nDeviceNum)
	{
		//停止发送广播音频数据
				
		//获取广播类型，文件或实时
		unsigned long ulBroadcastType = pAudioChannel->GetBroadcastType();        //lint !e732
		if (VOICE_FILE_BROADCAST == ulBroadcastType) //文件
		{
			ulRet = pAudioChannel->StopFileSend();
			pAudioChannel->ReleaseFileInputSource();
		}
		else //实时
		{
			ulRet = pAudioChannel->StopSend();
			pAudioChannel->ReleaseChannel();
		}
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                    "Stop send audio data failed, Result code = %u.", ulRet);
		}

		//销毁音频通道
		//CMediaChannelMgr::instance().DestroyVoiceChannel();//只会返回IVS_SUCCEED

		g_bIsStartVoiceBroadcast = false;

		IVS_LOG(IVS_LOG_INFO, "IVS PLAY StopVoiceBroadcast",
                "IVS dll:Stop voice broadcast end.");
	}
#endif
	return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：IVS_PLAY_GetSourceBufferRemain()
 功能描述：获取未解码帧缓冲区剩余的帧数         
 输入参数：无
 输出参数：无
 返 回 值： ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetSourceBufferRemain(
                                                        IVS_ULONG ulChannel,
                                                        IVS_UINT32 &uBufferCount)
{
    CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
    if (NULL != pMediaChannel)
    {
        return pMediaChannel->GetSourceBufferRemain(uBufferCount);
    }
    return IVS_FAIL;
}


/*
 *  IVS_PLAY_SetEventCallBack
 *  设置事件回调函数
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetEventCallBack(PLAYER_EVENT_CALLBACK cbEventCallBack, void* pUser)
{
    CMediaChannelMgr::instance().SetEventCallBack(cbEventCallBack, pUser);
}

IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetMediaBufferLimit(IVS_UINT32 uiLowerLimit, 
                                                               IVS_UINT32 uiUpperLimit)
{
    CMediaChannelMgr::instance().SetMediaBufferLimit(uiLowerLimit, uiUpperLimit);
}

IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_StopNotifyBuffEvent(IVS_ULONG ulChannel)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->StopNotifyBuffEvent();
	}
}


/*
 * IVS_PLAY_SetMeidaAttribute
 * 设置媒体属性
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetMediaAttribute(IVS_ULONG ulChannel,
	                                                          const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetMeidaAttribute(pVideoAttr, pAudioAttr);
	}
    return IVS_FAIL;
}

/*
 * IVS_PLAY_SetWaitForVertiBlank
 * 设置垂直同步
 */
IVS_PLAYER_API  IVS_INT32 __SDK_CALL IVS_PLAY_SetWaitForVertiBlank(IVS_ULONG ulChannel, int bEnable)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->SetWaitForVertiBlank(bEnable);
	}
	return IVS_FAIL;
}

/*
 * IVS_PLAY_ReSetFlowContrlFlag
 * 清空流控标志
 */
IVS_PLAYER_API  IVS_VOID __SDK_CALL IVS_PLAY_ReSetFlowContrlFlag(IVS_ULONG ulChannel)
{
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
		return pMediaChannel->ReSetFlowContrlFlag();
	}
	return;
}
