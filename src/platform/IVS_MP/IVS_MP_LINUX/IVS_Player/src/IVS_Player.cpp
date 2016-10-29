// IVS_Player.cpp : Defines the exported functions for the DLL application.
//
#include "IVS_Player.h"
#include "MediaChannel.h"
#include "MediaChannelMgr.h"
#include "ivs_log.h"
#include "ivs_timer.h"
#ifdef WIN32
#include "IntelDecoder.h"  //intel Ӳ������֧��
#include "NvdiaDecoder.h"
#endif

// ��־��������
#define LOG_CONF_PATH   "../log"          // ���õ�ǰĿ¼
#define LOG_CONF		"mp_log.conf"// �����ļ�����
#define LOG_FILE_NAME   "IVS_Player"
#define MOUDLE_TYPE     "player"      // ģ������
#define MOUDLE_ID       101           // ģ����

//�����Խ�������־
bool g_bIsStartVoiceTalkback = false;

//�����㲥������־
bool g_bIsStartVoiceBroadcast = false;


/*
 * PlayerExceptionCallBack
 * ȫ���쳣����
 */
static IVS_VOID __SDK_CALLBACK PlayerExceptionCallBack(IVS_ULONG /*ulChannel*/, 
	    IVS_INT32 iMsgType, IVS_VOID*  pParam, IVS_VOID*  pUser)
{
    CMediaChannelMgr::instance().DoExceptionCallBack(iMsgType, pParam, pUser);
}

/*
 * PlayerEventCallBack
 * ȫ���쳣����
 */
static IVS_VOID __SDK_CALLBACK PlayerEventCallBack(IVS_ULONG /*ulChannel*/, 
	    IVS_INT32 iMsgType, IVS_VOID*  pParam, IVS_VOID*  pUser)
{
    CMediaChannelMgr::instance().DoEventCallBack(iMsgType, pParam, pUser);
}

#ifndef WIN32
/*
 * GetModuleFilePath
 * ��ȡָ�����ļ�����Ŀ¼���ϼ�Ŀ¼
 * sModuleName [in] ���ļ����ƣ���"libIVS_Player.so"
 * sPath [out] ����Ŀ¼
 * unSize [int] sPath�ĳ���
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
		if (NULL == strstr(sLine, "r-xp") ||  NULL == pTmpFullDir || NULL == strstr(sLine, sModuleName))//r-xp:˽��ֻ����ִ������
		{
			continue;
		}

		//������ҵ�ģ��ֱ�ӷ��ڸ�Ŀ¼�£�����ʧ��
		pTmpModuleDir = strrchr(pTmpFullDir, '/');            
		if (pTmpModuleDir == pTmpFullDir)
		{
			break;
		}

		*pTmpModuleDir = '\0';
		//���ҵ�ģ��������libĿ¼�£���lib/libXXX.so
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
 * ��ʼ�����벥�ſ�
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_Init(const char* pLogPath)
{
    // ָ��Ŀ¼������̳�����
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
	//��ֹϵͳ�ر���ʾ����������״̬
    (void)SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_DISPLAY_REQUIRED);

    //��ֹϵͳ��������
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
 * ������벥�ſ�
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
    IVS_LogClean();  // �ر���־��������
}

/*
 * IVS_PLAY_GetChannel
 * ��ȡ����ͨ��
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
 * �ͷŲ���ͨ��
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
 * �����쳣�ص�
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK cbException, void* pUser)
{
    CMediaChannelMgr::instance().SetExceptionCallBack(cbException, pUser);
}

/*
 * IVS_PLAY_SetVideoDecodeType
 * ���ý���������
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
 * ��ȡ����������
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
 * ������Ƶ����������
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
 * ��ȡ��Ƶ����������
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
 * ����Ƶ��
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_OpenStream(IVS_ULONG ulChannel, IVS_UINT32 uiPlayeMode,
	                                                   IVS_UINT32 uiSourceType)
{
	IVS_LOG(IVS_LOG_DEBUG, "","IVS_PLAY_OpenStream in, Channel[%lu] PlayeMode[%u] SourceType[%u]", ulChannel, uiPlayeMode, uiSourceType);
	int iRet = IVS_FAIL;
	CMediaChannel* pMediaChannel = CMediaChannelMgr::instance().GetChannelbyID(ulChannel);
	if (NULL != pMediaChannel)
	{
        // ����ý�建������������;
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
 * �ر���Ƶ��
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
 * �������벥��
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
 * ֹͣ���벥��
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
 * ��ͣ����
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
 * �ָ�����
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
 * ���ò����ٶ�
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
 * ��������ǰ��
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
 * �������ź���
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
 * ������Ƶ����
 */
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_PlaySound(IVS_ULONG ulChannel)
{

    // add by wanglei 00165153:2013.6.9 DTS2013060606556
    if (g_bIsStartVoiceTalkback)  //�����Խ�������־
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
 * ֹͣ��Ƶ����
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
 * ��������
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
 * ��ȡ����
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
 * ����δ����֡���ݻص�
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
 * ���ý����֡���ݻص�
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
 * ���ò��Ŵ���
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
 * ���Ӳ��Ŵ���
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
 * ����ֲ����Ŵ���
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
 * ���¾ֲ��Ŵ󴰿ڲ���
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
 * ɾ�����Ŵ���
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
 * ���ò��Ŵ�����ʾ����
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
 * ����RTP������
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
 * ��ȡ��Ƶ���Ų���
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
 * ����Ƶ�ļ�
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
 * �����ļ�����ʱ�䷶Χ
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
 * ����\�ر�ˮӡУ��
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
 * ���ò��������Ȳ���
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
 * ��ȡ����ʱ��(�ļ�)
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
 * ��ȡ��ǰ����֡ʱ���
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
 * ���ò���ʱ������׼
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
 * ¼������ģʽ ����¼����ʼʱ��ͽ���ʱ��
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
 * ������Ƶ�����ȡ�����ģʽ
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
 * ��ȡӲ����������
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
        // TODO: NVDIAӲ��
        return DECODE_SW_HICILICON;
    }
}
#endif

/*
 * IVS_PLAY_GetDecodeAbility
 * ����Ӳ���־
 */
IVS_PLAYER_API IVS_VOID __SDK_CALL IVS_PLAY_SetHardwareDec(IVS_BOOL bHardwareDec)
{
    CMediaChannel::m_bHardwareDec = bHardwareDec;
}

/*
 * IVS_PLAY_ShowPlayStatis
 * ʹ����ʾ����ͳ��
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
 * ��������¼��
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
 * ֹͣ����¼��
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
 * ��ȡ��¼��ʱ��
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
 * ֹͣ��Ƶ����
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
 * ������Ƶ������Կ
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
 * ����ץ�ĵ���ͼƬ
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
 * ���ؾֲ��Ŵ�ץ�ĵ���ͼƬ
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
 * �������ſ���
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
 *  ���û�ͼ�ص�����
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
 *  ���û�ͼ�ص�����
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
 �������ƣ�IVS_PLAY_SetDelayFrameNum
 ����������������ʱ�����֡��
 ���������ulChannel      ͨ����;
           uDelayFrameNum ��ʱ�����֡��;
 ���������NA
 �� �� ֵ���ɹ�:IVS_SUCCEED;ʧ��:IVS_FAIL;
 *****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetDelayFrameNum(IVS_ULONG ulChannel, 
	                                                IVS_UINT32 uDelayFrameNum);

/*
 *  IVS_PLAY_SetDelayFrameNum
 *  ������ʱ�����֡��
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
 �������ƣ�IVS_PLAY_SetVoiceVolume()
 �������������������Խ�������
 ���������ulVolumeValue       ����ֵ����Χ�� MIN_WAVE_COEF(0) �� MAX_WAVE_COEF(100)
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceVolume(IVS_ULONG ulVolumeValue)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Voice Volume",
            "Set voice volume begin, volume value = %u.", ulVolumeValue);

	//��μ��
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
 �������ƣ�IVS_PLAY_GetVoiceVolume()
 ������������ȡ�����Խ�������
 �����������
 ���������ulVolumeValue       ����ֵ����Χ�� MIN_WAVE_COEF(0) �� MAX_WAVE_COEF(100)
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetVoiceVolume(IVS_UINT32* pVolumeValue)
{    
	//��μ��
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
 �������ƣ�IVS_PLAY_SetMicVolume()
 ����������������˷������
 ���������ulVolumeValue       ����ֵ����Χ�� MIN_WAVE_COEF(0) �� MAX_WAVE_COEF(100)
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetMicVolume(IVS_UINT32 uVolumeValue)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Mic Volume",
            "IVS dll:Set mic volume begin, volume value = %u.", uVolumeValue);

	//��μ��
    if (PLAYER_AUDIO_MAX_VOLUM < uVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Set Mic Volume",
                "Set Mic volume error, parameter is invalid.");
        //TODO ���������
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

	//��ʼ����������
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
 �������ƣ�IVS_PLAY_GetMicVolume
 ������������ȡ��˷������
 ���������pVolumeValue       ����ֵ����Χ�� MIN_WAVE_COEF(0) �� MAX_WAVE_COEF(100)
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetMicVolume(IVS_UINT32* pVolumeValue)
{    
	//��μ��
	if (NULL == pVolumeValue)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Get Mic Volume",
                "Get Mic volume error, parameter is invalide.");
        //TODO ���������
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

	//��ʼ����������
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
 �������ƣ�IVS_PLAY_SetVoiceProtocolType()
 ����������������������Э��
 ���������ulVoiceProtocolType             ��������Э��
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceProtocolType(IVS_UINT32 uVoiceProtocolType)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Get Mic Volume",
            "IVS dll:Set voice protocol type begin, voice protocol type = %u.",
		    uVoiceProtocolType);

    //��μ��
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
//  �������ƣ�IVS_PLAY_GetRecvVoiceAddr()
//  ������������ȡ���������ĵ�ַ��IP�Ͷ˿ںţ�
//  �����������
//  ���������pRecvVoicePort  ���������Ķ˿ں�
//  �� �� ֵ�� ULONG
// *****************************************************************************/
// IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_GetRecvVoiceAddr(IVS_ULONG* pRecvVoicePort)
// {
//     //��μ��
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
//  �������ƣ�IVS_PLAY_FreeRecvVoiceAddr()
//  �����������ͷŽ��������ĵ�ַ
//  ���������ulPort             ͨ����
//  �����������
//  �� �� ֵ�� ULONG
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
 �������ƣ�IVS_PLAY_SetVoiceDecoderType()
 ��������������������������
 ���������ulVoiceDecoderType             ������������
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_SetVoiceDecoderType(IVS_ULONG ulVoiceDecoderType)
{
	IVS_LOG(IVS_LOG_INFO, "IVS Set Voice Decoder Type",
            "Set voice decoder type begin, voice decoder type = %u.",
		    ulVoiceDecoderType);

    //��μ��
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
 �������ƣ�IVS_PLAY_StartVoiceTalkback()
 ������������ʼ�����Խ�         
 ���������pLocalIp    ����������IP��ַ
           ulLocalPort  ���������Ķ˿�
           pRemoteIp   Զ��������IP��ַ
           ulRemotePort Զ�������Ķ˿�
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StartVoiceTalkback(
                                                        IVS_CHAR* pLocalIp,
                                                        IVS_ULONG ulLocalPort,
                                                        IVS_CHAR* pRemoteIp,
                                                        IVS_ULONG ulRemotePort)
{
	//��μ��
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

    //�ж��Ƿ��Ѿ����������Խ�
    if (g_bIsStartVoiceTalkback)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error, voice talkback has opened.");
        return IVS_PLAYER_RET_VOICE_TALKBACK_STATUS_ERROR;
    }

#ifdef WIN32
    //���������Խ�ͨ��
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().CreateVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error,create voice channel fail");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }
    
    CMediaChannelMgr::instance().SetRecvStreamIp(pLocalIp);  //���浽Mgr��

    //����ͨ������
    pAudioChannel->m_strLocalIp = pLocalIp;    //lint !e1705
    pAudioChannel->m_ulLocalAudioRtpPort = ulLocalPort;//lint !e1705

    pAudioChannel->m_strSourceIp = pRemoteIp;
    pAudioChannel->m_ulSourceAudioRtpPort = ulRemotePort;

	pAudioChannel->SetOperationType(VOICE_TALKBACK); //����ҵ������

    //��ʼ��ͨ��
    long ulRet = pAudioChannel->InitChannel();
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Start Voice Talkback",
                "Start voice talk back error,initial audio channel fail.");
		CMediaChannelMgr::instance().DestroyVoiceChannel();
        return ulRet;
    }

    //�������������Խ�
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

    //�������������Խ�
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
 �������ƣ�IVS_PLAY_StopVoiceTalkback()
 ������������ʼ�����Խ�         
 �����������
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopVoiceTalkback()
{
    IVS_LOG(IVS_LOG_INFO, "IVS Stop Voice Talkback",
            "IVS dll:Stop voice talkback begin.");

#ifdef WIN32
    //ָ������ͨ����
    CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
    if (NULL == pAudioChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop voice talkback error, voice channel is null.");
        return IVS_FAIL;
    }

	//ֹͣ������Ƶ����
	long ulRet = pAudioChannel->StopSend();
	if (IVS_SUCCEED != ulRet)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop send audio data failed, Result code is %u.", ulRet);
	}
    
    //ֹͣ������Ƶ����
    ulRet = pAudioChannel->StopReceive();
    if (IVS_SUCCEED != ulRet)
    {
        IVS_LOG(IVS_LOG_ERR, "IVS Stop Voice Talkback",
                "Stop audio revcive failed, Result code is %u.", ulRet);
        //ʧ�ܼ���ִ��
    }
     
    //�ͷ���Ƶͨ����Դ
    pAudioChannel->ReleaseChannel();
	//CMediaChannelMgr::instance().DestroyVoiceChannel();//ֻ�᷵��IVS_SUCCEED

    g_bIsStartVoiceTalkback = false;

#endif

    IVS_LOG(IVS_LOG_INFO, "IVS Stop Voice Talkback",
            "IVS dll:Stop voice talkback end.");

	return IVS_SUCCEED;
}


/*****************************************************************************
 �������ƣ�IVS_PLAY_StartVoiceBroadcast()
 ������������ʼ�����㲥        
 ���������pLocalIp        ����������IP��ַ
           ulLocalPort     ���������Ķ˿�
           pDeviceList     �����㲥�豸�б�
           ulBroadcastType �����㲥����(0Ϊ�ļ��㲥��1Ϊʵʱ�㲥)
           pFileName       �����㲥�ļ���(����·��)
           bCycle          �Ƿ�ѭ������(0��ѭ������ 1ѭ������,ʵʱ�㲥��֧��ѭ��)
 �����������
 �� �� ֵ��ULONG
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
    //��μ��
	if ((NULL == pLocalIp) || (NULL == pRemoteIp)) 
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, parameter is invalid.");
		return IVS_PARA_INVALID;
	}

#ifdef WIN32
	//���ж��Ƿ��Ѿ����������Խ�
	if (g_bIsStartVoiceTalkback)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
            "Start voice broadcast error, because the voice talk has opened.");
		return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
	}

	IVS_LOG(IVS_LOG_INFO, "IVS PLAY StartVoiceBroadcast",
		    "Start voice broadcast begin, local ip = %s, local port = %u.,RemoteIp = %s, RemotePort = %u.", 
			pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);


	//�ж��Ƿ��Ѿ����������㲥�����Ѿ������㲥������AddBroadcast��ӹ㲥�豸�б�	
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

	//��û�п��������㲥����Ҫ���������㲥ͨ������һ�ι㲥������Ҫ��ӹ㲥�豸�б�
	CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().CreateVoiceChannel();
	if (NULL == pAudioChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
            "Start voice broadcast error, create voice broadcsat channel fail.");
		return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
	}

	//����ͨ������
	pAudioChannel->m_strLocalIp = pLocalIp;    //lint !e1705
	pAudioChannel->m_ulLocalAudioRtpPort = ulLocalPort;//lint !e1705

	pAudioChannel->SetOperationType(VOICE_BROADCAST); //����ҵ������

	// �ļ��㲥,֧��ѭ��
	if (VOICE_FILE_BROADCAST == ulBroadcastType)
	{
		//���ù㲥����
		pAudioChannel->SetBroadcastType(VOICE_FILE_BROADCAST);
		pAudioChannel->SetBroadcastFileName(pFileName);
        pAudioChannel->SetBroadcastCycle(bCycle);

		//��ʼ��ͨ��
		long ulRet = pAudioChannel->InitChannel();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Start voice broadcast error,initial audio channel fail.");
			//�ͷ���Ƶͨ����Դ			
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}

		//��ӹ㲥�豸�б�
		ulRet = pAudioChannel->AddBroadcastDevice(pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Add voice broadcast Device error.");
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;  //���㲥״̬��Ϊδ����
			return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
		}

        //������������,��ҪΪ�˽���NAT��Խ��
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

		//��ʼ�ļ��㲥
		ulRet = pAudioChannel->StartFileSend();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                "Start voice broadcast error, start send voice data failed, error code is %u.", ulRet);
			
			//�ͷ���Ƶͨ����Դ
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}
	}
	else   //ʵʱ�㲥,��֧��ѭ��
	{
		//���ù㲥����
		pAudioChannel->SetBroadcastType(VOICE_REAL_BROADCAST);		

		//��ʼ��ͨ��
		long ulRet = pAudioChannel->InitChannel();
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Start voice broadcast error,initial audio channel fail.");

			//�ͷ���Ƶͨ����Դ			
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;
			return ulRet;
		}

		//��ӹ㲥�豸�б�
		ulRet = pAudioChannel->AddBroadcastDevice(pLocalIp, ulLocalPort, pRemoteIp, ulRemotePort);
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StartVoiceBroadcast",
                    "Add voice broadcast Device error.");
			pAudioChannel->ReleaseChannel();
			CMediaChannelMgr::instance().DestroyVoiceChannel();
			g_bIsStartVoiceBroadcast = false;  //���㲥״̬��Ϊδ����
			return IVS_PLAYER_RET_VOICE_BROADCAST_STATUS_ERROR;
		}

        //������������,��ҪΪ�˽���NAT��Խ��
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

		//��ʼ����ʵʱ�㲥
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
	g_bIsStartVoiceBroadcast = true;  //���������㲥������־

	IVS_LOG(IVS_LOG_INFO, "IVS PLAY StartVoiceBroadcast",
            "IVS dll:Start voice broadcast end.");

    return IVS_SUCCEED;
}

/*****************************************************************************
 �������ƣ�IVS_PLAY_StopVoiceBroadcast()
 ����������ֹͣ�����㲥         
 ���������pRemoteIp    �����豸IP
         ulRemotePort �����豸�˿ں�
 �����������
 �� �� ֵ�� ULONG
*****************************************************************************/
IVS_PLAYER_API IVS_INT32 __SDK_CALL IVS_PLAY_StopVoiceBroadcast(
                                                        IVS_CHAR* pRemoteIp,
                                                        IVS_ULONG ulRemotePort)
{
    IVS_LOG(IVS_LOG_INFO, "IVS PLAY StopVoiceBroadcast",
            "IVS dll:Stop voice broadcast begin.");

	//��μ��
	if ((NULL == pRemoteIp)
		|| 0 == strlen(pRemoteIp))
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, parameter is invalid.");
		return IVS_PARA_INVALID;
	}
#ifdef WIN32
	//��ȡָ�������㲥ͨ����
	CAudioChannel* pAudioChannel = CMediaChannelMgr::instance().GetVoiceChannel();
	if (NULL == pAudioChannel)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, voice channel is null.");
		return IVS_PLAYER_RET_CHANNEL_NOT_EXIST_ERROR;
	}

	//ɾ���㲥�豸��Ϣ
	long ulRet = pAudioChannel->DelBroadcastDevice(pRemoteIp, ulRemotePort);
	if (IVS_SUCCEED != ulRet)
	{
		IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                "Stop voice broadcast error, delete the broadcast device false.");
		return ulRet;
	}

	int nDeviceNum = pAudioChannel->GetBroadcastNum();
	//�ж�ʧ���б��Ƿ�Ϊ�գ����Ϊ�գ���Ҫ�ͷ�ͨ����Դ��ֹͣ�㲥
	if (0 == nDeviceNum)
	{
		//ֹͣ���͹㲥��Ƶ����
				
		//��ȡ�㲥���ͣ��ļ���ʵʱ
		unsigned long ulBroadcastType = pAudioChannel->GetBroadcastType();        //lint !e732
		if (VOICE_FILE_BROADCAST == ulBroadcastType) //�ļ�
		{
			ulRet = pAudioChannel->StopFileSend();
			pAudioChannel->ReleaseFileInputSource();
		}
		else //ʵʱ
		{
			ulRet = pAudioChannel->StopSend();
			pAudioChannel->ReleaseChannel();
		}
		if (IVS_SUCCEED != ulRet)
		{
			IVS_LOG(IVS_LOG_ERR, "IVS PLAY StopVoiceBroadcast",
                    "Stop send audio data failed, Result code = %u.", ulRet);
		}

		//������Ƶͨ��
		//CMediaChannelMgr::instance().DestroyVoiceChannel();//ֻ�᷵��IVS_SUCCEED

		g_bIsStartVoiceBroadcast = false;

		IVS_LOG(IVS_LOG_INFO, "IVS PLAY StopVoiceBroadcast",
                "IVS dll:Stop voice broadcast end.");
	}
#endif
	return IVS_SUCCEED;
}

/*****************************************************************************
 �������ƣ�IVS_PLAY_GetSourceBufferRemain()
 ������������ȡδ����֡������ʣ���֡��         
 �����������
 �����������
 �� �� ֵ�� ULONG
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
 *  �����¼��ص�����
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
 * ����ý������
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
 * ���ô�ֱͬ��
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
 * ������ر�־
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
