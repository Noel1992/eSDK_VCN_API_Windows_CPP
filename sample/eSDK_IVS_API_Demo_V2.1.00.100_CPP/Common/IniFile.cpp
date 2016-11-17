#include "stdafx.h"
#include "IniFile.h"
using namespace eSDK;
CIniFile::CIniFile(void)
{
}
CIniFile::CIniFile(LPCTSTR strFileName)
{
	m_fileName = strFileName;
}

CIniFile::~CIniFile(void)
{
}

//! \brief 获取键值
CString CIniFile::GetValue(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strDefault)
{
	const int BUF_MAX_SIZE = 1024*2;
	wchar_t *pBuf = new wchar_t[BUF_MAX_SIZE];
	CString str;
	int nNamesLen = GetPrivateProfileString(strSection, strKey, strDefault, (LPSTR)pBuf, BUF_MAX_SIZE, m_fileName);
	str = pBuf;
	delete[] pBuf;
	return str;
}

BOOL CIniFile::WriteValue(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue)
{
	return TRUE;
}

BOOL CIniFile::GetSectionNames(STRING_LIST &nameList)
{
	const int BUF_MAX_SIZE = 1024*2;
	wchar_t *pSectionBuf = new wchar_t[BUF_MAX_SIZE];
	CString strSection;
	int nNamesLen = GetPrivateProfileSectionNames((LPSTR)pSectionBuf, BUF_MAX_SIZE, m_fileName);
	if(0==nNamesLen)
	{
		delete[] pSectionBuf;
		return FALSE;
	}
	wchar_t *pBuf = pSectionBuf;
	int nLen = 0;
	do 
	{
		nameList.push_back((LPSTR)pBuf);
		nLen = wcslen(pBuf) + 1;
		pBuf += nLen;
		nNamesLen -= nLen;
	} while (nNamesLen>0);
	delete[] pSectionBuf;
	return TRUE;
}

BOOL CIniFile::GetSectionValueMap(LPCTSTR strSection, VALUE_MAP& valueMap)
{
	const int BUF_MAX_SIZE = 1024*8;
	char *pValuesBuf = new char[BUF_MAX_SIZE];
	int nValuesLen = GetPrivateProfileSection(strSection, pValuesBuf, BUF_MAX_SIZE, m_fileName);
	if(0==nValuesLen)
	{
		delete[] pValuesBuf;
		return FALSE;
	}
	char *pBuf = pValuesBuf;
	int nLen = 0;
	do 
	{
		CString key_value = pBuf;
		int pos = key_value.Find(_T("="));
		CString strKey;
		CString strValue;
		if(pos>0)
		{
			strKey = key_value.Left(pos);
			strValue = key_value.Right(key_value.GetLength() - key_value.Find(_T("=")) - 1);
		}
		else if(pos<0)
		{
			strKey = key_value;
		}
		if(!strKey.IsEmpty())
		{
			VALUE_MAP::iterator ite = valueMap.find(strKey);
			if(ite==valueMap.end())
			{
				valueMap.insert(VALUE_MAP::value_type(strKey, strValue));
			}
			else
			{
				ite->second = strValue;
			}
		}
		nLen = strlen(pBuf);
		pBuf += nLen + 1;
		nValuesLen -= nLen + 1;
	} while (nValuesLen>0);
	delete[] pValuesBuf;
	return TRUE;
}


//! \brief 获取整个ini文件所有键值Map
BOOL CIniFile::GetAllValueMap(VALUE_MAP &valueMap)
{
	BOOL bResult = FALSE;
	STRING_LIST nameList;
	if(GetSectionNames(nameList))
	{
		for(STRING_LIST::iterator ite=nameList.begin();ite!=nameList.end();ite++)
		{
			CString strSection = *ite;
			VALUE_MAP tmpMap;
			if(GetSectionValueMap(strSection, tmpMap))
			{
				bResult = TRUE;
				valueMap.insert(tmpMap.begin(), tmpMap.end());
			}
		}
	}
	return TRUE;
}