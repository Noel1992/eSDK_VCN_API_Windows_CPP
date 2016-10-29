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

/***********************************************************
     Copyright, 2008 - 2010, Huawei Tech. co.,Ltd.
         All Rigths  Reserved
-------------------------------------------------------------
Project Code   :VRPV8
File name      :BaseJob.cpp
Author         :����    z00003175
Description    :��ʼ��������TASK
Function List  :
  BaseJob
  ~BaseJob
-------------------------------------------------------------
History
Date           Author         Modification
20100720       ����           created file
*************************************************************/
// #include "stdafx.h"
#include "BaseJob.h"

/************************************************************
Func Name    :BaseJob
Date Created :2010-07-20
Author       :����  z00003175
Description  :Task���캯������Ҫ��ɶ��ź����Ĵ���
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       ����           created file

************************************************************/
//##ModelId=4C3C08450286
BaseJob::BaseJob()
:m_hMutex((HANDLE)0)
{
    m_hMutex = ::CreateMutex(NULL,FALSE,NULL);
}

/************************************************************
Func Name    :~BaseJob
Date Created :2010-07-20
Author       :����  z00003175
Description  :Task������������Ҫ��ɶ��ź����ͷ�
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       ����           created file

************************************************************/
//##ModelId=4C3C085A0063
BaseJob::~BaseJob()
{
    if (m_hMutex != NULL)
    {
        ::CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}

