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

// TranTVWallStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "TranTVWallStatusBar.h"


// CTranTVWallStatusBar dialog

IMPLEMENT_DYNAMIC(CTranTVWallStatusBar, CTranslucentDialog)

CTranTVWallStatusBar::CTranTVWallStatusBar(Gdiplus::Image* pImage, CWnd* pParent)
	: CTranslucentDialog(CTranVideoRealtimeBar::IDD, pImage, pParent)
{

}

CTranTVWallStatusBar::~CTranTVWallStatusBar()
{
}

void CTranTVWallStatusBar::DoDataExchange(CDataExchange* pDX)
{
	CTranslucentDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTranTVWallStatusBar, CTranslucentDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTranTVWallStatusBar message handlers


void CTranTVWallStatusBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTranslucentDialog::OnPaint() for painting messages

	UpdateView();
}
void CTranTVWallStatusBar::OnInitChildrenWnds()
{

}
