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
	filename	: 	AutoResume
	author		:	z00193167
	created		:	2012/12/27
	description	:	�Զ��ָ���
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/27 ��ʼ�汾;
*********************************************************************/
#ifndef AUTO_RESUME_H
#define AUTO_RESUME_H
 
#include "SDKTimer.h"
#include "SDKDef.h"
#include "RealPlay.h"
#include "RealPlayMgr.h"
#include "LockGuard.h"
#include "vos.h"

 class CAutoResume : public ITrigger
 {
 public:
 	CAutoResume(void);
 	~CAutoResume(void);
 
 	virtual void onTrigger(void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/);

 	/**************************************************************************
 	* name       : clearCamerInfoList()
 	* description: ��������б�
 	* input      : NA
 	* output     : NA
 	* return     : NA
 	* remark     : NA
 	**************************************************************************/
 	void clearCamerInfoList();
 
 	/**************************************************************************
 	* name       : RemoveStopPlayHandle
 	* description: �Ƴ������Ĳ��ž��
 	* input      : NA
 	* output     : NA
 	* return     : �����б�
 	* remark     : NA
 	**************************************************************************/
	void RemoveStopPlayHandle(IVS_ULONG ulHandle);

	/**************************************************************************
	* name       : AddCameraInfoList(CAMERAINFO_LIST CamerInfoList)
	* description: ����һ���б����е��б���
	* input      : CAMERAINFO_LIST CamerInfoList �б�
	* output     : NA
	* return     : true - ��ӳɹ� false - ���ʧ��
	* remark     : NA
	**************************************************************************/
 	bool AddCameraPlayInfo2List(const CAMERA_PLAY_INFO& cameraPlayInfo);


 private:
 	CAMERA_PLAY_INFO_LIST m_CamerPlayInfoList;				//�������Ϣ�б�
	VOS_Mutex* m_pstAutoResumeMutex;
 };
 
#endif //AUTO_RESUME_H
