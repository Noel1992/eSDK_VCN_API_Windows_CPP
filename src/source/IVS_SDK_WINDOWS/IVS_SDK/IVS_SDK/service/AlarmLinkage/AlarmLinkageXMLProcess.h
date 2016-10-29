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
	filename	: 	AlarmLinkageXMLProcess.h
	author		:	wulixia
	created		:	2012/12/07	
	description	:	处理告警联动XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/07 初始版本
*********************************************************************/

#ifndef __ALARM_LINKAGE_MGR_XML_PROCESS_H__
#define __ALARM_LINKAGE_MGR_XML_PROCESS_H__

#include "SDKDef.h"

class CAlarmLinkagrXMLProcess
{
private:
	CAlarmLinkagrXMLProcess();
public:
	~CAlarmLinkagrXMLProcess();

public:

	/***********************************************************************************
    * name       : GetAlarmLinkageXML
    * description: 告警联动策略， 拼装请求xml(删除和查询详情用到)
    * input      : uiAlarmLinkageID:联动策略ID; 
    * output     : xml，目标xml
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAndDelAlarmLinkageGetXML(const IVS_CHAR* pReqXml, CXml& xml, std::string& strDomainCode);

	/***********************************************************************************
    * name       : GetAlarmInCode
    * description: 查询告警联动列表，修改响应消息中的AlarmInCode
    * input      : xml:请求的xml; pRspXml， 响应消息
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetAlarmInCode(const IVS_CHAR* pReqXML, IVS_CHAR*& pRspXml);

	/***********************************************************************************
    * name       : SetAlarmInCodeValue
    * description: 修改请求消息中的AlarmInCode，DevCode(去掉#域编码)(新增，修改联动策略)
    * input      : pReqXMl，请求报文  
	               strLocalDomainCode  本域域编码，用于和联动设备域编码进行比较，如果是相同，即DevDomainCode变为空
    * output     : xml，处理以后获得的xml，
	               strDomainCode，获得的域编码
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32  SetAlarmInCodeValue(const IVS_CHAR* pReqXMl, CXml& xml, std::string& strDomainCode);

	/***********************************************************************************
    * name       : GetAlarmLinkageListGetXml
    * description: 查询告警联动策略列表拼装请求xml，获取域编码
    * input      : pReqXMl，请求报文
    * output     : xml，处理以后获得的xml，strDomainCode，获得的域编码
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32  GetAlarmLinkageListGetXml(const IVS_CHAR* pReqXMl, CXml& xml, std::string& strDomainCode);

	/***********************************************************************************
    * name       : AddDomainCode4XML
    * description: 在xml中加入节点
    * input      : pDomainCode 要加入节点的值
	               pDomainCodePath  插入路径
				   pValueName   节点名称
				   pReqXml  请求xml
    * output     : xml，处理以后获得的xml，
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddDomainCode4XML(const char* pDomainCode, const IVS_CHAR* pDomainCodePath, const IVS_CHAR* pValueName, const IVS_CHAR* pReqXml, CXml& xml);

	/***********************************************************************************
    * name       : AppendDevCode
    * description: 在响应消息中取出域编码拼在告警源编码后面
    * input      : strNSSRsq 响应消息
    * output     : xml，处理以后获得的xml，
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AppendDevCode(std::string& strNSSRsq, const IVS_CHAR* pPath, CXml& xml, const std::string& strDomainCode);










};

#endif // __ALARM_LINKAGE_MGR_XML_PROCESS_H__
