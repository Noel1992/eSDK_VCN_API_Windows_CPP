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
#ifndef _OCX_TRAN_SLUCENT_BOTTON_H_
#define _OCX_TRAN_SLUCENT_BOTTON_H_

#include "TranslucentWnd.h"
#include <GdiPlus.h>
#include "SDKDef.h"

class CTranslucentButton : public CTranslucentWnd
{
	DECLARE_DYNAMIC(CTranslucentButton)

public:
	CTranslucentButton();
	virtual ~CTranslucentButton();

	bool LoadImageList(const LPCTSTR szImageList[TWS_BUTTON_NUM]);
	bool LoadImageList(const TCHAR pcImageList[TWS_BUTTON_NUM][MAX_PATH]);
	bool LoadImageList( const UINT imageIDList[TWS_BUTTON_NUM], LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	virtual void Render(Gdiplus::Graphics& g);

	// for checkbox or radiobutton
	void SetChecked(bool b);
	bool IsChecked() const;

	void SetFont(const WCHAR* familyName, float emSize, int style = Gdiplus::FontStyleRegular, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	void SetFormat(Gdiplus::StringAlignment align = Gdiplus::StringAlignmentCenter, Gdiplus::StringAlignment lineAlign = Gdiplus::StringAlignmentCenter);
	void SetColor(const Gdiplus::Color& color);

	const Gdiplus::Font* GetFont() const { return m_pFont; }				//lint !e1536
	const Gdiplus::StringFormat& GetFormat() const{ return m_format; }		//lint !e1536
	const Gdiplus::SolidBrush& GetBrush() const { return m_brush; }
	// 设置tip显示的内容
	void UpdateTipText(const std::string& strTip);
	// 获取tip的内容
	void GetTipText(CString& tipText);
protected:
	DECLARE_MESSAGE_MAP()

	virtual void UpdateState();
	Gdiplus::Image* m_pCurrImage;
	bool m_bMouseOver;
	bool m_bPressed;
	bool m_bSelected;

	Gdiplus::StringFormat m_format;
	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush m_brush;

	CToolTipCtrl m_ToolTip;
	CFont m_ToolTipFont;
public:
     virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void SetMouseOverBtnImage();

private:
	std::string strFunctionName;
public:
	const std::string& GetStrFunctionName() const { return strFunctionName; }
	void SetStrFunctionName(const std::string& val) { strFunctionName = val; }
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif	//_OCX_TRAN_SLUCENT_BOTTON_H_
