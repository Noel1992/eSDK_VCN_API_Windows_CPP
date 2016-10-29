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

#include "MediaXMLProcess.h"
#include "IVS_Trace.h"
#include "XmlProcess.h"

// C02视频解码能力：VIDEO_DEC_H264,VIDEO_DEC_MJPEG VIDEO_DEC_HEVC
#define IVS_C02_VIDEO_DEC_ABILITY "1,2,3,4,5,6,7,8"

// C02音频解码能力：AUDIO_DEC_G711A,AUDIO_DEC_G711U
#define IVS_C02_AUDIO_DEC_ABILITY "1,2"


CMediaXMLProcess::CMediaXMLProcess()
{

}
CMediaXMLProcess::~CMediaXMLProcess()
{

}
//查询MU的资源预算是否达到上限及当前摄像机视频流是否被引用
IVS_INT32 CMediaXMLProcess::VideoNumAndResourceGetXML(const std::string& strCameraCode, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strCameraCode.c_str());
	xml.OutOfElem();
	return IVS_SUCCEED;
}
//解析相应详细
IVS_INT32 CMediaXMLProcess::ParseVideoNumAndResource(CXml& xmlRsq,IVS_INT32& iReachLimit, IVS_INT32& iWhetherCited)
{
	const char*AttriValue = NULL;   //临时存储单个节点值
	if (xmlRsq.FindElemEx("Content"))
	{
		(void)xmlRsq.IntoElem();
		GET_ELEM_VALUE_NUM("ReachLimit", AttriValue, iReachLimit, xmlRsq);

		GET_ELEM_VALUE_NUM("WhetherCited", AttriValue, iWhetherCited, xmlRsq);
		return IVS_SUCCEED;
	}
	BP_RUN_LOG_ERR(IVS_FAIL, "Parse VideoNum And Resource XML", "Get VideoNum And Resource Error");
	return IVS_FAIL;
}

//请求录像加密的工作密钥或者异或因子xml;
IVS_INT32 CMediaXMLProcess::ApplyStreamencryPwdGetXML(const std::string& strCameraCode, const std::string& strDomainCode,
	                                                  const std::string& strNvrCode, const std::string& strDestModule, 
													  const int& iStreamType, const std::string& strSessionID, const std::string& strSecurityTransID, 
													  CXml &xml)
{
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("CameraCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strCameraCode.c_str());

	(void)xml.AddElem("DomainCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strDomainCode.c_str());

	(void)xml.AddElem("NVRCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strNvrCode.c_str());

	(void)xml.AddElem("DestModule");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strDestModule.c_str());

	if(!strSessionID.empty())
	{
		(void)xml.AddElem("SessionID");
		(void)(xml.IntoElem());
		(void)xml.SetElemValue(strSessionID.c_str());
	}

	(void)xml.AddElem("StreamID");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iStreamType).c_str());

	(void)xml.AddElem("ExchangeTransNo");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(strSecurityTransID.c_str());

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//解析密钥值
IVS_INT32 CMediaXMLProcess::ParseStreamencryKey(CXml& xmlRsq,char* pStreamencryKey)
{
	if (NULL == pStreamencryKey)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Streamencry Key Space is NULL", "Get Streamencry Key Error");
		return IVS_FAIL;
	}
	const char*AttriValue = NULL;   //临时存储单个节点值
	if (xmlRsq.FindElemEx("Content"))
	{
		(void)xmlRsq.IntoElem();
		GET_ELEM_VALUE_CHAR("StreamEncryPWD", AttriValue, pStreamencryKey, ENCRY_KEY_LEN, xmlRsq);
		return IVS_SUCCEED;
	}
	BP_RUN_LOG_ERR(IVS_FAIL, "Parse Streamencry Key XML", "Get Streamencry Key Error");
	return IVS_FAIL;
}


IVS_INT32 CMediaXMLProcess::GetURLXML(const ST_URL_MEDIA_INFO& stUrlMediaInfo,
										const ST_SRU_LOGIN_RSP& /*stSRULoginRsp*/, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	if ( 0 != strlen(stUrlMediaInfo.cDomainCode))
	{
		(void)xml.AddChildElem("DomainCode");
		(void)(xml.IntoElem());	
		(void)xml.SetElemValue(stUrlMediaInfo.cDomainCode);
		(void)xml.AddElem("MediaInfo");
		(void)xml.IntoElem();
	}
	else
	{
		(void)xml.AddChildElem("MediaInfo");
		(void)xml.IntoElem();
	}
	// {
	(void)xml.AddChildElem("ServiceType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(stUrlMediaInfo.ServiceType).c_str());//lint !e64 匹配
	(void)xml.AddElem("SrcCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(stUrlMediaInfo.szSrcCode);

	(void)xml.AddElem("DstCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(stUrlMediaInfo.szDstCode);

	const IVS_MEDIA_PARA& stMediaPara = stUrlMediaInfo.MediaPara;
	std::string strStreamType = CToolsHelp::Int2Str(stMediaPara.StreamType);
	(void)xml.AddElem("StreamID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strStreamType.c_str());//lint !e64 匹配
	(void)xml.AddElem("DstTP");
	(void)xml.IntoElem();
	std::string strProtocolType = CToolsHelp::Int2Str(stMediaPara.ProtocolType);
	(void)xml.SetElemValue(strProtocolType.c_str());//lint !e64 匹配
	(void)xml.AddElem("DstPP");
	(void)xml.IntoElem();
	std::string strPP = CToolsHelp::Int2Str(static_cast<int>(stUrlMediaInfo.PackProtocolType));
	(void)xml.SetElemValue(strPP.c_str());//lint !e64 匹配
	(void)xml.AddElem("DstCast");
	(void)xml.IntoElem();
	std::string strBroadcastType = CToolsHelp::Int2Str(stMediaPara.BroadCastType);
	(void)xml.SetElemValue(strBroadcastType.c_str());//lint !e64 匹配
	(void)xml.AddElem("DstIP");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(stUrlMediaInfo.szDstIP);
	(void)xml.AddElem("DstPort");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(stUrlMediaInfo.szDstPort);
	if(stUrlMediaInfo.ServiceType != SERVICE_TYPE_AUDIO_CALL && stUrlMediaInfo.ServiceType != SERVICE_TYPE_AUDIO_BROADCAST)
	{
		(void)xml.AddElem("VMedia");
		(void)xml.IntoElem();
		// 视频格式改为由MU去数据库中查，SDK只把MP的视频解码能力通知给MU
		std::string strVideoDecType = IVS_C02_VIDEO_DEC_ABILITY;
		(void)xml.SetElemValue(strVideoDecType.c_str());//lint !e64 匹配
	}
	(void)xml.AddElem("AMedia");
	(void)xml.IntoElem();
    // 音频格式改为由MU去数据库中查，SDK只把MP的音频解码能力通知给MU
	std::string strAudioDecType = IVS_C02_AUDIO_DEC_ABILITY;
	(void)xml.SetElemValue(strAudioDecType.c_str());//lint !e64 匹配
	if(SERVICE_TYPE_PLAYBACK == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_PU_PLAYBACK == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_DOWNLOAD == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_PU_DOWNLOAD == stUrlMediaInfo.ServiceType
        || SERVICE_TYPE_BACKUPRECORD_DOWNLOAD == stUrlMediaInfo.ServiceType
        || SERVICE_TYPE_BACKUPRECORD_PLAYBACK == stUrlMediaInfo.ServiceType
        // add by lilongxin 
        || SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD == stUrlMediaInfo.ServiceType
        || SERVICE_TYPE_DISASTEBACKUP_PLAYBACK == stUrlMediaInfo.ServiceType
        )
	{
		(void)xml.AddElem("TimeSpan");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(stUrlMediaInfo.szTimeSpan);
		//add by c00206592 添加NVRCode
		if (0 != strlen(stUrlMediaInfo.szNVRCode))
		{
			(void)xml.AddElem("NVRCode");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(stUrlMediaInfo.szNVRCode);
		}
		//end add
	}
	(void)xml.AddElem("TransMode");
	(void)xml.IntoElem();
	std::string strTransMode = CToolsHelp::Int2Str(stMediaPara.TransMode);
	(void)xml.SetElemValue(strTransMode.c_str());//lint !e64 匹配
	if(0!=stUrlMediaInfo.iClientType)
	{
		std::string strClientType = CToolsHelp::Int2Str(stUrlMediaInfo.iClientType);
		(void)xml.AddElem("ClientType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(strClientType.c_str());//lint !e64 匹配

		if(1 == stUrlMediaInfo.iClientType && 0 == stUrlMediaInfo.IsPassMTU)  //仅当client_type=1时设置PassMTU, 适用IVS_SDK_GetRtspURL接口NAT
		{
			std::string strPassMTU = CToolsHelp::Int2Str(stUrlMediaInfo.IsPassMTU);
			(void)xml.AddElem("PassMTU");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(strPassMTU.c_str());//lint !e64 匹配
		}
	}
	if(strlen(stUrlMediaInfo.szEncryAlgorithm)>0)
	{		
		(void)xml.AddElem("EncryAlgorithm");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(stUrlMediaInfo.szEncryAlgorithm);
	}
	if(SERVICE_TYPE_PLAYBACK == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_PU_PLAYBACK == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_PU_DOWNLOAD == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_DOWNLOAD  ==  stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_BACKUPRECORD_DOWNLOAD == stUrlMediaInfo.ServiceType
		|| SERVICE_TYPE_BACKUPRECORD_PLAYBACK == stUrlMediaInfo.ServiceType
        // add by lilongxin 
        || SERVICE_TYPE_DISASTEBACKUP_DOWNLOAD == stUrlMediaInfo.ServiceType
        || SERVICE_TYPE_DISASTEBACKUP_PLAYBACK == stUrlMediaInfo.ServiceType
        )
	{
		(void)xml.AddElem("FileName");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(stUrlMediaInfo.szFileName);
	}
    if(strlen(stUrlMediaInfo.szStreamID)>0)
    {		
        (void)xml.AddElem("Multiplex");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(stUrlMediaInfo.szStreamID);
    }
	if(stUrlMediaInfo.IsProgressionTransmit) //239
	{		
		(void)xml.AddElem("IsProgressionTransmit");
		(void)xml.IntoElem();
		(void)xml.SetElemValue("1");

		(void)xml.AddElem("CameraDomain");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(stUrlMediaInfo.cCameraDomain);
	}
	xml.OutOfElem();
	//}

	return IVS_SUCCEED;
}

/*lint -esym(1960,16-0-4) */
#define CHECK_SUCCESS(s)\
    do { if(!(s)){ return IVS_XML_INVALID;} else{}} while(0)
/*lint +esym(1960,16-0-4) */

IVS_INT32 CMediaXMLProcess::GetURLResponseData(CXml &xmlRsp, ST_MEDIA_RSP& stMediaRsp, bool& bAssociatedAudio)
{
	IVS_DEBUG_TRACE("");

	eSDK_MEMSET(&stMediaRsp, 0, sizeof(ST_MEDIA_RSP));
	const char*AttriValue = NULL;   //临时存储单个节点值

	CHECK_SUCCESS(xmlRsp.FindElem("Content"));
	CHECK_SUCCESS(xmlRsp.IntoElem());


	CHECK_SUCCESS(xmlRsp.FindElem("URL"));
	CHECK_SUCCESS(NULL != (AttriValue = xmlRsp.GetElemValue()));
	bool bRet = CToolsHelp::Memcpy(stMediaRsp.szURL, URL_LEN, AttriValue, strlen(AttriValue));
	if(!bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL Response Data", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	if (xmlRsp.FindElem("AssociatedAudio"))
	{
		CHECK_SUCCESS(NULL != (AttriValue = xmlRsp.GetElemValue()));
		bAssociatedAudio = (bool)atoi(AttriValue);	//lint !e1924
	}

	CHECK_SUCCESS(xmlRsp.FindElem("RtspIP"));
	CHECK_SUCCESS(NULL != (AttriValue = xmlRsp.GetElemValue()));
	bRet = CToolsHelp::Memcpy(stMediaRsp.szRTSPIP, IVS_IP_LEN, AttriValue, strlen(AttriValue));
	if(!bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL Response Data", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	CHECK_SUCCESS(xmlRsp.FindElem("RtspPort"));
	CHECK_SUCCESS(NULL != (AttriValue = xmlRsp.GetElemValue()));
	bRet = CToolsHelp::Memcpy(stMediaRsp.szRTSPPORT, PORT_LEN, AttriValue, strlen(AttriValue));
	if(!bRet)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL Response Data", "Memcpy error.");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	//add by zwx211831, Date:20140625, 在URL中添加UserID和DomainID，以实现查询点播业务。
	//如果是旧的版本，则不变。
	if (xmlRsp.FindElem("UserId") && xmlRsp.FindElem("DomainId"))
	{
		(void)xmlRsp.FindElem("DomainId");
		CHECK_SUCCESS(NULL != (AttriValue = xmlRsp.GetElemValue()));
		std::string strDomainCode("DomainCode=");
		strDomainCode += AttriValue;
		strDomainCode += "&";
		if(!CToolsHelp::Strncat(stMediaRsp.szURL, URL_LEN, strDomainCode.c_str()))//lint !e64 匹配
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL Response Data", "Strncat error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		(void)xmlRsp.FindElem("UserId");
		AttriValue = xmlRsp.GetElemValue();
		CHECK_SUCCESS(NULL != AttriValue);
		IVS_UINT32 uiUserID = 0;
		std::string strUserID("UserId=");
		GET_ELEM_VALUE_NUM_FOR_UINT("UserId", AttriValue, uiUserID, xmlRsp);//lint !e838
		strUserID += CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID));
		strUserID += "&";
		if(!CToolsHelp::Strncat(stMediaRsp.szURL, URL_LEN, strUserID.c_str()))//lint !e64 匹配
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get URL Response Data", "Strncat error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}

	xmlRsp.OutOfElem();

	return IVS_SUCCEED;
}



IVS_INT32 CMediaXMLProcess::GetNVRChannelXML(const IVS_UINT32& uiChannelID, CXml &xmlReq)
{
	IVS_DEBUG_TRACE("");
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.AddChildElem("ChannelNumber");
	(void)xmlReq.IntoElem();
	 char szTmp[STR_LENGTH_SIZE];
	 sprintf_s(szTmp, STR_LENGTH_SIZE, "%d", uiChannelID);
	(void)xmlReq.SetElemValue(szTmp);
	xmlReq.OutOfElem();

	return IVS_SUCCEED;
}


IVS_INT32 CMediaXMLProcess::ParseNVRChannelXML(CXml &xmlRsp, ChannelCameraCodeMap& stChannelCameraCodeMap)
{
	IVS_DEBUG_TRACE("");
	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR cChannelID[STR_LENGTH_SIZE] = {0}; 
	IVS_CHAR cStatus[3] = {0};
	IVS_CAMERACODE_INFO stCameraCodeInfo;
	CameraCodeList stCameraCodeList;
	stChannelCameraCodeMap.clear();
	if (!xmlRsp.FindElemEx("Content/ChannelList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse nvr channel rsp xml", "fail to find ChannelInfo node info, maybe this node no exist");
		return IVS_XML_INVALID;
	}

	do 
	{
		if (!xmlRsp.FindElemValue("ChannelInfo"))
		{
			break;
		}
		(void)xmlRsp.IntoElem();

		stCameraCodeList.clear();
		GET_ELEM_VALUE_CHAR("ChannelNumber", szElemValue, cChannelID, STR_LENGTH_SIZE, xmlRsp);
		IVS_UINT32 uiChannelID = (IVS_UINT32)CToolsHelp::StrToInt(cChannelID);

		if (!xmlRsp.FindElemValue("CameraList"))
		{
			break;
		}
		(void)xmlRsp.IntoElem();

		do 
		{
			if(!xmlRsp.FindElemValue("CameraInfo"))
			{
				break;
			}
			(void)xmlRsp.IntoElem();

			eSDK_MEMSET(&stCameraCodeInfo, 0, sizeof(IVS_CAMERACODE_INFO));
			GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, stCameraCodeInfo.cCameraCode, IVS_DEV_CODE_LEN, xmlRsp);
			GET_ELEM_VALUE_CHAR("Status", szElemValue, cStatus, 3, xmlRsp); 
			stCameraCodeInfo.bStatus =(IVS_INT32)CToolsHelp::StrToInt(cStatus);
			stCameraCodeList.push_back(stCameraCodeInfo);
			xmlRsp.OutOfElem();
		} while (xmlRsp.NextElem());
	
		(void)stChannelCameraCodeMap.insert(std::make_pair(uiChannelID, stCameraCodeList));

		xmlRsp.OutOfElem();
		xmlRsp.OutOfElem();
	} while (xmlRsp.NextElem());

	if(stChannelCameraCodeMap.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "ParseNVRChannelXML", "get no channel info");
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
	
}






