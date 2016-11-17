
// IVS_Demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "DemoDefine.h"

// CIVS_DemoApp:
// See IVS_Demo.cpp for the implementation of this class
//

class CIVS_DemoApp : public CWinApp
{
public:
	CIVS_DemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CIVS_DemoApp theApp;