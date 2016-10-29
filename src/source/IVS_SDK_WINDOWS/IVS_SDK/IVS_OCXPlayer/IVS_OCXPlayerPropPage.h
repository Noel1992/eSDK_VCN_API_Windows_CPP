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

/*****************************************************************
 filename    :    IVS_OCXPlayerPropPage.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    用于显示自定义控件的属性在图形界面的
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#ifndef _IVS_OCX_PLAYER_PROPPAGE_H_
#define _IVS_OCX_PLAYER_PROPPAGE_H_

// IVS_OCXPlayerPropPage.h : Declaration of the CIVS_OCXPlayerPropPage property page class.
// CIVS_OCXPlayerPropPage : See IVS_OCXPlayerPropPage.cpp for implementation.

class CIVS_OCXPlayerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CIVS_OCXPlayerPropPage)
	DECLARE_OLECREATE_EX(CIVS_OCXPlayerPropPage)	//lint !e1768  MFC自动生成的的函数

// Constructor
public:
	CIVS_OCXPlayerPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_IVS_OCXPLAYER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
}; //lint !e1712  MFC自动生成的的函数

#endif	//_IVS_OCX_PLAYER_PROPPAGE_H_
