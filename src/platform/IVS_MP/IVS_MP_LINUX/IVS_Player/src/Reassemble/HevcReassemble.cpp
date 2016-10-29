/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  HevcReassemble.cpp
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/

#include "HevcReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 

#define HEVC_STARTCODE_LEN 4 //startcode 长度
#define MAX_TIME_GAP  6000 //最大时间戳间隔

#define HEVC_RTP_APS   48
#define HEVC_RTP_FU    49
#define HEVC_RTP_PACI  50

#define RTP_HEVC_PAYLOAD_HEADER_SIZE  2


//lint -e954  本文件954错误无问题，统一屏蔽
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
    //同样一次性分配足够大的解码缓存空间
    m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
    if (NULL == m_pFrameBuf)
    {
		IVS_LOG(IVS_LOG_ERR, "Hevc Init", "Create frame buf error");
        return IVS_FAIL;
    }
    memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //设置0

	m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
	if (NULL == m_pDecryptDataBuf)
	{
		IVS_LOG(IVS_LOG_ERR,"Hevc Init", "Create Dec frame buf error");
		IVS_DELETE(m_pFrameBuf, MUILI);
		return IVS_FAIL;
	}
	memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //设置0

    //初始化内存池
    if (IVS_FAIL == m_packetPool.Init())
    {
		IVS_LOG(IVS_LOG_ERR,"Hevc Init","Create packetPool error");
        IVS_DELETE(m_pFrameBuf, MUILI);
        IVS_DELETE(m_pDecryptDataBuf, MUILI);
        m_packetPool.FreeMemPool();

        return IVS_FAIL;
    }
    m_pFrameBuf[3] = 0x01;     //startcode 的字节 00 00 00 01
    m_bStopInserFlag = false;
    m_bIsStepReset = false;
    return IVS_SUCCEED;
}

void CHevcReassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}


//清空本地缓存区的内容，回收全部内存块以及缓存区
int CHevcReassemble::ReSetReassemble()
{
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Hevc Frame begin.");

    m_bStopInserFlag = true;

	/*ClearFrameInfo();
	m_bStopInserFlag = false;*/
    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Hevc Frame end");

    return IVS_SUCCEED;
}

//插入RTP包，并完成组帧动作
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
			unsigned char nal_type = ((unsigned char)payload[0] >> 1) & 0x3f;
			
			RTP_HEVC_FU_HEADER *fuHdr = (RTP_HEVC_FU_HEADER *)(payload + RTP_HEVC_PAYLOAD_HEADER_SIZE);
            IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "pFuIndicator->TYPE:%u. pFuHeader->TYPE:%u.", nal_type, fuHdr->Type);

            if (!m_bIsStepReset)
            {
                IVS_LOG(IVS_LOG_DEBUG, "InsertPacket", "m_bIsStepReset is false.");
                //拖动录像回放，如果是P帧直接扔掉，直到收到第一个I帧，才真正插入
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

	// 合并打印日志;
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

    if (IVS_SUCCEED != iRet) //检查头错误
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
    //音频数据直接解码(rfc3551载荷类型定义)
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
		//检查时间戳是否发了跃变，发生则将队列的数据局强行发送清空缓存队列
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
	else//计算从MU到CU的丢包率,只计算视频包 
	{
		CaculateMU2CULostPacketNum(pstRtpData->usSeqNum);
		CaculateFPS(pstRtpData->ulTimeStamp);
		CaculateVideoCodeRate((unsigned long)pstRtpData->lPayloadLen, pstRtpData->ulTimeStamp);
	}
 	
	return iRet;
}


//检测并处理特殊事件，回放、时间戳跳变
void CHevcReassemble::CheckSpecialEvent(RTP_PACK_INFO* pstRtpData)
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
    if (4 > pstRtpData->lPayloadLen) //FU 长度要至少为4: 1 个payload
    {
        IVS_LOG(IVS_LOG_ERR, "Check IDR Frame", "HEVC insert FUA error, pstRtp data payload length:%u < 3",
                pstRtpData->lPayloadLen);
        return;
    }
    //获取当前分包的对象是IDR，还是P帧
    RTP_HEVC_FU_HEADER* pFuHeader = (RTP_HEVC_FU_HEADER*)(pstRtpData->pPayload + 2);
    //如果为回放模式，当收到一个IDR时，表示当前为新的GOP，强制发送缓存区的所有帧数据
    if (HEVC_NAL_IDR_N_LP == pFuHeader->Type ||
		HEVC_NAL_IDR_W_RADL == pFuHeader->Type)
    {
        //强制发送所有frameList里面的帧数据
        unsigned int listSize = m_FrameList.size();
        for (unsigned int i = 0; i < listSize; i++)
        {
            SendLastListFrame(FRAME_MUST_SEND);
        }
        IVS_LOG(IVS_LOG_DEBUG, "Check IDR Frame", "Clear Date. TYPE:%u.", pFuHeader->Type);
        m_FrameList.clear();
    }
} //lint !e818

//发送时间戳发送跳变之前的数据
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
	frameInfo->pPackList.push_front(pstRtpData);      //插入到list
	int iRet = IVS_SUCCEED;
	switch (nal_type)
	{
	case HEVC_NAL_SPS:      //如果为SPS,先预解获取图像宽高信息
		GetPicHW(frameInfo); 
		/* fall through */
		//lint -fallthrough
	default:
		if (TRUE == m_iPlayBackMode)
		{
			//单帧回放模式下SPS，PPS来了就发送
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

//插入FUA 分包,在插入新的数据之前判断是回放模式或者是否时间戳发生了跳变
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

    //获取当前分包的对象是IDR，还是P帧
    RTP_HEVC_FU_HEADER *pFuHeader = (RTP_HEVC_FU_HEADER *)(pstRtpData->pPayload + 2);
    //定义当前帧是IDR，还是P帧
    pstRtpData->hevc_nal_type = pFuHeader->Type;

    int iRet = IVS_FAIL;
    //查看缓存区内是否存在时间戳相同的帧数据
    if (NULL == GetFrameFromTimeTick(pstRtpData))
    {
        iRet = InsertNewFU(pstRtpData);

		tryToSendFU();
    }
    else
    {
        iRet = InsertExistFU(pstRtpData);
		//检查是否可以发送一帧
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

		//返回具有相同时间戳且类型一致的帧数据
		if ((*iterBegin)->ulTimeStamp == pstRtpData->ulTimeStamp &&
			(*iterBegin)->hevc_nal_type == pstRtpData->hevc_nal_type &&
			!(*iterBegin)->bAll)
		{
			return *iterBegin;
		}
	}

    return NULL;
}


//插入新的FU包
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
		//现在的绝对时间戳只在结束标示位保存;
		pstFrame->ullTick = pstRtpData->ullTick;
    }
} //lint !e1762

//插入已经存在的FUA包
int CHevcReassemble::InsertExistFU(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Exist FU", " Rtp Data is NULL" );
        return IVS_PARA_INVALID;
    }
    //这里肯定不会为空，只有不为空才会执行这个函数;
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
bool CHevcReassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
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
	if ((1 == pstFrame->pPackList.size()) && 
		(HEVC_RTP_FU != pstFrame->hevc_rtp_payload_type))
	{
		SendSingleNalu(pstFrame);
	}
	else
	{
		(void) SendFU(pstFrame); //已经删除了每个packet
	}
	return true;
}

//根据当前的播放模式选择尝试采用不同的发送帧方式
void  CHevcReassemble::tryToSendFU()
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

void CHevcReassemble::SendGopFrame()
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
    if (pstFrame->pPackList.size() != 1) //应该是一个的单包
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send Single Nalu",
                "HEVC splitter send single nalu error,time stamp:%u, pack list size:%d is not 1",
                pstFrame->pPackList.front()->ulTimeStamp, pstFrame->pPackList.size());
    }
    //当前要发送的帧和上一次发送的不一样才发送
    else
    {
        RTP_PACK_INFO* pPack = pstFrame->pPackList.front();
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send Single Nalu", "RTP Pack info is NULL" );
            ReleaseRtpInfo(pstFrame);
            return;
        }
        //拷贝到帧缓冲区，因为头部增加了startcode
        memcpy(m_pFrameBuf + HEVC_STARTCODE_LEN, pPack->pPayload, (unsigned int)pPack->lPayloadLen); //lint !e586
        //回调，将单包数据发送出
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H265;
        rawFrameInfo.uiFrameType = pstFrame->hevc_nal_type;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;
		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;

		//如果有丢包或帧不全，则将水印值赋值为0，不进行水印校验;
		if ((0 != pPack->usLostFrame) || (!pstFrame->bAll))
		{
			rawFrameInfo.uiWaterMarkValue = 0;
		}

        rawFrameInfo.uiWidth   = m_uiWidth;
        rawFrameInfo.uiHeight = m_uiHeight;
		rawFrameInfo.uiGopSequence = m_uGopSeq;
        //收包状态
        REC_PACKET_INFO recPacketInfo;
        recPacketInfo.uAllPacketNum  = SING_PACKET_SIZE;
        recPacketInfo.uLostPacketNum = UNLOSE_PACKET_NUM;

        CaculateLostPacketRate(SING_PACKET_SIZE, UNLOSE_PACKET_NUM);
        
        m_lock.Lock(); //lint !e534
        //回调数据发送拼完帧的数据
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, (unsigned long)pPack->lPayloadLen + HEVC_STARTCODE_LEN,
                              &recPacketInfo,
                              m_pUser);
        }
        m_lock.UnLock(); //lint !e534
        //设置最新发送的类型
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

//发送FUA帧,
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

    if (pstFrame->pPackList.empty()) //这种事情不可能
    {
        IVS_LOG(IVS_LOG_DEBUG, "Send FUA Frame", "frame pack list size is 0");
        return IVS_FAIL;
    }

    unsigned long ulCopydSize = 2 + HEVC_STARTCODE_LEN;  
    char* pCpBuf = m_pFrameBuf + 2 + HEVC_STARTCODE_LEN;

  	// 重构nal header
    unsigned char *payload = (unsigned char *)pstFrame->pPackList.back()->pPayload;
	unsigned char *pNalHdr = (unsigned char *)m_pFrameBuf + HEVC_STARTCODE_LEN;
	pNalHdr[0] = (payload[0] & 0x81) | ((payload[2] & 0x3f) << 1); 
	pNalHdr[1] = payload[1]; 

    bool bError = false;

    RTP_PACK_LIST_REV_ITER revBeginIter = pstFrame->pPackList.rbegin();

    RTP_PACK_LIST_REV_ITER revEndIter = pstFrame->pPackList.rend();

    int ret = IVS_FAIL;

	unsigned int skip = RTP_HEVC_PAYLOAD_HEADER_SIZE + sizeof(RTP_HEVC_FU_HEADER) + (m_using_donl_field ? 2 : 0);

    //循环删除里面的内存
    for (; revBeginIter != revEndIter; ++revBeginIter)
    {
        RTP_PACK_INFO* pPack = *revBeginIter;
        if (NULL == pPack)
        {
            IVS_LOG(IVS_LOG_ERR, "Send FUA Frame", "RTP Pack info is null.");
            continue;
        }

        unsigned long ulContentSize = (unsigned long)pPack->lPayloadLen - skip; //在插入的时候就保证了大于3,去掉头
        if ((ulCopydSize + ulContentSize) > m_ulMaxBufLen) //需要复制的数据已经溢出了缓冲区
        {
            IVS_LOG(IVS_LOG_ERR, "SendRaw FUA Frame", " Copy Data to buffer flooding");
            bError = true;
            break;
        }

        memcpy(pCpBuf, pPack->pPayload + skip, ulContentSize);  //lint !e586 //跳过2个FU，进行复制 

        pCpBuf += ulContentSize;      //地址递增
        ulCopydSize += ulContentSize; //递增copy的数字
    }

    //没有发生错误，且上次发送的时间戳不一致
    if (!bError && ((pstFrame->ulTimeStamp != m_preSendFrameTimeStamp) || (pstFrame->fuType != m_preSendFrameTimeStamp)))
    {
        //应该收到的包数
        unsigned short usShouldRevPacketNum = 0;

        //实际收到的包数
        unsigned short usRevPacketNum = 0;

        CaculatePacketNum(&usShouldRevPacketNum, &usRevPacketNum);

        //回调，将单包数据发送出去
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_H265;
        rawFrameInfo.uiFrameType = pstFrame->hevc_nal_type;
		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;

		rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
		//如果有丢包或帧不全，则将水印值赋值为0，不进行水印校验;
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

        //收包状态
        REC_PACKET_INFO recPacketInfo;
        recPacketInfo.uAllPacketNum  = usShouldRevPacketNum;
        recPacketInfo.uLostPacketNum = usShouldRevPacketNum - usRevPacketNum;

        CaculateLostPacketRate(recPacketInfo.uAllPacketNum, recPacketInfo.uLostPacketNum);
      
        m_lock.Lock(); //lint !e534
        //回调数据发送拼完帧的数据
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, ulCopydSize, &recPacketInfo, m_pUser);
        }
        m_lock.UnLock(); //lint !e534
        //设置最新发送的时间戳和类型
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
    m_FrameList.clear(); //清空List
    InitPara();
    IVS_LOG(IVS_LOG_DEBUG, "ClearFrameInfo", "Clear Hevc Frame end");
}

