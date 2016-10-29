/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  PacketReassemble.h
//  文件说明:	组帧基类类定义
//  版    本:   IVS V100R002C01
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明：
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
	unsigned short usLostFrame; //是否丢包，0没有丢包，1丢包，无水印时无效
	unsigned long long ullTick;
	int            bJumbo;
	char*          pPayload;
} RTP_PACK_INFO;

typedef  std::list <RTP_PACK_INFO*>   RTP_PACK_LIST;
typedef RTP_PACK_LIST::iterator RTP_PACK_LIST_ITER;
typedef RTP_PACK_LIST::reverse_iterator RTP_PACK_LIST_REV_ITER;



typedef struct
{
    bool          bS;			//开始标志是否收到
    bool          bAll;			//所有都收到
    bool          bE;			//结束标志是否收到
    unsigned long ulTimeStamp;  //当前包的时间戳
	unsigned long long ullTick;
    unsigned char fuType;		//当前帧的类型，sps,pps,IRD，具有相同的时间戳
	unsigned char nalu;

	unsigned char hevc_rtp_payload_type;
	unsigned char hevc_nal_type;

    RTP_PACK_LIST pPackList;
	unsigned int  uWaterMarkValue; //水印值
	unsigned short usLostFrame; //是否丢包，0没有丢包，1丢包，无水印时无效
}RTP_FRAME_INFO;

//List默认排序按时间戳的大小排序， 大->小
typedef std::list <RTP_FRAME_INFO *> RTP_FRAME_LIST;

typedef RTP_FRAME_LIST::iterator RTP_FRAME_LIST_ITER;

typedef RTP_FRAME_LIST::reverse_iterator RTP_FRAME_LIST_REV_ITER;


//关键帧发送状态
typedef enum
{
    //没有发送
    UNSEND = 0,
    //已经发送
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
    unsigned long   ulTimeStamp;     //时间戳
    bool            bKeyFrame;       //是否是关键帧
    unsigned short  usFirstSeq;      //每个帧第一个包的序号
    unsigned long   ulFrameType;     //帧类型
    char*           pBuf;            //缓冲区地址
    long   ulBufLen;        //数据长度
}RTP_FRAME_DATA;


#define RTP_MIN_FRAME_CACHE 3      //最少保存3帧缓存(延时100ms)

#define RTP_MAX_FRAME_CACHE 8      //最多保存8帧缓存(延时240ms)

#define FRAME_MUST_SEND 1          //定义该帧必须发送

#define RTP_EXT_HW_PROFILE "HW"    //华为水印数据特有标示

#define FRAME_BUFF_LENTH_SD (1024* 300)   //标清默认分配300K Byte

#define FRAME_BUFF_LENTH_HD (1024* 1500)  //高清默认分配1500K Byte

#define DEFAULT_TS_INC_NUM 10

#define DEFAULT_H264STREAM_SAMPLE_RATE 90000

#define DEFAULT_AUDIO_SAMPLE_RATE 8000

#define DEFAULT_AUDIO_RATE 64000

#define SING_PACKET_SIZE 1 //单包包长

#define UNLOSE_PACKET_NUM 0 //无丢包情况

#define RTP_PACK_LENTH 1600

#define DECRYPT_BUF_LENGTH 16000

#define MAX_GOP_FRAME_SIZE		65		//最大GOP的帧数（60帧）,多加5帧数据用做帧数据乱序发送的缓存排序发送


class CPacketReassemble
{
public:
    CPacketReassemble();

    virtual ~CPacketReassemble();


    virtual  int	InsertPacket( char* pBuf, unsigned int uBufLen) = 0;

	virtual  int    Init() = 0;

    //当拖动进度条的时候调用，清除组帧的时占用的内存
    virtual  int	ReSetReassemble() = 0;

	virtual  void    SetResetFlag(bool bIsReset) = 0;

	void            SetPlayBackMode(int bIsPlayBackMode){ m_iPlayBackMode = bIsPlayBackMode; } 

	//提供动态设置帧缓存大小
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

	//不强制发送，符合条件才发，可认定为完整帧才发
	virtual bool    SendCompleteFrame(RTP_FRAME_INFO* pstFrame) = 0 ;
	//强制发送帧，不保证发送的帧是完整的
	virtual void    SendFrame(RTP_FRAME_INFO* pstFrame) = 0;
	
	void            SendLastListFrame(int nSend = 0);

	void            SendAudioFrame(RTP_PACK_INFO* pstRtpData);

    long			GetRtpHdrLen( char* pBuf, unsigned long ulBufLen, unsigned long & ulRtpHdrLen);

	void            GetExtensionHead(char* pBuf, unsigned long ulBufLen,unsigned long& ulRtpHdrLen,
											unsigned long long& ullTick,unsigned short& usLostFrame, unsigned int& uiWaterMarkValue);

    void			CheckFrameSeq(RTP_FRAME_INFO* pstFrame);

    void			FreePacket(RTP_PACK_INFO *& pstRtpData);

    int				GetRtpPacketInfo(char* pBuf, unsigned long ulBufLen, RTP_PACK_INFO *& pstRtpData);

    //根据时间戳获取帧数据
    RTP_FRAME_INFO*	GetFrameFromTimeTick(const RTP_PACK_INFO* pstRtpData,E_REASSEMBLE_CLASS_TYPE eReassmbleType);

    //将帧数据加入帧缓存区
    int 			InsertFrameToCache(RTP_FRAME_INFO* pstFrame);

	//缓存区满，删除其中一帧
	bool             FrameListIsFullAndSend(const RTP_FRAME_INFO* pstFrame);

	//数据加入帧缓存区
	void            InsertFrameToList(RTP_FRAME_INFO* pstFrame);

	//计算视频码率
	void            CaculateVideoCodeRate(unsigned long ulLength, unsigned long ulTimeStamp);

	//计算音频码率
	void            CaculateAudioCodeRate(unsigned int uAudioType, unsigned long ulLength, unsigned long ulTimeStamp);

	//计算帧率
	void            CaculateFPS(unsigned long ulTimeStamp);

	//计算丢包率
	void            CaculateLostPacketRate(unsigned short  usCurrRevPacketNum,unsigned short  usCurrLostPacketNum);

	//计算应该收到的包数目以及实际收到的包数
	void            CaculatePacketNum(unsigned short*  usShouldRevPacketNum,unsigned short*  usRevPacketNum);

	//初始化成员变量
	void            InitPara();

	//回收RTP包所占资源
	void            ReleaseRtpInfo(RTP_FRAME_INFO* pstFrame);

	//解密Rtp包数据
	void            DecryptRtpPacketData(char* pEncryptData,unsigned long ulEncryptDataLen,unsigned char palyLoadType);

    //计算从MU到CU的丢包数量
    void            CaculateMU2CULostPacketNum(unsigned short usPacketSeqNum);

protected:
	//将帧数据加入单帧回放缓存区
	int 			InsertFrameToPlayBlackCache(RTP_FRAME_INFO* pstFrame,unsigned int uiGopNumLimit);

	//数据加入帧缓存区
	void            InsertFrameToPlayBlackList(RTP_FRAME_INFO* pstFrame);

private:
	//清空帧队列数据
	void            ClearFrameList();
public:
    //CIVSMutex               m_DestroyMutex; //析构函数和插包之间的锁      
protected:
	//正常时序：大->小；单帧回放：小->大
    RTP_FRAME_LIST m_FrameList;             //帧list

	unsigned long  m_preSendFrameTimeStamp; //上一次发送时间戳

    //内存池对象
    PacketPool m_packetPool;

	// 巨帧内存池
	PacketPool *m_pMemPoolJumbo;

    //帧数据
    char* m_pFrameBuf;

    //最大的缓存
    unsigned long m_ulMaxBufLen;

	//补齐16字节对齐的而外数据量
	unsigned int m_uiEncryptExLen;

    //播放模式（回放、正常）
    int m_iPlayBackMode;

	//是否启动视频解密数据
	bool m_bDecryptData;
	//视频密钥
	char* m_pSecretKey;
	//密钥长度
	unsigned long  m_ulKeyLen;
	//解密对象类型
	int m_iDecryptDataType;

	//是否启动音频解密数据
	bool m_bAudioDecryptData;
	//音频密钥
	char* m_pAudioSecretKey;
	//密钥长度
	unsigned long  m_ulAudioKeyLen;
	//音频解密对象类型
	int m_iAudioDecryptDataType;
	//AES解密需要的缓存数据空间
	unsigned char* m_pDecryptDataBuf;

	//用于AES全码流解密密钥
	aes_context m_stAesDecryptKey; //lint !e601
    //缓冲区大小
    unsigned int m_ulFrameCacheLimit;

	//视频码率
	unsigned int m_uVideoCodeRate;
	unsigned long long m_ulVideoByteCount;
	unsigned long long m_ulRecVideoByteCount;
	unsigned long long m_ulbeginVideoFrameTime;
	unsigned long long m_ulAvgBeginVideoFrameTime;
	unsigned long m_ulLastVideoFrameTime;
	unsigned int  m_uAvgVideoCodeRate;

	//音频码率
	unsigned int m_uAudioCodeRate;
	unsigned long long m_ulAudioByteCount;
	unsigned long long m_ulRecAudioByteCount;
	unsigned long long m_ulbeginAudioFrameTime;
	unsigned long long m_ulAvgBeginAudioFrameTime;
	unsigned long m_ulLastAudioFrameTime;
	unsigned int  m_uAvgAudioCodeRate;
	
    bool     m_bIsStepReset;

	//帧率
	unsigned int m_uiFPS;
	unsigned long m_ulVideoBeginTimeStamp;
	unsigned long m_ulAudioBeginTimeStamp;
    unsigned long m_ulVideoTimeStampInc;
    unsigned long m_ulAudioTimeStampInc;

	unsigned long m_ulTimeStamp;

	unsigned long m_ulTSIncSum;

	unsigned long m_ulTSIncSunNum;

	//应该收到的全部包数目
	unsigned long m_ulShouldBeRecvPacketNum;

	unsigned long m_uRevPacketNum;

	unsigned long m_ulOverrRecvPacketNum;

	unsigned long m_uLostPacketNum;

	int           m_nWaterMarkState;
	//用于计算平均丢包率时间
	unsigned long long m_ulLostPacketBeginTime;
	float m_fLosttPacketRate;


	//最大最小包序号
	unsigned short m_usMaxSeqNum;

	unsigned short m_usMinSeqNum;

	// GOP 序列号
	unsigned int   m_uGopSeq;

	//图像宽高
	unsigned int   m_uiWidth;
	unsigned int   m_uiHeight;

	CIVSMutex      m_lock; //lint !e601

	bool           m_bStopInserFlag;
    //回调函数
    REASSM_CALLBACK m_pReassmCallBack;
	void* m_pUser;
	
	//异常回调;
    PLAYER_EXCEPTION_CALLBACK m_pExceptionCallback;
	void *m_pExcUser;


    //rtp 媒体属性;
	MEDIA_ATTR   m_VideoAttr;
	MEDIA_ATTR   m_AudioAttr;

    bool m_bIsFirstGetVideoRate;      //是否第一次获取视频码流;
    bool m_bIsFirstGetAudioRate;      //是否第一次获取音频码流;
   
    unsigned long m_ulRecFromMUNums;    //应该从MU收到的包数量
    unsigned long m_ulLostMUPacketNums; //丢失的包的数量;
    unsigned short m_usPacketSeqNum;    //接收到的包序号;

    bool m_bPCMUorPCMA;                 //音频编码类型是否为PCMU或PCMA;


    bool m_bDestroyAll;              //已经调析构函数析构一切 

};

#endif//__IVS_PLATER_PACKET_REASSEMBLE_H__
