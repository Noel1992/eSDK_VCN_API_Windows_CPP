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

// TranslucentWnd.cpp : implementation file
//
#include "stdafx.h"
#include "TranslucentWnd.h"

// CTranslucentWnd

IMPLEMENT_DYNAMIC(CTranslucentWnd, CWnd)

CTranslucentWnd::CTranslucentWnd()
    : CWnd()
    , m_pRenderListener(NULL)
    , m_bDisabled(false)
    , m_bVisible(true)
{
}

CTranslucentWnd::~CTranslucentWnd()
{
    try
    {
        m_pRenderListener = NULL;
        DestroyImageList();
    }
    catch (...)
    {
        ;
    }
}

BEGIN_MESSAGE_MAP(CTranslucentWnd, CWnd)
    ON_WM_ENABLE()
END_MESSAGE_MAP()

// CTranslucentWnd message handlers
void CTranslucentWnd::OnEnable(BOOL bEnable)
{
    CWnd::OnEnable(bEnable);

    bool bDisabled = (bEnable != TRUE);
    if (static_cast<int>(bDisabled) != static_cast<int>(m_bDisabled))
    {
        m_bDisabled = bDisabled;
        UpdateState();
    }
}

BOOL CTranslucentWnd::ShowWindow(int nCmdShow)
{
    BOOL bPrevious = CWnd::ShowWindow(nCmdShow);
    bool visible = (GetWindowLong(GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE) != 0;
    if (static_cast<int>(visible) != static_cast<int>(m_bVisible))
    {
        m_bVisible = visible;
        NotifyRender();
    }

    return bPrevious;
}

void CTranslucentWnd::UpdateState()
{
    NotifyRender();
}

void CTranslucentWnd::NotifyRender()
{
    if (m_pRenderListener != NULL)
        m_pRenderListener->OnRenderEvent(this);
}

void CTranslucentWnd::DestroyImageList()
{
	size_t nSize = m_imageList.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		if (m_imageList[i] != NULL)
		{
			//Gdiplus::DllExports::GdipFree(m_imageList[i]);
			try
			{
				delete m_imageList[i];
				m_imageList[i] = NULL;
			}
			catch (...)
			{
				m_imageList[i] = NULL;
			}
		}
	}
}

void CTranslucentWnd::DestroyImage(UINT index)
{
	if (index < m_imageList.size())
	{
		if (m_imageList[index] != NULL)
		{
			//Gdiplus::DllExports::GdipFree(m_imageList[index]);
			try
			{
				delete m_imageList[index];
				m_imageList[index] = NULL;
			}
			catch (...)
			{
				m_imageList[index] = NULL;
			}
		}
	}
}

void CTranslucentWnd::PreSubclassWindow()
{
    LONG style = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
    m_bVisible = (style & WS_VISIBLE) != 0;
    m_bDisabled = (style & WS_DISABLED) != 0;
}

