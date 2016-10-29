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

#ifndef __CCRITICAL_SECTIONEX_H__
#define __CCRITICAL_SECTIONEX_H__

#include "vos.h"

//定义自己的临界区类，直接在init中使用CCRITICAL_SECTION将不可避免多次初始化;
class CCRITICAL_SECTIONEX
{
public:
	CCRITICAL_SECTIONEX();
	virtual ~CCRITICAL_SECTIONEX();
	void EnterCriticalSectionEx();
	void LeaveCriticalSectionEx();
protected:

	VOS_Mutex *m_pMutex;
};

#endif