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
filename	: 	NetKeyBoardService.cpp
author		:	
created		:	2011/10/28	
description	:	
copyright	:	Copyright (C) 2011-2015
history		:	2011/10/28 ��ʼ�汾
*********************************************************************/
/*!< Ԥ����ͷ�ļ� */
#include "StdAfx.h"
/*!< ����ͷ�ļ� */
#include "NetKeyBoard.h"
/*!< ������ͷ�ļ� */
#include "ToolsHelp.h"
/*!< ý����²�ӿ�ͷ�ļ� */
#include "IVSSDKNetKeyBoard.h"


IMPLEMENT_SINGLE_INSTANCE(CNetKeyBoard)

CNetKeyBoard::CNetKeyBoard(void)
{
    m_bRun = false;
}

CNetKeyBoard::~CNetKeyBoard(void)
{
    m_bRun = false;
}

/**************************************************************************
* name       : SetListenAddr
* description: ���÷������˼���IP�Ͷ˿ں�
* input      : iPort�������˼����˿ں�
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoard::SetNetKeyBoardListenAddr(unsigned short iPort)
{

    IVS_SDK_KB_SetNetKeyBoardListenAddr(iPort);

}//lint !e1762

/**************************************************************************
* name       : StartListenThread
* description: ���������߳�
* input      : lpCallBack �ص�����
* output     : NA
* return     : IVS_SUCCEED - �ɹ�,���� - ʧ��
* remark     : NA
**************************************************************************/
int CNetKeyBoard::StartNetKeyBoard(unsigned short ulNetKeyBoardVendor, unsigned short ulPort, LPCallBack lpCallBack)
{

    int iRet = IVS_SDK_KB_StartNetKeyBoardListenThread(ulNetKeyBoardVendor, ulPort, lpCallBack);
    if (IVS_SUCCEED != iRet)
    {
        m_bRun = false;
    }
    else
    {
        m_bRun = true;
    }

    return iRet;
}

/**************************************************************************
* name       : ReleaseThread
* description: �ͷ��߳�
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
int CNetKeyBoard::ReleaseThread()
{

    if (!m_bRun)
    {
        return IVS_FAIL;
    }
    IVS_SDK_KB_ReleaseThread();
    m_bRun = false;

    return IVS_SUCCEED;
}

