/********************************************************************
filename	: 	eSDK_Secure_C.cpp
author		:	zwx211831
created		:	2015/9/17	
description	:	实现 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2014-2016 Huawei Tech.Co.,Ltd
history		:	2015/9/17 初始版本;
*********************************************************************/

#include "stdafx.h"
#include "securec_win.h"
#include "securectype.h"
#include "securecutil.h"

//内存初始化函数
errno_t __SECUREC_CALL memset_s(void* dest, size_t destMax, int c, size_t count)
{
	if (destMax == 0 || destMax > SECUREC_MEM_MAX_LEN )
	{
		SECUREC_ERROR_INVALID_RANGE("memset_s");
		return ERANGE;
	}
	if (dest == NULL)
	{
		SECUREC_ERROR_INVALID_PARAMTER("memset_s");
		return EINVAL;
	}
	if (count > destMax)
	{
		(void)memset(dest, c, destMax);   /*set entire buffer to value c*/
		SECUREC_ERROR_INVALID_RANGE("memset_s");
		return ERANGE_AND_RESET;
	}

#ifdef CALL_LIBC_COR_API
	/*use underlying memcpy for performance consideration*/
	(void)memset(dest, c, count);
#else
	util_memset(dest, c, count);
#endif

	return EOK;
}
