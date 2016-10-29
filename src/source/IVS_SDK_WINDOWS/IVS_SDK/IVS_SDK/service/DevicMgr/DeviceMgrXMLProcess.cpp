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
filename : DevicMgrXMLProcess.cpp
author : gwx151720
created : 2012/12/03
description : 
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/
#include "DeviceMgrXMLProcess.h"
#include "devEnum.h"
#include "ToolsHelp.h"
#include "XmlProcess.h"
#include "IVS_Trace.h"

IVS_INT32 CDeviceMgrXMLProcess::AddDeviceListGetXML(const IVS_CHAR* pDomainCode,  CMulDeviceAddInfoList& mulDeviceAddIndoList, 
    std::list<std::string>& deviceEncryptPwdList, std::list<std::string>& deviceRegEncryptPwdList, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("DevList");    

	std::list<IVS_DEVICE_OPER_INFO>::iterator itor_device = mulDeviceAddIndoList.deviceInfoList.begin();
	std::list<std::string>::iterator itor_pwd = deviceEncryptPwdList.begin();
	std::list<std::string>::iterator itor_regPwd = deviceRegEncryptPwdList.begin();

	IVS_INT32 iTemp = 0;
	const IVS_UINT32 uiElemMaxNum = 130;
	IVS_CHAR chTemp[uiElemMaxNum] = {0};
	for (; itor_device != mulDeviceAddIndoList.deviceInfoList.end() && itor_pwd != deviceEncryptPwdList.end()
		&& itor_regPwd != deviceRegEncryptPwdList.end(); itor_device++, itor_pwd++,  itor_regPwd++)
	{

		if (mulDeviceAddIndoList.deviceInfoList.begin() == itor_device)
		{
			(void)xml.AddChildElem("DevInfo");
			(void)xml.IntoElem();
		}
		else
		{
			(void)xml.AddElem("DevInfo");
		}

		(void)xml.AddChildElem("Sequence");          // 序号
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(itor_device->uiSequence)).c_str());//lint !e64 匹配

		(void)xml.AddElem("DevCode");          // 主设备编码
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevName");          // 主设备设备名称
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevProtocolType");          // 主设备协议类型
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevLoginType");          // 认证类型
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)itor_device->stDeviceCfg.iLoginType).c_str());//lint !e64 匹配

		(void)xml.AddElem("DevVendorType");          // 主设备提供商类型
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevFormType");      // 主设备形态类型
		(void)xml.SetElemValue(CDevEnum::GetDevType(static_cast<IVS_INT32>(itor_device->stDeviceCfg.stDeviceInfo.uiType)));

		(void)xml.AddElem("DevModelType");    // 主设备型号
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevAlarmEnable");   // 设备告警使能 
		(void)xml.SetElemValue(CToolsHelp::Int2Str(itor_device->stDeviceCfg.bEnableAlarm).c_str());//lint !e64 匹配

		if (0 != itor_device->stDeviceCfg.uiVideoFormat)     // 视频制式 
		{
			(void)xml.AddElem("VideoFormat");
			(void)xml.SetElemValue(CDevEnum::GetVideoFormatType(static_cast<IVS_INT32>(itor_device->stDeviceCfg.uiVideoFormat)));
		}

		(void)xml.AddElem("DirectConnectFirst");   //是否直连优先
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(itor_device->stDeviceCfg.uiDirectConnectFirst)).c_str());//lint !e64 匹配

		(void)xml.AddElem("MaxDirectConnectNum");  // 直连最大数目
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(itor_device->stDeviceCfg.uiMaxDirectConnectNum)).c_str());//lint !e64 匹配

		(void)xml.AddElem("NVRCode"); // 设备所属NVR编码
		(void)xml.SetElemValue(mulDeviceAddIndoList.m_strComplexCode.c_str());//lint !e64 匹配

		(void)xml.AddElem("DCGCode");    // 设备所属TAS编码
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.cTasCode, IVS_DEV_CODE_LEN);
		(void)xml.SetElemValue(chTemp);   

        //TAS所在IP地址
        (void)xml.AddElem("DCGIP");
        eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
        (void)CToolsHelp::Memcpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stTasIP.cIP, IVS_IP_LEN);
        (void)xml.SetElemValue(chTemp); 

        (void)xml.AddElem("DevIPType"); // IP类型
        iTemp = static_cast<IVS_INT32>(itor_device->stDeviceCfg.stDeviceInfo.stIP.uiIPType);
        (void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());//lint !e64 匹配

		(void)xml.AddElem("DevIP");   // 设备所在IP地址
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN);
		(void)xml.SetElemValue(chTemp); 

		(void)xml.AddElem("DevPort");  // 设备登录端口
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(itor_device->stDeviceCfg.stDeviceInfo.uiPort)).c_str());//lint !e64 匹配

		(void)xml.AddElem("DevSN");       //  设备SN号
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevUser");  // 设备登录用户名
		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		(void)CToolsHelp::Strncpy(chTemp, uiElemMaxNum, itor_device->stDeviceCfg.cDevUser, IVS_NAME_LEN);
		(void)xml.SetElemValue(chTemp);

		(void)xml.AddElem("DevPWD");  // 设备登录密码
		std::string strDevPWD = *itor_pwd;
		(void)xml.SetElemValue(strDevPWD.c_str()); //lint !e64 匹配

		(void)xml.AddElem("DevRegPWD");  // 设备向平台密码
		std::string strDevRegPWD = *itor_regPwd;
		(void)xml.SetElemValue(strDevRegPWD.c_str());//lint !e64 匹配

		(void)xml.AddElem("BlanceSupport");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(itor_device->stDeviceCfg.bEnableSchedule).c_str());//lint !e64 匹配

		xml.OutOfElem(); 
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 重定向XML组装
IVS_INT32 CDeviceMgrXMLProcess::GetRedirectServiceReqXML(std::list<IVS_DEVICE_REDIRECT_INFO>& deviceRedirectInfolist, CXml& xml)
{
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("CameraList");
    (void)xml.IntoElem();
    std::list<IVS_DEVICE_REDIRECT_INFO>::iterator itor = deviceRedirectInfolist.begin();
    IVS_INT32 iNum = 0;
    while (deviceRedirectInfolist.end() != itor)
    {
        iNum == 0 ? xml.AddChildElem("CameraCode"): xml.AddElem("CameraCode");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(itor->cCameraCode);
        xml.OutOfElem();
        itor++;
        iNum++;
    }
    xml.OutOfElem();
    return iNum;
}
// 重定向XML解析
IVS_INT32 CDeviceMgrXMLProcess::ParseRedirectServiceRspXML(CXml& xml, std::list<IVS_DEVICE_REDIRECT_INFO>& deviceRedirectInfoList)
{

    if (!xml.FindElemEx("Content/LocationList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse redirect service rsp xml", "fail to find LocationList node info, maybe this node no exist");
        return IVS_XML_INVALID;
    }

    if (!xml.FindElem("LocationInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse redirect service rsp xml", "fail to find LocationInfo node info, maybe this node no exist");
        return IVS_XML_INVALID;
    }

    const IVS_CHAR* szElemValue = NULL;
    IVS_DEVICE_REDIRECT_INFO deviceRedirectInfo;
    eSDK_MEMSET(&deviceRedirectInfo, 0, sizeof(deviceRedirectInfo));
    std::list<IVS_DEVICE_REDIRECT_INFO>::iterator itor = deviceRedirectInfoList.begin();
    do{
        if (!xml.FindElemValue("LocationInfo"))
        {
            break;
        }
        (void)xml.IntoElem();

        eSDK_MEMSET(&deviceRedirectInfo, 0, sizeof(deviceRedirectInfo));
        GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, deviceRedirectInfo.cCameraCode, IVS_DEV_CODE_LEN, xml);  // CameraCode

        GET_ELEM_VALUE_NUM("Result", szElemValue, deviceRedirectInfo.iResult, xml);  // Result

        GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, deviceRedirectInfo.cNVRCode, IVS_NVR_CODE_LEN, xml);  // NvrCode

        for (itor = deviceRedirectInfoList.begin(); deviceRedirectInfoList.end() != itor; itor++)
        {
            if (0 == strncmp(itor->cCameraCode, deviceRedirectInfo.cCameraCode, IVS_DEV_CODE_LEN))
            {
                *itor = deviceRedirectInfo;
                break;
            }
            continue;
        }
        xml.OutOfElem();
    }while (xml.NextElem());

    return IVS_SUCCEED;
}

// 增加设备组装xml;
IVS_INT32 CDeviceMgrXMLProcess::AddDeviceClusterGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_OPER_INFO* pDevList, std::vector<std::string> &devPWDList, std::vector<std::string> &devRegPWDList, CXml &xml)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");	
    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);

    (void)xml.AddElem("DevList");    

    IVS_UINT32 iNum = 0;
    // 用来获取xml中temp的元素值
    IVS_INT32 iTemp = 0;
    const IVS_UINT32 uiElemMaxNum = 130;
    IVS_CHAR chTemp[uiElemMaxNum];
    while(iNum < uiDeviceNum)
    {
        iNum == 0 ? xml.AddChildElem("DevInfo"): xml.AddElem("DevInfo");

        xml.IntoElem();
        //添加序号
        (void)xml.AddChildElem("Sequence");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevList[iNum].uiSequence)).c_str());//lint !e64 匹配

        //主设备编码
		(void)xml.AddElem("DevCode");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(chTemp);

        //主设备设备名称
        (void)xml.AddElem("DevName");
        eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xml.SetElemValue(chTemp);
  
        //主设备协议类型
        (void)xml.AddElem("DevProtocolType");
		IVS_CHAR cDevProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pDevList[iNum].stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pDevList[iNum].stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xml.SetElemValue(cDevProtocolType);

		//认证类型
		(void)xml.AddElem("DevLoginType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDevList[iNum].stDeviceCfg.iLoginType).c_str()); //lint !e1924 !e64 匹配

        //主设备提供商类型
		(void)xml.AddElem("DevVendorType");
		IVS_CHAR cDevVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDevList[iNum].stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDevList[iNum].stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cDevVendorType);

		//主设备形态类型    =======================================
		iTemp = static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.stDeviceInfo.uiType);
		(void)xml.AddElem("DevFormType");
		(void)xml.SetElemValue(CDevEnum::GetDevType(iTemp));

        //主设备型号
		(void)xml.AddElem("DevModelType");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(chTemp);
        
        //设备告警使能   =============================================
		(void)xml.AddElem("DevAlarmEnable");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pDevList[iNum].stDeviceCfg.bEnableAlarm).c_str());//lint !e64 匹配

        //视频制式  =======================================
		if (0 != pDevList[iNum].stDeviceCfg.uiVideoFormat)
		{
			iTemp = static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.uiVideoFormat);
			(void)xml.AddElem("VideoFormat");
			(void)xml.SetElemValue(CDevEnum::GetVideoFormatType(iTemp));
		}
		
        
        //是否直连优先
		iTemp = static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.uiDirectConnectFirst);
		(void)xml.AddElem("DirectConnectFirst");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());//lint !e64 匹配

        //直连最大数目
		iTemp = static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.uiMaxDirectConnectNum);
		(void)xml.AddElem("MaxDirectConnectNum");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());//lint !e64 匹配
		
        //设备所属NVR编码
		(void)xml.AddElem("NVRCode");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cNvrCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cNvrCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(chTemp);
     
        //设备所属TAS编码
		(void)xml.AddElem("DCGCode");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cTasCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cTasCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		(void)xml.SetElemValue(chTemp);   

        //IP类型
		iTemp = static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.stDeviceInfo.stIP.uiIPType);
		(void)xml.AddElem("DevIPType");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());//lint !e64 匹配

        //设备所在IP地址
		(void)xml.AddElem("DevIP");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(chTemp); 

		//设备登录端口
		(void)xml.AddElem("DevPort");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevList[iNum].stDeviceCfg.stDeviceInfo.uiPort)).c_str());//lint !e64 匹配
		
		//设备SN号
		(void)xml.AddElem("DevSN");
		IVS_CHAR cDevSN[IVS_DEV_SERIALNO_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, pDevList[iNum].stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, pDevList[iNum].stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cDevSN);

        //设备登录用户名
		(void)xml.AddElem("DevUser");
		eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
		if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cDevUser, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[iNum].stDeviceCfg.cDevUser, IVS_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(chTemp);
  
        //设备登录密码
		(void)xml.AddElem("DevPWD");
		std::string strDevPWD = devPWDList.at(iNum);
		(void)xml.SetElemValue(strDevPWD.c_str());//lint !e64 匹配

        //设备向平台密码
		(void)xml.AddElem("DevRegPWD");
		std::string strDevRegPWD = devRegPWDList.at(iNum);
		(void)xml.SetElemValue(strDevRegPWD.c_str());//lint !e64 匹配

		(void)xml.AddElem("BlanceSupport");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pDevList[iNum].stDeviceCfg.bEnableSchedule).c_str());//lint !e64 匹配
        
        xml.OutOfElem();

        iNum++;      
    }

    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}

//增加设备解析xml
IVS_INT32 CDeviceMgrXMLProcess::AddDeviceClusterPraseXML( CXml& Rspxml, IVS_DEVICE_OPER_RESULT_LIST* pResultList, const IVS_INT32 &iReqNum )
{
    CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

    if(!Rspxml.FindElemEx("Content/DevList"))
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Add DeviceInfo PraseXML fail", "FALSE == xml.FindElemEx('Content/Dev_List')");
        return IVS_OPERATE_MEMORY_ERROR;
    }       

    if (!Rspxml.FindElem("DevInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Device ParseXml fail", "!Rspxml.FindElem(DevInfo)");
        return IVS_XML_INVALID;
    }

    /*IVS_DEVICE_CODE_INFO stDevInfo ;*/
    const IVS_CHAR* szElemValue = NULL;
    IVS_INT32 i = 0;
    IVS_INT32 iTemp = 0;

    do{
        if (!Rspxml.FindElemValue("DevInfo"))
        {
            break;
        }

        (void)Rspxml.IntoElem();

        IVS_DEVICE_OPER_RESULT &stDevInfo = pResultList->stCodeInfo[i];
        eSDK_MEMSET(&stDevInfo, 0 ,sizeof(IVS_DEVICE_OPER_RESULT));           		
        iTemp = 0;
        // Sequence
        GET_ELEM_VALUE_NUM("Sequence", szElemValue, iTemp, Rspxml);//lint !e838

        stDevInfo.uiSequence = static_cast<IVS_UINT32>(iTemp);
        // DevCode
        GET_ELEM_VALUE_CHAR("DevCode", szElemValue, stDevInfo.cCode, IVS_DEV_CODE_LEN, Rspxml);

        // NvrCode
        GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, stDevInfo.cNvrCode, IVS_NVR_CODE_LEN, Rspxml);
		
        iTemp = 0;
        // Result
        GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, Rspxml);//lint !e838
        stDevInfo.uiResult = static_cast<IVS_UINT32>(iTemp);     

        Rspxml.OutOfElem();
        i++;
    }while (Rspxml.NextElem() && i < iReqNum);

    return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgrXMLProcess::AddDeviceListParseRspXML(CXml& xml, std::list<IVS_DEVICE_OPER_RESULT>& operResultList)
{

	IVS_DEBUG_TRACE("");

	if(!xml.FindElemEx("Content/DevList"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Add DeviceInfo PraseXML fail", "FALSE == xml.FindElemEx('Content/Dev_List')");
		return IVS_OPERATE_MEMORY_ERROR;
	}       

	if (!xml.FindElem("DevInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Device ParseXml fail", "!Rspxml.FindElem(DevInfo)");
		return IVS_XML_INVALID;
	}

	const IVS_CHAR* szElemValue = NULL;
    IVS_INT32 iTemp = 0;
	IVS_DEVICE_OPER_RESULT stDevInfo;
	eSDK_MEMSET(&stDevInfo, 0, sizeof(stDevInfo));
	do{
		if (!xml.FindElemValue("DevInfo"))
		{
			break;
		}

		(void)xml.IntoElem();

		eSDK_MEMSET(&stDevInfo, 0 ,sizeof(IVS_DEVICE_OPER_RESULT));           		
        iTemp = 0;
		// Sequence
		GET_ELEM_VALUE_NUM("Sequence", szElemValue, iTemp, xml);//lint !e838

		stDevInfo.uiSequence = static_cast<IVS_UINT32>(iTemp);
		// DevCode
		GET_ELEM_VALUE_CHAR("DevCode", szElemValue, stDevInfo.cCode, IVS_DEV_CODE_LEN, xml);

        // NvrCode
        GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, stDevInfo.cNvrCode, IVS_NVR_CODE_LEN, xml);

        iTemp = 0;
        // Result
        GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, xml);//lint !e838
        stDevInfo.uiResult = static_cast<IVS_UINT32>(iTemp);     

		operResultList.push_back(stDevInfo);
		xml.OutOfElem();
	}while (xml.NextElem());

	return IVS_SUCCEED;
}

// 获取主设备列表解析XML
IVS_INT32 CDeviceMgrXMLProcess::GetMainDeviceListParseXML( IVS_DEVICE_BRIEF_INFO_LIST* pDevInfoList,  IVS_UINT32 uiBufferSize, CXml &Rspxml )
{
    CHECK_POINTER(pDevInfoList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_UINT32 iReqNum = (uiBufferSize-sizeof(IVS_DEVICE_BRIEF_INFO_LIST))/sizeof(IVS_DEVICE_BRIEF_INFO) + 1;

    // 没有找到相关的节点
    if (!Rspxml.FindElemEx("Content/PageInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Main DeviceList ParseXML fail", "Fail", "FALSE == Rspxml.FindElem(Content/PageInfo)");
        return IVS_XML_INVALID;
    }  

    const IVS_CHAR *pTemp = NULL;
    IVS_INT32 iFromTemp = 0;
    IVS_INT32 iToTemp = 0;
    IVS_INT32 iTotal = 0;

    GET_ELEM_VALUE_NUM("RealNum", pTemp, iTotal, Rspxml);//lint !e838
    GET_ELEM_VALUE_NUM("ToIndex", pTemp,  iToTemp, Rspxml);//lint !e838
    GET_ELEM_VALUE_NUM("FromIndex", pTemp, iFromTemp, Rspxml);//lint !e838
    // 判断是否正确 如果RealNum返回的是0，那么就直接返回
    if (0 == iTotal)
    {
        pDevInfoList->uiTotal = static_cast<IVS_UINT32>(iTotal);    
        pDevInfoList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);    
        pDevInfoList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToTemp);
        return IVS_SUCCEED;
    }
    else
    {
        if (iTotal < 0 || iToTemp < 0 || iFromTemp < 0 || iFromTemp > iToTemp)
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Main DeviceList ParseXML fail", "pDevInfoList->uiTotal=%d,pDevInfoList->stIndexRange.uiFromIndex=%d,pDevInfoList->stIndexRange.uiToIndex=%d",iTotal, iFromTemp, iToTemp);
            return IVS_XML_INVALID;
        }
    }

    pDevInfoList->uiTotal = static_cast<IVS_UINT32>(iTotal);    
    pDevInfoList->stIndexRange.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);    
    pDevInfoList->stIndexRange.uiToIndex = static_cast<IVS_UINT32>(iToTemp);

    IVS_UINT32 iRspNum = (pDevInfoList->stIndexRange.uiToIndex - pDevInfoList->stIndexRange.uiFromIndex) + 1;

    //判断返回的数量大于请求的   
    if (iRspNum > iReqNum)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Main DeviceList ParseXML fail", "iRspNum=%d,iReqNum=%d", iRspNum, iReqNum);
        return IVS_XML_INVALID;
    }
    else
    {
        iReqNum = iRspNum;
    }

    Rspxml.OutOfElem();

    if (!Rspxml.FindElem("DevList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Main DeviceList ParseXML fail", "no DevList element in rsPxml");
        return IVS_XML_INVALID;
    }
    Rspxml.IntoElem(); 

    IVS_UINT32 iIndex = 0;
    IVS_CHAR chTemp[128];
    do 
    {
        if (!Rspxml.FindElemValue("DevInfo"))
        {
            break;
        }
        Rspxml.IntoElem();

        IVS_DEVICE_BRIEF_INFO &stDevInfo = pDevInfoList->stDeviceBriefInfo[iIndex];
		SecureZeroMemory(&stDevInfo, sizeof(IVS_DEVICE_BRIEF_INFO));
        // cDomainCode
        GET_ELEM_VALUE_CHAR("DomainCode", pTemp, stDevInfo.stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, Rspxml);
        // 主设备编码
        GET_ELEM_VALUE_CHAR("DevCode", pTemp, stDevInfo.stDeviceInfo.cCode, IVS_DEV_CODE_LEN, Rspxml);
        // 主设备设备名称
        GET_ELEM_VALUE_CHAR("DevName", pTemp, stDevInfo.stDeviceInfo.cName , IVS_DEVICE_NAME_LEN, Rspxml);        
        // DevProtocolType
		GET_ELEM_VALUE_CHAR("DevProtocolType", pTemp, stDevInfo.cProtocolType , IVS_MAX_PROTOCOLTYPE_NAME_LEN, Rspxml);  
        // DevLoginType
        GET_ELEM_VALUE_NUM_FOR_UINT("DevLoginType", pTemp, stDevInfo.iLoginType , Rspxml); 
        // 主设备提供商类型    
		GET_ELEM_VALUE_CHAR("DevVendorType", pTemp, stDevInfo.stDeviceInfo.cVendorType , IVS_MAX_VENDOR_TYPE_NAME_LEN, Rspxml);  
	
        // 主设备形态类型 =============================
        eSDK_MEMSET(chTemp, 0, 128);
        GET_ELEM_VALUE_CHAR("DevFormType", pTemp, chTemp, 128, Rspxml);    

        stDevInfo.stDeviceInfo.uiType = static_cast<IVS_UINT32>(CDevEnum::GetDevTypeEnum(chTemp));
        // 主设备型号
        GET_ELEM_VALUE_CHAR("DevModelType", pTemp,  stDevInfo.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN, Rspxml); 
		// 设备SN号(移动模拟前端为IMEI号)
		GET_ELEM_VALUE_CHAR("DevSN", pTemp, stDevInfo.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, Rspxml);
        // IP类型
        GET_ELEM_VALUE_NUM_FOR_UINT("DevIPType", pTemp, stDevInfo.stDeviceInfo.stIP.uiIPType, Rspxml); 
        // 设备所在IP地址
        GET_ELEM_VALUE_CHAR("DevIP", pTemp,  stDevInfo.stDeviceInfo.stIP.cIP, IVS_IP_LEN, Rspxml); 
        // 是否在线  ==============================
        eSDK_MEMSET(chTemp, 0, 128);
        GET_ELEM_VALUE_CHAR("IsOnline", pTemp, chTemp, 128,Rspxml);  
        stDevInfo.uiStatus = static_cast<IVS_UINT32>(CDevEnum::GetDevStatusEnum(chTemp));
        // 是否支持集群调度 
        GET_ELEM_VALUE_NUM("BlanceSupport", pTemp, stDevInfo.bEnableSchedule, Rspxml);

		/*
		// 接入网关IP
		//GET_ELEM_VALUE_CHAR("DCGIP", pTemp,  stDevInfo.stDeviceInfo.cDCGIP, IVS_IP_LEN, Rspxml); 

		//影子IPC相关信息
		eSDK_MEMSET(chTemp,0,128);
		GET_ELEM_VALUE_CHAR("IsShadowDev",pTemp,chTemp,128,Rspxml);
		chTemp[128-1] = 0;
		// 是否影子IPC	0-否 1-是
		if (0 == strcmp(chTemp,"1"))
		{
			stDevInfo.stDeviceInfo.bIsShadowDev = 1;
		}
		else
		{
			stDevInfo.stDeviceInfo.bIsShadowDev = 0;
		}

		GET_ELEM_VALUE_CHAR("OrigDevCode",pTemp,stDevInfo.stDeviceInfo.cOrigDevCode,IVS_DEV_CODE_LEN,Rspxml); 
		
		GET_ELEM_VALUE_CHAR("OriDevDomainCode", pTemp, stDevInfo.stDeviceInfo.cOriDevDomainCode, IVS_DOMAIN_CODE_LEN, Rspxml);   
		*/
        Rspxml.OutOfElem();

        iIndex++;
    } while (Rspxml.NextElem() && iIndex < iReqNum);

    return IVS_SUCCEED;
}

// 删除设备拼接XML
IVS_INT32 CDeviceMgrXMLProcess::DeleteDeviceGetXML(const IVS_UINT32& uiDeviceNum,const IVS_DEVICE_CODE* pDevCodeList,CXml& xmlReq, const IVS_CHAR*  pDomainCode)
{
    CHECK_POINTER(pDevCodeList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    (void)xmlReq.IntoElem();
    
    (void)xmlReq.AddChildElem("DomainCode");
	(void)xmlReq.IntoElem();
    (void)xmlReq.SetElemValue(pDomainCode);
    (void)xmlReq.AddElem("DevList");
    (void)xmlReq.IntoElem();

    const IVS_UINT32 MAXELEMLEN =  IVS_DEV_CODE_LEN + 1;
    IVS_CHAR chTemp[MAXELEMLEN];
    for(IVS_UINT32 uiIndex = 0 ; uiIndex < uiDeviceNum; uiIndex++)
    { 
        if (0 == uiIndex)
        {
            (void)xmlReq.AddChildElem("DevCode");
        }
        else
        {
            (void)xmlReq.AddElem("DevCode");
        }
		(void)xmlReq.IntoElem();
        eSDK_MEMSET(chTemp, 0, MAXELEMLEN);
		if (!CToolsHelp::Strncpy(chTemp, MAXELEMLEN, pDevCodeList[uiIndex].cDevCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Strncpy(chTemp, MAXELEMLEN, pDevCodeList[uiIndex].cDevCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xmlReq.SetElemValue(chTemp);      
    }

    xmlReq.OutOfElem();
	xmlReq.GetRootPos();

    return IVS_SUCCEED;
}

// 删除设备解析XML
IVS_INT32 CDeviceMgrXMLProcess::DeleteDeviceParseXML(IVS_RESULT_LIST* pResultList, IVS_UINT32 uiResultBufferSize, CXml& xmlRsp)
{
    CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if(!xmlRsp.FindElemEx("Content/DevList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add DeviceInfo PraseXML fail", "FALSE == xml.FindElemEx('Content/Dev_List')");
        return IVS_XML_INVALID;
    }      

    if (!xmlRsp.FindElem("DevInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Device ParseXml fail", "FALSE == xml.FindElem(DevInfo)");
        return IVS_XML_INVALID;
    }

    // 总共的数量
    IVS_UINT32 uiReqNum =( (uiResultBufferSize - sizeof(IVS_RESULT_LIST))/sizeof(IVS_RESULT_INFO) ) + 1;

    const IVS_CHAR* szElemValue = NULL;
    IVS_UINT32 uiIndex = 0;

    do{
        if (!xmlRsp.FindElemValue("DevInfo"))
        {
            break;
        }

        IVS_RESULT_INFO &stResultInfo = pResultList->stInfo[uiIndex];
		SecureZeroMemory(&stResultInfo, sizeof(IVS_RESULT_INFO));
       
        (void)xmlRsp.IntoElem();
        GET_ELEM_VALUE_CHAR("DevCode", szElemValue, stResultInfo.cCode, IVS_DEV_CODE_LEN, xmlRsp);
        GET_ELEM_VALUE_NUM("Result", szElemValue, stResultInfo.iResult, xmlRsp);       
        xmlRsp.OutOfElem();

        uiIndex++;
    }while (xmlRsp.NextElem() && uiIndex < uiReqNum);

    return IVS_SUCCEED;
}

// 构造查询设备组列表请求xml
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceGroupXML(IVS_INT32 iType, 
    const IVS_CHAR* pDomainCode,
    const IVS_CHAR* pDevGroupCode,
    CXml& xml)
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
    (void)xml.SetElemValue(cTempDomainCode);
    (void)xml.AddElem("GroupID");
	IVS_CHAR cGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, pDevGroupCode, IVS_DEVICE_GROUP_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, pDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	IVS_CHAR szDevGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(void)CXmlProcess::ParseDevCode(cGroupID, szDevGroupCode, szDomainCode);

    (void)xml.SetElemValue(szDevGroupCode);
	(void)xml.AddElem("IsExDomain");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iType).c_str());//lint !e64 匹配
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 解析查询设备组列表响应xml
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceGroupParseXML(IVS_UINT32 &uiSize, IVS_DEVICE_GROUP_LIST* pDeviceGroupList, const IVS_UINT32 uiBufferSize, CXml& xml)
{
    CHECK_POINTER(pDeviceGroupList,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    const IVS_CHAR* szElemValue = NULL;

    if (!xml.FindElemEx("Content/GroupList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get DeviceGroup ParseXML fail","xml.FindElem(Content/GroupList)");
        return IVS_XML_INVALID;
    }   

	// FindElem会导致死循环
    while(xml.FindElemValue("GroupInfo"))
    {
        if ((sizeof(IVS_DEVICE_GROUP_LIST) + sizeof(IVS_DEVICE_GROUP)*uiSize) > uiBufferSize)
        {
			// 判断内存是否溢出
            BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Device GroupXML fail","out of memory");
            return IVS_OPERATE_MEMORY_ERROR;
        }

        (void)xml.IntoElem();
        IVS_DEVICE_GROUP stDevGroup;
        eSDK_MEMSET(&stDevGroup, 0, sizeof(IVS_DEVICE_GROUP));
        GET_ELEM_VALUE_CHAR("GroupID", szElemValue, stDevGroup.cGroupCode, IVS_DEVICE_GROUP_LEN, xml);

		GET_ELEM_VALUE_CHAR("ParentGroupID", szElemValue, stDevGroup.cParentGroupCode, IVS_DEVICE_GROUP_LEN, xml);

        GET_ELEM_VALUE_CHAR("GroupName", szElemValue, stDevGroup.cGroupName, IVS_NAME_LEN, xml);
		GET_ELEM_VALUE_NUM("IsExDomain", szElemValue, stDevGroup.bIsExDomain, xml);

        if (!CToolsHelp::Memcpy(&(pDeviceGroupList->stDeviceGroup[uiSize]), sizeof(IVS_DEVICE_GROUP), &stDevGroup, sizeof(IVS_DEVICE_GROUP)))
        {
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(&(pDeviceGroupList->stDeviceGroup[uiSize]), sizeof(IVS_DEV_GROUP), &stDevGroup, sizeof(IVS_DEV_GROUP) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
        }
        
        ++uiSize;

        xml.OutOfElem();
        if(!xml.NextElem())
        {
            break;
        }
    }
    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}


// 构造删除设备组请求xml
IVS_INT32  CDeviceMgrXMLProcess::DeleteDeviceGroupGetXML(
                                                         const IVS_CHAR* pDomainCode,
                                                         IVS_UINT32 uiDevGroupID,
                                                         CXml& xmlReq)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xmlReq.AddDeclaration("1.0","UTF-8","");
    (void)xmlReq.AddElem("Content");
    
    (void)xmlReq.AddChildElem("GroupInfo");
	(void)xmlReq.IntoElem();
    (void)xmlReq.AddChildElem("DomainCode");
    (void)xmlReq.IntoElem();
	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xmlReq.SetElemValue(cTempDomainCode);
    (void)xmlReq.AddElem("GroupID");
    (void)xmlReq.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiDevGroupID)).c_str());//lint !e64 匹配
    xmlReq.OutOfElem();
    xmlReq.OutOfElem();

    return IVS_SUCCEED;
}


// 获取设备详细信息解析XML
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceInfoParseXML(IVS_DEVICE_CFG* pDeviceInfo, CXml& xml)
{
    CHECK_POINTER(pDeviceInfo,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if (!xml.FindElemEx("Content/DevInfo"))
    {
        return IVS_XML_INVALID;
    }
	const IVS_INT32 maxLen = 120;
    IVS_CHAR chTemp[maxLen];
    const char* szElemValue = NULL;
    IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_CHAR("DevCode", szElemValue, pDeviceInfo->stDeviceInfo.cCode, IVS_DEV_CODE_LEN, xml);    //lint !e838
	GET_ELEM_VALUE_CHAR("DevName", szElemValue, pDeviceInfo->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN, xml);
	eSDK_MEMSET(chTemp, 0, maxLen);
	GET_ELEM_VALUE_CHAR("DevProtocolType", szElemValue, pDeviceInfo->cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, xml);
	//pDeviceInfo->uiProtocolType = static_cast<IVS_UINT32>(CDevEnum::GetDevProtocolTypeEnum(chTemp));
    iTemp = 0;                                                   //lint !e838
    GET_ELEM_VALUE_NUM("DevLoginType", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->iLoginType = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;
	eSDK_MEMSET(chTemp, 0, maxLen);
	GET_ELEM_VALUE_CHAR("DevVendorType", szElemValue, pDeviceInfo->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, xml);
	//pDeviceInfo->stDeviceInfo.uiVendorType = static_cast<IVS_UINT32>(CDevEnum::GetDevVendorTypeEnum(chTemp));
    eSDK_MEMSET(chTemp, 0, maxLen);
    GET_ELEM_VALUE_CHAR("DevFormType", szElemValue, chTemp, maxLen, xml);
    pDeviceInfo->stDeviceInfo.uiType = CDevEnum::GetDevTypeEnum(chTemp);
    GET_ELEM_VALUE_CHAR("DevModelType", szElemValue, pDeviceInfo->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN, xml);
   
    eSDK_MEMSET(chTemp, 0, maxLen);

	GET_ELEM_VALUE_NUM("DevAlarmEnable", szElemValue, iTemp, xml);//lint !e838
	pDeviceInfo->bEnableAlarm = iTemp;
	iTemp = 0;

    GET_ELEM_VALUE_CHAR("VideoFormat", szElemValue, chTemp, maxLen, xml);
    pDeviceInfo->uiVideoFormat = (IVS_UINT32)CDevEnum::GetVideoFormatEnum(chTemp);

    GET_ELEM_VALUE_NUM("DirectConnectFirst", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->uiDirectConnectFirst = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;

    GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, pDeviceInfo->cNvrCode, IVS_NVR_CODE_LEN, xml);
    GET_ELEM_VALUE_CHAR("DCGCode", szElemValue, pDeviceInfo->cTasCode, IVS_TAS_CODE_LEN, xml);

    GET_ELEM_VALUE_CHAR("DevUser", szElemValue, pDeviceInfo->cDevUser, IVS_NAME_LEN, xml);
    GET_ELEM_VALUE_CHAR("DevPWD", szElemValue, pDeviceInfo->cDevPasswd, IVS_PWD_LEN, xml);
    GET_ELEM_VALUE_CHAR("DevRegPWD", szElemValue, pDeviceInfo->cDevRegPasswd, IVS_PWD_LEN, xml);

    GET_ELEM_VALUE_NUM("BlanceSupport", szElemValue, pDeviceInfo->bEnableSchedule, xml);
    GET_ELEM_VALUE_NUM("MaxVideoNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxVideoNum = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;
    GET_ELEM_VALUE_NUM("MaxAudioInNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxAudioInNum = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;
    GET_ELEM_VALUE_NUM("MaxAudioOutNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxAudioOutNum = static_cast<IVS_UINT32>(iTemp);

    GET_ELEM_VALUE_NUM("MaxAlarmInNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxAlarmInNum = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;
    GET_ELEM_VALUE_NUM("MaxAlarmOutNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxAlarmOutNum = static_cast<IVS_UINT32>(iTemp);
    iTemp = 0;
    GET_ELEM_VALUE_NUM("MaxSerialNum", szElemValue, iTemp, xml);//lint !e838
    pDeviceInfo->stDeviceChannel.uiMaxSerialNum = static_cast<IVS_UINT32>(iTemp);
    //iTemp = 0;

    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 查询设备详细信息拼接XML
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceInfoXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pDevCode, CXml& xml)
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);
    (void)xml.AddElem("DevCode");
    (void)xml.SetElemValue(pDevCode);
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 修改设备组(名称)拼接XML
IVS_INT32 CDeviceMgrXMLProcess::ModifyDeviceGroupGetXML( 
                                                        const IVS_CHAR* pDomainCode,
                                                        IVS_UINT32 uiDevGroupID,
                                                        const IVS_CHAR* pNewDevGroupName , 
                                                        CXml &xml)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pNewDevGroupName, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("GroupInfo");
	(void)xml.IntoElem();
    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.SetElemValue(cTempDomainCode);  
    (void)xml.AddElem("GroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)uiDevGroupID)).c_str());//lint !e64 匹配
	char cTempGroupName[IVS_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempGroupName, IVS_NAME_LEN, pNewDevGroupName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempGroupName, IVS_NAME_LEN, pNewDevGroupName, IVS_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.AddElem("GroupName");
    (void)xml.SetElemValue(cTempGroupName);
    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}


// 新增设备组拼接XML
IVS_INT32 CDeviceMgrXMLProcess::MoveDeviceGroupGetXML( 
                                                      const IVS_CHAR* pDomainCode,
                                                      IVS_UINT32 uiNewParentDevGroupID,
                                                      IVS_UINT32 uiDevGroupID, 
                                                      CXml& xml)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    
    (void)xml.AddChildElem("GroupInfo");
	(void)xml.IntoElem();
    (void)xml.AddChildElem("DomainCode");
    (void)xml.IntoElem();
	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.SetElemValue(cTempDomainCode);
    (void)xml.AddElem("GroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)uiDevGroupID)).c_str());//lint !e64 匹配
    (void)xml.AddElem("TargetGroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)uiNewParentDevGroupID)).c_str());//lint !e64 匹配

    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}


// 新增设备组拼接XML
IVS_INT32 CDeviceMgrXMLProcess::AddDeviceGroupGetXML(const IVS_DEV_GROUP* pDevGroupInfo,CXml &xml)
{
    CHECK_POINTER(pDevGroupInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    
    (void)xml.AddChildElem("GroupInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("GroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDevGroupInfo->uiID).c_str());//lint !e64 匹配
    (void)xml.AddElem("ParentGroupID");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDevGroupInfo->uiParentID).c_str());//lint !e64 匹配

	char cTempGroupName[IVS_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempGroupName, IVS_NAME_LEN + 1, pDevGroupInfo->cName, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempGroupName, IVS_NAME_LEN + 1, pDevGroupInfo->cName, IVS_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.AddElem("GroupName");
	(void)xml.SetElemValue(cTempGroupName);

	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDevGroupInfo->cDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDevGroupInfo->cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(cTempDomainCode);
    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}


// 新增设备组解析XML
IVS_INT32 CDeviceMgrXMLProcess::AddDeviceGroupParseXML(CXml &xml, IVS_UINT32* uiDevGroupID)
{
    CHECK_POINTER(uiDevGroupID, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    const IVS_CHAR* szElemValue = NULL;

    IVS_UINT32 iTemp = 0;

    if(!xml.FindElemEx("Content/GroupID"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add DeviceGroup ParseXML fail", "!xml.FindElemEx(Content/GroupID)");
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", szElemValue, iTemp, xml);//lint !e838
    *uiDevGroupID = iTemp;

    return IVS_SUCCEED;
}


// 删除设备组的摄像机拼接XML
IVS_INT32 CDeviceMgrXMLProcess::DeleteDeviceFromGroupGetXML(
	const IVS_CHAR* pTargetDomainCode,
	IVS_UINT32 uiDeviceNum,
	const IVS_DEV_GROUP_CAMERA* pDeviceGroupList,
	CXml& xml)
{
	CHECK_POINTER(pTargetDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDeviceGroupList, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("DomainCode");    
	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pTargetDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.IntoElem();
	(void)xml.SetElemValue(cTempDomainCode);   

	(void)xml.AddElem("DevList");
	for (IVS_UINT32 uiIndex = 0; uiIndex <uiDeviceNum; uiIndex++)
	{
		if(uiIndex > 0)
		{
			(void)xml.AddElem("DevInfo");
		}
		else
		{
			(void)xml.AddChildElem("DevInfo");
			(void)xml.IntoElem();
		}
		// 设备编码
		(void)xml.AddChildElem("DevCode");
		(void)xml.IntoElem();
		IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		(void)CXmlProcess::ParseDevCode(pDeviceGroupList[uiIndex].cCameraCode, szCameraCode, szDomainCode);
		(void)xml.SetElemValue(szCameraCode);

		// 设备所属域编码
		(void)xml.AddElem("DevDomainCode");
		char cTempDomain[IVS_DOMAIN_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cTempDomain, IVS_DOMAIN_CODE_LEN + 1, (pDeviceGroupList + uiIndex)->cCameraDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomain, IVS_DOMAIN_CODE_LEN + 1, (pDeviceGroupList + uiIndex)->cCameraDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTempDomain);

		//设备组ID
		(void)xml.AddElem("FromGroupID");
		(void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)(pDeviceGroupList + uiIndex)->uiDevGroupID)).c_str());//lint !e64 匹配
		xml.OutOfElem();   
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}
// 获取网元列表参数拼接XML
IVS_INT32 CDeviceMgrXMLProcess::GetNVRListReqXML(const IVS_CHAR* pDomainCode,const IVS_INDEX_RANGE* pIndex,IVS_UINT32 uiServerType,CXml &xml)
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndex ,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("enter GetNVRListReqXML pDomainCode: %s,addr:%d",pDomainCode, &pDomainCode);

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");	

    (void)xml.AddChildElem("PageInfo");
	(void)xml.IntoElem();
    (void)xml.AddChildElem("FromIndex");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndex->uiFromIndex)).c_str());//lint !e64 匹配
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndex->uiToIndex)).c_str());//lint !e64 匹配
    xml.OutOfElem();

	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("NVRType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiServerType)).c_str());//lint !e64 匹配
	xml.OutOfElem();
    BP_DBG_LOG("leave GetNVRListReqXML pDomainCode:%s, addr:%d", pDomainCode,&pDomainCode);
    return IVS_SUCCEED;
}

// 获取网元列表解析XML
IVS_INT32 CDeviceMgrXMLProcess::ParseNVRListRsp(CXml& xml,IVS_DEVICE_BRIEF_INFO_LIST* pNvrList, IVS_UINT32 uiBufferSize)
{
    CHECK_POINTER(pNvrList,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_FAIL;

    if (!xml.FindElemEx("Content/PageInfo"))
    {
        iRet = IVS_XML_INVALID;
        BP_RUN_LOG_ERR(iRet, "Parse NVRListRsp fail", "!xml.FindElemEx(Content/PageInfo)");
        return iRet;
    }   

    (void)xml.IntoElem();
    const char* pTemp = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", pTemp, pNvrList->stIndexRange.uiFromIndex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", pTemp, pNvrList->stIndexRange.uiToIndex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",pTemp, pNvrList->uiTotal, xml);
    xml.OutOfElem();

    if (0 == pNvrList->uiTotal)
    {
        return IVS_SUCCEED;
    }

    if (!xml.FindElemEx("Content/NVRList"))
    {
        iRet = IVS_XML_INVALID;
        BP_RUN_LOG_ERR(iRet, "Parse NVRListRsp fail", "!xml.FindElemEx(Content/NVRList)");
        return iRet;
    }

    IVS_INT32 iRealNum = 0;
    if (xml.FindElemEx("Content/NVRList/NVRInfo"))
    {
        do
        {
            const char* szElemValue = NULL;

            IVS_DEVICE_BRIEF_INFO stDeiviceBriefInfo;
            eSDK_MEMSET(&stDeiviceBriefInfo, 0, sizeof(IVS_DEVICE_BRIEF_INFO));

            (void)xml.IntoElem();
            GET_ELEM_VALUE_CHAR("DomainCode",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.cDomainCode,IVS_DOMAIN_CODE_LEN,xml);//lint !e838
            GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.cCode,IVS_DEV_CODE_LEN,xml);
            GET_ELEM_VALUE_CHAR("NVRName",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.cName,IVS_DEVICE_NAME_LEN,xml);
            GET_ELEM_VALUE_NUM_FOR_UINT("NVRType",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.uiType,xml);
            GET_ELEM_VALUE_CHAR("NVRModel",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.cModel,sizeof(stDeiviceBriefInfo.stDeviceInfo.cModel),xml);
            //GET_ELEM_VALUE_NUM_FOR_UINT("NVRVendor",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.uiVendorType,xml);
			GET_ELEM_VALUE_CHAR("NVRVendor",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.cVendorType,IVS_MAX_VENDOR_TYPE_NAME_LEN,xml);
            GET_ELEM_VALUE_CHAR("NVRIp",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.stIP.cIP,IVS_IP_LEN,xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("NVRPort",szElemValue,  stDeiviceBriefInfo.stDeviceInfo.uiPort,xml);
            char cStatus[IVS_DEV_CODE_LEN];
            eSDK_MEMSET(cStatus,0,IVS_DEV_CODE_LEN);
            GET_ELEM_VALUE_CHAR("NVROnline",szElemValue,  cStatus,IVS_DEV_CODE_LEN,xml);
            stDeiviceBriefInfo.uiStatus = (IVS_UINT32)CDevEnum::GetAlamEnableBool(cStatus);
            xml.OutOfElem();

            if (!CToolsHelp::Memcpy(&(pNvrList->stDeviceBriefInfo[iRealNum]), sizeof(IVS_DEVICE_BRIEF_INFO), &stDeiviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO)))
            {
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(&(pNvrList->stDeviceBriefInfo[iRealNum]), sizeof(IVS_DEVICE_BRIEF_INFO), &stDeiviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO)) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
            }
            

            iRealNum++;
        }while(xml.NextElem());
    }

    xml.OutOfElem();

    // 添加尺寸校验

    IVS_UINT32 unItemSize =(IVS_UINT32)((iRealNum-1 > 0) ? (iRealNum-1) : 0);

    IVS_UINT32 uiSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + unItemSize * sizeof(IVS_DEVICE_BRIEF_INFO);
    if (uiSize > uiBufferSize)
    {
        iRet = IVS_OPERATE_MEMORY_ERROR;
        BP_RUN_LOG_ERR(iRet, "Parse NVRListRsp", "ErrorCode =%d",iRet);
        return iRet;
    }

    return IVS_SUCCEED;
}

// 查询集群信息列表XML拼装
IVS_INT32 CDeviceMgrXMLProcess::GetClusterInfoListReqXML(const IVS_CHAR* pDomainCode, CXml &xml)
{
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("enter GetClusterInfoListReqXML pDomainCode: %s,addr:%d",pDomainCode, &pDomainCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(pDomainCode);
	xml.OutOfElem();

	BP_DBG_LOG("leave GetNVRListReqXML pDomainCode:%s, addr:%d", pDomainCode,&pDomainCode);
	return IVS_SUCCEED;
}

// 查询集群列表响应XML解析
IVS_INT32 CDeviceMgrXMLProcess::ParseClusterListRsp(CXml& xml, IVS_CLUSTER_INFO_LIST* pClusterList)
{
    CHECK_POINTER(pClusterList,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    eSDK_MEMSET(pClusterList, 0, sizeof(IVS_CLUSTER_INFO_LIST));

    if (!xml.FindElemEx("Content/ClusterList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse xml fail", "fail to find node clusertlist");
        return IVS_XML_INVALID;
    }


    const char* szElemValue = NULL;

    IVS_UINT32 unArrayIndex = 0;
    while (xml.FindElemValue("ClusterInfo"))
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_CHAR("ClusterCode", szElemValue, pClusterList->stClusterInfo[unArrayIndex].cClusterCode, IVS_CLUSTER_CODE_LEN, xml);
        GET_ELEM_VALUE_CHAR("ClusterName", szElemValue, pClusterList->stClusterInfo[unArrayIndex].cClusterName, IVS_NAME_LEN, xml);
        GET_ELEM_VALUE_CHAR("ProductType", szElemValue, pClusterList->stClusterInfo[unArrayIndex].cProductType, IVS_CLUSTER_CODE_LEN, xml)
        
        char cStatus[IVS_DEV_CODE_LEN];
        eSDK_MEMSET(cStatus,0,IVS_DEV_CODE_LEN);
        GET_ELEM_VALUE_CHAR("ClusterOnline",szElemValue,  cStatus,IVS_DEV_CODE_LEN,xml);
        pClusterList->stClusterInfo[unArrayIndex].uiStatus = (IVS_UINT32)CDevEnum::GetAlamEnableBool(cStatus); //lint !e1924

        unArrayIndex++;
        xml.OutOfElem();

        if (!xml.NextElem())
        {
            break;
        }
    }
    xml.OutOfElem();

    pClusterList->uiTotal = unArrayIndex;

    return IVS_SUCCEED;
}


// 获取摄像机码流参数拼接XML
IVS_INT32 CDeviceMgrXMLProcess::GetCameraStreamParaGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pCode, CXml &xml )
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);
    (void)xml.AddElem("CameraCode"); 
    (void)xml.SetElemValue(pCode);

    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 获取摄像机列表数目
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceListNum(CXml &xml ,IVS_UINT32 &uiTotal)
{ 
	IVS_DEBUG_TRACE("");

    if (!xml.FindElemEx("Content/DevList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetXml DevinfoNum fail", "xml.FindElemEx(Content/DevList) is fail");
        return IVS_XML_INVALID;
    }

    //xml.IntoElem();
    if (!xml.FindElem("DevInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetXml DevinfoNum fail", "xml.FindElem(DevInfot) is fail");
        return IVS_XML_INVALID;
    }

    IVS_UINT32 iNum = 0;	

    do 
    {
        if (!xml.FindElemValue("DevInfo"))
        {
            break;
        }
        iNum++;
    } while (xml.NextElem());

    xml.OutOfElem();
    xml.OutOfElem();
	uiTotal = iNum;

    return IVS_SUCCEED;
}

// 获取设备状态
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceState(CXml& xml, IVS_DEVICE_STATE& stDeviceState)
{
    IVS_DEBUG_TRACE("");

    if (!xml.FindElem("Content"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get DeviceState", "faill is no 'Content' elemenet");
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();
    if (!xml.FindElem("DevInfo"))
    {
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();

    const char* pTemp = NULL;
    GET_ELEM_VALUE_CHAR("DevCode", pTemp,stDeviceState.cCode, IVS_DEV_CODE_LEN, xml);//lint !e838
    GET_ELEM_VALUE_NUM_FOR_UINT("Status", pTemp, stDeviceState.uiStatus, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("IPType", pTemp, stDeviceState.stIP.uiIPType, xml);

    GET_ELEM_VALUE_CHAR("DevIP",pTemp, stDeviceState.stIP.cIP, IVS_IP_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("DevPort",pTemp, stDeviceState.uiPort, xml);
    xml.OutOfElem();
    xml.OutOfElem();
    return IVS_SUCCEED;
}

// 新增设备下摄像机拼接XML
IVS_INT32 CDeviceMgrXMLProcess::AddDeviceToGroupGetXML(const IVS_CHAR* pTargetDomainCode, IVS_UINT32 uiTargetDevGroupID, IVS_UINT32 uiDeviceNum, const IVS_DEV_GROUP_CAMERA* pDeviceGroupList, CXml& xml )
{
	CHECK_POINTER(pTargetDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDeviceGroupList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("DomainCode");    
	char cTempDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomainCode, IVS_DOMAIN_CODE_LEN + 1, pTargetDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.IntoElem();
    (void)xml.SetElemValue(cTempDomainCode);   
    (void)xml.AddElem("DevList");
    for (IVS_UINT32 uiIndex = 0; uiIndex <uiDeviceNum; uiIndex++)
    {
		if(uiIndex > 0)
		{
			(void)xml.AddElem("DevInfo");
		}
		else
		{
			(void)xml.AddChildElem("DevInfo");
			(void)xml.IntoElem();
		}
        // 设备编码
        (void)xml.AddChildElem("DevCode");
		(void)xml.IntoElem();
		IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		(void)CXmlProcess::ParseDevCode(pDeviceGroupList[uiIndex].cCameraCode, szCameraCode, szDomainCode);
       (void)xml.SetElemValue(szCameraCode);

       // 设备所属域编码
        (void)xml.AddElem("DevDomainCode");
		char cTempDomain[IVS_DOMAIN_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cTempDomain, IVS_DOMAIN_CODE_LEN + 1, (pDeviceGroupList + uiIndex)->cCameraDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cTempDomain, IVS_DOMAIN_CODE_LEN + 1, (pDeviceGroupList + uiIndex)->cCameraDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTempDomain);

        //设备组ID
        (void)xml.AddElem("FromGroupID");
		(void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)(pDeviceGroupList + uiIndex)->uiDevGroupID)).c_str());//lint !e64 匹配
        xml.OutOfElem();   
    }
	xml.OutOfElem();
	(void)xml.AddElem("TargetGroupID");
    (void)xml.SetElemValue((CToolsHelp::Int2Str((IVS_INT32)uiTargetDevGroupID)).c_str());//lint !e64 匹配
    
    xml.OutOfElem();

    return IVS_SUCCEED;
}


// 根据主设备查询子设备拼接XML
IVS_INT32 CDeviceMgrXMLProcess::GetChannelListGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, CXml &xml )
{
    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddElem("Content");
	(void)xml.IntoElem();
    (void)xml.AddChildElem("DomainCode");  
	(void)xml.IntoElem();
    (void)xml.SetElemValue(pDomainCode);   

    (void)xml.AddElem("DevCode");
    (void)xml.SetElemValue(pDevCode);   
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 根据主设备查询子设备解析XML
IVS_INT32 CDeviceMgrXMLProcess::GetChannelListParseXML( CXml &xml , IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList )
{
    CHECK_POINTER(pChannelList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if(!xml.FindElemEx("Content/ChannelList"))
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ChannelList ParseXML fail", "FALSE == xml.FindElemEx('Content/ChannelList')");
        return IVS_OPERATE_MEMORY_ERROR;
    }       

    if (!xml.FindElem("ChannelInfo"))
    {
        BP_RUN_LOG_INF("SMU return XML, no ChannelInfo", "NA");
		pChannelList->uiTotal = 0;
		return IVS_SUCCEED;
    }

    IVS_UINT32 uiReqNum = (pChannelList->stIndexRange.uiToIndex - pChannelList->stIndexRange.uiFromIndex) + 1;
    const IVS_CHAR* szElemValue = NULL;
    IVS_UINT32 uiIndex = 0;

    do{
        if (!xml.FindElemValue("ChannelInfo"))
        {
            break;
        }

        (void)xml.IntoElem();

        IVS_DEV_CHANNEL_BRIEF_INFO &stDevInfo = pChannelList->stDevChannelBriefInfo[uiIndex];
		SecureZeroMemory(&stDevInfo, sizeof(IVS_DEV_CHANNEL_BRIEF_INFO));
     
        // 通道编码
        GET_ELEM_VALUE_CHAR("ChannelCode", szElemValue, stDevInfo.cChannelCode, IVS_DEV_CODE_LEN, xml);

        // 通道类型
        GET_ELEM_VALUE_NUM_FOR_UINT("ChannelType", szElemValue, stDevInfo.uiChannelType, xml);       

		// 通道名称
		GET_ELEM_VALUE_CHAR("ChannelName", szElemValue, stDevInfo.cChannelName, IVS_CHANNEL_NAME_LEN, xml);

        xml.OutOfElem();
        uiIndex++;
    }while (xml.NextElem() && uiIndex < uiReqNum);

    pChannelList->uiTotal  = uiIndex;

    return IVS_SUCCEED;
}

// 修改主设备名称拼接XML
IVS_INT32 CDeviceMgrXMLProcess::SetMainDeviceNameGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName,	CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewDevName, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);  
	(void)xml.AddElem("DevCode");
	(void)xml.SetElemValue(pDevCode);
	(void)xml.AddElem("DevName");
	(void)xml.SetElemValue(pNewDevName);
	xml.OutOfElem();

	return IVS_SUCCEED;

}

// "取得用户拥有设备的域列表"请求xml
IVS_INT32 CDeviceMgrXMLProcess::GetDomainListGetXml(
	const IVS_UINT32 &uiUserID,
	CXml &xml)
{
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("UserID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserID)).c_str());//lint !e64 匹配
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 组装"获取设备列表"时的请求xml
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceListGetXml(IVS_INT32 iType,
	const IVS_CHAR* pDomainCode,
	const IVS_UINT32 &uiDevType,
	const IVS_INDEX_RANGE &stRange,
	CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("DevType");
	(void)xml.IntoElem();
	switch (uiDevType)
	{
	case DEVICE_TYPE_CAMERA:
	case DEVICE_TYPE_CAMERA_EX:
		{
			(void)xml.SetElemValue("1");
			break;
		}
	case DEVICE_TYPE_ALARM:
		{
			(void)xml.SetElemValue("2");
			break;
		}
	case DEVICE_TYPE_VOICE:
		{
			(void)xml.SetElemValue("3");
			break;
		}
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			(void)xml.SetElemValue("9");
			break;
		}
    case DEVICE_TYPE_SHADOW_CAMERA:
        {
            (void)xml.SetElemValue("30");
            break;
        }
    case DEVICE_TYPE_CAMERA_AND_SHADOW:
        {
            (void)xml.SetElemValue("31");
            break;
        }

	default:
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get DeviceList GetXml fail","Error devType");
			return IVS_PARA_INVALID;
		}
	}
	(void)xml.AddElem("IsExDomain");
	(void)xml.IntoElem();
	if (0 == iType)
	{
		(void)xml.SetElemValue("0");
	}
	if (1 == iType)
	{
		(void)xml.SetElemValue("1");
	}

	(void)xml.AddElem("PageInfo");
	(void)xml.IntoElem();
	{
		(void)xml.AddChildElem("FromIndex");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRange.uiFromIndex)).c_str());//lint !e64 匹配

		(void)xml.AddElem("ToIndex");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(stRange.uiToIndex)).c_str());//lint !e64 匹配
		
		xml.OutOfElem();
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析 "取得用户拥有设备的域列表"的响应
IVS_INT32 CDeviceMgrXMLProcess::GetDomainListParseXml(CXml &xml,std::vector<std::string> &dmList)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/DomainList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get DomainList ParseXml fail", "xml.FindElemEx(Content/DomainList) is fail");
		return IVS_XML_INVALID;
	}

	// 假设一个用户至少有一个域
	if (!xml.FindElem("DomainCode"))
	{
		BP_RUN_LOG_INF("no domaincode", "xml.FindElem(DomainCode) is fail");
		return IVS_SUCCEED;
	}

	do 
	{
		if (!xml.FindElemValue("DomainCode"))
		{
			break;
		}
		const IVS_CHAR* szElemValue = xml.GetElemValue();
        if (NULL != szElemValue)
        {
            dmList.push_back(szElemValue);
        }
		
		
	} while (xml.NextElem());

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

#define DEV_FORM_TYPE_LEN 10

IVS_INT32 CDeviceMgrXMLProcess::ParseVideoChannelList(const IVS_CHAR* pDomainCode, 
	const IVS_CHAR* pData, 
	const IVS_INT32 iExDomainFlag, 
	IVS_PAGE_INFO& pageInfo, 
	CAMERA_BRIEF_INFO_ALL_VEC& videoChannelVector)
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pData, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    CXml xml;
    if (!xml.Parse(pData))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse video channel list", "fail to parse xml");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iRet = IVS_FAIL;
    const IVS_CHAR* szElemValue = NULL;
    if(!xml.FindElemEx("Content/PageInfo/RealNum"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse video channel list", "fail to find realnum item");
        return IVS_XML_INVALID;
    }
    else
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",szElemValue, pageInfo.uiRealNum, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex",szElemValue, pageInfo.uiFromIndex, xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex",szElemValue, pageInfo.uiToIndex, xml);
    }

    (void)xml.FindElemEx("Content/VideoInChanList");

    IVS_CAMERA_BRIEF_INFO_ALL stCameraInfo;
    do
    {
        eSDK_MEMSET(&stCameraInfo, 0 ,sizeof(stCameraInfo));    
        if (!xml.FindElemValue("VideoInChanInfo"))
        {
            break;
        }
        (void)xml.IntoElem();
        // 设备组编码，需要连接“#域编码”
        GET_ELEM_VALUE_CHAR("GroupID",szElemValue,stCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
        IVS_CHAR szGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
        eSDK_STRNCPY(szGroupCode, sizeof(szGroupCode), stCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
        std::string strDevGroupCode = szGroupCode;
        strDevGroupCode.append("#").append(pDomainCode);
        eSDK_STRNCPY(stCameraInfo.cDevGroupCode, sizeof(stCameraInfo.cDevGroupCode), strDevGroupCode.c_str(), IVS_DEVICE_GROUP_LEN);

        // 摄像机编码，需要连接“#域编码”
        GET_ELEM_VALUE_CHAR("CameraCode", szElemValue,stCameraInfo.cCode, IVS_DEV_CODE_LEN, xml);
        IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
        eSDK_STRNCPY(szCameraCode, sizeof(szCameraCode), stCameraInfo.cCode, IVS_DEV_CODE_LEN);
        std::string strCameraCode = szCameraCode;
        strCameraCode.append("#").append(pDomainCode);
        eSDK_STRNCPY(stCameraInfo.cCode, sizeof(stCameraInfo.cCode), strCameraCode.c_str(), IVS_DEV_CODE_LEN);

        // 摄像机类型
        const int tempLength = 10;
        IVS_CHAR tempChar[tempLength]={0};
        GET_ELEM_VALUE_CHAR("CameraType",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        stCameraInfo.uiType = (unsigned int)CDevEnum::GetCameraTypeEnum(tempChar);
        if (stCameraInfo.uiType > CAMERA_BULLET)
        {
            iRet = IVS_XML_INVALID;
            BP_RUN_LOG_ERR(iRet, "parse video channel list","Error CameraType:[%s]",tempChar);
            return iRet; 
        }

        GET_ELEM_VALUE_CHAR("CameraName", szElemValue,stCameraInfo.cName, IVS_CAMERA_NAME_LEN, xml);                                        // CameraName
        GET_ELEM_VALUE_CHAR("ParentCode", szElemValue,stCameraInfo.cParentCode, IVS_DEV_CODE_LEN, xml);                                           // ParentCode
        GET_ELEM_VALUE_CHAR("DevModelType", szElemValue,stCameraInfo.cDevModelType, IVS_DEV_MODEL_LEN, xml);                            // DevModelType
        GET_ELEM_VALUE_CHAR("DevVendorType", szElemValue, stCameraInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, xml);    // DevVendorType

        // DevFormType
        IVS_CHAR cDevFormType[DEV_FORM_TYPE_LEN + 1] = {0};
        GET_ELEM_VALUE_CHAR("DevFormType", szElemValue, cDevFormType, DEV_FORM_TYPE_LEN, xml);
        stCameraInfo.uiDevFormType = CDevEnum::GetDevTypeEnum(cDevFormType);

        GET_ELEM_VALUE_CHAR("CameraLocation", szElemValue,stCameraInfo.cCameraLocation, IVS_DESCRIBE_LEN, xml);         // CameraLocation

        GET_ELEM_VALUE_NUM_FOR_UINT("ExtendStatus",szElemValue,stCameraInfo.uiCameraStatus,xml);         // ExtendStatus

        // IsOnline
        eSDK_MEMSET(tempChar,0,tempLength);
        GET_ELEM_VALUE_CHAR("IsOnline",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        if(0 == strcmp(tempChar,"ON"))
        {
            stCameraInfo.uiStatus = DEV_STATUS_ONLINE;
        }
        else if(0 == strcmp(tempChar,"OFF"))
        {
            stCameraInfo.uiStatus = DEV_STATUS_OFFLINE;
        }
        else 
        {
            iRet = IVS_XML_INVALID;
            BP_RUN_LOG_ERR(iRet, "GetCameras ParseXml fail","Error IsOnline:[%s]",tempChar);
            return iRet; 
        }

        // NetType
        eSDK_MEMSET(tempChar,0,tempLength);
        GET_ELEM_VALUE_CHAR("NetType",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        if(0 == strcmp(tempChar,"LAN"))
        {
            stCameraInfo.uiNetType = NET_TYPE_LAN;
        }
        else
        {
            stCameraInfo.uiNetType = NET_TYPE_WAN;
        }

        // IsIntelligent
        eSDK_MEMSET(tempChar,0,tempLength);
        GET_ELEM_VALUE_CHAR("IsIntelligent",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        if(0 == strcmp(tempChar,"TRUE"))
        {
            stCameraInfo.bSupportIntelligent = 1;
        }
        else
        {
            stCameraInfo.bSupportIntelligent = 0;
        }

        // VoiceInEnable
        eSDK_MEMSET(tempChar,0,tempLength);
        GET_ELEM_VALUE_CHAR("VoiceInEnable",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        // 随路音频是否启用	0-停用 1-启用
        if (0 == strcmp(tempChar,"1"))
        {
            stCameraInfo.bEnableVoice = 1;
        }
        else
        {
            stCameraInfo.bEnableVoice = 0;
        }

        GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,stCameraInfo.cNvrCode,IVS_NVR_CODE_LEN,xml);   // NVRCode
        GET_ELEM_VALUE_CHAR("DevIP", szElemValue, stCameraInfo.cDevIp, IVS_IP_LEN, xml);   // DevIP
        GET_ELEM_VALUE_CHAR("DevCreateTime", szElemValue, stCameraInfo.cDevCreateTime, IVS_TIME_LEN, xml);   // DevCreateTime

		GET_ELEM_VALUE_CHAR("Longitude", szElemValue, stCameraInfo.cLongitude, IVS_LONGITUDE_LATITUDE_LEN, xml);   // 经度
		GET_ELEM_VALUE_CHAR("Latitude", szElemValue, stCameraInfo.cLatitude, IVS_LONGITUDE_LATITUDE_LEN, xml);   // 纬度
		GET_ELEM_VALUE_NUM_FOR_FLOAT("Height", szElemValue, stCameraInfo.fHeight, xml);		//摄像机安装高度

        stCameraInfo.bIsExDomain = iExDomainFlag;
        eSDK_MEMCPY(stCameraInfo.cDomainCode, sizeof(stCameraInfo.cDomainCode), pDomainCode, IVS_DOMAIN_CODE_LEN);           // 域编码

		//begin 以下节点只有影子设备才有
		GET_ELEM_VALUE_NUM_FOR_UINT("IsShadowDev", szElemValue,stCameraInfo.bIsShadowDev, xml);	//lint !e713		//是否为影子设备
		GET_ELEM_VALUE_CHAR("OrigDevCode", szElemValue, stCameraInfo.cOrigDevCode, IVS_DEV_CODE_LEN, xml);   // 原始设备编码
		GET_ELEM_VALUE_CHAR("OrigParentDevCode", szElemValue, stCameraInfo.cOrigParentDevCode, IVS_DEV_CODE_LEN, xml);   // 原始父设备编码
		GET_ELEM_VALUE_CHAR("OriDevDomainCode", szElemValue, stCameraInfo.cOriDevDomainCode, IVS_DOMAIN_CODE_LEN, xml);   // 原始域编码
		//end 

        eSDK_MEMSET(stCameraInfo.cReserve,0, sizeof(stCameraInfo.cReserve));          // 保留字段

        xml.OutOfElem();
        videoChannelVector.push_back(stCameraInfo);
    }while (xml.NextElem());

    return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgrXMLProcess::ParseAlarmChannelList(const IVS_CHAR* pDomainCode, 
    const IVS_CHAR* pData, 
    const IVS_INT32 iExDomainFlag,
    IVS_PAGE_INFO& pageInfo, 
    std::vector<IVS_ALARM_BRIEF_INFO>& alarmChannelVector)
{
    
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    CXml xml;
    if (!xml.Parse(pData))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "parse alarm channel list", "fail to parse xml");
        return IVS_XML_INVALID;
    }

    const IVS_CHAR* szElemValue = NULL;
    if(!xml.FindElemEx("Content/PageInfo/RealNum"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetAlarms ParseXml fail", "FALSE == xml.FindElemEx('Content/PageInfo/RealNum')");
        return IVS_XML_INVALID;
    }
    else
    {
        (void)xml.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",szElemValue,pageInfo.uiRealNum,xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex",szElemValue,pageInfo.uiFromIndex,xml);
        GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex",szElemValue,pageInfo.uiToIndex,xml);
    }

    if(!xml.FindElemEx("Content/AlarmDevList") && pageInfo.uiRealNum == 0)
    {
        BP_RUN_LOG_INF("GetAlarms ParseXml fail", "AlarmDevList is Empty");	
        return IVS_SUCCEED;
    } 

    do
    {
        if (!xml.FindElemValue("AlarmDevInfo"))
        {
            break;
        }

        (void)xml.IntoElem();

        IVS_ALARM_BRIEF_INFO stAlarmInfo;
        eSDK_MEMSET(&stAlarmInfo, 0 ,sizeof(IVS_ALARM_BRIEF_INFO));           		

        // 设备组编码
        GET_ELEM_VALUE_CHAR("GroupID",szElemValue,stAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
        // 连接上域编码
        IVS_CHAR szGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
        (void)CToolsHelp::Memcpy(szGroupCode, IVS_DEVICE_GROUP_LEN, stAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
        std::string strDevGroupCode = szGroupCode;
        strDevGroupCode.append("#").append(pDomainCode);
        (void)CToolsHelp::Memcpy(stAlarmInfo.cDevGroupCode,IVS_DEVICE_GROUP_LEN,strDevGroupCode.c_str(),strDevGroupCode.length());//lint !e64 匹配

        // 通道类型
        const int tempLength = 10;
        IVS_CHAR tempChar[tempLength]={0};
        GET_ELEM_VALUE_CHAR("ChannelType",szElemValue,tempChar,tempLength,xml);
        tempChar[tempLength-1] = 0;
        if(0 == strcmp(tempChar,"02"))
        {
            stAlarmInfo.uiChannelType = 0;
        }
        else
        {
            stAlarmInfo.uiChannelType = 1;
        }
        // 告警设备编码
        GET_ELEM_VALUE_CHAR("DevCode",szElemValue,stAlarmInfo.cAlarmCode,IVS_DEV_CODE_LEN,xml);
        // "#域编码"
        IVS_CHAR szDevCode[IVS_DEV_CODE_LEN + 1] = {0};
        (void)CToolsHelp::Memcpy(szDevCode, IVS_DEV_CODE_LEN, stAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN);
        std::string strAlarmCode = szDevCode;
        strAlarmCode.append("#").append(pDomainCode);
        (void)CToolsHelp::Memcpy(stAlarmInfo.cAlarmCode,IVS_ALARM_CODE_LEN,strAlarmCode.c_str(),strAlarmCode.length());//lint !e64 匹配

        // 域编码
        eSDK_STRNCPY(stAlarmInfo.cDomainCode, sizeof(stAlarmInfo.cDomainCode), pDomainCode, IVS_DOMAIN_CODE_LEN);

        // 告警设备名称
        GET_ELEM_VALUE_CHAR("DevName",szElemValue,stAlarmInfo.cAlarmName,IVS_ALARM_IN_NAME_LEN,xml);
        // 保留字段
        eSDK_MEMSET(stAlarmInfo.cReserve,0,32);

        GET_ELEM_VALUE_CHAR("ParentCode", szElemValue, stAlarmInfo.cParentCode, IVS_DEV_CODE_LEN, xml);

        // GET_ELEM_VALUE_NUM("IsExDomian", szElemValue, stAlarmInfo.bIsExDomain, xml);
        stAlarmInfo.bIsExDomain = iExDomainFlag;
        xml.OutOfElem();

        alarmChannelVector.push_back(stAlarmInfo);
        
    }while (xml.NextElem());

    return IVS_SUCCEED;
}

// 获取摄像机列表后的响应xml解析
IVS_INT32 CDeviceMgrXMLProcess::GetCamerasParseXml(IVS_INT32 iType, const IVS_CHAR* pDomainCode,
	CXml &xml,
	std::vector<IVS_SHADOW_CAMERA_BRIEF_INFO>* pCameraVec,
	IVS_UINT32 &uiSize,
	IVS_INDEX_RANGE& outRange)
{
	CHECK_POINTER(pCameraVec, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	const IVS_CHAR* szElemValue = NULL;
	if(!xml.FindElemEx("Content/PageInfo/RealNum"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Cameras ParseXml fail", "FindElemEx('Content/PageInfo/RealNum') Failed");
		return IVS_XML_INVALID;
	}
	else
	{
		(void)xml.IntoElem();
		GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",szElemValue,uiSize,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex",szElemValue,outRange.uiFromIndex,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex",szElemValue,outRange.uiToIndex,xml);
	}

	CHECK_SUCCESS_RET(xml.FindElemEx("Content/VideoInChanList"), IVS_SUCCEED);

	// modify by lWX175483 2013.5.22 DTS2013051802076
	// 否则如果SMU返回的报文中包含 <VideoInChanList />，SDK会直接返回IVS_XML_INVALID，而实际上该XML是合法的，这里对VideoInChanInfo进行校验无意义

	//CHECK_SUCCESS_RET(xml.FindElem("VideoInChanInfo"), IVS_XML_INVALID);

	do{
		if (!xml.FindElemValue("VideoInChanInfo"))
		{
			break;
		}

		(void)xml.IntoElem();

		IVS_SHADOW_CAMERA_BRIEF_INFO stCameraInfo;
		eSDK_MEMSET(&stCameraInfo, 0 ,sizeof(IVS_SHADOW_CAMERA_BRIEF_INFO));           		

		// 设备组编码
		GET_ELEM_VALUE_CHAR("GroupID",szElemValue,stCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
		// 连接上域编码
		IVS_CHAR szGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
		(void)CToolsHelp::Memcpy(szGroupCode, IVS_DEVICE_GROUP_LEN, stCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
		std::string strDevGroupCode = szGroupCode;
		strDevGroupCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stCameraInfo.cDevGroupCode,IVS_DEVICE_GROUP_LEN,strDevGroupCode.c_str(),strDevGroupCode.length());//lint !e64 匹配
		
		// 摄像机编码		
		GET_ELEM_VALUE_CHAR("CameraCode", szElemValue,stCameraInfo.cCode, IVS_DEV_CODE_LEN, xml);
		// "连接上#域编码"
		IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		(void)CToolsHelp::Memcpy(szCameraCode, IVS_DEV_CODE_LEN, stCameraInfo.cCode, IVS_DEV_CODE_LEN);
		std::string strCameraCode = szCameraCode;
		strCameraCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stCameraInfo.cCode,IVS_DEV_CODE_LEN,strCameraCode.c_str(),strCameraCode.length());//lint !e64 匹配

		// 域编码
		if (!CToolsHelp::Memcpy(stCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		// 摄像机类型
		const int tempLength = 10;
		IVS_CHAR tempChar[tempLength]={0};
		GET_ELEM_VALUE_CHAR("CameraType",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;

		stCameraInfo.uiType = (unsigned int)CDevEnum::GetCameraTypeEnum(tempChar); //lint !e1924
		if (stCameraInfo.uiType > CAMERA_BULLET)
		{
			iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "GetCameras ParseXml fail","Error CameraType:[%s]",tempChar);
			return iRet; 
		}
		// CameraName
		GET_ELEM_VALUE_CHAR("CameraName", szElemValue,stCameraInfo.cName, IVS_CAMERA_NAME_LEN, xml);

		// ParentCode
		GET_ELEM_VALUE_CHAR("ParentCode", szElemValue,stCameraInfo.cParentCode, IVS_DEV_CODE_LEN, xml);

		// DevModelType
		GET_ELEM_VALUE_CHAR("DevModelType", szElemValue,stCameraInfo.cDevModelType, IVS_DEV_MODEL_LEN, xml);

		// DevVendorType
		GET_ELEM_VALUE_CHAR("DevVendorType", szElemValue, stCameraInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, xml);

		// DevFormType
		IVS_CHAR cDevFormType[DEV_FORM_TYPE_LEN + 1] = {0};
		GET_ELEM_VALUE_CHAR("DevFormType", szElemValue, cDevFormType, DEV_FORM_TYPE_LEN, xml);
		stCameraInfo.uiDevFormType = CDevEnum::GetDevTypeEnum(cDevFormType);

		// CameraLocation
		GET_ELEM_VALUE_CHAR("CameraLocation", szElemValue,stCameraInfo.cCameraLocation, IVS_DESCRIBE_LEN, xml);

		// ExtendStatus
		GET_ELEM_VALUE_NUM_FOR_UINT("ExtendStatus",szElemValue,stCameraInfo.uiCameraStatus,xml);

		// IsOnline
		eSDK_MEMSET(tempChar,0,tempLength);
		GET_ELEM_VALUE_CHAR("IsOnline",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		if(0 == strcmp(tempChar,"ON"))
		{
			stCameraInfo.uiStatus = DEV_STATUS_ONLINE;
		}
		else if(0 == strcmp(tempChar,"OFF"))
		{
			stCameraInfo.uiStatus = DEV_STATUS_OFFLINE;
		}
		else 
		{
			iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "GetCameras ParseXml fail","Error IsOnline:[%s]",tempChar);
			return iRet; 
		}
		
		// NetType
		eSDK_MEMSET(tempChar,0,tempLength);
		GET_ELEM_VALUE_CHAR("NetType",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		if(0 == strcmp(tempChar,"LAN"))
		{
			stCameraInfo.uiNetType = NET_TYPE_LAN;
		}
		else
		{
			stCameraInfo.uiNetType = NET_TYPE_WAN;
		}

		// IsIntelligent
		eSDK_MEMSET(tempChar,0,tempLength);
		GET_ELEM_VALUE_CHAR("IsIntelligent",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		if(0 == strcmp(tempChar,"TRUE"))
		{
			stCameraInfo.bSupportIntelligent = 1;
		}
		else
		{
			stCameraInfo.bSupportIntelligent = 0;
		}

		// VoiceInEnable
		eSDK_MEMSET(tempChar,0,tempLength);
		GET_ELEM_VALUE_CHAR("VoiceInEnable",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		// 随路音频是否启用	0-停用 1-启用
		if (0 == strcmp(tempChar,"1"))
		{
			stCameraInfo.bEnableVoice = 1;
		}
		else
		{
			stCameraInfo.bEnableVoice = 0;
		}

		// NVRCode
		GET_ELEM_VALUE_CHAR("NVRCode",szElemValue,stCameraInfo.cNvrCode,IVS_NVR_CODE_LEN,xml);

		// DevCreateTime
		GET_ELEM_VALUE_CHAR("DevCreateTime", szElemValue, stCameraInfo.cDevCreateTime, IVS_TIME_LEN, xml);

		// DevIP
		GET_ELEM_VALUE_CHAR("DevIP", szElemValue, stCameraInfo.cDevIp, IVS_IP_LEN, xml);

		GET_ELEM_VALUE_CHAR("longitude", szElemValue, stCameraInfo.cLongitude, IVS_LONGITUDE_LATITUDE_LEN, xml);   // 经度
		GET_ELEM_VALUE_CHAR("latitude", szElemValue, stCameraInfo.cLongitude, IVS_LONGITUDE_LATITUDE_LEN, xml);   // 纬度
		GET_ELEM_VALUE_NUM_FOR_FLOAT("Height", szElemValue, stCameraInfo.fHeight, xml);		//摄像机安装高度

		if (!CToolsHelp::Memcpy(stCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		//影子IPC相关信息
		eSDK_MEMSET(tempChar,0,tempLength);
		GET_ELEM_VALUE_CHAR("IsShadowDev",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		// 是否影子IPC	0-否 1-是
		if (0 == strcmp(tempChar,"1"))
		{
			stCameraInfo.bIsShadowDev = 1;
		}
		else
		{
			stCameraInfo.bIsShadowDev = 0;
		}

		GET_ELEM_VALUE_CHAR("OrigDevCode",szElemValue,stCameraInfo.cOrigDevCode,IVS_DEV_CODE_LEN,xml); 
		GET_ELEM_VALUE_CHAR("OriDevDomainCode", szElemValue, stCameraInfo.cOriDevDomainCode, IVS_DOMAIN_CODE_LEN, xml);   

		
		// 保留字段
		eSDK_MEMSET(stCameraInfo.cReserve,0,32);

		stCameraInfo.bIsExDomain = iType;

		xml.OutOfElem();
		pCameraVec->push_back(stCameraInfo);
	}while (xml.NextElem());

	return IVS_SUCCEED;
}

// 获取告警列表后的响应xml解析
IVS_INT32 CDeviceMgrXMLProcess::GetAlarmsParseXml(const IVS_CHAR* pDomainCode,
	CXml &xml,
	std::vector<IVS_ALARM_BRIEF_INFO>* pAlarmVec,
	IVS_UINT32 &uiSize,
	IVS_INDEX_RANGE& outRange)
{
	CHECK_POINTER(pAlarmVec, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	const IVS_CHAR* szElemValue = NULL;
	if(!xml.FindElemEx("Content/PageInfo/RealNum"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetAlarms ParseXml fail", "FALSE == xml.FindElemEx('Content/PageInfo/RealNum')");
		return IVS_XML_INVALID;
	}
	else
	{
		(void)xml.IntoElem();
		GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",szElemValue,uiSize,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex",szElemValue,outRange.uiFromIndex,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex",szElemValue,outRange.uiToIndex,xml);
	}

	if(!xml.FindElemEx("Content/AlarmDevList") && uiSize == 0)
	{
		BP_RUN_LOG_INF("GetAlarms ParseXml fail", "AlarmDevList is Empty");	
		return IVS_SUCCEED;
	} 

	do{
		if (!xml.FindElemValue("AlarmDevInfo"))
		{
			break;
		}

		(void)xml.IntoElem();

		IVS_ALARM_BRIEF_INFO stAlarmInfo;
		eSDK_MEMSET(&stAlarmInfo, 0 ,sizeof(IVS_ALARM_BRIEF_INFO));           		

		// 设备组编码
		GET_ELEM_VALUE_CHAR("GroupID",szElemValue,stAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
		// 连接上域编码
		IVS_CHAR szGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
		(void)CToolsHelp::Memcpy(szGroupCode, IVS_DEVICE_GROUP_LEN, stAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
		std::string strDevGroupCode = szGroupCode;
		strDevGroupCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stAlarmInfo.cDevGroupCode,IVS_DEVICE_GROUP_LEN,strDevGroupCode.c_str(),strDevGroupCode.length());//lint !e64 匹配

		// 通道类型
		const int tempLength = 10;
		IVS_CHAR tempChar[tempLength]={0};
		GET_ELEM_VALUE_CHAR("ChannelType",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		if(0 == strcmp(tempChar,"02"))
		{
			stAlarmInfo.uiChannelType = 0;
		}
		else
		{
			stAlarmInfo.uiChannelType = 1;
		}
		// 告警设备编码
		GET_ELEM_VALUE_CHAR("DevCode",szElemValue,stAlarmInfo.cAlarmCode,IVS_DEV_CODE_LEN,xml);
		// "#域编码"
		IVS_CHAR szDevCode[IVS_DEV_CODE_LEN + 1] = {0};
		(void)CToolsHelp::Memcpy(szDevCode, IVS_DEV_CODE_LEN, stAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN);
		std::string strAlarmCode = szDevCode;
		strAlarmCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stAlarmInfo.cAlarmCode, sizeof(stAlarmInfo.cAlarmCode),strAlarmCode.c_str(),strAlarmCode.length());//lint !e64 匹配

		// 域编码
		if (!CToolsHelp::Memcpy(stAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		// 告警设备名称
		GET_ELEM_VALUE_CHAR("DevName",szElemValue,stAlarmInfo.cAlarmName,IVS_ALARM_IN_NAME_LEN,xml);
		// 保留字段
		eSDK_MEMSET(stAlarmInfo.cReserve,0,32);

		GET_ELEM_VALUE_CHAR("ParentCode", szElemValue, stAlarmInfo.cParentCode, IVS_DEV_CODE_LEN, xml);

		GET_ELEM_VALUE_NUM("IsExDomian", szElemValue, stAlarmInfo.bIsExDomain, xml);
		xml.OutOfElem();

		pAlarmVec->push_back(stAlarmInfo);
	}while (xml.NextElem());

	return IVS_SUCCEED;
}

// 获取语音设备列表后的响应xml解析
IVS_INT32 CDeviceMgrXMLProcess::GetAudiosParseXml(const IVS_CHAR* pDomainCode,
	CXml &xml,
	std::vector<IVS_AUDIO_BRIEF_INFO>* pAudioVec,
	IVS_UINT32 &uiSize,
	IVS_INDEX_RANGE& outRange)
{
	CHECK_POINTER(pAudioVec, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	const IVS_CHAR* szElemValue = NULL;
	if(!xml.FindElemEx("Content/PageInfo/RealNum"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetAudios ParseXml fail", "FindElemEx('Content/PageInfo/RealNum') Failed");
		return IVS_XML_INVALID;
	}
	else
	{
		(void)xml.IntoElem();
		GET_ELEM_VALUE_NUM_FOR_UINT("RealNum",szElemValue,uiSize,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex",szElemValue,outRange.uiFromIndex,xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex",szElemValue,outRange.uiToIndex,xml);
	}

	if(!xml.FindElemEx("Content/VoiceChanList") && uiSize == 0)
	{
		BP_RUN_LOG_INF("GetAudiosParseXml fail", "VoiceChanList is Empty");	
		return IVS_SUCCEED;
	} 

	do{
		if (!xml.FindElemValue("VoiceChanInfo"))
		{
			break;
		}

		(void)xml.IntoElem();

		IVS_AUDIO_BRIEF_INFO stAudioInfo;
		eSDK_MEMSET(&stAudioInfo, 0 ,sizeof(IVS_AUDIO_BRIEF_INFO));           		

		// 设备组编码
		// 设备组编码
		GET_ELEM_VALUE_CHAR("GroupID",szElemValue,stAudioInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, xml);
		// 连接上域编码
		IVS_CHAR szGroupCode[IVS_DEVICE_GROUP_LEN + 1] = {0};
		(void)CToolsHelp::Memcpy(szGroupCode, IVS_DEVICE_GROUP_LEN, stAudioInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN);
		std::string strDevGroupCode = szGroupCode;
		strDevGroupCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stAudioInfo.cDevGroupCode,IVS_DEVICE_GROUP_LEN,strDevGroupCode.c_str(),strDevGroupCode.length());//lint !e64 匹配

		// 通道类型
		const int tempLength = 10;
		IVS_CHAR tempChar[tempLength]={0};
		GET_ELEM_VALUE_CHAR("ChannelType",szElemValue,tempChar,tempLength,xml);
		tempChar[tempLength-1] = 0;
		if(0 == strcmp(tempChar,"04"))
		{
			stAudioInfo.uiChannelType = 0;
		}
		else
		{
			stAudioInfo.uiChannelType = 1;
		}
		// 语音设备编码
		char cAudioCodeTemp[IVS_DEV_CODE_LEN + 1] = {0};
		GET_ELEM_VALUE_CHAR("VoiceCode",szElemValue, cAudioCodeTemp,IVS_DEV_CODE_LEN,xml);
		std::string strAudioCode = cAudioCodeTemp;
		strAudioCode.append("#").append(pDomainCode);
		(void)CToolsHelp::Memcpy(stAudioInfo.cAudioCode, IVS_DEV_CODE_LEN, strAudioCode.c_str(), strAudioCode.length());//lint !e64 匹配

		// 语音设备名称
		GET_ELEM_VALUE_CHAR("VoiceName",szElemValue,stAudioInfo.cAudioName,IVS_AUDIO_NAME_LEN,xml);	
		// 保留字段
		eSDK_MEMSET(stAudioInfo.cReserve,0,32);

		xml.OutOfElem();
		pAudioVec->push_back(stAudioInfo);
	}while (xml.NextElem());

	return IVS_SUCCEED;
}

// 修改摄像机名称拼接XML
IVS_INT32 CDeviceMgrXMLProcess::SetCameraNameGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName,CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewCameraName, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);  
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("CameraName");
	(void)xml.SetElemValue(pNewCameraName);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 重启前端设备拼接XML
IVS_INT32 CDeviceMgrXMLProcess::RestartDeviceGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDeviceCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("RebootType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRebootType)).c_str());//lint !e64 匹配
	(void)xml.AddElem("DevCode");
	(void)xml.SetElemValue(pDeviceCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装查询平台抓拍计划请求的xml
IVS_INT32 CDeviceMgrXMLProcess::GetRecordTaskGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("UserDomain");
	(void)xml.SetElemValue(pUserDomainCode);
	(void)xml.AddElem("UserID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str());//lint !e64 匹配

	return IVS_SUCCEED;
}

#define SERVER_CODE_LEN 256

IVS_INT32 CDeviceMgrXMLProcess::GetServerCodeParseXML(CXml& xml, const IVS_CHAR* pXmlNodePath, std::string& strServerCode)
{
    
    if (!xml.FindElemEx(pXmlNodePath))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "get server code from request xml", "fail to find ServerCode node in xml");
        return IVS_XML_INVALID;
    }

    const IVS_CHAR* pServerCode = xml.GetElemValue();
    if (NULL == pServerCode)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "get server code from request xml", "fail to find ServerCode node in xml");
        return IVS_XML_INVALID;
    }
    strServerCode = pServerCode;//lint !e64 匹配
    return IVS_SUCCEED;
}

IVS_INT32 CDeviceMgrXMLProcess::GetDomainCodeParseXML(CXml& xml, const IVS_CHAR* pXmlNodePath, std::string& strDomainCode)
{

    if (!xml.FindElemEx(pXmlNodePath))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "get domain code from request xml", "fail to find DomainCode node in xml");
        return IVS_XML_INVALID;
    }

    const IVS_CHAR* pDomainCode = xml.GetElemValue();
    if (NULL == pDomainCode)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "get domain code from request xml", "fail to find ServerCode node in xml");
        return IVS_XML_INVALID;
    }
    strDomainCode = pDomainCode;//lint !e64 匹配
    return IVS_SUCCEED;
}

// 启动搜索前端设备解析XML
IVS_INT32 CDeviceMgrXMLProcess::SearchDeviceParseXML(CXml &xml, const IVS_CHAR* pAddress)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx(pAddress))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/SearchInfo/ServerCode) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR cServerCode[SERVER_CODE_LEN] = {0};
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("ServerCode", pTemp, cServerCode, SERVER_CODE_LEN, xml);//lint !e838
	if (0 == strcmp(cServerCode, ""))
	{
		BP_RUN_LOG_INF("ServerCode is NULL", "Start SearchDevice ParseXML");
		return IVS_PARA_INVALID;
	}
	return IVS_SUCCEED;
}

// 修改ServerCode
IVS_INT32 CDeviceMgrXMLProcess::ModifyServerCode(CXml &xml, const std::string& strServerCode, const IVS_CHAR* pAddress)
{
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx(pAddress))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(pAddress) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.SetElemValue(strServerCode.c_str());

	return IVS_SUCCEED;
}

IVS_INT32 CDeviceMgrXMLProcess::FindIdlestNvrParseXML(std::string& strServerCode, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/NVRInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/NVRInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pTemp = NULL;
	IVS_CHAR cServerCode[SERVER_CODE_LEN + 1] = {0};
	GET_ELEM_VALUE_CHAR("NVRCode", pTemp, cServerCode, SERVER_CODE_LEN, xml);//lint !e838
	strServerCode = cServerCode;

	return IVS_SUCCEED;
}

// 设置设备组备注信息拼接XML
IVS_INT32 CDeviceMgrXMLProcess::SetDevGroupRemarkGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, const IVS_CHAR* pDevGroupRemark, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevGroupRemark, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("GroupInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("GroupID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiDevGroupID)).c_str());//lint !e64 匹配

	(void)xml.AddElem("GroupRemark");
	(void)xml.SetElemValue(pDevGroupRemark);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询设备组备注信息拼接XML
IVS_INT32 CDeviceMgrXMLProcess::GetDevGroupRemarkGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("GroupID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiDevGroupID)).c_str());//lint !e64 匹配

	return IVS_SUCCEED;
}


#define IVS_GROUP_MAX_REMARK_LEN 324
// 查询设备组备注信息解析XML
IVS_INT32 CDeviceMgrXMLProcess::GetDevGroupRemarkParseXML(IVS_CHAR* pDevGroupRemark, CXml &xml, IVS_UINT32 uiRemarkLen)
{
	CHECK_POINTER(pDevGroupRemark, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("GroupRemark", pTemp, pDevGroupRemark, IVS_GROUP_MAX_REMARK_LEN, xml);//lint !e838
	if (strlen(pDevGroupRemark) > uiRemarkLen)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "strlen(pDevGroupRemark) > uiRemarkLen", "pDevGroupRemark fail");
		return IVS_XML_INVALID;
	}

	return IVS_SUCCEED;
}

// 查询摄像机码流能力
IVS_INT32 CDeviceMgrXMLProcess::GetDeviceCapabilityGetXML(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	return IVS_SUCCEED;
}

// 导入前端配置文件拼接XML
IVS_INT32 CDeviceMgrXMLProcess::ImportDevCfgFileGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCFGFile, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("DevCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	(void)xml.AddElem("CFGFile");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCFGFile);

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 设置前端设备密码拼接XML
IVS_INT32 CDeviceMgrXMLProcess::SetDevPWDGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const std::string &strDevPWD, const std::string &strDevRegPWD, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(strDevPWD.c_str(), IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("DevCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	(void)xml.AddElem("DevUser");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevUserName);

	(void)xml.AddElem("DevPWD");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strDevPWD.c_str());

	(void)xml.AddElem("DevRegPWD");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strDevRegPWD.c_str());

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 新增设备（堆叠密钥交换情况下）单条增加
IVS_INT32 CDeviceMgrXMLProcess::AddDevicePileGetXML(const IVS_CHAR* pDomainCode, const IVS_DEVICE_OPER_INFO* pDevList, const std::string &strDevPWD, const std::string &strDevRegPWD, IVS_UINT32 uiNum, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("DevList");    
	(void)xml.AddChildElem("DevInfo");
	(void)xml.IntoElem();
	//添加序号
	(void)xml.AddChildElem("Sequence");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevList[uiNum].uiSequence)).c_str());

	const IVS_UINT32 uiElemMaxNum = 130;
	IVS_CHAR chTemp[uiElemMaxNum];

	//主设备编码
	(void)xml.AddElem("DevCode");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp);

	//主设备设备名称
	(void)xml.AddElem("DevName");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp);

	//主设备协议类型
	(void)xml.AddElem("DevProtocolType");
	IVS_CHAR cDevProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pDevList[uiNum].stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pDevList[uiNum].stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevProtocolType);

	//认证类型
	(void)xml.AddElem("DevLoginType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDevList[uiNum].stDeviceCfg.iLoginType).c_str());

	//主设备提供商类型
	(void)xml.AddElem("DevVendorType");
	IVS_CHAR cDevVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevVendorType);

	//主设备形态类型    =======================================
	IVS_INT32 iTemp = static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.stDeviceInfo.uiType);
	(void)xml.AddElem("DevFormType");
	(void)xml.SetElemValue(CDevEnum::GetDevType(iTemp));

	//主设备型号
	(void)xml.AddElem("DevModelType");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp);

	//设备告警使能   =============================================
	(void)xml.AddElem("DevAlarmEnable");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pDevList[uiNum].stDeviceCfg.bEnableAlarm).c_str());

	//视频制式  =======================================
	if (0 != pDevList[uiNum].stDeviceCfg.uiVideoFormat)
	{
		iTemp = static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.uiVideoFormat);
		(void)xml.AddElem("VideoFormat");
		(void)xml.SetElemValue(CDevEnum::GetVideoFormatType(iTemp));
	}


	//是否直连优先
	iTemp = static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.uiDirectConnectFirst);
	(void)xml.AddElem("DirectConnectFirst");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());

	//直连最大数目
	iTemp = static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.uiMaxDirectConnectNum);
	(void)xml.AddElem("MaxDirectConnectNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());

	//设备所属NVR编码
	(void)xml.AddElem("NVRCode");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_DEV_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp);

	//设备所属TAS编码
	(void)xml.AddElem("DCGCode");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cTasCode, IVS_TAS_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cTasCode, IVS_DEV_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	(void)xml.SetElemValue(chTemp);   

	//TAS所在IP地址
	(void)xml.AddElem("DCGIP");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	(void)CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stTasIP.cIP, IVS_IP_LEN);
	(void)xml.SetElemValue(chTemp); 

	//IP类型
	iTemp = static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.stDeviceInfo.stIP.uiIPType);
	(void)xml.AddElem("DevIPType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iTemp).c_str());

	//设备所在IP地址
	(void)xml.AddElem("DevIP");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp); 

	//设备登录端口
	(void)xml.AddElem("DevPort");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevList[uiNum].stDeviceCfg.stDeviceInfo.uiPort)).c_str());

	//设备SN号
	(void)xml.AddElem("DevSN");
	IVS_CHAR cDevSN[IVS_DEV_SERIALNO_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevSN);

	//设备登录用户名
	(void)xml.AddElem("DevUser");
	eSDK_MEMSET(chTemp, 0, uiElemMaxNum);
	if (!CToolsHelp::Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cDevUser, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiElemMaxNum, pDevList[uiNum].stDeviceCfg.cDevUser, IVS_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(chTemp);

	//设备登录密码
	(void)xml.AddElem("DevPWD");
	(void)xml.SetElemValue(strDevPWD.c_str());      

	//设备向平台密码
	(void)xml.AddElem("DevRegPWD");
	(void)xml.SetElemValue(strDevRegPWD.c_str());

	(void)xml.AddElem("BlanceSupport");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pDevList[uiNum].stDeviceCfg.bEnableSchedule).c_str());

	xml.OutOfElem();

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 新增摄像机（密钥交换情况下）单条解析XML
IVS_INT32 CDeviceMgrXMLProcess::AddDevicePileParseXML(IVS_DEVICE_OPER_RESULT_LIST* pResultList, IVS_UINT32 uiNum, CXml &xml)
{
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	if(!xml.FindElemEx("Content/DevList"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Add DeviceInfo PraseXML fail", "FALSE == xml.FindElemEx('Content/Dev_List')");
		return IVS_OPERATE_MEMORY_ERROR;
	}       

	if (!xml.FindElem("DevInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Device ParseXml fail", "!Rspxml.FindElem(DevInfo)");
		return IVS_XML_INVALID;
	}

	/*IVS_DEVICE_CODE_INFO stDevInfo ;*/
	const IVS_CHAR* szElemValue = NULL;
	IVS_INT32 iTemp = 0;

	(void)xml.IntoElem();

	IVS_DEVICE_OPER_RESULT &stDevInfo = pResultList->stCodeInfo[uiNum];
	eSDK_MEMSET(&stDevInfo, 0 ,sizeof(IVS_DEVICE_OPER_RESULT));           		

	// Sequence
	GET_ELEM_VALUE_NUM("Sequence", szElemValue, iTemp, xml);//lint !e838

	stDevInfo.uiSequence = static_cast<IVS_UINT32>(iTemp);
	// DevCode
	GET_ELEM_VALUE_CHAR("DevCode", szElemValue, stDevInfo.cCode, IVS_DEV_CODE_LEN, xml);

	// NvrCode
	GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, stDevInfo.cNvrCode, IVS_NVR_CODE_LEN, xml);

	// Result
	GET_ELEM_VALUE_NUM("Result", szElemValue, iTemp, xml);//lint !e838
	stDevInfo.uiResult = static_cast<IVS_UINT32>(iTemp);     

	xml.OutOfElem();

	return IVS_SUCCEED;
}

IVS_INT32 CDeviceMgrXMLProcess::RefreshCameraListXML(IVS_INT32 iSessionID, CXml &xml)
{
    IVS_DEBUG_TRACE("");

    (void)xml.AddElem("Content");

    (void)xml.AddChildElem("LoginInfo");
    (void)xml.IntoElem();
    (void)xml.AddElem("LoginID");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(iSessionID)).c_str());//lint !e64 匹配

    return IVS_SUCCEED;
}

// 获取Obvif设备前端信息列表解析XML
IVS_INT32 CDeviceMgrXMLProcess::ParseCameraEncodeCapabilities(CXml& xml,
	IVS_STREAM_INFO_LIST* pStreameList)
{
	CHECK_POINTER(pStreameList,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	const char* szElemValue = NULL;

	if (!xml.FindElemEx("Content/StreamInfoList"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Parse Camera Encode Capabilities fail", "!xml.FindElemEx(Content/NVRList)");
		return iRet;
	}

	IVS_INT32 iRealNum = 0;
	bool bRet = false;
	if (xml.FindElem("StreamInfo"))
	{
		do 
		{
			IVS_UINT32 ulStreamInfoSize = sizeof(IVS_ST_STREAM_INFO);
			IVS_ST_STREAM_INFO* pStreamInfo = IVS_NEW(pStreamInfo, 1);
			if (NULL == pStreamInfo)
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse Camera Encode Capabilities PraseXML fail", "new IVS_ST_STREAM_INFO failed");
				return IVS_OPERATE_MEMORY_ERROR;
			}

			eSDK_MEMSET(pStreamInfo,0, ulStreamInfoSize);

			(void)xml.IntoElem();

			GET_ELEM_VALUE_CHAR("StreamType",szElemValue,  pStreamInfo->cStreamType,IVS_FILE_NAME_LEN,xml);
			if (xml.FindElem("EncodeModeList"))
			{
				(void)xml.IntoElem();
				IVS_INT32 iRealEncodeModeNum = 0;
				if (xml.FindElem("EncodeModeInfo"))
				{
					do 
					{
						IVS_UINT32 ulEncodeModeInfoSize = sizeof(IVS_ENCODEMODE_INFO);
						IVS_ENCODEMODE_INFO* pEncodeModeInfo = IVS_NEW(pEncodeModeInfo, 1);
						if (NULL == pEncodeModeInfo)
						{
							IVS_DELETE(pStreamInfo,MUILI);
							BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Parse Camera Encode Capabilities PraseXML fail", "new IVS_ENCODEMODE_INFO failed");
							return IVS_OPERATE_MEMORY_ERROR;
						}
						eSDK_MEMSET(pEncodeModeInfo, 0, ulEncodeModeInfoSize);
						(void)xml.IntoElem();

						GET_ELEM_VALUE_CHAR("EncodeMode",szElemValue,  pEncodeModeInfo->cEncodeMode,IVS_DEV_MODEL_LEN,xml);
						GET_ELEM_VALUE_CHAR("ProfileList",szElemValue,  pEncodeModeInfo->cProfileList,IVS_MAX_NAME_LIST_LEN,xml);
						GET_ELEM_VALUE_CHAR("BitRateList",szElemValue,  pEncodeModeInfo->cBitRateList,IVS_MAX_NAME_LIST_LEN,xml);

						if (xml.FindElem("IFrameInfo"))
						{
							(void)xml.IntoElem();
							if (xml.FindElem("PType"))
							{
								(void)xml.IntoElem();
								GET_ELEM_VALUE_NUM("MinIFrameInterval",szElemValue,  pEncodeModeInfo->stIFrameInfo.iPMinIFrameInterval,xml);
								GET_ELEM_VALUE_NUM("MaxIFrameInterval",szElemValue,  pEncodeModeInfo->stIFrameInfo.iPMaxIFrameInterval,xml);
								xml.OutOfElem();
							}
							if (xml.FindElem("NType"))
							{
								(void)xml.IntoElem();
								GET_ELEM_VALUE_NUM("MinIFrameInterval",szElemValue,  pEncodeModeInfo->stIFrameInfo.iNMinIFrameInterval,xml);
								GET_ELEM_VALUE_NUM("MaxIFrameInterval",szElemValue,  pEncodeModeInfo->stIFrameInfo.iNMaxIFrameInterval,xml);
								xml.OutOfElem();
							}
							xml.OutOfElem();
						}

						if (xml.FindElem("ResolutionList"))
						{
							(void)xml.IntoElem();
							IVS_INT32 iRealResolutionNum = 0;

							do 
							{
								IVS_RESOLUTION_INFO sResolutionInfo;
								eSDK_MEMSET(&sResolutionInfo, 0, sizeof(IVS_RESOLUTION_INFO));

								(void)xml.IntoElem();
								GET_ELEM_VALUE_CHAR("Resolution",szElemValue,  sResolutionInfo.cResolution,IVS_FILE_NAME_LEN,xml);
								if (xml.FindElem("PType"))
								{
									(void)xml.IntoElem();
									GET_ELEM_VALUE_CHAR("FrameList",szElemValue,  sResolutionInfo.cPFrameList,IVS_MAX_NAME_LIST_LEN,xml);
									xml.OutOfElem();
								}
								if (xml.FindElem("NType"))
								{
									(void)xml.IntoElem();
									GET_ELEM_VALUE_CHAR("FrameList",szElemValue,  sResolutionInfo.cNFrameList,IVS_MAX_NAME_LIST_LEN,xml);
									xml.OutOfElem();
								}
								xml.OutOfElem();

								if (iRealResolutionNum>IVS_MAX_RESOLUTION_LIST_NUM-1)
								{
									//超过最大个数限制放弃
									break;
								}
								bRet = CToolsHelp::Memcpy(&(pEncodeModeInfo->stResolution[iRealResolutionNum]), sizeof(IVS_RESOLUTION_INFO), &sResolutionInfo, sizeof(IVS_RESOLUTION_INFO));
								if(false == bRet)
								{
									BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
									IVS_DELETE(pEncodeModeInfo,MUILI);
									IVS_DELETE(pStreamInfo,MUILI);
									return IVS_ALLOC_MEMORY_ERROR;
								}

								iRealResolutionNum++;
							} while (xml.NextElem());
							pEncodeModeInfo->iRealNum = iRealResolutionNum;
							xml.OutOfElem();
						}

						xml.OutOfElem();

						if (iRealEncodeModeNum>IVS_MAX_ENCODE_LIST_NUM-1)
						{
							//超过最大个数限制放弃
							IVS_DELETE(pEncodeModeInfo,MUILI);
							break;
						}

						bRet = CToolsHelp::Memcpy(&(pStreamInfo->stEncodeMode[iRealEncodeModeNum]), sizeof(IVS_ENCODEMODE_INFO), pEncodeModeInfo, sizeof(IVS_ENCODEMODE_INFO));
						IVS_DELETE(pEncodeModeInfo,MUILI);
						if(false == bRet)
						{
							BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
							IVS_DELETE(pStreamInfo,MUILI);
							return IVS_ALLOC_MEMORY_ERROR;
						}

						iRealEncodeModeNum++;
					} while (xml.NextElem());
					pStreamInfo->iRealNum = iRealEncodeModeNum;
				}
				xml.OutOfElem();
			}

			xml.OutOfElem();
			if (iRealNum>IVS_MAX_STREAM_LIST_NUM-1)
			{
				//超过最大个数限制放弃
				IVS_DELETE(pStreamInfo,MUILI);
				break;
			}
			bRet = CToolsHelp::Memcpy(&(pStreameList->stStreamInfo[iRealNum]), sizeof(IVS_ST_STREAM_INFO), pStreamInfo, sizeof(IVS_ST_STREAM_INFO));
			IVS_DELETE(pStreamInfo,MUILI);

			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}

			iRealNum++;
		} while (xml.NextElem());
		pStreameList->iRealNum = iRealNum;
	}

	// 添加尺寸校验
	return IVS_SUCCEED;
}


//构造查询单台VCN的视频质量诊断计划请求xml
IVS_INT32 CDeviceMgrXMLProcess::GetVideoDiagnoseGetXML(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	CXml& xml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	IVS_CHAR cTmpDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cTmpDomainCode, IVS_DOMAIN_CODE_LEN + 1, pDomainCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetVideoDiagnoseGetXML", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTmpDomainCode);

	(void)xml.AddElem("CameraID");
	IVS_CHAR cTmpCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Strncpy(cTmpCameraCode, IVS_DEV_CODE_LEN + 1, pCameraCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetVideoDiagnoseGetXML", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTmpCameraCode);

	return IVS_SUCCEED;
}


//组装获取主设备调度请求XML
IVS_INT32 CDeviceMgrXMLProcess::GetClusterIPCScheduleReqXML(const IVS_CHAR* pDomainCode,
	const std::string strClusterCode, 
	std::list<IVS_DEVICE_OPER_INFO>& deviceInfoList, 
	CXml& xml)
{
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);


	(void)xml.AddElem("ClusterBody");    
	(void)xml.AddChildElem("ClusterCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strClusterCode.c_str());

	(void)xml.AddElem("TaskSchedule"); 
	(void)xml.AddChildElem("IsNeedResponse");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("true");
	xml.OutOfElem();

	(void)xml.AddElem("CommandExecute");
	(void)xml.AddChildElem("IsNeedExecute");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("false");
	(void)xml.AddElem("WaitCmdResponse");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("false");
	xml.OutOfElem();

	(void)xml.AddElem("TaskList");
	std::list<IVS_DEVICE_OPER_INFO>::iterator itor = deviceInfoList.begin();
	for (unsigned int i = 0; i < deviceInfoList.size() && itor != deviceInfoList.end(); i++, itor++)
	{
		if (deviceInfoList.begin() == itor)
		{
			(void)xml.AddChildElem("Task");
			(void)xml.IntoElem();
		}
		else
		{
			(void)xml.AddElem("Task");
		}

		//主设备ID;
		(void)xml.AddChildElem("ID");   
		(void)xml.IntoElem();

		//如果CameraCode为空，需要生成模拟编码(如IVS_1)，否则发送多个设备的调度时，SMU调度响应只有一个;
		std::string strCodeNum = "IVS_" + CToolsHelp::Int2Str(i);//lint !e713
		if (0 == strlen(itor->stDeviceCfg.stDeviceInfo.cCode))
		{
			(void)memcpy_s(itor->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN, strCodeNum.c_str(), IVS_DEV_CODE_LEN);
		}
		(void)xml.SetElemValue(itor->stDeviceCfg.stDeviceInfo.cCode);

		//主设备视频通道数;
		(void)xml.AddElem("ChanCnt");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(itor->stDeviceCfg.stDeviceChannel.uiMaxVideoNum)).c_str());		

		// 主设备型号
		(void)xml.AddElem("DevModelType");   
		(void)xml.IntoElem();
		(void)xml.SetElemValue(itor->stDeviceCfg.stDeviceInfo.cModel);	

		//主设备协议类型;
		(void)xml.AddElem("DevProtocolType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(itor->stDeviceCfg.cProtocolType);

		//主设备形态类型;
		(void)xml.AddElem("DevFormType");   
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CDevEnum::GetDevType(static_cast<IVS_INT32>(itor->stDeviceCfg.stDeviceInfo.uiType)));
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//解析获取主设备调度响应
IVS_INT32 CDeviceMgrXMLProcess::ParseGetScheduleRspXML(CXml& xml,
	std::list<IVS_DEVICE_OPER_INFO>& clusterdeviceList,
	std::list<IVS_DEVICE_OPER_INFO>& lFailedList)
{
	IVS_INT32 iRet = IVS_FAIL;
	const char* szElemValue = NULL;

	unsigned int uiLen = 0;
	const char* Rspxml = xml.GetXMLStream(uiLen);
	if (NULL != Rspxml)
	{
		BP_DBG_LOG("", "Get Schedule Rsp XML[%s]", Rspxml);
	}

	if (!xml.FindElemEx("Content/ClusterBody/ScheduleResultList"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Parse Get Schedule Rsp fail, ", "can not find ScheduleResultList.");
		return iRet;
	}

	IVS_DEVICE_OPER_INFO stDeviceInfo;
	eSDK_MEMSET(&stDeviceInfo, 0, sizeof(stDeviceInfo));
	std::string strNvrCode;	

	do 
	{
		if (!xml.FindElemValue("Task"))
		{
			break;
		}

		(void)xml.IntoElem();
		unsigned int uiResult = 0;
		GET_ELEM_VALUE_NUM_FOR_UINT("Result", szElemValue, uiResult, xml);//lint !e838

		eSDK_MEMSET(stDeviceInfo.stDeviceCfg.stDeviceInfo.cCode, 0x0, sizeof(stDeviceInfo.stDeviceCfg.stDeviceInfo.cCode));
		GET_ELEM_VALUE_CHAR("ID", szElemValue, stDeviceInfo.stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN, xml);

		eSDK_MEMSET(stDeviceInfo.stDeviceCfg.cNvrCode, 0x0, sizeof(stDeviceInfo.stDeviceCfg.cNvrCode));
		GET_ELEM_VALUE_CHAR("NodeID", szElemValue, stDeviceInfo.stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN, xml);
		strNvrCode = stDeviceInfo.stDeviceCfg.cNvrCode;

		if (0 != uiResult)
		{
			//将对应Code和DomaiCode的从列表中删除
			std::list<IVS_DEVICE_OPER_INFO>::iterator listItor = clusterdeviceList.begin();
			for (; listItor != clusterdeviceList.end(); listItor++)
			{				
				if (0 == strncmp(stDeviceInfo.stDeviceCfg.stDeviceInfo.cCode, listItor->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
				{
					clusterdeviceList.erase(listItor++);

                    stDeviceInfo.uiSequence = listItor->uiSequence;
				}
				
			}
            lFailedList.push_back(stDeviceInfo);
		}
		else
		{
			//将对应Code和DomaiCode的从列表中删除
			std::list<IVS_DEVICE_OPER_INFO>::iterator listItor = clusterdeviceList.begin();
			for (; listItor != clusterdeviceList.end(); listItor++)
			{				
				if (0 == strncmp(stDeviceInfo.stDeviceCfg.stDeviceInfo.cCode, listItor->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
				{
					(void)memcpy_s(listItor->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN, strNvrCode.c_str(), IVS_NVR_CODE_LEN);
				}
			}
		}	

		xml.OutOfElem();

	}while(xml.NextElem());

	return IVS_SUCCEED;
}

//组装新增影子设备请求XML
IVS_INT32 CDeviceMgrXMLProcess::GetAddShadowDevReqXML(const IVS_CHAR* pDomainCode, IVS_SHADOW_IPC_INFO& ShadowDevInfo, CXml& xml)
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	eSDK_STRNCPY(cDomainCode, sizeof(cDomainCode), pDomainCode, IVS_DOMAIN_CODE_LEN);
	(void)xml.SetElemValue(cDomainCode);

	(void)xml.AddElem("DevInfo");    
	(void)xml.AddChildElem("DevCode");
	(void)xml.IntoElem();
	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	eSDK_STRNCPY(cDevCode, sizeof(cDevCode), ShadowDevInfo.cDevCode, IVS_DEV_CODE_LEN);
	(void)xml.SetElemValue(cDevCode);

	(void)xml.AddElem("DevName");
	IVS_CHAR cDevName[IVS_DEVICE_NAME_LEN + 1] = {0};
	eSDK_STRNCPY(cDevName, sizeof(cDevName), ShadowDevInfo.cDevName, IVS_DEV_CODE_LEN);
	(void)xml.SetElemValue(cDevName);

	(void)xml.AddElem("NVRCode");
	IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
	eSDK_STRNCPY(cNVRCode, sizeof(cNVRCode), ShadowDevInfo.cNVRCode, IVS_NVR_CODE_LEN);
	(void)xml.SetElemValue(cNVRCode);

	(void)xml.AddElem("DevProtocolType");
	IVS_CHAR cDevProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN + 1] = {0};
	eSDK_STRNCPY(cDevProtocolType, sizeof(cDevProtocolType), ShadowDevInfo.cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN);
	(void)xml.SetElemValue(cDevProtocolType);

	(void)xml.AddElem("DevGroupID");
	IVS_CHAR cDevGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
	eSDK_STRNCPY(cDevGroupID, sizeof(cDevGroupID), ShadowDevInfo.cDevGroupID, IVS_DEVICE_GROUP_LEN);
	(void)xml.SetElemValue(cDevGroupID);

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet= CXmlProcess::ParseDevCode(ShadowDevInfo.cOrigDevCode, szCameraCode, szDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "ParseDevCode fail, Bigcode is : %s", "ShadowDevInfo.cDevCode");
		return IVS_PARA_INVALID;
	}

	(void)xml.AddElem("OrigDevDomain");
	(void)xml.SetElemValue(szDomainCode);

	(void)xml.AddElem("OrigDevCode");
	(void)xml.SetElemValue(szCameraCode);

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}//lint !e1764

//组装影子设备手动同步请求XML
IVS_INT32 CDeviceMgrXMLProcess::GetShadowDevSyncNotifyReqXML(const IVS_SHADOW_IPC_LIST* pDevList, std::string& strDomainCode, CXml& xmlReq)
{
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.IntoElem();

	(void)xmlReq.AddChildElem("DevList");
	(void)xmlReq.IntoElem();

	IVS_CHAR chTemp[IVS_DEV_CODE_LEN + 1];
	for(IVS_UINT32 uiIndex = 0 ; uiIndex < pDevList->uiNum; uiIndex++)
	{ 
		if(uiIndex > 0)
		{
			(void)xmlReq.AddElem("DevInfo");
		}
		else
		{
			(void)xmlReq.AddChildElem("DevInfo");
			(void)xmlReq.IntoElem();
		}

		// 设备编码
		(void)xmlReq.AddChildElem("DevCode");
		(void)xmlReq.IntoElem();

		eSDK_MEMSET(chTemp, 0, sizeof(chTemp));
		if (!CToolsHelp::Strncpy(chTemp, IVS_DEV_CODE_LEN + 1, pDevList->stCodeList[uiIndex].cDevCode, IVS_DEV_CODE_LEN))//lint !e662  pDevList大小由调用者保证
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Strncpy(chTemp, MAXELEMLEN, pDevCodeList[uiIndex].cDevCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		(void)CXmlProcess::ParseDevCode(chTemp, szCameraCode, szDomainCode);
		(void)xmlReq.SetElemValue(szCameraCode);

		// 记录下第一个设备的预编码，用于判断后面的是不是同一域
		if (0 == uiIndex)
		{
			strDomainCode = szDomainCode;//lint !e64 匹配
		}
		if (strDomainCode.compare(szDomainCode))
		{
			BP_RUN_LOG_ERR(IVS_SDK_ERR_DOMAINCODE_ERR, "DomainCode not unite", "NA");
			return IVS_SDK_ERR_DOMAINCODE_ERR;
		}

		xmlReq.OutOfElem();  
	}

	xmlReq.OutOfElem();
	xmlReq.GetRootPos();

	return IVS_SUCCEED;
}

