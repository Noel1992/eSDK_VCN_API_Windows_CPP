
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

void test_wscanf(void)
{
	int ret = 0,  conv= 0,iv = 0;
	float fv = 0;
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};

	printf("do you want to test wscanf? (y/n)");
	ret = getchar();
	ret = tolower(ret);

	while (ret == 'y'){
		
		printf("please input integer, float and string\n");
		conv = wscanf_s(L"%d%f%100s", &iv, &fv, wDest, DEST_BUFFER_SIZE);
		wprintf(L"convert %d items, you input value are %d  %f %.100s \n", conv, iv, fv, wDest);
	
		printf("continue test scanf? (y, n)");
		ret = tolower( getchar() );
	}
}

void test_wscanf2(void)
{
	int ret = 0;
	int      i,     result;
	float    fp;
	char     c,
		s[80];
	wchar_t  wc, ws[80];

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
#define TEST_WSTR L"1111111111.0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999922222222223333333333444444444455555555556666666666777777777788888888889999999999"

void testSwscanf(void)
{
	wchar_t wDest[DEST_BUFFER_SIZE] = {0};
	wchar_t wBuf[48];
	int a, b ,c,d = 0, conv;
	float fv;
	const wchar_t *ws = L"蝴蝶测试工具，好不好用呢？谁用谁知道啊！";
	wchar_t wStr[] = L"123 5689.4 asdfqwer";


	swscanf_s(L"2006:03:18", L"%d:%d:%d", &a, &b, &c);
	assert(a == 2006 && b == 3 && c ==18);

	swscanf_s(L"123456 ",L"%s", wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);

	swscanf_s(L"123456 ",L"%4s",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"1234") == 0);

	swscanf_s(L"123456 abcdedf",L"%[^ ]",wDest, DEST_BUFFER_SIZE);
	assert(my_wcscmp(wDest, L"123456") == 0);

	a = swscanf_s(ws, L"%s", wBuf , sizeof(wBuf) / sizeof(wchar_t));
	assert(a == 1);
	assert(0 == my_wcscmp(wBuf, ws));

	a = swscanf_s(L"34.12454",L"%f",&fv);
	assert(a == 1);
	assertFloatEqu(fv, 34.12454f);

		conv = swscanf_s(TEST_WSTR, L"%f",  &fv);
	assertFloatEqu(fv, 1111111111.0f);
	assert(conv == 1);

	conv = swscanf_s(wStr, L"%d%f%100s", &b, &fv, wDest, DEST_BUFFER_SIZE );
	assert(conv == 3 );
	assert(my_wcscmp(wDest, L"asdfqwer") == 0);
	assert(b == 123 );
	assertFloatEqu(fv, 5689.4f);
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