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


/******************************************************************************
   ��Ȩ���� (C), 2008-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �ļ���          : LockGuard.h
  �汾��          : 1.0
  ����            : 
  ��������        : 2008-8-17
  ����޸�        : 
  ��������        : ʵ������������
  �����б�        : 
  �޸���ʷ        : 
  1 ����          : 
    ����          : 
    �޸�����      : 
*******************************************************************************/


#ifndef CLOCKGUARD_H_INCLUDE
#define CLOCKGUARD_H_INCLUDE    


#include  "vos.h"

class CLockGuard
{
  public:
    CLockGuard(VOS_Mutex *pMutex);
    ~CLockGuard();
    
  public:
    static void lock(VOS_Mutex *pMutex);
    static void unlock(VOS_Mutex *pMutex);
    
 private:
    CLockGuard();
    VOS_Mutex *m_pMutex;
};

#endif // CLOCKGUARD_H_INCLUDE


