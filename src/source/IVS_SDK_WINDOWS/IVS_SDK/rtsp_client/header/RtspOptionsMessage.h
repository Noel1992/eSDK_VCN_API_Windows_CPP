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

#ifndef __RTSPOPTIONSMESSAGE_H_
#define __RTSPOPTIONSMESSAGE_H_

#include "RtspMessage.h"

class CRtspOptionsMessage : public CRtspMessage
{
public:
    CRtspOptionsMessage();
    virtual ~CRtspOptionsMessage();
    
    // 封装消息
    int encodeMessage(std::string &strMessage);

    // 解析消息
    int decodeMessage(const char* pszMessage,const int nMsgLen);

    // 解析Data
    int parseData(const std::string &strRtspMessage);

public:
    std::string         m_strDate;          // 时间信息
    std::string         m_strTimeinfo;
};

#endif //__RTSPOPTIONSMESSAGE_H_
