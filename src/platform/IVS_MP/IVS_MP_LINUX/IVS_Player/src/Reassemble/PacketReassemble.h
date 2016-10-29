/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  PacketReassemble.h
//  �ļ�˵��:	��֡�����ඨ��
//  ��    ��:   IVS V100R002C01
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#ifndef  __IVS_PLATER_PACKET_REASSEMBLE_H__
#define  __IVS_PLATER_PACKET_REASSEMBLE_H__
#ifdef WIN32
#include <WinSock2.h>
#include "..\..\inc\common\Lock.h"
#else
#include <netinet/in.h>
#include "Lock.h"
#endif

#include <list>
#include "IVSPlayerDataType.h"
#include "RtpDataType.h"
#include "PacketMemoryPoolMgr.h"
#include "ivs_timer.h"
#include "DecryptData.h"



#define SEQ_AFTER(seq_a, seq_b) ((short)((short)(seq_b) - (short)(seq_a)) < 0)  //lint !e1960

typedef struct
{
	unsigned short usSeqNum;
	unsigned long  ulTimeStamp;
	unsigned char  ucMark;
	unsigned long  ulHdrLen;
	unsigned char  fuType;
	unsigned char  nalu;

	unsigned char  hevc_nal_type;

	unsigned char  pt;
	long           lPayloadLen;
	unsigned int   uWaterMark;
	unsigned short usLostFrame; //�Ƿ񶪰���0û�ж�����1��������ˮӡʱ��Ч
	unsigned long long ullTick;
	int            bJumbo;
	char*          pPayload;
} RTP_PACK_INFO;

typedef  std::list <RTP_PACK_INFO*>   RTP_PACK_LIST;
typedef RTP_PACK_LIST::iterator RTP_PACK_LIST_ITER;
typedef RTP_PACK_LIST::reverse_iterator RTP_PACK_LIST_REV_ITER;



typedef struct
{
    bool          bS;			//��ʼ��־�Ƿ��յ�
    bool          bAll;			//���ж��յ�
    bool          bE;			//������־�Ƿ��յ�
    unsigned long ulTimeStamp;  //��ǰ����ʱ���
	unsigned long long ullTick;
    unsigned char fuType;		//��ǰ֡�����ͣ�sps,pps,IRD��������ͬ��ʱ���
	unsigned char nalu;

	unsigned char hevc_rtp_payload_type;
	unsigned char hevc_nal_type;

    RTP_PACK_LIST pPackList;
	unsigned int  uWaterMarkValue; //ˮӡֵ
	unsigned short usLostFrame; //�Ƿ񶪰���0û�ж�����1��������ˮӡʱ��Ч
}RTP_FRAME_INFO;

//ListĬ������ʱ����Ĵ�С���� ��->С
typedef std::list <RTP_FRAME_INFO *> RTP_FRAME_LIST;

typedef RTP_FRAME_LIST::iterator RTP_FRAME_LIST_ITER;

typedef RTP_FRAME_LIST::reverse_iterator RTP_FRAME_LIST_REV_ITER;


//�ؼ�֡����״̬
typedef enum
{
    //û�з���
    UNSEND = 0,
    //�Ѿ�����
    HADSEND = 1
} E_KEY_FRAME_STATE;

typedef enum
{
	H264REASSEMBLE = 0,
	MJPEGREASSEMBLE = 1
}E_REASSEMBLE_CLASS_TYPE;


typedef struct 
{
    char            pt;              //PAY_LOAD_TYPE
    unsigned long   ulTimeStamp;     //ʱ���
    bool            bKeyFrame;       //�Ƿ��ǹؼ�֡
    unsigned short  usFirstSeq;      //ÿ��֡��һ���������
    unsigned long   ulFrameType;     //֡����
    char*           pBuf;            //��������ַ
    long   ulBufLen;        //���ݳ���
}RTP_FRAME_DATA;


#define RTP_MIN_FRAME_CACHE 3      //���ٱ���3֡����(��ʱ100ms)

#define RTP_MAX_FRAME_CACHE 8      //��ౣ��8֡����(��ʱ240ms)

#define FRAME_MUST_SEND 1          //�����֡���뷢��

#define RTP_EXT_HW_PROFILE "HW"    //��Ϊˮӡ�������б�ʾ

#define FRAME_BUFF_LENTH_SD (1024* 300)   //����Ĭ�Ϸ���300K Byte

#define FRAME_BUFF_LENTH_HD (1024* 1500)  //����Ĭ�Ϸ���1500K Byte

#define DEFAULT_TS_INC_NUM 10

#define DEFAULT_H264STREAM_SAMPLE_RATE 90000

#define DEFAULT_AUDIO_SAMPLE_RATE 8000

#define DEFAULT_AUDIO_RATE 64000

#define SING_PACKET_SIZE 1 //��������

#define UNLOSE_PACKET_NUM 0 //�޶������

#define RTP_PACK_LENTH 1600

#define DECRYPT_BUF_LENGTH 16000

#define MAX_GOP_FRAME_SIZE		65		//���GOP��֡����60֡��,���5֡��������֡���������͵Ļ���������


class CPacketReassemble
{
public:
    CPacketReassemble();

    virtual ~CPacketReassemble();


    virtual  int	InsertPacket( char* pBuf, unsigned int uBufLen) = 0;

	virtual  int    Init() = 0;

    //���϶���������ʱ����ã������֡��ʱռ�õ��ڴ�
    virtual  int	ReSetReassemble() = 0;

	virtual  void    SetResetFlag(bool bIsReset) = 0;

	void            SetPlayBackMode(int bIsPlayBackMode){ m_iPlayBackMode = bIsPlayBackMode; } 

	//�ṩ��̬����֡�����С
	void			SetFrameCacheLimit(unsigned int iFrameCache);

	int             SetWaterMarkState(int nWaterMarkState);

	int             GetWaterMarkState() const{return m_nWaterMarkState;};

	int             SetCallbackFun(REASSM_CALLBACK cbFunc,void *pUser);

	void            SetExceptionCallback(PLAYER_EXCEPTION_CALLBACK pExceptionCallback, void *pUser);

	unsigned int    GetVideoCodeRate() const{ return m_uVideoCodeRate;};
	unsigned int    GetAvgVideoCodeRate();

	unsigned int    GetAudioCodeRate() const{ return m_uAudioCodeRate;};
	unsigned int    GetAvgAudioCodeRate();

	unsigned int    GetFPS() const{ return m_uiFPS;};
	float           GetLostPacketRate();

	unsigned int    GetUsedMemBlockNum() const{return (unsigned int)m_packetPool.GetUsedMemBlockNum();};

	unsigned int    GetFrameListSize()const {return m_FrameList.size();};

	int             SetDecryptDataKeyAndMode(const char* pSecretKey,unsigned long ulKeyLen, int iSecretKeyType);

	int             SetAudioDecryptDataKey(const char* pSecretKey,unsigned long ulKeyLen, int iSecretKeyType);

	void            StopDecryptVideoData();

	void            StopDecryptAudioData();

	void            SetMediaAttr(const MEDIA_ATTR *pVideoAttr, const MEDIA_ATTR *pAudioAttr);

protected:

	//��ǿ�Ʒ��ͣ����������ŷ������϶�Ϊ����֡�ŷ�
	virtual bool    SendCompleteFrame(RTP_FRAME_INFO* pstFrame) = 0 ;
	//ǿ�Ʒ���֡������֤���͵�֡��������
	virtual void    SendFrame(RTP_FRAME_INFO* pstFrame) = 0;
	
	void            SendLastListFrame(int nSend = 0);

	void            SendAudioFrame(RTP_PACK_INFO* pstRtpData);

    long			GetRtpHdrLen( char* pBuf, unsigned long ulBufLen, unsigned long & ulRtpHdrLen);

	void            GetExtensionHead(char* pBuf, unsigned long ulBufLen,unsigned long& ulRtpHdrLen,
											unsigned long long& ullTick,unsigned short& usLostFrame, unsigned int& uiWaterMarkValue);

    void			CheckFrameSeq(RTP_FRAME_INFO* pstFrame);

    void			FreePacket(RTP_PACK_INFO *& pstRtpData);

    int				GetRtpPacketInfo(char* pBuf, unsigned long ulBufLen, RTP_PACK_INFO *& pstRtpData);

    //����ʱ�����ȡ֡����
    RTP_FRAME_INFO*	GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData,E_REASSEMBLE_CLASS_TYPE eReassmbleType);

    //��֡���ݼ���֡������
    int 			InsertFrameToCache(RTP_FRAME_INFO* pstFrame);

	//����������ɾ������һ֡
	bool             FrameListIsFullAndSend(const RTP_FRAME_INFO* pstFrame);

	//���ݼ���֡������
	void            InsertFrameToList(RTP_FRAME_INFO* pstFrame);

	//������Ƶ����
	void            CaculateVideoCodeRate(unsigned long ulLength, unsigned long ulTimeStamp);

	//������Ƶ����
	void            CaculateAudioCodeRate(unsigned int uAudioType, unsigned long ulLength, unsigned long ulTimeStamp);

	//����֡��
	void            CaculateFPS(unsigned long ulTimeStamp);

	//���㶪����
	void            CaculateLostPacketRate(unsigned short  usCurrRevPacketNum,unsigned short  usCurrLostPacketNum);

	//����Ӧ���յ��İ���Ŀ�Լ�ʵ���յ��İ���
	void            CaculatePacketNum(unsigned short*  usShouldRevPacketNum,unsigned short*  usRevPacketNum);

	//��ʼ����Ա����
	void            InitPara();

	//����RTP����ռ��Դ
	void            ReleaseRtpInfo(RTP_FRAME_INFO* pstFrame);

	//����Rtp������
	void            DecryptRtpPacketData(char* pEncryptData,unsigned long ulEncryptDataLen,unsigned char palyLoadType);

    //�����MU��CU�Ķ�������
    void            CaculateMU2CULostPacketNum(unsigned short usPacketSeqNum);

protected:
	//��֡���ݼ��뵥֡�طŻ�����
	int 			InsertFrameToPlayBlackCache(RTP_FRAME_INFO* pstFrame,unsigned int uiGopNumLimit);

	//���ݼ���֡������
	void            InsertFrameToPlayBlackList(RTP_FRAME_INFO* pstFrame);

private:
	//���֡��������
	void            ClearFrameList();
public:
    //CIVSMutex               m_DestroyMutex; //���������Ͳ��֮�����      
protected:
	//����ʱ�򣺴�->С����֡�طţ�С->��
    RTP_FRAME_LIST m_FrameList;             //֡list

	unsigned long  m_preSendFrameTimeStamp; //��һ�η���ʱ���

    //�ڴ�ض���
    PacketPool m_packetPool;

	// ��֡�ڴ��
	PacketPool *m_pMemPoolJumbo;

    //֡����
    char* m_pFrameBuf;

    //���Ļ���
    unsigned long m_ulMaxBufLen;

	//����16�ֽڶ���Ķ���������
	unsigned int m_uiEncryptExLen;

    //����ģʽ���طš�������
    int m_iPlayBackMode;

	//�Ƿ�������Ƶ��������
	bool m_bDecryptData;
	//��Ƶ��Կ
	char* m_pSecretKey;
	//��Կ����
	unsigned long  m_ulKeyLen;
	//���ܶ�������
	int m_iDecryptDataType;

	//�Ƿ�������Ƶ��������
	bool m_bAudioDecryptData;
	//��Ƶ��Կ
	char* m_pAudioSecretKey;
	//��Կ����
	unsigned long  m_ulAudioKeyLen;
	//��Ƶ���ܶ�������
	int m_iAudioDecryptDataType;
	//AES������Ҫ�Ļ������ݿռ�
	unsigned char* m_pDecryptDataBuf;

	//����AESȫ����������Կ
	aes_context m_stAesDecryptKey; //lint !e601
    //��������С
    unsigned int m_ulFrameCacheLimit;

	//��Ƶ����
	unsigned int m_uVideoCodeRate;
	unsigned long long m_ulVideoByteCount;
	unsigned long long m_ulRecVideoByteCount;
	unsigned long long m_ulbeginVideoFrameTime;
	unsigned long long m_ulAvgBeginVideoFrameTime;
	unsigned long m_ulLastVideoFrameTime;
	unsigned int  m_uAvgVideoCodeRate;

	//��Ƶ����
	unsigned int m_uAudioCodeRate;
	unsigned long long m_ulAudioByteCount;
	unsigned long long m_ulRecAudioByteCount;
	unsigned long long m_ulbeginAudioFrameTime;
	unsigned long long m_ulAvgBeginAudioFrameTime;
	unsigned long m_ulLastAudioFrameTime;
	unsigned int  m_uAvgAudioCodeRate;
	
    bool     m_bIsStepReset;

	//֡��
	unsigned int m_uiFPS;
	unsigned long m_ulVideoBeginTimeStamp;
	unsigned long m_ulAudioBeginTimeStamp;
    unsigned long m_ulVideoTimeStampInc;
    unsigned long m_ulAudioTimeStampInc;

	unsigned long m_ulTimeStamp;

	unsigned long m_ulTSIncSum;

	unsigned long m_ulTSIncSunNum;

	//Ӧ���յ���ȫ������Ŀ
	unsigned long m_ulShouldBeRecvPacketNum;

	unsigned long m_uRevPacketNum;

	unsigned long m_ulOverrRecvPacketNum;

	unsigned long m_uLostPacketNum;

	int           m_nWaterMarkState;
	//���ڼ���ƽ��������ʱ��
	unsigned long long m_ulLostPacketBeginTime;
	float m_fLosttPacketRate;


	//�����С�����
	unsigned short m_usMaxSeqNum;

	unsigned short m_usMinSeqNum;

	// GOP ���к�
	unsigned int   m_uGopSeq;

	//ͼ����
	unsigned int   m_uiWidth;
	unsigned int   m_uiHeight;

	CIVSMutex      m_lock; //lint !e601

	bool           m_bStopInserFlag;
    //�ص�����
    REASSM_CALLBACK m_pReassmCallBack;
	void* m_pUser;
	
	//�쳣�ص�;
    PLAYER_EXCEPTION_CALLBACK m_pExceptionCallback;
	void *m_pExcUser;


    //rtp ý������;
	MEDIA_ATTR   m_VideoAttr;
	MEDIA_ATTR   m_AudioAttr;

    bool m_bIsFirstGetVideoRate;      //�Ƿ��һ�λ�ȡ��Ƶ����;
    bool m_bIsFirstGetAudioRate;      //�Ƿ��һ�λ�ȡ��Ƶ����;
   
    unsigned long m_ulRecFromMUNums;    //Ӧ�ô�MU�յ��İ�����
    unsigned long m_ulLostMUPacketNums; //��ʧ�İ�������;
    unsigned short m_usPacketSeqNum;    //���յ��İ����;

    bool m_bPCMUorPCMA;                 //��Ƶ���������Ƿ�ΪPCMU��PCMA;


    bool m_bDestroyAll;              //�Ѿ���������������һ�� 

};

#endif//__IVS_PLATER_PACKET_REASSEMBLE_H__
