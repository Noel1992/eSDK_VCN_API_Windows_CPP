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
	filename	: 	XMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	XML���ö���
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 ��ʼ�汾
*********************************************************************/

#ifndef __XML_PROCESS_H__
#define __XML_PROCESS_H__

#include "IVSCommon.h"
#include "ToolsHelp.h"
#include "QueryFiled.h"
#include "SDKDef.h"
#include "bp_os.h"

#define QUERYFILED CQueryFiled::GetInstance()

#define MATCHED_SUCCEED 1
#define MATCHED_FAIL 0
#define ON_LINE 1
#define OFF_LINE 0
#define MAXLEN 64
#define MATCHED 0

//sdk�ڲ������
#define IVS_SDK_DEV_CODE_LEN 32

//ͳһʹ��
#define BUILDRET   1
#define BUILSTRING 2

//��������
enum enConfigType
{
	SDKCAMERA = 1, //CameraCode
	SDKDEVICE,  //DevCode
	SDKALARMINCODE, //AlarmInCode
	SDKALARMOUTCODE, //AlarmOutCode
	SDKSERIAL, //SerialCode
	SDKVOICE //VoiceCode
};

#define GET_ELEM_VALUE_POINTER(elem, pXMLValue, xml)  \
    pXMLValue = NULL;                             \
    if (xml.FindElem(elem))                        \
{                                                                  \
    pXMLValue = xml.GetElemValue();    \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value pointer", "not find elem: %s", elem);							\
}

/* 
*  ��ȡ�����Ϣ-�ַ�
*  elem        �����ҵ�Ԫ�أ���"DEV_INFO"
*  szXMLValue  ��Ž��Ԫ�ص�ָ��
*  NodeInfo    ������Ԫ�ص�ֵ
*  iMaxLen     �����������ֵ
*  xml         ������XML
*/
#define GET_ELEM_VALUE_CHAR(elem, szXMLValue, NodeInfo, iMaxLen, xml)                  \
    szXMLValue = NULL;                                                                  \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
	{                                                                                  \
	(void)CToolsHelp::Memcpy(NodeInfo, iMaxLen, szXMLValue, strlen(szXMLValue));  \
	}                                                                                  \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value char", "not find elem: %s", elem);							\
}

/* 
*  ��ȡ�����Ϣ-��ֵ
*  elem        �����ҵ�Ԫ�أ���"DEV_INFO"
*  szXMLValue  ��Ž��Ԫ�ص�ָ��
*  NodeInfo    ������Ԫ�ص�ֵ
*  xml         ������XML
*/
#define GET_ELEM_VALUE_NUM(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = atoi(szXMLValue);                                                    \
   }                                                                                   \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value num", "not find elem: %s", elem);							\
}

#define GET_ELEM_VALUE_NUM_FOR_UINT(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_UINT32>(atoi(szXMLValue));                                                    \
   }                                                                                   \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value uint", "not find elem: %s", elem);							\
}

#define GET_ELEM_VALUE_NUM_FOR_ULONG(elem, szXMLValue, NodeInfo, xml)	\
	NodeInfo = 0;																																\
	if (xml.FindElem(elem))																												\
	{																																						\
		szXMLValue = xml.GetElemValue();																					\
		if (NULL != szXMLValue)																										\
		{																																					\
			NodeInfo = static_cast<IVS_ULONG>(BP_OS::strtoul(szXMLValue, NULL, 10));	\
		}																																					\
	}																								\
	else																							\
	{																								\
		BP_RUN_LOG_WAR("Get elem value ulong", "not find elem: %s", elem);							\
	}

#define GET_ELEM_VALUE_NUM_FOR_UINT64(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_UINT64>(BP_OS::strtoll(szXMLValue,NULL,10));                                             \
   }                                                                                   \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value uint64", "not find elem: %s", elem);							\
}

#define GET_ELEM_VALUE_NUM_FOR_FLOAT(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_FLOAT>(atof(szXMLValue));                                                    \
   }                                                                                   \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value float", "not find elem: %s", elem);							\
}

#define GET_ELEM_VALUE_NUM_FOR_DOUBLE(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
	{                                                                                   \
	NodeInfo = static_cast<IVS_DOUBLE>(atof(szXMLValue));                                                    \
	}                                                                                   \
}																								\
else																							\
{																								\
	BP_RUN_LOG_WAR("Get elem value double", "not find elem: %s", elem);							\
}


// �ж�ָ���Ƿ�Ϊ�� 
#define CHECK_IVSBOOL( retValue )    \
	if (IVS_SUCCEED != CParaCheck::CheckIsIvsBool(retValue))                    \
{                                            \
	return IVS_PARA_INVALID;                         \
}                                            \

class CUserMgr;//lint !e763
class CXmlProcess
{
public:
	/**************************************************************************
    * name       : GetUnifiedFormatQueryXML
    * description: ͨ�ò�ѯת����XML
    * input      : pUnifiedQuery   ͨ�÷�ҳ��ѯ�ṹ��
                   xml  �����Cxml��
    * output     : NA
    * return     : ͨ�÷���
    * remark     : NA
    **************************************************************************/
	static IVS_INT32 GetUnifiedFormatQueryXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, CXml &xml, 
													const IVS_CHAR* pDomainCode = NULL,const IVS_CHAR* pCamerCode = NULL);
	static IVS_INT32 PrintfXML2File(const IVS_CHAR* FileName,const IVS_CHAR* XMLContent);

    static IVS_INT32 GetCommBuildRet(CXml &xml, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID);
	static IVS_INT32 GetCommBuildRet(CXml &xmlReq, CXml &xmlRsp, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID);

    static IVS_INT32 ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode,IVS_CHAR* pDomainCode );

	//ֻ֧��ͬһ�����µ��豸�����ȡ
	static IVS_INT32 ParseDevCodeList(std::list<std::string>& lDevCodeList,std::string& strCamerCode,IVS_CHAR* pDomainCode );

	//ת��int���͵�BOOL
	static bool TransIntToBool(unsigned int iRet);

	//ת��0,1���ִ�ON,OFF
	static std::string TransIntToOnOff(unsigned int iRet);

	//ת��ON,OFF��0,1��
	static unsigned int TransOnOffToInt(const std::string &str);

	//�ַ���ת����long
	static long TransStrToLong(const std::string& str);

	//�ַ���0,1������0,1��ת��
	static  int TransStrToInt(const std::string& str);

	//�����ַ����鵽�ִ���ת������Ҫ�������ݽṹ���ַ����ֵ�ת��//
	static void SetElemValueStr(const IVS_CHAR* pXmlValue,IVS_UINT32 ulLength,CXml &xml);

	//�������ִ�ת����ʮ��������
	static unsigned int TransBinStrToDec(const std::string& str);

	// �޸�AlarmInCode
	static IVS_INT32  SetAlarmInCodeValue(const IVS_CHAR* pPath, CXml& xml);

	//ͨ�õĲ�ѯ����
	static IVS_INT32 GetCommConfigGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pDevCode, CXml& xml, enConfigType enType);

	// ʵ����������Ϣ��������
	static IVS_INT32 GetCommSendMgr(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode, const IVS_CHAR* pCameraCode = NULL);

	// ʵ����������Ϣ�������� -- ����ڴ�й¶
	static IVS_INT32 GetCommSendMgr(CXml &xml, CXml &OutXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode);

	// ʵ�ּ�����Ϣ���͹�������
	static IVS_INT32 GetEncrypSendMgr(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode, 
		std::string strLinkID, std::string strSecurityTransID, IVS_UINT32 uiTimeOut = 120);

	// ʵ����������Ϣ��������(֧���ض���)
	static IVS_INT32 GetCommSendMgrForRedirect(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType,  const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode);

	// ʵ����������Ϣ��������(֧���ض���)---- ����ڴ�й¶
	static IVS_INT32 GetCommSendMgrForRedirect(CXml &reqXml, CXml &rspXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode);

	// ͨ������XML����QueryField�ֶ�
	static IVS_INT32 AddQueryFieldForUnifiedFormatQueryXML(const IVS_CHAR* pField,const IVS_CHAR* pValue, CXml& xmlReq);

	static IVS_INT32 GetNodeValueParseXML(CXml& xml, const IVS_CHAR* pNodePath, std::string& strNodeValue);
	static IVS_INT32 ModifyNodeValueParseXML(CXml& xml, const IVS_CHAR* pNodePath, std::string& strNodeValue);
};

#endif
