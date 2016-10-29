/*******************************************************************************
//  ��Ȩ����    2012 ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  DecodeHandler.h
//  �ļ�˵��:   ���ݳ������ͣ�ͨ���ý����������ഴ������Ӧ��
	           ������������������ϲ����
//  ��    ��:   IVS V100R001C02
//  ��    ��:   z90003203
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵���� 
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
    Description:     ���캯��
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    Others:          // ����˵��
    *************************************************/
    DecodeHandler();
    /*************************************************
    Function:        DecodeHandler::~DecodeHandler
    Description:     ��������
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    Others:          // ����˵��
    *************************************************/
    virtual ~DecodeHandler();

public:
    /*************************************************
    Function:        DecodeHandler::CreateDecoder
    Description:     ����һ���������
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    int nEncoderType �� ��������
    int nVideoFormat �� ��Ƶ��ʽ
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int CreateDecoder();

    /*************************************************
    Function:        DecodeHandler::Release
    Description:     �ͷ�һ���������
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int ReleaseDecoder();

    /*****************************************************************************
    �������ƣ�Decoder_SetDecoderCB
    �������������ý�������ݻص�����
    ���������hDecoder     ���������
    fnDecoderCB  YUV�ص�����
    pUserParam   �û�����
    ���������NA
    �� �� ֵ��int
    *****************************************************************************/
    int SetDecoderCB(DECODER_CALLBACK fnDecoderCB, void *pUserParam);


    /*****************************************************************************
    �������ƣ�Decoder_OpenStream
    ��������������
    ���������hDecoder     ���������
              pVideoParam  ��Ƶ����
              pAudioParam  ��Ƶ����
              uBufferSize  һ֡�Ļ����С
              uBufferCount ֡�������
    ���������NA
    �� �� ֵ��int
    *****************************************************************************/
    int OpenStream(VIDEO_PARAM *pVideoParam,
                   AUDIO_PARAM *pAudioParam,
                   unsigned int uLowerLimit,
                   unsigned int uUpperLimit,
				   IVS_UINT32 uBufferCount,
				   IVS_UINT32 uExBufCount);


    /*****************************************************************************
    �������ƣ�Decoder_CloseStream
    �����������ر���
    ���������hDecoder     ���������
    ���������NA
    �� �� ֵ��int
    *****************************************************************************/
    int CloseStream();


    /*************************************************
    Function:        DecodeHandler::Start
    Description:     ����������
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int Start(HWND hWnd);

    /*************************************************
    Function:        DecodeHandler::Stop
    Description:     ֹͣ������
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int Stop();

    /*************************************************
    Function:        DecodeHandler::SendVideoFrame
    Description:     ����һ��Ƶ֡
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    pRawFrameInfo  δ�����֡��Ϣ
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int SendVideoFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                       char *pFrame,
                       unsigned int uFrameSize);

    /*************************************************
    Function:        DecodeHandler::SendAudioFrame
    Description:     ����һ��Ƶ֡
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    pRawFrameInfo  δ�����֡��Ϣ
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int SendAudioFrame(IVS_RAW_FRAME_INFO *pRawFrameInfo,
                       char *pFrame,
                       unsigned int uFrameSize);


    /*****************************************************************************
    �������ƣ�SetExceptionCallback
    �������������ý�����¼��ص�����
    ���������hDecoder     ���������
    fnDecExceptionCB �����¼��ص�����
    pUserParam   �û�����
    ���������NA
    �� �� ֵ��int
    *****************************************************************************/
    int SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK fnDecExceptionCB, void *pUserParam);


    /*************************************************
    Function:        DecodeHandler::StartAudio
    Description:     ������Ƶ
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int StartAudio() const;

    /*************************************************
    Function:        DecodeHandler::ShutDownAudio
    Description:     �ص���Ƶ
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int ShutDownAudio() const;

    /*************************************************
    Function:        DecodeHandler::SetAudioVolume
    Description:     ����������С
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    long volume ������ֵ
    Output:          // �����������˵��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int SetAudioVolume(long volume);

    /*************************************************
    Function:        DecodeHandler::GetAudioVolume
    Description:     �õ�������С
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    long &lVolume ���õ�����ֵ
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int GetAudioVolume(long &lVolume);

    // 	int InsertBuf(unsigned char *buf,
    // 						int buflen);
    /*************************************************
    Function:        DecodeHandler::GetVideoResolution
    Description:     �õ���Ƶ֡��С
    Calls:           // �����������õĺ����嵥������������
    Called By:       // ���ñ������ĺ����嵥������������
    Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
    Output:          // �����������˵��
    int &nWidth   ����Ƶ֡��
    int &nHeight  ����Ƶ֡��
    Return:          // ��������ֵ��˵��
    int
    Others:          // ����˵��
    *************************************************/
    int GetVideoResolution(int &iWidth, int &iHeight);

    /*****************************************************************************
    �������ƣ�Decoder_GetSourceBufferRemain
    ������������ȡδ����֡������ʣ���֡��
    ���������hDecoder     ���������
    uBufferCount ֡�������
    ���������NA
    �� �� ֵ��IVS_INT32
    *****************************************************************************/
    int GetSourceBufferRemain(unsigned int &uBufferCount) const;

    /*****************************************************************************
    �������ƣ�DecodeHandler::SetWaterMarkState
    ��������������ˮӡУ�鿪ʼ/ֹͣ
    ���������bIsCheckWaterMark ��ʼΪtrue,ֹͣΪfalse
    �����������
    �� �� ֵ����
    *****************************************************************************/
    int SetWaterMarkState(BOOL bIsCheckWaterMark);

    /*****************************************************************************
    �������ƣ�Decoder_ResetBuffer
    ������������ջ��������;
    �����������
    �����������
    �� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
    *****************************************************************************/
    int ResetBuffer();

    /*****************************************************************************
    �������ƣ�SetDelayFrameNum
    ����������������ʱ�����֡��;
    ���������uDelayFrameNum ��ʱ�����֡��
    �����������
    �� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
    *****************************************************************************/
    int SetDelayFrameNum(unsigned int uDelayFrameNum);

    /*****************************************************************************
    �������ƣ�StepFrames
    �������������õ��������֡��;
    ���������uStepFrameNum ���������֡��
    �����������
    �� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
    *****************************************************************************/
    int StepFrames(unsigned int uStepFrameNum);

    /*****************************************************************************
    �������ƣ�SetEventCallback
    �������������ý�����¼��ص�����
    ���������fnDecEventCB �����¼��ص�����
              pUserParam   �û�����
    ���������NA
    �� �� ֵ��int
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
