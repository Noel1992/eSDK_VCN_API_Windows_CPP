/********************************************************************
filename	: 	securec_win.h
author		:	zwx211831
created		:	2015/9/17	
description	:	定义 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2014-2016
history		:	2015/9/17 初始版本;
*********************************************************************/
#ifndef __SECUREC_WIN_H__
#define __SECUREC_WIN_H__

#if defined WIN32 || defined _MSC_VER
#ifdef ESDK_SECURE_C_EXPORTS
#define ESDK_SECURE_API __declspec(dllexport)
#else
#define ESDK_SECURE_API __declspec(dllimport)
#endif
#endif

#if defined WIN32 || defined _MSC_VER
#define __SECUREC_CALL __stdcall
#else
#define __SECUREC_CALL
#endif

#ifndef errno_t
typedef int errno_t;
#endif

/* for performance consideration, the following macro will the corresponding API 
* of libC for memcpy, memmove and memset
*/
#define CALL_LIBC_COR_API


/*define error code*/

/* success */
#define EOK (0)

/* invalid parameter */
#ifdef EINVAL
#undef EINVAL
#endif
#define EINVAL (22)
#define EINVAL_AND_RESET (22 | 0X80)
/* invalid parameter range */
#ifdef ERANGE
#undef ERANGE  /* to avoid redefinition */
#endif
#define ERANGE (34)
#define ERANGE_AND_RESET  (34 | 0X80)

#ifdef __cplusplus
extern "C"
{
#endif

	/**************************************************************************
	* name			: memset_s
	* description	: 内存初始化函数
	* input			: 1)dest目的地址；2)目的地址长度；3)初始化字符；4)初始化长度
	* output		: 初始化后的内存
	* remark		: 
	**************************************************************************/
	ESDK_SECURE_API errno_t __SECUREC_CALL memset_s(void* dest, size_t destMax, int c, size_t count);


#ifdef __cplusplus
};
#endif


#endif	//__SECUREC_WIN_H__

