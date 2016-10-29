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
#ifndef _OCX_TRAN_SLUCENT_WND_H_
#define _OCX_TRAN_SLUCENT_WND_H_

#include "SDKDef.h"
#include <GdiPlus.h>
#include <vector>
#include "ENUM_DEF.h"

class CTranslucentWnd;

struct IRenderListener
{
	virtual ~IRenderListener() { }
	virtual void OnRenderEvent(CTranslucentWnd* translucentWnd) = 0;
};

// CTranslucentWnd

class CTranslucentWnd : public CWnd
{
	DECLARE_DYNAMIC(CTranslucentWnd)

public:
	CTranslucentWnd();
	virtual ~CTranslucentWnd();

	virtual void Render(Gdiplus::Graphics& g) = 0;

	BOOL ShowWindow(int nCmdShow);

	void SetRenderListener(IRenderListener* pRenderListener) { m_pRenderListener = pRenderListener; }
	void NotifyRender();

	void DestroyImageList();

	void DestroyImage(UINT index);

	bool IsDisabled() const { return m_bDisabled; }
	bool IsVisible() const { return m_bVisible; }

protected:
	virtual void UpdateState();
	DECLARE_MESSAGE_MAP()
public:
	virtual void PreSubclassWindow();

public:
	afx_msg void OnEnable(BOOL bEnable);

protected:
    std::vector<Gdiplus::Image*> m_imageList;

    IRenderListener* m_pRenderListener;
    bool m_bDisabled;
    bool m_bVisible;
};

#endif	//_OCX_TRAN_SLUCENT_WND_H_
