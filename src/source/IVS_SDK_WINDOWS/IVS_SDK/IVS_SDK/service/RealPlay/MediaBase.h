/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/********************************************************************
 filename    :    MediaBase.h
 author      :    s00191067
 created     :    2012/11/19
 description :    媒体基类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*********************************************************************/
#ifndef __MEDIA_BASE_H__
#define __MEDIA_BASE_H__

/*!< 工具类头文件 */
#include "UserInfo.h"
#include "rtsp_client_api.h"
#include "RtspRetCode.h"
#include "IVS_NetSource.h"
#include "IVSPlayerDataType.h"
#include "ParseSdp.h"
#include "bp_environments.h"


enum SourcePlayType
{
	PLAY_TYPE_HWND = 0,         //渲染到句柄;
	PLAY_TYPE_CB,               //获取拼帧后的流
	PLAY_TYPE_YUV               //获取解码后的流
};

typedef enum
{
	MEDIA_STATUS_BUSY = 0,
	MEDIA_STATUS_FREE
}MEDIA_STATUS;

typedef struct tagStartPlayParam
{
    SOURCE_TYPE         enSourceType;
    IVS_SERVICE_TYPE    enServiceType;
    float               fSpeed;
    fDataCallBack       cbRtspDataCallBack; //lint !e601 !e10
    NET_DATA_CALLBACK   cbNetDataCallBack;
    bool                bBroadcast;         //是否为广播

    tagStartPlayParam()
        : enSourceType(SOURCE_RTP)
        , enServiceType(SERVICE_TYPE_REALVIDEO)
        , fSpeed(1.0)// 默认以1倍速的方式发流;
        , cbRtspDataCallBack(NULL) //lint !e40 !e10 !e1927
        , cbNetDataCallBack(NULL)
        , bBroadcast(false)
    {}//lint !e1401
}START_PLAY_PARAM;

class CUserMgr;
class CMediaBase
{
public:
	CMediaBase(void);
	virtual ~CMediaBase(void);

public:
    virtual void RetSet();

/**************************************************************************
    * name       : SetWaterMarkVerify
	* description: 设置开启/停止校验水印
	* input      : ulHandle          业务句柄，包括实况、回放
				   bIsStart          1：开启水印 0：停止水印
	* return     : 成功返回0，失败返回负数错误码
    * author     ：chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 SetWaterMarkVerify(IVS_UINT32 uiChannel, IVS_BOOL bIsStart) const;

/**************************************************************************
    * name       : SetPlayQualityMode
	* description: 设置实况播放模式：清晰、流畅
	* input      : ulHandle          业务句柄，包括实况、回放
	               iPlayQualityMode  播放模式 参考：IVS_PLAY_QUALITY_MODE
	* return     : 成功返回0，失败返回负数错误码
    * author     ：chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 SetPlayQualityMode(IVS_UINT32 uiChannel,IVS_INT32 iPlayQualityMode) const;

 	/**************************************************************************
    * name       : GetMediaInfo
    * description: 获取视频质量参数
    * input      : ulHandle          业务句柄，包括实况、回放
    * output     : pMediaInfo        媒体信息
    * return     : 成功返回0；失败返回负数错误码
    * remark     : NA
    **************************************************************************/
	IVS_INT32 GetMediaInfo(IVS_UINT32 uiChannel,IVS_MEDIA_INFO* pMediaInfo) const;

/**************************************************************************
    * name       : GetPlayedTime
    * description: 获取已播放时间。
	* input      : ulHandle           业务句柄，包括实况、回放
	* output     : puPlayedTime       当前播放时间
	* return     : 成功返回0，失败返回负数错误码
    * author     ：chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 GetPlayedTime(IVS_UINT32 uiChannel,IVS_UINT32 *puPlayTime) const;

/**************************************************************************
    * name       : SetDisplayScale
    * description: 设置播放显示比率。
	* input      : uiChannel           信道
	* output     : iDisplayScale       显示比率
	* return     : 成功返回0，失败返回负数错误码
    * author     ：chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 SetDisplayScale(IVS_UINT32 uiChannel, IVS_UINT32 iDisplayScale) const;

	void SetUserMgr(CUserMgr* pUserMgr);

	IVS_INT32 GetStatus();
	void SetStatus(IVS_INT32 iStatus);

	IVS_UINT64 GetStartTick(void) const {return m_uiStartTick;}
	time_t GetStartTime(void) const {return m_tmStart;}

    /*****************************************************************************
     函数名称：IsReuseRtspConnection
     功能描述：根据服务类型和协议类型判断是否复用rtsp连接;
     输入参数：enServiceType  服务类型;
               enProtocolType 协议类型;
     输出参数：NA
     返 回 值：true false;
    *****************************************************************************/
    bool IsReuseRtspConnection(IVS_SERVICE_TYPE enServiceType, IVS_PROTOCOL_TYPE enProtocolType, 
                               IVS_MEDIA_TRANS_MODE enTransMode, IVS_BROADCAST_TYPE enBroadcastType);

    /******************************************************************
     function   : StartPlay
     description: 启动播放;
     input      : const START_PLAY_PARAM & stParam
     output     : NA
     return     : int
    *******************************************************************/
    virtual int StartPlay(const START_PLAY_PARAM& stParam);

	virtual int StartDownload(const START_PLAY_PARAM& stParam, const char *pCam, const IVS_DOWNLOAD_PARAM *pParam);


    /******************************************************************
     function   : ReStartStream
     description: 实况开启流复用;
     input      : const START_PLAY_PARAM & stParam
     output     : NA
     return     : int
    *******************************************************************/
	virtual int ReStartStream(const START_PLAY_PARAM& stParam);

    /******************************************************************
     function   : StopStream
     description: 实况流停止
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	virtual int StopStream();

   /******************************************************************
     function   : StopPlay
     description: 实况停止
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/ 
    virtual int StopPlay();

	/******************************************************************
     function   : StopDownLoad
     description: 停止下载
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/ 
	virtual int StopDownLoad();

    /******************************************************************
     function   : PlaySound
     description: 开启音频播放
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int PlaySound();


    /******************************************************************
     function   : StopSound
     description: 关闭音频播放
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int StopSound();

    /******************************************************************
     function   : SetVolume
     description: 设置音量
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int SetVolume(unsigned int uiVolumeValue);

    /******************************************************************
     function   : GetVolume
     description: 获取音量
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int GetVolume(unsigned int* puiVolumeValue);

    /******************************************************************
     function   : NotifyStreamBufferState
     description: 通知流缓冲区状态
     input      : IVS_INT32 iBufferState
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    virtual IVS_INT32 NotifyStreamBufferState(IVS_INT32 iBufferState);

    /******************************************************************
     function   : StartLocalRecord
     description: 开始本地录像
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	int StartLocalRecord(const IVS_LOCAL_RECORD_PARAM* pRecordParam,const IVS_CHAR* pSaveFileName);

    /******************************************************************
     function   : StopLocalRecord
     description: 停止本地录像
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	int StopLocalRecord();

    /******************************************************************
     function   : LocalSnapshot
     description: 本地抓拍单张图片
	 input      : pSavePath			图片保存路径；
				  ulSnapshotFormat	图片保存类型
     output     : NA
     return     : int
    *******************************************************************/
	int LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat);
	int LocalSnapshotRect(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT* pRect);


    /******************************************************************
     function   : SetDrawCallBack
     description: 设置画图回调
     input      : callback;pUserData
     output     : NA
     return     : int
    *******************************************************************/
	int SetDrawCallBack(DRAW_PROC callback, void *pUserData);
	int SetDrawCallBackEx(HWND hWnd, DRAW_PROC callback, void *pUserData);

	int SetDelayFrameNum(IVS_UINT32 uDelayFrameNum);
	int GetCameraName(IVS_ULONG ulNameLen, char *pCamName);


    /******************************************************************
     function   : UTCStr2Time
     description: 将UTC时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,
                  即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数。
     input      : const char * pStr     待转换的字符串
     input      : const char * pFormat  字符串对应的格式;
     output     : NA
     return     : time_t 转换后的时间
    *******************************************************************/
    static time_t UTCStr2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : UTCStr2Span
     description: 将时间起止时间字符串转换为协议格式,如：
									20160214155505, 20160214160505--> 20160214T155505Z-20160214T160505Z
     input      : stStartTime     起始时间
     input      : stEndTime   终止时间
     output     : NA
     return     : 
    *******************************************************************/
    static void UTCStr2Span(std::string& strTime, const char* stStartTime, const char* stEndTime);

	/******************************************************************
     function   : UTCTime2Str
     description: 将UTC时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,
                  即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数。
     input      : const char * pStr     待转换的字符串
     input      : const char * pFormat  字符串对应的格式;
     output     : NA
     return     : time_t 转换后的时间
    *******************************************************************/
    static void UTCTime2Str(const time_t t, std::string& strTime, const std::string& strFormat);

	/******************************************************************
	 function   : GetRtspErrorCode
	 description: 将rtsp client的错误码转换为ivs错误码;
	 input      : int iErrorCode
	 output     : NA
	 return     : int 错误码;
	*******************************************************************/
	static int GetRtspErrorCode(int iErrorCode);

	/******************************************************************
	function	: SetSecretKey
	description : 设置密钥信息;
	input		: uiChannel   业务句柄
				  stMediaKeyInfo   加密信息
	output		: NA
	return		: 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 SetSecretKey(IVS_ULONG uiChannel, const MEDIA_KEY_INFO& stMediaKeyInfo);

    IVS_INT32 SetNoNeedSecretKey(IVS_ULONG uiChannel, const MEDIA_KEY_INFO& stMediaKeyInfo);

	/******************************************************************
	function	: GetStreamencryKey
	description : 请求录像加密的工作密钥或者异或因子
	input		: stMediaKeyInfo 媒体加密信息
	output		: strSecretKey 密钥 
	return		: 成功-0；失败-1
	*******************************************************************/
	IVS_INT32 GetStreamencryKey(IVS_CHAR* cSecretKey,const MEDIA_KEY_INFO& stMediaKeyInfo) const;

	/******************************************************************
	 function   : InitMediaPara
	 description: 初始化MediaPara
	 input      : IVS_MEDIA_PARA & stMediaPara
	 output     : NA
	 return     : void
	*******************************************************************/
	void InitMediaPara(IVS_MEDIA_PARA& stMediaPara) const;

	/******************************************************************
	 function   : GetSourceBufferRemain
	 description: 获取未解码数据的个数;
	 input      : IVS_UINT32 & uBufferCount
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetSourceBufferRemain(IVS_UINT32 &uBufferCount);





	IVS_INT32 SetMeidaAttribute(const MEDIA_ATTR &VideoAttr, const MEDIA_ATTR &AudioAttr);//lint !e601







private:

    /******************************************************************
     function   : ProcessRtspPlayMsg
     description: 处理rtsp的play流程;
     input      : const char * pszURL
     input      : const MEDIA_ADDR & stServerMediaAddr
     input      : const START_PLAY_PARAM & stParam
     output     : NA
     return     : int
    *******************************************************************/
    int ProcessRtspPlayMsg(const char* pszURL, const MEDIA_ADDR& /*stServerMediaAddr*/, const START_PLAY_PARAM& stParam); //lint !e49 !e10 !e808

	int ProcessRtspPlayMsgDownLoad(const char* pszURL, const MEDIA_ADDR& /*stServerMediaAddr*/, const START_PLAY_PARAM& stParam,  //lint !e10 !e808
		           const char *pCam, const IVS_DOWNLOAD_PARAM *pParam); //lint !e49

    /******************************************************************
     function   : Str2Time
     description: 将时间串格式以YYYY,MM,DD,hh,mm,ss分段转换，输出以秒为单位的日历时间,
                  即自国际标准时间公元1970年1月1日00:00:00以来经过的秒数。
     input      : const char * pStr
     input      : const char * pFormat
     output     : NA
     return     : time_t
    *******************************************************************/
    static time_t Str2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : Time2Str
	 description: 将日历时间（自国际标准时间公元1970年1月1日00:00:00以来经过的秒数）
	 *              转换为形如"20090630091030"的字符串
	 input      : char *pDestBuf，容纳时间字符串缓冲区指针（值结果参数）；int nbuflen，容纳时间字符串缓冲区大小；
	              const time_t *calptr，自国际标准时间公元1970年1月1日00:00:00以来经过的秒数
     output     : 返回指针pDestBuf
     return     : time_t
    *******************************************************************/
    static void Time2Str(const time_t t, std::string& strTime, const std::string& strFormat);

public:
	/***************************************************************************
		函 数 名 : AddrUint2Char
		功能描述 : 地址转换函数，整型-》字符串。
		输入参数 : name                    IP地址整型
		输出参数 : addr                    IP地址字符串
		返 回 值 : RET_OK -成功       RET_FAIL -失败
	***************************************************************************/
	static int AddrUint2Char(const unsigned int name, std::string &addr);
	 /***************************************************************************
     函 数 名 : GetURL
     功能描述 : 从SMU/SCU获取媒体URL
     输入参数 : stUrlMediaInfo，url媒体信息；stMediaRsp，响应的媒体信息
     输出参数 : xml                    请求xml
     返 回 值 : 0 -成功 ;1-失败
    ***************************************************************************/
	static IVS_INT32 GetURL(const char * /*pszAuthHeader*/, CUserMgr *pUserMgr, 
							ST_URL_MEDIA_INFO& stUrlMediaInfo, 
							ST_MEDIA_RSP& stMediaRsp, bool& bAssociatedAudio, bool bPlaybackorDownload = false);

	IVS_INT32 GetLocalIpPort();

    unsigned long GetPlayerChannel() const { return m_ulPlayerChannel; }

	IVS_ULONG GetRtspHandle() const { return m_iRtspHandle; }

	IVS_ULONG GetNetSourceHandle() const { return m_ulNetSourceChannel; }

	const char *GetUrlBuffer()const { return m_stMediaRsp.szURL; } 

	bool  IsStepPlay() const {return m_bStepPlay;}

	bool  IsStepPlayLastFrame() const {return m_bStepPlayIsLastFrame;}

	void  GetCameraID(std::string& strCameraID)const {strCameraID = m_szCameraID;}

	void  GetMediaSourceIP(std::string& strMediaSourceIP)const 
    {
        if(m_stuMediaParam.TransMode)
        {    
            strMediaSourceIP = m_szRemoteIp;
        }
        else 
        {
            strMediaSourceIP = m_strMuIpAddr;
        }
    }

    void SetSecretFlag(bool bGetKeyIsSuccess);

    bool GetSecretFlag()const;

    void SetSessionInfo(char* pSessionUrl, char* pSessionId);

	IVS_INT32 GetAccessProtocolByCameraCode(const IVS_CHAR* pCameraCode);
protected:
    /******************************************************************
     function   : SetMediaParam
     description: 设置媒体参数;
     input      : MEDIA_PARA * pMediaPara
     output     : strValue 
     return     : int
    *******************************************************************/
    int SetMediaParam(const IVS_MEDIA_PARA* pMediaPara);

    /******************************************************************
     function   : ParseURL
     description: 解析URL,pKey是想要得到的值
     input      : const char * pUrl
     input      : const char * pKey
     input      : std::string & strValue
     output     : NA
     return     : int IVS_SUCCEED成功 IVS_FAIL失败;
    *******************************************************************/
    int ParseURL(const char* pUrl, const char* pKey, std::string& strValue);

	 /******************************************************************
     function   : ParseIPAddr
     description: 解析URL,获取MU所在的IP地址
     input      : const char * pUrl
     input      : std::string & strValue
     output     : NA
     return     : int IVS_SUCCEED成功 IVS_FAIL失败;
    *******************************************************************/
    int ParseIPAddr(const char* pUrl,std::string& strValue) const;


	CUserMgr *m_pUserMgr;

protected:
	CUserInfo m_UserInfo;
	std::string m_strLocalIp;
	std::string m_strLocalPort;

	std::string m_strMuIpAddr;

    char m_szCameraID[CODE_LEN];                    // 摄像机编码
	char m_szNvrCode[IVS_NVR_CODE_LEN + 1];         // 设备所在的NVRCode
    IVS_MEDIA_PARA m_stuMediaParam;                 // 媒体参数
    SourcePlayType m_iStartType;                    // 打开方式(有句柄/CB/YUV)
    
    unsigned long m_ulHandle;                       // 播放句柄
    HANDLE  m_hRtspSetupEvent;

    bool m_bReuseRtspConn;                          // 是否重用Rtsp连接;

    IVS_SERVICE_TYPE m_enServiceType;               //服务类型;

    ST_MEDIA_RSP m_stMediaRsp;

    IVS_ULONG m_iRtspHandle;                              //Rtsp句柄;
    IVS_ULONG m_ulNetSourceChannel;              //网络收流句柄;
    IVS_ULONG m_ulPlayerChannel;                 //播放库句柄;

    VOS_Mutex*    m_pMediaMutex;       // 媒体锁;
	bool          m_bStepPlay;         // 单步播放，用于平台回放
	bool          m_bStepPlayIsLastFrame;  // 单步播放，用于平台回放

	bool		  m_bAssociatedAudio;		// 是否开启随路语音
	ST_URL_MEDIA_INFO m_stUrlMediaInfo;

    unsigned int m_uLocalPort;

    char m_szRemoteIp[IVS_IP_LEN];
    unsigned int m_uRemotePort;

    float              m_fSpeed;                         // 播放速度;
    bool m_bControlPause;                    // 主动发的流控暂停标志
private:
	//状态,是否空闲;
	IVS_INT32 m_iStatus;

    bool m_bGetKeyIsSuccess;
protected:
	time_t   m_tmStart;
	time_t   m_tmEnd;
	IVS_UINT64 m_uiStartTick;	//开始录像回放时的Tick
};

#endif




