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
filename    :    MulticastSource.h
author      :    z90003203
created     :    2013/1/27
description :    组播来源类定义
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/10 初始版本
*********************************************************************/
#ifndef _NETSOURCE_MULTICAST_SOURCE_H_
#define _NETSOURCE_MULTICAST_SOURCE_H_

#ifndef _C_MULTICAST_SOURCE_H_
#define _C_MULTICAST_SOURCE_H_

#include "UdpSource.h"

class CChannelInfo;
class CMulticastSource : public CUdpSource
{
public:
    CMulticastSource(void);
    ~CMulticastSource(void);

    /******************************************************************
     function   : Init
     description: 初始化
     input      : CChannelInfo * pChannelInfo
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    virtual int Init(CChannelInfo* pChannelInfo);

private:
    /******************************************************************
     function   : InitMulticast
     description: 初始化组播设置
     input      : NA
     output     : NA
     return     : int 成功:IVS_SUCCEED 失败:IVS_FAIL;
    *******************************************************************/
    int InitMulticast();
};

#endif

#endif	//_NETSOURCE_MULTICAST_SOURCE_H_
