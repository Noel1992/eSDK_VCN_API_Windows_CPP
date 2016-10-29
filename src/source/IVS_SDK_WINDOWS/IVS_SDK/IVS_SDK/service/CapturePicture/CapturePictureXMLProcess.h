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
	filename	: 	CapturePictureXMLProcess.h
	author		:	s00193168
	created		:	2013/1/24
	description	:	����ץ��XML������;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/1/24 ��ʼ�汾;
*********************************************************************/

#ifndef __CAPTUREPICTURE_XML_PROCESS_H__
#define __CAPTUREPICTURE_XML_PROCESS_H__

#include "XmlProcess.h"

class CapturePictureXMLProcess
{
public:
	/***********************************************************************************
    * function: PlatformSnapshotGetGenericXML
	* description: ƴװƽ̨�ֶ�ץ�������xml
	* input      : pLoginID ��¼ID
	               pCameraCode ���������
	* output     : xml  ƽ̨�ֶ�ץ�ĵ�xml
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 PlatformSnapshotGetGenericXML(const IVS_CHAR* pCameraCode,CXml &xml);

	/***********************************************************************************
    * function: SetSnapshotPlanGetXML
	* description: ƴװ����ƽ̨ץ�ļƻ������xml
	* input      : pCameraCode ���������
				   pSnapShotPlan ץ�ļƻ���Ϣ
	* output     : xml  ƽ̨�ֶ�ץ�ĵ�xml
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 SetSnapshotPlanGetXML(const IVS_CHAR* pCameraCode,const IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml &xml);

	/********************************************************************
	* name       : ParseGetSnapshotPlan
	* description: ������ѯƽ̨ץ�ļƻ���Ӧ��xml
	* input      : pSnapShotPlan��ƽ̨ץ�ļƻ���Ϣ�ṹ�壻 xml:����XML
	* output     : NA
	* return     : �ɹ�����0��ʧ�ܷ��ظ���������
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseGetSnapshotPlan(IVS_SNAPSHOT_PLAN* pSnapShotPlan,CXml& xml);


	/*****************************************************************
	     function   : GetSnapshotListGetXML
	     description: ƴװץ��ͼƬ��ѯ����xml
	     input      : const IVS_QUERY_SNAPSHOT_PARAM * pQueryParam  ץ��ͼƬ��ѯ����
	     output     : CXml & xml    ƴװץ��ͼƬ��ѯ������xml
	     return     : �ɹ�����0��ʧ�ܷ��ش�����;
	*****************************************************************/
	static IVS_INT32 GetSnapshotListGetXML(const IVS_CHAR* pCameraCode,const IVS_QUERY_SNAPSHOT_PARAM* pQueryParam, CXml &xml);

	/*****************************************************************
	     function   : GetSnapshotListParseXML
	     description: ����ץ��ͼƬ�б���Ϣxml
	     input      : CXml & xml   ץ��ͼƬ�б���Ϣ��xml
	     output     : IVS_SNAPSHOT_INFO_LIST * pSnapshotList   ץ��ͼƬ�б���Ϣ
	     output     : IVS_UINT32 uiBufSize  �����С
	     return     : �ɹ�����0��ʧ�ܷ��ش�����;
	*****************************************************************/
	static IVS_INT32 GetSnapshotListParseXML(CXml& xml, IVS_SNAPSHOT_INFO_LIST* pSnapshotList, IVS_UINT32 uiBufSize, const IVS_CHAR* pDomainCode);

	/*****************************************************************
	     function   : DelSnapshotPlanGetXml
	     description: ɾ��¼��ƻ�xml
	     input      : const IVS_CHAR * pCameraCode
	                  const IVS_CHAR * pDomainCode
	     output     : CXml & xml   ɾ��¼��ƻ�xml
	     return     :  �ɹ�����0��ʧ�ܷ��ش�����;
	*****************************************************************/
	static IVS_INT32 DelSnapshotPlanGetXml(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode,CXml& xml);


	/*****************************************************************
	     function   : DeleteSnapshotGetXML
	     description: ƴװɾ��ƽ̨ץ��ͼƬ�����xml
	     input      : const IVS_CHAR * pCameraCode  ���������
	     input      : IVS_UINT32 uiPictureID        ͼƬID
	     output     : CXml & xml                    ɾ��ƽ̨ץ��ͼƬxml
	     return     : �ɹ�����0��ʧ�ܷ��ش�����;
	*****************************************************************/
	static IVS_INT32 DeleteSnapshotGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID, CXml &xml);
};
#endif 
