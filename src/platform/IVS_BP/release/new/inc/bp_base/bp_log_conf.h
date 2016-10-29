/********************************************************************
filename    :    bp_log_conf.h
author      :    w90004666
created     :    2012/9/18
description :	 ��־ģ��ӿ��ļ�
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/9/18 ��ʼ�汾
*********************************************************************/
#ifndef BP_LOG_CONF_H
#define BP_LOG_CONF_H

#include "bp_log.h"
#include "bp_def.h"

#include <limits.h>

#include "bp_key_config.h"
#include "bp_errno.h"


#ifdef WIN32
    #define BP_KEY_CONFIG_FILE_NAME ("\\key.xml")
#else
    #define BP_KEY_CONFIG_FILE_NAME ("/key.xml")
#endif




extern "C" bool EXPORT_DLL read_log_config(const char * strConfigFile, struct BP_LOG_CONF & log_conf);

extern "C" bool EXPORT_DLL read_log_sftp_conf(struct BP_LOG_SFTP_INFO & sftp_info);
extern "C" bool EXPORT_DLL write_log_sftp_conf(const struct BP_LOG_SFTP_INFO & sftp_info);


#endif // BP_LOG_CONF_H

