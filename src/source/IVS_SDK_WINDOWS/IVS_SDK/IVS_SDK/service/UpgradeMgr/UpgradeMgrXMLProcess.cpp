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

#include "UpgradeMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"

CUpgradeMgrXMLProcess::CUpgradeMgrXMLProcess()
{
}

CUpgradeMgrXMLProcess::~CUpgradeMgrXMLProcess()
{
}

IVS_INT32 CUpgradeMgrXMLProcess::GetCUVersionReqXML(const IVS_CU_INFO* pCUInfo, CXml& xml)
{
    CHECK_POINTER(pCUInfo,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("CUInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("CUType");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pCUInfo->uiCUType).c_str());//lint !e64 Æ¥Åä
    (void)xml.AddElem("CUVersion");
    
    IVS_CHAR cCUVersion[IVS_VERSION_LEN + 1] = {0};
    eSDK_STRNCPY(cCUVersion, sizeof(cCUVersion), pCUInfo->cCUVersion, IVS_VERSION_LEN);
    (void)xml.SetElemValue(cCUVersion);
    xml.OutOfElem();
    xml.OutOfElem();
    return 0;
}

IVS_INT32 CUpgradeMgrXMLProcess::ParseGetCuVersionRspXML(const std::string& strRspXML, IVS_UPGRADE_INFO* pUpgradeInfo)
{
    CHECK_POINTER(pUpgradeInfo,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    eSDK_MEMSET(pUpgradeInfo, 0, sizeof(IVS_UPGRADE_INFO));

    IVS_INT32 iRet = IVS_FAIL;
    CXml xml;
    if (!xml.Parse(strRspXML.c_str()))//lint !e64 Æ¥Åä
    {
        BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
        return iRet;
    }

    if (!xml.FindElemEx("Content/UpgradeInfo"))
    {
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();

    const char* szElemValue = NULL;

	char szIsUpgrade[IVS_URL_LEN+1] = {0};
	GET_ELEM_VALUE_CHAR("IsUpgrade", szElemValue, szIsUpgrade, IVS_URL_LEN,xml);//lint !e838
	IVS_BOOL bIsUpgrade = 0;
	if (0 == strcmp("Y", szIsUpgrade))
	{
		bIsUpgrade = 1;
	}
	else
	{
		bIsUpgrade = 0;
	}

    //GET_ELEM_VALUE_NUM("IsUpgrade", szElemValue, pUpgradeInfo->bIsUpgrade, xml);
	pUpgradeInfo->bIsUpgrade = bIsUpgrade;
    GET_ELEM_VALUE_CHAR("SetupFileMD5", szElemValue, pUpgradeInfo->cSetupFileMd5, IVS_MD5_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("FileSize", szElemValue, pUpgradeInfo->uiFileSize, xml);
    GET_ELEM_VALUE_CHAR("HttpURL", szElemValue, pUpgradeInfo->cHttpURL, IVS_URL_LEN, xml);    
    xml.OutOfElem();
    return IVS_SUCCEED;
}
