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

#include "StdAfx.h"
#include "HikDecoderSDK.h"
#include "vos_common.h"
#include "HCNetSDK.h"
#include "ToolsHelp.h"
#include "SDKConfig.h"
#define MAX_WALL_WIN_COUNT      256
#define MAX_VW_DISPLAYPOSITION  256

typedef struct
{
    NET_DVR_RECTCFG_EX rect;
    IVS_INT32 iLinkMode;
}HC_RECT_LINKE_MODE;

const std::string DisplayLinkMode[] = {"NONE", "BNC", "VGA", "HDMI", "DVI", "SDI", "FIBER", "RGB", "YPrPb", "VGA/HDMI/DVI", "3GSDI", "VGA/DVI"}; 
//lint -e1960
CHikDecoderSDK::CHikDecoderSDK(void)
    :m_bInitHikSdk(false),m_bHikDecPlay(false)
{
    m_DecodeRegInfoMap.clear();
    m_DecoderInfoMap.clear();
    m_pUserData = NULL;
    m_CheckDecThreadRun = false;
    m_pDecodeRegInfoMapMutex = VOS_CreateMutex();
    m_Tigger.SetHikDecoderSDK(this);
    m_pFunCallBack = NULL;
    m_pCheckDecThread = NULL;
}


CHikDecoderSDK::~CHikDecoderSDK(void)
{
    m_CheckDecThreadRun = false;
    try
    {
        DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.begin();
        for(;it!=m_DecodeRegInfoMap.end();it++)
        {
            (void)NET_DVR_Logout(it->first);
        }
        m_DecodeRegInfoMap.clear();
        m_DecoderInfoMap.clear();
    }catch(...)
    {
    }

    if(NULL != m_pCheckDecThread)
    {
        unsigned long uiRet = WaitForSingleObject(m_pCheckDecThread->pthead, 500);

        //线程未结束，强制杀死线程
        if (WAIT_OBJECT_0 != uiRet)//lint !e835 !e1924
        {
            BP_RUN_LOG_INF("CHikDecoderSDK::~CHikDecoderSDK:", "kill the thread, WaitForSingleObject iRet:%u", uiRet);
            DWORD dwExitWord = 0;
            (void)GetExitCodeThread(m_pCheckDecThread->pthead,&dwExitWord);
            (void)TerminateThread(m_pCheckDecThread->pthead,dwExitWord);
        }
        (void)CloseHandle(m_pCheckDecThread->pthead);
        m_pCheckDecThread->pthead = NULL;
        m_pCheckDecThread = NULL;
    }
    if (NULL != m_pDecodeRegInfoMapMutex)
    {
        VOS_DestroyMutex(m_pDecodeRegInfoMapMutex);
        m_pDecodeRegInfoMapMutex = NULL;
    }

    if (m_bInitHikSdk)
    {
        (void)NET_DVR_Cleanup();
    }
    
    m_pFunCallBack = NULL;
    m_pUserData = NULL;
    
}

//注册给海康SDK的异常回调函数
void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    BP_DBG_LOG("CHikDecoderSDK::g_ExceptionCallBack: dwType = %d, lUserID = %d, lHandle = %d,", dwType, lUserID, lHandle);

    CHECK_POINTER_VOID(pUser);
    CHikDecoderSDK* pHikDecoderSDK = (CHikDecoderSDK*)pUser;


    switch(dwType)
    {
    case EXCEPTION_EXCHANGE: //the device is off line
        {
            BP_RUN_LOG_INF("CHikDecoderSDK::g_ExceptionCallBack: ","Dec Disconnect.");
            pHikDecoderSDK->SetDecOnlineStatus(lUserID, false);
            pHikDecoderSDK->NotifyDecDisconn(lUserID);
        }
        break;
    case RESUME_EXCHANGE://the device is on line
        {
            BP_RUN_LOG_INF("CHikDecoderSDK::g_ExceptionCallBack: ","Dec Resume Connect Suc.");
            pHikDecoderSDK->SetDecOnlineStatus(lUserID, true);
            pHikDecoderSDK->NotifyDecResumeConn(lUserID);
        }
        break;
    default:
        break;
    }
};

IVS_INT32 CHikDecoderSDK::InitHik(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
    CHECK_POINTER(pUserData,IVS_TVWALL_MEMORY_NULL);
    CHECK_POINTER(pFunNotifyCallBack,IVS_TVWALL_MEMORY_NULL);

    if (m_bInitHikSdk)
    {
        return IVS_SUCCEED;
    }

    m_pUserData = pUserData;             //用户数据指针
    m_pFunCallBack = pFunNotifyCallBack; //回调函数指针
    
    if (!NET_DVR_Init())
    {
        DWORD DRes = NET_DVR_GetLastError();
        BP_RUN_LOG_ERR((IVS_INT32)DRes,"InitHik","failed");
        return (IVS_INT32)DRes;
    }

    //注册接收异常消息的回调函数
    (void)NET_DVR_SetExceptionCallBack_V30(WM_NULL, NULL, g_ExceptionCallBack, this);

	// 设置Hik解码器日志路径
    std::string sRootLogPath = CSDKConfig::instance().GetLogPath();
	if (!sRootLogPath.empty())
	{
		IVS_CHAR lastChar= sRootLogPath[sRootLogPath.length() - 1];
		if(lastChar=='\\'||lastChar=='/')
		{
			sRootLogPath.append("HCSDKLog\\");
		}else
		{
			sRootLogPath.append("\\HCSDKLog\\");
		}
	}
	else
	{
		std::string strDllDir;
		CToolsHelp::GetDLLPath("IVS_SDK.dll", strDllDir);
		sRootLogPath = strDllDir;
		std::string strNetSourceLogPath = CSDKConfig::instance().GetNetSourceLogPath();
		sRootLogPath.append("\\" + strNetSourceLogPath);
		sRootLogPath.append("\\HCSDKLog\\");
	}

    IVS_ULONG pathLen = sRootLogPath.length() + 1;
    IVS_CHAR* logPath = VOS_NEW(logPath, pathLen);
	if (NULL == logPath)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Init Hik decoder failed", "new logpath failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    eSDK_MEMSET(logPath, 0, pathLen);
    eSDK_MEMCPY(logPath, pathLen, sRootLogPath.c_str(), pathLen - 1);

    (void)NET_DVR_SetLogToFile(3UL,logPath, TRUE);
    VOS_DELETE(logPath,MULTI);

    (void)NET_DVR_SetReconnect(30000UL,TRUE);
    
    NET_DVR_LOCAL_CHECK_DEV cLocalCheckDev = {0};
    cLocalCheckDev.dwCheckOnlineTimeout=30;
    cLocalCheckDev.dwCheckOnlineNetFailMax=3;
    (void)NET_DVR_SetSDKLocalCfg(NET_SDK_LOCAL_CFG_TYPE_CHECK_DEV,&cLocalCheckDev);
    m_bInitHikSdk = true;
    
    m_CheckDecThreadRun = true;
    StartCheckDecThread();

    BP_RUN_LOG_INF("InitHik", "Init Suc!");

    return IVS_SUCCEED;
}


IVS_INT32 CHikDecoderSDK::Login(IVS_LONG& ulSessionID,const DECODER_DEV* pDecoder,const IVS_CHAR* pPwd)
{
    BP_DBG_LOG("Login Start. IP = %s, Port= %d, Username = %s.", pDecoder->cDevIP, pDecoder->iPort,  pDecoder->cUser);
    NET_DVR_DEVICEINFO_V30 stDeviceInfo = {0};
    LONG lRes = NET_DVR_Login_V30((char*)pDecoder->cDevIP, (WORD)pDecoder->iPort, (char*)pDecoder->cUser, (char*)pPwd, &stDeviceInfo); 
    if (lRes < 0)
    {
		IVS_INT32 iErrorCode = NET_DVR_GetLastError();
        BP_RUN_LOG_ERR(lRes, "CHikDecoderSDK::Login: Login fail.","IP = %s, ErrorCode = %d", pDecoder->cDevIP, iErrorCode);	
        if (NET_DVR_PASSWORD_ERROR == iErrorCode) //用户名密码错误!
        {
            return IVS_PASSWORD_ERROR;
        }
		else if(NET_DVR_USER_LOCKED == iErrorCode)	//device/user locked
		{
			return IVS_SMU_USER_LOCKED;
		}
        else //由于网络原因或解码器忙, 注册失败!
        {	
            return IVS_TVWALL_NET_FAIL;
        }
    }
    BP_RUN_LOG_INF("Login Decoder Suc!", "IP = %s, lRes = %d.", pDecoder->cDevIP, lRes);
    
    ulSessionID = (IVS_LONG)lRes;
    BP_DBG_LOG("Login End.");

    return IVS_SUCCEED;
}


IVS_INT32 CHikDecoderSDK::Logout(IVS_LONG ulSessionID)
{
    BP_DBG_LOG("Logout. ulSessionID = %d.", ulSessionID);
	std::string stDecoderID("");
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find((LONG)ulSessionID);
    if(m_DecodeRegInfoMap.end() != it)
    {
		stDecoderID = it->second.cDecoderID;
        m_DecoderInfoMap.erase(it->second.cDecoderID);
        m_DecodeRegInfoMap.erase(it);
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);

    if (!NET_DVR_Logout(ulSessionID))
    {
		IVS_INT32 iRet = (IVS_INT32)NET_DVR_GetLastError();
        BP_RUN_LOG_ERR(iRet, "Logout", " Logout fail, ulSessionID = %d, decoderid: %s .", ulSessionID, stDecoderID.c_str());
		return IVS_FAIL;
    }
    BP_DBG_LOG("Logout success, ulSessionID = %d, decoderid: %s .", ulSessionID, stDecoderID.c_str());
    return IVS_SUCCEED;
}


int GetSubWin(IVS_ULONG dwWinNo,IVS_LONG lSessionID)
{
	NET_DVR_WALLWINPARAM struWinWallParam = {0}; 
	DWORD dwReturned ; 
	if (!NET_DVR_GetDVRConfig(lSessionID, NET_DVR_WALLWINPARAM_GET, (LONG)dwWinNo, &struWinWallParam, sizeof(struWinWallParam), &dwReturned))
	{
		return -1;
	}
	return (struWinWallParam.byWinMode==0)?1:struWinWallParam.byWinMode;
}


IVS_INT32 CHikDecoderSDK::GetChannels(IVS_LONG ulSessionID,DECODER_DEV* pDecoder)
{
	 BP_DBG_LOG("GetChannels. ulSessionID = %d.", ulSessionID);
    IVS_ULONG ulOutBufferSize=sizeof(IVS_ULONG) + MAX_WALL_WIN_COUNT *sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
    IVS_CHAR* pTemp=VOS_NEW((IVS_CHAR*&)pTemp,ulOutBufferSize);
    eSDK_MEMSET(pTemp, 0, ulOutBufferSize);
    IVS_ULONG ulWallNo = 1; 
    ulWallNo <<= 24; 
    if (!NET_DVR_GetDeviceConfig((LONG)ulSessionID, NET_DVR_GET_VIDEOWALLWINDOWPOSITION, 0xffffffff, &ulWallNo, sizeof(IVS_ULONG), NULL, pTemp, ulOutBufferSize))
    {
        VOS_DELETE(pTemp,MULTI);
        DWORD errorCode= NET_DVR_GetLastError();
        BP_RUN_LOG_ERR(IVS_FAIL,"Get Hik decoder channels failed","NET_DVR_GetDeviceConfig,%d,%d,%d",errorCode,ulSessionID,ulWallNo);
        return IVS_FAIL;
    }
 
    NET_DVR_DISPLAYCFG struDisplayCfg = {0};
    IVS_ULONG bytesReturned = 0;
    HC_RECT_LINKE_MODE* pRectLinkMode = NULL;
    IVS_ULONG displayCount = 0;
    //先获取各个连接方式，VGA,HDMI,BNC等
    if(!NET_DVR_GetDVRConfig((LONG)ulSessionID,NET_DVR_GET_VIDEOWALLDISPLAYNO,0,&struDisplayCfg,sizeof(NET_DVR_DISPLAYCFG),&bytesReturned))
    {
        DWORD errorCode= NET_DVR_GetLastError();
        BP_RUN_LOG_ERR(IVS_FAIL,"Get Hik decoder channels error","NET_DVR_GetDVRConfig,%d",errorCode);
    }else
    {
        //再获取所有窗口的坐标，记录每个窗口坐标和连接方式pRect
        DWORD ulDisplayOutBufferSize=sizeof(DWORD) + MAX_VW_DISPLAYPOSITION * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION);
        IVS_CHAR* pTempDisplay=VOS_NEW(pTempDisplay,ulDisplayOutBufferSize);
        if(!NET_DVR_GetDeviceConfig((LONG)ulSessionID,NET_DVR_GET_VIDEOWALLDISPLAYPOSITION,0xffffffff,&ulWallNo,sizeof(ulWallNo),NULL,pTempDisplay,ulDisplayOutBufferSize))
        {
            IVS_ULONG errorCode= NET_DVR_GetLastError();
            BP_RUN_LOG_ERR(IVS_FAIL,"Get Hik decoder channels error","NET_DVR_GetDVRConfig,%d",errorCode);
        }else
        {
            displayCount=*((DWORD*)pTempDisplay); //lint !e826
            (void)VOS_NEW(pRectLinkMode,displayCount*sizeof(HC_RECT_LINKE_MODE));
            
            const NET_DVR_VIDEOWALLDISPLAYPOSITION* const pDipPostion = (LPNET_DVR_VIDEOWALLDISPLAYPOSITION)(pTempDisplay+sizeof(DWORD));//lint !e826
            for (IVS_UINT32 i = 0; i < displayCount; i++)
            {
                if(pDipPostion[i].byEnable&&pDipPostion[i].dwVideoWallNo==ulWallNo)
                {
                    for (IVS_UINT32 j = 0; j < MAX_DISPLAY_NUM; j++)
                    {
                        if(struDisplayCfg.struDisplayParam[j].dwDisplayNo==pDipPostion[i].dwDisplayNo)
                        {
                            pRectLinkMode[i].rect=pDipPostion[i].struRectCfg;
                            pRectLinkMode[i].iLinkMode=struDisplayCfg.struDisplayParam[j].byDispChanType;
                            break;
                        }
                    }
                }
            }
        }
        VOS_DELETE(pTempDisplay,MULTI);
    }
   
    IVS_ULONG ulWinNos[MAX_WALL_WIN_COUNT]={0};
    IVS_ULONG ulWinCount = *((IVS_ULONG*)pTemp); //lint !e826
    BP_DBG_LOG("Get HIK win count %d",ulWinCount);
    LPNET_DVR_VIDEOWALLWINDOWPOSITION lpWinPos = (LPNET_DVR_VIDEOWALLWINDOWPOSITION)(pTemp + sizeof(IVS_ULONG));  //lint !e826
    std::map<IVS_INT32,IVS_INT32> mapWinNoLinkMode;
	//ioffSet偏移量 i为窗口  j为子窗口
	int ioffSet = 0; 
    for (int i = 0; i < (int)ulWinCount; i++)
    {
        //通过窗口坐标获取连接方式
        if(NULL != pRectLinkMode)
        {
            for (int j = 0; j  < (int)displayCount; j ++)
            {
                if(pRectLinkMode[j].rect.dwXCoordinate==lpWinPos[i].struRect.dwXCoordinate&&pRectLinkMode[j].rect.dwYCoordinate==lpWinPos[i].struRect.dwYCoordinate)
                {
                    (void)mapWinNoLinkMode.insert(std::make_pair(lpWinPos[i].dwWindowNo&0xff,pRectLinkMode[j].iLinkMode));
                    break;
                }
            }
        }
		int iSubMode = GetSubWin(lpWinPos[i].dwWindowNo,ulSessionID); 
        BP_DBG_LOG("Get HIK sub wind %d,%d",lpWinPos[i].dwWindowNo,iSubMode);
		if ( iSubMode == -1)
		{
            ulWinNos[i+ioffSet] = lpWinPos[i].dwWindowNo;
			continue;
		}
        for (int j=0; j < iSubMode; j++)
        {
			ulWinNos[i+ioffSet+j] = ((j+1)<<16) + lpWinPos[i].dwWindowNo; 
        }
		ioffSet += iSubMode - 1; 
    }  
	ulWinCount += (IVS_ULONG)ioffSet; 
    VOS_DELETE(pTemp,MULTI);
    
    for (int i = 0; i < (int)ulWinCount; i++)
    {
        CUMW_DECODER_CHANNEL_INFO* channel=&pDecoder->stChannels[i];   
        channel->ulChannelID=ulWinNos[i]; 
        std::string sChannelName;
        IVS_INT32 iWndNo = ulWinNos[i]&0xff;
        std::map<IVS_INT32,IVS_INT32>::iterator iter=mapWinNoLinkMode.find(iWndNo);
        if(iter!=mapWinNoLinkMode.end())
        {
            sChannelName.append(DisplayLinkMode[iter->second]);
        }
        sChannelName.append("_");
        sChannelName.append(CToolsHelp::Long2Str(iWndNo));
        sChannelName.append("_");
        sChannelName.append(CToolsHelp::Long2Str(ulWinNos[i]>>16&0xff));
        eSDK_MEMCPY(channel->szChannelName,CUMW_CHANNEL_NAME_LEN + 1,sChannelName.c_str(),CUMW_CHANNEL_NAME_LEN + 1);
    }
    if(NULL!=pRectLinkMode)
    {
        VOS_DELETE(pRectLinkMode,MULTI);
    }
    pDecoder->iChannelNum=(int)ulWinCount;
    SaveDecInfo(ulSessionID, *pDecoder);

    return IVS_SUCCEED;
}


IVS_INT32 CHikDecoderSDK::StartPlayURL(IVS_LONG ulSessionID,const IVS_CHAR* pUrl,CUMW_DECODER_CHANNEL_INFO* pChannel)
{
    CHECK_POINTER(pUrl, IVS_PARA_INVALID);
    CHECK_POINTER(pChannel, IVS_PARA_INVALID);

    BP_DBG_LOG("StartPlayURL Start. pUrl = %s.", pUrl);

    NET_DVR_PU_STREAM_CFG_V41 stSteamInfo = {0};

    stSteamInfo.byStreamMode = 2;/*取流模式，0-无效，1-通过IP或域名取流，2-通过URL取流,3-通过动态域名解析向设备取流*/
    stSteamInfo.uDecStreamMode.struUrlInfo.byEnable = 1;//取流信息
    //stSteamInfo.uDecStreamMode.struUrlInfo.byTransPortocol = 0;
    stSteamInfo.uDecStreamMode.struUrlInfo.byChannel = (BYTE)pChannel->ulChannelID;
    stSteamInfo.dwSize = sizeof(NET_DVR_PU_STREAM_CFG_V41);
    LONG lRes = GetIPID(pChannel, stSteamInfo.uDecStreamMode.struUrlInfo.wIPID);
    if (VOS_OK != lRes)
    {
        BP_RUN_LOG_ERR(VOS_FAIL, "StartPlayURL fail, Can't Find IPID.", "");
        return VOS_FAIL;
    }

    if(!CToolsHelp::Memcpy(stSteamInfo.uDecStreamMode.struUrlInfo.strURL, URL_LEN, pUrl, strlen(pUrl)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "StartPlayURL fail, Buffer Too Small.", "");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //DWORD dwDecChanNum = 1 + ((1&0xff) << 16) + ((1&0xff) << 24); 
    //BP_DBG_LOG("StartPlayURL. dwDecChanNum = %d", (int)dwDecChanNum);

    BOOL bRes = NET_DVR_MatrixStartDynamic_V41((LONG)ulSessionID, (DWORD)pChannel->ulChannelID, &stSteamInfo);
    if(TRUE != bRes)
    {
        DWORD dRes = NET_DVR_GetLastError();
        BP_RUN_LOG_ERR((IVS_INT32)dRes, "StartPlayURL fail.", "");
        return VOS_FAIL;
    }
    if(!GetDecOnlineStatus(ulSessionID))
    {
        SetDecOnlineStatus(ulSessionID,true);
        NotifyDecResumeConn(ulSessionID);
    }
    SetChannelStatus(ulSessionID, pChannel->ulChannelID, true);   
    BP_DBG_LOG("StartPlayURL End.");
    return VOS_OK;
}


bool CHikDecoderSDK::GetDecOnlineStatus(IVS_LONG ulSessionID)
{
     (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find(ulSessionID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        return 1 == (it->second).bLoginSuc;
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
    return false;
}


IVS_INT32 CHikDecoderSDK::StopPlayURL(IVS_LONG ulSessionID,CUMW_DECODER_CHANNEL_INFO* pChannel)
{
    BP_DBG_LOG("StopPlayURL Start. ulSessionID = %d.", ulSessionID);
    IVS_LONG lUserID = 0;
    IVS_INT32 lRes = GetUserIDbyDecorderID(pChannel->szDecoderID, lUserID);
    if (VOS_OK != lRes)
    {
        BP_RUN_LOG_ERR(lRes, "StopPlayURL fail, Can't Find UserID.","");
        return VOS_FAIL;
    }

    BOOL bRes = NET_DVR_MatrixStopDynamic(lUserID, (DWORD)pChannel->ulChannelID); 
    if(TRUE != bRes)
    {
        BP_RUN_LOG_ERR(VOS_FAIL, "StopPlayURL fail.", "");
        return VOS_FAIL;
    }
    SetChannelStatus(ulSessionID, pChannel->ulChannelID, false);
    BP_DBG_LOG("StopPlayURL End.");

    return VOS_OK;
}


IVS_INT32 CHikDecoderSDK::GetUserIDbyDecorderID(const IVS_CHAR * pszDecoderID, IVS_LONG& lUserID)
{
    BP_DBG_LOG("GetUserIDbyDecorderID Start. pszDecoderID = %s.", pszDecoderID);
    CLockGuard autoLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.begin();
    for(; it != m_DecodeRegInfoMap.end(); it++)
    {
        BP_DBG_LOG("GetUserIDbyDecorderID: szDecoderID = %s.", (it->second).cDecoderID);

        if ( 0 == memcmp(pszDecoderID, (it->second).cDecoderID, CUMW_DECODER_ID_LEN))
        {
            lUserID = it->first;
            BP_DBG_LOG("GetUserIDbyDecorderID End. lUserID = %d.", lUserID);

            return VOS_OK;
        }
    }
    
    return VOS_FAIL;
}


LONG CHikDecoderSDK::GetIPID(const CUMW_DECODER_CHANNEL_INFO* pChannel, WORD& wIPID)
{
    //设备ID号，wIPID = iDevInfoIndex + iGroupNO*64 +1
    return 0;
}


//判断窗口解码状态是否正常
bool CHikDecoderSDK::IsWinDecodeStatusNatural(IVS_LONG lUserID, IVS_ULONG iChannelEx)
{
    NET_DVR_WALLWIN_INFO struWallWinInfo = {0};
    NET_DVR_WALL_WIN_STATUS struWallWinStatus = {0}; 
    struWallWinInfo.dwSize = sizeof(NET_DVR_WALLWIN_INFO);
    struWallWinInfo.dwWinNum = iChannelEx &0xffff;
    struWallWinInfo.dwSubWinNum = (iChannelEx>>16)&0xff; //lint !e702
    struWallWinInfo.dwWallNo = (iChannelEx>>24)&0xff; //lint !e702
    if (!NET_DVR_GetDeviceStatus(lUserID, NET_DVR_MATRIX_GETWINSTATUS, 0, &struWallWinInfo, sizeof(NET_DVR_WALLWIN_INFO), NULL, &struWallWinStatus, sizeof(struWallWinStatus)))
    {
        BP_RUN_LOG_ERR(VOS_FAIL, "IsWinDecodeStatusNatural：", "Get Device Status Failed.");
        return false;
    }

    if (0 == struWallWinStatus.byDecodeStatus)
    {
        BP_RUN_LOG_ERR(VOS_FAIL, "IsWinDecodeStatusNatural：", "Device Not Decord.");
        return false;
    }

    return true; 
}


void CHikDecoderSDK::SetChannelStatus(IVS_LONG ulSessionID, unsigned long ulChannelID, bool bDecording)
{
    BP_DBG_LOG("SetChannelStatus Start. ulSessionID = %d.", ulSessionID);
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find((LONG)ulSessionID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        IVS_INT32 iChannelNum = (it->second).iChannelNum;
        DECODER_ID_INFO_MAP::iterator idIter=m_DecoderInfoMap.find(it->second.cDecoderID);
        for (int i = 0; i < iChannelNum; i++)
        {
            if (ulChannelID == (it->second).stChannels[i].ulChannelID)
            {
                (it->second).stChannels[i].bDecording = bDecording;
                idIter->second.stChannels[i].bDecording=bDecording;
                (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
                return;
            }
        }
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
    BP_DBG_LOG("SetChannelStatus End.");
}


void CHikDecoderSDK::GetDecInfo2Map(DECODER_REG_INFO_MAP& TempMap)
{
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator ite = m_DecodeRegInfoMap.begin();
    for (; ite != m_DecodeRegInfoMap.end(); ite++)
    { 
        DECODER_DEV stDevInfo = {0};
        eSDK_MEMCPY((void*)&stDevInfo, sizeof(DECODER_DEV), (void*)&(ite->second), sizeof(DECODER_DEV));

        TempMap.insert(std::make_pair(ite->first, stDevInfo));//lint !e534
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
}


//检查异常状态
void CHikDecoderSDK::CheckChannelExp()
{
    BP_DBG_LOG("CheckChannelExp Start.");
    DECODER_REG_INFO_MAP TempMap;
    //复制一份解码器信息的map
    GetDecInfo2Map(TempMap);
    BP_DBG_LOG("CheckChannelExp: size = %d.", TempMap.size());
    DECODER_REG_INFO_MAP::iterator it = TempMap.begin();
    for (; it != TempMap.end(); it++)
    {
        DECODER_DEV stDevInfo = {0};
        LONG lUserId = it->first;
        eSDK_MEMCPY((void*)&stDevInfo, sizeof(DECODER_DEV), (void*)&(it->second), sizeof(DECODER_DEV));

        for (int i = 0; i < stDevInfo.iChannelNum; i++)
        {
            if (!stDevInfo.stChannels[i].bDecording)
            {
                continue;
            }

            if (!IsWinDecodeStatusNatural(lUserId, stDevInfo.stChannels[i].ulChannelID))
            {
                //通知需要进行重连
                NotifyChannelExp(stDevInfo, stDevInfo.stChannels[i].ulChannelID);
                stDevInfo.stChannels[i].bDecording = false;
            }
        }
    }
    BP_DBG_LOG("CheckChannelExp End.");
}


void CHikDecoderSDK::NotifyChannelExp(const DECODER_DEV& stDevInfo, unsigned long ulChannelID)
{
    BP_DBG_LOG("NotifyChannelExp Start.");
    CUMW_NOTIFY_INFO stNotifyInfo = {0};
    CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO_EX stErrInfo ={0};

    stErrInfo.stErrorCode.ulErrorCode = 0x02;
    stErrInfo.usChannelNo = ulChannelID;
    eSDK_MEMCPY(stErrInfo.szDecoderID, CUMW_DECODER_ID_LEN + 1, stDevInfo.cDecoderID, CUMW_DECODER_ID_LEN);

    stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_DECODER_ERROR;
    stNotifyInfo.ulNotifyInfoLen = sizeof(CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO_EX);
    stNotifyInfo.pNotifyInfo = (void*)&stErrInfo;
    if(NULL!=m_pFunCallBack)
    {
        (void)m_pFunCallBack(&stNotifyInfo, m_pUserData);
    }
    BP_DBG_LOG("NotifyChannelExp End.");
}


void CHikDecoderSDK::NotifyDecDisconn(IVS_LONG lUserID)
{
    CUMW_NOTIFY_INFO stNotifyInfo = {0};
    CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO stErrInfo ={0};

    std::string str;
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find(lUserID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        str = (it->second).cDecoderID;
    }
    else
    {
        (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
        BP_RUN_LOG_ERR(VOS_FAIL, "NotifyDecDisconn：", "Canit Find Decorder.");
        return;
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);

    eSDK_MEMCPY(stErrInfo.szDecoderID, CUMW_DECODER_ID_LEN + 1, str.c_str(), CUMW_DECODER_ID_LEN);

    stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT;
    stNotifyInfo.ulNotifyInfoLen = sizeof(CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO);
    stNotifyInfo.pNotifyInfo = (void*)&stErrInfo;
    if(NULL!=m_pFunCallBack)
    {
        (void)m_pFunCallBack(&stNotifyInfo, m_pUserData);
    }
}


void CHikDecoderSDK::NotifyDecResumeConn(IVS_LONG lUserID)
{
    CUMW_NOTIFY_INFO stNotifyInfo = {0};
    CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO stErrInfo ={0};
    std::string str;
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find(lUserID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        str = (it->second).cDecoderID;
    }
    else
    {
        (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
        BP_RUN_LOG_ERR(VOS_FAIL, "NotifyDecResumeConn：", "Cannot Find Decorder.");
        return;
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);

    eSDK_MEMCPY(stErrInfo.szDecoderID, CUMW_DECODER_ID_LEN + 1, str.c_str(), CUMW_DECODER_ID_LEN);

    stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_RESUMECONNECT;
    stNotifyInfo.ulNotifyInfoLen = sizeof(CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO);
    stNotifyInfo.pNotifyInfo = (void*)&stErrInfo;
    if(NULL!=m_pFunCallBack)
    {
        (void)m_pFunCallBack(&stNotifyInfo, m_pUserData);
    }
}


void CHikDecoderSDK::SetDecOnlineStatus(IVS_LONG lUserID, bool bLoginSuc)
{
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find(lUserID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        (it->second).bLoginSuc = bLoginSuc;
        DECODER_ID_INFO_MAP::iterator idIter=m_DecoderInfoMap.find(it->second.cDecoderID);
        if(m_DecoderInfoMap.end()!=idIter)
        {
            idIter->second.bLoginSuc = bLoginSuc;
        }
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
}


bool CHikDecoderSDK::IsHikDecOnline(const char* pszDecoderID)
{
    CHECK_POINTER(pszDecoderID, false);
    BP_DBG_LOG("IsHikDecOnline: pszDecoderID = %s.", pszDecoderID);
    CLockGuard autoLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.begin();
    for(; it != m_DecodeRegInfoMap.end(); it++)
    {
        BP_DBG_LOG("IsHikDecOnline: szDecoderID = %s.", (it->second).cDecoderID);

        if ( 0 == memcmp(pszDecoderID, (it->second).cDecoderID, CUMW_DECODER_ID_LEN))
        {
            if ((it->second).bLoginSuc)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}


void CHikDecoderSDK::SaveDecInfo(IVS_LONG lUserID, DECODER_DEV& stDecoder)
{
    (void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
    DECODER_REG_INFO_MAP::iterator it = m_DecodeRegInfoMap.find(lUserID);
    if(m_DecodeRegInfoMap.end() != it)
    {
        (it->second).iChannelNum = stDecoder.iChannelNum;
        eSDK_MEMCPY((void*)&((it->second).stChannels[0]),sizeof(CUMW_DECODER_CHANNEL_INFO) * DECODER_MAX_CHANNEL_NUM,
            (void*)&(stDecoder.stChannels[0]), sizeof(CUMW_DECODER_CHANNEL_INFO) * DECODER_MAX_CHANNEL_NUM);
        DECODER_ID_INFO_MAP::iterator idIter= m_DecoderInfoMap.find(it->second.cDecoderID); 
        if(m_DecoderInfoMap.end()!=idIter)
        {
            idIter->second.iChannelNum=stDecoder.iChannelNum;
            eSDK_MEMCPY((void*)&((idIter->second).stChannels[0]),sizeof(CUMW_DECODER_CHANNEL_INFO) * DECODER_MAX_CHANNEL_NUM,
            (void*)&(stDecoder.stChannels[0]), sizeof(CUMW_DECODER_CHANNEL_INFO) * DECODER_MAX_CHANNEL_NUM);
            idIter->second.bLoginSuc = 1;
        }
        BP_DBG_LOG("SaveDecInfo: save szDecoderID = %s suc.", stDecoder.cDecoderID);
    }
    (void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);

}


void __cdecl CHikDecoderSDK::CheckDecThread(LPVOID lpvoid)
{
    CHECK_POINTER_VOID(lpvoid);
    CHikDecoderSDK* pHikDecoderSDK = (CHikDecoderSDK*)lpvoid;
    do 
    {
        Sleep(10 * 1000); 
        pHikDecoderSDK->CheckChannelExp();
    } while (pHikDecoderSDK->m_CheckDecThreadRun);
}


void CHikDecoderSDK::StartCheckDecThread()
{
    long lResult = (long)VOS_CreateThread((VOS_THREAD_FUNC)CheckDecThread, (void*)this, &m_pCheckDecThread, VOS_DEFAULT_STACK_SIZE); //lint !e1924
    if (IVS_SUCCEED != lResult)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "StartCheckDecThread", "VOS_CreateThread failed");
    }
}


IVS_INT32 CHikDecoderSDK::CheckDecoderChannel(const IVS_CHAR* pDecoderID,IVS_ULONG ulChannel)
{
    DECODER_REG_INFO_MAP::iterator it=m_DecodeRegInfoMap.begin();
    for(;m_DecodeRegInfoMap.end()!=it;it++)
    {
        if(std::string(it->second.cDecoderID)==std::string(pDecoderID))
        {
            break;
        }
    }
    if(m_DecodeRegInfoMap.end()!=it)
    {
        const DECODER_DEV* pDecoder = &it->second;
        for (int i = 0; i < DECODER_MAX_CHANNEL_NUM; i++)
        {
            if(pDecoder->stChannels[i].ulChannelID==ulChannel)
            {
                return IVS_DECODER_CHANNEL_FOUND;
            }
        }
        BP_RUN_LOG_INF("CheckDecoderChannel","Channel not find,%s,%d",pDecoderID,ulChannel);
        return IVS_DECODER_CHANNEL_NOT_FOUND;
    }else
    { 
        BP_RUN_LOG_INF("CheckDecoderChannel","Decoder not find,%s",pDecoderID);
        return IVS_DECODER_NOT_FOUND;
    }
}


IVS_INT32 CHikDecoderSDK::CreateHikDecoderID(std::string& strDecoderID)
{
	IVS_LONG idValue=100000;
	std::string tmpDecoderID("00000000000000");
	tmpDecoderID.append(CToolsHelp::Long2Str(idValue));

	CLockGuard autoLock(m_pDecodeRegInfoMapMutex);
	DECODER_ID_INFO_MAP::iterator BeginIter=m_DecoderInfoMap.begin();
	DECODER_ID_INFO_MAP::iterator EndIter=m_DecoderInfoMap.end();
	for(; BeginIter != EndIter; BeginIter++)
	{
		DECODER_ID_INFO_MAP::iterator tmpIter = m_DecoderInfoMap.find(tmpDecoderID);
		if(tmpIter != EndIter)
		{
			idValue++;
			tmpDecoderID.clear();
			tmpDecoderID.append("00000000000000");
			tmpDecoderID.append(CToolsHelp::Long2Str(idValue));
			BeginIter=m_DecoderInfoMap.begin();
			continue;
		}
		break;
	}

	if(CUMW_DECODER_ID_LEN < tmpDecoderID.size())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "CreateHikDecoderID", "decoderid memory overflow.");
		return IVS_PARA_INVALID;
	}
	strDecoderID = tmpDecoderID;
	BP_RUN_LOG_INF("CreateHikDecoderID","get Hik decoderid: %s", strDecoderID.c_str());
	return IVS_SUCCEED;
}

IVS_INT32 CHikDecoderSDK::AddDecoderInfo(IVS_LONG ulSessionID, const IVS_CHAR* pDecoderID, DECODER_DEV& stDecoder)
{
	CHECK_POINTER(pDecoderID, IVS_PARA_INVALID);

	// do not save passwd for safe
	if(!CToolsHelp::Memcpy(stDecoder.cDecoderID, CUMW_DECODER_ID_LEN+1, pDecoderID, CUMW_DECODER_ID_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Cpy DecoderID Fail, Buffer Too Small.", "");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	IVS_LONG oldUserId = 0;
	if(IVS_SUCCEED == GetUserIDbyDecorderID(stDecoder.cDecoderID,oldUserId))
	{
		(void)Logout(oldUserId);
	}
	
	(void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
	m_DecodeRegInfoMap.insert(std::make_pair(ulSessionID, stDecoder));
	m_DecoderInfoMap.insert(std::make_pair(stDecoder.cDecoderID, stDecoder));
	(void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
	
	return IVS_SUCCEED;
}


void CHikDecoderSDK::DecoderRelease()
{
	(void)VOS_MutexLock(m_pDecodeRegInfoMapMutex);
	DECODER_REG_INFO_MAP::iterator Beniter = m_DecodeRegInfoMap.begin();
	DECODER_REG_INFO_MAP::iterator Enditer = m_DecodeRegInfoMap.end();
	IVS_LONG lSessionID = SDK_SESSIONID_INIT;
	IVS_INT32 iRet = IVS_FAIL;
	std::string stDecoderID("");
	while (Beniter != Enditer)
	{
		lSessionID = (IVS_LONG)Beniter->first;
		stDecoderID = Beniter->second.cDecoderID;

		m_DecoderInfoMap.erase(Beniter->second.cDecoderID);
		m_DecodeRegInfoMap.erase(Beniter++);
		if (!NET_DVR_Logout((LONG)lSessionID))
		{
			IVS_INT32 iRet = (IVS_INT32)NET_DVR_GetLastError();
			BP_RUN_LOG_ERR(iRet, "Logout", " Logout fail, SessionID = %d, Decoderid: %s .", lSessionID, stDecoderID.c_str());
		}
	}

	(void)VOS_MutexUnlock(m_pDecodeRegInfoMapMutex);
	return;
}


CHikTrigger::CHikTrigger(void) : m_pHikDecoderSDK(NULL)
{//lint !e1928
}

CHikTrigger::~CHikTrigger(void)
{
    m_pHikDecoderSDK = NULL;
}


void CHikTrigger::SetHikDecoderSDK(CHikDecoderSDK* pHikDecoderSDK)
{
    m_pHikDecoderSDK = pHikDecoderSDK;
}


void CHikTrigger::onTrigger(void * /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*enStyle*/)
{
    CHECK_POINTER_VOID(m_pHikDecoderSDK);

    m_pHikDecoderSDK->CheckChannelExp();
}





