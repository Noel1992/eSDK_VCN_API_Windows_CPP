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
  �� �� �� : RtspPlayMessage.h
  ��    �� : y00182336
  �������� : RTSP Play��Ϣ
  �޸���ʷ :
    1 ���� : 2011-10-12
      ���� : y00182336
      �޸� : ����

 ******************************************************************************/
#ifndef __RTSPPLAYMESSAGE_H_
#define __RTSPPLAYMESSAGE_H_

#include "RtspMessage.h"

class CRtspPlayMessage : public CRtspMessage
{
public:
    CRtspPlayMessage();
    virtual ~CRtspPlayMessage();
    
    // ��װ��Ϣ
    int encodeMessage(std::string &strMessage);
    
    // ��װ����ʱ���
    void encodeRangeField(std::string &strMessage);

    // ���ò���ʱ���
    void setRang(const MEDIA_RANGE_S* stRange);

    // ���ò�������
    void setScale(const double dScale);

public:
    MEDIA_RANGE_S         m_stRange;             // ����ʱ���
    double              m_dScale;               // ��������
};


#endif //__RTSPPLAYMESSAGE_H_

