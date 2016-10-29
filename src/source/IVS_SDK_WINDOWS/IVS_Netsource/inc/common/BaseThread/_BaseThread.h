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
File name      :_BaseThread.h
Author         :邹清    z00003175
Description    :需要建立线程时，加入该头文件
-------------------------------------------------------------
History
Date           Author         Modification
20100720       邹清           created file
*************************************************************/


#ifndef BASETHREAD_H_HEADER_INCLUDED_B3F0F5E3
#define BASETHREAD_H_HEADER_INCLUDED_B3F0F5E3
class BaseJob;
#include "BaseJob.h"
//##ModelId=4C0E0850009F
class _BaseThread
{
  public:
/************************************************************
Func Name    :_BaseThread
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :线程的创建
Input        :
    BaseJob* job  :Task对象指针
    int StackSize  :线程创建时的栈大小 
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    //##ModelId=4C0E087E015B
    _BaseThread(BaseJob* pBaseJob, int StackSize);

/************************************************************
Func Name    :~_BaseThread
Date Created :2010-07-20
Author       :邹清  z00003175
Description  :线程对象的析构，将任务释放
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       邹清           created file

************************************************************/
    //##ModelId=4C0E08980310
    ~_BaseThread();
  private:
    //##ModelId=4C0F5F6402FC
    HANDLE m_hThread;

    //##ModelId=4C3C03CD00F8
    BaseJob* m_pBaseJob;
    //##ModelId=4C3C034700F8
};//lint !e1712 无需默认构造函数;
#endif /* BASETHREAD_H_HEADER_INCLUDED_B3F0F5E3 */
