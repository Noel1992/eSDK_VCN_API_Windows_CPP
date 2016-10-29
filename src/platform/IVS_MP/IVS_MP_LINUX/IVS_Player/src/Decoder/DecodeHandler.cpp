/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecodeHandler.cpp
//  文件说明:   根据厂商类型，通调用解码器工厂类创建出对应的
                解码器操作句柄，供上层调用
//  版    本:   IVS V100R001C02
//  作    者:   z90003203
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明： 
*******************************************************************************/

// DecodeHandler.cpp: implementation of the DecodeHandler class.
//
//////////////////////////////////////////////////////////////////////

//#include "OmniDecoder.h"
#include "DecodeHandler.h"
#include "DecoderFactory.h"


// 记录打开音频的解码器句柄
//CBaseDecoder* DecodeHandler::m_pAudioDecoder = NULL;

// 打开音频的解码器的全局锁，保证SDK同时只会打开一路音频
//CRITICAL_SECTION  g_csAudioDecoder;

// 音频解码器的全局锁的标志位
//bool g_AudioDecodeInitialized = false;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*************************************************
Function:        DecodeHandler::DecodeHandler
Description:     构造函数
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
Others:          // 其它说明
*************************************************/
DecodeHandler::DecodeHandler()
: m_pDecoder(NULL)
, m_nWidth(352)//默认视频宽为352
, m_nHeight(288)//默认视频长为288
{
//     if (!g_AudioDecodeInitialized)
//     {
//         InitializeCriticalSection(&g_csAudioDecoder);
//         g_AudioDecodeInitialized = true;
//     }
}

/*************************************************
Function:        DecodeHandler::~DecodeHandler
Description:     析构函数
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
Others:          // 其它说明
*************************************************/
DecodeHandler::~DecodeHandler()
{
    try
    {
        if(NULL != m_pDecoder)
        {
            m_pDecoder->ReleaseDecoder();

            delete m_pDecoder;
            m_pDecoder = NULL;
        }
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "Decode Handler",
                "delete Decoder throw exception!"); //lint !e1551
    }

//     m_pDecoder = NULL;
}

/*************************************************
Function:        DecodeHandler::CreateDecoder
Description:     创建一个解码对象
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
                    int nEncoderType ： 解码类型
                    int nVideoFormat ： 视频格式
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                     int
Others:          // 其它说明
*************************************************/
int DecodeHandler::CreateDecoder()
{
    if (NULL == m_pDecoder)
    {
        m_pDecoder = DecoderFactory::CreateDecoder();

        if (NULL == m_pDecoder)
        {
            return IVS_PLAYER_RET_CREATE_DECODER_ERROR;
        }
    }
	
	return IVS_SUCCEED;
}


/*************************************************
Function:        DecodeHandler::ShutDown
Description:     关掉解码器
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::ReleaseDecoder()
{
	if(NULL != m_pDecoder)
	{
        m_pDecoder->ReleaseDecoder();

        delete m_pDecoder;
        m_pDecoder = NULL;

        return IVS_SUCCEED;
	}

	return IVS_PLAYER_RET_ORDER_ERROR;
}


/*************************************************
Function:        DecodeHandler::OpenStream
Description:     开流
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::OpenStream(VIDEO_PARAM *pVideoParam,
                              AUDIO_PARAM *pAudioParam,
                              unsigned int uLowerLimit,
                              unsigned int uUpperLimit,
							  IVS_UINT32 uBufferCount,
							  IVS_UINT32 uExBufCount)
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Open Stream", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    return m_pDecoder->OpenStream(pVideoParam, pAudioParam, uLowerLimit, uUpperLimit, uBufferCount, uExBufCount);
}



/*************************************************
Function:        DecodeHandler::CloseStream
Description:     关流
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::CloseStream()
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Close Stream", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    return m_pDecoder->CloseStream();
}



/*************************************************
Function:        DecodeHandler::Start
Description:     启动解码器
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void *pUserParam)
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Set DecoderCB", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    m_pDecoder->SetDecoderCB(fnDecoderCB, pUserParam);

    return IVS_SUCCEED;
}


/*************************************************
Function:        DecodeHandler::Start
Description:     启动解码器
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::Start(HWND hWnd)
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Start", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    return m_pDecoder->Start(hWnd);
}

/*************************************************
Function:        DecodeHandler::Stop
Description:     停止解码器
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::Stop()
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Stop", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    m_pDecoder->Stop();

    return IVS_SUCCEED;
}



//由SendVideoFrame拉起视频解码器
/*************************************************
Function:        DecodeHandler::SendVideoFrame
Description:     发送一视频帧
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
                    pRawFrameInfo  未解码的帧信息
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::SendVideoFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                  char *pFrame,
                                  unsigned int uFrameSize)
{
    if(NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "DecodeHandler::SendVideoFrame()",
            "DecodeHandler::m_pobjDecoder error!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    return m_pDecoder->SendVideoFrame(pRawFrameInfo, pFrame, uFrameSize);
}

//由SendAudioFrame拉起解码器
/*************************************************
Function:        DecodeHandler::SendAudioFrame
Description:     发送一音频帧
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
                    pRawFrameInfo  未解码的帧信息
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::SendAudioFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                  char *pFrame,
                                  unsigned int uFrameSize)
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Send Audio Frame", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

    return m_pDecoder->SendAudioFrame(pRawFrameInfo, pFrame, uFrameSize);
}


/*************************************************
Function:        DecodeHandler::StartAudio
Description:     开启音频
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::StartAudio() const
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Start Audio", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

// 	m_pDecoder->StartAudio();

    // add for问题单 A14D04698 解码多路音频时，DEBUG下会中断
    // 限制用户只能打开一路，并保证多线程下正常 [11/17/2010 z90003203]
//     EnterCriticalSection(&g_csAudioDecoder);
// 
//     if (DecodeHandler::m_pAudioDecoder != NULL
//         && DecodeHandler::m_pAudioDecoder != m_pDecoder)
//     {
//         DecodeHandler::m_pAudioDecoder->ShutDownAudio();
//     }
// 
//     DecodeHandler::m_pAudioDecoder = m_pDecoder;
// 
//     LeaveCriticalSection(&g_csAudioDecoder);
    // end add

	return IVS_SUCCEED;
}
/*************************************************
Function:        DecodeHandler::ShutDownAudio
Description:     关掉音频
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::ShutDownAudio() const
{
// 	if(NULL == m_pDecoder)
// 	{
// 		IVS_LOG(IVS_LOG_ERR, "DecodeHandler::ShutDownAudio()",
//              "DecodeHandler::m_pobjDecoder error!");
// 		return int_ERROR;
// 	}
// 
// 	m_pDecoder->ShutDownAudio();
// 
//     // add for问题单 A14D04698 解码多路音频时，DEBUG下会中断
//     // 限制用户只能打开一路，并保证多线程下正常 [11/17/2010 z90003203]
//     EnterCriticalSection(&g_csAudioDecoder);
// 
//     if (m_pDecoder == DecodeHandler::m_pAudioDecoder)
//     {
//         DecodeHandler::m_pAudioDecoder = NULL;
//     }
// 
//     LeaveCriticalSection(&g_csAudioDecoder);
//     // end add
// 
// 	return int_OK
    return IVS_SUCCEED;

}



/*************************************************
Function:        DecodeHandler::GetVideoResolution
Description:     得到视频帧大小
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
                    int &nWidth   ：视频帧宽
                    int &nHeight  ：视频帧长
Return:          // 函数返回值的说明
                    int
Others:          // 其它说明
*************************************************/
int DecodeHandler::GetVideoResolution(int &iWidth, int &iHeight)
{
    if(NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Video Resolution", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    return m_pDecoder->GetVideoResolution(iWidth, iHeight);
}

/*****************************************************************************
函数名称：DecodeHandler::SetExceptionCallback
功能描述：设置解码库事件回调函数
输入参数：hDecoder     解码器句柄
fnDecEventCB 解码事件回调函数
pUserParam   用户参数
输出参数：NA
返 回 值：int
*****************************************************************************/
int DecodeHandler::SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void *pUserParam)
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Set Exception Callback", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    m_pDecoder->SetExceptionCallback(fnDecExceptionCB, pUserParam);

    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：DecodeHandler::GetSourceBufferRemain
功能描述：获取未解码帧缓冲区剩余的帧数
输入参数：hDecoder     解码器句柄
输出参数：uBufferCount 帧缓冲个数
返 回 值：int
*****************************************************************************/
int DecodeHandler::GetSourceBufferRemain(unsigned int &uBufferCount) const
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "DecodeHandler::GetSourceBufferRemain()",
            "DecodeHandler::m_pobjDecoder error!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    return m_pDecoder->GetSourceBufferRemain(uBufferCount);
}

/*****************************************************************************
函数名称：DecodeHandler::SetWaterMarkState
功能描述：设置水印校验开始/停止
输入参数：bIsCheckWaterMark 开始为true,停止为false
输出参数：无
返 回 值：无
*****************************************************************************/
int DecodeHandler::SetWaterMarkState(BOOL bIsCheckWaterMark)
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Set WaterMark State", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    m_pDecoder->SetWaterMarkState(bIsCheckWaterMark);

    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：Decoder_ResetBuffer
功能描述：清空缓存的数据;
输入参数：hDecoder     解码器句柄
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
int DecodeHandler::ResetBuffer()
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Reset Buffer", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    return m_pDecoder->ResetBuffer();
}

/*****************************************************************************
函数名称：SetDelayFrameNum
功能描述：设置延时解码的帧数;
输入参数：uDelayFrameNum 延时解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
int DecodeHandler::SetDelayFrameNum(unsigned int uDelayFrameNum)
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Set Delay FrameNum", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    m_pDecoder->SetDelayFrameNum(uDelayFrameNum);
    return IVS_SUCCEED;
}

/*****************************************************************************
函数名称：StepFrames
功能描述：设置单步解码的帧数;
输入参数：uStepFrameNum 单步解码的帧数
输出参数：无
返 回 值：成功:IVS_SUCCEED 失败:错误码
*****************************************************************************/
int DecodeHandler::StepFrames(unsigned int uStepFrameNum)
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Step Frames", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    return m_pDecoder->StepFrames(uStepFrameNum);
}

/*****************************************************************************
函数名称：DecodeHandler::SetEventCallback
功能描述：设置解码库事件回调函数
输入参数：fnDecEventCB 解码事件回调函数
          pUserParam   用户参数
输出参数：NA
返 回 值：int
*****************************************************************************/
int DecodeHandler::SetEventCallback(PLAYER_EVENT_CALLBACK fnDecEventCB, void* pUserParam)
{
    if (NULL == m_pDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Set Event Callback", "Decoder must be init first!");
        return IVS_PLAYER_RET_ORDER_ERROR;
    }

    m_pDecoder->SetEventCallback(fnDecEventCB, pUserParam);

    return IVS_SUCCEED;
}


int DecodeHandler::MarkEoS(bool IsEOS)
{
	if (NULL == m_pDecoder)
	{
		return IVS_FAIL; 
	}
	m_pDecoder->SetEoS(IsEOS);
	return IVS_SUCCEED;
}

bool DecodeHandler::IsCancel()
{
	if (NULL == m_pDecoder)
	{
		return true; 
	}
	return m_pDecoder->IsCancel();
}//lint !e1762

void DecodeHandler::StopNotifyBuffEvent()
{
    if (NULL == m_pDecoder)
    {
        return; 
    }
    m_pDecoder->StopNotifyBuffEvent();
}

void DecodeHandler::ReSetFlowContrlFlag()
{
    if (NULL == m_pDecoder)
    {
        return; 
    }
    m_pDecoder->ReSetFlowContrlFlag();
}

