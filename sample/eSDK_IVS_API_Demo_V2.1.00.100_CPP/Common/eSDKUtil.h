//! \file eSDKUtil.h
//! [\brief] 共用函数
//!
//! Description: 共用的函数在此声明实现
//! 要求所有的函数都是可重入的

#pragma once
#include <afx.h>
namespace eSDK
{
	class eSDKUtil
	{
	//以下是对外开放的函数
	public:
		//! \brief 得到动态库程序所在目录
		//! [\prama [in] bIncludeSep -- 是否包含最后的分隔符"\"
		static CString GetModulePath(BOOL bIncludeSep);

		//! \brief 获取完整路径的文件名
		//! Description 如果给定文件不是全路径，就是相对于动态库的路径
		static CString GetFullPath(LPCTSTR strCurFileName);

	//以下是内部函数，不对外开发
	private:


	};
};