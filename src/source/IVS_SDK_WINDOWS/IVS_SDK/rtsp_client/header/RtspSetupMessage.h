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
   ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� �� : RtspSetupMessage.h
  ��    �� : y00182336
  �������� : RTSP����
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#ifndef __RTSPSETUPMESSAGE_H_
#define __RTSPSETUPMESSAGE_H_

#include <string>
#include "ace_header.h"
#include "RtspMessage.h"
#include "RtspDef.h"

class CRtspSetupMessage : public CRtspMessage
{
public:
    CRtspSetupMessage();
    virtual ~CRtspSetupMessage();

    int encodeMessage(std::string &strMessage);

    // ��װRTSP Transport����
    void encodeTransport(std::string &strMessage);
    
    // ���ô���ģʽ
    void setTransType(unsigned int unTransType);

    // ���ý��ն˿�
    void setClientPort(const unsigned short usVedioPort,
                       const unsigned short usAedioPort);
    
    // ���ý��յ�ַ
    void setDestinationIp(unsigned int unIp);

    // ��ȡ���͵�ַ
    unsigned int getServerIp() const;
    
    // ��ȡ������Ƶ�˿�
    unsigned short getServerVideoPort()const;

    // ��ȡ������Ƶ�˿�
    unsigned short getServerAudioPort()const;
    
    // ������Ӧ��Ϣ
    int decodeMessage(const char* pszMessage, const unsigned int nMsgLen);

private:
    // ����Transport
    int parseUdpTransParam(const std::string &strRtspMessage);

    int parseTcpTransParam(const std::string &strRtspMessage);

public:
    bool                m_bTcpFlag;                     // ���ӷ�ʽ�� true TCP, false UDP
    //MEDIA_ADDR*         m_stPeerAddr;                   // �첽ģʽ�£���¼Peer��ַ

private:
    unsigned int        m_unTransType;                  // ����ģʽ: TCP/UDP 
    unsigned short      m_usClientVideoPort;            // Rtp�˿ڣ�RTCPĬ��Ϊ+1
    unsigned short      m_usClientAudioPort;
    unsigned int        m_unDestIp;
    unsigned short      m_usServerVideoPort;            // Rtp��Ƶ�˿ڣ�RTCPĬ��Ϊ+1
    unsigned short      m_usServerAudioProt;            // Rtp��Ƶ�Ͽ���RTCPĬ��Ϊ+1
    unsigned int        m_unSrcIp;
    bool                m_bIsNoAudio;
};

#endif //__RTSPSETUPMESSAGE_H_

