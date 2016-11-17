//! Copyright(C): 2012 by Huawei Technologies Co. Ltd
//! \file Language.h 
//! [\brief] 语言配置类。
//!
//! Description: 从语言配置文件中取出语言项。
//! 如果从文件中获取失败，则使用程序中默认的

#pragma once
#include <afx.h>
#include "IniFile.h"
#include <map>
using namespace std;

//! \brief CLanguage::GetString()的参数宏
#define GET_STRING_PARAM(ID) ID, _T(#ID)

namespace eSDK
{
	class CLanguage
	{
		typedef map<CString, CString> LANG_MAP;
	public:
		//! \brief 获取实例对象
		static CLanguage * GetInstance();
		//! \brief 根据语言ID载入语言
		BOOL LoadLanguage(LANGID language);
		//! \brief 根据ini配置文件载入语言
		BOOL LoadLanguage(CString sLanguageFile);
		//! \brief 根据字符串ID获取目标语言描述
		CString GetString(UINT ID, CString strID);
	private:
		CLanguage(void);
		static auto_ptr<CLanguage> m_inst;//!<单实例指针
		CString m_langFile;	//!<语言文件
		CIniFile m_iniFile;	//!<ini文件对象
		LANG_MAP m_langMap;//ID与描述的对应关系
	};
};

