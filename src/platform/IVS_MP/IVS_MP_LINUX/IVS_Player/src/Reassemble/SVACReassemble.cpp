/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  H264Reassemble.cpp
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/

#include "SVACReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define MAX_TIME_GAP  6000 //最大时间戳间隔

#define SVAC_RTP_FU   28
#define SVAC_RTP_NAL  0 

//lint -e954  本文件954错误无问题，统一屏蔽
CSvacReassemble::~CSvacReassemble()
{
	m_bStopInserFlag = true;
    m_bIsStepReset = false;
	m_uiWidth = 1920;
	m_uiHeight = 1080;
}

int CSvacReassemble::Init()
{
    //同样一次性分配足够大的解码缓存空间
    m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
    if (NULL == m_pFrameBuf)
    {
		IVS_LOG(IVS_LOG_ERR,"SVAC Init","Create frame buf error");
        return IVS_FAIL;
    }
    memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //设置0

	m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
	if (NULL == m_pDecryptDataBuf)
	{
		IVS_LOG(IVS_LOG_ERR,"SVAC Init","Create Dec frame buf error");
		IVS_DELETE(m_pFrameBuf, MUILI);
		return IVS_FAIL;
	}
	memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //设置0

    //初始化内存池
    if (IVS_FAIL == m_packetPool.Init())
    {
		IVS_LOG(IVS_LOG_ERR, "SVAC Init", "Create packetPool error");
        IVS_DELETE(m_pFrameBuf, MUILI);
        IVS_DELETE(m_pDecryptDataBuf, MUILI);
        m_packetPool.FreeMemPool();

        return IVS_FAIL;
    }
    m_pFrameBuf[3] = 1;     //startcode 的字节
    m_bStopInserFlag = false;
    m_bIsStepReset = false;
    return IVS_SUCCEED;
}

void CSvacReassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}


//清空本地缓存区的内容，回收全部内存块以及缓存区
int CSvacReassemble::ReSetReassemble()
{
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear SVAC Frame begin.");

    m_bStopInserFlag = true;

    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear SVAC Frame end");

    return IVS_SUCCEED;
}

//插入RTP包，并完成组帧动作
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

    //edit by w00172146 解决拖动花屏问题(拖动后接收到I帧才插入列表进行解码)
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
            //获取RTP头长度
            unsigned long ulRtpHdrLen = sizeof(FIXED_HEADER) + 4 * pRtpHdr->csrc_len; //头长度加上CC长度

            unsigned short usExtDateLen = 0;    //扩展数据长度，不包括2字节profile长度和2字节长度
            unsigned short usProfileLen = 0;    //自定义的profile长度2字节
            unsigned short usExtLen = 0;        //扩展长度2字节
            //如果有扩展字段
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
                //拖动录像回放，如果是P帧直接扔掉，直到收到第一个I帧，才真正插入
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
    
	//音频数据直接解码(rfc3551载荷类型定义)
    if ((PAY_LOAD_TYPE_PCMU == pstRtpData->pt) || 
		(PAY_LOAD_TYPE_PCMA == pstRtpData->pt))
    {
        m_bPCMUorPCMA = true;
        SendAudioFrame(pstRtpData);
        return IVS_SUCCEED;
    }

	//计算从MU到CU的丢包率,只计算视频包 
    CaculateMU2CULostPacketNum(pstRtpData->usSeqNum);
    CaculateFPS(pstRtpData->ulTimeStamp);
    CaculateVideoCodeRate((unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
  
	unsigned char type = (unsigned char)pstRtpData->pPayload[0];
    if (SVAC_RTP_NAL == type)  //单个NALU切片
    {
        iRet = InsertSingleNalu(pstRtpData);
    }
    else if (SVAC_RTP_FU == type) //暂时只支持FUA
    {
		//检查时间戳是否发了跃变，发生则将队列的数据局强行发送清空缓存队列
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

//检测并处理特殊事件，回放、时间戳跳变
void CSvacReassemble::CheckSpecialEvent(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData || m_FrameList.empty())
    {
        return;
    }

    //如果为回放模式，尝试将IDR帧之前的所有帧发送出去，非回放模式的话，时间戳是按顺序发送，无需调整
    if (m_iPlayBackMode)
    {
        CheckIDRAndSend(pstRtpData);
    }

    //检查时间戳是否发了跃变，发生则将队列的数据局强行发送清空缓存队列
    CheckTimeStampFail(pstRtpData->ulTimeStamp);
}

//回放的时候，采用收到新的GOP时候，强制发送所有剩余的缓存区数据
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
    if (3 > pstRtpData->lPayloadLen) //FUA 长度要至少为3
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
    //如果为回放模式，当收到一个IDR时，表示当前为新的GOP，强制发送缓存区的所有帧数据
    if (SVAC_NAL_SPS == nal_hdr->type)
    {
        //强制发送所有frameList里面的帧数据
        unsigned int listSize = m_FrameList.size();
        for (unsigned int i = 0; i < listSize; i++)
        {
            SendLastListFrame(FRAME_MUST_SEND);
        }
        m_FrameList.clear();
    }

} //lint !e818

//发送时间戳发送跳变之前的数据
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
    //获取队列中时间戳最大的数据
    RTP_FRAME_INFO* frameInfo = m_FrameList.front();
    if (NULL == frameInfo)
    {
        return;
    }
    unsigned long maxTimeStamp = frameInfo->ulTimeStamp;
    long timeGap = (long)(maxTimeStamp - ulTimeStamp);
    //只对时间戳跳变为小的做
    if (MAX_TIME_GAP > timeGap)
    {
        return;
    }
    //以下发送了时间戳的跳变
	IVS_LOG(IVS_LOG_ERR, "Check TimeStamp", " Time hopping %ld ", timeGap);
    //强制发送所有frameList里面的帧数据
    unsigned int listSize = m_FrameList.size();
    for (unsigned int i = 0; i < listSize; i++)
    {
        SendLastListFrame(FRAME_MUST_SEND);
    }
    m_FrameList.clear();
}

//单个Nalu的对象直接放回解码
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
	frameInfo->pPackList.push_front(pstRtpData);      //插入到list

	SendSingleNalu(frameInfo); //发送
	IVS_DELETE(frameInfo);
	return IVS_SUCCEED;
}

//插入FUA 分包,在插入新的数据之前判断是回放模式或者是否时间戳发生了跳变
int CSvacReassemble::InsertFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert FUA", " Rtp Data is NULL");
        return IVS_PARA_INVALID;
    }
    if (pstRtpData->lPayloadLen < 3) //FUA 长度要至少为3
    {
        IVS_LOG(IVS_LOG_ERR, "Insert FUA", "Svac insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return IVS_PARA_INVALID;
    }

    //查看缓存区内是否存在时间戳相同的帧数据
	int iRet = IVS_SUCCEED;
    if (NULL == GetFrameFromTimeTick(pstRtpData))
    {
        iRet = InsertNewFUA(pstRtpData);
		tryToSendFUA();
    }
    else
    {
        iRet = InsertExistFUA(pstRtpData);
		//检查是否可以发送一帧
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

		//返回具有相同时间戳且类型一致的帧数据
		if ((*iterBegin)->ulTimeStamp == pstRtpData->ulTimeStamp && !(*iterBegin)->bAll)
		{
			return *iterBegin;
		}
	}

	return NULL;
}


//插入新的FUA包
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
    frameInfo->pPackList.push_front(pstRtpData); //插入到list
    SetFrameByFu(frameInfo, pstRtpData); //设置起始位

	//检查帧是否收全;
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

//设置帧收到标志

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
		//现在的绝对时间戳只在结束标示位保存;
		pstFrame->ullTick = pstRtpData->ullTick;
    }
} //lint !e1762

//插入已经存在的FUA包
int CSvacReassemble::InsertExistFUA(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }
    //这里肯定不会为空，只有不为空才会执行这个函数;
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
            //完成插入，退出循环
            break;
        }
        //如果发现重复的包
        if (seqNum == (*beginIter)->usSeqNum)
        {
			IVS_LOG(IVS_LOG_ERR, "Insert Exist FUA", " Exist same seq packet" );
            return IVS_FAIL;
        }
    }
    //插入到正确的位置
    (void)pstList->insert(iterTmp, pstRtpData); //lint !e534
    SetFrameByFu(pstFrame, pstRtpData); //设置起始位
    CheckFrameSeq(pstFrame);

	//对已存在的包数据丢包值累加（水印有效）;
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

//不强制发送，符合条件才发，可认定为完整帧才发
bool CSvacReassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
{
	if (NULL == pstFrame)
	{
		IVS_LOG(IVS_LOG_ERR, "Send Complete Frame", " Rtp Data is NULL" );
		return false;
	}
	//如果要发送的帧没收全，返回，继续等待收全
	if (!pstFrame->bAll)
	{
		return false;
	}

	//如果帧收全，并且为单包，则以单包的形式发送
	if (1 == pstFrame->pPackList.size())
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFUA(pstFrame); //已经删除了每个packet
	}
	return true;
}

//根据当前的播放模式选择尝试采用不同的发送帧方式
void  CSvacReassemble::tryToSendFUA()
{
	//如果为单帧回放模式
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
	//查找FrameList里面的IDR位置
	RTP_FRAME_LIST_ITER idrIter = m_FrameList.begin();
	RTP_FRAME_LIST_ITER endIter = m_FrameList.end();
	//将帧数据发送
	RTP_FRAME_INFO* pFrameTmp = NULL;
	bool  bGetIdrFrame = false;
	//查找到队列里面的第一个IDR
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
	//如果当前帧已经收全或者缓存队列已经满
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
//发送单个nalu帧
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

    	//SVAC 单包自带00 00 00 01
        memcpy(m_pFrameBuf, pPack->pPayload, (unsigned int)pPack->lPayloadLen); //lint !e586
        //回调，将单包数据发送出
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

//发送FUA帧,
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
        if ((ulCopydSize + ulContentSize) > m_ulMaxBufLen) //需要复制的数据已经溢出了缓冲区
        {
            IVS_LOG(IVS_LOG_ERR, "SendRaw FU Frame", " Copy Data to buffer flooding");
            break;
        }

        memcpy(pCpBuf, pPack->pPayload + 2, ulContentSize);  //lint !e586 //跳过2个FU，进行复制 

        pCpBuf += ulContentSize;      //地址递增
        ulCopydSize += ulContentSize; //递增copy的数字
    }

    //回调，将单包数据发送出去
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
    m_FrameList.clear(); //清空List
    InitPara();
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear SVAC Frame end");
}

