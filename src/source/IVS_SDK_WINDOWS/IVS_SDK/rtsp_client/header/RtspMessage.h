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
  �� �� �� : RtspMessage.h
  ��    �� : y00182336
  �������� : RTSP��Ϣ
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#ifndef __RTSPMESSAGE_H_
#define __RTSPMESSAGE_H_

#include <string>
#include "ace_header.h"
#include "rtsp_client_datatype.h"
#include "RtspDef.h"

class CRtspMessage
{
public:
    CRtspMessage();
    virtual ~CRtspMessage();
    
    // ��װRTSP��Ӧ��Ϣ
    int encodeRtspRespMsg(unsigned int unStatusCode, std::string &strMessage);

    // ��װURL
    int encodeUrl(std::string &strMessage);
    
    // ��װRTSP���к�
    void encodeCSeq(std::string &strMessage);

    unsigned int getMethodType() const;

    // ������Ϣ���к�
    void setCSeq(unsigned int unCSeq);

    // ��ȡ��Ϣ���к�
    unsigned int getCSeq() const;

    // ����URL
    void setRtspUrl(const std::string &strUrl);
    
    // �����Ƿ�ΪRTSP��Ӧ��Ϣ
    bool isRtspRespMsg(const char* psRtspMsg,const unsigned int unLen);
    
    // �麯������װRTSP��Ϣ
    virtual int encodeMessage(std::string &strMessage) = 0;

    // ������Ӧ��Ϣ
    virtual int decodeMessage(const char* pszMessage,const unsigned int unMsgLen);

private:
    // ȥ���ո�
    void trimString(std::string& srcString) const;

public:
    unsigned int        m_unRtspMethod;         // Rtsp��Ϣ����
    std::string         m_strUrl;               // RTSP��Ϣ��URL

    static const char*  m_RtspCode[];           // RTSP ��Ӧ������
private:
    unsigned int        m_unCSeq;               // RTSP��Ϣ���к�
    static const char*  m_strRtspMethod[];      // RTSP ��Ϣ����

};


#endif //__RTSPMESSAGE_H_

