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
filename	: 	MotionDetection.h
author		:	z00201790
created		:	2012/12/06	
description	:	�����ƶ���������;
copyright	:	Copyright (C) 2011-2015
history		:	2012/12/06 ��ʼ�汾;
*********************************************************************/

#ifndef __MOTIONDETECTION_H__
#define __MOTIONDETECTION_H__

#include "hwsdk.h"
#include "ivs_error.h"


class CUserMgr;
class CMotionDetection
{
public:
	CMotionDetection(void);
	~CMotionDetection(void);

	// �����û�������
	void SetUserMgr(void *pUserMgr);

private:
	CUserMgr *m_pUserMgr;

public:
	
	// ��ѯƽ̨�˶�����������
	IVS_INT32 GetMotionRangeDataPlatform(const IVS_CHAR* pCameraCode,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize);
};

#endif // __MOTIONDETECTION_H__
