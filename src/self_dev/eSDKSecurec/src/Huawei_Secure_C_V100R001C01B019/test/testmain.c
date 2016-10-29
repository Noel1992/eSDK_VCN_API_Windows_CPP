

/******************************************************************************

Copyright (C), 2001-2012, Huawei Tech. Co., Ltd.

******************************************************************************
File Name     :
Version       :
Author        :
Created       : 2010/9/1
Last Modified :
Description   :
Function List :

History       :
1.Date        : 2010/9/1
Author      :
Modification: Created file

******************************************************************************/


#include "securec.h"
#include "unittest.h"
//#include "testutil.h"
#include <assert.h>
#include <string.h>
#include <locale.h>
#include "testutil.h"


void Printf( const char *pcChar,...)
{
#define MEM_BLOCK_SIZE 128

	va_list  argptr;
	char buf[MEM_BLOCK_SIZE];

	va_start(argptr, pcChar);
	vsnprintf_s(buf, MEM_BLOCK_SIZE - 1, MEM_BLOCK_SIZE - 1, pcChar, argptr);

	printf(buf);
	va_end(argptr);

}


#define COMP_STRCPY(p, dSize, s) (strcpy_s(p, dSize, s) == EOK ? p : NULL)
#define BUFFER_SIZE 128
/*
#define VSNPRINTF_S(strDest, destMax, count,format, arglist) vsnprintf_s(strDest, destMax, count,format, arglist)== -1 ? ( strlen(strDest) == count ? count : 0 ) :  -1;
#define VSNPRINTF_S(strDest, destMax, count,format, arglist) vsnprintf_s(strDest, destMax, count,format, arglist) > 0  ? ( strlen(strDest) == count ? count : 0 ) :  -1;
*/

void PPrintf( const char *format,...)
{

	va_list  argptr;
	va_start(argptr, format);

	printf("%s", format);
	va_end(argptr);

}

void testBigEndian(void)
{
	#define DEST_BUFFER_SIZE  20
	#define SRC_BUFFER_SIZE  200

	char dest[DEST_BUFFER_SIZE] = {0};
	char dest2[DEST_BUFFER_SIZE] = {0};
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	char inputStr[] = "123 567.4 asdfqwer";


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
	float fp = 0.0f;
	double dblVal;


	int j = 0;
	int inumber = 9000;
	int iRet = 0;
	UINT64T val64;
	
	conv = sscanf_s("3.456789","%lg",&dblVal);
	assert(conv == 1);
	assertFloatEqu((float)dblVal, 3.456789f);
	printf("%f\n", dblVal);

	conv = sscanf_s("3.456789","%Lg",&dblVal);
	assert(conv == 1);
	assertFloatEqu((float)dblVal, 3.456789f);
	printf("%f\n", dblVal);
	
	conv = sscanf_s("34.12454","%f",&fv);
	assertFloatEqu(fv, 34.12454f);
	printf("%f\n", fv);

	conv = sscanf_s("9223372036854775807", "%lld", &val64);
	assert(conv == 1);
	printf("%s   vs   %f\n", "9223372036854775807", (double)val64);

	conv = sscanf_s( "3.1415gh", "%f", &fv );
	assert(conv == 1);
	assertFloatEqu(fv, 3.1415f);
	printf("%f\n", fv);
	

	conv = sscanf_s("2345678.987654", "%f",  &fv);
	assert(conv == 1);
	printf("%f\n", fv);
	

	conv = sscanf_s("32145.1415926", "%f",  &fv);
	assert(conv == 1);
	printf("%f\n", fv);
	
	
	conv = sscanf_s("123 567.4 asdfqwer", "%d%f%s", &iv, &fv, dest,DEST_BUFFER_SIZE);
	assert(conv == 3);
	
	assert( strcmp(dest, "asdfqwer") == 0);
	assert( iv == 123 );
	assertFloatEqu(fv, 567.4f);
	printf("%d\n", iv);
	printf("%f\n", fv);
		

	sscanf_s("31.415926358", "%f",&fv);
	assertFloatEqu(fv, 31.415926358f);
	printf("%f\n", fv);

	sscanf_s("31.415926358", "%4f",&fv);
	assertFloatEqu(fv, 31.4f);
	printf("%f\n", fv);

	sscanf_s("31.415926358", "%6f",&fv);
	assertFloatEqu(fv, 31.415f);
	printf("%f\n", fv);

	sscanf_s("2147483647", "%d",&d);
	assert(d == 2147483647);
	printf("%d\n", d);

	sscanf_s("-2147483648", "%d",&d);
	assert(d == -2147483648L);
	printf("%d\n", d);

	sscanf_s("1234567890", "%1s", dest, DEST_BUFFER_SIZE);
	assert(strcmp(dest, "1") == 0);

	sscanf_s("2006:03:18", "%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);
	printf("%d : %d : %d\n", a, b, c);
	
	
	sscanf_s("0001A", "%x", &iv);
	assert(iv == 26);
	printf("%d\n", iv);

	sscanf_s("parent25","parent%d",&d);            /* result 25 returned*/
	assert(d == 25);
	printf("%d\n", d);

	ret = sscanf_s("parent2","%*[a-z]%d",&d);      /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);
	printf("%d\n", d);
	
	ret =  sscanf_s("parent2parent","%*[a-z]%d",&d);    /*Parse Correctly use WildCard*/
	assert(ret == 1);
	assert(d == 2);
	printf("%d\n", d);
	
	sscanf_s("parent22parent","%*[a-z]%1d",&d);    /*result 2 returned*/
	assert(d == 2);
	printf("%d\n", d);
	
	sscanf_s("asd/35@32","%*[^/]/%d",&d);        /*result 35 returned*/
	assert(d == 35);
	printf("%d\n", d);

}

int sscanfCompareD()
{

    int ret = 0;
    int test_i = 0;
    unsigned long long test_ull = 0;
    unsigned int test_ui = 0;
    unsigned long  test_ul = 0;
    long  test_l = 0;
    long long test_ll = 0;

	printf("-------------/u--------------\n");

    printf("\nmax int result(\%/u):\n");
    ret = sscanf_s("4294967295", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);
    ret = sscanf("4294967295", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);
    
    printf("\nmax int result(\%/u):\n");
    ret = sscanf_s("4294967296", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);
    ret = sscanf("4294967296", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);

    printf("\nmax int result(\%/u):\n");
    ret = sscanf_s("-1", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);
    ret = sscanf("-1", "%u", &test_ui);
    printf("%u--%d!\n", test_ui, ret);

    printf("\nmax int result(\%/lu):\n");
    ret = sscanf_s("18446744073709551616", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);
    ret = sscanf("18446744073709551616", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);

    printf("\nmax int result(\%/lu):\n");
    ret = sscanf_s("4294967296", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);
    ret = sscanf("4294967296", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);

	printf("\nmax int result(\%/lu):\n");
    ret = sscanf_s("-1", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);
    ret = sscanf("-1", "%lu", &test_ul);
    printf("%lu--%d!\n", test_ul, ret);

	printf("\nmax int result(\%/llu):\n");
    ret = sscanf_s("18446744073709551615", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);
    ret = sscanf("18446744073709551615", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);

	printf("\nmax int result(\%/llu):\n");
    ret = sscanf_s("18446744073709551616", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);
    ret = sscanf("18446744073709551616", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);

	printf("\nmax int result(\%/llu):\n");
    ret = sscanf_s("18446744073709551617", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);
    ret = sscanf("18446744073709551617", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);

	printf("\nmax int result(\%/llu):\n");
    ret = sscanf_s("-1", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);
    ret = sscanf("-1", "%llu", &test_ull);
    printf("%llu--%d!\n", test_ull, ret);

	printf("-------------/x--------------\n");

	printf("\nmax int result(\%/x):\n");
    ret = sscanf_s("ffffffff", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);
    ret = sscanf("ffffffff", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);

	printf("\nmax int result(\%/x):\n");
    ret = sscanf_s("7fffffff", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);
    ret = sscanf("7fffffff", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);

	printf("\nmax int result(\%/x):\n");
    ret = sscanf_s("80000000", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);
    ret = sscanf("80000000", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);

	printf("\nmax int result(\%/x):\n");
    ret = sscanf_s("80000001", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);
    ret = sscanf("80000001", "%x", &test_i);
    printf("%lld--%d!\n", (long long)test_i, ret);

	printf("\nmax int result(\%/x):\n");
    ret = sscanf_s("-1", "%x", &test_ui);
    printf("%x--%d!\n", test_ui, ret);
    ret = sscanf("-1", "%x", &test_ui);
    printf("%x--%d!\n", test_ui, ret);

 
    printf("\nmax int result(\%/llx):\n");
    ret = sscanf_s("7fffffffffffffff", "%llx", &test_ll);
    printf("%lld--%d!\n", test_ll, ret);
    ret = sscanf("7fffffffffffffff", "%llx", &test_ll);
    printf("%lld--%d!\n", test_ll, ret);

    printf("\nmax int result(\%/lx):\n");
    ret = sscanf_s("8000000000000000", "%llx", &test_ll);
    printf("%lld--%d!\n", test_ll, ret);
    ret = sscanf("8000000000000000", "%llx", &test_ll);
    printf("%lld--%d!\n", test_ll, ret);


    printf("\nmax int result(\%/lx):\n");
    ret = sscanf_s("-1", "%llx", &test_ll);
    printf("%lx--%d!\n", test_ll, ret);
    ret = sscanf("-1", "%llx", &test_ll);
    printf("%lx--%d!\n", test_ll, ret);

	printf("\nmax int result(\%/llx):\n");
    ret = sscanf_s("ffffffffffffffff", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);
    ret = sscanf("ffffffffffffffff", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);

	printf("\nmax int result(\%/llx):\n");
    ret = sscanf_s("10000000000000000", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);
    ret = sscanf("10000000000000000", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);

	printf("\nmax int result(\%/llx):\n");
    ret = sscanf_s("-1", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);
    ret = sscanf("-1", "%llx", &test_ll);
    printf("%llx--%d!\n", test_ll, ret);

	printf("-------------/X--------------\n");

	printf("\nmax int result(\%/X):\n");
    ret = sscanf_s("ffffffff", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);
    ret = sscanf("ffffffff", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);

	printf("\nmax int result(\%/X):\n");
    ret = sscanf_s("100000000", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);
    ret = sscanf("100000000", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);

	printf("\nmax int result(\%/X):\n");
    ret = sscanf_s("-1", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);
    ret = sscanf("-1", "%X", &test_ui);
    printf("%X--%d!\n", test_ui, ret);

    printf("\nmax int result(\%/lX):\n");
    ret = sscanf_s("10000000000000000", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);
    ret = sscanf("10000000000000000", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);

    printf("\nmax int result(\%/lX):\n");
    ret = sscanf_s("100000000", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);
    ret = sscanf("100000000", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);

    printf("\nmax int result(\%/lX):\n");
    ret = sscanf_s("-1", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);
    ret = sscanf("-1", "%lX", &test_ul);
    printf("%lX--%d!\n", test_ul, ret);

	printf("\nmax int result(\%/llX):\n");
    ret = sscanf_s("ffffffffffffffff", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);
    ret = sscanf("ffffffffffffffff", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);

	printf("\nmax int result(\%/llX):\n");
    ret = sscanf_s("10000000000000000", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);
    ret = sscanf("10000000000000000", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);

	printf("\nmax int result(\%/llX):\n");
    ret = sscanf_s("-1", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);
    ret = sscanf("-1", "%llX", &test_ll);
    printf("%llX--%d!\n", test_ll, ret);

	printf("-------------/o--------------\n");

	printf("\nmax int result(\%/o):\n");
    ret = sscanf_s("37777777777", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);
    ret = sscanf("37777777777", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);

	printf("\nmax int result(\%/o):\n");
    ret = sscanf_s("40000000000", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);
    ret = sscanf("40000000000", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);

	printf("\nmax int result(\%/o):\n");
    ret = sscanf_s("-1", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);
    ret = sscanf("-1", "%o", &test_ui);
    printf("%o--%d!\n", test_ui, ret);

    printf("\nmax int result(\%/lo):\n");
    ret = sscanf_s("2000000000000000000000", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);
    ret = sscanf("2000000000000000000000", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);

    printf("\nmax int result(\%/lo):\n");
    ret = sscanf_s("40000000000", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);
    ret = sscanf("40000000000", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);

    printf("\nmax int result(\%/lo):\n");
    ret = sscanf_s("-1", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);
    ret = sscanf("-1", "%lo", &test_ul);
    printf("%lo--%d!\n", test_ul, ret);

	printf("\nmax int result(\%/llo):\n");
    ret = sscanf_s("1777777777777777777777", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);
    ret = sscanf("1777777777777777777777", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);

	printf("\nmax int result(\%/llo):\n");
    ret = sscanf_s("2000000000000000000000", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);
    ret = sscanf("2000000000000000000000", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);

	printf("\nmax int result(\%/llo):\n");
    ret = sscanf_s("-1", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);
    ret = sscanf("-1", "%llo", &test_ull);
    printf("%llo--%d!\n", test_ull, ret);

    return 0;
}


int main(int argc, char* argv[])
{
	int testStrcat = 1, testStrcpy = 1,testStrtok= 1;
	int testMemcpy = 1, testMemmove = 1, testMemset =1;
	int testScanf = 1, testGets = 0;
	int testSprintf = 1;
    int compareTest = 1;
	int withPerformanceTest = 0;
	//int testStrcat = 0, testStrcpy = 0,testStrtok= 0;
	//int testMemcpy = 0, testMemmove = 0, testMemset = 0;
	//int testScanf = 1, testGets = 0;
	//int testSprintf = 0;
	char* locname = NULL;

	char a1[] = {'A', 'B'};

	int ret = 0;

	char testa2[2] = {0};
    char testa1[] = "AB";
	char abc[128]={0};

	INT64T i64 = 0;
	int item = 0;
char fmtCtl[] = "%[a-z]s";
	//long double va1,va2;
const char deli[] = ",";
char src[32] = {0};
	char dst[32] = {0};
	wchar_t testStr[] = {0};
	char ptrStr[32] = "12345678 123123";
	void* pp= src;
	long double value=0.0,orginvalue=0.0;

	// char buf[]="3.456789";
	wchar_t wcc;
	char tokenstring[] = "1511.1111 12 14...";
	char tmp[32]={0};
	double sddd = 1234.5678f;
	char d[2048] = {0};
	char *s = "asda";
	double ddd = 1234.5678;
	int aaa = -12345678;
	char str[10] ="abcd";
	char* ps = NULL;
	wchar_t www[2];
	int i;
	unsigned int gg = 0x8FFFFFFF;
	unsigned short usw = 0;
//	va_list arglist;
    char hhx[BUFFER_SIZE] = {0};
    unsigned char c = 1;
	unsigned short wValue = 0x1234;
unsigned char myVal[] = {0x12, 0x34, 0x56, 0x78, 0x91, 0x32, 0x54, 0x76  };

    //sscanfCompareD();

	getHwSecureCVersion(ptrStr, 32, &usw);
	printf("Lib Version: %s, major ver = %d, minor ver = %d\n", ptrStr, (usw & 0xFF00) >> 8, usw & 0x00FF);
	i = *(int*)(myVal +1);
	
	if (*(char*)&wValue == 0x12) {
		printf("On Big-Endian\n");
		if (i != 0x34567891) {
			printf("!!!!!!!!!!!!!!!!\n you machine need multi bytes value to be aligned!\n!!!!!!!!!!!!");
		}
	}else{
		printf("On Little-Endian\n");
		if (i != 0x91785634) {
			printf("!!!!!!!!!!!!!!!!\n you machine need multi bytes value to be aligned!\n!!!!!!!!!!!!");
		}
	}

	gg += gg;
	ps = COMP_STRCPY(str, 10, "abcd123");
	gg = (unsigned int)(-1);
	usw = gg;

	ret =swscanf_s(L"31.45678", L"%s", www, 2);
	ret =sscanf_s("31.45678", "%s", tmp, 2);
	ret =sscanf_s("31.45678", "%s", tmp, 32);

	ret = sizeof(long double);
	ret = sizeof(wcc) / sizeof(wchar_t);

	ret = sizeof(long int);
	i = 0;


#define xxxx_sss 12
#if (defined(_WIN32) )
    ret = sizeof(long double);
#endif

	sscanf_s( tokenstring, "%S" , dst, sizeof(dst) );

	do{
		continue;
	}while(++ret  < 5);

	//wcsncatPerformanceTest();
	assert( sizeof(UINT8T) == 1);
	assert( sizeof(int) == 4);
	assert( sizeof(INT64T) == 8);
	assert( sizeof(UINT64T) == 8);

#ifdef SECUREC_ON_64BITS
	printf("Secure C on 64Bits\n");
#else
	printf("Secure C on 32Bits\n");
#endif

    /*compare test*/
/*  if(compareTest)
    {
        test_sscanf_compare();
        test_scanf_compare();
        test_fwscanf_compare();
    }


	ret = sscanf(ptrStr, "%p", &item);
	ret = sscanf_s(ptrStr, "%p", &item);
	wprintf(L"%s\n", testStr);
	*/
	//dst = ;
	//	strcpy_sPerformanceTest();

//  strcpy_s(dst, sizeof(dst), __FUNCTION__);
//	pp = pp + 3;
	//strcpy_s(dst, sizeof(dst), src);
//item = __STDC_VERSION__;


//ret = sscanf_s("9223372036854775808", "%s", dest, 30);

printf("\n------start test big endian------\n");
	testBigEndian();
printf("\n------end test big endian------\n\n");

	testOverlap();

	if (testSprintf) 
	{
		printf("\n------start test sprintf------\n");
		test_sprintf_s_basic();
		test_sprintf_s_combination();
		test_sprintf_s_boundaryCondition();
	
		test_swprintf_s_combination();
		test_vsprintf_s();
		test_vswprintf_s();
	
		test_snprintf_s();
		test_vsnprintf_s();
		printf("------end test sprintf  OK!------\n");
	}

	if (testStrcat){
		printf("\n------start test strcat------\n");
		test_strcat_s();
		test_strncat_s();
		test_wcscat_s();
		test_wcsncat_s();
		printf("------end test strcat  OK!------\n");

	}

	if (testStrcpy) {
		printf("\n------start test strcpy------\n");
		TestStrcpy_s();
		TestStrncpy_s();

		TestWcscpy_s();
		TestWcsncpy_s();
		printf("------end test strcpy  OK!------\n");
	}

	//strtok
	if (testStrtok) {
		printf("\n------start test strtok------\n");
		//TestStrtok_s();
		test_strtok();
		test_wcstok();
		printf("------end test strtok  OK!------\n");
	}

	//for memcpy
	if (testMemcpy) {
		printf("\n------start test memcpy------\n");
		TestMemcpy_s();
		Test_wmemcpy_s();
		printf("------end test memcpy  OK!------\n");

	}

	//for memset
	if (testMemset) {
		printf("\n------start test memset------\n");
		test_memset_s();
		printf("------end test memset  OK!------\n");
	}

	//for memmove
	if (testMemmove) {
		printf("\n------start test memmove------\n");
		memmove_s_test();
		wmemmove_s_test();
		printf("------end test memmove  OK!------\n");
	}
	

	if (testScanf) 
	{
		printf("\n------start test sscanf------\n");
		
	//	the following test cases use console which can't automaticly run
	//		test_scanf();
	//		test_vscanf();
	//		test_wscanf();
	//		test_vwscanf();

		scanf_gbk();	//LSD 2014.3.17 add
		test_sscanf();
		test_vsscanf();

#ifndef ONLY_TEST_ANSI_VER_LIB
		testSwscanf();
		test_vswscanf();
		test_fwscanf();
		test_vfwscanf();
#endif

		testfscanf();
		test_vfscanf();
		
	/*
		test_sscanf_d();
		test_sscanf_i();
*/
		printf("------end test sscanf  OK!------\n");
	}

	if (testGets) {
		
		//the following "gets_s" test cases use console which can't automaticly run
		
		printf("\n------start test gets------\n");
		test_gets_s();
		printf("------end test gets  OK!------\n\n");
	}

	//TestMemcpyPerformance();
	
	if (withPerformanceTest) {
		printf("\n------Now porformance test------\n");
		sprintfPerformanceTest();

		strcpyChunkPerformanceTest();
		wcscpyChunkPerformanceTest();
		strcatChunkPerformanceTest();
		wcscatChunkPerformanceTest();


		memcpyPerformanceTest();
		strcpyPerformanceTest();
		wcscpyPerformanceTest();
		strncpyPerformanceTest();
		wcsncpyPerformanceTest();

		strcatPerformanceTest();
		wcscatPerformanceTest();
		strncatPerformanceTest();
		wcsncatPerformanceTest();
	}

	printf("\n\n--------------------\nall tests finished!\n");
	return 0;
}

