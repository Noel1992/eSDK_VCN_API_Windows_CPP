/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  MjpegReassemble.h
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#ifndef  __IVS_PLATER_MJPEG_REASSEMBLE_H__
#define  __IVS_PLATER_MJPEG_REASSEMBLE_H__

#include "PacketReassemble.h" //lint !e537

class CMjpegReassemble : public CPacketReassemble
{
public:

    int InsertPacket( char* pBuf, unsigned int uBufLen);
	int Init();
	int ReSetReassemble();
	void SetResetFlag(bool bIsReset);
	~CMjpegReassemble();
protected:

	//��ǿ�Ʒ��ͣ����������ŷ������϶�Ϊ����֡�ŷ�
	bool SendCompleteFrame(RTP_FRAME_INFO* pstFrame);

	//ǿ�Ʒ���֡������֤���͵�֡��������
	void SendFrame(RTP_FRAME_INFO* pstFrame);
private:

	int  PreDecodeMjpegHeader(char* pBuff, unsigned long ulBufLen);

	int  InsertFragment(RTP_PACK_INFO* pstRtpData);

	int  InsertNewFragment(RTP_PACK_INFO* pstRtpData);

	int  InsertExitFragment(RTP_PACK_INFO* pstRtpData);

	int  MakeMjpegHeadAndCopyToBuf(RTP_FRAME_INFO* pstFrame,unsigned short& usQtable_len,int& iHeader_len,int& iRst_header_len);

	void MakeTables(int q, unsigned char *lqt, unsigned char *cqt);

	unsigned char* MakeQuantHeader(unsigned char *p, const unsigned char *qt, const int tableNo);
	void MakeZigZag(const unsigned char *qt, unsigned char *outqt);

	unsigned char* MakeDRIHeader(unsigned char *p, u_short dri);

	unsigned char* MakeHuffmanHeader(unsigned char *p, const unsigned char *codelens, int ncodes, const unsigned char *symbols, int nsymbols, int tableNo, int tableClass);

	int MakeHeaders(unsigned char *p, int type, int w, int h, const unsigned char *lqt, const unsigned char *cqt, u_short dri, const unsigned char *last_qt);

	void ClearFrameInfo();
private:
	int m_nQ;
	int m_nType;

	unsigned char* m_pLqt;
	unsigned char* m_pCqt;
	unsigned char* m_pLast_qt;
	unsigned char* m_pMjpeg_header; // ���ɵ�ɨ��ͷ
};

#endif//__IVS_PLATER_MJPEG_REASSEMBLE_H__
