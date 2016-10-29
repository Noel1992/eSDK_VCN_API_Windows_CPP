/********************************************************************
filename    : ReassembleAPI.h
author      : c00206592
created     : 2012/11/17
description : ��֡����APIͷ�ļ�
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 ��ʼ�汾
 *********************************************************************/

#include "PacketReassemble.h"
#include "H264Reassemble.h"
#include "MjpegReassemble.h"
#include "HevcReassemble.h"
#include "ReassembleAPI.h"
#include "SVACReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define CHECK_HANDLE_NULL(p, f)  do{ if(!(p)) { return IVS_FAIL; }}while (0)  //lint !e1960                                

/*****************************************************************************
 �������ƣ�Reass_GetHandle
 ������������ȡ��Ⱦ���
 ���������nDecoderType ��������(VIDEO_DEC_TYPE)
 ���������NA
 �� �� ֵ��RENDER_HANDLE
*****************************************************************************/
IVS_HANDLE Reass_GetHandle(IVS_INT32 iDecoderType)
{
    IVS_TRACE();

    CPacketReassemble* reassHandle = NULL;
    switch (iDecoderType)
    {
    case VIDEO_DEC_H264:
        (void)IVS_NEW((CH264Reassemble * &)reassHandle);
        IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init H264Reassemble");
        break;

    case VIDEO_DEC_MJPEG:
        (void)IVS_NEW((CMjpegReassemble * &)reassHandle);
        IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init MjpegReassemble");
        break;

	case VIDEO_DEC_H265:
		(void)IVS_NEW((CHevcReassemble * &)reassHandle);
		IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init HevcReassemble");
		break;

	case VIDEO_DEC_SVAC:
		(void)IVS_NEW((CSvacReassemble * &)reassHandle);
		IVS_LOG(IVS_LOG_DEBUG, "Reass_GetHandle", "Init SVACReassemble");
		break;

    default:
        break;
    }

    if (NULL == reassHandle)
    {
        IVS_LOG(IVS_LOG_ERR, "Reass_GetHandle",
                "PacketReassemble initial assemble error, Create assemble failed.");
        return NULL;
    }

    if (IVS_FAIL == reassHandle->Init())
    {
        IVS_DELETE(reassHandle);
        reassHandle = NULL;
    }

    return reassHandle;
}//lint !e1788

/*****************************************************************************
 �������ƣ�Render_FreeHandle
 �����������ͷ���Ⱦ���
 ���������IVS_HANDLE ���
 ���������NA
 �� �� ֵ��void
*****************************************************************************/
void Reass_FreeHandle(IVS_HANDLE h)
{
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    IVS_DELETE(packeReassemble);
}

/*****************************************************************************
 �������ƣ�Reass_InsertPacket
 ��������������RTP��
 ���������
 IVS_HANDLE��     ��Ⱦͨ�����
 pBuf��           ������
 ulBufLen��       ���ݳ���
 ���������NA
 �� �� ֵ��void
*****************************************************************************/
int Reass_InsertPacket(IVS_HANDLE h, IVS_CHAR* pBuf, IVS_UINT32 ulBufLen)
{
    CHECK_HANDLE_NULL(h, "Reass_InsertPacket");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->InsertPacket(pBuf, ulBufLen);
}

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
int Reass_GetVideoCodeRate(IVS_HANDLE h, IVS_UINT32* uVideoCodeRate, IVS_UINT32* uAvgVideoCodeRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetVideoCodeRate");

    CHECK_HANDLE_NULL(uVideoCodeRate, "Reass_GetVideoCodeRate");

    CHECK_HANDLE_NULL(uAvgVideoCodeRate, "Reass_GetVideoCodeRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uVideoCodeRate = packeReassemble->GetVideoCodeRate();

    *uAvgVideoCodeRate = packeReassemble->GetAvgVideoCodeRate();

    return IVS_SUCCEED;
}

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
int Reass_GetAudioCodeRate(IVS_HANDLE h, IVS_UINT32* uAudioCodeRate, IVS_UINT32* uAvgAudioCodeRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetAudioCodeRate");

    CHECK_HANDLE_NULL(uAudioCodeRate, "Reass_GetAudioCodeRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uAudioCodeRate = packeReassemble->GetAudioCodeRate();

    *uAvgAudioCodeRate = packeReassemble->GetAvgAudioCodeRate();

    return IVS_SUCCEED;
}

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
int  Reass_GetFPS(IVS_HANDLE h, IVS_UINT32* uFps)
{
    CHECK_HANDLE_NULL(h, "Reass_GetFPS");

    CHECK_HANDLE_NULL(uFps, "Reass_GetFPS");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *uFps = packeReassemble->GetFPS();

    return IVS_SUCCEED;
}//lint !e954

/*****************************************************************************
 �������ƣ�Reass_GetLostPacketRate
 ������������ȡƽ��������
 ���������
 @IVS_HANDLE     �� ��Ⱦͨ�����
 @fLostPacketRate:  ������
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int  Reass_GetLostPacketRate(IVS_HANDLE h, IVS_FLOAT* pfLostPacketRate)
{
    CHECK_HANDLE_NULL(h, "Reass_GetLostPacketRate");

    CHECK_HANDLE_NULL(pfLostPacketRate, "Reass_GetLostPacketRate");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    *pfLostPacketRate = packeReassemble->GetLostPacketRate();
    return IVS_SUCCEED;
}

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
int Reass_ReSetReassemble(IVS_HANDLE h)
{
    CHECK_HANDLE_NULL(h, "Reass_ReSetReassemble");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->ReSetReassemble();
}

void Reass_SetResetFlag(IVS_HANDLE h, bool bIsReset)
{
	if (NULL == h)
	{
		IVS_LOG(IVS_LOG_ERR, "Reass_SetResetFlag h is null.", "");
		return;
	}

	CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

	packeReassemble->SetResetFlag(bIsReset);
}
/*****************************************************************************
 �������ƣ�Reass_SetRawFrameLimit
 ����������������������ʱ�򣬿��Ե��ø÷���������֡��������С
 ���������
 IVS_HANDLE�� ��Ⱦͨ�����
 uSize     �� �����С������ֵ��3~8��
 ���������NA
 �� �� ֵ��
 IVS_SUCCEED     ��  �ɹ�
 IVS_FAIL        ��  ʧ��
 IVS_PARA_INVALID�� ��������Ƿ�
*****************************************************************************/
int  Reass_SetRawFrameLimit(IVS_HANDLE h, IVS_UINT32 uSize)
{
    CHECK_HANDLE_NULL(h, "Reass_SetRawFrameLimit");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetFrameCacheLimit(uSize);

    return IVS_SUCCEED;
}

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
int   Reass_SetWaterMarkState(IVS_HANDLE h, IVS_BOOL bIsStart)
{
    CHECK_HANDLE_NULL(h, "Reass_SetWaterMarkState");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->SetWaterMarkState(bIsStart);
}

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
int   Reass_SetPlayBackMode(IVS_HANDLE h, IVS_BOOL bIsPlayBackMode)
{
    CHECK_HANDLE_NULL(h, "Reass_SetPlayBackMode");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetPlayBackMode(bIsPlayBackMode);

	return IVS_SUCCEED;
}

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
int  Reass_SetDecryptDataKeyAndMode(IVS_HANDLE h, const IVS_CHAR* pSecretKey, IVS_ULONG ulKeyLen,
                                         IVS_INT32 iSecretKeyType)
{
    CHECK_HANDLE_NULL(h, "Reass_SetVideoDecryptDataKeyAndMode h");
    //CHECK_HANDLE_NULL(pSecretKey, "Reass_SetVideoDecryptDataKeyAndMode pSecretKey")
    CPacketReassemble * packeReassemble = (CPacketReassemble *)h;
    return packeReassemble->SetDecryptDataKeyAndMode(pSecretKey, ulKeyLen, iSecretKeyType);
}

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
int Reass_StopVideoDecryptData(IVS_HANDLE h)
{
    CHECK_HANDLE_NULL(h, "Reass_StopVideoDecryptData");
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    packeReassemble->StopDecryptVideoData();
    return IVS_SUCCEED;
}

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
int  Reass_SetCompleteCallback(IVS_HANDLE h, REASSM_CALLBACK cbFunc, void *pUser)
{
    CHECK_HANDLE_NULL(h, "Reass_SetCompleteCallback");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    return packeReassemble->SetCallbackFun(cbFunc, pUser);
}

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
int  Reass_SetExceptionCallBack(IVS_HANDLE h, PLAYER_EXCEPTION_CALLBACK pExceptionCallBack, void *pUser)
{
    CHECK_HANDLE_NULL(h, "Reass_SetExceptionCallBack");

    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;

    packeReassemble->SetExceptionCallback(pExceptionCallBack, pUser);

    return IVS_SUCCEED;
}

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
int  Reass_GetStatisInfo(IVS_HANDLE h, CODE_RATE_INFO* pCodeRateInfo)
{
    CHECK_HANDLE_NULL(h, "Reass_ShowCodeRateInfo");
    CHECK_HANDLE_NULL(pCodeRateInfo, "Reass_ShowCodeRateInfo");
    CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    pCodeRateInfo->uAudioCodeRate = packeReassemble->GetAudioCodeRate();
    pCodeRateInfo->uAvgAudioCodeRate = packeReassemble->GetAvgAudioCodeRate();
    pCodeRateInfo->uAvgVideoCodeRate = packeReassemble->GetAvgVideoCodeRate();
    pCodeRateInfo->uVideoCodeRate   = packeReassemble->GetVideoCodeRate();
    pCodeRateInfo->uUsedMemBlockNum = packeReassemble->GetUsedMemBlockNum();
    pCodeRateInfo->uFrameListSize = packeReassemble->GetFrameListSize();
    return IVS_SUCCEED;
}

/*
 * Reass_SetMediaAttr
 * ����ý������
 */
int Reass_SetMediaAttr(IVS_HANDLE h, const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr)
{
    CHECK_HANDLE_NULL(h, "Reass_ShowCodeRateInfo");
	CPacketReassemble *packeReassemble = (CPacketReassemble *)h;
    packeReassemble->SetMediaAttr(pVideoAttr, pAudioAttr);
    return IVS_SUCCEED;
}

