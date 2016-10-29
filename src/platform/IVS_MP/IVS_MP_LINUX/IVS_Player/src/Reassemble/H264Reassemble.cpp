/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  H264Reassemble.cpp
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#include "H264Reassemble.h"
#include "IVSCommon.h"
#include "PreDecoder.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif

//lint -e954  ���ļ��е�954�澯������

CH264Reassemble::~CH264Reassemble()
{
	m_bStopInserFlag = true;
    m_bIsStepReset = false;
}

int CH264Reassemble::Init()
{
    //ͬ��һ���Է����㹻��Ľ��뻺��ռ�
    m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
    if (NULL == m_pFrameBuf)
    {
		IVS_LOG(IVS_LOG_ERR,"H264 Init","Create frame buf error");
        return IVS_FAIL;
    }
    memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //����0

	m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
	if (NULL == m_pDecryptDataBuf)
	{
		IVS_LOG(IVS_LOG_ERR,"H264 Init","Create Dec frame buf error");
		IVS_DELETE(m_pFrameBuf, MUILI);
		return IVS_FAIL;
	}
	memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //����0

    //��ʼ���ڴ��
    if (IVS_FAIL == m_packetPool.Init())
    {
		IVS_LOG(IVS_LOG_ERR,"H264 Init","Create packetPool error");
        IVS_DELETE(m_pFrameBuf, MUILI);
        IVS_DELETE(m_pDecryptDataBuf, MUILI);
        m_packetPool.FreeMemPool();

        return IVS_FAIL;
    }
    m_pFrameBuf[3] = 1;     //startcode ���ֽ�
    m_bStopInserFlag = false;
    m_bIsStepReset = false;
    return IVS_SUCCEED;
}

void CH264Reassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}


//��ձ��ػ����������ݣ�����ȫ���ڴ���Լ�������
int CH264Reassemble::ReSetReassemble()
{
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear H264 Frame begin.");

    m_bStopInserFlag = true;

    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear H264 Frame end");

    return IVS_SUCCEED;
}

//����RTP�����������֡����
int CH264Reassemble::InsertPacket(char* pBuf, unsigned int uBufLen)
{
    int iRet = IVS_FAIL;

    if (m_bDestroyAll)
    {
        IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "Destroy All, Not Need Free.");
        return iRet;
    }

    if (NULL == pBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", "Buffer is NULL");
        return iRet;
    }

	/*
	 * add by w00210470, filter payload type
	 */
	if (IVS_SUCCEED != CPacketReassemble::InsertPacket(pBuf, uBufLen))
	{
		return IVS_FAIL;
	}

    //edit by w00172146 ����϶���������(�϶�����յ�I֡�Ų����б���н���)
    if (m_bStopInserFlag)
    {
        FIXED_HEADER *pRtpHdr = (FIXED_HEADER*)pBuf; //lint !e826
        if (RTP_VERSION != pRtpHdr->version)
        {
            IVS_LOG(IVS_LOG_ERR, "InsertPacket", "version=[%d]", pRtpHdr->version);
            return IVS_FAIL;
        }

        if ((PAY_LOAD_TYPE_PCMA != pRtpHdr->payload) && (PAY_LOAD_TYPE_PCMU != pRtpHdr->payload))
        {
            //��ȡRTPͷ����
            unsigned long ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * pRtpHdr->csrc_len; //ͷ���ȼ���CC����

            unsigned short usExtDateLen = 0;    //��չ���ݳ��ȣ�������2�ֽ�profile���Ⱥ�2�ֽڳ���
            unsigned short usProfileLen = 0;    //�Զ����profile����2�ֽ�
            unsigned short usExtLen = 0;        //��չ����2�ֽ�
            //�������չ�ֶ�
            if (1 == pRtpHdr->extension)
            {
                RTP_EXTEND_HEADER *pExtHdr = (RTP_EXTEND_HEADER *)(pBuf + ulRtpHdrLen);//lint !e826
                usExtDateLen = ntohs(pExtHdr->usLength) * 4;//lint !e734
                usProfileLen = 2;
                usExtLen = 2;
            }

            FU_INDICATOR* pFuIndicator = (FU_INDICATOR*)(pBuf + ulRtpHdrLen + usProfileLen + usExtLen + usExtDateLen);
            FU_HEADER* pFuHeader = (FU_HEADER*)(pBuf + ulRtpHdrLen + usProfileLen + usExtLen + usExtDateLen + 1);
            IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "pFuIndicator->TYPE:%u. pFuHeader->TYPE:%u.", pFuIndicator->TYPE, pFuHeader->TYPE);

            if (!m_bIsStepReset)
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is false.");
                //�϶�¼��طţ������Pֱ֡���ӵ���ֱ���յ���һ��I֡������������
                if ((H264_SPS_NALU_TYPE != pFuIndicator->TYPE) && (H264_PPS_NALU_TYPE != pFuIndicator->TYPE) 
                    && (H264_SEI_NALU_TYPE != pFuIndicator->TYPE) && (H264_IDR_NALU_TYPE != pFuHeader->TYPE)
                    && (H264_IDR_NALU_TYPE != pFuIndicator->TYPE))
                {
                    IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "Not I Frame So Clear. pFuHeader->TYPE:%u.", pFuHeader->TYPE);
                    ClearFrameInfo();
                    return IVS_SUCCEED;
                }
                else if (H264_IDR_NALU_TYPE == pFuHeader->TYPE || H264_IDR_NALU_TYPE == pFuIndicator->TYPE)
                {
                    IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "This I Frame, Input It. pFuHeader->TYPE:%u, pFuIndicator->TYPE:%u.", 
                        pFuHeader->TYPE, pFuIndicator->TYPE);
                    m_bStopInserFlag = false;
                }
				else
				{

				}
            }
            else
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is true.");
                m_bIsStepReset = false;
                m_bStopInserFlag = false;
            }

        }//lint !e954
    }  

	FIXED_HEADER* pHdrTmp = (FIXED_HEADER*)pBuf;//lint !e826
    RTP_PACK_INFO* pstRtpData = NULL;
    iRet = GetRtpPacketInfo(pBuf, uBufLen, pstRtpData);

	// �ϲ���ӡ��־;
	if (NULL != pstRtpData && IVS_SUCCEED == iRet)
	{
		IVS_LOG(IVS_LOG_DEBUG, "Insert Packet", "TimeStamp:%u ,Seq:%u ,marker:%d BufLen:%u fuType:%u ",
				ntohl(pHdrTmp->timestamp), ntohs(pHdrTmp->seq_no), pHdrTmp->marker, uBufLen, pstRtpData->fuType);
	}
	else
	{
		IVS_LOG(IVS_LOG_DEBUG, "Insert Packet", "TimeStamp:%u ,Seq:%u ,marker:%d %u",
				ntohl(pHdrTmp->timestamp), ntohs(pHdrTmp->seq_no), pHdrTmp->marker, uBufLen);
	}

    if (IVS_SUCCEED != iRet) //���ͷ����
    {
        IVS_LOG(IVS_LOG_DEBUG, "Insert Packet",
                "H264 insert packet error, check RTP header failed, error code:%d", iRet);
		FreePacket(pstRtpData);
        return iRet;
    }

    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", "Get RTP Info error");
        return IVS_FAIL;
    }
    //��Ƶ����ֱ�ӽ���(rfc3551�غ����Ͷ���)
    if ((PAY_LOAD_TYPE_PCMU == pstRtpData->pt) || (PAY_LOAD_TYPE_PCMA == pstRtpData->pt))
    {
        m_bPCMUorPCMA = true;
        SendAudioFrame(pstRtpData);
        return IVS_SUCCEED;
    }

    //�Ƿ���NALU
    if ((0 == pstRtpData->fuType) || (H264_NALU_TYPE_END <= pstRtpData->fuType))
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", "Rtp FU Type error, FuType:%u Error",
                pstRtpData->fuType);
        FreePacket(pstRtpData);
        return IVS_PARA_INVALID;
    }

	// ����������һ�ֽ�
	pstRtpData->nalu = pstRtpData->fuType;

    
	//�����MU��CU�Ķ�����,ֻ������Ƶ�� 
    if (((0 < pstRtpData->fuType) && (H264_NALU_TYPE_STAP_A > pstRtpData->fuType)) || (H264_NALU_TYPE_FU_A == pstRtpData->fuType))
    {
        CaculateMU2CULostPacketNum(pstRtpData->usSeqNum);
        CaculateFPS(pstRtpData->ulTimeStamp);
        CaculateVideoCodeRate((unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
    }

    iRet = IVS_FAIL;
    if ((0 < pstRtpData->fuType) && (H264_NALU_TYPE_STAP_A > pstRtpData->fuType))         //����NALU��Ƭ
    {
        iRet = InsertSingleNalu(pstRtpData);
    }
    else if (H264_NALU_TYPE_FU_A == pstRtpData->fuType)         //��ʱֻ֧��FUA
    {
		//���ʱ����Ƿ���Ծ�䣬�����򽫶��е����ݾ�ǿ�з�����ջ������
		CheckTimeStampFail(pstRtpData->ulTimeStamp);
        iRet = InsertFUA(pstRtpData);
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", " unsupport SLICE type:%u", pstRtpData->fuType);
    }
    //�������ʧ��
    if (IVS_SUCCEED != iRet)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Packet", " Insert Frame Error,timeStamp: %u, Seq:%u", pstRtpData->ulTimeStamp,pstRtpData->usSeqNum);
        FreePacket(pstRtpData);
    }
    return iRet;
}//lint !e1788

//��Ⲣ���������¼����طš�ʱ�������
void CH264Reassemble::CheckSpecialEvent(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData || m_FrameList.empty())
    {
        return;
    }

    //���Ϊ�ط�ģʽ�����Խ�IDR֮֡ǰ������֡���ͳ�ȥ���ǻط�ģʽ�Ļ���ʱ����ǰ�˳���ͣ��������
    if (m_iPlayBackMode)
    {
        CheckIDRAndSend(pstRtpData);
    }

    //���ʱ����Ƿ���Ծ�䣬�����򽫶��е����ݾ�ǿ�з�����ջ������
    CheckTimeStampFail(pstRtpData->ulTimeStamp);
}

//�طŵ�ʱ�򣬲����յ��µ�GOPʱ��ǿ�Ʒ�������ʣ��Ļ���������
void CH264Reassemble::CheckIDRAndSend(RTP_PACK_INFO* pstRtpData)
{
    if (!m_iPlayBackMode)
    {
		IVS_LOG(IVS_LOG_DEBUG, "Check IDR Frame", "Is not playBack mode");
        return;
    }
	if (m_FrameList.empty())
	{
		IVS_LOG(IVS_LOG_DEBUG, "Check IDR Frame", "FrameSize is Empty");
		return;
	}
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Check IDR Frame", "RTP data is NULL");
        return;
    }
    if (3 > pstRtpData->lPayloadLen) //FUA ����Ҫ����Ϊ3
    {
        IVS_LOG(IVS_LOG_ERR, "Check IDR Frame", "H264 insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return;
    }
    //��ȡ��ǰ�ְ��Ķ�����IDR������P֡
    FU_HEADER* pFuHeader = (FU_HEADER*)(pstRtpData->pPayload + 1);
    //���Ϊ�ط�ģʽ�����յ�һ��IDRʱ����ʾ��ǰΪ�µ�GOP��ǿ�Ʒ��ͻ�����������֡����
    if (H264_IDR_NALU_TYPE == pFuHeader->TYPE)
    {
        //ǿ�Ʒ�������frameList�����֡����
        unsigned int listSize = m_FrameList.size();
        for (unsigned int i = 0; i < listSize; i++)
        {
            SendLastListFrame(FRAME_MUST_SEND);
        }
        IVS_LOG(IVS_LOG_DEBUG, "Check IDR Frame", "Clear Date. TYPE:%u.",
            pFuHeader->TYPE);
        m_FrameList.clear();
    }
} //lint !e818

//����ʱ�����������֮ǰ������
void CH264Reassemble::CheckTimeStampFail(unsigned long ulTimeStamp)
{
	if (TRUE == m_iPlayBackMode)
	{
		return;
	}
    if (m_FrameList.empty())
    {
		IVS_LOG(IVS_LOG_DEBUG, "Check TimeStamp", "FrameSize is Empty");
        return;
    }
    //��ȡ������ʱ�����������
    RTP_FRAME_INFO* frameInfo = m_FrameList.front();
    if (NULL == frameInfo)
    {
        return;
    }
    unsigned long maxTimeStamp = frameInfo->ulTimeStamp;
    long timeGap = (long)(maxTimeStamp - ulTimeStamp);
    //ֻ��ʱ�������ΪС����
    if (MAX_TIME_GAP > timeGap)
    {
        return;
    }
    //���·�����ʱ���������
	IVS_LOG(IVS_LOG_ERR, "Check TimeStamp", " Time hopping %ld ", timeGap);
    //ǿ�Ʒ�������frameList�����֡����
    unsigned int listSize = m_FrameList.size();
    for (unsigned int i = 0; i < listSize; i++)
    {
        SendLastListFrame(FRAME_MUST_SEND);
    }
    m_FrameList.clear();
}

//����Nalu�Ķ���ֱ�ӷŻؽ���
int CH264Reassemble::InsertSingleNalu(RTP_PACK_INFO* pstRtpData)
{
	if (NULL == pstRtpData)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Rtp Data is NULL");
		return IVS_PARA_INVALID;
	}

	NALU_HEADER* pNaluHeader = (NALU_HEADER*)(pstRtpData->pPayload);
    if (NULL == pNaluHeader)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Nalu Header is NULL");
        return IVS_PARA_INVALID;
    }
	//�������ͣ���ȡ���е�H264�ṹ
	unsigned char fuType = pNaluHeader->TYPE;

	RTP_FRAME_INFO* frameInfo = IVS_NEW((RTP_FRAME_INFO * &)frameInfo);
	if (NULL == frameInfo)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Creation Frame Info Error" );
		return IVS_FAIL;
	}

	frameInfo->nalu = pstRtpData->nalu;
	frameInfo->bE   = true;
	frameInfo->bS   = true;
	frameInfo->bAll = true;
	frameInfo->ullTick = pstRtpData->ullTick; 
	frameInfo->ulTimeStamp = pstRtpData->ulTimeStamp;
	frameInfo->fuType = fuType;
	frameInfo->usLostFrame = pstRtpData->usLostFrame;
	frameInfo->uWaterMarkValue = pstRtpData->uWaterMark;
	frameInfo->pPackList.push_front(pstRtpData);      //���뵽list
	int iRet = IVS_SUCCEED;
	switch (fuType)
	{
	case H264_SEI_NALU_TYPE:
		SendSingleNalu(frameInfo); //����
		IVS_DELETE(frameInfo);
		break;
	case H264_SPS_NALU_TYPE:      //���ΪSPS,��Ԥ���ȡͼ������Ϣ
		GetPicHW(frameInfo); 
		/* fall through */
		//lint -fallthrough
	default:
		if (TRUE == m_iPlayBackMode)
		{
			//��֡�ط�ģʽ��SPS��PPS���˾ͷ���
			if (H264_SPS_NALU_TYPE == fuType || H264_PPS_NALU_TYPE == fuType)
			{
				SendSingleNalu(frameInfo);
				IVS_DELETE(frameInfo);
			}
			else
			{
				iRet = InsertFrameToPlayBlackCache(frameInfo,MAX_GOP_FRAME_SIZE);
			}
			break;
		}
		iRet= InsertFrameToCache(frameInfo);
		tryToSendFUA();
		break;
	}
	return iRet;
}

//����FUA �ְ�,�ڲ����µ�����֮ǰ�ж��ǻط�ģʽ�����Ƿ�ʱ�������������
int CH264Reassemble::InsertFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert FUA", " Rtp Data is NULL");
        return IVS_PARA_INVALID;
    }
    if (pstRtpData->lPayloadLen < 3) //FUA ����Ҫ����Ϊ3
    {
        IVS_LOG(IVS_LOG_ERR, "Insert FUA", "H264 insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return IVS_PARA_INVALID;
    }
    //��ȡ��ǰ�ְ��Ķ�����IDR������P֡
    FU_HEADER* pFuHeader = (FU_HEADER*)(pstRtpData->pPayload + 1);
    //���嵱ǰ֡��IDR������P֡
    pstRtpData->fuType = pFuHeader->TYPE;
    int iRet = IVS_FAIL;
    //�鿴���������Ƿ����ʱ�����ͬ��֡����
    if (NULL == GetFrameFromTimeTick(pstRtpData,H264REASSEMBLE))
    {
        iRet = InsertNewFUA(pstRtpData);

		tryToSendFUA();
    }
    else
    {
        iRet = InsertExistFUA(pstRtpData);
		//����Ƿ���Է���һ֡
		tryToSendFUA();
    }
    return iRet;
}

//�����µ�FUA��
int CH264Reassemble::InsertNewFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert New FUA", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }

    RTP_FRAME_INFO* frameInfo = IVS_NEW((RTP_FRAME_INFO * &)frameInfo);
    if (NULL == frameInfo)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert New FUA", " Alloc Frame Info Error" );
        return IVS_FAIL;
    }

    frameInfo->bE     = false;
    frameInfo->bS     = false;
    frameInfo->bAll   = false;

	frameInfo->nalu   = pstRtpData->nalu;
    frameInfo->fuType = pstRtpData->fuType;
	frameInfo->ullTick = pstRtpData->ullTick; 
    frameInfo->ulTimeStamp = pstRtpData->ulTimeStamp;
	frameInfo->usLostFrame = pstRtpData->usLostFrame;
	frameInfo->uWaterMarkValue = pstRtpData->uWaterMark;
    frameInfo->pPackList.push_front(pstRtpData); //���뵽list
    SetFrameByFu(frameInfo, pstRtpData); //������ʼλ

	//���֡�Ƿ���ȫ;
	CheckFrameSeq(frameInfo);  

	int iRet = IVS_FAIL;
	if (TRUE == m_iPlayBackMode)
	{
		iRet = InsertFrameToPlayBlackCache(frameInfo, MAX_GOP_FRAME_SIZE);
	}
	else
	{
		iRet= InsertFrameToCache(frameInfo);
	}
    if (IVS_SUCCEED != iRet)
    {
        IVS_DELETE(frameInfo);
        frameInfo = NULL;
    }
    return iRet;//lint !e438
}

//����֡�յ���־

void CH264Reassemble::SetFrameByFu(RTP_FRAME_INFO* pstFrame, const RTP_PACK_INFO* pstRtpData)
{
    if ((NULL == pstRtpData) || (NULL == pstFrame))
    {
        IVS_LOG(IVS_LOG_ERR, "Set Frame state", "Data is NULL" );
        return;
    }
    FU_HEADER* pFuHdr = (FU_HEADER*)(pstRtpData->pPayload + 1);
    if (pFuHdr->S)
    {
        pstFrame->bS = true;
    }

    if (pFuHdr->E)
    {
        pstFrame->bE = true;
		//���ڵľ���ʱ���ֻ�ڽ�����ʾλ����;
		pstFrame->ullTick = pstRtpData->ullTick;
    }
} //lint !e1762

//�����Ѿ����ڵ�FUA��
int CH264Reassemble::InsertExistFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }
    //����϶�����Ϊ�գ�ֻ�в�Ϊ�ղŻ�ִ���������;
    RTP_FRAME_INFO* pstFrame = GetFrameFromTimeTick(pstRtpData, H264REASSEMBLE);
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " pstFrame is NULL" );
        return IVS_PARA_INVALID;
    }

    RTP_PACK_LIST* pstList = &(pstFrame->pPackList);
    RTP_PACK_LIST_ITER beginIter = pstList->begin();
    RTP_PACK_LIST_ITER endIter = pstList->end();
    RTP_PACK_LIST_ITER iterTmp = endIter;
    unsigned short seqNum = pstRtpData->usSeqNum;
    for (; beginIter != endIter; ++beginIter)
    {
        if (NULL == (*beginIter))
        {
            IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " RTP Pack info is NULL" );
            continue;
        }

        if (SEQ_AFTER(seqNum, (*beginIter)->usSeqNum))
        {
            iterTmp = beginIter;
            //��ɲ��룬�˳�ѭ��
            break;
        }
        //��������ظ��İ�
        if (seqNum == (*beginIter)->usSeqNum)
        {
			IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " Exist same seq packet" );
            return IVS_FAIL;
        }
    }
    //���뵽��ȷ��λ��
    (void)pstList->insert(iterTmp, pstRtpData); //lint !e534
    SetFrameByFu(pstFrame, pstRtpData); //������ʼλ
    CheckFrameSeq(pstFrame);

	//���Ѵ��ڵİ����ݶ���ֵ�ۼӣ�ˮӡ��Ч��;
	pstFrame->usLostFrame += pstRtpData->usLostFrame;
	if (0 != pstRtpData->uWaterMark)
	{
		pstFrame->uWaterMarkValue = pstRtpData->uWaterMark;
	}

	if (0 != pstRtpData->ullTick)
	{
		pstFrame->ullTick = pstRtpData->ullTick;
	}
    return IVS_SUCCEED;
}

//��ǿ�Ʒ��ͣ����������ŷ������϶�Ϊ����֡�ŷ�
bool CH264Reassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Complete Frame", " Rtp Data is NULL" );
		return false;
	}
	//���Ҫ���͵�֡û��ȫ�����أ������ȴ���ȫ
	if (!pstFrame->bAll)
	{
		return false;
	}

	//���֡��ȫ������Ϊ���������Ե�������ʽ����
	if ((1 == pstFrame->pPackList.size()) && (H264_NALU_TYPE_FU_A != pstFrame->nalu))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFUA(pstFrame); //�Ѿ�ɾ����ÿ��packet
	}
	return true;
}

//���ݵ�ǰ�Ĳ���ģʽѡ���Բ��ò�ͬ�ķ���֡��ʽ
void  CH264Reassemble::tryToSendFUA()
{
	//���Ϊ��֡�ط�ģʽ
	if (TRUE == m_iPlayBackMode)
	{
		SendGopFrame();
	}
	else
	{
		SendLastListFrame();
	}
}

void CH264Reassemble::SendGopFrame()
{
	if (m_FrameList.empty())
	{
		IVS_LOG(IVS_LOG_DEBUG, "Send Frame", "No Frame Info to Send");
		return;
	}
	//����FrameList�����IDRλ��
	RTP_FRAME_LIST_ITER idrIter = m_FrameList.begin();
	RTP_FRAME_LIST_ITER endIter = m_FrameList.end();
	//��֡���ݷ���
	RTP_FRAME_INFO* pFrameTmp = NULL;
	bool  bGetIdrFrame = false;
	//���ҵ���������ĵ�һ��IDR
	for (;idrIter != endIter; idrIter++)
	{
		pFrameTmp = *idrIter;
		if (NULL != pFrameTmp && pFrameTmp->fuType == H264_IDR_NALU_TYPE)
		{
			bGetIdrFrame = true;
			break;
		}
	}
	if (NULL == pFrameTmp)
	{
		return;
	}
	//�����ǰ֡�Ѿ���ȫ���߻�������Ѿ���
	if (bGetIdrFrame && (pFrameTmp->bAll ||
		MAX_GOP_FRAME_SIZE <= m_FrameList.size()))
	{
		for (;idrIter != endIter;)
		{
			pFrameTmp = *idrIter;
			SendFrame(pFrameTmp);
			m_FrameList.erase(idrIter++);
			IVS_DELETE(pFrameTmp);
			pFrameTmp = NULL;
		}
	}	
}
//���͵���nalu֡
void CH264Reassemble::SendSingleNalu(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Send Single Nalu", "Frame Info is NULL" );
        return;
    }
	if (NULL == m_pFrameBuf)
	{
		ReleaseRtpInfo(pstFrame);
		IVS_LOG(IVS_LOG_ERR, "Send Single Nalu", "Frame Buf is NULL" );
		return ;
	}
    if (pstFrame->pPackList.size() != 1) //Ӧ����һ���ĵ���
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send Single Nalu",
                "H264 splitter send single nalu error,time stamp:%u, pack list size:%d is not 1",
                pstFrame->pPackList.front()->ulTimeStamp, pstFrame->pPackList.size());
    }
    //��ǰҪ���͵�֡����һ�η��͵Ĳ�һ���ŷ���
    else
    {
        RTP_PACK_INFO* pPack = pstFrame->pPackList.front();
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send Single Nalu", "RTP Pack info is NULL" );
            ReleaseRtpInfo(pstFrame);
            return;
        }
        //������֡����������Ϊͷ��������startcode
        memcpy(m_pFrameBuf + H264_STARTCODE_LEN, pPack->pPayload, (unsigned int)pPack->lPayloadLen);
        //�ص������������ݷ��ͳ�
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H264;
        rawFrameInfo.uiFrameType = pstFrame->fuType;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;
		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;

		//����ж�����֡��ȫ����ˮӡֵ��ֵΪ0��������ˮӡУ��;
		if ((0 != pPack->usLostFrame) || (!pstFrame->bAll))
		{
			rawFrameInfo.uiWaterMarkValue = 0;
		}

        rawFrameInfo.uiWidth   = m_uiWidth;
        rawFrameInfo.uiHeight = m_uiHeight;
		rawFrameInfo.uiGopSequence = m_uGopSeq;
        //�հ�״̬
        REC_PACKET_INFO recPacketInfo;
        recPacketInfo.uAllPacketNum  = SING_PACKET_SIZE;
        recPacketInfo.uLostPacketNum = UNLOSE_PACKET_NUM;

        CaculateLostPacketRate(SING_PACKET_SIZE, UNLOSE_PACKET_NUM);
        
        m_lock.Lock(); //lint !e534
        //�ص����ݷ���ƴ��֡������
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, (unsigned long)pPack->lPayloadLen + H264_STARTCODE_LEN,
                              &recPacketInfo,
                              m_pUser);
        }
        m_lock.UnLock(); //lint !e534
        //�������·��͵�����
        m_preSendFrameTimeStamp = pstFrame->fuType;
    }
    ReleaseRtpInfo(pstFrame);
}
void CH264Reassemble::SendFrame(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Frame", " Rtp Data is NULL" );
		return;
	}
    
	if ((1 == pstFrame->pPackList.size()) && (H264_NALU_TYPE_FU_A != pstFrame->nalu))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFUA(pstFrame);
	}
}

//����FUA֡,
int CH264Reassemble::SendFUA(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "Frame Info is NULL" );
        return IVS_FAIL;
    }

    if (NULL == m_pFrameBuf)
    {
        ReleaseRtpInfo(pstFrame);
        IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "Frame Buf is NULL" );
        return IVS_FAIL;
    }

    if (pstFrame->pPackList.empty()) //�������鲻����
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send FUA Frame", "frame pack list size is 0");
        return IVS_FAIL;
    }

    unsigned long ulCopydSize = 1 + H264_STARTCODE_LEN;  //Ĭ�Ͼ���һ��nalu����
    char* pCpBuf = m_pFrameBuf + 1 + H264_STARTCODE_LEN;  //��ʼҪȥ��NALU������startcode

    NALU_HEADER* pNalu = (NALU_HEADER*)(m_pFrameBuf + H264_STARTCODE_LEN); //����startcode
    //���ȶ�nalu��ֵ
    FU_INDICATOR* pFuIndicator = (FU_INDICATOR*)(pstFrame->pPackList.back()->pPayload);
    FU_HEADER* pFuHdr = (FU_HEADER*)(pstFrame->pPackList.back()->pPayload + 1);

    //��nalu��ֵ
    pNalu->F    = pFuIndicator->F;
    pNalu->NRI  = pFuIndicator->NRI;
    pNalu->TYPE = pFuHdr->TYPE;

    bool bError = false;

    RTP_PACK_LIST_REV_ITER revBeginIter = pstFrame->pPackList.rbegin();

    RTP_PACK_LIST_REV_ITER revEndIter = pstFrame->pPackList.rend();

    int ret = IVS_FAIL;

    //ѭ��ɾ��������ڴ�
    for (; revBeginIter != revEndIter; ++revBeginIter)
    {
        RTP_PACK_INFO* pPack = *revBeginIter;
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "RTP Pack info is null.");
            continue;
        }

        unsigned long ulContentSize = (unsigned long)pPack->lPayloadLen - 2; //�ڲ����ʱ��ͱ�֤�˴���2,ȥ��ͷ
        if ((ulCopydSize + ulContentSize) > m_ulMaxBufLen) //��Ҫ���Ƶ������Ѿ�����˻�����
        {
            IVS_LOG(IVS_LOG_ERR, "SendRaw FUA Frame", " Copy Data to buffer flooding");
            bError = true;
            break;
        }

        memcpy(pCpBuf, pPack->pPayload + 2, ulContentSize); //����2��FU�����и���

        pCpBuf += ulContentSize;      //��ַ����
        ulCopydSize += ulContentSize; //����copy������
    }

    //û�з����������ϴη��͵�ʱ�����һ��
    if (!bError && ((pstFrame->ulTimeStamp != m_preSendFrameTimeStamp) || (pstFrame->fuType != m_preSendFrameTimeStamp)))
    {
        //Ӧ���յ��İ���
        unsigned short usShouldRevPacketNum = 0;

        //ʵ���յ��İ���
        unsigned short usRevPacketNum = 0;

        CaculatePacketNum(&usShouldRevPacketNum, &usRevPacketNum);

        //�ص������������ݷ��ͳ�ȥ
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H264;
        rawFrameInfo.uiFrameType = pstFrame->fuType;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;

		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
		//����ж�����֡��ȫ����ˮӡֵ��ֵΪ0��������ˮӡУ��;
		if ((0 != pstFrame->usLostFrame) || (!pstFrame->bAll))
		{
			rawFrameInfo.uiWaterMarkValue = 0;
		}

        rawFrameInfo.uiWidth   = m_uiWidth;
        rawFrameInfo.uiHeight = m_uiHeight;
		if (H264_IDR_NALU_TYPE == rawFrameInfo.uiFrameType)
		{
             m_uGopSeq++;
		}
		rawFrameInfo.uiGopSequence = m_uGopSeq;

        //�հ�״̬
        REC_PACKET_INFO recPacketInfo;
        recPacketInfo.uAllPacketNum  = usShouldRevPacketNum;
        recPacketInfo.uLostPacketNum = usShouldRevPacketNum - usRevPacketNum;

        CaculateLostPacketRate(recPacketInfo.uAllPacketNum, recPacketInfo.uLostPacketNum);
      
        m_lock.Lock(); //lint !e534
        //�ص����ݷ���ƴ��֡������
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, ulCopydSize, &recPacketInfo, m_pUser);
        }
        m_lock.UnLock(); //lint !e534
        //�������·��͵�ʱ���������
        m_preSendFrameTimeStamp = pstFrame->ulTimeStamp;
        m_preSendFrameTimeStamp = pstFrame->fuType;
        ret = IVS_SUCCEED;
    }
    ReleaseRtpInfo(pstFrame);
    return ret;
}//lint !e954
void CH264Reassemble::GetPicHW(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_DEBUG, "GetPicHW", "SPS is NULL");
        return;
    }
    RTP_PACK_INFO* rtpData = pstFrame->pPackList.front();
    if (NULL == rtpData)
    {
        IVS_LOG(IVS_LOG_DEBUG, "GetPicHW", "RTP INFO is NULL");
        return;
    }

    unsigned char* pBuff   = (unsigned char*)(rtpData->pPayload + sizeof(NALU_HEADER));
    unsigned int uiBuffLen = (unsigned int) rtpData->lPayloadLen - sizeof(NALU_HEADER);

    FRAME_HEADER decoderInfo = {0};

    int iRet = GetSPSInfo(pBuff, uiBuffLen, &decoderInfo);
    if (IVS_SUCCEED == iRet)
    {
        m_uiWidth  = decoderInfo.uMBWidth * 16;
        m_uiHeight = decoderInfo.uMBHeight * 16;
    }
}

void CH264Reassemble::ClearFrameInfo()
{
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear H264 Frame");
    RTP_FRAME_LIST_ITER frameListBeginIter = m_FrameList.begin();
    RTP_FRAME_LIST_ITER frameListEndIter = m_FrameList.end();
    for (; frameListBeginIter != frameListEndIter; ++frameListBeginIter)
    {
        ReleaseRtpInfo(*frameListBeginIter);
        IVS_DELETE(*frameListBeginIter);
    }
    m_FrameList.clear(); //���List
    InitPara();
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear H264 Frame end");
}

