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

// PalyBackPane.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "PalyBackPane.h"
#include "afxdialogex.h"


// CPalyBackPane dialog

IMPLEMENT_DYNAMIC(CPalyBackPane, CDialogEx)

CPalyBackPane::CPalyBackPane(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPalyBackPane::IDD, pParent)
{

}

CPalyBackPane::~CPalyBackPane()
{
}

void CPalyBackPane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAYBACK, m_btnPlay);
}


BEGIN_MESSAGE_MAP(CPalyBackPane, CDialogEx)
END_MESSAGE_MAP()//lint !e751


// CPalyBackPane message handlers
