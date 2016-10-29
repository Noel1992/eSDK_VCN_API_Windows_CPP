/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  PacketReassemble.cpp
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/
#include "PacketReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif

#define  AVG_TIME_COUNT_TIME 20000      //定义平均码率帧率码率的统计时间间隔（ms） 20S
#define  GET_LOST_PACKET_RATE_TIME 5000 //定义统计丢包率的时间间隔(ms) 5S

#define  MAX_TIME_STAMP_DIFF 99000  // RTP时间戳跳跃

#define  DEC_RATIO 0.2      //AES部分解码率

#define  NO_NEED_ENCRYPTED (-1)  //不加密标志

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

    //初始化宽高
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

//释放当前RTP包占用内存块
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

	unsigned short usExtSize =(unsigned short) ntohs(pExtHdr->usLength) * 4; //lint !e734 // 4字节倍数
	if (ulRtpHdrLen + usExtSize >= ulBufLen)
	{
		IVS_LOG(IVS_LOG_ERR, "Get Extension Head", "Extension Defect");
		return ;
	}

	// 只处理HW扩展头
	if (pExtHdr->usProfile == 0x5748)  // 0x5748 = "HW"
	{
		if (usExtSize == sizeof(RTP_EXTENSION_DATA_EX))
		{
			// 扩展包含水印数据和时间戳数据
			RTP_EXTENSION_DATA_EX *pExtension_ = (RTP_EXTENSION_DATA_EX *)(pBuf + ulRtpHdrLen);//lint !e826
			if (WATER_MARK == (unsigned short) ntohs(pExtension_->usWaterMarkType))
			{
				//水印值
				uiWaterMarkValue =  pExtension_->uiWaterMarkValue;

				//是否丢包，0没有丢包，1丢包，无水印时无效
				usLostFrame = pExtension_->usLostFrame;
			}
			//补齐16字节对齐的额外数据量
			if (m_bDecryptData)
			{
				m_uiEncryptExLen = (unsigned int)pExtension_->uiEncryptExLen;
			}

			ullTick = (unsigned long long)ntohl(pExtension_->uiGmtSecond) * 1000 + ntohs(pExtension_->usGmtMillsecond);//lint !e647
		}
		//直连时的扩展结构体
		else if (usExtSize == sizeof(RTP_EXTENSION_DATA_S_EX))
		{
			// 扩展包含水印数据和时间戳数据
			RTP_EXTENSION_DATA_S_EX *pExtensionDcg = (RTP_EXTENSION_DATA_S_EX *)(pBuf + ulRtpHdrLen);//lint !e826
			if (WATER_MARK == (unsigned short) ntohs(pExtensionDcg->usWaterMarkType))
			{
				//水印值
				uiWaterMarkValue =  pExtensionDcg->uiWaterMarkValue;
			}
			//补齐16字节对齐的额外数据量
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

//对输入的流数据转换为RTP结构数据
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
	//初始化该包扩展长度
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

    ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * pRtpHdr->csrc_len; //头长度加上CC长度
    if (ulRtpHdrLen >= ulBufLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Big Head Size=[%u]", ulRtpHdrLen);
        return IVS_FAIL;
    }
    // 解析扩展头部
    if (1 == pRtpHdr->extension)
    {
        GetExtensionHead(pBuf,ulBufLen,ulRtpHdrLen,ullTick,usLostFrame,uWaterMark);
    }
	
    //在内存池中取出一块可用的内存区域
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
		//发现内存池已经满了，回收所有资源
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
        //总长度去掉头长度（包括扩展）和填充长度 .此处ulBufLen必然大于ulRtpHdrLen
        pRtpPackInfo->lPayloadLen = (long) (ulBufLen - ulRtpHdrLen) - (long)(unsigned char)pBuf[ulBufLen - 1]; //lint !e737
    }
    else
    {
        //总长度去掉头长度（包括扩展）
        pRtpPackInfo->lPayloadLen = (long)(ulBufLen - ulRtpHdrLen);
    }

    if ((0 >= pRtpPackInfo->lPayloadLen)
        || (long)(/*BLOCK_SIZE*/BlockSize - sizeof(RTP_PACK_INFO)) < pRtpPackInfo->lPayloadLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Get Rtp Packet Info", "Payload too long %ld", pRtpPackInfo->lPayloadLen);
        return IVS_FAIL;
    }

    //如果启用了解密
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

    pRtpPackInfo->fuType = pFu->TYPE;     //设置type，FU_INDICATOR和NALU是相同的

    return IVS_SUCCEED;
}

//尝试将新进的帧数据加入到单帧回放缓存当中
int CPacketReassemble::InsertFrameToPlayBlackCache(RTP_FRAME_INFO* pstFrame,unsigned int uiGopNumLimit)
{
    //获取现有frameList缓存的frame个数
    unsigned int uiFrameSize = m_FrameList.size();
    int iRet = IVS_FAIL;

    //如果该布尔型为false,则表示当前帧有问题，不需要执行下步
    bool bDoNextFlag = true;

    //如果大于一个限定的最大GOP个数，则尝试发送收到的数据
    if (uiGopNumLimit <= uiFrameSize)
    {
        bDoNextFlag = FrameListIsFullAndSend(pstFrame);
    }
    //插入到FrameList中
    if (bDoNextFlag)
    {
        InsertFrameToPlayBlackList(pstFrame);
        iRet = IVS_SUCCEED;
    }
    return iRet;
}


//将数据加入到单帧回放数据list
void CPacketReassemble::InsertFrameToPlayBlackList(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame to play black List", " Frame Info is NULL");
        return;
    }

    //如果缓存区的空间未满
    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
    RTP_FRAME_LIST_ITER iterTmp = iterEnd;
    unsigned long newTimeStamp = pstFrame->ulTimeStamp;
    if (iterBegin != iterEnd)
    {
        //判断当前时间戳是否小于取出的最大时间戳
        unsigned long ulTimeStamp = (*iterBegin)->ulTimeStamp;
        for (; iterBegin != iterEnd; iterBegin++)
        {
            ulTimeStamp = (*iterBegin)->ulTimeStamp;

            //如果当前时间戳小于迭代到的元素，插入该位置
            if (TIME_BEFORE(newTimeStamp, ulTimeStamp))
            {
                iterTmp = iterBegin;
                break;
            }
        }
    }

    //插入到指定的位置
    (void)m_FrameList.insert(iterTmp, pstFrame);
    return;
}

/****************************************************************************************
*
****************************************************************************************/

//尝试将新进的帧数据插入到缓存当中
int CPacketReassemble::InsertFrameToCache(RTP_FRAME_INFO* pstFrame)
{
    //获取现有frameList缓存的frame个数
    unsigned int uiFrameSize = m_FrameList.size();
    int iRet = IVS_FAIL;

    //如果该布尔型为false,则表示当前帧有问题，不需要执行下步
    bool bDoNextFlag = true;

    unsigned int frameSizeLimit = m_ulFrameCacheLimit;

    //缓存区已经满
    if (frameSizeLimit <= uiFrameSize)
    {
        bDoNextFlag = FrameListIsFullAndSend(pstFrame);
    }

    //插入到FrameList中
    if (bDoNextFlag)
    {
        InsertFrameToList(pstFrame);
        iRet = IVS_SUCCEED;
    }

    return iRet;
}

//缓存区满，删除其中一帧,返回的值表示是否需要做后面的动作
bool CPacketReassemble::FrameListIsFullAndSend(const RTP_FRAME_INFO* pstFrame)
{
    //强制发缓存队列的最后一帧
    SendLastListFrame(FRAME_MUST_SEND);
    return true;
}//lint !e715

//数据加入帧缓存区
void CPacketReassemble::InsertFrameToList(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Frame to List", " Frame Info is NULL");
        return;
    }

    //如果缓存区的空间未满
    RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
    RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();

    RTP_FRAME_LIST_ITER iterTmp = iterEnd;

    unsigned long newTimeStamp = pstFrame->ulTimeStamp;

    if (iterBegin != iterEnd)
    {
        //判断当前时间戳是否大于取出的最大时间戳
        unsigned long ulTimeStamp = (*iterBegin)->ulTimeStamp;

        for (; iterBegin != iterEnd; iterBegin++)
        {
            ulTimeStamp = (*iterBegin)->ulTimeStamp;

            //如果当前时间戳大于迭代到的元素，插入该位置
            if (TIME_AFTER(newTimeStamp, ulTimeStamp) || (newTimeStamp == ulTimeStamp))
            {
                iterTmp = iterBegin;
                break;
            }
        }
    }

    //插入到指定的位置
    (void)m_FrameList.insert(iterTmp, pstFrame);

    return;
}

//获取具有相同时间戳的帧数据
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

			//返回具有相同时间戳且类型一致的帧数据
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

		//Mjpeg不需要判断是否有相同类型
		for (; iterBegin != iterEnd; iterBegin++)
		{
			if (NULL == (*iterBegin))
        	{
            	IVS_LOG(IVS_LOG_ERR, "Get frame TimeTick", " MJPEG Rtp Pack info is NULL");
            	continue;
        	}
			//返回具有相同时间戳且类型一致的帧数据
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

//判断包是否收齐，包排序默认按序号从大到小
void CPacketReassemble::CheckFrameSeq(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		return;
	}

    pstFrame->bAll = false;

    //收到了包头包尾
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

//取帧list里面的最后一帧数据尝试发送 nSend 是否必须发送帧 1：强制发送  0：按完整帧发送
void CPacketReassemble::SendLastListFrame(int nSend)
{
    if (m_FrameList.empty())
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send Small TimeTick Frame", "Frame map is empty.");
        return;
    }
    RTP_FRAME_INFO* pFrameTmp = m_FrameList.back();

    //如果强制发送
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
        //只发送完整的帧
        if (SendCompleteFrame(pFrameTmp))
        {
            m_FrameList.pop_back();
            IVS_DELETE(pFrameTmp);
            pFrameTmp = NULL;
        }
    }
}//lint !e438

//发送音频数据
void CPacketReassemble::SendAudioFrame(RTP_PACK_INFO* pstRtpData)
{
    //回调，将单包数据发送出去
    IVS_RAW_FRAME_INFO rawFrameInfo;

    rawFrameInfo.uiStreamType = (unsigned int) pstRtpData->pt;

    //该字段只有视频数据有效
    rawFrameInfo.uiFrameType = NULL;
    rawFrameInfo.ullTimeTick = pstRtpData->ullTick;
    rawFrameInfo.uiTimeStamp = (unsigned int) pstRtpData->ulTimeStamp;
    rawFrameInfo.uiWaterMarkValue = pstRtpData->uWaterMark;
    //收包状态
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
    FreePacket(pstRtpData); //删除该包
}

//获得RTP包头长度，包括扩展头
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

    unsigned char cc = pRtpHdr->csrc_len; //csrc 数量

    ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * cc; //头长度加上CC长度

    if (1 == pRtpHdr->extension)     //有扩展
    {
        RTP_EXTEND_HEADER *pExtHdr = (RTP_EXTEND_HEADER *)(pBuf + ulRtpHdrLen);//lint !e826
        unsigned short usExtSize = ntohs(pExtHdr->usLength);

        // 只处理HW扩展头
        if (pExtHdr->usProfile == 0x5748)  // 0x5748 = "HW"
        {
            if (usExtSize == sizeof(RTP_EXTEND_HEADER) + sizeof(RTP_EXT_DATA))
            {
                // 只包含水印数据
            }
            else if (usExtSize == sizeof(RTP_EXTEND_HEADER) + sizeof(RTP_EXTENSION_DATA_EX))
            {
                // 包含水印数据和时间戳数据
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

//回收所有帧数据
void CPacketReassemble::ClearFrameList()
{
	RTP_FRAME_LIST_ITER iterBegin = m_FrameList.begin();
	RTP_FRAME_LIST_ITER iterEnd = m_FrameList.end();
	if (iterBegin == iterEnd)
	{
		return;
	}
	RTP_FRAME_INFO* pFrameTmp = NULL;
	//循环回收所有rtp包
	for (;iterBegin != iterEnd;)
	{
		pFrameTmp = *iterBegin;
		ReleaseRtpInfo(pFrameTmp);
		m_FrameList.erase(iterBegin++);
		IVS_DELETE(pFrameTmp);
		pFrameTmp = NULL;
	}
}


//删除RTP包所占资源
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
        FreePacket(*rtpListbeginIter); //删除该包
    }

    pstFrame->pPackList.clear();
    return;
}

//设置组帧结果回调
int CPacketReassemble::SetCallbackFun(REASSM_CALLBACK cbFunc, void *pUser)
{
    CAutoLock ilock(m_lock); //lint !e522

    m_pReassmCallBack = cbFunc;
    m_pUser = pUser;
    return IVS_SUCCEED;
}//lint !e1788

//设置异常回调
void CPacketReassemble::SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void *pUser)
{
    CAutoLock ilock(m_lock); //lint !e522

    m_pExceptionCallback = pExceptionCallback;
    m_pExcUser = pUser;
}//lint !e1788

//设置帧缓存大小
void CPacketReassemble::SetFrameCacheLimit(unsigned int iFrameCache)
{
    if ((RTP_MIN_FRAME_CACHE > iFrameCache) || (RTP_MAX_FRAME_CACHE < iFrameCache))
    {
        IVS_LOG(IVS_LOG_ERR, "Set frame Size", "Frame Size is illegal, frame size :%d ", iFrameCache);
        return;
    }

    m_lock.Lock(); //lint !e534

    //由大变小，要将前面的几帧发送出去，达到新设帧的大小
    if (m_ulFrameCacheLimit > iFrameCache)
    {
        unsigned int frameSize = m_FrameList.size();

        //如果目前缓存队列的数据多于要设置的大小，则发送前面的帧
        while (frameSize > iFrameCache)
        {
            SendLastListFrame(FRAME_MUST_SEND);

            frameSize = m_FrameList.size();
        }
    }

    m_ulFrameCacheLimit = iFrameCache;
    m_lock.UnLock(); //lint !e534
}

//计算视频码率
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
			
            //重置状态
            m_ulVideoByteCount = 0;
            m_ulVideoTimeStampInc = 0;
        }
    }
}

//计算音频码率
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

            //计算码率，乘以1000是因为时间为毫秒
            m_uAudioCodeRate = (unsigned long )((m_ulAudioByteCount * 8000.0) / (ULONGLONG)(ulInterval*1000));  //lint !e647
            IVS_LOG(IVS_LOG_DEBUG, "CaculateVideoCodeRate", "m_uAudioCodeRate:%u, m_ulAudioByteCount:%llu", m_uAudioCodeRate, m_ulAudioByteCount);

            //重置状态
            m_ulAudioByteCount = 0;
            m_ulAudioTimeStampInc = 0;
        }
    }
}

unsigned int CPacketReassemble::GetAvgVideoCodeRate()
{
    //第一次调用需要清理下m_ulRecVideoByteCount，防止第一次计算时有误 by w00172146
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
        //将标志设置成true,下次进来会先将m_ulRecVideoByteCount清零,保证计算正确性
        //如果不设置,平均码流计算会偏大
        m_bIsFirstGetVideoRate = true;
	}
	return m_uAvgVideoCodeRate;
}

unsigned int CPacketReassemble::GetAvgAudioCodeRate()
{
    //第一次调用需要清理下m_ulRecAudioByteCount，防止第一次计算时有误 by w00172146
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
        //将标志设置成true,下次进来会先将m_ulRecAudioByteCount清零,保证计算正确性
        //如果不设置,平均码流计算会偏大
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
	//// 检查误差; RTP时间戳跳跃
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
		//计算时间戳增量均值,(a-1) / b + 1 是取上整的算法
		unsigned long ltsInc = (m_ulTSIncSum - 1) / m_ulTSIncSunNum + 1;
		//由于帧率都比较小，最大为60，最小为1，所以改为四舍五入
		m_uiFPS = (unsigned int)(DEFAULT_H264STREAM_SAMPLE_RATE / (double)ltsInc + 0.5);
        IVS_LOG(IVS_LOG_DEBUG, "CaculateFPS", "ltsInc:%lu, m_uiFPS:%u", ltsInc, m_uiFPS);
		if (m_uiFPS == 0)
		{
			m_uiFPS = 1;
		}
		//重置计算参数
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
        //如果原来的密钥没有为NULL,则先删除原有的密钥
        if (NULL != m_pSecretKey)
        {
            IVS_DELETE(m_pSecretKey, MUILI);
        }

        m_ulKeyLen = ulKeyLen;
        //设置为不需要解密
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

        //如果原来的密钥没有为NULL,则先删除原有的密钥
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
        //设置解密密钥
        memcpy(m_pSecretKey, pSecretKey, m_ulKeyLen);
        //设置AES全码流解密密钥
        (void)DecryptDataGetAesKey(&m_stAesDecryptKey,(unsigned char *)pSecretKey,(int)m_ulKeyLen); //lint !e1773
        m_iDecryptDataType = iSecretKeyType;

        //设置为需要解密
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

    //如果原来的密钥没有为NULL,则先删除原有的密钥
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

    //设置为需要解密
    m_bAudioDecryptData = true;
    return IVS_SUCCEED;
}//lint !e1788

void CPacketReassemble::CaculateLostPacketRate(unsigned short usCurrRevPacketNum, unsigned short usCurrLostPacketNum)
{
	//累加应该收到的全部包数
	m_uRevPacketNum += usCurrRevPacketNum;
	//累加丢失的包个数
	m_uLostPacketNum += usCurrLostPacketNum;
}

void CPacketReassemble::CaculatePacketNum(unsigned short*  usShouldRevPacketNum, unsigned short*  usRevPacketNum)
{
	//获取时间戳最小的一帧数据
	RTP_FRAME_INFO* frameInfo = m_FrameList.back();
	if (NULL == frameInfo)
	{
		return;
	}
	*usRevPacketNum = (unsigned short) frameInfo->pPackList.size(); //实际收到的报数
	//应该收到的包数
	*usShouldRevPacketNum =(unsigned short) (((short)frameInfo->pPackList.front()->usSeqNum - (short)frameInfo->pPackList.back()->usSeqNum) + 1); 
	//如果没有收到结束包，表示应该收到的包还缺一个
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
        
        //丢包率算法1-(1-A)*(1-B)。A:pu到mu的丢包率;B:mu到cu的丢包率
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

	//视频
	m_ulVideoByteCount = 0;
	m_ulRecVideoByteCount = 0;
	m_ulbeginVideoFrameTime = 0;
	m_ulLastVideoFrameTime = 0;
	m_ulAvgBeginVideoFrameTime = 0;
	m_uVideoCodeRate = 0;
	m_uAvgVideoCodeRate = 0;
	//音频
	m_uAudioCodeRate = 0;
	m_uAvgAudioCodeRate = 0;
	m_ulAudioByteCount = 0;
	m_ulRecAudioByteCount = 0;
	m_ulLastAudioFrameTime = 0;
	m_ulbeginAudioFrameTime = 0;
	m_ulAvgBeginAudioFrameTime = 0;

	//帧率初始化值为1
    m_uiFPS = 1;
    m_ulVideoBeginTimeStamp = 0;

    m_ulAudioBeginTimeStamp = 0;
    m_ulTSIncSum = 0;
    m_ulTSIncSunNum = 0;
    m_uRevPacketNum = 0;
    m_uLostPacketNum = 0;
	m_ulLostPacketBeginTime = 0;
	m_fLosttPacketRate = 0.0;

    //最大RTP包序号
    m_usMaxSeqNum = 0x0000;
    //最小RTP包序号
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


	//音频数据全部解码//如果为明文帧头
	if ((PAY_LOAD_TYPE_PCMU != palyLoadType && PAY_LOAD_TYPE_PCMA != palyLoadType)
		&& IPC_ENCRYPT_SIMPLE != m_iDecryptDataType)
	{
		if (PAY_LOAD_TYPE_H264 == palyLoadType)
		{
			pEncryptDataTmp  += 1;  //默认跳过nalu长度
			ulEncryptDataLen -= 1;
			FU_INDICATOR* pFu = (FU_INDICATOR*)(pEncryptData);
			//非单包ss，明文帧头需要跳过两个字节 FU_identifier 和 FU_header
			if (H264_NALU_TYPE_FU_A == pFu->TYPE)  
			{
				pEncryptDataTmp  += 1;  //默认跳过nalu长度
				ulEncryptDataLen -= 1;
			}
		}
		else if (PAY_LOAD_TYPE_H265 == palyLoadType)
		{
		    unsigned char nal_type = ((unsigned char)pEncryptData[0] >> 1) & 0x3f;
			pEncryptDataTmp  += 2;  //默认跳过nalu长度
			ulEncryptDataLen -= 2;
			if (nal_type == 49) 
			{			
				pEncryptDataTmp  += 1;  //默认跳过nalu长度
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

    //在此处没有回收帧解密是所需的缓存空间，在析构的时候再回收
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
 * 设置媒体属性
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
