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

#include "SDKMainService.h"
#include "bp_environments.h"
#include "ivs_xml.h"
#include "ivs_error.h"
#include "IVSCommon.h"
#include "ToolsHelp.h"
#include "RecordXMLProcess.h"

// 处理告警信息通知Cmd
IVS_ALARM_NOTIFY* CSDKMainService::ProcessAlarmAlarmNotifyCmd(CCmd* pCmd)
{
	BP_RUN_LOG_INF("Process AlarmAlarmNotify", "start");
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "cmd is null", "NA");
		return NULL;
	}

	int len = 0;
	IVS_CHAR* pData = CNSSOperator::instance().ParseCmd2Data(pCmd, len);
	CHECK_POINTER(pData, NULL);
	if (len < (int)((((sizeof(IVS_ALARM_NOTIFY) - IVS_ALARM_DESCRIPTION_LEN) - 32) - IVS_ALARM_EX_PARAM_LEN) + 6))  // 32为保留字段cReserver,6为TLV头长度 
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "data from server error", "data error");
        IVS_DELETE(pData, MUILI);
        return NULL;
    }
	IVS_ALARM_NOTIFY* pAlarmNotifyTmp = reinterpret_cast<IVS_ALARM_NOTIFY*>(pData); //lint !e826
//     if (NULL == pAlarmNotifyTmp)
//     {
//         BP_RUN_LOG_ERR(IVS_SDK_RET_INTRINSIC_PTR_ERROR, "pAlarmNotifyTmp is null", "NA");
//         IVS_DELETE(pData, MUILI);
//         return NULL;
//     }
	// 将网络字节序转成主机字节序 ntohl 32 ntohs16
	BP_ntohll(pAlarmNotifyTmp->ullAlarmEventID, pAlarmNotifyTmp->ullAlarmEventID);
	pAlarmNotifyTmp->uiAlarmInType = ntohl(pAlarmNotifyTmp->uiAlarmInType);
	pAlarmNotifyTmp->uiAlarmLevelValue = ntohl(pAlarmNotifyTmp->uiAlarmLevelValue);
	pAlarmNotifyTmp->uiAlarmStatus = ntohl(pAlarmNotifyTmp->uiAlarmStatus);
	pAlarmNotifyTmp->uiOccurNumber = ntohl(pAlarmNotifyTmp->uiOccurNumber);

	pAlarmNotifyTmp->bExistsRecord = static_cast<IVS_INT32>(ntohl(static_cast<IVS_UINT32>(pAlarmNotifyTmp->bExistsRecord)));
	pAlarmNotifyTmp->bIsCommission = static_cast<IVS_INT32>(ntohl(static_cast<IVS_UINT32>(pAlarmNotifyTmp->bIsCommission)));

	// 拷贝前几个字节
	IVS_UINT32 iSize = ((sizeof(IVS_ALARM_NOTIFY) - IVS_ALARM_DESCRIPTION_LEN) - IVS_ALARM_EX_PARAM_LEN) - 32;
	IVS_ALARM_NOTIFY* pAlarmNotify = IVS_NEW(pAlarmNotify);
    if (NULL == pAlarmNotify)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pAlarmNotify is null", "NA");
        IVS_DELETE(pData, MUILI);
        return NULL;
    }
	eSDK_MEMSET(pAlarmNotify, 0x0, sizeof(IVS_ALARM_NOTIFY));
	if (!CToolsHelp::Memcpy(pAlarmNotify, sizeof(IVS_ALARM_NOTIFY), pAlarmNotifyTmp, iSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmNotify);
		return NULL;
	}
	// 对两个TLV字段(告警描述，扩展参数)的处理
	TNssTLVMsg tlvMsg;
	CNSSOperator::instance().ParseData2TLV((void*)(pData + iSize), tlvMsg);//访问越界
	
	// 判断tag
	if (tlvMsg.usiTag != ALARMDESC_TAG) //lint !e1013
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "tag error", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmNotify);
		return NULL;
	}
	//  IVS_ALARM_DESCRIPTION_LEN
	if (tlvMsg.uiLength > IVS_ALARM_DESCRIPTION_LEN)  //lint !e1013
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "length error", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmNotify);
		return NULL;
	}
	else if (0 == tlvMsg.uiLength)
	{
		BP_RUN_LOG_WAR("Process alarm alarm notify", "the length of the first TLV = 0");
	}
	else
	{
		if (NULL == tlvMsg.pszValue)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Process alarm alarm notify fail", "the value of the first TLV is NULL");
			IVS_DELETE(pData, MUILI);
			IVS_DELETE(pAlarmNotify);
			return NULL;
		}
		if (!CToolsHelp::Memcpy((IVS_CHAR*)pAlarmNotify + iSize + 32, IVS_ALARM_DESCRIPTION_LEN, tlvMsg.pszValue, tlvMsg.uiLength))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
			IVS_DELETE(pData, MUILI);
			IVS_DELETE(pAlarmNotify);
			return NULL;
		}
	}

	// 开始处理第二个TLV
	iSize += sizeof(IVS_USHORT) + sizeof(IVS_INT32) + tlvMsg.uiLength;
	eSDK_MEMSET(&tlvMsg, 0x0, sizeof(TNssTLVMsg));
	CNSSOperator::instance().ParseData2TLV((void*)(pData + iSize), tlvMsg);
	if (EXTPARAM_TAG != tlvMsg.usiTag)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "tag error", "NA");
		IVS_DELETE(pData, MUILI);
		IVS_DELETE(pAlarmNotify);
		return NULL;
	}
	if (IVS_ALARM_EX_PARAM_LEN < tlvMsg.uiLength)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "0x0004 != tlvMsg.uiLength(second) fail", "NA");
		IVS_DELETE(pData, MUILI);
		IVS_DELETE(pAlarmNotify);
		return NULL;
	}
	else if (0 == tlvMsg.uiLength)
	{
		BP_RUN_LOG_WAR("Process alarm alarm notify", "the length of the second TLV = 0");
	}
	else
	{
		if (NULL == tlvMsg.pszValue)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Process alarm alarm notify fail", "the value of the second TLV is NULL");
			IVS_DELETE(pData, MUILI);
			IVS_DELETE(pAlarmNotify);
			return NULL;
		}
		iSize = sizeof(IVS_ALARM_NOTIFY) - IVS_ALARM_EX_PARAM_LEN;
		if (!CToolsHelp::Memcpy((IVS_CHAR*)pAlarmNotify + iSize, IVS_ALARM_EX_PARAM_LEN, tlvMsg.pszValue, tlvMsg.uiLength))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
			IVS_DELETE(pData, MUILI);
			IVS_DELETE(pAlarmNotify);
			return NULL;
		}
	}

	IVS_DELETE(pData, MUILI);
	BP_RUN_LOG_INF("Process AlarmAlarmNotify", "end");
	return pAlarmNotify;
}

//处理联动动作执行通知
IVS_CHAR* CSDKMainService::ProcessMAUCmd(CCmd* pCmd)
{
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "cmd is null", "NA");
		return NULL;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	IVS_CHAR* pDataXML = CNSSOperator::instance().ParseCmd2XML(pCmd, iRet);
	BP_RUN_LOG_INF("CNSSOperator::instance().ParseCmd2XML(pCmd, iRet)", "iRet = %d", iRet);
	if (NULL == pDataXML)
	{
		BP_RUN_LOG_INF("NULL == pDataXML", "NA");
		return NULL;
	}

	return pDataXML;
}

IVS_ALARM_STATUS_NOTIFY* CSDKMainService::ProcessAlarmStatusNotify(CCmd* pCmd)
{
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "cmd is null", "NA");
		return NULL;
	}

	int len = 0;
	IVS_CHAR* pData = CNSSOperator::instance().ParseCmd2Data(pCmd, len);
    CHECK_POINTER(pData, NULL);
    if (len < (int)(((sizeof(IVS_ALARM_STATUS_NOTIFY) - sizeof(IVS_ALARM_OPERATE_INFO)) - 32) + 6)) // 32为保留字段cReserver,6为TLV头长度
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "data from server error", "data error");
        IVS_DELETE(pData, MUILI);
        return NULL;
    }
	// 获得告警上报信息
	IVS_ALARM_STATUS_NOTIFY* pAlarmStatusNotifyTmp = reinterpret_cast<IVS_ALARM_STATUS_NOTIFY*>(pData); //lint !e826
//     if (NULL == pAlarmStatusNotifyTmp)
//     {
//         BP_RUN_LOG_ERR(IVS_SDK_RET_INTRINSIC_PTR_ERROR, "pAlarmStatusNotifyTmp is null", "NA");
//         IVS_DELETE(pData, MUILI);
// 		return NULL;
//     }
	IVS_ALARM_STATUS_NOTIFY* pAlarmStatusNotify = IVS_NEW(pAlarmStatusNotify);
    if (NULL == pAlarmStatusNotify)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pAlarmStatusNotifyTmp is null", "NA");
        IVS_DELETE(pData, MUILI);
		return NULL;
    }
	eSDK_MEMSET(pAlarmStatusNotify, 0x0, sizeof(IVS_ALARM_STATUS_NOTIFY));
	/*=========================== Notify ======================================*/
	// 将网络字节序转成主机字节序 ntohl 32 ntohs16
	BP_ntohll(pAlarmStatusNotifyTmp->ullAlarmEventID, pAlarmStatusNotifyTmp->ullAlarmEventID);
	pAlarmStatusNotifyTmp->uiAlarmStatus = ntohl(pAlarmStatusNotifyTmp->uiAlarmStatus);
	pAlarmStatusNotifyTmp->uiOccurNumber = ntohl(pAlarmStatusNotifyTmp->uiOccurNumber);

	// 拷贝AlarmNotify
	IVS_UINT32 iSize = (sizeof(IVS_ALARM_STATUS_NOTIFY) - 32) - sizeof(IVS_ALARM_OPERATE_INFO);
	if (!CToolsHelp::Memcpy(pAlarmStatusNotify, sizeof(IVS_ALARM_STATUS_NOTIFY), pAlarmStatusNotifyTmp, iSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmStatusNotify);
		return NULL;
	}

	/*=========================== Operator =====================================*/
	IVS_ALARM_OPERATE_INFO* pAlarmOperateInfo = reinterpret_cast<IVS_ALARM_OPERATE_INFO*>(pData + iSize);  //lint !e826
//     if (NULL == pAlarmOperateInfo)
//     {
//         BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pAlarmOperateInfo is null", "NA");
//         IVS_DELETE(pData, MUILI);
//         IVS_DELETE(pAlarmStatusNotify);
// 		return NULL;
//     }
	// 将网络字节序转成主机字节序 ntohl 32 ntohs16
	pAlarmOperateInfo->uiOperatorID = ntohl(pAlarmOperateInfo->uiOperatorID);

	// 拷贝Operator
	IVS_UINT32 uiSize = sizeof(IVS_ALARM_OPERATE_INFO) - IVS_ALARM_DESCRIPTION_LEN;
	if (!CToolsHelp::Memcpy((IVS_CHAR*)pAlarmStatusNotify + iSize + 32, sizeof(IVS_ALARM_OPERATE_INFO), pAlarmOperateInfo, uiSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmStatusNotify);
		return NULL;
	}

	/*========================= 对TLV字段的处理 ================================*/
	TNssTLVMsg tlvMsg;
	CNSSOperator::instance().ParseData2TLV((void*)(pData + iSize + uiSize), tlvMsg);

	// 判断tag
	if (tlvMsg.usiTag != OPERATEINFO_TAG) //lint !e1013
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "tag error", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmStatusNotify);
		return NULL;
	}
	if (tlvMsg.uiLength > IVS_ALARM_DESCRIPTION_LEN)  //lint !e1013
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "length error", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmStatusNotify);
		return NULL;
	}
	if (!CToolsHelp::Memcpy((IVS_CHAR*)pAlarmStatusNotify + iSize + 32 + uiSize, IVS_ALARM_DESCRIPTION_LEN,tlvMsg.pszValue, tlvMsg.uiLength))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
        IVS_DELETE(pData, MUILI);
        IVS_DELETE(pAlarmStatusNotify);
		return NULL;
	}
	// 删除内存
	IVS_DELETE(pData, MUILI);

	return pAlarmStatusNotify;
}

// 推送发现到的前端设备;模式轨迹录制结束通知
IVS_CHAR* CSDKMainService::ProcessDevNotify(CCmd* pCmd)
{
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "cmd is null", "NA");
		return NULL;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	IVS_CHAR* pDataXML = CNSSOperator::instance().ParseCmd2XML(pCmd, iRet);
	BP_RUN_LOG_INF("Parse Cmd2XML(pCmd) ReturnCode", "iRet = %d", iRet);
	if (NULL == pDataXML)
	{
		BP_RUN_LOG_INF("NULL == pDataXML", "NA");
		return NULL;
	}

	return pDataXML;
}
// 设备告警上报
IVS_INT32 CSDKMainService::ProcessDevAlarmNotify(CCmd* pCmd, IVS_DEVICE_ALARM_NOTIFY* pDevAlarmNotify)
{
	if (NULL == pCmd)
	{
		BP_RUN_LOG_INF("NULL == pCmd", "NA");
		return IVS_FAIL;
	}
	if (NULL == pDevAlarmNotify)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "NULL == pDevAlarmNotify", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_INT32 xmlLen = 0;
	IVS_CHAR* pData = CNSSOperator::instance().ParseCmd2Data(pCmd, xmlLen);
	CHECK_POINTER(pData, IVS_FAIL);
    int iNeedLen = (int)((sizeof(IVS_DEVICE_ALARM_NOTIFY) - IVS_ALARM_DESCRIPTION_LEN) - IVS_ALARM_EX_PARAM_LEN) + 12;	// 12是两个TLV需要的最小空间大小
    if (xmlLen < iNeedLen)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "data from server error", "data error");
        IVS_DELETE(pData, MUILI);
        return IVS_FAIL;
    }
	IVS_DEVICE_ALARM_NOTIFY* pDevAlarmNotifyTep = reinterpret_cast<IVS_DEVICE_ALARM_NOTIFY*>(pData);//lint !e826
//     if (NULL == pDevAlarmNotifyTep)
//     {
//         BP_RUN_LOG_ERR(IVS_SDK_RET_INTRINSIC_PTR_ERROR, "pDevAlarmNotifyTep is null", "NA");
//         IVS_DELETE(pData, MUILI);
//         return IVS_FAIL;
//     }
	// 网络字节序到本地字节的转换
	BP_ntohll(pDevAlarmNotifyTep->ullAlarmEventID, pDevAlarmNotifyTep->ullAlarmEventID);
	pDevAlarmNotifyTep->uiAlarmInType = ntohl(pDevAlarmNotifyTep->uiAlarmInType);
	pDevAlarmNotifyTep->uiAlarmLevelValue = ntohl(pDevAlarmNotifyTep->uiAlarmLevelValue);
	pDevAlarmNotifyTep->uiOccurNumber = ntohl(pDevAlarmNotifyTep->uiOccurNumber);
	
	// 拷贝固定长数据
	IVS_UINT32 uiSTSize = (sizeof(IVS_DEVICE_ALARM_NOTIFY) - IVS_ALARM_DESCRIPTION_LEN) - IVS_ALARM_EX_PARAM_LEN;
	if (!CToolsHelp::Memcpy((IVS_CHAR*)pDevAlarmNotify, sizeof(IVS_DEVICE_ALARM_NOTIFY), pDevAlarmNotifyTep, uiSTSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pDevAlarmNotifyTep to pDevAlarmNotify fail", "NA");
		IVS_DELETE(pData, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}

	/************************************************************************/
	/*   处理两个TLV结构                                                                                                             */
	/************************************************************************/                                                  
	// 第一个TLV
	TNssTLVMsg tlvMsg;
	CNSSOperator::instance().ParseData2TLV((void*)(pData + uiSTSize), tlvMsg);
	if (ALARMDESC_TAG != tlvMsg.usiTag)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "TLV tag error", "NA");
		IVS_DELETE(pData, MUILI);
		return IVS_FAIL;
	}
	if (IVS_ALARM_DESCRIPTION_LEN < tlvMsg.uiLength)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "TLV Length error", "NA");
		IVS_DELETE(pData, MUILI);
		return IVS_FAIL;
	}
	else if (0 == tlvMsg.uiLength)
	{
		BP_RUN_LOG_WAR("Process device alarm notify", "the length of the first TLV = 0");
	}
	else
	{
		if (NULL == tlvMsg.pszValue)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Process device alarm notify fail", "the value of the first TLV is NULL");
			IVS_DELETE(pData, MUILI);
			return IVS_PARA_INVALID;
		}
		if(CToolsHelp::Memcpy((IVS_CHAR*)(pDevAlarmNotify) + uiSTSize, IVS_ALARM_DESCRIPTION_LEN, tlvMsg.pszValue, tlvMsg.uiLength))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy tlvMsg.pszValue to pDevAlarmNotify fail", "NA");
			IVS_DELETE(pData, MUILI);
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
	
	uiSTSize += (sizeof(IVS_SHORT) + sizeof(IVS_INT32)) + tlvMsg.uiLength;
	
	// 第二个TLV
	eSDK_MEMSET(&tlvMsg, 0x0, sizeof(TNssTLVMsg));
	CNSSOperator::instance().ParseData2TLV((void*)(pData + uiSTSize), tlvMsg);

	if (EXTPARAM_TAG != tlvMsg.usiTag)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "TLV tag error", "NA");
		IVS_DELETE(pData, MUILI);
		return IVS_FAIL;
	}
	if (IVS_ALARM_EX_PARAM_LEN < tlvMsg.uiLength)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "TLV Length error", "NA");
		IVS_DELETE(pData, MUILI);
		return IVS_FAIL;
	}
	else if (0 == tlvMsg.uiLength)
	{
		BP_RUN_LOG_WAR("Process device alarm notify", "the length of the second TLV = 0");
	}
	else
	{
		if (NULL == tlvMsg.pszValue)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Process device alarm notify fail", "the value of the first TLV is NULL");
			IVS_DELETE(pData, MUILI);
			return IVS_PARA_INVALID;
		}
		uiSTSize = sizeof(IVS_DEVICE_ALARM_NOTIFY) - IVS_ALARM_EX_PARAM_LEN;
		if (!CToolsHelp::Memcpy((IVS_CHAR*)(pDevAlarmNotify) + uiSTSize, IVS_ALARM_EX_PARAM_LEN, tlvMsg.pszValue, tlvMsg.uiLength))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy tlvMsg.pszValue to pDevAlarmNotify fail", "NA");
			IVS_DELETE(pData, MUILI);
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
	
	IVS_DELETE(pData, MUILI);
	return IVS_SUCCEED;
}

IVS_INT32 CSDKMainService::ProcessManualRecordStateNotify(CCmd* pCmd, IVS_MANUAL_RECORD_STATUS_NOTIFY& stManualRecordStatusNotify)
{
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "cmd is null", "NA");
		return IVS_PARA_INVALID;
	}

	IVS_INT32 iRet = IVS_SUCCEED;
	//IVS_MANUAL_RECORD_STATUS_NOTIFY pManualRecordStatusNotify; 
	//eSDK_MEMSET(&pManualRecordStatusNotify, 0, sizeof(IVS_MANUAL_RECORD_STATUS_NOTIFY));

	IVS_CHAR* pDataXML = CNSSOperator::instance().ParseCmd2XML(pCmd, iRet);
	BP_RUN_LOG_INF("Parse Cmd2XML(pCmd) ReturnCode", "iRet = %d", iRet);
	if (NULL == pDataXML)
	{
		BP_RUN_LOG_INF("NULL == pDataXML", "NA");
		return IVS_PARA_INVALID;
	}
	CXml xmlData;
	(void)xmlData.Parse(pDataXML);

	//解析XML
	if (IVS_SUCCEED != CRecordXMLProcess::ParseManualRecordStateXML(xmlData,&stManualRecordStatusNotify))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Manual Record State failed","failed");
	    IVS_DELETE(pDataXML, MUILI);
		return IVS_FAIL;
	}	

	IVS_DELETE(pDataXML, MUILI);
	return IVS_SUCCEED;
}
