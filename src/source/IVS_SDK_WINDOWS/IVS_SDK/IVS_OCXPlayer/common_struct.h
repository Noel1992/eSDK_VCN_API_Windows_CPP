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
 filename    :    common_struct.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    常用结构体
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#ifndef _COMMON_STRUCT_H_A9366BA0_5095_4557_970B_4D3881F526A6_
#define _COMMON_STRUCT_H_A9366BA0_5095_4557_970B_4D3881F526A6_

#include "VideoPane.h"

#define CAMERA_ID_LEN 32

//华赛SDK远程录像下载会话号与句柄;
typedef struct _stPORT_SESSIONID_INFO
{
    int     iHsSessionID;
    ULONG   iHsHandle;
}PORT_SESSIONID_INFO;

typedef struct _stHSHANDLE_INFO
{
    TCHAR   szRecordPath[MAX_PATH];    // 录像下载位置
    ULONG   ulPlayedTime;              // 当前播放时间
    ULONG   ulDownloadedSize;          // 当前下载录像大小
}HSHANDLE_INFO , *LPHSHANDLE_INFO;

typedef struct  _stCaptureMsgData
{
    unsigned long ulRetCode;
    unsigned long ulSuccessPicNum;
    unsigned long ulTotalPicNum;
}CAPTURE_MSG_DATA, *LPCAPTURE_MSG_DATA;


typedef std::map <ULONG , PORT_SESSIONID_INFO>  DOWNLOAD_HANDLE_MAP;    //华赛SDK远程录像下载map表;
typedef std::map <ULONG, CVideoPane*>           MAP_VIDEOPANE;          // 管理窗格的map表
typedef MAP_VIDEOPANE::iterator                 MAP_VIDEOPANE_ITER;          
typedef std::pair<const ULONG, CVideoPane*>     PairVideoPane;
typedef std::map <HWND, CVideoPane*>           WATER_MASK_VIDEO_PANE_MAP;      // 水印map表(窗口句柄->窗口)
typedef std::map <std::string, HWND>                 WATER_MASK_CAMERA_MAP;      // 水印map表(CameraID->窗口句柄)
typedef std::list<CRect> RECTLIST;
typedef RECTLIST::iterator RECTLISTITER;


typedef std::map<HWND, ULONG*> HWND_WID_MAP;

#endif // _COMMON_STRUCT_H_A9366BA0_5095_4557_970B_4D3881F526A6_
