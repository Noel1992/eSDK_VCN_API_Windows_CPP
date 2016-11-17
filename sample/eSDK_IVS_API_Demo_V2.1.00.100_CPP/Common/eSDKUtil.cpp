#include "stdafx.h"
#include "eSDKUtil.h"
using namespace eSDK;

//获取完整路径的文件名
CString eSDKUtil::GetFullPath(LPCTSTR strCurFileName)
{
	CString strPath = strCurFileName;   
	if(!strPath.IsEmpty())  
	{
		//-相对路径-       
		if(strPath.Find(_T(":"))<=0)            
		{               
			strPath.Format(_T("%s%s"), GetModulePath(strPath.Left(1)!=L"\\"), strCurFileName);
		}      
	}       
	return strPath;  
}   


// 得到动态库程序所在目录
//  BOOL bIncludeSep -- 是否包含最后的分隔符"/"
CString eSDKUtil::GetModulePath(BOOL bIncludeSep)
{ 
	// 得到当前的文件名 
	CString strFileName; 
	
	GetModuleFileName(AfxGetStaticModuleState()->m_hCurrentInstanceHandle ,strFileName.GetBuffer(_MAX_PATH),_MAX_PATH); 
	strFileName.ReleaseBuffer();    

	// 得到当前目录 
	strFileName=strFileName.Left(strFileName.ReverseFind(_T('\\'))+1);  

	if(bIncludeSep)  
		return strFileName; 
	else  
		return strFileName.Left(strFileName.GetLength()-1);
}

