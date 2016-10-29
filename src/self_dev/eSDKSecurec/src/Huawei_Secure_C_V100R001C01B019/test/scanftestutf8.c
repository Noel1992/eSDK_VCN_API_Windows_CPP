#include "securec.h"
#include "unittest.h"
#include "testutil.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>

#define EPSINON 0.00001
#define DEST_BUFFER_SIZE  20
#define SRC_BUFFER_SIZE  200

#if !defined (COUNTOF)
	#define COUNTOF(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

/*
int strcmp (   const char * src,   const char * dst  )
{
	int ret = 0 ;

	while( ! (ret = *(uint8_t *)src - *(uint8_t*)dst) && *dst)
		++src, ++dst;

	if ( ret < 0 )
		ret = -1 ;
	else if ( ret > 0 )
		ret = 1 ;

	return( ret );
}

*/


void test_scanf(void)
{
	int ret = 0, j = 0, conv= 0,iv = 0;
	float fv = 0;
	char dest[DEST_BUFFER_SIZE] = {0};
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};

	printf("do you want to test scanf? (y/n)");
	ret = getchar();
	ret = tolower(ret);

	while (ret == 'y'){
		
		printf("please input integer float string\n");
		
		if ( j % 2 == 0) {
			conv = scanf_s("%d%f%100s", &iv, &fv, dest, DEST_BUFFER_SIZE);
			printf("convert %d items, you input value are %d  %f %.100s \n", conv, iv, fv, dest);
		}else{
			conv = wscanf_s(L"%d%f%100s", &iv, &fv, wDest, DEST_BUFFER_SIZE);
			wprintf(L"convert %d items, you input value are %d  %f %.100s \n", conv, iv, fv, wDest);
		}
		
		
		++j;
		
		printf("continue test scanf? (y, n)");
		ret = tolower( getchar() );
	}
}

int indirect_scanf(const char* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vscanf_s( format, args ) ;
	va_end(args);
	return ret;
	
}
void clearStdinBuffer(void)
{
/*	int ret = 0;
	while( (ret =  fgetc(stdin)) != '\n' ){
	//	printf("%c", ret);
	}
	char buf[5];
	while (gets_s(buf, 5) != NULL && slen(buf) > 0 ){
	}
	*/
}
void test_vscanf(void)
{
	int ret = 0, j = 0, conv= 0,iv = 0;
	float fv = 0;
	char dest[DEST_BUFFER_SIZE] = {0};
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};

	printf("do you want to test vscanf? (y/n)");
	ret = getchar();
	ret = tolower(ret);

	while (ret == 'y'){
		
		printf("input format:  %s\n", "%d%f%100s");
		
		conv = indirect_scanf("%d%f%100s", &iv, &fv, dest, DEST_BUFFER_SIZE);
		printf("convert %d items, you input value are %d  %f %.100s \n", conv, iv, fv, dest);
		
		if (conv <= 0){
			clearStdinBuffer();

		}
		printf("continue test vscanf? (y, n)");
		ret = tolower( getchar() );
	}
}


void test_wscanf(void)
{
	int ret = 0;
	int      i,     result;
	float    fp;
	char     c,
		s[80];
	wchar_t  wc,
		ws[80];

	printf("do you want to test wscanf? (y/n)");
	ret = tolower(getchar());

	while (ret == 'y'){


		printf("%s", "input format: %d %f %hc %lc %S %ls\n");

		result = wscanf_s( L"%d %f %hc %lc %S %ls", &i, &fp, &c, 1,
			&wc, 1, s, COUNTOF(s), ws, COUNTOF(ws) );

		wprintf( L"The number of fields input is %d\n", result );
		wprintf( L"The contents are: %d %f %C %c %hs %s\n", i, fp,
			c, wc, s, ws);


		printf("continue test wscanf? (y, n)");
		ret = tolower( getchar() );
	}
}

int indirect_wscanf(const wchar_t* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vwscanf_s( format, args );
	va_end(args);
	return ret;
	
}
void test_vwscanf(void)
{
	int ret = 0;
	int      i,     result;
	float    fp;
	char     c,
		s[80];
	wchar_t  wc,
		ws[80];

	printf("do you want to test vwscanf? (y/n)\n");
	ret = getchar();
	ret = tolower(ret);

	while (ret == 'y'){

		printf("%s", "input format: %d %f %hc %lc %S %ls\n");

		result = indirect_wscanf( L"%d %f %hc %lc %S %ls", &i, &fp, &c, 1,
			&wc, 1, s, COUNTOF(s), ws, COUNTOF(ws) );

		wprintf( L"The number of fields input is %d\n", result );
		wprintf( L"The contents are: %d %f %C %c %hs %s\n", i, fp,
			c, wc, s, ws);


		printf("continue test vwscanf? (y, n)\n");
		ret = tolower( getchar() );
	}
}
#define TEST_STR "1111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000.00000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999"
#define TEST_STR2 "1111111111.0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999922222222223333333333444444444455555555556666666666777777777788888888889999999999"
#define TEST_WSTR L"1111111111.0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999922222222223333333333444444444455555555556666666666777777777788888888889999999999"


#ifdef __GNUC__ 

#define CSET_GBK    "GBK" 
#define CSET_UTF8   "UTF-8" 
#define LC_NAME_zh_CN   "zh_CN" 

#elif defined(_MSC_VER) 

#define CSET_GBK    "936" 
#define CSET_UTF8   "65001" 
#define LC_NAME_zh_CN   "Chinese_People's Republic of China" 

#endif 

#define LC_NAME_zh_CN_GBK       LC_NAME_zh_CN "." CSET_GBK 
#define LC_NAME_zh_CN_UTF8      LC_NAME_zh_CN "." CSET_UTF8 

#ifdef __GNUC__ 
	#define LC_NAME_zh_CN_DEFAULT   LC_NAME_zh_CN_UTF8 
#elif defined(_MSC_VER) 
	#define LC_NAME_zh_CN_DEFAULT   LC_NAME_zh_CN_GBK 
#endif 
/*
void TC_01()
{
	char buf[] ="21341234 123 4123412 -1";
	int point=0,orginvalue=0;
	int hLLT_Result = sscanf_s(buf,"%p",&point);
	int orgin_Result = sscanf(buf,"%p",&orginvalue);

	assert(orgin_Result == hLLT_Result);
	assert(point == orginvalue);
}	
void TC_02()
{
	char buf[] ="21341234 123 4123412 -1";
	int point=0,orginvalue=0;

	int hLLT_Result = sscanf_s(buf,"%Fp",&point);
	int orgin_Result = sscanf(buf,"%Fp",&orginvalue);

	assert(orgin_Result == hLLT_Result);
	assert(point == orginvalue);
}
void TC_03()
{
	char buf[128]={0},orginss[128]={0};

	int hLLT_Result = sscanf_s("452345jwertjlkqwe;rt35",
			"%ws",
			buf,sizeof(buf));
	int orgin_Result = sscanf("452345jwertjlkqwe;rt35",
			"%ws",
			orginss);
	assert(orgin_Result == hLLT_Result);
	assert(0 == strcmp(orginss,buf));
}

void TC_04()
{
	char buf[128]={0},orginss[128]={0};

	int hLLT_Result = sscanf_s("hello, world",
			"%S",
			buf,sizeof(buf));
	int orgin_Result = sscanf("hello, world",
			"%S",
			orginss);
	assert(orgin_Result == hLLT_Result);
	assert(0 == strcmp(orginss,buf));
}
*/
void TC_05()
{
/*	FILE *stream;
	int iRet;
	int iv;
	int a, b, c, l;
	wchar_t buf[1024];

	//big.out   little.txt
	if( (stream = fopen( "d:/big.out", "r+" )) == NULL )
	{
		wprintf( L"The file fscanf1.out was not opened\n" );
	}
	else
	{

	//	iRet= fwscanf( stream, L"%s", buf,1024);

		iRet= fwscanf( stream, L"%sd", &iv);

		wprintf( L"%s\r\n", &buf[2]);
		fclose( stream );
	}
*/
}

void scanf_gbk(void)
{
	char* oriLocale = NULL;
	char* newLocale = NULL;
	const char* as = "蝴蝶测试工具，好不好用呢？谁用谁知道啊！";
	const wchar_t *ws = L"蝴蝶测试工具，好不好用呢？谁用谁知道啊！";
	wchar_t wBuf[48];
	char atmp[88]={0};
	int ret = 0;
/*
	TC_01();
	TC_02();
	TC_03();
	TC_04();
	TC_05();
*/
	ret = MB_CUR_MAX;
	oriLocale = setlocale(LC_ALL, NULL);


	newLocale = setlocale(LC_ALL, LC_NAME_zh_CN_DEFAULT);
	if ( NULL == newLocale ) 
	{ 
		printf("setlocale() with %s failed.\n", LC_NAME_zh_CN_DEFAULT); 
	} 
	else 
	{ 
		printf("setlocale() with %s succeed.\n", LC_NAME_zh_CN_DEFAULT); 
		ret = MB_CUR_MAX;
		ret = wctomb(atmp, ws[0] );
		ret = sscanf_s( as, "%S", wBuf , sizeof(wBuf) / sizeof(wchar_t) );
		assert(ret == 1);
		assert(0 == my_wcscmp(wBuf, ws));
	
		ret = sscanf_s( as, "蝴蝶%s", atmp , sizeof(atmp));
		assert(ret == 1);
		assert(0 == strcmp(atmp, "测试工具，好不好用呢？谁用谁知道啊！"));
		
	}

	setlocale(LC_ALL, oriLocale);	//restore original locale
	ret = MB_CUR_MAX;

	ret = sscanf_s( as, "%s", atmp , sizeof(atmp));
	assert(ret == 1);
	assert(0 == strcmp(atmp, as));

	ret = swscanf_s(ws, L"%s", wBuf , sizeof(wBuf) / 2);
	assert(ret == 1);
	assert(0 == my_wcscmp(wBuf, ws));


}
int vfscf(FILE *pf, const char *fmt, ...)
{
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vfscanf_s(pf, fmt, argptr);
	va_end(argptr);
	return(cnt);
}

 typedef struct _MY_INT64{
        unsigned long LowPart;
        long HighPart;
    }MY_INT64;

void test_sscanf(void)
{

	char dest[DEST_BUFFER_SIZE] = {0};
	char dest2[DEST_BUFFER_SIZE] = {0};
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	char inputStr[] = "123 567.4 asdfqwer";
	wchar_t wStr[] = L"123 5689.4 asdfqwer";

	char src[SRC_BUFFER_SIZE] = "abcd";
	char str[512] = {0};

	int iv = 0xFF, 	ret ;
	float fv = 0;
	int conv = 0;
	unsigned short wv = iv;
	char* pTest = dest;
	unsigned int  ui = 0;
	int a, b ,c,d = 0;
	char  tokenstring[] = "15 12 14...";
	char  s[81] = {0};
	char  ch;
	int   i;
	float fp = 0.0f;
	double dblVal;


	int j = 0;
	int inumber = 9000;
	int iRet = 0;
	FILE *ffp;
	MY_INT64 val64;
	
	conv = sscanf_s("3.456789","%lg",&dblVal);
	assert(conv == 1);
	assertFloatEqu((float)dblVal, 3.456789f);

	conv = sscanf_s("3.456789","%Lg",&dblVal);
	assert(conv == 1);
	assertFloatEqu((float)dblVal, 3.456789f);

/*	conv = sscanf_s("3.456789","%llg",&dblVal);
	assert(conv == 1);
	assertFloatEqu(dblVal, 3.456789f);
*/

	conv = sscanf_s("34.12454","%f",&fv);
	assertFloatEqu(fv, 34.12454f);

	conv = swscanf_s(L"34.12454",L"%f",&fv);
	assertFloatEqu(fv, 34.12454f);

	if( !setlocale(LC_ALL, LC_NAME_zh_CN_DEFAULT) ) {
		printf("setlocale failed\n" );	
	}
	else{
		conv = sscanf_s("a中a23elp2345","a中%s", dest, DEST_BUFFER_SIZE);
		assert(conv == 1);
		assert( strcmp(dest, "a23elp2345") == 0);

		conv = sscanf_s("a中a23elp2345中国","a中%s", dest, DEST_BUFFER_SIZE);
		assert(conv == 1);
		assert( strcmp(dest, "a23elp2345中国") == 0);
	}

	conv = sscanf_s("9223372036854775807", "%lld", &val64);
	//assert(sizeof(MY_INT64) == 8);
	assert(conv == 1);
	//printf("%I64u/n", *(__int64*)&val64);

	conv = sscanf_s( "3.1415gh", "%f", &fv );
	assert(conv == 1);
	assertFloatEqu(fv, 3.1415f);

	conv = sscanf_s("123asd45678abcd  12323", "%s",  dest, DEST_BUFFER_SIZE);
	assert(conv == 1);
	ffp = tmpfile();

	if (ffp == NULL)
	{
		printf("tmpfile() fail");
	}else{

		fprintf(ffp,"%d\n",inumber);
		rewind(ffp);
		iRet = vfscf(ffp, "%d",&inumber);
		fclose(ffp);
		assert(iRet == 1);
		assert(inumber == 9000);
	}

	conv = sscanf_s("123asdfasdf1123 123","%s", dest, -1);
	assert(conv == 0);

	strcpy_s(dest,DEST_BUFFER_SIZE, "asdfqwer");
	conv = sscanf_s("123asdfasdf1123","%[a-z",dest, DEST_BUFFER_SIZE);
	assert(conv == 0);
	assert( strcmp(dest, "") == 0);

	conv = sscanf_s(TEST_STR, "%f",  &fv);
	assert(conv == 1);

	conv = sscanf_s(TEST_STR2, "%f",  &fv);
	assertFloatEqu(fv, 1111111111.0f);
	assert(conv == 1);

	conv = sscanf_s("32145.1415926", "%f",  &fv);
	assert(conv == 1);

	strcpy_s(src, DEST_BUFFER_SIZE, "123 567.4 asdfqwer");
	conv = sscanf_s(src, "%d%f%s", &iv, &fv, dest,DEST_BUFFER_SIZE);
	assert(conv == 3);
	assert( strcmp(dest, "asdfqwer") == 0);
	assert( iv == 123 );
	assertFloatEqu(fv, 567.4f);

	conv = swscanf_s(TEST_WSTR, L"%f",  &fv);
	assertFloatEqu(fv, 1111111111.0f);
	assert(conv == 1);

	conv = swscanf_s(wStr, L"%d%f%100s", &iv, &fv, wDest, DEST_BUFFER_SIZE );
	assert(conv == 3 );
	assert(my_wcscmp(wDest, L"asdfqwer") == 0);
	assert(iv == 123 );
	assertFloatEqu(fv, 5689.4f);


	ret = sscanf_s("", "%10s", dest, DEST_BUFFER_SIZE);	
	assert(ret == -1);

	ret = sscanf_s("123", "%10s", dest, DEST_BUFFER_SIZE);	
	assert(strcmp(dest, "123") == 0);

	//2014 add test case
	ret = sscanf_s("12345678", "%3s", NULL, DEST_BUFFER_SIZE);	
	assert( ret == -1);

	ret = sscanf_s("12345678", "%3s", dest, DEST_BUFFER_SIZE);	
	assert(strcmp(dest, "123") == 0);

	sscanf_s("123", "%8s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123") == 0);

	sscanf_s("31.415926358", "%f",&fv);
	assertFloatEqu(fv, 31.415926358f);

	sscanf_s("31.415926358", "%4f",&fv);
	assertFloatEqu(fv, 31.4f);

	sscanf_s("31.415926358", "%6f",&fv);
	assertFloatEqu(fv, 31.415f);

	sscanf_s("2147483647", "%d",&d);
	assert(d == 2147483647);

	sscanf_s("2147483648", "%d",&d);
	assert(d == -2147483648L);


	sscanf_s("2147483648", "%u",&ui);
	assert(ui == 2147483648);

	ret = sscanf_s("10 0x1b aaaaaaaa bbbbbbbb","%d %x %5[a-z] %*s %f",&d,&a,dest, DEST_BUFFER_SIZE,dest, DEST_BUFFER_SIZE);
	assert(ret == 3);
	assert(d ==10);
	assert(a ==27);
	assert(strcmp(dest, "aaaaa") == 0);

	sscanf_s("-2147483648", "%d",&d);
	assert(d == -2147483648L);

	sscanf_s("3147483647", "%d",&d);
	assert(d == -1147483649);

	sscanf_s("789456", "%i",&d);
	assert(d == 789456);


	sscanf_s("1234567890", "%1s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1") == 0);

	sscanf_s("2006:03:18", "%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);

	sscanf_s("1234567890", "%2s%3s", dest, DEST_BUFFER_SIZE,dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "12") == 0);
	assert(strcmp(dest2, "345") == 0);

	ret = sscanf_s("hello, world", "%*s%s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "world") == 0);

	ret = sscanf_s("wpc:123456", "%127[^:]:%127[^ ]", dest, DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "wpc") == 0);
	assert(strcmp(dest2, "123456") == 0);


	sscanf_s("0001A", "%4x", &iv);
	assert(iv == 1);

	sscanf_s("0001A", "%x", &iv);
	assert(iv == 26);

	sscanf_s("123456 ", "%4s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1234") == 0);

	sscanf_s("a", "%4s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "a") == 0);

	sscanf_s("123456abcdedfBCDEF", "%[1-9a-z]", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);

	sscanf_s("123456abcdedfBCDEF","%[1-9A-Z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	sscanf_s("123456abcdedfBCDEF", "%[^A-Z]", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);

	ret = sscanf_s("iios/12DDWDFF@122", "%*[^/]/%[^@]", dest, DEST_BUFFER_SIZE);
	assert(1 == ret);
	assert(strcmp(dest, "12DDWDFF") == 0);

	ret = sscanf_s("hello, world", "%*s%s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "world") == 0);

	ret = sscanf_s("parent 2","%*s%d",&d);            /*Can Parse Correctly*/
	assert(1 == ret);
	assert(d == 2);

	sscanf_s("parent25","parent%d",&d);            /* result 25 returned*/
	assert(d == 25);

	ret = sscanf_s("parent2","%*s%d",&d);          /*Cannot parse because %s is assigned "parent2"*/
	assert(ret == 0);


	ret = sscanf_s("parent2","%*6s%d",&d);         /*Can Parse Corrently Because width specified*/
	assert(ret == 1);
	assert(d == 2);

	ret = sscanf_s("parent2","%*[a-z]%d",&d);      /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);

	ret =  sscanf_s("parent2parent","%*[a-z]%d",&d);    /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);

	sscanf_s("parent22parent","%*[a-z]%1d",&d);    /*result 2 returned*/
	assert(d == 2);

	sscanf_s("asd/35@32","%*[^/]/%d",&d);        /*result 35 returned*/
	assert(d == 35);


	sscanf_s( "iios/12DDWDFF@122", "%*[^/]/%[^@]", dest,DEST_BUFFER_SIZE); 
	assert(strcmp(dest, "12DDWDFF") == 0);


	sscanf_s("123456 ","%s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	sscanf_s("123456 ","%4s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1234") == 0);

	sscanf_s("123456 abcdedf","%[^ ]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	sscanf_s("123456abcdedfBCDEF","%[1-9a-z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);


	sscanf_s("123456abcdedfBCDEF","%[^A-Z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);


	sscanf_s( tokenstring, "%80s", dest, DEST_BUFFER_SIZE ); 
	assert(strcmp(dest, "15") == 0);

	sscanf_s( tokenstring, "%c", &ch, sizeof(char) ); 
	assert(ch == '1');

	sscanf_s( tokenstring, "%d", &i );  
	assert(i == 15);

	sscanf_s( tokenstring, "%f", &fv ); 
	assertFloatEqu(fv, 15.0f);

	sscanf_s("2006:03:18-2006:04:18", "%[0-9,:]-%[0-9,:]", dest,DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "2006:03:18") == 0);
	assert(strcmp(dest2, "2006:04:18") == 0);

}

int indirect_sscanf(const char* str, const char* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vsscanf_s(str, format, args ) ;
	va_end(args);
	return ret;
	
}
void test_vsscanf(void)
{

	char dest[DEST_BUFFER_SIZE] = {0};
	char dest2[DEST_BUFFER_SIZE] = {0};
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	char inputStr[] = "123 567.4 asdfqwer";
	wchar_t wStr[] = L"123 5689.4 asdfqwer";

	char src[SRC_BUFFER_SIZE] = "abcd";
	char str[512] = {0};

	int iv = 0xFF, 	ret ;
	float fv = 0;
	int conv = 0;
	unsigned short wv = iv;
	char* pTest = dest;
	unsigned int  ui = 0;
	int a, b ,c,d = 0;
	char  tokenstring[] = "15 12 14...";
	char  s[81] = {0};
	char  ch;
	int   i;
	float fp = 0.0f;


	int j = 0;


	strcpy_s(src, DEST_BUFFER_SIZE, "123 567.4 asdfqwer");
	conv = indirect_sscanf(src, "%d%f%s", &iv, &fv, dest,DEST_BUFFER_SIZE);
	assert(conv == 3);
	assert( strcmp(dest, "asdfqwer") == 0);
	assert( iv == 123 );
	assertFloatEqu(fv, 567.4f);

	conv = swscanf_s(wStr, L"%d%f%100s", &iv, &fv, wDest, DEST_BUFFER_SIZE );
	assert(conv == 3 );
	assert(my_wcscmp(wDest, L"asdfqwer") == 0);
	assert(iv == 123 );
	assertFloatEqu(fv, 5689.4f);

	ret = sprintf_s(dest, DEST_BUFFER_SIZE, "asdasd%s", NULL);
	/*assert(0);*/

	ret = indirect_sscanf("", "%10s", dest, DEST_BUFFER_SIZE);	
	assert(ret == -1);

	ret = indirect_sscanf("123", "%10s", dest, DEST_BUFFER_SIZE);	
	assert(strcmp(dest, "123") == 0);

	//2014 add test case
	ret = indirect_sscanf("12345678", "%3s", NULL, DEST_BUFFER_SIZE);	
	assert( ret == -1);

	ret = indirect_sscanf("12345678", "%3s", dest, DEST_BUFFER_SIZE);	
	assert(strcmp(dest, "123") == 0);

	indirect_sscanf("123", "%8s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123") == 0);

	indirect_sscanf("31.415926358", "%f",&fv);
	assertFloatEqu(fv, 31.415926358f);

	indirect_sscanf("31.415926358", "%4f",&fv);
	assertFloatEqu(fv, 31.4f);

	indirect_sscanf("31.415926358", "%6f",&fv);
	assertFloatEqu(fv, 31.415f);

	indirect_sscanf("2147483647", "%d",&d);
	assert(d == 2147483647);

	indirect_sscanf("2147483648", "%d",&d);
	assert(d == -2147483648L);


	indirect_sscanf("2147483648", "%u",&ui);
	assert(ui == 2147483648);

	indirect_sscanf("10 0x1b aaaaaaaa bbbbbbbb","%d %x %5[a-z] %*s %f",&d,&a,dest, DEST_BUFFER_SIZE,dest, DEST_BUFFER_SIZE);
	assert(d ==10);
	assert(a ==27);
	assert(strcmp(dest, "aaaaa") == 0);

	indirect_sscanf("-2147483648", "%d",&d);
	assert(d == -2147483648L);

	indirect_sscanf("3147483647", "%d",&d);
	assert(d == -1147483649);

	indirect_sscanf("789456", "%i",&d);
	assert(d == 789456);


	indirect_sscanf("1234567890", "%1s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1") == 0);

	indirect_sscanf("2006:03:18", "%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);

	indirect_sscanf("1234567890", "%2s%3s", dest, DEST_BUFFER_SIZE,dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "12") == 0);
	assert(strcmp(dest2, "345") == 0);

	indirect_sscanf("hello, world", "%*s%s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "world") == 0);

	indirect_sscanf("wpc:123456", "%127[^:]:%127[^ ]", dest, DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "wpc") == 0);
	assert(strcmp(dest2, "123456") == 0);


	indirect_sscanf("0001A", "%4x", &iv);
	assert(iv == 1);

	indirect_sscanf("0001A", "%x", &iv);
	assert(iv == 26);

	indirect_sscanf("123456 ", "%4s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1234") == 0);

	indirect_sscanf("a", "%4s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "a") == 0);

	indirect_sscanf("123456abcdedfBCDEF", "%[1-9a-z]", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);

	indirect_sscanf("123456abcdedfBCDEF","%[1-9A-Z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	indirect_sscanf("123456abcdedfBCDEF", "%[^A-Z]", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);

	indirect_sscanf("iios/12DDWDFF@122", "%*[^/]/%[^@]", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "12DDWDFF") == 0);

	indirect_sscanf("hello, world", "%*s%s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "world") == 0);

	indirect_sscanf("parent 2","%*s%d",&d);            /*Can Parse Correctly*/
	assert(d == 2);

	indirect_sscanf("parent25","parent%d",&d);            /* result 25 returned*/
	assert(d == 25);

	ret = indirect_sscanf("parent2","%*s%d",&d);          /*Cannot parse because %s is assigned "parent2"*/
	assert(ret == 0);


	ret = indirect_sscanf("parent2","%*6s%d",&d);         /*Can Parse Corrently Because width specified*/
	assert(ret == 1);
	assert(d == 2);

	ret = indirect_sscanf("parent2","%*[a-z]%d",&d);      /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);

	ret =  indirect_sscanf("parent2parent","%*[a-z]%d",&d);    /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);

	indirect_sscanf("parent22parent","%*[a-z]%1d",&d);    /*result 2 returned*/
	assert(d == 2);

	indirect_sscanf("asd/35@32","%*[^/]/%d",&d);        /*result 35 returned*/
	assert(d == 35);


	indirect_sscanf( "iios/12DDWDFF@122", "%*[^/]/%[^@]", dest,DEST_BUFFER_SIZE); 
	assert(strcmp(dest, "12DDWDFF") == 0);


	indirect_sscanf("123456 ","%s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	indirect_sscanf("123456 ","%4s",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1234") == 0);

	indirect_sscanf("123456 abcdedf","%[^ ]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456") == 0);

	indirect_sscanf("123456abcdedfBCDEF","%[1-9a-z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);


	indirect_sscanf("123456abcdedfBCDEF","%[^A-Z]",dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "123456abcdedf") == 0);


	indirect_sscanf( tokenstring, "%80s", dest, DEST_BUFFER_SIZE ); 
	assert(strcmp(dest, "15") == 0);

	indirect_sscanf( tokenstring, "%c", &ch, sizeof(char) ); 
	assert(ch == '1');

	indirect_sscanf( tokenstring, "%d", &i );  
	assert(i == 15);

	indirect_sscanf( tokenstring, "%f", &fv ); 
	assertFloatEqu(fv, 15.0f);

	indirect_sscanf("2006:03:18-2006:04:18", "%[0-9,:]-%[0-9,:]", dest,DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "2006:03:18") == 0);
	assert(strcmp(dest2, "2006:04:18") == 0);

}

void testSwscanf(void)
{
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	int a, b ,c,d = 0;

	swscanf_s(L"2006:03:18", L"%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);

	swscanf_s(L"123456 ",L"%s", wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);

	swscanf_s(L"123456 ",L"%4s",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"1234") == 0);

	swscanf_s(L"123456 abcdedf",L"%[^ ]",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);

}

int indirect_swscanf(const wchar_t* str, const wchar_t* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vswscanf_s(str, format, args ) ;
	va_end(args);
	return ret;
	
}
void test_vswscanf(void)
{
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	int a, b ,c,d = 0;

	indirect_swscanf(L"2006:03:18", L"%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);

	indirect_swscanf(L"123456 ",L"%s", wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);

	indirect_swscanf(L"123456 ",L"%4s",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"1234") == 0);

	indirect_swscanf(L"123456 abcdedf",L"%[^ ]",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);
}

void testfscanf(void)
{

	char filename[256];
	char dest[DEST_BUFFER_SIZE] = {0};
	char dest2[DEST_BUFFER_SIZE] = {0};
	int fileId = 0;
	FILE* pf;
	char  ch;
	float fv = 0;
	int a, b ,c,d = 0, iv=0, i =0;

	sprintf_s(filename, 256, "./fscanfFiles/f%d.txt",fileId );

	while( (pf =  fopen(filename,"r")) != NULL){
		switch(fileId){
			case 0:
				fscanf_s(pf, "%d:%d:%d", &a, &b, &c);
				assert(a == 2006 && b == 3 && c ==18);
				break;

			case 1:
				fscanf_s(pf, "%2s%3s", dest, DEST_BUFFER_SIZE,dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "12") == 0);
				assert(strcmp(dest2, "345") == 0);
				break;

			case 2:
				fscanf_s(pf, "%*s%s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "world") == 0);
				break;
			case 3:
				fscanf_s(pf, "%127[^:]:%127[^ ]", dest, DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "wpc") == 0);
				assert(strcmp(dest2, "123456") == 0);
				break;
			case 4:
				fscanf_s(pf, "%4x", &iv);
				assert(iv == 1);
				break;
			case 5:
				fscanf_s(pf, "%x", &iv);
				assert(iv == 26);
				break;
			case 6:
				fscanf_s(pf, "%4s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "1234") == 0);
				break;
			case 7:
				fscanf_s(pf, "%4s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "a") == 0);
				break;
			case 8:
				fscanf_s(pf, "%[1-9a-z]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 9:
				fscanf_s(pf, "%[1-9A-Z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 10:
				fscanf_s(pf, "%[^A-Z]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 11:
				fscanf_s(pf, "%*[^/]/%[^@]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "12DDWDFF") == 0);
				break;
			case 12:
				fscanf_s(pf, "%*s%s",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "world") == 0);
				break;
			case 13:
				fscanf_s(pf,"%*s%d",&d);            /*Can Parse Correctly*/
				assert(d == 2);
				break;
			case 14:
				fscanf_s(pf,"parent%d",&d);            /* result 25 returned*/
				assert(d == 25);
				break;
			case 15:
				fscanf_s(pf,"%*[a-z]%1d",&d);    /*result 2 returned*/
				assert(d == 2);
				break;
			case 16:
				fscanf_s(pf,"%*[^/]/%d",&d);        /*result 35 returned*/
				assert(d == 35);
				break;
			case 17:
				fscanf_s(pf, "%*[^/]/%[^@]", dest,DEST_BUFFER_SIZE); 
				assert(strcmp(dest, "12DDWDFF") == 0);
				break;
			case 18:
				fscanf_s(pf,"%s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 19:
				fscanf_s(pf,"%4s",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "1234") == 0);
				break;
			case 20:
				fscanf_s(pf,"%[^ ]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 21:
				fscanf_s(pf, "%[1-9a-z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 22:
				fscanf_s(pf,"%[^A-Z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 23:
				fscanf_s(pf, "%80s", dest, DEST_BUFFER_SIZE ); 
				assert(strcmp(dest, "15") == 0);
				break;
			case 24:
				fscanf_s(pf,  "%c", &ch, sizeof(char) ); 
				assert(ch == '1');
				break;
			case 25:
				fscanf_s(pf,  "%d", &i );  
				assert(i == 15);
				break;
			case 26:
				fscanf_s(pf,  "%f", &fv ); 
				assertFloatEqu(fv, 15.0f);
				break;
			case 27:
				fscanf_s(pf,  "%[0-9,:]-%[0-9,:]", dest,DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "2006:03:18") == 0);
				assert(strcmp(dest2, "2006:04:18") == 0);
				break;
			case 28:
				break;

		}
		fclose(pf);

		fileId ++;
		sprintf_s(filename, 256, "./fscanfFiles/f%d.txt",fileId );
	}

	printf("fscanf test %d files\n", fileId);
	
}

void test_fwscanf(void)
{
	FILE *stream;
	long l;
	float fp;
	wchar_t s[81];
	wchar_t c;
	
	if( (stream = fopen( "./fwscanf.out", "wb+" )) == NULL )
	{
		printf( "The file fwscanf.out was not opened\n" );
	}
	else
	{
	
		l = fwrite(L"a-string 65000 3.14159x",24 * sizeof(wchar_t), 1, stream);
	
		// Set pointer to beginning of file:
		if ( fseek( stream, 0L, SEEK_SET ) )
		{
			wprintf( L"fseek failed\n" );
		}
		
		// Read data back from file:
		fwscanf_s( stream, L"%s", s, 81 );
		fwscanf_s( stream, L"%ld", &l );
		
		fwscanf_s( stream, L"%f", &fp );
		fwscanf_s( stream, L"%c", &c, 1 );
		
		assert( my_wcscmp(s, L"a-string") == 0);
		assert(l == 65000);
		assertFloatEqu(fp, 3.14159F);
		assert(c == 'x');

		fclose( stream );
		
	}
}

int indirect_fscanf(FILE* file, const char* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vfscanf_s(file, format, args ) ;
	va_end(args);
	return ret;
	
}

void test_vfscanf(void)
{

	char filename[256];
	char dest[DEST_BUFFER_SIZE] = {0};
	char dest2[DEST_BUFFER_SIZE] = {0};
	int fileId = 0;
	FILE* pf;
	char  ch;
	float fv = 0;
	int a, b ,c,d = 0, iv=0, i =0;

	sprintf_s(filename, 256, "./fscanfFiles/f%d.txt",fileId );

	while( (pf =  fopen(filename,"r")) != NULL){
		switch(fileId){
			case 0:
				indirect_fscanf(pf, "%d:%d:%d", &a, &b, &c);
				assert(a == 2006 && b == 3 && c ==18);
				break;

			case 1:
				indirect_fscanf(pf, "%2s%3s", dest, DEST_BUFFER_SIZE,dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "12") == 0);
				assert(strcmp(dest2, "345") == 0);
				break;

			case 2:
				indirect_fscanf(pf, "%*s%s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "world") == 0);
				break;
			case 3:
				indirect_fscanf(pf, "%127[^:]:%127[^ ]", dest, DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "wpc") == 0);
				assert(strcmp(dest2, "123456") == 0);
				break;
			case 4:
				indirect_fscanf(pf, "%4x", &iv);
				assert(iv == 1);
				break;
			case 5:
				indirect_fscanf(pf, "%x", &iv);
				assert(iv == 26);
				break;
			case 6:
				indirect_fscanf(pf, "%4s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "1234") == 0);
				break;
			case 7:
				indirect_fscanf(pf, "%4s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "a") == 0);
				break;
			case 8:
				indirect_fscanf(pf, "%[1-9a-z]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 9:
				indirect_fscanf(pf, "%[1-9A-Z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 10:
				indirect_fscanf(pf, "%[^A-Z]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 11:
				indirect_fscanf(pf, "%*[^/]/%[^@]", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "12DDWDFF") == 0);
				break;
			case 12:
				indirect_fscanf(pf, "%*s%s",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "world") == 0);
				break;
			case 13:
				indirect_fscanf(pf,"%*s%d",&d);            /*Can Parse Correctly*/
				assert(d == 2);
				break;
			case 14:
				indirect_fscanf(pf,"parent%d",&d);            /* result 25 returned*/
				assert(d == 25);
				break;
			case 15:
				indirect_fscanf(pf,"%*[a-z]%1d",&d);    /*result 2 returned*/
				assert(d == 2);
				break;
			case 16:
				indirect_fscanf(pf,"%*[^/]/%d",&d);        /*result 35 returned*/
				assert(d == 35);
				break;
			case 17:
				indirect_fscanf(pf, "%*[^/]/%[^@]", dest,DEST_BUFFER_SIZE); 
				assert(strcmp(dest, "12DDWDFF") == 0);
				break;
			case 18:
				indirect_fscanf(pf,"%s", dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 19:
				indirect_fscanf(pf,"%4s",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "1234") == 0);
				break;
			case 20:
				indirect_fscanf(pf,"%[^ ]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456") == 0);
				break;
			case 21:
				indirect_fscanf(pf, "%[1-9a-z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 22:
				indirect_fscanf(pf,"%[^A-Z]",dest, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "123456abcdedf") == 0);
				break;
			case 23:
				indirect_fscanf(pf, "%80s", dest, DEST_BUFFER_SIZE ); 
				assert(strcmp(dest, "15") == 0);
				break;
			case 24:
				indirect_fscanf(pf,  "%c", &ch, sizeof(char) ); 
				assert(ch == '1');
				break;
			case 25:
				indirect_fscanf(pf,  "%d", &i );  
				assert(i == 15);
				break;
			case 26:
				indirect_fscanf(pf,  "%f", &fv ); 
				assertFloatEqu(fv, 15.0f);
				break;
			case 27:
				indirect_fscanf(pf,  "%[0-9,:]-%[0-9,:]", dest,DEST_BUFFER_SIZE, dest2, DEST_BUFFER_SIZE);
				assert(strcmp(dest, "2006:03:18") == 0);
				assert(strcmp(dest2, "2006:04:18") == 0);
				break;
			case 28:
				break;

		}
		fclose(pf);

		fileId ++;
		sprintf_s(filename, 256, "./fscanfFiles/f%d.txt",fileId );
	}

	printf("vfscanf test %d files\n", fileId);
	
}

int indirect_fwscanf(FILE* f, const wchar_t* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vfwscanf_s(f, format, args ) ;
	va_end(args);
	return ret;
	
}
void test_vfwscanf(void)
{
	FILE *stream;
	long l;
	float fp;
	wchar_t s[81];
	wchar_t c;
	
	if( (stream = fopen( "./vfwscanf.out", "wb+" )) == NULL )
	{
		wprintf( L"The file vfwscanf.out was not opened\n" );
	}
	else
	{

		l = fwrite(L"a-string 65000 3.14159x", 24 * sizeof(wchar_t), 1, stream);
	
		// Set pointer to beginning of file:
		if ( fseek( stream, 0L, SEEK_SET ) )
		{
			wprintf( L"fseek failed\n" );
		}
		
		// Read data back from file:
		indirect_fwscanf(stream,L"%s %ld %f%c", s, 81,  &l, &fp,  &c, 1 );	

		assert( my_wcscmp(s, L"a-string") == 0);
		assert(l == 65000);
		assertFloatEqu(fp, 3.14159F);
		assert(c == 'x');
		
		fclose( stream );
		
	}
}

