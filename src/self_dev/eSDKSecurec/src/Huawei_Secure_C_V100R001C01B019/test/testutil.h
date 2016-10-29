/*
testutil.h
*/

#ifndef __TESTUTIL_H__076E9CE7_555E_49b2_8DE3_A37C82A6F431
#define __UNITTEST_H__076E9CE7_555E_49b2_8DE3_A37C82A6F431

#include <stddef.h>
#include <time.h>
#include <limits.h>

#if !(defined(_WIN32) ||  defined(_WIN64))
#include <sys/time.h>
#endif

#ifdef _DEBUG
#define NULL_OR_SOME_ERROR_HANDLER printf("NOT OK")
#define COMP_MEMCPY(dest, destSize, src, cnt) (memcpy_s(dest, destSize, src, cnt) == EOK ? dest : NULL_OR_SOME_ERROR_HANDLER)
#define COMP_STRCPY(p, dSize, s) (strcpy_s(p, dSize, s) == EOK ? p : NULL)
#else /*for release*/
#define COMP_MEMCPY(dest, destSize, src, cnt) (memcpy_s(dest, destSize, src, cnt) , dest)
#define COMP_STRCPY(p, dSize, s) (strcpy_s(p, dSize, s), p)
#endif

#if (defined(_MSC_VER) && (_MSC_VER >= 1400)) || (defined(__GNUC__) && (__GNUC__ > 3))
/*VC6 don't support "..." in macro*/
/* coverity[exp_identifier] */
#define COMP_SNPRINTF(strDest, destMax, count,format, ...) snprintf_s(strDest, destMax, count,format, ##__VA_ARGS__)== -1 ? ( strlen(strDest) == count ? count : -1 ) : strlen(strDest);
#endif

#define COMP_VSNPRINTF(strDest, destMax, count,format, arglist) vsnprintf_s(strDest, destMax, count,format, arglist)== -1 ? ( strlen(strDest) == count ? count : -1 ) : strlen(strDest);


void assertFloatEqu(float v, float expectedVal);
size_t  slen (const char * str);
size_t  wslen (const wchar_t * str);
int  my_wcscmp (  const wchar_t * src,   const wchar_t * dst);

int is64Bits(void);
int is32Bits(void);

unsigned long long GetCurMilliSecond(void);

#endif
