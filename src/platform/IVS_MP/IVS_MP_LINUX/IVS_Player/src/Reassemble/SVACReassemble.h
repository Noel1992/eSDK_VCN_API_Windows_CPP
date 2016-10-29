/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  H264Reassemble.h
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
*******************************************************************************/

#ifndef  __IVS_PLATER_SVAC_REASSEMBLE_H__
#define  __IVS_PLATER_SVAC_REASSEMBLE_H__

#include "PacketReassemble.h" //lint !e537
#include <stdlib.h>





class CSvacReassemble : public CPacketReassemble
{
public:

    int     InsertPacket( char* pBuf, unsigned int ulBufLen);

    int     Init();

	int		ReSetReassemble();

	void     SetResetFlag(bool bIsReset);

	~CSvacReassemble();

protected:
	//不强制发送，符合条件才发，可认定为完整帧才发
	bool   SendCompleteFrame(RTP_FRAME_INFO* pstFrame);
	//强制发送帧，不保证发送的帧是完整的
	void   SendFrame(RTP_FRAME_INFO* pstFrame);

private:
	int    InsertSingleNalu(RTP_PACK_INFO* pstRtpData);

	int    InsertFUA(RTP_PACK_INFO* pstRtpData);

	int    InsertNewFUA(RTP_PACK_INFO* pstRtpData);

	int    InsertExistFUA(RTP_PACK_INFO* pstRtpData);

	void   SendSingleNalu(RTP_FRAME_INFO* pFrame);

	int    SendFUA(RTP_FRAME_INFO* pFrame);

	//检测并处理特殊事件，回放、时间戳跳变
	void   CheckSpecialEvent(RTP_PACK_INFO* pstRtpData);

	//查看当前收到的是否为IDR帧，并且根据是否为回放模式，决定是否强制发送队列里面的数据，解决回放时间戳序的问题
	void   CheckIDRAndSend(RTP_PACK_INFO* pstRtpData);

	//查看当前时间戳是否发生了跃变，如果发生了，则强制发送队列里面的数据
	void   CheckTimeStampFail(unsigned long  ulTimeStamp);

    void   SetFrameByFu(RTP_FRAME_INFO* pFrame, const RTP_PACK_INFO* pstRtpData);

	void   ClearFrameInfo();


	//根据当前的播放模式选择尝试采用不同的发送帧方式
	void   tryToSendFUA();

	//单帧回放的时候调用，发送一个GOP的帧数据
	void   SendGopFrame();

	RTP_FRAME_INFO*  GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData);
};

#endif/*end __IVS_PLATER_H264_REASSEMBLE_H__*/
