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
filename    : RecordQueryInCluster.h
author      : c00206592
created     : 2013/02/01
description : ��Ⱥģʽ�µ�¼�������SMU�쳣��
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2013/02/01 ��ʼ�汾
*********************************************************************/

#ifndef __RECORD_QUERY_IN_CLUSTER_H__
#define __RECORD_QUERY_IN_CLUSTER_H__
#include "UserMgr.h"

typedef std::list<std::string> NVR_LIST;
typedef NVR_LIST::iterator NVR_LIST_ITER;

//key:ʱ�����䣬value:�������������¼����Ŀ;��¼¼���¼���ܺ�
typedef std::map<time_t,int> TIME_SPAN_ALL_NUM_MAP;
typedef TIME_SPAN_ALL_NUM_MAP::iterator TIME_SPAN_ALL_NUM_MAP_ITER;

//��ʱ���С����ĳ������ͷ������¼����Ϣ
typedef std::map<time_t,RET_RECORD_INFO*> RET_RECORD_INFO_MAP;
typedef RET_RECORD_INFO_MAP::iterator RET_RECORD_INFO_MAP_ITER;

class CRecordData
{
public:
	time_t  m_start;
	time_t  m_end;
	std::string m_strRecordInfo;
};
typedef std::map<time_t,CRecordData> REACORD_INFO_MAP;
typedef REACORD_INFO_MAP::iterator REACORD_INFO_MAP_ITER;


typedef std::map<std::string,std::string> REACORD_INFO_STR_MAP;
typedef REACORD_INFO_STR_MAP::iterator REACORD_INFO_STR_MAP_ITER;

class CRecordQueryInCluster
{
public:
	CRecordQueryInCluster();
	~CRecordQueryInCluster();
public:
	void SetUserMgr(CUserMgr *pUserMgr);

	void SetIndexRange(const IVS_INDEX_RANGE* pIndexRange);

	/***********************************************************************************
    * name       : GetRecordInCluster
    * description: ��ѯ¼���б���Ⱥģʽ��SMU����������ʱ��
	* input      : pNssMsgInfo     ָ��ṹ��
	* input      : pTimeSpan       ��ѯ��ʱ������
	* output     : pRecordInfoList ��ѯ���list
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 GetRecordInCluster(CSendNssMsgInfo& sendNssMsgInfo, const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, std::string strClusterCode, std::string& strSendXml);

protected:

private:
	/***********************************************************************************
    * name       : GetTimeSpan
    * description: ��ȡ��ѯ��ʱ������
	* input      : pUnifiedQuery ͳһ��ѯ����
	* output     : stTimeSpan ��ѯʱ������
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 GetTimeSpan(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_TIME_SPAN& stTimeSpan);

	/***********************************************************************************
    * name       : UpDateQueryIndex
    * description: ���²�ѯ����
	* input      : pUnifiedQuery ͳһ��ѯ����
	* output     : stTimeSpan ��ѯʱ������
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 UpDateQueryIndex(std::string& strQueryXml);

private:
	/***********************************************************************************
    * name       : GetCamerTraceInfo
    * description: ��SCU������������켣��Ϣ����Ⱥģʽ��SMU����������ʱ��
	* input      : pNssMsgInfo     ָ��ṹ��
	* output     : NA
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 GetCamerTraceInfo(CSendNssMsgInfo& sendNssMsgInfo, std::string strClusterCode);

	/***********************************************************************************
    * name       : GetRecordSegment
    * description: ��ʱ����ڼ�¼����ͳ����Ϣ������¼���ҳ��������Ⱥģʽ��SMU����������ʱ��
	* input      : pNssMsgInfo     ָ��ṹ��
	* input      : pUnifiedQuery   ͳһ��ѯ
	* output     : NA
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 GetRecordSegment(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery);
	/***********************************************************************************
    * name       : ParseRecordSegmentInfo
    * description: ������ʱ����ڼ�¼����ͳ����Ϣ������¼���ҳ��������Ⱥģʽ��SMU����������ʱ��
	* input      : xml    
	* output     : timeSpanMap
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 ParseRecordSegmentInfo(CXml &xml);
	/***********************************************************************************
    * name       : GetRecordListInfo
    * description: ����¼�����ݣ���Ⱥģʽ��SMU����������ʱ��
	* input      : sendNssMsgInfo     
	* output     : pRecordInfoList
	* return     : IVS_INT32 IVS_SUCCEED�ɹ� IVS_FAILʧ��
    ***********************************************************************************/
	IVS_INT32 SendRecordListInfo(CSendNssMsgInfo& sendNssMsgInfo,std::string& strSendXml);



	IVS_INT32 GetRecordListRSP(const std::string& strXml,std::string& strSendXml);
	IVS_INT32 SortAndSetRecordInfo(const std::string& strRecordInfo);
//	IVS_INT32 GetTimeFromRecordInfo(const std::string& strRecordInfo,time_t& startTime, time_t& endTime);
	IVS_INT32 GetTimeFromRecordInfo(const std::string& strRecordInfo,std::string& strKeyValue);
	IVS_INT32 DeleteOverlapTimeSpan();
	IVS_INT32 SetTimeToRecordInfo(std::string& strRecordInfo,time_t& start,time_t& end);
	IVS_INT32 AssembleTimeSpanXml(std::string& strXml);
	IVS_INT32 AssembleDomainCodeXml(std::string& strXml,std::string& strDoaminCode);
	std::string AssembleRspXml();


private:
	unsigned int m_uiFromIndex; // ��ʼ����
	unsigned int m_uiToIndex;   // ��������
	unsigned int m_uiFromIndexReMove; //��ʼ������ʡ�Եļ�¼��
	unsigned int m_uiToIndexReMove; //����������ʡ�Եļ�¼��
	CUserMgr *m_pUserMgr;
	NVR_LIST m_nvrList;
	TIME_SPAN_ALL_NUM_MAP m_timeSpanAllNumMap;
	RET_RECORD_INFO_MAP m_retRecordInfoMap;
	time_t m_StarQueryTime; //���ղ�ѯ��ʼʱ���
	time_t m_EndQueryTime;  //���ղ�ѯ����ʱ���
	unsigned int uiRealNum; //�ܵķ��������ļ�¼��
	std::string m_strCamerCode;
	std::string m_strRspXml;
	REACORD_INFO_MAP m_ReacordInfoMap;

	REACORD_INFO_STR_MAP m_ReacordInfoStrMap;
};

#endif//__RECORD_QUERY_IN_CLUSTER_H__
