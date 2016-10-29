/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecodeHandler.h
//  文件说明:   根据厂商类型，通调用解码器工厂类创建出对应的
	           解码器操作句柄，供上层调用
//  版    本:   IVS V100R001C02
//  作    者:   z90003203
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明： 
*******************************************************************************/
#ifndef _DECODEHANDLER_H
#define _DECODEHANDLER_H

// #include "BaseDecoder.h"
#include "DecoderIVS.h"

class DecodeHandler
{

public:
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
    DecodeHandler();
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
    virtual ~DecodeHandler();

public:
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
    int CreateDecoder();

    /*************************************************
    Function:        DecodeHandler::Release
    Description:     释放一个解码对象
    Calls:           // 被本函数调用的函数清单（建议描述）
    Called By:       // 调用本函数的函数清单（建议描述）
    Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
    Output:          // 对输出参数的说明
    Return:          // 函数返回值的说明
    int
    Others:          // 其它说明
    *************************************************/
    int ReleaseDecoder();

    /*****************************************************************************
    函数名称：Decoder_SetDecoderCB
    功能描述：设置解码后数据回调函数
    输入参数：hDecoder     解码器句柄
    fnDecoderCB  YUV回调函数
    pUserParam   用户参数
    输出参数：NA
    返 回 值：int
    *****************************************************************************/
    int SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void *pUserParam);


    /*****************************************************************************
    函数名称：Decoder_OpenStream
    功能描述：打开流
    输入参数：hDecoder     解码器句柄
              pVideoParam  视频参数
              pAudioParam  音频参数
              uBufferSize  一帧的缓冲大小
              uBufferCount 帧缓冲个数
    输出参数：NA
    返 回 值：int
    *****************************************************************************/
    int OpenStream(VIDEO_PARAM *pVideoParam,
                   AUDIO_PARAM *pAudioParam,
                   unsigned int uLowerLimit,
                   unsigned int uUpperLimit,
				   IVS_UINT32 uBufferCount,
				   IVS_UINT32 uExBufCount);


    /*****************************************************************************
    函数名称：Decoder_CloseStream
    功能描述：关闭流
    输入参数：hDecoder     解码器句柄
    输出参数：NA
    返 回 值：int
    *****************************************************************************/
    int CloseStream();


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
    int Start(HWND hWnd);

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
    int Stop();

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
    int SendVideoFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                       char *pFrame,
                       unsigned int uFrameSize);

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
    int SendAudioFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                       char *pFrame,
                       unsigned int uFrameSize);


    /*****************************************************************************
    函数名称：SetExceptionCallback
    功能描述：设置解码库事件回调函数
    输入参数：hDecoder     解码器句柄
    fnDecExceptionCB 解码事件回调函数
    pUserParam   用户参数
    输出参数：NA
    返 回 值：int
    *****************************************************************************/
    int SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void *pUserParam);


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
    int StartAudio() const;

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
    int ShutDownAudio() const;

    /*************************************************
    Function:        DecodeHandler::SetAudioVolume
    Description:     设置音量大小
    Calls:           // 被本函数调用的函数清单（建议描述）
    Called By:       // 调用本函数的函数清单（建议描述）
    Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
    long volume ：音量值
    Output:          // 对输出参数的说明
    Return:          // 函数返回值的说明
    int
    Others:          // 其它说明
    *************************************************/
    int SetAudioVolume(long volume);

    /*************************************************
    Function:        DecodeHandler::GetAudioVolume
    Description:     得到音量大小
    Calls:           // 被本函数调用的函数清单（建议描述）
    Called By:       // 调用本函数的函数清单（建议描述）
    Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
    Output:          // 对输出参数的说明
    long &lVolume ：得到音量值
    Return:          // 函数返回值的说明
    int
    Others:          // 其它说明
    *************************************************/
    int GetAudioVolume(long &lVolume);

    // 	int InsertBuf(unsigned char *buf,
    // 						int buflen);
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
    int GetVideoResolution(int &iWidth, int &iHeight);

    /*****************************************************************************
    函数名称：Decoder_GetSourceBufferRemain
    功能描述：获取未解码帧缓冲区剩余的帧数
    输入参数：hDecoder     解码器句柄
    uBufferCount 帧缓冲个数
    输出参数：NA
    返 回 值：IVS_INT32
    *****************************************************************************/
    int GetSourceBufferRemain(unsigned int &uBufferCount) const;

    /*****************************************************************************
    函数名称：DecodeHandler::SetWaterMarkState
    功能描述：设置水印校验开始/停止
    输入参数：bIsCheckWaterMark 开始为true,停止为false
    输出参数：无
    返 回 值：无
    *****************************************************************************/
    int SetWaterMarkState(BOOL bIsCheckWaterMark);

    /*****************************************************************************
    函数名称：Decoder_ResetBuffer
    功能描述：清空缓存的数据;
    输入参数：无
    输出参数：无
    返 回 值：成功:IVS_SUCCEED 失败:错误码
    *****************************************************************************/
    int ResetBuffer();

    /*****************************************************************************
    函数名称：SetDelayFrameNum
    功能描述：设置延时解码的帧数;
    输入参数：uDelayFrameNum 延时解码的帧数
    输出参数：无
    返 回 值：成功:IVS_SUCCEED 失败:错误码
    *****************************************************************************/
    int SetDelayFrameNum(unsigned int uDelayFrameNum);

    /*****************************************************************************
    函数名称：StepFrames
    功能描述：设置单步解码的帧数;
    输入参数：uStepFrameNum 单步解码的帧数
    输出参数：无
    返 回 值：成功:IVS_SUCCEED 失败:错误码
    *****************************************************************************/
    int StepFrames(unsigned int uStepFrameNum);

    /*****************************************************************************
    函数名称：SetEventCallback
    功能描述：设置解码库事件回调函数
    输入参数：fnDecEventCB 解码事件回调函数
              pUserParam   用户参数
    输出参数：NA
    返 回 值：int
    *****************************************************************************/
    int SetEventCallback(PLAYER_EVENT_CALLBACK fnDecEventCB, void* pUserParam);

	
	int  MarkEoS(bool IsEos);
	
	bool IsCancel();

    void StopNotifyBuffEvent();

    void ReSetFlowContrlFlag();
private:

    CDecoderIVS *m_pDecoder;
    int m_nWidth;
    int m_nHeight;
};

#endif // !defined(AFX_DECODEHANDLER_H__F8708A5D_8934_4F3C_8014_3D53D6011E39__INCLUDED_)
