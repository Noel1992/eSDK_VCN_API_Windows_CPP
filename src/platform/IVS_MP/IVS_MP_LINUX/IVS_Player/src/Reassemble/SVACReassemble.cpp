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

#include "SVACReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define MAX_TIME_GAP  6000 //���ʱ������

#define SVAC_RTP_FU   28
#define SVAC_RTP_NAL  0 

//lint -e954  ���ļ�954���������⣬ͳһ����
CSvacReassemble::~CSvacReassemble()
{
	m_bStopInserFlag = true;
    m_bIsStepReset = false;
	m_uiWidth = 1920;
	m_uiHeight = 1080;
}

int CSvacReassemble::Init()
{
    //ͬ��һ���Է����㹻��Ľ��뻺��ռ�
    m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
    if (NULL == m_pFrameBuf)
    {
		IVS_LOG(IVS_LOG_ERR,"SVAC Init","Create frame buf error");
        return IVS_FAIL;
    }
    memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //����0

	m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
	if (NULL == m_pDecryptDataBuf)
	{
		IVS_LOG(IVS_LOG_ERR,"SVAC Init","Create Dec frame buf error");
		IVS_DELETE(m_pFrameBuf, MUILI);
		return IVS_FAIL;
	}
	memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //����0

    //��ʼ���ڴ��
    if (IVS_FAIL == m_packetPool.Init())
    {
		IVS_LOG(IVS_LOG_ERR, "SVAC Init", "Create packetPool error");
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

void CSvacReassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}


//��ձ��ػ����������ݣ�����ȫ���ڴ���Լ�������
int CSvacReassemble::ReSetReassemble()
{
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear SVAC Frame begin.");

    m_bStopInserFlag = true;

    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear SVAC Frame end");

    return IVS_SUCCEED;
}

//����RTP�����������֡����
int CSvacReassemble::InsertPacket(char* pBuf, unsigned int uBufLen)
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
			char type = payload[0];
			if (type == SVAC_RTP_FU)
			{
				IVS_LOG(IVS_LOG_ERR, "InsertPacket", "Drop fro New GOP");
				return IVS_SUCCEED;
			}
			
			SVAC_NALU_HEADER *nal_hdr = (SVAC_NALU_HEADER *)&payload[4];
            if (!m_bIsStepReset)
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is false.");
                //�϶�¼��طţ������Pֱ֡���ӵ���ֱ���յ���һ��I֡������������
                if ((SVAC_NAL_SPS != nal_hdr->type) && (SVAC_NAL_PPS != nal_hdr->type))
                {
                    ClearFrameInfo();
                    return IVS_SUCCEED;
                }
				m_bStopInserFlag = false;
            }
            else
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is true.");
                m_bIsStepReset   = false;
                m_bStopInserFlag = false;
            }
        }
    }
	
	FIXED_HEADER* pHdrTmp = (FIXED_HEADER*)pBuf;//lint !e826
	IVS_LOG(IVS_LOG_DEBUG, "Insert Packet", "TimeStamp:%u ,Seq:%u ,marker:%d %u",
		    ntohl(pHdrTmp->timestamp), ntohs(pHdrTmp->seq_no), pHdrTmp->marker, uBufLen);

    RTP_PACK_INFO* pstRtpData = NULL;
    (void)GetRtpPacketInfo(pBuf, uBufLen, pstRtpData);
    if (NULL == pstRtpData)// for pclint
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", "Get RTP Info error");
        return IVS_FAIL;
    }
    
	//��Ƶ����ֱ�ӽ���(rfc3551�غ����Ͷ���)
    if ((PAY_LOAD_TYPE_PCMU == pstRtpData->pt) || 
		(PAY_LOAD_TYPE_PCMA == pstRtpData->pt))
    {
        m_bPCMUorPCMA = true;
        SendAudioFrame(pstRtpData);
        return IVS_SUCCEED;
    }

	//�����MU��CU�Ķ�����,ֻ������Ƶ�� 
    CaculateMU2CULostPacketNum(pstRtpData->usSeqNum);
    CaculateFPS(pstRtpData->ulTimeStamp);
    CaculateVideoCodeRate((unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
  
	unsigned char type = (unsigned char)pstRtpData->pPayload[0];
    if (SVAC_RTP_NAL == type)  //����NALU��Ƭ
    {
        iRet = InsertSingleNalu(pstRtpData);
    }
    else if (SVAC_RTP_FU == type) //��ʱֻ֧��FUA
    {
		//���ʱ����Ƿ���Ծ�䣬�����򽫶��е����ݾ�ǿ�з�����ջ������
		CheckTimeStampFail(pstRtpData->ulTimeStamp);
        iRet = InsertFUA(pstRtpData);
    }
    else
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", " unsupport nal type:%u", type);
    }

    if (IVS_SUCCEED != iRet)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Packet ", "error, timeStamp: %u, Seq:%u", 
			pstRtpData->ulTimeStamp,pstRtpData->usSeqNum);
        FreePacket(pstRtpData);
    }
    return iRet;
}

//��Ⲣ���������¼����طš�ʱ�������
void CSvacReassemble::CheckSpecialEvent(RTP_PACK_INFO* pstRtpData)
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
void CSvacReassemble::CheckIDRAndSend(RTP_PACK_INFO* pstRtpData)
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
        IVS_LOG(IVS_LOG_ERR, "Check IDR Frame", "Svac insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return;
    }

	if (*(pstRtpData->pPayload) == 0)
	{
		return;
	}

    SVAC_NALU_HEADER* nal_hdr = (SVAC_NALU_HEADER*)(pstRtpData->pPayload + 4);
    //���Ϊ�ط�ģʽ�����յ�һ��IDRʱ����ʾ��ǰΪ�µ�GOP��ǿ�Ʒ��ͻ�����������֡����
    if (SVAC_NAL_SPS == nal_hdr->type)
    {
        //ǿ�Ʒ�������frameList�����֡����
        unsigned int listSize = m_FrameList.size();
        for (unsigned int i = 0; i < listSize; i++)
        {
            SendLastListFrame(FRAME_MUST_SEND);
        }
        m_FrameList.clear();
    }

} //lint !e818

//����ʱ�����������֮ǰ������
void CSvacReassemble::CheckTimeStampFail(unsigned long ulTimeStamp)
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
int CSvacReassemble::InsertSingleNalu(RTP_PACK_INFO* pstRtpData)
{
	if (NULL == pstRtpData)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Rtp Data is NULL");
		return IVS_PARA_INVALID;
	}

	RTP_FRAME_INFO* frameInfo = IVS_NEW((RTP_FRAME_INFO * &)frameInfo);
	if (NULL == frameInfo)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Single Nalu", " Creation Frame Info Error" );
		return IVS_FAIL;
	}
	
	SVAC_NALU_HEADER *nalu_hdr = (SVAC_NALU_HEADER *)&pstRtpData->pPayload[4];
	frameInfo->bE   = true;
	frameInfo->bS   = true;
	frameInfo->bAll = true;
    frameInfo->nalu = nalu_hdr->type;
	frameInfo->ullTick     = pstRtpData->ullTick; 
	frameInfo->ulTimeStamp = pstRtpData->ulTimeStamp;
	frameInfo->usLostFrame = pstRtpData->usLostFrame;
	frameInfo->uWaterMarkValue = pstRtpData->uWaterMark;
	frameInfo->pPackList.push_front(pstRtpData);      //���뵽list

	SendSingleNalu(frameInfo); //����
	IVS_DELETE(frameInfo);
	return IVS_SUCCEED;
}

//����FUA �ְ�,�ڲ����µ�����֮ǰ�ж��ǻط�ģʽ�����Ƿ�ʱ�������������
int CSvacReassemble::InsertFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert FUA", " Rtp Data is NULL");
        return IVS_PARA_INVALID;
    }
    if (pstRtpData->lPayloadLen < 3) //FUA ����Ҫ����Ϊ3
    {
        IVS_LOG(IVS_LOG_ERR, "Insert FUA", "Svac insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return IVS_PARA_INVALID;
    }

    //�鿴���������Ƿ����ʱ�����ͬ��֡����
	int iRet = IVS_SUCCEED;
    if (NULL == GetFrameFromTimeTick(pstRtpData))
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

RTP_FRAME_INFO*  CSvacReassemble::GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData)
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
			IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " Svac Rtp Pack info is NULL");
			continue;
		}

		//���ؾ�����ͬʱ���������һ�µ�֡����
		if ((*iterBegin)->ulTimeStamp == pstRtpData->ulTimeStamp && !(*iterBegin)->bAll)
		{
			return *iterBegin;
		}
	}

	return NULL;
}


//�����µ�FUA��
int CSvacReassemble::InsertNewFUA(RTP_PACK_INFO* pstRtpData)
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
	frameInfo->nalu   = 0;
	
	RTP_SVAC_FU_HEADER* pFuHdr = (RTP_SVAC_FU_HEADER*)(pstRtpData->pPayload + 1);
	if (pFuHdr->S)
	{
		SVAC_NALU_HEADER *nalu_hdr = (SVAC_NALU_HEADER *)&pstRtpData->pPayload[5];
		frameInfo->nalu = nalu_hdr->type;
	}
	
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

void CSvacReassemble::SetFrameByFu(RTP_FRAME_INFO* pstFrame, const RTP_PACK_INFO* pstRtpData)
{
    if ((NULL == pstRtpData) || (NULL == pstFrame))
    {
        IVS_LOG(IVS_LOG_ERR, "Set Frame state", "Data is NULL" );
        return;
    }

    RTP_SVAC_FU_HEADER* pFuHdr = (RTP_SVAC_FU_HEADER*)(pstRtpData->pPayload + 1);
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
int CSvacReassemble::InsertExistFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }
    //����϶�����Ϊ�գ�ֻ�в�Ϊ�ղŻ�ִ���������;
    RTP_FRAME_INFO* pstFrame = GetFrameFromTimeTick(pstRtpData);
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " pstFrame is NULL" );
        return IVS_PARA_INVALID;
    }

	RTP_SVAC_FU_HEADER* pFuHdr = (RTP_SVAC_FU_HEADER*)(pstRtpData->pPayload + 1);
	if (pFuHdr->S)
	{
		SVAC_NALU_HEADER *nalu_hdr = (SVAC_NALU_HEADER *)&pstRtpData->pPayload[5];
		pstFrame->nalu = nalu_hdr->type;
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
bool CSvacReassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
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
	if (1 == pstFrame->pPackList.size())
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
void  CSvacReassemble::tryToSendFUA()
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

void CSvacReassemble::SendGopFrame()
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
		if (NULL != pFrameTmp && pFrameTmp->nalu == SVAC_NAL_IDR)
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
void CSvacReassemble::SendSingleNalu(RTP_FRAME_INFO* pstFrame)
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
    
	if (pstFrame->pPackList.size() != 1)
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send Single Nalu",
                "SVAC splitter send single nalu error, timestamp:%u, pack list size:%d is not 1",
                pstFrame->pPackList.front()->ulTimeStamp, pstFrame->pPackList.size());
    }
    else
    {
        RTP_PACK_INFO* pPack = pstFrame->pPackList.front();
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send Single Nalu", "RTP Pack info is NULL" );
            ReleaseRtpInfo(pstFrame);
            return;
        }

    	//SVAC �����Դ�00 00 00 01
        memcpy(m_pFrameBuf, pPack->pPayload, (unsigned int)pPack->lPayloadLen); //lint !e586
        //�ص������������ݷ��ͳ�
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_SVAC;
        rawFrameInfo.uiFrameType = ((SVAC_NALU_HEADER*)(&pPack->pPayload[4]))->type;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;
		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
        rawFrameInfo.uiWidth  = m_uiWidth;
        rawFrameInfo.uiHeight = m_uiHeight;
		rawFrameInfo.uiGopSequence = m_uGopSeq;

        CaculateLostPacketRate(SING_PACKET_SIZE, UNLOSE_PACKET_NUM);
        
        m_lock.Lock(); //lint !e534
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, 
				  (unsigned long)pPack->lPayloadLen, NULL, m_pUser);
        }
        m_lock.UnLock(); //lint !e534
    }

    ReleaseRtpInfo(pstFrame);
}
void CSvacReassemble::SendFrame(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Frame", " Rtp Data is NULL" );
		return;
	}
    
	if ((1 == pstFrame->pPackList.size()) && (SVAC_RTP_FU != pstFrame->nalu))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFUA(pstFrame);
	}
}

//����FUA֡,
int CSvacReassemble::SendFUA(RTP_FRAME_INFO *pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "Frame Info is NULL" );
        return IVS_FAIL;
    }

	unsigned short usShouldRevPacketNum = 0;
    unsigned short usRevPacketNum = 0;
    CaculatePacketNum(&usShouldRevPacketNum, &usRevPacketNum);
    CaculateLostPacketRate(usShouldRevPacketNum, usRevPacketNum);

	if (!pstFrame->bAll)
	{
		ReleaseRtpInfo(pstFrame);
		return IVS_FAIL;
	}

    unsigned long ulCopydSize  = 1;
    char* pCpBuf = m_pFrameBuf + 1;
	m_pFrameBuf[0] = 0;

	RTP_PACK_LIST_REV_ITER revBeginIter = pstFrame->pPackList.rbegin();
    RTP_PACK_LIST_REV_ITER revEndIter   = pstFrame->pPackList.rend();
    for (; revBeginIter != revEndIter; ++revBeginIter)
    {
        RTP_PACK_INFO* pPack = *revBeginIter;
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send FU Frame", "RTP Pack info is null.");
            continue;
        }

        unsigned long ulContentSize = (unsigned long)pPack->lPayloadLen - 2;
        if ((ulCopydSize + ulContentSize) > m_ulMaxBufLen) //��Ҫ���Ƶ������Ѿ�����˻�����
        {
            IVS_LOG(IVS_LOG_ERR, "SendRaw FU Frame", " Copy Data to buffer flooding");
            break;
        }

        memcpy(pCpBuf, pPack->pPayload + 2, ulContentSize);  //lint !e586 //����2��FU�����и��� 

        pCpBuf += ulContentSize;      //��ַ����
        ulCopydSize += ulContentSize; //����copy������
    }

    //�ص������������ݷ��ͳ�ȥ
    IVS_RAW_FRAME_INFO rawFrameInfo;
    rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_SVAC;
	rawFrameInfo.uiFrameType = ((SVAC_NALU_HEADER *)&m_pFrameBuf[4])->type;
	rawFrameInfo.ullTimeTick = pstFrame->ullTick;
    rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;
	rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
    rawFrameInfo.uiWidth  = 1920/*m_uiWidth*/;
    rawFrameInfo.uiHeight = 1080/*m_uiHeight*/;
	if (SVAC_NAL_IDR == rawFrameInfo.uiFrameType)
	{
         m_uGopSeq++;
	}
	rawFrameInfo.uiGopSequence = m_uGopSeq;
      
    m_lock.Lock(); //lint !e534
    if (NULL != m_pReassmCallBack)
    {
        m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, ulCopydSize, NULL, m_pUser);
    }
    m_lock.UnLock(); //lint !e534
			
		
	ReleaseRtpInfo(pstFrame);
    return IVS_SUCCEED;
}

void CSvacReassemble::ClearFrameInfo()
{
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear SVAC Frame");
    RTP_FRAME_LIST_ITER frameListBeginIter = m_FrameList.begin();
    RTP_FRAME_LIST_ITER frameListEndIter = m_FrameList.end();
    for (; frameListBeginIter != frameListEndIter; ++frameListBeginIter)
    {
        ReleaseRtpInfo(*frameListBeginIter);
        IVS_DELETE(*frameListBeginIter);
    }
    m_FrameList.clear(); //���List
    InitPara();
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear SVAC Frame end");
}

