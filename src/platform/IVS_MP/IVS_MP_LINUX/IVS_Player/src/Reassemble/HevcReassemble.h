#ifndef IVS_PLAYSER_HEVC_REASS_H
#define IVS_PLAYSER_HEVC_REASS_H

#include "PacketReassemble.h"





class CHevcReassemble : public CPacketReassemble
{
public:

	int     InsertPacket( char* pBuf, unsigned int ulBufLen);

	int     Init();

	int		ReSetReassemble();

	void     SetResetFlag(bool bIsReset);
	CHevcReassemble();
	~CHevcReassemble();

protected:
	//不强制发送，符合条件才发，可认定为完整帧才发
	bool   SendCompleteFrame(RTP_FRAME_INFO* pstFrame);
	//强制发送帧，不保证发送的帧是完整的
	void   SendFrame(RTP_FRAME_INFO* pstFrame);

private:
	int    InsertSingleNalu(RTP_PACK_INFO* pstRtpData);

	int    InsertFU(RTP_PACK_INFO* pstRtpData);

	int    InsertNewFU(RTP_PACK_INFO* pstRtpData);

	int    InsertExistFU(RTP_PACK_INFO* pstRtpData);

	void   SendSingleNalu(RTP_FRAME_INFO* pFrame);

	int    SendFU(RTP_FRAME_INFO* pFrame);

	//检测并处理特殊事件，回放、时间戳跳变
	void   CheckSpecialEvent(RTP_PACK_INFO* pstRtpData);

	//查看当前收到的是否为IDR帧，并且根据是否为回放模式，决定是否强制发送队列里面的数据，解决回放时间戳序的问题
	void   CheckIDRAndSend(RTP_PACK_INFO* pstRtpData);

	//查看当前时间戳是否发生了跃变，如果发生了，则强制发送队列里面的数据
	void   CheckTimeStampFail(unsigned long  ulTimeStamp);

	void   SetFrameByFu(RTP_FRAME_INFO* pFrame, const RTP_PACK_INFO* pstRtpData);

	void   ClearFrameInfo();

	void   GetPicHW(RTP_FRAME_INFO* pstFrame);

	//根据当前的播放模式选择尝试采用不同的发送帧方式
	void   tryToSendFU();

	//单帧回放的时候调用，发送一个GOP的帧数据
	void   SendGopFrame();

	RTP_FRAME_INFO*  GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData);

private:
	bool  m_using_donl_field;



};










#endif


