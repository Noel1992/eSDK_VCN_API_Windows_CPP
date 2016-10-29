
#include "securec.h"
#include "unittest.h"
#include "testutil.h"

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

#define DEST_BUFFER_SIZE  20
#define SRC_BUFFER_SIZE  200


#ifndef INT_MIN
	#define INT_MIN     (-2147483648) /* minimum (signed) int value */
#endif

#define BUFFER_SIZE 3
#define BIG_BUFFER_SIZE 256
#define MATH_PI 3.1415926

void assertMeetExpectedStr(const char* formattedStr, const char* expected, int funcRet, int lineId )
{
	int destLen = 0;
	char* errorInfo = NULL;

	if ( funcRet != slen(expected) ||  0 != strcmp(formattedStr, expected) ) {
		destLen = slen(formattedStr) + slen(expected) + 30;
		errorInfo = (char*)malloc(destLen);
		
		if (errorInfo){
			sprintf_s(errorInfo,destLen, "Out: %s; expected: %s; at: %d", formattedStr, expected, lineId -1 );
			printf("%s\r\n", errorInfo);
			assert(0);
			free(errorInfo);
		}
	}

}

void assertMeetExpectedWstr(const wchar_t* formattedStr, const wchar_t* expected, int funcRet, int lineId )
{
	int destLen = 0;
	wchar_t* errorInfo = NULL;

	if ( funcRet != wslen(expected) ||  0 != my_wcscmp(formattedStr, expected) ) {
		destLen = wslen(formattedStr) + wslen(expected) + 30;
		errorInfo = (wchar_t*)malloc(destLen * sizeof(wchar_t));
		
		if (errorInfo){
			swprintf_s(errorInfo,destLen, L"Out: %s; expected: %s; at: %d", formattedStr, expected, lineId -1 );
			wprintf(L"%s\r\n", errorInfo);
			assert(0);
			free(errorInfo);
		}
	}

}

void test_sprintf_s_basic(void)
{
	char bigBuf[BIG_BUFFER_SIZE];
	int iv = 0;
	char d[2048] = {0};
	char *s = "asda";
	double ddd = 1234.5678;
	 int aaa = -12345678;
	 char buf[128] = {0};
	 size_t sizeVal = 123456;

	int ret = 0, result;

	result = sprintf_s(d, 2048, "%s...%10s...%-10s...%10.5s...%-10.5s===\r\n", s, s, s, s, s); 
	printf("length is %d, content is \r\n%s\r\n", result, d);
	

	result = sprintf_s(d, 2048, "%0.*f===\r\n", 2, ddd); 
	printf("length is %d, content is \r\n%s\r\n", result, d);

	result = sprintf_s(d, 2048, "%0.*f===\r\n", -1, ddd);
	printf("length is %d, content is \r\n%s\r\n", result, d);


	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "abc %n", &iv);
	assert(ret == -1);
	assert( 0 == strcmp(bigBuf, "") );

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%.8g", 3.141592654);
	assertMeetExpectedStr(bigBuf, "3.1415927", ret, __LINE__);

	    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", 3.141592654);
	assertMeetExpectedStr(buf, "3.1415927", ret, __LINE__);
	

    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", 3.1415926);
    assertMeetExpectedStr(buf, "3.1415926", ret, __LINE__);
	
    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", (double)90);
     assertMeetExpectedStr(buf, "90", ret, __LINE__);
	 
    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", 90.1);
    assertMeetExpectedStr(buf, "90.1", ret, __LINE__);
	


	memset(buf, 0, sizeof(buf));
	ret = sprintf_s(buf, sizeof(buf), "%g\n", 3.14);
	assertMeetExpectedStr(buf, "3.14\n", ret, __LINE__);

	memset(buf, 0, sizeof(buf));
	ret = sprintf_s(buf, sizeof(buf), "%g\n", 3.14e+5);
	assertMeetExpectedStr(buf, "314000\n", ret, __LINE__);

	memset(buf, 0, sizeof(buf));
	ret = sprintf_s(buf, sizeof(buf), "%g\n", 3.14e-5);
#ifdef COMPATIBLE_LINUX_FORMAT
	assertMeetExpectedStr(buf, "3.14e-05\n", ret, __LINE__);
#else
	assertMeetExpectedStr(buf, "3.14e-005\n", ret, __LINE__);
#endif

	memset(buf, 0, sizeof(buf));
	ret = sprintf_s(buf, sizeof(buf), "%g\n", 3.14e+100);
	assertMeetExpectedStr(buf, "3.14e+100\n", ret, __LINE__);

	memset(buf, 0, sizeof(buf));
	ret = sprintf_s(buf, sizeof(buf), "%g\n", 3.0);
	assertMeetExpectedStr(buf, "3\n", ret, __LINE__);

    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", 1.23e+1);
    assertMeetExpectedStr(buf, "12.3", ret, __LINE__);
	
    memset(buf, 0, sizeof(buf));
    ret = sprintf_s(buf,128, "%.8g", 1.23e+100);
    assertMeetExpectedStr(buf, "1.23e+100", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%s", "abc123");
	assertMeetExpectedStr(bigBuf, "abc123", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "header: %s", "abc123");
	assertMeetExpectedStr(bigBuf, "header: abc123", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "header: %.2s", "abc123");
	assertMeetExpectedStr(bigBuf, "header: ab", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%d", INT_MAX);
	assertMeetExpectedStr(bigBuf, "2147483647", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%lld", 120259084288UL);
	assertMeetExpectedStr(bigBuf, "120259084288", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zd", 2147483647);
	assertMeetExpectedStr(bigBuf, "2147483647", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zi", 2147483647);
	assertMeetExpectedStr(bigBuf, "2147483647", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zd", -2147483648);
	assertMeetExpectedStr(bigBuf, "-2147483648", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zi", -2147483648);
	assertMeetExpectedStr(bigBuf, "-2147483648", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zu", 4294967295);
	assertMeetExpectedStr(bigBuf, "4294967295", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zx", 0xffffffff);
	assertMeetExpectedStr(bigBuf, "ffffffff", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zo", 037777777777);
	assertMeetExpectedStr(bigBuf, "37777777777", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%llu", 18446744073709551611UL);
	assertMeetExpectedStr(bigBuf, "18446744073709551611", ret, __LINE__);

#ifdef _WIN64
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%I64d", 120259084288UL);
	assertMeetExpectedStr(bigBuf, "120259084288", ret, __LINE__);
#endif
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%i", INT_MAX);
	assertMeetExpectedStr(bigBuf, "2147483647", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%i", INT_MIN);
	assertMeetExpectedStr(bigBuf, "-2147483648", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%u", INT_MIN);
	assertMeetExpectedStr(bigBuf, "2147483648", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%u",  -1 / 2 -1);
	assertMeetExpectedStr(bigBuf, "4294967295", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%o",  0xFF);
	assertMeetExpectedStr(bigBuf, "377", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%x", INT_MAX);
	assertMeetExpectedStr(bigBuf, "7fffffff", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "0X%X", INT_MAX);
	assertMeetExpectedStr(bigBuf, "0X7FFFFFFF", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%e", MATH_PI);
#ifdef COMPATIBLE_LINUX_FORMAT
	assertMeetExpectedStr(bigBuf, "3.141593e+00", ret, __LINE__);
#else
	assertMeetExpectedStr(bigBuf, "3.141593e+000", ret, __LINE__);
#endif
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%E", MATH_PI);
	#ifdef COMPATIBLE_LINUX_FORMAT
	assertMeetExpectedStr(bigBuf, "3.141593E+00", ret, __LINE__);
#else
	assertMeetExpectedStr(bigBuf, "3.141593E+000", ret, __LINE__);
#endif
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%g", MATH_PI);
	assertMeetExpectedStr(bigBuf, "3.14159", ret, __LINE__);

		ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%f", -MATH_PI);
		/* !! is different from above "3.14159" */
	assertMeetExpectedStr(bigBuf, "-3.141593", ret, __LINE__);

	/* !! bug */
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%A", MATH_PI);
	//FIXME assertMeetExpectedStr(bigBuf, "0X1.921FB5P+1", ret, __LINE__);



	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "abc %h", iv);
	assert(ret == -1);
	assert( 0 == strcmp(bigBuf, "") );

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "abc %k", iv);
	assert(ret == -1);
	assert( 0 == strcmp(bigBuf, "") );

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%p", (void*)0x1234ab);
#ifdef COMPATIBLE_LINUX_FORMAT
	if (is64Bits()){
		assertMeetExpectedStr(bigBuf, "0x1234ab", ret, __LINE__);
	}else if (is32Bits()){
		assertMeetExpectedStr(bigBuf, "0x1234ab", ret, __LINE__);
	}
#else
	if (is64Bits()){
		assertMeetExpectedStr(bigBuf, "00000000001234AB", ret, __LINE__);
	}else if (is32Bits()){
		assertMeetExpectedStr(bigBuf, "001234AB", ret, __LINE__);
	}else{
		printf("unknown dest system pointer size!!");
	}
#endif

	if (is64Bits()){ /*2014 7 31 add test cases*/
		/*test 64bits pointer*/
		ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%p", (void*)0x0012345678abLL);
		assert(ret == 12);
#ifdef COMPATIBLE_LINUX_FORMAT
		assertMeetExpectedStr(bigBuf, "0x12345678ab", ret, __LINE__);
#else
		assertMeetExpectedStr(bigBuf, "12345678AB", ret, __LINE__);
#endif

		ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%p", (void*)0x012345678abcdLL);
		assert(ret == 14);
#ifdef COMPATIBLE_LINUX_FORMAT
		assertMeetExpectedStr(bigBuf, "0x12345678abcd", ret, __LINE__);
#else
		assertMeetExpectedStr(bigBuf, "12345678ABCD", ret, __LINE__);
#endif
	}

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%p", (void*)0x12EF789);
#ifdef COMPATIBLE_LINUX_FORMAT
	if (is64Bits()){
		assertMeetExpectedStr(bigBuf, "0x12ef789", ret, __LINE__);
	}else if(is32Bits()){
		assertMeetExpectedStr(bigBuf, "0x12ef789", ret, __LINE__);
	}
#else
	if (is64Bits()){
		assertMeetExpectedStr(bigBuf, "00000000012EF789", ret, __LINE__);
	}else if(is32Bits()){
		assertMeetExpectedStr(bigBuf, "012EF789", ret, __LINE__);
	}else{
		printf("unknown dest system pointer size!!");
	}
#endif
	
	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%zd", sizeVal);
	assertMeetExpectedStr(bigBuf, "123456", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%Id", sizeVal);
	assertMeetExpectedStr(bigBuf, "123456", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%c%c", 'a', 'b');
	assertMeetExpectedStr(bigBuf, "ab", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%s %s", "security", "design");
	assertMeetExpectedStr(bigBuf, "security design", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%8d%8d", 123, 4567);
	assertMeetExpectedStr(bigBuf, "     123    4567", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%05d", 123);
	assertMeetExpectedStr(bigBuf, "00123", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%-8d%8d", 123, 4567);
	assertMeetExpectedStr(bigBuf, "123         4567", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%8x", 4567);
	assertMeetExpectedStr(bigBuf, "    11d7", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%-8X", 4568);
	assertMeetExpectedStr(bigBuf, "11D8    ", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%08X", 4567);
	assertMeetExpectedStr(bigBuf, "000011D7", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%04X", (short)-1);
	assertMeetExpectedStr(bigBuf, "FFFFFFFF", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%04X", (unsigned short)-1);
	assertMeetExpectedStr(bigBuf, "FFFF", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%10.3f", MATH_PI);
	assertMeetExpectedStr(bigBuf, "     3.142", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%-10.3f", MATH_PI);
	assertMeetExpectedStr(bigBuf, "3.142     ", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%.3f", MATH_PI);
	assertMeetExpectedStr(bigBuf, "3.142", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "asdasd%s", NULL);
	assertMeetExpectedStr(bigBuf, "asdasd(null)", ret, __LINE__);

}

void test_sprintf_s_combination(void)
{
	char bigBuf[BIG_BUFFER_SIZE];
	char a1[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	char a2[] = {'H', 'I', 'J', 'K', 'L', 'M', 'N'};
	int ret = 0;

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%d %.3f %s", 456, MATH_PI, "abcdef321");
	assertMeetExpectedStr(bigBuf, "456 3.142 abcdef321", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%.3f", MATH_PI);
	assertMeetExpectedStr(bigBuf, "3.142", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%.7s%.7s", a1, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFGHIJKLMN", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE,"%.6s%.5s", a1, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFHIJKL", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%.*s%.*s", 7, a1, 7, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFGHIJKLMN", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%-*d", 4, 'A'); 
	assertMeetExpectedStr(bigBuf, "65  ", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE,"%#0*X", 8, 128);
	assertMeetExpectedStr(bigBuf, "0X000080", ret, __LINE__);

	ret = sprintf_s(bigBuf, BIG_BUFFER_SIZE, "%*.*f", 10, 2, 3.1415926);
	assertMeetExpectedStr(bigBuf, "      3.14", ret, __LINE__);
}

int indirect_sprintf(char *string, size_t sizeInBytes, const char *format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vsprintf_s(string, sizeInBytes, format, args );
	va_end(args);
	return ret;
	
}
void test_vsprintf_s(void)
{
	char bigBuf[BIG_BUFFER_SIZE];
	char a1[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	char a2[] = {'H', 'I', 'J', 'K', 'L', 'M', 'N'};
	int ret = 0;

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%d %.3f %s", 456, MATH_PI, "abcdef321");
	assertMeetExpectedStr(bigBuf, "456 3.142 abcdef321", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%.3f", MATH_PI);
	assertMeetExpectedStr(bigBuf, "3.142", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%.7s%.7s", a1, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFGHIJKLMN", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE,"%.6s%.5s", a1, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFHIJKL", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%.*s%.*s", 7, a1, 7, a2);
	assertMeetExpectedStr(bigBuf, "ABCDEFGHIJKLMN", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%-*d", 4, 'A'); 
	assertMeetExpectedStr(bigBuf, "65  ", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE,"%#0*X", 8, 128);
	assertMeetExpectedStr(bigBuf, "0X000080", ret, __LINE__);

	ret = indirect_sprintf(bigBuf, BIG_BUFFER_SIZE, "%*.*f", 10, 2, 3.1415926);
	assertMeetExpectedStr(bigBuf, "      3.14", ret, __LINE__);
}

void test_sprintf_s_boundaryCondition(void)
{
	char buf[BUFFER_SIZE];
	int ret = 0;

	ret = sprintf_s(buf, BUFFER_SIZE, "%s", "abc");
	assert(ret == -1);

	ret = sprintf_s(buf, BUFFER_SIZE, "%d", 123);
	assert(ret == -1);

	ret = sprintf_s(buf, BUFFER_SIZE, "%d", -12);
	assert(ret == -1);

	ret = sprintf_s(buf, BUFFER_SIZE, "%d", -123);
	assert(ret == -1);

	ret = sprintf_s(buf, BUFFER_SIZE, "%u", 123);
	assert(ret == -1);

}

void test_swprintf_s_combination(void)
{
	wchar_t bigBuf[BIG_BUFFER_SIZE];
	wchar_t a1[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	wchar_t a2[] = {'H', 'I', 'J', 'K', 'L', 'M', 'N'};
	int ret = 0;

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"asdasd%s", NULL);
	assertMeetExpectedWstr(bigBuf, L"asdasd(null)", ret, __LINE__);


	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%d", 456);
	assertMeetExpectedWstr(bigBuf, L"456", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%d %.3f %s", 456, MATH_PI, L"abcdef321");
	assertMeetExpectedWstr(bigBuf, L"456 3.142 abcdef321", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%.3f", MATH_PI);
	assertMeetExpectedWstr(bigBuf, L"3.142", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%.7s%.7s", a1, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFGHIJKLMN", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%.6s%.5s", a1, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFHIJKL", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%.*s%.*s", 7, a1, 7, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFGHIJKLMN", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%-*d", 4, 'A'); 
	assertMeetExpectedWstr(bigBuf, L"65  ", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE,L"%#0*X", 8, 128);
	assertMeetExpectedWstr(bigBuf, L"0X000080", ret, __LINE__);

	ret = swprintf_s(bigBuf, BIG_BUFFER_SIZE, L"%*.*f", 10, 2, 3.1415926);
	assertMeetExpectedWstr(bigBuf, L"      3.14", ret, __LINE__);

}

int indirect_swprintf(wchar_t *string, size_t sizeInWords, const wchar_t* format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vswprintf_s(string, sizeInWords, format, args ) ;
	va_end(args);
	return ret;
	
}
void test_vswprintf_s(void)
{
	wchar_t bigBuf[BIG_BUFFER_SIZE];
	wchar_t a1[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};
	wchar_t a2[] = {'H', 'I', 'J', 'K', 'L', 'M', 'N'};

	int iv = 0;

	int ret = 0;

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%d", 456);
	assertMeetExpectedWstr(bigBuf, L"456", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%d %.3f %s", 456, MATH_PI, L"abcdef321");
	assertMeetExpectedWstr(bigBuf, L"456 3.142 abcdef321", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%.3f", MATH_PI);
	assertMeetExpectedWstr(bigBuf, L"3.142", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%.7s%.7s", a1, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFGHIJKLMN", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%.6s%.5s", a1, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFHIJKL", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%.*s%.*s", 7, a1, 7, a2);
	assertMeetExpectedWstr(bigBuf, L"ABCDEFGHIJKLMN", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%-*d", 4, 'A'); 
	assertMeetExpectedWstr(bigBuf, L"65  ", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE,L"%#0*X", 8, 128);
	assertMeetExpectedWstr(bigBuf, L"0X000080", ret, __LINE__);

	ret = indirect_swprintf(bigBuf, BIG_BUFFER_SIZE, L"%*.*f", 10, 2, 3.1415926);
	assertMeetExpectedWstr(bigBuf, L"      3.14", ret, __LINE__);

}


void test_snprintf_s(void)
{
	char buf[BUFFER_SIZE];
	char bigBuf[BIG_BUFFER_SIZE];
	int ret = 0;

	ret = snprintf_s(buf, BUFFER_SIZE, 2, "%s", "abc");
	assert(ret == -1);
	assert(0 == strcmp(buf, "ab") );

#if (defined(_MSC_VER) && (_MSC_VER >= 1400)) || (defined(__GNUC__) && (__GNUC__ > 3))
	ret = COMP_SNPRINTF(buf, BUFFER_SIZE, 2, "%s", "abc");
	assert(ret == 2);
	assert(0 == strcmp(buf, "ab") );

	ret = COMP_SNPRINTF(bigBuf, BIG_BUFFER_SIZE, 20, "%s %d", "abc", 345);
	assert(ret == 7);
	assert(0 == strcmp(bigBuf, "abc 345") );
#endif

	ret = snprintf_s(bigBuf, BIG_BUFFER_SIZE, 2, "%d", 10000);
	assert(ret == -1);
	assert(0 == strcmp(bigBuf, "10") );

	ret = snprintf_s(bigBuf, BIG_BUFFER_SIZE, 3, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = snprintf_s(bigBuf, BIG_BUFFER_SIZE, 5, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = snprintf_s(bigBuf, 10, 20, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);


	ret = snprintf_s(bigBuf, BIG_BUFFER_SIZE, sizeof(bigBuf) - 1, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = snprintf_s(bigBuf, 4, 3, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = snprintf_s(buf, 3, 3, "%s", "abc");
	assert(ret == -1);
	assert(0 == strcmp(buf, "") );
}

int indirect_snprintf(char *string, size_t sizeInBytes, size_t count, const char *format, ...)
{
	va_list args;
	int ret = 0;
	
	va_start( args, format );
	ret = vsnprintf_s(string, sizeInBytes, count, format, args ) ;
	va_end(args);
	return ret;
	
}

void test_vsnprintf_s(void)
{
	char buf[BUFFER_SIZE];
	char bigBuf[BIG_BUFFER_SIZE];
	int ret = 0;

	ret = indirect_snprintf(buf, BUFFER_SIZE, 2, "%s", "abc");
	assert(ret == -1);
	assert(0 == strcmp(buf, "ab") );

	ret = indirect_snprintf(bigBuf, BIG_BUFFER_SIZE, 3, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = indirect_snprintf(bigBuf, BIG_BUFFER_SIZE, 5, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = indirect_snprintf(bigBuf, 10, 20, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);


	ret = indirect_snprintf(bigBuf, BIG_BUFFER_SIZE, sizeof(bigBuf) - 1, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = indirect_snprintf(bigBuf, 4, 3, "%s", "abc");
	assertMeetExpectedStr(bigBuf, "abc", ret, __LINE__);

	ret = indirect_snprintf(buf, 3, 3, "%s", "abc");
	assert(ret == -1);
	assert(0 == strcmp(buf, "") );
}
