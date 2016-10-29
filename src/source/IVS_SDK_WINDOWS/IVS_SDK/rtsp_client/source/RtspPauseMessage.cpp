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

/*
 *  RtspPauseMessage.cpp
 *
 *  Created on: 2011-10-11
 *      Author: s00201096
 */
#include <sstream>
#include "RtspRetCode.h"
#include "RtspPauseMessage.h"

CRtspPauseMessage::CRtspPauseMessage()
{
    m_unRtspMethod = RTSP_METHOD_PAUSE;
}

CRtspPauseMessage::~CRtspPauseMessage()
{

}

// ¸²Ð´PauseµÄ·â×°º¯Êý
int CRtspPauseMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    if (RET_OK != encodeUrl(strMessage))
    {
        return RET_FAIL;
    }
    encodeCSeq(strMessage);
    strMessage += RTSP_TOKEN_STR_SESSION;
    strMessage += "1";
    strMessage += RTSP_END_TAG;
    strMessage += RTSP_END_TAG;
    
    return RET_OK;
}
