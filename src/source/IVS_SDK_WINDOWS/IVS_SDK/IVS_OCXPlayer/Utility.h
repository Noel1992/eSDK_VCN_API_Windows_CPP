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

#ifndef UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_
#define UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_

#include "stdafx.h"

#ifndef _CVIDEOPANEMGR_DECLARATION_
#define _CVIDEOPANEMGR_DECLARATION_
class CVideoPaneMgr;
#endif


class CUtility
{
public:
    static bool CheckIsWin7(void);

    //从资源获取PNG图片 到 Image
    //static  bool ImageFromIDResource(UINT nID, LPCTSTR sTR,Image * &pImg);

private:
    static CVideoPaneMgr *pVideoPaneMgr;
public:
    static CVideoPaneMgr * GetVideoPaneMgr();
    static void SetVideoPaneMgr(CVideoPaneMgr* val); 

};

#endif // UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_
