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
filename    :    TcpSource.h
author      :    s00191067
created     :    2012/11/05
description :    TCP������
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/05 ��ʼ�汾
*********************************************************************/
#ifndef TCP_SOURCE_H
#define TCP_SOURCE_H

#include "NetSource.h"
#include "RtspPacket.h"

class CChannelInfo;
class CTcpSource : public CNetSource
{
public:
    CTcpSource(void);
    virtual ~CTcpSource(void);

    /******************************************************************
     function   : Init
     description: ��ʼ��;
     input      : CChannelInfo * pChannelInfo
     output     : NA
     return     : int �ɹ�:IVS_SUCCEED ʧ��:IVS_FAIL;
    *******************************************************************/
    virtual int Init(CChannelInfo* pChannelInfo); 

protected:
	/******************************************************************
     function   : ProcessRecevData
     description: ����
     output     : NA
     return     : void
    *******************************************************************/
    virtual void ProcessRecevData();

	/******************************************************************
     function   : CheckNatSend
     description: ����Ƿ�����NAT��Խ����,ÿ��30s�����������nat��Ϣ;
     output     : NA
     return     : int IVS_FAIL��ʾ������������ IVS_SUCCEED��ʾ��������;
    *******************************************************************/
    int CheckNatSend();

private:
	long RecvRTSP();

	void ProcRtsp();

	void PacketRtspInfo(const string strLocalIp, unsigned long ulLocalPort, const string strSourceIp, unsigned long ulSourcePort);

    void HandleTimeOutEvent();
private:
    CRtspPacket      m_RtspStack;               // RTSPЭ��ջ;
	SOCKET           m_socket;                  // soket����;
    bool             m_bIsFirstConnect;         // ��һ������
    std::string      m_strRemoteIp;             // Զ��IP
    unsigned int     m_uiRemotePort;            // Զ�˶˿�
}; 
#endif // TCP_SOURCE_H
