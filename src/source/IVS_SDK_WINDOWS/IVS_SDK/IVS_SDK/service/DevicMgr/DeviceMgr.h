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
	filename	: 	DeviceMgr.h
	author		:	gwx151720
	created		:	2012/11/01	
	description	:	�豸������࣬�ṩ��cuʹ��
	copyright	:	Copyright (C) 2012
	history		:	2012/11/01
*********************************************************************/
#ifndef _DEVICE_MGR_H_
#define _DEVICE_MGR_H_

#include "ivs_error.h"
#include "hwsdk.h"
#include "vos.h"
#include "DeviceGroupMgr.h"
#include "ivs_xml.h"
#include "SDKDef.h"
#include "SDKSecurityClient.h"

// �豸������Ϣ����
typedef std::list<IVS_CAMERA_BRIEF_INFO> CAMERA_BRIEF_INFO_LIST;
typedef CAMERA_BRIEF_INFO_LIST::iterator CAMERA_BRIEF_INFO_LIST_ITER;

typedef std::vector<std::string> DOMAIN_VEC;
typedef DOMAIN_VEC::iterator DOMAIN_VEC_ITER;
typedef std::vector<IVS_CAMERA_BRIEF_INFO> CAMERA_BRIEF_INFO_VEC;
typedef std::vector<IVS_CAMERA_BRIEF_INFO_EX> CAMERA_BRIEF_INFO_EX_VEC;
typedef std::vector<IVS_SHADOW_CAMERA_BRIEF_INFO> CAMERA_BRIEF_INFO_SHADOW_VEC;//add by y00138234

//��Ƶ���豸ȫ����Ϣ����
typedef std::vector<IVS_CAMERA_BRIEF_INFO_ALL> CAMERA_BRIEF_INFO_ALL_VEC;

typedef std::vector<IVS_CHAR[IVS_CAMERA_STRUCT_LEN]> CAMERA_STRUCT_VEC;

typedef std::vector<IVS_ALARM_BRIEF_INFO> ALARM_BRIEF_INFO_VEC;
typedef std::vector<IVS_AUDIO_BRIEF_INFO> AUDIO_BRIEF_INFO_VEC;

typedef std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> RETRIVE_STATUS_INFO_LIST;
typedef RETRIVE_STATUS_INFO_LIST::iterator RETRIVE_STATUS_INFO_LIST_ITER;

// NVR��Ϣ����
typedef std::list<IVS_DEVICE_BRIEF_INFO> NVR_INFO_LIST;
typedef NVR_INFO_LIST::iterator NVR_INFO_LIST_ITOR;
//typedef std::list<IVS_DEVICE_BRIEF_INFO*> DEVICE_BRIEF_INFO_LIST;
//typedef DEVICE_BRIEF_INFO_LIST::iterator DEVICE_BRIEF_INFO_LIST_ITER;

// �����Ϊ�������豸������Ϣ�����map����
//typedef std::map<std::string, DEVICE_BRIEF_INFO_LIST*> DEVICE_BRIEF_INFO_LIST_MAP;
//typedef DEVICE_BRIEF_INFO_LIST_MAP::iterator DEVICE_BRIEF_INFO_LIST_MAP_ITER;
typedef std::map<std::string, NVR_INFO_LIST> NVR_INFO_MAP;
typedef NVR_INFO_MAP::iterator NVR_INFO_MAP_ITOR;


enum THREAD_RETRIVE_STATUS
{
	eM_Thread_Retrive_Status_Init,				// ��ʼ��
	eM_Thread_Retrive_Status_Retriving,	// ��ȡ��
	eM_Thread_Retrive_Status_Ready,			// ����
	eM_Thread_Retrive_Excepting					// �����쳣
};

//��ȡ�豸��Ϣ����
enum GET_DEVICE_INFO_TYPE
{
	eM_Get_Camera,										// ��ȡ������豸�б�
	eM_Get_Camera_Ex,							        // ��ȡ����������Ϣ��������豸�б�
	eM_Get_Camera_Shadow,                               // ��ȡ�豸����ΪӰ�������
	eM_Get_Camera_ALL									// ��ȡ�������͵�����ͷ
};

class CRecordTaskCameraInfo
{
private:
	unsigned int uiRecordMethod;               //�Ƿ���Ҫ����XML
	std::string m_strDomainCode;           //�����
	std::string m_strCameraCode;           //��������� 
public:
	CRecordTaskCameraInfo()
		:  uiRecordMethod(0)
	{}
	unsigned int GetRecordMethod()const { return uiRecordMethod; };
	//delete by zwx211831, Date:20140924, û�õ�
	//void SetRecordMethod(unsigned int m_uiRecordMethod)const{ m_uiRecordMethod = uiRecordMethod; };//lint !e550

	std::string& GetDomainCode(){ return m_strDomainCode; };//lint !e1036 !e1536
	void SetDomainCode(const std::string& usDomainCode){ m_strDomainCode = usDomainCode; };
	std::string& GetCameraCode(){ return m_strCameraCode; };//lint !e1036 !e1536
	void SetCameraCode(const std::string& usCameraCode){ m_strCameraCode = usCameraCode; };
};

// �����Ϊ�����ĵ�ǰ�û����ֶ�¼��������Ϣ�����map����
typedef std::map<std::string,CRecordTaskCameraInfo*> DEVICE_RECORD_TASK_CAMERA_INFO_MAP;
typedef DEVICE_RECORD_TASK_CAMERA_INFO_MAP::iterator DEVICE_RECORD_TASK_CAMERA_INFO_MAP_ITER;

//����豸��Ϣ
class CMulDeviceAddInfoList
{
public:
	IVS_UINT32 m_uiType;
	std::string m_strComplexCode;
	std::list<IVS_DEVICE_OPER_INFO> deviceInfoList;
	CMulDeviceAddInfoList():m_uiType(0){}
	~CMulDeviceAddInfoList(){}
	std::string GetComplexCode()const{return m_strComplexCode;}
};


typedef std::map<std::string, CMulDeviceAddInfoList> MUL_DEVICE_ADD_NVR_MAP;
typedef MUL_DEVICE_ADD_NVR_MAP::iterator MUL_DEVICE_ADD_NVR_MAP_ITOR;

//class CUserMgr;
class CDeviceMgr
{
public:
	CDeviceMgr();
	~CDeviceMgr();

public:
	void SetUserMgr(CUserMgr *pUserMgr);
    void Clear();

	IVS_UINT32 ProcessBatchRedirectService(DeviceRedirectMap& deviceRedirectInfoMap);

	IVS_UINT32 ProcessRedirectService(const IVS_CHAR* pDomainCode, IVS_DEVICE_REDIRECT_INFO& deviceRedirectInfo);

	IVS_UINT32 UpdateCameraInfoListToCache(const IVS_CHAR* pDomainCode, std::list<IVS_DEVICE_REDIRECT_INFO>& deviceRedirectInfoList);

	//��ȡNVR����
	IVS_UINT32 GetNVRMode(const IVS_CHAR* pDomainCode, const std::string& nvrCode, std::string& complexCode);

	// ����Ҫ��ӵ��豸�б�װNVR���࣬��ȷ����NVR������Ϊ��Ⱥ��ѵ�
	IVS_UINT32 MasterDeviceListTransform(const IVS_CHAR*   pDomainCode,
		const IVS_DEVICE_OPER_INFO*  pDevList,
		IVS_UINT32       uiDeviceNum,
		MUL_DEVICE_ADD_NVR_MAP &mulDeviceAddNVRMap,
		std::list<std::string>& lClusterIDList);

	/*****************************************************************
     function   : AddDevice
     description: �����豸
	 input      : pDomainCode:�����; uiDeviceNum:�豸����; pDevList:�豸��Ϣ�ṹ��; pResultList:�豸�����б�; uiResultBufferSize:�����ڴ��С;
     output     : NA
     return     : �ɹ�����0��ʧ�ܷ��ظ���������
   ***************************************************************/

	IVS_INT32 AddDevice(const IVS_CHAR* pDomainCode, 
                        IVS_UINT32 uiDeviceNum, 
                        const IVS_DEVICE_OPER_INFO* pDevList, 
                        IVS_DEVICE_OPER_RESULT_LIST* pResultList, 
                        IVS_UINT32 uiResultBufferSize)const;

	// ����豸���������������ѵ��ͼ�Ⱥͳһ�ӿ�
	IVS_INT32 AddMulDevice(const IVS_CHAR* pDomainCode, 
		IVS_UINT32 uiDeviceNum, 
		const IVS_DEVICE_OPER_INFO* pDevList,
		IVS_DEVICE_OPER_RESULT_LIST* pResultList,
		IVS_UINT32 uiResultBufferSize);

	IVS_INT32 DeviceAddOperResultListTransform(std::list<IVS_DEVICE_OPER_RESULT>& operResultList,
		IVS_DEVICE_OPER_RESULT_LIST* pResultList,
		IVS_UINT32 uiResultBufferSize)const;

	IVS_INT32 ProcessAddMulDevice(const IVS_CHAR* pDomainCode, 
		IVS_UINT32 uiDeviceNum, 
		MUL_DEVICE_ADD_NVR_MAP& mulDeviceAddNVRMap,
		std::list<IVS_DEVICE_OPER_RESULT> lScheduleResultList,
		IVS_DEVICE_OPER_RESULT_LIST* pResultList,
		IVS_UINT32 uiResultBufferSize);

	IVS_INT32 ProcessAddMulDeviceToSameNVR(const IVS_CHAR* pDomainCode,
		CMulDeviceAddInfoList& mulDeviceAddIndoList,
		std::list<IVS_DEVICE_OPER_RESULT>& operResultList);

	/*****************************************************************
	     function   : DeleteDevice
	     description: ɾ���豸
	     input      : pDomainCode:�����
	                  uiDeviceNum:�豸����
	                  pDevCodeList:�豸�����б�
	                  pResultList:�豸������������б�
	                  uiResultBufferSize:�����ڴ��С
		 output     : NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ���������
	*****************************************************************/
	IVS_INT32 DeleteDevice(const IVS_CHAR*  pDomainCode,
                            IVS_UINT32 uiDeviceNum , 
                            const IVS_DEVICE_CODE* pDevCodeList , 
                            IVS_RESULT_LIST* pResultList ,
                            IVS_UINT32 uiResultBufferSize)const;

    /*****************************************************************
         function   : QueryDeviceList
         description: ��ѯ�豸�б�
         input      : uiDeviceType:�豸����
                      pDomainCode:�����
                      pQueryFormat:��ѯͨ�ø�ʽ
                      pBuffer:�����ڴ�ָ��
                      uiBufferSize:�����ڴ��С
		 output     : NA
         return     : �ɹ�����0��ʧ�ܷ��ظ���������
    *****************************************************************/
    IVS_INT32 QueryDeviceList(IVS_UINT32 uiDeviceType,
							const IVS_CHAR* pDomainCode,
                            const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                            IVS_VOID* pBuffer, 
                            IVS_UINT32 uiBufferSize);

	
	/******************************************************************
	function : GetDeviceList
	description: ��ȡ�豸�б�
	input : uiDeviceType:�豸����
	input : pIndexRange:��ҳ��Ϣ
	input : uiBufferSize:�����ڴ��С
	input : pDeviceList:�����ڴ�ָ��
	output : NA
	return : �ɹ�����0��ʧ�ܷ��ظ���������
	*******************************************************************/
	IVS_INT32 GetDeviceList(IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);

	/******************************************************************
	function : GetDomainDeviceList
	description: ��ȡָ�����豸�б�
	input : pDomainCode:�豸���ڵ������
	input : uiDeviceType:�豸����
	input : pIndexRange:��ҳ��Ϣ
	input : uiBufferSize:�����ڴ��С
	input : pDeviceList:�����ڴ�ָ��
	output : NA
	return : �ɹ�����0��ʧ�ܷ��ظ���������
	*******************************************************************/
	IVS_INT32 GetDomainDeviceList(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);

	/******************************************************************
	function : GetCameras
	description: ��ȡ�����
	input : const IVS_INDEX_RANGE * pQueryRange:��ҳ��Ϣ
	input : IVS_UINT32 uiBufferSize�������ڴ��С
	input : IVS_CAMERA_BRIEF_INFO_LIST * pList	����Ƶ���豸�б�
	output ��NA 
	return : �ɹ�����0��ʧ�ܷ��ظ���������
	*******************************************************************/
	IVS_INT32 GetCameras(const IVS_INDEX_RANGE* pQueryRange,
		                IVS_CAMERA_BRIEF_INFO_LIST* pList,
						IVS_UINT32 uiBufferSize);

    // ��ѯ������б�
    IVS_INT32 GetVideoChannelList(const IVS_INDEX_RANGE* pQueryRange, 
		IVS_VOID* pList, 
		IVS_UINT32 uiBufferSize);

	// ��ѯָ�����������б�
	IVS_INT32 GetDomainVideoChannelList(const IVS_CHAR* pDomainCode,
		const IVS_INDEX_RANGE* pQueryRange,
		IVS_VOID* pList,
		IVS_UINT32 uiBufferSize);

    // ��ѯ�澯�豸�б�
    IVS_INT32 GetAlarmChannelList(const IVS_INDEX_RANGE* pQueryRange,
                        IVS_ALARM_BRIEF_INFO_LIST* pList,
                        IVS_UINT32 uiBufferSize,
                        IVS_UINT32 uiAlarmType);

	// ��ѯָ����ĸ澯�豸�б�
	IVS_INT32 GetDomainAlarmChannelList(const IVS_CHAR* pDomainCode,
		const IVS_INDEX_RANGE* pQueryRange,
		IVS_ALARM_BRIEF_INFO_LIST* pList,
		IVS_UINT32 uiBufferSize,
		IVS_UINT32 uiAlarmType);

	//�ӻ����л�ȡ�豸��Ϣ
    IVS_INT32 GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_VOID* pList);
	IVS_INT32 GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, 
		IVS_VOID* pList, 
		const CAMERA_BRIEF_INFO_ALL_VEC &vecCameraDev);

	//���������е�һ���ڵ㵽�û��ڴ���
	IVS_VOID CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, IVS_CAMERA_BRIEF_INFO& CamInfo) const;
	IVS_VOID CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, IVS_CAMERA_BRIEF_INFO_EX& CamInfoEx) const;
	IVS_VOID CopyCamInfoFromCache(const IVS_CAMERA_BRIEF_INFO_ALL& CamInfoALL, IVS_SHADOW_CAMERA_BRIEF_INFO& CamInfoShadow) const;

    IVS_INT32 GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList);
	IVS_INT32 GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList, ALARM_BRIEF_INFO_VEC &vecAlarmDev);

	IVS_INT32 ParseChannelListRsp(const IVS_CHAR* pData, IVS_UINT32 uiChannelType, const IVS_INT32 iExDomainFlag, 
		const IVS_CHAR* pTargetDomainCode, IVS_PAGE_INFO& pageInfo, IVS_UINT32& uiItemSize, IVS_INT32 iGetChannelType);

    IVS_INT32 ProcessGetUserChannelListRsp(IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus, const IVS_CHAR* pData);

    void SetChannelListRetriveStatusValue(IVS_UINT32 uiChannelType, 
                    const IVS_CHAR* pTargetDomainCode, 
                    IVS_INT32 iExDomainFlag, 
                    IVS_UINT32 uiRealNum,
                    IVS_INT32 emThreadStatus,
					IVS_INT32 iGetChannelType);

	//��ʼ���豸����״̬��Ϣ�б�
	IVS_INT32 InitChannelRetriveStatusInfoList(DOMAIN_VEC &vecDomainRout,
		RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
		VOS_Mutex* pMutex,
		THREAD_RETRIVE_STATUS &eThreadStatus,
		IVS_DEVICE_TYPE eDeviceType) const;

	//�Ƴ��豸����״̬��Ϣ�б���ָ�������Ľڵ�
	IVS_INT32 RemoveChannelRetriveStatusInfoList(const std::string &strTargetDomainCode,
		RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
		VOS_Mutex* pMutex,
		IVS_INT32 iExDomainFlag) const;

	//�����豸����״̬��Ϣ�б���ָ�������Ľڵ�
	IVS_INT32 FindChannelRetriveStatusInfoList(const std::string &strTargetDomainCode,
		RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
		VOS_Mutex* pMutex,
		IVS_INT32 iExDomainFlag,
		IVS_CHANNLELIST_RETRIVE_STATUS_INFO &retriveStatusInfo) const;

	//�����豸����״̬��ϢΪready
	IVS_INT32 SetChannelRetriveStatusReady(RETRIVE_STATUS_INFO_LIST &listRetriveStatusInfo,
		VOS_Mutex* pMutex,
		THREAD_RETRIVE_STATUS &eThreadStatus) const;

	/******************************************************************
	function : GetAudios
	description: ��ȡ�����豸�б�
	input : const IVS_INDEX_RANGE * pQueryRange����ҳ��Ϣ
	input : IVS_AUDIO_BRIEF_INFO_LIST * pList�������豸�б���Ϣ
	input : IVS_UINT32 uiBufferSize�������ڴ��С
	output ��NA
	return : �ɹ�����0��ʧ�ܷ��ظ���������
	*******************************************************************/
	IVS_INT32 GetAudios(const IVS_INDEX_RANGE* pQueryRange,
						IVS_AUDIO_BRIEF_INFO_LIST* pList,
						IVS_UINT32 uiBufferSize);
	/******************************************************************
	function : GetAlarms
	description: ��ȡ�澯�豸�б�
	author: fengjiaji f00152768
	input : const IVS_INDEX_RANGE * pQueryRange����ҳ��Ϣ
	input : IVS_UINT32 uiBufferSize�������ڴ��С
	output : IVS_ALARM_BRIEF_INFO_LIST * pList���澯�豸�б���Ϣ
	return : �ɹ�����0��ʧ�ܷ��ظ���������
	*******************************************************************/
	//IVS_INT32 GetAlarms(const IVS_INDEX_RANGE* pQueryRange,
	//					IVS_ALARM_BRIEF_INFO_LIST* pList,
	//					IVS_UINT32 uiBufferSize);
						

    /*****************************************************************
         function   : GetMainDeviceList
         description: ��ȡ���豸�б�
         input     : const IVS_CHAR * pDomainCode�������
         input     : const IVS_QUERY_UNIFIED_FORMAT * pQueryFormat����ѯͨ�ø�ʽ
         input     : IVS_DEVICE_BRIEF_INFO_LIST * pBuffer���豸��ϸ��Ϣ�б�
         input     : IVS_UINT32 uiBufferSize���ڴ��С
		 output    : NA
         return    : �ɹ�����0��ʧ�ܷ��ظ���������
    *****************************************************************/
    IVS_INT32 GetMainDeviceList(const IVS_CHAR* pDomainCode,
								const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                                IVS_DEVICE_BRIEF_INFO_LIST* pBuffer, 
                                IVS_UINT32 uiBufferSize);

    /*****************************************************************
         function   : GetDeviceInfo
         description: ��ѯ���豸��ϸ��Ϣ
         input     : const IVS_CHAR * pDomainCode�������
         input     : const IVS_CHAR * pDevCode���豸����
         input     : IVS_DEVICE_INFO * pDeviceInfo�����豸��ϸ��Ϣ
		 output    ��NA
         return    : �ɹ�����0��ʧ�ܷ��ظ���������
    *****************************************************************/
    IVS_INT32 GetDeviceInfo(const IVS_CHAR* pDomainCode,
                            const IVS_CHAR* pDevCode, 
                            const IVS_DEVICE_INFO* pDeviceInfo)const;

    /*****************************************************************
         function   : GetChannelList
         description: �������豸��ѯ���豸�б�
         input     : const IVS_CHAR * pDevCode
         input     : IVS_DEV_CHANNEL_BRIEF_INFO_LIST * pChannelList
         input     : IVS_UINT32 uiBufferSize
		 output    ��NA
         return    : �ɹ�����0��ʧ�ܷ��ظ���������
    *****************************************************************/
    IVS_INT32 GetChannelList(const IVS_CHAR* pDevCode,
                             IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList, 
                             IVS_UINT32 uiBufferSize)const;

	/*****************************************************************
	     function   : SetMainDeviceName
	     description: �޸����豸����
	     input      : const IVS_CHAR * pDevCode���豸����
	     input      : const IVS_CHAR * pNewDevName�����豸����
		 output     ��NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ���������
	*****************************************************************/
	IVS_INT32 SetMainDeviceName(const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName)const;

	/*****************************************************************
	     function   : SetCameraName
	     description: �޸����������
	     input      : const IVS_CHAR * pCameraCode�����������
	     input      : const IVS_CHAR * pNewCameraName�����豸����
		 output     ��NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ���������
	*****************************************************************/
	IVS_INT32 SetCameraName(const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName)const;

    /******************************************************************
     function   : GetCameraBriefInfobyCode
     description: �����������Ż�ȡ�������Ҫ��Ϣ;
     input      : const std::string & strCameraCode         ���������
				  bHasDomainCode ��ѯ������������Ƿ�������룬Ĭ��û��
     output     : IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo  �������Ҫ��Ϣ
     return     : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
    IVS_INT32 GetCameraBriefInfobyCode(const std::string& strCameraCode, IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo, bool bHasDomainCode = false);



    IVS_INT32 UpdateNVRListToCache(const IVS_CHAR* pDomainCode, std::list<IVS_DEVICE_BRIEF_INFO>& nvrInfoList, const bool bCacheClearFlag);

	/*****************************************************************
	     function   : GetNVRList
	     description: ��ѯ��Ԫ�б�
	     input      : const IVS_CHAR * pDomainCode�������
	     input      : IVS_UINT32 uiServerType������������
	     input      : const IVS_INDEX_RANGE * pIndex����ҳ��Ϣ
	     input      : IVS_DEVICE_BRIEF_INFO_LIST * pNvrList���豸��Ϣ�б�
	     input      : IVS_UINT32 uiBufferSize�������ڴ��С
		 output     ��NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ��������� 
	*****************************************************************/
	IVS_INT32 GetNVRList(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType,const IVS_INDEX_RANGE* pIndex,IVS_DEVICE_BRIEF_INFO_LIST* pNvrList,
		IVS_UINT32 uiBufferSize);

	/******************************************************************
     function   : GetClusterList
     description: ��ѯ��Ⱥ��Ϣ�б�;
     input      : 
     output     : NA
     return     : �ɹ�����0��ʧ�ܷ��ظ��������� 
    *******************************************************************/
    IVS_INT32 GetClusterList(const IVS_CHAR* pDomainCode,  IVS_CLUSTER_INFO_LIST* pClusterList);

	 /******************************************************************
     function   : GetFirstNVR
     description: ����������ҵ���һ��NVR���������Ƶ��б�β��;
     input      : const std::string & strDomainCode            �����;
     input      : IVS_DEVICE_BASIC_INFO & stDeviceBasicInfo    NVR��Ϣ;
     output     : NA
     return     : �ɹ�����0��ʧ�ܷ��ظ��������� 
    *******************************************************************/
    IVS_INT32 GetFirstNVR(const std::string& strDomainCode, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo);

    /******************************************************************
     function   : GetNVRSize
     description: ����������ȡlist����;
     input      : const std::string & strDomainCode
     output     : NA
     return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
    *******************************************************************/
    IVS_UINT32 GetNVRSize(const std::string& strDomainCode);

	/*****************************************************************
	     function   : GetNVRInfoByDomainCodeAndNVRCode
	     description: ����������NVRCode��ȡNVR��Ϣ
	     input      : const std::string & strDomainCode�������
	     input      : const std::string & strNVRCode��NVR����
	     input      : IVS_DEVICE_BASIC_INFO & stDeviceBasicInfo��ͨ���豸��Ϣ
		 output     : NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 GetNVRInfoByDomainCodeAndNVRCode(const std::string& strDomainCode,const std::string& strNVRCode,IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo);

	//��ȡ���е�NVR��Ϣ,�ӿڸ��ڲ���¼ʹ�ã�����һ�β�ѯ������浽Map����
	/*****************************************************************
	     function   : GetAllNVRInfo
	     description: ��ȡ���е�NVR��Ϣ
	     input      : const IVS_CHAR * pDomainCode�������
	     input      : IVS_UINT32 uiServerType������������
		 output     : NA
	     return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 GetAllNVRInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType);

	/*****************************************************************
	     function   : DeleteNVRListByDomainCode
	     description: ���������ɵ���Ӧ���б�
	     input      : const std::string & strDomainCode�������
		 output     : NA
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 DeleteNVRListByDomainCode(const std::string& strDomainCode);

	/*****************************************************************
	     function   : ModifyDevStatusByDomainCode
	     description: ����״̬
	     input      : const std::string & strDomainCode�������
	     input      : const std::string & strCameraCode���豸����
	     input      : const IVS_UINT32 & uiStatus��״̬
	     input      : const std::string & strNVRCode����Ԫ����
		 output     : NA
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 ModifyDevStatusByDomainCode(const std::string& /*strDomainCode*/, const std::string& strCameraCode, const IVS_UINT32& uiStatus, const std::string& strNVRCode);

	/*****************************************************************
	     function   : RestartDevice
	     description: ����ǰ���豸
	     input      : const IVS_CHAR * pDeviceCode���豸����
					  uiRebootType  �������ͣ�0-���� 1-�ָ�Ĭ�����ò�����
		 output     : NA
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 RestartDevice(const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType)const;

	/*****************************************************************
	     function   : StartSearchDevice
	     description: ��������ǰ���豸
	     input      : const IVS_CHAR* pDomainCode Ŀ�������
						const IVS_CHAR * pSearchInfo �豸������ϢXML
		 output     : NA
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 StartSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);
	IVS_INT32 StartSearchDeviceEX(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	IVS_INT32 FindIdlestNvr(const IVS_CHAR* pDomainCode, const std::string& strClusterCode, std::string& strIdleNvrCode)const;
	/*****************************************************************
	     function   : StopSearchDevice
	     description: ֹͣ����ǰ���豸
	     input      : const IVS_CHAR* pDomainCode Ŀ�������
					    const IVS_CHAR * pSearchInfo �豸������ϢXML
		 output     : NA
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 StopSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)const;

	/*****************************************************************
	     function   : VerifyDevice
	     description: ��֤ǰ���豸
	     input      : const IVS_CHAR* pDomainCode Ŀ�������
						const IVS_CHAR * pVerifyInfo �豸��֤��ϢXML
		 output     : IVS_CHAR** pRspXml           ��֤���XML 
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 VerifyDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)const;
	IVS_INT32 VerifyDeviceEX(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml);

	/*****************************************************************
	     function   : GetDevLog
	     description: ��ѯǰ����־
	     input      : const IVS_CHAR * pQueryXml   ��ѯ����XML
		 output     : IVS_CHAR** pRspXml           ��֤���XML 
		 return     : �ɹ�����0��ʧ�ܷ��ظ��������� ;
	*****************************************************************/
	IVS_INT32 GetDevLog(const IVS_CHAR* pQueryXml, IVS_CHAR** pRspXml)const;

	/***********************************************************************************
    * name       : GetRecordTask
    * description: ��ѯ��ǰ�û����ֶ�¼������
    * input      : pRspXml:��ǰ�û����ֶ�¼��������Ϣ
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordTask(const IVS_UINT32 uiUserId,CDeviceMgr &m_DeviceMgr,IVS_CHAR** pRspXml);

	/******************************************************************
    function : GetDeviceCapability
    description: ��ѯ�������������
    input  : uiCapabilityType  �������ͣ�IVS_DEVICE_CAPABILITY_TYPE��DEVICE_ENCODE_CAPABILITY
    input  : pDevCode          �豸����#�����
	output : pRspXml           ��ѯXML
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 GetDeviceCapability(IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, IVS_CHAR** pRspXml)const;

	// ��ȡ�����Ԥ��ͼ�ӿ�
	IVS_INT32 GetCameraThumbnail(const IVS_CHAR* pCameraCode, IVS_CHAR* pThumbnailURL,IVS_UINT32 uiBufferSize)const;

	/******************************************************************
    function : SetDevPWD
    description: ����ǰ���豸����
    input  : const IVS_CHAR* pDevCode��            �豸����
    input  : const IVS_CHAR* pDevUserName          �豸��¼�û���
	input  : const IVS_CHAR* pDevPWD               �豸��¼����
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 SetDevPWD(const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD);

	/******************************************************************
    function : ImportDevCfgFile
    description: ����ǰ�������ļ�
    input  : const IVS_CHAR* pDevCode��            �豸����
    input  : const IVS_CHAR* pCFGFile              �����ļ�
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 ImportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile);

	/******************************************************************
    function : ExportDevCfgFile
    description: ����ǰ�������ļ�
    input  : const IVS_CHAR* pDevCode��                �豸����
    input  : const IVS_CHAR* pCFGFilePath              �����ļ�ȫ·��
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 ExportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFilePath);

	/******************************************************************
    function : ExGetDeviceStatus
    description: �豸״̬��ѯ
	input : pReqXml,����XML
	output : pRspXml ��ӦXML
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 ExGetDeviceStatus(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const;

	/******************************************************************
    function : ExGetDeviceInfo
    description: �豸��Ϣ��ѯ
	input : pReqXml,����XML
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 ExGetDeviceInfo(const IVS_CHAR* pReqXml)const;

	/******************************************************************
    function : GetCameraEncodeCapabilities
    description: Onvif�豸��ǰ�˻�ȡ����������Χ
	input : pCameraCode,��ѯ��������������
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
	IVS_INT32 GetDeviceEncodeCapability(const IVS_CHAR* pCameraCode,
		IVS_STREAM_INFO_LIST* pStreamInfoList)const;

    /******************************************************************
    function : GetCameraList
    description: ˢ��������б���SMU�·�ͬ���¼���������б�
	input : iSessionID
    return : �ɹ�����0��ʧ�ܷ��ظ���������
    *******************************************************************/
    IVS_INT32 RefreshCameraList(IVS_INT32 iSessionID);

	IVS_INT32 GetNvrListByClusterCode(std::string strClusterCode, std::list<std::string>& NvrList);

	void GetClusterCodeByNvrCode(std::string strNvrCode, std::string& strClusterCode);

	// ����Ӱ���豸
	IVS_INT32 AddShadowDev(const IVS_CHAR* pDomainCode, IVS_SHADOW_IPC_INFO& ShadowDevInfo);
	// Ӱ���豸�ֶ�ͬ��
	IVS_INT32 ShadowDevSyncNotify(const IVS_SHADOW_IPC_LIST* pDevList);
	// �ж��Ƿ�Ӱ���豸
	bool IsShadowDev(const IVS_CHAR* pDevCode)const;
	// ��ȡӰ���豸��Դ�豸��������
	IVS_INT32 GetOrigDevInfo(const IVS_CHAR* pDevCode, std::string& strOrigDevDomain, std::string& strOrigDevCode)const;

	int GetAccessProtocolByCameraCode(const std::string  strCameraCode);

	IVS_INT32 GetRecordTaskByChannel(const IVS_UINT32 uiUserId, CDeviceMgr &m_DeviceMgr,  IVS_CHAR** pRspXml);

private:
 
    /******************************************************************
    function : CheckBufferSize
    description: ������ռ��Ƿ����
    input : const IVS_UINT32 & uiDevType:�豸����
    input : const IVS_UINT32 & iNum:�豸����
	input : const IVS_UINT32 & uiBufferSize:�����ڴ��С
    output : NA
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 CheckBufferSize(const IVS_UINT32 &uiDevType, const IVS_UINT32 &iNum, const IVS_UINT32& uiBufferSize);

	/******************************************************************
    function : GetCamerasInfo
    description: ��ȡ������б�ṹ�帳ֵ
    input : IVS_UINT32 uiSize:ѭ������
    input : IVS_CAMERA_BRIEF_INFO_LIST* pList:��Ƶ���豸�б�
	input : const IVS_INDEX_RANGE* pQueryRange:��ҳ��Ϣ
    output : NA
    return : IVS_INT32
    *******************************************************************/
	// IVS_INT32 GetCamerasInfo(IVS_UINT32 uiSize, IVS_CAMERA_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	/******************************************************************
    function : GetAudiosInfo
    description: ��ȡ�����б�ṹ�帳ֵ
    input : IVS_UINT32 uiSize:ѭ������
    input : IVS_AUDIO_BRIEF_INFO_LIST* pList:�����豸�б���Ϣ
	input : const IVS_INDEX_RANGE* pQueryRange:��ҳ��Ϣ
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetAudiosInfo(IVS_UINT32 uiSize, IVS_AUDIO_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	/******************************************************************
    function : GetAudiosInfo
    description: ��ȡ�澯�豸�б�ṹ�帳ֵ
    input : IVS_UINT32 uiSize:ѭ������
    input : IVS_ALARM_BRIEF_INFO_LIST* pList:�澯�豸�б���Ϣ
	input : const IVS_INDEX_RANGE* pQueryRange:��ҳ��Ϣ
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetAlarmsInfo(IVS_UINT32 uiSize, IVS_ALARM_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	// �ѵ��������豸
	IVS_INT32 AddDevicePile(const IVS_CHAR* pDomainCode, 
		IVS_UINT32 uiDeviceNum, 
		const IVS_DEVICE_OPER_INFO* pDevList, 
		IVS_DEVICE_OPER_RESULT_LIST* pResultList)const;

	// ��Ⱥ�������豸
	IVS_INT32 AddDeviceCluster(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_OPER_INFO* pDevList, IVS_DEVICE_OPER_RESULT_LIST* pResultList)const;

	// �ѵ��������豸����
	IVS_INT32 SetDevPWDPile(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const;

	// ��Ⱥ�������豸����
	IVS_INT32 SetDevPWDCluster(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const;

	// ��Ⱥ�������豸������Կ������Ϣ����
	IVS_INT32 SetDevPWDClusterMsg(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD, NSS_MSG_TYPE_E msgType, const char* pszDestModule, const char * pszDeviceCode = "")const;

	// ��֤ǰ���豸��Ϣ
	IVS_INT32 VerifyDeviceMsg(const IVS_CHAR* pDomainCode, const IVS_CHAR* pServerCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml, enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE)const;

	bool IsVirtualCode(std::string strCameraCode) const;

	/******************************************************************
    function	: RefreshALLCameraCache
    description	: ˢ�»��棬���»�ȡ��������豸��Ϣ
    input		: NA
    output		: uiMsgSendSucCount	��ȡ�豸�б�������Ϣ���ͳɹ�������
    return		: IVS_INT32
    *******************************************************************/
	IVS_INT32 RefreshALLCameraCache(IVS_UINT32& uiMsgSendSucCount);
	IVS_INT32 RefreshDomainCameraCache(const IVS_CHAR *pDomainCode, IVS_UINT32& uiMsgSendSucCount);

public:

	/******************************************************************
	function : GetDomainList
	description: ��ȡ���б�
	author: fengjiaji f00152768
	input : NA
	output : DOMAIN_VEC& DmList
	return : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetDomainListByUserID(DOMAIN_VEC& dmList)const;

	/******************************************************************
	function			: GetVideoDiagnose
	description	: ��Ƶ������ϼƻ���ѯ
	author			: zwx211831
	input				: pDomainCode �����
	input				: pCameraCode ���������
	output			: pRspXml ��̨VCN����Ƶ������ϼƻ�xml
	return				: IVS_INT32
	*******************************************************************/
	IVS_INT32 GetVideoDiagnose(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_CHAR** pRspXml)const;

	/******************************************************************
	function			: SetVideoDiagnose
	description	: ��Ƶ������ϼƻ�����
	author			: zwx211831
	input				: pReqXml ��̨VCN����Ƶ������ϼƻ�xml
	output			: NA
	return				: IVS_INT32
	*******************************************************************/
	IVS_INT32 SetVideoDiagnose(const IVS_CHAR* pReqXml)const;


	//������б���س�Ա����	
	DOMAIN_VEC m_vecCameraDomain;//��������б�

private:
	CUserMgr   *m_pUserMgr;
	IVS_BOOL   bIsOutAlarm; // 0 ��������澯�豸 1������澯�豸   

	//��Ų�ѯNVR/TAS�豸�б��Map
    NVR_INFO_MAP m_NvrInfoMap;
	//DEVICE_BRIEF_INFO_LIST_MAP m_DeviceInfoListMap;
	VOS_Mutex* m_pMutexNVRLock;


    std::map<std::string, IVS_CLUSTER_INFO> m_CLusterInfoMap;
 //   VOS_Mutex* m_pMutexClusterLock;

	//��Ų�ѯ�ֶ�¼��������Ϣ��Map
	DEVICE_RECORD_TASK_CAMERA_INFO_MAP m_DeviceRecordTaskCameraInfoMap;
	VOS_Mutex* m_pMutexRecordTaskLock;
	enum THREAD_STATUS
	{
		eM_Thread_Ready=0,//�߳�׼��
		eM_Thread_Init,//�̳߳�ʼ��
		eM_Thread_ExceptionRet,//�߳����쳣����
		eM_Thread_DataIsReving,//�߳����ڽ�������
		eM_Thread_DataIsReady//�߳�ȫ�������������
	};


	//��ȡ�豸�б�����
	enum GET_CHANNEL_TYPE
	{
		eM_Get_Device_All,							// ��ȡ�����豸�б�
		eM_Get_Device_ByDomainCode					// ��ȡָ��������豸�б�
	}; 

	// ������б���س�Ա����	
	CAMERA_BRIEF_INFO_ALL_VEC m_vecAllCameraDev;	// ������豸ȫ����Ϣ����
    IVS_UINT32 m_uiVideoChannelTotalNum;			// ������豸����
	VOS_Mutex* m_pMutexCameraLock;					// ������б����������	
    THREAD_RETRIVE_STATUS m_VideoChannelRetriveThreadStatus;								// �߳�״̬
    std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_VideoChannelRetriveStatusInfoList;		// ��¼ÿ������Ӧ״̬
    VOS_Mutex* m_pVideoRetriveStatusInfoListMutex;											// �߳�״̬���������

	CAMERA_BRIEF_INFO_ALL_VEC m_vecAllCamDevByDom;	// ָ������������������б�
	IVS_UINT32 m_uiVideoChannelTotalNumByDom;		// ָ��������������б�����
	THREAD_RETRIVE_STATUS m_VideoChannelRetriveThreadStatusByDom;							// �߳�״̬
	std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_VideoChannelRetriveStatusInfoListByDom;// ��¼ÿ������Ӧ״̬
	VOS_Mutex* m_pVideoRetriveStatusInfoListMutexByDom;										// �߳�״̬���������

    // �澯�豸�б���س�Ա����
    ALARM_BRIEF_INFO_VEC m_vecAllAlarmDev;			// �澯�豸ȫ����Ϣ����
    IVS_UINT32 m_uiAlarmChannelTotalNum;			// �澯�豸����
    VOS_Mutex* m_pMutexAlarmLock;					// �澯�б����������	
    THREAD_RETRIVE_STATUS m_AlarmChannelRetriveThreadStatus;								// �߳�״̬
    std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_AlarmChannelRetriveStatusInfoList;		// ��¼ÿ������Ӧ״̬
    VOS_Mutex* m_pAlarmRetriveStatusInfoListMutex;											// �߳�״̬���������

	ALARM_BRIEF_INFO_VEC m_vecAllAlarmDevByDom;				// ָ�����������и澯�豸�б�
	IVS_UINT32 m_uiAlarmChannelTotalNumByDom;				// ָ���������豸�б�����
 
	IVS_DEVICE_TYPE m_GetDeviceInfoType;					// ��ȡ�豸��Ϣ����

	//VOS_Thread* m_pGetCamerasThread; //��ȡ������б��߳�ָ��	
	//static long __stdcall GetCamerasThreadEntry(void* pArg);// ��ȡ������б��߳����
	//THREAD_STATUS mGetCamerasThreadStatus;//GetCamerasThreadEntry�߳�״̬
	//IVS_INT32 iThreadGetCameraRet;//GetCamerasThreadEntry�̷߳���ֵ

	////�澯�豸�б���س�Ա����
	//DOMAIN_VEC m_vecAlarmDomain;//�澯�豸���б�
	//ALARM_BRIEF_INFO_VEC m_vecAllAlarmDev;
	//VOS_Mutex* m_pMutexAlarmLock;//�澯�豸�б����������	
	//VOS_Thread* m_pGetAlarmsThread; //��ȡ�澯�б��߳�ָ��	
	//static long __stdcall GetAlarmsThreadEntry(void* pArg);// ��ȡ�澯�豸�б��߳����
	//THREAD_STATUS mGetAlarmsThreadStatus;//GetAlarmsThreadEntry�߳�״̬
	//IVS_INT32 iThreadGetAlarmRet;//GetAlarmsThreadEntry�̷߳���ֵ

	////�����豸�б���س�Ա����
	//DOMAIN_VEC m_vecAudioDomain;//�����豸���б�
	//AUDIO_BRIEF_INFO_VEC m_vecAllAudioDev;
	//VOS_Mutex* m_pMutexAudioLock;//�����豸�б����������	
	//VOS_Thread* m_pGetAudiosThread; //��ȡ�豸�б��߳�ָ��	
	//static long __stdcall GetAudiosThreadEntry(void* pArg);// ��ȡ�����豸�б��߳����
	//THREAD_STATUS mGetAudiosThreadStatus;//GetAudiosThreadEntry�߳�״̬
	//IVS_INT32 iThreadGetAudioRet;//GetAudiosThreadEntry�̷߳���ֵ

	//static bool m_bSendLocalCmdFail;      //���ͱ���������Ϣʧ��
	//static bool m_bSendExdomainCmdFail;   //��������������Ϣʧ��
};

#endif
