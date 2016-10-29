/********************************************************************
filename    :    bp_load_key_connection_config.h
author      :    y00356758
created     :    2016/01/21
description :    ��ȡBP��Կ����
copyright   :    Copyright (c) 2016-2020 Huawei Tech.Co.,Ltd
history     :    2016/01/21 ��ʼ�汾
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

    //��ȡXML�����ļ�
    int LoadConfigFile(const std::string& strConfigPath);
    const CBPKeyConfig& GetBPKeyConfig ( void ) const;

public:
    //��ȡDatabase�ڵ�������Ϣ
    int LoadBPKeyConfig( const std::string& strConfigPath );

    bool GetSSLTcpServerKeyElement ( std::string & strSSLTcpServerKey );

    bool GetNSSPKeyElement ( std::string & strNSSPKey );

    bool GetSFTPKeyElement ( std::string & strSFTPKey );

    bool GetDBConectionKeyElement ( std::string & strDBConectionKey );

	

private:
    CBB::CXml                   m_BPKeyXml;    // �����ļ�����
    CBPKeyConfig   				m_BPKeyConfig; // ���ݿ����Ӳ���ʵ��
};

NAMESPACE_BP_END   //

#endif //BP_LOAD_KEY_CONFIG_H
