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

#ifndef _TRAN_ZOOM_BAR_H__
#define _TRAN_ZOOM_BAR_H__

#include "afxwin.h"
#include "afxcmn.h"
#include "ZoomSliderCtrl.h"

const ULONG MAX_REAT_PERCETN = 800;     // ���Ŵ����
const ULONG MIN_REAT_PERCETN = 100;     // ��С�Ŵ����
const ULONG BTN_REAT_INTERVAL = 100;     // ��ťÿ�ΰ��¼��

// CTranZoomBar dialog

class CTranZoomBar : public CTranslucentDialog
{
	DECLARE_DYNAMIC(CTranZoomBar)

public:
	CTranZoomBar();
	CTranZoomBar(LPCTSTR lpszFile, CWnd* pParent /*=NULL*/);   // standard constructor
	virtual ~CTranZoomBar();

// Dialog Data
	enum { IDD = IDD_DLG_ZOOMBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitChildrenWnds();
	void SetButtonPicture(CTranslucentButton &tButton,const std::string &strButtonName);
	void SetCurrentPaneToActivePane();
	void SetStaticPicture( CTranslucentStatic &tStatic,const std::string &strButtonName );
	void SetSnapShotEnableStatus(BOOL bEnable);
	void SetBarPictrue(LPCTSTR lpszFile);
private:
	CTranslucentStatic m_MagnifyRateText;
	CTranslucentButton m_btnShrink;
	CTranslucentButton m_btnMagnify;
	CTranslucentButton m_btnSnapShot;

	CZoomSliderCtrl m_ZoomSliderCtrl;

	IVS_INT32 m_iMagnifyRate; // �ٷֱ�
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedButtonSnapshot();
	afx_msg void OnBnClickedButtonShrink();
	afx_msg void OnBnClickedButtonMagnify();

	void SetMagnifyRate(IVS_INT32 iRate);
	HWND GetZoomInHwnd()const { return m_btnShrink.GetSafeHwnd(); }
	HWND GetZoomOutHwnd()const { return m_btnMagnify.GetSafeHwnd(); }
	void SetRateTextColor(ULONG ulColor);
};

#endif //_TRAN_ZOOM_BAR_H__
