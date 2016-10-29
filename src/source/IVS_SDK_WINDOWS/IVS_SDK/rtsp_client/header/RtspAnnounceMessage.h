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
  �� �� �� : RtspAnnounceMessage.h
  ��    �� : y00182336
  �������� : RTSP Announce��Ϣ
  �޸���ʷ :
    1 ���� : 2011-11-8
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#ifndef __RTSPANNOUNCEMESSAGE_H_
#define __RTSPANNOUNCEMESSAGE_H_
#include "RtspMessage.h"

class CRtspAnnounceMessage : public CRtspMessage
{
public:
    CRtspAnnounceMessage();
    virtual ~CRtspAnnounceMessage();

    // ��װ��Ϣ
    int encodeMessage(std::string &strMessage);
    
    // ������Ϣ
    int decodeMessage(const char* pszMessage,const unsigned int unMsgLen);
    
    // ����SDP
    std::string getRtspSdp()const;
    
    void setStatusCode(unsigned int unStatusCode);
private:
    std::string     m_strRtspSdp;               // ��Ϣ���е�SDP�ֶΣ���Ҫ�ص���CU
    unsigned int    m_unStatusCode;             // ��Ӧ��Ϣ�еķ�����
};


#endif



