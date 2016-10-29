#ifndef _IVS_BP_STORE_MODE_H
#define _IVS_BP_STORE_MODE_H


#define BP_STORE_MODE_NONE      0 //无效存储模式
#define BP_STORE_MODE_DISK      1 //存储模式: 散盘
#define BP_STORE_MODE_SAFEVIDEO 2 //存储模式: SafeVideo
#define BP_STORE_MODE_SIMP_SAFEVIDEO 3 //存储模式: SafeVideo 小于6块盘

//存储模式键值，/ivs_log目录任何模块都有全新读取。
#define BP_STORE_MODE_KEY_NAME "/share/.ivs_key"

#endif //_IVS_BP_STORE_MODE_H

