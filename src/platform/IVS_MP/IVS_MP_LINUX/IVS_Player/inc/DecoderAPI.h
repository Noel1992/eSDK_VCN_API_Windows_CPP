/********************************************************************
filename    : DecoderAPI.h
author      : z90003203
created     : 2012/11/23
description : ����Ƶ����ģ��ӿڷ�װ
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/10/31 ��ʼ�汾
*********************************************************************/

#ifndef __DECODER_API__
#define __DECODER_API__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "IVSPlayerDataType.h"


/*****************************************************************************
�������ƣ�Decoder_GetHandle
�����������������������ɹ����ؽ��������
���������NA  
���������NA
�� �� ֵ���ɹ������������ ʧ�ܣ�NULL
*****************************************************************************/
IVS_HANDLE Decoder_GetHandle();


/*****************************************************************************
�������ƣ�Decoder_FreeHandle
�����������ͷ�ָ����������Դ
���������hDecoder Decoder_GetHandle�д����Ľ��������
���������NA
�� �� ֵ��IVS_VOID
*****************************************************************************/
IVS_VOID Decoder_FreeHandle(IVS_HANDLE hDecoder);


/*****************************************************************************
�������ƣ�Decoder_OpenStream
��������������
���������hDecoder     ���������
          pVideoParam  ��Ƶ����
          pAudioParam  ��Ƶ����
          uBufferSize  һ֡�Ļ����С
          uBufferCount ֡�������
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_OpenStream(IVS_HANDLE hDecoder,
                             VIDEO_PARAM *pVideoParam,
                             AUDIO_PARAM *pAudioParam,
                             IVS_UINT32 uLowerLimit,
                             IVS_UINT32 uUpperLimit,
                             IVS_UINT32 uBufferCount = DECODER_FRAME_COUNT,
							 IVS_UINT32 uExBufCount  = VIDEO_BUF_EX_COUNT);


/*****************************************************************************
�������ƣ�Decoder_CloseStream
�����������ر���
���������hDecoder     ���������
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_CloseStream(IVS_HANDLE hDecoder);


/*****************************************************************************
�������ƣ�Decoder_SetDecoderCB
�������������ý�������ݻص�����
���������hDecoder     ���������
          fnDecoderCB  YUV�ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetDecoderCB(IVS_HANDLE hDecoder, DECODER_CALLBACK fnDecoderCB, void *pUserParam);


/*****************************************************************************
�������ƣ�Decoder_Start
������������ʼ����
���������hDecoder     ���������
          hWnd         ���ھ�����˰汾���ݲ�֧����ʾ�����ڲ���Ⱦ
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_Start(IVS_HANDLE hDecoder, HWND hWnd = NULL);


/*****************************************************************************
�������ƣ�Decoder_Stop
����������ֹͣ����
���������hDecoder     ���������  
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_Stop(IVS_HANDLE hDecoder);


/*****************************************************************************
�������ƣ�Decoder_SendVideoFrame
������������һ֡��Ƶ֡�������
���������hDecoder     ���������
          pRawFrameInfo  δ�������Ƶ֡��Ϣ
          pFrame        ֡��ַ
          uFrameSize    ֡����
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SendVideoFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize);


/*****************************************************************************
�������ƣ�Decoder_SendAudioFrame
������������һ֡��Ƶ֡�������
���������hDecoder     ���������
          pRawFrameInfo  δ�������Ƶ֡��Ϣ
          pFrame        ֡��ַ
          uFrameSize    ֡����
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SendAudioFrame(IVS_HANDLE hDecoder,
                                 IVS_RAW_FRAME_INFO *pRawFrameInfo,
                                 IVS_CHAR* pFrame,
                                 IVS_UINT32 uFrameSize);



/*****************************************************************************
�������ƣ�Decoder_SetExceptionCallback
�������������ý�����¼��ص�����
���������hDecoder     ���������
          fnDecExceptionCB �����¼��ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetExceptionCallback(IVS_HANDLE hDecoder, PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void *pUserParam);


/*****************************************************************************
�������ƣ�Decoder_GetSourceBufferRemain
������������ȡδ����֡������ʣ���֡��
���������hDecoder     ���������
���������iBufferCount ֡�������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_GetSourceBufferRemain(IVS_HANDLE hDecoder, IVS_UINT32 &uBufferCount);


/*****************************************************************************
�������ƣ�Decoder_SetWaterMarkState
��������������ˮӡУ�鿪ʼ/ֹͣ
���������bIsCheckWaterMark ��ʼΪtrue,ֹͣΪfalse
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetWaterMarkState(IVS_HANDLE hDecoder, IVS_BOOL bIsCheckWaterMark);

/*****************************************************************************
�������ƣ�Decoder_ResetBuffer
������������ջ��������;
���������hDecoder     ���������
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_ResetBuffer(IVS_HANDLE hDecoder);

/*****************************************************************************
�������ƣ�Decoder_SetDelayFrameNum
����������������ʱ�����֡��;
���������hDecoder       ���������
          uDelayFrameNum ��ʱ�����֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetDelayFrameNum(IVS_HANDLE hDecoder, IVS_UINT32 uDelayFrameNum);

/*****************************************************************************
�������ƣ�Decoder_StepFrames
�������������õ��������֡��;
���������hDecoder       ���������
          uStepFrameNum ���������֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_StepFrames(IVS_HANDLE hDecoder, IVS_UINT32 uStepFrameNum);

/*****************************************************************************
�������ƣ�Decoder_SetEventCallback
�������������ý�����¼��ص�����
���������hDecoder     ���������
          fnDecEventCB �����¼��ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
*****************************************************************************/
IVS_INT32 Decoder_SetEventCallback(IVS_HANDLE hDecoder, PLAYER_EVENT_CALLBACK fnDecEventCB, void *pUserParam);


/*
 * Decoder_MarkStreamEnd
 * ��������������
 */
IVS_INT32 Decoder_MarkStreamEnd(IVS_HANDLE hDecoder, IVS_BOOL bIsEnd);



/*
 * add by w00210470; test
 */
IVS_BOOL Decoder_IsCancel(IVS_HANDLE hDecoder);

IVS_VOID Decoder_StopNotifyBuffEvent(IVS_HANDLE hDecoder);

IVS_VOID Decoder_ReSetFlowContrlFlag(IVS_HANDLE hDecoder);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __DECODER_API__
