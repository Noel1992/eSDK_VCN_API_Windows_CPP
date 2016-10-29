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
	filename	: 	RealXMLProcess.h
	author		:	xusida WX168257
	created		:	2012/3/29	
	description	:	实时XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/3/29 初始版本
*********************************************************************/

#ifndef __REAL_XML_PROCESS_H__
#define __REAL_XML_PROCESS_H__

#include "XmlProcess.h"

typedef std::vector<IVS_CAMERA_PLAN_SET_INFO> PLAN_INFO_VEC;

class CRealXMLProcess
{
public:
	
	/********************************************************************
	* name       : AddBookmarkGetXML
	* description: 添加实况书签的请求XML
	* input      : uiUserId：用户ID;cUserName:用户姓名;pCameraCode:摄像机编号；pBookmarkName：书签名称； pBookmarkTime:录像书签时间; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 AddBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime, CXml &xml);
	/********************************************************************
	* name       : ModifyBookmarkGetXML
	* description: 修改实况书签的请求XML
	* input      : uiUserId：用户ID;cUserName:用户姓名;pCameraCode:摄像机编号；pBookmarkName：书签名称； uiBookmarkID:录像书签ID; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ModifyBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName, CXml &xml);
	/********************************************************************
	* name       : DeleteBookmarkGetXML
	* description: 删除实况书签的请求XML
	* input      : pCameraCode:摄像机编号； uiBookmarkID:录像书签ID; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 DeleteBookmarkGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,CXml &xml);

	/********************************************************************
	* name			: ParseShutdownNotifyXML
	* description	: 解析客户端关闭视频业务通知返回的xml
	* input			: xmlRsq
	* output			: NA
	* return			: 成功返回0，失败返回负数错误码
	* remark		: NA
	*********************************************************************/
	static IVS_INT32 ParseShutdownNotifyXML(CXml& xmlRsq, IVS_SHUTDOWN_NOTIFY_INFO *pShutdownNotifyInfo);

	private:
	/********************************************************************
	* name       : GetCameraDomainCodeByCameraCode
	* description: 获得摄像机编号中的域编号
	* input      : uiDeviceNum：摄像机数量；pDeviceList:带域编号的摄像机编号；CameraCode[64]:摄像机编号;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_CHAR* GetCameraDomainCodeByCameraCode(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,const IVS_CHAR CameraCode[64]);

};
#endif // __RECORD_XML_PROCESS_H__
