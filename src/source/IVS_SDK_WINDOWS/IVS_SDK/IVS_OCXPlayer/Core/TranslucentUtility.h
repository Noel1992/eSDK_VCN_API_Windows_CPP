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
#ifndef _OCX_TRAN_SLUCENT_UTILITY_H_
#define _OCX_TRAN_SLUCENT_UTILITY_H_

#include <GdiPlus.h>

class CTranslucentUtility
{
public:
	static CString GetModulePath(HMODULE hModule = NULL);

	static BOOL IsFileExist(LPCTSTR lpszFilePath);

	static BOOL ExtractResourceToFile( LPCTSTR lpszType
		, UINT nResID
		, LPCTSTR lpszFilename
		, HMODULE hModule
		);

	static Gdiplus::Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance =nullptr );

	static HRGN CreateRegionFromBitmap(Gdiplus::Bitmap* bitmap, BYTE alphaValve = 0);

	static HRGN CreateRegion(LPCTSTR lpszFile, BYTE alphaValve = 0);

	static HRGN CreateRegion(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL, BYTE alphaValve = 0); 

	static WCHAR * ttowc(const TCHAR *str ); 
};

#endif	//_OCX_TRAN_SLUCENT_UTILITY_H_
