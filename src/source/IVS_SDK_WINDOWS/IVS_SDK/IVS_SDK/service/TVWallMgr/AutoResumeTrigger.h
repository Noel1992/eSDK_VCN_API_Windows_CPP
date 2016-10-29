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
 filename    :    AutoResumeTrigger.h
 author      :    g00209332
 created     :    2013/1/7
 description :    电视墙自动恢复触发器类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/7 初始版本
*********************************************************************/
#ifndef _SDK_AUTO_RESUME_TRIGGER_H_
#define _SDK_AUTO_RESUME_TRIGGER_H_

#include "sdktimer.h"

class CTVWallMgr;//lint !e763
class CAutoResumeTrigger : public ITrigger
{
public:
    CAutoResumeTrigger(void);
    ~CAutoResumeTrigger(void);
    virtual void onTrigger(void * /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*enStyle*/);
	//设置电视墙管理对象
	void SetTVWallMgr(CTVWallMgr* pTVWallMgr);
private:
	CTVWallMgr* m_pTVWallMgr;		//电视墙管理对象
};

#endif	//_SDK_AUTO_RESUME_TRIGGER_H_
