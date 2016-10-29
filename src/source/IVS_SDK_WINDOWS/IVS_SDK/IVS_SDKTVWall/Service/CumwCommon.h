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
    filename	: 	CumwCommon.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	公共头文件包含
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/

#ifndef CUMW_COMMON_H
#define CUMW_COMMON_H

//去掉map告警
#pragma warning(disable : 4786 4503)

//标准库头文件
#include <string>
#include <sstream>

//VOS头文件
#include "vos.h"            //跨平台类型定义
#include "SyncQueue.h"     //跨平台消息队列
#include "ConnMgr.h"       //跨平台通信平台
#include "NVSTimer.h"      //跨平台定时器
#include "LockGuard.h"      //自解锁
#include "LockableObject.h" //可锁对象
#include "bp_log.h"
//CUMW的头文件
#include "hwsdk.h"
#include "ivs_error.h"
#include "IVS_SDKTVWall.h"
#include "IVSCommon.h"
#include "CumwInterface.h"      //对外接口
#include "CumwTypes.h"          //内部类型定义
#include "CumwStatusClasses.h"  //回调通知
#include "Business.h"          //业务对象定义

//#include <windows.h>
#define IVS_RUN_LOG_EMR(fmt, ...) \
	(void)IVS_LogRun(IVSLOG_EMERG, "TVWallDecoder", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define IVS_RUN_LOG_CRI(fmt, ...) \
	(void)IVS_LogRun(IVSLOG_CRIT, "TVWallDecoder", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define IVS_RUN_LOG_ERR(fmt, ...)\
	(void)IVS_LogRun(IVSLOG_ERR, "TVWallDecoder", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define IVS_RUN_LOG_WAR(fmt, ...)\
	(void)IVS_LogRun(IVSLOG_WARNING, "TVWallDecoder", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define IVS_RUN_LOG_INF(fmt, ...)\
	(void)IVS_LogRun(IVSLOG_INFO, "TVWallDecoder", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define IVS_DBG_LOG(fmt, ...) \
	(void)IVS_LogDbg(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif //CUMW_COMMON_H
