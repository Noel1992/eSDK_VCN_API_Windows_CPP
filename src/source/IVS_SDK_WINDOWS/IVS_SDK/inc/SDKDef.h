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

/*
 * SDK�ڲ�����ĳ�����ö�١�map���ṹ��
 */
#ifndef __SDK_DEF_H__
#define __SDK_DEF_H__

#include <stdio.h>
#include <string>
#include <map>
#include <list>
#include <queue>
#include <vector>
#include <cmath>
#include <time.h>

#include "hwsdk.h"
#include "ivs_error.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "nss_mt.h"
#include "bp_def.h"
#include "eSDK_Securec.h"
using namespace CBB;

//SDK�汾
//20140628 �汾�Ÿ�Ϊ1.3.30.001 for V1R3C30SPC001 gaoshuming 00209332
//20140703 �汾�Ÿ�Ϊ1.3.20.100 for V1R3C20SPC100 gaoshuming 00209332
//20140723 �汾�Ÿ�Ϊ1.3.30 for V1R3C30 gaoshuming 00209332
//20140814 �汾�Ÿ�Ϊ1.3.30.100 for V1R3C30 zhuohaofan zwx211831
//20140814 �汾�Ÿ�Ϊ1.3.30.200 for V1R3C30 zhuohaofan zwx211831
//20141118 �汾�Ÿ�Ϊ1.3.50 for V1R3C50 zhuohaofan zwx211831
//20150109 �汾�Ÿ�Ϊ1.3.50.100 for V1R3C50SPC100 zhuohaofan zwx211831
//20150514 �汾�Ÿ�Ϊ1.5.20 for V1R5C20 zhuohaofan zwx211831
//20150802 �汾�Ÿ�Ϊ1.5.30 for V1R5C30 zhuohaofan zwx211831
//20151208 �汾�Ÿ�Ϊ1.5.50 for V1R5C50 zhuohaofan zwx211831
//20160202 �汾�Ÿ�Ϊ1.5.60 for V1R5C60 zhuohaofan zwx211831
//20160319 �汾�Ÿ�Ϊ1.6.0.0.1 for V1R6C00T1 zhuohaofan zwx211831
//20160412 �汾�Ÿ�Ϊ1.5.60.100 for V1R5C60SPC100 zhuohaofan zwx211831

#define IVS_SDK_MAINE_VERSION           2
#define IVS_SDK_SUB_VERSION             1
#define IVS_SDK_REVISE_VERSION          0
#define IVS_SDK_RESERVATION_VERSION     0


#define CFG_FILE_MAX_LEN   5*1024*1024     // �����ļ���󳤶�
#define  MAX_LINKID_COUNT  10
#define  MAX_NSS_RSP_SIZE 1024
#define SDK_CLIENT_STARTNO                  0                // SDK�пͻ��˵�¼����ʼ��Ŀ
#define SDK_CLIENT_TOTAL                    128              // SDK����ͻ��˵�¼�������Ŀ
#define SDK_SESSION_NOTUSE                  0                // SDK�Ựδ��ʹ��
#define SDK_SESSION_INUSE                   1                // SDK�Ự����ʹ��
#define SDK_SESSIONID_INIT                  -1               // ��ʼ��sessionIdֵ

#define IVS_CAMERA_STRUCT_LEN   (2*1024)        //�豸�ṹ������ĳ���1500���ֽڣ�ĿǰΪ1196���ֽڣ�Ԥ�����ֺ�����չ,add by y00138234

#define IVS_OCXPLAYER_CLSID	"3556A474-8B23-496F-9E5D-38F7B74654F4"

// ��Ƶ���豸ȫ����Ϣ
typedef struct
{
	IVS_CHAR	cCode[IVS_DEV_CODE_LEN];	    // �豸����
	IVS_CHAR	cName[IVS_CAMERA_NAME_LEN];		// ���������

	IVS_CHAR	cDevGroupCode[IVS_DEVICE_GROUP_LEN];// �����豸�����
	IVS_CHAR	cParentCode[IVS_DEV_CODE_LEN];	    // ���豸����
	IVS_CHAR	cDomainCode[IVS_DOMAIN_CODE_LEN];	// �����
	IVS_CHAR	cDevModelType[IVS_DEV_MODEL_LEN];	// ���豸�ͺ�

	IVS_CHAR	cVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN];  // �豸�ṩ������: "HUAWEI" "HIK" "DAHUA"
	IVS_UINT32	uiDevFormType;                              // ���豸���ͣ��ο� IVS_MAIN_DEVICE_TYPE

	IVS_UINT32	uiType;								// ��������� IVS_CAMERA_TYPE
	IVS_CHAR	cCameraLocation[IVS_DESCRIBE_LEN];	// �������װλ������
	IVS_UINT32	uiCameraStatus;		                // �������չ״̬��1 �C ����	2 �C ��Ƶ��ʧ

	IVS_UINT32	uiStatus;			                // �豸״̬��0-���ߣ�1-���ߣ�2-���� �ο� IVS_DEV_STATUS

	IVS_UINT32  uiNetType;                          // �������� 0-����  1-����, �ο� IVS_NET_TYPE
	IVS_BOOL    bSupportIntelligent;                // �Ƿ�֧�����ܷ���  0-��֧�� 1-֧��

	IVS_BOOL	bEnableVoice;		                // ��·��Ƶ�Ƿ�����	0-ͣ�� 1-����
	IVS_CHAR	cNvrCode[IVS_NVR_CODE_LEN];	        // �豸����NVR����, ��ʾ���豸����NVR����

	IVS_CHAR    cDevCreateTime[IVS_TIME_LEN];       // �豸����ʱ��
	IVS_BOOL	bIsExDomain;                        // �Ƿ�Ϊ�ⲿ�� 0-��  1-��
	IVS_CHAR    cDevIp[IVS_IP_LEN];                 // ǰ��IP

	IVS_CHAR	cLongitude[20];					//����
	IVS_CHAR	cLatitude[20];						//γ��
	IVS_FLOAT	fHeight;								//�������װ�߶�

	IVS_BOOL	bIsShadowDev;										//�Ƿ�ΪӰ���������0-��1-��
	IVS_CHAR	cOrigDevCode[IVS_DEV_CODE_LEN];                      //ԭʼ�豸����
	IVS_CHAR	cOrigParentDevCode[IVS_DEV_CODE_LEN];                      //ԭʼ���豸����
	IVS_CHAR	cOriDevDomainCode[IVS_DOMAIN_CODE_LEN];                      //ԭʼ�����

	IVS_CHAR	cReserve[32];
}IVS_CAMERA_BRIEF_INFO_ALL;

#define IVS_EVENT_RTSP_MSG                  900001           // rtsp client�׳����¼�;
#define IVS_EVENT_NETSOURCE_MSG				900002			 // net source�׳����¼�;

#define LOG_TIME_SPAN (10*365*24*60*60)      // ��־���ʮ�������

#define STREAM_ID_LEN_MAX 32                 //�ط���ID����
#define SSO_RESUME_MAX_SEC		(10 * 60)			//�����¼���������ʱ�䣬��λΪ��

//operational character
typedef enum
{
	OPERATE_ADD = 1,
	OPERATE_SUB,
	OPERATE_MULTI,
	OPERATE_DIVISION
}eM_OPERATE_CHARACTER;

typedef std::map<std::string,std::string> MAP_LINKID_TO_SMU;
typedef std::map<std::string,std::string> MAP_LINKID_TO_SCU;
typedef std::map<std::string,std::string> MAP_LINKID_TO_OMU;

typedef enum TYPE_LINK_ID_MAP
{
	TYPE_LINK_ID_MAP_SMU = 1,
	TYPE_LINK_ID_MAP_SCU,
	TYPE_LINK_ID_MAP_OMU
};

typedef enum CRUD_TYPE
{
    CRUD_TYPE_CREATE = 1,
    CRUD_TYPE_RETRIEV,
    CRUD_TYPE_UPDATE,
    CRUD_TYPE_DELETE
};

typedef enum RECORD_TYPE
{
	RECORD_TYPE_PLAT  = 0,
	RECORD_TYPE_PU    = 1,
	RECORD_TYPE_LOCAL = 2
};

#define LOGIN_ID_LEN         64
#define PLATFORM_VERSION_LEN 64
#define TIME_ZONE_LEN        10
#define SESSION_ID_LEN       64
#define ORG_CODE_LEN         64
#define TIME_LEN         64
//#define FILE_NAME_LEN        256  //�Ƶ�hwsdk.h
#define CODE_LEN             64
#define NVR_CODE_LEN         32 
#define PORT_LEN             6
//#define TIME_SPAN_LEN        36	//�Ƶ�hwsdk.h
#define URL_LEN              1024
#define CAPTURE_TIME_LEN     64

#define LOCAL_DOMAIN_CODE_INIT "0"    // ��ʼ��������룬��½ʱ����linkRouteMgr�����LinkInfo��LinkInfo��������ֶ�Ϊ��ʼ��ֵ

// http/httpsĬ�϶˿�
#define HTTP_PORT  8800
#define HTTPS_PORT 8443

#define DEFAULT_SMU_PORT 9900     // SMUĬ�϶˿�
#define DEFAULT_SCU_PORT 9901     // SCUĬ�϶˿�

// ��¼��Ӧ��Ϣ���ڲ���
typedef struct SRURLoginRsp
{
	char   szLoginID[LOGIN_ID_LEN];
	int    iKeepAliveTime;
	int    iKeepAliveCount;
	char   cPlatformVersion[PLATFORM_VERSION_LEN];
	int    iStatusCode;
	char   szTimeZone[TIME_ZONE_LEN];
	char   szTime[TIME_LEN];
	char   szOrgCode[ORG_CODE_LEN];
	char   szSessionID[SESSION_ID_LEN];
	int     iOmuPort; // add OMU�˿ڣ���½�ɹ������ں�OMU����TCP������
    IVS_UINT32   uiUserID;  // add �û�ID����ѯ��·�ɣ�nvrList��Ҫ
    char  szLocalDomainCode[IVS_DOMAIN_CODE_LEN]; // ��½���ر���SMUCode
}ST_SRU_LOGIN_RSP;

// �ض�����Ϣ
typedef struct   
{
    IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1];
    IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1];
    IVS_INT32	iResult;    
}IVS_DEVICE_REDIRECT_INFO;

/*
 *	ý����Ϣ�ṹ��
 */
typedef struct stUrlMediaInfo
{
    IVS_SERVICE_TYPE ServiceType;
    IVS_PACK_PROTOCOL_TYPE PackProtocolType;
    char szSrcCode[CODE_LEN];
    char szDstCode[CODE_LEN];
    char szDstIP[IVS_IP_LEN];
    char szDstPort[PORT_LEN];
    char szTimeSpan[TIME_SPAN_LEN];
    char szFileName[FILE_NAME_LEN];
	char szNVRCode[NVR_CODE_LEN+1];
	char cDomainCode[IVS_DOMAIN_CODE_LEN+1];	// �����
    char szStreamID[STREAM_ID_LEN_MAX + 1];     //�طŵ�ý����ID����MU���أ����ط���ǽʹ��
	IVS_INT32 iClientType;				//�ͻ������ͣ���szEncryAlgorithm��ϱ�ʶ��Ƶ�Ƿ���Ҫ����
    char szEncryAlgorithm[IVS_PWD_LEN]; //��Ƶ�����㷨���ַ���10λ��Ĭ��Ϊ�գ�""����"0000000000"��ʾ������
    IVS_MEDIA_PARA MediaPara;
	bool IsProgressionTransmit;         //�Ƿ�ת��
	bool IsPassMTU;         //�Ƿ񾭹�MTU,����IVS_SDK_GetRtspURL�ӿ�ֻ�ܵ���0������֧��NAT�ĳ���
	char cCameraDomain[IVS_DOMAIN_CODE_LEN+1];	// �豸���������
}ST_URL_MEDIA_INFO;

/*
 *	ý����Ϣ��Ӧ�ṹ��
 */
typedef struct stMediaRsp
{
    char szURL[URL_LEN];
    char szRTSPIP[IVS_IP_LEN];
    char szRTSPPORT[PORT_LEN];
}ST_MEDIA_RSP;

// ��¼��Ӧ��Ϣ
typedef struct ST_LOGIN_RSP_INFO
{
    IVS_CHAR cLoginID[LOGIN_ID_LEN + 1];                    // ��¼ID��Ψһ��ʶ�û���һ�ε�¼
    IVS_CHAR cOrgCode[ORG_CODE_LEN + 1];                    // ��ǰ��֯����
    IVS_CHAR cPlatformVersion[PLATFORM_VERSION_LEN + 1];    // ƽ̨�汾��16��128��OTHER
    IVS_CHAR cTimeZone[TIME_ZONE_LEN + 1];                  // ��׼ʱ��UT
    IVS_CHAR cSessionID1[SESSION_ID_LEN + 1];               // ÿ���ͻ��˸����������ӵ�ΨһID�����ε�¼��Ч
    IVS_INT32 iResultCode;                                  // ��¼�����
}LOGIN_RSP_INFO;


// ��ѯ������豸�б���״̬��Ϣ
typedef struct 
{
        IVS_INT32 iSessionID; 
        IVS_LONG iSendTime;			// ����ʱ�䣬�����̳߳�ʱ
        IVS_UINT32 uiChannelType;	// ���豸����
        IVS_UINT32 uiRealNum;         
        IVS_INT32 iThreadStatus;
		IVS_INT32 iGetChannelType;	//��ȡ�豸�б�����
        IVS_INT32 iLastRetCode;		// ������
        IVS_UINT32 uiFromIndex;
        IVS_UINT32 uiToIndex;
        IVS_INT32 iExDomainFlag;
        IVS_CHAR szTargetDomainCode[IVS_DOMAIN_CODE_LEN + 1];  // �����
}IVS_CHANNLELIST_RETRIVE_STATUS_INFO;


typedef struct  
{
    IVS_UINT32 uiRealNum;
    IVS_UINT32 uiFromIndex;
    IVS_UINT32 uiToIndex;
}IVS_PAGE_INFO;

/*ʱ��Ƭ�ṹ��*/
typedef struct ST_TIME_SPAN_INTERNAL
{
    IVS_INT32 iStartTime;//��ʼʱ��
    IVS_INT32 iEndTime;//����ʱ��
}TIME_SPAN_INTERNAL;

typedef struct ST_REMOTE_PLAY_BACK_INFO
{
	IVS_INT32 iPlayBackStatus;			            //�ط�״̬��0 ��ͣ 1 �ָ�
	IVS_CHAR cPlayBackStartTime[IVS_TIME_LEN];	    //�طſ�ʼʱ��, ��ʽΪyyyyMMddHHmmss
	IVS_CHAR cPlayBackEndTime[IVS_TIME_LEN];		//�طŽ���ʱ��, ��ʽΪ yyyyMMddHHmmss
	IVS_CHAR cPlayBackTime[IVS_TIME_LEN];			//��ǰ�ط�ʱ��, ��ʽΪ yyyyMMddHHmmss
	IVS_FLOAT fPlayBackSpeed;			            //�ط��ٶȣ���ֵ��ʾ����
}REMOTE_PLAY_BACK_INFO;

// ��ǰ�ڲ��ŵĴ��ڵ���Ϣ
typedef struct CameraPalyInfo
{
	IVS_CHAR    cCameraCode[IVS_DEV_CODE_LEN];      // ���������
	IVS_MEDIA_PARA stMediaPara;
	IVS_ULONG  uPlayHandle;
	void*      pUserData;
	void*      pPlayObj;
}CAMERA_PLAY_INFO;

//add by c00206592
//����ڲ�¼������ṹ��Ϣ
// ¼�������Ϣ������Ϣ
typedef struct
{
	IVS_CHAR	cCameraCode[IVS_DEV_CODE_LEN]; // ����ͷ����
	IVS_CHAR    cNVRCode[IVS_NVR_CODE_LEN];		//¼������NVR��Ϣ
	IVS_CHAR	cRecordFileName[IVS_FILE_NAME_LEN];	// ¼���ļ���
	IVS_UINT32	uiRecordMethod;		// ¼��洢λ�ã��ο� IVS_RECORD_METHOD
	IVS_UINT32	uiRecordType;		// ¼�����ͣ��ο� IVS_RECORD_TYPE
	IVS_TIME_SPAN	stTime;			// ¼����ֹʱ��
	IVS_UINT32	uiFrameExtractionTimes;	// ¼���֡�������ο� IVS_FRAME_EXTRACTION_TIMES
	IVS_CHAR    cAlarmType[IVS_QUERY_VALUE_LEN];		// �澯Դ���� Ӣ���ַ���
	IVS_RECORD_BOOKMARK_INFO	stBookmarkInfo;	    // ¼���ǩ��Ϣ����ѯ��ʽΪ����ǩʱ��Ч
	IVS_RECORD_LOCK_INFO	stLockInfo;		        // ������Ϣ����ѯ��ʽΪ������״̬ʱ��Ч
	IVS_RECORD_PTZ_PRESET_INFO	stPtzPresetInfo;	// ¼��Ԥ��λ��Ϣ����ѯ��ʽΪ��Ԥ��λ��ѯʱ��Ч
}RET_RECORD_INFO;


typedef struct
{
	IVS_INT32	iStartTime; //��ʼʱ��
    IVS_INT32	iEndTime;	//����ʱ��
	IVS_CHAR    cNVRCode[IVS_NVR_CODE_LEN+1];		//¼������NVR��Ϣ
}RECORD_DATA_PLACE;

typedef struct
{
	IVS_CHAR	cCameraCode[IVS_DEV_CODE_LEN]; // ����ͷ����
	IVS_UINT32  uiTotal;					// �ܼ�¼��
	RECORD_DATA_PLACE stRecordData[1];     // ¼�������Ϣ:��ʼ����ʱ���Լ�����NVR
}IVS_RECORD_DATA_PLACE;
//add end c00206592

//add begin
// ���ݷ�����Ǩ����Ϣ�б�
typedef struct
{
	IVS_CHAR cMBUCode[IVS_DOMAIN_CODE_LEN];	        //MBU����
	IVS_CHAR cMBUDomainCode[IVS_DOMAIN_CODE_LEN];	//MBU���������
	IVS_CHAR	cStart[IVS_TIME_LEN];   // ������ʼʱ��,��ʽ��yyyyMMddHHmmss
	IVS_CHAR	cEnd[IVS_TIME_LEN];     // ���ݽ�ֹʱ��,��ʽ��yyyyMMddHHmmss
	IVS_UINT32 uiFromIndex; // ��ʼ��������Ҫ���ظ��û��Ŀ�ʼ��¼����
	IVS_UINT32 uiToIndex;   // ������������Ҫ���ظ��û��Ľ�����¼����
	IVS_UINT32 uiRecordNum; // ��MBU�������ڸñ���ʱ����ڵ�¼���¼��
}IVS_MBU_CHANGE_INFO;
typedef struct
{
	IVS_UINT32   uiTotal;         // �ܼ�¼��
	IVS_INDEX_RANGE stIndexRange;  // ��ҳ��Ϣ
	IVS_MBU_CHANGE_INFO stMBUChangeInfo[1];  // ���ݷ�����Ǩ����Ϣ
}IVS_MBU_CHANGE_INFO_LIST;
//add end by s00193168

//add begin
// ����ץ�Ĳ���
typedef struct
{
	IVS_CHAR   cCapturePath[IVS_FILE_NAME_LEN + 1];			// ����ץ�ı���·��
	IVS_CHAR   cCaptureDownloadPath[IVS_FILE_NAME_LEN + 1]; // ץ��ͼƬ���ر���·��
	IVS_UINT32 uiSnapshotMode;			// ����ץ��ģʽ(0:����,1:����)
	IVS_UINT32 uiSnapshotCount;			// ��������ץ������(1-50)
	IVS_UINT32 uiSnapshotInterval;		// ��������ץ�ļ��(1-10)
	IVS_UINT32 uiSnapshotFormat;		// ����ץ��ͼƬ��ʽ(0:JPG,1:BMP)
	IVS_UINT32 uiNameRule;				// ����ץ����������(1,2,3)
	IVS_CHAR   cFullPath[IVS_FILE_NAME_LEN + 1];
	IVS_CHAR   cFileName[IVS_FILE_NAME_LEN + 1];
	IVS_CHAR   cCameraName[IVS_CAMERA_NAME_LEN + 1];
	IVS_CHAR   cCurrentTime[CAPTURE_TIME_LEN + 1];
	IVS_UINT32 uiPictureNum;
	IVS_UINT32 uiPlayState;				// �������ͣ�1��ʾʵ����2��ʾ¼��

}IVS_LOCAL_CAPTURE_PARAM;
//add end by z00227377

typedef std::list<const IVS_DEVICE_CODE*>			DeviceCodeList;
typedef DeviceCodeList::iterator DeviceCodeListIpIter;

typedef std::map<std::string, DeviceCodeList>	DeviceCodeMap;
typedef DeviceCodeMap::iterator DeviceCodeMapIterator;

typedef std::list<IVS_RESULT_INFO> DeviceResultList;
typedef std::map<std::string, DeviceResultList > DeviceResultMap;

typedef std::list<IVS_DEVICE_REDIRECT_INFO> DeviceRedirectList;
typedef std::map<std::string, DeviceRedirectList> DeviceRedirectMap;


#define POINTER_MAX_NUM    280
#define ALARM_COLOR_LEN    16
#define ALARM_RESERVER_LEN 32

#define LOGIN_LEN    128 // LoginId����
#define USERINFO_TAG 0x0002		 //�Ƿ��û���ʶ��
#define BINSTREAM_TAG 0x0003	//�������������ʶ
#define ALARMDESC_TAG 0x0004	//�澯������ʶ
#define EXTPARAM_TAG 0x0005	//�澯��չ������ʶ
#define OPERATEINFO_TAG 0x0006 //ȷ����Ϣ������

#define STR_LEN 20
#define  EABLE_LENGTH 10
#define DEV_TIME_CFG_LEN 10
#define QUERY_INDEX_COUNT 128 //���չ��һ��VMU������128��MPU
#define MAX_EVENT_QUEUE_LEN    4096
#define SMU_SERVER_DEF_PORT 9900
#define  CONNET_TIME_OUT        8
#define TEMP_BUF_MAX_LENGTH 400

//¼��طų���
#define DECODER_TYPE_HUAWEI     1
#define MAX_PLAYHANDLE 128
#define VEDIO_PLAY_TYPE_LOCAL   0
#define VEDIO_PLAY_TYPE_PLAT    1
#define VEDIO_PLAY_TYPE_FOARD   2

//�˶���ⳣ��
#define ENABLEMOTIONDETECTION 1
#define STR_LEN 20
#define  EABLE_LENGTH 10

//¼����
#define MAX_RECORD_COUNT 81

//ʵ������
#define  MIN_REAL_PLAY_HANDLE      10000
#define SDK_INVALID_HANDLE                   (0)           //�Ƿ��ľ��
#define CLIENT_TYPE_MOBILE 1 //�ͻ����������ֻ��ն�
#define CLIENT_TYPE_PCG 2	//�ͻ���������PCG����
#define CLIENT_TYPE_DECODER 3 //�ͻ��������ǽ�����
#define RTSP_CLIENT_TIMEOUT                 10             // ���峬ʱʱ��1s
#define URL_SERVICE_TYPE_FLAG            "ServiceType="  
#define URL_MEDIA_TRANS_MODE             "MediaTransMode="
#define URL_BROADCAST_TYPE               "BroadcastType="
#define URL_STREAM_TYPE                  "StreamID="
#define URL_TIME_SPAN                    "TimeSpan="
#define IVS_DATE_TIME_NET_FORMAT  "YYYYMMDDThhmmssZ"  // Э����Լ���ı����д��������&ʱ���ʽ
#define IVS_DATE_TIME_FORMAT      "YYYYMMDDhhmmss"    // SDK\CUԼ��������&ʱ���ʽ

//UserData����
#define MAX_FILE_LENGTH IVS_UINT32 (1024 * 1024 * 20)

// LOGO����
#define MAX_FILE_SUFFIX_LENGTH 10
#define LANGUAGE_TYPE_LENGTH 16

//DecoderChannel����
#define STOP_WEIT_TIME 200
#define URL_STREAM_TYPE                  "StreamID="

//aes256Operator����
#define  AES256_KEYLEN 32
#define AES256_INITKEY "12345678901234567890123456789012"   //Ĭ��key

//OCX�豸����
#define  ENABLE_lENGTH 10
#define DEV_TIME_CFG_LEN 10

//OCX�澯����
#define MATCHED_SUCCEED     1
#define POINTER_MAX_NUM     280
#define ALARM_COLOR_NUM     16
#define ALARM_RESERVER_NUM  32

//OCX�豸�ղس���
#define IVS_FAVORITE_ID_LEN 64  //�豸�ղ�ID����
#define IVS_FOLDER_NAME_LEN 64  //�ղؼ����Ƴ������ֵ
#define SCENARIO_INFO_NODE   "ScenarioInfo"
#define SCENARIO_ID_NODE     "ID"
#define SCENARIO_NAME_NODE   "Name"
#define SCENARIO_LAYOUT_NODE "Layout"
#define TEMP_SCENARIO_INFO_ID "love"

#define BACK_UP_USER_DATA_TIME 18000

#define CAMERASWITCH_ID_NODE "CameraSwitchID"
#define CAMERASWITCH_INFO_NODE   "CameraSwitchInfo"
#define CAMERASWITCHPLAN_INFO_NODE   "CameraSwitchPlan"

//SDKTimer
// 4����־����
#define    TIMER_OPERATOR_LOG    16
#define    TIMER_RUN_LOG         17
#define    TIMER_SECURITY_LOG    20
#define    TIMER_USER_LOG        19
#define    MAX_TIMER_LOG_LENTH 512

#define TEMP_BUFFER_SIZE 32

//����ǽ����
#define TVWALL_INFO_ROOT "TVWall"
#define TVWALL_LIST_ROOT "TVWallList"
#define TVWALL_FILE_NAME "TVWallInfo.CBB::CBB::CXml"
#define TVWALL_TEMP_SCENARIO "TVWallTempScenario.CBB::CBB::CXml"
#define TVWALL_SCENARIO "TVWallScenario.CBB::CBB::CXml"

//¼����
#define RECORD_TIME_SPLIT   30      // ʱ��ָ�Ĭ��ֵ 30min
#define RECORD_SIZE_SPLIT   500     // �ߴ�ָ�Ĭ��ֵ 500M
#define RECORD_TIME_MAX     300     // ���¼��ʱ��   5min
#define RECORD_DISK_WARN    1000    // ���̿ռ�1000MB�澯
#define RECORD_DISK_FULL    500     // ���̿ռ�500MBֹͣ����¼��
#define RECORD_DIRECTORY   "C:\\IVS\\LocalRecordFile\\"   // Ĭ�ϱ���·��

//ץ�ĳ���
#define CAPTURE_DEFAULT_PATH			"C:\\IVS\\LocalPictureFile\\"		// ����ץ�ı���·��
#define CAPTURE_DEFAULT_DOWNLOAD_PATH	"C:\\IVS\\DownloadPictureFile\\"    // ץ��ͼƬ���ر���·��
#define CAPTURE_DEFAULT_MODE			CAPTURE_MODE_SINGLE			// ����ץ��ģʽ(0:����,1:����)
#define CAPTURE_DEFAULT_COUNT			1							// ��������ץ������
#define CAPTURE_DEFAULT_INTERVAL		1							// ��������ץ�ļ��
#define CAPTURE_DEFAULT_FORMAT			CAPTURE_FORMAT_JPG			// ����ץ��ͼƬ��ʽ
#define CAPTURE_DEFAULT_NAME_RULE		CAPTURE_RULE_NAME_NUM_TIME  // ����ץ����������

// ץ����ʽ
enum  CAPTURE_MODE {
	CAPTURE_MODE_SINGLE	= 0,	// ����ץ��
	CAPTURE_MODE_SERIAL	= 1,	// ����ץ��
	// add new types here
	CAPTURE_MODE_OTHERS	= 2,
};

// ץ��ͼƬ��������
enum  CAPTURE_NAME_RULE {
	CAPTURE_RULE_NAME_NUM_TIME = 1,	// �������_���_ʱ�䣨YYYY-MM-DD-hh-mm-ss��
	CAPTURE_RULE_NAME_TIME_NUM = 2,	// �������_ʱ�䣨YYYY-MM-DD-hh-mm-ss��_���
	CAPTURE_RULE_TIME_NAME_NUM = 3,	// ʱ�䣨YYYY-MM-DD-hh-mm-ss��_�������_���
    CAPTURE_RULE_CAMERA_TIME_NUM = 4,	// ��������豸����_ʱ�䣨YYYY-MM-DD-hh-mm-ss��_���
	// add new types here
	CAPTURE_RULE_OTHERS		   = 5
};

// ץ��ͼƬ��ʽ
enum  CAPTURE_FORMAT {
	CAPTURE_FORMAT_NONE		= 0,
	CAPTURE_FORMAT_JPG		= 1,	// JPG
	CAPTURE_FORMAT_BMP		= 2,	// BMP
	// add new types here
	CAPTURE_FORMAT_OTHERS	= 3,
};

enum EXDOMAIN_FLAG
{
    EXDOMAIN_FLAG_LOCAL = 0,
    EXDOMAIN_FLAG_OUT = 1,
};

//CommonĿ¼����
#define  STR_LENGTH_SIZE	    20
#define	 INT_FORMAT_D		    10
#define	 INT_FORMAT_X		    16
#define  STR_FORMAT_D		"%d"
#define  STR_FORMAT_X		"%x"
#define  STR_FORMAT_L		"%ld"
#define CONV_CODE_PAGE      936

//OCXXmlProcess����
#define MATCHED_SUCCEED 1
#define MATCHED_FAIL 0
#define ON_LINE 1
#define OFF_LINE 0
#define MATCHED 0



#define INTERNATIONAL_WORD_LEN   (100) // ���ʻ����ʳ���

//VideoPane����
#define INVALID_PORT                        0xFFFFFFFF
#define MAGNIFY_OPEN 1		// �򿪾ֲ��Ŵ�
#define MAGNIFY_CLOSE 0		// �رվֲ��Ŵ�
#define MAGNIFY_MAX_NUM 3   // �ֲ��Ŵ�������
#define IVS_OCX_SERVICE_TYPE_LOCAL_PLAYBACK     0   // ����¼��ط�ҵ������
#define IVS_OCX_SERVICE_TYPE_REMOTE_PLAYBACK    1   // Զ��¼��ط�ҵ������

#define  WM_TRANSPARENT_BK  (WM_USER + 1021)	//lint !e750 �����ļ�ʹ��
#define  WM_NOTIFY_TRANSPARENT (WM_USER + 3221) //lint !e750 �����ļ�ʹ��

//��¼����;
typedef enum
{
	NORMAL_LOGIN	= 0,	//������¼;
	SSO_LOGIN		= 1		//�����¼;
}IVS_LOGIN_TYPE_E;

typedef enum
{
	PAY_AUDIO_LOAD_TYPE_PCMA = 1,    // G711��a��
	PAY_AUDIO_LOAD_TYPE_PCMU,    // G711��u��
	PAY_AUDIO_LOAD_TYPE_G726,    // G726
	PAY_AUDIO_LOAD_TYPE_AAC,     // AAC
}IVS_AUDIO_PAYLOAD_TYPE;


// �ж�AB�����  ��!=��
#define CHECK_NOT_EQUAL( A, B, retValue ) \
	if ( (B) != (A) )                     \
{                                     \
	BP_DBG_LOG("A not Equal B.");        \
	return retValue;                  \
}

#define CHECK_SUCCESS(s)						\
	do { if(!(s)) return IVS_FAIL;} while(0)

// �ָ��ַ���ʱ�ж�ָ���Ƿ�Ϊ��
#define CHECK_STRTOK_POINTER(pPointer, PointerName)	\
	if ( NULL == ( pPointer ) )																\
	{																											\
		BP_DBG_LOG("Point " #PointerName " is null.");					\
		return IVS_FAIL;																			\
	}

// ��־ģ����
#ifdef WIN32
#define LOG_MODULE_NAME_SDK             "\\sdk"
#define LOG_MODULE_NAME_NETSOURCE       "\\netsource"
#define LOG_MODULE_NAME_PLAYER          "\\player"
#define LOG_MODULE_NAME_IA_DRAW         "\\ia_draw"
#define LOG_MODULE_NAME_RTSP            "\\rtsp_log"
#define LOG_SPLIT_CHAR                  "\\"
#else
#define LOG_MODULE_NAME_SDK             "/sdk"
#define LOG_MODULE_NAME_NETSOURCE       "/netsource"
#define LOG_MODULE_NAME_PLAYER          "/player"
#define LOG_MODULE_NAME_IA_DRAW         "/ia_draw"
#define LOG_MODULE_NAME_RTSP            "/rtsp_log"
#define LOG_SPLIT_CHAR                  "/"
#endif

typedef struct
{
    IVS_ALARM_WATERMARK_INFO stWaterMarkInfo; // ˮӡ������Ϣ
    bool bIsNeedReport;				// �Ƿ���Ҫ�ϱ�ˮӡ�澯����ʱ��
    bool bIsOpenedWater;              // �Ƿ��Ѿ�����ˮӡУ��
}ST_WATERMARK_ALARM_INFO;

typedef std::map<IVS_ULONG, ST_WATERMARK_ALARM_INFO> MAP_WATERMARK_ALARM_INFO;
typedef MAP_WATERMARK_ALARM_INFO::iterator MAP_WATERMARK_ALARM_INFO_ITOR;

#define IA_INVALID_REQUEST_ID	0
#define TRANS_ID_LENTH				32

#define IA_QUERY_TYPE_REALTIME	0
#define IA_QUERY_TYPE_RECORD		1
#define IA_INVALID_QUERY_TYPE	ULONG_MAX

//IADraw���
typedef void * DRAW_HANDLE;

//Vedio Type
const short  VIDEO_PLAY_STATE_NOT_PLAYING    =  0  ;
const short  VIDEO_PLAY_STATE_REALTIME       =  1  ;
const short  VIDEO_PLAY_STATE_VOD            =  2  ;
const short  VIDEO_PLAY_STATE_LOCALFILE      =  3  ;
const short  VIDEO_PLAY_STATE_RESTARTING     =  4  ;
const short  VIDEO_PLAY_STATE_STARTING       =  5  ;
const short  VIDEO_PLAY_STATE_EMAP           =  6  ;


typedef struct
{
	IVS_ULONG ulRequestID;														// Panelʶ���
	IVS_ULONG ulRuleType;														// �������ͣ�1����Ϊ���� 3������ʶ�� 4ǰ�����ܷ��� 5����ץ��
	IVS_ULONG ulQueryType;														// �������ͣ�0��ʾʵ����1��ʾ¼��
	IVS_CHAR  CameraID[IVS_DEV_CODE_LEN + 1];				// ��������
	IVS_CHAR  NVRCode[IVS_NVR_CODE_LEN + 1];			// ��������ڵ�NVR����
	IVS_CHAR  StartTime[IVS_TIME_LEN + 1];							// �켣���ӿ�ʼʱ��, ��ʽΪyyyyMMddHHmmss
	IVS_CHAR  StopTime[IVS_TIME_LEN + 1];						// �켣���ӽ���ʱ��, ��ʽΪyyyyMMddHHmmss
}IA_REQ_PARAM;

#define RECORD_OVERLAY_TIMESTEP 60	//¼��켣��������켣���ʱ�䣬��λ����


typedef std::list<IVS_CAMERACODE_INFO> CameraCodeList;		// �����ID�б�
typedef CameraCodeList::iterator CameraCodeListIpIter;

typedef std::map<IVS_UINT32, CameraCodeList> ChannelCameraCodeMap;	// NVRͨ���ź������ID MAP
typedef ChannelCameraCodeMap::iterator ChannelCameraCodeMapIterator;

#endif


