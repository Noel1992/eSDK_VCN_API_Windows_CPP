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
filename    : MediaXMLProcess.h
author      : c00206592
created     : 2013/01/31
description : ý����������XML���ɽ�����
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2013/01/31 ��ʼ�汾
*********************************************************************/

#ifndef __MEDIA_XML_PROCESS_H__
#define __MEDIA_XML_PROCESS_H__

#include "SDKDef.h"  //lint !e537

#define ENCRY_KEY_LEN 90 

class CMediaXMLProcess
{
public:
	CMediaXMLProcess();
	~CMediaXMLProcess();
public:
	/***************************************************************************
     �� �� �� : VideoNumAndResourceGetXML
     �������� : ��ѯMU����ԴԤ���Ƿ�ﵽ���޼���ǰ�������Ƶ���Ƿ�����
     ������� : xml                    ����xml
     �� �� ֵ : 0 -�ɹ� ;1-ʧ��
    ***************************************************************************/
	static IVS_INT32 VideoNumAndResourceGetXML(const std::string& strCameraCode, CXml &xml);

	/******************************************************************
     function   : ParseVideoReqNumberAndReqRSP
     description: ������ԴԤ���Ƿ�ﵽ���޼���ǰ�������Ƶ���Ƿ�����
     input      : const char * pXML
     output     : iReachLimit �Ƿ�ﵽ����
				  iWhetherCited �Ƿ�����
     return     : int IVS_SUCCEED�ɹ� IVS_FAILʧ��;
    *******************************************************************/
	static IVS_INT32 ParseVideoNumAndResource(CXml& xmlRsq,IVS_INT32& iReachLimit, IVS_INT32& iWhetherCited);

	/***************************************************************************
     �� �� �� : ApplyStreamencryPwdGetXML
     �������� : ����¼����ܵĹ�����Կ�����������xml;
     ������� : xml                    ����xml
     �� �� ֵ : 0 -�ɹ� ;1-ʧ��
    ***************************************************************************/
	static IVS_INT32 ApplyStreamencryPwdGetXML(const std::string& strCameraCode,const std::string& strDomainCode,const std::string& strNvrCode, 
		const std::string& strDestModule,const int& iStreamType,const std::string& strSessionID,const std::string& strSecurityTransID, CXml &xml);

	/******************************************************************
     function   : ParseStreamencryKey
     description: ����ý�尲ȫ�㷨�õ���key������ֵ
     input      : const char * pXML
     output      : std::string & strStreamencryKey
     return     : int IVS_SUCCEED�ɹ� IVS_FAILʧ��;
    *******************************************************************/
	static IVS_INT32 ParseStreamencryKey(CXml& xmlRsq,char* pStreamencryKey);

	/***************************************************************************
     �� �� �� : GetURLXML
     �������� : ���ṹ��ת��xml;
     ������� : stUrlMediaInfo         urlý����Ϣ
	 ������� ��stSRULoginRsp          ��½��Ϣ
     ������� : xml                    ����xml
     �� �� ֵ : 0 -�ɹ� ;1-ʧ��
    ***************************************************************************/
	static IVS_INT32 GetURLXML(const ST_URL_MEDIA_INFO& stUrlMediaInfo,
								const ST_SRU_LOGIN_RSP& /*stSRULoginRsp*/, CXml &xml);

	/***************************************************************************
     �� �� �� : GetURLResponseData
     �������� : ����GetUrlXML
     ������� : stMediaRsp         ý����Ϣ
     ������� : xmlRsp                    ��Ӧxml
     �� �� ֵ : 0 -�ɹ� ;1-ʧ��
    ***************************************************************************/
	static IVS_INT32 GetURLResponseData(CXml &xmlRsp, ST_MEDIA_RSP& stMediaRsp, bool& bAssociatedAudio);

	/***************************************************************************
     *name				: GetNVRChannelXML
     *description	: create NVRChannel XML
     *input				: uiChannelID        channel id
     *output			: xmlReq                    request xml
     *return			: 0 -SUCCESS ;1-FAIL
    ***************************************************************************/
	static IVS_INT32 GetNVRChannelXML(const IVS_UINT32& uiChannelID, CXml& xmlReq);

	/***************************************************************************
	*name				: ParseNVRChannelXML
	*description	: Parse NVRChannel XML
	*input				: xmlRsp		response xml
	*output			: stChannelCameraCodeList		channel id &CameraCode list
	*return				: 0 -SUCCESS ;1-FAIL
    ***************************************************************************/
	static IVS_INT32 ParseNVRChannelXML(CXml& xmlRsp, ChannelCameraCodeMap& stChannelCameraCodeMap);
};
#endif //__MEDIA_XML_PROCESS_H__



