/********************************************************************
filename    : DecoderAPI.cpp
author      : z90003203
created     : 2012/11/23
description : ����Ƶ����ģ��ӿڷ�װ
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/10/31 ��ʼ�汾
*********************************************************************/

#include "DecoderAPI.h"
#include "DecodeHandler.h"
#include "IVSPlayerDataType.h"
#include "IVSCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus 

/*****************************************************************************
�������ƣ�Decoder_GetHandle
�����������������������ɹ����ؽ��������
���������NA  
���������NA
�� �� ֵ��IVS_HANDLE
*****************************************************************************/
IVS_HANDLE Decoder_GetHandle()
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_GetHandle", "Enter");
    DecodeHandler* pDecodeHandler = NULL;

    try
    {
        pDecodeHandler = new DecodeHandler();
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
            "Decoder Create new DecodeHandler throw exception!");
		pDecodeHandler = NULL;
    }

    if(NULL == pDecodeHandler)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
                "Decoder Create pDecodeHandler error!");

        return NULL;
    }

    int iRet = pDecodeHandler->CreateDecoder();
    if(IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pDecodeHandler);

        IVS_LOG(IVS_LOG_ERR, "Decoder GetHandle",
                "Decoder Create CreateDecoder error!");

        return NULL;
    }

    IVS_HANDLE phDecodeHandler = reinterpret_cast<IVS_HANDLE>(pDecodeHandler);
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_GetHandle", "Leave Handle 0x%x", phDecodeHandler);

    return phDecodeHandler;
}


/*****************************************************************************
�������ƣ�Decoder_FreeHandle
�����������ͷ�ָ����������Դ
���������hDecoder Decoder_GetHandle�д����Ľ��������
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_VOID Decoder_FreeHandle(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_FreeHandle", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle",
                "Decoder Release hDecoder error!");
        return;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int nRet = pDecodeHandler->Stop();
    if(IVS_SUCCEED != nRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle", "pobjDecodeHandler Stop error!");
    }

    nRet = pDecodeHandler->ReleaseDecoder();
    if(IVS_SUCCEED != nRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder FreeHandle", "pobjDecodeHandler ReleaseDecoder error!");
    }

    IVS_DELETE(pDecodeHandler);
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_FreeHandle", "Leave");
}


/*****************************************************************************
�������ƣ�Decoder_OpenStream
��������������
���������hDecoder     ���������
          pVideoParam  ��Ƶ����
          pAudioParam  ��Ƶ����
          iBufferSize  һ֡�Ļ����С
          iBufferCount ֡�������
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_OpenStream(IVS_HANDLE hDecoder,
                             VIDEO_PARAM* pVideoParam,
                             AUDIO_PARAM* pAudioParam,
                             IVS_UINT32 uLowerLimit,
                             IVS_UINT32 uUpperLimit,
							 IVS_UINT32 uBufferCount,
							 IVS_UINT32 uExBufCount)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_OpenStream",
			"Enter Handle 0x%x VideoParam 0x%x AudioParam 0x%x Limit %u %u BufferCount %u",
			hDecoder, pVideoParam, pAudioParam, uLowerLimit, uUpperLimit, uBufferCount);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder OpenStream",
				"Decoder OpenStream hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->OpenStream(pVideoParam,
                                                pAudioParam,
                                                uLowerLimit,
                                                uUpperLimit,
												uBufferCount, 
												uExBufCount);

    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder OpenStream",
            "Decoder OpenStream OpenStream error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_OpenStream", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_CloseStream
�����������ر���
���������hDecoder     ���������
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_CloseStream(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_CloseStream", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder CloseStream",
            "Decoder CloseStream hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->CloseStream();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder CloseStream",
            "Decoder CloseStream CloseStream error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_CloseStream", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_SetDecoderCB
�������������ý�������ݻص�����
���������hDecoder     ���������
          fnDecoderCB  YUV�ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetDecoderCB(IVS_HANDLE hDecoder,
                               DECODER_CALLBACK fnDecoderCB,
                               void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDecoderCB",
        "Enter Handle 0x%x DECODER_CALLBACK 0x%x UserParam 0x%x",
        hDecoder, fnDecoderCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDecoderCB",
            "Decoder SetDecoderCB hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SetDecoderCB(fnDecoderCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDecoderCB",
            "Decoder SetDecoderCB SetDecoderCB error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDecoderCB", "Leave");
    return IVS_SUCCEED;
}



/*****************************************************************************
 �������ƣ�Decoder_Start
 ������������ʼ����
 ���������hDecoder     ���������  
 ���������NA
 �� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_Start(IVS_HANDLE hDecoder, HWND hWnd)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Start", "Enter Handle 0x%x HWND 0x%x",
            hDecoder, hWnd);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Start",
            "Decoder Start hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->Start(hWnd);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Start",
            "Decoder Start Start error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Start", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_Stop
����������ֹͣ����
���������hDecoder     ���������  
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_Stop(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Stop", "Enter Handle 0x%x", hDecoder);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Stop",
            "Decoder Stop hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->Stop();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder Stop",
            "Decoder Stop Stop error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_Stop", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_SendVideoFrame
������������һ֡��Ƶ֡�������
���������hDecoder     ���������
          pRawFrameInfo  δ�����֡��Ϣ
          pFrame        ֡��ַ
          uFrameSize    ֡����
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SendVideoFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendVideoFrame",
            "Enter Handle 0x%x pRawFrameInfo 0x%x pFrame 0x%x uFrameSize %u",
            hDecoder, pRawFrameInfo, pFrame, uFrameSize);

    if(NULL == hDecoder
       || NULL == pFrame
       || 0 == uFrameSize)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendVideoFrame",
            "Decoder SendVideoFrame hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SendVideoFrame(pRawFrameInfo,
                                                    pFrame,
                                                    uFrameSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendVideoFrame",
            "Decoder SendVideoFrame SendVideoFrame error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendVideoFrame", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_SendAudioFrame
������������һ֡��Ƶ֡�������
���������hDecoder      ���������
          pRawFrameInfo δ�����֡��Ϣ
          pFrame        ֡��ַ
          uFrameSize    ֡����
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SendAudioFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO* pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendAudioFrame",
        "Enter Handle 0x%x pRawFrameInfo 0x%x pFrame 0x%x uFrameSize %u",
        hDecoder, pRawFrameInfo, pFrame, uFrameSize);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendAudioFrame",
            "Decoder SendAudioFrame hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    IVS_INT32 iRet = pDecodeHandler->SendAudioFrame(pRawFrameInfo,
                                                    pFrame,
                                                    uFrameSize);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SendAudioFrame",
            "Decoder SendAudioFrame SendAudioFrame error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SendAudioFrame", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_SetDecEventCB
�������������ý�����¼��ص�����
���������hDecoder     ���������
          fnDecEventCB �����¼��ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetExceptionCallback(IVS_HANDLE hDecoder, PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetExceptionCallback",
            "Enter Handle 0x%x PLAYER_EXCEPTION_CALLBACK 0x%x UserParam 0x%x",
            hDecoder, fnDecExceptionCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetExceptionCallback",
            "Decoder SetExceptionCallback hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetExceptionCallback(fnDecExceptionCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetExceptionCallback",
            "Decoder SetExceptionCallback SetExceptionCallback error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetExceptionCallback", "Leave");
    return IVS_SUCCEED;
}


/*****************************************************************************
�������ƣ�Decoder_GetSourceBufferRemain
������������ȡδ����֡������ʣ���֡��
���������hDecoder     ���������
���������uBufferCount ֡�������
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_GetSourceBufferRemain(IVS_HANDLE hDecoder, IVS_UINT32 &uBufferCount)
{
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetSourceBufferRemain",
            "Decoder GetSourceBufferRemain hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->GetSourceBufferRemain(uBufferCount);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder GetSourceBufferRemain",
            "Decoder GetSourceBufferRemain GetSourceBufferRemain error!");
        return iRet;
    }

    return IVS_SUCCEED;
}//lint !e954

/*****************************************************************************
�������ƣ�Decoder_SetWaterMarkState
��������������ˮӡУ�鿪ʼ/ֹͣ
���������bIsCheckWaterMark ��ʼΪtrue,ֹͣΪfalse
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetWaterMarkState(IVS_HANDLE hDecoder, IVS_BOOL bIsCheckWaterMark)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetWaterMarkState",
            "Enter Handle 0x%x WaterMark Flag %d", hDecoder, bIsCheckWaterMark);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetWaterMarkState",
            "Decoder SetWaterMarkState hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetWaterMarkState(bIsCheckWaterMark);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetWaterMarkState",
            "Decoder SetWaterMarkState SetWaterMarkState error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetWaterMarkState", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
�������ƣ�Decoder_ResetBuffer
������������ջ��������;
���������hDecoder     ���������
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_ResetBuffer(IVS_HANDLE hDecoder)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_ResetBuffer", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder ResetBuffer",
            "Decoder ResetBuffer hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->ResetBuffer();
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder ResetBuffer",
            "Decoder ResetBuffer ResetBuffer error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_ResetBuffer", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
�������ƣ�Decoder_SetDelayFrameNum
����������������ʱ�����֡��;
���������hDecoder       ���������
          uDelayFrameNum ��ʱ�����֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetDelayFrameNum(IVS_HANDLE hDecoder, IVS_UINT32 uDelayFrameNum)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDelayFrameNum", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDelayFrameNum",
            "Decoder SetDelayFrameNum hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetDelayFrameNum(uDelayFrameNum);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetDelayFrameNum",
            "Decoder SetDelayFrameNum SetDelayFrameNum error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetDelayFrameNum", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
�������ƣ�Decoder_StepFrames
�������������õ��������֡��;
���������hDecoder       ���������
          uStepFrameNum ���������֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_StepFrames(IVS_HANDLE hDecoder, IVS_UINT32 uStepFrameNum)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_StepFrames", "Enter Handle 0x%x", hDecoder);
    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder StepFrames",
            "Decoder StepFrames hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->StepFrames(uStepFrameNum);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder StepFrames",
            "Decoder StepFrames StepFrames error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_StepFrames", "Leave");
    return IVS_SUCCEED;
}

/*****************************************************************************
�������ƣ�Decoder_SetDecEventCB
�������������ý�����¼��ص�����
���������hDecoder     ���������
          fnDecEventCB �����¼��ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ��IVS_INT32
*****************************************************************************/
IVS_INT32 Decoder_SetEventCallback(IVS_HANDLE hDecoder, PLAYER_EVENT_CALLBACK fnDecEventCB, void* pUserParam)
{
    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetEventCallback",
            "Enter Handle 0x%x PLAYER_EVENT_CALLBACK 0x%x UserParam 0x%x",
            hDecoder, fnDecEventCB, pUserParam);

    if(NULL == hDecoder)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetEventCallback",
            "Decoder SetEventCallback hDecoder error!");
        return IVS_PARA_INVALID;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);

    int iRet = pDecodeHandler->SetEventCallback(fnDecEventCB, pUserParam);
    if(IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Decoder SetEventCallback",
            "Decoder SetEventCallback SetEventCallback error!");
        return iRet;
    }

    IVS_LOG(IVS_LOG_DEBUG, "Decoder_SetEventCallback", "Leave");
    return IVS_SUCCEED;
}

IVS_INT32 Decoder_MarkStreamEnd(IVS_HANDLE hDecoder, IVS_BOOL bIsEnd)
{
	if(NULL == hDecoder)
	{
		IVS_LOG(IVS_LOG_ERR, "Decoder MarkStreamEnd", "hDecoder = NULL");
		return IVS_PARA_INVALID;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	return pDecodeHandler->MarkEoS(!!bIsEnd);
}

IVS_BOOL Decoder_IsCancel(IVS_HANDLE hDecoder)
{
    if (NULL == hDecoder)
	{
        return IVS_TRUE;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	return  (int)pDecodeHandler->IsCancel();
}

IVS_VOID Decoder_StopNotifyBuffEvent(IVS_HANDLE hDecoder)
{
	if (NULL == hDecoder)
	{
		return;  // IVS_TRUE;
	}

	DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
	pDecodeHandler->StopNotifyBuffEvent();
}

IVS_VOID Decoder_ReSetFlowContrlFlag(IVS_HANDLE hDecoder)
{
    if (NULL == hDecoder)
    {
        return;
    }

    DecodeHandler* pDecodeHandler = reinterpret_cast<DecodeHandler *>(hDecoder);
    pDecodeHandler->ReSetFlowContrlFlag();
}

#ifdef __cplusplus
}
#endif // __cplusplus

