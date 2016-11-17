//! Copyright(C): 2012 by Huawei Technologies Co. Ltd
//! \file IniFIle.h 
//! [\brief] ini文件处理类。
//!
//! Description: ini文件读写封装。

#pragma once
#include <afx.h>
#include <map>
#include <list>
using namespace std;
namespace eSDK
{
	class CIniFile
	{
		typedef map<CString, CString> VALUE_MAP;
		typedef list<CString> STRING_LIST;
	public:
		CIniFile(void);
		CIniFile(LPCTSTR strFileName);
		~CIniFile(void);
		//! \brief 设置ini文件名
		void SetFileName(LPCTSTR strFileName)
		{
			m_fileName = strFileName;
		}
		//! \brief 获取键值
		CString GetValue(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strDefault);
		//! \brief 键值写入文件
		BOOL WriteValue(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue);
		//! \brief 获取指定Section下所有键值对应关系
		BOOL GetSectionValueMap(LPCTSTR strSection, VALUE_MAP& valueMap);
		//! \brief 获取整个ini文件所有键值对应关系
		BOOL GetAllValueMap(VALUE_MAP &valueMap);
	private:
		//! \brief 获取文件中所有Section名字
		BOOL GetSectionNames(STRING_LIST &list);
	private:
		CString m_fileName;	//!<ini文件名
	};
}
