/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: securecutil.c
* Decription: 
*             provides internal functions used by this library, such as memory
*             copy and memory move. Besides, include some helper function for
*             printf family API, such as vsnprintf_helper, __putc_nolock,
*             __putwc_nolock. Also includes some folat converting function, such
*             as cvt, ecvtbuf, fcvtbuf, cfltcvt.
* History:   
*     1. Date: 2014/4/10
*        Author: LiShunda
*        Modification: move vswprintf_helper() function from this file to vswprintf.c,
*                      which make this file only contain ANSI string function. This will
*                      facilitate the ANSI string API building.
*     2. Date: 2014/4/10
*        Author: LiShunda
*        Modification: add int putWcharStrEndingZero(SECUREC_XPRINTF_STREAM* str, int zerosNum)
*                      function. In vswprintf.c, the original code use (__putc_nolock('\0', str) != EOF )
*                      four times and a do-while wrapper which is NOT easy to read and understand,
*                      so abstract this function.
*     3. Date: 2014/4/10
*        Author: LiShunda
*        Modification: change a variabel name "exp" in function "cfltcvt" to "expVal", for "exp" is a function name in <math.h>,
*                      which make pclint warning.
*     4. Date: 2014/4/10
*        Author: LiShunda
*        Modification: remove 'char* s__nullstring = "(null)"' and 'wchar_t* s__wnullstring = L"(null)"' to
*                      to avoid pclint warning on "redundant statement declared symbol's__nullstring'".
*     4. Date: 2014/5/7
*        Author: LiShunda
*        Modification: replace memcpy with memcpy_s at line 558
*     5. Date: 2014/5/20
*        Author: LiShunda
*        Modification: change the return type of function "__putwc_nolock" from unsigned short to
*                      wchar_t.
*     6. Date: 2014/6/4
*        Author: LiShunda
*        Modification: change the function name from "fcvtbuf" to "fcvtbuf_hw", and add "static" modifier.
*                      change the function name from "ecvtbuf" to "ecvtbuf_hw", and add "static" modifier.
*     7. Date: 2014/6/8
*         Author: LiShunda
*         Modification: move the functions of __putwc_nolock, write_char_w, write_multi_char_w 
*                       and write_string_w from this file to secureprintoutput_w.c. In ANSI mode build,
*                       these functions are NOT needed, which can reduce code generation size
*                       and let securecutil.c NOT include <wchar.h>.
*     8. Date: 2014/7/1
*         Author:  LiShunda
*         Modification: move "securec__lookuptable_s[]" from this file to output.inl, which is to avoid 
*                     redefinition when multiple secure c library are integrated.
********************************************************************************
*/

#include <math.h>
#include "securec.h"
#include "securecutil.h"
#include "secureprintoutput.h"

#include <string.h>
#include <stdarg.h>

void getHwSecureCVersion(char* verStr, int bufSize, unsigned short* verNumber)
{
    if (verStr != NULL && bufSize > 0 )
    {
        (void)strcpy_s(verStr, (size_t)bufSize, "Huawei Secure C 1.8");
    }
    if (verNumber != NULL)
    {
        *verNumber = (1 << 8 | 8);
    }
}

void memcpy_8b(void* dest, const void* src, size_t count)
{
    UINT8T* pDest = (UINT8T*)dest;
    UINT8T* pSrc = (UINT8T*)src;

    while (count-- )
    {
        *pDest = *pSrc;
        ++pDest;
        ++pSrc;
    }
}

#ifndef CALL_LIBC_COR_API

#define SIZE_OF_4_BYTES (4)
typedef unsigned long int DWORD;

void memcpy_32b(void* dest, const void* src, size_t count)
{
    UINT8T* pDest = (UINT8T*)dest;
    UINT8T* pSrc = (UINT8T*)src;
    UINT8T* pEndPos = pDest + count;
    size_t loops = 0;
    DWORD* pdwDest = NULL;
    DWORD* pdwSrc = NULL;

    if (((DWORD)pDest % SIZE_OF_4_BYTES) != 0)
    {
        /*copy header unaligned bytes*/
        do
        {
            *pDest = *pSrc;
            ++pDest;
            ++pSrc;
        }
        while (pDest < pEndPos && ((DWORD)pDest % SIZE_OF_4_BYTES != 0));

        if (pDest == pEndPos)
        {
            return;
        }
    }

    loops = (pEndPos - pDest) / SIZE_OF_4_BYTES;
    if (loops > 0)
    {
        pdwDest = (DWORD*)pDest;
        pdwSrc = (DWORD*)pSrc;

        while (loops)
        {
            *pdwDest = *pdwSrc;
            ++pdwDest;
            ++pdwSrc;
            --loops;
        }
        pDest = (UINT8T*)pdwDest;
        pSrc = (UINT8T*)pdwSrc;

    }

    loops = pEndPos - pDest;

    /* copy ending unaligned bytes */
    while (loops )
    {
        *pDest = *pSrc;
        ++pDest;
        ++pSrc;
        --loops;
    }
}

#define SIZE_OF_8_BYTES (8)

void memcpy_64b(void* dest, const void* src, size_t count)
{
    UINT8T* pDest = (UINT8T*)dest;
    UINT8T* pSrc = (UINT8T*)src;
    UINT8T* pEndPos = pDest + count;
    size_t loops = 0;
    UINT64T* pqwDest = NULL;
    UINT64T* pqwSrc = NULL;

    if (((UINT64T)pDest % SIZE_OF_8_BYTES) != 0)
    {
        /*copy header unaligned bytes*/
        do
        {
            *pDest = *pSrc;
            ++pDest;
            ++pSrc;
        }
        while (pDest < pEndPos && ((UINT64T)pDest % SIZE_OF_8_BYTES != 0) );

        if (pDest == pEndPos)
        {
            return;
        }
    }

    loops = (pEndPos - pDest) / SIZE_OF_8_BYTES;

    if (loops > 0)
    {
        pqwDest = (UINT64T*)pDest;
        pqwSrc = (UINT64T*)pSrc;

        while (loops)
        {
            *pqwDest = *pqwSrc;
            ++pqwDest;
            ++pqwSrc;
            --loops;
        }
        pDest = (UINT8T*)pqwDest;
        pSrc = (UINT8T*)pqwSrc;
    }

    loops = pEndPos - pDest;

    /*copy ending unaligned bytes*/
    while (loops )
    {
        *pDest = *pSrc;
        ++pDest;
        ++pSrc;
        --loops;
    }
}
#endif    /*CALL_LIBC_COR_API*/

void util_memmove (void* dst, const void* src, size_t count)
{
    UINT8T* pDest = (UINT8T*)dst;
    UINT8T* pSrc = (UINT8T*)src;

    if (dst <= src || pDest >= (pSrc + count))    _CHECK_BUFFER_OVERLAP
    {
        /*
        * Non-Overlapping Buffers
        * copy from lower addresses to higher addresses
        */
        while (count--)
        {
            *pDest = *(UINT8T*)pSrc;
            ++pDest;
            ++pSrc;
        }
    }
    else
    {
        /*
        * Overlapping Buffers
        * copy from higher addresses to lower addresses
        */
        pDest = pDest + count - 1;
        pSrc = pSrc + count - 1;

        while (count--)
        {
            *pDest = *pSrc;

            --pDest;
            --pSrc;
        }
    }
}
/*put a char to output stream */
#define __putc_nolock(_c,_stream)    (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) :  EOF)

int putWcharStrEndingZero(SECUREC_XPRINTF_STREAM* str, int zerosNum)
{
    int succeed = 0, i = 0;

    for (; i < zerosNum && (__putc_nolock('\0', str) != EOF ); ++i)
    {
    }
    if (i == zerosNum)
    {
        succeed = 1;
    }
    return succeed;
}

int vsnprintf_helper (char* string, size_t count, const char* format, va_list arglist)
{
    SECUREC_XPRINTF_STREAM str;
    int retval;
    
    str._cnt = (int)count;

    str._ptr = string;

    retval = securec_output_s(&str, format, arglist );

    if ((retval >= 0) && (__putc_nolock('\0', &str) != EOF))
    {
        return (retval);
    }

    if (string != NULL)
    {
        string[count - 1] = 0;
    }

    if (str._cnt < 0)
    {
        /* the buffer was too small; we return -2 to indicate truncation */
        return -2;
    }
    return -1;
}


/*
below functions used for output/woutput
#ifdef __cplusplus
extern "C"
#endif
*/

static char* cvt(double arg, int ndigits, int* decpt, int* sign, char* buf, int eflag)
{
    int r2;
    double fi, fj;
    char* p, *p1;

    if (ndigits < 0)
    {
        ndigits = 0;
    }
    if (ndigits >= CVTBUFSIZE - 1)
    {
        ndigits = CVTBUFSIZE - 2;
    }

    r2 = 0;
    *sign = 0;
    p = &buf[0];
    if (arg < 0)
    {
        *sign = 1;
        arg = -arg;
    }
    arg = modf(arg, &fi);
    p1 = &buf[CVTBUFSIZE];

    if (fi != 0)
    {
        p1 = &buf[CVTBUFSIZE];
        while (fi != 0 && p1 > buf)    _CHECK_BUFFER_OVERLAP
        {
            fj = modf(fi / 10, &fi);
            *--p1 = (int)((fj + .03) * 10) + '0'; /*lint !e734, it should be '0'-'9'*/
            r2++;
        }
        while (p1 < &buf[CVTBUFSIZE] )    _CHECK_BUFFER_OVERLAP
        {
            *p++ = *p1++;
        }
    }
    else if (arg > 0)
    {
        while ((fj = arg * 10) < 1)
        {
            arg = fj;
            r2--;
        }
    }
    p1 = &buf[ndigits];
    if (eflag == 0) 
    {
        p1 += r2; 
    }
    *decpt = r2;
    if (p1 < &buf[0])    _CHECK_BUFFER_OVERLAP
    {
        buf[0] = '\0';
        return buf;
    }
    while (p <= p1 && p < &buf[CVTBUFSIZE])    _CHECK_BUFFER_OVERLAP
    {
        arg *= 10;
        arg = modf(arg, &fj);
        *p++ = (int) fj + '0';  /*lint !e734*/
    }
    if (p1 >= &buf[CVTBUFSIZE])    _CHECK_BUFFER_OVERLAP
    {
        buf[CVTBUFSIZE - 1] = '\0';
        return buf;
    }
    p = p1;
    *p1 += 5;
    while (*p1 > '9')
    {
        *p1 = '0';
        if (p1 > buf)    _CHECK_BUFFER_OVERLAP
        {
            ++*--p1;
        }
        else
        {
            *p1 = '1';
            (*decpt)++;
            if (eflag == 0)
            {
                if (p > buf)    _CHECK_BUFFER_OVERLAP
                {
                    *p = '0'; 
                }
                p++;
            }
        }
    }
    *p = '\0';
    return buf;
}

static char* ecvtbuf_hw(double arg, int ndigits, int* decpt, int* sign, char* buf)
{
    return cvt(arg, ndigits, decpt, sign, buf, 1);
}

static char* fcvtbuf_hw(double arg, int ndigits, int* decpt, int* sign, char* buf)
{
    return cvt(arg, ndigits, decpt, sign, buf, 0);
}

void cfltcvt(double value, char* buffer, int bufSize, char fmt, int precision, int capexp)
{
    int decpt, sign, expVal, pos;
    char* digits = NULL;
    char cvtbuf[CVTBUFSIZE];
    char* oriPos = buffer;
    int magnitude;
    char oriFmt = fmt;
    int littleDigit = 0;
    char* gPos = NULL;

    if (fmt == 'g')
    {
        /*digits =*/
        (void)ecvtbuf_hw(value, precision, &decpt, &sign, cvtbuf);
        magnitude = decpt - 1;
        if (magnitude < -4  ||  magnitude > precision - 1)
        {
            fmt = 'e';
            precision -= 1;
        }
        else
        {
            fmt = 'f';
            precision -= decpt;
        }
    }

    if (fmt == 'e')
    {
        digits = ecvtbuf_hw(value, precision + 1, &decpt, &sign, cvtbuf);

        if (sign) 
        { 
            *buffer++ = '-'; 
        }
        *buffer++ = *digits;
        if (precision > 0)
        {
            *buffer++ = '.';
        }
        (void)memcpy_s(buffer, bufSize, digits + 1, precision); /*lint !e732*/
        buffer += precision;
        littleDigit = precision;
        gPos = buffer;
        if('g' == oriFmt)
        {
            while(littleDigit-- > 0)
            {
                gPos--;
                if('0' == *gPos)
                {
                    *gPos = '\0';
                    if(0 == littleDigit)
                    {
                        gPos--;
                        if('.' == *gPos)
                        {
                            *gPos = '\0';
                        }
                        buffer = ++gPos;
                    }
                }
                else
                {
                    buffer = ++gPos;
                    break;
                }
            }
        }
        *buffer++ = capexp ? 'E' : 'e';

        if (decpt == 0)
        {
            if (value == 0.0)
            {
                expVal = 0;
            }
            else
            {
                expVal = -1;
            }
        }
        else
        {
            expVal = decpt - 1;
        }

        if (expVal < 0)
        {
            *buffer++ = '-';
            expVal = -expVal;
        }
        else
        {
            *buffer++ = '+';
        }

        buffer[2] = (expVal % 10) + '0';
        expVal = expVal / 10;
        buffer[1] = (expVal % 10) + '0';
        expVal = expVal / 10;

#ifdef COMPATIBLE_LINUX_FORMAT
        if ((expVal % 10) + '0' == '0')
        {
            buffer[0] = buffer[1]; 
            buffer[1] = buffer[2];
            buffer[2] = '\0';
            buffer += 2;
        }
        else
        {
            buffer[0] = (expVal % 10) + '0';
            buffer += 3;
        }
#else
        buffer[0] = (expVal % 10) + '0';
        buffer += 3;
#endif


    }
    else if (fmt == 'f')
    {
        digits = fcvtbuf_hw(value, precision, &decpt, &sign, cvtbuf);


        if (sign)
        {
            *buffer++ = '-';
        }
        if (*digits)
        {
            if (decpt <= 0)
            {
                *buffer++ = '0';
                *buffer++ = '.';
                for (pos = 0; pos < -decpt; pos++)
                {
                    *buffer++ = '0';
                }
                while (*digits)
                {
                    if (buffer - oriPos >= bufSize)    _CHECK_BUFFER_OVERLAP
                    {
                        break;
                    }
                    *buffer++ = *digits++;
                }
            }
            else
            {
                pos = 0;
                while (*digits)
                {
                    if (buffer - oriPos >= bufSize)    _CHECK_BUFFER_OVERLAP
                    {
                        break;
                    }
                    if (pos++ == decpt)
                    { 
                        *buffer++ = '.'; 
                    }
                    *buffer++ = *digits++;
                }
            }


        }
        else
        {
            *buffer++ = '0';
            if (precision > 0)
            {
                *buffer++ = '.';
                for (pos = 0; pos < precision; pos++)
                {
                    *buffer++ = '0';
                }
            }
        }
        if('g' == oriFmt)
        {
            littleDigit = precision;
            gPos = buffer;
            while(littleDigit-- > 0)
            {
                gPos--;
                if('0' == *gPos)
                {
                    *gPos = '\0';
                    if(0 == littleDigit)
                    {
                        gPos--;
                        if('.' == *gPos)
                            *gPos = '\0';
                        buffer = ++gPos;
                    }
                }
                else
                {
                    buffer = ++gPos;
                    break;
                }
            }
        }   
    }

    if ( buffer - oriPos >= bufSize)    _CHECK_BUFFER_OVERLAP
    {
        /*buffer overflow*/
        (void)memset_s(oriPos, (size_t)bufSize, 0, (size_t)bufSize);
    }else
    {
        *buffer = '\0';
    }
}

void write_char_a( char ch, SECUREC_XPRINTF_STREAM* f, int* pnumwritten)
{
    if (__putc_nolock(ch, f) == EOF)
    {
        *pnumwritten = -1;
    }
    else
    {
        ++(*pnumwritten);
    }
}



void write_multi_char_a(char ch, int num, SECUREC_XPRINTF_STREAM* f, int* pnumwritten)
{
    while (num-- > 0)
    {
        write_char_a(ch, f, pnumwritten);
        if (*pnumwritten == -1)
        {
            break;
        }
    }
}



void write_string_a (char* string, int len, SECUREC_XPRINTF_STREAM* f, int* pnumwritten)
{
    while (len-- > 0)
    {
        write_char_a(*string++, f, pnumwritten);
        if (*pnumwritten == -1)
        {
            /*
            if (errno == EILSEQ)
                write_char(_T('?'), f, pnumwritten);
            else
                break;
            */
            break;
        }
    }
}




