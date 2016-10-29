/********************************************************************
filename    :    bp_key_config.h
author      :    y00356758
created     :    2016/01/21
description :    读取BP所需密钥
copyright   :    Copyright (c) 2016-2020 Huawei Tech.Co.,Ltd
history     :    2016/01/21 初始版本
*********************************************************************/
#ifndef BP_KEY_CONFIG_H
#define BP_KEY_CONFIG_H

#include <string>
//#include "db_parameter.h"
#include "bp_namespace.h"
#include "bp_def.h"


NAMESPACE_BP_BEGIN //lint !e129 !e10

class EXPORT_DLL CBPKeyConfig 
{
public:
    CBPKeyConfig ()  {}
    ~CBPKeyConfig () {}

    const std::string& GetSSLTcpServerKey() const { return this->m_strSSLTcpServerKey; }
    void SetSSLTcpServerKey ( const std::string& strSSLTcpServerKey ) { this->m_strSSLTcpServerKey = strSSLTcpServerKey; }

	const std::string& GetNSSPKey() const { return this->m_strNSSPKey; }
	void SetNSSPKey ( const std::string& strNSSPKey ) { this->m_strNSSPKey = strNSSPKey; }

    const std::string& GetSFTPKey() const { return this->m_strSFTPKey; }
    void SetSFTPKey ( const std::string& strSFTPKey ) { this->m_strSFTPKey = strSFTPKey; }

    const std::string& GetDBConectionKey() const { return this->m_strDBConectionKey; }
    void SetDBConectionKey ( const std::string& strDBConectionKey ) { this->m_strDBConectionKey = strDBConectionKey; }


private:
    std::string m_strSSLTcpServerKey;    //用于ssl tcpserver的秘钥
    std::string m_strNSSPKey;           //网络接入nssp中加密协议秘钥
    std::string m_strSFTPKey;           //日志sftp备份需要的秘钥
    std::string m_strDBConectionKey;    //数据库连接秘钥
};

NAMESPACE_BP_END   //

#endif //BP_KEY_CONFIG_H

