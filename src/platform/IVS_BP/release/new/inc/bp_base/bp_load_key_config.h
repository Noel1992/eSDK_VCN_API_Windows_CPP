/********************************************************************
filename    :    bp_load_key_connection_config.h
author      :    y00356758
created     :    2016/01/21
description :    读取BP密钥配置
copyright   :    Copyright (c) 2016-2020 Huawei Tech.Co.,Ltd
history     :    2016/01/21 初始版本
*********************************************************************/
#ifndef BP_LOAD_KEY_CONFIG_H
#define BP_LOAD_KEY_CONFIG_H

#include "ivs_xml.h"
#include "bp_key_config.h"
#include "bp_config_alarmer.h"
#include <string>
//#include "db_parameter.h"
#include "bp_def.h"
NAMESPACE_BP_BEGIN //lint !e129 !e10

class EXPORT_DLL CLoadBPKeyConfig : private CConfigAlarmer
{
public:
    CLoadBPKeyConfig ( void );
    virtual ~CLoadBPKeyConfig (void );

    //读取XML配置文件
    int LoadConfigFile(const std::string& strConfigPath);
    const CBPKeyConfig& GetBPKeyConfig ( void ) const;

public:
    //读取Database节点配置信息
    int LoadBPKeyConfig( const std::string& strConfigPath );

    bool GetSSLTcpServerKeyElement ( std::string & strSSLTcpServerKey );

    bool GetNSSPKeyElement ( std::string & strNSSPKey );

    bool GetSFTPKeyElement ( std::string & strSFTPKey );

    bool GetDBConectionKeyElement ( std::string & strDBConectionKey );

	

private:
    CBB::CXml                   m_BPKeyXml;    // 配置文件对象
    CBPKeyConfig   				m_BPKeyConfig; // 数据库连接参数实例
};

NAMESPACE_BP_END   //

#endif //BP_LOAD_KEY_CONFIG_H
