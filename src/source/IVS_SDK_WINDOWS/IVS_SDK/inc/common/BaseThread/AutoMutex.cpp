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
File name      :Decoder.cpp
Author         :����    z00003175
Description    :������Զ�������
Function List  :
   AutoMutex
   ~AutoMutex
-------------------------------------------------------------
History
Date           Author         Modification
20100720       ����           created file
*************************************************************/
// #include "StdAfx.h"
#include "AutoMutex.h"

/************************************************************
Func Name    :AutoMutex
Date Created :2010-07-20
Author       :����  z00003175
Description  :���캯��,������Ļ������������,�����뻥����
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       ����           created file

************************************************************/
AutoMutex::AutoMutex(HANDLE hMutex)
:m_hMutex(hMutex)
{
    if (NULL == m_hMutex)
    {
        return;
    }

    ::WaitForSingleObject(m_hMutex,INFINITE);
}

/************************************************************
Func Name    :~AutoMutex
Date Created :2010-07-20
Author       :����  z00003175
Description  :��������,�ͷŻ�����
Input        :
Output       :
Return       :
Caution      :
History
Date           Author         Modification
20100720       ����           created file

************************************************************/
AutoMutex::~AutoMutex(void)
{
    if (m_hMutex)
    {
        ::ReleaseMutex(m_hMutex);
        m_hMutex = NULL;
    }
}
