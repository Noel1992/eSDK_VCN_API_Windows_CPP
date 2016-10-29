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
 * SDK内部定义的常量、枚举、map及结构体
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

//SDK版本
//20140628 版本号改为1.3.30.001 for V1R3C30SPC001 gaoshuming 00209332
//20140703 版本号改为1.3.20.100 for V1R3C20SPC100 gaoshuming 00209332
//20140723 版本号改为1.3.30 for V1R3C30 gaoshuming 00209332
//20140814 版本号改为1.3.30.100 for V1R3C30 zhuohaofan zwx211831
//20140814 版本号改为1.3.30.200 for V1R3C30 zhuohaofan zwx211831
//20141118 版本号改为1.3.50 for V1R3C50 zhuohaofan zwx211831
//20150109 版本号改为1.3.50.100 for V1R3C50SPC100 zhuohaofan zwx211831
//20150514 版本号改为1.5.20 for V1R5C20 zhuohaofan zwx211831
//20150802 版本号改为1.5.30 for V1R5C30 zhuohaofan zwx211831
//20151208 版本号改为1.5.50 for V1R5C50 zhuohaofan zwx211831
//20160202 版本号改为1.5.60 for V1R5C60 zhuohaofan zwx211831
//20160319 版本号改为1.6.0.0.1 for V1R6C00T1 zhuohaofan zwx211831
//20160412 版本号改为1.5.60.100 for V1R5C60SPC100 zhuohaofan zwx211831

#define IVS_SDK_MAINE_VERSION           2
#define IVS_SDK_SUB_VERSION             1
#define IVS_SDK_REVISE_VERSION          0
#define IVS_SDK_RESERVATION_VERSION     0


#define CFG_FILE_MAX_LEN   5*1024*1024     // 配置文件最大长度
#define  MAX_LINKID_COUNT  10
#define  MAX_NSS_RSP_SIZE 1024
#define SDK_CLIENT_STARTNO                  0                // SDK中客户端登录的起始数目
#define SDK_CLIENT_TOTAL                    128              // SDK允许客户端登录的最大数目
#define SDK_SESSION_NOTUSE                  0                // SDK会话未被使用
#define SDK_SESSION_INUSE                   1                // SDK会话正被使用
#define SDK_SESSIONID_INIT                  -1               // 初始化sessionId值

#define IVS_CAMERA_STRUCT_LEN   (2*1024)        //设备结构体申请的长度1500个字节，目前为1196个字节，预留部分后续扩展,add by y00138234

#define IVS_OCXPLAYER_CLSID	"3556A474-8B23-496F-9E5D-38F7B74654F4"

// 视频子设备全部信息
typedef struct
{
	IVS_CHAR	cCode[IVS_DEV_CODE_LEN];	    // 设备编码
	IVS_CHAR	cName[IVS_CAMERA_NAME_LEN];		// 摄像机名称

	IVS_CHAR	cDevGroupCode[IVS_DEVICE_GROUP_LEN];// 所属设备组编码
	IVS_CHAR	cParentCode[IVS_DEV_CODE_LEN];	    // 父设备编码
	IVS_CHAR	cDomainCode[IVS_DOMAIN_CODE_LEN];	// 域编码
	IVS_CHAR	cDevModelType[IVS_DEV_MODEL_LEN];	// 主设备型号

	IVS_CHAR	cVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN];  // 设备提供商类型: "HUAWEI" "HIK" "DAHUA"
	IVS_UINT32	uiDevFormType;                              // 主设备类型：参考 IVS_MAIN_DEVICE_TYPE

	IVS_UINT32	uiType;								// 摄像机类型 IVS_CAMERA_TYPE
	IVS_CHAR	cCameraLocation[IVS_DESCRIBE_LEN];	// 摄像机安装位置描述
	IVS_UINT32	uiCameraStatus;		                // 摄像机扩展状态：1 – 正常	2 – 视频丢失

	IVS_UINT32	uiStatus;			                // 设备状态：0-离线，1-在线，2-休眠 参考 IVS_DEV_STATUS

	IVS_UINT32  uiNetType;                          // 网络类型 0-有线  1-无线, 参考 IVS_NET_TYPE
	IVS_BOOL    bSupportIntelligent;                // 是否支持智能分析  0-不支持 1-支持

	IVS_BOOL	bEnableVoice;		                // 随路音频是否启用	0-停用 1-启用
	IVS_CHAR	cNvrCode[IVS_NVR_CODE_LEN];	        // 设备所属NVR编码, 表示该设备被该NVR管理

	IVS_CHAR    cDevCreateTime[IVS_TIME_LEN];       // 设备创建时间
	IVS_BOOL	bIsExDomain;                        // 是否为外部域 0-否  1-是
	IVS_CHAR    cDevIp[IVS_IP_LEN];                 // 前端IP

	IVS_CHAR	cLongitude[20];					//经度
	IVS_CHAR	cLatitude[20];						//纬度
	IVS_FLOAT	fHeight;								//摄像机安装高度

	IVS_BOOL	bIsShadowDev;										//是否为影子摄像机，0-否，1-是
	IVS_CHAR	cOrigDevCode[IVS_DEV_CODE_LEN];                      //原始设备编码
	IVS_CHAR	cOrigParentDevCode[IVS_DEV_CODE_LEN];                      //原始父设备编码
	IVS_CHAR	cOriDevDomainCode[IVS_DOMAIN_CODE_LEN];                      //原始域编码

	IVS_CHAR	cReserve[32];
}IVS_CAMERA_BRIEF_INFO_ALL;

#define IVS_EVENT_RTSP_MSG                  900001           // rtsp client抛出的事件;
#define IVS_EVENT_NETSOURCE_MSG				900002			 // net source抛出的事件;

#define LOG_TIME_SPAN (10*365*24*60*60)      // 日志间隔十年的秒数

#define STREAM_ID_LEN_MAX 32                 //回放流ID长度
#define SSO_RESUME_MAX_SEC		(10 * 60)			//单点登录断线重连最长时间，单位为秒

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
//#define FILE_NAME_LEN        256  //移到hwsdk.h
#define CODE_LEN             64
#define NVR_CODE_LEN         32 
#define PORT_LEN             6
//#define TIME_SPAN_LEN        36	//移到hwsdk.h
#define URL_LEN              1024
#define CAPTURE_TIME_LEN     64

#define LOCAL_DOMAIN_CODE_INIT "0"    // 初始化本域编码，登陆时会向linkRouteMgr中添加LinkInfo，LinkInfo的域编码字段为初始化值

// http/https默认端口
#define HTTP_PORT  8800
#define HTTPS_PORT 8443

#define DEFAULT_SMU_PORT 9900     // SMU默认端口
#define DEFAULT_SCU_PORT 9901     // SCU默认端口

// 登录响应信息（内部）
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
	int     iOmuPort; // add OMU端口，登陆成功后用于和OMU建立TCP长连接
    IVS_UINT32   uiUserID;  // add 用户ID，查询域路由，nvrList需要
    char  szLocalDomainCode[IVS_DOMAIN_CODE_LEN]; // 登陆返回本域SMUCode
}ST_SRU_LOGIN_RSP;

// 重定向消息
typedef struct   
{
    IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1];
    IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1];
    IVS_INT32	iResult;    
}IVS_DEVICE_REDIRECT_INFO;

/*
 *	媒体信息结构体
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
	char cDomainCode[IVS_DOMAIN_CODE_LEN+1];	// 域编码
    char szStreamID[STREAM_ID_LEN_MAX + 1];     //回放的媒体流ID，由MU返回，供回放上墙使用
	IVS_INT32 iClientType;				//客户端类型，与szEncryAlgorithm配合标识视频是否需要加密
    char szEncryAlgorithm[IVS_PWD_LEN]; //视频加密算法，字符串10位，默认为空（""），"0000000000"表示不加密
    IVS_MEDIA_PARA MediaPara;
	bool IsProgressionTransmit;         //是否转发
	bool IsPassMTU;         //是否经过MTU,对于IVS_SDK_GetRtspURL接口只能等于0，才能支持NAT的场景
	char cCameraDomain[IVS_DOMAIN_CODE_LEN+1];	// 设备所在域编码
}ST_URL_MEDIA_INFO;

/*
 *	媒体信息响应结构体
 */
typedef struct stMediaRsp
{
    char szURL[URL_LEN];
    char szRTSPIP[IVS_IP_LEN];
    char szRTSPPORT[PORT_LEN];
}ST_MEDIA_RSP;

// 登录响应信息
typedef struct ST_LOGIN_RSP_INFO
{
    IVS_CHAR cLoginID[LOGIN_ID_LEN + 1];                    // 登录ID，唯一标识用户的一次登录
    IVS_CHAR cOrgCode[ORG_CODE_LEN + 1];                    // 当前组织编码
    IVS_CHAR cPlatformVersion[PLATFORM_VERSION_LEN + 1];    // 平台版本：16、128、OTHER
    IVS_CHAR cTimeZone[TIME_ZONE_LEN + 1];                  // 标准时间UT
    IVS_CHAR cSessionID1[SESSION_ID_LEN + 1];               // 每个客户端跟服务器连接的唯一ID，本次登录有效
    IVS_INT32 iResultCode;                                  // 登录结果码
}LOGIN_RSP_INFO;


// 查询摄像机设备列表返回状态信息
typedef struct 
{
        IVS_INT32 iSessionID; 
        IVS_LONG iSendTime;			// 请求时间，用于线程超时
        IVS_UINT32 uiChannelType;	// 子设备类型
        IVS_UINT32 uiRealNum;         
        IVS_INT32 iThreadStatus;
		IVS_INT32 iGetChannelType;	//获取设备列表类型
        IVS_INT32 iLastRetCode;		// 错误码
        IVS_UINT32 uiFromIndex;
        IVS_UINT32 uiToIndex;
        IVS_INT32 iExDomainFlag;
        IVS_CHAR szTargetDomainCode[IVS_DOMAIN_CODE_LEN + 1];  // 域编码
}IVS_CHANNLELIST_RETRIVE_STATUS_INFO;


typedef struct  
{
    IVS_UINT32 uiRealNum;
    IVS_UINT32 uiFromIndex;
    IVS_UINT32 uiToIndex;
}IVS_PAGE_INFO;

/*时间片结构体*/
typedef struct ST_TIME_SPAN_INTERNAL
{
    IVS_INT32 iStartTime;//开始时间
    IVS_INT32 iEndTime;//结束时间
}TIME_SPAN_INTERNAL;

typedef struct ST_REMOTE_PLAY_BACK_INFO
{
	IVS_INT32 iPlayBackStatus;			            //回放状态，0 暂停 1 恢复
	IVS_CHAR cPlayBackStartTime[IVS_TIME_LEN];	    //回放开始时间, 格式为yyyyMMddHHmmss
	IVS_CHAR cPlayBackEndTime[IVS_TIME_LEN];		//回放结束时间, 格式为 yyyyMMddHHmmss
	IVS_CHAR cPlayBackTime[IVS_TIME_LEN];			//当前回放时间, 格式为 yyyyMMddHHmmss
	IVS_FLOAT fPlayBackSpeed;			            //回放速度，负值表示快退
}REMOTE_PLAY_BACK_INFO;

// 当前在播放的窗口的信息
typedef struct CameraPalyInfo
{
	IVS_CHAR    cCameraCode[IVS_DEV_CODE_LEN];      // 摄像机编码
	IVS_MEDIA_PARA stMediaPara;
	IVS_ULONG  uPlayHandle;
	void*      pUserData;
	void*      pPlayObj;
}CAMERA_PLAY_INFO;

//add by c00206592
//添加内部录像检索结构信息
// 录像检索信息返回信息
typedef struct
{
	IVS_CHAR	cCameraCode[IVS_DEV_CODE_LEN]; // 摄像头编码
	IVS_CHAR    cNVRCode[IVS_NVR_CODE_LEN];		//录像所在NVR信息
	IVS_CHAR	cRecordFileName[IVS_FILE_NAME_LEN];	// 录像文件名
	IVS_UINT32	uiRecordMethod;		// 录像存储位置：参考 IVS_RECORD_METHOD
	IVS_UINT32	uiRecordType;		// 录像类型：参考 IVS_RECORD_TYPE
	IVS_TIME_SPAN	stTime;			// 录像起止时间
	IVS_UINT32	uiFrameExtractionTimes;	// 录像抽帧次数：参考 IVS_FRAME_EXTRACTION_TIMES
	IVS_CHAR    cAlarmType[IVS_QUERY_VALUE_LEN];		// 告警源类型 英文字符串
	IVS_RECORD_BOOKMARK_INFO	stBookmarkInfo;	    // 录像标签信息，查询方式为按书签时有效
	IVS_RECORD_LOCK_INFO	stLockInfo;		        // 锁定信息，查询方式为按锁定状态时有效
	IVS_RECORD_PTZ_PRESET_INFO	stPtzPresetInfo;	// 录像预置位信息，查询方式为按预置位查询时有效
}RET_RECORD_INFO;


typedef struct
{
	IVS_INT32	iStartTime; //开始时间
    IVS_INT32	iEndTime;	//结束时间
	IVS_CHAR    cNVRCode[IVS_NVR_CODE_LEN+1];		//录像所在NVR信息
}RECORD_DATA_PLACE;

typedef struct
{
	IVS_CHAR	cCameraCode[IVS_DEV_CODE_LEN]; // 摄像头编码
	IVS_UINT32  uiTotal;					// 总记录数
	RECORD_DATA_PLACE stRecordData[1];     // 录像检索信息:开始结束时间以及所在NVR
}IVS_RECORD_DATA_PLACE;
//add end c00206592

//add begin
// 备份服务器迁移信息列表
typedef struct
{
	IVS_CHAR cMBUCode[IVS_DOMAIN_CODE_LEN];	        //MBU编码
	IVS_CHAR cMBUDomainCode[IVS_DOMAIN_CODE_LEN];	//MBU所在域编码
	IVS_CHAR	cStart[IVS_TIME_LEN];   // 备份起始时间,格式如yyyyMMddHHmmss
	IVS_CHAR	cEnd[IVS_TIME_LEN];     // 备份截止时间,格式如yyyyMMddHHmmss
	IVS_UINT32 uiFromIndex; // 开始索引：需要返回给用户的开始记录索引
	IVS_UINT32 uiToIndex;   // 结束索引：需要返回给用户的结束记录索引
	IVS_UINT32 uiRecordNum; // 该MBU服务器在该备份时间段内的录像记录数
}IVS_MBU_CHANGE_INFO;
typedef struct
{
	IVS_UINT32   uiTotal;         // 总记录数
	IVS_INDEX_RANGE stIndexRange;  // 分页信息
	IVS_MBU_CHANGE_INFO stMBUChangeInfo[1];  // 备份服务器迁移信息
}IVS_MBU_CHANGE_INFO_LIST;
//add end by s00193168

//add begin
// 本地抓拍参数
typedef struct
{
	IVS_CHAR   cCapturePath[IVS_FILE_NAME_LEN + 1];			// 本地抓拍保存路径
	IVS_CHAR   cCaptureDownloadPath[IVS_FILE_NAME_LEN + 1]; // 抓拍图片下载保存路径
	IVS_UINT32 uiSnapshotMode;			// 本地抓拍模式(0:单张,1:连续)
	IVS_UINT32 uiSnapshotCount;			// 本地连续抓拍张数(1-50)
	IVS_UINT32 uiSnapshotInterval;		// 本地连续抓拍间隔(1-10)
	IVS_UINT32 uiSnapshotFormat;		// 本地抓拍图片格式(0:JPG,1:BMP)
	IVS_UINT32 uiNameRule;				// 本地抓拍命名规则(1,2,3)
	IVS_CHAR   cFullPath[IVS_FILE_NAME_LEN + 1];
	IVS_CHAR   cFileName[IVS_FILE_NAME_LEN + 1];
	IVS_CHAR   cCameraName[IVS_CAMERA_NAME_LEN + 1];
	IVS_CHAR   cCurrentTime[CAPTURE_TIME_LEN + 1];
	IVS_UINT32 uiPictureNum;
	IVS_UINT32 uiPlayState;				// 请求类型，1表示实况，2表示录像

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

#define LOGIN_LEN    128 // LoginId长度
#define USERINFO_TAG 0x0002		 //非法用户标识符
#define BINSTREAM_TAG 0x0003	//代表二进制流标识
#define ALARMDESC_TAG 0x0004	//告警描述标识
#define EXTPARAM_TAG 0x0005	//告警扩展参数标识
#define OPERATEINFO_TAG 0x0006 //确认消息的描述

#define STR_LEN 20
#define  EABLE_LENGTH 10
#define DEV_TIME_CFG_LEN 10
#define QUERY_INDEX_COUNT 128 //按照规格一个VMU下最多挂128个MPU
#define MAX_EVENT_QUEUE_LEN    4096
#define SMU_SERVER_DEF_PORT 9900
#define  CONNET_TIME_OUT        8
#define TEMP_BUF_MAX_LENGTH 400

//录像回放常量
#define DECODER_TYPE_HUAWEI     1
#define MAX_PLAYHANDLE 128
#define VEDIO_PLAY_TYPE_LOCAL   0
#define VEDIO_PLAY_TYPE_PLAT    1
#define VEDIO_PLAY_TYPE_FOARD   2

//运动侦测常量
#define ENABLEMOTIONDETECTION 1
#define STR_LEN 20
#define  EABLE_LENGTH 10

//录像常量
#define MAX_RECORD_COUNT 81

//实况常量
#define  MIN_REAL_PLAY_HANDLE      10000
#define SDK_INVALID_HANDLE                   (0)           //非法的句柄
#define CLIENT_TYPE_MOBILE 1 //客户端类型是手机终端
#define CLIENT_TYPE_PCG 2	//客户端类型是PCG外域
#define CLIENT_TYPE_DECODER 3 //客户端类型是解码器
#define RTSP_CLIENT_TIMEOUT                 10             // 定义超时时间1s
#define URL_SERVICE_TYPE_FLAG            "ServiceType="  
#define URL_MEDIA_TRANS_MODE             "MediaTransMode="
#define URL_BROADCAST_TYPE               "BroadcastType="
#define URL_STREAM_TYPE                  "StreamID="
#define URL_TIME_SPAN                    "TimeSpan="
#define IVS_DATE_TIME_NET_FORMAT  "YYYYMMDDThhmmssZ"  // 协议里约定的报文中传输的日期&时间格式
#define IVS_DATE_TIME_FORMAT      "YYYYMMDDhhmmss"    // SDK\CU约定的日期&时间格式

//UserData常量
#define MAX_FILE_LENGTH IVS_UINT32 (1024 * 1024 * 20)

// LOGO常量
#define MAX_FILE_SUFFIX_LENGTH 10
#define LANGUAGE_TYPE_LENGTH 16

//DecoderChannel常量
#define STOP_WEIT_TIME 200
#define URL_STREAM_TYPE                  "StreamID="

//aes256Operator常量
#define  AES256_KEYLEN 32
#define AES256_INITKEY "12345678901234567890123456789012"   //默认key

//OCX设备常量
#define  ENABLE_lENGTH 10
#define DEV_TIME_CFG_LEN 10

//OCX告警常量
#define MATCHED_SUCCEED     1
#define POINTER_MAX_NUM     280
#define ALARM_COLOR_NUM     16
#define ALARM_RESERVER_NUM  32

//OCX设备收藏场景
#define IVS_FAVORITE_ID_LEN 64  //设备收藏ID长度
#define IVS_FOLDER_NAME_LEN 64  //收藏夹名称长度最大值
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
// 4种日志操作
#define    TIMER_OPERATOR_LOG    16
#define    TIMER_RUN_LOG         17
#define    TIMER_SECURITY_LOG    20
#define    TIMER_USER_LOG        19
#define    MAX_TIMER_LOG_LENTH 512

#define TEMP_BUFFER_SIZE 32

//电视墙常量
#define TVWALL_INFO_ROOT "TVWall"
#define TVWALL_LIST_ROOT "TVWallList"
#define TVWALL_FILE_NAME "TVWallInfo.CBB::CBB::CXml"
#define TVWALL_TEMP_SCENARIO "TVWallTempScenario.CBB::CBB::CXml"
#define TVWALL_SCENARIO "TVWallScenario.CBB::CBB::CXml"

//录像常量
#define RECORD_TIME_SPLIT   30      // 时间分割默认值 30min
#define RECORD_SIZE_SPLIT   500     // 尺寸分割默认值 500M
#define RECORD_TIME_MAX     300     // 最大录像时间   5min
#define RECORD_DISK_WARN    1000    // 磁盘空间1000MB告警
#define RECORD_DISK_FULL    500     // 磁盘空间500MB停止本地录像
#define RECORD_DIRECTORY   "C:\\IVS\\LocalRecordFile\\"   // 默认保存路径

//抓拍常量
#define CAPTURE_DEFAULT_PATH			"C:\\IVS\\LocalPictureFile\\"		// 本地抓拍保存路径
#define CAPTURE_DEFAULT_DOWNLOAD_PATH	"C:\\IVS\\DownloadPictureFile\\"    // 抓拍图片下载保存路径
#define CAPTURE_DEFAULT_MODE			CAPTURE_MODE_SINGLE			// 本地抓拍模式(0:单张,1:连续)
#define CAPTURE_DEFAULT_COUNT			1							// 本地连续抓拍张数
#define CAPTURE_DEFAULT_INTERVAL		1							// 本地连续抓拍间隔
#define CAPTURE_DEFAULT_FORMAT			CAPTURE_FORMAT_JPG			// 本地抓拍图片格式
#define CAPTURE_DEFAULT_NAME_RULE		CAPTURE_RULE_NAME_NUM_TIME  // 本地抓拍命名规则

// 抓拍形式
enum  CAPTURE_MODE {
	CAPTURE_MODE_SINGLE	= 0,	// 单张抓拍
	CAPTURE_MODE_SERIAL	= 1,	// 连续抓拍
	// add new types here
	CAPTURE_MODE_OTHERS	= 2,
};

// 抓拍图片命名规则
enum  CAPTURE_NAME_RULE {
	CAPTURE_RULE_NAME_NUM_TIME = 1,	// 摄像机名_序号_时间（YYYY-MM-DD-hh-mm-ss）
	CAPTURE_RULE_NAME_TIME_NUM = 2,	// 摄像机名_时间（YYYY-MM-DD-hh-mm-ss）_序号
	CAPTURE_RULE_TIME_NAME_NUM = 3,	// 时间（YYYY-MM-DD-hh-mm-ss）_摄像机名_序号
    CAPTURE_RULE_CAMERA_TIME_NUM = 4,	// 摄像机的设备编码_时间（YYYY-MM-DD-hh-mm-ss）_序号
	// add new types here
	CAPTURE_RULE_OTHERS		   = 5
};

// 抓拍图片格式
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

//Common目录常量
#define  STR_LENGTH_SIZE	    20
#define	 INT_FORMAT_D		    10
#define	 INT_FORMAT_X		    16
#define  STR_FORMAT_D		"%d"
#define  STR_FORMAT_X		"%x"
#define  STR_FORMAT_L		"%ld"
#define CONV_CODE_PAGE      936

//OCXXmlProcess常量
#define MATCHED_SUCCEED 1
#define MATCHED_FAIL 0
#define ON_LINE 1
#define OFF_LINE 0
#define MATCHED 0



#define INTERNATIONAL_WORD_LEN   (100) // 国际化单词长度

//VideoPane常量
#define INVALID_PORT                        0xFFFFFFFF
#define MAGNIFY_OPEN 1		// 打开局部放大
#define MAGNIFY_CLOSE 0		// 关闭局部放大
#define MAGNIFY_MAX_NUM 3   // 局部放大最大个数
#define IVS_OCX_SERVICE_TYPE_LOCAL_PLAYBACK     0   // 本地录像回放业务类型
#define IVS_OCX_SERVICE_TYPE_REMOTE_PLAYBACK    1   // 远程录像回放业务类型

#define  WM_TRANSPARENT_BK  (WM_USER + 1021)	//lint !e750 其他文件使用
#define  WM_NOTIFY_TRANSPARENT (WM_USER + 3221) //lint !e750 其他文件使用

//登录类型;
typedef enum
{
	NORMAL_LOGIN	= 0,	//正常登录;
	SSO_LOGIN		= 1		//单点登录;
}IVS_LOGIN_TYPE_E;

typedef enum
{
	PAY_AUDIO_LOAD_TYPE_PCMA = 1,    // G711的a率
	PAY_AUDIO_LOAD_TYPE_PCMU,    // G711的u率
	PAY_AUDIO_LOAD_TYPE_G726,    // G726
	PAY_AUDIO_LOAD_TYPE_AAC,     // AAC
}IVS_AUDIO_PAYLOAD_TYPE;


// 判断AB不相等  （!=）
#define CHECK_NOT_EQUAL( A, B, retValue ) \
	if ( (B) != (A) )                     \
{                                     \
	BP_DBG_LOG("A not Equal B.");        \
	return retValue;                  \
}

#define CHECK_SUCCESS(s)						\
	do { if(!(s)) return IVS_FAIL;} while(0)

// 分割字符串时判断指针是否为空
#define CHECK_STRTOK_POINTER(pPointer, PointerName)	\
	if ( NULL == ( pPointer ) )																\
	{																											\
		BP_DBG_LOG("Point " #PointerName " is null.");					\
		return IVS_FAIL;																			\
	}

// 日志模块名
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
    IVS_ALARM_WATERMARK_INFO stWaterMarkInfo; // 水印数据信息
    bool bIsNeedReport;				// 是否需要上报水印告警结束时间
    bool bIsOpenedWater;              // 是否已经开启水印校验
}ST_WATERMARK_ALARM_INFO;

typedef std::map<IVS_ULONG, ST_WATERMARK_ALARM_INFO> MAP_WATERMARK_ALARM_INFO;
typedef MAP_WATERMARK_ALARM_INFO::iterator MAP_WATERMARK_ALARM_INFO_ITOR;

#define IA_INVALID_REQUEST_ID	0
#define TRANS_ID_LENTH				32

#define IA_QUERY_TYPE_REALTIME	0
#define IA_QUERY_TYPE_RECORD		1
#define IA_INVALID_QUERY_TYPE	ULONG_MAX

//IADraw句柄
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
	IVS_ULONG ulRequestID;														// Panel识别号
	IVS_ULONG ulRuleType;														// 规则类型，1是行为分析 3是人脸识别 4前端智能分析 5人脸抓拍
	IVS_ULONG ulQueryType;														// 请求类型，0表示实况，1表示录像
	IVS_CHAR  CameraID[IVS_DEV_CODE_LEN + 1];				// 摄像机编号
	IVS_CHAR  NVRCode[IVS_NVR_CODE_LEN + 1];			// 摄像机所在的NVR编码
	IVS_CHAR  StartTime[IVS_TIME_LEN + 1];							// 轨迹叠加开始时间, 格式为yyyyMMddHHmmss
	IVS_CHAR  StopTime[IVS_TIME_LEN + 1];						// 轨迹叠加结束时间, 格式为yyyyMMddHHmmss
}IA_REQ_PARAM;

#define RECORD_OVERLAY_TIMESTEP 60	//录像轨迹叠加请求轨迹间隔时间，单位是秒


typedef std::list<IVS_CAMERACODE_INFO> CameraCodeList;		// 摄像机ID列表
typedef CameraCodeList::iterator CameraCodeListIpIter;

typedef std::map<IVS_UINT32, CameraCodeList> ChannelCameraCodeMap;	// NVR通道号和摄像机ID MAP
typedef ChannelCameraCodeMap::iterator ChannelCameraCodeMapIterator;

#endif


