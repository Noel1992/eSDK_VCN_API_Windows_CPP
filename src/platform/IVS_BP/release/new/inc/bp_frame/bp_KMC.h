/********************************************************************
filename    :    DumpProcess.h 
author      :    w00210470
created     :    2012/05/07
description :	 生成CoreDump文件 
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/05/07 初始版本
*********************************************************************/
#ifndef  BP_KMC_H
#define  BP_KMC_H

#include "bp_def.h"

extern "C" int EXPORT_DLL InitKMC();
extern "C" void EXPORT_DLL UninitKMC();

extern "C" int EXPORT_DLL InitKMC_SDK(char* keystore,char* keystorebackup,char* configcbb,char* configcbbbackup);

#endif  // end of  BP_KMC_H