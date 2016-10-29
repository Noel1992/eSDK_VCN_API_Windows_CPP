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

#include "EventMgr.h"
#include "ivs_error.h"
#include "_BaseThread.h"
#include "IVSCommon.h"
#include "EventCallBackJob.h"
#include "SDKInterface.h"
#include "RealPlay.h"
#include "AutoResume.h"
#include "IVS_SDK.h"
#include "IVS_Trace.h"
#include "ToolHelp.h"
#include "IAMessageMgr.h"
#include "TPPlayChannel.h"
//lint -e526
extern CSDKInterface *g_pNvs ;//lint !e526

//add by c00206592 定义接收的水印串改告警ID
//key:摄像机编码  value:告警ID
typedef std::map<std::string, std::string> WATER_ALARM_MAP;
typedef WATER_ALARM_MAP::iterator WATER_ALARM_MAP_ITER;
//告警开始时间MAP：  key:告警ID,Value:开始时间
typedef std::map<std::string ,std::string >ALARM_STARTIME_MAP;
typedef ALARM_STARTIME_MAP::iterator ALARM_STARTIME_MAP_ITER;

static  WATER_ALARM_MAP m_WaterAlarmEventIdMap;
static  ALARM_STARTIME_MAP m_AlarmTimeMap;

#define MAX_WATER_ALARM_MAP_LEN 128
//end add by c00206592 

static void GetWaterAlarmXml(const std::string& strCameraCode,const std::string& strUserIdDomainCode,
	const std::string& strDomianCode,const std::string& strStartTime,std::string& strReq)
{
	IVS_DEBUG_TRACE("");
	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("UserIDDomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strUserIdDomainCode.c_str());//lint !e64 匹配
	xml.OutOfElem();

	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strCameraCode.c_str());//lint !e64 匹配
	(void)xml.AddElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strDomianCode.c_str());//lint !e64 匹配
	(void)xml.AddElem("AlarmTypeID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolHelp::IntToStr(0).c_str());//lint !e64 匹配
	(void)xml.AddElem("OccurStartTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strStartTime.c_str());//lint !e64 匹配
	xml.OutOfElem();
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);

    if (NULL != pReq)
    {
        strReq = pReq;
    }

}

static void PraseResponseData(const std::string& strCameraCode, const std::string& strStartTime, 
                              const std::string& strpRsp, std::string& strWaterAlarmId)
{
	//解析数据
	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());//lint !e64 匹配
	IVS_DEBUG_TRACE("");

	if (!xmlRsp.FindElemEx("Content/AlarmEventID"))
	{
		return;
	}
	const char* AttriValue = xmlRsp.GetElemValue(); //临时存储单个节点值
	if (NULL == AttriValue)
	{
		return;
	}
	strWaterAlarmId = AttriValue;

	if (MAX_WATER_ALARM_MAP_LEN <= m_WaterAlarmEventIdMap.size())
	{
		WATER_ALARM_MAP_ITER  firstIter = m_WaterAlarmEventIdMap.begin();
		m_WaterAlarmEventIdMap.erase(firstIter);
	}
	if (MAX_WATER_ALARM_MAP_LEN <= m_AlarmTimeMap.size())
	{
		ALARM_STARTIME_MAP_ITER  firstIter = m_AlarmTimeMap.begin();
		m_AlarmTimeMap.erase(firstIter);
	}

    WATER_ALARM_MAP_ITER waterAlarmMapIter = m_WaterAlarmEventIdMap.find(strCameraCode);
    if (waterAlarmMapIter != m_WaterAlarmEventIdMap.end())
    {
        m_WaterAlarmEventIdMap.erase(waterAlarmMapIter);
    }

	m_WaterAlarmEventIdMap.insert(make_pair(strCameraCode, strWaterAlarmId));//lint !e534

    ALARM_STARTIME_MAP_ITER alarmTimeIter = m_AlarmTimeMap.find(strWaterAlarmId);
    if (alarmTimeIter != m_AlarmTimeMap.end())
    {
        m_AlarmTimeMap.erase(alarmTimeIter);
    }

	m_AlarmTimeMap.insert(make_pair(strWaterAlarmId, strStartTime));//lint !e534

}
static int GetUpdateWaterAlarmXml(const std::string& strCameraCode,const std::string& strUserIdDomainCode,const std::string& strDomianCode,const std::string& strStopTime,
									std::string& strReq,std::string& strStartTime,std::string& strAlarmEventID)
{
	IVS_DEBUG_TRACE("");
	WATER_ALARM_MAP_ITER waterAlarmMapIter = m_WaterAlarmEventIdMap.find(strCameraCode);
	WATER_ALARM_MAP_ITER waterAlarmMapIterEnd = m_WaterAlarmEventIdMap.end();
	if (waterAlarmMapIter == waterAlarmMapIterEnd)
	{
		return IVS_FAIL;
	}
	ALARM_STARTIME_MAP_ITER alarmTimeIter = m_AlarmTimeMap.find(waterAlarmMapIter->second);
	ALARM_STARTIME_MAP_ITER alarmTimeIterEnd = m_AlarmTimeMap.end();
	if (alarmTimeIter != alarmTimeIterEnd)
	{
		strStartTime = alarmTimeIter->second;
		m_AlarmTimeMap.erase(alarmTimeIter);
	}
	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("UserIDDomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strUserIdDomainCode.c_str());//lint !e64 匹配
	xml.OutOfElem();

	(void)xml.AddElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strDomianCode.c_str());//lint !e64 匹配
	(void)xml.AddElem("AlarmEventID");
	(void)xml.IntoElem();
	strAlarmEventID = waterAlarmMapIter->second;
	(void)xml.SetElemValue(strAlarmEventID.c_str());//lint !e64 匹配
	(void)xml.AddElem("OccurStopTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strStopTime.c_str());//lint !e64 匹配
	xml.OutOfElem();
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
    if (NULL != pReq)
    {
        strReq = pReq;
    }
	
	m_WaterAlarmEventIdMap.erase(waterAlarmMapIter);
	return IVS_SUCCEED;
}

//事件回调函数
void __SDK_CALL EventCallBackFun(int iEventType, void *pEventBuf, int iBufSize, void *pUserData)
{
    int iSessionID = (int)pUserData;
    (void)CEventCallBackJob::instance().PostEvent(iSessionID, iEventType, pEventBuf, iBufSize);
} //lint !e818


// rtsp_client异常回调;
static void __SDK_CALL RtspExceptionCallBack(IVS_ULONG          handle,
                                             unsigned int ulMsgType,
                                             unsigned int ulErrCode,           // 回调函数错误码，详见_enRtspClientRetCode
                                             void *       pParameter,
                                             void *       pUser)    // 注意这个可以为NULL
{
	BP_RUN_LOG_INF("RtspExceptionCallBack", "rtsp handle[0x%p] MsgType[%u] ErrCode[%u]", handle, ulMsgType, ulErrCode);

	CHECK_POINTER_VOID(g_pNvs);
	int iSessionID = reinterpret_cast<int>(pUser);
	IVS_ULONG ulPlayHandle = 0;
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	if (NULL == pUserMgr)
	{
        BP_RUN_LOG_WAR("RtspExceptionCallBack", "Can not get usermgr by sessionid:%d", iSessionID);
	}
	else
	{
		ulPlayHandle = pUserMgr->GetPlayHandlebyRtspHandle(handle);
	}
	//add by wangguangyan start
	if (0 == ulPlayHandle)
	{
		pUserMgr = g_pNvs->GetUserMgrbyRtspHandle(handle);
		if (NULL == pUserMgr)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID,"RtspExceptionCallBack", "Can not get usermgr by handle:0x%p", handle);
			return;
		}
		else
		{
			ulPlayHandle = pUserMgr->GetPlayHandlebyRtspHandle(handle);
			iSessionID = pUserMgr->GetSessionID();
		}
	}
	//add by wangguangyan end
	if (0 == ulPlayHandle)
	{
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "RtspExceptionCallBack", "Get playhandle by rtsphandle failed, rtsp handle:0x%p", handle);
		return;
	}


	STREAM_EXCEPTION_INFO exceptionInfo = {0};
    exceptionInfo.ulHandle = ulPlayHandle;
    exceptionInfo.iSessionID = iSessionID;
    exceptionInfo.iExceptionCode = 0;
	IVS_INT32 iRet = IVS_FAIL;
    // 处理MU正常、异常停止流的情况;
    if (RTSP_METHOD_ANNOUNCE == ulMsgType)
    {
		//获取到了SDP信息
		if (IVS_SUCCEED == ulErrCode && NULL != pParameter)
		{
			//进行密钥协商过程，失败则提示密钥协商失败;
			CHECK_POINTER_VOID(pUserMgr);
			iRet = pUserMgr->SetPlaySecretKeybyRtspHandle(handle,(char*)pParameter);
			if (IVS_SUCCEED == iRet)
			{
				BP_DBG_LOG("Set Play Secret Key by Rtsp Handle succeed, rtsp handle:0x%p", handle);
				return;
			}
			if(IVS_SDK_DOWNLOAD_UNSUPPORTED_VIDEODECTYPE == iRet)
			{
				exceptionInfo.iExceptionCode = iRet;
			}else
			{
				BP_DBG_LOG("Set Play Secret Key by Rtsp Handle failed, rtsp handle:0x%p", handle);
				return;
			}
            //exceptionInfo.iExceptionCode = IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
		}
        // 正常结束，只需回调
        if(RTSP_ERR_NORMAL_STOP == ulErrCode)
        {
            exceptionInfo.iExceptionCode = IVS_PLAYER_RET_RTSP_NORMAL_STOP;
        }
        // 异常结束
        else if(RTSP_ERR_ABNORMAL_STOP == ulErrCode)
        {
            exceptionInfo.iExceptionCode = IVS_PLAYER_RET_RTSP_ABNORMAL_STOP;
        }
    }
	// 异常断开连接
	if (RTSP_ERR_DISCONNECT == ulErrCode)
	{
		//IVS_LOG(LOG_INFO, "IVS handle Rtsp connect abnormal begin");
		exceptionInfo.iExceptionCode = IVS_PLAYER_RET_RTSP_DISCONNECT;
	}
	else if (RTSP_ERR_TIMEOUT == ulErrCode)
	{
		exceptionInfo.iExceptionCode = IVS_PLAYER_RET_RECV_DATA_TIMEOUT;
	}
	
    // Rtsp事件类型;
	ulMsgType = IVS_EVENT_RTSP_MSG;

	if (RTSP_ERR_TIMEOUT != ulErrCode)
	{
		CHECK_POINTER_VOID(pUserMgr);
		pUserMgr->StopNotifyBufEvent(ulPlayHandle);
	}

	// 根据消息类型和错误码，将pParameter转换为对象，再sizeof获得大小;
	int iBufSize = sizeof(STREAM_EXCEPTION_INFO);
	exceptionInfo.iReseved1 = handle;//lint !e713
	BP_RUN_LOG_INF("RtspExceptionCallBack", "Rtsp Error");

	EventCallBackFun(static_cast<IVS_INT32>(ulMsgType), &exceptionInfo, iBufSize, (void *)iSessionID);
}//lint !e818 原有库的函数定义，不修改

// net_source异常回调;
static void __SDK_CALL NetSourceExceptionCallBack(IVS_ULONG ulChannel, int iMsgType, void*  /*pParam*/, void* pUser)
{
	BP_DBG_LOG("NetSource ExceptionCallBack Channel[%lu] MsgType[%d]", ulChannel, iMsgType);
	CHECK_POINTER_VOID(g_pNvs);
	//CHECK_POINTER_VOID(pUser); pUser是iSessionID的值，不是地址，不能当指针检查;
    int iSessionID = reinterpret_cast <int>(pUser);

    // 根据消息类型和错误码，将pParameter转换为对象，再sizeof获得大小;
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	if (NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "NetSourc ExceptionCallBack", "Can not get usermgr by sessionid:%d", iSessionID);
		return;
	}

	IVS_ULONG playHandle = (IVS_ULONG)pUserMgr->GetPlayHandlebyNet(ulChannel);
	if (0 == playHandle)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "NetSourc ExceptionCallBack", "Get playhandle by rtsphandle failed, rtsp handle:%d", playHandle);
		return;
	}

	if (IVS_PLAYER_RET_RECV_DATA_TIMEOUT != iMsgType)
	{
		pUserMgr->StopNotifyBufEvent(playHandle);
	}

	STREAM_EXCEPTION_INFO exceptionInfo = {0};

	exceptionInfo.ulHandle = playHandle;
	exceptionInfo.iSessionID = iSessionID;
	//在NetSource中返回的错误码信息就为iMsgType
	exceptionInfo.iExceptionCode = iMsgType;
	exceptionInfo.iReseved1 = (IVS_INT32)ulChannel;

	// 根据消息类型和错误码，将pParameter转换为对象，再sizeof获得大小;
	int iBufSize = sizeof(STREAM_EXCEPTION_INFO);
	BP_DBG_LOG("NetSourceExceptionCallBack NetSource Error");
    EventCallBackFun(IVS_EVENT_NETSOURCE_MSG, &exceptionInfo, iBufSize, pUser);
}

// ivs_player播放库异常回调;
static void __SDK_CALL PlayerExceptionCallBack(IVS_ULONG ulChannel, int iMsgType, void*  pParam, void*  pUser)
{
	//播放库返回的错误码信息已经封装成STREAM_EXCEPTION_INFO数据结构返回，其中包含了错误码信息
	
    int iSessionID = reinterpret_cast <int>(pUser);
	BP_DBG_LOG("Player ExceptionCallBack Channel[%lu] MsgType[%d] SessionID[%d]", ulChannel, iMsgType, iSessionID);
    // 根据消息类型和错误码，将pParameter转换为对象，再sizeof获得大小;
    //int iBufSize = sizeof(pParam);

    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	if (NULL == pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "RtspExceptionCallBack", "Can not get usermgr by sessionid:%d", iSessionID);
		return;
	}

	IVS_ULONG playHandle = pUserMgr->GetPlayHandlebyPlayChannel(ulChannel);
	if ((0 == playHandle) && (SERVICE_TYPE_AUDIO_BROADCAST != iMsgType))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "RtspExceptionCallBack", "Get playhandle by rtsphandle failed, rtsp handle:  0x%p", playHandle);
		return;
	}
	
	switch (iMsgType)
	{
	case IVS_EVENT_LOCAL_RECORD_SUCCESS:
	case IVS_EVENT_DOWNLOAD_SUCCESS:
		{
			char Buf[sizeof(IVS_LOCAL_RECORD_SUCCESS_INFO) + sizeof(IVS_ULONG)];
			IVS_LOCAL_RECORD_SUCCESS_INFO *recordInfo = (IVS_LOCAL_RECORD_SUCCESS_INFO *)(Buf);
			IVS_ULONG *pHandle = (IVS_ULONG *)(recordInfo + 1);//lint !e826
			if (!CToolsHelp::Memcpy(recordInfo, sizeof(IVS_LOCAL_RECORD_SUCCESS_INFO), pParam, sizeof(IVS_LOCAL_RECORD_SUCCESS_INFO)))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
				return;
			}
			*pHandle = playHandle;
			EventCallBackFun(iMsgType, Buf, sizeof(Buf), (void*)iSessionID);
		}
		break;

	case IVS_EVENT_LOCAL_PLAYBACK_FAILED:
		{	
			STREAM_EXCEPTION_INFO  info = {0};
			info.iExceptionCode = 0;
			info.iSessionID     = iSessionID;
			info.ulHandle       = playHandle;
			info.iReseved1      = 0;
			EventCallBackFun(iMsgType, &info, sizeof(info), (void*)iSessionID);
		}
		break;

		//水印篡改告警
	case IVS_EVENT_PLAYER_WATER_MARK_EXCEPTION:
		{
			WATER_MARK_EXCEPTION_DATA* pExceptionData;
			pExceptionData = (WATER_MARK_EXCEPTION_DATA*)pParam;
			CMediaBase * pMediaBase = (CMediaBase *)playHandle;
			if (NULL == pMediaBase)
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"ivs player ret water mark error", "MediaBase is null.");
				return ;
			}
			std::string strCameraID;
			pMediaBase->GetCameraID(strCameraID);
			std::string strUserDomainCode; //用户所在DomainCode
			//分解获取CameraCode和DomainCode
			std::string strCameraCode;
			std::string strDomainCode;
			IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strCameraID, strCameraCode, strDomainCode);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet,"DO Player Exception", "SplitCameraCode failed, RetCode=%d.", iRet);
				return ;
			}
			//获取用户本域编码
			pUserMgr->GetDomainCode(strUserDomainCode);
			//生成XML告诉SMU入库数据
			CSendNssMsgInfo sendNssMsgInfo;	
			sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
			sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
			sendNssMsgInfo.SetCameraCode(strCameraCode);
			sendNssMsgInfo.SetDomainCode(strDomainCode);
			std::string strReq;
			//时间格式为毫秒级别，除以1000转化为秒
			time_t stTime_t = pExceptionData->ullTimeStamp / 1000; //lint !e713
			std::string strTime;
			//获取格式化的string Time
			CMediaBase::UTCTime2Str(stTime_t, strTime, IVS_DATE_TIME_FORMAT);
			//返回XML
			std::string strpRsp;
			IVS_INT32 iNeedRedirect = IVS_FAIL;
			IVS_ALARM_WATERMARK_INFO stWaterAlarmInfo;
			eSDK_MEMSET(&stWaterAlarmInfo,0,sizeof(IVS_ALARM_WATERMARK_INFO));
			eSDK_MEMCPY(stWaterAlarmInfo.cCameraCode, IVS_DEV_CODE_LEN, strCameraID.c_str(),IVS_DEV_CODE_LEN);
			//异常回调数据，尾部指针的数据保存当前播放的handle
			char Buf[sizeof(IVS_ALARM_WATERMARK_INFO) + sizeof(IVS_ULONG)];
			IVS_ALARM_WATERMARK_INFO *recordInfo = (IVS_ALARM_WATERMARK_INFO *)(Buf);
			IVS_ULONG *pHandle = (IVS_ULONG *)(recordInfo + 1);//lint !e826
			*pHandle = playHandle;

            std::string strWaterAlarmId("");

			//发生水印校验异常
			if (IVS_PLAYER_RET_WATER_MARK_ERROR == pExceptionData->ulExceptionCode)
			{
				eSDK_MEMCPY(stWaterAlarmInfo.cStartTime, IVS_TIME_LEN, strTime.c_str(), strTime.size());
				sendNssMsgInfo.SetReqID(NSS_WATERMARK_ALARM_NOTIFY_REQ);

				GetWaterAlarmXml(strCameraCode, strUserDomainCode, strDomainCode, strTime, strReq);
				sendNssMsgInfo.SetReqData(strReq);

                if (0 == strReq.length())
                {
                    BP_DBG_LOG("", "NSS_WATERMARK_ALARM_NOTIFY_REQ xml is null, no need send msg to smu.");
                    return;
                }

				//发送操作失败，直接返回
				iRet = pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
				if (IVS_SUCCEED == iRet)
				{   
                    //解析返回数据值
					PraseResponseData(strCameraCode, strTime, strpRsp, strWaterAlarmId);
					if (!CToolsHelp::Memcpy(recordInfo, sizeof(IVS_ALARM_WATERMARK_INFO), &stWaterAlarmInfo, sizeof(IVS_ALARM_WATERMARK_INFO)))
					{
						BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
						return;
					}
					//发现水印篡改提示
					//OnEventCallBack(iSessionID, IVS_EVENT_REPORT_WATERMARK_TIP, &stWaterAlarmInfo, sizeof(IVS_ALARM_WATERMARK_INFO));
					EventCallBackFun(IVS_EVENT_REPORT_WATERMARK_TIP, Buf, sizeof(Buf), (void*)iSessionID);

                    stWaterAlarmInfo.ullAlarmEventID = static_cast<IVS_UINT64>(CToolHelp::StrToInt(strWaterAlarmId.c_str()));//lint !e571
                    BP_DBG_LOG("PlayerExceptionCallBack, ullAlarmEventID:%d, code:%d", stWaterAlarmInfo.ullAlarmEventID, IVS_PLAYER_RET_WATER_MARK_ERROR);

                    //保存水印数据信息
                    ST_WATERMARK_ALARM_INFO stWaterMarkAlarmInfo;
                    eSDK_MEMSET(&stWaterMarkAlarmInfo, 0, sizeof(stWaterMarkAlarmInfo));
                    eSDK_MEMCPY(&stWaterMarkAlarmInfo.stWaterMarkInfo, sizeof(stWaterMarkAlarmInfo.stWaterMarkInfo), &stWaterAlarmInfo, sizeof(stWaterMarkAlarmInfo.stWaterMarkInfo));
                    stWaterMarkAlarmInfo.bIsNeedReport = true;
                    stWaterMarkAlarmInfo.bIsOpenedWater = true;

                    pUserMgr->SetWaterMarkInfo(playHandle, stWaterMarkAlarmInfo);
                    BP_DBG_LOG("PlayerExceptionCallBack, cCameraCode:%s, cStartTime:%s, ullAlarmEventID:%d",
                               stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, 
                               stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID);
				}
                else
                {
                    //发送NSS_WATERMARK_ALARM_NOTIFY_REQ消息给SMU失败
                    BP_RUN_LOG_ERR(iRet, "", "Send NSS_WATERMARK_ALARM_NOTIFY_REQ(0x0C31) Msg To SMU Fail. ErrCode:%d.", iRet);
                }
			}
			//水印告警恢复
			if(IVS_PLAYER_RET_WATER_MARK_OK == pExceptionData->ulExceptionCode)
			{
				std::string strStartTime;
				std::string strAlarmEventID;
				sendNssMsgInfo.SetReqID(NSS_WATERMARK_ALARM_UPDATE_REQ);
				(void)GetUpdateWaterAlarmXml(strCameraCode,strUserDomainCode,strDomainCode,strTime,strReq,strStartTime,strAlarmEventID);
                
                if (0 == strReq.length())
                {
                    BP_DBG_LOG("", "NSS_WATERMARK_ALARM_UPDATE_REQ xml is null, no need send msg to smu.");
                    return;
                }

				eSDK_MEMCPY(stWaterAlarmInfo.cStartTime, IVS_TIME_LEN, strStartTime.c_str(),strStartTime.size());  //开始时间
				eSDK_MEMCPY(stWaterAlarmInfo.cEndTime, IVS_TIME_LEN, strTime.c_str(),strTime.size());				//结束时间
				stWaterAlarmInfo.ullAlarmEventID =(unsigned long long) CToolsHelp::StrToInt(strAlarmEventID);//lint !e571
				sendNssMsgInfo.SetReqData(strReq);
				iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
				//返回通用相应，不需要解析
				//只有发生水印告警恢复的时候才向CU上抛事件
				if (IVS_SUCCEED == iRet)
				{
					if (!CToolsHelp::Memcpy(recordInfo, sizeof(IVS_ALARM_WATERMARK_INFO), &stWaterAlarmInfo, sizeof(IVS_ALARM_WATERMARK_INFO)))
					{
						BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
						return;
					}
					// 水印告警记录产生通知（有篡改开始时间和停止时间）
					//OnEventCallBack(iSessionID, IVS_EVENT_REPORT_WATERMARK_ALARM,&stWaterAlarmInfo, sizeof(IVS_ALARM_WATERMARK_INFO));
					EventCallBackFun(IVS_EVENT_REPORT_WATERMARK_ALARM, Buf, sizeof(Buf), (void*)iSessionID);
                    
                    //保存水印数据信息
                    ST_WATERMARK_ALARM_INFO stWaterMarkAlarmInfo;
                    eSDK_MEMSET(&stWaterMarkAlarmInfo, 0, sizeof(stWaterMarkAlarmInfo));
                    eSDK_MEMCPY(&stWaterMarkAlarmInfo.stWaterMarkInfo, sizeof(stWaterMarkAlarmInfo.stWaterMarkInfo), &stWaterAlarmInfo, sizeof(stWaterMarkAlarmInfo.stWaterMarkInfo));
                    stWaterMarkAlarmInfo.bIsNeedReport = false;
                    stWaterMarkAlarmInfo.bIsOpenedWater = true;

                    pUserMgr->SetWaterMarkInfo(playHandle, stWaterMarkAlarmInfo);
                    //pUserMgr->SetWaterExceptionFlag(playHandle, true);
                    BP_DBG_LOG("WaterMark PlayerExceptionCallBack, cCameraCode:%s, cStartTime:%s, cEndTime:%s, ullAlarmEventID:%d",
                                stWaterMarkAlarmInfo.stWaterMarkInfo.cCameraCode, stWaterMarkAlarmInfo.stWaterMarkInfo.cStartTime, 
                                stWaterMarkAlarmInfo.stWaterMarkInfo.cEndTime, stWaterMarkAlarmInfo.stWaterMarkInfo.ullAlarmEventID);
                }
                else
                {
                    //发送NSS_WATERMARK_ALARM_UPDATE_REQ消息给SMU失败
                    BP_RUN_LOG_ERR(iRet, "", "Send NSS_WATERMARK_ALARM_UPDATE_REQ(0x0C32) Msg To SMU Fail. ErrCode:%d.", iRet);
                }
			}
		}
		break;
        //本地文件广播结束，上报给OCX，由OCX关闭文件广播
    case SERVICE_TYPE_AUDIO_BROADCAST:   
		STREAM_EXCEPTION_INFO  info;
		eSDK_MEMSET(&info,  0, sizeof(info));
        info.iExceptionCode = IVS_PLAYER_RET_BROADCAST_FILE_IS_END;
        info.iSessionID     = iSessionID;
        info.ulHandle       = playHandle;
        info.iReseved1      = 0;
        EventCallBackFun(IVS_EVENT_FILE_BROADCAST_END, &info, sizeof(info), (void*)iSessionID);
        break;
	case IVS_EVENT_LOCAL_RECORD_EGENERIC:
	case IVS_EVENT_DOWNLOAD_FAILED:
	default:
		char Buf[sizeof(unsigned int) + sizeof(IVS_ULONG)];
		IVS_ULONG *pCode   = (IVS_ULONG *)(Buf);
		IVS_ULONG *pHandle = (IVS_ULONG *)(pCode + 1);//lint !e826

		*pCode   = (IVS_ULONG)pParam;
		*pHandle = (IVS_ULONG)playHandle;
		EventCallBackFun(iMsgType, Buf, sizeof(Buf), (void*)iSessionID);
		break;
	}
	

    //EventCallBackFun(iMsgType, Buf, sizeof(Buf), (void*)iSessionID);
}

CEventMgr::CEventMgr(void)
{
    for (int i = 0; i < SDK_CLIENT_TOTAL + 1; i++)
    {
        m_EventUserData[i] = NULL;
        m_EventCallBack[i] = NULL;
    }

    m_pEventCallbackThread = NULL;
	m_pMutexEventCB = VOS_CreateMutex();
}

CEventMgr::~CEventMgr(void)
{
    // 停止事件回调线程;
    IVS_DELETE(m_pEventCallbackThread);//lint !e1551

	try
	{
		if (NULL != m_pMutexEventCB)
		{
			VOS_DestroyMutex(m_pMutexEventCB);
			m_pMutexEventCB = NULL;
		}
	}
	catch(...)
	{ //lint !e1775

	}
}

int CEventMgr::Init()
{
    int iRet = IVS_SUCCEED;

    //开启事件回调线程;
    try
    {
		CEventCallBackJob::instance().SetEventMgr(this);
        m_pEventCallbackThread = new _BaseThread(&CEventCallBackJob::instance(), 0);
    }
    catch (...)
    {
		BP_RUN_LOG_ERR(IVS_PLAYER_RET_ALLOC_MEM_ERROR, "new thread", "NA");
        m_pEventCallbackThread = NULL;
        iRet = IVS_PLAYER_RET_ALLOC_MEM_ERROR;
    }
    return iRet;
}

void CEventMgr::UnInit()
{
    // 停止事件回调线程;
    IVS_DELETE(m_pEventCallbackThread);
}

int CEventMgr::SetEventCallBack( int iSessionID, EventCallBack fEventCallBack /*= NULL*/, void *pUserData /*= NULL*/ )
{
	CLockGuard lock(m_pMutexEventCB);
    if (iSessionID == -1)
    {
        //记录回调函数;
        m_EventCallBack[iSessionID + 1] = fEventCallBack;
        m_EventUserData[iSessionID + 1] = pUserData;

        //SetLocalFilePlayBackEventCallBack(EventCallBackFun, (void*)iSessionID);
        return IVS_SUCCEED;
    }

    if ((SDK_CLIENT_STARTNO > iSessionID) || (SDK_CLIENT_TOTAL <= iSessionID))
    {
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "iSessionID invalid", "NA");
        return IVS_PARA_INVALID;
    }

    //记录回调函数;
    m_EventCallBack[iSessionID + 1] = fEventCallBack;
    m_EventUserData[iSessionID + 1] = pUserData;

    //pUserMgr->SetEventCallBack(EventCallBackFun, (void*)iSessionID);
    //SetLocalFilePlayBackEventCallBack(EventCallBackFun, (void*)iSessionID);

    return IVS_SUCCEED;
}

//异步的回调
int CEventMgr::OnEventCallBack(int iSessionID, int iEventType, void* pEventData, int iBufSize)
{
    switch(iEventType)
    {
    case IVS_EVENT_RTSP_MSG:
    case IVS_EVENT_NETSOURCE_MSG:
        {
            BP_DBG_LOG("OnEventCallBack, iEventType:%d", iEventType);
            HandleStreamMsg(iSessionID, iEventType, pEventData, iBufSize);
            return IVS_SUCCEED;
        }
	case IVS_EVENT_IA_PUSH_ALARM_LOCUS:
		{
			HandleIntelligenceAnalysisEvent(pEventData);
			return IVS_SUCCEED;
		}
		//add by zwx211831, 20140509, 增加是否响应10026事件的设置处理
	case IVS_EVENT_OMU_REPORT_ALARM:
		{
			CHECK_POINTER(g_pNvs, IVS_OPERATE_MEMORY_ERROR);
			CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
			CHECK_POINTER(pUserMgr, IVS_SDK_RET_INVALID_SESSION_ID);
			if (!pUserMgr->GetReceiveOMUAlarm())
			{
				BP_DBG_LOG("CEventMgr::OnEventCallBack", "Receive OMU Alarm, EventType: %d. Ignore", iEventType);
				return IVS_SUCCEED;
			}
			break;
		}
    default:
        break;
    }
    
	CLockGuard lock(m_pMutexEventCB);
    EventCallBack fEventCallBack = m_EventCallBack[iSessionID + 1];
    if (NULL != fEventCallBack)
    {
        fEventCallBack(iEventType, pEventData, static_cast<IVS_UINT32>(iBufSize), m_EventUserData[iSessionID + 1]);
		BP_DBG_LOG("fEventCallBack End, EventType: %d, SessionID: %d, EventData: %p, BufSize: %d", 
			iEventType, iSessionID, pEventData, iBufSize);
    }
	else
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "OnEventCallBack", "fEventCallBack is null.");
	}
    return IVS_SUCCEED;
}//lint !e1762


fExceptionCallBack CEventMgr::GetRtspExceptionCallBack() const
{
    return RtspExceptionCallBack;
}

NET_EXCEPTION_CALLBACK CEventMgr::GetNetExceptionCallBack() const
{
    return NetSourceExceptionCallBack;//lint !e64 匹配
}

PLAYER_EXCEPTION_CALLBACK CEventMgr::GetPlayerExceptionCallBack() const
{
    return PlayerExceptionCallBack;//lint !e64 匹配
}

// 不同业务类型对应的流消息处理方法表
CEventMgr::STREAM_MSG_PRO CEventMgr::m_proStreamMsg[] =
{
	{ SERVICE_TYPE_REALVIDEO,					&CEventMgr::HandleRealVideoStreamMsg},
	{ SERVICE_TYPE_PLAYBACK,					&CEventMgr::HandlePlaybackStreamMsg},
	{ SERVICE_TYPE_BACKUPRECORD_PLAYBACK,		&CEventMgr::HandleBackupRecordStreamMsg},
	{ SERVICE_TYPE_DISASTEBACKUP_PLAYBACK,		&CEventMgr::HandleDisasteBackupRecordStreamMsg},
	{ SERVICE_TYPE_PU_PLAYBACK,					&CEventMgr::HandlePUPlaybackStreamMsg},
	{ SERVICE_TYPE_DOWNLOAD,					&CEventMgr::HandleDownloadStreamMsg},
	{ SERVICE_TYPE_BACKUPRECORD_DOWNLOAD,		&CEventMgr::HandleDownloadStreamMsg},
	{ SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD,		&CEventMgr::HandleDownloadStreamMsg},
	{ SERVICE_TYPE_PU_DOWNLOAD,					&CEventMgr::HandleDownloadStreamMsg},
	{ SERVICE_TYPE_AUDIO_BROADCAST,				&CEventMgr::HandleBroadcastStreamMsg},
	{ SERVICE_TYPE_AUDIO_CALL,					&CEventMgr::HandleAudioCallStreamMsg}
}; 

CEventMgr::FnProStreamMsg CEventMgr::GetFnProStreamMsg(IVS_SERVICE_TYPE enServiceType) const
{
	FnProStreamMsg fn = NULL;
	int iSize = sizeof(m_proStreamMsg) / sizeof(STREAM_MSG_PRO);

	for ( int i = 0; i < iSize; i++ )
	{
		if ( m_proStreamMsg[i].enServiceType == enServiceType )
		{
			fn = m_proStreamMsg[i].fnProStreamMsg;
			break;
		}
	}
	return fn;
}

void CEventMgr::HandleStreamMsg(int iSessionID, int iEventType, void* pEventData, int iBufSize)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER_VOID(g_pNvs);
    CUserMgr *pUserMgr = g_pNvs->GetUserMgr(iSessionID);
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(pEventData);

    STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	// 获取业务类型 ServiceType
    IVS_SERVICE_TYPE enServiceType;
    int iRet = pUserMgr->GetServiceTypebyPlayHandle(pExceptionInfo->ulHandle, enServiceType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "HandleStreamMsg", "GetServiceTypebyPlayHandle failed. ulPlayHandle(%lu) is not realvideo or not platform playback.", pExceptionInfo->ulHandle);
        return;
    }
    BP_RUN_LOG_INF("HandleStreamMsg"," ServiceType = %d, ExceptionCode = %d.",enServiceType, pExceptionInfo->iExceptionCode);

	// 避免全复杂度此处做了分发，如果需要新增处理函数在上面m_proStreamMsg数组中添加消息类型，和处理函数即可
	FnProStreamMsg fnProStreamMsg = GetFnProStreamMsg(enServiceType);
	if (NULL == fnProStreamMsg)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "no process fn","Server Type = %d", enServiceType);
		return;
	}

	EVENT_CALL_BACK_DATA stEventData;
	stEventData.iSessionID = iSessionID;
	stEventData.iEventType = iEventType;
	stEventData.iBufSize = iBufSize;
	stEventData.pEventData = pEventData;
	return (this->*fnProStreamMsg)(stEventData, pUserMgr);
}

void CEventMgr::HandleIntelligenceAnalysisEvent(const void *pEventBuf)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pEventBuf);

	CIAMessageMgr &Mgr = CIAMessageMgr::Instance();
	Mgr.IAPushAlarmLocus((IVS_CHAR *)pEventBuf);//lint !e1773
}//lint !e1762

// 处理实况流消息
void CEventMgr::HandleRealVideoStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);
	CRealPlay *pRealPlay = pUserMgr->GetRealPlayMgr().GetRealPlayEx(pExceptionInfo->ulHandle);

	//电视墙使用的是同样类型的rtsp
	if(NULL == pRealPlay)
	{
		HandleTVWallRealVideoStreamMsg(pUserMgr, *pExceptionInfo);
		return;
	}
	// 获取播放所需要的参数
	CAMERA_PLAY_INFO cameraPlayInfo;
	eSDK_MEMSET(&cameraPlayInfo, 0 ,sizeof(CAMERA_PLAY_INFO));
	pRealPlay->GetCameraPlayInfo(cameraPlayInfo);
	BP_DBG_LOG("CameraPlayInfo: StreamType[%d], Protocal[%d]", cameraPlayInfo.stMediaPara.StreamType, cameraPlayInfo.stMediaPara.ProtocolType);
	// 停止播放
	(void)pRealPlay->StopStream();
	// 上报事件
	(void)OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REALPLAY_FAILED, stEventData.pEventData, 1);

	// 实况的断开连接和收流超时需要考虑重连;
	if (IVS_PLAYER_RET_RTSP_DISCONNECT == pExceptionInfo->iExceptionCode 
		|| IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode
		|| IVS_PLAYER_RET_RTSP_ABNORMAL_STOP == pExceptionInfo->iExceptionCode)
	{
		BP_RUN_LOG_INF("rtsp exception, add to autoresume list","NA");
		CAutoResume& autoResume = pUserMgr->GetAutoResume();
		autoResume.AddCameraPlayInfo2List(cameraPlayInfo);
		pRealPlay->GetRef();
	}

	pUserMgr->GetRealPlayMgr().ReleaseRealPlay(pRealPlay);
}

// 处理电视墙实况流消息
void CEventMgr::HandleTVWallRealVideoStreamMsg(CUserMgr *pUserMgr, STREAM_EXCEPTION_INFO& stExceptionInfo)
{
	IVS_DEBUG_TRACE("");

	CHECK_POINTER_VOID(pUserMgr);
	CTVWallMgr& mTVwallMgr = pUserMgr->GetTVWallMgr();				
	CDecoderChannel* pChannel = mTVwallMgr.GetChannel(stExceptionInfo.ulHandle);
	CHECK_POINTER_VOID(pChannel);	

	CUMW_NOTIFY_INFO info = {0};
	CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO stErrorInfo = {0};

	const IVS_CHAR* pCode = pChannel->GetDecoderID();
	CHECK_POINTER_VOID(pCode);
	(void)CToolsHelp::Strncpy(stErrorInfo.szDecoderID, CUMW_DECODER_ID_LEN + 1, pCode, strlen(pCode));//lint !e831
	stErrorInfo.szDecoderID[CUMW_DECODER_ID_LEN] = 0;

	stErrorInfo.usChannelNo = pChannel->GetChannel();//lint !e734
	if (IVS_PLAYER_RET_RTSP_NORMAL_STOP == stExceptionInfo.iExceptionCode)
	{
		stErrorInfo.stErrorCode.ulErrorCode = (unsigned long)stExceptionInfo.iExceptionCode;
	}
	BP_DBG_LOG("HandleStreamMsg: DecoderCode = %s, usChannelNo = %d.", stErrorInfo.szDecoderID, stErrorInfo.usChannelNo);

	//如果是电视墙大屏无缝切换，则将改路的视频关掉，判断是否为大屏切换，如果是free掉
	if (IVS_PLAYER_RET_RTSP_ABNORMAL_STOP == stExceptionInfo.iExceptionCode)
	{
		BP_DBG_LOG("HandleStreamMsg IVS_PLAYER_RET_RTSP_ABNORMAL_STOP, pChannel->GetSwitchPlay() is :%d,pExceptionInfo->ulHandle:%d,pChannel->GetHandle():%d", 
			pChannel->GetSwitchPlay(),stExceptionInfo.ulHandle,pChannel->GetHandle());
		if(pChannel->GetSwitchPlay())
		{
			if (stExceptionInfo.ulHandle != pChannel->GetHandle())
			{
				pChannel->FreeRTSPHandle(stExceptionInfo.ulHandle);
				//删除实况列表Map中对应的节点
				mTVwallMgr.DeleteChannel(stExceptionInfo.ulHandle);
				BP_RUN_LOG_INF("HandleStreamMsg", "Free Switched RTSP Hanlde, Handle is :%lu", stExceptionInfo.ulHandle);
				return;
			}
		}
	}

	info.ulNotifyType = CUMW_NOTIFY_TYPE_RTSP_EXCEPTION;
	info.pNotifyInfo = (void*)(&stErrorInfo);
	info.ulNotifyInfoLen = sizeof(info);

	mTVwallMgr.DeleteChannel(pChannel->GetHandle());
	mTVwallMgr.NotifyEvent(&info);			
	mTVwallMgr.AddAutoResume(pChannel, CUMW_BREAKDOWN_REASON_NO_VIDEO);

	BP_RUN_LOG_INF("TVWall rtsp exception.","Ready to AutoResume.");

	return;
}

// 处理录像流消息
void CEventMgr::HandlePlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CPlayback* pPlayBack = pUserMgr->GetPlaybackMgr().GetPlayBack(pExceptionInfo->ulHandle);
	if (NULL != pPlayBack)
	{
		// 判断如果是缓冲区的数据是否播放完毕，如果没有播放，再次插入到队列，等待下次检查;
		unsigned int uiRemainCount = 0;
		(void)pPlayBack->GetSourceBufferRemain(uiRemainCount);
		if (uiRemainCount > 0 
			&& IVS_EVENT_RTSP_MSG == stEventData.iEventType 
			&& pExceptionInfo->iExceptionCode == IVS_PLAYER_RET_RTSP_NORMAL_STOP)
		{
			Sleep(100);
			EventCallBackFun(IVS_EVENT_RTSP_MSG, stEventData.pEventData, stEventData.iBufSize, (void*)stEventData.iSessionID);
			BP_RUN_LOG_INF("Handle Rtsp Msg", "Error Code[%u] RemainCount[%u]", pExceptionInfo->iExceptionCode, uiRemainCount);

			return;
		}

		if ((pPlayBack->IsStepPlay() || pPlayBack->IsPaused() || pPlayBack->IsControlPaused()) && 
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Timeout Msg on Pause State");
			return;
		}

		if ((pPlayBack->IsStepPlay()|| pPlayBack->IsPaused()) && pPlayBack->IsStepPlayLastFrame() && IVS_PLAYER_RET_RTSP_NORMAL_STOP == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "StepPlay Is Last Frame Msg on Pause State");
			return;
		}

		if (uiRemainCount > 0 && 
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Decode buffer remain count[%u], do not handle timeout exception", uiRemainCount);
			return;
		}

		BP_RUN_LOG_INF("HandleStreamMsg", "Notify to OCX, Error Code[%u] RemainCount[%u]", pExceptionInfo->iExceptionCode, uiRemainCount);
		// 判断状态
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REMOTE_PLAYBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}
	else
	{
		HandleTPPlaybackStreamMsg(stEventData, pUserMgr);
	}

	return;
}


// 处理智真录像流消息
void CEventMgr::HandleTPPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CTPPlayChannel * pTPPlayChannel = pUserMgr->GetTelepresenceMgr().GetPlayChannel(pExceptionInfo->ulHandle);
	if (NULL != pTPPlayChannel)
	{
		if (pTPPlayChannel->IsPaused() && IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Timeout Msg on Pause State");
			return;
		}

		BP_RUN_LOG_INF("HandleStreamMsg", "Notify to OCX, Error Code[%u]", pExceptionInfo->iExceptionCode);
		// 判断状态
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REMOTE_PLAYBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}

	return;
}


// 处理备份录像流消息
void CEventMgr::HandleBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CPlayback* pPlayBack = pUserMgr->GetPlaybackMgr().GetPlayBack(pExceptionInfo->ulHandle);
	if (NULL != pPlayBack)
	{
		if ((pPlayBack->IsStepPlay() || pPlayBack->IsPaused()) &&
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Timeout Msg on Pause State");
			return;
		}

		// 判断状态
		(void)pUserMgr->GetPlaybackMgr().StopBackupPlayBack(pExceptionInfo->ulHandle);
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REMOTE_PLAYBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}

	return;
}

// 处理容灾录像流消息
void CEventMgr::HandleDisasteBackupRecordStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CPlayback* pPlayBack = pUserMgr->GetPlaybackMgr().GetPlayBack(pExceptionInfo->ulHandle);
	if (NULL != pPlayBack)
	{
		if ((pPlayBack->IsStepPlay() || pPlayBack->IsPaused()) && 
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Timeout Msg on Pause State");
			return;
		}

		// 判断状态
		(void)pUserMgr->GetPlaybackMgr().StopDisasterRecoveryPlayBack(pExceptionInfo->ulHandle);
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REMOTE_PLAYBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}

	return;
}

// 处理前端录像流消息
void CEventMgr::HandlePUPlaybackStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CPlayback* pPlayBack = pUserMgr->GetPlaybackMgr().GetPlayBack(pExceptionInfo->ulHandle);
	if (NULL != pPlayBack)
	{
		if (pPlayBack->IsPaused() && 
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Pu Play Back Timeout Msg on Pause State");
			return;
		}

		(void)IVS_SDK_StopPUPlayBack(pExceptionInfo->iSessionID, pExceptionInfo->ulHandle);
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_REMOTE_PLAYBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}
}

// 处理下载流消息
void CEventMgr::HandleDownloadStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	CDownload* pDownload = pUserMgr->GetDownLoadMgr().GetDownLoad(pExceptionInfo->ulHandle);
	if (NULL != pDownload)
	{
		if (pDownload->IsPaused() && 
			IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
		{
			BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Download Timeout Msg on Pause State");
			return;
		}
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_DOWNLOAD_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}
}

// 处理音频广播流消息
void CEventMgr::HandleBroadcastStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	// Modify by wanglei 00165153:2013.6.27
	// 广播业务不需要进行收流异常处理
	// DTS2013062701806 
	if (IVS_PLAYER_RET_RECV_DATA_TIMEOUT == pExceptionInfo->iExceptionCode)
	{
		BP_RUN_LOG_INF("HandleStreamMsg", "Ignore Broadcast Timeout Msg on Pause State");
		return;
	}

	IVS_ULONG ulConnHandle = (IVS_ULONG)pExceptionInfo->iReseved1;
	std::string strDeviceCode;
	IVS_INT32 iRet = pUserMgr->GetAudioMgr().GetBroadCastDeviceCodeByConnHandle(ulConnHandle, strDeviceCode);
	if (IVS_SUCCEED == iRet)
	{
		// 增加结构体返回异常的语音广播Code码给OCX，OCX组装成xml返回给CU;
		eSDK_MEMSET(pExceptionInfo->szDeviceCode, 0, IVS_DEV_CODE_LEN);
		eSDK_STRNCPY(pExceptionInfo->szDeviceCode, sizeof(pExceptionInfo->szDeviceCode), strDeviceCode.c_str(), IVS_DEV_CODE_LEN - 1);
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_BROADCAST_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}
}

// 处理音频对接流消息
void CEventMgr::HandleAudioCallStreamMsg(EVENT_CALL_BACK_DATA &stEventData, CUserMgr *pUserMgr)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER_VOID(pUserMgr);
	CHECK_POINTER_VOID(stEventData.pEventData);

	STREAM_EXCEPTION_INFO* pExceptionInfo = static_cast<STREAM_EXCEPTION_INFO*>(stEventData.pEventData);
	CHECK_POINTER_VOID(pExceptionInfo);

	IVS_ULONG ulConnHandle = (IVS_ULONG)pExceptionInfo->iReseved1;
	std::string strDeviceCode;
	IVS_INT32 iRet = pUserMgr->GetAudioMgr().GetTalkBackDeviceCodeByConnHandle(ulConnHandle, strDeviceCode);
	if (IVS_SUCCEED == iRet)
	{
		// 增加结构体返回异常的语音广播Code码给OCX，OCX组装成xml返回给CU;
		eSDK_MEMSET(pExceptionInfo->szDeviceCode, 0, sizeof(pExceptionInfo->szDeviceCode));
		eSDK_STRNCPY(pExceptionInfo->szDeviceCode, sizeof(pExceptionInfo->szDeviceCode), strDeviceCode.c_str(), IVS_DEV_CODE_LEN - 1);
		OnEventCallBack(stEventData.iSessionID, IVS_EVENT_TALKBACK_FAILED, stEventData.pEventData, stEventData.iBufSize);
	}
}


