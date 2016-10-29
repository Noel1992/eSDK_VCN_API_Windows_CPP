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

#include "UserMgr.h"
#include "Cmd.h"
#include "IVS_Trace.h"
#include "nss_xml_msg.h"
#include "MediaXMLProcess.h"
#include "NVRChannelMgr.h"

CNVRChannelMgr::CNVRChannelMgr(void)
	: m_pUserMgr(NULL)
{
	m_pMutexChannelCameraCodeMap = VOS_CreateMutex();
	m_stChannelCameraCodeMap.clear();
}

CNVRChannelMgr::~CNVRChannelMgr(void)
{

	try
	{
		m_pUserMgr = NULL;
		m_stChannelCameraCodeMap.clear();
		if(m_pMutexChannelCameraCodeMap != NULL)
		{
			VOS_DestroyMutex(m_pMutexChannelCameraCodeMap);
			m_pMutexChannelCameraCodeMap = NULL;
		}	
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "~CNVRChannelMgr fail", "NA");
	}
	
}

void CNVRChannelMgr::SetUserMgr(CUserMgr *pUserMgr)
{
		m_pUserMgr = pUserMgr;
}


void CNVRChannelMgr::ClearChannelCameraCodeMap(void)
{

	m_stChannelCameraCodeMap.clear();
}


//get all available cameracodes
IVS_INT32 CNVRChannelMgr::GetCameraCodeByChannel(const IVS_UINT32 uiChannelID)
{
	IVS_INFO_TRACE("GetCameraCodeByChannel, ChannelID = %u", uiChannelID);
	if(MAX_CHANNELID_VALUE +1 < uiChannelID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetCameraCodeByChannel", "NOT support channelid = %u", uiChannelID);
		return IVS_PARA_INVALID;
	}
	IVS_INT32 iRet = -1;
	// create query request xml
	CXml xmlReq;
	if(uiChannelID <= MAX_CHANNELID_VALUE)	//send null message body when uiChannelID greater than MAX_CHANNELID_VALUE
	{
		iRet = CMediaXMLProcess::GetNVRChannelXML(uiChannelID, xmlReq);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get CameraCode By Channel:GetNVRChannelXML is failed", "failed");
			return iRet;
		}
	}
	unsigned int xmlLen = 0;
	const IVS_CHAR * pNVRChannelReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pNVRChannelReq, IVS_OPERATE_MEMORY_ERROR);

	// send nss protocol request message
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CHANNEL_CAMERA_MAP_REQ);
	sendNssMsgInfo.SetReqData(pNVRChannelReq); 
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CameraCode By Channel:SendCmd", "failed");
		return iRet;
	}

	// parse nss protocol response message
	CXml xmlRsp;
	xmlRsp.Parse(strpRsp.c_str());//lint !e64 ∆•≈‰
	CLockGuard cChannelCameraCodeMapLock(m_pMutexChannelCameraCodeMap);
	ClearChannelCameraCodeMap();
	iRet = CMediaXMLProcess::ParseNVRChannelXML(xmlRsp, m_stChannelCameraCodeMap);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get CameraCode By Channel:ParseNVRChannelXML", "failed");
		return IVS_XML_INVALID;
	}
	if(m_stChannelCameraCodeMap.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetCameraCodeByChannel", "get none available IPC by channel: %u.", uiChannelID);
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}


//get channelid  corresponding 0 or more cameracodes
IVS_INT32 CNVRChannelMgr::GetMultiCameraCodeByChannel(const IVS_UINT32 uiChannelID, std::vector<IVS_DEVICE_CODE>& stCameraCodeVct)
{
	IVS_INFO_TRACE(" ");
	if(MAX_CHANNELID_VALUE < uiChannelID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetMultiCameraCodeByChannel", "NOT support channelid = %u", uiChannelID);
		return IVS_PARA_INVALID;
	}

	//get devcode
	IVS_INT32 iRet = GetCameraCodeByChannel(uiChannelID);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetMultiCameraCodeByChannel", " GetCameraCodeByChannel failed");
		return iRet;
	}

	// get available devcode
	stCameraCodeVct.clear();
	IVS_DEVICE_CODE stCameraCode;
	eSDK_MEMSET(&stCameraCode, 0, sizeof(IVS_DEVICE_CODE));
	CLockGuard cChannelCameraCodeMapLock(m_pMutexChannelCameraCodeMap);
	ChannelCameraCodeMapIterator CodeItor = m_stChannelCameraCodeMap.find(uiChannelID);
	if(CodeItor != m_stChannelCameraCodeMap.end())
	{		
		CameraCodeListIpIter CodeBeginItor = CodeItor->second.begin();
		CameraCodeListIpIter CodeEndItor = CodeItor->second.end();
		for(; CodeBeginItor != CodeEndItor; CodeBeginItor++)
		{	
			if(0 == strlen(CodeBeginItor->cCameraCode))
			{
				continue;
			}
			eSDK_MEMCPY(stCameraCode.cDevCode, IVS_DEV_CODE_LEN, CodeBeginItor->cCameraCode, IVS_DEV_CODE_LEN);
			if(IVS_CAMERACODE_LEN == strlen(stCameraCode.cDevCode))
			{
				(void)m_pUserMgr->CreateDomianCameraCode(stCameraCode.cDevCode);	// create domaincode#camreacode pattern 
			}
			stCameraCodeVct.push_back(stCameraCode);
		}
	}
	if(m_stChannelCameraCodeMap.empty() || stCameraCodeVct.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetMultiCameraCodeByChannel", "NOT find available IPC by channel: %u.", uiChannelID);
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}


// get unique cameracode which online
IVS_INT32 CNVRChannelMgr::GetUniCameraCodeByChannel(const IVS_UINT32 uiChannelID, IVS_CHAR* pDevCode)
{
	IVS_INFO_TRACE(" ");
	if(MAX_CHANNELID_VALUE < uiChannelID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "GetUniCameraCodeByChannel", "NOT support channelid = %u", uiChannelID);
		return IVS_PARA_INVALID;
	}

	//get devcode
	IVS_INT32 iRet = GetCameraCodeByChannel(uiChannelID);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetMultiCameraCodeByChannel", " GetCameraCodeByChannel failed");
		return iRet;
	}

	// get available devcode
	IVS_DEVICE_CODE stCameraCode;
	eSDK_MEMSET(&stCameraCode, 0, sizeof(IVS_DEVICE_CODE));
	eSDK_MEMSET(pDevCode, 0, IVS_DEV_CODE_LEN);
	CLockGuard cChannelCameraCodeMapLock(m_pMutexChannelCameraCodeMap);
	ChannelCameraCodeMapIterator CodeItor = m_stChannelCameraCodeMap.find(uiChannelID);
	if(CodeItor != m_stChannelCameraCodeMap.end())
	{
		CameraCodeListIpIter CodeBeginItor = CodeItor->second.begin();
		CameraCodeListIpIter CodeEndItor = CodeItor->second.end();
		for(; CodeBeginItor != CodeEndItor; CodeBeginItor++)
		{	
			if(0 == strlen(CodeBeginItor->cCameraCode))
			{
				continue;
			}
			eSDK_MEMCPY(stCameraCode.cDevCode, IVS_DEV_CODE_LEN, CodeBeginItor->cCameraCode, IVS_DEV_CODE_LEN);
			if(IVS_CAMERACODE_LEN == strlen(stCameraCode.cDevCode))
			{
				(void)m_pUserMgr->CreateDomianCameraCode(stCameraCode.cDevCode);	// create domaincode#camreacode pattern 
			}
			if(1 == CodeBeginItor->bStatus)
			{
				eSDK_MEMCPY(pDevCode, IVS_DEV_CODE_LEN, stCameraCode.cDevCode, IVS_DEV_CODE_LEN); //quit loop when get online cameracode
				break;	
			}
		}
	}

	if(m_stChannelCameraCodeMap.empty() || 0 == strlen(pDevCode))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetUniCameraCodeByChannel", "NOT find available IPC by channel: %u.", uiChannelID);
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}


// get cameracodes by channel list
IVS_INT32 CNVRChannelMgr::GetMultiCameraCodeByChannelList(const IVS_UINT32  uiDeviceNum, const IVS_CHANNEL  *pChannelList,  IVS_DEVICE_CODE** pDeviceList)
{
	IVS_INFO_TRACE(" ");
	CHECK_POINTER(pChannelList, IVS_OPERATE_MEMORY_ERROR);

	//get devcode
	IVS_INT32 iRet = GetCameraCodeByChannel(MAX_CHANNELID_VALUE+1);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetMultiCameraCodeByChannel", " GetCameraCodeByChannel failed");
		return iRet;
	}
	
	// get available devcode
	IVS_DEVICE_CODE stCameraCode;
	eSDK_MEMSET(&stCameraCode, 0, sizeof(IVS_DEVICE_CODE));
	std::vector<IVS_DEVICE_CODE> stCameraCodeVct;
	stCameraCodeVct.clear();
	CLockGuard cChannelCameraCodeMapLock(m_pMutexChannelCameraCodeMap);
	ChannelCameraCodeMapIterator EndItor = m_stChannelCameraCodeMap.end();
	for(IVS_UINT32 i = 0; i < uiDeviceNum; i++)
	{
		ChannelCameraCodeMapIterator BeginItor = m_stChannelCameraCodeMap.find(pChannelList[i].uiChannel);
		if(BeginItor != EndItor) 
		{
			CameraCodeListIpIter CodeBeginItor = BeginItor->second.begin();
			CameraCodeListIpIter CodeEndItor = BeginItor->second.end();
			for(; CodeBeginItor != CodeEndItor; CodeBeginItor++)
			{	
				if(0 == strlen(CodeBeginItor->cCameraCode))
				{
					continue;
				}		
	
				if(1 == CodeBeginItor->bStatus)
				{
					eSDK_MEMCPY(stCameraCode.cDevCode, IVS_DEV_CODE_LEN, CodeBeginItor->cCameraCode, IVS_DEV_CODE_LEN);
					if(IVS_CAMERACODE_LEN == strlen(stCameraCode.cDevCode))
					{
						(void)m_pUserMgr->CreateDomianCameraCode(stCameraCode.cDevCode);	// create domaincode#camreacode pattern 
					}
					stCameraCodeVct.push_back(stCameraCode); //quit loop when get online cameracode			
					break;		
				}	
			}
		}
	}

	IVS_UINT32 uiCameraCodeNum = stCameraCodeVct.size();
	(void)IVS_NEW((IVS_DEVICE_CODE *&)*pDeviceList, uiCameraCodeNum);
	CHECK_POINTER(*pDeviceList, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(*pDeviceList, 0, uiCameraCodeNum * sizeof(IVS_DEVICE_CODE));
	for(IVS_UINT32 i = 0; i < uiCameraCodeNum; ++i)
	{
		eSDK_MEMCPY(((*pDeviceList)+i)->cDevCode, IVS_DEV_CODE_LEN, stCameraCodeVct[i].cDevCode, IVS_DEV_CODE_LEN);
	}
	return IVS_SUCCEED;
}


IVS_INT32 CNVRChannelMgr::GetChannelByCameraCode(const IVS_CHAR* pDevCode, IVS_UINT32& uiChannelID)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	if(m_stChannelCameraCodeMap.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetChannelByCameraCode", "ChannelCameraCodeMap is empty.");
		return IVS_FAIL;
	}

	std::string tmpString;
	CLockGuard cChannelCameraCodeMapLock(m_pMutexChannelCameraCodeMap);
	ChannelCameraCodeMapIterator BeginItor = m_stChannelCameraCodeMap.begin();
	ChannelCameraCodeMapIterator EndItor = m_stChannelCameraCodeMap.end();
	for(; BeginItor != EndItor; BeginItor++)
	{
		CameraCodeListIpIter CodeBeginItor = BeginItor->second.begin();
		CameraCodeListIpIter CodeEndItor = BeginItor->second.end();
		for(; CodeBeginItor != CodeEndItor; CodeBeginItor++)
		{	
			tmpString = CodeBeginItor->cCameraCode;
			if(0 == strncmp(pDevCode, CodeBeginItor->cCameraCode, tmpString.find_first_of('#')))
			{
				uiChannelID = static_cast<IVS_UINT32>(BeginItor->first);
				return IVS_SUCCEED;
			}	
		}				
	}

	if(BeginItor == EndItor)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetChannelByCameraCode", "Not get available channel by devcode.");
		uiChannelID = static_cast<IVS_UINT32>(MAX_CHANNELID_VALUE+1);
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}


IVS_INT32 CNVRChannelMgr::AddCamerCodeOfQuery(const IVS_CHAR* pDevCode, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_QUERY_UNIFIED_FORMAT** pNewUnifiedQuery)
{
	IVS_INFO_TRACE("");
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR); //pNewUnifiedQuery can be NULL

	IVS_INT32 iFieldNum = pUnifiedQuery->iFieldNum + 1;
	IVS_UINT32 uiQueryBuf = sizeof(IVS_QUERY_UNIFIED_FORMAT) + (pUnifiedQuery->iFieldNum)*sizeof(IVS_QUERY_FIELD);//lint !e737
	IVS_CHAR* ptmpUnifiedQuery = IVS_NEW((IVS_CHAR*&)ptmpUnifiedQuery, uiQueryBuf);
	CHECK_POINTER(ptmpUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	eSDK_MEMSET(ptmpUnifiedQuery, 0, uiQueryBuf);
	*pNewUnifiedQuery =reinterpret_cast<IVS_QUERY_UNIFIED_FORMAT*>(ptmpUnifiedQuery);//lint !e826

	(*pNewUnifiedQuery)->iFieldNum  = iFieldNum;
	(*pNewUnifiedQuery)->stIndex.uiFromIndex = pUnifiedQuery->stIndex.uiFromIndex;
	(*pNewUnifiedQuery)->stIndex.uiToIndex = pUnifiedQuery->stIndex.uiToIndex;
	//add camera code as a condition
	(*pNewUnifiedQuery)->stQueryField[0].eFieID = QUERY_CAMERA_CODE;
	(*pNewUnifiedQuery)->stQueryField[0].bExactQuery = 1;
	eSDK_MEMCPY((*pNewUnifiedQuery)->stQueryField[0].cValue, IVS_QUERY_VALUE_LEN, pDevCode, IVS_DEV_CODE_LEN);
	for(IVS_INT32 i = 1; i < iFieldNum; i++)
	{
		(*pNewUnifiedQuery)->stQueryField[i].eFieID = pUnifiedQuery->stQueryField[i-1].eFieID;//lint !e661
		(*pNewUnifiedQuery)->stQueryField[i].bExactQuery = pUnifiedQuery->stQueryField[i-1].bExactQuery;//lint !e661
		eSDK_MEMCPY((*pNewUnifiedQuery)->stQueryField[i].cValue, IVS_QUERY_VALUE_LEN, pUnifiedQuery->stQueryField[i-1].cValue, IVS_QUERY_VALUE_LEN);//lint !e662
	}

	return IVS_SUCCEED;

}






