/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  MjpegReassemble.h
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
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

	//不强制发送，符合条件才发，可认定为完整帧才发
	bool SendCompleteFrame(RTP_FRAME_INFO* pstFrame);

	//强制发送帧，不保证发送的帧是完整的
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
	unsigned char* m_pMjpeg_header; // 生成的扫描头
};

#endif//__IVS_PLATER_MJPEG_REASSEMBLE_H__
