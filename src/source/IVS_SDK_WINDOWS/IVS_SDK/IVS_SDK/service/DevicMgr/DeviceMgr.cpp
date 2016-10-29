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
	filename	: 	DeviceMgr.cpp
	author		:	gwx151720
	created		:	2012/11/01	
	description	:	设备组管理类，提供给cu使用
	copyright	:	Copyright (C) 2012
	history		:	2012/11/01
*********************************************************************/
#include "DeviceMgr.h"
#include "NSSOperator.h"
#include "UserMgr.h"
#include "DeviceMgrXMLProcess.h"
#include "ToolHelp.h"
#include "IVS_Trace.h"
#include "SDKInterface.h"
#include "devEnum.h"
#include "../AlarmMgr/AlarmMgrXMLProcess.h"
#ifndef WIN32
#include "SDKConfig.h"
#endif

const IVS_UINT32 STEP_INDEX = 1000;//查询摄像机/告警设备/语音设备列表时，每次步进1000	

bool bSendLocalCmdFail = false;
bool bSendExdomainCmdFail = false;
std::string strTmpDomainCode = "";

CDeviceMgr::CDeviceMgr(void)
    : m_pUserMgr(NULL)
    , bIsOutAlarm(0)//lint !e1729
    , m_uiVideoChannelTotalNum(0)
	, m_uiVideoChannelTotalNumByDom(0)
    , m_uiAlarmChannelTotalNum(0)
	, m_uiAlarmChannelTotalNumByDom(0)
{
	m_pMutexNVRLock = VOS_CreateMutex();
	m_pMutexRecordTaskLock = VOS_CreateMutex();
	//获取摄像机/告警/语音 列表相关数据锁
	m_pMutexCameraLock = VOS_CreateMutex();
	m_pMutexAlarmLock = VOS_CreateMutex();
    m_pVideoRetriveStatusInfoListMutex = VOS_CreateMutex();
	m_pVideoRetriveStatusInfoListMutexByDom = VOS_CreateMutex();
    m_pAlarmRetriveStatusInfoListMutex = VOS_CreateMutex();
    m_VideoChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
	m_VideoChannelRetriveThreadStatusByDom = eM_Thread_Retrive_Status_Init;
    m_AlarmChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
	m_GetDeviceInfoType = DEVICE_TYPE_CAMERA;
}


CDeviceMgr::~CDeviceMgr(void)
{
	m_pUserMgr = NULL;

    try
	{  
		Clear();
		VOS_DestroyMutex(m_pMutexNVRLock);
		m_pMutexNVRLock = NULL;

		VOS_DestroyMutex(m_pMutexCameraLock);
		m_pMutexCameraLock = NULL;

		VOS_DestroyMutex(m_pMutexAlarmLock);
		m_pMutexAlarmLock = NULL;

		VOS_DestroyMutex(m_pMutexRecordTaskLock);
		m_pMutexRecordTaskLock = NULL;

        VOS_DestroyMutex(m_pVideoRetriveStatusInfoListMutex);
        m_pVideoRetriveStatusInfoListMutex = NULL;

		VOS_DestroyMutex(m_pVideoRetriveStatusInfoListMutexByDom);
		m_pVideoRetriveStatusInfoListMutexByDom = NULL;

        VOS_DestroyMutex(m_pAlarmRetriveStatusInfoListMutex);
        m_pAlarmRetriveStatusInfoListMutex = NULL;
	}
    catch (...)
    {
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "~CDeviceMgr fail", "NA");//lint !e1551
	}
}


void CDeviceMgr::Clear()
{
    // 注意要清空所有申请的链表
    (void)VOS_MutexLock(m_pMutexCameraLock);
	m_vecAllCameraDev.clear();
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

    (void)VOS_MutexLock(m_pMutexAlarmLock);
    m_vecAllAlarmDev.clear();
    (void)VOS_MutexUnlock(m_pMutexAlarmLock);

	// 清理m_DeviceInfiListSelfMap；
	(void)VOS_MutexLock(m_pMutexNVRLock);
	NVR_INFO_MAP_ITOR device_list_map_iter;
	device_list_map_iter = m_NvrInfoMap.begin();
	while (device_list_map_iter != m_NvrInfoMap.end())
	{
		device_list_map_iter->second.clear();
		device_list_map_iter++;
	}
	m_NvrInfoMap.clear();
	(void)VOS_MutexUnlock(m_pMutexNVRLock);
}

void CDeviceMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}


IVS_UINT32 CDeviceMgr::ProcessBatchRedirectService(DeviceRedirectMap & deviceRedirectInfoMap)
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	if (deviceRedirectInfoMap.empty())
	{
		return 0;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	DeviceRedirectMap::iterator itor = deviceRedirectInfoMap.begin();
	CCmd *pCmd = NULL;
	CmdMap cmdMap;
	std::map<std::string, std::string> domainCodeTransIDMap;
	IVS_UINT32 uiTotalNum = 0;
	for (; deviceRedirectInfoMap.end() != itor; itor++)
	{
		CXml xml;
		IVS_INT32 iNum = CDeviceMgrXMLProcess::GetRedirectServiceReqXML(itor->second, xml);
		if (0 == iNum)
		{
			continue;
		}

		string strReqXML;
		IVS_UINT32 uiLen;
		const IVS_CHAR* pReqXML = xml.GetXMLStream(uiLen);
		if (NULL == pReqXML)
		{
			continue;
		}

		std::string strDomainCode = itor->first;
		CSendNssMsgInfo sendNssMsgInfo;	
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_MAIN_DEV_BATCH_REQ);
		sendNssMsgInfo.SetReqData(pReqXML);	
		sendNssMsgInfo.SetDomainCode(strDomainCode);
		pCmd = m_pUserMgr->BuildCmd(sendNssMsgInfo);
		if (NULL == pCmd)
		{
			continue;
		}
		uiTotalNum += (IVS_UINT32)iNum;
		(void)cmdMap.insert(std::make_pair(pCmd->GetTransID(), pCmd));
		(void)domainCodeTransIDMap.insert(std::make_pair(pCmd->GetTransID(), strDomainCode));
	}
	if (cmdMap.empty())
	{
		return 0;
	}

	IVS_UINT32 uiTimeOut =  CSDKConfig::instance().GetTimeOutValue();
	(void)CDispatcher::instance().dispatcherSyncMsgOutBatch(cmdMap, uiTimeOut);

	std::map<std::string, std::string>::iterator domainCodeTransIDMapItor = domainCodeTransIDMap.begin();

	CmdMapIterator cmdMapItor = cmdMap.begin();
	IVS_INT32 iEachRet = 0;
	for (; cmdMap.end() != cmdMapItor; cmdMapItor++)
	{
		domainCodeTransIDMapItor = domainCodeTransIDMap.find(cmdMapItor->first);
		if (domainCodeTransIDMap.end() == domainCodeTransIDMapItor)
		{
			continue;
		}

		CCmd* pRspCmd = cmdMapItor->second;
		if (NULL == pRspCmd)
		{
			continue;
		}

		IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pRspCmd,iEachRet);

		CXml xmlRsp;
		if (NULL == pRsp || !xmlRsp.Parse(pRsp))
		{
			IVS_DELETE(pRsp, MUILI);
			continue;
		}
		IVS_DELETE(pRsp, MUILI);

		itor = deviceRedirectInfoMap.find(domainCodeTransIDMapItor->second);
		if (deviceRedirectInfoMap.end() == itor)
		{
			continue;
		}

		iRet = CDeviceMgrXMLProcess::ParseRedirectServiceRspXML(xmlRsp, itor->second);
		if (IVS_SUCCEED != iRet)
		{
			continue;
		}

		// step 4: 刷新重定向信息到缓存
		(void)UpdateCameraInfoListToCache(itor->first.c_str(), itor->second);

	}
	return uiTotalNum;
}

// 进行重定向业务
IVS_UINT32 CDeviceMgr::ProcessRedirectService(const IVS_CHAR* pDomainCode, IVS_DEVICE_REDIRECT_INFO& deviceRedirectInfo)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);


	// step 1: 组装重定向请求XML消息
	CXml xml;
	std::list<IVS_DEVICE_REDIRECT_INFO> deviceRedirectInfolist; 
	deviceRedirectInfolist.push_back(deviceRedirectInfo);
	IVS_INT32 iNum = CDeviceMgrXMLProcess::GetRedirectServiceReqXML(deviceRedirectInfolist, xml);
	if (0 == iNum)
	{
		return IVS_SUCCEED;
	}
	string strReqXML;
	IVS_UINT32 uiLen;
	const IVS_CHAR* pReqXML = xml.GetXMLStream(uiLen);
	if (NULL == pReqXML)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "process redirect service", "fail to get redirect service req xml, camera code:%s", deviceRedirectInfo.cCameraCode);
		return IVS_XML_INVALID;
	}

	// step 2: 发送重定向消息
	std::string strDomainCode = pDomainCode;
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_MAIN_DEV_BATCH_REQ);
	sendNssMsgInfo.SetReqData(pReqXML);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strRspXML;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmdEX(sendNssMsgInfo, strRspXML, iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "process redirect service", "fail to send redirect message, camera code:%s", deviceRedirectInfo.cCameraCode);
		return (IVS_UINT32)iRet;
	}

	CXml xmlRsp;
	if (!xmlRsp.Parse(strRspXML.c_str()))//lint !e64 匹配
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "process redirect service", "fail to parse response xml:%s, camera code:%s", 
			strRspXML.c_str(), deviceRedirectInfo.cCameraCode);
		return IVS_XML_INVALID;
	}
	//  step 3: 解析重定向响应消息
	iRet = CDeviceMgrXMLProcess::ParseRedirectServiceRspXML(xmlRsp, deviceRedirectInfolist);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "process redirect service", "fail to parse response xml:%s, camera code:%s", 
			strRspXML.c_str(), deviceRedirectInfo.cCameraCode);
		return (IVS_UINT32)iRet;
	}

	// step 4: 刷新重定向信息到缓存
	(void)UpdateCameraInfoListToCache(pDomainCode, deviceRedirectInfolist);

	return IVS_SUCCEED;

}

// 更新摄像机(包括主设备、其他子设备)路由信息到缓存，只更新，不考虑增删
IVS_UINT32 CDeviceMgr::UpdateCameraInfoListToCache(const IVS_CHAR* pDomainCode, std::list<IVS_DEVICE_REDIRECT_INFO>& deviceRedirectInfoList) //lint !e715 !e1762 !e1764
{
	std::list<IVS_DEVICE_REDIRECT_INFO>::iterator itor = deviceRedirectInfoList.begin();
	for (; itor != deviceRedirectInfoList.end(); itor++)
	{
		if (0 == itor->iResult)
		{
			(void)VOS_MutexLock(m_pMutexCameraLock);
			for (unsigned int i = 0; i < m_vecAllCameraDev.size(); i++)
			{
				// 是否需要根据DomainCode+CameraCode来判断？
				if ((0 == strncmp(m_vecAllCameraDev.at(i).cCode, itor->cCameraCode, IVS_DEV_CODE_LEN))
					&& (0 == strncmp(m_vecAllCameraDev.at(i).cDomainCode, pDomainCode, IVS_DOMAIN_CODE_LEN)))
				{	
					eSDK_MEMSET(m_vecAllCameraDev.at(i).cNvrCode, 0, IVS_NVR_CODE_LEN);
					(void)memcpy_s(m_vecAllCameraDev.at(i).cNvrCode, IVS_NVR_CODE_LEN, itor->cNVRCode, IVS_NVR_CODE_LEN);		
				}		
			}
			(void)VOS_MutexUnlock(m_pMutexCameraLock);
		}		
	}

	return IVS_SUCCEED;
}

//获取NVR类型
IVS_UINT32 CDeviceMgr::GetNVRMode(const IVS_CHAR* pDomainCode, const std::string& nvrCode, std::string& complexCode)
{

	IVS_DEVICE_BASIC_INFO nvrInfo;
	eSDK_MEMSET(&nvrInfo, 0, sizeof(nvrInfo));
	IVS_UINT32 uiNvrMode = NVR_MODE_NONE;

	if (m_CLusterInfoMap.end() != m_CLusterInfoMap.find(nvrCode))
	{
		complexCode = nvrCode;
		uiNvrMode = NVR_MODE_CLUSTER;
		return uiNvrMode;
	}

	IVS_INT32 iRet = GetNVRInfoByDomainCodeAndNVRCode(pDomainCode, nvrCode, nvrInfo);
	if (IVS_SUCCEED != iRet)
	{
		return uiNvrMode;
	}

	IVS_CHAR cParentCode[IVS_NVR_CODE_LEN + 1] = {0};
	(void)CToolsHelp::Strncpy(cParentCode, IVS_NVR_CODE_LEN + 1, nvrInfo.cParentCode, IVS_NVR_CODE_LEN);
	if ('\0' != cParentCode[0])
	{
		uiNvrMode = NVR_MODE_CLUSTER;
		complexCode = cParentCode;
	}
	else
	{
		uiNvrMode = NVR_MODE_STACK;
		complexCode = nvrCode;
	}
	return uiNvrMode;
}

// 将需要添加的设备列表安装NVR分类，并确定该NVR的属性为集群或堆叠
IVS_UINT32 CDeviceMgr::MasterDeviceListTransform(const IVS_CHAR*   pDomainCode,
	const IVS_DEVICE_OPER_INFO*  pDevList,
	IVS_UINT32       uiDeviceNum,
	MUL_DEVICE_ADD_NVR_MAP &mulDeviceAddNVRMap,
	std::list<std::string>& lClusterIDList)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR cNvrCode[IVS_NVR_CODE_LEN + 1] = {0};
	IVS_UINT32 uiNvrMode = NVR_MODE_NONE;
	string strComplexCode;
	lClusterIDList.clear();
	for (IVS_UINT32 i = 0; i < uiDeviceNum; i++)
	{
		eSDK_MEMSET(cNvrCode, 0, sizeof(cNvrCode));
		(void)CToolsHelp::Strncpy(cNvrCode, IVS_NVR_CODE_LEN + 1, pDevList[i].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
		string strNvrCode = cNvrCode;
		uiNvrMode = GetNVRMode(pDomainCode, strNvrCode, strComplexCode);

		//如果是集群,添加集群编码到集群列表中	
		if (NVR_MODE_CLUSTER == uiNvrMode)
		{		
			if (0 == lClusterIDList.size())
			{
				lClusterIDList.push_back(strComplexCode);
			}
			else
			{
				bool bHaveSameClusterID = false;
				std::list<std::string>::iterator listItor = lClusterIDList.begin();	
				for (; lClusterIDList.end() != listItor; listItor++)
				{
					if (strComplexCode == *listItor)
					{
						bHaveSameClusterID = true;
						break;
					}
				}

				if (!bHaveSameClusterID)
				{
					lClusterIDList.push_back(strComplexCode);
				}
			}
		}

		MUL_DEVICE_ADD_NVR_MAP::iterator itor = mulDeviceAddNVRMap.find(strComplexCode);
		if (mulDeviceAddNVRMap.end() == itor)
		{
			CMulDeviceAddInfoList MulDeviceAddInfoList;
			MulDeviceAddInfoList.deviceInfoList.push_back(pDevList[i]);
			MulDeviceAddInfoList.m_uiType = uiNvrMode;
			MulDeviceAddInfoList.m_strComplexCode = strComplexCode;
			(void)mulDeviceAddNVRMap.insert(std::make_pair(strComplexCode, MulDeviceAddInfoList));
		}
		else
		{
			(itor->second).deviceInfoList.push_back(pDevList[i]);
		}
	}
	return IVS_SUCCEED;
}


// 添加设备，单个与批量，堆叠和集群统一接口
IVS_INT32 CDeviceMgr::AddMulDevice(const IVS_CHAR* pDomainCode, 
	IVS_UINT32 uiDeviceNum, 
	const IVS_DEVICE_OPER_INFO* pDevList,
	IVS_DEVICE_OPER_RESULT_LIST* pResultList,
	IVS_UINT32 uiResultBufferSize)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pDevList, IVS_PARA_INVALID);
	CHECK_POINTER(pResultList, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	// 对设备的buffersize进行判断
	IVS_UINT32 uiAllocNum = ((uiResultBufferSize - sizeof(IVS_DEVICE_OPER_RESULT_LIST))/sizeof(IVS_DEVICE_OPER_RESULT))+1;
	if (uiDeviceNum != uiAllocNum)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "sdk add device", "fail to para-verify, device num:%d, result data space size:%d", uiDeviceNum, uiAllocNum);
		return IVS_PARA_INVALID;
	}

	if (0 == strlen(pDomainCode))
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_DOMAINCODE_NOT_EXIST, "sdk add device", "fail to para-vertify, domain code is null");
		return IVS_SDK_ERR_DOMAINCODE_NOT_EXIST;
	}

	// 设备添加列表按照NVR（或集群ID）进行分类
	MUL_DEVICE_ADD_NVR_MAP mulDeviceAddNVRMap;
	std::list<IVS_DEVICE_OPER_RESULT> lScheduleResultList;
	std::list<std::string> lClusterCodeList;
	lClusterCodeList.clear();
	(void)MasterDeviceListTransform(pDomainCode, pDevList, uiDeviceNum, mulDeviceAddNVRMap, lClusterCodeList);

	// 如果ClusterList为空,表示没有集群,不用去SMU获取调度;
	IVS_UINT32 uiClusterListSize = lClusterCodeList.size();
	if (0 != uiClusterListSize)
	{
		std::string strClusterCode;
		while(!lClusterCodeList.empty())
		{
			//1.根据集群ID在MAP中查找需要向SMU获取主设备调度的设备列表
			strClusterCode = lClusterCodeList.front();
			lClusterCodeList.pop_front();

			std::list<IVS_DEVICE_OPER_INFO> clusterdeviceList;
			clusterdeviceList.clear();

			MUL_DEVICE_ADD_NVR_MAP::iterator itor = mulDeviceAddNVRMap.find(strClusterCode);
			if (itor != mulDeviceAddNVRMap.end())
			{
				clusterdeviceList = itor->second.deviceInfoList;
				mulDeviceAddNVRMap.erase(itor);
			}
			else
			{
				break;
			}

			//2.组装获取主设备调度请求XML;
			CXml xml;
			(void)CDeviceMgrXMLProcess::GetClusterIPCScheduleReqXML(pDomainCode, strClusterCode, clusterdeviceList, xml);

			IVS_UINT32 uiLen;
			const IVS_CHAR* pReqXML = xml.GetXMLStream(uiLen);
			BP_RUN_LOG_INF("process add muldevice, ", "get schedule req xml[%s]", pReqXML);

			//3.发送获取主设备调度消息
			std::string strDomainCode = pDomainCode;
			CSendNssMsgInfo sendNssMsgInfo;	
			sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
			sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
			sendNssMsgInfo.SetReqID(NSS_CLUSTER_TASK_SCHEDULE_REQ);
			sendNssMsgInfo.SetReqData(pReqXML);	
			sendNssMsgInfo.SetDomainCode(strDomainCode);

			std::string strRspXML;
#if 1
			IVS_INT32 iNeedRedirect = IVS_FAIL;
			IVS_INT32 iRet = m_pUserMgr->SendCmdEX(sendNssMsgInfo, strRspXML, iNeedRedirect);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process add muldevice, ", "fail to send get schedule message.DomainCode:%s, ClusterCode:%s", 
					pDomainCode, strClusterCode.c_str());
				return iRet;
			}

            do
            {
                CXml Xml;
                if(!Xml.Parse(strRspXML.c_str()))//lint !e64 匹配
                {
                    break;
                }

                if(Xml.FindElemEx("Content/ClusterBody/ScheduleResultList/Task"))
                {	  
                    if(Xml.FindElemValue("Result"))
                    {
                        const char* pValue = NULL;
                        (void)Xml.IntoElem();
                        if (Xml.FindElem("Result"))
                        {
                            pValue = Xml.GetElemValue();
                            if (NULL != pValue)                                                            
                            {
                                iRet = atoi(pValue); //lint !e732
                            }
                        }
                     }
                }
  
                if (IVS_SUCCEED != iRet)
                {
                    BP_RUN_LOG_ERR(iRet, "process add muldevice, ", "fail to send get schedule message.DomainCode:%s, ClusterCode:%s", 
                        pDomainCode, strClusterCode.c_str());
                    return iRet;
                }
            }while(0);
#endif

			CXml xmlRsp;
			if (!xmlRsp.Parse(strRspXML.c_str()))//lint !e64 匹配
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "process add muldevice, ", "get schedule response invalid, xml:%s", 
					strRspXML.c_str());
				return IVS_XML_INVALID;
			}

			//4.解析获取主设备调度响应消息
			std::list<IVS_DEVICE_OPER_INFO> lFailedList;
			iRet = CDeviceMgrXMLProcess::ParseGetScheduleRspXML(xmlRsp, clusterdeviceList, lFailedList);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process add muldevice, ", "fail to parse get schedule response xml:%s", 
					strRspXML.c_str());
				return iRet;
			}

			//5.将响应失败的调度关系插入调度结果列表ScheduleResultList中;将响应成功的调度关系合入原先的NVRmap中进行下一步操作
			IVS_DEVICE_OPER_RESULT stOperResult;
			eSDK_MEMSET(&stOperResult, 0, sizeof(stOperResult));
			if (0 != lFailedList.size())
			{
				std::list<IVS_DEVICE_OPER_INFO>::iterator FailItor = lFailedList.begin();
				for (unsigned int i = 0; i < lFailedList.size() && FailItor != lFailedList.end(); i++, FailItor++)
				{
					//向导式添加CameraCode是SDK生成的模拟编码(IVS_1)，需要删除;
					if (IsVirtualCode(FailItor->stDeviceCfg.stDeviceInfo.cCode))
					{
						eSDK_MEMSET(FailItor->stDeviceCfg.stDeviceInfo.cCode, 0, sizeof(FailItor->stDeviceCfg.stDeviceInfo.cCode));
					}

					(void)memcpy_s(stOperResult.cCode, IVS_DEV_CODE_LEN, FailItor->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
					(void)memcpy_s(stOperResult.cNvrCode, IVS_NVR_CODE_LEN, FailItor->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
					stOperResult.uiResult = IVS_CMU_NO_AVAILABLE_CONTAINER_ERROR;
					stOperResult.uiSequence = FailItor->uiSequence;

					lScheduleResultList.push_back(stOperResult);
				}
			}

			if (0 != clusterdeviceList.size())
			{
				std::list<IVS_DEVICE_OPER_INFO>::iterator SucceedItor = clusterdeviceList.begin();
				for (; SucceedItor != clusterdeviceList.end(); SucceedItor++)
				{
					//向导式添加CameraCode是SDK生成的模拟编码(IVS_1)，需要删除;
					if (IsVirtualCode(SucceedItor->stDeviceCfg.stDeviceInfo.cCode))
					{
						eSDK_MEMSET(SucceedItor->stDeviceCfg.stDeviceInfo.cCode, 0, sizeof(SucceedItor->stDeviceCfg.stDeviceInfo.cCode));
					}

					std::string strNvrCode = SucceedItor->stDeviceCfg.cNvrCode;

					MUL_DEVICE_ADD_NVR_MAP::iterator itor = mulDeviceAddNVRMap.find(strNvrCode);//lint !e578
					if (mulDeviceAddNVRMap.end() == itor)
					{
						CMulDeviceAddInfoList MulDeviceAddInfoList;
						MulDeviceAddInfoList.deviceInfoList.push_back(*SucceedItor);
						MulDeviceAddInfoList.m_uiType = NVR_MODE_STACK;
						MulDeviceAddInfoList.m_strComplexCode = strNvrCode;
						(void)mulDeviceAddNVRMap.insert(std::make_pair(strNvrCode, MulDeviceAddInfoList));
					}
					else
					{
						(itor->second).deviceInfoList.push_back(*SucceedItor);
					}
				}
			}
		}	
	}
	//处理添加设备
	return ProcessAddMulDevice(pDomainCode, uiDeviceNum, mulDeviceAddNVRMap, lScheduleResultList, pResultList, uiResultBufferSize);
}

bool CDeviceMgr::IsVirtualCode(std::string strCameraCode) const
{
	if (0 == strCameraCode.length())
	{
		return false;
	}

	unsigned int position = strCameraCode.find("IVS");
	if (std::string::npos == position)
	{
		return false;
	}

	return true;
}

IVS_INT32 CDeviceMgr::DeviceAddOperResultListTransform(std::list<IVS_DEVICE_OPER_RESULT>& operResultList,
	IVS_DEVICE_OPER_RESULT_LIST* pResultList,
	IVS_UINT32 uiResultBufferSize)const
{
	eSDK_MEMSET(pResultList, 0, uiResultBufferSize);
	std::list<IVS_DEVICE_OPER_RESULT>::iterator itor = operResultList.begin();

	for (unsigned int i = 0; i < uiResultBufferSize && itor != operResultList.end(); i++, itor++)
	{
		pResultList->stCodeInfo[i] = *itor;
	}

	pResultList->uiNum = operResultList.size();
	return IVS_SUCCEED;
}

IVS_INT32 CDeviceMgr::ProcessAddMulDevice(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, 
	MUL_DEVICE_ADD_NVR_MAP& mulDeviceAddNVRMap, std::list<IVS_DEVICE_OPER_RESULT> lScheduleResultList, 
	IVS_DEVICE_OPER_RESULT_LIST* pResultList, IVS_UINT32 uiResultBufferSize) //lint !e715
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = -1;
	std::list<IVS_DEVICE_OPER_RESULT> operResultList;


	MUL_DEVICE_ADD_NVR_MAP_ITOR itor  = mulDeviceAddNVRMap.begin();

	for (; itor != mulDeviceAddNVRMap.end(); itor++)
	{
		iRet = ProcessAddMulDeviceToSameNVR(pDomainCode, itor->second, operResultList);

		if (IVS_SUCCEED != iRet)
		{
			std::list<IVS_DEVICE_OPER_INFO>::iterator itor_device = itor->second.deviceInfoList.begin();
			IVS_DEVICE_OPER_RESULT stDevInfo;
			eSDK_MEMSET(&stDevInfo, 0, sizeof(IVS_DEVICE_OPER_RESULT));
			for (; itor_device != itor->second.deviceInfoList.end(); itor_device++)
			{
				(void)memcpy_s(stDevInfo.cCode, IVS_DEV_CODE_LEN, itor_device->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
				(void)memcpy_s(stDevInfo.cNvrCode, IVS_NVR_CODE_LEN, itor_device->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
				stDevInfo.uiSequence = itor_device->uiSequence;
				stDevInfo.uiResult = (IVS_UINT32)iRet;
				operResultList.push_back(stDevInfo);
			}

			BP_RUN_LOG_ERR(iRet, "add mul device", "fail to add mul device to the nvr:%s", itor->first.c_str());
		}
	}

	//将调度失败的列表合入operResultList中
	std::list<IVS_DEVICE_OPER_RESULT>::iterator itorresult = lScheduleResultList.begin();
	for (; itorresult != lScheduleResultList.end(); itorresult++)
	{
		IVS_DEVICE_OPER_RESULT stResultInfo;
		eSDK_MEMSET(&stResultInfo, 0, sizeof(stResultInfo));

		(void)memcpy_s(stResultInfo.cCode, IVS_DEV_CODE_LEN, itorresult->cCode, IVS_DEV_CODE_LEN);
		(void)memcpy_s(stResultInfo.cNvrCode, IVS_NVR_CODE_LEN, itorresult->cNvrCode, IVS_NVR_CODE_LEN);
		stResultInfo.uiResult = itorresult->uiResult;
		stResultInfo.uiSequence = itorresult->uiSequence;

		operResultList.push_back(stResultInfo);
	}

	(void)DeviceAddOperResultListTransform(operResultList, pResultList, uiResultBufferSize);

	
	IVS_UINT32 uiResultNum = operResultList.size();
	IVS_UINT32 uiFailedNum = 0;
	std::list<IVS_DEVICE_OPER_RESULT>::iterator Operitor = operResultList.begin();
	for (; Operitor != operResultList.end();  Operitor++)
	{
		if(0 != Operitor->uiResult)
		{
			uiFailedNum++;
		}
	}

	if(uiFailedNum == uiResultNum)
	{
		iRet = IVS_BATCH_OPERATE_ALL_ERROR;//全部失败
	}
	else if(0 < uiFailedNum && uiResultNum > uiFailedNum)
	{
		iRet = IVS_BATCH_OPERATE_PART_SUCCEED ;//部分成功
	}
	else
	{
		if(uiDeviceNum == uiResultNum)
		{
			iRet = IVS_SUCCEED;
		}
		else
		{
			iRet = IVS_BATCH_OPERATE_PART_SUCCEED ;//部分成功
		}
	}
	BP_RUN_LOG_INF("ProcessAddMulDevice", "device num:%d, result num:%d", uiDeviceNum, uiResultNum);

	return iRet;
}

IVS_INT32 CDeviceMgr::ProcessAddMulDeviceToSameNVR(const IVS_CHAR* pDomainCode,
                                                   CMulDeviceAddInfoList& mulDeviceAddIndoList,
                                                   std::list<IVS_DEVICE_OPER_RESULT>& operResultList)
{
	IVS_DEBUG_TRACE("");
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

    /************************************************************************
        ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
        **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
        ************************************************************************/
    
    // 初始化密钥交换类对象,完成密钥交换
    CSDKSecurityClient oSecurityClient;
    std::string strLinkID;
    IVS_UINT32 uiNVRMode = mulDeviceAddIndoList.m_uiType;

    IVS_INT32 iRet = IVS_SUCCEED;
    string strNVRMode;
    if (NVR_MODE_CLUSTER == uiNVRMode) // 集群模式下密钥交换
    {
        strNVRMode = "Cluster";
        iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, 
            pDomainCode, strLinkID, "CMU", "", mulDeviceAddIndoList.GetComplexCode().c_str());//lint !e64 匹配
    }
    else if (NVR_MODE_STACK == uiNVRMode) // 堆叠下密钥交换
    {
        strNVRMode = "Stack";
		iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, 
			                                          pDomainCode, strLinkID, "SCU", "", mulDeviceAddIndoList.GetComplexCode().c_str());//lint !e64 匹配
    }
    else
    {
        strNVRMode = "not cluster not stack";
        iRet = IVS_SDK_EXCHANGE_KEY_ERR_GET_KEY_FAIL;
    }

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "sdk exchange key", "fail to exchange key, complex code:%s, nvr mode:%s, domain code:%s",
            mulDeviceAddIndoList.GetComplexCode().c_str(), strNVRMode.c_str(), pDomainCode);
        return iRet;
    }

    std::list<std::string> deviceEncryptPwdList;
    std::list<std::string> deviceRegEncryptPwdList;
    deviceEncryptPwdList.clear();
    deviceRegEncryptPwdList.clear();
    std::list<IVS_DEVICE_OPER_INFO>::iterator itor = mulDeviceAddIndoList.deviceInfoList.begin();

    char cDevPasswdTemp[IVS_PWD_LEN + 1] = {0};
    char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
    //	// 加密数据后拼装
    std::string strDevPWDCipher;
    std::string strDevRegPWDCipher;

    for (; itor != mulDeviceAddIndoList.deviceInfoList.end(); itor++)
    {
        eSDK_MEMSET(cDevPasswdTemp, 0, sizeof(cDevPasswdTemp));
        eSDK_MEMSET(cDevRegPWDTemp, 0, sizeof(cDevRegPWDTemp));
        strDevPWDCipher.clear();
        strDevRegPWDCipher.clear();

        // 加密设备密码
        if (0 != strcmp(itor->stDeviceCfg.cDevPasswd, ""))
        {
            (void)CToolsHelp::Strncpy(cDevPasswdTemp, IVS_PWD_LEN + 1, itor->stDeviceCfg.cDevPasswd, IVS_PWD_LEN);
            iRet = oSecurityClient.EncryptString(cDevPasswdTemp, strDevPWDCipher);
            if (IVS_SUCCEED != iRet)
            {
                 BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
                 return IVS_DATA_ENCRYPT_ERROR;
            }
        }
        deviceEncryptPwdList.push_back(strDevPWDCipher);

        // 加密平台密码
        if (0 != strcmp(itor->stDeviceCfg.cDevRegPasswd, ""))
        {
            (void)CToolsHelp::Strncpy(cDevRegPWDTemp, IVS_PWD_LEN + 1, itor->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN);
            iRet = oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWDCipher);
            if (IVS_SUCCEED != iRet)
            {
                BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
                return IVS_DATA_ENCRYPT_ERROR;
            }
        }
        deviceRegEncryptPwdList.push_back(strDevRegPWDCipher);

    }

    // 组装添加设备XML消息
    CXml xmlReq;
    iRet = CDeviceMgrXMLProcess::AddDeviceListGetXML(pDomainCode, mulDeviceAddIndoList, deviceEncryptPwdList, deviceRegEncryptPwdList, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk add device", "fail to get add devcie request xml, domain code:%s, complex code:%s, nvr mode:%s",
            pDomainCode, mulDeviceAddIndoList.GetComplexCode().c_str(), strNVRMode.c_str());
		return iRet;
	}

    std::string strSecurityTransID = oSecurityClient.GetSecurityDataTransID();

    iRet = CXmlProcess::GetEncrypSendMgr(xmlReq, m_pUserMgr, NSS_ADD_MAIN_DEV_REQ, BUILSTRING, pDomainCode, strLinkID, strSecurityTransID);

    // 业务处理： 全部失败，部分成功，全部成功
	if (IVS_SMU_DEV_ADD_ALL_FAIL == iRet || IVS_SMU_DEV_ADD_PART_SUCCEED == iRet || IVS_SUCCEED == iRet)
	{     
        (void)CDeviceMgrXMLProcess::AddDeviceListParseRspXML(xmlReq, operResultList);

		// 将设备添加结果码转换为批量操作结果码
		if(IVS_SMU_DEV_ADD_ALL_FAIL == iRet)
		{
			iRet = IVS_BATCH_OPERATE_ALL_ERROR;
		}
		else if(IVS_SMU_DEV_ADD_PART_SUCCEED == iRet)
		{
			iRet = IVS_BATCH_OPERATE_PART_SUCCEED;
		}
		else
		{
			iRet = IVS_SUCCEED;
		}		
	}
	return iRet;
}

// 增加设备
IVS_INT32 CDeviceMgr::AddDevice(const IVS_CHAR*              pDomainCode, 
                                IVS_UINT32                   uiDeviceNum, 
                                const IVS_DEVICE_OPER_INFO*  pDevList,
                                IVS_DEVICE_OPER_RESULT_LIST* pResultList,
                                IVS_UINT32 uiResultBufferSize)const
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    // 对设备的buffersize进行判断
    IVS_UINT32 uiAllocNum = ((uiResultBufferSize - sizeof(IVS_DEVICE_OPER_RESULT_LIST))/sizeof(IVS_DEVICE_OPER_RESULT))+1;
    if (uiDeviceNum != uiAllocNum)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "uiAllocNum fail", "uiDeviceNum=%u,uiAllocNum=%u", uiDeviceNum, uiAllocNum);
        return IVS_PARA_INVALID;
    }

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
    eSDK_MEMSET(&stDomainRoute, 0, sizeof(stDomainRoute));

	std::string strDomainCode = pDomainCode;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	(void)oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	IVS_INT32 iRet;
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		iRet = AddDeviceCluster(pDomainCode, uiDeviceNum, pDevList, pResultList);

		return iRet;
	}
	// 堆叠情况、单机
	iRet = AddDevicePile(pDomainCode, uiDeviceNum, pDevList, pResultList);

	return iRet;
}

// 堆叠下新增设备
IVS_INT32 CDeviceMgr::AddDevicePile(const IVS_CHAR* pDomainCode, 
	IVS_UINT32 uiDeviceNum, 
	const IVS_DEVICE_OPER_INFO* pDevList, 
	IVS_DEVICE_OPER_RESULT_LIST* pResultList)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiNum = 0;
	IVS_INT32 iRet = 0;
	// modify by limingjie 2013.6.27
	// 在返回的链表中，给设备数赋值
	pResultList->uiNum = uiDeviceNum;

	while(uiNum < uiDeviceNum)
	{
		IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
		if (0 == strcmp(pDevList[uiNum].stDeviceCfg.cNvrCode, ""))
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "pDevList->stDeviceCfg.cNvrCode fail", "CU send Para fail");
			return IVS_PARA_INVALID;
		}
		if (!CToolsHelp::Memcpy(cNVRCode, IVS_NVR_CODE_LEN, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cNVRCode, IVS_NVR_CODE_LEN, pDevList->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		
		/************************************************************************
		** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
		**              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
		************************************************************************/
		// 初始化密钥交换类对象,完成密钥交换
		CSDKSecurityClient oSecurityClient;
		std::string strLinkID;
		int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, pDomainCode, strLinkID, "SCU", "", cNVRCode);//lint !e64 匹配
		if (IVS_SUCCEED != iMultiExchangeKey)
		{
			BP_RUN_LOG_ERR(iMultiExchangeKey, "AddDevice fail", "oSecurityClient Init failed");
			// modify by limingjie 2013.6.27  DTS2013062507914 
			// 不需要直接返回错误，在返回的链表中填入相对应的值，跳出这一轮循环，继续操作。
			pResultList->stCodeInfo[uiNum].uiResult = (IVS_UINT32)iMultiExchangeKey;
			pResultList->stCodeInfo[uiNum].uiSequence = pDevList[uiNum].uiSequence;
			BP_DBG_LOG("uiSequence = %d",pResultList->stCodeInfo[uiNum].uiSequence);
			(void)CToolsHelp::Memcpy(pResultList->stCodeInfo[uiNum].cCode, IVS_DEV_CODE_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
			(void)CToolsHelp::Memcpy(pResultList->stCodeInfo[uiNum].cNvrCode, IVS_NVR_CODE_LEN, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
			uiNum++;
			continue;
		}
		// 加密数据后拼装
		std::string strDevPWDCipher;
		std::string strDevRegPWDCipher;
		char cDevPasswdTemp[IVS_PWD_LEN + 1] = {0};
		char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevPasswd, ""))
		{
			// 设备登录密码
			if (!CToolsHelp::Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPasswdTemp, strDevPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevRegPasswd, ""))
		{
			// 设备向平台密码
			if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set MainDevConfig", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}

		CXml xml;
		iRet = CDeviceMgrXMLProcess::AddDevicePileGetXML(pDomainCode, pDevList, strDevPWDCipher, strDevRegPWDCipher, uiNum, xml);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "AddDevice GetXML fail", "NA");
			return iRet;
		}
		// 获取流到pReq中
		IVS_UINT32 uLen = 0;
		const IVS_CHAR* pReq = xml.GetXMLStream(uLen);
		CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR); 
		BP_RUN_LOG_INF("AddDevice", "send XML = %s", pReq);
		std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

		//创建要发送的CMD，拼装了NSS消息头
		CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_ADD_MAIN_DEV_REQ, pReq, strSecurityTransID);//lint !e64 匹配
		CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
		//发送消息
		CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
		//CCmd *pCmdRsp = CDispatcher::instance().dispatcherSyncMsgOut(pCmd, 120);
		CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
		const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
		HW_DELETE(pCmdRsp);

		if (IVS_SMU_DEV_ADD_ALL_FAIL == iRet || IVS_SMU_DEV_ADD_PART_SUCCEED == iRet || IVS_SUCCEED == iRet)
		{
			CXml xmlRsp;
			if (!xmlRsp.Parse(pRsp))
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xmlRsp.Parse(pRsp) fail", "NA");
				IVS_DELETE(pRsp, MUILI);
				return IVS_OPERATE_MEMORY_ERROR;
			}
			iRet = CDeviceMgrXMLProcess::AddDevicePileParseXML(pResultList, uiNum, xmlRsp);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "AddDevice ParseXML fail", "NA");
				IVS_DELETE(pRsp, MUILI);
				return iRet;
			}
			IVS_DELETE(pRsp, MUILI);
			uiNum++;
			continue;
		}
		IVS_DELETE(pRsp, MUILI);
		return iRet;
	}

	return iRet;
}

// 集群下新增设备
IVS_INT32 CDeviceMgr::AddDeviceCluster(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_OPER_INFO* pDevList, IVS_DEVICE_OPER_RESULT_LIST* pResultList)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");


	std::vector<std::string> m_vecDevPasswd;// 集群下设备密码密文
	std::vector<std::string> m_vecDevRegPWD;// 集群下设备平台密码密文
    m_vecDevPasswd.clear();
    m_vecDevRegPWD.clear();
	/************************************************************************
	** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
	**              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
	************************************************************************/
	// 初始化密钥交换类对象,完成密钥交换
	CSDKSecurityClient oSecurityClient;
	std::string strLinkID;
	int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, pDomainCode, strLinkID, "CMU", "", "", BP::DOMAIN_CLUSTER);//lint !e64 匹配
	if (IVS_SUCCEED != iMultiExchangeKey)
	{
		BP_RUN_LOG_ERR(iMultiExchangeKey, "AddDevice fail", "oSecurityClient Init failed");
		return iMultiExchangeKey;
	}
	IVS_UINT32 uiNum = 0;
	while (uiNum < uiDeviceNum)
	{
		// 加密数据后拼装
		std::string strDevPWDCipher;
		std::string strDevRegPWDCipher;
		char cDevPasswdTemp[IVS_PWD_LEN + 1] = {0};
		char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevPasswd, ""))
		{
			// 设备登录密码
			if (!CToolsHelp::Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPasswdTemp, strDevPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		m_vecDevPasswd.push_back(strDevPWDCipher);

		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevRegPasswd, ""))
		{
			// 设备向平台密码
			if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set MainDevConfig", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		m_vecDevRegPWD.push_back(strDevRegPWDCipher);
		uiNum++;
	}
	// 增加设备组装xml
	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::AddDeviceClusterGetXML(pDomainCode, uiDeviceNum, pDevList, m_vecDevPasswd, m_vecDevRegPWD, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "XML.AddDevice Info GetXML is failed", "NA");
		return iRet;
	}
	// 获取流到pReq中
	IVS_UINT32 uLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR); 
	BP_RUN_LOG_INF("AddDevice", "send XML = %s", pReq);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_ADD_MAIN_DEV_REQ, pReq, strSecurityTransID);//lint !e64 匹配
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	//CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CCmd *pCmdRsp = CDispatcher::instance().dispatcherSyncMsgOut(pCmd, 120);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	CHECK_POINTER(pRsp, iRet);
	if (IVS_SMU_DEV_ADD_ALL_FAIL == iRet || IVS_SMU_DEV_ADD_PART_SUCCEED == iRet || IVS_SUCCEED == iRet)
	{
		CXml xmlRsq;
		if (xmlRsq.Parse(pRsp))
		{
			IVS_UINT32 uiTotal = 0;
			iRet = CDeviceMgrXMLProcess::GetDeviceListNum(xmlRsq, uiTotal);
			pResultList->uiNum = uiTotal;
			if (pResultList->uiNum == uiDeviceNum)
			{
				iRet = CDeviceMgrXMLProcess::AddDeviceClusterPraseXML(xmlRsq, pResultList, (IVS_INT32)uiDeviceNum);
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Add Device fail", "pResultList->uiNum != uiDeviceNum fail");
				IVS_DELETE(pRsp, MUILI);
				return IVS_SMU_XML_INVALID;
			}
		}
		IVS_DELETE(pRsp, MUILI);
		return IVS_SUCCEED;//lint !e438 暂时不知道是否可以将436和437行去掉
	}

	IVS_DELETE(pRsp, MUILI);
	return iRet;
}


// 删除设备
IVS_INT32 CDeviceMgr::DeleteDevice(const IVS_CHAR*  pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_CODE* pDevCodeList , IVS_RESULT_LIST* pResultList , IVS_UINT32 uiResultBufferSize)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCodeList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiTotallDevSize = uiResultBufferSize - sizeof(IVS_RESULT_LIST);
	IVS_UINT32 uiResultInfoSize = sizeof(IVS_RESULT_INFO);
	IVS_UINT32 uiAllocNum = uiTotallDevSize / uiResultInfoSize;
	uiAllocNum =uiAllocNum + 1;

    if (uiDeviceNum != uiAllocNum)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "uiAllocNum fail", "uiDeviceNum=%u,uiAllocNum=%u", uiDeviceNum, uiAllocNum);
        return IVS_PARA_INVALID;
    }

	CXml xmlReq;	
    IVS_INT32 iRet = CDeviceMgrXMLProcess::DeleteDeviceGetXML(uiDeviceNum, pDevCodeList, xmlReq, pDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != Delete Device GetXML", "NA");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	BP_RUN_LOG_INF("DeleteDevice", "send XML = %s", pReq);

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_MAIN_DEV_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmdEX(sendNssMsgInfo, strpRsp, iNeedRedirect, 60);

	BP_RUN_LOG_INF("DeleteDevice", "revice XML = %s", strpRsp.c_str());
    if (IVS_SUCCEED == iRet)
    {
        CXml xmlRsp;
        iRet = IVS_XML_INVALID;
        if (xmlRsp.Parse(strpRsp.c_str()))//lint !e64 匹配
        {
			IVS_UINT32 uitotal = 0;
            iRet = CDeviceMgrXMLProcess::GetDeviceListNum(xmlRsp, uitotal);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Delete Device fail", "Get DeviceListNum fail");
				return IVS_SMU_XML_INVALID;
			}
			pResultList->uiNum = uitotal;
            if (pResultList->uiNum == uiDeviceNum)
            {
                iRet = CDeviceMgrXMLProcess::DeleteDeviceParseXML(pResultList, uiResultBufferSize, xmlRsp);
            } 
			else
			{
				BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Delete Device fail", "xml node is not equal device num");
				return IVS_SMU_XML_INVALID;
			}
        }

		IVS_UINT32 uiFailedNum = 0;
		for(IVS_UINT32 i = 0; i < pResultList->uiNum; i++)
		{
			if(0 != pResultList->stInfo[i].iResult)
			{
				uiFailedNum++;
			}
		}
		if(uiFailedNum == uiDeviceNum)
		{
			iRet = IVS_BATCH_OPERATE_ALL_ERROR;//全部失败
		}
		else if(0 < uiFailedNum && uiDeviceNum > uiFailedNum)
		{
			iRet = IVS_BATCH_OPERATE_PART_SUCCEED ;//部分成功
		}
		else
		{
			iRet = IVS_SUCCEED;
		}
    }
	return iRet;
}

// 查询设备列表
IVS_INT32 CDeviceMgr::QueryDeviceList(IVS_UINT32 uiDeviceType, 
									const IVS_CHAR* pDomainCode,
                                    const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                                    IVS_VOID*                       pBuffer, 
                                    IVS_UINT32                      uiBufferSize)
{        
    CHECK_POINTER(pQueryFormat, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pBuffer,      IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode,  IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    //对fromindex和toindex进行判断
    if (pQueryFormat->stIndex.uiToIndex < pQueryFormat->stIndex.uiFromIndex)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Query Device List fail", "uiToIndex < uiFromIndex,uiToIndex=%u,uiFromIndex=%u", pQueryFormat->stIndex.uiToIndex, pQueryFormat->stIndex.uiFromIndex);
        return IVS_PARA_INVALID;
    }

    IVS_UINT32 uiReqNum = (pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1;
    IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
        return iRet;
    }

    switch (uiDeviceType)
    {
    case DEVICE_TYPE_MAIN:
        {
            IVS_DEVICE_BRIEF_INFO_LIST* pMainList = (IVS_DEVICE_BRIEF_INFO_LIST*) pBuffer;
            return GetMainDeviceList(pDomainCode,pQueryFormat, pMainList, uiBufferSize);
        }        
    default:
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Device Type is not right value","%u", uiDeviceType);
			break;
    }

    return IVS_XML_INVALID;
}
//取得用户拥有设备的域列表
IVS_INT32 CDeviceMgr::GetDomainListByUserID(DOMAIN_VEC& DmList)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if(IVS_SUCCEED != CDeviceMgrXMLProcess::GetDomainListGetXml(m_pUserMgr->GetUserID(),xmlReq))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetDomainListGetXml() Failed", "NA");
		return IVS_FAIL;
	}
    std::string strSMULinkID;
    IVS_INT32 iRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get LocalDomainLinkID failed", "NA");
        return iRet;
    }
	CXml xmlRsp;
	iRet = CXmlProcess::GetCommBuildRet(xmlReq, xmlRsp, NSS_GET_DOMAIN_LIST_BY_USER_REQ, BUILSTRING, strSMULinkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CommBuildRet fail", "NA");
		return iRet;
	}
	iRet = CDeviceMgrXMLProcess::GetDomainListParseXml(xmlRsp, DmList);

	return iRet;
}
// 获取设备列表
IVS_INT32 CDeviceMgr::GetDeviceList(IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDeviceList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 对fromindex和toindex进行判断
	if (0 == pIndexRange->uiToIndex || 0 == pIndexRange->uiFromIndex || pIndexRange->uiToIndex < pIndexRange->uiFromIndex )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Device List fail", "uiToIndex=%u,uiFromIndex=%u", pIndexRange->uiToIndex, pIndexRange->uiFromIndex);
		return IVS_PARA_INVALID;
	}

	// 检测输出大小是否正确
	IVS_UINT32 uiReqNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex) + 1;
	IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
		return iRet;
	}

	switch (uiDeviceType)
	{
		//摄像机类设备信息
	case DEVICE_TYPE_CAMERA:
	case DEVICE_TYPE_CAMERA_EX:
    case DEVICE_TYPE_SHADOW_CAMERA:
	case DEVICE_TYPE_CAMERA_AND_SHADOW:
		{
            iRet = GetVideoChannelList(pIndexRange, pDeviceList, uiBufferSize);
			break;
		}
		//告警类设备信息
	case DEVICE_TYPE_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList; //lint !e1924
            iRet = GetAlarmChannelList(pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList; //lint !e1924
            iRet = GetAlarmChannelList(pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
		//其他
	default:
		{
			BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Device Type Error","%u", uiDeviceType);
			iRet = IVS_DATA_INVALID;
			break;
		}
	}

	return iRet;

}


// 获取指定域设备列表
IVS_INT32 CDeviceMgr::GetDomainDeviceList(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);

	// 对fromindex和toindex进行判断
	if (0 == pIndexRange->uiToIndex || 0 == pIndexRange->uiFromIndex || pIndexRange->uiToIndex < pIndexRange->uiFromIndex )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Device List fail", "uiToIndex=%u,uiFromIndex=%u", pIndexRange->uiToIndex, pIndexRange->uiFromIndex);
		return IVS_PARA_INVALID;
	}

	// 检测输出大小是否正确
	IVS_UINT32 uiReqNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex) + 1;
	IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
		return iRet;
	}

	// 只处理DEVICE_TYPE_CAMERA/DEVICE_TYPE_ALARM/DEVICE_TYPE_EXTEND_ALARM三种设备类型
	switch (uiDeviceType)
	{
	case DEVICE_TYPE_CAMERA:
	case DEVICE_TYPE_CAMERA_EX:
    case DEVICE_TYPE_SHADOW_CAMERA:
	case DEVICE_TYPE_CAMERA_AND_SHADOW:
		{
			iRet = GetDomainVideoChannelList(pDomainCode, pIndexRange, pDeviceList, uiBufferSize);
			break;
		}
	case DEVICE_TYPE_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			iRet = GetDomainAlarmChannelList(pDomainCode, pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			iRet = GetDomainAlarmChannelList(pDomainCode, pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	default:
		{
			BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Device Type Error","%u", uiDeviceType);
			iRet = IVS_DATA_INVALID;
			break;
		}
	}

	return iRet;

}


// 获取摄像机类设备信息
IVS_INT32 CDeviceMgr::GetVideoChannelList(const IVS_INDEX_RANGE* pQueryRange,
	IVS_VOID* pList,
	IVS_UINT32 uiBufferSize)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
    CHECK_POINTER(pList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");
  
    // 如果是重新获取
    if (1 == pQueryRange->uiFromIndex)
    {
		//刷新缓存，重新获取摄像机类设备信息
		IVS_UINT32 uiMsgSendSucCount;
		IVS_INT32 iRet = RefreshALLCameraCache(uiMsgSendSucCount);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Refresh ALL Camera Cache Failed", "NA");
			return iRet;
		}
		if (0 == uiMsgSendSucCount)
		{
			BP_RUN_LOG_WAR("Refresh ALL Camera Cache End, the success count of request message is zero.", "NA");
			return IVS_SUCCEED;
		}
    }

	// 等待响应线程处理完成后，拷贝缓存数据给用户
    time_t currentTime = time(0); 
    IVS_INT32 iRet = IVS_SUCCEED;
    while(true) //lint !e716
    {
        switch(m_VideoChannelRetriveThreadStatus)
        {
        case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
            {
                BP_DBG_LOG("get user channel list, data is retriving.");
                (void)VOS_MutexLock(m_pMutexCameraLock);
                IVS_UINT32 uiChannelListSize = m_vecAllCameraDev.size();
                (void)VOS_MutexUnlock(m_pMutexCameraLock);
                if (uiChannelListSize < pQueryRange->uiToIndex)
                {
                    break;
                }
                iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
                return iRet;
            }
        case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
            BP_DBG_LOG("get user channel list, data is ready.");
            iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        case eM_Thread_Retrive_Excepting:  // 出现异常, 但不影响已有数据
            BP_DBG_LOG("get user channel list, data is exception.");
            iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        default:
            ;
        }
        VOS_Sleep(10);
        if (30 < time(0) - currentTime)  // 默认30秒超时
        {
			BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "get video channel list failed", "NA");
            return IVS_NET_RECV_TIMEOUT;
        }
        
    }
}


IVS_INT32 CDeviceMgr::GetDomainVideoChannelList(const IVS_CHAR* pDomainCode,
	const IVS_INDEX_RANGE* pQueryRange,
	IVS_VOID* pList,
	IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
	CHECK_POINTER(pList, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	// 如果是重新获取
	if (1 == pQueryRange->uiFromIndex)
	{
		//刷新缓存，重新获取摄像机类设备信息
		IVS_UINT32 uiMsgSendSucCount;
		IVS_INT32 iRet = RefreshDomainCameraCache(pDomainCode, uiMsgSendSucCount);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Refresh Domain Code Camera Cache Failed", "Domain Code: %s", pDomainCode);
			return iRet;
		}
		if (0 == uiMsgSendSucCount)
		{
			BP_RUN_LOG_WAR("Refresh ALL Camera Cache End, the success count of request message is zero.", "Domain Code: %s", pDomainCode);
			return IVS_SUCCEED;
		}
	}

	// 等待响应线程处理完成后，拷贝缓存数据给用户
	time_t currentTime = time(0); 
	IVS_INT32 iRet = IVS_SUCCEED;
	while(true)//lint !e716
	{
		switch(m_VideoChannelRetriveThreadStatusByDom)
		{
		case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
			{
				BP_DBG_LOG("get user channel list, data is retriving.");
				(void)VOS_MutexLock(m_pMutexCameraLock);
				IVS_UINT32 uiChannelListSize = m_vecAllCamDevByDom.size();
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				if (uiChannelListSize < pQueryRange->uiToIndex)
				{
					break;
				}
				iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
				return iRet;
			}
		case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
			BP_DBG_LOG("get user channel list, data is ready.");
			iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
			return iRet;

		case eM_Thread_Retrive_Excepting:  // 出现异常, 但不影响已有数据
			BP_DBG_LOG("get user channel list, data is exception.");
			iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
			return iRet;

		default:
			;
		}
		VOS_Sleep(10);
		if (30 < time(0) - currentTime)  // 默认30秒超时
		{
			return IVS_NET_RECV_TIMEOUT;
		}
	}
}


IVS_INT32 CDeviceMgr::GetAlarmChannelList(const IVS_INDEX_RANGE* pQueryRange,
    IVS_ALARM_BRIEF_INFO_LIST* pList,
    IVS_UINT32 uiBufferSize,
    IVS_UINT32 uiAlarmType)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
    CHECK_POINTER(pList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    eSDK_MEMSET(pList,0,uiBufferSize);
    // 初始化pList
    pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
    pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

    // from 
    if (1 == pQueryRange->uiFromIndex)
    {
        IVS_INDEX_RANGE stRange;
        stRange.uiFromIndex = 1;
        stRange.uiToIndex = STEP_INDEX;
        m_uiAlarmChannelTotalNum = 0;
        DOMAIN_VEC dmList;
        IVS_INT32 iRet = GetDomainListByUserID(dmList);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "get domain list by user id", "fail to get domain list");
            return iRet;
        }
        std::string strTransID;
        std::string strTargetDomainCode;
        //std::string strSendDomainCode;
        std::string strExdomainFlag;
        //m_pUserMgr->GetDomainCode(strLocalDomainCode);
        DOMAIN_VEC::iterator itor = dmList.begin();
		DOMAIN_VEC::iterator itorEnd = dmList.end();

        IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
        eSDK_MEMSET(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

        (void)VOS_MutexLock(m_pMutexAlarmLock);
        m_vecAllAlarmDev.clear();
        (void)VOS_MutexUnlock(m_pMutexAlarmLock);

		//先将所有的检索状态信息插入List中，防止取的比插入的快
		iRet = InitChannelRetriveStatusInfoList(dmList, m_AlarmChannelRetriveStatusInfoList, m_pAlarmRetriveStatusInfoListMutex,
			m_AlarmChannelRetriveThreadStatus, (IVS_DEVICE_TYPE)uiAlarmType);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get Alarm ChannelList", "Init Channel Retrive Status Info List failed");
			return iRet;
		}

		//发送请求消息。如果发送失败则将域编码对应的ChannelRetriveStatusInfo从List中删除。
		for(; itorEnd != itor; itor++)
		{
			strTargetDomainCode = *itor;
			for (int i = 0; i < 2; i++)
			{
				IVS_INT32 iExDomainFlag;
				// 发送本域消息
				CXml reqVideoXml;
				strTransID = CMKTransID::Instance().GenerateTransID();

				if (1== i && DEVICE_TYPE_EXTEND_ALARM == uiAlarmType)
				{
					// 外部域告警设备不需要向外域（PCG）请求
					continue;
				}

				if (0 == i)
				{
					(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);//lint !e64 匹配
					strExdomainFlag = "Local Domain";
					iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
				}
				else
				{
					(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);//lint !e64 匹配
					strExdomainFlag = "ExDomain";
					iExDomainFlag = EXDOMAIN_FLAG_OUT;
				}
				iRet = m_pUserMgr->SendAsynMsg(strTargetDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);//lint !e64 匹配

				BP_RUN_LOG_INF("process get user channel list,", "start to send alarm[%u] request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s], return: %d", 
					uiAlarmType, stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), strTargetDomainCode.c_str(), strTransID.c_str(), iRet);

				if (IVS_SUCCEED == iRet)
				{
					//将ChannelRetriveStatusInfoList中对应节点加到TransIDChannelListRetriveStatusMap中。
					iRet = FindChannelRetriveStatusInfoList(strTargetDomainCode, m_AlarmChannelRetriveStatusInfoList, m_pAlarmRetriveStatusInfoListMutex, iExDomainFlag, retriveStatusInfo);
					if (IVS_SUCCEED != iRet)
					{
						BP_RUN_LOG_ERR(iRet, "Get Alarm ChannelList", "Find Channel Retrive Status Info List failed");
						return iRet;
					}
					CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
					if (NULL != pSingleSvrProxy)
					{
						pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
					}
				}
				else
				{
					iRet = RemoveChannelRetriveStatusInfoList(strTargetDomainCode, m_AlarmChannelRetriveStatusInfoList, m_pAlarmRetriveStatusInfoListMutex, iExDomainFlag);
					if (IVS_SUCCEED != iRet)
					{
						BP_RUN_LOG_ERR(iRet, "Get Alarm ChannelList", "Remove Channel Retrive Status Info List failed");
						return iRet;
					}
				}
			}
		}

		(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
		IVS_UINT32 iStatusInfoListSize = m_AlarmChannelRetriveStatusInfoList.size();
		(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
		if (iStatusInfoListSize == 0)
		{
			pList->uiTotal = 0;
			return IVS_SUCCEED;
		}

    }


    time_t currentTime = time(0); 
    IVS_INT32 iRet = IVS_SUCCEED;
    while(true) //lint !e716
    {
		(void)SetChannelRetriveStatusReady(m_AlarmChannelRetriveStatusInfoList, m_pAlarmRetriveStatusInfoListMutex, m_AlarmChannelRetriveThreadStatus);

        switch(m_AlarmChannelRetriveThreadStatus)
        {
        case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
            {
                BP_DBG_LOG("get user channel list, data is retriving.");
                (void)VOS_MutexLock(m_pMutexAlarmLock);
                IVS_UINT32 uiChannelListSize = m_vecAllAlarmDev.size();
                (void)VOS_MutexUnlock(m_pMutexAlarmLock);
                if (uiChannelListSize < pQueryRange->uiToIndex)
                {
                    break;
                }
                iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
                return iRet;
            }
        case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
            BP_DBG_LOG("get user channel list, data is ready.");
            iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        case eM_Thread_Retrive_Excepting:  // 出现异常
            BP_DBG_LOG("get user channel list, data is exception.");
             iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        default:
            ;
        }

        VOS_Sleep(10);
        if (30 < time(0) - currentTime)  // 默认30秒超时
        {
            return IVS_NET_RECV_TIMEOUT;
        }

    }
}


IVS_INT32 CDeviceMgr::GetDomainAlarmChannelList(const IVS_CHAR* pDomainCode,
	const IVS_INDEX_RANGE* pQueryRange,
	IVS_ALARM_BRIEF_INFO_LIST* pList,
	IVS_UINT32 uiBufferSize,
	IVS_UINT32 uiAlarmType)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
	CHECK_POINTER(pList, IVS_PARA_INVALID);
	
	IVS_INT32 iRet = IVS_FAIL;

	eSDK_MEMSET(pList,0,uiBufferSize);
	// 初始化pList
	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
	pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

	// from 
	if (1 == pQueryRange->uiFromIndex)
	{
		IVS_INDEX_RANGE stRange;
		stRange.uiFromIndex = 1;
		stRange.uiToIndex = STEP_INDEX;
		m_uiAlarmChannelTotalNumByDom = 0;

		std::string strTransID;
		std::string strTargetDomainCode;
		std::string strExdomainFlag;
		IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
		eSDK_MEMSET(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

		(void)VOS_MutexLock(m_pMutexAlarmLock);
		m_vecAllAlarmDevByDom.clear();
		(void)VOS_MutexUnlock(m_pMutexAlarmLock);

		(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
		m_AlarmChannelRetriveStatusInfoList.clear();
		m_AlarmChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
		(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);

		strTargetDomainCode = pDomainCode;
		for (int i = 0; i < 2; i++)
		{
			// 发送本域消息
			CXml reqVideoXml;
			strTransID = CMKTransID::Instance().GenerateTransID();

			if (1== i && DEVICE_TYPE_EXTEND_ALARM == uiAlarmType)
			{
				// 外部域告警设备不需要向外域（PCG）请求
				continue;
			}

			if (0 == i)
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);//lint !e64 匹配
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
				strExdomainFlag = "Local Domain";
			}
			else
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);	//lint !e64 匹配
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
				strExdomainFlag = "ExDomain";
			}
			retriveStatusInfo.iSendTime = clock();
			iRet = m_pUserMgr->SendAsynMsg(strTargetDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);//lint !e64 匹配

			BP_RUN_LOG_INF("process get user channel list,", "start to send alarm[%u] request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
				uiAlarmType, stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), strTargetDomainCode.c_str(), strTransID.c_str());

			if (IVS_SUCCEED == iRet)
			{
				retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
				retriveStatusInfo.uiChannelType = uiAlarmType;
				retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
				eSDK_STRNCPY(retriveStatusInfo.szTargetDomainCode, sizeof(retriveStatusInfo.szTargetDomainCode), strTargetDomainCode.c_str(), IVS_DOMAIN_CODE_LEN);
				retriveStatusInfo.iGetChannelType = eM_Get_Device_ByDomainCode;

				CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
				if (NULL != pSingleSvrProxy)
				{
					pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
				}

				(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
				m_AlarmChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
				(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
			}
		}

		(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
		IVS_UINT32 iStatusInfoListSize = m_AlarmChannelRetriveStatusInfoList.size();
		(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
		if (iStatusInfoListSize == 0)
		{
			pList->uiTotal = 0;
			return IVS_SUCCEED;
		}

	}


	time_t currentTime = time(0); 
	iRet = IVS_SUCCEED;
	while(true) //lint !e716
	{
		switch(m_AlarmChannelRetriveThreadStatus)
		{
		case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
			{
				BP_DBG_LOG("get user channel list, data is retriving.");
				(void)VOS_MutexLock(m_pMutexCameraLock);
				IVS_UINT32 uiChannelListSize = m_vecAllAlarmDevByDom.size();
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				if (uiChannelListSize < pQueryRange->uiToIndex)
				{
					break;
				}
				iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
				return iRet;
			}
		case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
			BP_DBG_LOG("get user channel list, data is ready.");
			iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
			return iRet;

		case eM_Thread_Retrive_Excepting:  // 出现异常
			BP_DBG_LOG("get user channel list, data is exception.");
			iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
			return iRet;

		default:
			;
		}
		VOS_Sleep(10);
		if (30 < time(0) - currentTime)  // 默认30秒超时
		{

			return IVS_NET_RECV_TIMEOUT;
		}
	}
}


IVS_INT32 CDeviceMgr::GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList)
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiToIndex = 0;
    (void)VOS_MutexLock(m_pMutexAlarmLock);

    BP_RUN_LOG_INF("get alarm channel list from cache", "cache size:%u, real num:%u, page index[%u - %u]", 
        m_vecAllAlarmDev.size(), m_uiAlarmChannelTotalNum, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

    if (pQueryRange->uiFromIndex > m_vecAllAlarmDev.size())
    {
        (void)VOS_MutexUnlock(m_pMutexAlarmLock);
        return IVS_SUCCEED;
    }

    if (pQueryRange->uiFromIndex < m_uiAlarmChannelTotalNum && pQueryRange->uiToIndex > m_uiAlarmChannelTotalNum)
    {
        // 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
        pList->uiTotal = m_vecAllAlarmDev.size();
    }
    else
    {
        pList->uiTotal = m_uiAlarmChannelTotalNum;
    }


    if(pQueryRange->uiToIndex > m_vecAllAlarmDev.size())
    {
        uiToIndex = (IVS_UINT32)((m_vecAllAlarmDev.size() - pQueryRange->uiFromIndex)+1);
    }
    else
    {
        uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
    }

    for(IVS_UINT32 i = 0; i< uiToIndex; i++)
    {
        pList->stAlarmBriefInfo[i] = m_vecAllAlarmDev.at(pQueryRange->uiFromIndex + i - 1); //lint !e64 匹配
    }
    (void)VOS_MutexUnlock(m_pMutexAlarmLock);
    return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList, ALARM_BRIEF_INFO_VEC &vecAlarmDev)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_UINT32 uiToIndex = 0;
	(void)VOS_MutexLock(m_pMutexAlarmLock);

	BP_RUN_LOG_INF("get alarm channel list from cache", "cache size:%u, real num:%u, page index[%u - %u]", 
		vecAlarmDev.size(), m_uiAlarmChannelTotalNumByDom, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

	if (pQueryRange->uiFromIndex > vecAlarmDev.size())
	{
		(void)VOS_MutexUnlock(m_pMutexAlarmLock);
		return IVS_SUCCEED;
	}

	if (pQueryRange->uiFromIndex < m_uiAlarmChannelTotalNumByDom && pQueryRange->uiToIndex > m_uiAlarmChannelTotalNumByDom)
	{
		// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
		pList->uiTotal = vecAlarmDev.size();
	}
	else
	{
		pList->uiTotal = m_uiAlarmChannelTotalNumByDom;
	}


	if(pQueryRange->uiToIndex > vecAlarmDev.size())
	{
		uiToIndex = (IVS_UINT32)((vecAlarmDev.size() - pQueryRange->uiFromIndex)+1);
	}
	else
	{
		uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
	}

	for(IVS_UINT32 i = 0; i< uiToIndex; i++)
	{
		pList->stAlarmBriefInfo[i] = vecAlarmDev.at(pQueryRange->uiFromIndex + i - 1); //lint !e64 匹配
	}
	(void)VOS_MutexUnlock(m_pMutexAlarmLock);
	return IVS_SUCCEED;
}

// 从缓存中拷贝数据到用户内存
IVS_INT32 CDeviceMgr::GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_VOID* pList)
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiToIndex = 0;	// 真实返回的数量
    (void)VOS_MutexLock(m_pMutexCameraLock);

    BP_RUN_LOG_INF("GetVideoChannelInfoListFromCache", "cache size:%u, RealNum:%u, query from index:%u, query to index:%u", 
        m_vecAllCameraDev.size(), m_uiVideoChannelTotalNum, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

	// 如果缓存的数量小于用户请求的起始值，则不拷贝直接返回
    if (pQueryRange->uiFromIndex > m_vecAllCameraDev.size())
    {
        (void)VOS_MutexUnlock(m_pMutexCameraLock);
        return IVS_SUCCEED;
    }

    if(pQueryRange->uiToIndex > m_vecAllCameraDev.size())
    {
        uiToIndex = (IVS_UINT32)((m_vecAllCameraDev.size() - pQueryRange->uiFromIndex) + 1);
    }
    else
    {
        uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex) + 1);
    }

	// 如果需要获取带有扩展高度、经纬度信息
	if (DEVICE_TYPE_CAMERA_EX == m_GetDeviceInfoType)
	{
		IVS_CAMERA_BRIEF_INFO_LIST_EX *pUserList = reinterpret_cast<IVS_CAMERA_BRIEF_INFO_LIST_EX *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNum && pQueryRange->uiToIndex > m_uiVideoChannelTotalNum)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCameraDev.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNum;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		// 将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCameraDev.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}

	//如果需要获取影子摄像机信息
	if (DEVICE_TYPE_SHADOW_CAMERA == m_GetDeviceInfoType ||
		DEVICE_TYPE_CAMERA_AND_SHADOW == m_GetDeviceInfoType)
	{
		IVS_SHADOW_CAMERA_BRIEF_INFO_LIST *pUserList = reinterpret_cast<IVS_SHADOW_CAMERA_BRIEF_INFO_LIST *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNum && pQueryRange->uiToIndex > m_uiVideoChannelTotalNum)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCameraDev.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNum;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		//将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCameraDev.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}

	//如果需要获取摄像机信息
	if (DEVICE_TYPE_CAMERA == m_GetDeviceInfoType)
	{
		IVS_CAMERA_BRIEF_INFO_LIST *pUserList = reinterpret_cast<IVS_CAMERA_BRIEF_INFO_LIST *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNum && pQueryRange->uiToIndex > m_uiVideoChannelTotalNum)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCameraDev.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNum;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		//将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCameraDev.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}
    
    (void)VOS_MutexUnlock(m_pMutexCameraLock);
    return IVS_SUCCEED;
}

//从缓存中获取摄像机类设备信息
IVS_INT32 CDeviceMgr::GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange,
	IVS_VOID* pList, 
	const CAMERA_BRIEF_INFO_ALL_VEC &vecCameraDev)
{
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiToIndex = 0;					//真实返回的数量
	(void)VOS_MutexLock(m_pMutexCameraLock);

	BP_RUN_LOG_INF("GetVideoChannelInfoListFromCache", "cache size:%u, RealNum:%u, query from index:%u, query to index:%u", 
		vecCameraDev.size(), m_uiVideoChannelTotalNumByDom, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

	//如果缓存的数量小于用户请求的起始值，则不拷贝直接返回
	if (pQueryRange->uiFromIndex > vecCameraDev.size())
	{
		(void)VOS_MutexUnlock(m_pMutexCameraLock);
		return IVS_SUCCEED;
	}

	// 计算uiToIndex
	if(pQueryRange->uiToIndex > m_vecAllCamDevByDom.size())
	{
		uiToIndex = (IVS_UINT32)((m_vecAllCamDevByDom.size() - pQueryRange->uiFromIndex)+1);
	}
	else
	{
		uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
	}

	//如果需要获取带有扩展高度、经纬度信息
	if (DEVICE_TYPE_CAMERA_EX == m_GetDeviceInfoType)
	{
		IVS_CAMERA_BRIEF_INFO_LIST_EX *pUserList = reinterpret_cast<IVS_CAMERA_BRIEF_INFO_LIST_EX *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNumByDom && pQueryRange->uiToIndex > m_uiVideoChannelTotalNumByDom)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCamDevByDom.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNumByDom;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		//将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCamDevByDom.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}

	//如果需要获取影子摄像机信息
	if (DEVICE_TYPE_SHADOW_CAMERA == m_GetDeviceInfoType ||
		DEVICE_TYPE_CAMERA_AND_SHADOW == m_GetDeviceInfoType)
	{
		IVS_SHADOW_CAMERA_BRIEF_INFO_LIST *pUserList = reinterpret_cast<IVS_SHADOW_CAMERA_BRIEF_INFO_LIST *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNum && pQueryRange->uiToIndex > m_uiVideoChannelTotalNum)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCamDevByDom.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNumByDom;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		//将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCamDevByDom.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}

	//如果需要获取摄像机信息
	if (DEVICE_TYPE_CAMERA == m_GetDeviceInfoType)
	{
		IVS_CAMERA_BRIEF_INFO_LIST *pUserList = reinterpret_cast<IVS_CAMERA_BRIEF_INFO_LIST *>(pList);
		pUserList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
		if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNumByDom && pQueryRange->uiToIndex > m_uiVideoChannelTotalNumByDom)
		{
			// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
			pUserList->uiTotal = m_vecAllCamDevByDom.size();
			pUserList->stIndexRange.uiToIndex = pUserList->uiTotal;
		}
		else
		{
			pUserList->uiTotal = m_uiVideoChannelTotalNumByDom;
			pUserList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
		}

		//将数据从缓存拷贝到用户内存中
		for(IVS_UINT32 i = 0; i< uiToIndex; i++)
		{
			CopyCamInfoFromCache(m_vecAllCamDevByDom.at(pQueryRange->uiFromIndex + i - 1), pUserList->stCameraBriefInfo[i]);//lint !e64 匹配
		}
	}

	(void)VOS_MutexUnlock(m_pMutexCameraLock);
	return IVS_SUCCEED;
}


//拷贝摄像机信息
IVS_VOID CDeviceMgr::CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, 
	IVS_CAMERA_BRIEF_INFO& CamInfo) const
{
	const IVS_UINT32 uiReserveSize = 32;
	const IVS_UINT32 uiCamInfoSize = sizeof(IVS_CAMERA_BRIEF_INFO);

	eSDK_MEMCPY(&CamInfo, uiCamInfoSize, &CamInfoALL, uiCamInfoSize - uiReserveSize);
	eSDK_MEMSET(CamInfo.cReserve, 0, uiReserveSize);
}

//拷贝带有高度、经纬度的扩展摄像机信息
IVS_VOID CDeviceMgr::CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, 
	IVS_CAMERA_BRIEF_INFO_EX& CamInfoEx) const
{
	const IVS_UINT32 uiReserveSize = 32;
	const IVS_UINT32 uiCamInfoExSize = sizeof(IVS_CAMERA_BRIEF_INFO_EX);

	eSDK_MEMCPY(&CamInfoEx, uiCamInfoExSize, &CamInfoALL, uiCamInfoExSize - uiReserveSize);
	eSDK_MEMSET(CamInfoEx.cReserve, 0, uiReserveSize);
}

//拷贝影子摄像机信息
IVS_VOID CDeviceMgr::CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, 
	IVS_SHADOW_CAMERA_BRIEF_INFO& CamInfoShadow) const
{
	const IVS_UINT32 uiReserveSize = 32;
	const IVS_UINT32 uiCamInfoShadowSize = sizeof(IVS_SHADOW_CAMERA_BRIEF_INFO);

	eSDK_MEMCPY(&CamInfoShadow, uiCamInfoShadowSize, &CamInfoALL, uiCamInfoShadowSize - uiReserveSize);
	eSDK_MEMSET(CamInfoShadow.cReserve, 0, uiReserveSize);
}

void CDeviceMgr::SetChannelListRetriveStatusValue(IVS_UINT32 uiChannelType, 
                                                                                         const IVS_CHAR* pTargetDomainCode, 
                                                                                         IVS_INT32 iExDomainFlag, 
                                                                                         IVS_UINT32 uiRealNum,
                                                                                         IVS_INT32 emThreadStatus,
																						 IVS_INT32 iGetChannelType)
{
    IVS_INT32 iCurrentThreadStatus = eM_Thread_Retrive_Status_Ready;
    IVS_UINT32 uiTotalNum = 0;
    if (DEVICE_TYPE_CAMERA == uiChannelType ||
		DEVICE_TYPE_CAMERA_EX == uiChannelType ||
		DEVICE_TYPE_SHADOW_CAMERA == uiChannelType ||
		DEVICE_TYPE_CAMERA_AND_SHADOW == uiChannelType)
    {
        (void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);

		// 如果是查询所有设备列表
        if (eM_Get_Device_All == iGetChannelType)
        {
			// 设置线程状态
			std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_VideoChannelRetriveStatusInfoList.begin();
			for (; m_VideoChannelRetriveStatusInfoList.end() != itor; itor++)
			{
				if (itor->iExDomainFlag == iExDomainFlag && 
					0 == strncmp(itor->szTargetDomainCode, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
				{
					if (eM_Thread_Retrive_Excepting != emThreadStatus)
					{
						itor->uiRealNum =  uiRealNum;     // 当前分页异常，不影响已有完成分页
					}

					itor->iThreadStatus = emThreadStatus;
				}
				uiTotalNum += itor->uiRealNum;
				if (iCurrentThreadStatus > itor->iThreadStatus)
				{
					iCurrentThreadStatus = itor->iThreadStatus;
				}
			}
			m_VideoChannelRetriveThreadStatus = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;

			// 设置总数
			if (uiTotalNum > m_uiVideoChannelTotalNum)
			{
				m_uiVideoChannelTotalNum = uiTotalNum;
			}
            
        }
		// 如果是查询指定域设备类别
		else if (eM_Get_Device_ByDomainCode == iGetChannelType)
		{
			// 设置线程状态
			std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_VideoChannelRetriveStatusInfoListByDom.begin();
			for (; m_VideoChannelRetriveStatusInfoListByDom.end() != itor; itor++)
			{
				if (itor->iExDomainFlag == iExDomainFlag && 
					0 == strncmp(itor->szTargetDomainCode, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
				{
					if (eM_Thread_Retrive_Excepting != emThreadStatus)
					{
						itor->uiRealNum =  uiRealNum;     // 当前分页异常，不影响已有完成分页
					}

					itor->iThreadStatus = emThreadStatus;
				}
				uiTotalNum += itor->uiRealNum;
				if (iCurrentThreadStatus > itor->iThreadStatus)
				{
					iCurrentThreadStatus = itor->iThreadStatus;
				}
			}
			m_VideoChannelRetriveThreadStatusByDom = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;

			// 设置总数
			if (uiTotalNum > m_uiVideoChannelTotalNumByDom)
			{
				m_uiVideoChannelTotalNumByDom = uiTotalNum;
				BP_RUN_LOG_INF("SetChannelListRetriveStatusValue", "RealNum: %d", m_uiVideoChannelTotalNumByDom);
			}
		}

        (void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

    }
    else
    {
        (void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);

        std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_AlarmChannelRetriveStatusInfoList.begin();
        for (; m_AlarmChannelRetriveStatusInfoList.end() != itor; itor++)
        {
            if (itor->iExDomainFlag == iExDomainFlag && 
                0 == strncmp(itor->szTargetDomainCode, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
            {
                if (eM_Thread_Retrive_Excepting != emThreadStatus)
                {
                    itor->uiRealNum =  uiRealNum; // 当前分页异常，不影响已有完成分页
                }

                itor->iThreadStatus = emThreadStatus;
            }
            uiTotalNum += itor->uiRealNum;
            if (iCurrentThreadStatus > itor->iThreadStatus)
            {
                iCurrentThreadStatus = itor->iThreadStatus;
            }
        }
        m_AlarmChannelRetriveThreadStatus = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;
        if (eM_Get_Device_All == iGetChannelType && uiTotalNum > m_uiAlarmChannelTotalNum)
        {
            m_uiAlarmChannelTotalNum = uiTotalNum;
        }
		else if (eM_Get_Device_ByDomainCode == iGetChannelType && uiTotalNum > m_uiAlarmChannelTotalNumByDom)
		{
			m_uiAlarmChannelTotalNumByDom = uiTotalNum;
		}

        (void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
    }

}


IVS_INT32 CDeviceMgr::ParseChannelListRsp(const IVS_CHAR* pData, IVS_UINT32 uiChannelType, const IVS_INT32 iExDomainFlag, 
	const IVS_CHAR* pTargetDomainCode, IVS_PAGE_INFO& pageInfo, IVS_UINT32& uiItemSize, IVS_INT32 iGetChannelType)
{
	CHECK_POINTER(pTargetDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pData, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_SUCCEED;
	switch(uiChannelType)
	{
	case DEVICE_TYPE_CAMERA:
	case DEVICE_TYPE_CAMERA_EX:
	case DEVICE_TYPE_SHADOW_CAMERA:
	case DEVICE_TYPE_CAMERA_AND_SHADOW:
		{
			// 解析xml
			CAMERA_BRIEF_INFO_ALL_VEC videoChannelVector;
			iRet = CDeviceMgrXMLProcess::ParseVideoChannelList(pTargetDomainCode, pData, iExDomainFlag, pageInfo, videoChannelVector);            
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process get user channel list rsp", "fail to parse xml.");
				return iRet;
			}

			uiItemSize = videoChannelVector.size();

			//插入到缓存中
			(void)VOS_MutexLock(m_pMutexCameraLock);
			CAMERA_BRIEF_INFO_ALL_VEC::iterator VideoIterBegin = videoChannelVector.begin();
			CAMERA_BRIEF_INFO_ALL_VEC::iterator VideoIterEnd = videoChannelVector.end();
			if (eM_Get_Device_All == iGetChannelType)
			{
				CAMERA_BRIEF_INFO_ALL_VEC::iterator IterEnd = m_vecAllCameraDev.end();
				m_vecAllCameraDev.insert(IterEnd, VideoIterBegin, VideoIterEnd);
			}
			else if (eM_Get_Device_ByDomainCode == iGetChannelType)
			{
				CAMERA_BRIEF_INFO_ALL_VEC::iterator IterEnd = m_vecAllCamDevByDom.end();
				m_vecAllCamDevByDom.insert(IterEnd, VideoIterBegin, VideoIterEnd);
			}
			(void)VOS_MutexUnlock(m_pMutexCameraLock);
		}
		
		break;
	case DEVICE_TYPE_EXTEND_ALARM:
	case DEVICE_TYPE_ALARM:
		{
			std::vector<IVS_ALARM_BRIEF_INFO> alarmChannelVector;

			iRet = CDeviceMgrXMLProcess::ParseAlarmChannelList(pTargetDomainCode, pData, iExDomainFlag, pageInfo, alarmChannelVector);            
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process get user channel list rsp", "fail to parse xml.");
				return iRet;
			}
			uiItemSize = alarmChannelVector.size();
			(void)VOS_MutexLock(m_pMutexAlarmLock);
			std::vector<IVS_ALARM_BRIEF_INFO>::iterator AlarmIterBegin = alarmChannelVector.begin();
			std::vector<IVS_ALARM_BRIEF_INFO>::iterator AlarmIterEnd = alarmChannelVector.end();
			if (eM_Get_Device_All == iGetChannelType)
			{
				ALARM_BRIEF_INFO_VEC::iterator IterEnd = m_vecAllAlarmDev.end();
				m_vecAllAlarmDev.insert(IterEnd, AlarmIterBegin, AlarmIterEnd);
			}
			else if (eM_Get_Device_ByDomainCode == iGetChannelType)
			{
				ALARM_BRIEF_INFO_VEC::iterator IterEnd = m_vecAllAlarmDevByDom.end();
				m_vecAllAlarmDevByDom.insert(IterEnd, AlarmIterBegin, AlarmIterEnd);
			}
			
			(void)VOS_MutexUnlock(m_pMutexAlarmLock);
		}
		break;
	default:
		;
	}
	return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::ProcessGetUserChannelListRsp(IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus, const IVS_CHAR* pData)
{

    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    std::string strSendDomainCode = channelListRetriveStatus.szTargetDomainCode;
    std::string strChannelType;
    switch (channelListRetriveStatus.uiChannelType)
    {
	case DEVICE_TYPE_CAMERA:
	case DEVICE_TYPE_CAMERA_EX:
	case DEVICE_TYPE_SHADOW_CAMERA:
	case DEVICE_TYPE_CAMERA_AND_SHADOW:
        strChannelType = "video channel";
        m_pUserMgr->GetDomainCode(strSendDomainCode);
        break;
    case DEVICE_TYPE_ALARM:
        strChannelType = "alarm channel";
        break;
    case DEVICE_TYPE_EXTEND_ALARM:
        strChannelType = "extend alarm channel";
        break;
    default:
        strChannelType = "invalid channel";
    }

    std:: string strExDomainFlag = (EXDOMAIN_FLAG_LOCAL == channelListRetriveStatus.iExDomainFlag) ? ("Local Domain") : ("ExDomain");

    if (IVS_SUCCEED != channelListRetriveStatus.iLastRetCode)
    {
        BP_RUN_LOG_ERR(channelListRetriveStatus.iLastRetCode, "process get user channel list rsp", 
            "request type:[%s], page index:[%u - %u], target domain:[%s : %s]", 
            strChannelType.c_str(), channelListRetriveStatus.uiFromIndex, channelListRetriveStatus.uiToIndex, 
            strExDomainFlag.c_str(), channelListRetriveStatus.szTargetDomainCode);

        // 当前请求返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);

        return channelListRetriveStatus.iLastRetCode;
    }

    IVS_PAGE_INFO pageInfo;
    eSDK_MEMSET(&pageInfo, 0, sizeof(pageInfo));
    IVS_UINT32 uiListSize = 0;
    long rspTime = clock();
    IVS_INT32 iRet = ParseChannelListRsp(pData, channelListRetriveStatus.uiChannelType, channelListRetriveStatus.iExDomainFlag, 
        channelListRetriveStatus.szTargetDomainCode, pageInfo, uiListSize, channelListRetriveStatus.iGetChannelType);

    if (IVS_SUCCEED != iRet)
    {
        // 当前XML解析返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);
		BP_RUN_LOG_ERR(iRet, "process get user channel list rsp failed", "target domaincode = %s, from = %d, to = %d", 
			channelListRetriveStatus.szTargetDomainCode, pageInfo.uiFromIndex, pageInfo.uiToIndex);
        return iRet;
    }

    BP_RUN_LOG_INF("process get user channel list rsp,",  
        "receive [%s] message, real num:[%u], page index:[%u - %u], xml item size:[%u], target domain:[%s : %s], rsp time interval:[%u],  xml parse time:[%u].", 
        strChannelType.c_str(),  pageInfo.uiRealNum, pageInfo.uiFromIndex, pageInfo.uiToIndex, uiListSize, strExDomainFlag.c_str(), 
        channelListRetriveStatus.szTargetDomainCode, (rspTime - channelListRetriveStatus.iSendTime), (clock() - rspTime));

    if (0 == pageInfo.uiRealNum || pageInfo.uiRealNum <= pageInfo.uiToIndex)
    {
        // 数据已经接收完毕
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, pageInfo.uiRealNum, eM_Thread_Retrive_Status_Ready, channelListRetriveStatus.iGetChannelType);
        return IVS_SUCCEED;
    }

    // 继续接收数据
    SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
        channelListRetriveStatus.iExDomainFlag, pageInfo.uiRealNum, eM_Thread_Retrive_Status_Retriving, channelListRetriveStatus.iGetChannelType);

    CXml reqReqXml;
    std::string strSendTransID = CMKTransID::Instance().GenerateTransID();
    IVS_INDEX_RANGE stRange;
    stRange.uiFromIndex = pageInfo.uiToIndex + 1;
    stRange.uiToIndex = pageInfo.uiToIndex + STEP_INDEX;
    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(channelListRetriveStatus.iExDomainFlag,  
        channelListRetriveStatus.szTargetDomainCode, channelListRetriveStatus.uiChannelType, stRange, reqReqXml);	

    BP_RUN_LOG_INF("process get user channel list rsp",   "start to send next page request, page index:[%u - %u].", 
        stRange.uiFromIndex, stRange.uiToIndex);

    iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strSendTransID, &reqReqXml);//lint !e64 匹配
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "process get user channel list rsp,", "fail to send message.");
        // 当前发送返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);
        return iRet;
    }

    IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo = channelListRetriveStatus;
    retriveStatusInfo.iSendTime = clock();
    CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
    if (NULL != pSingleSvrProxy)
    {
        pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strSendTransID, retriveStatusInfo);
    }
    return IVS_SUCCEED;
}

// 获取告警设备列表结构体赋值
IVS_INT32 CDeviceMgr::GetAlarmsInfo(IVS_UINT32 uiSize, IVS_ALARM_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange)
{
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	pList->uiTotal = m_vecAllAlarmDev.size();
	for(IVS_UINT32 i = 0; i < uiSize; i++)
	{
		IVS_ALARM_BRIEF_INFO tempAlarmInfo = m_vecAllAlarmDev.at(pQueryRange->uiFromIndex + i - 1);//lint !e64 匹配
		pList->stAlarmBriefInfo[i].uiChannelType = tempAlarmInfo.uiChannelType;
		pList->stAlarmBriefInfo[i].bIsExDomain = tempAlarmInfo.bIsExDomain;
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cAlarmCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cAlarmCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cAlarmName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cAlarmName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cParentCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}

	return IVS_SUCCEED;
}

// 获取主设备列表
IVS_INT32 CDeviceMgr::GetMainDeviceList(const IVS_CHAR* pDomainCode,
	const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, 
	IVS_DEVICE_BRIEF_INFO_LIST* pBuffer, 
	IVS_UINT32 uiBufferSize )
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryFormat, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    CXml xmlReq;
    IVS_QUERY_UNIFIED_FORMAT *pNoConst = const_cast<IVS_QUERY_UNIFIED_FORMAT*>(pQueryFormat);
    // 组装xml，准备发送给smu

    IVS_INT32 iRet = CXmlProcess::GetUnifiedFormatQueryXML(pNoConst, xmlReq, pDomainCode);  
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Main DeviceList fail","IVS_SUCCEED != iRet");
        return iRet;
    }

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_GET_MAIN_DEV_LIST_REQ, BUILSTRING, pDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CommSendMgr fail", "NA");
		return iRet;
	}

	iRet = CDeviceMgrXMLProcess::GetMainDeviceListParseXML(pBuffer, uiBufferSize, xmlReq);

    return iRet;
}

// 查询主设备详细信息
IVS_INT32 CDeviceMgr::GetDeviceInfo(const IVS_CHAR*  pDomainCode, const IVS_CHAR*  pDevCode,const IVS_DEVICE_INFO* pDeviceInfo)const
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDeviceInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    CXml xmlReq;
    (IVS_VOID)CDeviceMgrXMLProcess::GetDeviceInfoXML(pDomainCode, pDevCode, xmlReq);
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	IVS_INT32 iRet = CXmlProcess::GetCommBuildRet(xmlReq, NSS_GET_MAIN_DEV_INFO_REQ, BUILSTRING, strSMULinkID);

    return iRet;
}

IVS_INT32 CDeviceMgr::CheckBufferSize( const IVS_UINT32 &uiDevType, const IVS_UINT32 &uiNum, const IVS_UINT32& uiBufferSize)
{
	IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_UINT32 uiCheckBufferSize = 0;
	m_GetDeviceInfoType = static_cast<IVS_DEVICE_TYPE>(uiDevType);

    if (0 == uiNum)
    {
        return IVS_PARA_INVALID;
    }
    
    switch(m_GetDeviceInfoType)
    {
    case DEVICE_TYPE_MAIN:
        uiCheckBufferSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + sizeof(IVS_DEVICE_BRIEF_INFO) * (uiNum - 1);
        break;
    case DEVICE_TYPE_CAMERA:
		uiCheckBufferSize = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) + sizeof(IVS_CAMERA_BRIEF_INFO) * (uiNum - 1);
		break;
	case DEVICE_TYPE_CAMERA_EX:
		uiCheckBufferSize = sizeof(IVS_CAMERA_BRIEF_INFO_LIST_EX) + sizeof(IVS_CAMERA_BRIEF_INFO_EX) * (uiNum - 1);
		break;
    case DEVICE_TYPE_SHADOW_CAMERA:
	case DEVICE_TYPE_CAMERA_AND_SHADOW:
		uiCheckBufferSize = sizeof(IVS_SHADOW_CAMERA_BRIEF_INFO_LIST) + sizeof(IVS_SHADOW_CAMERA_BRIEF_INFO) * (uiNum - 1);  
        break;
	case DEVICE_TYPE_ALARM:
	case DEVICE_TYPE_EXTEND_ALARM:
		uiCheckBufferSize = sizeof(IVS_ALARM_BRIEF_INFO_LIST) + sizeof(IVS_ALARM_BRIEF_INFO) * (uiNum - 1);       
		break;
	case DEVICE_TYPE_VOICE:
		uiCheckBufferSize = sizeof(IVS_AUDIO_BRIEF_INFO_LIST) + sizeof(IVS_AUDIO_BRIEF_INFO) * (uiNum - 1);       
		break;
    default:
        iRet = IVS_PARA_INVALID;
    }

    // 判断用户空间是否足够大
    if (uiCheckBufferSize > uiBufferSize && IVS_SUCCEED == iRet)
    {
		iRet = IVS_SDK_RET_MEMORY_CHECK_ERROR;
    }

    return iRet;
}

// 根据主设备查询子设备列表
IVS_INT32 CDeviceMgr::GetChannelList( const IVS_CHAR* pDevCode, IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList, IVS_UINT32 uiBufferSize )const
{
    CHECK_POINTER(pChannelList,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiReqNum = (pChannelList->stIndexRange.uiToIndex - pChannelList->stIndexRange.uiFromIndex) + 1;
    IVS_UINT32 uiBufferNum = (uiBufferSize - sizeof(IVS_DEV_CHANNEL_BRIEF_INFO_LIST))/sizeof(IVS_DEV_CHANNEL_BRIEF_INFO) + 1;

    // 通道最大的数量是256
    if(uiReqNum != uiBufferNum || uiReqNum > IVS_MAX_DEVICE_CHANNEL_NUM)
    {        
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ChannelList fail", "uiReqNum=%u,uiBufferNum=%u", uiReqNum, uiBufferNum);
        return IVS_OPERATE_MEMORY_ERROR;
    }

    IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];

    (IVS_VOID)CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
    CXml xmlTemp;
    (IVS_VOID)CDeviceMgrXMLProcess::GetChannelListGetXML(chDomaCode, chDevCode, xmlTemp);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlTemp, m_pUserMgr, NSS_GET_CHANNEL_INFO_REQ, BUILSTRING, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
		return iRet;
	}

	iRet = CDeviceMgrXMLProcess::GetChannelListParseXML(xmlTemp, pChannelList);

    return iRet;
}

// 修改主设备名称
IVS_INT32 CDeviceMgr::SetMainDeviceName(const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName)const
{
	CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewDevName,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 根据pDevCode解析出域号和设备号
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);

	CXml xmlReq;
	(IVS_VOID)CDeviceMgrXMLProcess::SetMainDeviceNameGetXML(chDomaCode,chDevCode,pNewDevName,xmlReq);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_SET_MAIN_DEV_NAME_REQ, BUILDRET, chDomaCode);

	return iRet;
}

// 修改摄像机名称
IVS_INT32 CDeviceMgr::SetCameraName(const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName)const
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewCameraName,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

	// 根据pCameraCode解析出域号和设备号
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	// 组装请求xml
	CXml xmlReq;
	(IVS_VOID)CDeviceMgrXMLProcess::SetCameraNameGetXML(chDomaCode,chDevCode,pNewCameraName,xmlReq);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_SET_CAMERA_NAME_REQ, BUILDRET, chDomaCode);

	return iRet;
}

// 摄像机信息里面的strCameraCode是带域编码，查询的时候传入的可以是带域编码的 也可以是不带域编码的，默认是不带域编码
IVS_INT32 CDeviceMgr::GetCameraBriefInfobyCode(const std::string& strCameraCode, IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo, bool bHasDomainCode/* = false*/)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    // 根据设备编码查找摄像机信息
	IVS_UINT32 uiSize = m_vecAllCameraDev.size();
	for(IVS_UINT32 i = 0; i < uiSize; i++)
	{
		IVS_CAMERA_BRIEF_INFO_ALL tempCamera = m_vecAllCameraDev.at(i);//lint !e64 匹配
		// 这个是带域编码
        IVS_CHAR cTempCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
        (void)CToolsHelp::Memcpy(cTempCameraCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN);
		std::string strDstCameraCode = cTempCameraCode; 
		std::string strCmpCameraCode;
		if (bHasDomainCode)
		{
			strCmpCameraCode = strDstCameraCode;
		}
		else
		{
			IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
			IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
			(void)CXmlProcess::ParseDevCode(strDstCameraCode.c_str(), chDevCode, chDomaCode);//lint !e64 匹配
			// 这个是不带域编码的
			strCmpCameraCode = chDevCode; 
		}
		if (strCameraCode.compare(strCmpCameraCode) == 0)
		{
			stCameraBriefInfo.bEnableVoice = tempCamera.bEnableVoice;
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cCameraLocation, IVS_DESCRIBE_LEN, tempCamera.cCameraLocation, IVS_DESCRIBE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cCameraLocation, IVS_DESCRIBE_LEN, tempCamera.cCameraLocation, IVS_DESCRIBE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cName, IVS_CAMERA_NAME_LEN, tempCamera.cName, IVS_CAMERA_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cName, IVS_CAMERA_NAME_LEN, tempCamera.cName, IVS_CAMERA_NAME_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cNvrCode, IVS_NVR_CODE_LEN, tempCamera.cNvrCode, IVS_NVR_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cNvrCode, IVS_DEV_CODE_LEN, tempCamera.cNvrCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cParentCode, IVS_DEV_CODE_LEN, tempCamera.cParentCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cParentCode, IVS_DEV_CODE_LEN, tempCamera.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cReserve, 32, tempCamera.cReserve, 32))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cReserve, 32, tempCamera.cReserve, 32) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCamera.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCamera.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			stCameraBriefInfo.uiCameraStatus = tempCamera.uiCameraStatus;
			//stCameraBriefInfo.uiGroupID = tempCamera.uiGroupID;
			stCameraBriefInfo.uiStatus = tempCamera.uiStatus;
			stCameraBriefInfo.uiType = tempCamera.uiType;
			iRet = IVS_SUCCEED;
			break;
		}
	}	

    return iRet;
}

// 根据域编码找到第一个NVR，并将其移到列表尾部
IVS_INT32 CDeviceMgr::GetFirstNVR(const std::string& strDomainCode, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo)
{
	IVS_DEBUG_TRACE("");

	(void)VOS_MutexLock(m_pMutexNVRLock);
	// 根据DomainCode找到DEVICE_BRIEF_INFO_LIST
	NVR_INFO_MAP_ITOR IpIter;
	IpIter = m_NvrInfoMap.find(strDomainCode);

	if (m_NvrInfoMap.end() == IpIter || IpIter->second.empty())
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get First NVR", "nvr in this domain  = NULL");
		(void)VOS_MutexUnlock(m_pMutexNVRLock);
		return IVS_OPERATE_MEMORY_ERROR;
	}

	//取队列的第一个数据;
	IVS_DEVICE_BRIEF_INFO stDeviceBriefInfo = *(IpIter->second.begin());//lint !e64 匹配
	stDeviceBasicInfo = stDeviceBriefInfo.stDeviceInfo;

	//将第一个移出插入到队列的尾部;
	IpIter->second.pop_front();
	IpIter->second.push_back(stDeviceBriefInfo);

	(void)VOS_MutexUnlock(m_pMutexNVRLock);

	return IVS_SUCCEED;
}

// 根据域编码获取list长度
IVS_UINT32 CDeviceMgr::GetNVRSize(const std::string& strDomainCode)
{
	IVS_UINT32 iSize = 0;
	// TODO 通用请求; zhouyunping
	(void)VOS_MutexLock(m_pMutexNVRLock);

	NVR_INFO_MAP_ITOR IpIter = m_NvrInfoMap.find(strDomainCode);

	if (m_NvrInfoMap.end() != IpIter)
	{
		iSize = IpIter->second.size();
	}

	(void)VOS_MutexUnlock(m_pMutexNVRLock);
	return iSize;
}

IVS_INT32 CDeviceMgr::UpdateNVRListToCache(const IVS_CHAR* pDomainCode, std::list<IVS_DEVICE_BRIEF_INFO>& nvrInfoList, const bool bCacheClearFlag)
{
    (void)VOS_MutexLock(m_pMutexNVRLock);
    NVR_INFO_MAP_ITOR IpIter = m_NvrInfoMap.find(pDomainCode);

    if (m_NvrInfoMap.end() == IpIter)
    {
        (void)m_NvrInfoMap.insert(make_pair(pDomainCode, nvrInfoList));
    }
    else
    {
        if (bCacheClearFlag)
        {
            IpIter->second.clear();
        }
        IpIter->second.splice(IpIter->second.end(), nvrInfoList);
    }
    (void)VOS_MutexUnlock(m_pMutexNVRLock);
    return IVS_SUCCEED;
}

// 查询网元列表
IVS_INT32 CDeviceMgr::GetNVRList(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType,const IVS_INDEX_RANGE* pIndex,IVS_DEVICE_BRIEF_INFO_LIST* pNvrList, IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNvrList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("pDomainCode: %s,addr:%d",pDomainCode, &pDomainCode);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = CDeviceMgrXMLProcess::GetNVRListReqXML(pDomainCode,pIndex,uiServerType,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get NVR List fail","Get NVR List Req XML failed");
		return iRet;
	}

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_GET_NVR_LIST_REQ, BUILSTRING, pDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "GetNVRList fail","Send Cmd failed");
        return iRet;
    }

	//解析查询请求XML
	iRet = CDeviceMgrXMLProcess::ParseNVRListRsp(xmlReq, pNvrList, uiBufferSize);

    NVR_INFO_LIST nvrInfoList;
    for (IVS_UINT32 i = 0; i < pNvrList->uiTotal; i++)
    {
        nvrInfoList.push_back(pNvrList->stDeviceBriefInfo[i]);
    }
    
    // 只更新NVR
    if (2 != uiServerType)
    {
        if (1 == pIndex->uiFromIndex)
        {
            (void)UpdateNVRListToCache(pDomainCode, nvrInfoList, true);
        }
        else
        {
            (void)UpdateNVRListToCache(pDomainCode, nvrInfoList, false);
        }
    }

	return iRet;
}

IVS_INT32 CDeviceMgr::GetClusterList(const IVS_CHAR* pDomainCode,  IVS_CLUSTER_INFO_LIST* pClusterList)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pClusterList, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::GetClusterInfoListReqXML(pDomainCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Cluster List fail","Get Cluster List Req XML failed");
		return iRet;
	}

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_GET_CLUSTER_LIST_REQ, BUILSTRING, pDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetClusterList fail","Send Cmd failed");
		return iRet;
	}

	// 解析响应XML
	(void)CDeviceMgrXMLProcess::ParseClusterListRsp(xmlReq, pClusterList);

	// 保存解析结果到内存中
	for (IVS_UINT32 i = 0; i < pClusterList->uiTotal; i++)
	{
		(void)m_CLusterInfoMap.insert(std::make_pair(pClusterList->stClusterInfo[i].cClusterCode, pClusterList->stClusterInfo[i]));
	}
	return 0; //lint !e438
}

// 获取所有的NVR消息
IVS_INT32 CDeviceMgr::GetAllNVRInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType)
{
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	IVS_INDEX_RANGE stIndex;
	eSDK_MEMSET(&stIndex,0,sizeof(IVS_INDEX_RANGE));
	stIndex.uiFromIndex = 1;
	stIndex.uiToIndex = QUERY_INDEX_COUNT;
	
    //为结果LIST分配内存
    IVS_UINT32 uiBufSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + (QUERY_INDEX_COUNT - 1) * sizeof(IVS_DEVICE_BRIEF_INFO);
    IVS_CHAR* pNVRListBuf = IVS_NEW(pNVRListBuf, uiBufSize);
    CHECK_POINTER(pNVRListBuf, IVS_OPERATE_MEMORY_ERROR);
    eSDK_MEMSET(pNVRListBuf, 0, uiBufSize);
    IVS_DEVICE_BRIEF_INFO_LIST* pNvrList = reinterpret_cast<IVS_DEVICE_BRIEF_INFO_LIST*>(pNVRListBuf);//lint !e826
	IVS_BOOL bIsQueryContinnue = IVS_FAIL;

	do 
	{
		iRet = GetNVRList(pDomainCode, uiServerType, &stIndex, pNvrList, uiBufSize);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Get All NVR Info","Query failed");
			IVS_DELETE(pNVRListBuf, MUILI);
			return iRet;
		}

		//如果查询回来的总个数比请求结束个数小，终止查询//
		if (pNvrList->uiTotal <= stIndex.uiToIndex)
		{
			bIsQueryContinnue = IVS_SUCCEED;
			IVS_DELETE(pNVRListBuf, MUILI);
			break;
		}
		else
		{
			//否则索引向后移继续查询
	//		ulQueryCount++;
			stIndex.uiFromIndex += QUERY_INDEX_COUNT;
			stIndex.uiToIndex += QUERY_INDEX_COUNT;
		}
		IVS_DELETE(pNVRListBuf, MUILI);
	} while (!bIsQueryContinnue);

	return iRet;//lint !e438
}


// 根据域编码和NVRCode获取NVR信息
IVS_INT32 CDeviceMgr::GetNVRInfoByDomainCodeAndNVRCode(const std::string& strDomainCode,const std::string& strNVRCode,IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo)
{
	IVS_DEBUG_TRACE("mapsize[%d]", m_NvrInfoMap.size());

	(void)VOS_MutexLock(m_pMutexNVRLock);

	// 根据DoaminCode查询Map
	NVR_INFO_MAP_ITOR IpIter = m_NvrInfoMap.find(strDomainCode.c_str());

	if (IpIter ==m_NvrInfoMap.end() || IpIter->second.empty())
	{
		//add by zwx211831 没有找到就获取一下NVRlist，再查找
		(void)VOS_MutexUnlock(m_pMutexNVRLock);	//该锁在GetAllNVRInfo内部会被再次用到，这里先释放
		IVS_INT32 iRet = GetAllNVRInfo(strDomainCode.c_str(), 0);//lint !e64 匹配
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get NVR Info By Domain Code And NVR Code", "GetAllNVRInfo failed, DomainCode: %s.", strDomainCode.c_str());
			return iRet;
		}

		(void)VOS_MutexLock(m_pMutexNVRLock);
		IpIter = m_NvrInfoMap.find(strDomainCode.c_str());
		if (IpIter ==m_NvrInfoMap.end() || IpIter->second.empty())
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get NVR Info By Domain Code And NVR Code", "pDeviceBriefInfoList = NULL.");
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return IVS_OPERATE_MEMORY_ERROR;
		}
	}

	NVR_INFO_LIST_ITOR NvrInfoLisrItor = IpIter->second.begin();

	for (; NvrInfoLisrItor != IpIter->second.end(); NvrInfoLisrItor++)
	{
		if (0 == strNVRCode.compare(NvrInfoLisrItor->stDeviceInfo.cCode))
		{
			// 拷贝内容
			stDeviceBasicInfo = NvrInfoLisrItor->stDeviceInfo;
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return IVS_SUCCEED;
		}
	}
	//解锁
	(void)VOS_MutexUnlock(m_pMutexNVRLock);

	return IVS_FAIL;
}

// 根据域编码修改设备状态
IVS_INT32 CDeviceMgr::ModifyDevStatusByDomainCode(const std::string& /*strDomainCode*/, const std::string& strCameraCode, const IVS_UINT32& uiStatus, const std::string& strNVRCode)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	// 加锁
	(void)VOS_MutexLock(m_pMutexCameraLock);

	// 根据设备编码查找摄像机信息
	IVS_UINT32 uisize = m_vecAllCameraDev.size();
	for(IVS_UINT32 i = 0;i<uisize;i++)
	{
		IVS_CAMERA_BRIEF_INFO_ALL tempCamera = m_vecAllCameraDev.at(i);//lint !e64 匹配
		// 这个是带域编码
		std::string strDstCameraCode = tempCamera.cCode; 

		IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
		IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
		(void)CXmlProcess::ParseDevCode(strDstCameraCode.c_str(), chDevCode, chDomaCode);//lint !e64 匹配
		// 这个是不带域编码的
		std::string strCmpCameraCode = chDevCode; 

		if (strCameraCode.compare(strCmpCameraCode) == 0)
		{
			m_vecAllCameraDev.at(i).uiStatus = uiStatus;
			eSDK_MEMSET(m_vecAllCameraDev.at(i).cNvrCode, 0x0,IVS_NVR_CODE_LEN);
			if (!CToolsHelp::Memcpy(m_vecAllCameraDev.at(i).cNvrCode, IVS_NVR_CODE_LEN, strNVRCode.c_str(), IVS_NVR_CODE_LEN))//lint !e64 匹配
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(m_vecAllCameraDev.at(i).cNvrCode, IVS_NVR_CODE_LEN, strNVRCode.c_str(), IVS_NVR_CODE_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRet = IVS_SUCCEED;
			break;
		}
	}
	
	// 解锁
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	return iRet;
}

// 根据域编码干掉对应的列表
IVS_INT32 CDeviceMgr::DeleteNVRListByDomainCode(const std::string& strDomainCode)
{
	IVS_DEBUG_TRACE("");

	(void)VOS_MutexLock(m_pMutexNVRLock);

    NVR_INFO_MAP_ITOR IpIter = m_NvrInfoMap.find(strDomainCode);

    if (m_NvrInfoMap.end() != IpIter)
    {
        IpIter->second.clear();
        m_NvrInfoMap.erase(IpIter);
    }
	(void)VOS_MutexUnlock(m_pMutexNVRLock);
	return IVS_SUCCEED;
}

// 重启前端设备
IVS_INT32 CDeviceMgr::RestartDevice(const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType)const
{
	CHECK_POINTER(pDeviceCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pDeviceCode, chDevCode, chDomaCode);

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::RestartDeviceGetXML(chDomaCode, chDevCode, uiRebootType, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Restart Device GetXML fail","IVS_SUCCEED != RestartDevice");
		return iRet;
	}

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_DEVICE_REBOOT_REQ, BUILDRET, chDomaCode);

	return iRet;
}

IVS_INT32 CDeviceMgr::FindIdlestNvr(const IVS_CHAR* pDomainCode, const std::string& strClusterCode, std::string& strIdleNvrCode)const
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

    CXml xml;
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ClusterCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(strClusterCode.c_str());//lint !e64 匹配  
    xml.OutOfElem();

    IVS_UINT32 uiLen = 0;
    const IVS_CHAR* pReqXml = xml.GetXMLStream(uiLen);
    if (NULL== pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to create request xml for finding idlest nvr, cluster code:%s", strClusterCode.c_str());
        return IVS_XML_INVALID;
    }

    std::string strRspXml;
    IVS_INT32 iRet = m_pUserMgr->SendMsg(pDomainCode, NSS_CLUSTER_FIND_IDLEST_NVR_REQ, pReqXml, strRspXml, NULL);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "start search device", "fail to send message for finding idlest nvr, cluster code:%s", strClusterCode.c_str());
        return iRet;
    }

    CXml xmlRsp;
    if (!xmlRsp.Parse(strRspXml.c_str()))//lint !e64 匹配
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to parse find idle nvr response xml, cluster code:%s", strClusterCode.c_str());
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* pNvrCode = xmlRsp.GetElemValueEx("Content/NVRInfo/NVRCode");
    if (NULL == pNvrCode)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to find nvr code in response xml, cluster code:%s", strClusterCode.c_str());
        return IVS_XML_INVALID;
    }
    strIdleNvrCode = pNvrCode;
    return IVS_SUCCEED;
}

IVS_INT32 CDeviceMgr::StartSearchDeviceEX(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
    CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

    CXml xml;
    if (!xml.Parse(pSearchInfo))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to parse request xml, input xml string:%s", pSearchInfo);
        return IVS_XML_INVALID;
    }
    std::string strServerCode;
    IVS_INT32 iRet = CDeviceMgrXMLProcess::GetServerCodeParseXML(xml, "Content/SearchInfo/ServerCode" , strServerCode);
    if (IVS_SUCCEED != iRet)
    {
        return iRet;
    }

    std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		iRet = CDeviceMgrXMLProcess::GetDomainCodeParseXML(xml, "Content/SearchInfo/DomainCode" , strDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to parse DomainCode from xml, input xml string:%s", pSearchInfo);
			m_pUserMgr->GetDomainCode(strDomainCode);
		}
	}
	else
	{
		strDomainCode = pDomainCode;
	}
    
    std::string strComplexCode;
    IVS_UINT32 uiNvrMode = GetNVRMode(strDomainCode.c_str(), strServerCode, strComplexCode);//lint !e64 匹配
    if (NVR_MODE_CLUSTER == uiNvrMode)
    {
        iRet = FindIdlestNvr(strDomainCode.c_str(), strServerCode, strComplexCode);//lint !e64 匹配
        if (IVS_SUCCEED != iRet)
        {
            return iRet;
        }
        const IVS_CHAR* pAddr = "Content/SearchInfo/ServerCode";
        iRet = CXmlProcess::ModifyNodeValueParseXML(xml, pAddr, strComplexCode);
        if (IVS_SUCCEED != iRet)
        {
            return iRet;
        }
    }
    m_pUserMgr->SetDevServerCode(strComplexCode);
    m_pUserMgr->SetDevDomainCode(strDomainCode);

    IVS_UINT32 uiLen = 0;
    const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
    if (NULL == pReq)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to create request xml, input xml string:%s", pSearchInfo);
        return IVS_XML_INVALID;
    }
    std::string strRspXml;

    iRet = m_pUserMgr->SendMsg(strDomainCode.c_str(), NSS_START_SEARCH_DEVICE_REQ, pReq, strRspXml, NULL);//lint !e64 匹配

	return iRet;
}

// 启动搜索前端设备
IVS_INT32 CDeviceMgr::StartSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	IVS_DOMAIN_ROUTE stDomainRoute;
	CCmd *pCmd = NULL;
	CXml xml;
	if (!xml.Parse(pSearchInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
    
	// 获取指定域SMU连接
    std::string strSMULinkID = "";
	IVS_INT32 iRet = m_pUserMgr->GetSMULinkID(strSMULinkID, strDomainCode);//lint !e64 匹配
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get SMU LinkID failed", "NA");
		return iRet;
	}
	const IVS_CHAR* pAddp = "Content/SearchInfo/ServerCode";
	iRet = CDeviceMgrXMLProcess::SearchDeviceParseXML(xml, pAddp);
	if (IVS_SUCCEED != iRet)
	{
		CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
		(void)oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
		std::string strServerCode;
		if (1 == stDomainRoute.uiDomainType)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "ServerCode is NULL", "ServerCode fail");
			return IVS_XML_INVALID;
		}
		if (2 == stDomainRoute.uiDomainType)
		{
			// 构造带域的请求消息，并发送
			CSendNssMsgInfo sendNssMsgInfo;
			sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
			sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
			sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_IDLEST_NVR_REQ);
			sendNssMsgInfo.SetReqData("");	
			sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
			std::string strpRsp;
			IVS_INT32 iNeedRedirect = IVS_FAIL;
			iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
				return iRet;
			}
			CXml pRsqXml;
			if (!pRsqXml.Parse(strpRsp.c_str()))//lint !e64 匹配
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
				return IVS_XML_INVALID;
			}
			iRet = CDeviceMgrXMLProcess::FindIdlestNvrParseXML(strServerCode, pRsqXml);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "Start Search Device", "Find Id lest Nvr ParseXML failed");
				return iRet;
			}
		}
		m_pUserMgr->SetDevServerCode(strServerCode);
		const IVS_CHAR* pAddr = "Content/SearchInfo/ServerCode";
		iRet = CDeviceMgrXMLProcess::ModifyServerCode(xml, strServerCode, pAddr);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Start Search Device", "Modify ServerCode failed");
			return iRet;
		}
		IVS_UINT32 uiLen = 0;
		const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_START_SEARCH_DEVICE_REQ, pReq, strSMULinkID);
	}
	else
	{
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_START_SEARCH_DEVICE_REQ, pSearchInfo, strSMULinkID);
	}
		
	CHECK_POINTER(pCmd,IVS_OPERATE_MEMORY_ERROR);

	m_pUserMgr->SetDevTransID(pCmd->GetTransID());//lint !e1013 !e1055 !e746 !e64
	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 停止搜索前端设备
IVS_INT32 CDeviceMgr::StopSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)const
{
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

    // 获取指定域SMU连接
    std::string strSMULinkID = "";
	IVS_INT32 iRet = m_pUserMgr->GetSMULinkID(strSMULinkID, strDomainCode);//lint !e64 匹配
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get SMU LinkID failed", "NA");
		return iRet;
	}
	std::string strTransID;
	std::string strServerCode;
	m_pUserMgr->GetDevTransID(strTransID);
	m_pUserMgr->GetDevServerCode(strServerCode);

	CXml xml;
	if (!xml.Parse(pSearchInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
	// 拼装发送报文
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strSMULinkID, NET_ELE_SMU_NSS, NSS_STOP_SEARCH_DEVICE_REQ, pReq, strTransID);//lint !e64 匹配
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

IVS_INT32 CDeviceMgr::VerifyDeviceEX(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CXml xml;
	if (!xml.Parse(pVerifyInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "verify device", "request xml is valid, fail to parse");
		return IVS_XML_INVALID;
	}

	std::string strServerCode;
	IVS_INT32 iRet = CXmlProcess::GetNodeValueParseXML(xml, "Content/ValidateInfo/ServerCode", strServerCode);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		iRet = CDeviceMgrXMLProcess::GetDomainCodeParseXML(xml, "Content/ValidateInfo/DomainCode" , strDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "start search device", "fail to parse DomainCode from xml");
			m_pUserMgr->GetDomainCode(strDomainCode);
		}
	}
	else
	{
		strDomainCode = pDomainCode;
	}
	

	std::string strComplexCode;
	IVS_UINT32 uiNvrMode = GetNVRMode(strDomainCode.c_str(), strServerCode, strComplexCode);//lint !e64 匹配

	CSDKSecurityClient oSecurityClient;
	std::string strLinkID;
	if (NVR_MODE_CLUSTER == uiNvrMode)
	{
		iRet = FindIdlestNvr(strDomainCode.c_str(), strServerCode, strComplexCode);//lint !e64 匹配
		if (IVS_SUCCEED != iRet)
		{
			return iRet;
		}
		iRet = CXmlProcess::ModifyNodeValueParseXML(xml, "Content/ValidateInfo/ServerCode", strComplexCode);
		if (IVS_SUCCEED != iRet)
		{
			return iRet;
		}
		//iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, 
		//    strDomainCode.c_str(), strLinkID, "CMU", "", strComplexCode.c_str());

	}

	iRet = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, 
		strDomainCode.c_str(), strLinkID, "SCU", "", strComplexCode.c_str());


	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk verify device", "fail to exchange key, complex code:%s, domain code:%s",
			strComplexCode.c_str(), strDomainCode.c_str());
		return iRet;
	}
	std::string strDevPwd;
	iRet = CXmlProcess::GetNodeValueParseXML(xml, "Content/ValidateInfo/DevPWD", strDevPwd);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk verify device", "fail to get password from request xml, complex code:%s, domain code:%s",
			strComplexCode.c_str(), strDomainCode.c_str());
		return iRet;
	}
	std::string strDevPWDCipher;
	iRet = oSecurityClient.EncryptString(strDevPwd.c_str(), strDevPWDCipher);//lint !e64 匹配
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "sdk verify device", "failed to encrypt string, complex code:%s, domain code:%s",
			strComplexCode.c_str(), strDomainCode.c_str());
		return IVS_DATA_ENCRYPT_ERROR;
	}

	iRet = CXmlProcess::ModifyNodeValueParseXML(xml, "Content/ValidateInfo/DevPWD", strDevPWDCipher);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "sdk verify device", "failed to modify encrypy node, complex code:%s, domain code:%s",
			strComplexCode.c_str(), strDomainCode.c_str());
		return iRet;
	}


	std::string strSecurityTransID = oSecurityClient.GetSecurityDataTransID();

	iRet = CXmlProcess::GetEncrypSendMgr(xml, m_pUserMgr, NSS_VALIDATE_DEVICE_REQ, BUILSTRING, strDomainCode.c_str(), strLinkID, strSecurityTransID);//lint !e64 匹配

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk verify device", "fail to verify device, complex code:%s, domain code:%s", strComplexCode.c_str(), strDomainCode.c_str());
		return iRet;
	}
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pRspString = xml.GetXMLStream(xmlLen);
	if (NULL != pRspString)
	{
		*pRspXml = IVS_NEW(*pRspXml, xmlLen + 1);
		CHECK_POINTER(*pRspXml, IVS_OPERATE_MEMORY_ERROR);
		eSDK_MEMSET(*pRspXml, 0, xmlLen + 1);
		(void)CToolsHelp::Strncpy(*pRspXml, xmlLen + 1, pRspString, strlen(pRspString));//lint !e587
	}

	return IVS_SUCCEED;
}

// 验证前端设备
IVS_INT32 CDeviceMgr::VerifyDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");


	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}
	CXml xml;
	if (!xml.Parse(pVerifyInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlCode.Parse(pVerifyInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElemEx("Content/ValidateInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlCode.FindElemEx(Content/ValidateInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pTemp = NULL;
	IVS_CHAR cServerCode[IVS_NVR_CODE_LEN + 1] = {0};
	GET_ELEM_VALUE_CHAR("ServerCode", pTemp, cServerCode, IVS_NVR_CODE_LEN, xml);//lint !e838

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	IVS_INT32 iRet = oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Domain Route Info By Code fail", "NA");
		return iRet;
	}
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		iRet = VerifyDeviceMsg(strDomainCode.c_str(), cServerCode, pVerifyInfo, pRspXml, BP::DOMAIN_CLUSTER);//lint !e64 匹配

		return iRet;
	}
	// 堆叠模式、单机模式
	iRet = VerifyDeviceMsg(strDomainCode.c_str(), cServerCode, pVerifyInfo, pRspXml);//lint !e64 匹配

	return iRet;
}


// 验证前端设备(集群、堆叠)
IVS_INT32 CDeviceMgr::VerifyDeviceMsg(const IVS_CHAR* pDomainCode, const IVS_CHAR* pServerCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml, enum BP::DOMAIN_TYPE enDomainType)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pServerCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	if (!xml.Parse(pVerifyInfo))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.Parse(pVerifyInfo) fail", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	/************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
	std::string strDomainCode = pDomainCode;
	std::string strServerCode;
	// 若是集群则寻找最空闲NVR
	if (0 == strcmp(pServerCode, ""))
	{
		// 构造带域的请求消息，并发送
		CSendNssMsgInfo sendNssMsgInfo;
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_IDLEST_NVR_REQ);
		sendNssMsgInfo.SetReqData("");	
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		std::string strpRsp;
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
			return iRet;
		}
		CXml pRsqXml;
		if (!pRsqXml.Parse(strpRsp.c_str()))//lint !e64 匹配
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
			return IVS_XML_INVALID;
		}
		iRet = CDeviceMgrXMLProcess::FindIdlestNvrParseXML(strServerCode, pRsqXml);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "FindIdlestNvrParseXML fail", "NA");
			return iRet;
		}
		const IVS_CHAR* pAddr = "Content/ValidateInfo/ServerCode";
		iRet = CDeviceMgrXMLProcess::ModifyServerCode(xml, strServerCode, pAddr);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ModifyServerCode fail", "NA");
			return iRet;
		}
	}
	else
	{
		strServerCode = pServerCode;
	}
	// 初始化密钥交换类对象,完成密钥交换
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, "SCU", "", strServerCode.c_str(), enDomainType);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "VerifyDevice fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
	
	if (!xml.FindElemEx("Content/ValidateInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/ValidateInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR cDevPWD[IVS_PWD_LEN + 1] = {0};
	std::string strDevPWDCipher;
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("DevPWD", pTemp, cDevPWD, IVS_PWD_LEN, xml);//lint !e838
	if (0 != strcmp(cDevPWD, ""))
	{
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPWD, strDevPWDCipher))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "VerifyDevice fail", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
		if (!xml.FindElem("DevPWD"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevPWD) fail", "NA");
			return IVS_XML_INVALID;
		}
		xml.ModifyElemValue(strDevPWDCipher.c_str());//lint !e64 匹配
	}
	
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_VALIDATE_DEVICE_REQ, pReq, strSecurityTransID);//lint !e64 匹配
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	
	// 返回解析报文错误码
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
};


// 查询前端日志
IVS_INT32 CDeviceMgr::GetDevLog(const IVS_CHAR* pQueryXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pQueryXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	CXml xmlReq;
	IVS_UINT32 xmlLen = 0;
	IVS_INT32 iRet = IVS_SUCCEED;
	if (!xmlReq.Parse(pQueryXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pQueryXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	// 根据Subscriber节点值获取域编码
	if (xmlReq.FindElemEx("Content/PageInfo/QueryCond"))
	{
		if (!xmlReq.FindElem("QueryField"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElem(QueryFiel) fail", "NA");
			return IVS_XML_INVALID;
		}

		do 
		{
			(void)xmlReq.IntoElem();
			if (!xmlReq.FindElem("Field"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElem(Field) fail", "NA");
				return IVS_XML_INVALID;
			}
			const IVS_CHAR* pDevCode = xmlReq.GetElemValue();

            if (NULL == pDevCode)
            {
                pDevCode = "";
            }
			if (0 == strcmp(pDevCode, "DevCode"))
			{
				if (!xmlReq.FindElem("Value"))
				{
					BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsq.FindElem(Value) error", "NA");
					return IVS_XML_INVALID;
				}
				std::string strBigCode;
               
                const IVS_CHAR* pBigCode = xmlReq.GetElemValue();
                if (NULL != pBigCode)
                {
                    strBigCode = pBigCode;
                }

				// 解析DevCode
				iRet = CXmlProcess::ParseDevCode(strBigCode.c_str(), chDevCode, chDomaCode);//lint !e64 匹配
				if (IVS_SUCCEED != iRet)
				{
					BP_RUN_LOG_ERR(iRet, "Get DevLog fail", "(ParseDevCode)iRet=%d", iRet);
					return iRet;
				}
				if (!strBigCode.empty())
				{
					xmlReq.ModifyElemValue(chDevCode);
					break;
				}
			}
			xmlReq.OutOfElem();
		} while (xmlReq.NextElem());
	}
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }


	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEV_LOG_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	
	IVS_CHAR* pRsp = IVS_NEW((IVS_CHAR*&)pRsp, strpRsp.size() + 1);
	CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(pRsp, 0x0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(pRsp, strpRsp.size(), strpRsp.c_str(), strpRsp.size()))//lint !e64 匹配
	{
		IVS_DELETE(pRsp, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(*pRspXml, strpRsp.size(), strpRsp.c_str(), strpRsp.size()) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	*pRspXml = pRsp;

	return iRet;
}

// 查询当前用户的手动录像任务
IVS_INT32 CDeviceMgr::GetRecordTask(const IVS_UINT32 uiUserId,CDeviceMgr &m_DeviceMgr,IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_UINT32 xmlLen = 0;
	std::string strDomainCode;
	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	std::string strResult = "<Content><CameraList>";
	const IVS_CHAR* szElemValue = NULL;

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_TASK_REQ);

	m_vecCameraDomain.clear();
	// 取得用户拥有设备的域列表
	IVS_INT32 iRet = m_DeviceMgr.GetDomainListByUserID(m_DeviceMgr.m_vecCameraDomain);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainList By UserID return failed", "NA");
		return iRet;
	}

	// 查询所有域的摄像机手动录像状态
	IVS_UINT32 iSize = m_DeviceMgr.m_vecCameraDomain.size();	
	IVS_UINT32 uiRecordMethod = 0;
    std::string strUserDomainCode;
	m_pUserMgr->GetDomainCode(strUserDomainCode);

	bool bHaveOneSuc = false;
	// 遍历域编码列表
	for(IVS_UINT32 i=0;i<iSize;i++)
	{
		CXml xmlReq;
		CXml xmlRsp;
		std::string strpRsp;
		strDomainCode = m_DeviceMgr.m_vecCameraDomain.at(i);	
		iRet = CDeviceMgrXMLProcess::GetRecordTaskGetXML(uiUserId,strUserDomainCode.c_str(),strDomainCode.c_str(),xmlReq);//lint !e64 匹配
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Get Record Task:ReqXML is failed","failed");
			return iRet;
		}
		
		const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);

        if (NULL == pReq)
        {
            return IVS_XML_INVALID;
        }

		sendNssMsgInfo.SetReqData(pReq); 
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get Record Task From One Domain failed", "NA");
			continue;
		}
		// 若解析xml失败
		if (xmlRsp.Parse(strpRsp.c_str()))//lint !e64 匹配
		{
			if(xmlRsp.FindElemEx("Content/CameraList"))
			{
				do 
				{
					xmlRsp.IntoElem();
					(void)xmlRsp.IntoElem();
					GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,chCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
					GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,uiRecordMethod,xmlRsp);
					strResult += "<CameraInfo>";
					strResult += "<DomainCode>";
					strResult += strDomainCode;
					strResult += "</DomainCode>";
					strResult += "<CameraCode>";
					strResult += chCameraCode;
					strResult += "</CameraCode>";
					strResult += "<RecordMethod>";
					strResult += CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod));
					strResult += "</RecordMethod>";
					strResult += "</CameraInfo>";

					xmlRsp.OutOfElem();
				} while(xmlRsp.NextElem());
				
				bHaveOneSuc = true;
			}
		} 
		else
		{
		    iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "Get Record Task From One Domain failed", "Parse Xml Failed.");
		}
	}	

	if (bHaveOneSuc)
	{
		iRet = IVS_SUCCEED;
	}


	strResult += "</CameraList></Content>";
	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strResult.size() + 1);//lint !e1924
    CHECK_POINTER(*pRspXml, IVS_ALLOC_MEMORY_ERROR);

	eSDK_MEMSET(*pRspXml, 0, strResult.size() + 1);
	(void)!CToolsHelp::Memcpy(*pRspXml, strResult.size() + 1, (const void *)strResult.c_str(), strResult.size());
	return iRet;
}

// 查询摄像机码流能力
IVS_INT32 CDeviceMgr::GetDeviceCapability(IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chCameraCode[IVS_SDK_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chCameraCode, chDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse DevCode fail", "NA");
		return iRet;
	}
	CXml xmlReq;
	iRet = CXmlProcess::GetCommConfigGetXML(chCameraCode, chDomainCode, xmlReq, SDKCAMERA);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get DeviceCapability GetXML fail", "NA");
		return iRet;
	}

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = IVS_FAIL;
	// 拼装发送报文
	CCmd *pCmd = NULL;
	// 拼装发送报文
	switch(uiCapabilityType)
	{
	case DEVICE_ENCODE_CAPABILITY:
        // 获取本域SMU连接
        iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
        if (IVS_SUCCEED != iGetLinkRet)
        {
            BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
            return iGetLinkRet;
        }
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_CAMERA_ENCODE_CAPABILITIES_REQ, pReq, strSMULinkID);
		break;
	default:
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get DeviceCapability fail", "uiCapabilityType fail");
			iRet = IVS_OPERATE_MEMORY_ERROR;
		}
		return iRet;
	}
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 获取摄像机预览图接口
IVS_INT32 CDeviceMgr::GetCameraThumbnail(const IVS_CHAR* pCameraCode, IVS_CHAR* pThumbnailURL,IVS_UINT32 uiBufferSize)const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
    CHECK_POINTER(pThumbnailURL, IVS_PARA_INVALID);
	
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	if (uiBufferSize>256)
	{
		return IVS_PARA_INVALID;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Thumbnail fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 发送消息
	// 拼装发送请求XML
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");	
	(void)xmlReq.AddChildElem("ThumbnailList");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("ThumbnailInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("DevCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDevCode);

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }

	BP_RUN_LOG_INF("Get Camera Thumbnail", "!Rspxml.FindElem(DevInfo)%s",pReq);

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_THUMBNAIL_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED == iRet)
	{
		CXml xmlRsp;
		(void)xmlRsp.Parse(strpRsp.c_str());//lint !e64 匹配
		const IVS_CHAR* szElemValue = NULL;
		if (!xmlRsp.FindElemEx("Content/ThumbnailList/ThumbnailInfo/ThumbnailURL"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Camera Thumbnail", "!Rspxml.FindElem(DevInfo)");
			return IVS_XML_INVALID;
		}
#ifdef WIN32
		GET_ELEM_VALUE_CHAR("ThumbnailURL", szElemValue, pThumbnailURL, MAX_PATH, xmlRsp);//lint !e838
#else
		GET_ELEM_VALUE_CHAR("ThumbnailURL", szElemValue, pThumbnailURL, PATH_MAX, xmlRsp);
#endif
	}

	return iRet;
}

// 设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWD(const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetDevPWD fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
    eSDK_MEMSET(&stDomainRoute, 0, sizeof(stDomainRoute));
	std::string strDomainCode = chDomaCode;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	(void)oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		
		iRet = SetDevPWDCluster(chDevCode, chDomaCode, pDevUserName, pDevPWD, pDevRegPWD);

		return iRet;
	}
	// 堆叠模式、单机模式
	iRet = SetDevPWDPile(chDevCode, chDomaCode, pDevUserName, pDevPWD, pDevRegPWD);

	return iRet;
}

// 堆叠下设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWDPile(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode = pDomainCode;
	 /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
	CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, "SCU", pDevCode);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "SetDevPWD fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密数据后拼装
    std::string strDevPWD;
    char cPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cPWDTemp, IVS_PWD_LEN, pDevPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cPWDTemp, strDevPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	std::string strDevRegPWD;
	char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevRegPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevRegPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::SetDevPWDGetXML(pDomainCode, pDevCode, pDevUserName, strDevPWD, strDevRegPWD, xmlReq); 
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set DevPWD GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_SET_DEV_PWD_REQ, pReq, strSecurityTransID);//lint !e64 匹配
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);
	return iRet;

}

// 集群下设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWDCluster(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 第一次目标域 SCU
	IVS_INT32 iRet = SetDevPWDClusterMsg(pDevCode, pDomainCode, pDevUserName, pDevPWD, pDevRegPWD, NSS_SET_DEV_PWD_REQ, "SCU", pDevCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send Set Dev PWD Msg to SCU fail", "NA");
		return iRet;
	}
	// 第二次目标域 CMU  不需要发送给CMU2014-04-30
	//iRet = SetDevPWDClusterMsg(pDevCode, pDomainCode, pDevUserName, pDevPWD, pDevRegPWD, NSS_CLUSTER_SET_DEV_PWD_REQ, "CMU");
	//if (IVS_SUCCEED != iRet)
	//{
	//	BP_RUN_LOG_ERR(iRet, "Send Set Dev PWD Msg to CMU fail", "NA");
	//	iRet = IVS_SDK_SET_CLUSTER_MSG_FAILED;
	//}

	return iRet;
}

IVS_INT32 CDeviceMgr::SetDevPWDClusterMsg(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD, NSS_MSG_TYPE_E msgType, const char* pszDestModule, const char * pszDeviceCode)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pszDestModule, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode = pDomainCode;
	 /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/

    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
	CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, pszDestModule, pszDeviceCode, "", BP::DOMAIN_CLUSTER);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "SetDevPWD fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密数据后拼装
    std::string strDevPWD;
    char cPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cPWDTemp, IVS_PWD_LEN, pDevPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cPWDTemp, strDevPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	std::string strDevRegPWD;
	char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevRegPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevRegPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::SetDevPWDGetXML(pDomainCode, pDevCode, pDevUserName, strDevPWD, strDevRegPWD, xmlReq); 
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set DevPWD GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	BP_RUN_LOG_INF("SetDevPWDClusterMsg", "send XML = %s", pReq);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, msgType, pReq, strSecurityTransID);//lint !e64 匹配
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);
	return iRet;
}

// 导入前端配置文件
IVS_INT32 CDeviceMgr::ImportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCFGFile, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Import DevCfgFile fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 文件路径，包含文件名
	std::string strCFGFile = pCFGFile;

	//读取文件
	ifstream ifCFGFile;
	ifCFGFile.open(strCFGFile.c_str(), ios::binary);
	if(!ifCFGFile)      
	{        
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "file open failed", "NA");
		return IVS_OPEN_FILE_ERROR;
	} 
	// 校验文件大小，不能超过最大
	ifCFGFile.seekg(0, ios::end); //lint !e747
	IVS_UINT32 ifLength = (IVS_UINT32)ifCFGFile.tellg();
	ifCFGFile.seekg(0, ios::beg); //lint !e747
	if (ifLength > CFG_FILE_MAX_LEN)
	{
		BP_RUN_LOG_ERR(IVS_WRITE_FILE_ERROR, "upload logo failed", "file is oversize");
		return IVS_WRITE_FILE_ERROR;
	}
	IVS_CHAR *buffer = IVS_NEW(buffer,ifLength + 1);
	CHECK_POINTER(buffer, IVS_ALLOC_MEMORY_ERROR);
	eSDK_MEMSET(buffer, 0, ifLength + 1);
	ifCFGFile.read(buffer, ifLength); //lint !e747
	ifCFGFile.close();

	// 拼装文件的TLV
	TNssTLVMsg tlvMsg;
	tlvMsg.usiTag = BINSTREAM_TAG; // 以二进制方式读取
	tlvMsg.uiLength = (IVS_UINT32)ifLength;
	tlvMsg.pszValue = buffer;

	//将文件TLV转换成字符串
	IVS_USHORT usiTag = htons(tlvMsg.usiTag);
	IVS_UINT32 uiLength = htonl(tlvMsg.uiLength);

	IVS_UINT32 uiLen = 0;
	IVS_UINT32 uiMsgBuffer = IVS_SDK_DEV_CODE_LEN + IVS_DOMAIN_CODE_LEN + ifLength  + sizeof(IVS_USHORT) + sizeof(IVS_UINT32) + CFG_FILE_MAX_LEN;
	IVS_CHAR* pMsgBuffer = IVS_NEW(pMsgBuffer, uiMsgBuffer);
	if (NULL == pMsgBuffer)
	{
		IVS_DELETE(buffer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UploadLogo", "alloc memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	eSDK_MEMSET(pMsgBuffer, 0, uiMsgBuffer);

	// 域编码
	if (!CToolsHelp::Memcpy(pMsgBuffer, uiMsgBuffer, chDomaCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDomaCode, IVS_SDK_DEV_CODE_LEN+1) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += IVS_DOMAIN_CODE_LEN;

	// 设备编码
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDevCode, IVS_SDK_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDevCode, IVS_DOMAIN_CODE_LEN+1)fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += IVS_SDK_DEV_CODE_LEN;

	// tag
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, &usiTag, sizeof(IVS_USHORT)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, usiTag, sizeof(IVS_USHORT)) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += sizeof(IVS_USHORT);

	// length
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, &uiLength, sizeof(IVS_UINT32)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, uiLength, sizeof(IVS_UINT32)) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += sizeof(IVS_UINT32);

	// value
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, buffer, tlvMsg.uiLength))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, buffer, tlvMsg.uiLength) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_DELETE(buffer, MUILI);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_IMPORT_DEV_CFG_FILE_REQ);
	sendNssMsgInfo.SetReqDataPointer(pMsgBuffer);
	sendNssMsgInfo.SetReqLength(uiMsgBuffer);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp, iNeedRedirect);
	IVS_DELETE(pMsgBuffer, MUILI);

	return iRet;
}

// 导出前端配置文件
IVS_INT32 CDeviceMgr::ExportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFilePath)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCFGFilePath, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Export DevCfgFile fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 拼装请求XML报文
	CXml xmlReq;
	iRet = CXmlProcess::GetCommConfigGetXML(chDomaCode, chDevCode, xmlReq, SDKDEVICE);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ExportDevCfg GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }
	// 发送报文
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_EXPORT_DEV_CFG_FILE_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp, iNeedRedirect);

	unsigned int ulBuffSize = strpRsp.length();
	if (ulBuffSize < sizeof(uint16_t) + sizeof(uint32_t))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "length error", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR* pRsq = IVS_NEW((IVS_CHAR*&)pRsq, ulBuffSize); //lint !e1924
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(pRsq, 0x0, ulBuffSize);
	if (!CToolsHelp::Memcpy(pRsq, ulBuffSize, strpRsp.c_str(), ulBuffSize))//lint !e64 匹配
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pRsq, strpRsp.size(), strpRsp.c_str(), strpRsp.size()) fail", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}



	// 处理TLV字段(配置文件(二进制)的处理
	TNssTLVMsg tlvMsg;
	CNSSOperator::instance().ParseData2TLV((void*)pRsq, tlvMsg);//访问越界

	// 判断tag
	if (tlvMsg.usiTag != ALARMDESC_TAG)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "tag error", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_XML_INVALID;
	}

	// 判断二进制文件大小
	if (tlvMsg.uiLength > CFG_FILE_MAX_LEN)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "length error", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_XML_INVALID;
	}

	// 存储路径
	std::string strAddress;
	strAddress.append(pCFGFilePath);
	strAddress.append(chDevCode);
	strAddress.append(".cfg");

	//读取文件
	ofstream ofCFGFile;
	ofCFGFile.open(strAddress.c_str(), ios::binary);
	if (!ofCFGFile)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "ofCFGFile.open fail", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_OPERATE_MEMORY_ERROR;
	}
	if(NULL != tlvMsg.pszValue)
	{
		ofCFGFile.write (tlvMsg.pszValue, tlvMsg.uiLength);//lint !e747
	}
	ofCFGFile.close();
	IVS_DELETE(pRsq, MUILI);

	return iRet;
}

//设备状态查询
IVS_INT32 CDeviceMgr::ExGetDeviceStatus(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("DevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ExDomain Get Device Status Req XML DevCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}

	// 添加外域编码
	xmlReq.GetRootPos();
	if (!xmlReq.MkAndSetElemValueEx("Content/DomainCode", szDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"ExDomain Get Device Status:Set DomainCode Value", "add Camera DomainCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEVICE_STATUS_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());//lint !e64 匹配
	if (xmlRsp.FindElemEx("Content/ShareDevInfo"))
	{

		(void)xmlRsp.IntoElem();
		const char* szElemValue = NULL;
		char cCameraCode[IVS_DEV_CODE_LEN+1]={0};
		if (!xmlRsp.FindElem("LocalDevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(LocalDevCode)", "NA");
			return IVS_XML_INVALID;
		}
		GET_ELEM_VALUE_CHAR("LocalDevCode",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xmlRsp);//lint !e838
		std::string strBigCode;
		strBigCode.append(cCameraCode).append("#").append(szDomainCode);
		xmlRsp.ModifyElemValue(strBigCode.c_str()); //lint !e64 匹配
		xmlRsp.OutOfElem();
	}
	IVS_UINT32 uiXmlLen = 0;
	
    const IVS_CHAR* pXmlRsp = xmlRsp.GetXMLStream(uiXmlLen);
    if (NULL != pXmlRsp)
    {
        strpRsp = pXmlRsp;
    }

	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strpRsp.size() + 1); //lint !e1924
	if (NULL == *pRspXml)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	eSDK_MEMSET(*pRspXml, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pRspXml, strpRsp.size() + 1, (const void *)strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ExDomain Get Device Status", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

//设备信息查询
IVS_INT32 CDeviceMgr::ExGetDeviceInfo(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("DevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ExDomain Get Device Info Req XML DevCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}

	// 添加外域编码
	xmlReq.GetRootPos();
	if (!xmlReq.MkAndSetElemValueEx("Content/DomainCode", szDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"ExDomain Get Device Info:Set DomainCode Value", "add Camera DomainCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEVICE_INFO_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

// 刷新摄像机列表，同步下级域摄像机列表
IVS_INT32 CDeviceMgr::RefreshCameraList(IVS_INT32 iSessionID)
{
    IVS_DEBUG_TRACE("");

    CXml xmlReq;	
    IVS_INT32 iRet = CDeviceMgrXMLProcess::RefreshCameraListXML(iSessionID, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != Get Camera List XML", "NA");
        return iRet;
    }

    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    BP_RUN_LOG_INF("RefreshCameraList", "send XML = %s", pReq);

    // 构造请求消息，并向SMU发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_REFRESH_CAMERA_LIST_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect, 120, 1);

    BP_RUN_LOG_INF("RefreshCameraList", "revice XML = %s", strpRsp.c_str());

    return iRet;
}

//Onvif设备从前端获取能力参数范围
IVS_INT32 CDeviceMgr::GetDeviceEncodeCapability(const IVS_CHAR* pDevCode,
	IVS_STREAM_INFO_LIST* pStreamInfoList)const
{
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Thumbnail fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 发送消息
	// 拼装发送请求XML
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");	
	(void)xmlReq.AddChildElem("DomainCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDomaCode);
	(void)xmlReq.AddElem("VideoInChannelInfo");
	(void)xmlReq.AddChildElem("CameraCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDevCode);
	xmlReq.OutOfElem();
	xmlReq.OutOfElem();

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		return IVS_XML_INVALID;
	}

	BP_RUN_LOG_INF("Get Camera Encode Capabilities", "!Rspxml.FindElem(DevInfo)%s",pReq);

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_ENCODE_CAPABILITIES_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;

	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if (IVS_SUCCEED == iRet)
	{
		CXml xmlRsp;

		//解析查询返回的前端编码参数信息
		(void)xmlRsp.Parse(strpRsp.c_str());//lint !e64 匹配
		iRet = CDeviceMgrXMLProcess::ParseCameraEncodeCapabilities(xmlRsp,pStreamInfoList);
	}

	return iRet;
}


//视频质量诊断计划查询
IVS_INT32 CDeviceMgr::GetVideoDiagnose(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_CHAR** pRspXml)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "ParseDevCode Failed");
		return iRet;
	}

	//获取请求xml流
	CXml xmlReq;
	iRet = CDeviceMgrXMLProcess::GetVideoDiagnoseGetXML(chDomaCode, chDevCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "GetVideoDiagnoseGetXML failed");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetVideoDiagnose", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_QD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;

	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "SendCmd failed");
		return iRet;
	}
	IVS_CHAR* pRsp = IVS_NEW((IVS_CHAR*&)pRsp, strpRsp.size() + 1);
	CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(pRsp, 0x0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(pRsp, strpRsp.size(), (const void *)strpRsp.c_str(), strpRsp.size()))
	{
		IVS_DELETE(pRsp, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetVideoDiagnose", "Memcpy fail");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	*pRspXml = pRsp;
	return iRet;
}


//视频质量诊断计划配置
IVS_INT32 CDeviceMgr::SetVideoDiagnose(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("SetVideoDiagnose, pReqXml: %s", pReqXml);

	//获取目标域编码，如果请求xml中不为空就使用请求xml中的目标域编码，否则使用本地域编码。
	CXml xmlReq;
	std::string strDstDomainCode;
	(void)CAlarmMgrXMLProcess::GetDstDomainCode(xmlReq, pReqXml, strDstDomainCode);
	if (strDstDomainCode.empty())
	{
		m_pUserMgr->GetDomainCode(strDstDomainCode);
	}

	// 构造发送结构体
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SET_QD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDstDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetVideoDiagnose", "SendCmd failed");
	}

	return iRet;
}


//初始化设备检索状态信息列表
IVS_INT32 CDeviceMgr::InitChannelRetriveStatusInfoList(DOMAIN_VEC &vecDomainRout,
	RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
	VOS_Mutex* pMutex,
	THREAD_RETRIVE_STATUS &eThreadStatus,
	IVS_DEVICE_TYPE eDeviceType) const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pMutex, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	DOMAIN_VEC_ITER itor = vecDomainRout.begin();
	DOMAIN_VEC_ITER itorEnd = vecDomainRout.end();
	IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo = {0};

	(void)VOS_MutexLock(pMutex);
	listRetriveStatusInfo.clear();
	eThreadStatus = eM_Thread_Retrive_Status_Init;

	for(; itorEnd != itor; itor++)
	{
		for(int i = 0; i < 2; i++)
		{
			if (1 == i && DEVICE_TYPE_EXTEND_ALARM == eDeviceType)
			{
				// 外部域告警设备不需要向外域（PCG）请求
				continue;
			}

			if (0 == i)
			{
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
			}
			else
			{
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
			}

			retriveStatusInfo.iSendTime = clock();
			retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
			retriveStatusInfo.uiChannelType = eDeviceType;
			retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
			eSDK_STRNCPY(retriveStatusInfo.szTargetDomainCode, sizeof(retriveStatusInfo.szTargetDomainCode), (*itor).c_str(), IVS_DOMAIN_CODE_LEN);
			retriveStatusInfo.iGetChannelType = eM_Get_Device_All;

			listRetriveStatusInfo.push_back(retriveStatusInfo);
		}
	}

	(void)VOS_MutexUnlock(pMutex);
	return IVS_SUCCEED;
}


//移除设备检索状态信息列表中指定域编码的节点
IVS_INT32 CDeviceMgr::RemoveChannelRetriveStatusInfoList(const std::string &strTargetDomainCode,
	RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
	VOS_Mutex* pMutex,
	IVS_INT32 iExDomainFlag) const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pMutex, IVS_PARA_INVALID);

	(void)VOS_MutexLock(pMutex);
	RETRIVE_STATUS_INFO_LIST_ITER iter = listRetriveStatusInfo.begin();
	RETRIVE_STATUS_INFO_LIST_ITER iterEnd = listRetriveStatusInfo.end();

	for(; iterEnd != iter; iter ++)
	{
		if ((!strncmp(strTargetDomainCode.c_str(), iter->szTargetDomainCode, IVS_DOMAIN_CODE_LEN)) && iExDomainFlag == iter->iExDomainFlag)
		{
			listRetriveStatusInfo.erase(iter);
			(void)VOS_MutexUnlock(pMutex);
			return IVS_SUCCEED;
		}
	}

	(void)VOS_MutexUnlock(pMutex);
	BP_RUN_LOG_ERR(IVS_FAIL, "Remove Channel Retrive Status Info List", "Can't find DomainCode: %s in the List", strTargetDomainCode.c_str());
	return IVS_FAIL;	
}


//查找设备检索状态信息列表中指定域编码的节点
IVS_INT32 CDeviceMgr::FindChannelRetriveStatusInfoList(const std::string &strTargetDomainCode,
	RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
	VOS_Mutex* pMutex,
	IVS_INT32 iExDomainFlag,
	IVS_CHANNLELIST_RETRIVE_STATUS_INFO &retriveStatusInfo) const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pMutex, IVS_PARA_INVALID);

	(void)VOS_MutexLock(pMutex);
	RETRIVE_STATUS_INFO_LIST_ITER iter = listRetriveStatusInfo.begin();
	RETRIVE_STATUS_INFO_LIST_ITER iterEnd = listRetriveStatusInfo.end();

	for(; iterEnd != iter; iter ++)
	{
		if ((!strncmp(strTargetDomainCode.c_str(), iter->szTargetDomainCode, IVS_DOMAIN_CODE_LEN)) && iExDomainFlag == iter->iExDomainFlag)
		{
			retriveStatusInfo = *iter;
			(void)VOS_MutexUnlock(pMutex);
			return IVS_SUCCEED;
		}
	}

	(void)VOS_MutexUnlock(pMutex);
	BP_RUN_LOG_ERR(IVS_FAIL, "Find Channel Retrive Status Info List", "Can't find DomainCode: %s in the List", strTargetDomainCode.c_str());
	return IVS_FAIL;	
}


//判断设备检索状态信息是否应该为ready
IVS_INT32 CDeviceMgr::SetChannelRetriveStatusReady(RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
	VOS_Mutex* pMutex,
	THREAD_RETRIVE_STATUS &eThreadStatus) const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pMutex, IVS_PARA_INVALID);

	THREAD_RETRIVE_STATUS iCurrentThreadStatus = eM_Thread_Retrive_Status_Ready;

	(void)VOS_MutexLock(pMutex);
	RETRIVE_STATUS_INFO_LIST_ITER iter = listRetriveStatusInfo.begin();
	RETRIVE_STATUS_INFO_LIST_ITER iterEnd = listRetriveStatusInfo.end();

	for(; iterEnd != iter; iter ++)
	{
		if (iCurrentThreadStatus > iter->iThreadStatus)
		{
			iCurrentThreadStatus = (THREAD_RETRIVE_STATUS)iter->iThreadStatus;
		}
	}
	eThreadStatus = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;

	(void)VOS_MutexUnlock(pMutex);

	return IVS_SUCCEED;	
}

IVS_INT32 CDeviceMgr::GetNvrListByClusterCode(std::string strClusterCode, std::list<std::string>& NvrList)
{
	NVR_INFO_MAP_ITOR NvrListMapiter;
    (void)VOS_MutexLock(m_pMutexNVRLock);
	NvrListMapiter = m_NvrInfoMap.begin();
	bool bFound = false;
	while(NvrListMapiter != m_NvrInfoMap.end())
	{
		for (NVR_INFO_LIST::iterator iter = NvrListMapiter->second.begin(); iter != NvrListMapiter->second.end(); iter++)
		{
			IVS_DEVICE_BRIEF_INFO stDeviceBriefInfo  = *iter;
			std::string strTmpCode = stDeviceBriefInfo.stDeviceInfo.cParentCode;

			if (!strClusterCode.compare(strTmpCode))
			{
				NvrList.push_back(stDeviceBriefInfo.stDeviceInfo.cCode);
				bFound = true;
			}
		}

		if (bFound)
		{
			break;
		}

		NvrListMapiter++;
	}
    (void)VOS_MutexUnlock(m_pMutexNVRLock);
	return IVS_SUCCEED;
}

void CDeviceMgr::GetClusterCodeByNvrCode(std::string strNvrCode, std::string& strClusterCode)
{
	NVR_INFO_MAP_ITOR NvrListMapiter;
    (void)VOS_MutexLock(m_pMutexNVRLock);
	NvrListMapiter = m_NvrInfoMap.begin();
	bool bFound = false;
	while(NvrListMapiter != m_NvrInfoMap.end())
	{
		for (NVR_INFO_LIST::iterator iter = NvrListMapiter->second.begin(); iter != NvrListMapiter->second.end(); iter++)
		{
			IVS_DEVICE_BRIEF_INFO stDeviceBriefInfo  = *iter;
			std::string strTmpCode = stDeviceBriefInfo.stDeviceInfo.cCode;

			if (!strNvrCode.compare(strTmpCode))
			{
				strClusterCode = stDeviceBriefInfo.stDeviceInfo.cParentCode;
				bFound = true;
			}
		}

		if (bFound)
		{
			break;
		}

		NvrListMapiter++;
	}
    (void)VOS_MutexUnlock(m_pMutexNVRLock);
	return ;
}

// 新增影子设备
IVS_INT32 CDeviceMgr::AddShadowDev(const IVS_CHAR* pDomainCode,
	IVS_SHADOW_IPC_INFO& ShadowDevInfo)
{
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	CXml xml;
	(void)CDeviceMgrXMLProcess::GetAddShadowDevReqXML(pDomainCode, ShadowDevInfo, xml);

	IVS_UINT32 uiLen;
	const IVS_CHAR* pReqXML = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pReqXML, IVS_OPERATE_MEMORY_ERROR);
	BP_RUN_LOG_INF("process AddMulDevice, ", "get schedule req xml[%s]", pReqXML);

	std::string strDomainCode = pDomainCode;
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_SHADOW_DEV_REQ);
	sendNssMsgInfo.SetReqData(pReqXML);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);

	std::string strRspXML;

	IVS_INT32 iNeedRedirect = IVS_FAIL;
	IVS_INT32 iRet = m_pUserMgr->SendCmdEX(sendNssMsgInfo, strRspXML, iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "process AddMulDevice, ", "fail to send get schedule message.DomainCode:%s", 
			pDomainCode);
	}

	return iRet;
}

// 影子设备手动同步
IVS_INT32 CDeviceMgr::ShadowDevSyncNotify(const IVS_SHADOW_IPC_LIST* pDevList)
{
	CHECK_POINTER(pDevList, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	CXml xml;
	std::string strDomainCode;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::GetShadowDevSyncNotifyReqXML(pDevList, strDomainCode, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ShadowDev Sync Notify ReqXML failed", "NA");
		return iRet;
	}

	IVS_UINT32 uiLen;
	const IVS_CHAR* pReqXML = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pReqXML, IVS_OPERATE_MEMORY_ERROR);
	BP_RUN_LOG_INF("process ShadowDevSyncNotify, ", "get schedule req xml[%s]", pReqXML);

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_START_CLONE_ORIG_DEV_INFO_REQ);
	sendNssMsgInfo.SetReqData(pReqXML);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}


//刷新缓存，重新获取摄像机类设备信息
IVS_INT32 CDeviceMgr::RefreshALLCameraCache(IVS_UINT32& uiMsgSendSucCount)
{
	IVS_DEBUG_TRACE("");

	IVS_INDEX_RANGE stRange;	//分页信息
	stRange.uiFromIndex = 1;
	stRange.uiToIndex = STEP_INDEX;

	//初始化摄像机设备总数
	m_uiVideoChannelTotalNum = 0;

	//获取用户的域路由
	DOMAIN_VEC dmList;
	IVS_INT32 iRet = GetDomainListByUserID(dmList);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get domain list by user id", "fail to get domain list");
		return iRet;
	}

	std::string strTransID;
	std::string strTargetDomainCode;	//需要获取设备的目标域
	std::string strSendDomainCode;		//消息发送目标域
	std::string strExdomainFlag;		//是否为外域
	DOMAIN_VEC::iterator itor = dmList.begin();
	IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;	//查询摄像机设备列表返回状态信息
	eSDK_MEMSET(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

	(void)VOS_MutexLock(m_pMutexCameraLock);
	m_vecAllCameraDev.clear();
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
	m_VideoChannelRetriveStatusInfoList.clear();
	m_VideoChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
	(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

	for(; dmList.end() != itor; itor++)
	{
		strTargetDomainCode = *itor;
		for (int i = 0; i < 2; i++)
		{
			// 发送本域消息
			CXml reqVideoXml;
			strTransID = CMKTransID::Instance().GenerateTransID();

			if (0 == i)
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), m_GetDeviceInfoType, stRange, reqVideoXml);//lint !e64 匹配	
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
				m_pUserMgr->GetDomainCode(strSendDomainCode);//lint !e613
				strExdomainFlag = "Local Domain";
			}
			else
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), m_GetDeviceInfoType, stRange, reqVideoXml);	//lint !e64 匹配
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
				strSendDomainCode = strTargetDomainCode;
				strExdomainFlag = "ExDomain";
			}
			retriveStatusInfo.iSendTime = clock();
			iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);//lint !e64 !e613 匹配

			//modify by zwx211831, Date:20140709, 日志中消息发送的目标域编码打印有误
			BP_RUN_LOG_INF("process get user channel list,", "start to send video channel request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
				stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), strTargetDomainCode.c_str(), strTransID.c_str());
			//end

			if (IVS_SUCCEED == iRet)
			{
				retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();//lint !e613
				retriveStatusInfo.uiChannelType = m_GetDeviceInfoType;
				retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
				eSDK_STRNCPY(retriveStatusInfo.szTargetDomainCode, sizeof(retriveStatusInfo.szTargetDomainCode), strTargetDomainCode.c_str(), IVS_DOMAIN_CODE_LEN);
				retriveStatusInfo.iGetChannelType = eM_Get_Device_All;

				CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());//lint !e613
				if (NULL != pSingleSvrProxy)
				{
					pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
				}

				(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
				m_VideoChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
				(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
			}
		}
	}

	(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
	uiMsgSendSucCount = m_VideoChannelRetriveStatusInfoList.size();
	(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

	return IVS_SUCCEED;
}


//刷新缓存，重新获取指定域摄像机类设备信息
IVS_INT32 CDeviceMgr::RefreshDomainCameraCache(const IVS_CHAR *pDomainCode, IVS_UINT32& uiMsgSendSucCount)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");

	IVS_INDEX_RANGE stRange;			//分页信息
	stRange.uiFromIndex = 1;
	stRange.uiToIndex = STEP_INDEX;

	//初始化摄像机设备总数
	m_uiVideoChannelTotalNumByDom = 0;	

	std::string strTransID;
	std::string strSendDomainCode;		//消息发送目标域
	std::string strExdomainFlag;		//是否为外域

	IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;			//查询摄像机设备列表返回状态信息
	eSDK_MEMSET(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

	(void)VOS_MutexLock(m_pMutexCameraLock);
	m_vecAllCamDevByDom.clear();
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutexByDom);
	m_VideoChannelRetriveStatusInfoListByDom.clear();
	m_VideoChannelRetriveThreadStatusByDom = eM_Thread_Retrive_Status_Init;
	(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutexByDom);

	for (int i = 0; i < 2; i++)
	{
		// 发送本域消息
		CXml reqVideoXml;
		strTransID = CMKTransID::Instance().GenerateTransID();

		if (0 == i)
		{
			(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, pDomainCode, m_GetDeviceInfoType, stRange, reqVideoXml);
			retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
			m_pUserMgr->GetDomainCode(strSendDomainCode);//lint !e613
			strExdomainFlag = "Local Domain";
		}
		else
		{
			(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, pDomainCode, m_GetDeviceInfoType, stRange, reqVideoXml);	
			retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
			strSendDomainCode = pDomainCode;
			strExdomainFlag = "ExDomain";
		}
		retriveStatusInfo.iSendTime = clock();
		IVS_INT32 iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);//lint !e64 !e613匹配

		BP_RUN_LOG_INF("process get user channel list,", "start to send video channel request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
			stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), pDomainCode, strTransID.c_str());

		if (IVS_SUCCEED == iRet)
		{
			retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();//lint !e613
			retriveStatusInfo.uiChannelType = m_GetDeviceInfoType;
			retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
			eSDK_STRNCPY(retriveStatusInfo.szTargetDomainCode, sizeof(retriveStatusInfo.szTargetDomainCode), pDomainCode, IVS_DOMAIN_CODE_LEN);
			retriveStatusInfo.iGetChannelType = eM_Get_Device_ByDomainCode;

			CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());//lint !e613
			if (NULL != pSingleSvrProxy)
			{
				pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
			}

			(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
			m_VideoChannelRetriveStatusInfoListByDom.push_back(retriveStatusInfo);
			(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
		}
	}

	(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
	uiMsgSendSucCount = m_VideoChannelRetriveStatusInfoListByDom.size();
	(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

	return IVS_SUCCEED;
}


// 判断是否影子设备
bool CDeviceMgr::IsShadowDev(const IVS_CHAR* pDevCode)const
{
	CHECK_POINTER(pDevCode, false);
	IVS_INFO_TRACE("pDevCode = %s", pDevCode);

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(void)CXmlProcess::ParseDevCode(pDevCode, szCameraCode, szDomainCode);

	(void)VOS_MutexLock(m_pMutexCameraLock);
	for (unsigned int i = 0; i < m_vecAllCameraDev.size(); i++)
	{
		if ((0 == strncmp(m_vecAllCameraDev.at(i).cCode, pDevCode, IVS_DEV_CODE_LEN))
			&& (0 == strncmp(m_vecAllCameraDev.at(i).cDomainCode, szDomainCode, IVS_DOMAIN_CODE_LEN)))
		{	
			if (m_vecAllCameraDev.at(i).bIsShadowDev)
			{
				(void)VOS_MutexUnlock(m_pMutexCameraLock);

				BP_RUN_LOG_INF("IsShadowDev", "true");
				return true;
			}
			else
			{
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				BP_RUN_LOG_INF("IsShadowDev", "false");
				return false;
			}  
		}		
	}
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	BP_RUN_LOG_INF("IsShadowDev", "false");
	return false;
}


// 获取影子设备的源设备码和域编码
IVS_INT32 CDeviceMgr::GetOrigDevInfo(const IVS_CHAR* pDevCode, std::string& strOrigDevDomain, std::string& strOrigDevCode)const
{
	CHECK_POINTER(pDevCode, IVS_FAIL);
	IVS_INFO_TRACE("pDevCode = %s", pDevCode);

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(void)CXmlProcess::ParseDevCode(pDevCode, szCameraCode, szDomainCode);

	(void)VOS_MutexLock(m_pMutexCameraLock);
	for (unsigned int i = 0; i < m_vecAllCameraDev.size(); i++)
	{
		if ((0 == strncmp(m_vecAllCameraDev.at(i).cCode, szCameraCode, IVS_DEV_CODE_LEN))
			&& (0 == strncmp(m_vecAllCameraDev.at(i).cDomainCode, szDomainCode, IVS_DOMAIN_CODE_LEN)))
		{	
			if (m_vecAllCameraDev.at(i).bIsShadowDev)
			{
				strOrigDevCode   = m_vecAllCameraDev.at(i).cOrigDevCode;
				strOrigDevDomain = m_vecAllCameraDev.at(i).cOriDevDomainCode;

				(void)VOS_MutexUnlock(m_pMutexCameraLock);

				BP_RUN_LOG_INF("GetOrigDevInfo: Find!", "CameraCode:%s, DomainCode:%s", 
					strOrigDevCode.c_str(), strOrigDevDomain.c_str());

				return IVS_SUCCEED;
			}
			else
			{
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				BP_RUN_LOG_INF("GetOrigDevInfo: Not Find!", "");
				return IVS_FAIL;
			}  
		}		
	}
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	BP_RUN_LOG_INF("GetOrigDevInfo: Not Find!", "");
	return IVS_FAIL;
}


int CDeviceMgr::GetAccessProtocolByCameraCode(const std::string  strCameraCode)
{
	CHECK_POINTER(m_pUserMgr, DEVICE_PRO_TEYES);
	if (0 == strCameraCode.length())
	{
		return DEVICE_PRO_TEYES;
	}

	// 根据设备编码查找接入协议类型;
	(void)VOS_MutexLock(m_pMutexCameraLock);
	IVS_UINT32 uisize = m_vecAllCameraDev.size();
	(void)VOS_MutexUnlock(m_pMutexCameraLock);
	for(IVS_UINT32 i = 0; i<uisize; i++)
	{
		IVS_CAMERA_BRIEF_INFO_ALL stCameraInfoAll = m_vecAllCameraDev.at(i);//lint !e64 匹配
		std::string strDstCameraCode = stCameraInfoAll.cCode; 

		if (0 == strCameraCode.compare(strDstCameraCode))
		{
			return CDevEnum::GetDevProtocolTypeEnum(stCameraInfoAll.cVendorType);
		}
	}

	return DEVICE_PRO_TEYES;
}



// 查询当前用户的手动录像任务
IVS_INT32 CDeviceMgr::GetRecordTaskByChannel(const IVS_UINT32 uiUserId,
		CDeviceMgr &m_DeviceMgr,  
		IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_UINT32 xmlLen = 0;
	std::string strDomainCode;
	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	std::string strResult = "<Content><CameraList>";
	const IVS_CHAR* szElemValue = NULL;

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_TASK_REQ);

	m_vecCameraDomain.clear();
	// 取得用户拥有设备的域列表
	IVS_INT32 iRet = m_DeviceMgr.GetDomainListByUserID(m_DeviceMgr.m_vecCameraDomain);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainList By UserID return failed", "NA");
		return iRet;
	}

	// 查询所有域的摄像机手动录像状态
	IVS_UINT32 iSize = m_DeviceMgr.m_vecCameraDomain.size();	
	IVS_UINT32 uiRecordMethod = 0;
	std::string strUserDomainCode;
	m_pUserMgr->GetDomainCode(strUserDomainCode);

	bool bHaveOneSuc = false;
	// 遍历域编码列表
	for(IVS_UINT32 i=0;i<iSize;i++)
	{
		CXml xmlReq;
		CXml xmlRsp;
		std::string strpRsp;
		strDomainCode = m_DeviceMgr.m_vecCameraDomain.at(i);	
		iRet = CDeviceMgrXMLProcess::GetRecordTaskGetXML(uiUserId,strUserDomainCode.c_str(),strDomainCode.c_str(),xmlReq);//lint !e64 匹配
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Get Record Task:ReqXML is failed","failed");
			return iRet;
		}

		const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);

		if (NULL == pReq)
		{
			return IVS_XML_INVALID;
		}

		sendNssMsgInfo.SetReqData(pReq); 
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get Record Task From One Domain failed", "NA");
			continue;
		}

		IVS_UINT32 uiChannelID = MAX_CHANNELID_VALUE+1;
		// 若解析xml失败
		if (xmlRsp.Parse(strpRsp.c_str()))//lint !e64 匹配
		{
			if(xmlRsp.FindElemEx("Content/CameraList"))
			{
				do 
				{
					xmlRsp.IntoElem();
					(void)xmlRsp.IntoElem();
					GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,chCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
					GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,uiRecordMethod,xmlRsp);
					(void)m_pUserMgr->GetNVRChannelMgr().GetChannelByCameraCode(chCameraCode, uiChannelID);
					strResult += "<CameraInfo>";
					strResult += "<Channel>";
					if(MAX_CHANNELID_VALUE + 1 == uiChannelID)
					{
						strResult += "none";
					}
					else
					{
						strResult += CToolsHelp::ULong2Str(static_cast<IVS_ULONG>(uiChannelID));
					}
		
					strResult += "</Channel>";
					strResult += "<RecordMethod>";
					strResult += CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod));
					strResult += "</RecordMethod>";
					strResult += "</CameraInfo>";

					xmlRsp.OutOfElem();
				} while(xmlRsp.NextElem());

				bHaveOneSuc = true;
			}
		} 
		else
		{
			iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "Get Record Task From One Domain failed", "Parse Xml Failed.");
		}
	}	

	if (bHaveOneSuc)
	{
		iRet = IVS_SUCCEED;
	}

	strResult += "</CameraList></Content>";
	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strResult.size() + 1);//lint !e1924
	CHECK_POINTER(*pRspXml, IVS_ALLOC_MEMORY_ERROR);

	eSDK_MEMSET(*pRspXml, 0, strResult.size() + 1);
	(void)!CToolsHelp::Memcpy(*pRspXml, strResult.size() + 1, (const void *)strResult.c_str(), strResult.size());
	return iRet;
}











