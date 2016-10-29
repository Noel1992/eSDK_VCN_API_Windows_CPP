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

/*****************************************************************
 filename    :    LogMgr.h
 author      :    wWX155351
 created     :    2012/11/14
 description :    ��ѯ������־;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/14 ��ʼ�汾
*****************************************************************/
#ifndef _OPERATIONLOG_MNG_H_
#define _OPERATIONLOG_MNG_H_


#include "hwsdk.h"
#include "ivs_error.h"

/*
#include "NSSOperator.h"
#include "nss_mt.h"*/
class CUserMgr;
class CLogMgr
{
public:
	CLogMgr(void);
	~CLogMgr(void);

	// �����û�������
	void SetUserMgr(void *pUserMgr);

private:
	CUserMgr *m_pUserMgr;

public:
	// ��ѯ������־
	IVS_INT32 GetOperationLog(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, IVS_OPERATION_LOG_LIST* pResOperLogList,const IVS_UINT32& uiBufSize) const;

	/*****************************************************************
		 function   : GetOperationLogEx
		 description: ��չ��ѯ������־
		 * input      : pUnifiedQuery ͳһͨ�ò�ѯ����
		 * input      : pOptLogQueryEx ��չ��ѯ������־����
		 * input      : uiBufSize �¼����ݴ�С
		 * output   : pResOperLogList ������־��ѯ��ѯ���
		 * return		: �ɹ�����IVS_SUCCESSED��ʧ�ܷ��ش�����
		 return		: �ɹ�����0��ʧ�ܷ��ش�����;
		 *****************************************************************/
	IVS_INT32 GetOperationLogEx(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
		IVS_OPERATION_LOG_LIST* pResOperLogList, 
		const IVS_UINT32 uiBufSize) const;

};
#endif 

