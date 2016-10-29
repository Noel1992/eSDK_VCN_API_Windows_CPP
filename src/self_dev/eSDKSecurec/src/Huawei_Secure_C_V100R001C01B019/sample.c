

#include "./include/securec.h"
#include <stdio.h>
#include <wchar.h>
#include <assert.h>
#include "sample.h"

#if !defined (COUNTOF)
#define COUNTOF(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

#define LEN   ( 128 )

void scanfSample();
void sscanfSample( void );
void fscanfSample( void );

void swscanfSample( void );
void vsscanfSample( void );


void memcpySample(void);
void memmoveSample(void);
void memsetSample(void);

void strcpySample(void);
void wcscpySample(void);
void strncpySample(void);

void strcatSample(void);
void strncatSample(void);

void wcscatSample(void);
void wcsncatSample(void);


void strtokSample(void);

void sprintfSample(void);
void swprintfSample(void);
void vsprintfSample(void);

void snprintfSample(void);
void vsnprintfSample(void);


void getsSample(void);

void RunfSamples(void)
{
	scanfSample();
	sscanfSample( );
	fscanfSample(  );
	
	swscanfSample(  );
	vsscanfSample(  );
	
	
	memcpySample();
	memmoveSample();
	memsetSample();
	
	strcpySample();
	wcscpySample();
	strncpySample();
	
	strcatSample();
	strncatSample();
	
	wcscatSample();
	wcsncatSample();
	
	
	strtokSample();
	
	sprintfSample();
	swprintfSample();
	vsprintfSample();
	
	snprintfSample();
	vsnprintfSample();
	
	
	getsSample();
	
}

int mainSample(int argc, char* argv[])
{
	char dest[234];
	wchar_t wc = 'a', w2 = 'b';
	
	unsigned int k= -1 / 2 -1;

	sprintf_s(dest, 234, "%lc  %lc", wc, w2);
	printf("%s\n\n", dest);
	
	sprintf_s(dest, 234, "%lc ", 123 );
	printf("%s\n", dest);

	vsnprintfSample();
	RunfSamples();

	return 0;
}

void scanfSample()
{
	int      i,     result;
	float    fp;
	char     c,
		s[80];
	wchar_t  wc,
		ws[80];
	
	
	/* scanf_s sample */
	printf("input a integer:\n");
	result =  scanf_s( "%d", &i);
	printf("%d item converted\n", result);


	printf("%s", "input format: %d %f %c %C %s %S\n");
	result = scanf_s( "%d %f %c %C %s %S", &i, &fp, &c, 1,
		&wc, 1, s, COUNTOF(s), ws, COUNTOF(ws) );
	printf( "The number of fields input is %d\n", result );
	printf( "The contents are: %d %f %c %C %s %S\n", i, fp, c,
		wc, s, ws);
	
	/* wscanf_s sample */
	printf("%s", "input format: %d %f %hc %lc %S %ls\n");
	result = wscanf_s( L"%d %f %hc %lc %S %ls", &i, &fp, &c, 2,
		&wc, 1, s, COUNTOF(s), ws, COUNTOF(ws) );
	wprintf( L"The number of fields input is %d\n", result );
	wprintf( L"The contents are: %d %f %C %c %hs %s\n", i, fp,
		c, wc, s, ws);
	
	/* the following is input sample
		71 98.6 h z Byte characters
		36 92.3 y n Wide characters
		
	*/
}

void sscanfSample( void )
{
	char  tokenstring[] = "15 12 14...";
	char  s[81];
	char  c;
	int   i;
	float fp;
	
	// Input various data from tokenstring:
	// max 80 character string:
	sscanf_s( tokenstring, "%80s", s, 81 ); 
	sscanf_s( tokenstring, "%c", &c, 1 );  
	sscanf_s( tokenstring, "%d", &i );  
	sscanf_s( tokenstring, "%f", &fp ); 
	
	// Output the data read
	printf( "String    = %s\n", s );
	printf( "Character = %c\n", c );
	printf( "Integer:  = %d\n", i );
	printf( "Real:     = %f\n", fp );
}

void swscanfSample( void )
{
	wchar_t  tokenstring[] =L"15 12 14...";
	wchar_t  s[81];
	wchar_t  c;
	int   i;
	float fp;
	int ret = 0, totalConvertedItems = 0;
	
	// Input various data from tokenstring:
	// max 80 character string:
	ret = swscanf_s( tokenstring, L"%80s", s, 81 ); 
	totalConvertedItems += ret;

	ret = swscanf_s( tokenstring, L"%lc", &c, sizeof(c) / sizeof(wchar_t) );  
	totalConvertedItems += ret;

	ret = swscanf_s( tokenstring, L"%d", &i );  
	totalConvertedItems += ret;

	ret = swscanf_s( tokenstring, L"%f", &fp ); 
	totalConvertedItems += ret;
	
	printf( "total %d items converted\n", totalConvertedItems );

	// Output the data read
	wprintf( L"String    = %s\n", s );
	printf( "Character = %c\n", c );
	printf( "Integer:  = %d\n", i );
	printf( "Real:     = %f\n", fp );
}

void timerSscan(char* format, ...)
{
	va_list arglist;
	int ret;
	char userInput[250] = "2014-01-28_12:15:23,record_temperature, 7 ";
	
	va_start( arglist, format );
	ret = vsscanf_s(userInput, format, arglist ) ;
    printf("convert %d items\n", ret  );
	va_end(arglist);
	
}

void vsscanfSample( void )
{
	char date[20];
	char oper[20];
	int temperature = 0;
	
#ifdef _DEBUG
#define TIMER_SCAN timerSscan
#else
#define TIMER_SCAN (void)
#endif
	
	timerSscan("%19s,%18s,%d", date, 20, oper, 20, &temperature);
}

void fscanfSample( void )
{
	FILE *stream;
	long l;
	float fp;
	char s[81];
	char c;
	
	if( (stream = fopen( "../sampleFiles/fscanf.out", "w+" )) == NULL )
	{
		printf( "The file fscanf.out was not opened\n" );
	}
	else
	{
		fprintf( stream, "%s %ld %f%c", "a-string", 
			65000, 3.14159, 'x' );
		// Set pointer to beginning of file:
		if ( fseek( stream, 0L, SEEK_SET ) )
		{
			printf( "fseek failed\n" );
		}
		
		// Read data back from file:
		fscanf_s( stream, "%s", s, 81 );
		fscanf_s( stream, "%ld", &l );
		
		fscanf_s( stream, "%f", &fp );
		fscanf_s( stream, "%c", &c, 1 );
		
		// Output data read:
		printf( "%s\n", s );
		printf( "%ld\n", l );
		printf( "%f\n", fp );
		printf( "%c\n", c );
		
		fclose( stream );
		
	}
}

void strcpySample(void)
{
	char   str1[LEN];
	char   str2[LEN];
	
	strcpy_s(str1, LEN, "qqweqeqeqeq");
    strcpy_s(str2, LEN, "keep it simple");
	
	strcpy_s(str1, LEN, str2);
}

void strncpySample(void)
{
	errno_t rc;
	char   str1[LEN];
	char   str2[LEN] = "security design department";
	
	rc = strncpy_s(str1, LEN, str2, LEN);
	if (rc != EOK)
	{
		printf("strncpy_s failed\n");
	}
}

void wcscpySample(void)
{
	errno_t rc;
	wchar_t   str1[LEN] = L"Foo___xxxx";
	wchar_t   str2[LEN] = L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	
	
	rc = wcscpy_s(str1, LEN, str2);	
	if (rc != EOK)
	{
		printf("wcscpy_s failed\n");
	}
}

void strcatSample(void)
{
	char   str1[LEN] = "2012 Labs";
	char   str2[LEN] = "Security Design department";
	errno_t rc;
	
	rc = strcat_s(str1, LEN, str2);
	if (rc != EOK)
	{
		printf("strcat_s failed\n");
	}
}

void strncatSample(void)
{
	char   str1[LEN];
	char   str2[LEN];
	errno_t rc;
	
	strcpy_s(str1, LEN, "aaaaaaaaaa");
	strcpy_s(str2, LEN, "keep it simple");
	
	rc = strncat_s(str1, LEN, str2, 8);
	if (rc != EOK)
	{
		printf("strncat_s failed\n");
	}
}

void strtokSample(void)
{
	
	char string1[] ="A string\tof ,,tokens\nand some  more tokens";
	char string2[] = "Another string\n\tparsed at the same time.";
	char seps[]   = " ,\t\n";
	
	char *token1,
		*token2,
		*next_token1,
		*next_token2;
	
	token1 = strtok_s( string1, seps, &next_token1);
    token2 = strtok_s ( string2, seps, &next_token2);
	
    // While there are tokens in "string1" or "string2"
    while ((token1 != NULL) || (token2 != NULL))
    {
        // Get next token:
        if (token1 != NULL)
        {
            printf( " %s\n", token1 );
            token1 = strtok_s( NULL, seps, &next_token1);
        }
        if (token2 != NULL)
        {
            printf("        %s\n", token2 );
            token2 = strtok_s (NULL, seps, &next_token2); 
        }
    }
	
}

void sprintfSample(void)
{
	char  buffer[200], c = 'l';
	int   i = 35, j;
	float fp = 1.7320534f;
	
	// Format and print various data: 
	j  = sprintf_s( buffer, 200,     "   String:    %s\n", "computer" );
	assert( j > 0);
	j += sprintf_s( buffer + j, 200 - j, "   Character: %c\n", c );
	j += sprintf_s( buffer + j, 200 - j, "   Integer:   %d\n", i );
	j += sprintf_s( buffer + j, 200 - j, "   Real:      %f\n", fp );
	
	printf( "Output:\n%s\ncharacter count = %d\n", buffer, j );
}


void swprintfSample(void)
{
	wchar_t buf[100];
	int len = swprintf_s( buf, 100, L"%d %s", 2012, L"Hello world" );
	printf( "wrote %d characters\n", len );
	
	// swprintf_s fails because string contains WEOF (\xffff)
	//   len = swprintf_s( buf, 100, L"%s", L"Hello\xffff world" );
	//  printf( "wrote %d characters\n", len );
	
}

void dbgPrintf(char* format, ...)
{
	va_list arglist;
	int len;
	char dest[250];
	
	va_start( arglist, format );
	len = vsprintf_s(dest, 250, format, arglist ) ;
	printf("%s   Len: %d\n", dest, len );
	va_end(arglist);

	
}

void vsprintfSample(void)
{
#ifdef _DEBUG
	#define DBG_PRINTF dbgPrintf
#else
	#define DBG_PRINTF (void)
#endif
	
	DBG_PRINTF( "%d %.30s", -23, "file not found!");
}

void dbgNPrintf(int count, char* format, ...)
{
	va_list arglist;
	int len;
	char dest[250];
	
	va_start( arglist, format );
	len = vsnprintf_s(dest, 250, count, format, arglist ) ;

	printf("%s    Len: %d\n", dest, len );
	va_end(arglist);
}

void vsnprintfSample(void)
{
#ifdef _DEBUG
	#define DBG_NPRINTF dbgNPrintf
#else
	#define DBG_NPRINTF (void)
#endif
	
	DBG_NPRINTF(10, "%d %.30s", -23, "file not found!");
	
}

void snprintfSample(void)
{
	char dest[10];
	
	int ret = snprintf_s( dest, sizeof(dest), 8, "<<<%d>>>", 121 );
	printf( "    new contents of dest: '%s'   len: %d\n", dest, ret);
	
	ret = snprintf_s( dest, sizeof(dest), 9, "<<<%d>>>", 121 );
	printf( "    new contents of dest: '%s'     len: %d\n", dest, ret );
	
}


void memcpySample(void)
{
	int a1[10], a2[100], i;
	errno_t err;
	wchar_t dest[] = L"Huawei Tech";
	// Populate a2 with squares of integers
	for (i = 0; i < 100; i++)
	{
		a2[i] = i*i;
	}
	
	// Tell memcpy_s to copy 10 ints (40 bytes), giving
	// the size of the a1 array (also 40 bytes).
	err = memcpy_s(a1, sizeof(a1), a2, 10 * sizeof (int) );    
	if (err)
	{
		printf("Error executing memcpy_s.\n");
	}
	
	wmemcpy_s(dest, sizeof(dest) / sizeof (wchar_t), L"2012Labs", 8 );
	
}

void memmoveSample(void)
{
	char str[] = "0123456789";
	wchar_t wStr[] = L"0123456789";
	printf("Before: %s\n", str);
	
	// Move six bytes from the start of the string
	// to a new position shifted by one byte. To protect against
	// buffer overrun, the secure version of memmove requires the
	// the length of the destination string to be specified. 
	
	memmove_s((str + 1), 9, str, 6); 
	
	printf(" After: %s\n", str);
	
	wmemmove_s( (wStr + 1), 9, wStr, 6);
	
}

void memsetSample(void)
{
	char buffer[] = "This is a test of the memset function";
	
	printf( "Before: %s\n", buffer );
	memset_s( buffer, sizeof(buffer), '*', 4 );
	printf( "After:  %s\n", buffer );
	
}

void getsSample(void)
{
	char line[21]; // room for 20 chars + '\0'
	gets_s( line, 20 );
	printf( "The line entered was: %s\n", line );

}

void wcscatSample(void)
{
	wchar_t   str1[LEN] = L"2012 Labs";
	wchar_t   str2[] = L"Security Design Department";
	errno_t rc1 = 0, rc2 = 0;
	
	rc1 = wcscat_s(str1, LEN, L" ");
	rc2 = wcscat_s(str1, LEN, str2);
	if (rc1 != EOK || rc2 != EOK)
	{
		printf("wcscat_s failed\n");
	}
}

void wcsncatSample(void)
{
	wchar_t   str1[LEN] = L"2012 Labs";
	wchar_t   str2[] = L"Security Design Department";
	errno_t rc1 = 0, rc2 = 0;
	
	rc1 = wcsncat_s(str1, LEN, L" ", 1);
	rc2 = wcsncat_s(str1, LEN, str2, 15);
	if (rc1 != EOK || rc2 != EOK)
	{
		printf("wcsncat_s failed\n");
	}
}
