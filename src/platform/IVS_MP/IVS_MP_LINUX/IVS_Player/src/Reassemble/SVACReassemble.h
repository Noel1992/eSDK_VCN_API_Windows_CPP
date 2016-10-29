/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  H264Reassemble.h
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
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
	//��ǿ�Ʒ��ͣ����������ŷ������϶�Ϊ����֡�ŷ�
	bool   SendCompleteFrame(RTP_FRAME_INFO* pstFrame);
	//ǿ�Ʒ���֡������֤���͵�֡��������
	void   SendFrame(RTP_FRAME_INFO* pstFrame);

private:
	int    InsertSingleNalu(RTP_PACK_INFO* pstRtpData);

	int    InsertFUA(RTP_PACK_INFO* pstRtpData);

	int    InsertNewFUA(RTP_PACK_INFO* pstRtpData);

	int    InsertExistFUA(RTP_PACK_INFO* pstRtpData);

	void   SendSingleNalu(RTP_FRAME_INFO* pFrame);

	int    SendFUA(RTP_FRAME_INFO* pFrame);

	//��Ⲣ���������¼����طš�ʱ�������
	void   CheckSpecialEvent(RTP_PACK_INFO* pstRtpData);

	//�鿴��ǰ�յ����Ƿ�ΪIDR֡�����Ҹ����Ƿ�Ϊ�ط�ģʽ�������Ƿ�ǿ�Ʒ��Ͷ�����������ݣ�����ط�ʱ����������
	void   CheckIDRAndSend(RTP_PACK_INFO* pstRtpData);

	//�鿴��ǰʱ����Ƿ�����Ծ�䣬��������ˣ���ǿ�Ʒ��Ͷ������������
	void   CheckTimeStampFail(unsigned long  ulTimeStamp);

    void   SetFrameByFu(RTP_FRAME_INFO* pFrame, const RTP_PACK_INFO* pstRtpData);

	void   ClearFrameInfo();


	//���ݵ�ǰ�Ĳ���ģʽѡ���Բ��ò�ͬ�ķ���֡��ʽ
	void   tryToSendFUA();

	//��֡�طŵ�ʱ����ã�����һ��GOP��֡����
	void   SendGopFrame();

	RTP_FRAME_INFO*  GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData);
};

#endif/*end __IVS_PLATER_H264_REASSEMBLE_H__*/
