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
	//��ǿ�Ʒ��ͣ����������ŷ������϶�Ϊ����֡�ŷ�
	bool   SendCompleteFrame(RTP_FRAME_INFO* pstFrame);
	//ǿ�Ʒ���֡������֤���͵�֡��������
	void   SendFrame(RTP_FRAME_INFO* pstFrame);

private:
	int    InsertSingleNalu(RTP_PACK_INFO* pstRtpData);

	int    InsertFU(RTP_PACK_INFO* pstRtpData);

	int    InsertNewFU(RTP_PACK_INFO* pstRtpData);

	int    InsertExistFU(RTP_PACK_INFO* pstRtpData);

	void   SendSingleNalu(RTP_FRAME_INFO* pFrame);

	int    SendFU(RTP_FRAME_INFO* pFrame);

	//��Ⲣ���������¼����طš�ʱ�������
	void   CheckSpecialEvent(RTP_PACK_INFO* pstRtpData);

	//�鿴��ǰ�յ����Ƿ�ΪIDR֡�����Ҹ����Ƿ�Ϊ�ط�ģʽ�������Ƿ�ǿ�Ʒ��Ͷ�����������ݣ�����ط�ʱ����������
	void   CheckIDRAndSend(RTP_PACK_INFO* pstRtpData);

	//�鿴��ǰʱ����Ƿ�����Ծ�䣬��������ˣ���ǿ�Ʒ��Ͷ������������
	void   CheckTimeStampFail(unsigned long  ulTimeStamp);

	void   SetFrameByFu(RTP_FRAME_INFO* pFrame, const RTP_PACK_INFO* pstRtpData);

	void   ClearFrameInfo();

	void   GetPicHW(RTP_FRAME_INFO* pstFrame);

	//���ݵ�ǰ�Ĳ���ģʽѡ���Բ��ò�ͬ�ķ���֡��ʽ
	void   tryToSendFU();

	//��֡�طŵ�ʱ����ã�����һ��GOP��֡����
	void   SendGopFrame();

	RTP_FRAME_INFO*  GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData);

private:
	bool  m_using_donl_field;



};










#endif


