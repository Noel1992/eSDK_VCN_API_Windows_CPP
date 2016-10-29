/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: secureprintoutput.h
* Decription: 
*             define macro, enum, data struct, and declare internal used function
*             prototype, which is used by output.inl, secureprintoutput_w.c and
*             secureprintoutput_a.c.
* History:   
*     1. Date:
*         Author:    
*         Modification:
********************************************************************************
*/

#ifndef __SECUREPRINTOUTPUT_H__E950DA2C_902F_4B15_BECD_948E99090D9C
#define __SECUREPRINTOUTPUT_H__E950DA2C_902F_4B15_BECD_948E99090D9C

/* flag definitions */
enum{

FLAG_SIGN           = 0x00001,   
FLAG_SIGN_SPACE     = 0x00002,   
FLAG_LEFT           = 0x00004,   
FLAG_LEADZERO       = 0x00008,   
FLAG_LONG           = 0x00010,   
FLAG_SHORT          = 0x00020,   
FLAG_SIGNED         = 0x00040,   
FLAG_ALTERNATE      = 0x00080,   
FLAG_NEGATIVE       = 0x00100,   
FLAG_FORCE_OCTAL    = 0x00200,   
FLAG_LONG_DOUBLE    = 0x00400,   
FLAG_WIDECHAR       = 0x00800,   
FLAG_LONGLONG       = 0x01000,   
FLAG_I64            = 0x08000 
};



/* character type values */
typedef enum _CHARTYPE
{
    CHAR_OTHER,                   /* character with no special meaning */
    CHAR_PERCENT,                 /* '%' */
    CHAR_DOT,                     /* '.' */
    CHAR_STAR,                    /* '*' */
    CHAR_ZERO,                    /* '0' */
    CHAR_DIGIT,                   /* '1'..'9' */
    CHAR_FLAG,                    /* ' ', '+', '-', '#' */
    CHAR_SIZE,                    /* 'h', 'l', 'L', 'N', 'F', 'w' */
    CHAR_TYPE                     /* type specifying character */
}CHARTYPE;

/* state definitions */
typedef enum _FMT_STATE
{
    STAT_NORMAL,           
    STAT_PERCENT,          
    STAT_FLAG,             
    STAT_WIDTH,            
    STAT_DOT,              
    STAT_PRECIS,           
    STAT_SIZE,             
    STAT_TYPE,             
    STAT_INVALID           
}FMT_STATE;

#define NUMSTATES (STAT_INVALID + 1)

#define BUFFERSIZE    512
#define MAXPRECISION  BUFFERSIZE

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 5
#endif
#define CVTBUFSIZE (309+40)      /* # of digits in max. dp value + slop */

#define FIND_CHAR_CLASS(lookuptbl, c)  ((c) < _T(' ') || (c) > _T('z') ? CHAR_OTHER : (CHARTYPE)(lookuptbl[(c)-_T(' ')] & 0xF))             

#define FIND_NEXT_STATE(lookuptbl, charClass, state)  (FMT_STATE)(lookuptbl[(charClass) * NUMSTATES + (state)] >> 4)


typedef struct _SECUREC_XPRINTF_STREAM
{
    int _cnt;
    char* _ptr;
} SECUREC_XPRINTF_STREAM;

/*LSD remove int util_get_int_arg (va_list *pargptr);
* long util_get_long_arg (va_list *pargptr);
* INT64T util_get_int64_arg (va_list *pargptr);
* LSD this function is deprecated short util_get_short_arg (va_list *pargptr); 
* void* util_get_ptr_arg (va_list *pargptr);
* long long is int64
* #define util_get_long_long_arg(x) util_get_int64_arg(x)
*/

void cfltcvt(double value, char* buffer, int bufSize, char fmt, int precision, int capexp);

void write_char_a
(
    char ch,
    SECUREC_XPRINTF_STREAM* f,
    int* pnumwritten
);

void write_multi_char_a
(
    char ch,
    int num,
    SECUREC_XPRINTF_STREAM* f,
    int* pnumwritten
);

void write_string_a
(
    char* string,
    int len,
    SECUREC_XPRINTF_STREAM* f,
    int* pnumwritten
);

int securec_output_s
(
    SECUREC_XPRINTF_STREAM* stream,
    const char* format,
    va_list argptr
);

int securec_woutput_s
(
    SECUREC_XPRINTF_STREAM* stream,
    const wchar_t* format,
    va_list argptr
);

#endif


