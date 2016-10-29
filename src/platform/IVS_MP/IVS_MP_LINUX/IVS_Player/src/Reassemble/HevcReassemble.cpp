/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  HevcReassemble.cpp
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#include "HevcReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define HEVC_STARTCODE_LEN 4 //startcode ����
#define MAX_TIME_GAP  6000 //���ʱ������

#define HEVC_RTP_APS   48
#define HEVC_RTP_FU    49
#define HEVC_RTP_PACI  50

#define RTP_HEVC_PAYLOAD_HEADER_SIZE  2


//lint -e954  ���ļ�954���������⣬ͳһ����
CHevcReassemble::CHevcReassemble() 
	: CPacketReassemble()
    , m_using_donl_field(false)
{
	m_using_donl_field = false;
}

CHevcReassemble::~CHevcReassemble()
{
	m_bStopInserFlag = true;
    m_bIsStepReset = false;
}

int CHevcReassemble::Init()
{
    //ͬ��һ���Է����㹻��Ľ��뻺��ռ�
    m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
    if (NULL == m_pFrameBuf)
    {
		IVS_LOG(IVS_LOG_ERR, "Hevc Init", "Create frame buf error");
        return IVS_FAIL;
    }
    memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //����0

	m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
	if (NULL == m_pDecryptDataBuf)
	{
		IVS_LOG(IVS_LOG_ERR,"Hevc Init", "Create Dec frame buf error");
		IVS_DELETE(m_pFrameBuf, MUILI);
		return IVS_FAIL;
	}
	memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //����0

    //��ʼ���ڴ��
    if (IVS_FAIL == m_packetPool.Init())
    {
		IVS_LOG(IVS_LOG_ERR,"Hevc Init","Create packetPool error");
        IVS_DELETE(m_pFrameBuf, MUILI);
        IVS_DELETE(m_pDecryptDataBuf, MUILI);
        m_packetPool.FreeMemPool();

        return IVS_FAIL;
    }
    m_pFrameBuf[3] = 0x01;     //startcode ���ֽ� 00 00 00 01
    m_bStopInserFlag = false;
    m_bIsStepReset = false;
    return IVS_SUCCEED;
}

void CHevcReassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}


//��ձ��ػ����������ݣ�����ȫ���ڴ���Լ�������
int CHevcReassemble::ReSetReassemble()
{
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Hevc Frame begin.");

    m_bStopInserFlag = true;

	/*ClearFrameInfo();
	m_bStopInserFlag = false;*/
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Hevc Frame end");

    return IVS_SUCCEED;
}

//����RTP�����������֡����
int CHevcReassemble::InsertPacket(char* pBuf, unsigned int uBufLen)
{
    int iRet = IVS_FAIL;

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

    		char *payload = pBuf + ulRtpHdrLen + usProfileLen + usExtLen + usExtDateLen;
			unsigned char nal_type = ((unsigned char)payload[0] >> 1) & 0x3f;
			
			RTP_HEVC_FU_HEADER *fuHdr = (RTP_HEVC_FU_HEADER *)(payload + RTP_HEVC_PAYLOAD_HEADER_SIZE);
            IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "pFuIndicator->TYPE:%u. pFuHeader->TYPE:%u.", nal_type, fuHdr->Type);

            if (!m_bIsStepReset)
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is false.");
                //�϶�¼��طţ������Pֱ֡���ӵ���ֱ���յ���һ��I֡������������
                if ((HEVC_NAL_VPS != nal_type) && 
					(HEVC_NAL_SPS != nal_type) &&
					(HEVC_NAL_PPS != nal_type) &&
					(HEVC_NAL_SEI_PREFIX != nal_type) &&
					(HEVC_NAL_SEI_SUFFIX != nal_type) && 
					(HEVC_NAL_IDR_N_LP  != fuHdr->Type) &&
					(HEVC_NAL_IDR_W_RADL!= fuHdr->Type))
                {
                    IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "Not I Frame So Clear. nal_type:%u. fu_type:%u", nal_type, fuHdr->Type);
                    ClearFrameInfo();
                    return IVS_SUCCEED;
                }
				else if (HEVC_NAL_IDR_W_RADL == fuHdr->Type || // fix me
					     HEVC_NAL_IDR_N_LP   == fuHdr->Type)
                {
                    IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "This I Frame, Input It. pFuHeader->TYPE:%u.", fuHdr->Type);
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
        }
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
                "Hevc insert packet error, check RTP header failed, error code:%d", iRet);
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

	unsigned char nal_type = ((unsigned char)pstRtpData->pPayload[0] >> 1) & 0x3f;

	switch (nal_type)
	{
	case HEVC_RTP_FU:
		//���ʱ����Ƿ���Ծ�䣬�����򽫶��е����ݾ�ǿ�з�����ջ������
		CheckTimeStampFail(pstRtpData->ulTimeStamp);
		iRet = InsertFU(pstRtpData);
		break;

	case HEVC_RTP_APS:
	case HEVC_RTP_PACI:
		iRet = IVS_FAIL;
		IVS_LOG(IVS_LOG_ERR, "Hevc Insert Packet", "unsupport Type type:%u", nal_type);
		break;
 	
	case HEVC_NAL_VPS:
	case HEVC_NAL_SPS:
	case HEVC_NAL_PPS:
	case HEVC_NAL_SEI_PREFIX:
	case HEVC_NAL_SEI_SUFFIX:
	default:
		iRet = InsertSingleNalu(pstRtpData);
		break;
	}

	if (iRet != IVS_SUCCEED)
	{
		FreePacket(pstRtpData);
		return IVS_FAIL;
	}
	else//�����MU��CU�Ķ�����,ֻ������Ƶ�� 
	{
		CaculateMU2CULostPacketNum(pstRtpData->usSeqNum);
		CaculateFPS(pstRtpData->ulTimeStamp);
		CaculateVideoCodeRate((unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
	}
 	
	return iRet;
}


//��Ⲣ���������¼����طš�ʱ�������
void CHevcReassemble::CheckSpecialEvent(RTP_PACK_INFO* pstRtpData)
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
void CHevcReassemble::CheckIDRAndSend(RTP_PACK_INFO* pstRtpData)
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
    if (4 > pstRtpData->lPayloadLen) //FU ����Ҫ����Ϊ4: 1 ��payload
    {
        IVS_LOG(IVS_LOG_ERR, "Check IDR Frame", "HEVC insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return;
    }
    //��ȡ��ǰ�ְ��Ķ�����IDR������P֡
    RTP_HEVC_FU_HEADER* pFuHeader = (RTP_HEVC_FU_HEADER*)(pstRtpData->pPayload + 2);
    //���Ϊ�ط�ģʽ�����յ�һ��IDRʱ����ʾ��ǰΪ�µ�GOP��ǿ�Ʒ��ͻ�����������֡����
    if (HEVC_NAL_IDR_N_LP == pFuHeader->Type ||
		HEVC_NAL_IDR_W_RADL == pFuHeader->Type)
    {
        //ǿ�Ʒ�������frameList�����֡����
        unsigned int listSize = m_FrameList.size();
        for (unsigned int i = 0; i < listSize; i++)
        {
            SendLastListFrame(FRAME_MUST_SEND);
        }
        IVS_LOG(IVS_LOG_DEBUG, "Check IDR Frame", "Clear Date. TYPE:%u.", pFuHeader->Type);
        m_FrameList.clear();
    }
} //lint !e818

//����ʱ�����������֮ǰ������
void CHevcReassemble::CheckTimeStampFail(unsigned long ulTimeStamp)
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
int CHevcReassemble::InsertSingleNalu(RTP_PACK_INFO* pstRtpData)
{
	if (NULL == pstRtpData)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Rtp Data is NULL");
		return IVS_PARA_INVALID;
	}

	unsigned char nal_type = ((unsigned char)pstRtpData->pPayload[0] >> 1) & 0x3f;
	RTP_FRAME_INFO* frameInfo = IVS_NEW((RTP_FRAME_INFO * &)frameInfo);
	if (NULL == frameInfo)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Creation Frame Info Error" );
		return IVS_FAIL;
	}
	
	frameInfo->bE   = true;
	frameInfo->bS   = true;
	frameInfo->bAll = true;
	frameInfo->ullTick = pstRtpData->ullTick; 
	frameInfo->ulTimeStamp = pstRtpData->ulTimeStamp;
	frameInfo->hevc_rtp_payload_type = nal_type;
	frameInfo->hevc_nal_type = nal_type;
	frameInfo->usLostFrame = pstRtpData->usLostFrame;
	frameInfo->uWaterMarkValue = pstRtpData->uWaterMark;
	frameInfo->pPackList.push_front(pstRtpData);      //���뵽list
	int iRet = IVS_SUCCEED;
	switch (nal_type)
	{
	case HEVC_NAL_SPS:      //���ΪSPS,��Ԥ���ȡͼ������Ϣ
		GetPicHW(frameInfo); 
		/* fall through */
		//lint -fallthrough
	default:
		if (TRUE == m_iPlayBackMode)
		{
			//��֡�ط�ģʽ��SPS��PPS���˾ͷ���
			if (HEVC_NAL_SEI_PREFIX == nal_type ||
				HEVC_NAL_SEI_SUFFIX == nal_type ||
				HEVC_NAL_SPS == nal_type || 
				HEVC_NAL_PPS == nal_type ||  
				HEVC_NAL_VPS == nal_type)
			{
				SendSingleNalu(frameInfo);
				IVS_DELETE(frameInfo);
			}
			else
			{
				(void)InsertFrameToPlayBlackCache(frameInfo, MAX_GOP_FRAME_SIZE);
			}
			break;
		}
		iRet= InsertFrameToCache(frameInfo);
		tryToSendFU();
		break;
	}
	return iRet;
}

//����FUA �ְ�,�ڲ����µ�����֮ǰ�ж��ǻط�ģʽ�����Ƿ�ʱ�������������
int CHevcReassemble::InsertFU(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert FUA", " Rtp Data is NULL");
        return IVS_PARA_INVALID;
    }
    if (pstRtpData->lPayloadLen < (2 + 1 + 1)) // playlaodhdr + fuheader + 1byte payload
    {
        IVS_LOG(IVS_LOG_ERR, "Insert FU", "Hevc insert FU error, pstRtp data payload length:%u < 4",
                pstRtpData->lPayloadLen);
        return IVS_PARA_INVALID;
    }

    //��ȡ��ǰ�ְ��Ķ�����IDR������P֡
    RTP_HEVC_FU_HEADER *pFuHeader = (RTP_HEVC_FU_HEADER *)(pstRtpData->pPayload + 2);
    //���嵱ǰ֡��IDR������P֡
    pstRtpData->hevc_nal_type = pFuHeader->Type;

    int iRet = IVS_FAIL;
    //�鿴���������Ƿ����ʱ�����ͬ��֡����
    if (NULL == GetFrameFromTimeTick(pstRtpData))
    {
        iRet = InsertNewFU(pstRtpData);

		tryToSendFU();
    }
    else
    {
        iRet = InsertExistFU(pstRtpData);
		//����Ƿ���Է���һ֡
		tryToSendFU();
    }
    return iRet;
}


RTP_FRAME_INFO*  CHevcReassemble::GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " Rtp Data is NULL");
        return NULL;
    }

    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
	for (; iterBegin != iterEnd; iterBegin++)
	{
        if (NULL == (*iterBegin))
        {
            IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " Hevc Rtp Pack info is NULL");
            continue;
        }

		//���ؾ�����ͬʱ���������һ�µ�֡����
		if ((*iterBegin)->ulTimeStamp == pstRtpData->ulTimeStamp &&
			(*iterBegin)->hevc_nal_type == pstRtpData->hevc_nal_type &&
			!(*iterBegin)->bAll)
		{
			return *iterBegin;
		}
	}

    return NULL;
}


//�����µ�FU��
int CHevcReassemble::InsertNewFU(RTP_PACK_INFO* pstRtpData)
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
	frameInfo->hevc_rtp_payload_type = HEVC_RTP_FU;
    frameInfo->hevc_nal_type = pstRtpData->hevc_nal_type;
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

void CHevcReassemble::SetFrameByFu(RTP_FRAME_INFO* pstFrame, const RTP_PACK_INFO* pstRtpData)
{
    if ((NULL == pstRtpData) || (NULL == pstFrame))
    {
        IVS_LOG(IVS_LOG_ERR, "Set Frame state", "Data is NULL" );
        return;
    }
    RTP_HEVC_FU_HEADER* pFuHdr = (RTP_HEVC_FU_HEADER*)(pstRtpData->pPayload + 2);
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
int CHevcReassemble::InsertExistFU(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FU", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }
    //����϶�����Ϊ�գ�ֻ�в�Ϊ�ղŻ�ִ���������;
    RTP_FRAME_INFO* pstFrame = GetFrameFromTimeTick(pstRtpData);
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
            IVS_LOG(IVS_LOG_ERR, "Insert Exist FU", " RTP Pack info is NULL" );
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
bool CHevcReassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
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
	if ((1 == pstFrame->pPackList.size()) && 
		(HEVC_RTP_FU != pstFrame->hevc_rtp_payload_type))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFU(pstFrame); //�Ѿ�ɾ����ÿ��packet
	}
	return true;
}

//���ݵ�ǰ�Ĳ���ģʽѡ���Բ��ò�ͬ�ķ���֡��ʽ
void  CHevcReassemble::tryToSendFU()
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

void CHevcReassemble::SendGopFrame()
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
		if (NULL != pFrameTmp && 
			(pFrameTmp->hevc_nal_type == HEVC_NAL_IDR_W_RADL || 
    		 pFrameTmp->hevc_nal_type == HEVC_NAL_IDR_N_LP))
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
void CHevcReassemble::SendSingleNalu(RTP_FRAME_INFO* pstFrame)
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
                "HEVC splitter send single nalu error,time stamp:%u, pack list size:%d is not 1",
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
        memcpy(m_pFrameBuf + HEVC_STARTCODE_LEN, pPack->pPayload, (unsigned int)pPack->lPayloadLen); //lint !e586
        //�ص������������ݷ��ͳ�
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H265;
        rawFrameInfo.uiFrameType = pstFrame->hevc_nal_type;
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
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, (unsigned long)pPack->lPayloadLen + HEVC_STARTCODE_LEN,
                              &recPacketInfo,
                              m_pUser);
        }
        m_lock.UnLock(); //lint !e534
        //�������·��͵�����
        m_preSendFrameTimeStamp = pstFrame->fuType;
    }
    ReleaseRtpInfo(pstFrame);
}
void CHevcReassemble::SendFrame(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Frame", " Rtp Data is NULL" );
		return;
	}
    
	if ((1 == pstFrame->pPackList.size()) && 
		(HEVC_RTP_FU != pstFrame->hevc_rtp_payload_type))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void)SendFU(pstFrame);
	}
}

//����FUA֡,
int CHevcReassemble::SendFU(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Send FU Frame", "Frame Info is NULL" );
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

    unsigned long ulCopydSize = 2 + HEVC_STARTCODE_LEN;  
    char* pCpBuf = m_pFrameBuf + 2 + HEVC_STARTCODE_LEN;

  	// �ع�nal header
    unsigned char *payload = (unsigned char *)pstFrame->pPackList.back()->pPayload;
	unsigned char *pNalHdr = (unsigned char *)m_pFrameBuf + HEVC_STARTCODE_LEN;
	pNalHdr[0] = (payload[0] & 0x81) | ((payload[2] & 0x3f) << 1); 
	pNalHdr[1] = payload[1]; 

    bool bError = false;

    RTP_PACK_LIST_REV_ITER revBeginIter = pstFrame->pPackList.rbegin();

    RTP_PACK_LIST_REV_ITER revEndIter = pstFrame->pPackList.rend();

    int ret = IVS_FAIL;

	unsigned int skip = RTP_HEVC_PAYLOAD_HEADER_SIZE + sizeof(RTP_HEVC_FU_HEADER) + (m_using_donl_field ? 2 : 0);

    //ѭ��ɾ��������ڴ�
    for (; revBeginIter != revEndIter; ++revBeginIter)
    {
        RTP_PACK_INFO* pPack = *revBeginIter;
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "RTP Pack info is null.");
            continue;
        }

        unsigned long ulContentSize = (unsigned long)pPack->lPayloadLen - skip; //�ڲ����ʱ��ͱ�֤�˴���3,ȥ��ͷ
        if ((ulCopydSize + ulContentSize) > m_ulMaxBufLen) //��Ҫ���Ƶ������Ѿ�����˻�����
        {
            IVS_LOG(IVS_LOG_ERR, "SendRaw FUA Frame", " Copy Data to buffer flooding");
            bError = true;
            break;
        }

        memcpy(pCpBuf, pPack->pPayload + skip, ulContentSize);  //lint !e586 //����2��FU�����и��� 

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
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H265;
        rawFrameInfo.uiFrameType = pstFrame->hevc_nal_type;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;

		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
		//����ж�����֡��ȫ����ˮӡֵ��ֵΪ0��������ˮӡУ��;
		if ((0 != pstFrame->usLostFrame) || (!pstFrame->bAll))
		{
			rawFrameInfo.uiWaterMarkValue = 0;
		}

        rawFrameInfo.uiWidth  = m_uiWidth;
        rawFrameInfo.uiHeight = m_uiHeight;
		if (HEVC_NAL_IDR_W_RADL == rawFrameInfo.uiFrameType || 
			HEVC_NAL_IDR_N_LP == rawFrameInfo.uiFrameType)
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
}



void CHevcReassemble::GetPicHW(RTP_FRAME_INFO* pstFrame)
{
	m_uiWidth  = 1920;
	m_uiHeight = 1080;
}//lint !e715 !e818


void CHevcReassemble::ClearFrameInfo()
{
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear Hevc Frame");
    RTP_FRAME_LIST_ITER frameListBeginIter = m_FrameList.begin();
    RTP_FRAME_LIST_ITER frameListEndIter = m_FrameList.end();
    for (; frameListBeginIter != frameListEndIter; ++frameListBeginIter)
    {
        ReleaseRtpInfo(*frameListBeginIter);
        IVS_DELETE(*frameListBeginIter);
    }
    m_FrameList.clear(); //���List
    InitPara();
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear Hevc Frame end");
}

