/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: memcpy_s.c
* History:   
*     1. Date:
*         Author:    
*         Modification:
********************************************************************************
*/

#include "securec.h"
#include "securecutil.h"

/*******************************************************************************
 * <NAME>
 *    memcpy_s
 *
 * <SYNOPSIS>
 *    errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
 *
 * <FUNCTION DESCRIPTION>
 *    memcpy_s copies count bytes from src to dest
 *
 * <INPUT PARAMETERS>
 *    dest                       new buffer.
 *    destMax                    Size of the destination buffer.
 *    src                        Buffer to copy from.
 *    count                      Number of characters to copy
 *
 * <OUTPUT PARAMETERS>
 *    dest buffer                is updated.
 *
 * <RETURN VALUE>
 *    EOK                        Success
 *    EINVAL                     dest == NULL or strSrc == NULL
 *    ERANGE                     count > destMax or destMax > 
 *                               SECUREC_MEM_MAX_LEN or destMax == 0
 *    EOVERLAP_AND_RESET         dest buffer and source buffer are overlapped
 *
 *    if an error occured, dest will be filled with 0.
 *    If the source and destination overlap, the behavior of memcpy_s is undefined.
 *    Use memmove_s to handle overlapping regions.
 *******************************************************************************
*/

errno_t memcpy_s(void* dest, size_t destMax, const void* src, size_t count)
{
    if (destMax == 0 || destMax > SECUREC_MEM_MAX_LEN )
    {
        SECUREC_ERROR_INVALID_RANGE("memcpy_s");
        return ERANGE;
    }

    if (dest == NULL || src == NULL)
    {
        SECUREC_ERROR_INVALID_PARAMTER("memcpy_s");
        if (dest != NULL )
        {
            (void)memset(dest, 0, destMax);
            return EINVAL_AND_RESET;
        }
        return EINVAL;
    }
    if (count > destMax)
    {
        (void)memset(dest, 0, destMax);
        SECUREC_ERROR_INVALID_RANGE("memcpy_s");
        return ERANGE_AND_RESET;
    }
    if (dest == src)
    {
        return EOK;
    }
    if ((dest > src && dest < (void*)((UINT8T*)src + count)) ||
        (src > dest && src < (void*)((UINT8T*)dest + count)) )
    {
        (void)memset(dest, 0, destMax);
        SECUREC_ERROR_BUFFER_OVERLAP("memcpy_s");
        return EOVERLAP_AND_RESET;
    }

#ifdef CALL_LIBC_COR_API
    /*use underlying memcpy for performance consideration*/
    (void)memcpy(dest, src, count);
#else
    /*
       User can use gcc's __SIZEOF_POINTER__ macro to copy memory by single byte, 4 bytes or 8 bytes.
       User can reference memcpy_32b() and memcpy_64b() in securecutil.c
    */
    memcpy_8b(dest, src, count );
#endif

    return EOK;
}


