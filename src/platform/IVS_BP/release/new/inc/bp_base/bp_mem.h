#ifndef _IVS_BP_MEM_H
#define _IVS_BP_MEM_H

#include "ivs_memory.h"

enum BP_MEM_OBJ_TYPE
{
    BP_MEM_NONE     = 0, //预留
    BP_MEM_LOG_BUF  = 1, //日志缓冲区
    BP_MEM_DBCONN   = 2, //数据库连接
    MEMID_XMLBUFF   = 3, //XML缓存
    MEMID_CmdID     = 4, //命令ID
    MEMID_MsgBlock  = 5, //消息块

    //0 ~ 100给BP内部使用, 101 ~ 200给业务模块用, 各业务模块可以定义相同的值
    MEMID_BP_MAX = 100,

    //请在此之前增加新类型.
    // 设定内存分配的最大ID号，暂定200
    BP_MEM_ID_MAX = 200
};

#ifdef WIN32

#ifdef BASE_DLL
extern __declspec(dllexport) int g_bp_memstat[];
#else
extern __declspec(dllimport) int g_bp_memstat[];
#endif

#else
extern int g_bp_memstat[];
#endif

/**************************************************************************
* name       : BP_NEW_P
* description: 根据类型创建对象，并在类型ID数组中记录。
* input      : ObjectID 对象ID，用户自定义。
* output     : NA
* remark     : 对象ID，暂定最大200
**************************************************************************/
#define BP_NEW_P(ObjectID, ObjectPtr, Type) \
    do {\
        (ObjectPtr) = NULL;\
        /*lint -e845 -e944*/\
        if ((BP_MEM_ID_MAX < ObjectID) || (0 > ObjectID)){break;}\
        /*lint +e845 +e944*/\
        try {\
            (ObjectPtr) = new Type;\
            if(NULL != (ObjectPtr)){++g_bp_memstat[ObjectID];}\
        }\
        catch (...) {\
            (ObjectPtr) = NULL;\
        }\
    } while (0)

/**************************************************************************
* name       : BP_DEL_P
* description: 根据类型释放对象，并在类型ID数组中消除。
* input      : ObjectID 对象ID，用户自定义。
* output     : NA
* remark     : 对象ID，暂定最大200
**************************************************************************/
#define BP_DEL_P(ObjectID, ObjectPtr) \
    do {\
        if(NULL != ObjectPtr) \
        {  \
            delete (ObjectPtr);\
            (ObjectPtr) = NULL;\
            --g_bp_memstat[ObjectID];\
        }  \
    } while (0)

/**************************************************************************
* name       : BP_NEW_A
* description: 根据类型创建对象数组，并在类型ID数组中记录。
* input      : ObjectID 对象ID，用户自定义。
* output     : NA
* remark     : 对象ID，暂定最大200
**************************************************************************/
#define BP_NEW_A(ObjectID, ObjectPtr, Type, Size)\
    do {\
        /*lint -e506 -e714 -e774 -e587 -e944*/  \
        if ((BP_MEM_ID_MAX < ObjectID) || (0 > ObjectID) || (Size < 1)) {break;}\
        /*lint +e506 +e714 +e774 +e587 +e944*/  \
        try {\
            (ObjectPtr) = new Type[Size];\
            if(NULL != (ObjectPtr)){++g_bp_memstat[ObjectID];}\
        }\
        catch (...) {\
            (ObjectPtr) = NULL;\
        }\
    } while (0)

/**************************************************************************
* name       : BP_NEW_A
* description: 根据类型释放对象数组，并在类型ID数组中消除。
* input      : ObjectID 对象ID，用户自定义。
* output     : NA
* remark     : 对象ID，暂定最大200
**************************************************************************/
#define BP_DEL_A(ObjectID, ObjectPtr)\
    do {\
        if(NULL != ObjectPtr) \
        {  \
            delete [] (ObjectPtr);    \
            (ObjectPtr) = NULL;       \
            --g_bp_memstat[ObjectID]; \
        }  \
    } while (0)

// 定义一组内存分配的外覆盖宏定义，使用默认ID。因为pclint，不得不复制粘贴。罪过。
#define HW_NEW(var, Type) \
    do {\
    (var) = NULL;\
    try {\
    (var) = new Type;\
    if(NULL != (var)){++g_bp_memstat[BP_MEM_ID_MAX];}\
    }\
    catch (...) {\
    (var) = NULL;\
    }\
    } while (0)

#define HW_DELETE(var) \
    do {\
    if(NULL != var) \
    {\
    delete (var);\
    (var) = NULL;\
    --g_bp_memstat[BP_MEM_ID_MAX];\
    }\
    } while (0)

#define HW_NEW_A(var, type, size)\
    do {\
    /*lint -e506 -e714 -e774 -e587 -e944*/  \
    if (size < 1) {break;}\
    /*lint +e506 +e714 +e774 +e587 +e944*/  \
    try {\
        (var) = new type[size];\
        if(NULL != (var)){++g_bp_memstat[BP_MEM_ID_MAX];}\
    }\
    catch (...) {\
    (var) = NULL;\
    }\
    } while (0)

#define HW_DELETE_A(var)\
    do {\
    if(NULL != var) \
    {  \
    delete [] (var);    \
    (var) = NULL;       \
    --g_bp_memstat[BP_MEM_ID_MAX]; \
    }  \
    } while (0)
#ifndef SAFE_RELEASE_MB
#define SAFE_RELEASE_MB(var) \
    do {\
        if (NULL != var) \
        { \
            if (var->reference_count() > 1) \
            { \
                var->release(); \
            } \
            var->reset(); \
            HW_DELETE(var); \
        } \
    } while (0)
#endif
/**************************************************************************
* name       : BP_CheckMemLeak
* description: 检测分配内存是否泄露。
* input      : NA
* output     : 泄露的ID个数
* remark     : NA
**************************************************************************/
#ifdef WIN32
#ifdef __cplusplus
extern "C"
{
#endif
__declspec(dllexport) int BP_CheckMemLeak(void);
#ifdef __cplusplus
}
#endif
//extern "C" __declspec(dllexport) int BP_CheckMemLeak(void);
#else
extern "C" int BP_CheckMemLeak(void);
#endif

#define HW_MEMSET    IVS_MEMSET
#define HW_MEMCPY    IVS_MEMCPY
#define HW_MEMMOVE   IVS_MEMMOVE
#define HW_STRNCPY   IVS_STRNCPY
#define HW_STRNCAT   IVS_STRNCAT
#define HW_SNPRINTF  IVS_SNPRINTF 
#define HW_VSNPRINTF IVS_VSNPRINTF
//#define HW_SWPRINTF  IVS_SWPRINTF
#define HW_GETS      IVS_GETS
#define HW_SSCANF    IVS_SSCANF
#define HW_STRCPY    IVS_STRCPY

#endif //_IVS_BP_MEM_H
