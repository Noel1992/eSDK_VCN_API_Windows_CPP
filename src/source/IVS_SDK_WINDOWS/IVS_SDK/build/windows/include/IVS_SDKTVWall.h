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
filename	: 	IVS_SDKTVWall.h
author		:	g00209332
created		:	2012/12/21	
description	:	SDK对外提供电视墙相关接口定义
                因为电视墙接口不对外开放，只供OCX使用，所以不放在IVS_SDK.h中
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/21 初始版本;
*********************************************************************/
#ifndef __IVS_SDK_TVWALL_H__
#define __IVS_SDK_TVWALL_H__

#include "hwsdk.h"

#ifdef WIN32
#ifdef  IVS_SDK_EXPORTS
#define IVS_SDK_API __declspec(dllexport)
#else
#define IVS_SDK_API __declspec(dllimport)
#endif
#else
//#define IVS_SDK_API __attribute__((visibility))
#define IVS_SDK_API __attribute__((visibility("default")))
#endif

#define CUMW_SESSION_ID_LEN          512  //会话ID的最大长度
#define CUMW_SESSION_URL_LEN         512  //会话Url的最大长度
#define CUMW_ERROR_DESCRIPTION_LEN   512  //错误码对应错误描述的最大长度
#define CUMW_DECODER_ID_LEN          20   //解码器ID的长度
#define CUMW_DECODER_NAME_LEN        32   //解码器通道名称长度
#define CUMW_DECODER_ENCODING_LEN    16   //解码器编码格式最大长度
#define CUMW_DECODER_CHANNEL_MAX_NUM 36   //解码器通道个数最大值
#define CUMW_CHANNEL_NAME_LEN        32   //解码器通道名称长度
#define CUMW_PAY_LOAD_TYPE_NUM       16   //PayLoadType的最大个数
#define CUMW_MAX_PORT_ONE_TYPE_NUM   8    //一个解码器支持的某种类型端口最大的数目
#define DECODER_MAX_CHANNEL_NUM      32

//TVWall返回错误码 (错误码后三位编号：OCX 000～199 SDK 200～399 MP 400～599 TVWALL 600~799)
typedef enum
{
	IVS_TVWALL_MEMORY_NULL = 999115600,			      //指针为空
	IVS_TVWALL_CREATE_THREAD_FAIL = 999115601,        //创建线程失败
	IVS_TVWALL_BUSINESS_ID_NO_EXIST = 999115602,      //业务ID不存在
	IVS_TVWALL_SEND_FAIL = 999115603,                 //发送消息失败

	IVS_TVWALL_INIT_NO = 999115607,                   //尚未初始化
	IVS_TVWALL_INIT_ALREADY = 999115608,              //已经初始化
	IVS_TVWALL_TV_WALL_SRC_RUNING = 999115609,        //电视墙服务已经运行
	IVS_TVWALL_TV_WALL_SRC_STOPED = 999115610,	      //电视墙服务尚未运行
	IVS_TVWALL_DECODER_OFFLINE = 999115611,           //解码器不在线
	IVS_TVWALL_DECODER_NO_CHANNEL = 999115612,        //解码器没有该通道
	IVS_TVWALL_DECODER_INIT_CHANNEL_FAIL = 999115613, //初始化解码器通道失败
	IVS_TVWALL_STOP_FLAG = 999115614,                  //正在启动上墙的过程中，收到停止的命令（内部使用）
	IVS_TVWALL_DECODER_NO_CAPABILITY = 999115615,                  //解码器不具备无缝切换能力

	IVS_TVWALL_FIND_PACKET_FAIL = 999115616, //查找包失败
	IVS_TVWALL_NET_FAIL = 999115617, //由于网络原因或解码器忙, 登录失败!
	IVS_TVWALL_HAS_SAME_IP_DEC = 999115618, //已经有相同IP的解码器加入系统中了
} IVS_TVWALL_ERR_CODE;


//视频流传输协议
enum RDA_PROTOCOL
{
	RDA_PROTOCOL_NONE = 0x00,
	RDA_PROTOCOL_NSS_OVER_UDP = 0x02, //NSS Over UDP
	RDA_PROTOCOL_NSS_OVER_TCP = 0x03, //NSS Over TCP
	RDA_PROTOCOL_RTP_OVER_UDP = 0x04, //RTP Over UDP
	RDA_PROTOCOL_RTP_OVER_TCP = 0x05, //RTP Over TCP
	RDA_PROTOCOL_MAX  = 0x06 //最大值
};

//媒体的网络信息
typedef struct
{
	unsigned short usTcpInterleavedPort;       //TCP交织端口
	unsigned short usUdpVideoRtpPort;          //UDP视频RTP端口
	unsigned short usUdpVideoRtcpPort;         //UDP视频RTCP端口
	unsigned short usUdpAudioRtpPort;          //UDP音频RTP端口
	unsigned short usUdpAudioRtcpPort;         //UDP音频RTCP端口

	char          szMediaIP[IVS_IP_LEN];//解码器的媒体通道IP
	unsigned char ucTransportProtocol;         //视频传输协议
} CUMW_MEDIA_NET_INFO;

//解码初始化消息
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned short usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
	unsigned char  ucTransportProtocol;                 //视频传输协议
} IPI_INIT_BUSINESS_REQ;


//启动远程解码器消息
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned long  ulBusinessID;
	unsigned short usChannelNo;                         //通道编号

	char           szSessionId[CUMW_SESSION_ID_LEN];    //会话ID
	char           szSessionUrl[CUMW_SESSION_URL_LEN];  //会话URL
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）

	unsigned char  ucVideoPayloadType;                  //视频Payload类型
	unsigned char  ucAudioPayloadType;                  //音频Payload类型

	CUMW_MEDIA_NET_INFO stMediaNetInfo;                 //媒体的网络信息
} IPI_START_BUSINESS_REQ;

//停止音视频解码消息
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned long  ulBusinessID;
	unsigned short usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
} IPI_STOP_BUSINESS_REQ;

//播放/停止播放音频消息
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned long  ulBusinessID;
	unsigned short usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
	unsigned char  cAudioCmd;                           //0x00 停止播放 、0x01开始播放
} IPI_AUDIO_CONTROL_REQ;

//音量调节信息
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned long  ulBusinessID;
	unsigned short usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
	unsigned char  cVolume;                             //音量大小（0~100）
} IPI_TUNEUP_VOLUME_REQ;

/*********************通知回调相关定义***********************************/
//通知类型定义
typedef enum
{
	CUMW_NOTIFY_TYPE_EVENT = 0x0001,                     //事件通知
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_EVENT
	CUMW_NOTIFY_TYPE_ASYNC_RET = 0x0002,                 //异步返回通知
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_ASYNC_RET
	CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER = 0x0003,   //远程解码器注册通知
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_REMOTE_DECODER_STATUS
	CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER = 0x0004, //远程解码器注销
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO
	CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT = 0x0005, //远程解码器断连
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO
	CUMW_NOTIFY_TYPE_INIT_DECODER = 0x0006,              //初始化解码器
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_INIT_DECODER_INFO
	CUMW_NOTIFY_TYPE_START_DECODER = 0x0007,             //启动远程解码器
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_START_STOP_DECODER_INFO
	CUMW_NOTIFY_TYPE_DECODER_ERROR = 0x0008,             //解码器通知错误
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO
	CUMW_NOTIFY_TYPE_AUDIO_CONTROL = 0x0009,             //播放/停止播放音频消息通知
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_COM_RES_INFO
	CUMW_NOTIFY_TYPE_SET_VOLUME = 0x000A,                //音量调节消息通知
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_COM_RES_INFO
	CUMW_NOTIFY_TYPE_STOP_DECODER = 0x000B,              //停止解码
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_START_STOP_DECODER_INFO
	CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO = 0x000C,              

	CUMW_NOTIFY_TYPE_AUTO_RESUME_RES = 0x000D,            
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO
	CUMW_NOTIFY_TYPE_RTSP_EXCEPTION = 0x000E,					//RTSP异常，
	//对应pNotifyInfo结构体为: CUMW_NOTIFY_TYPE_DECODER_CAPABILITY_INFO
	CUMW_NOTIFY_TYPE_REMOTE_DECODER_CAPABILITY = 0x000F,//解码器能力，
	//对应pNotifyInfo结构体为: 
	CUMW_NOTIFY_TYPE_SET_PORT_CONFIG_RET = 0x0010,       //设置解码器输出端口配置
	//对应pNotifyInfo结构体为: 
	CUMW_NOTIFY_TYPE_GET_DEV_ABILITY_RET = 0x0011,       //获取设备能力
	//对应pNotifyInfo结构体为: 
	CUMW_NOTIFY_TYPE_SDK_LOGIN_DEC_SUC = 0x0012,         //sdk方式连接的解码器登录成功
	//对应pNotifyInfo结构体为: 
	CUMW_NOTIFY_TYPE_REMOTE_DECODER_RESUMECONNECT =0x0013,
	
	CUMW_NOTIFY_TYPE_INTERFACE_MAX = 0x8000              //接口最大值
} CUMW_NOTIFY_TYPE;

//解码器类型
typedef enum
{
	CUMW_DECODER_TYPE_RDA = 0,			//RDA解码器
	CUMW_DECODER_TYPE_SDK = 1			//SDK解码器
} CUMW_DECODER_TYPE;

//事件通知
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_EVENT
typedef struct
{
	unsigned long ulEventType;    //事件类型，取值见CUMW_NOTIFY_EVENT_TYPE
	unsigned long ulEventPara;    //事件参数
} CUMW_NOTIFY_EVENT;

//异步返回通知
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_ASYNC_RET
//TVWall->SDK
typedef struct
{
	unsigned long   ulAsyncType;        //异步接口类型，取值见CUMW_ASYNC_TYPE
	unsigned long   ulAsyncSeq;         //异步调用序号
	unsigned long   ulAsyncRetCode;     //异步返回码，取值见CUMW_RET_CODE
	unsigned long   ulAsyncRetReason;   //错误详细原因，没有详细原因就是0.错误码参考接入文档
	unsigned long   ulBusinessID;       //业务ID。异步接口类型为开始媒体业务的异步接口时才有效
	void*           pAsyncRetInfo;      //异步返回信息，其意义决定于ulAsyncType，取值见CUMW_ASYNC_TYPE
	unsigned long   ulAsyncRetInfoLen;  //pAsyncRetInfo内容的实际长度
} CUMW_NOTIFY_ASYNC_RET;
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_ASYNC_RET
//SDK->OCX
typedef enum
{
	CUMW_BREAKDOWN_REASON_UNKNOWN = 0,				//未知原因
	CUMW_BREAKDOWN_REASON_DECODER_OFFLINE = 1,		//解码器离线
	CUMW_BREAKDOWN_REASON_NO_VIDEO = 2				//视频中断
}CUMW_BREAKDOWN_REASON;

typedef struct
{
	IVS_INT32 iRetCode;                                 //异步操作返回码
	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN+1];          //摄像头编码
	IVS_CHAR szDecoderID[IVS_TVWALL_DECODER_CODE_LEN+1];//解码器ID
	IVS_UINT32 uiChannel;                               //解码器通道号
	CUMW_BREAKDOWN_REASON iReason;						//异常原因
} CUMW_NOTIFY_ASYNC_RET_EX;

//解码器通道信息
typedef struct
{
	IVS_CHAR szDecoderID[IVS_TVWALL_DECODER_CODE_LEN + 1]; //解码器ID
	unsigned long ulChannelID;                             //解码器通道号
	char          szChannelName[CUMW_CHANNEL_NAME_LEN + 1];//解码器通道名称
	unsigned char ucPortMode;							   //通道对应端口模式（SDK解码器有该功能） 1-1画面 2-2画面 4-4画面 9-9画面 16-16画面
	unsigned char ucChannelType;						   //通道类型 0-输入 1-输出
	bool          bDecording;                              //true-正在解码 false-未解码
} CUMW_DECODER_CHANNEL_INFO;

//远程解码器状态通知
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_REMOTE_DECODER_STATUS
typedef struct
{
	unsigned char			  ucDecoderType;								   //解码器类型 0-RDA解码器 1-SDK解码器
	unsigned long             ulDecoderStatus;                                 //解码器状态，定义参见CUMW_DECODER_STATUS
	unsigned long             ulVideoPayloadTypeNum;                           //支持的视频PayloadType个数。最大值不超过16
	unsigned long             ulAudioPayloadTypeNum;                           //支持的音频PayloadType个数。最大值不超过16
	unsigned long             ulChannelNum;                                    //解码器有效通道个数，最大不超过CUMW_DECODER_CHANNEL_MAX_NUM
	char                      szDecoderID[CUMW_DECODER_ID_LEN + 1];            //解码器ID
	char                      szDecoderName[CUMW_DECODER_NAME_LEN + 1];        //解码器名称
	char				      szDecoderEncoding[CUMW_DECODER_ENCODING_LEN + 1];//解码器编码格式 
	char                      szDecoderIP[IVS_IP_LEN];                        //解码器IP
	unsigned char             ucVideoPayloadTypeArray[CUMW_PAY_LOAD_TYPE_NUM]; //支持的视频PayloadType固定数组。元素个数为16。实际元素个数为ulVideoPayloadTypeNum
	unsigned char             ucAudioPayloadTypeArray[CUMW_PAY_LOAD_TYPE_NUM]; //支持的音频PayloadType固定数组。元素个数为16。实际元素个数为ulAudioPayloadTypeNum
	CUMW_DECODER_CHANNEL_INFO stChannelInfoArray[CUMW_DECODER_CHANNEL_MAX_NUM];//解码器通道信息的数组元素个数为ulChannelNum
} CUMW_NOTIFY_REMOTE_DECODER_STATUS;

typedef struct
{
    IVS_UINT32   uiTotal;                                   // 总记录数
    IVS_INDEX_RANGE stIndexRange;                           // 分页信息  
    CUMW_NOTIFY_REMOTE_DECODER_STATUS stDecoderList[1];     // 解码器列表
} CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST;

//远程解码器断连、注销通知结构体
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT、CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER
typedef struct
{
	char   szDecoderID[CUMW_DECODER_ID_LEN + 1]; //解码器ID
} CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO;


//远程解码器返回的操作结果的信息
typedef struct
{
	unsigned long ulErrorCode;                                   //返回码，0代表成功
	char          szErrorDescription[CUMW_ERROR_DESCRIPTION_LEN];//错误码对应错误描述
}CUMW_ERROR_CODE_INFO;

//解码器初始化通知信息结构体
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_INIT_DECODER
typedef struct
{
	unsigned long ulAsyncSeq;                          //异步调用序号
	unsigned long ulBusinessID;                         
	char          szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）

	CUMW_MEDIA_NET_INFO  stMediaNetInfo;               //媒体的网络信息
	CUMW_ERROR_CODE_INFO stErrorCode;                  //返回信息
} CUMW_NOTIFY_INIT_DECODER_INFO;


//启动远程解码器、停止远程解码器通知信息结构体
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_START_DECODER、CUMW_NOTIFY_TYPE_STOP_DECODER
typedef struct
{
	unsigned long ulAsyncSeq;                          //异步调用序号
	unsigned long ulBusinessID;
	char          szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）

	CUMW_ERROR_CODE_INFO stErrorCode;                  //返回信息
}CUMW_NOTIFY_START_STOP_DECODER_INFO;

//解码器通知错误的消息结构
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_DECODER_ERROR
typedef struct
{
	unsigned short usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）

	CUMW_ERROR_CODE_INFO stErrorCode;                   //返回信息
} CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO;


//通用通知信息结构体
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_SET_VOLUME、CUMW_NOTIFY_TYPE_AUDIO_CONTROL
typedef struct
{
	unsigned long ulAsyncSeq;                          //异步调用序号
	unsigned long ulBusinessID;
	char          szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）

	CUMW_ERROR_CODE_INFO stErrorCode;                  //返回信息
}CUMW_NOTIFY_COM_RES_INFO;


//通知回调函数的通知信息结构体
typedef struct
{
	unsigned long   ulNotifyType;       //通知类型，取值见CUMW_NOTIFY_TYPE
	void*           pNotifyInfo;        //通知信息，其意义决定于ulNotifyType，取值见CUMW_NOTIFY_TYPE
	unsigned long   ulNotifyInfoLen;    //pNotifyInfo内容的实际长度，必须正确
} CUMW_NOTIFY_INFO;

//解码器能力通知信息结构体
//对应的CUMW_NOTIFY_TYPE为: CUMW_NOTIFY_TYPE_REMOTE_DECODER_CAPABILITY
typedef struct
{
	char szDecoderID[CUMW_DECODER_ID_LEN + 1];		//解码器ID
	unsigned long ulSupportSwitch;											//支持无缝切换，1表示支持，0表示不支持
	unsigned long ulReserve1;													//保留字段1
	unsigned long ulReserve2;													//保留字段2
}CUMW_NOTIFY_TYPE_DECODER_CAPABILITY_INFO;


//解码器连接类型
typedef enum
{
	CUMW_DECODER_CONNECT_HIK = 0,		//海康解码器
	CUMW_DECODER_CONNECT_HW
} CUMW_DECODER_CONNECT_TYPE;

// SDK方式连接海康解码器结构体
typedef struct
{
	IVS_CHAR cDecoderID[CUMW_DECODER_ID_LEN+1];
	IVS_CHAR cDecoderName[CUMW_DECODER_NAME_LEN+1];
	IVS_CHAR cDevIP[IVS_IP_LEN + 1];
	IVS_INT32 iPort;
	IVS_CHAR cUser[IVS_NAME_LEN+1];
	IVS_CHAR cPassword[IVS_PWD_LEN + 1];
	IVS_BOOL bLoginSuc;                   // 0-未登录，1-已登录  
	IVS_INT32 iChannelNum;
	CUMW_DECODER_CHANNEL_INFO stChannels[DECODER_MAX_CHANNEL_NUM];
}DECODER_DEV;


//结构体类型定义
typedef struct
{
	unsigned short usChannelNo;
	char szChannelName[CUMW_DECODER_NAME_LEN + 1];
} CHANNEL_INFO;


typedef struct
{
	unsigned short usChannelNum;
	CHANNEL_INFO stChannelInfo[CUMW_DECODER_CHANNEL_MAX_NUM];
} CHANNELS_INFO;

//一个输出端口的配置信息
typedef struct
{
	unsigned short         usPortEnable;     //端口是否使能
	unsigned short         usResolution;     //端口输出分辨率 
	unsigned short         usFrameRate;      //端口刷新率 
	unsigned short         usChannelMode;    //通道模式1，4，9，16 
	CHANNELS_INFO stChannelsInfo;   //端口绑定的通道信息
} PORT_CONFIG;


//多个输出端口的配置信息
typedef struct
{
	unsigned short   usPortNum;
	PORT_CONFIG  stPortConfig[CUMW_MAX_PORT_ONE_TYPE_NUM];
} PORTS_CONFIG;

typedef struct
{
	unsigned long ulVideoPayloadTypeNum;                           //支持的视频PayloadType个数。最大值不超过16
	unsigned long ulAudioPayloadTypeNum; 

	unsigned char ucVideoPayloadTypeArray[CUMW_PAY_LOAD_TYPE_NUM]; //支持的视频PayloadType固定数组。元素个数为16。实际元素个数为ulVideoPayloadTypeNum
	unsigned char ucAudioPayloadTypeArray[CUMW_PAY_LOAD_TYPE_NUM]; //支持的音频PayloadType固定数组。元素个数为16。实际元素个数为ulAudioPayloadTypeNum

	PORTS_CONFIG  stBNCPortCfgs;  //解码器BNC端口的配置 
	PORTS_CONFIG  stVGAPortCfgs;  //解码器VGA端口的配置
	PORTS_CONFIG  stHDMIPortCfgs; //解码器HDMI端口的配置
	PORTS_CONFIG  stDVIPortCfgs;  //解码器DVI端口的配置
} PORTS_INFO;

//设置端口配置
typedef struct
{
	unsigned long  ulAsyncSeq;                          //异步调用序号
	unsigned long  ulBusinessID;
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
	PORTS_INFO    stPortsInfo;
} IPI_SET_PORTS_CONFIG_REQ;

typedef struct
{
	unsigned long  usChannelNo;                         //通道编号
	char           szDecoderID[CUMW_DECODER_ID_LEN + 1];//解码器ID（20位，不包括\0）
	CUMW_ERROR_CODE_INFO stErrorCode;                   //返回信息
} CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO_EX;



// 登陆SDK解码器信息
typedef struct
{
	IVS_CHAR cUserName[IVS_NAME_LEN]; // 用户名
	IVS_CHAR pPWD[IVS_PWD_LEN]; // 密码
	IVS_IP stIP; // 登录解码器IP
	IVS_UINT32 uiPort; // 登录解码器端口		//海康解码器端口8000
	IVS_CHAR cDecoderName[CUMW_DECODER_NAME_LEN];
	IVS_CHAR cReserve[256]; //保留字段
}SDK_DECODER_LOGIN_INFO;



//通知回调函数类型
typedef IVS_INT32 (__SDK_CALL * PCUMW_CALLBACK_NOTIFY)(CUMW_NOTIFY_INFO* pstNotifyInfo,void* pUserData);

#ifdef __cplusplus
extern "C"
{
#endif
	/*************************************************************************
	* name				: IVS_SDK_InitTVWall
	* description	: 初始化电视墙
	* input				: iSessionID   登录成功后的会话ID
	*								pFunNotifyCallBack 回调函数地址, 
	*								pUserData 回调函数数据，不能为NULL
	*								enDecoderType 初始化的解码器类型
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：gaoshuming g00209332
	* remark     : NA
	* history    : 2012-12-20 17.28[add]
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_InitTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, 
														void* pUserData);

	/*************************************************************************
	* name       : IVS_SDK_CleanupTVWall
	* description: 释放电视墙
	* input      : iSessionID   登录成功后的会话ID
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* author     ：gaoshuming g00209332
	* remark     : NA
	* history    : 2012-12-20 17.28[add]
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_CleanupTVWall(IVS_INT32 iSessionID);

	/**********************************************************************
	* name			: IVS_SDK_GetDecoderList 
	* description	: 获取解码器列表
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDecoderList(IVS_INT32 iSessionID,
															const IVS_INDEX_RANGE* pIndexRange,
															CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
															IVS_UINT32 uiBufferSize);
	/**********************************************************************
	* name			: IVS_SDK_StartRealPlayTVWall
	* description	: 启动电视墙实时浏览
	* input			: iSessionID   登录成功后的会话ID
	*				  pCameraCode 摄像机编码
	*				  pRealplayParam 实时浏览参数XML
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayTVWall(IVS_INT32 iSessionID,
																const IVS_CHAR* pCameraCode,
																const IVS_REALPLAY_PARAM* pRealplayParam,
																const IVS_TVWALL_PARAM* pTVWallParam);

	/**********************************************************************
	* name			: IVS_SDK_StopRealPlayTVWall
	* description	: 停止电视墙实时浏览
	* input			: pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayTVWall(IVS_INT32 iSessionID,
																const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StartPlatformPlayBackTVWall
	* description	: 开始电视墙平台录像回放
    * input			: iSessionID   登录成功后的会话ID
	*				  pCameraCode 摄像机编码
    *				  pPlaybackParam 录像回放参数XML
    *				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackTVWall (IVS_INT32 iSessionID,
                                                                            const IVS_CHAR* pCameraCode,
                                                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                            const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StopPlatformPlayBackTVWall
	* description	: 停止电视墙平台录像回放
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBackTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StartPUPlayBackTVWall
	* description	: 开始电视墙前端录像回放
    * input			: iSessionID   登录成功后的会话ID
	*				  pCameraCode 摄像机编码
    *				  pPlaybackParam 录像回放参数XML
    *				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBackTVWall (IVS_INT32 iSessionID,
                                                                            const IVS_CHAR* pCameraCode,
                                                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                            const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StopPUPlayBackTVWall
	* description	: 停止电视墙前端录像回放
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPUPlayBackTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StartBackupPlayBackTVWall
	* description	: 开始电视墙备份录像回放
    * input			: iSessionID   登录成功后的会话ID
	*				  pDomainCode 域编码
	*				  pNVRCode    NVR编码
	*				  pCameraCode 摄像机编码
    *				  pPlaybackParam 录像回放参数XML
    *				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
    IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartBackupPlayBackTVWall (IVS_INT32 iSessionID,
																			const IVS_CHAR* pDomainCode,
																			const IVS_CHAR* pNVRCode,
                                                                            const IVS_CHAR* pCameraCode,
                                                                            const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                            const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_StopBackupPlayBackTVWall
	* description	: 停止电视墙备份录像回放
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopBackupPlayBackTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_PlayBackPauseTVWall
	* description	: 暂停电视墙录像回放
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackPauseTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_PlayBackResumeTVWall
	* description	: 恢复电视墙前端录像回放
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackResumeTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/**********************************************************************
	* name			: IVS_SDK_PlaybackFrameStepForwardTVWall
	* description	: 回放时单帧快进
	* input			: iSessionID   登录成功后的会话ID
	*				  pTVWallParam 电视墙解码器参数
	* output     	: NA
	* return     	: 成功-0 失败-错误码 
	* remark     	: NA   
	*********************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlaybackFrameStepForwardTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam);
	/*************************************************************************
	* name       : IVS_SDK_AddCallBackTVWall
	* description: 增加电视墙事件回调
	* input      : iSessionID   登录成功后的会话ID
	*			   pFunNotifyCallBack 回调函数地址, 
	*			   pUserData 回调函数数据，不能为NULL
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddCallBackTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, 
																void* pUserData);
																
	/*************************************************************************
	* name       : IVS_SDK_RemoveCallBackTVWall
	* description: 移除电视墙事件回调
	* input      : iSessionID   登录成功后的会话ID
	*			   pUserData 回调函数数据，不能为NULL
	* output     : NA
	* return     : 成功返回0，失败返回错误码
	* remark     : NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_RemoveCallBackTVWall(IVS_INT32 iSessionID, void* pUserData);

	/*************************************************************************
	* name			:	IVS_SDK_SwitchRealPlayTVWall
	* description	:	电视墙无缝切屏
	* input			:	iSessionID	登录成功后的会话ID
	* input			:	pCameraCode	摄像机编码
	* input			:	pRealplayParam	实时浏览参数
	* input			:	pTVWallParam	电视墙解码器参数
	* output			:	NA
	* return			:	成功返回0，失败返回错误码
	* remark		:	NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SwitchRealPlayTVWall(IVS_INT32 iSessionID, 
		const IVS_CHAR* pCameraCode,  
		const IVS_REALPLAY_PARAM* pRealplayParam,  
		const IVS_TVWALL_PARAM* pTVWallParam);


	/*************************************************************************
	* name				: IVS_SDK_AddDecoder
	* description	: 登录解码器
	* input				: iSessionID     登录成功后的会话ID
									emConnectType 解码器类型
	*								pSDKDecoderLoginInfo 解码器登录信息
	* output			: NA
	* return			: 成功返回0，失败返回错误码
	* remark			: NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDecoder(IVS_INT32 iSessionID, CUMW_DECODER_CONNECT_TYPE emConnectType, const SDK_DECODER_LOGIN_INFO *pSDKDecoderLoginInfo);

	/*************************************************************************
	* name				: IVS_SDK_DelDecoder
	* description	: 登出解码器
	* input				: iSessionID     登录成功后的会话ID
									emConnectType 解码器类型
									pDecoderID 解码器ID
	* output			: NA
	* return			: 成功返回0，失败返回错误码
	* remark			: NA
	*************************************************************************/
	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelDecoder(IVS_INT32 iSessionID, CUMW_DECODER_CONNECT_TYPE emConnectType, const IVS_CHAR* pDecoderID);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__IVS_SDK_TVWALL_H__
