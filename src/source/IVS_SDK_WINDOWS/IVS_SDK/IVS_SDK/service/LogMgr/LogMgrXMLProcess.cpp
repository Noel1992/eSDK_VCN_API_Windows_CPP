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

#include "LogMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "ParaCheck.h"

CLogMgrXMLProcess::CLogMgrXMLProcess()
{
}

CLogMgrXMLProcess::~CLogMgrXMLProcess()
{
}

// 解析响应xml
IVS_INT32 CLogMgrXMLProcess::PraseResOperationLogXML(CXml& xml,IVS_OPERATION_LOG_LIST* pResOperationLog,const IVS_UINT32& uiBufSize, const CSDKSecurityClient& oSecurityClient)
{
	CHECK_POINTER(pResOperationLog, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML ", "xml.FindElem(Content) is fail");
		return IVS_XML_INVALID;
	}   
	xml.IntoElem();
	if (!xml.FindElem("PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "xml.FindElem(PageInfo) is fail");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();

	eSDK_MEMSET(pResOperationLog, 0, uiBufSize);
	const char* szElemValue = NULL;
	IVS_INT32 iTotal = 0;
	IVS_INT32 iFromIndex = 0;
	IVS_INT32 iToIndex = 0;
	GET_ELEM_VALUE_NUM("RealNum", szElemValue,iTotal , xml);//lint !e838
	GET_ELEM_VALUE_NUM("FromIndex", szElemValue, iFromIndex, xml);//lint !e838
	GET_ELEM_VALUE_NUM("ToIndex",szElemValue,iToIndex , xml);//lint !e838

	// 判断RealNum，FromIndex，ToIndex是否合法
	if (iTotal < 0 || iToIndex < 0 || iFromIndex < 0 || iFromIndex > iToIndex)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "ToIndex[%d], FromIndex[%d], ToIndex[%d]", iTotal, iFromIndex, iToIndex);
		return IVS_XML_INVALID;
	}	

	pResOperationLog->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromIndex);
	pResOperationLog->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToIndex);
	pResOperationLog->uiTotal = static_cast<IVS_UINT32>(iTotal);
	xml.OutOfElem();	
	if (0 == iTotal)
	{
		return IVS_SUCCEED;
	}
	if (!xml.FindElem("OperationLogList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogList) failed");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();  
	// 响应消息中传回来的页数
	IVS_UINT32 uiResNum = ((pResOperationLog->stIndexRange).uiToIndex - (pResOperationLog->stIndexRange).uiFromIndex) + 1;
	// 实际要用到的内存大小
	IVS_UINT32 uiResBufSize = sizeof(IVS_OPERATION_LOG_LIST) + ((uiResNum) - 1) * sizeof(IVS_OPERATION_LOG_INFO);
	// 实际需要内存与分配内存大小的比较
	if (uiBufSize < uiResBufSize)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Prase Response Operation Log XML","memory is not enough");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_UINT32 i = 0;	

    if (!xml.FindElemValue("OperationLogInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Prase Response Operation Log XML", "Find Elememt(OperationLogInfo) failed");
        return IVS_XML_INVALID;
    }  
    xml.IntoElem();	

    BP_DBG_LOG("PraseResOperationLogXML", "uiResNum[%d].", uiResNum);

    char szBuf[2048];
    while(i < uiResNum)
    {
        IVS_OPERATION_LOG_INFO &pOperationLogInfo = pResOperationLog->stOperationInfo[i];
        IVS_INT32 iTemp = 0;
        
        eSDK_MEMSET(szBuf, 0x00, sizeof(szBuf));
        if (xml.FindElemValue("OperationLogInfo"))
        {
            const char* pValue = NULL;
            pValue = xml.GetElemValue();//lint !e838
            if (NULL != pValue)                                                            
            {   
                eSDK_MEMCPY(szBuf, 2048, pValue, 2047);
            }
         }

        //进行解密		
        std::string str;
        IVS_INT32 iRet = oSecurityClient.DecryptString(szBuf, str);
        if (IVS_SUCCEED == iRet)
        {
            if(str.length() > (sizeof(szBuf) -1))
            {
                BP_RUN_LOG_ERR(IVS_DATA_DECRYPT_ERROR, "", "size is too large.");
                return IVS_DATA_DECRYPT_ERROR;
            }

            eSDK_STRNCPY(szBuf, IVS_NAME_LEN, str.c_str(),str.length());
            szBuf[str.length()] = '\0';
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_DATA_DECRYPT_ERROR, "Prase ResOperation Log XML", "DecryptString fail");
            return IVS_DATA_DECRYPT_ERROR;
        }

        std::string strTemp = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><Content>";
        strTemp += str;
        strTemp += "</Content>";

        CXml xmlMini;
        if (!xmlMini.Parse(strTemp.c_str()))//lint !e64 匹配
        {
            BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Operation Log", "xmlRsp.Parse(pRsp) is fail");
            return IVS_XML_INVALID;
        }

        if(!xmlMini.FindElemEx("Content"))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "!xml.FindElemEx(Content/Decoder)", "NA");
            return IVS_XML_INVALID;
        }  

        GET_ELEM_VALUE_CHAR("Account", szElemValue,  pOperationLogInfo.cAccount, (IVS_NAME_LEN), xmlMini);
        BP_RUN_LOG_INF("PraseResOperationLogXML", "pOperationLogInfo.cAccount[%s].",
                pOperationLogInfo.cAccount);
        
        GET_ELEM_VALUE_CHAR("ClientIP", szElemValue, pOperationLogInfo.cClientIp, (IVS_IP_LEN), xmlMini);
        GET_ELEM_VALUE_NUM("Grade", szElemValue, iTemp,xmlMini);//lint !e838
        pOperationLogInfo.iGrade = static_cast<IVS_UINT32>(iTemp);
        GET_ELEM_VALUE_CHAR("ServiceCode", szElemValue,pOperationLogInfo.cServiceCode, (IVS_DESCRIBE_LEN), xmlMini);
        GET_ELEM_VALUE_CHAR("ModuleType", szElemValue,pOperationLogInfo.cModuleType, (IVS_DESCRIBE_LEN), xmlMini);
        GET_ELEM_VALUE_CHAR("ModuleCode", szElemValue,pOperationLogInfo.cModuleCode, (IVS_DEV_CODE_LEN), xmlMini);
        GET_ELEM_VALUE_CHAR("OperationCode", szElemValue,pOperationLogInfo.cOperationCode, (IVS_DEV_CODE_LEN), xmlMini);
        GET_ELEM_VALUE_CHAR("OperationObjectCode", szElemValue,pOperationLogInfo.cOperationObjectCode, (IVS_DESCRIBE_LEN), xmlMini);
        // 判断是否需要转码
		IVS_BOOL bTranslated = CSDKConfig::instance().GetTranslateConfig();
		if (TRUE == bTranslated)
		{
			// utf8->ansi
			char cOperationObjectCode[IVS_DESCRIBE_LEN + 1] = {0};
			(void)CToolsHelp::Memcpy(cOperationObjectCode, IVS_DESCRIBE_LEN, pOperationLogInfo.cOperationObjectCode,IVS_DESCRIBE_LEN);
			char* pDest = CToolsHelp::UTF8ToANSI(cOperationObjectCode);
			if (NULL != pDest)
			{
				// 清理掉先
				eSDK_MEMSET(pOperationLogInfo.cOperationObjectCode, 0, IVS_DESCRIBE_LEN);
				(void)CToolsHelp::Memcpy(pOperationLogInfo.cOperationObjectCode, IVS_DESCRIBE_LEN, pDest,strlen(pDest));
				free(pDest);
				pDest = NULL;
			}
		}//lint !e438
        
        GET_ELEM_VALUE_CHAR("OccurTime", szElemValue,pOperationLogInfo.cOccurTime, (IVS_TIME_LEN),xmlMini);
        GET_ELEM_VALUE_CHAR("ErrorCode", szElemValue,pOperationLogInfo.cErrorCode, (IVS_DESCRIBE_LEN), xmlMini);
        GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, xmlMini);
        pOperationLogInfo.iResult = static_cast<IVS_UINT32>(iTemp);
        GET_ELEM_VALUE_CHAR("AdditionalInfo", szElemValue,pOperationLogInfo.cAdditionalInfo, (IVS_DESCRIBE_LEN), xmlMini);

        if (!xml.NextElem())
        {
            break;
        }
        i++;
    }//lint !e438    

    xml.OutOfElem();
    return IVS_SUCCEED;
}


//拼装查询操作日志请求Xml
IVS_INT32 CLogMgrXMLProcess::GetResOperationLogExXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
	const IVS_QUERY_OPTLOG_EX *pOptLogQueryEx,
	CXml &xml)
{
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR); 
	CHECK_POINTER(pOptLogQueryEx, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("GetResOperationLogExXML NVRCode: %s, ModuleName: %s", 
		pOptLogQueryEx->cNVRCode, pOptLogQueryEx->cModuleName);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	
	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("LoginID");
	(void)xml.IntoElem();
	IVS_CHAR cLoginID[IVS_NAME_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cLoginID, IVS_NAME_LEN + 1, pOptLogQueryEx->cLoginID, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetResOperationLogExXML failed", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLoginID);
	xml.OutOfElem();

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiFromIndex)).c_str());//lint !e64 匹配
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiToIndex)).c_str());//lint !e64 匹配

	IVS_UINT32 iFieldNum = static_cast<IVS_UINT32>(pUnifiedQuery->iFieldNum);	

	if (iFieldNum > 0) //如果是0的话，这个QueryCond节点都不要了
	{
		IVS_CHAR szValue[IVS_QUERY_VALUE_LEN+1]={0};
		(void)xml.AddElem("QueryCond");	
		for (unsigned int i=0;i < iFieldNum;i++)
		{	
			if (0 == i)
			{
				(void)xml.AddChildElem("QueryField");
			}
			else
			{
				(void)xml.AddElem("QueryField");	
			}

			(void)xml.IntoElem();
			(void)xml.AddChildElem("Field");
			(void)xml.IntoElem();

			std::string  iField = QUERYFILED.GetValue(pUnifiedQuery->stQueryField[i].eFieID);
			(void)xml.SetElemValue(iField.c_str());//lint !e64 匹配
			(void)xml.AddElem("Value");
			if (!CToolsHelp::Memcpy(szValue, IVS_QUERY_VALUE_LEN, pUnifiedQuery->stQueryField[i].cValue, IVS_QUERY_VALUE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Unified Format Query XML", "Memcpy szValue failed");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(szValue);

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stQueryField[i].bExactQuery);

			(void)xml.AddElem("QueryType");
			if (MATCHED_SUCCEED == pUnifiedQuery->stQueryField[i].bExactQuery)
			{
				(void)xml.SetElemValue("EXACT");
			}
			else
			{
				(void)xml.SetElemValue("INEXACT");
			}
			xml.OutOfElem();
		}
		xml.OutOfElem();

		//注意如果这里的值不是0/1，直接返回参数错误的错误码
		CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bEnableOrder);

		if (pUnifiedQuery->stOrderCond.bEnableOrder)
		{
			(void)xml.AddElem("OrderCond");
			(void)xml.AddChildElem("OrderField");
			(void)xml.IntoElem();	
			std::string iOrderField = QUERYFILED.GetValue(pUnifiedQuery->stOrderCond.eFieID);
			(void)xml.SetElemValue(iOrderField.c_str());//lint !e64 匹配

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bUp);

			(void)xml.AddElem("Order");
			if (MATCHED_SUCCEED==pUnifiedQuery->stOrderCond.bUp)
			{
				(void)xml.SetElemValue("UP");
			}
			else
			{
				(void)xml.SetElemValue("DOWN");
			}
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();

	(void)xml.AddElem("NodeCode");
	IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
	if(!CToolsHelp::Strncpy(cNVRCode, IVS_NVR_CODE_LEN + 1, pOptLogQueryEx->cNVRCode, IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "GetResOperationLogExXML failed", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cNVRCode);

	(void)xml.AddElem("ModuleName");
	IVS_CHAR cTmpModuleName[IVS_DESCRIBE_LEN + 1] = {0};
	(void)CToolsHelp::Strncpy(cTmpModuleName, IVS_DESCRIBE_LEN + 1, pOptLogQueryEx->cModuleName, IVS_DESCRIBE_LEN);
	(void)xml.SetElemValue(cTmpModuleName);
	(void)xml.AddElem("PortalClientIP");
	IVS_CHAR cTmpIP[IVS_IP_LEN + 1] = {0};
	(void)CToolsHelp::Strncpy(cTmpIP, IVS_IP_LEN + 1, pOptLogQueryEx->cPortalClientIP, IVS_IP_LEN);
	(void)xml.SetElemValue(cTmpIP);

	return IVS_SUCCEED;
}

