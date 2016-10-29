/********************************************************************
filename    : ReassembleAPI.h
author      : c00206592
created     : 2012/11/17
description : ��֡����APIͷ�ļ�
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 ��ʼ�汾
 *********************************************************************/

#ifndef __IVS_PLATER_REASSEMBLE_API_H__
#define __IVS_PLATER_REASSEMBLE_API_H__

#include "IVSPlayerDataType.h"

/*****************************************************************************
 �������ƣ�Reass_GetHandle
 ������������ȡ��Ⱦ���
 ���������nDecoderType ��������(VIDEO_DEC_TYPE)
 ���������NA
 �� �� ֵ��RENDER_HANDLE
*****************************************************************************/
IVS_HANDLE Reass_GetHandle(IVS_INT32 iDecoderType);

/*****************************************************************************
 �������ƣ�Render_FreeHandle
 �����������ͷ���Ⱦ���
 ���������IVS_HANDLE ���
 ���������NA
 �� �� ֵ��void
*****************************************************************************/
void       Reass_FreeHandle(IVS_HANDLE h);

/*****************************************************************************
 �������ƣ�Reass_InsertPacket
 ��������������RTP��
 ���������
 IVS_HANDLE��     ��Ⱦͨ�����
 pBuf��           ������
 ulBufLen��       ���ݳ���
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED   ��  �ɹ�
 IVS_FAIL      ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_InsertPacket(IVS_HANDLE h, IVS_CHAR* pBuf, IVS_UINT32 ulBufLen);

/*****************************************************************************
 �������ƣ�Reass_GetVideoCodeRate
 ������������ȡ��Ƶ��������
 ���������
 IVS_HANDLE     �� ��Ⱦͨ�����
 uVideoCodeRate :  ʵʱ����
 uAvgVideoCodeRate��ƽ������
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED   ��  �ɹ�
 IVS_FAIL      ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_GetVideoCodeRate(IVS_HANDLE h, IVS_UINT32* uVideoCodeRate, IVS_UINT32* uAvgVideoCodeRate);

/*****************************************************************************
 �������ƣ�Reass_GetAudioCodeRate
 ������������ȡ��Ƶ��������
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 uAudioCodeRate:  ʵʱ����
 uAvgAudioCodeRate :ƽ������
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_GetAudioCodeRate(IVS_HANDLE h, IVS_UINT32* uAudioCodeRate, IVS_UINT32* uAvgAudioCodeRate);

/*****************************************************************************
 �������ƣ�Reass_GetFPS
 ������������ȡ��Ƶ֡��
 ���������
 IVS_HANDLE �� ��Ⱦͨ�����
 uFps       :  ����
 �������   ��NA
 �� �� ֵ   ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_GetFPS(IVS_HANDLE h, IVS_UINT32* uFps);

/*****************************************************************************
 �������ƣ�Reass_GetLostPacketRate
 ������������ȡƽ��������
 ���������
 IVS_HANDLE     �� ��Ⱦͨ�����
 pfLostPacketRate:  ������
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_GetLostPacketRate(IVS_HANDLE h, IVS_FLOAT* pfLostPacketRate);

/*****************************************************************************
 �������ƣ�Reass_ReSetReassemble
 ���������������϶��������ȶ�����ʱ�򣬵������跽���������������
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_ReSetReassemble(IVS_HANDLE h);

void       Reass_SetResetFlag(IVS_HANDLE h, bool bIsReset);
/*****************************************************************************
 �������ƣ�Reass_SetRawFrameLimit
 ����������������������ʱ�򣬿��Ե��ø÷���������֡��������С
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 uSize     �� �����С������ֵ��4~7��
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetRawFrameLimit(IVS_HANDLE h, IVS_UINT32 uSize);

/*****************************************************************************
 �������ƣ�Reass_SetWaterMarkState
 ���������������Ƿ�����ˮӡУ��
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 IVS_BOOL  �� �Ƿ�����ˮӡУ��
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetWaterMarkState(IVS_HANDLE h, IVS_BOOL bIsStart);


/*****************************************************************************
 �������ƣ�Reass_SetPlayBackMode
 ����������������Ƶ�ط�ģʽ
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 IVS_BOOL  �� �Ƿ������ط���֡ģʽ ture:������false:������
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetPlayBackMode(IVS_HANDLE h, IVS_BOOL bIsPlayBackMode);


/*****************************************************************************
 �������ƣ�Reass_SetDecryptDataKeyAndMode
 ����������������Ƶ������Կ
 ���������
 IVS_HANDLE  �� ��Ⱦͨ�����
 pSecretKey  �� ������Կ
 ulKeyLen    :  ��Կ����
 iSecretKeyType ����Կ���� (�ο� DECRYP_TYPE )
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetDecryptDataKeyAndMode(IVS_HANDLE h,const IVS_CHAR* pSecretKey,IVS_ULONG ulKeyLen, IVS_INT32 iSecretKeyType);

///*****************************************************************************
// �������ƣ�Reass_SetAudioDecryptDataKey
// ����������������ƵƵ������Կ
// ���������
// IVS_HANDLE  �� ��Ⱦͨ�����
// pSecretKey  �� ������Կ
// ulKeyLen    :  ��Կ����
// iSecretKeyType ����Կ���� (�ο� DECRYP_TYPE )
// ���������NA
// �� �� ֵ��
// IVS_SUCCEED     ��  �ɹ�
// IVS_FAIL        ��  ʧ��
// IVS_PARA_INVALID�� ��������Ƿ�
//*****************************************************************************/
//int        Reass_SetAudioDecryptDataKey(IVS_HANDLE h,const IVS_CHAR* pSecretKey,IVS_ULONG ulKeyLen, IVS_INT32 iSecretKeyType);

/*****************************************************************************
 �������ƣ�Reass_StopAudioDecryptData
 ����������ֹͣ��Ƶ����
 ���������
 IVS_HANDLE  �� ��Ⱦͨ�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
//int        Reass_StopAudioDecryptData(IVS_HANDLE h);

/*****************************************************************************
 �������ƣ�Reass_StopVideoDecryptData
 ����������ֹͣ��Ƶ����
 ���������
 IVS_HANDLE  �� ��Ⱦͨ�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_StopVideoDecryptData(IVS_HANDLE h);


/*****************************************************************************
 �������ƣ�Reass_SetCompleteCallback
 �����������ص�����
 ���������
 IVS_HANDLE��     ��Ⱦͨ�����
 cbFunc��         �ص�����ָ��
 pUser��          �û�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetCompleteCallback(IVS_HANDLE h, REASSM_CALLBACK cbFunc, void *pUser);

/*****************************************************************************
 �������ƣ�Reass_SetExceptionCallBack
 ���������������쳣�ص�����
 ���������
 IVS_HANDLE��     ��Ⱦͨ�����
 pExceptionCallBack�� �ص�����ָ��
 pUser��          �û�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetExceptionCallBack(IVS_HANDLE h, PLAYER_EXCEPTION_CALLBACK pExceptionCallBack, void *pUser);


/*****************************************************************************
 �������ƣ�Reass_GetStatisInfo
 ������������ȡ������Ϣ
 ���������
 IVS_HANDLE��     ��Ⱦͨ�����
 CODE_RATE_INFO�� ������Ϣ
 pUser��          �û�����
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_GetStatisInfo(IVS_HANDLE h, CODE_RATE_INFO* pCodeRateInfo);


/*****************************************************************************
 �������ƣ�Reass_SetMediaAttr
 ��������������rtpý������
 ���������
 IVS_HANDLE��     ��֡ͨ�����

 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int        Reass_SetMediaAttr(IVS_HANDLE h, const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr);



#endif //__IVS_PLATER_REASSEMBLE_API_H__
