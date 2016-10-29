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

#ifndef _TRAN_ZOOM_TITLEBAR_H__
#define _TRAN_ZOOM_TITLEBAR_H__

#include "afxwin.h"
#include "afxcmn.h"

// CTranZoomTitleBar dialog

class CTranZoomTitleBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranZoomTitleBar)

public:
	CTranZoomTitleBar();
	CTranZoomTitleBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/);   // standard constructor
	virtual ~CTranZoomTitleBar();

// Dialog Data
	enum { IDD = IDD_DLG_ZOOMTITLEBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();
	void SetBarPictrue(LPCTSTR lpszFile);
public:
	CTranslucentButton m_btnZoomOver;
	CTranslucentStatic m_StaticZoomName;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedClose();
	std::string GetZoomTitleName() const;
	void SetZoomTitleName( const std::string &strDevName );
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif //_TRAN_ZOOM_TITLEBAR_H__
