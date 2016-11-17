//! Copyright(C): 2012 by Huawei Technologies Co. Ltd
//! \file Logger.h 
//! [\brief] 日志记录器头文件。
//!
//! Description: eSDK需要记录输出日志时使用该类。
//! 鉴于插件不存在大量的日志，因此该日志记录器比较简单，没有用多线程来写日志。

#pragma once
#include <afx.h>

namespace eSDK
{
	class CLogger
	{
	public:
		//!\enum 日志级别
		//!\brief 日志输出时的级别
		enum LOG_LEVEL
		{
			LEVEL_ERROR = 0,	//!<错误
			LEVEL_WARN,			//!<告警
			LEVEL_DEBUG,		//!<调试
			LEVEL_INFO			//!<信息
		};
	private:
		CLogger(void);
		~CLogger(void);
	public:
		//!\brief 设置日志最低输出级别
		//!Description 当日志级别低于level时日志不写入文件
		//![\param [in]  level 日志级别 
		//![\param [in]  enable 日志级别是否启用
		static void SetLevel(LOG_LEVEL level,BOOL enable);
		//!\brief 日志输出
		//!Description 将日志输出到指定的文件中
		//![\param [in]  level 日志级别 
		//![\param [in]  strFormat 字符串格式
		static void LogOut(LOG_LEVEL level, LPCTSTR strFormat = NULL,...);
		//!\brief 以错误日志级别输出日志
		static void Error(LPCTSTR strFormat = NULL,...);
		//!\brief 以告警日志级别输出日志
		static void Warn(LPCTSTR strFormat = NULL,...);
		//!\brief 以调试日志级别输出日志
		static void Debug(LPCTSTR strFormat = NULL,...);
		//!\brief 以信息日志级别输出日志
		static void Info(LPCTSTR strFormat = NULL,...);

	private:
		//!\brief 设置日志文件路径名
		static short  SetLogFile(LPCTSTR strPath); 
		//!\brief 日志写入文件
		static void  WriteFile(LOG_LEVEL level, LPCTSTR strFormat = NULL, va_list valist = NULL); 
		static CString  m_strLogFile;//!<日志文件
		static HANDLE  m_hWriteEvent;//!写日志事件句柄
		static LOG_LEVEL m_outLevel;//!<日志输出级别
		static BOOL m_levelEnable; //!<日志级别是否有效。TURE-根据日记级别记录日志，FALSE-记录所有日志。默认FALSE
	};
};
