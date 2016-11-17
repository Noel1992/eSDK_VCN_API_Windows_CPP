#include "stdafx.h"
#include "Logger.h"
#include "eSDKUtil.h"

using namespace eSDK;

CLogger::CLogger(void)
{
}

CLogger::~CLogger(void)
{
}

CString GetLevelStr(CLogger::LOG_LEVEL level)
{
	switch(level)
	{
	case CLogger::LEVEL_ERROR:
		return L"ERROR";
	case CLogger::LEVEL_WARN:
		return L"WARN";
	case CLogger::LEVEL_DEBUG:
		return L"DEBUG";
	case CLogger::LEVEL_INFO:
		return L"INFO";
	default:
		return L"OTHER";
	}
}

#define LOG_EVENT _T("eSDKLogWrite")
CString CLogger::m_strLogFile = _T("");
HANDLE CLogger::m_hWriteEvent = NULL;
#ifdef _DEBUG
CLogger::LOG_LEVEL CLogger::m_outLevel = CLogger::LEVEL_DEBUG;
BOOL CLogger::m_levelEnable = FALSE;		//调试状态下默认所有日志都记录
#else
CLogger::LOG_LEVEL CLogger::m_outLevel = CLogger::LEVEL_ERROR;
BOOL CLogger::m_levelEnable = TRUE;			//非调试状态下默认只记录ERROR日志
#endif

short CLogger::SetLogFile(LPCTSTR strPath)
{ 
	if(strPath==NULL || strPath[0]==0) 
		m_strLogFile = eSDKUtil::GetFullPath(L"eSDKlog.log"); 
	else  m_strLogFile = eSDKUtil::GetFullPath(strPath); 
	return 1;
}

void CLogger::WriteFile(LOG_LEVEL level, LPCTSTR strFormat, va_list valist)
{  
	if(m_levelEnable && level>m_outLevel )//不需要记录该级别日志
		return;
	CString   strPart_Time; 
	{  
		SYSTEMTIME sysTime  = {0};
		GetLocalTime(&sysTime);  
		strPart_Time.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d.%03d]"),    
							sysTime.wYear,sysTime.wMonth, sysTime.wDay,     
							sysTime.wHour, sysTime.wMinute, sysTime.wSecond,    
							sysTime.wMilliseconds);   
	}  

	CString   strPart_Type; 
	CString strLevel = GetLevelStr(level);
	if(strLevel && strLevel[0]) 
	{  
		strPart_Type.Format(_T("[%s]"), strLevel); 
	}  

	CString   strPart_Info; 
	{     
		strPart_Info.FormatV(strFormat, valist); 
	}   

	CString strInfo = strPart_Time + strPart_Type+ strPart_Info + L"\n"; 

	char*  pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_ACP,	0,strInfo,	-1,	NULL,0,	NULL,NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_ACP,0,strInfo,-1,pElementText,iTextLen,NULL,NULL );

	if(m_hWriteEvent==NULL)
	{  
		m_hWriteEvent = OpenEvent(0, FALSE,LOG_EVENT);  
		if(m_hWriteEvent==NULL)   
			m_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, LOG_EVENT);  
	} 
	WaitForSingleObject(m_hWriteEvent, INFINITE);

	//-打开关闭文件- 
	if(m_strLogFile.IsEmpty())  
		SetLogFile(NULL); 
	FILE* fp = NULL;
	try
	{
		int err = _wfopen_s(&fp, m_strLogFile, L"a+");
		if(err==0)
		{
			fwrite(pElementText, 1, iTextLen-1, fp);
			fclose(fp);
		}
	}
	catch(...)
	{

	}
	delete[] pElementText;
	SetEvent(m_hWriteEvent); 
}

void CLogger::SetLevel(LOG_LEVEL level, BOOL enable)
{
	m_outLevel = level;
	m_levelEnable = enable;
}

void CLogger::LogOut(LOG_LEVEL level, LPCTSTR strFormat,...)
{
	CString strLevel = GetLevelStr(level);
	va_list  valist; 
	va_start(valist, strFormat); 
	WriteFile(level, strFormat, valist); 
	va_end(valist);
}

void  CLogger::Error(LPCTSTR strFormat,...)
{
	va_list  valist; 
	va_start(valist, strFormat); 
	WriteFile(LEVEL_ERROR, strFormat, valist); 
	va_end(valist);

}

void  CLogger::Warn(LPCTSTR strFormat,...)
{
	va_list  valist; 
	va_start(valist, strFormat); 
	WriteFile(LEVEL_WARN, strFormat, valist); 
	va_end(valist);

}

void  CLogger::Debug(LPCTSTR strFormat,...)
{
	va_list  valist; 
	va_start(valist, strFormat); 
	WriteFile(LEVEL_DEBUG, strFormat, valist); 
	va_end(valist);

}

void  CLogger::Info(LPCTSTR strFormat,...)
{
	va_list  valist; 
	va_start(valist, strFormat); 
	WriteFile(LEVEL_INFO, strFormat, valist); 
	va_end(valist);
}

