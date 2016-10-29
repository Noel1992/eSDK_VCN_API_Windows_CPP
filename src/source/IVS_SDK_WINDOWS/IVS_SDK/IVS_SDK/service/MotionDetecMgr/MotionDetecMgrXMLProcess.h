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
filename	: 	MotionDetecMgrXMLProcess.h
author		:	z00201790
created		:	2012/12/06	
description	:	������ƴװ�ƶ�������xml����;
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/06 ��ʼ�汾;
*********************************************************************/
#ifndef __MOTIONDETECTION_MGR_XML_PROCESS_H__
#define __MOTIONDETECTION_MGR_XML_PROCESS_H__

#include "XmlProcess.h"
#include "SDKDef.h"

class CMotionDetecMgrXMLProcess
{
private:
	CMotionDetecMgrXMLProcess();
public:
	~CMotionDetecMgrXMLProcess();

public:
	/***********************************************************************************
    * function       : GetMotionDetecXML
	* description: ƴװ��ѯ�˶���������xml
	* input      : pLoginId,��¼Id��pCameraCode����������룻pDomainCode�������
	* output     : xml�������Ŀ��xml
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 GetMotionDetecXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, CXml& xml);

	/***********************************************************************************
    * function       : ParseMotionDetecXML
	* description: �����˶�������Ӧ��Ϣxml
	* input      : xml,��Ӧ��Ϣxml
	* output     : pMotionDetection���˶����ṹ��
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 ParseMotionDetecXML(CXml& xml, IVS_MOTION_DETECTION* pMotionDetection);

	/***********************************************************************************
    * function       : GetMotionDetecAreaInfoNum
	* description: �����˶�������Ӧ��Ϣ��AreaInfo������
	* input      : xml,��Ӧ��Ϣxml
	* output     : iNum��AreaInfo������
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum);

	/***********************************************************************************
    * function       : SetMotionDetecGetXML
	* description: ƴװ�޸��˶���������xml
	* input      : pLoginId,��¼Id��pCameraCode����������룻pDomainCode�������
	               pMotionDetection, �˶����ṹ��
	* output     : xml�������Ŀ��xml
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 SetMotionDetecGetXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, const IVS_MOTION_DETECTION* pMotionDetection, CXml& xml);
	
	/***********************************************************************************
    * function   : GetMotionRangeDataPlatformReqXML
	* description: ƴװ��ѯ�ƶ����������ݵ�����xml
	* input      : pCameraCode����������룻pDomainCode�������
	               pTimeSpan, ʱ�����ṹ�壻pIndexRange����ҳ��Ϣ�ṹ��
	* output     : xml�������Ŀ��xml
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 GetMotionRangeDataPlatformReqXML(const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,CXml& xml);

	/***********************************************************************************
    * function   : ParseGetMotionRangeDataPlatformRspXML
	* description: ������ѯ�ƶ����������ݵ���Ӧxml
	* input      : uiBufferSize:��Ӧ�������ռ��С��
				   RspXml�������Ŀ��xml
	* output     : pMotionRangeData���ƶ����������ݵ���Ӧ��Ϣ�ṹ��
	* return     : �ɹ�����0��ʧ�ܷ��ش�����;
    ***********************************************************************************/
	static IVS_INT32 ParseGetMotionRangeDataPlatformRspXML(CXml &RspXml, IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize);
	
};

#endif //__MOTIONDETECTION_MGR_XML_PROCESS_H__
