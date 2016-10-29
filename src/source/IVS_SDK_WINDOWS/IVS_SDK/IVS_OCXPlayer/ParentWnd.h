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
 filename    :    ParentWnd.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    VideoPane的父窗体
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#ifndef _PARENTWND_H__
#define _PARENTWND_H__

#include "VideoPaneMgr.h"

#define  BACKGROUND_CHANGE 1

// CParentWnd dialog
typedef std::list <CRect>   LISTCRECT;
typedef LISTCRECT::iterator LISTITER;

class CParentWnd : public CDialog
{
    DECLARE_DYNAMIC(CParentWnd)

public:
    CParentWnd(CWnd* pParent = NULL);   // standard constructor
    virtual ~CParentWnd();

    // Dialog Data
    LISTCRECT m_ListCrect;

    int m_nRightEdge;
    int m_nBottomEdge;
    enum { IDD = IDD_DLG_PARENT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnPaint();
    afx_msg void OnClose(){};//lint !e1762
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
    bool m_bIsDrag;
	bool m_bDisableDrag;
public:
    bool GetIsDrag() const { return m_bIsDrag; }
    void SetIsDrag(bool val) { m_bIsDrag = val; }

	bool GetDisableDrag() const { return m_bDisableDrag; }
	void SetDisableDrag(bool val) { m_bDisableDrag = val; }
private:
    CVideoPaneMgr * m_pVideoPaneMgr;
	DWORD m_dwStart;
	DWORD m_dwEnd;
public:
    void SetVideoPaneMgr(CVideoPaneMgr * val) { m_pVideoPaneMgr = val; }
	void StartDrag();
};
#endif // _PARENTWND_H__
