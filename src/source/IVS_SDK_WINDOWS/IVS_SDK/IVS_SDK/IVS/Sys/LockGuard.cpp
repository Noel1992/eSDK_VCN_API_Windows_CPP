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
  �ļ���          : CLockGuard.cpp
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
#include "LockGuard.h"

CLockGuard::CLockGuard(VOS_Mutex *pMutex)
{
    m_pMutex = NULL;

    if(NULL == pMutex)
    {
        return;
    }

    m_pMutex = pMutex;

    (void)VOS_MutexLock(m_pMutex);
}

CLockGuard::~CLockGuard()
{
    if(NULL == m_pMutex)
    {
        return;
    }
    (void)VOS_MutexUnlock(m_pMutex);
    
    m_pMutex = NULL;
}

/*******************************************************************************
Function:       // CLockGuard::lock
Description:    // ����
Calls:          // 
Data Accessed:  // 
Data Updated:   // 
Input:          // VOS_Mutex *pMutex
Output:         // ��
Return:         // ��
Others:         // ��
*******************************************************************************/
void CLockGuard::lock(VOS_Mutex *pMutex)
{
    if(NULL == pMutex)
    {
        return;
    }
    (void)VOS_MutexLock(pMutex);
}

/*******************************************************************************
Function:       // CLockGuard::unlock
Description:    // �ͷ���
Calls:          // 
Data Accessed:  // 
Data Updated:   // 
Input:          // VOS_Mutex *pMutex
Output:         // ��
Return:         // ��
Others:         // ��
*******************************************************************************/
void CLockGuard::unlock(VOS_Mutex *pMutex)
{
    if(NULL == pMutex)
    {
        return;
    }
    (void)VOS_MutexUnlock(pMutex);
}
