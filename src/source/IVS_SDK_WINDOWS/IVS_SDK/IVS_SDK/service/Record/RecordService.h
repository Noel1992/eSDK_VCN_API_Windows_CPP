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
	filename	: 	RecordService.h
	author		:	z00201790
	created		:	2012/11/12	
	description	:	����¼��ҵ����������;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/05 ��ʼ�汾;
*********************************************************************/

#ifndef __RECORD_SERVICE_H__
#define __RECORD_SERVICE_H__

#include "IVSCommon.h"
#include "vos.h"
#include "Record.h"
#include "SDKDef.h"

class CRecordService
{
public:
	CRecordService(void);
	~CRecordService(void);

public:
	void SetUserMgr(CUserMgr *pUserMgr);

	/******************************************************************
	function : DealException
	description: ����MP�����쳣�ص�;
	input : iPort,�˿ڣ�iMsgType���ص���Ϣ���ͣ�pHandle�������
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	void DealException(IVS_INT32 iPort, IVS_INT32 iMsgType,const void* pHandle);

	/******************************************************************
	function : Release
	description: �ͷ�¼���߳���;
	input : ��
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	void Release();
	/******************************************************************
	function : GetRecordInst
	description: 
	input : 
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	int GetRecordInst(int iPlayHandle, bool bStart, unsigned int& uiIndex);
	/******************************************************************
	function : CancelRecordInst
	description: 
	input : 
	output : NA
	return : ��
	*******************************************************************/
	void CancelRecordInst(unsigned int uiIndex);
	/******************************************************************
	function : GetRecordInstByMpPort
	description: 
	input : 
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	 int GetRecordInstByMpPort(unsigned long ulMpPort, unsigned int& uiIndex);

	/******************************************************************
	function : StartPlatformRecord
	description: ��ʼƽ̨¼��;
	input : pCameraCode,�������ţ�iRecordTime��¼��ʱ��
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StartPlatformRecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

	/******************************************************************
	function : StopPlatformRecord
	description: ֹͣƽ̨¼��;
	input : pCameraCode,��������
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StopPlatformRecord(const IVS_CHAR* pCameraCode);

	/******************************************************************
	function : StartPURecord
	description: ��ʼǰ��¼��;
	input : pCameraCode,�������ţ�iRecordTime��¼��ʱ��
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StartPURecord(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordTime);

	/******************************************************************
	function : StopPURecord
	description: ֹͣǰ��¼��;
	input : pCameraCode,��������
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 StopPURecord(const IVS_CHAR* pCameraCode);

	/******************************************************************
	function : GetRecordPolicy
	description: ��ȡ¼�����;
	input : pCameraCode,�������ţ�pRecordPolicy��¼�������Ϣ
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 GetRecordPolicy(const IVS_CHAR* pCameraCode,IVS_RECORD_POLICY_TIME* pRecordPolicy)const;

	/******************************************************************
	function : SetRecordPolicy
	description: ����¼�����;
	input : pCameraCode,�������ţ�pRecordPolicy��¼�������Ϣ
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 SetRecordPolicy(const IVS_CHAR* pCameraCode,const IVS_RECORD_POLICY_TIME* pRecordPolicy)const;

	/******************************************************************
	function : GetExDomainRecordPolicy
	description: ��ѯ�������������¼�����;
	input : pReqXml,����XML
	output : pRspXml ��ӦXML
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 GetExDomainRecordPolicy(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const;

	/******************************************************************
	function : SetExDomainRecordPolicy
	description: �����������������¼�����;
	input : pReqXml,����XML
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 SetExDomainRecordPolicy(const IVS_CHAR* pReqXml)const;

    /******************************************************************
	function : AddExDomainRecordPlan
	description: �����������������¼��ƻ�;
	input : pReqXml,����XML
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 AddExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;
    /******************************************************************
	function : ModExDomainRecordPlan
	description: �޸��������������¼��ƻ�;
	input : pReqXml,����XML
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
    IVS_INT32 ModExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

    /******************************************************************
	function : DelExDomainRecordPlan
	description: ɾ���������������¼��ƻ�;
	input : pReqXml,����XML
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/

    IVS_INT32 DelExDomainRecordPlan(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        IVS_UINT32 uiRecordMethod,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize)const;

    
	/******************************************************************
	function : GetExDomainRecordPlan
	description: ��ѯ�������������¼��ƻ�;
	input : pCameraCode,�������ţ�uiRecordMethod��¼��ƻ���ʽ��pRecordPlan��¼��ƻ���ѯ�����
	output : NA
	return : �ɹ�-0��ʧ��-1
	*******************************************************************/
	IVS_INT32 GetExDomainRecordPlan(const char* pCameraCode,IVS_UINT32 uiRecordMethod,
		IVS_RECORD_PLAN* pRecordPlan);

	/***********************************************************************************
    * name       : GetPrerecordCameraList
    * description: ��ѯ����Ԥ¼��������б�
	input      : IVS_INT32 iSessionID ��¼�ɹ���ĻỰID;
	output     : IVS_CHAR * * pRspXml  ��ѯ¼�񱸷������б�����ϢXML
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetPrerecordCameraList(IVS_CHAR** pRspXml);

private:
	CRecord m_RecordInst[MAX_RECORD_COUNT];

	CUserMgr *m_pUserMgr;
    VOS_Mutex* m_pInstMutex; //��
};

#endif
