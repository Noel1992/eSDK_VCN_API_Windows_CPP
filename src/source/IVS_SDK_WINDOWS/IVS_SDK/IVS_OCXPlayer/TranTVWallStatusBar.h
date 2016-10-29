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

/*****************************************************************
 filename    :    TranAnalysisToolBar.h
 author      :    f00152768
 created     :    2013/01/25
 description :    电视墙窗格通道状态图标
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/25 初始版本
*****************************************************************/
#ifndef _OCX_TRAN_TVWALL_STATUSBAR_H_
#define _OCX_TRAN_TVWALL_STATUSBAR_H_

class CTranTVWallStatusBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranTVWallStatusBar)

public:
	virtual ~CTranTVWallStatusBar();
	CTranTVWallStatusBar(Gdiplus::Image* pImage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_TVWALL_STATUSBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	virtual void OnInitChildrenWnds();
};//lint !e1712 MFC窗体类

#endif	//_OCX_TRAN_TVWALL_STATUSBAR_H_
