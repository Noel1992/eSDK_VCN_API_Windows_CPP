/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  MjpegReassemble.cpp
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/

#include "MjpegReassemble.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif

#define RST_HEADER_LEN 4
#define QTABLE__LEN 4
#define JPEG_HEADER_LEN 8

int jpeg_luma_quantizer[64] = {
    16, 11, 10, 16,  24,  40,  51,	61,
    12, 12, 14, 19,  26,  58,  60,	55,
    14, 13, 16, 24,  40,  57,  69,	56,
    14, 17, 22, 29,  51,  87,  80,	62,
    18, 22, 37, 56,  68, 109, 103,	77,
    24, 35, 55, 64,  81, 104, 113,	92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

/*
 *	Table K.2 from JPEG spec.
 */
int jpeg_chroma_quantizer[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};



//Z字型扫描数组; 
int jpeg_natural_order[64+16] = {
	0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63,
	63, 63, 63, 63, 63, 63, 63, 63,//extra entries for safety
	63, 63, 63, 63, 63, 63, 63, 63
};

unsigned char lum_dc_codelens[] = {
    0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

unsigned char lum_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

unsigned char lum_ac_codelens[] = {
    0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

unsigned char lum_ac_symbols[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

unsigned char chm_dc_codelens[] = {
    0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

unsigned char chm_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

unsigned char chm_ac_codelens[] = {
    0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

unsigned char chm_ac_symbols[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

//lint -e954  本文件954错误无问题，统一屏蔽
//lint -e944  安全函数宏定义，不作修改

CMjpegReassemble::~CMjpegReassemble()
{
	m_bStopInserFlag = true;
	try
	{
		IVS_DELETE(m_pLqt,MUILI);
		IVS_DELETE(m_pCqt,MUILI);
		IVS_DELETE(m_pLast_qt,MUILI);
		IVS_DELETE(m_pMjpeg_header,MUILI);
	}
	catch (...)
	{}
}

int CMjpegReassemble::Init()
{
    do
    {
        //同样一次性分配足够大的解码缓存空间
        m_pFrameBuf = IVS_NEW(m_pFrameBuf, FRAME_BUFF_LENTH_HD);
        if (NULL == m_pFrameBuf)
        {
            break;
        }
        memset(m_pFrameBuf, 0, FRAME_BUFF_LENTH_HD);       //设置0
        if (IVS_FAIL == m_packetPool.Init())
        {
            break;
        }
        m_pLqt = IVS_NEW(m_pLqt, 64);
        if (NULL == m_pLqt)
        {
            break;
        }
        memset(m_pLqt, 0, 64);       //设置0
        m_pCqt = IVS_NEW(m_pCqt, 64);
        if (NULL == m_pCqt)
        {
            break;
        }
        memset(m_pCqt, 0, 64);       //设置0
        m_pLast_qt = IVS_NEW(m_pLast_qt, 64);
        if (NULL == m_pLast_qt)
        {
            break;
        }
        memset(m_pLast_qt, 0, 64);       //设置0
        m_pMjpeg_header = IVS_NEW(m_pMjpeg_header, 2048);
        if (NULL == m_pMjpeg_header)
        {
            break;
        }
        memset(m_pMjpeg_header, 0, 2048);       //设置0

		m_pDecryptDataBuf = IVS_NEW(m_pDecryptDataBuf, DECRYPT_BUF_LENGTH);
		if (NULL == m_pDecryptDataBuf)
		{
			break;
		}
		memset(m_pDecryptDataBuf, 0, DECRYPT_BUF_LENGTH);       //设置0

        m_nQ = 0;
        m_nType = 0;
		m_bStopInserFlag = false;
        return IVS_SUCCEED;
    } while (false);  //lint !e717
	IVS_LOG(IVS_LOG_ERR,"Mjpeg Init","Create memory error");
    //失败回收申请的内存空间
    IVS_DELETE(m_pFrameBuf, MUILI);
    IVS_DELETE(m_pLqt, MUILI);
    IVS_DELETE(m_pCqt, MUILI);
    IVS_DELETE(m_pLast_qt, MUILI);
    IVS_DELETE(m_pMjpeg_header, MUILI);
	IVS_DELETE(m_pDecryptDataBuf, MUILI);
    m_packetPool.FreeMemPool();
    return IVS_FAIL;
}

int CMjpegReassemble::InsertPacket( char* pBuf, unsigned int uBufLen)
{
    int iRet = IVS_FAIL;
    if (NULL == pBuf)
    {
        IVS_LOG(IVS_LOG_ERR, "Mjpeg Insert Packet", "Buffer is NULL");
        return iRet;
    }

    if (m_bStopInserFlag)
    {
        ClearFrameInfo();
        m_bStopInserFlag = false;
        return IVS_SUCCEED;
    }

	/*
	 * add by w00210470, filter payload type
	 */
	if (IVS_SUCCEED != CPacketReassemble::InsertPacket(pBuf, uBufLen))
	{
		return IVS_FAIL;
	}

	FIXED_HEADER* pHdrTmp = (FIXED_HEADER*)pBuf;//lint !e826
	IVS_LOG(IVS_LOG_DEBUG, "Insert Packet", "TimeStamp:%u ,Seq:%u ,marker:%d ",ntohl(pHdrTmp->timestamp),ntohs(pHdrTmp->seq_no),pHdrTmp->marker);

    RTP_PACK_INFO* pstRtpData = NULL;
    iRet = GetRtpPacketInfo(pBuf, uBufLen, pstRtpData);

    if (IVS_SUCCEED != iRet) //检查头错误
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Packet", "Mjpeg insert packet error, check RTP header failed");
		FreePacket(pstRtpData);
        return iRet;
    }

    if (NULL == pstRtpData)
    {
        IVS_LOG(IVS_LOG_ERR, "Mjpeg Insert Packet", "Get RTP Info error");
        return IVS_FAIL;
    }
    //音频数据直接解码(rfc3551载荷类型定义)
    if ((PAY_LOAD_TYPE_PCMU == pstRtpData->pt) || (PAY_LOAD_TYPE_PCMA == pstRtpData->pt))
    {
		SendAudioFrame(pstRtpData);
		return IVS_SUCCEED;
    }

    (void)PreDecodeMjpegHeader(pstRtpData->pPayload,(unsigned long) pstRtpData->lPayloadLen);
    //负载部分要去掉一个rtp的包长，紧接着是jpeg的Head
    iRet = InsertFragment(pstRtpData);
    if (IVS_SUCCEED != iRet)
    {
		IVS_LOG(IVS_LOG_ERR,"Insert Packet", "Insert Frame Error,timeStamp: %d ", pstRtpData->ulTimeStamp);
        FreePacket(pstRtpData);
    }
    return iRet;
}

void CMjpegReassemble::SetResetFlag(bool bIsReset)
{
	m_bIsStepReset = bIsReset;
}

int CMjpegReassemble::ReSetReassemble()
{
	IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Mjpeg Frame begin.");
	m_bStopInserFlag = true;

    IVS_LOG(IVS_LOG_DEBUG, "ReSetReassemble", "Clear Mjpeg Frame end");
    return IVS_SUCCEED;
}

int CMjpegReassemble::PreDecodeMjpegHeader(char* pBuff, unsigned long ulBufLen)
{
    if ((NULL == pBuff) || (0 == ulBufLen))
    {
        IVS_LOG(IVS_LOG_ERR, "PreDecode Mjpeg Header", "PreDecode mjpeg header failed, input param is invalid.");
        return IVS_FAIL;
    }

    JPEG_HEADER* pJPEGHeader = (JPEG_HEADER*)pBuff; //lint !e826

    m_nQ = pJPEGHeader->q;
    m_nType = pJPEGHeader->type;
	m_uiWidth = pJPEGHeader->width * 8;
	m_uiHeight = pJPEGHeader->height * 8;
	 
    return IVS_SUCCEED;
}

int CMjpegReassemble::InsertFragment(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Fragment", "Rtp data is NULL");
        return IVS_PARA_INVALID;
    }

    if (3 > pstRtpData->lPayloadLen)
    {
        IVS_LOG(IVS_LOG_ERR, "Insert Fragment", "pstRtp data payload length Error");
        return IVS_FAIL;
    }

    int iRet = IVS_FAIL;
    if (GetFrameFromTimeTick(pstRtpData, MJPEGREASSEMBLE) == NULL)
    {
        //在插入新的Fragment时加上头
        iRet = InsertNewFragment(pstRtpData);
    }
    else
    {
        iRet = InsertExitFragment(pstRtpData);
		SendLastListFrame();
    }
    return iRet;
}

int CMjpegReassemble::InsertNewFragment(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert New Fragment", "Rtp data is NULL");
        return IVS_FAIL;
    }
    RTP_FRAME_INFO* pstframeInfo = IVS_NEW(pstframeInfo);
    if (NULL == pstframeInfo)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert New Fragment", "Alloc Frame Error");
        return IVS_FAIL;
    }
	pstframeInfo->bAll = false;
	pstframeInfo->bE   = false;
	pstframeInfo->bS   = false;

	pstframeInfo->ullTick = pstRtpData->ullTick; 
	pstframeInfo->ulTimeStamp = pstRtpData->ulTimeStamp;
	pstframeInfo->usLostFrame = pstRtpData->usLostFrame;
	pstframeInfo->uWaterMarkValue = pstRtpData->uWaterMark;


    JPEG_HEADER* pJPEGHeader = (JPEG_HEADER*)pstRtpData->pPayload; //lint !e826
    if ((0 == pJPEGHeader->off) && !(pstRtpData->ucMark))
    {
        pstframeInfo->bS = true;
    }
    pstframeInfo->pPackList.push_back(pstRtpData); //插入到list
	int iRet = IVS_FAIL;
	if (TRUE == m_iPlayBackMode)
	{
		//在单帧回放模式中不存在参考帧的概念
		iRet = InsertFrameToPlayBlackCache(pstframeInfo,m_ulFrameCacheLimit);
	}
	else
	{
		iRet= InsertFrameToCache(pstframeInfo);
	}
    if (IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pstframeInfo);
    }

    return iRet;
}

int CMjpegReassemble::InsertExitFragment(RTP_PACK_INFO* pstRtpData)
{
    if (NULL == pstRtpData)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Exit Fragment", "Rtp data is NULL");
        return IVS_FAIL;
    }

    //获取具有相同时间戳的帧数据
    RTP_FRAME_INFO* pstframeInfo = GetFrameFromTimeTick(pstRtpData,MJPEGREASSEMBLE);
    if (NULL == pstframeInfo)
    {
		IVS_LOG(IVS_LOG_ERR, "Insert Exit Fragment", "get same timeStamp frame is NULL");
        return IVS_FAIL;
    }


	if (0 != pstRtpData->ullTick)
	{
		pstframeInfo->ullTick = pstRtpData->ullTick;
	}

    RTP_PACK_LIST* pstList = &(pstframeInfo->pPackList);

    //开始遍历所有RTP包，插入RTP包
    RTP_PACK_LIST_ITER beginIter = pstList->begin();
    RTP_PACK_LIST_ITER endIter = pstList->end();
	RTP_PACK_LIST_ITER iterTmp = endIter;
    unsigned short seqNum = pstRtpData->usSeqNum;

	for (; beginIter != endIter; ++beginIter)
	{
		if (SEQ_AFTER(seqNum, (*beginIter)->usSeqNum))
		{
			iterTmp = beginIter;
			//完成插入，退出循环
			break;
		}
		//如果发现重复的包
		if (seqNum == (*beginIter)->usSeqNum)
		{
			return IVS_FAIL;
		}
	}
	//插入到正确的位置
	(void)pstList->insert(iterTmp,pstRtpData);//lint !e534

	//查看是否发现开始、结束标示
	JPEG_HEADER* pJPEGHeader = (JPEG_HEADER*)pstRtpData->pPayload; //lint !e826
	if ((0 != pJPEGHeader->off) && (pstRtpData->ucMark))
	{
	    pstframeInfo->bE = true;
	}

    if ((0 == pJPEGHeader->off) && !(pstRtpData->ucMark))
    {
    	pstframeInfo->bS = true;
    }

    //检测帧中的包数据是否全部收全
    CheckFrameSeq(pstframeInfo);
    return IVS_SUCCEED;
}

bool CMjpegReassemble::SendCompleteFrame(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
		IVS_LOG(IVS_LOG_ERR, "Send Complete Frame", "Rtp data is NULL");
        return false;
    }

    //如果要发送的帧没收全，返回，继续等待收全
    if (!pstFrame->bAll)
    {
        return false;
    }

    SendFrame(pstFrame);

    return true;
}

int CMjpegReassemble::MakeMjpegHeadAndCopyToBuf(RTP_FRAME_INFO* pstFrame, unsigned short& usQtable_len,
                                                int& iHeader_len,
                                                int& iRst_header_len)
{
    if (NULL == pstFrame)
    {
        IVS_LOG(IVS_LOG_ERR, "Make Mjpeg Head", "Frame info is NULL");
        return IVS_FAIL;
    }

    if ((NULL == m_pLqt) || (NULL == m_pCqt) || (NULL == m_pLast_qt) || (NULL == m_pMjpeg_header))
    {
        IVS_LOG(IVS_LOG_ERR, "Make Mjpeg Head", "Mjpeg Head Sapce is NULL");
        return IVS_FAIL;
    }

    RTP_PACK_INFO* pPack = pstFrame->pPackList.back();

    unsigned char* pBuff = (unsigned char*)(pPack->pPayload);

    JPEG_HEADER* pJPEGHeader = (JPEG_HEADER*)pBuff; //lint !e826

    //重新赋值m_nQ、m_nType
    m_nQ = pJPEGHeader->q;
    m_nType = pJPEGHeader->type;

    int iMBWidth  = pJPEGHeader->width;
    int iMBHeight = pJPEGHeader->height;

    jpeghdr_rst *rst_header; // 复位标记头
    unsigned short dri = 0;
    jpeghdr_qtable *qTable; // 量化表头
	bool bUseLastqt = false;
    //重新初始化对象值（此处不初始化也可以，后面赋值的时候会全部循环重新赋值）
    memset(m_pLqt, 0, 64);
    memset(m_pCqt, 0, 64);

    if ((m_nType >= 64) && (m_nType <= 127))
    {
        // 此时mainheader后跟着复位标记头
        rst_header = (jpeghdr_rst*) (pBuff + 8); //lint !e826
        dri = ntohs(rst_header->dri);
        iRst_header_len = RST_HEADER_LEN;
    }

    if ((m_nQ >= 0) && (m_nQ <= 127))
    {
        // 此时根据Q值计算出量化表
        MakeTables(m_nQ, m_pLqt, m_pCqt);
    }
    else
    {
        //此时量化表头跟在第一个jpeg header和复位标记头后面，需要手动取出来，根据rfc2435协议，偏移32字节后的字节即为量化表头
        qTable = (jpeghdr_qtable *) (pBuff + JPEG_HEADER_LEN + iRst_header_len); //lint !e826
        usQtable_len = ntohs(qTable->length); //lint !e732 !e713
        if (usQtable_len > 192)
        {
			IVS_LOG(IVS_LOG_ERR, "MakeMjpegHeadAndCopyToBuf", "usQtable_len[%d] > 192", usQtable_len);
            return IVS_FAIL;
        }

        //此处会不会发生拷贝越界，需要考虑
        memcpy(m_pLqt, pPack->pPayload  + JPEG_HEADER_LEN + iRst_header_len + QTABLE__LEN, 64); //跳过JPEG_HEADER和前面的量化表头结构体，复制第一个量化表到lqt
        memcpy(m_pCqt, pPack->pPayload  + JPEG_HEADER_LEN + iRst_header_len + QTABLE__LEN + 64, 64); //跳过JPEG_HEADER和前面的量化表头结构体和第一个量化表，复制第二个量化表到cqt
        if (usQtable_len == 192)
        {
            memcpy(m_pLast_qt, pPack->pPayload + JPEG_HEADER_LEN + iRst_header_len
                   + QTABLE__LEN + 128, 64); //复制第三个量化表到last_qt
			bUseLastqt = true;
        }
    }

    memset(m_pMjpeg_header, 0, 2048);
	if (bUseLastqt)
	{
		iHeader_len = MakeHeaders(m_pMjpeg_header, m_nType, iMBWidth, iMBHeight, m_pLqt, m_pCqt, dri, m_pLast_qt);
	}
	else
	{
		iHeader_len = MakeHeaders(m_pMjpeg_header, m_nType, iMBWidth, iMBHeight, m_pLqt, m_pCqt, dri, NULL);
	}

    if (0 > iHeader_len || 2048 < iHeader_len)
    {
		IVS_LOG(IVS_LOG_ERR, "MakeMjpegHeadAndCopyToBuf", "Copy m_pMjpeg_header Error");
		return IVS_FAIL;
    }
    //先拷贝生成的mjpeg头
    memcpy(m_pFrameBuf, m_pMjpeg_header, (unsigned int)iHeader_len);

    return IVS_SUCCEED;
}

void CMjpegReassemble::SendFrame(RTP_FRAME_INFO* pstFrame)
{
    if (NULL == pstFrame)
    {
		IVS_LOG(IVS_LOG_ERR, "Send Frame", " frame info is NULL");
        return;
    }
    
	/*
	 * DTS2013060708724  
	 * MJPEG 只发送完整帧，否则解码花屏
	 * fix by w00210470
	 */
	if (!pstFrame->bAll)
	{
		IVS_LOG(IVS_LOG_INFO, "Send Frame", "MJPEG Drop Frame With Packet Lost");
		ReleaseRtpInfo(pstFrame);
		return;
	}
	
	if (pstFrame->pPackList.empty()) //这种事情不可能
    {
        IVS_LOG(IVS_LOG_ERR, "Send Frame", "MJPEG send frame error,frame pack list size is 0.");
        return;
    }
	if (NULL == m_pFrameBuf)
	{
		 IVS_LOG(IVS_LOG_ERR, "Make Mjpeg Head", "Frame Buf is NULL");
		 ReleaseRtpInfo(pstFrame);
		 return;
	}
    int iHeader_len = 0;
    int iRst_header_len = 0;
    unsigned short usQtable_len = 0;
    char* pCpBuf = m_pFrameBuf;

    int iRet = MakeMjpegHeadAndCopyToBuf(pstFrame, usQtable_len, iHeader_len, iRst_header_len);

    if (IVS_SUCCEED != iRet)
    {
		IVS_LOG(IVS_LOG_ERR, "Make Mjpeg Head", "Make Mjpeg Head Error");
        ReleaseRtpInfo(pstFrame);
        return;
    }

    unsigned long ulCopiedSize = 0;
    pCpBuf += iHeader_len;      //地址递增
    ulCopiedSize += iHeader_len; //lint !e737 //递增copy的数字

    bool bError = false;

    RTP_PACK_LIST_REV_ITER revBeginIter = pstFrame->pPackList.rbegin();

    RTP_PACK_LIST_REV_ITER revEndIter = pstFrame->pPackList.rend();

	RTP_PACK_LIST_REV_ITER headIter = pstFrame->pPackList.rbegin();

    //循环删除里面的内存
    for (; revBeginIter != revEndIter; ++revBeginIter)
    {
        RTP_PACK_INFO* pPack = *revBeginIter;
        long lContentSize;
        lContentSize = (pPack->lPayloadLen - JPEG_HEADER_LEN) - iRst_header_len; //跳过JPEG_HEADER
        if ((ulCopiedSize + (unsigned long)lContentSize) > m_ulMaxBufLen) //需要复制的数据已经溢出了缓冲区
        {
            IVS_LOG(IVS_LOG_ERR, "Send Frame",
                    "MJPEG send frame error, copied size:%u + content size:%u > buffer size:%d",
                    ulCopiedSize, lContentSize, m_ulMaxBufLen);
            bError = true;
            break;
        }
		//如果为表头（起始包）
        if ((revBeginIter == headIter) && (m_nQ >= 128) && (m_nQ <= 255))
        {
            lContentSize = (pPack->lPayloadLen - JPEG_HEADER_LEN) - iRst_header_len - QTABLE__LEN - usQtable_len; //lint !e834 //跳过JPEG_HEADER
            if (0 > lContentSize)
            {
                IVS_LOG(IVS_LOG_ERR, "Send Frame", "Mjpeg Memroy coye size Error");
                bError = true;
                break;
            }

            memcpy(pCpBuf, pPack->pPayload + JPEG_HEADER_LEN + iRst_header_len
                   + QTABLE__LEN + usQtable_len, (unsigned int)lContentSize); //跳过JPEG_HEADER和前面的复位标记和量化表头，进行复制
        }
        else
        {
            // 每个包后面都可能存在复位标志头
            lContentSize = pPack->lPayloadLen - JPEG_HEADER_LEN - iRst_header_len; //lint !e834 !e737 //跳过JPEG_HEADER
            if (0 > lContentSize)
            {
                IVS_LOG(IVS_LOG_ERR, "Send Frame", "Mjpeg Memroy coye size Error");
                bError = true;
                break;
            }
            memcpy(pCpBuf, pPack->pPayload + JPEG_HEADER_LEN + iRst_header_len,
                   (unsigned int)lContentSize);                                                                                  //跳过JPEG_HEADER，进行复制
        }
        pCpBuf += lContentSize;      //地址递增
        ulCopiedSize += (unsigned long)lContentSize; //递增copy的数字
    }

    if (!bError)
    {
		//应该收到的包数
		unsigned short usShouldRevPacketNum = 0;
		//实际收到的包数
		unsigned short usRevPacketNum = 0;
		CaculatePacketNum(&usShouldRevPacketNum, &usRevPacketNum);
        //回调，将单包数据发送出去
        IVS_RAW_FRAME_INFO rawFrameInfo;
        rawFrameInfo.uiStreamType = PAY_LOAD_TYPE_MJPEG;

		rawFrameInfo.ullTimeTick = pstFrame->ullTick;
        rawFrameInfo.uiTimeStamp = (unsigned int) pstFrame->ulTimeStamp;

        rawFrameInfo.uiWaterMarkValue = pstFrame->uWaterMarkValue;
		//如果丢包，则设置水印值为0
		if (0 != pstFrame->usLostFrame)
		{
			rawFrameInfo.uiWaterMarkValue = 0;
		}

		rawFrameInfo.uiWidth = m_uiWidth;
		rawFrameInfo.uiHeight = m_uiHeight;
		rawFrameInfo.uiGopSequence = m_uGopSeq++;
        //收包状态
        REC_PACKET_INFO recPacketInfo;
        recPacketInfo.uAllPacketNum  = usShouldRevPacketNum;
        recPacketInfo.uLostPacketNum = usShouldRevPacketNum - usRevPacketNum;
        CaculateVideoCodeRate(ulCopiedSize, pstFrame->ulTimeStamp);
        CaculateLostPacketRate(recPacketInfo.uAllPacketNum, recPacketInfo.uLostPacketNum);
		CaculateFPS(pstFrame->ulTimeStamp);
        m_lock.Lock(); //lint !e534

        //回调数据发送拼完帧的数据
        if (NULL != m_pReassmCallBack)
        {
            m_pReassmCallBack(&rawFrameInfo, m_pFrameBuf, ulCopiedSize, &recPacketInfo, m_pUser);
        }

        m_lock.UnLock(); //lint !e534
    }
    ReleaseRtpInfo(pstFrame);
}

void CMjpegReassemble::ClearFrameInfo()
{
	IVS_LOG(IVS_LOG_INFO, "Clear Frame Info", "Clear Frame Info begin");
	RTP_FRAME_LIST_ITER frameListBeginIter = m_FrameList.begin();

	RTP_FRAME_LIST_ITER frameListEndIter = m_FrameList.end();

	for (; frameListBeginIter != frameListEndIter; ++frameListBeginIter)
	{
		ReleaseRtpInfo(*frameListBeginIter);
		IVS_DELETE(*frameListBeginIter);
	}

	m_FrameList.clear(); //清空List
	InitPara();
	IVS_LOG(IVS_LOG_INFO, "Clear Frame Info", "Clear Frame Info end");
}

unsigned char * CMjpegReassemble::MakeQuantHeader(unsigned char *p, const unsigned char *qt, const int tableNo)
{
    *p++ = 0xff;
    *p++ = 0xdb;            /* DQT */
    *p++ = 0;               /* length msb */
    *p++ = 67;              /* length lsb */
    *p++ = tableNo; //lint !e734
    memcpy(p, qt, 64);
    return (p + 64);
} //lint !e1762

unsigned char * CMjpegReassemble::MakeDRIHeader(unsigned char *p, u_short dri)
{
    *p++ = 0xff;
    *p++ = 0xdd;            /* DRI */
    *p++ = 0x0;             /* length msb */
    *p++ = 4;               /* length lsb */
    *p++ = dri >> 8;        /* dri msb */
    *p++ = dri & 0xff;      /* dri lsb */
    return (p);
} //lint !e1762

unsigned char * CMjpegReassemble::MakeHuffmanHeader(unsigned char *p, const unsigned char *codelens, int ncodes,
                                                    const unsigned char *symbols, int nsymbols, int tableNo,
                                                    int tableClass)
{
    *p++ = 0xff;

    *p++ = 0xc4;            /* DHT */
    *p++ = 0;               /* length msb */
    *p++ = 3 + ncodes + nsymbols; //lint !e734 /* length lsb */
    *p++ = (tableClass << 4) | tableNo; //lint !e701 !e734
    memcpy(p, codelens, ncodes); //lint !e732
    p += ncodes;
    memcpy(p, symbols, nsymbols); //lint !e732
    p += nsymbols;
    return (p);
} //lint !e1762

void CMjpegReassemble::MakeTables(int q, unsigned char *lqt, unsigned char *cqt)
{
    int I;
    int factor = q;

    if (q < 1)
    {
        factor = 1;
    }

    if (q > 99)
    {
        factor = 99;
    }

    if (q < 50)
    {
        q = 5000 / factor;
    }
    else
    {
        q = 200 - factor * 2;
    }

    for (I = 0; I < 64; I++)
    {
        int lq = (jpeg_luma_quantizer[I] * q + 50) / 100;
        int cq = (jpeg_chroma_quantizer[I] * q + 50) / 100;

        /* Limit the quantizers to 1 <= q <= 255 */
        if (lq < 1)
        {
            lq = 1;
        }
        else if (lq > 255)
        {
            lq = 255;
        }
		else
		{

		}

        lqt[I] = lq; //lint !e734

        if (cq < 1)
        {
            cq = 1;
        }
        else if (cq > 255)
        {
            cq = 255;
        }
		else
		{

		}

        cqt[I] = cq; //lint !e734	
    }
} //lint !e1762

int CMjpegReassemble::MakeHeaders(unsigned char *p, int type, int w, int h, const unsigned char *lqt,
                                  const unsigned char *cqt, u_short dri, const unsigned char *last_qt)
{
    unsigned char *start = p;

    /* convert from blocks to pixels */
    w <<= 3; //lint !e701
    h  <<= 3; //lint !e701
    *p++ = 0xff;
    *p++ = 0xd8;            /* SOI */

    // 为了适配海思解码器将三个量化表放到一起做了修改;
    if (NULL == last_qt)
    {
		//先对lqt\cqt进行zigzag处理;
		unsigned char tmpLqt[64] = {0};
		MakeZigZag(lqt, tmpLqt);
		p = MakeQuantHeader(p, tmpLqt, 0);

		unsigned char tmpCqt[64] = {0};
		MakeZigZag(cqt, tmpCqt);
		p = MakeQuantHeader(p, tmpCqt, 1);

        if (dri != 0)
        {
            p = MakeDRIHeader(p, dri);
        }
        *p++ = 0xff;
		// SOF 
        *p++ = 0xc0;            
		//length msb 
        *p++ = 0;               
		//length lsb
        *p++ = 17;  
		//8-bit precision 
        *p++ = 8;    
		//height msb
        *p++ = h >> 8;          //lint !e702 !e734
		//height lsb
        *p++ = h;               //lint !e734 
		//width msb
        *p++ = w >> 8;          //lint !e702 !e734 
		//wudth lsb
        *p++ = w;               //lint !e702 !e734 
		 //number of components
        *p++ = 3;              
        *p++ = 0;               /* comp 0 */
        if (type == 0)
        {
            *p++ = 0x21;
        }                   /* hsamp = 2, vsamp = 1 */
        else
        {
            *p++ = 0x22;
        }                   /* hsamp = 2, vsamp = 2 */
        *p++ = 0;               /* quant table 0 */
        *p++ = 1;               /* comp 1 */
        *p++ = 0x11;            /* hsamp = 1, vsamp = 1 */
        *p++ = 1;               /* quant table 1 */
        *p++ = 2;               /* comp 2 */
        *p++ = 0x11;            /* hsamp = 1, vsamp = 1 */
        *p++ = 1;               /* quant table 1 */
    }
    else
    {
        //  海思编码时将三个量化表放到一起;
        *p++ = 0xff;
        *p++ = 0xdb;            /* DQT */
        *p++ = 0;               /* length msb */
        *p++ = 197;              /* length lsb */

        // 第一个
        *p++ = 0;
		unsigned char tmpLqt[64] = {0};
		MakeZigZag(lqt, tmpLqt);
        memcpy(p, tmpLqt, 64); //lint !e586
        p = p + 64;

		// 第二个
        *p++ = 1;
		unsigned char tmpCqt[64] = {0};
		MakeZigZag(cqt, tmpCqt);
        memcpy(p, tmpCqt, 64); //lint !e586
        p = p + 64;

		// 第三个
        *p++ = 2;
		unsigned char tmpLastqt[64] = {0};
		MakeZigZag(last_qt, tmpLastqt);
		memcpy(p, tmpLastqt, 64); //lint !e586
        p = p + 64;

        *p++ = 0xff;
        *p++ = 0xc0;           
        *p++ = 0;           
        *p++ = 17;          
        *p++ = 8;             
        *p++ = h >> 8;//lint !e702 !e734
        *p++ = h;     //lint !e734     
        *p++ = w >> 8;//lint !e702 !e734   
        *p++ = w;     //lint !e734      
        *p++ = 3;               
        *p++ = 1;               /* comp 0 */
        if ((type == 0) || (type == 64))
        {
            *p++ = 0x21;
        }                   /* hsamp = 2, vsamp = 1 */
        else
        {
            *p++ = 0x22;
        }                   /* hsamp = 2, vsamp = 2 */
        *p++ = 0;               /* quant table 0 */
        *p++ = 2;               /* comp 1 */
        *p++ = 0x11;            /* hsamp = 1, vsamp = 1 */
        *p++ = 1;               /* quant table 1 */
        *p++ = 3;               /* comp 2 */
        *p++ = 0x11;            /* hsamp = 1, vsamp = 1 */
        *p++ = 2;               /* quant table 1 */
    }

    if (NULL == last_qt)
    {
        p = MakeHuffmanHeader(p, lum_dc_codelens,
                              sizeof(lum_dc_codelens),
                              lum_dc_symbols,
                              sizeof(lum_dc_symbols), 0, 0);
        p = MakeHuffmanHeader(p, lum_ac_codelens,
                              sizeof(lum_ac_codelens),
                              lum_ac_symbols,
                              sizeof(lum_ac_symbols), 0, 1);
        p = MakeHuffmanHeader(p, chm_dc_codelens,
                              sizeof(chm_dc_codelens),
                              chm_dc_symbols,
                              sizeof(chm_dc_symbols), 1, 0);
        p = MakeHuffmanHeader(p, chm_ac_codelens,
                              sizeof(chm_ac_codelens),
                              chm_ac_symbols,
                              sizeof(chm_ac_symbols), 1, 1);
    }
    else
    {
        *p++ = 0xff;
        *p++ = 0xc4;            /* DHT */
        *p++ = 1;               /* length msb */
        *p++ = 0xA2; /* length lsb */
        *p++ = (0 << 4) | 0; //lint !e835
        memcpy(p, lum_dc_codelens, sizeof(lum_dc_codelens));
        p += sizeof(lum_dc_codelens);
        memcpy(p, lum_dc_symbols, sizeof(lum_dc_symbols));
        p += sizeof(lum_dc_symbols);

        *p++ = (1 << 4) | 0;
        memcpy(p, lum_ac_codelens, sizeof(lum_ac_codelens));
        p += sizeof(lum_ac_codelens);
        memcpy(p, lum_ac_symbols, sizeof(lum_ac_symbols));
        p += sizeof(lum_ac_symbols);

        *p++ = (0 << 4) | 1; //lint !e835
        memcpy(p, chm_dc_codelens, sizeof(chm_dc_codelens));
        p += sizeof(chm_dc_codelens);
        memcpy(p, chm_dc_symbols, sizeof(chm_dc_symbols));
        p += sizeof(chm_dc_symbols);

        *p++ = (1 << 4) | 1;
        memcpy(p, chm_ac_codelens, sizeof(chm_ac_codelens));
        p += sizeof(chm_ac_codelens);
        memcpy(p, chm_ac_symbols, sizeof(chm_ac_symbols));
        p += sizeof(chm_ac_symbols);
    }

    if (NULL != last_qt)
    {
        //海思的做法
        if (dri != 0)
        {
            p = MakeDRIHeader(p, dri);
        }

        *p++ = 0xff;
        *p++ = 0xda;            /* SOS */
        *p++ = 0;               /* length msb */
        *p++ = 12;              /* length lsb */
        *p++ = 3;               /* 3 components */
        *p++ = 1;               /* comp 0 */
        *p++ = 0;               /* Success table 0 */
        *p++ = 2;               /* comp 1 */
        *p++ = 0x11;            /* Success table 1 */
        *p++ = 3;               /* comp 2 */
        *p++ = 0x11;            /* Success table 1 */
        *p++ = 0;               /* first DCT coeff */
        *p++ = 63;              /* last DCT coeff */
        *p++ = 0;               /* Successive approx. */
    }
    else
    {
        *p++ = 0xff;
        *p++ = 0xda;            /* SOS */
        *p++ = 0;               /* length msb */
        *p++ = 12;              /* length lsb */
        *p++ = 3;               /* 3 components */
        *p++ = 0;               /* comp 0 */
        *p++ = 0;               /* Success table 0 */
        *p++ = 1;               /* comp 1 */
        *p++ = 0x11;            /* Success table 1 */
        *p++ = 2;               /* comp 2 */
        *p++ = 0x11;            /* Success table 1 */
        *p++ = 0;               /* first DCT coeff */
        *p++ = 63;              /* last DCT coeff */
        *p++ = 0;               /* Successive approx. */
    }

    return (p - start);
};

void CMjpegReassemble::MakeZigZag(const unsigned char *qt, unsigned char *outqt)
{
	for (int i = 0; i < 64; i++) 
	{
		outqt[i] = (unsigned char)qt[jpeg_natural_order[i]];
	}
}//lint !e1762


