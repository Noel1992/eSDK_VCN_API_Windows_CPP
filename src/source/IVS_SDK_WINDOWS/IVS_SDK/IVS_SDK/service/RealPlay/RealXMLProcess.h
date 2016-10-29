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
	description	:	ʵʱXML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/3/29 ��ʼ�汾
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
	* description: ���ʵ����ǩ������XML
	* input      : uiUserId���û�ID;cUserName:�û�����;pCameraCode:�������ţ�pBookmarkName����ǩ���ƣ� pBookmarkTime:¼����ǩʱ��; xml:����XML
	* output     : NA
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 AddBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime, CXml &xml);
	/********************************************************************
	* name       : ModifyBookmarkGetXML
	* description: �޸�ʵ����ǩ������XML
	* input      : uiUserId���û�ID;cUserName:�û�����;pCameraCode:�������ţ�pBookmarkName����ǩ���ƣ� uiBookmarkID:¼����ǩID; xml:����XML
	* output     : NA
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ModifyBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName, CXml &xml);
	/********************************************************************
	* name       : DeleteBookmarkGetXML
	* description: ɾ��ʵ����ǩ������XML
	* input      : pCameraCode:�������ţ� uiBookmarkID:¼����ǩID; xml:����XML
	* output     : NA
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 DeleteBookmarkGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,CXml &xml);

	/********************************************************************
	* name			: ParseShutdownNotifyXML
	* description	: �����ͻ��˹ر���Ƶҵ��֪ͨ���ص�xml
	* input			: xmlRsq
	* output			: NA
	* return			: �ɹ�����0��ʧ�ܷ��ظ���������
	* remark		: NA
	*********************************************************************/
	static IVS_INT32 ParseShutdownNotifyXML(CXml& xmlRsq, IVS_SHUTDOWN_NOTIFY_INFO *pShutdownNotifyInfo);

	private:
	/********************************************************************
	* name       : GetCameraDomainCodeByCameraCode
	* description: ������������е�����
	* input      : uiDeviceNum�������������pDeviceList:�����ŵ��������ţ�CameraCode[64]:��������;
	* output     : NA
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
	* remark     : NA
	*********************************************************************/
	static IVS_CHAR* GetCameraDomainCodeByCameraCode(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,const IVS_CHAR CameraCode[64]);

};
#endif // __RECORD_XML_PROCESS_H__
