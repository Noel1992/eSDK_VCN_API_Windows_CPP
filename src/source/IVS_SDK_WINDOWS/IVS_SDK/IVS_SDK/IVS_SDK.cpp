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
filename	: 	IVSSDK.cpp
author		:	z00201790
created		:	2012/10/23	
description	:	定义 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2011-2015 Huawei Tech.Co.,Ltd
history		:	2012/10/23 初始版本;
*********************************************************************/

/*!< 预编译头文件 */
#ifdef WIN32
#include "stdafx.h"
#endif

#include <stdlib.h> 

#include "IVS_SDK.h"
#include "IVS_SDKTVWall.h"
#include "IVS_SDKNetKeyBoard.h"
#include "NetKeyBoard.h"
//#include "IVS/SDKInterface.h"
#include "SDKInterface.h"
//#include "IVS/Sys/CriticalSectionEx.h"
#include "CriticalSectionEx.h"
#include "bp_log.h"
#include "IVS_Player.h"
#include "Aes256Operator.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "AudioMgr.h"
#include "ivs_password.h"
#include "EventCallBackJob.h"
#include "ToolHelp.h"
#include "SDKConfig.h"
#include "eSDKLogAPI.h"
#include "UnhandledExceptionFilter.h"
#include "NVRChannelMgr.h"

#ifndef DEBUG
#ifdef WIN64
//#include "Rename2Java.h"
#else
#include "Rename2Java.h"
#endif
#endif

CSDKInterface *g_pNvs = NULL;
CCRITICAL_SECTIONEX g_InitLock; //用于保护g_pNvs、g_InitCount，支持多线程调用;
int g_InitCount = 0;
//对讲
VOS_Mutex* m_pMutexTalkbackFlagLock;
bool g_bIsVoiceTalkbackOpened = false;
int g_voiceTalkbackHandle = SDK_INVALID_HANDLE;


static CMediaBase* GetMediaBaseAndPortInfo(IVS_INT32 iSessionID,IVS_ULONG ulHandle,IVS_UINT32& uiPort)
{
	IVS_INFO_TRACE("SessionID: %d,Handle: %lu",iSessionID,ulHandle);
	if (NULL == g_pNvs)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get Media And Port Info", "Get Media And Port Info Error");
		return NULL;
	}
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	if (NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"Get Media And Port Info", "Session ID Error");
		return NULL;
	}
	IVS_INT32 iRet = IVS_FAIL;
	IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_REALVIDEO;
	iRet = pUserMgr->GetServiceTypebyPlayHandle(ulHandle, enServiceType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Media And Port Info", "Get Service Type failed. enServiceType = %d", (int)enServiceType);
		return NULL;
	}
	CMediaBase* mediaBase = NULL;
	switch(enServiceType)
	{
	case SERVICE_TYPE_REALVIDEO:
		mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulHandle);
		uiPort= pUserMgr->GetRealPlayMgr().GetPlayerChannelByHandle(ulHandle);
		break;
	case SERVICE_TYPE_PLAYBACK:				//录像回放
    case SERVICE_TYPE_PU_PLAYBACK:			//前端录像回放
	case SERVICE_TYPE_BACKUPRECORD_PLAYBACK://录像备份回放
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK://容灾录像回放
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(ulHandle);
		uiPort= pUserMgr->GetPlaybackMgr().GetPlayerChannelByHandle(ulHandle);
		break;
	default:
		break;
	}
	if (NULL == mediaBase)
	{
		BP_RUN_LOG_ERR(iRet,"Get Media And Port Info", "Get Media Info Error");
		return NULL;
	}
	return mediaBase;
}

//初始化SDK
IVS_INT32 __SDK_CALL IVS_SDK_Init()
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	(void)DeleteFile("C:\\mp.dat");

	//获取IVS_SDK.dll路径
	TCHAR pBuf[MAX_PATH] = {'\0'};
	GetModuleFileName(::GetModuleHandle("IVS_SDK.dll"), pBuf, MAX_PATH-sizeof(TCHAR));
	char* cTemp = (char*)pBuf;
	std::string strDLLPath = cTemp;
	strDLLPath = strDLLPath.substr(0,strDLLPath.rfind('\\')+1);

	//获取统计日志的配置文件路径
	std::string strInitPath = strDLLPath;
	strInitPath.append(__CLIENT_LOG_CFG__);
	//获取统计日志输出路径
	//std::string strLogOutPath = strDLLPath;
	//strLogOutPath.append("log");
	std::string strLogOutPath =  INVALID_LOG_PATH; //表示日志统计的logpath根据eSDKClientLogCfg.ini中的LogPath确定

	unsigned int uiClientLogLevel[LOG_CATEGORY] = {INVALID_LOG_LEVEL,INVALID_LOG_LEVEL,INVALID_LOG_LEVEL};
	LogInit(__PRODUCT_NAME__, strInitPath.c_str(), uiClientLogLevel, strLogOutPath.c_str());

	g_InitLock.EnterCriticalSectionEx();
	
	//如果已经获取了全局CSDKInterface实例，那么只增加计数;
	if (g_pNvs != NULL)
	{
		g_InitCount ++;
		g_InitLock.LeaveCriticalSectionEx();
		BP_RUN_LOG_INF("IVS SDK Init Success", "g_pNvs exist");

		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_NO_SESSION(reqTime,rspTime,IVS_SUCCEED,"");
		return IVS_SUCCEED;
	}

	try
	{
		g_pNvs = new CSDKInterface();
		if(NULL == g_pNvs)
		{
			g_InitLock.LeaveCriticalSectionEx();

			eSDK_LOG_RUN_WRAN("NEW SDK Interface Failed.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_NO_SESSION(reqTime,rspTime,IVS_OPERATE_MEMORY_ERROR,"");

			LogFini(__PRODUCT_NAME__);
			return IVS_OPERATE_MEMORY_ERROR;
		}

		g_InitCount ++;
		g_InitLock.LeaveCriticalSectionEx();

#ifdef WIN32
		SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);
#endif

		IVS_INT32 iRet = g_pNvs->Init();
		if (IVS_SUCCEED != iRet)
		{
			g_InitLock.EnterCriticalSectionEx();
			g_InitCount--;
			delete g_pNvs;
			g_pNvs = NULL;
			g_InitLock.LeaveCriticalSectionEx();

			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");

			LogFini(__PRODUCT_NAME__);
			return iRet;
		}
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");

		return iRet;
	}
	catch (...)
	{
		g_InitLock.LeaveCriticalSectionEx();
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"IVS SDK Init Failed.", "Reason:Try NEW SDK Interface Failed.");

		eSDK_LOG_RUN_WRAN("Try NEW SDK Interface Failed.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_NO_SESSION(reqTime,rspTime,IVS_OPERATE_MEMORY_ERROR,"");

		LogFini(__PRODUCT_NAME__);
		return IVS_OPERATE_MEMORY_ERROR;
	}
}

//释放SDK
IVS_INT32 __SDK_CALL IVS_SDK_Cleanup()
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	IVS_INFO_TRACE("");

	int iRet = IVS_SUCCEED;
	g_InitLock.EnterCriticalSectionEx();
	if(g_InitCount)
	{
		g_InitCount--;
	}

	if (g_InitCount <= 0)
	{
		if(NULL == g_pNvs)
		{
			g_InitLock.LeaveCriticalSectionEx();
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"IVS SDK Clean up Failed.", "Reason:g_pNvs is Null");

			eSDK_LOG_RUN_WRAN("Try NEW SDK Interface Failed.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_NO_SESSION(reqTime,rspTime,IVS_OPERATE_MEMORY_ERROR,"");

			LogFini(__PRODUCT_NAME__);
			return IVS_OPERATE_MEMORY_ERROR;
		}
		iRet = g_pNvs->Cleanup();
		if (g_pNvs)
		{
			delete g_pNvs;
			g_pNvs = NULL;
		}
	}

	g_InitLock.LeaveCriticalSectionEx();
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");
	LogFini(__PRODUCT_NAME__);
	return iRet;  
}

//获取SDK版本号，当前版本号为2.1.0.1
IVS_INT32 __SDK_CALL IVS_SDK_GetVersion()
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("");
		
	WORD nlVersion = MAKEWORD(IVS_SDK_RESERVATION_VERSION,IVS_SDK_REVISE_VERSION);//lint !e835 !e845
	WORD nhVersion = MAKEWORD(IVS_SDK_SUB_VERSION,IVS_SDK_MAINE_VERSION);
	DWORD ulVersion;
	ulVersion = MAKELONG(nlVersion, nhVersion);
	
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,ulVersion,"");
	return ulVersion;//lint !e713
}

//设置事件回调函数
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetEventCallBack(IVS_INT32 iSessionID, EventCallBack fEventCallBack, void *pUserData)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime); 
	

	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = g_pNvs->SetEventCallBack(iSessionID, fEventCallBack, pUserData);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//删除设备组
IVS_INT32 __SDK_CALL IVS_SDK_DeleteDeviceGroup( IVS_INT32 iSessionID,
                                                const IVS_CHAR* pDomainCode,
                                                IVS_UINT32 uiDevGroupID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR); 
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,DevGroupID: %u",iSessionID,pDomainCode,uiDevGroupID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);

    IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().DeleteDeviceGroup(pDomainCode, uiDevGroupID);

	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,DevGroupID=%u",pDomainCode,uiDevGroupID);
	return iRet;
}


// 添加用户
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddUser(IVS_INT32 iSessionID, const IVS_USER_INFO* pUserInfo, IVS_UINT32* puiUserID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserInfo, IVS_PARA_INVALID);
    CHECK_POINTER(puiUserID, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d",iSessionID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().AddUser(pUserInfo, puiUserID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 修改用户
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyUser(IVS_INT32 iSessionID, const IVS_USER_INFO* pUserInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d",iSessionID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserInfo, IVS_PARA_INVALID);
	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify User failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet =  pUserMgr->GetUserInfoMgr().ModifyUser(pUserInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"")
	return iRet;

}

// 删除用户
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID)
{    
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet =  pUserMgr->GetUserInfoMgr().DeleteUser(pDomainCode, uiUserID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u",pDomainCode,uiUserID);
	return iRet;

}

// 获取用户列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_USER_INFO_LIST* pUserList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,BufferSize: %u",iSessionID,uiBufferSize);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
    CHECK_POINTER(pUserList, IVS_PARA_INVALID);

	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get User List Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet =  pUserMgr->GetUserInfoMgr().GetUserList(pDomainCode, pUnifiedQuery, pUserList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,BufferSize=%u",pDomainCode,uiBufferSize);
	return iRet;
}

// 获取在线用户列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOnlineUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ONLINE_USER_LIST* pOnlineUserList, IVS_UINT32 uiBufferSize)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
    IVS_INFO_TRACE("SessionID: %d, BufferSize: %u",iSessionID,uiBufferSize);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
    CHECK_POINTER(pOnlineUserList, IVS_PARA_INVALID);

	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Online User Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);    
    IVS_INT32 iRet =  pUserMgr->GetUserInfoMgr().GetOnlineUser(pDomainCode, pUnifiedQuery, pOnlineUserList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,BufferSize=%u",pDomainCode,uiBufferSize);
	return iRet;
}

// 获取用户信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserInfo(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_USER_INFO* pUserInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserInfo, IVS_PARA_INVALID);

	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get User Info Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);    
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetUserInfo(pDomainCode, uiUserID, pUserInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u",pDomainCode,uiUserID);
	return iRet;
}

// 锁定/解锁用户
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LockUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_BOOL bLock)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Lock User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);    
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().LockUser(pDomainCode, (IVS_INT32)uiUserID, uiUserID, bLock);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u",pDomainCode,uiUserID);
	return iRet;
}

// 设置用户设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetUserDeviceList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 uiBufferSize)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	BP_RUN_LOG_INF("IVS_SDK_SetUserDeviceList", "");
    IVS_INFO_TRACE("SessionID: %d,UserID: %u,BufferSize: %u",iSessionID,uiUserID,uiBufferSize);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);

	

	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Set User Device List Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	// TQE
	if (uiBufferSize < sizeof(IVS_USER_DEVICE_INFO_LIST))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"buffer size error", "NA");
		return IVS_PARA_INVALID;
	}

	// 拆掉与编码 TODO
	IVS_USER_DEVICE_INFO_LIST* pUserDevListIn = (IVS_USER_DEVICE_INFO_LIST*)IVS_NEW((IVS_CHAR* &)pUserDevListIn,uiBufferSize);
	(void)CToolsHelp::Memcpy(pUserDevListIn, uiBufferSize, pUserDevList, uiBufferSize);
	IVS_UINT32 uiNum = pUserDevListIn->uiTotal;
	for (IVS_UINT32 i = 0; i < uiNum; i++)
	{
		IVS_CHAR szDevGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
		IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		(void)CXmlProcess::ParseDevCode(pUserDevListIn->stUserDeviceInfo[i].cDevGroupCode, szDevGroupCode, szDomainCode);
		(void)CToolsHelp::Memcpy(pUserDevListIn->stUserDeviceInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, szDevGroupCode, IVS_DEVICE_GROUP_LEN);
	}
	IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().SetUserDeviceList(pDomainCode, uiUserID, pUserDevList, uiBufferSize);
	IVS_DELETE((IVS_CHAR* &)pUserDevListIn, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u,BufferSize=%u",pDomainCode,uiUserID,uiBufferSize);
	return iRet;
}

// 查询用户设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserDeviceList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_DEVICE_INFO_LIST* pUserDevList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,UserID: %u,BufferSize: %u",iSessionID,uiUserID,uiBufferSize);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
    CHECK_POINTER(pUserDevList, IVS_PARA_INVALID);	

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get User Device List Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetUserDeviceList(pDomainCode, uiUserID, pIndexRange, pUserDevList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u,BufferSize=%u",pDomainCode,uiUserID,uiBufferSize);
	return iRet;
}

// 修改密码
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ChangePWD(IVS_INT32 iSessionID, const IVS_CHAR* pOldPWD, const IVS_CHAR* pNewPWD)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pOldPWD, IVS_PARA_INVALID);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Change PWD Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);    
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().ChangePWD(pOldPWD, pNewPWD);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 重置密码
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ResetPWD(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID, const IVS_CHAR* pNewPWD)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Reset PWD Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().ResetPWD(pDomainCode, uiUserID, pNewPWD);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,uiUserID=%u",pDomainCode,uiUserID);
	return iRet;
}

// 校验密码是否满足账号规则
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_CheckPWDByRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, const IVS_CHAR* pPWD)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pLoginName, IVS_PARA_INVALID);
    CHECK_POINTER(pPWD, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d,RoleID: %u,LoginName: %s",iSessionID,uiRoleID,pLoginName);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Check PWD By Role Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().CheckPWDByRole(pDomainCode, uiRoleID, pLoginName, pPWD);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,RoleID=%u,LoginName=%s",pDomainCode,uiRoleID,pLoginName);
	return iRet;
}

// 获取密码强度
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPWDStrength(const IVS_CHAR* pPWD, IVS_UINT32* uiPWDStrength)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWD, IVS_PARA_INVALID);
    IVS_INFO_TRACE("uiPWDStrength: %u",uiPWDStrength);

    *uiPWDStrength = GetPasswordReliability(pPWD);
    IVS_INT32 iRet = IVS_SUCCEED;

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");
	return iRet;
}

// 获取密码过期信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPWDExpireInfo(IVS_INT32 iSessionID, IVS_PWD_EXPIRE_INFO* pPWDExpireInfo)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d",iSessionID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWDExpireInfo, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Reset PWD Failed.", "Reason:SessionID is invalid");


		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetPWDExpireInfo(pPWDExpireInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 根据用户ID签退用户的所有会话
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

    IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Sign Out User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);     
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().SignOutUser(pDomainCode, uiUserID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u",pDomainCode,uiUserID);
	return iRet;
}

// 根据角色ID签退用户的所有会话
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutUserByRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiRoleID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

    IVS_INFO_TRACE("SessionID: %d,RoleID: %u",iSessionID,uiRoleID);	

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Sign Out User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);     
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().SignOutUserByRole(pDomainCode, uiRoleID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"RoleID=%u",uiRoleID);
	return iRet;
}

// 签退在线用户
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SignOutOnlineUser(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_UINT32 uiUserOnlineID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,UserOnlineID: %u",iSessionID,uiUserOnlineID);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Sign Out Online User Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);     
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().SignOutOnlineUser(pDomainCode, uiUserOnlineID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"UserOnlineID=%u",uiUserOnlineID);
	return iRet;
}

// 添加用户组init
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddUserGroup(IVS_INT32 iSessionID, const IVS_USER_GROUP* pUserGroup, IVS_UINT32* puiUserGroupID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroup, IVS_PARA_INVALID);
    CHECK_POINTER(puiUserGroupID, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d",iSessionID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserGroupMgr().AddUserGroup(pUserGroup, puiUserGroupID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;

}

// 删除用户组
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteUserGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserGroupID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

    IVS_INFO_TRACE("SessionID: %d,UserGroupID: %u",iSessionID,uiUserGroupID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserGroupMgr().DeleteUserGroup(pDomainCode, uiUserGroupID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserGroupID=%u",pDomainCode,uiUserGroupID);
	return iRet;

}

// 修改用户组
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyUserGroup(IVS_INT32 iSessionID, const IVS_USER_GROUP* pUserGroup)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroup, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserGroupMgr().ModifyUserGroup(pUserGroup);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 移动用户组
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_MoveUserGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiTargetGroupID, const IVS_USER_GROUP_LIST* pUserGroupList)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,TargetGroupID: %u",iSessionID,uiTargetGroupID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUserGroupList, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Move User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);


    IVS_INT32 iRet = pUserMgr->GetUserGroupMgr().MoveUserGroup(pDomainCode, uiTargetGroupID, pUserGroupList);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,TargetGroupID=%d",pDomainCode,uiTargetGroupID);
	return iRet;
}

// 获取用户组列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserGroupList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_USER_GROUP_LIST* pUserGroupList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d,BufferSize: %u",iSessionID,uiBufferSize);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
    CHECK_POINTER(pUserGroupList, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get User Group List Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserGroupMgr().GetUserGroupList(pDomainCode, pIndexRange, pUserGroupList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,BufferSize=%u",pDomainCode,uiBufferSize);
	return iRet;
}

// 添加角色
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRole(IVS_INT32 iSessionID, const IVS_ROLE_INFO* pRoleInfo, IVS_UINT32* puiRoleID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRoleInfo, IVS_PARA_INVALID);
    CHECK_POINTER(puiRoleID, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add Role Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);   

    IVS_INT32 iRet = pUserMgr->GetRoleMgr().AddRole(pRoleInfo, puiRoleID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;

}

// 删除角色
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRole(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,RoleID: %u",iSessionID,uiRoleID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete Role Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

    IVS_INT32 iRet = pUserMgr->GetRoleMgr().DeleteRole(pDomainCode, uiRoleID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,RoleID=%u",pDomainCode,uiRoleID);
	return iRet;
}

// 修改角色
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRole(IVS_INT32 iSessionID, const IVS_ROLE_INFO* pRoleInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRoleInfo, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify Role Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);     
    IVS_INT32 iRet = pUserMgr->GetRoleMgr().ModifyRole(pRoleInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 获取角色列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_INDEX_RANGE* pIndexRange, IVS_ROLE_INFO_LIST* pRoleInfoList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,uiBufferSize: %u",iSessionID,uiBufferSize);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
    CHECK_POINTER(pRoleInfoList, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Role List Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRoleMgr().GetRoleList(pDomainCode, pIndexRange, pRoleInfoList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,BufferSize=%u",pDomainCode,uiBufferSize);
	return iRet;
}

// 根据角色ID获取角色详细信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleInfoByRoleID(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, IVS_ROLE_INFO* pRoleInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRoleInfo, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d,RoleID: %u",iSessionID,uiRoleID); 

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RoleInfo By RoleID Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRoleMgr().GetRoleInfoByRoleID(pDomainCode, uiRoleID, pRoleInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,RoleID=%u",pDomainCode,uiRoleID);
	return iRet;
}

//根据角色获取符合账号规则的密码 
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAutoCreatePWD(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiRoleID, const IVS_CHAR* pLoginName, IVS_CHAR* pNewPWD, IVS_UINT32 uiPWDLen)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pLoginName, IVS_PARA_INVALID);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d,RoleID: %u,LoginName: %s",iSessionID,uiRoleID,pLoginName);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get AutoCreate PWD Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetAutoCreatePWD(pDomainCode, uiRoleID, pLoginName, pNewPWD, uiPWDLen);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,RoleID=%u,LoginName=%s",pDomainCode,uiRoleID,pLoginName);
	return iRet;

}

// 根据用户ID获取角色详细信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRoleInfoByUserID(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiUserID, IVS_ROLE_INFO* pRoleInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,UserID: %u",iSessionID,uiUserID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRoleInfo, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RoleInfo By UserID Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

    IVS_INT32 iRet = pUserMgr->GetRoleMgr().GetRoleInfoByUserID(pDomainCode, uiUserID, pRoleInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%u",pDomainCode,uiUserID);
	return iRet;
}

// 获取默认账号规则
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDefaultRoleAccountRule(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_ROLE_ACCOUNT_RULE* pRoleAccountRule)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRoleAccountRule, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Default RoleAccount Rule Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

    IVS_INT32 iRet = pUserMgr->GetRoleMgr().GetDefaultRoleAccountRule(pDomainCode, pRoleAccountRule);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s",pDomainCode);
	return iRet;
}

// 获取用户权限信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetOperationRightGroup(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_OPERATION_RIGHT_GROUP_LIST* pRightGroupList,IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,BufferSize: %u",iSessionID,uiBufferSize);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRightGroupList, IVS_PARA_INVALID);

    //校验sessionId
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Operation Right Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    //获取单个用户对象
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRoleMgr().GetOperationRightGroup(pDomainCode, pRightGroupList);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,BufferSize=%u",pDomainCode,uiBufferSize);
	return iRet;
}

//新增平台录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordPlan(IVS_INT32 iSessionID,
	                                                   IVS_UINT32 uiDeviceNum,
	                                                   const IVS_DEVICE_CODE* pDeviceList,
	                                                   const IVS_RECORD_PLAN* pRecordPlan,
	                                                   IVS_RESULT_LIST* pResultList,
	                                                   IVS_UINT32 uiResultBufferSize)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("Add Record Plan SessionID: %d,DeviceNum: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiResultBufferSize);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add Record Plan Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    CRecordPlanMgr& recordPlanMgr = pUserMgr->GetRecordPlanMgr();
    recordPlanMgr.SetUserMgr(pUserMgr); 
    IVS_INT32 iRet = recordPlanMgr.AddRecordPlan(uiDeviceNum,pDeviceList, pRecordPlan ,pResultList, uiResultBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,ResultBufferSize=%u",uiDeviceNum,uiResultBufferSize);
	return iRet;
}

//修改平台录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRecordPlan(IVS_INT32 iSessionID,IVS_UINT32 uiDeviceNum,
                                                          const IVS_DEVICE_CODE* pDeviceList,
	                                                      const IVS_RECORD_PLAN* pRecordPlan,
                                                          IVS_RESULT_LIST* pResultList,
                                                          IVS_UINT32 uiResultBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("Modify Record Plan SessionID: %d,DeviceNum: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiResultBufferSize);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify RecordPlan Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().ModifyRecordPlan(uiDeviceNum,pDeviceList, pRecordPlan ,pResultList, uiResultBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,ResultBufferSize=%u",uiDeviceNum,uiResultBufferSize);
	return iRet;
}

//删除平台录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordPlan(IVS_INT32 iSessionID,
	                                                      IVS_UINT32 uiDeviceNum,
	                                                      const IVS_DEVICE_CODE* pDeviceList,
	                                                      IVS_UINT32 uiRecordMethod,
	                                                      IVS_RESULT_LIST* pResultList,
	                                                      IVS_UINT32 uiResultBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("SessionID: %d,DeviceNum: %u,RecordMethod: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiRecordMethod,uiResultBufferSize);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete RecordPlan Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().DeleteRecordPlan(uiDeviceNum, pDeviceList, uiRecordMethod,pResultList, uiResultBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,RecordMethod=%u,ResultBufferSize=%u",uiDeviceNum,uiRecordMethod,uiResultBufferSize);
	return iRet;
}

//查询平台录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPlan(IVS_INT32 iSessionID,
	                                                   const char* pCameraCode,
	                                                   IVS_UINT32 uiRecordMethod,
	                                                   IVS_RECORD_PLAN* pRecordPlan)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode :%s,RecordMethod: %u",iSessionID,pCameraCode,uiRecordMethod);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RecordPlan Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().GetRecordPlan(pCameraCode, uiRecordMethod, pRecordPlan);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,RecordMethod=%u",pCameraCode,uiRecordMethod);
	return iRet;
}

//新增录像书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddBookmark(IVS_INT32 iSessionID,
	const IVS_CHAR* pCameraCode,
	const IVS_CHAR* pNVRCode,
	const IVS_CHAR* pBookmarkName,
	const IVS_CHAR* pBookmarkTime,
	IVS_UINT32* uiBookmarkID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	//CHECK_POINTER(pBookmarkName, IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	CHECK_POINTER(pBookmarkTime, IVS_PARA_INVALID);
	CHECK_POINTER(uiBookmarkID, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,NVRCode: %s,BookmarkName :%s,BookmarkTime :%s,AlarmLevelNum: %u",iSessionID,pCameraCode,pNVRCode,pBookmarkName,pBookmarkTime,*uiBookmarkID);


	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
	IVS_INT32 iRet = pUserMgr->GetRecordBookmark().AddBookmark(RECORD_METHOD_PLATFORM,strDomainCode.c_str(),pNVRCode,pCameraCode, pBookmarkName, pBookmarkTime,uiBookmarkID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,NVRCode=%s,BookmarkName=%s,BookmarkTime=%s",pCameraCode,pNVRCode,pBookmarkName,pBookmarkTime);
	return iRet;
}

//修改录像书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	//CHECK_POINTER(pNewBookmarkName, IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,NVRCode: %s,CameraCode: %s,BookmarkID: %u,NewBookmarkName: %s",iSessionID,pNVRCode,pCameraCode,uiBookmarkID,pNewBookmarkName);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
    IVS_INT32 iRet = pUserMgr->GetRecordBookmark().ModifyBookmark(RECORD_METHOD_PLATFORM,strDomainCode.c_str(),pNVRCode,pCameraCode,uiBookmarkID,pNewBookmarkName);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"NVRCode=%s,CameraCode=%s,BookmarkID=%u,NewBookmarkName=%s",pNVRCode,pCameraCode,uiBookmarkID,pNewBookmarkName);
	return iRet;

}

//删除录像书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	

	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,NVRCode: %s,CameraCode: %s,BookmarkID: %u",iSessionID,pNVRCode,pCameraCode,uiBookmarkID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
    IVS_INT32 iRet = pUserMgr->GetRecordBookmark().DeleteBookmark(RECORD_METHOD_PLATFORM,strDomainCode.c_str(),pNVRCode,pCameraCode, uiBookmarkID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"NVRCode=%s,CameraCode=%s,BookmarkID=%u",pNVRCode,pCameraCode,uiBookmarkID);
	return iRet;
}

//新增实况书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRealPlayBookmark(IVS_INT32 iSessionID,
	                                                const IVS_CHAR* pCameraCode,	                                                 
	                                                const IVS_CHAR* pBookmarkName,
	                                                const IVS_CHAR* pBookmarkTime,
													IVS_UINT32* uiBookmarkID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	//CHECK_POINTER(pBookmarkName, IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	CHECK_POINTER(pBookmarkTime, IVS_PARA_INVALID);
	CHECK_POINTER(uiBookmarkID, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,BookmarkName :%s,BookmarkTime :%s,AlarmLevelNum: %u",iSessionID,pCameraCode,pBookmarkName,pBookmarkTime,*uiBookmarkID);


    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
    IVS_INT32 iRet = pUserMgr->GetRealPlayBookmark().AddBookmark(strDomainCode.c_str(),pCameraCode, pBookmarkName, pBookmarkTime,uiBookmarkID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,BookmarkName=%s,BookmarkTime=%s",pCameraCode,pBookmarkName,pBookmarkTime);
	return iRet;
}

//修改实况书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRealPlayBookmark(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	//CHECK_POINTER(pNewBookmarkName, IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,BookmarkID: %u,NewBookmarkName: %s",iSessionID,pCameraCode,uiBookmarkID,pNewBookmarkName);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
    IVS_INT32 iRet = pUserMgr->GetRealPlayBookmark().ModifyBookmark(strDomainCode.c_str(),pCameraCode,uiBookmarkID,pNewBookmarkName);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,BookmarkID=%u,NewBookmarkName=%s",pCameraCode,uiBookmarkID,pNewBookmarkName);
	return iRet;

}

//删除实况书签
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRealPlayBookmark(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,BookmarkID: %u",iSessionID,pCameraCode,uiBookmarkID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete Bookmark Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	std::string strDomainCode = "NULL";
    IVS_INT32 iRet = pUserMgr->GetRealPlayBookmark().DeleteBookmark(strDomainCode.c_str(),pCameraCode, uiBookmarkID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,BookmarkID=%u",pCameraCode,uiBookmarkID);
	return iRet;
}


//查询录像书签列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetBookmarkList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %u",iSessionID,uiBufSize);
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get BookMark List Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordBookmark().GetBookmarkList(RECORD_METHOD_PLATFORM,pUnifiedQuery,pBookmarkList,uiBufSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%u",uiBufSize);
	return iRet;
}

//获取录像列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryRecordList(IVS_INT32 iSessionID, 
                                                         const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,  
	                                                     IVS_RECORD_INFO_LIST* pRecordInfoList,
                                                         IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %u",iSessionID,uiBufferSize);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Query RecordList Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordList().QueryRecordList(pUnifiedQuery,pRecordInfoList,uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%u",uiBufferSize);
	return iRet;
}

//查询录像列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordList(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordMethod,const IVS_TIME_SPAN* pTimeSpan,
	const IVS_INDEX_RANGE* pIndexRange,IVS_RECORD_INFO_LIST* pRecordList,IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordList, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode; %s,RecordMethod: %d,BufSize: %u",iSessionID,pCameraCode,iRecordMethod,uiBufSize);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RecordList Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordList().GetRecordList(pCameraCode,iRecordMethod,pTimeSpan,pIndexRange,pRecordList,uiBufSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,RecordMethod=%d,BufSize=%u",pCameraCode,iRecordMethod,uiBufSize);
	return iRet;
}

//查询平台录像策略
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPolicyByTime(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_RECORD_POLICY_TIME* pRecordPolicy)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordPolicy, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Record Policy By Time Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordService().GetRecordPolicy(pCameraCode,pRecordPolicy);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//设置录像空间
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetRecordPolicyByTime(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,const IVS_RECORD_POLICY_TIME* pRecordSpaceInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordSpaceInfo, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Set RecordPolicy By Time Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetRecordService().SetRecordPolicy(pCameraCode,pRecordSpaceInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//查看告警订阅列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SubscribeAlarmQuery(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s",iSessionID,pReqXml);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAlarmMgr().SubscribeQuery(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//新增,修改,取消告警订阅
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SubscribeAlarm(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s",iSessionID,pReqXml);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAlarmMgr().SubscribeAlarm(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//查询自定义告警类型列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmTypeList(IVS_INT32 iSessionID, 
                                                          const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
                                                          IVS_ALARM_TYPE_LIST* pAlarmTypeList, 
                                                          IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %u",iSessionID,uiBufferSize);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery,IVS_PARA_INVALID);
    CHECK_POINTER(pAlarmTypeList,IVS_PARA_INVALID);
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetAlarmTypeList(pUnifiedQuery, pAlarmTypeList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufferSize=%u",uiBufferSize);
	return iRet;
}

//设置告警类型级别
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetAlarmTypeLevel(IVS_INT32 iSessionID,  
                                                           const IVS_ALARM_TYPE_LIST* pAlarmTypeList)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    if (NULL == pAlarmTypeList)
    {
        return IVS_PARA_INVALID;
    }

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAlarmMgr().SetAlarmTypeLevel(pAlarmTypeList);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//查询告警级别列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLevelList(IVS_INT32 iSessionID,IVS_UINT32 uiMaxAlarmLevelNum,IVS_UINT32* puiAlarmLevelNum,IVS_ALARM_LEVEL* pAlarmLevel,IVS_UINT32 uiBufferSize)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmLevel, IVS_PARA_INVALID);
	CHECK_POINTER(puiAlarmLevelNum, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,MaxAlarmLevelNum: %u,AlarmLevelNum: %u,BufferSize: %u",iSessionID,uiMaxAlarmLevelNum,*puiAlarmLevelNum,uiBufferSize);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    if (NULL == pUserMgr)
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    IVS_INT32 iRet = pUserMgr->GetAlarmLevelMgr().GetAlarmLevelList(uiMaxAlarmLevelNum, puiAlarmLevelNum, pAlarmLevel, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"MaxAlarmLevelNum=%u,AlarmLevelNum=%u,BufferSize=%u",uiMaxAlarmLevelNum,*puiAlarmLevelNum,uiBufferSize);
	return iRet;
}

// 新增防区
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddAlarmArea(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s",iSessionID,pReqXml);

    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    if (NULL == pUserMgr)
    {
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
     
    IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().AddAlarmArea(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//修改防区
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAlarmArea(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs,  IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s", iSessionID,pReqXml);	

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

    IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().ModifyAlarmArea(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//删除防区
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAlarmArea(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,uiAlarmAreaId: %u", iSessionID, uiAlarmAreaId);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().DeleteAlarmArea(pDomainCode, uiAlarmAreaId);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmAreaId=%u",uiAlarmAreaId);
	return iRet;
}

//查看防区列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmAreaList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d", iSessionID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_PARA_INVALID);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().GetAlarmAreaList(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}

// 查看防区详情
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmAreaInfo (IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId,IVS_CHAR** pAlarmArea)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,pDomainCode: %s, AlarmAreaId:%u", iSessionID, pDomainCode, uiAlarmAreaId);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet =  pUserMgr->GetAlarmAreaMgr().GetAlarmAreaInfo(pDomainCode, uiAlarmAreaId, pAlarmArea);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,AlarmAreaId=%u",pDomainCode,uiAlarmAreaId);
	return iRet;
}

// 手动布防
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartAlarmAreaGuard(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode,IVS_UINT32 uiAlarmAreaId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("SessionID: %d,pDomainCode: %s, AlarmAreaId:%u", iSessionID, pDomainCode, uiAlarmAreaId);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().StartAlarmAreaGuard(pDomainCode, uiAlarmAreaId);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,AlarmAreaId=%u",pDomainCode,uiAlarmAreaId);
	return iRet;
}

// 手动撤防
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopAlarmAreaGuard(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	
	IVS_INFO_TRACE("SessionID: %d,pDomainCode: %s, AlarmAreaId:%u", iSessionID, pDomainCode, uiAlarmAreaId);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().StopAlarmAreaGuard(pDomainCode, uiAlarmAreaId);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,AlarmAreaId=%u",pDomainCode,uiAlarmAreaId);
	return iRet;
}

// 新增布防计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AllocAreaGuardPlan (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s", iSessionID,pReqXml);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().AllocAreaGuardPlan(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}

// 查看布防计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAreaGuardPlan (IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, IVS_UINT32 uiAlarmAreaId, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,AlarmAreaID:%u", iSessionID, uiAlarmAreaId);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmAreaMgr().GetAreaGuardPlan(pDomainCode, uiAlarmAreaId, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,AlarmAreaId=%u",pDomainCode,uiAlarmAreaId);
	return iRet;
}

// 新增告警联动策略
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddAlarmLinkage(IVS_INT32 iSessionID,	const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs,  IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s", iSessionID,pReqXml);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().AddAlarmLinkage(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

// 修改告警联动策略
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyAlarmLinkage(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s", iSessionID,pReqXml);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().ModifyAlarmLinkage(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

// 删除告警联动策略
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteAlarmLinkage(IVS_INT32 iSessionID,const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	IVS_INFO_TRACE("SessionID: %d,pReqXml: %s",iSessionID, pReqXml);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().DeleteAlarmLinkage(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//查看告警联动策略列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmlinkageList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,ReqXml: %s", iSessionID,pReqXml);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().GetAlarmlinkageList(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//查询告警联动策略
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLinkage (IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)

{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,pReqXml :%s", iSessionID,pReqXml);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().GetAlarmLinkage(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

//告警确认
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmConfirm(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,AlarmEventID: %llu", iSessionID,ullAlarmEventID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmOperateInfo, IVS_PARA_INVALID);
	CHECK_POINTER(pAlarmInCode, IVS_OPERATE_MEMORY_ERROR);
	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().ConfirmAlarm(ullAlarmEventID, pAlarmInCode, pAlarmOperateInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmEventID=%llu,AlarmInCode=%s",ullAlarmEventID,pAlarmInCode);
	return iRet;
}

//告警撤销
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmCancel(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,AlarmEventID: %llu", iSessionID,ullAlarmEventID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmOperateInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmInCode, IVS_OPERATE_MEMORY_ERROR);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().CancelAlarm(ullAlarmEventID, pAlarmInCode, pAlarmOperateInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmEventID=%llu,AlarmInCode=%s",ullAlarmEventID,pAlarmInCode);
	return iRet;
}

//告警授权
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmCommission (IVS_INT32 iSessionID, const IVS_CHAR* pAlarmInCode, const IVS_ALARM_COMMISSION* pAlarmCommission)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d", iSessionID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmCommission, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmInCode, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAlarmMgr().AlarmCommission(pAlarmInCode, pAlarmCommission);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//授权告警用户认证
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UserAuthentication(IVS_INT32 iSessionID, const IVS_CHAR *pPWD)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ", iSessionID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pPWD, IVS_PARA_INVALID);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().UserAlarmAuthorization(pPWD);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 告警信息上报（告警推送）
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AlarmReport(IVS_INT32 iSessionID, const IVS_ALARM_REPORT* pAlarmReport)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d", iSessionID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmReport, IVS_PARA_INVALID);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().AlarmReport(pAlarmReport);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//启动开关量输出告警
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartAlarmOut(IVS_INT32 iSessionID, const IVS_CHAR* pAlarmOutCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmOutCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,AlarmOutCode: %s", iSessionID,pAlarmOutCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().StartAlarmOut(pAlarmOutCode);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmOutCode=%s",pAlarmOutCode);
	return iRet;
}

//停止开关量输出告警
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopAlarmOut(IVS_INT32 iSessionID, const IVS_CHAR* pAlarmOutCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmOutCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,AlarmOutCode: %s", iSessionID,pAlarmOutCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().StopAlarmOut(pAlarmOutCode);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmOutCode=%s",pAlarmOutCode);
	return iRet;
}

//查询告警信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmEventInfo(IVS_INT32 iSessionID, IVS_UINT64 ullAlarmEventID, const IVS_CHAR* pAlarmInCode, IVS_ALARM_EVENT* pAlarmEvent)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,AlarmEventID: %llu",iSessionID,ullAlarmEventID);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmInCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pAlarmEvent, IVS_OPERATE_MEMORY_ERROR);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetAlarmInfo(ullAlarmEventID, pAlarmInCode, pAlarmEvent);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmEventID: %llu,AlarmInCode=%s",ullAlarmEventID,pAlarmInCode);
	return iRet;
}
// 查询历史告警信息列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmEventList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %u", iSessionID,uiBufferSize);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmEventList, IVS_OPERATE_MEMORY_ERROR);
	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetAlarmList( pUnifiedQuery, pAlarmEventList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufferSize=%u",uiBufferSize);
	return iRet;
}

//用户登录
IVS_INT32 __SDK_CALL IVS_SDK_Login(IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("");
    CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);
    
	*pSessionId = SDK_SESSIONID_INIT; // 初始化sessionId
    IVS_INT32 iRet = g_pNvs->Login(pLoginReqInfo, pSessionId);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"SerIP=[%s:%d],DomainName=%s, MachineName=%s, ClientType=%d, LoginType=%d",
		pLoginReqInfo->stIP.cIP, pLoginReqInfo->uiPort, pLoginReqInfo->cDomainName, 
		pLoginReqInfo->cMachineName, pLoginReqInfo->uiClientType, pLoginReqInfo->uiLoginType);
	return iRet;
}//lint !e818


//用户单点登录;
IVS_INT32 __SDK_CALL IVS_SDK_LoginByTicket(IVS_LOGIN_INFO_EX* pLoginReqInfoEx, IVS_INT32* pSessionId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_DEBUG_TRACE("TicketID: %s, SerIP: [%s : %d], DomainName: %s, MachineName: %s, ClientType: %d, LoginType: %d", 
		pLoginReqInfoEx->cTicketID, pLoginReqInfoEx->stIP.cIP, pLoginReqInfoEx->uiPort, pLoginReqInfoEx->cDomainName, 
		pLoginReqInfoEx->cMachineName, pLoginReqInfoEx->uiClientType, pLoginReqInfoEx->uiLoginType);
	CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pLoginReqInfoEx, IVS_OPERATE_MEMORY_ERROR);



	*pSessionId = SDK_SESSIONID_INIT; // 初始化sessionId;
	IVS_INT32 iRet = g_pNvs->LoginByTicket(pLoginReqInfoEx, pSessionId);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"SerIP=[%s:%d],DomainName=%s, MachineName=%s, ClientType=%d, LoginType=%d",
		pLoginReqInfoEx->stIP.cIP, pLoginReqInfoEx->uiPort, pLoginReqInfoEx->cDomainName, 
		pLoginReqInfoEx->cMachineName, pLoginReqInfoEx->uiClientType, pLoginReqInfoEx->uiLoginType);
	return iRet;
}


//用户登录
IVS_INT32 __SDK_CALL IVS_SDK_WinUserLogin(IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("");

	CHECK_POINTER(pSessionId, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pLoginReqInfo, IVS_OPERATE_MEMORY_ERROR);

	IVS_INT32 iRet = g_pNvs->WinUserLogin(pLoginReqInfo, pSessionId);

	*pSessionId = SDK_SESSIONID_INIT; // 初始化sessionId
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"SerIP=[%s:%d],DomainName=%s, MachineName=%s, ClientType=%d, LoginType=%d",
		pLoginReqInfo->stIP.cIP, pLoginReqInfo->uiPort, pLoginReqInfo->cDomainName, 
		pLoginReqInfo->cMachineName, pLoginReqInfo->uiClientType, pLoginReqInfo->uiLoginType);	
	return iRet;
}//lint !e818

//获取登陆用户的用户ID
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetUserID(IVS_INT32 iSessionID, IVS_UINT32* puiUserID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
  	IVS_INFO_TRACE("SessionID: %d", iSessionID);    
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(puiUserID, IVS_OPERATE_MEMORY_ERROR);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

    IVS_UINT32 uiUserID = pUserMgr->GetLoginMgr().GetUserID();
    *puiUserID = uiUserID;   

	IVS_INT32 iRet = IVS_SUCCEED;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//获取登录响应信息
IVS_INT32 __SDK_CALL IVS_SDK_Logout( IVS_INT32 iSessionID )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	IVS_INFO_TRACE("SessionID: %d", iSessionID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	int iRet = g_pNvs->Logout(iSessionID);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 删除设备组的摄像机
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteDeviceFromGroup( IVS_INT32 iSessionID, 
                                                                const IVS_CHAR* pTargetDomainCode,
                                                                IVS_UINT32 uiDeviceNum,
                                                                const IVS_DEV_GROUP_CAMERA* pGroupCameraList)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGroupCameraList, IVS_PARA_INVALID);
	CHECK_POINTER(pTargetDomainCode, IVS_PARA_INVALID);

 	IVS_INFO_TRACE("SessionID: %d,TargetDomainCode: %s,DeviceNum: %u",iSessionID,pTargetDomainCode,uiDeviceNum);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().DeleteDeviceFromGroup(pTargetDomainCode, uiDeviceNum, pGroupCameraList);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"TargetDomainCode=%s,DeviceNum=%u",pTargetDomainCode,uiDeviceNum);
	return iRet;
}

//增加设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDevice(IVS_INT32 iSessionID, 
                                                   const IVS_CHAR* pDomainCode,
                                                   IVS_UINT32 uiDeviceNum,
                                                   const IVS_DEVICE_OPER_INFO* pDevList,
                                                   IVS_DEVICE_OPER_RESULT_LIST* pResultList,
                                                   IVS_UINT32 uiResultBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDevList, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,TargetDomainCode: %s,DeviceNum: %u,ResultBufSize: %u",
		           iSessionID,pDomainCode,uiDeviceNum,uiResultBufferSize);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().AddMulDevice(pDomainCode, uiDeviceNum, pDevList, pResultList, uiResultBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,DeviceNum=%u",pDomainCode,uiDeviceNum);
	return iRet;
}

//更改主设备，子设备参数
 IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDeviceConfig(IVS_INT32 iSessionID,
                                                          const IVS_CHAR* pDevCode,
                                                          IVS_UINT32 uiConfigType,
                                                          IVS_VOID* pBuffer,
                                                          IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
    IVS_INFO_TRACE("SessionID: %d,DevCode: %s,ConfigType: %u,BufSize: %u", iSessionID,pDevCode,uiConfigType,uiBufSize);
    CHECK_POINTER(pBuffer, IVS_PARA_INVALID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceParaConfig().SetDeviceConfig(pDevCode, uiConfigType, pBuffer, uiBufSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s,uiConfigType=%u",pDevCode,uiConfigType);
	return iRet;
}//lint !e818

//更改主设备，子设备参数
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceConfig(IVS_INT32 iSessionID,
                                                        const IVS_CHAR* pDevCode, 
                                                        IVS_UINT32 uiConfigType,
                                                        IVS_VOID* pBuffer,
                                                        IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevCode: %s,ConfigType: %u,BufSize: %u",iSessionID,pDevCode,uiConfigType,uiBufSize);

 	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceParaConfig().GetDeviceConfig(pDevCode, uiConfigType, pBuffer, uiBufSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s,uiConfigType=%u",pDevCode,uiConfigType);
	return iRet;
}

//删除设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteDevice(IVS_INT32 iSessionID,const IVS_CHAR* pDomainCode,IVS_UINT32 uiDeviceNum,
	const IVS_DEVICE_CODE* pDevCodeList, IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)	
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCodeList,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,DeviceNum: %u,BufSize: %u", iSessionID,pDomainCode,uiDeviceNum,uiResultBufferSize);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().DeleteDevice(pDomainCode,uiDeviceNum,pDevCodeList,pResultList,uiResultBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,uiDeviceNum=%u",pDomainCode,uiDeviceNum);
	return iRet;
}

//查询设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryDeviceList(IVS_INT32 iSessionID, 
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiDeviceType, 
	const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, 
	IVS_VOID* pBuffer, 
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pQueryFormat, IVS_PARA_INVALID);
    CHECK_POINTER(pBuffer, IVS_PARA_INVALID);
    CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
    CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,DeviceType: %u,BufSize: %u", iSessionID,pDomainCode,uiDeviceType,uiBufferSize);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().QueryDeviceList(uiDeviceType, pDomainCode,pQueryFormat, pBuffer, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,DeviceType=%u,BufSize=%u",pDomainCode,uiDeviceType,uiBufferSize);
	return iRet;
}
//获取获取设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceList(IVS_INT32 iSessionID,
	                                                   IVS_UINT32 uiDeviceType,
	                                                   const IVS_INDEX_RANGE* pIndexRange,
	                                                   IVS_VOID* pDeviceList,
	                                                   IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevType: %u,BufSize: %u",iSessionID,uiDeviceType,uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetDeviceList(uiDeviceType, pIndexRange, pDeviceList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceType=%u,BufSize=%u",uiDeviceType,uiBufferSize);
	return iRet;
}

//新增设备组
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDeviceGroup (IVS_INT32 iSessionID, 
	                                                     const IVS_DEV_GROUP* pDevGroup,
  	                                                     IVS_UINT32* puiDevGroupID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
    CHECK_POINTER(pDevGroup, IVS_PARA_INVALID);
    CHECK_POINTER(puiDevGroupID, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevGroupID: %u", iSessionID,*puiDevGroupID);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().AddDeviceGroup(pDevGroup, puiDevGroupID);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}


//查询设备组列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceGroup(IVS_INT32 iSessionID,
                                                        const IVS_CHAR* pDomainCode,
                                                        const IVS_CHAR* pDevGroupCode,
                                                        IVS_DEVICE_GROUP_LIST* pDeviceGroupList,
                                                        IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDevGroupCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceGroupList, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,GroupID: %s,BufSize: %u",iSessionID,pDomainCode,pDevGroupCode,uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().GetDeviceGroup(pDomainCode, pDevGroupCode, pDeviceGroupList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,GroupID=%s,BufSize=%u",pDomainCode,pDevGroupCode,uiBufferSize);
	return iRet;
}


//开始实时浏览
IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlay( IVS_INT32 iSessionID, IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, HWND hWnd, IVS_ULONG* pHandle )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
    
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlay(pRealplayPara, pCameraCode, hWnd, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}
//开始实时浏览
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBRaw(IVS_INT32 iSessionID, 
	                                                        IVS_REALPLAY_PARAM* pRealplayPara, 
	                                                        const IVS_CHAR* pCameraCode, 
	                                                        RealPlayCallBackRaw fRealPlayCallBackRaw, 
	                                                        void* pUserData, 
	                                                        IVS_ULONG* pHandle )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s", iSessionID, pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlayCBRaw(pRealplayPara, pCameraCode, fRealPlayCallBackRaw, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}
//开始实时浏览
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBFrame( IVS_INT32 iSessionID, IVS_REALPLAY_PARAM* pRealplayPara, const IVS_CHAR* pCameraCode, RealPlayCallBackFrame fRealPlayCallBackFrame, void* pUserData, IVS_ULONG* pHandle )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID, pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlayCBFrame(pRealplayPara, pCameraCode, fRealPlayCallBackFrame, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//停止实时浏览
IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlay( IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StopRealPlay(ulPlayHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPlayWnd(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pRealPlay)
	{
		BP_RUN_LOG_INF("real play zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().AddPlayWindow(ulPlayHandle, hWnd);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	CPlayback *pPlayback = dynamic_cast<CPlayback *>(pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pPlayback)
	{
		BP_RUN_LOG_INF("paly back zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().AddPlayWindow(ulPlayHandle, hWnd);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	IVS_INT32 iRet = IVS_FAIL;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelPlayWnd(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pRealPlay)
	{
		BP_RUN_LOG_INF("real play zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().DelPlayWindow(ulPlayHandle, hWnd);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	CPlayback *pPlayback = dynamic_cast<CPlayback *>(pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pPlayback)
	{
		BP_RUN_LOG_INF("paly back zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().DelPlayWindow(ulPlayHandle, hWnd);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	IVS_INT32 iRet = IVS_FAIL;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPlayWindowPartial(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, void *pPartial)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pRealPlay)
	{
		BP_RUN_LOG_INF("real play zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().AddPlayWindowPartial(ulPlayHandle, hWnd, pPartial);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	CPlayback *pPlayback = dynamic_cast<CPlayback *>(pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pPlayback)
	{
		BP_RUN_LOG_INF("paly back zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().AddPlayWindowPartial(ulPlayHandle, hWnd, pPartial);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}

	IVS_INT32 iRet = IVS_FAIL;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_UpdateWindowPartial(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, void *pPartial)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	CRealPlay *pRealPlay = dynamic_cast<CRealPlay *>(pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pRealPlay)
	{
		BP_RUN_LOG_INF("real play zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().UpdateWindowPartial(ulPlayHandle, hWnd, pPartial);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	CPlayback *pPlayback = dynamic_cast<CPlayback *>(pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle));
	if (NULL != pPlayback)
	{
		BP_RUN_LOG_INF("paly back zoom","NA");
		IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().UpdateWindowPartial(ulPlayHandle, hWnd, pPartial);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	IVS_INT32 iRet = IVS_FAIL;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

//设置实况播放模式：清晰、流畅
IVS_INT32 __SDK_CALL IVS_SDK_SetPlayQualityMode(IVS_INT32 iSessionID, IVS_ULONG ulHandle,IVS_INT32 iPlayQualityMode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,Handle: %lu,PlayQualityMode: %d",iSessionID,ulHandle,iPlayQualityMode);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

    IVS_INT32  iRet = IVS_FAIL;
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulHandle);
	if(NULL != mediaBase)
	{
		unsigned int port = pUserMgr->GetRealPlayMgr().GetPlayerChannelByHandle(ulHandle);
		iRet = mediaBase->SetPlayQualityMode(port,iPlayQualityMode);
	}
	
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu,PlayQualityMode=%d",ulHandle,iPlayQualityMode);
	return iRet;
}

static std::string GetPlayBackCurrentTime(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
    //时间格式为毫秒级别，除以1000转化为秒
    IVS_UINT64 tickNow = 0;
    (void)IVS_SDK_GetCurrentFrameTick(iSessionID, ulHandle, &tickNow);
    time_t stTime_t = tickNow / 1000; //lint !e713

    std::string strCurrentTime("");
    //获取格式化的string Time
    CMediaBase::UTCTime2Str(stTime_t, strCurrentTime, IVS_DATE_TIME_FORMAT);
    BP_DBG_LOG("IVS_SDK_SetWaterMarkVerify, strCurrentTime:%s.", strCurrentTime.c_str());
    return strCurrentTime;
}

static void BuildUpdateWaterAlarmXml(const std::string strUserIdDomainCode, const std::string strDomianCode,
                                     IVS_ALARM_WATERMARK_INFO &stWaterMarkInfo, std::string& strReq)
{
    CXml xml;
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("LoginInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("UserIDDomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(strUserIdDomainCode.c_str());
    xml.OutOfElem();

    (void)xml.AddElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(strDomianCode.c_str());
    (void)xml.AddElem("AlarmEventID");
    (void)xml.IntoElem();
    
    (void)xml.SetElemValue(CToolHelp::IntToStr(stWaterMarkInfo.ullAlarmEventID).c_str());
    (void)xml.AddElem("OccurStopTime");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(stWaterMarkInfo.cEndTime);
    xml.OutOfElem();
    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
    if (NULL != pReq)
    {
        strReq = pReq;
    }
    
    return;
}

static IVS_INT32 SendWaterAlarmToSmu(IVS_INT32 iSessionID, IVS_ULONG ulHandle, ST_WATERMARK_ALARM_INFO &stWaterMarkAlarmInfo, std::string& strRsp)
{
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_PARA_INVALID);

    //解析获取CameraCode、DomainCode
    std::string strCameraID("");   
    std::string strCameraCode("");
    std::string strDomainCode("");
    CMediaBase* pMediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(ulHandle);
    if (NULL != pMediaBase)
    {
        pMediaBase->GetCameraID(strCameraID);

        IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCameraID, strCameraCode, strDomainCode);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet,"DO Player Exception", "SplitCameraCode failed, RetCode=%d.", iRet);

			eSDK_LOG_RUN_WRAN("SplitCameraCode failed.");
            return IVS_FAIL;
        }
    }
    
    //获取用户本域编码，用户所在DomainCode
    std::string strUserDomainCode(""); 
    pUserMgr->GetDomainCode(strUserDomainCode);
    
    //生成XML告诉SMU入库数据
    CSendNssMsgInfo sendNssMsgInfo;	
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetCameraCode(strCameraCode);
    sendNssMsgInfo.SetDomainCode(strDomainCode);

    sendNssMsgInfo.SetReqID(NSS_WATERMARK_ALARM_UPDATE_REQ);
    std::string strReq("");
    //组装发送SMU的XML
    BuildUpdateWaterAlarmXml(strUserDomainCode, strDomainCode, stWaterMarkAlarmInfo.stWaterMarkInfo, strReq);
    BP_DBG_LOG("BuildUpdateWaterAlarmXml, strReq：%s", strReq.c_str());

    sendNssMsgInfo.SetReqData(strReq);
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    IVS_INT32 iRet = pUserMgr->SendCmd(sendNssMsgInfo, strRsp, iNeedRedirect);

	return iRet;
}

//设置开启/停止校验水印
IVS_INT32 __SDK_CALL IVS_SDK_SetWaterMarkVerify(IVS_INT32 iSessionID, IVS_ULONG ulHandle,IVS_BOOL bIsStart)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);	

	IVS_INFO_TRACE("SessionID: %d, Handle: %lu, IsStart: %d",iSessionID, ulHandle, (int)bIsStart);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

    IVS_INT32  iRet = IVS_FAIL;
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	
	//只对回放做水印校验
	CMediaBase* mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(ulHandle);
	if (NULL != mediaBase)
	{
		unsigned int port = pUserMgr->GetPlaybackMgr().GetPlayerChannelByHandle(ulHandle);
		iRet = mediaBase->SetWaterMarkVerify(port, bIsStart);
        BP_DBG_LOG("SetWaterMarkVerify, iRet:%d", iRet);
	}
	
    if ((IVS_SUCCEED == iRet) && (1 == bIsStart))
    {
        ST_WATERMARK_ALARM_INFO stWaterMarkAlarmInfo;
        eSDK_MEMSET(&stWaterMarkAlarmInfo, 0, sizeof(stWaterMarkAlarmInfo));  

        //获取水印数据信息
        pUserMgr->GetWaterMarkInfo(ulHandle, stWaterMarkAlarmInfo);
        BP_DBG_LOG("IVS_SDK_SetWaterMarkVerify, GetWaterMarkInfo, ulHandle:%d, cCameraCode:%s, cStartTime:%s, ullAlarmEventID:%d",
            ulHandle, stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, 
            stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID);

        //获取当前录像播放时间
        std::string strTime = GetPlayBackCurrentTime(iSessionID, ulHandle);
        BP_DBG_LOG("IVS_SDK_SetWaterMarkVerify, strTime:%s.", strTime.c_str());

        std::string strRsp("");
        //判断如果停止水印校验并且需要上报水印告警的结束时间，则发送告警信息给SMU并上报给OCX->CU
        if (/*(1 == bIsStart) && */(stWaterMarkAlarmInfo.bIsNeedReport))
        {
			(void)CToolsHelp::Memcpy(stWaterMarkAlarmInfo.stWaterMarkInfo.cEndTime, IVS_TIME_LEN, strTime.c_str(), strTime.length());
            //组装发送给SMU的水印告警信息      
            iRet = SendWaterAlarmToSmu(iSessionID, ulHandle, stWaterMarkAlarmInfo, strRsp);
            //如果SMU响应成功则上报给OCX
            if (IVS_SUCCEED == iRet)
            {
                pUserMgr->RemoveWaterMarkInfoFromMap(ulHandle);

                IVS_ALARM_WATERMARK_INFO stWaterMarkInfo;
                eSDK_MEMSET(&stWaterMarkInfo, 0, sizeof(stWaterMarkInfo));

                eSDK_MEMCPY(stWaterMarkInfo.cCameraCode, sizeof(stWaterMarkInfo.cCameraCode), stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, sizeof(stWaterMarkInfo.cCameraCode));
                eSDK_MEMCPY(stWaterMarkInfo.cStartTime, sizeof(stWaterMarkInfo.cStartTime), stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, sizeof(stWaterMarkInfo.cStartTime));

				(void)CToolsHelp::Strncpy(stWaterMarkInfo.cEndTime, sizeof(stWaterMarkInfo.cEndTime), strTime.c_str(), strTime.length());
                stWaterMarkInfo.ullAlarmEventID = stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID;

                BP_DBG_LOG("IVS_SDK_SetWaterMarkVerify>>>>>>, strCameraCode:%s, cStartTime:%s, strTime:%s, ullAlarmEventID:%d", 
                    stWaterMarkInfo.cCameraCode, stWaterMarkInfo.cStartTime, stWaterMarkInfo.cEndTime,
                    stWaterMarkInfo.ullAlarmEventID);

                char Buf[sizeof(IVS_ALARM_WATERMARK_INFO) + sizeof(unsigned long)];
                IVS_ALARM_WATERMARK_INFO *recordInfo = (IVS_ALARM_WATERMARK_INFO *)(Buf);
                unsigned long *pHandle = (unsigned long *)(recordInfo + 1);//lint !e826
                *pHandle = ulHandle;

                (void)CToolsHelp::Memcpy(recordInfo, sizeof(IVS_ALARM_WATERMARK_INFO), &stWaterMarkInfo, sizeof(IVS_ALARM_WATERMARK_INFO));

                BP_DBG_LOG("IVS_SDK_SetWaterMarkVerify, pHandle:%d, ulHandle:%d",*pHandle, ulHandle);

                (void)CEventCallBackJob::instance().PostEvent(iSessionID, IVS_EVENT_REPORT_WATERMARK_ALARM, Buf, sizeof(Buf)+1);
            }
            else
            {
                //记录日志
                BP_RUN_LOG_ERR(IVS_FAIL, "Send WaterAlarm To SMU Fail.", "EventType:[%d]", IVS_EVENT_REPORT_WATERMARK_ALARM);
				eSDK_LOG_RUN_WRAN("Send WaterAlarm To SMU Fail.");
            }

        }
    }
    SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu, IsStart=%d",ulHandle, (int)bIsStart);
	return iRet;
}

//设置宽屏显示
IVS_INT32 __SDK_CALL IVS_SDK_SetDisplayScale(IVS_INT32 iSessionID,IVS_ULONG ulHandle,IVS_INT32 iDisplayScale)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,Handle: %lu,DisplayScale: %d",iSessionID,ulHandle,iDisplayScale);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	unsigned int uiPort = 0;
	CMediaBase* mediaBase = GetMediaBaseAndPortInfo(iSessionID,ulHandle,uiPort);
	IVS_INT32 iRet = IVS_FAIL;
	if (NULL != mediaBase)
	{
		iRet = mediaBase->SetDisplayScale(uiPort,static_cast<IVS_UINT32>(iDisplayScale));
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu,DisplayScale=%d",ulHandle,iDisplayScale);
	return iRet;
}
//查询操作日志。
 IVS_INT32 __SDK_CALL IVS_SDK_GetOperationLog(IVS_INT32 iSessionID,
	                                         const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
	                                         IVS_OPERATION_LOG_LIST* pResOperLogList,
	                                         IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %u",iSessionID,uiBufSize);
    CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
	CHECK_POINTER(pResOperLogList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs,IVS_OPERATE_MEMORY_ERROR );
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetLogMgr().GetOperationLog(pUnifiedQuery,pResOperLogList,uiBufSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%u",uiBufSize);
	return iRet;
 }


 //扩展查询操作日志
 IVS_INT32 __SDK_CALL IVS_SDK_GetOperationLogEx(IVS_INT32 iSessionID,
	 const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
	 const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
	 IVS_OPERATION_LOG_LIST* pResOperLogList,
	 const IVS_UINT32 uiBufSize)
 {
	 SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	 
	 CHECK_POINTER(pUnifiedQuery, IVS_PARA_INVALID);
	 CHECK_POINTER(pResOperLogList, IVS_PARA_INVALID);
	 CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pOptLogQueryEx, IVS_PARA_INVALID);
	 IVS_INFO_TRACE("SessionID: %d, NVRCode: %s, ModuleName: %s", 
		 iSessionID, pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName);

	 CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	 CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	 IVS_INT32 iRet = pUserMgr->GetLogMgr().GetOperationLogEx(pUnifiedQuery, pOptLogQueryEx, pResOperLogList, uiBufSize);
	 SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	 INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"NVRCode=%s,ModuleName=%s,BufSize=%u",pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName,uiBufSize);
	 return iRet;
 }


IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBack(IVS_INT32 iSessionID,
                                                                const IVS_CHAR* pCameraCode,
                                                                const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                HWND hWnd,
                                                                IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBack(pCameraCode, pPlaybackParam, hWnd, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//停止平台录像回放
IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBack(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

    ST_WATERMARK_ALARM_INFO stWaterMarkInfo;
    eSDK_MEMSET(&stWaterMarkInfo, 0, sizeof(stWaterMarkInfo));
    pUserMgr->GetWaterMarkInfo(ulPlayHandle, stWaterMarkInfo);

    //如果水印校验开启中，则关闭水印校验
    if (stWaterMarkInfo.bIsOpenedWater)
    {
        IVS_INT32 iRet = IVS_SDK_SetWaterMarkVerify(iSessionID, ulPlayHandle, 1);
        BP_DBG_LOG("Stop Platform PlayBack, Stop WaterMark Verify. Retcode:%d", iRet);
        //停100ms，防止所在的播放句柄已经被注销
        Sleep(100);
    }

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StopPlatformPlayBack(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackByIP(IVS_INT32 iSessionID,
																const IVS_CHAR* pCameraCode,
																const IVS_PLAYBACK_PARAM* pPlaybackParam,
																const IVS_MEDIA_ADDR* pMediaAddrDst,
																IVS_MEDIA_ADDR* pMediaAddrSrc,
																IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrDst, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrSrc, IVS_PARA_INVALID);
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, CameraCode: %s, IP: %s, VideoPort: %u, AudioPort: %u",
		iSessionID, pCameraCode, pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_ULONG ulHandle = *pHandle;
	IVS_MEDIA_ADDR stMediaAddr = {0};
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StartPlatformPlayBackByIP(pCameraCode, *pPlaybackParam, *pMediaAddrDst, stMediaAddr, ulHandle);
	*pHandle = ulHandle;
	bool bSucc = CToolsHelp::Memcpy(pMediaAddrSrc, sizeof(IVS_MEDIA_ADDR), &stMediaAddr, sizeof(IVS_MEDIA_ADDR));
	if(!bSucc)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Start Platform PlayBack ByIP", "Memcpy failed.");
		eSDK_LOG_RUN_WRAN("Memcpy failed.");
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,IP=%s,VideoPort=%u,AudioPort=%u",pCameraCode, pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBackByIP(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Handle: %lu",iSessionID, ulPlayHandle);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StopPlayByIP(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

//暂停平台录像回放
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlatformPlayBackPauseByIP(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Handle: %lu",iSessionID, ulPlayHandle);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().PlatformPlayBackPauseByIP(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}


//恢复平台录像回放
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlatformPlayBackResumeByIP(IVS_INT32 iSessionID,
	IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Handle: %lu",iSessionID, ulPlayHandle);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().PlatformPlayBackResumeByIP(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

//设置平台录像码流回放速度
IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackSpeedByIP(IVS_INT32 iSessionID, 
	IVS_ULONG ulPlayHandle, 
	IVS_FLOAT fSpeed)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	IVS_INFO_TRACE("SessionID: %d, PlayHandle: %lu, Speed:%f", iSessionID, ulPlayHandle, fSpeed);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().SetPlayBackSpeedByIP(ulPlayHandle, fSpeed);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,Speed=%f",ulPlayHandle,fSpeed);
	return iRet;
}

//设置平台录像码流回放速度
IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackSpeedByIPnTime(IVS_INT32 iSessionID, 
	IVS_ULONG ulPlayHandle, 
	IVS_FLOAT fSpeed, IVS_UINT64 uiOffsetTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, PlayHandle: %lu, Speed:%f, OffsetTime: %lu", iSessionID, ulPlayHandle, fSpeed, uiOffsetTime);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().SetPlayBackSpeedByIPnTime(ulPlayHandle, fSpeed, uiOffsetTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,Speed=%f, OffsetTime: %lu",ulPlayHandle,fSpeed, uiOffsetTime);
	return iRet;
}


//设置平台录像码流回放时间
IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackTimeByIP(IVS_INT32 iSessionID, 
	IVS_ULONG ulPlayHandle, 
	IVS_UINT32 uiTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu Time:%d", iSessionID, ulPlayHandle, uiTime);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().SetPlayBackTimeByIP(ulPlayHandle, uiTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,Time=%d",ulPlayHandle,uiTime);
	return iRet;
}


// 停止容灾录像回放
IVS_SDK_API IVS_INT32  __SDK_CALL  IVS_SDK_StopDisasterRecoveryPlayBack(
    IVS_INT32 iSessionID,
    IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
    IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StopDisasterRecoveryPlayBack(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

//开始平台录像回放，以回调方式输出拼帧后的裸码流。
IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBRaw(IVS_INT32 iSessionID,
														const IVS_CHAR* pCameraCode,
														const IVS_PLAYBACK_PARAM* pPlaybackParam,
														PlayBackCallBackRaw fPlayBackCallBackRaw, 
														void* pUserData,
														IVS_ULONG* pHandle)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(fPlayBackCallBackRaw, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack CBRaw SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackCB(pCameraCode, pPlaybackParam, fPlayBackCallBackRaw, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}//lint !e818

//开始平台录像回放，以回调方式输出YUV流。
IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBFrame(IVS_INT32 iSessionID,
	 													  const IVS_CHAR* pCameraCode,
														  const IVS_PLAYBACK_PARAM* pPlaybackParam,
														  PlayBackCallBackFrame fPlayBackCallBackFrame, 
														  void* pUserData,
														  IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(fPlayBackCallBackFrame, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack CBFrame SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackCBFrame(pCameraCode, pPlaybackParam, fPlayBackCallBackFrame, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}//lint !e818

#ifdef R5C20SPC100
//开始平台录像回放，以回调方式输出YUV流。
IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBFrameEx(IVS_INT32 iSessionID,
	const IVS_CHAR* pCameraCode,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	DisplayCallBackFrame fDisplayCallBack, 
	void* pUserData,
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(fDisplayCallBack, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack CBFrame SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackCBFrameEx(pCameraCode, pPlaybackParam, fDisplayCallBack, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}//lint !e818
#endif

// 开始本地录像回放
IVS_INT32 __SDK_CALL IVS_SDK_StartLocalPlayBack (IVS_INT32 iSessionID, const IVS_CHAR* pFileName, 
	           const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam, HWND hWnd, IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("SessionID: %d, pFileName: %s",iSessionID, pFileName);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartLocalPlayBack(pFileName, pPlaybackParam, hWnd, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"FileName=%s",pFileName);
	return iRet;
}

// 停止本地录像回放
IVS_INT32 __SDK_CALL IVS_SDK_StopLocalPlayBack(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StopLocalPlayBack(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}
 

IVS_INT32 __SDK_CALL IVS_SDK_PlayBackPause(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().PlayBackPause(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_PlayBackResume(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().PlayBackResume(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}


IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackStatus(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_BOOL* pbPause)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_BOOL bPause = 0;//初始化值为暂停;
	int iRet = pUserMgr->GetPlaybackMgr().GetPlayBackStatus(ulPlayHandle, bPause);
	*pbPause = bPause;
	BP_RUN_LOG_INF("Get PlayBack Status", "Status[%d] Ret[%d]", bPause, iRet);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_PlayBackFrameStepForward(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID, ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().PlayBackFrameStepForward(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}


IVS_INT32 __SDK_CALL IVS_SDK_PlayBackFrameStepBackward(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID, ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().PlayBackFrameStepBackward(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}
 

IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackSpeed(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_FLOAT fSpeed)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %p Speed:%f", iSessionID, ulPlayHandle, fSpeed);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().SetPlayBackSpeed(ulPlayHandle, fSpeed);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%p,Speed=%f",ulPlayHandle,fSpeed);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackSpeed(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_FLOAT* pfSpeed)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu", iSessionID, ulPlayHandle);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	float fSpeed = 0.0;
	int iRet = pUserMgr->GetPlaybackMgr().GetPlayBackSpeed(ulPlayHandle, fSpeed);
	*pfSpeed = fSpeed;
	BP_RUN_LOG_INF("Get PlayBack Speed", "Speed[%f] Ret[%d]", fSpeed, iRet);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_SetPlayBackTime(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT32 uiTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu,Time: %u",iSessionID,ulPlayHandle,uiTime);
	
	// 支持拖动到开始时间,0 == uiTime的情况合法
	/*if (0 == uiTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS SDK SetPlayBackTime Failed.", "Reason:Param invalid, uiTime = 0");
		return IVS_PARA_INVALID;
	}*/
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().SetPlayBackTime(ulPlayHandle, uiTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,Time=%u",ulPlayHandle,uiTime);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_GetPlayBackTime(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_PLAYBACK_TIME* pPlayBackTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().GetPlayBackTime(ulPlayHandle, pPlayBackTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

//开始前端录像回放
IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBack(IVS_INT32 iSessionID,
                                    const IVS_CHAR* pCameraCode,
                                    const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                    HWND hWnd,
                                    IVS_ULONG* pHandle)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
    CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
    IVS_INFO_TRACE("Start PU Play Back SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);
	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPUPlayBack(pCameraCode, pPlaybackParam, hWnd, pHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);	
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//开始前端录像回放裸码流回调
IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBackCBRaw(IVS_INT32 iSessionID,
	const IVS_CHAR* pCameraCode,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	PlayBackCallBackRaw fPlayBackCallBackRaw, 
	void* pUserData,
	IVS_ULONG* pHandle)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start PU Play Back CBRaw SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPUPlayBackCBRaw(pCameraCode, pPlaybackParam, fPlayBackCallBackRaw, pUserData, pHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);	
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//开始前端录像回放
IVS_INT32 __SDK_CALL IVS_SDK_StopPUPlayBack(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
    IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StopPUPlayBack(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

IVS_INT32 __SDK_CALL IVS_SDK_GetCurrentFrameTick(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, IVS_UINT64* pTick)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().GetCurrentFrameTick(ulPlayHandle, pTick);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

 /*
  * IVS_SDK_StartPlatformDownload
  * 启动平台录像下载
  */
IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformDownload(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,
	                     const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode,    IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDownloadParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pulHandle,      IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, CamCode: %s",iSessionID, pCameraCode);

	// 命名规则
    if (pDownloadParam->uiNameRule < RULE_NAME_NUM_TIME ||
		pDownloadParam->uiNameRule > RULE_TIME_NAME_NUM)
	{
        BP_RUN_LOG_INF("Start Platform Record Download", "Name Rule=[%u]", pDownloadParam->uiNameRule);
		return IVS_PARA_INVALID;
	}

	// 文件格式
    if (pDownloadParam->uiRecordFormat < IVS_FILE_MP4 || 
		pDownloadParam->uiRecordFormat > IVS_FILE_AVI)
	{
        BP_RUN_LOG_INF("Start Platform Record Download", "Record Format=[%u]", pDownloadParam->uiRecordFormat);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型
	if (pDownloadParam->uiSplitterType < RECORD_SPLITE_TIME ||
		pDownloadParam->uiSplitterType > RECORD_SPLITE_CAPACITY)
	{
		BP_RUN_LOG_INF("Start Platform Record Download", "Splitter Type=[%u]", pDownloadParam->uiSplitterType);
		return IVS_PARA_INVALID;
	}
    
	// 文件分割类型值
    if (RECORD_SPLITE_TIME == pDownloadParam->uiSplitterType)
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_TIME_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_TIME_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Platform Record Download", "Splitter Time=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_SPACE_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_SPACE_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Platform Record Download", "Splitter FileSize=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StartPlatformDownLoad(NULL,pDownloadParam->cNVRCode,pCameraCode, pDownloadParam, pulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

/*
* IVS_SDK_StopPlatformDownload
* 停止平台录像下载
*/
IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformDownload(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ulHandle: %lu", iSessionID, ulHandle);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StopPlatformDownLoad(ulHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;	
}

/*
* IVS_SDK_StartPUDownload
* 启动录像下载
*/
IVS_INT32 __SDK_CALL IVS_SDK_StartPUDownload(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode,
	        const IVS_DOWNLOAD_PARAM* pDownloadParam, IVS_ULONG* pulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDownloadParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pulHandle, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, CamID: %s", iSessionID, pCameraCode);

		// 参数校验

	// 命名规则
	if (pDownloadParam->uiNameRule < RULE_NAME_NUM_TIME ||
		pDownloadParam->uiNameRule > RULE_TIME_NAME_NUM)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Name Rule=[%u]", pDownloadParam->uiNameRule);
		return IVS_PARA_INVALID;
	}


	// 文件格式
	if (pDownloadParam->uiRecordFormat < IVS_FILE_MP4 || 
		pDownloadParam->uiRecordFormat > IVS_FILE_AVI)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Record Format=[%u]", pDownloadParam->uiRecordFormat);
		return IVS_PARA_INVALID;
	}


	// 文件分割类型
	if (pDownloadParam->uiSplitterType < RECORD_SPLITE_TIME ||
		pDownloadParam->uiSplitterType > RECORD_SPLITE_CAPACITY)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Splitter Type=[%u]", pDownloadParam->uiSplitterType);
		return IVS_PARA_INVALID;
	}
    
	// 文件分割类型值
	if (RECORD_SPLITE_TIME == pDownloadParam->uiSplitterType)
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_TIME_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_TIME_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start PU Record Download", "Splitter Time=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_SPACE_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_SPACE_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start PU Record Download", "Splitter FileSize=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StartPUDownLoad(pCameraCode, pDownloadParam, pulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

/*
 * IVS_SDK_StopPUDownload
 * 停止前端录像下载
 */
IVS_INT32 __SDK_CALL IVS_SDK_StopPUDownload(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ulHandle: %lu",iSessionID, ulHandle);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StopPUDownLoad(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

/*
 * IVS_SDK_DownloadPause
 */
IVS_INT32 __SDK_CALL IVS_SDK_DownloadPause(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ulHandle: %lu",iSessionID, ulHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().DownLoadPause(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

/*
 * IVS_SDK_DownloadResume
 */
IVS_INT32 __SDK_CALL IVS_SDK_DownloadResume(IVS_INT32 iSessionID,IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ulHandle: %lu",iSessionID, ulHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().DownLoadResume(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}


/*
 * IVS_SDK_GetDownloadInfo
 */
IVS_INT32 __SDK_CALL IVS_SDK_GetDownloadInfo(IVS_INT32 iSessionID, 
		              IVS_ULONG ulHandle,IVS_DOWNLOAD_INFO* pDownloadInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d, ulHandle: %lu",iSessionID, ulHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().GetDownLoadInfo(ulHandle, pDownloadInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}


//获取媒体信息
IVS_INT32 __SDK_CALL IVS_SDK_GetMediaInfo(IVS_INT32 iSessionID, IVS_ULONG ulHandle, IVS_MEDIA_INFO* pMediaInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,Handle: %lu",iSessionID,ulHandle);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pMediaInfo, IVS_OPERATE_MEMORY_ERROR);
	unsigned int uiPort = 0;
	CMediaBase* mediaBase = GetMediaBaseAndPortInfo(iSessionID,ulHandle,uiPort);
	CHECK_POINTER(mediaBase, IVS_OPERATE_MEMORY_ERROR);
	//add by c0020659 获取媒体端地址
	std::string strMediaSourceIP;
	mediaBase->GetMediaSourceIP(strMediaSourceIP);
	if (strMediaSourceIP.size() <= IVS_IP_LEN)
	{
		eSDK_MEMCPY(pMediaInfo->stMediaSourceIP.cIP, sizeof(pMediaInfo->stMediaSourceIP.cIP), strMediaSourceIP.c_str(),strMediaSourceIP.size());
	}
	//add end c0020659 获取媒体端地址
	IVS_INT32 iRet = mediaBase->GetMediaInfo(uiPort,pMediaInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

//查询网元列表
IVS_INT32 __SDK_CALL IVS_SDK_GetNVRList(IVS_INT32 iSessionID, 
		                                const IVS_CHAR* pDomainCode,
		                                IVS_UINT32 uiServerType,
		                                const IVS_INDEX_RANGE* pIndexRange,
		                                IVS_DEVICE_BRIEF_INFO_LIST* pNvrList,
		                                IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,ServiceType: %u,BufSize: %u",iSessionID,pDomainCode,uiServerType,uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetNVRList(pDomainCode,uiServerType,pIndexRange,pNvrList,uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,ServiceType=%u,BufSize=%u",pDomainCode,uiServerType,uiBufferSize);
	return iRet;
}

//移动设备组下设备
IVS_SDK_API IVS_INT32  __SDK_CALL IVS_SDK_ModifyDeviceGroupName(IVS_INT32 iSessionID, 
                                                const IVS_CHAR* pDomainCode,
                                                IVS_UINT32 uiDevGroupID,
                                                const IVS_CHAR* pNewDevGroupName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNewDevGroupName, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,DevGroupID: %u,NewDevGroupName: %s",iSessionID,pDomainCode,uiDevGroupID,pNewDevGroupName);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().ModifyDeviceGroupName(pDomainCode, uiDevGroupID, pNewDevGroupName);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,DevGroupID=%u,NewDevGroupName=%s",pDomainCode,uiDevGroupID,pNewDevGroupName);
	return iRet;
}


//移动设备组下设备
IVS_SDK_API IVS_INT32  __SDK_CALL IVS_SDK_MoveDeviceGroup(IVS_INT32 iSessionID, 
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiNewParentDevGroupID,
	IVS_UINT32 uiDevGroupID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s,NewParentDevGroupID: %u,DevGroupID: %u",
		iSessionID,pDomainCode,uiNewParentDevGroupID,uiDevGroupID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().MoveDeviceGroup(pDomainCode, uiNewParentDevGroupID, uiDevGroupID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,NewParentDevGroupID=%u,DevGroupID=%u",pDomainCode,uiNewParentDevGroupID,uiDevGroupID);
	return iRet;
}

//查询告警级别详情
IVS_SDK_API IVS_INT32  __SDK_CALL IVS_SDK_GetAlarmLevel(IVS_INT32 iSessionID,
	                                                     IVS_UINT32 uiAlarmLevelID,
	                                                     IVS_ALARM_LEVEL* pAlarmLevel)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,AlarmLevelID: %u",iSessionID,uiAlarmLevelID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmLevel, IVS_PARA_INVALID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLevelMgr().GetAlarmLevel(uiAlarmLevelID,pAlarmLevel);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"AlarmLevelID=%u",uiAlarmLevelID);
	return iRet;
}

//开始平台录像
 IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformRecord(IVS_INT32 iSessionID,
	                                                          const IVS_CHAR* pCameraCode,
	                                                          IVS_INT32 iRecordTime)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,RecordTime: %d",iSessionID,pCameraCode,iRecordTime);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StartPlatformRecord(pCameraCode,iRecordTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,RecordTime=%d",pCameraCode,iRecordTime);
	return iRet;
 }

//停止平台录像
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformRecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StopPlatformRecord(pCameraCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//开始前端录像
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPURecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,RecordTime: %d",iSessionID,pCameraCode,iRecordTime);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StartPURecord(pCameraCode,iRecordTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,RecordTime=%d",pCameraCode,iRecordTime);
	return iRet;
}
//停止前端录像
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPURecord(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID,pCameraCode);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StopPURecord(pCameraCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}


//开始本地录像
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartLocalRecord(IVS_INT32 iSessionID,
	                                                    const IVS_LOCAL_RECORD_PARAM* pRecordParam, 
                                                        IVS_ULONG ulPlayHandle, 
                                                        const IVS_CHAR* pSaveFileName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	// ******pSaveFileName 可以为空**************
	// 	CHECK_POINTER(pSaveFileName, IVS_PARA_INVALID);
	CHECK_POINTER(pRecordParam, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu,SaveFileName: %s",
			iSessionID, ulPlayHandle, pSaveFileName != NULL ? pSaveFileName : "NULL");
	
	// 参数校验

	// 命名规则
	if (pRecordParam->uiNameRule < RULE_NAME_NUM_TIME ||
		pRecordParam->uiNameRule > RULE_TIME_NAME_NUM)
	{
		BP_RUN_LOG_INF("Start Local Record", "Name Rule=[%u]", pRecordParam->uiNameRule);
		return IVS_PARA_INVALID;
	}

	// 文件格式
	if (pRecordParam->uiRecordFormat < IVS_FILE_MP4 || 
		pRecordParam->uiRecordFormat > IVS_FILE_AVI)
	{
		BP_RUN_LOG_INF("Start Local Record", "Record Format=[%u]", pRecordParam->uiRecordFormat);
		return IVS_PARA_INVALID;
	}

	// 录像时间
	if (pRecordParam->uiRecordTime < IVS_MIN_RECORD_TIME ||
		pRecordParam->uiRecordTime > IVS_MAX_RECORD_TIME) 
	{
		BP_RUN_LOG_INF("Start Local Record", "Record Time=[%u]", pRecordParam->uiRecordTime);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型
	if (pRecordParam->uiSplitterType < RECORD_SPLITE_TIME ||
		pRecordParam->uiSplitterType > RECORD_SPLITE_CAPACITY)
	{
		BP_RUN_LOG_INF("Start Local Record", "Splitter Type=[%u]", pRecordParam->uiSplitterType);
		return IVS_PARA_INVALID;
	}
    
	// 文件分割类型值
	if (RECORD_SPLITE_TIME == pRecordParam->uiSplitterType)
	{
		if (pRecordParam->uiSplitterValue < IVS_MIN_TIME_SPLITTER_VALUE || 
			pRecordParam->uiSplitterValue > IVS_MAX_TIME_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Local Record", "Splitter Time=[%u]", pRecordParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (pRecordParam->uiSplitterValue < IVS_MIN_SPACE_SPLITTER_VALUE || 
			pRecordParam->uiSplitterValue > IVS_MAX_SPACE_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Local Record", "Splitter FileSize=[%u]", pRecordParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}

	if (NULL != pSaveFileName && strlen(pSaveFileName) > 256)
	{
		BP_RUN_LOG_INF("Start Local Record", "pSaveFileName too long [%s]", pSaveFileName);
		return IVS_PARA_INVALID;
	}

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = IVS_FAIL;
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle);
	CHECK_POINTER(mediaBase,IVS_FAIL);
	//unsigned int port = pUserMgr->GetRealPlayMgr().GetPlayerChannelByHandle(ulPlayHandle);
	iRet = mediaBase->StartLocalRecord(pRecordParam, pSaveFileName);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,SaveFileName=%s",ulPlayHandle, pSaveFileName != NULL ? pSaveFileName : "NULL");
	return iRet;
}

//停止本地录像
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopLocalRecord(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, PlayHandle: %lu",iSessionID,ulPlayHandle);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

    IVS_INT32 iRet = IVS_FAIL;
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	
	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle);
	if(NULL != mediaBase)
	{
		//unsigned int port = pUserMgr->GetRealPlayMgr().GetPlayerChannelByHandle(ulPlayHandle);
		iRet = mediaBase->StopLocalRecord();
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}


//查询域路由信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainRoute( IVS_INT32 iSessionID, IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, IVS_UINT32 uiBufferSize )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,BufSize: %lu",iSessionID,uiBufferSize);

	CHECK_POINTER(pDomainRouteList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDomainRouteMgr().GetDomainRoute(pDomainRouteList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%lu",uiBufferSize);
	return iRet;
}

//查询域路由信息(转发域扩展)
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainRouteEx( IVS_INT32 iSessionID, IVS_DOMAIN_ROUTE_LIST_EX* pDomainRouteList, IVS_UINT32 uiBufferSize )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	IVS_INFO_TRACE("SessionID: %d,BufSize: %lu",iSessionID,uiBufferSize);

	CHECK_POINTER(pDomainRouteList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDomainRouteMgr().GetDomainRouteEx(pDomainRouteList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%lu",uiBufferSize);
	return iRet;
}

//释放SDK内部申请的内存
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ReleaseBuffer(IVS_CHAR *pBuffer)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	IVS_INFO_TRACE("");
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INT32 iRet = g_pNvs->ReleaseBuffer(pBuffer);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");

	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDeviceToGroup( IVS_INT32 iSessionID, 
	const IVS_CHAR* pTargetDomainCode,
	IVS_UINT32 uiTargetDevGroupID, 
	IVS_UINT32 uiDeviceNum,
	const IVS_DEV_GROUP_CAMERA* pGroupCameraList )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGroupCameraList, IVS_PARA_INVALID);
	CHECK_POINTER(pTargetDomainCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,TargetDomainCode: %s,TargetDevGroupID: %u,DeviceNum: %u",iSessionID,pTargetDomainCode,uiTargetDevGroupID,uiDeviceNum);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceGroupMgr().AddDeviceToGroup(pTargetDomainCode, uiTargetDevGroupID, uiDeviceNum, pGroupCameraList);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"TargetDomainCode=%s,TargetDevGroupID=%u,DeviceNum=%u",pTargetDomainCode,uiTargetDevGroupID,uiDeviceNum);
	return iRet;
	
}


//根据主设备查询子设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetChannelList(IVS_INT32 iSessionID, 
                                                        const IVS_CHAR* pDevCode,
                                                        IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList,
                                                        IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pChannelList, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevCode: %s,BufSize: %u",iSessionID,pDevCode,uiBufferSize);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetChannelList(pDevCode, pChannelList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s,BufSize=%u",pDevCode,uiBufferSize);
	return iRet;
}

//获取云台位置信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPTZAbsPosition(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode, const IVS_CHAR* pCameraCode, IVS_PTZ_ABSPOSITION *pPtzAbsPosition)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPtzAbsPosition, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s", iSessionID, pCameraCode);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetPtzAbsPosition(pCameraCode, pPtzAbsPosition);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//云镜控制
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PtzControl(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode, IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2, IVS_UINT32* pLockStatus)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara1, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara2, IVS_PARA_INVALID);
	CHECK_POINTER(pLockStatus, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,ControlCode: %d,ControlPara1: %s,ControlPara2: %s",
		iSessionID,pCameraCode,iControlCode,pControlPara1,pControlPara2);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_PTZ_CONTROL_INFO PtzControlInfo = {0};
	IVS_INT32 iRet = pUserMgr->GetPtzControl().PtzControl(pCameraCode, iControlCode, pControlPara1,pControlPara2, &PtzControlInfo);
	*pLockStatus = PtzControlInfo.uiLockStatus;
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,ControlCode=%d,ControlPara1=%s,ControlPara2=%s",pCameraCode,iControlCode,pControlPara1,pControlPara2);
	return iRet;
}



IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PtzControlWithLockerInfo(IVS_INT32 iSessionID,
	const IVS_CHAR* pCameraCode, 
	IVS_INT32 iControlCode,
	const IVS_CHAR* pControlPara1,
	const IVS_CHAR* pControlPara2, 
	IVS_PTZ_CONTROL_INFO* pPtzControlInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara1, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara2, IVS_PARA_INVALID);
	CHECK_POINTER(pPtzControlInfo, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,ControlCode: %d,ControlPara1: %s,ControlPara2: %s",
		iSessionID,pCameraCode,iControlCode,pControlPara1,pControlPara2);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().PtzControl(pCameraCode, iControlCode, pControlPara1,pControlPara2, pPtzControlInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,ControlCode=%d,ControlPara1=%s,ControlPara2=%s",pCameraCode,iControlCode,pControlPara1,pControlPara2);
	return iRet;
}

//查询录像状态
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordStatus(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,IVS_UINT32 uiRecordMethod,IVS_UINT32* pRecordState)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,RecordMethod: %u",iSessionID,pCameraCode,uiRecordMethod);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordList().GetRecordStatus(pCameraCode, uiRecordMethod, pRecordState);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,RecordMethod=%u",pCameraCode,uiRecordMethod);
	return iRet;
}
//修改主设备名称
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetMainDeviceName(IVS_INT32 iSessionID,const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNewDevName, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevCode: %s,NewDevName: %s",iSessionID,pDevCode,pNewDevName);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().SetMainDeviceName(pDevCode, pNewDevName);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s,NewDevName=%s",pDevCode,pNewDevName);
	return iRet;
}
//修改摄像机名称
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetCameraName(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNewCameraName, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s,NewCameraName: %s",iSessionID,pCameraCode,pNewCameraName);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().SetCameraName(pCameraCode,pNewCameraName);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,NewCameraName=%s",pCameraCode,pNewCameraName);
	return iRet;
} 

// 处理BT的Infursion问题，抽取重复代码
static IVS_INT32 DealAudioSound(IVS_INT32 iSessionID, IVS_UINT32 uiPlayHandle, IVS_INT32 iPlayFlag)
{
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_REALVIDEO;
    IVS_INT32 iRet = pUserMgr->GetServiceTypebyPlayHandle(uiPlayHandle, enServiceType);//lint !e1013 !e516
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"IVS SDK Play Sound Failed.", "Reason:Get ServiceType by PlayHandle Failed. enServiceType = %d", (int)enServiceType);
		eSDK_LOG_RUN_WRAN("Get ServiceType by PlayHandle Failed.");
        return iRet;
    }

    switch (enServiceType)
    {
    case SERVICE_TYPE_REALVIDEO:
        if (0 == iPlayFlag)// 0为播放
        {
            iRet = pUserMgr->GetRealPlayMgr().PlayAudioSound(uiPlayHandle);
        }
        else// 1 为停止
        {
            iRet = pUserMgr->GetRealPlayMgr().StopAudioSound(uiPlayHandle);
        }
        break;
    case SERVICE_TYPE_PLAYBACK:
    case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
    case SERVICE_TYPE_PU_PLAYBACK:
        if (0 == iPlayFlag)// 0为播放
        {
            iRet = pUserMgr->GetPlaybackMgr().PlayAudioSound(uiPlayHandle);
        }
        else// 1 为停止
        {
            iRet = pUserMgr->GetPlaybackMgr().StopAudioSound(uiPlayHandle);
        }
        break;
    default:
        break;
    }
    return iRet;
}

//播放随路语音
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlaySound(IVS_INT32 iSessionID, IVS_UINT32 uiPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
    IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu", iSessionID, uiPlayHandle);
    IVS_INT32 iRet = DealAudioSound(iSessionID, uiPlayHandle, 0);// 0为播放
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",uiPlayHandle);
    return iRet;
}

//停止播放随路语音
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopSound(IVS_INT32 iSessionID, IVS_UINT32 uiPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
    IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu", iSessionID, uiPlayHandle);
    IVS_INT32 iRet = DealAudioSound(iSessionID, uiPlayHandle, 1);// 1为停止播放
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",uiPlayHandle);
    return iRet;
}

//设置音量
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVolume(IVS_INT32 iSessionID,
                                                   IVS_UINT32 uiPlayHandle, 
                                                   IVS_UINT32 uiVolumeValue)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %u,VolumeValue: %u",iSessionID,uiPlayHandle,uiVolumeValue);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_REALVIDEO;
	IVS_INT32 iRet = pUserMgr->GetServiceTypebyPlayHandle(uiPlayHandle, enServiceType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS SDK Set Volume Failed.", "Reason:Get ServiceType by PlayHandle Failed. enServiceType = %d", (int)enServiceType);
		eSDK_LOG_RUN_WRAN("Get ServiceType by PlayHandle Failed.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");
		return iRet;
	}

	switch (enServiceType)
	{
	case SERVICE_TYPE_REALVIDEO:
		iRet = pUserMgr->GetRealPlayMgr().SetVolume(uiPlayHandle,uiVolumeValue);
		break;
	case SERVICE_TYPE_PLAYBACK:
	case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
    case SERVICE_TYPE_PU_PLAYBACK:
		iRet = pUserMgr->GetPlaybackMgr().SetVolume(uiPlayHandle,uiVolumeValue);
		break;
    case SERVICE_TYPE_AUDIO_CALL:
        {
            iRet = pUserMgr->GetAudioMgr().SetVolume(uiPlayHandle,uiVolumeValue);
            break;
        }
	default:
		break;
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,VolumeValue=%u",uiPlayHandle,uiVolumeValue);
    return iRet;
}


//获取音量
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVolume(IVS_INT32 iSessionID,
                                                   IVS_UINT32 uiPlayHandle, 
                                                   IVS_UINT32* puiVolumeValue)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %u",iSessionID,uiPlayHandle);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);

	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_SERVICE_TYPE enServiceType = SERVICE_TYPE_REALVIDEO;
	IVS_INT32 iRet = pUserMgr->GetServiceTypebyPlayHandle(uiPlayHandle, enServiceType);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS SDK Get Volume Failed.", "Reason:Get ServiceType by PlayHandle Failed. enServiceType = %d", (int)enServiceType);
		eSDK_LOG_RUN_WRAN("Get ServiceType by PlayHandle Failed.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
		return iRet;
	}

	switch (enServiceType)
	{
	case SERVICE_TYPE_REALVIDEO:
		iRet = pUserMgr->GetRealPlayMgr().GetVolume(uiPlayHandle,puiVolumeValue);
		break;
	case SERVICE_TYPE_PLAYBACK:
	case SERVICE_TYPE_BACKUPRECORD_PLAYBACK:
	case SERVICE_TYPE_DISASTEBACKUP_PLAYBACK:
    case SERVICE_TYPE_PU_PLAYBACK:
		iRet = pUserMgr->GetPlaybackMgr().GetVolume(uiPlayHandle,puiVolumeValue);
		break;
    case SERVICE_TYPE_AUDIO_CALL:
        {
            iRet = pUserMgr->GetAudioMgr().GetVolume(uiPlayHandle, puiVolumeValue);
            break;
        }
	default:
		break;
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",uiPlayHandle);
    return iRet;
}



//初始化电视墙
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_InitTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	CHECK_POINTER(pFunNotifyCallBack, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUserData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("pSDK: 0x%p, SessionID: %d",g_pNvs, iSessionID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().InitTVWall(pFunNotifyCallBack, pUserData);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//释放电视墙
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_CleanupTVWall(IVS_INT32 iSessionID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().CleanupTVWall();
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDecoderList(IVS_INT32 iSessionID,
														const IVS_INDEX_RANGE* pIndexRange,
														CUMW_NOTIFY_REMOTE_DECODER_STATUS_LIST* pDecoderList,
														IVS_UINT32 uiBufferSize)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDecoderList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("pSDK: 0x%p, SessionID: %d, BufferSize: %u",g_pNvs, iSessionID, uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().GetDecoderList(pIndexRange, pDecoderList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pSDK=0x%p,BufferSize=%u",g_pNvs, uiBufferSize);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayTVWall(IVS_INT32 iSessionID,
															const IVS_CHAR* pCameraCode,
															const IVS_REALPLAY_PARAM* pRealplayParam,
															const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s",iSessionID, pCameraCode);
	CHECK_POINTER(pRealplayParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StartRealPlayTVWall(pCameraCode, pRealplayParam, pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;

}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayTVWall(IVS_INT32 iSessionID,
															const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StopRealPlayTVWall(pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackTVWall(IVS_INT32 iSessionID,
                                                                        const IVS_CHAR* pCameraCode,
                                                                        const IVS_PLAYBACK_PARAM* pPlaybackParam,
                                                                        const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("pSDK: 0x%p,SessionID: %d,CameraCode: %s",g_pNvs, iSessionID, pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StartPlayBackTVWall(pCameraCode, pPlaybackParam, pTVWallParam, REPLAY_TYPE_PLATFORM, (const IVS_CHAR*)0,pPlaybackParam->cNVRCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pSDK=0x%p,CameraCode=%s",g_pNvs, pCameraCode);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformPlayBackTVWall(IVS_INT32 iSessionID,
                                                                    const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StopPlayBackTVWall(pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBackTVWall (IVS_INT32 iSessionID,
																const IVS_CHAR* pCameraCode,
																const IVS_PLAYBACK_PARAM* pPlaybackParam,
																const IVS_TVWALL_PARAM* pTVWallParam)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start PU PlayBack TVWallSessionID: %d,CameraCode: %s",iSessionID, pCameraCode);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StartPlayBackTVWall(pCameraCode, pPlaybackParam, pTVWallParam, REPLAY_TYPE_PU);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPUPlayBackTVWall(IVS_INT32 iSessionID,
																const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INT32 iRet =  IVS_SDK_StopPlatformPlayBackTVWall(iSessionID, pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartBackupPlayBackTVWall (IVS_INT32 iSessionID,
																	const IVS_CHAR* pDomainCode,
																	const IVS_CHAR* pNVRCode,
																	const IVS_CHAR* pCameraCode,
																	const IVS_PLAYBACK_PARAM* pPlaybackParam,
																	const IVS_TVWALL_PARAM* pTVWallParam)
{	
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(pTVWallParam, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("Start Backup PlayBack TVWallSessionID: %d,DomainCode: %s,NVRCode:%s,CameraCode: %s, PlaybackParam: %s",
					iSessionID, pDomainCode, pNVRCode, pCameraCode, pPlaybackParam);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().StartPlayBackTVWall(pCameraCode, pPlaybackParam, pTVWallParam, REPLAY_TYPE_BACKUP, pDomainCode, pNVRCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,NVRCode=%s,CameraCode=%s, PlaybackParam=%s",pDomainCode, pNVRCode, pCameraCode, pPlaybackParam);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopBackupPlayBackTVWall(IVS_INT32 iSessionID,
	const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INT32 iRet = IVS_SDK_StopPlatformPlayBackTVWall(iSessionID, pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackPauseTVWall(IVS_INT32 iSessionID,
															const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().PlayBackPauseTVWall(*pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlayBackResumeTVWall(IVS_INT32 iSessionID,
															const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().PlayBackResumeTVWall(*pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PlaybackFrameStepForwardTVWall(IVS_INT32 iSessionID,
																		const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().PlaybackFrameStepForwardTVWall(*pTVWallParam);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddCallBackTVWall(IVS_INT32 iSessionID, PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, 
															void* pUserData)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pFunNotifyCallBack, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUserData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("pSDK: 0x%p, SessionID: %d",g_pNvs, iSessionID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().AddCallBack(pFunNotifyCallBack, pUserData);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;

}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_RemoveCallBackTVWall(IVS_INT32 iSessionID, void* pUserData)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	CHECK_POINTER(pUserData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("pSDK: 0x%p, SessionID: %d",g_pNvs, iSessionID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().RemoveCallBack(pUserData);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pSDK=0x%p",g_pNvs);
	return iRet;
}

// 获取拥有指定设备权限的用户列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceUserList(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, IVS_UINT32 uiIsUserOnline, const IVS_INDEX_RANGE* pIndexRange, IVS_DEVICE_USER_INFO_LIST* pDevUserList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDevUserList, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,DomainCode: %s, DevCode: %s,BufSize: %u",iSessionID,pDomainCode, pDevCode,uiBufferSize);
	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetDeviceUserList(pDomainCode, pDevCode, uiIsUserOnline, pIndexRange, pDevUserList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s, DevCode=%s,BufSize=%u",pDomainCode, pDevCode,uiBufferSize);
	return iRet;
}

// 重启前端设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_RebootDevice (IVS_INT32 iSessionID, const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDeviceCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,DevCode: %s",iSessionID,pDeviceCode);	

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetDeviceMgr().RestartDevice(pDeviceCode, uiRebootType);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s",pDeviceCode);
	return iRet;
}

// 本地抓拍
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_LocalSnapshot(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, 
														IVS_UINT32 uiPictureFormat, const IVS_CHAR* pFileName)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pFileName, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, PlayHandle: %lu, PictureFormat: %lu, FileName: %s",
		iSessionID, ulPlayHandle, uiPictureFormat, pFileName);
	if (IVS_FILE_NAME_LEN < strlen(pFileName))
	{
		BP_RUN_LOG_INF("Local Snapshot", "File Name too long");
		return IVS_NAME_INVALID;
	}
	if (CAPTURE_FORMAT_NONE == uiPictureFormat || CAPTURE_FORMAT_OTHERS <= uiPictureFormat)
	{
		BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "Local Snapshot", "Picture Format Invalid");
		eSDK_LOG_RUN_WRAN("Picture Format Invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_FILE_FORMAT_ERROR,"");
		return IVS_FILE_FORMAT_ERROR;
	}

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle);
	if (NULL == mediaBase)
	{
		mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle);
	}
 	CHECK_POINTER(mediaBase, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = mediaBase->LocalSnapshot(pFileName, uiPictureFormat);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu,PictureFormat=%lu,FileName=%s",ulPlayHandle, uiPictureFormat, pFileName);
	return iRet;
}

// 增加预置位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName, IVS_UINT32* uiPresetIndex)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPresetName, IVS_PARA_INVALID);
	CHECK_POINTER(uiPresetIndex, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().AddPTZPreset(pCameraCode, pPresetName, uiPresetIndex);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 删除预置位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s,uiPresetIndex: %u",iSessionID,pCameraCode, uiPresetIndex);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().DelPTZPreset(pCameraCode, uiPresetIndex);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,PresetIndex=%u",pCameraCode,uiPresetIndex);
	return iRet;
}

// 修改预置位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModPTZPreset (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPTZPreset, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().ModPTZPreset(pCameraCode, pPTZPreset);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 查询预置位列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetPTZPresetList (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiPTZPresetNum)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPTZPresetList, IVS_PARA_INVALID);
	CHECK_POINTER(uiPTZPresetNum, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s,uiBufferSize: %u",iSessionID,pCameraCode, uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetPTZPresetList(pCameraCode, pPTZPresetList, uiBufferSize, uiPTZPresetNum);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,BufferSize=%u",pCameraCode,uiBufferSize);
	return iRet;
}

// 设置看守位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pGuardPosInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().SetGuardPos(pCameraCode, pGuardPosInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

//删除看守位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().DelGuardPos(pCameraCode, uiPresetIndex);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,PresetIndex=%u",pCameraCode,uiPresetIndex);
	return iRet;
}


// 获取看守位
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetGuardPos (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pGuardPosInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetGuardPos(pCameraCode, pGuardPosInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 增加巡航轨迹
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, IVS_UINT32* uiTrackIndex)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCuriseTrackInfo, IVS_PARA_INVALID);
	CHECK_POINTER(uiTrackIndex, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s, uiTrackIndex: %u", iSessionID, pCameraCode, *uiTrackIndex);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().AddCruiseTrack(pCameraCode, pCuriseTrackInfo, uiTrackIndex);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,TrackIndex=%u",pCameraCode,*uiTrackIndex);
	return iRet;
}

// 删除巡航轨迹
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s, uiTrackIndex: %u, uiCruiseType: %u", iSessionID, pCameraCode, uiTrackIndex, uiCruiseType);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().DelCruiseTrack(pCameraCode, uiTrackIndex, uiCruiseType);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,uiTrackIndex=%u,uiCruiseType=%u",pCameraCode,uiTrackIndex,uiCruiseType);
	return iRet;
}

// 修改巡航轨迹
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCuriseTrackInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().ModCruiseTrack(pCameraCode, pCuriseTrackInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 查询巡航轨迹列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruiseTrackList (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiCruiseTrackNum)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCruiseTrackList, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s, uiBufferSize:%u",iSessionID,pCameraCode,uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetCruiseTrackList(pCameraCode, pCruiseTrackList, uiBufferSize, uiCruiseTrackNum);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,uiBufferSize=%u",pCameraCode,uiBufferSize);
	return iRet;
}

// 查询巡航轨迹信息
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruiseTrack (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, IVS_CRUISE_TRACK* pCruiseTrackInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCruiseTrackInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s, uiTrackIndex:%u",iSessionID,pCameraCode, uiTrackIndex);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetCruiseTrack(pCameraCode, uiTrackIndex, pCruiseTrackInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s, TrackIndex=%u",pCameraCode,uiTrackIndex);
	return iRet;
}

// 设置巡航计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetCruisePlan (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCruisePlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().SetCruisePlan(pCameraCode, pCruisePlan);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 查询巡航计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCruisePlan (IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_CRUISE_PLAN* pCruisePlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCruisePlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPtzControl().GetCruisePlan(pCameraCode, pCruisePlan);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceAlarmList(IVS_INT32 iSessionID, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_DEVICE_ALARM_EVENT_LIST* pAlarmEventList, IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pAlarmEventList, IVS_OPERATE_MEMORY_ERROR);

	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CUserMgr* pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetDeviceAlarmList(pUnifiedQuery, pAlarmEventList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

//平台抓拍 
IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_PlatformSnapshot(IVS_INT32 iSessionID,const IVS_CHAR* pCameraCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetCapturePicture().PlatformSnapshot(pCameraCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 启动搜索前端设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d, DomainCode:%s, pSearchInfo: %s", iSessionID, pDomainCode, pSearchInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().StartSearchDeviceEX(pDomainCode, pSearchInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,SearchInfo=%s",pDomainCode,pSearchInfo);
	return iRet;
}

// 停止搜索前端设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d, DomainCode:%s, pSearchInfo: %s", iSessionID, pDomainCode, pSearchInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().StopSearchDevice(pDomainCode, pSearchInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,SearchInfo=%s",pDomainCode,pSearchInfo);
	return iRet;
}

// 验证前端设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_VerifyDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d, DomainCode:%s", iSessionID, pDomainCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().VerifyDeviceEX(pDomainCode, pVerifyInfo, pRspXml);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,VerifyInfo=%s",pDomainCode,pVerifyInfo);
	return iRet;
}

// 获取平台抓拍图片列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetSnapshotList(IVS_INT32 iSessionID,
	const IVS_CHAR* pCameraCode,
	const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam,
	IVS_SNAPSHOT_INFO_LIST* pSnapshotList,
	IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryParam, IVS_PARA_INVALID);
	CHECK_POINTER(pSnapshotList, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetCapturePicture().GetSnapshotList(pCameraCode, pQueryParam, pSnapshotList, uiBufSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,BufSize=%u",pCameraCode,uiBufSize);
	return iRet;
}

// CPU/内存性能查询
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetCPUMemory(IVS_INT32 iSessionID, const IVS_CHAR* pNodeCode, IVS_CHAR** pPerformance)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNodeCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPerformance, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	
	CUserMgr* pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetClientNetworkMgr().GetCPUMemory(pNodeCode, pPerformance);
	
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"NodeCode=%s",pNodeCode);
	return iRet;
}

// 开始语音对讲
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartVoiceTalkback(
                                IVS_INT32 iSessionID,
                                const IVS_VOICE_TALKBACK_PARAM* pTalkbackParam,
                                const IVS_CHAR* pCameraCode,
                                IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pTalkbackParam, IVS_PARA_INVALID);
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
    IVS_INFO_TRACE("SessionID: %d, VoiceCode: %s", iSessionID, pCameraCode);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StartVoiceTalkback(pTalkbackParam, pCameraCode, pHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}


// 停止语音对讲
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopVoiceTalkback(
                                                        IVS_INT32 iSessionID,
                                                        IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
    IVS_INFO_TRACE("SessionID: %d,Handle: 0x%p",iSessionID,ulHandle);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StopVoiceTalkback(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

// 设置麦克风音量
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetMicVolume(IVS_UINT32 uiVolumeValue)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("VolumeValue: %u", uiVolumeValue);
	
    IVS_INT32 iRet = IVS_PLAY_SetMicVolume(uiVolumeValue);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"VolumeValue=%u",uiVolumeValue);
	return iRet;
}

// 获取麦克风音量
IVS_SDK_API IVS_UINT32 __SDK_CALL IVS_SDK_GetMicVolume()
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_UINT32 uVolumeValue = 0;
    if (IVS_SUCCEED != IVS_PLAY_GetMicVolume(&uVolumeValue))
    {
		uVolumeValue = 0;
    }

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
    INT_LOG_NO_SESSION(reqTime,rspTime,uVolumeValue,"");
    return uVolumeValue;
}

// 开始实时语音广播
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealBroadcast(IVS_INT32 iSessionID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
	

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StartRealBroadcast();

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 停止实时语音广播
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealBroadcast(IVS_INT32 iSessionID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
	

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StopRealBroadcast();

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 开始文件语音广播
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartFileBroadcast(
                                                IVS_INT32 iSessionID,
                                                const IVS_CHAR* pFileName,
                                                IVS_BOOL bCycle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
	
    IVS_INFO_TRACE("SessionID: %d,FileName: %s, Cycle %d",iSessionID, pFileName, bCycle);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StartFileBroadcast(pFileName, bCycle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"FileName=%s,Cycle=%d",pFileName, bCycle);
	return iRet;
}

// 停止文件语音广播
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopFileBroadcast(IVS_INT32 iSessionID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    IVS_INFO_TRACE("SessionID: %d",iSessionID);
	
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().StopFileBroadcast();

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 添加广播设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddBroadcastDevice(
                                                IVS_INT32 iSessionID,
                                                const IVS_CHAR* pCameraCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
    IVS_INFO_TRACE("SessionID: %d,VoiceCode: %s",iSessionID, pCameraCode);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().AddBroadcastDevice(pCameraCode);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 删除广播设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteBroadcastDevice(
                                                IVS_INT32 iSessionID,
                                                const IVS_CHAR* pCameraCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
    IVS_INFO_TRACE("SessionID: %d,VoiceCode: %s",iSessionID, pCameraCode);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetAudioMgr().DeleteBroadcastDevice(pCameraCode);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 添加录像备份计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pBackupPlan: %s",iSessionID, pBackupPlan);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().AddRecordBackupPlan(pBackupPlan);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BackupPlan=%s",pBackupPlan);
	return iRet;
}


// 修改录像备份计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pBackupPlan: %s",iSessionID, pBackupPlan);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().ModifyRecordBackupPlan(pBackupPlan);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BackupPlan=%s",pBackupPlan);
	return iRet;
}

// 删除备份录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pBackupPlan: %s",iSessionID, pBackupPlan);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().DeleteRecordBackupPlan(pBackupPlan);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BackupPlan=%s",pBackupPlan);
	return iRet;
}

// 查询备份录像计划
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, IVS_CHAR** pBackupPlan)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(pBackupPlan, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pQueryInfo: %s",iSessionID, pQueryInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().GetRecordBackupPlan(pQueryInfo, pBackupPlan);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"QueryInfo=%s",pQueryInfo);
	return iRet;
}

// 查询当前用户的手动录像任务
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordTask(IVS_INT32 iSessionID, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pRspXml: %s",iSessionID, pRspXml);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetRecordTask(pUserMgr->GetUserID(),pUserMgr->GetDeviceMgr(),pRspXml);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 查询录像备份任务列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordBackupTaskList (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo,IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pQueryInfo: %s",iSessionID, pQueryInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().GetRecordBackupTaskList(pQueryInfo, pRspXml);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"QueryInfo=%s",pQueryInfo);
	return iRet;
}

// 启动/继续录像备份任务
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pQueryInfo: %s",iSessionID, pQueryInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().StartRecordBackupTask(pQueryInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"QueryInfo=%s",pQueryInfo);
	return iRet;
}

// 暂停/停止录像备份任务
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pQueryInfo: %s",iSessionID, pQueryInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().StopRecordBackupTask(pQueryInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"QueryInfo=%s",pQueryInfo);
	return iRet;
}

// 删除录像备份任务
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordBackupTask (IVS_INT32 iSessionID,const IVS_CHAR* pQueryInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryInfo, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pQueryInfo: %s",iSessionID, pQueryInfo);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().DeleteRecordBackupTask(pQueryInfo);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"QueryInfo=%s",pQueryInfo);
	return iRet;
}
// 新增录像备份任务
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddRecordBackupTask(IVS_INT32 iSessionID, const IVS_CHAR* pBackupTask)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBackupTask, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, pBackupTask: %s",iSessionID, pBackupTask);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordBackupMgr().AddRecordBackupTask(pBackupTask);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BackupTask=%s",pBackupTask);
	return iRet;
}

// 查询告警联动动作详情
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmLinkageAction(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d, pReqXml: %s",iSessionID, pReqXml);
	CUserMgr* pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmLinkageMgr().GetAlarmLinkageAction(pReqXml, pRsqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayByIP(IVS_INT32 iSessionID, 
															const IVS_CHAR* pCameraCode,
															const IVS_MEDIA_ADDR* pMediaAddrDst,
															IVS_MEDIA_ADDR* pMediaAddrSrc,
															IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrDst, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrSrc, IVS_PARA_INVALID);
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d, CameraCode: %s, IP: %s, VideoPort: %u, AudioPort: %u",
					iSessionID, pCameraCode, 
					pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_ULONG ulHandle = *pHandle;
	IVS_MEDIA_ADDR stMediaAddr = {0};
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StartRealPlayByIP(pCameraCode, *pMediaAddrDst, stMediaAddr, ulHandle);
	*pHandle = ulHandle;
	bool bSucc = CToolsHelp::Memcpy(pMediaAddrSrc, sizeof(IVS_MEDIA_ADDR), &stMediaAddr, sizeof(IVS_MEDIA_ADDR));
	if(!bSucc)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TP Start Play", "Memcpy failed.");
		eSDK_LOG_RUN_WRAN("Memcpy failed.");
	}
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s,IP=%s,VideoPort=%u,AudioPort=%u",pCameraCode, 
		pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayByIP(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INFO_TRACE("SessionID: %d, Handle: %lu",iSessionID, ulHandle);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StopPlayByIP(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayByIPEx(IVS_INT32 iSessionID, 
															const IVS_REALPLAY_PARAM  *pRealplayParam,
															const IVS_CHAR* pCameraCode,
															const IVS_MEDIA_ADDR* pMediaAddrDst,
															IVS_MEDIA_ADDR* pMediaAddrSrc,
															IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRealplayParam, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrDst, IVS_PARA_INVALID);
	CHECK_POINTER(pMediaAddrSrc, IVS_PARA_INVALID);
	CHECK_POINTER(pHandle, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d, CameraCode: %s, IP: %s, VideoPort: %u, AudioPort: %u",
		iSessionID, pCameraCode, pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_ULONG ulHandle = *pHandle;
	IVS_MEDIA_ADDR stMediaAddr = {0};
	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StartRealPlayByIPEx(pCameraCode, *pRealplayParam, *pMediaAddrDst, stMediaAddr, ulHandle);
	*pHandle = ulHandle;
	bool bSucc = CToolsHelp::Memcpy(pMediaAddrSrc, sizeof(IVS_MEDIA_ADDR), &stMediaAddr, sizeof(IVS_MEDIA_ADDR));
	if(!bSucc)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "TP Start Play", "Memcpy failed.");
		eSDK_LOG_RUN_WRAN("Memcpy failed.");
	}

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s, IP=%s, VideoPort=%u, AudioPort=%u",pCameraCode, pMediaAddrDst->stIP.cIP, pMediaAddrDst->uiVideoPort, pMediaAddrDst->uiAudioPort);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopRealPlayByIPEx(IVS_INT32 iSessionID, IVS_ULONG ulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Handle: %lu",iSessionID, ulHandle);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetTelepresenceMgr().StopPlayByIP(ulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Handle=%lu",ulHandle);
	return iRet;
}

// 录像完整性报表查询
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRecordIntegralityReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d,UserGroupID: %u",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetReportMgr().OMUQueryRecordIntegralityReport(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}


// 磁盘存储占用率报表查询
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDiskUsageRateReport(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    IVS_INFO_TRACE("SessionID: %d,UserGroupID: %u",iSessionID);
    CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	
    if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
        BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete User Group Failed.", "Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
        return IVS_SDK_RET_INVALID_SESSION_ID;
    }
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
    CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
    IVS_INT32 iRet = pUserMgr->GetReportMgr().OMUQueryDiskUsageRateReport(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}


// 查询磁盘信息接口
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryDiskProperty(IVS_INT32 iSessionID, 
	IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d",iSessionID);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetReportMgr().OMUQueryDiskProperty(pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

// 删除平台抓拍图片
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteSnapshot(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("SessionID: %d,pCameraCode: %s",iSessionID,pCameraCode);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetCapturePicture().DeleteSnapshot(pCameraCode, uiPictureID);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}

// 设置日志根路径
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetLogPath(const IVS_CHAR* pLogPath)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


    CHECK_POINTER(pLogPath, IVS_PARA_INVALID);
    CSDKConfig::instance().SetLogPath(pLogPath);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime, IVS_SUCCEED, "pLogPath=%s",pLogPath);
    return IVS_SUCCEED;
}

// 设置服务器端监听IP和端口号
IVS_INT32 IVS_SDK_API __SDK_CALL IVS_SDK_SetNetKeyBoardPort(IVS_ULONG ulPort)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CNetKeyBoard *pNetKeyBoardService = CNetKeyBoard::GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    pNetKeyBoardService->SetNetKeyBoardListenAddr((unsigned short )ulPort);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime, IVS_SUCCEED, "ulPort=%u",ulPort);
    return IVS_SUCCEED;
}

// 启动监听线程
IVS_INT32 IVS_SDK_API __SDK_CALL IVS_SDK_StartNetKeyBoard(IVS_ULONG ulNetKeyBoardVendor, IVS_ULONG ulPort, LPCallBack lpCallBack)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CNetKeyBoard *pNetKeyBoardService = CNetKeyBoard::GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    ULONG ulRet = (ULONG)pNetKeyBoardService->StartNetKeyBoard(ulNetKeyBoardVendor, ulPort, lpCallBack);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime, ulRet, "ulNetKeyBoardVendor=%lu",ulNetKeyBoardVendor);
    return ulRet;
}

// 释放线程
IVS_INT32 IVS_SDK_API __SDK_CALL IVS_SDK_StopNetKeyBoard()
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

    CNetKeyBoard *pNetKeyBoardService = CNetKeyBoard::GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    ULONG ulRet = (ULONG)pNetKeyBoardService->ReleaseThread();

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime, ulRet, "");
    return ulRet;
}

//停止备份录像回放
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopBackupPlayBack(IVS_INT32 iSessionID,IVS_ULONG ulPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StopBackupPlayBack(ulPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
	return iRet;
}

// 设置前端设备密码
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDevPWD(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDevUserName, IVS_PARA_INVALID);
	CHECK_POINTER(pDevPWD, IVS_PARA_INVALID);
	CHECK_POINTER(pDevRegPWD, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,pDevCode: %s,pDevUserName: %s",iSessionID,pDevCode,pDevUserName);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().SetDevPWD(pDevCode, pDevUserName, pDevPWD, pDevRegPWD);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DevCode=%s,DevUserName=%s",pDevCode,pDevUserName);
	return iRet;
}

// 获取告警抓拍图片列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetAlarmSnapshotList(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRsqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pReqXml: %s", iSessionID, pReqXml);
	
	CUserMgr* pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetAlarmMgr().GetAlarmSnapshotList(pReqXml, pRsqXml);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRaidGroup(IVS_INT32 iSessionID, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d", iSessionID);
	

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetClientNetworkMgr().QueryRaidGroup("<Content></Content>", pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_OMUQueryRaidGroupDisk(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,pReqXml: %s",iSessionID,pReqXml);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetClientNetworkMgr().QueryRaidGroupDisk(pReqXml, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ReqXml=%s",pReqXml);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetVideoDiagnose(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, DomainCode: %s, CameraCode: %s", iSessionID, pDomainCode, pCameraCode);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetVideoDiagnose(pDomainCode, pCameraCode, pRspXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,CameraCode=%s",pDomainCode, pCameraCode);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVideoDiagnose(IVS_INT32 iSessionID, 
	const IVS_CHAR* pReqXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pReqXml: %s", iSessionID, pReqXml);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().SetVideoDiagnose(pReqXml);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pReqXml=%s",pReqXml);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetStreamListByCam(IVS_INT32 iSessionID, 
	const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_CAM* pStreamListByCam)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pStreamListByCam, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pDomainCode: %s, pCameraCode: %s, uiBufSize: %d", 
		iSessionID, pDomainCode, pCameraCode, uiBufSize);
	
	if (sizeof(IVS_STREAM_LIST_BY_CAM) + (IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO)
		!= uiBufSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS_SDK_GetStreamListByCam", "uiBufSize should equal %d",
			sizeof(IVS_STREAM_LIST_BY_CAM) + (IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO));


		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_PARA_INVALID;
	}

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetStreamListByCam(pDomainCode, pCameraCode, uiBufSize, pStreamListByCam);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,CameraCode=%s,BufSize=%d",pDomainCode, pCameraCode, uiBufSize);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetStreamListByUser(IVS_INT32 iSessionID, 
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID,
	IVS_UINT32 uiBufSize,
	IVS_STREAM_LIST_BY_USER* pStreamListByUser)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pStreamListByUser, IVS_PARA_INVALID);
	
	IVS_DEBUG_TRACE("SessionID: %d, pDomainCode: %s, uiUserID: %d, uiBufSize: %d", 
		iSessionID, pDomainCode, uiUserID, uiBufSize);

	if (sizeof(IVS_STREAM_LIST_BY_USER) + (IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO)
		!= uiBufSize)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS_SDK_GetStreamListByCam", "uiBufSize should equal %d",
			sizeof(IVS_STREAM_LIST_BY_USER) + (IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO));


		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_PARA_INVALID;
	}

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().GetStreamListByUser(pDomainCode, uiUserID, uiBufSize, pStreamListByUser);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,uiUserID=%d,uiBufSize=%d",pDomainCode, uiUserID, uiBufSize);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ShutdownStreamByUser(IVS_INT32 iSessionID, 
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiUserID,  
	const IVS_CHAR* pCameraCode,
	IVS_INT32 iStreamType,
	IVS_INT32 iDuration)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pDomainCode: %s, uiUserID: %d, pCameraCode: %s, iStreamType: %d, iDuration: %d", 
		iSessionID, pDomainCode, uiUserID, pCameraCode, iStreamType, iDuration);
	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().ShutdownStreamByUser(pDomainCode, uiUserID, pCameraCode, iStreamType, iDuration);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DomainCode=%s,UserID=%d,CameraCode=%s,StreamType=%d,Duration=%d",pDomainCode, uiUserID, pCameraCode, iStreamType, iDuration);
	return iRet;
}


// 用户名、密码验证
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_PasswdValidation(IVS_INT32 iSessionID,
	const IVS_CHAR* pLoginName,
	const IVS_CHAR *pPasswd)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pLoginName, IVS_PARA_INVALID);
	CHECK_POINTER(pPasswd, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetUserInfoMgr().PasswdValidation(pLoginName, pPasswd);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}


// 录像视频质量调节
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetVideoEffect(IVS_INT32 iSessionID,
	IVS_ULONG ulPlayHandle, IVS_LONG lBrightValue,
	IVS_LONG lContrastValue, IVS_LONG lSaturationValue,
	IVS_LONG lHueValue)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	
	IVS_INFO_TRACE("Playhandle: %lu, BrightValue: %d, ContrastValue: %d, SaturationValue: %d, HueValue: %d",
		ulPlayHandle, lBrightValue, lContrastValue, lSaturationValue, lHueValue);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().SetVideoEffect(ulPlayHandle, lBrightValue, lContrastValue, lSaturationValue, lHueValue);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Playhandle=%lu,BrightValue=%d, ContrastValue=%d,SaturationValue=%d,HueValue=%d",ulPlayHandle, lBrightValue, lContrastValue, lSaturationValue, lHueValue);
	return iRet;
}


// 获取指定域所包含的设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainDeviceList(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	
	IVS_INFO_TRACE("iSessionID: %d, pDomainCode: %s, uiDeviceType: %u, pIndexRange: [%u - %u], uiBufferSize: %u",
		iSessionID, pDomainCode, uiDeviceType, pIndexRange->uiFromIndex, pIndexRange->uiToIndex, uiBufferSize);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetDomainDeviceList(pDomainCode, uiDeviceType, pIndexRange, pDeviceList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,uiDeviceType=%u,pIndexRange=[%u - %u], uiBufferSize=%u",pDomainCode, uiDeviceType, pIndexRange->uiFromIndex, pIndexRange->uiToIndex, uiBufferSize);
	return iRet;
}


// 启动某个摄像机实况的智能分析轨迹叠加
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_EnablePURealplayIAOverLay(IVS_INT32 iSessionID,
	IVS_ULONG ulRealPlayHandle,
	const IVS_CHAR* pCameraCode,
	const IVS_CHAR* pNVRCode,
	HWND hWnd)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(hWnd, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d, RealPlayHandle: %lu, CameraCode: %s, NVRCode: %s", iSessionID, ulRealPlayHandle, pCameraCode, pNVRCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().EnablePURealplayIAOverLay(ulRealPlayHandle, pCameraCode, pNVRCode, hWnd);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"RealPlayHandle: %lu, CameraCode: %s, NVRCode", ulRealPlayHandle, pCameraCode, pNVRCode);
	return iRet;
}

// 停止某个摄像机实况的智能分析轨迹叠加
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DisablePURealplayIAOverLay(IVS_INT32 iSessionID,
	IVS_ULONG ulRealPlayHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	IVS_INFO_TRACE("SessionID: %d, RealPlayHandle: %lu", iSessionID, ulRealPlayHandle);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().DisablePURealplayIAOverLay(ulRealPlayHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"RealPlayHandle: %lu", ulRealPlayHandle);
	return iRet;
}


//获取获取设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDeviceListEx(IVS_INT32 iSessionID,
	IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);


	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,DevType: %u,BufSize: %u",iSessionID,uiDeviceType,uiBufferSize);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetDeviceList(uiDeviceType, pIndexRange, pDeviceList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceType=%u,BufSize=%u",uiDeviceType,uiBufferSize);
	return iRet;
}


// 获取指定域所包含的设备列表
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetDomainDeviceListEx(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime;GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);

	IVS_INFO_TRACE("iSessionID: %d, pDomainCode: %s, uiDeviceType: %u, pIndexRange: [%u - %u], uiBufferSize: %u",
		iSessionID, pDomainCode, uiDeviceType, pIndexRange->uiFromIndex, pIndexRange->uiToIndex, uiBufferSize);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetDomainDeviceList(pDomainCode, uiDeviceType, pIndexRange, pDeviceList, uiBufferSize);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"pDomainCode=%s,uiDeviceType=%u,pIndexRange=[%u - %u], uiBufferSize=%u",pDomainCode, uiDeviceType, pIndexRange->uiFromIndex, pIndexRange->uiToIndex, uiBufferSize);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SwitchRealPlayTVWall(IVS_INT32 iSessionID, 
	const IVS_CHAR* pCameraCode,  
	const IVS_REALPLAY_PARAM* pRealplayParam,  
	const IVS_TVWALL_PARAM* pTVWallParam)
{
	SYSTEMTIME reqTime; 
	GetLocalTime(&reqTime);

	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,CameraCode: %s, DecoderCode: %s, Channel: %u", 
		iSessionID, pCameraCode, pTVWallParam->cDecoderCode, pTVWallParam->uiChannel);
	CHECK_POINTER(pRealplayParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTVWallParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().SwitchRealPlayTVWall(pCameraCode, pRealplayParam, pTVWallParam);
	SYSTEMTIME rspTime; 
	GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode=%s",pCameraCode);
	return iRet;
}


// 启动摄像机录像的智能分析轨迹叠加
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_EnablePUPlaybackIAOverLay(IVS_INT32 iSessionID,
	IVS_ULONG ulPlaybackHandle,
	const IVS_CHAR* pCameraCode,
	const IVS_CHAR* pNVRCode,
	HWND hWnd)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode, IVS_PARA_INVALID);
	CHECK_POINTER(hWnd, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d, RealPlayHandle: %lu, CameraCode: %s, NVRCode: %s", iSessionID, ulPlaybackHandle, pCameraCode, pNVRCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().EnablePUPlaybackIAOverLay(ulPlaybackHandle, pCameraCode, pNVRCode, hWnd);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"RealPlayHandle: %lu, CameraCode: %s, NVRCode", ulPlaybackHandle, pCameraCode, pNVRCode);
	return iRet;
}

// 停止摄像机录像的智能分析轨迹叠加
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DisablePUPlaybackIAOverLay(IVS_INT32 iSessionID,
	IVS_ULONG ulPlaybackHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	IVS_INFO_TRACE("SessionID: %d, RealPlayHandle: %lu", iSessionID, ulPlaybackHandle);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().DisablePUPlaybackIAOverLay(ulPlaybackHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"RealPlayHandle: %lu", ulPlaybackHandle);
	return iRet;
}

// 获取实况URL
IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_GetRtspURL(IVS_INT32 iSessionID, 
	const IVS_CHAR* pCameraCode, 
	const IVS_URL_MEDIA_PARAM *pURLMediaParam, 
	IVS_CHAR *pRtspURL, 
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d, CameraCode: %s,  uiBufferSize: %d", iSessionID, pCameraCode, uiBufferSize);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	if (STREAM_TYPE_UNSPECIFIED >= pURLMediaParam-> StreamType || STREAM_TYPE_SUB2 < pURLMediaParam-> StreamType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Rtsp URL failed", "Parameter StreamType invalid [%d]", pURLMediaParam-> StreamType);
		return IVS_DATA_INVALID;
	}

	if (MEDIA_TRANS > pURLMediaParam-> TransMode  || MEDIA_DIRECT < pURLMediaParam-> TransMode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Rtsp URL failed", "Parameter TransMode invalid [%d]", pURLMediaParam-> StreamType);
		return IVS_DATA_INVALID;
	}

	if (BROADCAST_UNICAST > pURLMediaParam-> BroadCastType || BROADCAST_MULTICAST < pURLMediaParam-> BroadCastType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Rtsp URL failed", "Parameter BroadCastType invalid [%d]", pURLMediaParam-> StreamType);
		return IVS_DATA_INVALID;
	}

	if (PACK_PROTOCOL_ES > pURLMediaParam->PackProtocolType || PACK_PROTOCOL_TS < pURLMediaParam->PackProtocolType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Rtsp URL failed", "Parameter PackProtocolType invalid [%d]", pURLMediaParam->PackProtocolType);
		return IVS_DATA_INVALID;
	}

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = -1;

	switch(pURLMediaParam->ServiceType)
	{
		case SERVICE_TYPE_REALVIDEO:
			{
				iRet = pUserMgr->GetRealPlayMgr().GetRealPlayRtspURL(pCameraCode, pURLMediaParam, pRtspURL, uiBufferSize);
				break;
			}
		case SERVICE_TYPE_DOWNLOAD:
			{
				iRet = pUserMgr->GetDownLoadMgr().GetDownloadRtspURL(pCameraCode, pURLMediaParam, pRtspURL, uiBufferSize);
				break;
			}	
		case SERVICE_TYPE_PLAYBACK:
			{
				iRet = pUserMgr->GetPlaybackMgr().GetPlaybackRtspURL(pCameraCode, pURLMediaParam, pRtspURL, uiBufferSize);
				break;
			}
		default:
			{
				BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get Rtsp URL failed", "not support ServiceType[%d]",  pURLMediaParam->ServiceType);
				return IVS_DATA_INVALID;
			}		
	}
	
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"CameraCode: %s", pCameraCode);
	return iRet;
}

//新增影子设备
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddShadowIPC(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode,
	const IVS_SHADOW_IPC_INFO* pShadowIPCInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pShadowIPCInfo, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d,DevCode: %s",pShadowIPCInfo->cDevCode);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_SHADOW_IPC_INFO ShadowDevInfo = *pShadowIPCInfo;
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().AddShadowDev(pDomainCode, ShadowDevInfo);

	BP_RUN_LOG_INF("sdk Add Shadow Dev", "finish steps, result code:%d", iRet);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"dev code:%s", pShadowIPCInfo->cDevCode);

	return iRet;
}

//影子设备手动同步
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SYNC_ShadowIPC(IVS_INT32 iSessionID, 
	const IVS_SHADOW_IPC_LIST* pShadowIPCList)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pShadowIPCList, IVS_PARA_INVALID);

	IVS_INFO_TRACE("SessionID: %d",iSessionID);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().ShadowDevSyncNotify(pShadowIPCList);

	BP_RUN_LOG_INF("sdk Add Shadow Dev", "finish steps, result code:%d", iRet);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet, "");

	return iRet;
}

// 设置画图回调
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetDrawCallBackEx(IVS_INT32 iSessionID, IVS_ULONG ulPlayHandle, HWND hWnd, DRAW_PROC callback, void *pUserData)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,PlayHandle: %lu",iSessionID,ulPlayHandle);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	 // callback和pUserData可以为空

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	CMediaBase* mediaBase = pUserMgr->GetRealPlayMgr().GetMediaBase(ulPlayHandle);
	if (NULL == mediaBase)
	{
	    mediaBase = pUserMgr->GetPlaybackMgr().GetMediaBase(ulPlayHandle);
	}

	if (NULL != mediaBase)
	{
		IVS_INT32 iRet = mediaBase->SetDrawCallBackEx(hWnd, callback, pUserData);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"PlayHandle=%lu",ulPlayHandle);
		return iRet;
	}
	return IVS_FAIL;
	
}


//设置逐级转发
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetProgressionTransmit(IVS_INT32 iSessionID, 
	IVS_BOOL  bSupportProgressionTransmit, const IVS_CHAR* pProgressionDomainCode)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);
	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d",iSessionID);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->SetProgressionTransmit(bSupportProgressionTransmit, pProgressionDomainCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayByChannel(IVS_INT32 iSessionID, 
	IVS_REALPLAY_PARAM* pRealplayPara, 
	IVS_UINT32 uiChannel, 
	HWND hWnd, 
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_PARA_INVALID);
	IVS_INFO_TRACE("ChannelID: %d",uiChannel);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlay(pRealplayPara, pDevCode, hWnd, pHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"uiChannel=%lu",uiChannel);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordListByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiChannel, 
	IVS_INT32 iRecordMethod,
	const IVS_TIME_SPAN* pTimeSpan,
	const IVS_INDEX_RANGE* pIndexRange, 
	IVS_RECORD_INFO_LIST* pRecordList, 
	IVS_UINT32 uiBufSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordList, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,ChannelID: %u,RecordMethod: %d,BufSize: %u",iSessionID,uiChannel,iRecordMethod,uiBufSize);

	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RecordList Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet =  pUserMgr->GetRecordList().GetRecordListByChannel(uiChannel,iRecordMethod,pTimeSpan,pIndexRange,pRecordList,uiBufSize); 
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"ChannelID=%u,RecordMethod=%d,BufSize=%u",uiChannel,iRecordMethod,uiBufSize);
	return iRet;
}



IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_QueryRecordListByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiChannel,
	IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	IVS_RECORD_INFO_LIST* pRecordInfoList,
	IVS_UINT32 uiBufferSize)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d,BufSize: %u",iSessionID,uiBufferSize);
	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

	if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Query RecordList Failed.","Reason:SessionID is invalid");

		eSDK_LOG_RUN_WRAN("SessionID is invalid.");
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetRecordList().QueryRecordListByChannel(uiChannel, pUnifiedQuery, pRecordInfoList, uiBufferSize);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"BufSize=%u",uiBufferSize);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordTaskByChannel(IVS_INT32 iSessionID, IVS_CHAR** pRspXml)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d, pRspXml: %s",iSessionID, pRspXml);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	 (void)pUserMgr->GetNVRChannelMgr().GetCameraCodeByChannel(MAX_CHANNELID_VALUE+1); 
	IVS_INT32 iRet = pUserMgr->GetDeviceMgr().GetRecordTaskByChannel(pUserMgr->GetUserID(),pUserMgr->GetDeviceMgr(), pRspXml);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"");
	return iRet;
}



IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBRawByChannel(IVS_INT32 iSessionID, 
	IVS_REALPLAY_PARAM* pRealplayPara, 
	IVS_UINT32 uiChannel, 
	RealPlayCallBackRaw fRealPlayCallBackRaw, 
	void* pUserData, 
	IVS_ULONG* pHandle )
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u", iSessionID, uiChannel);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlayCBRaw(pRealplayPara, pDevCode, fRealPlayCallBackRaw, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartRealPlayCBFrameByChannel(IVS_INT32 iSessionID, 
	IVS_REALPLAY_PARAM* pRealplayPara, 
	IVS_UINT32 uiChannel, 
	RealPlayCallBackFrame fRealPlayCallBackFrame, 
	void* pUserData, 
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRealplayPara, IVS_PARA_INVALID);
	CHECK_POINTER(pUserData, IVS_PARA_INVALID);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u",iSessionID, uiChannel);	

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);	
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	
	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];	
	(void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRealPlayMgr().StartRealPlayCBFrame(pRealplayPara, pDevCode, fRealPlayCallBackFrame, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformRecordByChannel(IVS_INT32 iSessionID, IVS_UINT32 uiChannel, IVS_INT32 iRecordTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u,RecordTime: %d",iSessionID,uiChannel,iRecordTime);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	(void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StartPlatformRecord(pDevCode,iRecordTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u,RecordTime=%d",uiChannel,iRecordTime);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPlatformRecordByChannel(IVS_INT32 iSessionID, IVS_UINT32 uiChannel)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u",iSessionID,uiChannel);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StopPlatformRecord(pDevCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}



IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPURecordByChannel(IVS_INT32 iSessionID, IVS_UINT32 uiChannel, IVS_INT32 iRecordTime)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u,RecordTime: %d",iSessionID,uiChannel,iRecordTime);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	(void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StartPURecord(pDevCode,iRecordTime);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u,RecordTime=%d",uiChannel,iRecordTime);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StopPURecordByChannel(IVS_INT32 iSessionID, IVS_UINT32 uiChannel)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u",iSessionID,uiChannel);	
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRecordService().StopPURecord(pDevCode);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordStatusByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiChannel, 
	IVS_UINT32 uiRecordMethod, 
	IVS_UINT32* pRecordState)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d,Channel: %u,RecordMethod: %u",iSessionID,uiChannel,uiRecordMethod);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
	 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
	IVS_INT32 iRet = pUserMgr->GetRecordList().GetRecordStatus(pDevCode, uiRecordMethod, pRecordState);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u,RecordMethod=%u",uiChannel,uiRecordMethod);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformDownloadByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiChannel,
	const IVS_DOWNLOAD_PARAM* pDownloadParam, 
	IVS_ULONG* pulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pDownloadParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pulHandle,      IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Channel: %u",iSessionID, uiChannel);

	// 命名规则
	if (pDownloadParam->uiNameRule < RULE_NAME_NUM_TIME ||
		pDownloadParam->uiNameRule > RULE_TIME_NAME_NUM)
	{
		BP_RUN_LOG_INF("Start Platform Record Download", "Name Rule=[%u]", pDownloadParam->uiNameRule);
		return IVS_PARA_INVALID;
	}

	// 文件格式
	if (pDownloadParam->uiRecordFormat < IVS_FILE_MP4 || 
		pDownloadParam->uiRecordFormat > IVS_FILE_AVI)
	{
		BP_RUN_LOG_INF("Start Platform Record Download", "Record Format=[%u]", pDownloadParam->uiRecordFormat);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型
	if (pDownloadParam->uiSplitterType < RECORD_SPLITE_TIME ||
		pDownloadParam->uiSplitterType > RECORD_SPLITE_CAPACITY)
	{
		BP_RUN_LOG_INF("Start Platform Record Download", "Splitter Type=[%u]", pDownloadParam->uiSplitterType);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型值
	if (RECORD_SPLITE_TIME == pDownloadParam->uiSplitterType)
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_TIME_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_TIME_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Platform Record Download", "Splitter Time=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_SPACE_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_SPACE_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start Platform Record Download", "Splitter FileSize=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StartPlatformDownLoadByChannel(NULL,pDownloadParam->cNVRCode,uiChannel, pDownloadParam, pulHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUDownloadByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiChannel,
	const IVS_DOWNLOAD_PARAM* pDownloadParam, 
	IVS_ULONG* pulHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(pDownloadParam, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pulHandle, IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("SessionID: %d, Channel: %u", iSessionID, uiChannel);

	// 命名规则
	if (pDownloadParam->uiNameRule < RULE_NAME_NUM_TIME ||
		pDownloadParam->uiNameRule > RULE_TIME_NAME_NUM)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Name Rule=[%u]", pDownloadParam->uiNameRule);
		return IVS_PARA_INVALID;
	}

	// 文件格式
	if (pDownloadParam->uiRecordFormat < IVS_FILE_MP4 || 
		pDownloadParam->uiRecordFormat > IVS_FILE_AVI)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Record Format=[%u]", pDownloadParam->uiRecordFormat);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型
	if (pDownloadParam->uiSplitterType < RECORD_SPLITE_TIME ||
		pDownloadParam->uiSplitterType > RECORD_SPLITE_CAPACITY)
	{
		BP_RUN_LOG_INF("Start PU Record Download", "Splitter Type=[%u]", pDownloadParam->uiSplitterType);
		return IVS_PARA_INVALID;
	}

	// 文件分割类型值
	if (RECORD_SPLITE_TIME == pDownloadParam->uiSplitterType)
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_TIME_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_TIME_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start PU Record Download", "Splitter Time=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (pDownloadParam->uiSplitterValue < IVS_MIN_SPACE_SPLITTER_VALUE || 
			pDownloadParam->uiSplitterValue > IVS_MAX_SPACE_SPLITTER_VALUE)
		{
			BP_RUN_LOG_INF("Start PU Record Download", "Splitter FileSize=[%u]", pDownloadParam->uiSplitterValue);
			return IVS_PARA_INVALID;
		}
	}

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetDownLoadMgr().StartPUDownLoadByChannel(uiChannel, pDownloadParam, pulHandle);

	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}


IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackByChannel(IVS_INT32 iSessionID,
	IVS_UINT32 uiChannel,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	HWND hWnd,
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack SessionID: %d,Channel: %u",iSessionID,uiChannel);
	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);

	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackByChannel(uiChannel, pPlaybackParam, hWnd, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBRawByChannel(IVS_INT32 iSessionID,
	IVS_UINT32 uiChannel,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	PlayBackCallBackRaw fPlayBackCallBackRaw, 
	void* pUserData,
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(fPlayBackCallBackRaw, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack CBRaw SessionID: %d,Channel: %u",iSessionID,uiChannel);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackCBByChannel(uiChannel, pPlaybackParam, fPlayBackCallBackRaw, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}

IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPlatformPlayBackCBFrameByChannel(IVS_INT32 iSessionID,
	IVS_UINT32 uiChannel,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	PlayBackCallBackFrame fPlayBackCallBackFrame, 
	void* pUserData,
	IVS_ULONG* pHandle)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
	CHECK_POINTER(fPlayBackCallBackFrame, IVS_PARA_INVALID);
	IVS_INFO_TRACE("Start Platform PlayBack CBFrame SessionID: %d,Channel: %u",iSessionID,uiChannel);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPlatformPlayBackCBFrameByChannel(uiChannel, pPlaybackParam, fPlayBackCallBackFrame, pUserData, pHandle);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
	return iRet;
}



	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_StartPUPlayBackByChannel(
	IVS_INT32 iSessionID,
	IVS_UINT32 uiChannel,
	const IVS_PLAYBACK_PARAM* pPlaybackParam,
	HWND hWnd,
	IVS_ULONG* pHandle)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
		CHECK_POINTER(pPlaybackParam, IVS_PARA_INVALID);
		IVS_INFO_TRACE("Start PU Play Back SessionID: %d,Channel: %u",iSessionID,uiChannel);

		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr,IVS_SDK_RET_INVALID_SESSION_ID);
		IVS_INT32 iRet = pUserMgr->GetPlaybackMgr().StartPUPlayBackByChannel(uiChannel, pPlaybackParam, hWnd, pHandle);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);	
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
		return iRet;
	}


	IVS_SDK_API IVS_INT32 __SDK_CALL  IVS_SDK_AddRecordPlanByChannel(IVS_INT32  iSessionID, 
	IVS_UINT32  uiDeviceNum, 
	const IVS_CHANNEL  *pChannelList, 
	const IVS_RECORD_PLAN  *pRecordPlan, 
	IVS_RESULT_LIST  *pResultList, 
	IVS_UINT32  uiResultBufferSize)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		IVS_INFO_TRACE("Add Record Plan SessionID: %d,DeviceNum: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiResultBufferSize);
		CHECK_POINTER(pChannelList, IVS_PARA_INVALID);
		CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
		CHECK_POINTER(pResultList, IVS_PARA_INVALID);
		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Add Record Plan Failed.","Reason:SessionID is invalid");

			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}

		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
		CRecordPlanMgr& recordPlanMgr = pUserMgr->GetRecordPlanMgr();
		recordPlanMgr.SetUserMgr(pUserMgr); 
	
		IVS_DEVICE_CODE* pDeviceList = NULL;
		(void)pUserMgr->GetNVRChannelMgr().GetMultiCameraCodeByChannelList(uiDeviceNum, pChannelList, &pDeviceList);
		IVS_INT32 iRet = recordPlanMgr.AddRecordPlan(uiDeviceNum,pDeviceList, pRecordPlan ,pResultList, uiResultBufferSize);
		IVS_DELETE(pDeviceList, MUILI);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,ResultBufferSize=%u",uiDeviceNum,uiResultBufferSize);
		return iRet;
	}

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_ModifyRecordPlanByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiDeviceNum, 
	const IVS_CHANNEL  *pChannelList, 
	const IVS_RECORD_PLAN  *pRecordPlan, 
	IVS_RESULT_LIST  *pResultList, 
	IVS_UINT32 uiResultBufferSize )
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		IVS_INFO_TRACE("Modify Record Plan SessionID: %d,DeviceNum: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiResultBufferSize);
		CHECK_POINTER(pChannelList, IVS_PARA_INVALID);
		CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
		CHECK_POINTER(pResultList, IVS_PARA_INVALID);
		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Modify RecordPlan Failed.","Reason:SessionID is invalid");

			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}
		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

		IVS_DEVICE_CODE* pDeviceList = NULL;
		 (void)pUserMgr->GetNVRChannelMgr().GetMultiCameraCodeByChannelList(uiDeviceNum, pChannelList, &pDeviceList);
		IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().ModifyRecordPlan(uiDeviceNum,pDeviceList, pRecordPlan ,pResultList, uiResultBufferSize);
		IVS_DELETE(pDeviceList, MUILI);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,ResultBufferSize=%u",uiDeviceNum,uiResultBufferSize);
		return iRet;
	}


	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DeleteRecordPlanByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32 uiDeviceNum, 
	const IVS_CHANNEL   *pChannelList, 
	IVS_UINT32  uiRecordMethod, 
	IVS_RESULT_LIST *pResultList, 
	IVS_UINT32  uiResultBufferSize)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		IVS_INFO_TRACE("SessionID: %d,DeviceNum: %u,RecordMethod: %u,ResultBufferSize: %u",iSessionID,uiDeviceNum,uiRecordMethod,uiResultBufferSize);
		CHECK_POINTER(pChannelList, IVS_PARA_INVALID);
		CHECK_POINTER(pResultList, IVS_PARA_INVALID);
		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Delete RecordPlan Failed.","Reason:SessionID is invalid");
			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}
		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

		IVS_DEVICE_CODE* pDeviceList = NULL;
		(void)pUserMgr->GetNVRChannelMgr().GetMultiCameraCodeByChannelList(uiDeviceNum, pChannelList, &pDeviceList);
		IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().DeleteRecordPlan(uiDeviceNum, pDeviceList, uiRecordMethod,pResultList, uiResultBufferSize);
		IVS_DELETE(pDeviceList, MUILI);
		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"DeviceNum=%u,RecordMethod=%u,ResultBufferSize=%u",uiDeviceNum,uiRecordMethod,uiResultBufferSize);
		return iRet;
	}

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPlanByChannel(IVS_INT32 iSessionID, 
	IVS_UINT32  uiChannel, 
	IVS_UINT32   uiRecordMethod, 
	IVS_RECORD_PLAN *pRecordPlan)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		CHECK_POINTER(pRecordPlan, IVS_PARA_INVALID);
		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
		IVS_INFO_TRACE("SessionID: %d,Channel :%u,RecordMethod: %u",iSessionID,uiChannel,uiRecordMethod);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get RecordPlan Failed.","Reason:SessionID is invalid");

			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}
		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

		IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
		 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
		IVS_INT32 iRet = pUserMgr->GetRecordPlanMgr().GetRecordPlan(pDevCode, uiRecordMethod, pRecordPlan);

		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u,RecordMethod=%u",uiChannel,uiRecordMethod);
		return iRet;
	}

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_SetRecordPolicyByTimeByChannel(IVS_INT32  iSessionID, 
	IVS_UINT32  uiChannel, 
	const IVS_RECORD_POLICY_TIME  *pRecordPolicy)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
		CHECK_POINTER(pRecordPolicy, IVS_PARA_INVALID);
		IVS_INFO_TRACE("SessionID: %d,Channel: %u",iSessionID,uiChannel);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Set RecordPolicy By Time Failed.", "Reason:SessionID is invalid");

			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}
		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

		IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
		 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
		IVS_INT32 iRet = pUserMgr->GetRecordService().SetRecordPolicy(pDevCode,pRecordPolicy);

		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"uiChannel=%u",uiChannel);
		return iRet;
	}

	IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_GetRecordPolicyByTimeByChannel(IVS_INT32  iSessionID, 
	IVS_UINT32  uiChannel, 
	IVS_RECORD_POLICY_TIME  *pRecordPolicy)
	{
		SYSTEMTIME reqTime; GetLocalTime(&reqTime);

		CHECK_POINTER(pRecordPolicy, IVS_PARA_INVALID);
		CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
		IVS_INFO_TRACE("SessionID: %d,Channel: %u",iSessionID,uiChannel);

		if((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
		{
			BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"IVS SDK Get Record Policy By Time Failed.","Reason:SessionID is invalid");

			eSDK_LOG_RUN_WRAN("SessionID is invalid.");
			SYSTEMTIME rspTime; GetLocalTime(&rspTime);
			INT_LOG_SESSION(iSessionID,reqTime,rspTime,IVS_SDK_RET_INVALID_SESSION_ID,"");
			return IVS_SDK_RET_INVALID_SESSION_ID;
		}
		CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
		CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);

		IVS_CHAR pDevCode[IVS_DEV_CODE_LEN];
		 (void)pUserMgr->GetNVRChannelMgr().GetUniCameraCodeByChannel(uiChannel, pDevCode);
		IVS_INT32 iRet = pUserMgr->GetRecordService().GetRecordPolicy(pDevCode,pRecordPolicy);

		SYSTEMTIME rspTime; GetLocalTime(&rspTime);
		INT_LOG_SESSION(iSessionID,reqTime,rspTime,iRet,"Channel=%u",uiChannel);
		return iRet;
	}

// HIK decoder login
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_AddDecoder(IVS_INT32 iSessionID, CUMW_DECODER_CONNECT_TYPE emConnectType, const SDK_DECODER_LOGIN_INFO *pSDKDecoderLoginInfo)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d", iSessionID);
	CHECK_POINTER(pSDKDecoderLoginInfo, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().DecoderLogin(emConnectType, pSDKDecoderLoginInfo);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"DecoderName=%s", pSDKDecoderLoginInfo->cDecoderName);
	return iRet;
}


// HIK decoder logout
IVS_SDK_API IVS_INT32 __SDK_CALL IVS_SDK_DelDecoder(IVS_INT32 iSessionID, CUMW_DECODER_CONNECT_TYPE emConnectType, const IVS_CHAR* pDecoderID)
{
	SYSTEMTIME reqTime; GetLocalTime(&reqTime);

	IVS_INFO_TRACE("SessionID: %d", iSessionID);
	CHECK_POINTER(pDecoderID, IVS_OPERATE_MEMORY_ERROR);

	CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
	IVS_INT32 iRet = pUserMgr->GetTVWallMgr().DecoderLogout(emConnectType, pDecoderID);
	SYSTEMTIME rspTime; GetLocalTime(&rspTime);
	INT_LOG_NO_SESSION(reqTime,rspTime,iRet,"");
	return iRet;
}












