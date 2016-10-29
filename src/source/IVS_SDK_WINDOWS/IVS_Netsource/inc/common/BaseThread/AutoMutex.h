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

/************************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :AutoMutex.h
Author         :邹清    z00003175
Description    :定义个自动互斥量
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/
#ifndef _AUTO_MUTEX_H_
#define _AUTO_MUTEX_H_

#include <WTypes.h>

class AutoMutex
{
public:
/************************************************************
Func Name    :AutoMutex
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :构造函数,将输入的互斥量句柄保存,并进入互斥量
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    AutoMutex(HANDLE hMutex);
/************************************************************
Func Name    :~AutoMutex
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :析构函数,释放互斥量
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    ~AutoMutex(void);
private:
    HANDLE m_hMutex;
};//lint !e1712 不需要默认构造函数;

#endif	//_AUTO_MUTEX_H_
