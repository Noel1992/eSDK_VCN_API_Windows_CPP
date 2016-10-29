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
 filename    :    OCXDeviceGroupMgr.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    ʵ���豸��;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 ��ʼ�汾
*****************************************************************/
#ifndef __OCX_DEVICE_GROUP_MGR_H__
#define __OCX_DEVICE_GROUP_MGR_H__

#include <afx.h>
#include "SDKDef.h"


class COCXDeviceGroupMgr
{
public:
    ~COCXDeviceGroupMgr();

	// �����豸��
    static IVS_VOID AddDeviceGroup(IVS_INT32 iSessionID, CXml& xmlReq, CString& strResult);

	// ��ѯ�豸���б���Ϣ
    static IVS_VOID GetDeviceGroup(IVS_INT32 iSessionID, CXml& xmlReq, CString& strResult);

	// ɾ���豸��
    static IVS_INT32 DeleteDeviceGroup(IVS_INT32 iSessionID, CXml& xmlReq);

	// �ƶ��豸��
    static IVS_INT32 MoveDeviceGroup(IVS_INT32 iSessionID, CXml& xmlReq);

	// �޸��豸������
    static IVS_INT32 ModifyDeviceGroupName(IVS_INT32 iSessionID, CXml& xmlReq);

	// ɾ���豸�������
    static IVS_INT32 DeleteDeviceFromGroup(IVS_INT32 iSessionID, CXml& xmlReq);

	// �����豸�������
    static IVS_INT32 AddDeviceToGroup(IVS_INT32 iSessionID, CXml& xmlReq);

    // �����豸�鱸ע��Ϣ
	static IVS_INT32 SetDevGroupRemark(IVS_INT32 iSessionID, CXml& xmlReq);

	// ��ѯ�豸�鱸ע��Ϣ
	static IVS_VOID GetDevGroupRemark(IVS_INT32 iSessionID,  CXml& xmlReq, CString& strResult);
    
protected:
private:
    COCXDeviceGroupMgr();
   
};



#endif
