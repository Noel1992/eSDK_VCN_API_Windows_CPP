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

#include "StdAfx.h"
#include "memdc.h"

XCMemDC::XCMemDC()
{
    m_oldBitmap = NULL;
    m_pDC = NULL;
    m_bMemDC = FALSE;
}

XCMemDC::XCMemDC(CDC* pDC, const CRect* pRect, BOOL bBg)
{
    (void)CDC();

    m_oldBitmap = NULL;
    m_pDC = NULL;
    m_bMemDC = FALSE;

    Initialize(pDC, pRect, bBg);
}

void XCMemDC::Initialize(CDC* pDC, const CRect* pRect, BOOL bBg)
{
    ASSERT(pDC != NULL);

    m_pDC = pDC;
    m_oldBitmap = NULL;
    m_bMemDC = !pDC->IsPrinting();
    if (pRect == NULL)
    {
        (void)pDC->GetClipBox(&m_rect);
    }
    else
    {
        m_rect = *pRect;
    }

    if (m_bMemDC)
    {
        (void)CreateCompatibleDC(pDC);
        pDC->LPtoDP(&m_rect);

        (void)m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
        m_oldBitmap = SelectObject(&m_bitmap);

        (void)SetMapMode(pDC->GetMapMode());

        (void)SetWindowExt(pDC->GetWindowExt());
        (void)SetViewportExt(pDC->GetViewportExt());

        pDC->DPtoLP(&m_rect);
        (void)SetWindowOrg(m_rect.left, m_rect.top);
    }
    else
    {
        m_bPrinting = pDC->m_bPrinting;
        m_hDC = pDC->m_hDC;
        m_hAttribDC = pDC->m_hAttribDC;
    }

    if (bBg)
    {
        (void)BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                     m_pDC, m_rect.left, m_rect.top, SRCCOPY);
    }
    else
    {
        FillSolidRect(m_rect, pDC->GetBkColor());
    }
}

XCMemDC::~XCMemDC()
{
    try
    {
        if (m_bMemDC)
        {
            // Copy the offscreen bitmap onto the screen.
            if (m_pDC != NULL)
            {
                (void)m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                                    this, m_rect.left, m_rect.top, SRCCOPY);
            }

            //Swap back the original bitmap.
            (void)SelectObject(m_oldBitmap);
			(void)m_bitmap.DeleteObject();
        }
        else
        {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }

        m_oldBitmap = NULL;
        m_pDC = NULL;
    } catch (...) {}
}
