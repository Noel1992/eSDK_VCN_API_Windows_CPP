/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if defined _MSC_VER || defined WIN32
# include <sys/timeb.h>  // for _ftime
#endif 

#include "compat.h"

#if defined _MSC_VER && (_MSC_VER >= 1500)
#pragma warning (disable : 4996)
#endif 


/*
 * ivs_vsnprintf
 */
#if defined _MSC_VER || defined _WIN32
int IVS_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
    int len = vsnprintf_s(buffer, count, count, format, argptr);
    return len == -1? (int)count : len;
}
#endif 

#if defined __GNUC__ || defined LINUX
int ivs_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
    size_t len = eSDK_VSPRINTF(buffer, count, format, argptr);
    return len >= count ?  count - 1 : len; 
}
#endif


/*
 * ivs_snprintf
 */
int IVS_snprintf(char *buffer, size_t count, const char *format, ...)
{
    int len = 0;
    va_list args;
    va_start(args, format);
    len = IVS_vsnprintf(buffer, count, format, args);
    va_end(args);
    return len;
}

/*
 * IVS_strcasecmp
 */
#if defined _MSC_VER || defined _WIN32
int IVS_strcasecmp(const char *s1, const char *s2)
{
    return _stricmp(s1, s2);
}

int IVS_strncasecmp(const char *s1, const char* s2, size_t count)
{
    return  _strnicmp(s1, s2, count);
}
#else
int IVS_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}

int IVS_strncascmp(const char *s1, const char *s2, size_t count)
{
    return  strncasecmp(s1, s2, count);
}

#endif 


/*
 * IVS_localtime
 */
#ifdef _MSC_VER
int IVS_localtime(struct tm *_tm, const time_t *_tt)
{
    if (NULL == _tm || NULL == _tt)
    {
        return -1;
    }
    localtime_s(_tm, _tt);
    return 0;
}

int IVS_gettimeofday(struct timeval *tv)
{
    if (NULL == tv)
    {
        return (-1);
    }
    
    struct _timeb  localTime;
    _ftime(&localTime);
    tv->tv_sec  = (long)(localTime.time + localTime.timezone);
    tv->tv_usec = localTime.millitm * 1000;
    return (0);
}
#endif 


#if defined __GNUC__ || defined LINUX
int IVS_localtime(struct tm *_tm, const time_t *_tt)
{
    if (NULL == _tm || NULL == _tt)
    {
        return -1;
    }
    localtime_r(_tt, _tm);
    return 0;
}

int IVS_gettimeofday(struct timeval *tv)
{
    if (NULL == tv)
    {
        return -1;
    }
    gettimeofday(tv, NULL);
    return 0;
}
#endif 

