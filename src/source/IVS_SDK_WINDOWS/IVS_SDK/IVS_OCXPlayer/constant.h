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
 filename    :    constant.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    ���������ļ�
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 ��ʼ�汾
*****************************************************************/
#ifndef constant_h__
#define constant_h__

const int  REALTIMEBAR_BUTTON_CUT         = 1;
const int  REALTIMEBAR_BUTTON_NUM_BS      = 9;
const int  REALTIMEBAR_BUTTON_SPACE       = 2;
const int  REALTIMEBAR_BUTTON_WITDH       = 22 + REALTIMEBAR_BUTTON_SPACE;
//����ȡֵ��Χ
#define PLAYER_AUDIO_MIN_VOLUM 0                //��С����
#define PLAYER_AUDIO_MAX_VOLUM 100              //�������

const int ANALYSISTOOLBAR_BUTTON_HEIGHT   = 22;
const int ANALYSISTOOLBAR_BUTTON_WITDH    = 22;
const int ANALYSISTOOLBAR_BUTTON_SPACE    = 6;

#define MIN_PANE_NUMBER 1
#define MAX_PANE_NUMBER 144//��64�ĳ�81

const IVS_INT32 TOOLBARBTNNUM = 15;
const IVS_INT32 IATOOLBARBTNNUM = 5;

const ULONG LADDERLAYOUTMINWINDOWSNUM = 4;
const ULONG LADDERLAYOUTMAXWINDOWSNUM = 20;

const LONG  PANE_EDGE_PIXEL  = 2 ;
const LONG  TITLEBAR_HEIGHT  = 24;
const LONG  BOTTOMBAR_HEIGHT = 26;
const LONG  TVWALLSTATUSBAR_WIDTH = 22;
const LONG  TVWALLSTATUSBAR_HEIGHT = 22;
const ULONG  DISPLAY_ASPECT_RATIO_FULLSCREEN  =0 ; //����
const ULONG  DISPLAY_ASPECT_RATIO_ORIGINAL    =1 ; //ԭʼ����
const ULONG  DISPLAY_ASPECT_RATIO_NONSPECIFIC =2 ; //δָ����ʹ�õ�ǰֵ

#define COLOR_VIDEOPANE_BKG RGB(0,0,0)
#define COLOR_ACTIVATE_EDGE RGB(99,153,217)
#define COLOR_NOT_ACTIVATE_EDGE RGB(41,41,41)
#define COLOR_WARING_EDGE   RGB(255, 0, 0)
#define COLOR_TITLEBAR_BKG  RGB(46, 46, 46)
#define COLOR_BOTTOMBAR_BKG RGB(46, 46, 46)

#define MAXNUM_SHOW_BUTTON 5
#endif // constant_h__
