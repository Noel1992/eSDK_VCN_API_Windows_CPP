/********************************************************************
filename    :    ivs_memory.h
author      :    yWX150774
created     :    2012/12/11
description :    CBB 模块中用到的内存管理宏
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/12/11 初始版本
*********************************************************************/

#ifndef CBB_BASE_MEMORY_H
#define CBB_BASE_MEMORY_H 

#ifdef WIN32
#include "securec_win.h"
#else
#include "securec.h"
#endif


#define CBB_NEW(var, Type)               \
    do                                   \
    {                                    \
        try                              \
        {                                \
            (var) = new Type;            \
        }                                \
        catch (...)                      \
        {                                \
            (var) = NULL;                \
        }                                \
    } while (false)

#define CBB_DELETE(var)                  \
    do                                   \
    {                                    \
        if(NULL == var)                  \
        {                                \
            break;                       \
        }                                \
        try                              \
        {                                \
            delete (var);                \
        }                                \
        catch (...)                      \
        {                                \
                                         \
        }                                \
        (var) = NULL;                    \
    } while (false)

#define CBB_NEW_A(var, Type, size)       \
    do                                   \
    {                                    \
        /*lint -e506 -e714 -e774 -e587 -e944*/    \
        if (size < 1) {break;}\
        /*lint +e506 +e714 +e774 +e587 +e944*/    \
        try                              \
        {                                \
            (var) = new Type[size];      \
        }                                \
        catch (...)                      \
        {                                \
            (var) = NULL;                \
        }                                \
    } while (false)

#define CBB_DELETE_A(var)                \
    do                                   \
    {                                    \
        if(NULL == var)                  \
        {                                \
            break;                       \
        }                                \
        try                              \
        {                                \
            delete [] (var);             \
        }                                \
        catch (...)                      \
        {                                \
                                         \
        }                                \
        (var) = NULL;                    \
    } while (false)

// 判断指针是否为空 
#define CBB_CHECK_POINTER( pPoint, retValue )   \
    if ( NULL == ( pPoint ) )                   \
    {                                           \
        return retValue;                        \
    }

// 判断指针是否为空,无返回值,可能会提示多了分号
#define CBB_CHECK_POINTER_VOID( pPoint )        \
    if ( NULL == ( pPoint ) )                   \
    {                                           \
        return ;                                \
    }

#ifdef WIN32
#define STRTOK  strtok_s
#else
#define STRTOK  strtok_r
#endif


/**************************************************************************
* name       : IVS_MEMSET
* description: 内存初始化函数
* input      : 1)dest目的地址；2)初始化字符；3)初始化长度
* output     : 初始化后的内存
* remark     : mem**set win32下暂无可替换安全函数，为保证win32和linux平台调用一致性，这里封装为3个参数
**************************************************************************/
#define IVS_MEMSET(dest, c, destMax)\
	do {\
	memset_s(dest, (destMax), c, (destMax)); \
	} while (0)

/**************************************************************************
* name       : IVS_MEMCPY
* description: 内存拷贝
* input      : 1)dest目的地址；2)目的内存大小；3)源地址；4)预拷贝长度
* output     : dest拷贝结果
* remark     : 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define IVS_MEMCPY(dest, destMax, src, count)\
    do {\
        /*lint -e506 -e714 -e774 -e587 -e944*/  \
        const size_t copylen = ((count)>(destMax)?(destMax):(count)); \
        memcpy_s(dest, destMax, src, copylen); \
        /*lint +e506 +e714 +e774 +e587 +e944*/  \
    } while (0)

/**************************************************************************
* name       : IVS_MEMMOVE
* description: 内存拷贝
* input      : 1)dest目的地址；2)目的内存大小；3)源地址；4)预拷贝长度
* output     : dest拷贝结果
* remark     : 比mem**move增加目的长度参数
               memmove_s(void* dest, size_t destMax, const void* src, size_t count)
**************************************************************************/
#define IVS_MEMMOVE memmove_s

/**************************************************************************
* name       : IVS_STRNCPY
* description: 字符串拷贝
* input      : 1)目的字符串；2)目的长度；3)源字串；4)预拷贝长度
* output     : dest拷贝结果
* remark     : 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define IVS_STRNCPY(dest, destMax, src, count)\
    do {\
        /*lint -e506 -e714 -e774 -e587 -e944*/  \
        if (0 < (destMax)) \
        { \
            const size_t copylen = ((count)>=(destMax)?((destMax)-1):(count)); \
            strncpy_s(dest, (destMax), src, copylen); \
        } \
        /*lint +e506 +e714 +e774 +e587 +e944*/  \
    } while (0)

/**************************************************************************
* name       : IVS_STRNCAT
* description: 字符串串接
* input      : 1)目的字符串；2)目的长度；3)源字串；4)串接长度
* output     : dest拷贝结果
* remark     : 比str*n*cat增加目的长度参数
               strncat_s(char* strDest, size_t destMax, const char* strSrc, size_t count)
**************************************************************************/
#define IVS_STRNCAT strncat_s

/**************************************************************************
* name       : IVS_SNPRINTF HW_VSNPRINTF HW_SWPRINTF
* description: 格式化输出
* input      : 1)目的字符串；2)目的长度；3)格式化长度；4)格式化字符
* output     : dest格式化后的字符串
* remark     : 对比 sn*p*rintf 增加 “2)目的长度” 参数
               snprintf_s(char* strDest, size_t destMax, size_t count, const char* format, ...)
**************************************************************************/
#ifdef WIN32
#ifdef __cplusplus
extern "C"
{
#endif
    __declspec(dllexport) int IVS_SNPRINTF(char* strDest, size_t destMax, size_t count, const char* format, ...);
    //__declspec(dllexport) int IVS_SWPRINTF(char* strDest, size_t destMax, const char* format, ...);
#ifdef __cplusplus
}
#endif
#else
extern "C" int IVS_SNPRINTF(char* strDest, size_t destMax, size_t count, const char* format, ...);
//extern "C" int IVS_SWPRINTF(char* strDest, size_t destMax, const char* format, ...);
#endif

#define IVS_VSNPRINTF vsnprintf_s

/**************************************************************************
* name       : IVS_GETS      
* description: 标准输入流输入
* input      : 1)目的字符串；2)目的长度
* output     : 输入结果
* remark     : gets_s(char* buffer, size_t destMax)
**************************************************************************/
#define IVS_GETS gets_s

/**************************************************************************
* name       : IVS_SSCANF
* description: 标准输入流输入
* input      : 1)目的字符串；2)格式化参数
* output     : 输入结果
* remark     :     sscanf_s(const char* buffer, const char* format, ...)
**************************************************************************/
#define IVS_SSCANF sscanf_s 

/**************************************************************************
* name       : IVS_STRCPY
* description: 字符串拷贝
* input      : 1)目的字符串；2)目标字符串长度；3）源字符串
* output     : 拷贝结果
* remark     :     strcpy_s(char* strDest, size_t destMax, const char* strSrc);
**************************************************************************/
#define IVS_STRCPY strcpy_s 

#endif // CBB_BASE_MEMORY_H
