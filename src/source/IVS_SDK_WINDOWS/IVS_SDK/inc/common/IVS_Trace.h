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
filename	: 	IVS_Trace.h
author		:	w00210470
created		:	2013/01/14	
description	:	TRACE 功能
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 初始版本;
*********************************************************************/
#ifndef __IVS_TRACE_H__
#define __IVS_TRACE_H__

#include "bp_log.h"
#include <stdarg.h>
#include <stdio.h>
#include "eSDKLogAPI.h"
#include "eSDK_Securec.h"

#define SNPRINTF  eSDK_VSPRINTF

#ifdef eSDK_OCX
#define __PRODUCT_NAME__	"eSDK-VCN-OCX-Windows-Cpp"
#else
#define __PRODUCT_NAME__	"eSDK-VCN-API-Windows-Cpp"
#endif

#define __CLIENT_LOG_CFG__	"eSDKClientLogCfg.ini"

class CIVSTrace {
public:
	CIVSTrace(LOG_LEVEL level, const char *file, int line, const char *func, const char *fmt, ...)
		:m_enLevel(level), m_pFunc(func), m_pFile(file), m_nLine(line)
	{
		va_list args;
		int len;
		char * buffer;

		va_start(args, fmt);
		len = _vscprintf(fmt, args) + 1;
		if (len > 0)
		{
			buffer = static_cast<char *>(malloc( len * sizeof(char) ));
			if (NULL != buffer)
			{
				(void)SNPRINTF(buffer, len, fmt, args);
				va_end(args);
				(void)IVS_LogRun(m_enLevel, "Enter ", m_pFile, m_nLine, m_pFunc, "%s %s", m_pFunc, buffer);

				free(buffer);
				buffer = NULL;
			}
			else
			{
				va_end(args);
			}
		}
		else
		{
			va_end(args);
		}
	}

	~CIVSTrace()
	{
		try {
			IVS_LogRun(m_enLevel, "Leave ", m_pFile, m_nLine, m_pFunc, "%s", m_pFunc);
		} 
		catch(...){} ; //lint !e1740
	}
private:
	CIVSTrace();
    LOG_LEVEL   m_enLevel;
	const char *m_pFunc;
	const char *m_pFile;
	int         m_nLine;
};

#define IVS_INFO_TRACE(fmt, ...) CIVSTrace IvsTrace(IVSLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define IVS_DEBUG_TRACE(fmt, ...) CIVSTrace IvsTrace(IVSLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

//无SessionID有返回值的宏
#define INT_LOG_NO_SESSION(reqTime, rspTime, resultcode, fmt, ...) {\
	char strReqTime[256] = {0};\
	char strRspTime[256] = {0};\
	eSDK_SPRINTF(strReqTime, sizeof(strReqTime),"%04d-%02d-%02d %02d:%02d:%02d %03d",reqTime.wYear,reqTime.wMonth,reqTime.wDay,reqTime.wHour,reqTime.wMinute,reqTime.wSecond,reqTime.wMilliseconds);\
	eSDK_SPRINTF(strRspTime, sizeof(strRspTime),"%04d-%02d-%02d %02d:%02d:%02d %03d",rspTime.wYear,rspTime.wMonth,rspTime.wDay,rspTime.wHour,rspTime.wMinute,rspTime.wSecond,rspTime.wMilliseconds);\
	char strResultCode[256] = {0};\
	(void)itoa(resultcode,strResultCode,10);\
	strResultCode[255] = '\0';\
	if(0 == resultcode)\
	{\
		Log_Interface_Info(__PRODUCT_NAME__,"1","",__FUNCTION__,"","","",strReqTime,strRspTime,strResultCode,fmt,##__VA_ARGS__);\
	}\
	else\
	{\
		Log_Interface_Error(__PRODUCT_NAME__,"1","",__FUNCTION__,"","","",strReqTime,strRspTime,strResultCode,fmt,##__VA_ARGS__);\
	}\
}

//有SeesionID有返回值的宏
#define INT_LOG_SESSION(SessionID, reqTime, rspTime, resultcode, fmt, ...) {\
	char strTransID[256]={0};\
	(void)itoa(SessionID,strTransID,10);\
	strTransID[255] = '\0';\
	char strReqTime[256] = {0};\
	char strRspTime[256] = {0};\
	eSDK_SPRINTF(strReqTime, sizeof(strReqTime),"%04d-%02d-%02d %02d:%02d:%02d %03d",reqTime.wYear,reqTime.wMonth,reqTime.wDay,reqTime.wHour,reqTime.wMinute,reqTime.wSecond,reqTime.wMilliseconds);\
	eSDK_SPRINTF(strRspTime, sizeof(strRspTime),"%04d-%02d-%02d %02d:%02d:%02d %03d",rspTime.wYear,rspTime.wMonth,rspTime.wDay,rspTime.wHour,rspTime.wMinute,rspTime.wSecond,rspTime.wMilliseconds);\
	char strResultCode[256] = {0};\
	(void)itoa(resultcode,strResultCode,10);\
	strResultCode[255] = '\0';\
	if(0 == resultcode)\
	{\
		Log_Interface_Info(__PRODUCT_NAME__,"1","",__FUNCTION__,"","",strTransID,strReqTime,strRspTime,strResultCode,fmt,##__VA_ARGS__);\
	}\
		else\
	{\
		Log_Interface_Error(__PRODUCT_NAME__,"1","",__FUNCTION__,"","",strTransID,strReqTime,strRspTime,strResultCode,fmt,##__VA_ARGS__);\
	}\
}
//eSDK统计日志，运行警告级别
#define eSDK_LOG_RUN_WRAN(param) Log_Run_Warn(__PRODUCT_NAME__, param)

#endif
