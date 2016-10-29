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
#ifndef _OCX_TRAN_SLUCENT_STATIC_H_
#define _OCX_TRAN_SLUCENT_STATIC_H_

#include "SDKDef.h"
#include <GdiPlus.h>
#include "TranslucentWnd.h"
// CTranslucentStatic

class CTranslucentStatic : public CTranslucentWnd
{
	DECLARE_DYNAMIC(CTranslucentStatic)

public:
	CTranslucentStatic();
	virtual ~CTranslucentStatic();

	virtual void Render(Gdiplus::Graphics& g);
	virtual void PreSubclassWindow();

private:
	Gdiplus::Image* m_pCurrImage;
public:
	Gdiplus::Image* GetCurrImage() const { return m_pCurrImage; } //lint !e1763
	void SetCurrImage(Gdiplus::Image* val) { m_pCurrImage = val; }

protected:
	DECLARE_MESSAGE_MAP()

	Gdiplus::StringFormat m_format;
	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush m_brush;
public:
	void SetFont(const WCHAR* familyName, float emSize, int style = Gdiplus::FontStyleRegular, Gdiplus::Unit unit = Gdiplus::UnitPixel);
	void SetFormat(Gdiplus::StringAlignment align = Gdiplus::StringAlignmentCenter, Gdiplus::StringAlignment lineAlign = Gdiplus::StringAlignmentCenter);
	void SetColor(const Gdiplus::Color& color);

	const Gdiplus::Font* GetFont() const { return m_pFont; }				//lint !e1536
	const Gdiplus::StringFormat& GetFormat() const{ return m_format; }		//lint !e1536
	const Gdiplus::SolidBrush& GetBrush() const { return m_brush; }
};

#endif	//_OCX_TRAN_SLUCENT_STATIC_H_
