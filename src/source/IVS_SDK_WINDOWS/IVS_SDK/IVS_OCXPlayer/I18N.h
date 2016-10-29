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

/********************************************************************
	filename	: 	IVS_OCXPlayerRes.h
	author		:	z00201790
	created		:	2013/01/24	
	description	:	定义多国语言资源处理的类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/01/24 初始版本;
*********************************************************************/
#ifndef __IVS_I18N_H__
#define __IVS_I18N_H__
#include "vos.h"
#include "SDKDef.h"
typedef std::map<std::string,std::string> MAP_RESOURCE_TO_LANGUAGE;
typedef MAP_RESOURCE_TO_LANGUAGE::iterator MAP_RESOURCE_TO_LANGUAGE_ITER;

class CI18N
{
public:
	CI18N(void);
	~CI18N(void);

	static CI18N& GetInstance()
	{
		static CI18N inst;
		return inst;
	}
public:
	//根据XML的资源路径将XML加载到Map中
	bool SetLanguage(const std::string& strLanguageXMLPath);
	bool GetResourceLanguage(const std::string& strResource,std::string& strLanguage);
	void Clear();

private:
	VOS_Mutex* m_pMutexLock;

	MAP_RESOURCE_TO_LANGUAGE m_mapResource2Language;
};

#endif
