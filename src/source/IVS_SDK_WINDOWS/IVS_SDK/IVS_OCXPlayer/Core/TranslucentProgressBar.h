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
#ifndef _OCX_TRAN_SLUCENT_PROGRESS_BAR_H_
#define _OCX_TRAN_SLUCENT_PROGRESS_BAR_H_
#include "TranslucentWnd.h"

// CTranslucentProgressBar

class CTranslucentProgressBar : public CTranslucentWnd
{
	DECLARE_DYNAMIC(CTranslucentProgressBar)

public:
	CTranslucentProgressBar();
	virtual ~CTranslucentProgressBar();

	// Set the background image
	bool SetBgImage(LPCTSTR szBgImage);
	bool SetBgImage(UINT nBgImageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	// Set the foreground image
	bool SetFgImage(LPCTSTR szFgImage);
	bool SetFgImage(UINT nFgImageID, LPCTSTR lpszResType = _T("PNG"), HINSTANCE hInstance = NULL);

	void GetRange(int& lower, int& upper) const;
	int GetPos() const;
	int GetStep() const;
	void StepIt();
	BOOL SetStep(int nStep);
	void OffsetPos(int nPos);
	BOOL SetPos(int nPos);
	void SetRange(int nLower, int nUpper);

	virtual void Render(Gdiplus::Graphics& g);

	void SetFont(const WCHAR* familyName, float emSize, int style = Gdiplus::FontStyleRegular, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	void SetFormat(Gdiplus::StringAlignment align = Gdiplus::StringAlignmentCenter, Gdiplus::StringAlignment lineAlign = Gdiplus::StringAlignmentCenter);
	void SetColor(const Gdiplus::Color& color);

	Gdiplus::Font* GetFont() { return m_pFont; }
	const 	Gdiplus::StringFormat& GetFormat() const { return m_format; }
	const Gdiplus::SolidBrush& GetBrush() const { return m_brush; }

protected:
	DECLARE_MESSAGE_MAP()

private:
	int m_nLower;
	int m_nUpper;
	int m_nStep;
	int m_nPos;

	Gdiplus::StringFormat m_format;
	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush m_brush;
};

#endif	//_OCX_TRAN_SLUCENT_PROGRESS_BAR_H_
