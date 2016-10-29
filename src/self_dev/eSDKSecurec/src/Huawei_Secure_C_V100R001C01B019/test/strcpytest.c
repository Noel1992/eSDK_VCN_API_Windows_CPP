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

/* LSD remark

*/

#define IGNORE_BUG		

#include "securec.h"
#include "unittest.h"
#include <assert.h>
#include <string.h>

#define LEN   ( 128 )

typedef unsigned int uint32_t;
typedef int int32_t;


void TestStrcpy_s()
{
	errno_t rc;
	int32_t  ind;
	char   str1[LEN];
	char   str2[LEN];

	rc = strcpy_s(NULL, LEN, str2);	//NULL pointer case
	assert( EINVAL == rc);



	/*--------------------------------------------------*/

	strcpy_s(str1, LEN, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	rc = strcpy_s(str1, 5, NULL);	//src NULL pointer case
	assert ((rc & 0x7F) == EINVAL);

	assert(str1[0] == '\0') ;

	/*--------------------------------------------------*/

	rc = strcpy_s(str1, 0, str2);	//size is 0
	assert ((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/

	rc = strcpy_s(str1, (size_t)(SECUREC_STRING_MAX_LEN+1), str2);	//over maxi limit
	assert ((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	strcpy_s(str1, LEN, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';

	rc = strcpy_s(str1, LEN/2, str2);	//src is balnk
	assert ((rc & 0x7F) == EOK);
	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	rc = strcpy_s(str1, LEN, str1);	//full overlap
	assert ((rc & 0x7F) == EOK);
	assert(str1[0] == 'a');
	assert (strcmp(str1,  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);

	/*--------------------------------------------------*/

	strcpy(&str1[0], "keep it simple");

	rc = strcpy_s(&str1[0], LEN, &str1[5]);	//part overlap

	assert (rc == EOVERLAP_AND_RESET);


	/*--------------------------------------------------*/

	strcpy(&str1[0], "keep it simple");
	str2[0] = '\0';

	rc = strcpy_s(str1, LEN, str2);	//blank src
	assert ((rc & 0x7F) == EOK);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	str1[0] = '\0';
	strcpy(&str2[0], "keep it simple");

	rc = strcpy_s(str1, LEN, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	rc = strcpy_s(str1, LEN, str2);	//different copy
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	rc = strcpy_s(str1, 1, str2);
	assert ((rc & 0x7F) == ERANGE);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	rc = strcpy_s(str1, 2, str2);
	assert ((rc & 0x7F) == ERANGE);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "it");

	rc = strcpy_s(str1, 3, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	strcpy(str1, "qq12345weqeqeqeq");
	strcpy(str2, "it");

	rc = strcpy_s(str1, 10, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcpy */
	ind = strcmp(str1, str2);
	assert (ind == 0);

	/*   for (i=0; i<10; i++) {
	printf("str1[%i] = %d \n", i, str1[i]);
	}
	*/

}

void testOverlap(void)
{
	errno_t rc;
	int32_t  i;
	char   str1[LEN];
	char lsdStr[] = "123\0abcdef";
	char catStr[20] = "0123456789";

	for(i = 0; i <LEN - 1; ++i) {
		str1[i] = 'a' + (i %26);
	}
	str1[i] = '\0';

	rc = strcpy_s(str1, LEN, str1);	
	assert( EOK == rc);

	rc = strcpy_s(str1, LEN, str1 +10);	
	assert( EOVERLAP_AND_RESET == rc);

	strcpy_s(str1, LEN, "abcdefghijklmnopqrst");	
	rc = strcpy_s(str1+ 10, LEN, str1);	
	assert( EOVERLAP_AND_RESET == rc);

	rc = strcat_s(catStr,  sizeof(catStr), catStr + 5);	
	assert( EOVERLAP_AND_RESET == rc);

	strcpy_s(catStr, sizeof(catStr), "0123456789");	
	rc = strncat_s(catStr+ 4,  sizeof(catStr) -4, catStr, 2);	
	assert( EOK == rc);
	assert(strcmp(catStr, "012345678901") ==0);

	strcpy_s(catStr, sizeof(catStr), "0123456789");	
	rc = strncat_s(catStr+ 4,  sizeof(catStr) -4, catStr, 3);	
	assert( EOK == rc);
	assert(strcmp(catStr, "0123456789012") ==0);

	rc = strcpy_s(catStr, sizeof(catStr), "0123456789");	
	assert( EOK == rc);
	rc = strncat_s(catStr+ 4,  sizeof(catStr) -4, catStr, 4);	
	assert( EOVERLAP_AND_RESET == rc);
	assert(strcmp(catStr + 4, "") ==0);


	strcpy_s(catStr, sizeof(catStr), "0123456789");	
	rc = strncat_s(catStr+ 4,  sizeof(catStr) -4, catStr, 9);	
	assert( EOVERLAP_AND_RESET == rc);

}

void TestStrncpy_s()
{
	errno_t rc;
	size_t nlen;
	int32_t ind;
	char   str1[LEN];
	char   str2[LEN];
	char   dest[LEN];

	/*--------------------------------------------------*/

	nlen = 5;
	rc = strncpy_s(NULL, LEN, str2, nlen);	//test dest is NULL
	assert((rc & 0x7F) == EINVAL);

	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	nlen = 5;
	rc = strncpy_s(str1, 5, NULL, nlen);
	assert((rc & 0x7F) == EINVAL);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	nlen = 5;
	rc = strncpy_s(str1, 0, str2, nlen);
	assert((rc & 0x7F) == ERANGE);
	assert(str1[0] == 0);

	/*--------------------------------------------------*/

	rc = strncpy_s(str1, (size_t)-2, str2, nlen);
	assert((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';

	rc = strncpy_s(str1, 5, str2, 0);
	assert((rc & 0x7F) == EOK) ;	//the return value is different to safeC ESZEROL

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/
	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	strcpy(str2, "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");

	rc = strncpy_s(str1, 5, str2, (size_t)(SECUREC_STRING_MAX_LEN+1));	//src over range
	assert((rc & 0x7F) == ERANGE);


	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';
	nlen = 5;

	rc = strncpy_s(&str1[0], LEN/2, &str2[0], nlen);
	assert((rc & 0x7F) == EOK) ;

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	nlen = 5;

	/* test overlap */
	rc = strncpy_s(str1, LEN, str1, nlen);
	assert((rc & 0x7F) == EOK);

	assert(str1[0] == 'a');

	/*--------------------------------------------------*/

	strcpy(str1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	nlen = 18;

	rc = strncpy_s(&str1[0], LEN, &str1[5], nlen);
	assert(rc == EOVERLAP_AND_RESET);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "keep it simple");
	str2[0] = '\0';

	nlen = 10;
	rc = strncpy_s(str1, LEN, str2, nlen);
	assert((rc & 0x7F) == EOK);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	str1[0] = '\0';
	strcpy(str2, "keep it simple");

	nlen = 20;
	rc = strncpy_s(str1, LEN, str2, nlen);	//overwrite dest
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert(ind == 0);


	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	nlen = 32;
	rc = strncpy_s(str1, LEN, str2, nlen);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert(ind == 0) ;

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	rc = strncpy_s(str1, 1, str2, nlen);	//over dest range
	assert((rc & 0x7F) == ERANGE);

	assert(*str1 == '\0');

	/*--------------------------------------------------*/

	strcpy(str1, "qqweqeqeqeq");
	strcpy(str2, "keep it simple");

	rc = strncpy_s(str1, 2, str2, nlen);
	assert((rc & 0x7F) == ERANGE);

	assert(*str1 == '\0');

	/*--------------------------------------------------*/
	/* TR example */

	strcpy(dest, "                            ");
	strcpy(str1, "hello");

	rc = strncpy_s(dest, 6, str1, 100);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(dest, str1);
	assert(ind == 0);

	/*--------------------------------------------------*/
	/* TR example */

	strcpy(dest, "                            ");
	strcpy(str2, "goodbye");

	rc = strncpy_s(dest, 5, str2, 7);
	assert((rc & 0x7F) == ERANGE);
	assert(dest[0] == 0);

	/*--------------------------------------------------*/
	/* TR example */

	strcpy(dest, "                            ");
	strcpy(str2, "goodbye");

	rc = strncpy_s(dest, 5, str2, 4);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(dest, "good");
	assert(ind == 0);

	/*--------------------------------------------------*/

	strcpy(dest, "                            ");
	strcpy(str2, "good");

	/*   strnlen("good") < 5   */
	rc = strncpy_s(dest, 5, str2, 8);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = strcmp(dest, "good");
	assert(ind == 0);

	/*--------------------------------------------------*/

	strcpy(str1, "qq12345weqeqeqeq");
	strcpy(str2, "it");

	nlen = 10;
	rc = strncpy_s(str1, 10, str2, nlen);
	assert((rc & 0x7F) == EOK) ;

	/* be sure the results are the same as strcmp */
	ind = strcmp(str1, str2);
	assert(ind == 0) ;
}

void TestWcscpy_s()
{
	errno_t rc;
	int32_t  ind;
	wchar_t   str1[LEN];
	wchar_t   str2[LEN];

	rc = wcscpy_s(NULL, LEN, str2);	//NULL pointer case
	assert( EINVAL == rc);



	/*--------------------------------------------------*/

	wcscpy_s(str1, LEN, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	rc = wcscpy_s(str1, 5, NULL);	//src NULL pointer case
	assert ((rc & 0x7F) == EINVAL);

	assert(str1[0] == '\0') ;

	/*--------------------------------------------------*/

	rc = wcscpy_s(str1, 0, str2);	//size is 0
	assert ((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/

	rc = wcscpy_s(str1, (size_t)(SECUREC_STRING_MAX_LEN+1), str2);	//over maxi limit
	assert ((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	wcscpy_s(str1, LEN, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';

	rc = wcscpy_s(str1, LEN/2, str2);	//src is balnk
	assert ((rc & 0x7F) == EOK);
	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	rc = wcscpy_s(str1, LEN, str1);	//full overlap
	assert ((rc & 0x7F) == EOK);
	assert(str1[0] == 'a');
	assert (wcscmp(str1,  L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0);

	/*--------------------------------------------------*/

	wcscpy(&str1[0], L"keep it simple");

	rc = wcscpy_s(&str1[0], LEN, &str1[5]);	//part overlap

	assert (rc == EOVERLAP_AND_RESET);



	/*--------------------------------------------------*/

	wcscpy(&str1[0], L"keep it simple");
	str2[0] = '\0';

	rc = wcscpy_s(str1, LEN, str2);	//blank src
	assert ((rc & 0x7F) == EOK);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	str1[0] = '\0';
	wcscpy(&str2[0], L"keep it simple");

	rc = wcscpy_s(str1, LEN, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = wcscmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	rc = wcscpy_s(str1, LEN, str2);	//different copy
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = wcscmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	rc = wcscpy_s(str1, 1, str2);
	assert ((rc & 0x7F) == ERANGE);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	rc = wcscpy_s(str1, 2, str2);
	assert ((rc & 0x7F) == ERANGE);

	assert (*str1 == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"it");

	rc = wcscpy_s(str1, 3, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as strcmp */
	ind = wcscmp(str1, str2);
	assert (ind == 0);

	/*--------------------------------------------------*/

	wcscpy(str1, L"qq12345weqeqeqeq");
	wcscpy(str2, L"it");

	rc = wcscpy_s(str1, 10, str2);
	assert ((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscpy */
	ind = wcscmp(str1, str2);
	assert (ind == 0);

	/*   for (i=0; i<10; i++) {
	printf("str1[%i] = %d \n", i, str1[i]);
	}
	*/

}

void TestWcsncpy_s()
{
	errno_t rc;
	size_t nlen;
	int32_t ind;
	wchar_t   str1[LEN];
	wchar_t   str2[LEN];
	wchar_t   dest[LEN];

	/*--------------------------------------------------*/

	nlen = 5;
	rc = wcsncpy_s(NULL, LEN, str2, nlen);	//test dest is NULL
	assert((rc & 0x7F) == EINVAL);

	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	nlen = 5;
	rc = wcsncpy_s(str1, 5, NULL, nlen);
	assert((rc & 0x7F) == EINVAL);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	nlen = 5;
	rc = wcsncpy_s(str1, 0, str2, nlen);
	assert((rc & 0x7F) == ERANGE);
	assert(str1[0] == 0);

	/*--------------------------------------------------*/

	rc = wcsncpy_s(str1, (size_t)-2, str2, nlen);
	assert((rc & 0x7F) == ERANGE);

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';

	rc = wcsncpy_s(str1, 5, str2, 0);
	assert((rc & 0x7F) == EOK) ;	//the return value is different to safeC ESZEROL

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/
	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	wcscpy(str2, L"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");

	rc = wcsncpy_s(str1, 5, str2, (size_t)(SECUREC_STRING_MAX_LEN+1));	//src over range
	assert((rc & 0x7F) == ERANGE);


	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	str2[0] = '\0';
	nlen = 5;

	rc = wcsncpy_s(&str1[0], LEN/2, &str2[0], nlen);
	assert((rc & 0x7F) == EOK) ;

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	nlen = 5;

	/* test overlap */
	rc = wcsncpy_s(str1, LEN, str1, nlen);
	assert((rc & 0x7F) == EOK);

	assert(str1[0] == 'a');

	/*--------------------------------------------------*/

	wcscpy(str1, L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	nlen = 18;

	rc = wcsncpy_s(&str1[0], LEN, &str1[5], nlen);
	assert(rc == EOVERLAP_AND_RESET);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"keep it simple");
	str2[0] = '\0';

	nlen = 10;
	rc = wcsncpy_s(str1, LEN, str2, nlen);
	assert((rc & 0x7F) == EOK);

	assert(str1[0] == '\0');

	/*--------------------------------------------------*/

	str1[0] = '\0';
	wcscpy(str2, L"keep it simple");

	nlen = 20;
	rc = wcsncpy_s(str1, LEN, str2, nlen);	//overwrite dest
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(str1, str2);
	assert(ind == 0);


	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	nlen = 32;
	rc = wcsncpy_s(str1, LEN, str2, nlen);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(str1, str2);
	assert(ind == 0) ;

	/*--------------------------------------------------*/
	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	rc = wcsncpy_s(str1, 1, str2, nlen);	//over dest range
	assert((rc & 0x7F) == ERANGE);

	assert(*str1 == '\0');

	/*--------------------------------------------------*/

	wcscpy(str1, L"qqweqeqeqeq");
	wcscpy(str2, L"keep it simple");

	rc = wcsncpy_s(str1, 2, str2, nlen);
	assert((rc & 0x7F) == ERANGE);

	assert(*str1 == '\0');

	/*--------------------------------------------------*/
	/* TR example */

	wcscpy(dest, L"                            ");
	wcscpy(str1, L"hello");

	rc = wcsncpy_s(dest, 6, str1, 100);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(dest, str1);
	assert(ind == 0);

	/*--------------------------------------------------*/
	/* TR example */

	wcscpy(dest, L"                            ");
	wcscpy(str2, L"goodbye");

	rc = wcsncpy_s(dest, 5, str2, 7);
	assert((rc & 0x7F) == ERANGE);
	assert(dest[0] == 0);

	/*--------------------------------------------------*/
	/* TR example */

	wcscpy(dest, L"                            ");
	wcscpy(str2, L"goodbye");

	rc = wcsncpy_s(dest, 5, str2, 4);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(dest, L"good");
	assert(ind == 0);

	/*--------------------------------------------------*/

	wcscpy(dest, L"                            ");
	wcscpy(str2, L"good");

	/*   strnlen("good") < 5   */
	rc = wcsncpy_s(dest, 5, str2, 8);
	assert((rc & 0x7F) == EOK);

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(dest, L"good");
	assert(ind == 0);

	/*--------------------------------------------------*/

	wcscpy(str1, L"qq12345weqeqeqeq");
	wcscpy(str2, L"it");

	nlen = 10;
	rc = wcsncpy_s(str1, 10, str2, nlen);
	assert((rc & 0x7F) == EOK) ;

	/* be sure the results are the same as wcscmp */
	ind = wcscmp(str1, str2);
	assert(ind == 0) ;


}



