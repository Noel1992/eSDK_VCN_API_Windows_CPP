/********************************************************************
filename		: 	eSDK_Securec.h
author			:	zwx211831
created			:	2015/3/10
description	:	安全函数API接口;
copyright		:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history			:	2015/3/10 初始版本;
*********************************************************************/
#ifndef __ESDK_SECUREC_H__
#define __ESDK_SECUREC_H__

#if defined __GNUC__ || defined LINUX
#include "securec.h"
#endif

/**************************************************************************
* name			: eSDK_MEMCPY
* description	: 内存拷贝
* input			: 1)dest目的地址；2)目的内存大小；3)源地址；4)预拷贝长度
* output			: dest拷贝结果
* remark		: 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_MEMCPY(dest, destMax, src, count)								\
	do {																													\
	const size_t copylen = ((count)>(destMax)?(destMax):(count));		\
	memcpy_s(dest, destMax, src, copylen);												\
	} while (0)

/**************************************************************************
* name			: eSDK_WMEMCPY
* description	: 宽字节内存拷贝
* input			: 1)dest目的地址；2)目的内存大小；3)源地址；4)预拷贝长度
* output			: dest拷贝结果
* remark		: 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_WMEMCPY(dest, destMax, src, count)							\
	do {																													\
	const size_t copylen = ((count)>(destMax)?(destMax):(count));		\
	wmemcpy_s(dest, destMax, src, copylen);											\
	} while (0)

/**************************************************************************
* name			: eSDK_MEMMOVE
* description	: 内存移动
* input			: 1)dest目的地址；2)目的内存大小；3)源地址；4)预移动长度
* output			: dest移动结果
* remark		: 若预移动长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_MEMMOVE(dest, destMax, src, count)							\
	do {																													\
	const size_t movelen = ((count)>(destMax)?(destMax):(count));	\
	memmove_s(dest, destMax, src, movelen);											\
	} while (0)

/**************************************************************************
* name			: eSDK_WMEMMOVE
* description	: 内存移动
* input			: 1)dest目的地址；2)目的内存大小；3)源地址；4)预移动长度
* output			: dest移动结果
* remark		: 若预移动长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_WMEMMOVE(dest, destMax, src, count)						\
	do {																													\
	const size_t movelen = ((count)>(destMax)?(destMax):(count));	\
	wmemmove_s(dest, destMax, src, movelen);										\
	} while (0)

/**************************************************************************
* name			: eSDK_MEMSET
* description: 内存初始化函数
* input			: 1)dest目的地址；2)初始化字符；3)初始化长度
* output			: 初始化后的内存
* remark		: memset win32下暂无可替换安全函数，为保证win32和linux平台调用一致性，这里封装为3个参数
**************************************************************************/
#if defined WIN32 || defined _MSC_VER
#define eSDK_MEMSET  memset
#else
#define eSDK_MEMSET(dest, c, destMax)		\
	do {																		\
	memset_s(dest, (destMax), c, (destMax)); \
	} while (0)
#endif

/**************************************************************************
* name			: eSDK_STRCPY
* description	: 字符串拷贝
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)源缓冲区
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源缓冲区中的数据和结束符
							strcpy_s(char* strDest, size_t destMax, const char* strSrc)
**************************************************************************/
#define eSDK_STRCPY strcpy_s

/**************************************************************************
* name			: eSDK_WCSCPY
* description	: 宽字节字符串拷贝
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)源缓冲区
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源缓冲区中的数据和结束符
							wcscpy_s(wchar_t* strDest, size_t destMax, const wchar_t* strSrc)
**************************************************************************/
#define eSDK_WCSCPY wcscpy_s

/**************************************************************************
* name			: eSDK_STRNCPY
* description	: 字符串拷贝
* input			: 1)目的字符串；2)目的长度；3)源字串；4)预拷贝长度
* output			: dest拷贝结果
* remark		: 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_STRNCPY(dest, destMax, src, count)											\
    do {																																\
        if (0 < (destMax))																									\
        {																																\
            const size_t copylen = ((count)>=(destMax)?((destMax)-1):(count));\
            strncpy_s(dest, (destMax), src, copylen);													\
        }																																\
    } while (0)

/**************************************************************************
* name			: eSDK_WCSNCPY
* description	: 宽字节字符串拷贝
* input			: 1)目的字符串；2)目的长度；3)源字串；4)预拷贝长度
* output			: dest拷贝结果
* remark		: 若预拷贝长度count超过目的缓存大小，则截断
**************************************************************************/
#define eSDK_WCSNCPY(dest, destMax, src, count)										\
	do {																															\
	if (0 < (destMax))																									\
		{																															\
		const size_t copylen = ((count)>=(destMax)?((destMax)-1):(count));\
		wcsncpy_s(dest, (destMax), src, copylen);													\
		}																															\
	} while (0)

/**************************************************************************
* name			: eSDK_STRCAT
* description: 字符串串接
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)源缓冲区
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源字符串、目的字符串和结束符
						strcat_s(char* strDest, size_t destMax, const char* strSrc)
**************************************************************************/
#define eSDK_STRCAT strcat_s

/**************************************************************************
* name			: eSDK_WCSCAT
* description	: 宽字节字符串串接
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)源缓冲区
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源字符串、目的字符串和结束符
						wcscat_s(wchar_t* strDest, size_t destMax, const wchar_t* strSrc)
**************************************************************************/
#define eSDK_WCSCAT wcscat_s

/**************************************************************************
* name			: eSDK_STRNCAT
* description	: 字符串串接
* input			: 1)目的字符串；2)目的长度；3)源字串；4)串接长度
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源字符串、目的字符串和结束符
               strncat_s(char* strDest, size_t destMax, const char* strSrc, size_t count)
**************************************************************************/
#define eSDK_STRNCAT strncat_s

/**************************************************************************
* name			: eSDK_WCSNCAT
* description	: 宽字节字符串串接
* input			: 1)目的字符串；2)目的长度；3)源字串；4)串接长度
* output			: dest拷贝结果
* remark		: 目的缓冲区必须有足够的空间来存储源字符串、目的字符串和结束符
			wcsncat_s(wchar_t* strDest, size_t destMax, const wchar_t* strSrc, size_t count)
**************************************************************************/
#define eSDK_WCSNCAT wcsncat_s

/**************************************************************************
* name			: eSDK_STRTOK
* description	: 分割字符串
* input			: 1)要分割的字符串；2)字符串分隔符；3)保存调用strok_s后的位置信息
* output			: 指向被分割出的子字符串的指针
* remark		: 如果strToken为NULL，但context是一个指向合法context指针的指针的情况，是正确的
						strtok_s(char* strToken, const char* strDelimit, char** context)
**************************************************************************/
#define eSDK_STRTOK strtok_s

/**************************************************************************
* name			: eSDK_WCSTOK
* description	: 宽字节分割字符串
* input			: 1)要分割的字符串；2)字符串分隔符；3)保存调用strok_s后的位置信息
* output			: 指向被分割出的子字符串的指针
* remark		: 如果strToken为NULL，但context是一个指向合法context指针的指针的情况，是正确的
						wcstok_s(wchar_t* strToken, const wchar_t* strDelimit, wchar_t** context)
**************************************************************************/
#define eSDK_WCSTOK wcstok_s

/**************************************************************************
* name			: eSDK_SPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)格式化控制字符串；…可选参数
* output			: 目的缓冲区
* remark		: 当strDest太小以至于不能保存要打印进去的数据时，strDest首字节会被设置为空，并且返回-1
						sprintf_s能够确保strDest中的字符串是带结束符的（除非strDest的大小为0）
						sprintf_s(char* strDest, size_t destMax, const char* format, ...)
**************************************************************************/
#define eSDK_SPRINTF sprintf_s

/**************************************************************************
* name			: eSDK_SWPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)格式化控制字符串；…可选参数
* output			: 目的缓冲区
* remark		: 当strDest太小以至于不能保存要打印进去的数据时，strDest首字节会被设置为空，并且返回-1
						swprintf_s能够确保strDest中的字符串是带结束符的（除非strDest的大小为0）
						swprintf_s(wchar_t* strDest, size_t destMax, const wchar_t* format, ...)
**************************************************************************/
#define eSDK_SWPRINTF swprintf_s

/**************************************************************************
* name			: eSDK_SNPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)要输出到目的缓冲区的格式化字符个数；4)格式化控制字符串；…可选参数
* output			: 目的缓冲区
* remark		: 
						snprintf_s(char* strDest, size_t destMax, size_t count, const char* format, ...)
**************************************************************************/
#if defined WIN32 || defined _MSC_VER
#define eSDK_SNPRINTF  _snprintf_s
#else
#define eSDK_SNPRINTF snprintf_s
#endif

/**************************************************************************
* name			: eSDK_VSPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)格式化控制字符串；4)指向参数列表(va_list)的指针
* output			: 目的缓冲区
* remark		: 当format含有非法格式化说明符时，vsprintf_s会返回-1
						vsprintf_s(char* strDest, size_t destMax, const char* format, va_list arglist)
**************************************************************************/
#define eSDK_VSPRINTF vsprintf_s

/**************************************************************************
* name			: eSDK_VSWPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)格式化控制字符串；4)指向参数列表(va_list)的指针
* output			: 目的缓冲区
* remark		: 当format含有非法格式化说明符时，vswprintf_s会返回-1
						vswprintf_s(wchar_t* strDest, size_t destMax, const wchar_t* format, va_list arglist)
**************************************************************************/
#define eSDK_VSWPRINTF vswprintf_s

/**************************************************************************
* name			: eSDK_VSNPRINTF
* description	: 格式化输出到目的缓冲区
* input			: 1)目的缓冲区；2)目的缓冲区大小；3)要输出到目的缓冲区的格式化字符个数
						  4)格式化控制字符串；5)指向参数列表(va_list)的指针
* output			: 目的缓冲区
* remark		: 当format含有非法格式化说明符时，vsnprintf_s会返回-1
						为了保证字符串结尾能够添加结束符，一定要保证count严格的小于strDest的长度
						vsnprintf_s(char* strDest, size_t destMax, size_t count, const char* format, va_list arglist)
**************************************************************************/
#define eSDK_VSNPRINTF vsnprintf_s

/**************************************************************************
* name			: eSDK_FSCANF
* description	: 从文件输入流中读取格式化数据
* input			: 1)指向FILE结构的指针；2)格式化控制字符串；…可选参数
* output			: 输入结果
* remark		: 参数的类型必须与格式化控制字符串（format）中的类型保持一致
						fscanf_s(FILE* stream, const char* format, ...)
**************************************************************************/
#define eSDK_FSCANF fscanf_s

/**************************************************************************
* name			: eSDK_FWSCANF
* description	: 从文件输入流中读取格式化数据，宽字节版本
* input			: 1)指向FILE结构的指针；2)格式化控制字符串；…可选参数
* output			: 输入结果
* remark		: 参数的类型必须与格式化控制字符串（format）中的类型保持一致
						fwscanf_s(FILE* stream, const wchar_t* format, ...)
**************************************************************************/
#define eSDK_FWSCANF fwscanf_s

/**************************************************************************
* name			: eSDK_SCANF
* description	: 标准输入流输入
* input			: 1)格式化控制字符串；…可选参数
* output			: 输入结果
* remark		: 参数的类型必须与格式化控制字符串（format）中的类型保持一致
						scanf_s(const char* format, ...)
**************************************************************************/
#define eSDK_SCANF scanf_s

/**************************************************************************
* name			: eSDK_WSCANF
* description	: 标准输入流输入，宽字节版本
* input			: 1)格式化控制字符串；…可选参数
* output			: 输入结果
* remark		: 参数的类型必须与格式化控制字符串（format）中的类型保持一致
						wscanf_s(const char* format, ...)
**************************************************************************/
#define eSDK_WSCANF wscanf_s

/**************************************************************************
* name			: eSDK_SSCANF
* description	: 标准输入流输入
* input			: 1)目的字符串；2)格式化参数
* output			: 输入结果
* remark		: 可选参数的类型必须与格式化控制字符串（format）中的类型保持一致
						如果buffer与输入参数指针之间的内存存在重叠，则sscanf_s的行为未定义
						sscanf_s(const char* buffer, const char* format, ...)
**************************************************************************/
#define eSDK_SSCANF sscanf_s

/**************************************************************************
* name			: eSDK_SWSCANF
* description	: 标准输入流输入，宽字节版本
* input			: 1)目的字符串；2)格式化参数
* output			: 输入结果
* remark		: 可选参数的类型必须与格式化控制字符串（format）中的类型保持一致
						如果buffer与输入参数指针之间的内存存在重叠，则swscanf_s的行为未定义
						swscanf_s(const wchar_t* buffer, const wchar_t* format, ...)
**************************************************************************/
#define eSDK_SWSCANF swscanf_s

/**************************************************************************
* name			: eSDK_GETS
* description	: 标准输入流输入
* input			: 1)目的字符串；2)目的长度
* output			: 输入结果
* remark		: gets_s(char* buffer, size_t destMax)
**************************************************************************/
#define eSDK_GETS gets_s


#endif

