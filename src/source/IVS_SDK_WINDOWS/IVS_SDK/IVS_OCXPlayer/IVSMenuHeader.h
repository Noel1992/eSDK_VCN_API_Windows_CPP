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

#ifndef MENUHEADER_H
#define MENUHEADER_H

/////////////////////////////////////////////////////////////////////////////
// CIVSHeader window

class CIVSHeader : public CIVSButton
{
// Construction
public:
	CIVSHeader();

protected:
	BOOL	m_bOpen;					//当前是否打开
	HICON	m_hIconStatus;
// Operations
public:
	void SetHeaderStatus(BOOL bOpen);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIVSHeader)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIVSHeader();

	// Generated message map functions
protected:
	void GradientFillRect( HDC hDC, const CRect &rcFill, ULONG nMode, COLORREF crLeftTop, COLORREF crRightBottom ) const;
	//{{AFX_MSG(CIVSHeader)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif 
