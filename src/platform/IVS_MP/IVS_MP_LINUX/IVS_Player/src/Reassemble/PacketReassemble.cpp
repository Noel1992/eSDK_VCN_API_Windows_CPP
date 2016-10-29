/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  PacketReassemble.cpp
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/
#include "PacketReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif

#define  AVG_TIME_COUNT_TIME 20000      //����ƽ������֡�����ʵ�ͳ��ʱ������ms�� 20S
#define  GET_LOST_PACKET_RATE_TIME 5000 //����ͳ�ƶ����ʵ�ʱ����(ms) 5S

#define  MAX_TIME_STAMP_DIFF 99000  // RTPʱ�����Ծ

#define  DEC_RATIO 0.2      //AES���ֽ�����

#define  NO_NEED_ENCRYPTED (-1)  //�����ܱ�־

CPacketReassemble::CPacketReassemble()
{
    InitPara();

    m_uGopSeq = 0;

    m_pReassmCallBack = NULL;
    m_pUser = NULL;
    m_pExceptionCallback = NULL;
    m_pExcUser = NULL;
    m_ulFrameCacheLimit = RTP_MIN_FRAME_CACHE;
    m_iPlayBackMode = FALSE;
    m_bDecryptData = false;
    m_pSecretKey = NULL;
    m_ulKeyLen = 0;
    m_iDecryptDataType  = NO_DEC_TYPE;
    m_bAudioDecryptData = false;
    m_pAudioSecretKey = NULL;
    m_ulAudioKeyLen = 0;
    m_iAudioDecryptDataType = NO_DEC_TYPE;
    m_nWaterMarkState = 0;
    m_ulTimeStamp = 0;
	m_pFrameBuf = NULL;
	m_pDecryptDataBuf = NULL;

	m_pMemPoolJumbo = NULL;

    //��ʼ�����
    m_uiWidth  = 0;
    m_uiHeight = 0;

	memset(&m_AudioAttr, 0, sizeof(m_AudioAttr));
	memset(&m_VideoAttr, 0, sizeof(m_VideoAttr));

    m_bIsFirstGetVideoRate = true;
    m_bIsFirstGetAudioRate = true;

    m_ulRecFromMUNums = 0;
    m_ulLostMUPacketNums = 0;
    m_usPacketSeqNum = 0;

    m_bStopInserFlag = true;
    m_bIsStepReset = false;
	
	m_bDestroyAll = false;
}

CPacketReassemble::~CPacketReassemble()
{
    //CAutoLock lock(m_DestroyMutex); //lint !e522 !e1551

    m_bDestroyAll = true;

    try
    {
        m_bStopInserFlag = true;

        RTP_FRAME_LIST_ITER frameListBeginIter = m_FrameList.begin();

        RTP_FRAME_LIST_ITER frameListEndIter = m_FrameList.end();

        for (; frameListBeginIter != frameListEndIter; ++frameListBeginIter)
        {
			ReleaseRtpInfo(*frameListBeginIter);
            IVS_DELETE(*frameListBeginIter);
        }
        
		m_FrameList.clear();

		if (m_pMemPoolJumbo)
		{
			delete m_pMemPoolJumbo;
			m_pMemPoolJumbo = NULL;
		}
	}
	catch(...)
	{
		IVS_LOG(IVS_LOG_ERR, "~CPacketReassemble", "throw exception.");//lint !e1551
	}

	try
	{
        IVS_DELETE(m_pFrameBuf, MUILI);
        IVS_DELETE(m_pSecretKey, MUILI);
        IVS_DELETE(m_pAudioSecretKey, MUILI);
        IVS_DELETE(m_pDecryptDataBuf, MUILI);

        m_pReassmCallBack = NULL;

        m_pUser = NULL;

        m_pExceptionCallback = NULL;

        m_pExcUser = NULL;

        m_packetPool.FreeMemPool();

        m_bStopInserFlag = false;
    }
    catch (...)
    {
		IVS_LOG(IVS_LOG_ERR, "~CPacketReassemble", "throw exception.");//lint !e1551
	}
}//lint !e1579 !e1788 !e1551

//�ͷŵ�ǰRTP��ռ���ڴ��
void CPacketReassemble::FreePacket(RTP_PACK_INFO*& pstRtpData)
{
    if (m_bDestroyAll)
    {
        IVS_LOG(IVS_LOG_DEBUG, "FreePacket", "Destroy All, Not Need Free.");
        return;
    }

    if (NULL != pstRtpData)
    {
		if (pstRtpData->bJumbo)
		{
			IVS_ASSERT(m_pMemPoolJumbo);
			if (m_pMemPoolJumbo)
			{
				(void)m_pMemPoolJumbo->RecMemBlock((char *)pstRtpData);
			}
			else
			{
			    IVS_LOG(IVS_LOG_ALERT, "FreePacket", "BUG, jumbo frame NULL");	
			}
		}
		else
		{
			 (void)m_packetPool.RecMemBlock((char *)pstRtpData);
		}
        pstRtpData = NULL;
    }
}

void CPacketReassemble::GetExtensionHead(char* pBuf, unsigned long ulBufLen,unsigned long& ulRtpHdrLen,
											unsigned long long& ullTick,unsigned short& usLostFrame, unsigned int& uiWaterMarkValue)
{
	if (NULL == pBuf)
	{
		IVS_LOG(IVS_LOG_ERR, "Get Extension Head", "buf is NULL");
		return ;
	}
	if (ulRtpHdrLen + sizeof(RTP_EXTEND_HEADER) >= ulBufLen)
	{
		IVS_LOG(IVS_LOG_ERR, "Get Extension Head", "Extension Header Defect");
		return ;
	}

	RTP_EXTEND_HEADER *pExtHdr = (RTP_EXTEND_HEADER *)(pBuf + ulRtpHdrLen);//lint !e826
	ulRtpHdrLen += sizeof(RTP_EXTEND_HEADER);

	unsigned short usExtSize =(unsigned short) ntohs(pExtHdr->usLength) * 4; //lint !e734 // 4�ֽڱ���
	if (ulRtpHdrLen + usExtSize >= ulBufLen)
	{
		IVS_LOG(IVS_LOG_ERR, "Get Extension Head", "Extension Defect");
		return ;
	}

	// ֻ����HW��չͷ
	if (pExtHdr->usProfile == 0x5748)  // 0x5748 = "HW"
	{
		if (usExtSize == sizeof(RTP_EXTENSION_DATA_EX))
		{
			// ��չ����ˮӡ���ݺ�ʱ�������
			RTP_EXTENSION_DATA_EX *pExtension_ = (RTP_EXTENSION_DATA_EX *)(pBuf + ulRtpHdrLen);//lint !e826
			if (WATER_MARK == (unsigned short) ntohs(pExtension_->usWaterMarkType))
			{
				//ˮӡֵ
				uiWaterMarkValue =  pExtension_->uiWaterMarkValue;

				//�Ƿ񶪰���0û�ж�����1��������ˮӡʱ��Ч
				usLostFrame = pExtension_->usLostFrame;
			}
			//����16�ֽڶ���Ķ���������
			if (m_bDecryptData)
			{
				m_uiEncryptExLen = (unsigned int)pExtension_->uiEncryptExLen;
			}

			ullTick = (unsigned long long)ntohl(pExtension_->uiGmtSecond) * 1000 + ntohs(pExtension_->usGmtMillsecond);//lint !e647
		}
		//ֱ��ʱ����չ�ṹ��
		else if (usExtSize == sizeof(RTP_EXTENSION_DATA_S_EX))
		{
			// ��չ����ˮӡ���ݺ�ʱ�������
			RTP_EXTENSION_DATA_S_EX *pExtensionDcg = (RTP_EXTENSION_DATA_S_EX *)(pBuf + ulRtpHdrLen);//lint !e826
			if (WATER_MARK == (unsigned short) ntohs(pExtensionDcg->usWaterMarkType))
			{
				//ˮӡֵ
				uiWaterMarkValue =  pExtensionDcg->uiWaterMarkValue;
			}
			//����16�ֽڶ���Ķ���������
			if (m_bDecryptData)
			{
				m_uiEncryptExLen = (unsigned int)pExtensionDcg->uiEncryptExLen;
			}
		}
		else
		{
			IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr", "unsupported RTP Extention Header[%u]", usExtSize);
		}
	}

	ulRtpHdrLen += usExtSize;
}

//�������������ת��ΪRTP�ṹ����
int CPacketReassemble::GetRtpPacketInfo(char* pBuf, unsigned long ulBufLen, RTP_PACK_INFO *& pstRtpData)
{
    if (NULL == pBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "RTP Packet length :%d", ulBufLen);
        return IVS_FAIL;
    }

    unsigned long ulRtpHdrLen  = 0;
    unsigned int  uWaterMark   = 0;
    unsigned long long ullTick = 0;
	unsigned short usLostFrame = 0;
	//��ʼ���ð���չ����
	m_uiEncryptExLen = 0;

    if (ulBufLen <= sizeof(FIXED_HEADER))
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Packet Too Small Size=[%u]", ulBufLen);
        return IVS_FAIL;
    }

    FIXED_HEADER *pRtpHdr = (FIXED_HEADER*)pBuf; //lint !e826
    if (RTP_VERSION != pRtpHdr->version)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "version=[%d]", pRtpHdr->version);
        return IVS_FAIL;
    }

    ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * pRtpHdr->csrc_len; //ͷ���ȼ���CC����
    if (ulRtpHdrLen >= ulBufLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Big Head Size=[%u]", ulRtpHdrLen);
        return IVS_FAIL;
    }
    // ������չͷ��
    if (1 == pRtpHdr->extension)
    {
        GetExtensionHead(pBuf,ulBufLen,ulRtpHdrLen,ullTick,usLostFrame,uWaterMark);
    }
	
    //���ڴ����ȡ��һ����õ��ڴ�����
    char* pRtpBuffer = NULL; 
	unsigned int BlockSize = BLOCK_SIZE;

	if ((sizeof (RTP_PACK_INFO) + ulBufLen - sizeof(FIXED_HEADER)) > BLOCK_SIZE)
	{
		if (JUMBO_BLOCK_SIZE < (sizeof (RTP_PACK_INFO) + ulBufLen - sizeof(FIXED_HEADER)))
		{
			IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Buf len too large=[%u]", ulBufLen);
			return IVS_FAIL;
		}

	    if (NULL == m_pMemPoolJumbo)
		{
			m_pMemPoolJumbo = IVS_NEW(m_pMemPoolJumbo);
			if (NULL == m_pMemPoolJumbo || IVS_SUCCEED != m_pMemPoolJumbo->Init(JUMBO_BLOCK_COUNT, JUMBO_BLOCK_SIZE) )
			{
				 IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Init Jumbo frame buffer fail");
				 return IVS_FAIL;
			}
    	}
		pRtpBuffer = m_pMemPoolJumbo->GetAvailableMemBlock();
		BlockSize = JUMBO_BLOCK_SIZE;
	}
	else
	{
		pRtpBuffer = m_packetPool.GetAvailableMemBlock();
	}

	if (NULL == pRtpBuffer)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Malloc memory for rtp buffer failed.");
		//�����ڴ���Ѿ����ˣ�����������Դ
		ClearFrameList();
        return IVS_FAIL;
    }

    RTP_PACK_INFO *pRtpPackInfo = (RTP_PACK_INFO *)pRtpBuffer; //lint !e826
    pstRtpData                = pRtpPackInfo;
    pRtpPackInfo->usSeqNum    = ntohs(pRtpHdr->seq_no);
    pRtpPackInfo->ulTimeStamp = ntohl(pRtpHdr->timestamp);
    pRtpPackInfo->ucMark      = pRtpHdr->marker;
    pRtpPackInfo->ulHdrLen    = ulRtpHdrLen;
    pRtpPackInfo->pt          = pRtpHdr->payload;
    pRtpPackInfo->uWaterMark  = uWaterMark;
    pRtpPackInfo->usLostFrame = usLostFrame;
    pRtpPackInfo->ullTick     = ullTick;
    pRtpPackInfo->pPayload    = pRtpBuffer + sizeof(RTP_PACK_INFO);
	pRtpPackInfo->bJumbo      = (int)(BlockSize == JUMBO_BLOCK_SIZE);
	
	if (1 == pRtpHdr->padding)
    {
        //�ܳ���ȥ��ͷ���ȣ�������չ������䳤�� .�˴�ulBufLen��Ȼ����ulRtpHdrLen
        pRtpPackInfo->lPayloadLen = (long) (ulBufLen - ulRtpHdrLen) - (long)(unsigned char)pBuf[ulBufLen - 1]; //lint !e737
    }
    else
    {
        //�ܳ���ȥ��ͷ���ȣ�������չ��
        pRtpPackInfo->lPayloadLen = (long)(ulBufLen - ulRtpHdrLen);
    }

    if ((0 >= pRtpPackInfo->lPayloadLen)
        || (long)(/*BLOCK_SIZE*/BlockSize - sizeof(RTP_PACK_INFO)) < pRtpPackInfo->lPayloadLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Payload too long %ld", pRtpPackInfo->lPayloadLen);
        return IVS_FAIL;
    }

    //��������˽���
    if (m_bDecryptData)
    {
        IVS_LOG(IVS_LOG_DEBUG, "Get Rtp Packet Info", "Need Encrypt, m_uiEncryptExLen:%u ", m_uiEncryptExLen);
        DecryptRtpPacketData(pBuf + ulRtpHdrLen, (unsigned long)pRtpPackInfo->lPayloadLen, pRtpPackInfo->pt);
    }
    else
    {
        m_uiEncryptExLen = 0;
    }

	long lPlayLoadLen = pRtpPackInfo->lPayloadLen - (long)m_uiEncryptExLen;
	if (0 > lPlayLoadLen || lPlayLoadLen > pRtpPackInfo->lPayloadLen )
	{
		IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Encrypt extend Length Error %ld PlayLoadLen[%ld] RtpPackInfoPayLoadLen[%ld]", m_uiEncryptExLen, lPlayLoadLen, pRtpPackInfo->lPayloadLen);
		return IVS_FAIL;
	}
	pRtpPackInfo->lPayloadLen = lPlayLoadLen;
    memcpy(pRtpPackInfo->pPayload, pBuf + ulRtpHdrLen, (unsigned int)pRtpPackInfo->lPayloadLen);

    FU_INDICATOR* pFu = (FU_INDICATOR*)(pRtpPackInfo->pPayload);

    pRtpPackInfo->fuType = pFu->TYPE;     //����type��FU_INDICATOR��NALU����ͬ��

    return IVS_SUCCEED;
}

//���Խ��½���֡���ݼ��뵽��֡�طŻ��浱��
int CPacketReassemble::InsertFrameToPlayBlackCache(RTP_FRAME_INFO* pstFrame,unsigned int uiGopNumLimit)
{
    //��ȡ����frameList�����frame����
    unsigned int uiFrameSize = m_FrameList.size();
    int iRet = IVS_FAIL;

    //����ò�����Ϊfalse,���ʾ��ǰ֡�����⣬����Ҫִ���²�
    bool bDoNextFlag = true;

    //�������һ���޶������GOP���������Է����յ�������
    if (uiGopNumLimit <= uiFrameSize)
    {
        bDoNextFlag = FrameListIsFullAndSend(pstFrame);
    }
    //���뵽FrameList��
    if (bDoNextFlag)
    {
        InsertFrameToPlayBlackList(pstFrame);
        iRet = IVS_SUCCEED;
    }
    return iRet;
}


//�����ݼ��뵽��֡�ط�����list
void CPacketReassemble::InsertFrameToPlayBlackList(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame to play black List", " Frame Info is NULL");
        return;
    }

    //����������Ŀռ�δ��
    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
    RTP_FRAME_LIST_ITER iterTmp = iterEnd;
    unsigned long newTimeStamp = pstFrame->ulTimeStamp;
    if (iterBegin != iterEnd)
    {
        //�жϵ�ǰʱ����Ƿ�С��ȡ�������ʱ���
        unsigned long ulTimeStamp = (*iterBegin)->ulTimeStamp;
        for (; iterBegin != iterEnd; iterBegin++)
        {
            ulTimeStamp = (*iterBegin)->ulTimeStamp;

            //�����ǰʱ���С�ڵ�������Ԫ�أ������λ��
            if (TIME_BEFORE(newTimeStamp, ulTimeStamp))
            {
                iterTmp = iterBegin;
                break;
            }
        }
    }

    //���뵽ָ����λ��
    (void)m_FrameList.insert(iterTmp, pstFrame);
    return;
}

/****************************************************************************************
*
****************************************************************************************/

//���Խ��½���֡���ݲ��뵽���浱��
int CPacketReassemble::InsertFrameToCache(RTP_FRAME_INFO* pstFrame)
{
    //��ȡ����frameList�����frame����
    unsigned int uiFrameSize = m_FrameList.size();
    int iRet = IVS_FAIL;

    //����ò�����Ϊfalse,���ʾ��ǰ֡�����⣬����Ҫִ���²�
    bool bDoNextFlag = true;

    unsigned int frameSizeLimit = m_ulFrameCacheLimit;

    //�������Ѿ���
    if (frameSizeLimit <= uiFrameSize)
    {
        bDoNextFlag = FrameListIsFullAndSend(pstFrame);
    }

    //���뵽FrameList��
    if (bDoNextFlag)
    {
        InsertFrameToList(pstFrame);
        iRet = IVS_SUCCEED;
    }

    return iRet;
}

//����������ɾ������һ֡,���ص�ֵ��ʾ�Ƿ���Ҫ������Ķ���
bool CPacketReassemble::FrameListIsFullAndSend(const RTP_FRAME_INFO* pstFrame)
{
    //ǿ�Ʒ�������е����һ֡
    SendLastListFrame(FRAME_MUST_SEND);
    return true;
}//lint !e715

//���ݼ���֡������
void CPacketReassemble::InsertFrameToList(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame to List", " Frame Info is NULL");
        return;
    }

    //����������Ŀռ�δ��
    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();

    RTP_FRAME_LIST_ITER iterTmp = iterEnd;

    unsigned long newTimeStamp = pstFrame->ulTimeStamp;

    if (iterBegin != iterEnd)
    {
        //�жϵ�ǰʱ����Ƿ����ȡ�������ʱ���
        unsigned long ulTimeStamp = (*iterBegin)->ulTimeStamp;

        for (; iterBegin != iterEnd; iterBegin++)
        {
            ulTimeStamp = (*iterBegin)->ulTimeStamp;

            //�����ǰʱ������ڵ�������Ԫ�أ������λ��
            if (TIME_AFTER(newTimeStamp, ulTimeStamp) || (newTimeStamp == ulTimeStamp))
            {
                iterTmp = iterBegin;
                break;
            }
        }
    }

    //���뵽ָ����λ��
    (void)m_FrameList.insert(iterTmp, pstFrame);

    return;
}

//��ȡ������ͬʱ�����֡����
RTP_FRAME_INFO*  CPacketReassemble::GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData,E_REASSEMBLE_CLASS_TYPE eReassmbleType)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " Rtp Data is NULL");
        return NULL;
    }

    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
    unsigned long timeStamp = pstRtpData->ulTimeStamp;
	unsigned char cFuType = pstRtpData->fuType;
	if (H264REASSEMBLE == eReassmbleType)
	{
		for (; iterBegin != iterEnd; iterBegin++)
		{
            if (NULL == (*iterBegin))
            {
                IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " H264 Rtp Pack info is NULL");
                continue;
            }

			//���ؾ�����ͬʱ���������һ�µ�֡����
			if ((*iterBegin)->ulTimeStamp == timeStamp &&
				(*iterBegin)->fuType == cFuType &&
				!(*iterBegin)->bAll)
			{
				return *iterBegin;
			}
		}

	}
	else if (MJPEGREASSEMBLE == eReassmbleType)
	{

		//Mjpeg����Ҫ�ж��Ƿ�����ͬ����
		for (; iterBegin != iterEnd; iterBegin++)
		{
			if (NULL == (*iterBegin))
        	{
            	IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " MJPEG Rtp Pack info is NULL");
            	continue;
        	}
			//���ؾ�����ͬʱ���������һ�µ�֡����
			if ((*iterBegin)->ulTimeStamp == timeStamp)
			{
				return *iterBegin;
			}
		}
	}
	else
	{

	}
    return NULL;
}

//�жϰ��Ƿ����룬������Ĭ�ϰ���ŴӴ�С
void CPacketReassemble::CheckFrameSeq(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		return;
	}

    pstFrame->bAll = false;

    //�յ��˰�ͷ��β
    if (pstFrame->bE && pstFrame->bS)
    {
        RTP_PACK_LIST* pList = &(pstFrame->pPackList);

        unsigned short packSize = (pList->front()->usSeqNum - pList->back()->usSeqNum) + 1;

        if (pList->size() == packSize)
        {
            pstFrame->bAll = true;
        }
    }
} //lint !e1762

//ȡ֡list��������һ֡���ݳ��Է��� nSend �Ƿ���뷢��֡ 1��ǿ�Ʒ���  0��������֡����
void CPacketReassemble::SendLastListFrame(int nSend)
{
    if (m_FrameList.empty())
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send Small TimeTick Frame", "Frame map is empty.");
        return;
    }
    RTP_FRAME_INFO* pFrameTmp = m_FrameList.back();

    //���ǿ�Ʒ���
    if (nSend)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SendLastListFrame", "Forced to send Frame");
        SendFrame(pFrameTmp);
        m_FrameList.pop_back();
        IVS_DELETE(pFrameTmp);
        pFrameTmp = NULL;
    }
    else
    {
        //ֻ����������֡
        if (SendCompleteFrame(pFrameTmp))
        {
            m_FrameList.pop_back();
            IVS_DELETE(pFrameTmp);
            pFrameTmp = NULL;
        }
    }
}//lint !e438

//������Ƶ����
void CPacketReassemble::SendAudioFrame(RTP_PACK_INFO* pstRtpData)
{
    //�ص������������ݷ��ͳ�ȥ
    IVS_RAW_FRAME_INFO rawFrameInfo;

    rawFrameInfo.uiStreamType = (unsigned int) pstRtpData->pt;

    //���ֶ�ֻ����Ƶ������Ч
    rawFrameInfo.uiFrameType = NULL;
    rawFrameInfo.ullTimeTick = pstRtpData->ullTick;
    rawFrameInfo.uiTimeStamp = (unsigned int) pstRtpData->ulTimeStamp;
    rawFrameInfo.uiWaterMarkValue = pstRtpData->uWaterMark;
    //�հ�״̬
    REC_PACKET_INFO recPacketInfo;
    recPacketInfo.uAllPacketNum  = SING_PACKET_SIZE;
    recPacketInfo.uLostPacketNum = UNLOSE_PACKET_NUM;

    CaculateAudioCodeRate(pstRtpData->pt, (unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
    m_lock.Lock(); //lint !e534
    if (NULL != m_pReassmCallBack)
    {
        m_pReassmCallBack(&rawFrameInfo, pstRtpData->pPayload, (unsigned long)pstRtpData->lPayloadLen, &recPacketInfo,
                          m_pUser);
    }

    m_lock.UnLock(); //lint !e534
    FreePacket(pstRtpData); //ɾ���ð�
}

//���RTP��ͷ���ȣ�������չͷ
long CPacketReassemble::GetRtpHdrLen( char* pBuf, unsigned long ulBufLen, unsigned long& ulRtpHdrLen)
{
    if (NULL == pBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr", "Buffer Info is NULL.");
        return IVS_FAIL;
    }

    if (ulBufLen <= sizeof(FIXED_HEADER))
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr",
                "Get RTP header length error,the RTP buffer length = :%u is too short",
                ulBufLen);
        return IVS_FAIL;
    }

    FIXED_HEADER* pRtpHdr = (FIXED_HEADER*)pBuf; //lint !e826
    if (RTP_VERSION != pRtpHdr->version)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr", "Get RTP header length error,version:%d incorrect", pRtpHdr->version);
        return IVS_FAIL;
    }

    unsigned char cc = pRtpHdr->csrc_len; //csrc ����

    ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * cc; //ͷ���ȼ���CC����

    if (1 == pRtpHdr->extension)     //����չ
    {
        RTP_EXTEND_HEADER *pExtHdr = (RTP_EXTEND_HEADER *)(pBuf + ulRtpHdrLen);//lint !e826
        unsigned short usExtSize = ntohs(pExtHdr->usLength);

        // ֻ����HW��չͷ
        if (pExtHdr->usProfile == 0x5748)  // 0x5748 = "HW"
        {
            if (usExtSize == sizeof(RTP_EXTEND_HEADER) + sizeof(RTP_EXT_DATA))
            {
                // ֻ����ˮӡ����
            }
            else if (usExtSize == sizeof(RTP_EXTEND_HEADER) + sizeof(RTP_EXTENSION_DATA_EX))
            {
                // ����ˮӡ���ݺ�ʱ�������
            }
            else
            {
                IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr", "unsupported RTP Extention Header[%u]", usExtSize);
            }
        }
    }

    if (ulRtpHdrLen >= ulBufLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Hdr",
                "Get RTP header length error,RTP header lengt:%u longer than  buffer length:%u.", ulRtpHdrLen,
                ulBufLen);
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}//lint !e1762

//��������֡����
void CPacketReassemble::ClearFrameList()
{
	RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
	RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
	if (iterBegin == iterEnd)
	{
		return;
	}
	RTP_FRAME_INFO* pFrameTmp = NULL;
	//ѭ����������rtp��
	for (;iterBegin != iterEnd;)
	{
		pFrameTmp = *iterBegin;
		ReleaseRtpInfo(pFrameTmp);
		m_FrameList.erase(iterBegin++);
		IVS_DELETE(pFrameTmp);
		pFrameTmp = NULL;
	}
}


//ɾ��RTP����ռ��Դ
void CPacketReassemble::ReleaseRtpInfo(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Release Rtp Info", "Frame info is NULL");
        return;
    }

    RTP_PACK_LIST_ITER rtpListbeginIter = pstFrame->pPackList.begin();

    RTP_PACK_LIST_ITER rtpListEndIter = pstFrame->pPackList.end();

    for (; rtpListbeginIter != rtpListEndIter; ++rtpListbeginIter)
    {
        FreePacket(*rtpListbeginIter); //ɾ���ð�
    }

    pstFrame->pPackList.clear();
    return;
}

//������֡����ص�
int CPacketReassemble::SetCallbackFun(REASSM_CALLBACK cbFunc, void *pUser)
{
    CAutoLock ilock(m_lock); //lint !e522

    m_pReassmCallBack = cbFunc;
    m_pUser = pUser;
    return IVS_SUCCEED;
}//lint !e1788

//�����쳣�ص�
void CPacketReassemble::SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void *pUser)
{
    CAutoLock ilock(m_lock); //lint !e522

    m_pExceptionCallback = pExceptionCallback;
    m_pExcUser = pUser;
}//lint !e1788

//����֡�����С
void CPacketReassemble::SetFrameCacheLimit(unsigned int iFrameCache)
{
    if ((RTP_MIN_FRAME_CACHE > iFrameCache) || (RTP_MAX_FRAME_CACHE < iFrameCache))
    {
        IVS_LOG(IVS_LOG_ERR, "Set frame Size", "Frame Size is illegal, frame size :%d ", iFrameCache);
        return;
    }

    m_lock.Lock(); //lint !e534

    //�ɴ��С��Ҫ��ǰ��ļ�֡���ͳ�ȥ���ﵽ����֡�Ĵ�С
    if (m_ulFrameCacheLimit > iFrameCache)
    {
        unsigned int frameSize = m_FrameList.size();

        //���Ŀǰ������е����ݶ���Ҫ���õĴ�С������ǰ���֡
        while (frameSize > iFrameCache)
        {
            SendLastListFrame(FRAME_MUST_SEND);

            frameSize = m_FrameList.size();
        }
    }

    m_ulFrameCacheLimit = iFrameCache;
    m_lock.UnLock(); //lint !e534
}

//������Ƶ����
void CPacketReassemble::CaculateVideoCodeRate(unsigned long ulLength, unsigned long ulTimeStamp)
{
    //IVS_LOG(IVS_LOG_DEBUG, "CaculateVideoCodeRate", "ulLength:%lu, ulTimeStamp:%lu", ulLength, ulTimeStamp);

    unsigned long ulBeginTime = IVS_GetTimeTick();
    m_ulVideoByteCount += ulLength;
    m_ulRecVideoByteCount += ulLength;
    long lTimeStampInc = 0;

    if (0 == m_ulVideoBeginTimeStamp)
    {
        m_ulVideoBeginTimeStamp = ulTimeStamp;
    }
    else
    {
        lTimeStampInc = abs((long)ulTimeStamp - (long)m_ulVideoBeginTimeStamp);
        m_ulVideoTimeStampInc += lTimeStampInc; //lint !e737

        m_ulVideoBeginTimeStamp = ulTimeStamp;
        if (DEFAULT_H264STREAM_SAMPLE_RATE <= m_ulVideoTimeStampInc)
        {
            unsigned long ulEndTime = IVS_GetTimeTick();
            unsigned long ulInterval = (ulEndTime - ulBeginTime);
            if (0 == ulInterval)
            {
                ulInterval = 1;
            }
			
            m_uVideoCodeRate = (unsigned long)((m_ulVideoByteCount * 8000.0) / (ULONGLONG)(ulInterval*1000)); //lint !e647
			
            //����״̬
            m_ulVideoByteCount = 0;
            m_ulVideoTimeStampInc = 0;
        }
    }
}

//������Ƶ����
void CPacketReassemble::CaculateAudioCodeRate(unsigned int uAudioType, unsigned long ulLength, unsigned long ulTimeStamp)
{
    unsigned long ulBeginTime = IVS_GetTimeTick();
    m_ulAudioByteCount       += ulLength;
    m_ulRecAudioByteCount    += ulLength;
    
    if ((PAY_LOAD_TYPE_PCMU == uAudioType) || (PAY_LOAD_TYPE_PCMA == uAudioType))
    {
        m_uAudioCodeRate = DEFAULT_AUDIO_RATE;
        return;
    }

    long lTimeStampInc = 0;
    if (0 == m_ulAudioBeginTimeStamp)
    {
        m_ulAudioBeginTimeStamp = ulTimeStamp;
    }
    else
    {
        lTimeStampInc = abs((long)ulTimeStamp - (long)m_ulAudioBeginTimeStamp);
        m_ulAudioTimeStampInc += lTimeStampInc;  //lint !e737
		IVS_LOG(IVS_LOG_DEBUG, "CaculateAudioCodeRate", "m_ulAudioTimeStampInc:%lu, ulTimeStamp:%lu, m_ulVideoBeginTimeStamp:%lu, m_AudioAttr.samplerate:%u", 
			m_ulAudioTimeStampInc, ulTimeStamp, m_ulAudioBeginTimeStamp,m_AudioAttr.samplerate);

        m_ulAudioBeginTimeStamp = ulTimeStamp;
        if (m_AudioAttr.samplerate <= m_ulAudioTimeStampInc)
        {
            unsigned long ulEndTime = IVS_GetTimeTick();
            unsigned long ulInterval = (ulEndTime - ulBeginTime);
            if (0 == ulInterval)
            {
                ulInterval = 1;
            }

            //�������ʣ�����1000����Ϊʱ��Ϊ����
            m_uAudioCodeRate = (unsigned long )((m_ulAudioByteCount * 8000.0) / (ULONGLONG)(ulInterval*1000));  //lint !e647
            IVS_LOG(IVS_LOG_DEBUG, "CaculateVideoCodeRate", "m_uAudioCodeRate:%u, m_ulAudioByteCount:%llu", m_uAudioCodeRate, m_ulAudioByteCount);

            //����״̬
            m_ulAudioByteCount = 0;
            m_ulAudioTimeStampInc = 0;
        }
    }
}

unsigned int CPacketReassemble::GetAvgVideoCodeRate()
{
    //��һ�ε�����Ҫ������m_ulRecVideoByteCount����ֹ��һ�μ���ʱ���� by w00172146
    if (m_bIsFirstGetVideoRate)
    {
        m_ulRecVideoByteCount  = 0;
        m_bIsFirstGetVideoRate = false;
    }

	unsigned long long currTime = IVS_GetTimeTick();//IVS_GetSysTickUs() / 1000;
	if (0 == m_ulAvgBeginVideoFrameTime)
	{
		m_ulAvgBeginVideoFrameTime = currTime;
		return m_uAvgVideoCodeRate;
	}
	unsigned long long ulInterval = currTime - m_ulAvgBeginVideoFrameTime;
	if ( AVG_TIME_COUNT_TIME <= ulInterval)
	{
	    m_uAvgVideoCodeRate        = (unsigned int)((m_ulRecVideoByteCount * 8000) / (ULONGLONG)ulInterval);	        
        m_ulAvgBeginVideoFrameTime = 0;
		m_ulRecVideoByteCount      = 0;
        //����־���ó�true,�´ν������Ƚ�m_ulRecVideoByteCount����,��֤������ȷ��
        //���������,ƽ�����������ƫ��
        m_bIsFirstGetVideoRate = true;
	}
	return m_uAvgVideoCodeRate;
}

unsigned int CPacketReassemble::GetAvgAudioCodeRate()
{
    //��һ�ε�����Ҫ������m_ulRecAudioByteCount����ֹ��һ�μ���ʱ���� by w00172146
    if (m_bIsFirstGetAudioRate)
    {
        m_ulRecAudioByteCount = 0;
        m_bIsFirstGetAudioRate = false;
    }

	unsigned long long currTime = IVS_GetTimeTick();//IVS_GetSysTickUs() / 1000;
	if (0 == m_ulAvgBeginAudioFrameTime)
	{
		m_ulAvgBeginAudioFrameTime = currTime;
		return m_uAvgAudioCodeRate;
	}
	unsigned long long ulInterval = currTime - m_ulAvgBeginAudioFrameTime;
   
	if (AVG_TIME_COUNT_TIME <= ulInterval )
	{
        if (m_bPCMUorPCMA)
        {
            m_uAvgAudioCodeRate = DEFAULT_AUDIO_RATE;
            return m_uAvgAudioCodeRate;
        }

		m_uAvgAudioCodeRate =  (unsigned int)((m_ulRecAudioByteCount * 8000) / (ULONGLONG)ulInterval);
		m_ulAvgBeginAudioFrameTime = 0;
		m_ulRecAudioByteCount = 0;
        //����־���ó�true,�´ν������Ƚ�m_ulRecAudioByteCount����,��֤������ȷ��
        //���������,ƽ�����������ƫ��
        m_bIsFirstGetAudioRate = true;
	}
	return m_uAvgAudioCodeRate;
}

void CPacketReassemble::CaculateFPS(unsigned long ulTimeStamp)
{
	long lTimeStampInc = 0;
	if (0 == m_ulTimeStamp)
	{
		m_ulTimeStamp = ulTimeStamp;
		return;
	}
	else
	{
		lTimeStampInc = abs((long)ulTimeStamp - (long)m_ulTimeStamp);    //lint !e713
	}
	//// ������; RTPʱ�����Ծ
	if ((lTimeStampInc < 0) || (lTimeStampInc > MAX_TIME_STAMP_DIFF))
	{
		m_ulTimeStamp = 0;
		m_ulTSIncSum = 0;
		m_ulTSIncSunNum = 0;
	    return;
	}

	if (0 == lTimeStampInc)
	{
		return;
	}
	else
	{
		m_ulTSIncSum += lTimeStampInc;   //lint !e737
		m_ulTSIncSunNum++;
		m_ulTimeStamp = ulTimeStamp;
	}
	if (DEFAULT_TS_INC_NUM == m_ulTSIncSunNum)
	{
		//����ʱ���������ֵ,(a-1) / b + 1 ��ȡ�������㷨
		unsigned long ltsInc = (m_ulTSIncSum - 1) / m_ulTSIncSunNum + 1;
		//����֡�ʶ��Ƚ�С�����Ϊ60����СΪ1�����Ը�Ϊ��������
		m_uiFPS = (unsigned int)(DEFAULT_H264STREAM_SAMPLE_RATE / (double)ltsInc + 0.5);
        IVS_LOG(IVS_LOG_DEBUG, "CaculateFPS", "ltsInc:%lu, m_uiFPS:%u", ltsInc, m_uiFPS);
		if (m_uiFPS == 0)
		{
			m_uiFPS = 1;
		}
		//���ü������
		m_ulTimeStamp = 0;
		m_ulTSIncSum = 0;
		m_ulTSIncSunNum = 0;
	}
}

int CPacketReassemble::SetWaterMarkState(int nWaterMarkState)
{
    m_nWaterMarkState = nWaterMarkState;
    return IVS_SUCCEED;
}

int CPacketReassemble::SetDecryptDataKeyAndMode(const char* pSecretKey, unsigned long ulKeyLen, int iSecretKeyType)
{
    if (NO_NEED_ENCRYPTED == iSecretKeyType)
    {
        IVS_LOG(IVS_LOG_DEBUG, "Set Video DecryptData Key And Mode", "No Need Key, SecretKey is NULL.iSecretKeyType:%d", 
                iSecretKeyType);
        //���ԭ������Կû��ΪNULL,����ɾ��ԭ�е���Կ
        if (NULL != m_pSecretKey)
        {
            IVS_DELETE(m_pSecretKey, MUILI);
        }

        m_ulKeyLen = ulKeyLen;
        //����Ϊ����Ҫ����
        m_bDecryptData = false;
        return IVS_SUCCEED;
    }
    else
    {
        if ((NULL == pSecretKey) || (0 == ulKeyLen))
        {
            IVS_LOG(IVS_LOG_ERR, "Set Video DecryptData Key And Mode", "SecretKey is NULL");
            return IVS_FAIL;
        }

        if ((IPC_ENCRYPT_SIMPLE > iSecretKeyType) || (IPC_ENCRYPT_AES256_PARTIAL < iSecretKeyType))
        {
            IVS_LOG(IVS_LOG_ERR, "Set Video DecryptData Key And Mode", "SecretKeyType Error");
            return IVS_FAIL;
        }

        CAutoLock ilock(m_lock); //lint !e522

        //���ԭ������Կû��ΪNULL,����ɾ��ԭ�е���Կ
        if (NULL != m_pSecretKey)
        {
            IVS_DELETE(m_pSecretKey, MUILI);
        }

        m_ulKeyLen   = ulKeyLen;
        m_pSecretKey = IVS_NEW(m_pSecretKey, m_ulKeyLen + 1);
        if (NULL == m_pSecretKey)
        {
            IVS_LOG(IVS_LOG_ERR, "Set Video DecryptData Key And Mode", "Create Key Space Error");
            return IVS_FAIL;
        }
        memset(m_pSecretKey,0,m_ulKeyLen + 1);
        //���ý�����Կ
        memcpy(m_pSecretKey, pSecretKey, m_ulKeyLen);
        //����AESȫ����������Կ
        (void)DecryptDataGetAesKey(&m_stAesDecryptKey,(unsigned char *)pSecretKey,(int)m_ulKeyLen); //lint !e1773
        m_iDecryptDataType = iSecretKeyType;

        //����Ϊ��Ҫ����
        m_bDecryptData = true;
        return IVS_SUCCEED;
    }//lint !e1788

}

int CPacketReassemble::SetAudioDecryptDataKey(const char* pSecretKey, unsigned long ulKeyLen, int iSecretKeyType)
{
    if ((NULL == pSecretKey) || (0 == ulKeyLen))
    {
        IVS_LOG(IVS_LOG_ERR, "Set Audio DecryptData Key And Mode", "SecretKey is NULL");
        return IVS_FAIL;
    }

    if ((IPC_ENCRYPT_SIMPLE > iSecretKeyType) || (IPC_ENCRYPT_AES256_PARTIAL < iSecretKeyType))
    {
        IVS_LOG(IVS_LOG_ERR, "Set Audio DecryptData Key And Mode", "SecretKeyType Error");
        return IVS_FAIL;
    }

    CAutoLock ilock(m_lock); //lint !e522

    //���ԭ������Կû��ΪNULL,����ɾ��ԭ�е���Կ
    if (NULL != m_pAudioSecretKey)
    {
        IVS_DELETE(m_pAudioSecretKey, MUILI);
    }

    m_ulAudioKeyLen   = ulKeyLen;
    m_pAudioSecretKey = IVS_NEW(m_pAudioSecretKey, m_ulAudioKeyLen);
    if (NULL == m_pAudioSecretKey)
    {
        IVS_LOG(IVS_LOG_ERR, "Set Audio DecryptData Key And Mode", "Create Key Space Error");
        return IVS_FAIL;
    }

    memcpy(m_pAudioSecretKey, pSecretKey, m_ulAudioKeyLen);
    m_iAudioDecryptDataType = iSecretKeyType;

    //����Ϊ��Ҫ����
    m_bAudioDecryptData = true;
    return IVS_SUCCEED;
}//lint !e1788

void CPacketReassemble::CaculateLostPacketRate(unsigned short usCurrRevPacketNum, unsigned short usCurrLostPacketNum)
{
	//�ۼ�Ӧ���յ���ȫ������
	m_uRevPacketNum += usCurrRevPacketNum;
	//�ۼӶ�ʧ�İ�����
	m_uLostPacketNum += usCurrLostPacketNum;
}

void CPacketReassemble::CaculatePacketNum(unsigned short*  usShouldRevPacketNum, unsigned short*  usRevPacketNum)
{
	//��ȡʱ�����С��һ֡����
	RTP_FRAME_INFO* frameInfo = m_FrameList.back();
	if (NULL == frameInfo)
	{
		return;
	}
	*usRevPacketNum = (unsigned short) frameInfo->pPackList.size(); //ʵ���յ��ı���
	//Ӧ���յ��İ���
	*usShouldRevPacketNum =(unsigned short) (((short)frameInfo->pPackList.front()->usSeqNum - (short)frameInfo->pPackList.back()->usSeqNum) + 1); 
	//���û���յ�����������ʾӦ���յ��İ���ȱһ��
	if (!frameInfo->bE)
	{
		(*usShouldRevPacketNum)++;  
	}
	
	return;
}

float CPacketReassemble::GetLostPacketRate()
{
	unsigned long long currTime = IVS_GetSysTickUs() / 1000;
	if (0 == m_ulLostPacketBeginTime)
	{
		m_ulLostPacketBeginTime = currTime;
		return m_fLosttPacketRate;

	}
	unsigned long long ulInterval = currTime - m_ulLostPacketBeginTime;
    if (GET_LOST_PACKET_RATE_TIME <= ulInterval &&  0 < m_ulRecFromMUNums)
    {
        IVS_LOG(IVS_LOG_DEBUG, "GetLostPacketRate", "m_uLostPacketNum:%u, m_uRevPacketNum:%u, m_ulLostMUPacketNums:%u, m_ulRecFromMUNums:%u.", 
                m_uLostPacketNum, m_uRevPacketNum, m_ulLostMUPacketNums, m_ulRecFromMUNums);
        
        //�������㷨1-(1-A)*(1-B)��A:pu��mu�Ķ�����;B:mu��cu�Ķ�����
        m_fLosttPacketRate = (float)(m_ulLostMUPacketNums * 1.0) / m_ulRecFromMUNums;
        m_fLosttPacketRate = m_fLosttPacketRate > 1 ? 1 :m_fLosttPacketRate;
        m_uRevPacketNum = 0;
        m_uLostPacketNum = 0;
        m_ulLostPacketBeginTime = 0;
        m_ulLostMUPacketNums = 0;
        m_ulRecFromMUNums = 0;
        IVS_LOG(IVS_LOG_DEBUG, "GetLostPacketRate", "m_fLosttPacketRate:%f.", m_fLosttPacketRate);
        return m_fLosttPacketRate;
    }
	else
	{
		return m_fLosttPacketRate;
	}
}

void CPacketReassemble::InitPara()
{
    m_ulMaxBufLen = FRAME_BUFF_LENTH_HD;

    m_preSendFrameTimeStamp = 0;

    m_preSendFrameTimeStamp = H264_NALU_TYPE_UNDEFINED;

	//��Ƶ
	m_ulVideoByteCount = 0;
	m_ulRecVideoByteCount = 0;
	m_ulbeginVideoFrameTime = 0;
	m_ulLastVideoFrameTime = 0;
	m_ulAvgBeginVideoFrameTime = 0;
	m_uVideoCodeRate = 0;
	m_uAvgVideoCodeRate = 0;
	//��Ƶ
	m_uAudioCodeRate = 0;
	m_uAvgAudioCodeRate = 0;
	m_ulAudioByteCount = 0;
	m_ulRecAudioByteCount = 0;
	m_ulLastAudioFrameTime = 0;
	m_ulbeginAudioFrameTime = 0;
	m_ulAvgBeginAudioFrameTime = 0;

	//֡�ʳ�ʼ��ֵΪ1
    m_uiFPS = 1;
    m_ulVideoBeginTimeStamp = 0;

    m_ulAudioBeginTimeStamp = 0;
    m_ulTSIncSum = 0;
    m_ulTSIncSunNum = 0;
    m_uRevPacketNum = 0;
    m_uLostPacketNum = 0;
	m_ulLostPacketBeginTime = 0;
	m_fLosttPacketRate = 0.0;

    //���RTP�����
    m_usMaxSeqNum = 0x0000;
    //��СRTP�����
    m_usMinSeqNum = 0xFFFF;

    m_ulShouldBeRecvPacketNum = 0;

    m_ulOverrRecvPacketNum = 0;

	m_uiEncryptExLen = 0;
	memset(&m_stAesDecryptKey,0x0,sizeof(aes_context));

    m_ulVideoTimeStampInc = 0;
    m_ulAudioTimeStampInc = 0;

    m_bPCMUorPCMA = false;
}


void CPacketReassemble::DecryptRtpPacketData(char* pEncryptData, unsigned long ulEncryptDataLen,
                                             unsigned char palyLoadType)
{
	int iRet = IVS_FAIL;

	if ((NULL == pEncryptData) || (1 >= ulEncryptDataLen))
	{
		return;
	}
	
	char* pEncryptDataTmp = pEncryptData;


	//��Ƶ����ȫ������//���Ϊ����֡ͷ
	if ((PAY_LOAD_TYPE_PCMU != palyLoadType && PAY_LOAD_TYPE_PCMA != palyLoadType)
		&& IPC_ENCRYPT_SIMPLE != m_iDecryptDataType)
	{
		if (PAY_LOAD_TYPE_H264 == palyLoadType)
		{
			pEncryptDataTmp  += 1;  //Ĭ������nalu����
			ulEncryptDataLen -= 1;
			FU_INDICATOR* pFu = (FU_INDICATOR*)(pEncryptData);
			//�ǵ���ss������֡ͷ��Ҫ���������ֽ� FU_identifier �� FU_header
			if (H264_NALU_TYPE_FU_A == pFu->TYPE)  
			{
				pEncryptDataTmp  += 1;  //Ĭ������nalu����
				ulEncryptDataLen -= 1;
			}
		}
		else if (PAY_LOAD_TYPE_H265 == palyLoadType)
		{
		    unsigned char nal_type = ((unsigned char)pEncryptData[0] >> 1) & 0x3f;
			pEncryptDataTmp  += 2;  //Ĭ������nalu����
			ulEncryptDataLen -= 2;
			if (nal_type == 49) 
			{			
				pEncryptDataTmp  += 1;  //Ĭ������nalu����
			    ulEncryptDataLen -= 1;
			}
		}
		else
		{

		}
	}

	switch (m_iDecryptDataType)
	{
	case IPC_ENCRYPT_SIMPLE: 
	case IPC_ENCRYPT_SIMPLE_HEADER:
		iRet = DecryptDataXOR(m_pSecretKey, m_ulKeyLen, pEncryptDataTmp, ulEncryptDataLen);
		break;
	case IPC_ENCRYPT_AES256:
		iRet = DecryptDataAesALLValue((unsigned char*)pEncryptDataTmp,ulEncryptDataLen,m_pDecryptDataBuf,&m_stAesDecryptKey,m_uiEncryptExLen) ;
		break;
	case IPC_ENCRYPT_AES256_PARTIAL:
		iRet = DecryptDataAES( m_pSecretKey, 32, 16, pEncryptDataTmp, ulEncryptDataLen, m_pDecryptDataBuf, DEC_RATIO,0);
		break;
	default:
		break;
	}
	if (IVS_SUCCEED != iRet)
	{
		IVS_LOG(IVS_LOG_ERR, "Decrypt Rtp Packet Data", "Decrypt Rtp Data Error");
	}
}

void CPacketReassemble::StopDecryptVideoData()
{
    CAutoLock ilock(m_lock); //lint !e522

    //�ڴ˴�û�л���֡����������Ļ���ռ䣬��������ʱ���ٻ���
    IVS_DELETE(m_pSecretKey, MUILI);
    m_ulKeyLen = 0;
    m_iDecryptDataType = NO_DEC_TYPE;
    m_bDecryptData = false;
}//lint !e1788

void CPacketReassemble::StopDecryptAudioData()
{
    CAutoLock ilock(m_lock); //lint !e522

    IVS_DELETE(m_pAudioSecretKey, MUILI);
    m_ulAudioKeyLen = 0;
    m_bAudioDecryptData = false;
}//lint !e1788

/*
 * SetMediaAttr
 * ����ý������
 */
void CPacketReassemble::SetMediaAttr(const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr)
{
	if (NULL != pVideoAttr)
	{
		memcpy(&m_VideoAttr, pVideoAttr, sizeof(MEDIA_ATTR));
	}

	if (NULL != pAudioAttr)
	{
		memcpy(&m_AudioAttr, pAudioAttr, sizeof(MEDIA_ATTR));
	}
}

/*
 * InsertPacket, check video type and fix up payload type
 * add by w00210470
 */
int CPacketReassemble::InsertPacket( char* pBuf, unsigned int uBufLen)
{
	FIXED_HEADER *phdr = (FIXED_HEADER *)pBuf;//lint !e826
    
	if (PAY_LOAD_TYPE_PCMA == phdr->payload || 
		PAY_LOAD_TYPE_PCMU == phdr->payload)
	{
		return IVS_SUCCEED;
	}

	/* 
	 * force to the one we known
	 */
	if (VIDEO_DEC_H264 == m_VideoAttr.type)
	{
		phdr->payload = PAY_LOAD_TYPE_H264;
	}
	else if (VIDEO_DEC_H265 == m_VideoAttr.type)
	{
		phdr->payload = PAY_LOAD_TYPE_H265;
	}
	else if (VIDEO_DEC_SVAC == m_VideoAttr.type)
	{
	    phdr->payload = PAY_LOAD_TYPE_SVAC;
	}
	else
	{
		phdr->payload = PAY_LOAD_TYPE_MJPEG;
	}
 
	return IVS_SUCCEED;
}//lint !e715 !e1961

void CPacketReassemble::CaculateMU2CULostPacketNum(unsigned short usPacketSeqNum)
{
    if (0 == m_usPacketSeqNum)
    {
        m_usPacketSeqNum = usPacketSeqNum;
        m_ulLostMUPacketNums = 0;
        m_ulRecFromMUNums++;
        IVS_LOG(IVS_LOG_DEBUG, "1", "usPacketSeqNum:%u, m_ulLostMUPacketNums:%u, m_ulRecFromMUNums:%u.", usPacketSeqNum, m_ulLostMUPacketNums, m_ulRecFromMUNums);
    }
    else
    {
        if (SEQ_AFTER(usPacketSeqNum, m_usPacketSeqNum))
        {
            m_ulLostMUPacketNums += (((unsigned short)(usPacketSeqNum - m_usPacketSeqNum)) - 1);
            m_ulRecFromMUNums += (unsigned short)(usPacketSeqNum - m_usPacketSeqNum);
        }
        else
        {
            m_ulLostMUPacketNums += (((unsigned short)(m_usPacketSeqNum - usPacketSeqNum)) - 1);
            m_ulRecFromMUNums += (unsigned short)(m_usPacketSeqNum - usPacketSeqNum);
        }
        
        m_usPacketSeqNum = usPacketSeqNum;
    }
}
