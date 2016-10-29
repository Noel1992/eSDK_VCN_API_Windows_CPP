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
	filename	: 	OCXAlarmLinkageMgr.h
	author		:	z00193167
	created		:	2012/12/06	
	description	:	����澯OCX
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 ��ʼ�汾
*********************************************************************/

#ifndef __OCX_ALARMLINKAGE_MGR_H__
#define __OCX_ALARMLINKAGE_MGR_H__

#include "hwsdk.h"

class COCXAlarmLinkageMgr
{
public:
	COCXAlarmLinkageMgr();
	~COCXAlarmLinkageMgr();

public:
	/***********************************************************************************
    * name       : AddAlarmLinkage
    * description: �����澯��������ocx
    * input      : iSessionId:��¼Id; pReqXml:CU������; strResult:����ֵ
    * output     : strResult:���ظ�CU��xml
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : ModifyAlarmLinkage
    * description: �޸ĸ澯��������ocx
    * input      : iSessionId:��¼Id; pReqXml:CU������; 
    * output     : NA
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ModifyAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);

	/***********************************************************************************
    * name       : DeleteAlarmLinkage
    * description: ɾ���澯��������ocx��iAlarmLinkageID���澯��������ID
    * input      : iSessionId:��¼Id; 
    * output     : NA
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DeleteAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml);


	/***********************************************************************************
    * name       : GetAlarmLinkage
	* description: ��ȡ����������ϸ��Ϣ
	* input      : iSessionId:��¼Id; uiAlarmLinkageID:��������Id; strResult:����ֵ;
	* output     : strResult:���ظ�CU��xml
	* return     : NA
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmLinkage(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : GetAlarmLinkageList
	* description: ��ȡ���������б�
	* input      : iSessionId:��¼Id; pReqXml:CU������; strResult:����ֵ;
	* output     : strResult:���ظ�CU��xml
	* return     : NA
	* remark     : NA
	***********************************************************************************/
	static IVS_VOID GetAlarmLinkageList(IVS_INT32 iSessionId, const IVS_CHAR* pReqXml, CString& strResult);

	/***********************************************************************************
    * name       : GetAlarmLinkageAction
	* description: ��ѯ�澯������������
	* input      : iSessionId:��¼Id; pReqXml����ѯ����xml
	* output     : NA
	* return     : �޷���ֵ
	* remark     : NA
	***********************************************************************************/
	static IVS_INT32 GetAlarmLinkageAction(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString& strResult);
};

#endif //__OCX_ALARMLINKAGE_MGR_H__

