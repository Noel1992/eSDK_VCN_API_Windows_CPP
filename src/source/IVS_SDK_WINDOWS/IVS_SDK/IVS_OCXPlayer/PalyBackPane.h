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

#ifndef _PLAYBACK_PANE_H__
#define _PLAYBACK_PANE_H__
#include "afxwin.h"


// CPalyBackPane dialog
//用来设备录像回放背景上按钮的类
class CPalyBackPane : public CDialogEx
{
	DECLARE_DYNAMIC(CPalyBackPane)

public:
	CPalyBackPane(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPalyBackPane();

// Dialog Data
	enum { IDD = IDD_DLG_PLAYBACKPANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnPlay;
};

#endif //_PLAYBACK_PANE_H__
