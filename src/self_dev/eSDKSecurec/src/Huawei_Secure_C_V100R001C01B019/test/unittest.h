/*
unittest.h
*/

#ifndef __UNITTEST_H__B918_45ed_AECF_676FAD108434
#define __UNITTEST_H__B918_45ed_AECF_676FAD108434



void test_strcat_s(void);
void test_strncat_s(void);
void test_wcscat_s(void);
void test_wcsncat_s(void);

void testOverlap(void);
	void TestStrcpy_s();
	void TestStrncpy_s();
         
	void TestWcscpy_s();
	void TestWcsncpy_s();
         
	//for strcat 
	void TestStrcat_s();
	void TestStrncat_s();
       
	//strtok
	void TestStrtok_s();
	void test_strtok(void);
	void test_wcstok(void);
         
	//for memcpy
	void TestMemcpy_s();
	void Test_wmemcpy_s();

	//for memmove
	void memmove_s_test();
	void wmemmove_s_test();

	//for memset
	void test_memset_s(void);

	//for sprintf
	void test_snprintf_s(void);
	void test_sprintf_s_boundaryCondition(void);
	void test_sprintf_s_combination(void);
	void test_sprintf_s_basic(void);
	void test_vsprintf_s(void);
	void test_vswprintf_s(void);

	void test_swprintf_s_combination(void);
	void test_vsnprintf_s(void);

	//for sscanf
	void scanf_gbk(void);
	void test_scanf(void);
	void test_wscanf(void);
	void test_vscanf(void);
	void test_vwscanf(void);

	void test_sscanf(void);
	void testSwscanf(void);

	void testfscanf(void);
	void test_fwscanf(void);
	void test_vfscanf(void);
	void test_vfwscanf(void);

	void test_vsscanf(void);
	void test_vswscanf(void);

	//for gets_s
	void test_gets_s(void);

	//for performance test
	void TestMemcpyPerformance(void);
 
#define RUN_TIME         12

	//double strcpyTest(int is_secure, int loopCnt);
void strcpyPerformanceTest(void);
void wcscpyPerformanceTest(void);
void strncpyPerformanceTest(void);
void wcsncpyPerformanceTest(void);

void strcatPerformanceTest(void);
void wcscatPerformanceTest(void);
void strncatPerformanceTest(void);
void wcsncatPerformanceTest(void);

void memcpyPerformanceTest(void);

void strcpyChunkPerformanceTest(void);
void wcscpyChunkPerformanceTest(void);
void strcatChunkPerformanceTest(void);
void wcscatChunkPerformanceTest(void);

void strcpy_sChunkPerformanceTest(void);
errno_t strcpy_s_s(char *strDest, size_t numberOfElements, const char *strSource);

void sprintfPerformanceTest(void);
void ThreeMemcpyPerformanceTest(void);
void ThreeMemsetPerformanceTest(void);
void analyseBestMaxCopyLen(void);

void test_sscanf_d(void);
void test_sscanf_i(void);

void test_sscanf_compare(void);
void test_scanf_compare(void);
void test_fscanf_compare(void);
void test_fwscanf_compare(void);
void test_wscanf_format_s(void);

#endif
