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

#ifndef __ZOOM_SLIDER_CTRL_H_
#define __ZOOM_SLIDER_CTRL_H_

#include "BitmapSlider.h"

const ULONG MAGBIFYCTRL_SLIDER_INTERVAL = 1; // 滑动条间隔
const ULONG MAGBIFYCTRL_SLIDER_MAX = 800;     // 滑动条长度
const ULONG MAGBIFYCTRL_SLIDER_MIN = 100;     // 起点
const ULONG MAGBIFYCTRL_CLICK_INTERVAL = 100;     // 点击增加间隔
const ULONG CHANNEL_HEIGHT = 10;            // 滑动条高度

// CVideoSoundCtrl dialog

class CZoomSliderCtrl : public CDialog
{
	DECLARE_DYNAMIC(CZoomSliderCtrl)

public:
	CZoomSliderCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CZoomSliderCtrl();

// Dialog Data
	enum { IDD = IDD_DLG_MAGNIFYCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	//CWnd *        m_pRealTimeToolBar;         
	CBitmapSlider m_sliderMagbifyCtrl;
	ULONG         m_ulSliderPosition;  // 滑动条位置	
public:
	virtual BOOL    OnInitDialog();
	afx_msg void    OnPaint();
	afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
	afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);

	LRESULT        OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);   // 进度条移动完事件
	void           ProcessSliderMoved();  // 播放进度滑动条滑动事件

	// 设置位置
	void           SetSliderPosition(ULONG ulPos);
	ULONG          GetSliderPosition()const;
	void           IncSliderPosition();
	void           DecSliderPosition();
public:
	CBitmapSlider& GetMagbifyCtrl(){ return m_sliderMagbifyCtrl;} //lint !e1536
};
#endif // __ZOOM_SLIDER_CTRL_H_
