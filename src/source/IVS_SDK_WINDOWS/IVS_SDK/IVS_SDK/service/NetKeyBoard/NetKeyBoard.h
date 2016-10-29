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
	filename	: 	NetKeyBoardService.h
	author		:	
	created		:	2011/10/28	
	description	:	网络键盘实现类
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28 初始版本
*********************************************************************/
#ifndef NET_KEY_BOARD_H
#define NET_KEY_BOARD_H

/*!< 单键实例头文件 */
#include "SingleInst.h"
/*!< 公共宏、结构体头文件 */
#include "IVSSDKNetKeyBoardDef.h"


class CNetKeyBoard
{
    DECLARE_SINGLE_INSTANCE(CNetKeyBoard)

public:
    CNetKeyBoard(void);
    virtual ~CNetKeyBoard(void);


    /**************************************************************************
    * name       : SetListenAddr
    * description: 设置服务器端监听IP和端口号
    * input      : iPort服务器端监听端口号
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void SetNetKeyBoardListenAddr(unsigned short iPort);

    /**************************************************************************
    * name       : StartListenThread
    * description: 启动监听线程
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : g_hThread
    * remark     : NA
    **************************************************************************/
    int StartNetKeyBoard(unsigned short ulNetKeyBoardVendor, unsigned short ulPort, LPCallBack lpCallBack);

    /**************************************************************************
    * name       : ReleaseThread
    * description: 释放线程
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    int ReleaseThread();

private:
    bool m_bRun;

};
#endif //NET_KEY_BOARD_H
