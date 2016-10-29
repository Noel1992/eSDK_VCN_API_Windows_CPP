/********************************************************************
filename    :    ivs_memory.h
author      :    yWX150774
created     :    2012/12/11
description :    CBB ģ�����õ����ڴ�����
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/12/11 ��ʼ�汾
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

// �ж�ָ���Ƿ�Ϊ�� 
#define CBB_CHECK_POINTER( pPoint, retValue )   \
    if ( NULL == ( pPoint ) )                   \
    {                                           \
        return retValue;                        \
    }

// �ж�ָ���Ƿ�Ϊ��,�޷���ֵ,���ܻ���ʾ���˷ֺ�
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
* description: �ڴ��ʼ������
* input      : 1)destĿ�ĵ�ַ��2)��ʼ���ַ���3)��ʼ������
* output     : ��ʼ������ڴ�
* remark     : mem**set win32�����޿��滻��ȫ������Ϊ��֤win32��linuxƽ̨����һ���ԣ������װΪ3������
**************************************************************************/
#define IVS_MEMSET(dest, c, destMax)\
	do {\
	memset_s(dest, (destMax), c, (destMax)); \
	} while (0)

/**************************************************************************
* name       : IVS_MEMCPY
* description: �ڴ濽��
* input      : 1)destĿ�ĵ�ַ��2)Ŀ���ڴ��С��3)Դ��ַ��4)Ԥ��������
* output     : dest�������
* remark     : ��Ԥ��������count����Ŀ�Ļ����С����ض�
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
* description: �ڴ濽��
* input      : 1)destĿ�ĵ�ַ��2)Ŀ���ڴ��С��3)Դ��ַ��4)Ԥ��������
* output     : dest�������
* remark     : ��mem**move����Ŀ�ĳ��Ȳ���
               memmove_s(void* dest, size_t destMax, const void* src, size_t count)
**************************************************************************/
#define IVS_MEMMOVE memmove_s

/**************************************************************************
* name       : IVS_STRNCPY
* description: �ַ�������
* input      : 1)Ŀ���ַ�����2)Ŀ�ĳ��ȣ�3)Դ�ִ���4)Ԥ��������
* output     : dest�������
* remark     : ��Ԥ��������count����Ŀ�Ļ����С����ض�
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
* description: �ַ�������
* input      : 1)Ŀ���ַ�����2)Ŀ�ĳ��ȣ�3)Դ�ִ���4)���ӳ���
* output     : dest�������
* remark     : ��str*n*cat����Ŀ�ĳ��Ȳ���
               strncat_s(char* strDest, size_t destMax, const char* strSrc, size_t count)
**************************************************************************/
#define IVS_STRNCAT strncat_s

/**************************************************************************
* name       : IVS_SNPRINTF HW_VSNPRINTF HW_SWPRINTF
* description: ��ʽ�����
* input      : 1)Ŀ���ַ�����2)Ŀ�ĳ��ȣ�3)��ʽ�����ȣ�4)��ʽ���ַ�
* output     : dest��ʽ������ַ���
* remark     : �Ա� sn*p*rintf ���� ��2)Ŀ�ĳ��ȡ� ����
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
* description: ��׼����������
* input      : 1)Ŀ���ַ�����2)Ŀ�ĳ���
* output     : ������
* remark     : gets_s(char* buffer, size_t destMax)
**************************************************************************/
#define IVS_GETS gets_s

/**************************************************************************
* name       : IVS_SSCANF
* description: ��׼����������
* input      : 1)Ŀ���ַ�����2)��ʽ������
* output     : ������
* remark     :     sscanf_s(const char* buffer, const char* format, ...)
**************************************************************************/
#define IVS_SSCANF sscanf_s 

/**************************************************************************
* name       : IVS_STRCPY
* description: �ַ�������
* input      : 1)Ŀ���ַ�����2)Ŀ���ַ������ȣ�3��Դ�ַ���
* output     : �������
* remark     :     strcpy_s(char* strDest, size_t destMax, const char* strSrc);
**************************************************************************/
#define IVS_STRCPY strcpy_s 

#endif // CBB_BASE_MEMORY_H
