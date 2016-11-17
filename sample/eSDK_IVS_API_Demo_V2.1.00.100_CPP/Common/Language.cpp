#include "stdafx.h"
#include "Language.h"
#include "eSDKUtil.h"
using namespace eSDK;

CLanguage::CLanguage(void)
{

}

auto_ptr<CLanguage> CLanguage::m_inst;
CLanguage * CLanguage::GetInstance()
{
	//if(m_pLangUtil==NULL)
	//	m_pLangUtil = new CLanguage();
	//return m_pLangUtil;
	if(m_inst.get() == 0)
		m_inst = auto_ptr<CLanguage>(new CLanguage);
	return m_inst.get();
}
BOOL CLanguage::LoadLanguage(LANGID language)
{
	CString iniFile;
	m_langMap.clear();
	iniFile.Format(_T("lang\\%d.ini"),language);
	CString str = eSDKUtil::GetFullPath(iniFile);
	m_iniFile.SetFileName(str);
	return m_iniFile.GetAllValueMap(m_langMap);
}
BOOL CLanguage::LoadLanguage(CString sLanguageFile)
{
	m_langMap.clear();
	m_iniFile.SetFileName(sLanguageFile);
	return m_iniFile.GetAllValueMap(m_langMap);
}
CString CLanguage::GetString(UINT ID, CString strID)
{
	CString str;
	LANG_MAP::iterator ite = m_langMap.find(strID);
	if(ite!=m_langMap.end())
	{
		str = ite->second;
	}
	if(str.IsEmpty())
	{
		TCHAR strLang[100];
		memset(strLang, 0, sizeof(strLang));
		int err = LoadString(AfxGetStaticModuleState()->m_hCurrentResourceHandle, ID, strLang, sizeof(strLang));
		str = strLang;
	}
	return str;
}
