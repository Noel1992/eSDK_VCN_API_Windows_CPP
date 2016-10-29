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
 description :    ý�����;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 ��ʼ�汾
*********************************************************************/
#ifndef __MEDIA_BASE_H__
#define __MEDIA_BASE_H__

/*!< ������ͷ�ļ� */
#include "UserInfo.h"
#include "rtsp_client_api.h"
#include "RtspRetCode.h"
#include "IVS_NetSource.h"
#include "IVSPlayerDataType.h"
#include "ParseSdp.h"
#include "bp_environments.h"


enum SourcePlayType
{
	PLAY_TYPE_HWND = 0,         //��Ⱦ�����;
	PLAY_TYPE_CB,               //��ȡƴ֡�����
	PLAY_TYPE_YUV               //��ȡ��������
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
    bool                bBroadcast;         //�Ƿ�Ϊ�㲥

    tagStartPlayParam()
        : enSourceType(SOURCE_RTP)
        , enServiceType(SERVICE_TYPE_REALVIDEO)
        , fSpeed(1.0)// Ĭ����1���ٵķ�ʽ����;
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
	* description: ���ÿ���/ֹͣУ��ˮӡ
	* input      : ulHandle          ҵ����������ʵ�����ط�
				   bIsStart          1������ˮӡ 0��ֹͣˮӡ
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * author     ��chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 SetWaterMarkVerify(IVS_UINT32 uiChannel, IVS_BOOL bIsStart) const;

/**************************************************************************
    * name       : SetPlayQualityMode
	* description: ����ʵ������ģʽ������������
	* input      : ulHandle          ҵ����������ʵ�����ط�
	               iPlayQualityMode  ����ģʽ �ο���IVS_PLAY_QUALITY_MODE
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * author     ��chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 SetPlayQualityMode(IVS_UINT32 uiChannel,IVS_INT32 iPlayQualityMode) const;

 	/**************************************************************************
    * name       : GetMediaInfo
    * description: ��ȡ��Ƶ��������
    * input      : ulHandle          ҵ����������ʵ�����ط�
    * output     : pMediaInfo        ý����Ϣ
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    **************************************************************************/
	IVS_INT32 GetMediaInfo(IVS_UINT32 uiChannel,IVS_MEDIA_INFO* pMediaInfo) const;

/**************************************************************************
    * name       : GetPlayedTime
    * description: ��ȡ�Ѳ���ʱ�䡣
	* input      : ulHandle           ҵ����������ʵ�����ط�
	* output     : puPlayedTime       ��ǰ����ʱ��
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * author     ��chenxianxiao00206592
	* remark     : NA
	* history    : 2012-11-23
**************************************************************************/
	IVS_INT32 GetPlayedTime(IVS_UINT32 uiChannel,IVS_UINT32 *puPlayTime) const;

/**************************************************************************
    * name       : SetDisplayScale
    * description: ���ò�����ʾ���ʡ�
	* input      : uiChannel           �ŵ�
	* output     : iDisplayScale       ��ʾ����
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * author     ��chenxianxiao00206592
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
     �������ƣ�IsReuseRtspConnection
     �������������ݷ������ͺ�Э�������ж��Ƿ���rtsp����;
     ���������enServiceType  ��������;
               enProtocolType Э������;
     ���������NA
     �� �� ֵ��true false;
    *****************************************************************************/
    bool IsReuseRtspConnection(IVS_SERVICE_TYPE enServiceType, IVS_PROTOCOL_TYPE enProtocolType, 
                               IVS_MEDIA_TRANS_MODE enTransMode, IVS_BROADCAST_TYPE enBroadcastType);

    /******************************************************************
     function   : StartPlay
     description: ��������;
     input      : const START_PLAY_PARAM & stParam
     output     : NA
     return     : int
    *******************************************************************/
    virtual int StartPlay(const START_PLAY_PARAM& stParam);

	virtual int StartDownload(const START_PLAY_PARAM& stParam, const char *pCam, const IVS_DOWNLOAD_PARAM *pParam);


    /******************************************************************
     function   : ReStartStream
     description: ʵ������������;
     input      : const START_PLAY_PARAM & stParam
     output     : NA
     return     : int
    *******************************************************************/
	virtual int ReStartStream(const START_PLAY_PARAM& stParam);

    /******************************************************************
     function   : StopStream
     description: ʵ����ֹͣ
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	virtual int StopStream();

   /******************************************************************
     function   : StopPlay
     description: ʵ��ֹͣ
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/ 
    virtual int StopPlay();

	/******************************************************************
     function   : StopDownLoad
     description: ֹͣ����
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/ 
	virtual int StopDownLoad();

    /******************************************************************
     function   : PlaySound
     description: ������Ƶ����
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int PlaySound();


    /******************************************************************
     function   : StopSound
     description: �ر���Ƶ����
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int StopSound();

    /******************************************************************
     function   : SetVolume
     description: ��������
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int SetVolume(unsigned int uiVolumeValue);

    /******************************************************************
     function   : GetVolume
     description: ��ȡ����
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
    virtual int GetVolume(unsigned int* puiVolumeValue);

    /******************************************************************
     function   : NotifyStreamBufferState
     description: ֪ͨ��������״̬
     input      : IVS_INT32 iBufferState
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    virtual IVS_INT32 NotifyStreamBufferState(IVS_INT32 iBufferState);

    /******************************************************************
     function   : StartLocalRecord
     description: ��ʼ����¼��
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	int StartLocalRecord(const IVS_LOCAL_RECORD_PARAM* pRecordParam,const IVS_CHAR* pSaveFileName);

    /******************************************************************
     function   : StopLocalRecord
     description: ֹͣ����¼��
     input      : NA
     output     : NA
     return     : int
    *******************************************************************/
	int StopLocalRecord();

    /******************************************************************
     function   : LocalSnapshot
     description: ����ץ�ĵ���ͼƬ
	 input      : pSavePath			ͼƬ����·����
				  ulSnapshotFormat	ͼƬ��������
     output     : NA
     return     : int
    *******************************************************************/
	int LocalSnapshot(const char *pSavePath, IVS_ULONG ulSnapshotFormat);
	int LocalSnapshotRect(const char *pSavePath, IVS_ULONG ulSnapshotFormat, const IVS_RECT* pRect);


    /******************************************************************
     function   : SetDrawCallBack
     description: ���û�ͼ�ص�
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
     description: ��UTCʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr     ��ת�����ַ���
     input      : const char * pFormat  �ַ�����Ӧ�ĸ�ʽ;
     output     : NA
     return     : time_t ת�����ʱ��
    *******************************************************************/
    static time_t UTCStr2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : UTCStr2Span
     description: ��ʱ����ֹʱ���ַ���ת��ΪЭ���ʽ,�磺
									20160214155505, 20160214160505--> 20160214T155505Z-20160214T160505Z
     input      : stStartTime     ��ʼʱ��
     input      : stEndTime   ��ֹʱ��
     output     : NA
     return     : 
    *******************************************************************/
    static void UTCStr2Span(std::string& strTime, const char* stStartTime, const char* stEndTime);

	/******************************************************************
     function   : UTCTime2Str
     description: ��UTCʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr     ��ת�����ַ���
     input      : const char * pFormat  �ַ�����Ӧ�ĸ�ʽ;
     output     : NA
     return     : time_t ת�����ʱ��
    *******************************************************************/
    static void UTCTime2Str(const time_t t, std::string& strTime, const std::string& strFormat);

	/******************************************************************
	 function   : GetRtspErrorCode
	 description: ��rtsp client�Ĵ�����ת��Ϊivs������;
	 input      : int iErrorCode
	 output     : NA
	 return     : int ������;
	*******************************************************************/
	static int GetRtspErrorCode(int iErrorCode);

	/******************************************************************
	function	: SetSecretKey
	description : ������Կ��Ϣ;
	input		: uiChannel   ҵ����
				  stMediaKeyInfo   ������Ϣ
	output		: NA
	return		: �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 SetSecretKey(IVS_ULONG uiChannel, const MEDIA_KEY_INFO& stMediaKeyInfo);

    IVS_INT32 SetNoNeedSecretKey(IVS_ULONG uiChannel, const MEDIA_KEY_INFO& stMediaKeyInfo);

	/******************************************************************
	function	: GetStreamencryKey
	description : ����¼����ܵĹ�����Կ�����������
	input		: stMediaKeyInfo ý�������Ϣ
	output		: strSecretKey ��Կ 
	return		: �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 GetStreamencryKey(IVS_CHAR* cSecretKey,const MEDIA_KEY_INFO& stMediaKeyInfo) const;

	/******************************************************************
	 function   : InitMediaPara
	 description: ��ʼ��MediaPara
	 input      : IVS_MEDIA_PARA & stMediaPara
	 output     : NA
	 return     : void
	*******************************************************************/
	void InitMediaPara(IVS_MEDIA_PARA& stMediaPara) const;

	/******************************************************************
	 function   : GetSourceBufferRemain
	 description: ��ȡδ�������ݵĸ���;
	 input      : IVS_UINT32 & uBufferCount
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetSourceBufferRemain(IVS_UINT32 &uBufferCount);





	IVS_INT32 SetMeidaAttribute(const MEDIA_ATTR &VideoAttr, const MEDIA_ATTR &AudioAttr);//lint !e601







private:

    /******************************************************************
     function   : ProcessRtspPlayMsg
     description: ����rtsp��play����;
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
     description: ��ʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr
     input      : const char * pFormat
     output     : NA
     return     : time_t
    *******************************************************************/
    static time_t Str2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : Time2Str
	 description: ������ʱ�䣨�Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
	 *              ת��Ϊ����"20090630091030"���ַ���
	 input      : char *pDestBuf������ʱ���ַ���������ָ�루ֵ�����������int nbuflen������ʱ���ַ�����������С��
	              const time_t *calptr���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00��������������
     output     : ����ָ��pDestBuf
     return     : time_t
    *******************************************************************/
    static void Time2Str(const time_t t, std::string& strTime, const std::string& strFormat);

public:
	/***************************************************************************
		�� �� �� : AddrUint2Char
		�������� : ��ַת������������-���ַ�����
		������� : name                    IP��ַ����
		������� : addr                    IP��ַ�ַ���
		�� �� ֵ : RET_OK -�ɹ�       RET_FAIL -ʧ��
	***************************************************************************/
	static int AddrUint2Char(const unsigned int name, std::string &addr);
	 /***************************************************************************
     �� �� �� : GetURL
     �������� : ��SMU/SCU��ȡý��URL
     ������� : stUrlMediaInfo��urlý����Ϣ��stMediaRsp����Ӧ��ý����Ϣ
     ������� : xml                    ����xml
     �� �� ֵ : 0 -�ɹ� ;1-ʧ��
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
     description: ����ý�����;
     input      : MEDIA_PARA * pMediaPara
     output     : strValue 
     return     : int
    *******************************************************************/
    int SetMediaParam(const IVS_MEDIA_PARA* pMediaPara);

    /******************************************************************
     function   : ParseURL
     description: ����URL,pKey����Ҫ�õ���ֵ
     input      : const char * pUrl
     input      : const char * pKey
     input      : std::string & strValue
     output     : NA
     return     : int IVS_SUCCEED�ɹ� IVS_FAILʧ��;
    *******************************************************************/
    int ParseURL(const char* pUrl, const char* pKey, std::string& strValue);

	 /******************************************************************
     function   : ParseIPAddr
     description: ����URL,��ȡMU���ڵ�IP��ַ
     input      : const char * pUrl
     input      : std::string & strValue
     output     : NA
     return     : int IVS_SUCCEED�ɹ� IVS_FAILʧ��;
    *******************************************************************/
    int ParseIPAddr(const char* pUrl,std::string& strValue) const;


	CUserMgr *m_pUserMgr;

protected:
	CUserInfo m_UserInfo;
	std::string m_strLocalIp;
	std::string m_strLocalPort;

	std::string m_strMuIpAddr;

    char m_szCameraID[CODE_LEN];                    // ���������
	char m_szNvrCode[IVS_NVR_CODE_LEN + 1];         // �豸���ڵ�NVRCode
    IVS_MEDIA_PARA m_stuMediaParam;                 // ý�����
    SourcePlayType m_iStartType;                    // �򿪷�ʽ(�о��/CB/YUV)
    
    unsigned long m_ulHandle;                       // ���ž��
    HANDLE  m_hRtspSetupEvent;

    bool m_bReuseRtspConn;                          // �Ƿ�����Rtsp����;

    IVS_SERVICE_TYPE m_enServiceType;               //��������;

    ST_MEDIA_RSP m_stMediaRsp;

    IVS_ULONG m_iRtspHandle;                              //Rtsp���;
    IVS_ULONG m_ulNetSourceChannel;              //�����������;
    IVS_ULONG m_ulPlayerChannel;                 //���ſ���;

    VOS_Mutex*    m_pMediaMutex;       // ý����;
	bool          m_bStepPlay;         // �������ţ�����ƽ̨�ط�
	bool          m_bStepPlayIsLastFrame;  // �������ţ�����ƽ̨�ط�

	bool		  m_bAssociatedAudio;		// �Ƿ�����·����
	ST_URL_MEDIA_INFO m_stUrlMediaInfo;

    unsigned int m_uLocalPort;

    char m_szRemoteIp[IVS_IP_LEN];
    unsigned int m_uRemotePort;

    float              m_fSpeed;                         // �����ٶ�;
    bool m_bControlPause;                    // ��������������ͣ��־
private:
	//״̬,�Ƿ����;
	IVS_INT32 m_iStatus;

    bool m_bGetKeyIsSuccess;
protected:
	time_t   m_tmStart;
	time_t   m_tmEnd;
	IVS_UINT64 m_uiStartTick;	//��ʼ¼��ط�ʱ��Tick
};

#endif




