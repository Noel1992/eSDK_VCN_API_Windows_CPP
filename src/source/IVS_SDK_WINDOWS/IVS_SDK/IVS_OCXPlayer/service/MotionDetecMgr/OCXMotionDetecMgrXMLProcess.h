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
	filename	: 	OCXMotionDetecMgrXMLProcess.h
	author		:	wWX155351
	created		:	2012/12/03	
	description	:	ƴװ�ͽ����ƶ�����xml
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/12/03 ��ʼ�汾
*********************************************************************/

#ifndef __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__
#define __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__

#include "OCXXmlProcess.h"

class COCXMotionDetecMgrXMLProcess
{
public: 
	COCXMotionDetecMgrXMLProcess();
	~COCXMotionDetecMgrXMLProcess();

public:
	//����ƽ̨�ƶ����xml
	static IVS_INT32 ParseSetMotionDetecXml(IVS_MOTION_DETECTION* pMotionDetection, CXml& xml);

	/*****************************************************************
	     function   : GetMotionDetecAreaInfoNum
	     description: ��ȡ�˶�������Ӧ��Ϣ��AreaInfo������
	     input      : CXml & xml   ��Ӧ��Ϣxml
	     output     : IVS_INT32 & iNum  AreaInfo������
	     return     : AreaInfo������
	*****************************************************************/
	static IVS_INT32 GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum);

	// ƴװƽ̨�ƶ������Ӧ��Ϣxml
	static IVS_INT32 SetMotionDetecGetXML(const IVS_MOTION_DETECTION* pMotionDetec, CXml& xml);

	/*****************************************************************
	     function   : ParseGetMotionRangeDataPlatformXml
	     description: ������ѯƽ̨�˶�����������xml
	     input      : pStartTime����ѯ��ʼʱ�䣻
		              pEndTime����ѯ��ֹʱ�䣻
					  pMotionDetection����ѯƽ̨�˶����������ݽṹ�壻
					  iRet������룻
	     output     : CXml & xml   ��Ӧ��Ϣxml
	     return     : �����
	*****************************************************************/
	static IVS_INT32 ParseGetMotionRangeDataPlatformXml(const IVS_CHAR* pStartTime,const IVS_CHAR* pEndTime,
		const IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_INT32 iRetCode, CXml& xml);
};
#endif // __OCX_MOTIONDETECTION_MGR_XML_PROCESS_H__
