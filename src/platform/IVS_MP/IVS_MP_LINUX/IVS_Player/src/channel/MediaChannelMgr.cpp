#include "MediaChannel.h"
#include "MediaChannelMgr.h"
#include "IVSCommon.h"
#include "MediaChannel.h"
#include "ivs_log.h"

#ifdef WIN32
CAudioChannel* CMediaChannelMgr::m_pAudioChannel = NULL;
#endif

CMediaChannelMgr::CMediaChannelMgr(void)
{
    Init();
}

CMediaChannelMgr::~CMediaChannelMgr(void)
{
    try
    {
        UnInit();
    }
    catch (...)
    {
    }
}

void CMediaChannelMgr::Init(void)
{
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr Init in");
    m_cbExceptionCallBack = NULL;
    m_pUserData = NULL;
    m_strRecvStreamIp = "";   //初始化RecvStreamIp

    m_cbEventCallBack = NULL;
    m_pEventUserData = NULL;

    m_uiLowerLimit = 10;
    m_uiUpperLimit = 80;
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr Init Out");
}

void CMediaChannelMgr::UnInit(void)
{
    CAutoLock lock(m_mutexChannel);
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr UnInit in");
    CMediaChannel* pMediaChannel = NULL;
    MediaChannelMapIter iter = m_mapMediaChannel.begin();
    for (;iter != m_mapMediaChannel.end();iter++)
    {
        pMediaChannel = dynamic_cast<CMediaChannel*>(iter->second);//lint !e611
        IVS_DELETE(pMediaChannel);
    }
    m_mapMediaChannel.clear();

    m_cbExceptionCallBack = NULL;
    m_pUserData = NULL;

#ifdef WIN32
    m_pAudioChannel = NULL;
#endif

    m_cbEventCallBack = NULL;
    m_pEventUserData = NULL;
    m_uiLowerLimit = 10;
    m_uiUpperLimit = 80;
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr UnInit Out");
}

//获取可用句柄
int CMediaChannelMgr::GetChannel(unsigned long& ulChannel)
{
    CAutoLock lock(m_mutexChannel);
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr GetChannel in");
    int iRet = IVS_PLAYER_RET_GET_CHANNEL_ERROR;
    unsigned int uiIndex = m_mapMediaChannel.size();

    // 超过最大限制;
    if (PLAYER_TOTAL_CHANNEL_NUM <= uiIndex)
    {
        IVS_LOG(IVS_LOG_ERR, "GetChannel", "Get channel over total number = %d", PLAYER_TOTAL_CHANNEL_NUM);
        return iRet;
    }

    //在MAP中创建通道资源信息对象，与通道号对应插入MAP中
    CMediaChannel* pMediaChannel = IVS_NEW((CMediaChannel*&)pMediaChannel);
    if (NULL == pMediaChannel)
    {
        IVS_LOG(IVS_LOG_ERR, "GetChannel", "Create media channel failed.");
        return IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    } 

    // 将Channel对象指针作为唯一标识;
    ulChannel = reinterpret_cast<unsigned long>(pMediaChannel);
    (void)m_mapMediaChannel.insert(std::make_pair(ulChannel, pMediaChannel)); //lint !e1025 !e119

    IVS_LOG(IVS_LOG_DEBUG, "GetChannel", "Media Channel Manager GetChannel end");
    return IVS_SUCCEED;
}

// 释放指定句柄;
int CMediaChannelMgr::FreeChannel(unsigned long ulChannel)
{
    int iRet = IVS_FAIL;
    CAutoLock lock(m_mutexChannel);
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr FreeChannel, Channel[%lu]", ulChannel);
    MediaChannelMapIter iter = m_mapMediaChannel.find(ulChannel);
    if (iter != m_mapMediaChannel.end())
    {
        CMediaChannel* pMediaChannel = dynamic_cast<CMediaChannel*>(iter->second);//lint !e611
        IVS_DELETE(pMediaChannel);
        m_mapMediaChannel.erase(iter);
		IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr UnInit success");
        iRet = IVS_SUCCEED;
    }
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr UnInit failed");
    return iRet;
}

CMediaChannel* CMediaChannelMgr::GetChannelbyID(unsigned long ulChannel)
{
    CAutoLock lock(m_mutexChannel);
	IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr GetChannelbyID, Channel[%lu]", ulChannel);
    CMediaChannel* pMediaChannel = NULL;
    MediaChannelMapIter iter = m_mapMediaChannel.find(ulChannel);
    if (iter != m_mapMediaChannel.end())
    {
        pMediaChannel = dynamic_cast<CMediaChannel*>(iter->second);//lint !e611
		IVS_LOG(IVS_LOG_DEBUG, "","MediaChannelMgr GetChannelbyID success");
    }
    return pMediaChannel;
}

void CMediaChannelMgr::SetExceptionCallBack(PLAYER_EXCEPTION_CALLBACK cbExceptionCallBack, void* pUser)
{
    CAutoLock lock(m_mutexExceptionCallback);
    m_cbExceptionCallBack = cbExceptionCallBack;
    m_pUserData = pUser;
}

void CMediaChannelMgr::DoExceptionCallBack(int iMsgType, void* pParam, void* pUser)
{
    if (NULL == m_cbExceptionCallBack)
    {
        IVS_LOG(IVS_LOG_ERR, "DoExceptionCallBack", "Order error, m_cbExceptionCallBack is NULL.");
        return;
    }
    CAutoLock lock(m_mutexExceptionCallback);
	unsigned long ulChannel = reinterpret_cast<unsigned long>(pUser);

	m_cbExceptionCallBack(ulChannel, iMsgType, pParam, m_pUserData);
}

#ifdef WIN32
/*****************************************************************************
 函数名称：CreateVoiceChannel()
 功能描述：创建语音对讲通道
 输入参数：ulPort 通道号
 输出参数：无
 返 回 值： CAudioChannel *
*****************************************************************************/
CAudioChannel * CMediaChannelMgr::CreateVoiceChannel() const
{
	IVS_LOG(IVS_LOG_DEBUG, "Create Voice Channel",
            "Media Channel manager Create Voice Channel begin.");

    // modify by wanglei 00165153:2013.6.15 
    // 不知道原因，为何关闭对讲的时候不进行DestroyVoiceChannel，为保险起见，在申请的时候进行判空处理，防止内存泄露
    if (NULL != m_pAudioChannel)
    {
        DestroyVoiceChannel();
    }

	try
	{
		m_pAudioChannel = new CAudioChannel(m_cbExceptionCallBack, m_pUserData);
	}
	catch(...)
	{
		m_pAudioChannel = NULL;
	}
	
    IVS_LOG(IVS_LOG_DEBUG, "Create Voice Channel",
            "Media Channel manager Create Voice Channel end.");
	return m_pAudioChannel;
}
#endif

void CMediaChannelMgr::SetEventCallBack(PLAYER_EVENT_CALLBACK cbEventCallBack, void* pUser)
{
    CAutoLock lock(m_mutexEventCallback);
    m_cbEventCallBack = cbEventCallBack;
    m_pEventUserData = pUser;
}
#ifdef WIN32
/*****************************************************************************
 函数名称：GetVoiceChannel()
 功能描述：获取语音对讲通道对象
 输入参数：无
 输出参数：无
 返 回 值： CAudioChannel *
*****************************************************************************/
CAudioChannel* CMediaChannelMgr::GetVoiceChannel() const
{
    return m_pAudioChannel;
}



/*****************************************************************************
 函数名称：ResetVoiceChannel()
 功能描述：重置语音对讲通道
 输入参数：无
 输出参数：无
 返 回 值： IVS_SUCCEED
*****************************************************************************/
int CMediaChannelMgr::ResetVoiceChannel() const
{
	IVS_LOG(IVS_LOG_DEBUG, "Reset Voice Channel",
            "Media Channel manager Reset Voice Channel begin");

    m_pAudioChannel->ReleaseChannel();

    IVS_LOG(IVS_LOG_DEBUG, "Reset Voice Channel",
            "Media Channel manager Reset Voice Channel begin");
	return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：DestroyVoiceChannel()
 功能描述：销毁语音对讲通道
 输入参数：无
 输出参数：无
 返 回 值： IVS_SUCCEED
*****************************************************************************/
void CMediaChannelMgr::DestroyVoiceChannel() const
{
	IVS_LOG(IVS_LOG_DEBUG, "Destroy Voice Channel",
            "Media channel manager destroy voice channel begin");

    //删除语音通道对象
	IVS_DELETE(m_pAudioChannel);

    IVS_LOG(IVS_LOG_DEBUG, "Destroy Voice Channel",
            "Media channel manager destroy voice channel end");
	return;
}

/*****************************************************************************
 函数名称：SetVoiceProtocolType()
 功能描述：设置语音对讲协议类型
 输入参数：ulProtocolType	 协议类型           
 输出参数：无
 返 回 值： void
*****************************************************************************/
void CMediaChannelMgr::SetVoiceProtocolType(unsigned int uVoiceProtocolType) const
{
	IVS_LOG(IVS_LOG_DEBUG, "Get Voice Protocol Type",
            "Media Channel manager Set Voice Protocol Type begin");

    CAudioChannel::m_uAudioProtocolType = uVoiceProtocolType;
    //m_pAudioChannel->m_uAudioProtocolType = ulVoiceProtocolType;

    IVS_LOG(IVS_LOG_DEBUG, "Get Voice Protocol Type",
            "Media Channel manager Set Voice Protocol Type end");
	return;
}

/*****************************************************************************
 函数名称：GetVoiceProtocolType()
 功能描述：获取语音对讲协议类型
 输入参数：无           
 输出参数：pVoiceProtocolType	 语音对讲协议类型
 返 回 值： IVS_SUCCEED
*****************************************************************************/
int CMediaChannelMgr::GetVoiceProtocolType(unsigned int* pVoiceProtocolType) const
{
	IVS_LOG(IVS_LOG_DEBUG, "Get Voice Protocol Type",
            "Media Channel manager Get Voice Protocol Type begin");

    //*pVoiceProtocolType = m_pAudioChannel->m_uAudioProtocolType;
    *pVoiceProtocolType = CAudioChannel::m_uAudioProtocolType;

    IVS_LOG(IVS_LOG_DEBUG, "Get Voice Protocol Type",
            "Media Channel manager Get Voice Protocol Type end");
	return IVS_SUCCEED;
}

/*****************************************************************************
 函数名称：SetVoiceDecoderType()
 功能描述：设置语音解码类型
 输入参数：ulVoiceDecoderType	 语音解码类型           
 输出参数：无
 返 回 值： void
*****************************************************************************/
void CMediaChannelMgr::SetVoiceDecoderType(unsigned int uVoiceDecoderType) const
{
	IVS_LOG(IVS_LOG_DEBUG, "Set Voice Decoder Type",
            "Media Channel manager Set Voice Decoder Type begin");

    CAudioChannel::m_uAudioDecoderType = uVoiceDecoderType;
    //m_pAudioChannel->m_uAudioDecoderType = ulVoiceDecoderType;

    IVS_LOG(IVS_LOG_DEBUG, "Set Voice Decoder Type",
            "Media Channel manager Set Voice Decoder Type end");
	return;
}

/*****************************************************************************
 函数名称：GetVoiceDecoderType()
 功能描述：获取语音解码类型
 输入参数：无          
 输出参数：pVoiceDecoderType  语音解码类型
 返 回 值： IVS_SUCCEED
*****************************************************************************/
int CMediaChannelMgr::GetVoiceDecoderType(unsigned int* pVoiceDecoderType) const
{
	//IVS_LOG(IVS_LOG_DEBUG, "Media Channel manager Get VoiceDecoder Type begin");

    //*pVoiceDecoderType = m_pAudioChannel->m_uAudioDecoderType;
    *pVoiceDecoderType = CAudioChannel::m_uAudioDecoderType;

    //IVS_LOG(IVS_LOG_DEBUG, "Media Channel manager Get VoiceDecoder Type end");
	return IVS_SUCCEED;
}


#endif

/*****************************************************************************
 函数名称：SetRecvStreamIp()
 功能描述：设置接收码流的IP
 输入参数：pRecvStreamIp 接收码流的IP           
 输出参数：无
 返 回 值： void
*****************************************************************************/
void CMediaChannelMgr::SetRecvStreamIp(char* pRecvStreamIp)
{
    m_strRecvStreamIp = pRecvStreamIp;
}  //lint !e818 暂不用const

/*****************************************************************************
 函数名称：GetRecvStreamIp()
 功能描述：获取接收码流的IP
 输入参数：无           
 输出参数：pRecvStreamIp 接收码流的IP
 返 回 值： void
*****************************************************************************/
int CMediaChannelMgr::GetRecvStreamIp(char*  pRecvStreamIp) const
{
	IVS_LOG(IVS_LOG_DEBUG, "Get Recv Stream Ip",
            "Media Channel manager Get Receive Stream Ip begin");

    strncpy(pRecvStreamIp, m_strRecvStreamIp.c_str(), m_strRecvStreamIp.length() + 1);

    IVS_LOG(IVS_LOG_DEBUG, "Get Recv Stream Ip",
            "Media Channel manager Get Receive Stream Ip end");
	return IVS_SUCCEED;
}

void CMediaChannelMgr::DoEventCallBack(int iMsgType, void* pParam, void* pUser)
{
    if (NULL == m_cbEventCallBack)
    {
        IVS_LOG(IVS_LOG_ERR, "DoEventCallBack", "Order error, m_cbEventCallBack is NULL.");
        return;
    }
    CAutoLock lock(m_mutexEventCallback);
    unsigned long ulChannel = reinterpret_cast<unsigned long>(pUser);

    m_cbEventCallBack(ulChannel, iMsgType, pParam, m_pEventUserData);
}



