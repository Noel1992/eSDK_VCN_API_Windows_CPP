/*******************************************************************************
//  ��Ȩ����    2012 ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  DecodeHandler.cpp
//  �ļ�˵��:   ���ݳ������ͣ�ͨ���ý����������ഴ������Ӧ��
                ������������������ϲ����
//  ��    ��:   IVS V100R001C02
//  ��    ��:   z90003203
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵���� 
*******************************************************************************/

// DecodeHandler.cpp: implementation of the DecodeHandler class.
//
//////////////////////////////////////////////////////////////////////

//#include "OmniDecoder.h"
#include "DecodeHandler.h"
#include "DecoderFactory.h"


// ��¼����Ƶ�Ľ��������
//CBaseDecoder* DecodeHandler::m_pAudioDecoder = NULL;

// ����Ƶ�Ľ�������ȫ��������֤SDKͬʱֻ���һ·��Ƶ
//CRITICAL_SECTION  g_csAudioDecoder;

// ��Ƶ��������ȫ�����ı�־λ
//bool g_AudioDecodeInitialized = false;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
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
DecodeHandler::DecodeHandler()
: m_pDecoder(NULL)
, m_nWidth(352)//Ĭ����Ƶ��Ϊ352
, m_nHeight(288)//Ĭ����Ƶ��Ϊ288
{
//     if (!g_AudioDecodeInitialized)
//     {
//         InitializeCriticalSection(&g_csAudioDecoder);
//         g_AudioDecodeInitialized = true;
//     }
}

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
Description:     �ص�������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
Description:     ����
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
Description:     ����
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
Description:     ����������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
Description:     ����������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
Description:     ֹͣ������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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



//��SendVideoFrame������Ƶ������
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

//��SendAudioFrame���������
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
Description:     ������Ƶ
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
*************************************************/
int DecodeHandler::StartAudio() const
{
	if(NULL == m_pDecoder)
	{
        IVS_LOG(IVS_LOG_ERR, "Start Audio", "Decoder must be init first!");
		return IVS_PLAYER_RET_ORDER_ERROR;
	}

// 	m_pDecoder->StartAudio();

    // add for���ⵥ A14D04698 �����·��Ƶʱ��DEBUG�»��ж�
    // �����û�ֻ�ܴ�һ·������֤���߳������� [11/17/2010 z90003203]
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
Description:     �ص���Ƶ
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    int
Others:          // ����˵��
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
//     // add for���ⵥ A14D04698 �����·��Ƶʱ��DEBUG�»��ж�
//     // �����û�ֻ�ܴ�һ·������֤���߳������� [11/17/2010 z90003203]
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
�������ƣ�DecodeHandler::SetExceptionCallback
�������������ý�����¼��ص�����
���������hDecoder     ���������
fnDecEventCB �����¼��ص�����
pUserParam   �û�����
���������NA
�� �� ֵ��int
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
�������ƣ�DecodeHandler::GetSourceBufferRemain
������������ȡδ����֡������ʣ���֡��
���������hDecoder     ���������
���������uBufferCount ֡�������
�� �� ֵ��int
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
�������ƣ�DecodeHandler::SetWaterMarkState
��������������ˮӡУ�鿪ʼ/ֹͣ
���������bIsCheckWaterMark ��ʼΪtrue,ֹͣΪfalse
�����������
�� �� ֵ����
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
�������ƣ�Decoder_ResetBuffer
������������ջ��������;
���������hDecoder     ���������
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
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
�������ƣ�SetDelayFrameNum
����������������ʱ�����֡��;
���������uDelayFrameNum ��ʱ�����֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
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
�������ƣ�StepFrames
�������������õ��������֡��;
���������uStepFrameNum ���������֡��
�����������
�� �� ֵ���ɹ�:IVS_SUCCEED ʧ��:������
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
�������ƣ�DecodeHandler::SetEventCallback
�������������ý�����¼��ص�����
���������fnDecEventCB �����¼��ص�����
          pUserParam   �û�����
���������NA
�� �� ֵ��int
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

