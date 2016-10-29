/*
memcpytest.c

*/
#include "securec.h"
#include "unittest.h"
#include "testutil.h"
#include <assert.h>
#include <string.h>

#define MEM_LEN   ( 1024 )
typedef unsigned int uint32_t;

#define TEST_STR "abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789abcdefghij0123456789"

void TestOffset1()
{
	static UINT8T  mem1[MEM_LEN + 2];
	static UINT8T  mem2[MEM_LEN + 2];
	errno_t rc;

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,0);
assert(rc == EOK);
memcpy(mem2, TEST_STR,0);
assert(0 == memcmp(mem1 + 1, mem2, 0));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,1);
assert(rc == EOK);
memcpy(mem2, TEST_STR,1);
assert(0 == memcmp(mem1 + 1, mem2, 1));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,2);
assert(rc == EOK);
memcpy(mem2, TEST_STR,2);
assert(0 == memcmp(mem1 + 1, mem2, 2));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,3);
assert(rc == EOK);
memcpy(mem2, TEST_STR,3);
assert(0 == memcmp(mem1 + 1, mem2, 3));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,4);
assert(rc == EOK);
memcpy(mem2, TEST_STR,4);
assert(0 == memcmp(mem1 + 1, mem2, 4));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,5);
assert(rc == EOK);
memcpy(mem2, TEST_STR,5);
assert(0 == memcmp(mem1 + 1, mem2, 5));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,6);
assert(rc == EOK);
memcpy(mem2, TEST_STR,6);
assert(0 == memcmp(mem1 + 1, mem2, 6));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,7);
assert(rc == EOK);
memcpy(mem2, TEST_STR,7);
assert(0 == memcmp(mem1 + 1, mem2, 7));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,8);
assert(rc == EOK);
memcpy(mem2, TEST_STR,8);
assert(0 == memcmp(mem1 + 1, mem2, 8));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,9);
assert(rc == EOK);
memcpy(mem2, TEST_STR,9);
assert(0 == memcmp(mem1 + 1, mem2, 9));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,10);
assert(rc == EOK);
memcpy(mem2, TEST_STR,10);
assert(0 == memcmp(mem1 + 1, mem2, 10));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,11);
assert(rc == EOK);
memcpy(mem2, TEST_STR,11);
assert(0 == memcmp(mem1 + 1, mem2, 11));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,12);
assert(rc == EOK);
memcpy(mem2, TEST_STR,12);
assert(0 == memcmp(mem1 + 1, mem2, 12));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,13);
assert(rc == EOK);
memcpy(mem2, TEST_STR,13);
assert(0 == memcmp(mem1 + 1, mem2, 13));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,14);
assert(rc == EOK);
memcpy(mem2, TEST_STR,14);
assert(0 == memcmp(mem1 + 1, mem2, 14));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,15);
assert(rc == EOK);
memcpy(mem2, TEST_STR,15);
assert(0 == memcmp(mem1 + 1, mem2, 15));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,16);
assert(rc == EOK);
memcpy(mem2, TEST_STR,16);
assert(0 == memcmp(mem1 + 1, mem2, 16));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,17);
assert(rc == EOK);
memcpy(mem2, TEST_STR,17);
assert(0 == memcmp(mem1 + 1, mem2, 17));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,18);
assert(rc == EOK);
memcpy(mem2, TEST_STR,18);
assert(0 == memcmp(mem1 + 1, mem2, 18));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,19);
assert(rc == EOK);
memcpy(mem2, TEST_STR,19);
assert(0 == memcmp(mem1 + 1, mem2, 19));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,20);
assert(rc == EOK);
memcpy(mem2, TEST_STR,20);
assert(0 == memcmp(mem1 + 1, mem2, 20));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,21);
assert(rc == EOK);
memcpy(mem2, TEST_STR,21);
assert(0 == memcmp(mem1 + 1, mem2, 21));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,22);
assert(rc == EOK);
memcpy(mem2, TEST_STR,22);
assert(0 == memcmp(mem1 + 1, mem2, 22));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,23);
assert(rc == EOK);
memcpy(mem2, TEST_STR,23);
assert(0 == memcmp(mem1 + 1, mem2, 23));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,24);
assert(rc == EOK);
memcpy(mem2, TEST_STR,24);
assert(0 == memcmp(mem1 + 1, mem2, 24));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,25);
assert(rc == EOK);
memcpy(mem2, TEST_STR,25);
assert(0 == memcmp(mem1 + 1, mem2, 25));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,26);
assert(rc == EOK);
memcpy(mem2, TEST_STR,26);
assert(0 == memcmp(mem1 + 1, mem2, 26));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,27);
assert(rc == EOK);
memcpy(mem2, TEST_STR,27);
assert(0 == memcmp(mem1 + 1, mem2, 27));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,28);
assert(rc == EOK);
memcpy(mem2, TEST_STR,28);
assert(0 == memcmp(mem1 + 1, mem2, 28));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,29);
assert(rc == EOK);
memcpy(mem2, TEST_STR,29);
assert(0 == memcmp(mem1 + 1, mem2, 29));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,30);
assert(rc == EOK);
memcpy(mem2, TEST_STR,30);
assert(0 == memcmp(mem1 + 1, mem2, 30));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,31);
assert(rc == EOK);
memcpy(mem2, TEST_STR,31);
assert(0 == memcmp(mem1 + 1, mem2, 31));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,32);
assert(rc == EOK);
memcpy(mem2, TEST_STR,32);
assert(0 == memcmp(mem1 + 1, mem2, 32));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,33);
assert(rc == EOK);
memcpy(mem2, TEST_STR,33);
assert(0 == memcmp(mem1 + 1, mem2, 33));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,34);
assert(rc == EOK);
memcpy(mem2, TEST_STR,34);
assert(0 == memcmp(mem1 + 1, mem2, 34));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,35);
assert(rc == EOK);
memcpy(mem2, TEST_STR,35);
assert(0 == memcmp(mem1 + 1, mem2, 35));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,36);
assert(rc == EOK);
memcpy(mem2, TEST_STR,36);
assert(0 == memcmp(mem1 + 1, mem2, 36));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,37);
assert(rc == EOK);
memcpy(mem2, TEST_STR,37);
assert(0 == memcmp(mem1 + 1, mem2, 37));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,38);
assert(rc == EOK);
memcpy(mem2, TEST_STR,38);
assert(0 == memcmp(mem1 + 1, mem2, 38));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,39);
assert(rc == EOK);
memcpy(mem2, TEST_STR,39);
assert(0 == memcmp(mem1 + 1, mem2, 39));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,40);
assert(rc == EOK);
memcpy(mem2, TEST_STR,40);
assert(0 == memcmp(mem1 + 1, mem2, 40));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,41);
assert(rc == EOK);
memcpy(mem2, TEST_STR,41);
assert(0 == memcmp(mem1 + 1, mem2, 41));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,42);
assert(rc == EOK);
memcpy(mem2, TEST_STR,42);
assert(0 == memcmp(mem1 + 1, mem2, 42));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,43);
assert(rc == EOK);
memcpy(mem2, TEST_STR,43);
assert(0 == memcmp(mem1 + 1, mem2, 43));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,44);
assert(rc == EOK);
memcpy(mem2, TEST_STR,44);
assert(0 == memcmp(mem1 + 1, mem2, 44));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,45);
assert(rc == EOK);
memcpy(mem2, TEST_STR,45);
assert(0 == memcmp(mem1 + 1, mem2, 45));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,46);
assert(rc == EOK);
memcpy(mem2, TEST_STR,46);
assert(0 == memcmp(mem1 + 1, mem2, 46));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,47);
assert(rc == EOK);
memcpy(mem2, TEST_STR,47);
assert(0 == memcmp(mem1 + 1, mem2, 47));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,48);
assert(rc == EOK);
memcpy(mem2, TEST_STR,48);
assert(0 == memcmp(mem1 + 1, mem2, 48));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,49);
assert(rc == EOK);
memcpy(mem2, TEST_STR,49);
assert(0 == memcmp(mem1 + 1, mem2, 49));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,50);
assert(rc == EOK);
memcpy(mem2, TEST_STR,50);
assert(0 == memcmp(mem1 + 1, mem2, 50));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,51);
assert(rc == EOK);
memcpy(mem2, TEST_STR,51);
assert(0 == memcmp(mem1 + 1, mem2, 51));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,52);
assert(rc == EOK);
memcpy(mem2, TEST_STR,52);
assert(0 == memcmp(mem1 + 1, mem2, 52));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,53);
assert(rc == EOK);
memcpy(mem2, TEST_STR,53);
assert(0 == memcmp(mem1 + 1, mem2, 53));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,54);
assert(rc == EOK);
memcpy(mem2, TEST_STR,54);
assert(0 == memcmp(mem1 + 1, mem2, 54));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,55);
assert(rc == EOK);
memcpy(mem2, TEST_STR,55);
assert(0 == memcmp(mem1 + 1, mem2, 55));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,56);
assert(rc == EOK);
memcpy(mem2, TEST_STR,56);
assert(0 == memcmp(mem1 + 1, mem2, 56));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,57);
assert(rc == EOK);
memcpy(mem2, TEST_STR,57);
assert(0 == memcmp(mem1 + 1, mem2, 57));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,58);
assert(rc == EOK);
memcpy(mem2, TEST_STR,58);
assert(0 == memcmp(mem1 + 1, mem2, 58));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,59);
assert(rc == EOK);
memcpy(mem2, TEST_STR,59);
assert(0 == memcmp(mem1 + 1, mem2, 59));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,60);
assert(rc == EOK);
memcpy(mem2, TEST_STR,60);
assert(0 == memcmp(mem1 + 1, mem2, 60));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,61);
assert(rc == EOK);
memcpy(mem2, TEST_STR,61);
assert(0 == memcmp(mem1 + 1, mem2, 61));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,62);
assert(rc == EOK);
memcpy(mem2, TEST_STR,62);
assert(0 == memcmp(mem1 + 1, mem2, 62));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,63);
assert(rc == EOK);
memcpy(mem2, TEST_STR,63);
assert(0 == memcmp(mem1 + 1, mem2, 63));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,64);
assert(rc == EOK);
memcpy(mem2, TEST_STR,64);
assert(0 == memcmp(mem1 + 1, mem2, 64));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,65);
assert(rc == EOK);
memcpy(mem2, TEST_STR,65);
assert(0 == memcmp(mem1 + 1, mem2, 65));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,66);
assert(rc == EOK);
memcpy(mem2, TEST_STR,66);
assert(0 == memcmp(mem1 + 1, mem2, 66));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,67);
assert(rc == EOK);
memcpy(mem2, TEST_STR,67);
assert(0 == memcmp(mem1 + 1, mem2, 67));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,68);
assert(rc == EOK);
memcpy(mem2, TEST_STR,68);
assert(0 == memcmp(mem1 + 1, mem2, 68));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,69);
assert(rc == EOK);
memcpy(mem2, TEST_STR,69);
assert(0 == memcmp(mem1 + 1, mem2, 69));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,70);
assert(rc == EOK);
memcpy(mem2, TEST_STR,70);
assert(0 == memcmp(mem1 + 1, mem2, 70));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,71);
assert(rc == EOK);
memcpy(mem2, TEST_STR,71);
assert(0 == memcmp(mem1 + 1, mem2, 71));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,72);
assert(rc == EOK);
memcpy(mem2, TEST_STR,72);
assert(0 == memcmp(mem1 + 1, mem2, 72));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,73);
assert(rc == EOK);
memcpy(mem2, TEST_STR,73);
assert(0 == memcmp(mem1 + 1, mem2, 73));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,74);
assert(rc == EOK);
memcpy(mem2, TEST_STR,74);
assert(0 == memcmp(mem1 + 1, mem2, 74));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,75);
assert(rc == EOK);
memcpy(mem2, TEST_STR,75);
assert(0 == memcmp(mem1 + 1, mem2, 75));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,76);
assert(rc == EOK);
memcpy(mem2, TEST_STR,76);
assert(0 == memcmp(mem1 + 1, mem2, 76));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,77);
assert(rc == EOK);
memcpy(mem2, TEST_STR,77);
assert(0 == memcmp(mem1 + 1, mem2, 77));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,78);
assert(rc == EOK);
memcpy(mem2, TEST_STR,78);
assert(0 == memcmp(mem1 + 1, mem2, 78));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,79);
assert(rc == EOK);
memcpy(mem2, TEST_STR,79);
assert(0 == memcmp(mem1 + 1, mem2, 79));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,80);
assert(rc == EOK);
memcpy(mem2, TEST_STR,80);
assert(0 == memcmp(mem1 + 1, mem2, 80));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,81);
assert(rc == EOK);
memcpy(mem2, TEST_STR,81);
assert(0 == memcmp(mem1 + 1, mem2, 81));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,82);
assert(rc == EOK);
memcpy(mem2, TEST_STR,82);
assert(0 == memcmp(mem1 + 1, mem2, 82));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,83);
assert(rc == EOK);
memcpy(mem2, TEST_STR,83);
assert(0 == memcmp(mem1 + 1, mem2, 83));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,84);
assert(rc == EOK);
memcpy(mem2, TEST_STR,84);
assert(0 == memcmp(mem1 + 1, mem2, 84));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,85);
assert(rc == EOK);
memcpy(mem2, TEST_STR,85);
assert(0 == memcmp(mem1 + 1, mem2, 85));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,86);
assert(rc == EOK);
memcpy(mem2, TEST_STR,86);
assert(0 == memcmp(mem1 + 1, mem2, 86));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,87);
assert(rc == EOK);
memcpy(mem2, TEST_STR,87);
assert(0 == memcmp(mem1 + 1, mem2, 87));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,88);
assert(rc == EOK);
memcpy(mem2, TEST_STR,88);
assert(0 == memcmp(mem1 + 1, mem2, 88));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,89);
assert(rc == EOK);
memcpy(mem2, TEST_STR,89);
assert(0 == memcmp(mem1 + 1, mem2, 89));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,90);
assert(rc == EOK);
memcpy(mem2, TEST_STR,90);
assert(0 == memcmp(mem1 + 1, mem2, 90));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,91);
assert(rc == EOK);
memcpy(mem2, TEST_STR,91);
assert(0 == memcmp(mem1 + 1, mem2, 91));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,92);
assert(rc == EOK);
memcpy(mem2, TEST_STR,92);
assert(0 == memcmp(mem1 + 1, mem2, 92));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,93);
assert(rc == EOK);
memcpy(mem2, TEST_STR,93);
assert(0 == memcmp(mem1 + 1, mem2, 93));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,94);
assert(rc == EOK);
memcpy(mem2, TEST_STR,94);
assert(0 == memcmp(mem1 + 1, mem2, 94));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,95);
assert(rc == EOK);
memcpy(mem2, TEST_STR,95);
assert(0 == memcmp(mem1 + 1, mem2, 95));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,96);
assert(rc == EOK);
memcpy(mem2, TEST_STR,96);
assert(0 == memcmp(mem1 + 1, mem2, 96));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,97);
assert(rc == EOK);
memcpy(mem2, TEST_STR,97);
assert(0 == memcmp(mem1 + 1, mem2, 97));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,98);
assert(rc == EOK);
memcpy(mem2, TEST_STR,98);
assert(0 == memcmp(mem1 + 1, mem2, 98));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,99);
assert(rc == EOK);
memcpy(mem2, TEST_STR,99);
assert(0 == memcmp(mem1 + 1, mem2, 99));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,100);
assert(rc == EOK);
memcpy(mem2, TEST_STR,100);
assert(0 == memcmp(mem1 + 1, mem2, 100));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,101);
assert(rc == EOK);
memcpy(mem2, TEST_STR,101);
assert(0 == memcmp(mem1 + 1, mem2, 101));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,102);
assert(rc == EOK);
memcpy(mem2, TEST_STR,102);
assert(0 == memcmp(mem1 + 1, mem2, 102));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,103);
assert(rc == EOK);
memcpy(mem2, TEST_STR,103);
assert(0 == memcmp(mem1 + 1, mem2, 103));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,104);
assert(rc == EOK);
memcpy(mem2, TEST_STR,104);
assert(0 == memcmp(mem1 + 1, mem2, 104));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,105);
assert(rc == EOK);
memcpy(mem2, TEST_STR,105);
assert(0 == memcmp(mem1 + 1, mem2, 105));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,106);
assert(rc == EOK);
memcpy(mem2, TEST_STR,106);
assert(0 == memcmp(mem1 + 1, mem2, 106));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,107);
assert(rc == EOK);
memcpy(mem2, TEST_STR,107);
assert(0 == memcmp(mem1 + 1, mem2, 107));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,108);
assert(rc == EOK);
memcpy(mem2, TEST_STR,108);
assert(0 == memcmp(mem1 + 1, mem2, 108));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,109);
assert(rc == EOK);
memcpy(mem2, TEST_STR,109);
assert(0 == memcmp(mem1 + 1, mem2, 109));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,110);
assert(rc == EOK);
memcpy(mem2, TEST_STR,110);
assert(0 == memcmp(mem1 + 1, mem2, 110));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,111);
assert(rc == EOK);
memcpy(mem2, TEST_STR,111);
assert(0 == memcmp(mem1 + 1, mem2, 111));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,112);
assert(rc == EOK);
memcpy(mem2, TEST_STR,112);
assert(0 == memcmp(mem1 + 1, mem2, 112));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,113);
assert(rc == EOK);
memcpy(mem2, TEST_STR,113);
assert(0 == memcmp(mem1 + 1, mem2, 113));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,114);
assert(rc == EOK);
memcpy(mem2, TEST_STR,114);
assert(0 == memcmp(mem1 + 1, mem2, 114));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,115);
assert(rc == EOK);
memcpy(mem2, TEST_STR,115);
assert(0 == memcmp(mem1 + 1, mem2, 115));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,116);
assert(rc == EOK);
memcpy(mem2, TEST_STR,116);
assert(0 == memcmp(mem1 + 1, mem2, 116));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,117);
assert(rc == EOK);
memcpy(mem2, TEST_STR,117);
assert(0 == memcmp(mem1 + 1, mem2, 117));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,118);
assert(rc == EOK);
memcpy(mem2, TEST_STR,118);
assert(0 == memcmp(mem1 + 1, mem2, 118));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,119);
assert(rc == EOK);
memcpy(mem2, TEST_STR,119);
assert(0 == memcmp(mem1 + 1, mem2, 119));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,120);
assert(rc == EOK);
memcpy(mem2, TEST_STR,120);
assert(0 == memcmp(mem1 + 1, mem2, 120));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,121);
assert(rc == EOK);
memcpy(mem2, TEST_STR,121);
assert(0 == memcmp(mem1 + 1, mem2, 121));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,122);
assert(rc == EOK);
memcpy(mem2, TEST_STR,122);
assert(0 == memcmp(mem1 + 1, mem2, 122));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,123);
assert(rc == EOK);
memcpy(mem2, TEST_STR,123);
assert(0 == memcmp(mem1 + 1, mem2, 123));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,124);
assert(rc == EOK);
memcpy(mem2, TEST_STR,124);
assert(0 == memcmp(mem1 + 1, mem2, 124));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,125);
assert(rc == EOK);
memcpy(mem2, TEST_STR,125);
assert(0 == memcmp(mem1 + 1, mem2, 125));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,126);
assert(rc == EOK);
memcpy(mem2, TEST_STR,126);
assert(0 == memcmp(mem1 + 1, mem2, 126));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,127);
assert(rc == EOK);
memcpy(mem2, TEST_STR,127);
assert(0 == memcmp(mem1 + 1, mem2, 127));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,128);
assert(rc == EOK);
memcpy(mem2, TEST_STR,128);
assert(0 == memcmp(mem1 + 1, mem2, 128));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,129);
assert(rc == EOK);
memcpy(mem2, TEST_STR,129);
assert(0 == memcmp(mem1 + 1, mem2, 129));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,130);
assert(rc == EOK);
memcpy(mem2, TEST_STR,130);
assert(0 == memcmp(mem1 + 1, mem2, 130));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,131);
assert(rc == EOK);
memcpy(mem2, TEST_STR,131);
assert(0 == memcmp(mem1 + 1, mem2, 131));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,132);
assert(rc == EOK);
memcpy(mem2, TEST_STR,132);
assert(0 == memcmp(mem1 + 1, mem2, 132));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,133);
assert(rc == EOK);
memcpy(mem2, TEST_STR,133);
assert(0 == memcmp(mem1 + 1, mem2, 133));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,134);
assert(rc == EOK);
memcpy(mem2, TEST_STR,134);
assert(0 == memcmp(mem1 + 1, mem2, 134));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,135);
assert(rc == EOK);
memcpy(mem2, TEST_STR,135);
assert(0 == memcmp(mem1 + 1, mem2, 135));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,136);
assert(rc == EOK);
memcpy(mem2, TEST_STR,136);
assert(0 == memcmp(mem1 + 1, mem2, 136));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,137);
assert(rc == EOK);
memcpy(mem2, TEST_STR,137);
assert(0 == memcmp(mem1 + 1, mem2, 137));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,138);
assert(rc == EOK);
memcpy(mem2, TEST_STR,138);
assert(0 == memcmp(mem1 + 1, mem2, 138));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,139);
assert(rc == EOK);
memcpy(mem2, TEST_STR,139);
assert(0 == memcmp(mem1 + 1, mem2, 139));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,140);
assert(rc == EOK);
memcpy(mem2, TEST_STR,140);
assert(0 == memcmp(mem1 + 1, mem2, 140));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,141);
assert(rc == EOK);
memcpy(mem2, TEST_STR,141);
assert(0 == memcmp(mem1 + 1, mem2, 141));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,142);
assert(rc == EOK);
memcpy(mem2, TEST_STR,142);
assert(0 == memcmp(mem1 + 1, mem2, 142));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,143);
assert(rc == EOK);
memcpy(mem2, TEST_STR,143);
assert(0 == memcmp(mem1 + 1, mem2, 143));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,144);
assert(rc == EOK);
memcpy(mem2, TEST_STR,144);
assert(0 == memcmp(mem1 + 1, mem2, 144));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,145);
assert(rc == EOK);
memcpy(mem2, TEST_STR,145);
assert(0 == memcmp(mem1 + 1, mem2, 145));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,146);
assert(rc == EOK);
memcpy(mem2, TEST_STR,146);
assert(0 == memcmp(mem1 + 1, mem2, 146));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,147);
assert(rc == EOK);
memcpy(mem2, TEST_STR,147);
assert(0 == memcmp(mem1 + 1, mem2, 147));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,148);
assert(rc == EOK);
memcpy(mem2, TEST_STR,148);
assert(0 == memcmp(mem1 + 1, mem2, 148));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,149);
assert(rc == EOK);
memcpy(mem2, TEST_STR,149);
assert(0 == memcmp(mem1 + 1, mem2, 149));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,150);
assert(rc == EOK);
memcpy(mem2, TEST_STR,150);
assert(0 == memcmp(mem1 + 1, mem2, 150));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,151);
assert(rc == EOK);
memcpy(mem2, TEST_STR,151);
assert(0 == memcmp(mem1 + 1, mem2, 151));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,152);
assert(rc == EOK);
memcpy(mem2, TEST_STR,152);
assert(0 == memcmp(mem1 + 1, mem2, 152));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,153);
assert(rc == EOK);
memcpy(mem2, TEST_STR,153);
assert(0 == memcmp(mem1 + 1, mem2, 153));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,154);
assert(rc == EOK);
memcpy(mem2, TEST_STR,154);
assert(0 == memcmp(mem1 + 1, mem2, 154));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,155);
assert(rc == EOK);
memcpy(mem2, TEST_STR,155);
assert(0 == memcmp(mem1 + 1, mem2, 155));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,156);
assert(rc == EOK);
memcpy(mem2, TEST_STR,156);
assert(0 == memcmp(mem1 + 1, mem2, 156));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,157);
assert(rc == EOK);
memcpy(mem2, TEST_STR,157);
assert(0 == memcmp(mem1 + 1, mem2, 157));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,158);
assert(rc == EOK);
memcpy(mem2, TEST_STR,158);
assert(0 == memcmp(mem1 + 1, mem2, 158));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,159);
assert(rc == EOK);
memcpy(mem2, TEST_STR,159);
assert(0 == memcmp(mem1 + 1, mem2, 159));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,160);
assert(rc == EOK);
memcpy(mem2, TEST_STR,160);
assert(0 == memcmp(mem1 + 1, mem2, 160));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,161);
assert(rc == EOK);
memcpy(mem2, TEST_STR,161);
assert(0 == memcmp(mem1 + 1, mem2, 161));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,162);
assert(rc == EOK);
memcpy(mem2, TEST_STR,162);
assert(0 == memcmp(mem1 + 1, mem2, 162));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,163);
assert(rc == EOK);
memcpy(mem2, TEST_STR,163);
assert(0 == memcmp(mem1 + 1, mem2, 163));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,164);
assert(rc == EOK);
memcpy(mem2, TEST_STR,164);
assert(0 == memcmp(mem1 + 1, mem2, 164));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,165);
assert(rc == EOK);
memcpy(mem2, TEST_STR,165);
assert(0 == memcmp(mem1 + 1, mem2, 165));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,166);
assert(rc == EOK);
memcpy(mem2, TEST_STR,166);
assert(0 == memcmp(mem1 + 1, mem2, 166));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,167);
assert(rc == EOK);
memcpy(mem2, TEST_STR,167);
assert(0 == memcmp(mem1 + 1, mem2, 167));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,168);
assert(rc == EOK);
memcpy(mem2, TEST_STR,168);
assert(0 == memcmp(mem1 + 1, mem2, 168));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,169);
assert(rc == EOK);
memcpy(mem2, TEST_STR,169);
assert(0 == memcmp(mem1 + 1, mem2, 169));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,170);
assert(rc == EOK);
memcpy(mem2, TEST_STR,170);
assert(0 == memcmp(mem1 + 1, mem2, 170));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,171);
assert(rc == EOK);
memcpy(mem2, TEST_STR,171);
assert(0 == memcmp(mem1 + 1, mem2, 171));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,172);
assert(rc == EOK);
memcpy(mem2, TEST_STR,172);
assert(0 == memcmp(mem1 + 1, mem2, 172));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,173);
assert(rc == EOK);
memcpy(mem2, TEST_STR,173);
assert(0 == memcmp(mem1 + 1, mem2, 173));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,174);
assert(rc == EOK);
memcpy(mem2, TEST_STR,174);
assert(0 == memcmp(mem1 + 1, mem2, 174));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,175);
assert(rc == EOK);
memcpy(mem2, TEST_STR,175);
assert(0 == memcmp(mem1 + 1, mem2, 175));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,176);
assert(rc == EOK);
memcpy(mem2, TEST_STR,176);
assert(0 == memcmp(mem1 + 1, mem2, 176));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,177);
assert(rc == EOK);
memcpy(mem2, TEST_STR,177);
assert(0 == memcmp(mem1 + 1, mem2, 177));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,178);
assert(rc == EOK);
memcpy(mem2, TEST_STR,178);
assert(0 == memcmp(mem1 + 1, mem2, 178));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,179);
assert(rc == EOK);
memcpy(mem2, TEST_STR,179);
assert(0 == memcmp(mem1 + 1, mem2, 179));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,180);
assert(rc == EOK);
memcpy(mem2, TEST_STR,180);
assert(0 == memcmp(mem1 + 1, mem2, 180));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,181);
assert(rc == EOK);
memcpy(mem2, TEST_STR,181);
assert(0 == memcmp(mem1 + 1, mem2, 181));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,182);
assert(rc == EOK);
memcpy(mem2, TEST_STR,182);
assert(0 == memcmp(mem1 + 1, mem2, 182));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,183);
assert(rc == EOK);
memcpy(mem2, TEST_STR,183);
assert(0 == memcmp(mem1 + 1, mem2, 183));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,184);
assert(rc == EOK);
memcpy(mem2, TEST_STR,184);
assert(0 == memcmp(mem1 + 1, mem2, 184));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,185);
assert(rc == EOK);
memcpy(mem2, TEST_STR,185);
assert(0 == memcmp(mem1 + 1, mem2, 185));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,186);
assert(rc == EOK);
memcpy(mem2, TEST_STR,186);
assert(0 == memcmp(mem1 + 1, mem2, 186));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,187);
assert(rc == EOK);
memcpy(mem2, TEST_STR,187);
assert(0 == memcmp(mem1 + 1, mem2, 187));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,188);
assert(rc == EOK);
memcpy(mem2, TEST_STR,188);
assert(0 == memcmp(mem1 + 1, mem2, 188));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,189);
assert(rc == EOK);
memcpy(mem2, TEST_STR,189);
assert(0 == memcmp(mem1 + 1, mem2, 189));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,190);
assert(rc == EOK);
memcpy(mem2, TEST_STR,190);
assert(0 == memcmp(mem1 + 1, mem2, 190));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,191);
assert(rc == EOK);
memcpy(mem2, TEST_STR,191);
assert(0 == memcmp(mem1 + 1, mem2, 191));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,192);
assert(rc == EOK);
memcpy(mem2, TEST_STR,192);
assert(0 == memcmp(mem1 + 1, mem2, 192));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,193);
assert(rc == EOK);
memcpy(mem2, TEST_STR,193);
assert(0 == memcmp(mem1 + 1, mem2, 193));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,194);
assert(rc == EOK);
memcpy(mem2, TEST_STR,194);
assert(0 == memcmp(mem1 + 1, mem2, 194));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,195);
assert(rc == EOK);
memcpy(mem2, TEST_STR,195);
assert(0 == memcmp(mem1 + 1, mem2, 195));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,196);
assert(rc == EOK);
memcpy(mem2, TEST_STR,196);
assert(0 == memcmp(mem1 + 1, mem2, 196));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,197);
assert(rc == EOK);
memcpy(mem2, TEST_STR,197);
assert(0 == memcmp(mem1 + 1, mem2, 197));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,198);
assert(rc == EOK);
memcpy(mem2, TEST_STR,198);
assert(0 == memcmp(mem1 + 1, mem2, 198));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,199);
assert(rc == EOK);
memcpy(mem2, TEST_STR,199);
assert(0 == memcmp(mem1 + 1, mem2, 199));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 1, MEM_LEN, TEST_STR,200);
assert(rc == EOK);
memcpy(mem2, TEST_STR,200);
assert(0 == memcmp(mem1 + 1, mem2, 200));
memset_s(mem1, MEM_LEN, 0, 210);

}

void TestOffset2()
{
	static UINT8T  mem1[MEM_LEN + 2];
	static UINT8T  mem2[MEM_LEN + 2];
	errno_t rc;
rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,0);
assert(rc == EOK);
memcpy(mem2, TEST_STR,0);
assert(0 == memcmp(mem1 + 2, mem2, 0));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,1);
assert(rc == EOK);
memcpy(mem2, TEST_STR,1);
assert(0 == memcmp(mem1 + 2, mem2, 1));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,2);
assert(rc == EOK);
memcpy(mem2, TEST_STR,2);
assert(0 == memcmp(mem1 + 2, mem2, 2));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,3);
assert(rc == EOK);
memcpy(mem2, TEST_STR,3);
assert(0 == memcmp(mem1 + 2, mem2, 3));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,4);
assert(rc == EOK);
memcpy(mem2, TEST_STR,4);
assert(0 == memcmp(mem1 + 2, mem2, 4));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,5);
assert(rc == EOK);
memcpy(mem2, TEST_STR,5);
assert(0 == memcmp(mem1 + 2, mem2, 5));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,6);
assert(rc == EOK);
memcpy(mem2, TEST_STR,6);
assert(0 == memcmp(mem1 + 2, mem2, 6));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,7);
assert(rc == EOK);
memcpy(mem2, TEST_STR,7);
assert(0 == memcmp(mem1 + 2, mem2, 7));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,8);
assert(rc == EOK);
memcpy(mem2, TEST_STR,8);
assert(0 == memcmp(mem1 + 2, mem2, 8));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,9);
assert(rc == EOK);
memcpy(mem2, TEST_STR,9);
assert(0 == memcmp(mem1 + 2, mem2, 9));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,10);
assert(rc == EOK);
memcpy(mem2, TEST_STR,10);
assert(0 == memcmp(mem1 + 2, mem2, 10));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,11);
assert(rc == EOK);
memcpy(mem2, TEST_STR,11);
assert(0 == memcmp(mem1 + 2, mem2, 11));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,12);
assert(rc == EOK);
memcpy(mem2, TEST_STR,12);
assert(0 == memcmp(mem1 + 2, mem2, 12));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,13);
assert(rc == EOK);
memcpy(mem2, TEST_STR,13);
assert(0 == memcmp(mem1 + 2, mem2, 13));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,14);
assert(rc == EOK);
memcpy(mem2, TEST_STR,14);
assert(0 == memcmp(mem1 + 2, mem2, 14));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,15);
assert(rc == EOK);
memcpy(mem2, TEST_STR,15);
assert(0 == memcmp(mem1 + 2, mem2, 15));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,16);
assert(rc == EOK);
memcpy(mem2, TEST_STR,16);
assert(0 == memcmp(mem1 + 2, mem2, 16));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,17);
assert(rc == EOK);
memcpy(mem2, TEST_STR,17);
assert(0 == memcmp(mem1 + 2, mem2, 17));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,18);
assert(rc == EOK);
memcpy(mem2, TEST_STR,18);
assert(0 == memcmp(mem1 + 2, mem2, 18));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,19);
assert(rc == EOK);
memcpy(mem2, TEST_STR,19);
assert(0 == memcmp(mem1 + 2, mem2, 19));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,20);
assert(rc == EOK);
memcpy(mem2, TEST_STR,20);
assert(0 == memcmp(mem1 + 2, mem2, 20));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,21);
assert(rc == EOK);
memcpy(mem2, TEST_STR,21);
assert(0 == memcmp(mem1 + 2, mem2, 21));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,22);
assert(rc == EOK);
memcpy(mem2, TEST_STR,22);
assert(0 == memcmp(mem1 + 2, mem2, 22));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,23);
assert(rc == EOK);
memcpy(mem2, TEST_STR,23);
assert(0 == memcmp(mem1 + 2, mem2, 23));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,24);
assert(rc == EOK);
memcpy(mem2, TEST_STR,24);
assert(0 == memcmp(mem1 + 2, mem2, 24));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,25);
assert(rc == EOK);
memcpy(mem2, TEST_STR,25);
assert(0 == memcmp(mem1 + 2, mem2, 25));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,26);
assert(rc == EOK);
memcpy(mem2, TEST_STR,26);
assert(0 == memcmp(mem1 + 2, mem2, 26));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,27);
assert(rc == EOK);
memcpy(mem2, TEST_STR,27);
assert(0 == memcmp(mem1 + 2, mem2, 27));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,28);
assert(rc == EOK);
memcpy(mem2, TEST_STR,28);
assert(0 == memcmp(mem1 + 2, mem2, 28));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,29);
assert(rc == EOK);
memcpy(mem2, TEST_STR,29);
assert(0 == memcmp(mem1 + 2, mem2, 29));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,30);
assert(rc == EOK);
memcpy(mem2, TEST_STR,30);
assert(0 == memcmp(mem1 + 2, mem2, 30));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,31);
assert(rc == EOK);
memcpy(mem2, TEST_STR,31);
assert(0 == memcmp(mem1 + 2, mem2, 31));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,32);
assert(rc == EOK);
memcpy(mem2, TEST_STR,32);
assert(0 == memcmp(mem1 + 2, mem2, 32));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,33);
assert(rc == EOK);
memcpy(mem2, TEST_STR,33);
assert(0 == memcmp(mem1 + 2, mem2, 33));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,34);
assert(rc == EOK);
memcpy(mem2, TEST_STR,34);
assert(0 == memcmp(mem1 + 2, mem2, 34));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,35);
assert(rc == EOK);
memcpy(mem2, TEST_STR,35);
assert(0 == memcmp(mem1 + 2, mem2, 35));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,36);
assert(rc == EOK);
memcpy(mem2, TEST_STR,36);
assert(0 == memcmp(mem1 + 2, mem2, 36));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,37);
assert(rc == EOK);
memcpy(mem2, TEST_STR,37);
assert(0 == memcmp(mem1 + 2, mem2, 37));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,38);
assert(rc == EOK);
memcpy(mem2, TEST_STR,38);
assert(0 == memcmp(mem1 + 2, mem2, 38));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,39);
assert(rc == EOK);
memcpy(mem2, TEST_STR,39);
assert(0 == memcmp(mem1 + 2, mem2, 39));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,40);
assert(rc == EOK);
memcpy(mem2, TEST_STR,40);
assert(0 == memcmp(mem1 + 2, mem2, 40));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,41);
assert(rc == EOK);
memcpy(mem2, TEST_STR,41);
assert(0 == memcmp(mem1 + 2, mem2, 41));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,42);
assert(rc == EOK);
memcpy(mem2, TEST_STR,42);
assert(0 == memcmp(mem1 + 2, mem2, 42));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,43);
assert(rc == EOK);
memcpy(mem2, TEST_STR,43);
assert(0 == memcmp(mem1 + 2, mem2, 43));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,44);
assert(rc == EOK);
memcpy(mem2, TEST_STR,44);
assert(0 == memcmp(mem1 + 2, mem2, 44));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,45);
assert(rc == EOK);
memcpy(mem2, TEST_STR,45);
assert(0 == memcmp(mem1 + 2, mem2, 45));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,46);
assert(rc == EOK);
memcpy(mem2, TEST_STR,46);
assert(0 == memcmp(mem1 + 2, mem2, 46));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,47);
assert(rc == EOK);
memcpy(mem2, TEST_STR,47);
assert(0 == memcmp(mem1 + 2, mem2, 47));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,48);
assert(rc == EOK);
memcpy(mem2, TEST_STR,48);
assert(0 == memcmp(mem1 + 2, mem2, 48));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,49);
assert(rc == EOK);
memcpy(mem2, TEST_STR,49);
assert(0 == memcmp(mem1 + 2, mem2, 49));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,50);
assert(rc == EOK);
memcpy(mem2, TEST_STR,50);
assert(0 == memcmp(mem1 + 2, mem2, 50));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,51);
assert(rc == EOK);
memcpy(mem2, TEST_STR,51);
assert(0 == memcmp(mem1 + 2, mem2, 51));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,52);
assert(rc == EOK);
memcpy(mem2, TEST_STR,52);
assert(0 == memcmp(mem1 + 2, mem2, 52));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,53);
assert(rc == EOK);
memcpy(mem2, TEST_STR,53);
assert(0 == memcmp(mem1 + 2, mem2, 53));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,54);
assert(rc == EOK);
memcpy(mem2, TEST_STR,54);
assert(0 == memcmp(mem1 + 2, mem2, 54));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,55);
assert(rc == EOK);
memcpy(mem2, TEST_STR,55);
assert(0 == memcmp(mem1 + 2, mem2, 55));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,56);
assert(rc == EOK);
memcpy(mem2, TEST_STR,56);
assert(0 == memcmp(mem1 + 2, mem2, 56));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,57);
assert(rc == EOK);
memcpy(mem2, TEST_STR,57);
assert(0 == memcmp(mem1 + 2, mem2, 57));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,58);
assert(rc == EOK);
memcpy(mem2, TEST_STR,58);
assert(0 == memcmp(mem1 + 2, mem2, 58));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,59);
assert(rc == EOK);
memcpy(mem2, TEST_STR,59);
assert(0 == memcmp(mem1 + 2, mem2, 59));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,60);
assert(rc == EOK);
memcpy(mem2, TEST_STR,60);
assert(0 == memcmp(mem1 + 2, mem2, 60));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,61);
assert(rc == EOK);
memcpy(mem2, TEST_STR,61);
assert(0 == memcmp(mem1 + 2, mem2, 61));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,62);
assert(rc == EOK);
memcpy(mem2, TEST_STR,62);
assert(0 == memcmp(mem1 + 2, mem2, 62));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,63);
assert(rc == EOK);
memcpy(mem2, TEST_STR,63);
assert(0 == memcmp(mem1 + 2, mem2, 63));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,64);
assert(rc == EOK);
memcpy(mem2, TEST_STR,64);
assert(0 == memcmp(mem1 + 2, mem2, 64));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,65);
assert(rc == EOK);
memcpy(mem2, TEST_STR,65);
assert(0 == memcmp(mem1 + 2, mem2, 65));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,66);
assert(rc == EOK);
memcpy(mem2, TEST_STR,66);
assert(0 == memcmp(mem1 + 2, mem2, 66));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,67);
assert(rc == EOK);
memcpy(mem2, TEST_STR,67);
assert(0 == memcmp(mem1 + 2, mem2, 67));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,68);
assert(rc == EOK);
memcpy(mem2, TEST_STR,68);
assert(0 == memcmp(mem1 + 2, mem2, 68));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,69);
assert(rc == EOK);
memcpy(mem2, TEST_STR,69);
assert(0 == memcmp(mem1 + 2, mem2, 69));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,70);
assert(rc == EOK);
memcpy(mem2, TEST_STR,70);
assert(0 == memcmp(mem1 + 2, mem2, 70));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,71);
assert(rc == EOK);
memcpy(mem2, TEST_STR,71);
assert(0 == memcmp(mem1 + 2, mem2, 71));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,72);
assert(rc == EOK);
memcpy(mem2, TEST_STR,72);
assert(0 == memcmp(mem1 + 2, mem2, 72));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,73);
assert(rc == EOK);
memcpy(mem2, TEST_STR,73);
assert(0 == memcmp(mem1 + 2, mem2, 73));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,74);
assert(rc == EOK);
memcpy(mem2, TEST_STR,74);
assert(0 == memcmp(mem1 + 2, mem2, 74));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,75);
assert(rc == EOK);
memcpy(mem2, TEST_STR,75);
assert(0 == memcmp(mem1 + 2, mem2, 75));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,76);
assert(rc == EOK);
memcpy(mem2, TEST_STR,76);
assert(0 == memcmp(mem1 + 2, mem2, 76));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,77);
assert(rc == EOK);
memcpy(mem2, TEST_STR,77);
assert(0 == memcmp(mem1 + 2, mem2, 77));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,78);
assert(rc == EOK);
memcpy(mem2, TEST_STR,78);
assert(0 == memcmp(mem1 + 2, mem2, 78));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,79);
assert(rc == EOK);
memcpy(mem2, TEST_STR,79);
assert(0 == memcmp(mem1 + 2, mem2, 79));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,80);
assert(rc == EOK);
memcpy(mem2, TEST_STR,80);
assert(0 == memcmp(mem1 + 2, mem2, 80));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,81);
assert(rc == EOK);
memcpy(mem2, TEST_STR,81);
assert(0 == memcmp(mem1 + 2, mem2, 81));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,82);
assert(rc == EOK);
memcpy(mem2, TEST_STR,82);
assert(0 == memcmp(mem1 + 2, mem2, 82));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,83);
assert(rc == EOK);
memcpy(mem2, TEST_STR,83);
assert(0 == memcmp(mem1 + 2, mem2, 83));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,84);
assert(rc == EOK);
memcpy(mem2, TEST_STR,84);
assert(0 == memcmp(mem1 + 2, mem2, 84));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,85);
assert(rc == EOK);
memcpy(mem2, TEST_STR,85);
assert(0 == memcmp(mem1 + 2, mem2, 85));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,86);
assert(rc == EOK);
memcpy(mem2, TEST_STR,86);
assert(0 == memcmp(mem1 + 2, mem2, 86));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,87);
assert(rc == EOK);
memcpy(mem2, TEST_STR,87);
assert(0 == memcmp(mem1 + 2, mem2, 87));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,88);
assert(rc == EOK);
memcpy(mem2, TEST_STR,88);
assert(0 == memcmp(mem1 + 2, mem2, 88));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,89);
assert(rc == EOK);
memcpy(mem2, TEST_STR,89);
assert(0 == memcmp(mem1 + 2, mem2, 89));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,90);
assert(rc == EOK);
memcpy(mem2, TEST_STR,90);
assert(0 == memcmp(mem1 + 2, mem2, 90));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,91);
assert(rc == EOK);
memcpy(mem2, TEST_STR,91);
assert(0 == memcmp(mem1 + 2, mem2, 91));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,92);
assert(rc == EOK);
memcpy(mem2, TEST_STR,92);
assert(0 == memcmp(mem1 + 2, mem2, 92));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,93);
assert(rc == EOK);
memcpy(mem2, TEST_STR,93);
assert(0 == memcmp(mem1 + 2, mem2, 93));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,94);
assert(rc == EOK);
memcpy(mem2, TEST_STR,94);
assert(0 == memcmp(mem1 + 2, mem2, 94));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,95);
assert(rc == EOK);
memcpy(mem2, TEST_STR,95);
assert(0 == memcmp(mem1 + 2, mem2, 95));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,96);
assert(rc == EOK);
memcpy(mem2, TEST_STR,96);
assert(0 == memcmp(mem1 + 2, mem2, 96));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,97);
assert(rc == EOK);
memcpy(mem2, TEST_STR,97);
assert(0 == memcmp(mem1 + 2, mem2, 97));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,98);
assert(rc == EOK);
memcpy(mem2, TEST_STR,98);
assert(0 == memcmp(mem1 + 2, mem2, 98));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,99);
assert(rc == EOK);
memcpy(mem2, TEST_STR,99);
assert(0 == memcmp(mem1 + 2, mem2, 99));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,100);
assert(rc == EOK);
memcpy(mem2, TEST_STR,100);
assert(0 == memcmp(mem1 + 2, mem2, 100));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,101);
assert(rc == EOK);
memcpy(mem2, TEST_STR,101);
assert(0 == memcmp(mem1 + 2, mem2, 101));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,102);
assert(rc == EOK);
memcpy(mem2, TEST_STR,102);
assert(0 == memcmp(mem1 + 2, mem2, 102));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,103);
assert(rc == EOK);
memcpy(mem2, TEST_STR,103);
assert(0 == memcmp(mem1 + 2, mem2, 103));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,104);
assert(rc == EOK);
memcpy(mem2, TEST_STR,104);
assert(0 == memcmp(mem1 + 2, mem2, 104));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,105);
assert(rc == EOK);
memcpy(mem2, TEST_STR,105);
assert(0 == memcmp(mem1 + 2, mem2, 105));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,106);
assert(rc == EOK);
memcpy(mem2, TEST_STR,106);
assert(0 == memcmp(mem1 + 2, mem2, 106));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,107);
assert(rc == EOK);
memcpy(mem2, TEST_STR,107);
assert(0 == memcmp(mem1 + 2, mem2, 107));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,108);
assert(rc == EOK);
memcpy(mem2, TEST_STR,108);
assert(0 == memcmp(mem1 + 2, mem2, 108));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,109);
assert(rc == EOK);
memcpy(mem2, TEST_STR,109);
assert(0 == memcmp(mem1 + 2, mem2, 109));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,110);
assert(rc == EOK);
memcpy(mem2, TEST_STR,110);
assert(0 == memcmp(mem1 + 2, mem2, 110));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,111);
assert(rc == EOK);
memcpy(mem2, TEST_STR,111);
assert(0 == memcmp(mem1 + 2, mem2, 111));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,112);
assert(rc == EOK);
memcpy(mem2, TEST_STR,112);
assert(0 == memcmp(mem1 + 2, mem2, 112));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,113);
assert(rc == EOK);
memcpy(mem2, TEST_STR,113);
assert(0 == memcmp(mem1 + 2, mem2, 113));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,114);
assert(rc == EOK);
memcpy(mem2, TEST_STR,114);
assert(0 == memcmp(mem1 + 2, mem2, 114));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,115);
assert(rc == EOK);
memcpy(mem2, TEST_STR,115);
assert(0 == memcmp(mem1 + 2, mem2, 115));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,116);
assert(rc == EOK);
memcpy(mem2, TEST_STR,116);
assert(0 == memcmp(mem1 + 2, mem2, 116));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,117);
assert(rc == EOK);
memcpy(mem2, TEST_STR,117);
assert(0 == memcmp(mem1 + 2, mem2, 117));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,118);
assert(rc == EOK);
memcpy(mem2, TEST_STR,118);
assert(0 == memcmp(mem1 + 2, mem2, 118));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,119);
assert(rc == EOK);
memcpy(mem2, TEST_STR,119);
assert(0 == memcmp(mem1 + 2, mem2, 119));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,120);
assert(rc == EOK);
memcpy(mem2, TEST_STR,120);
assert(0 == memcmp(mem1 + 2, mem2, 120));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,121);
assert(rc == EOK);
memcpy(mem2, TEST_STR,121);
assert(0 == memcmp(mem1 + 2, mem2, 121));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,122);
assert(rc == EOK);
memcpy(mem2, TEST_STR,122);
assert(0 == memcmp(mem1 + 2, mem2, 122));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,123);
assert(rc == EOK);
memcpy(mem2, TEST_STR,123);
assert(0 == memcmp(mem1 + 2, mem2, 123));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,124);
assert(rc == EOK);
memcpy(mem2, TEST_STR,124);
assert(0 == memcmp(mem1 + 2, mem2, 124));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,125);
assert(rc == EOK);
memcpy(mem2, TEST_STR,125);
assert(0 == memcmp(mem1 + 2, mem2, 125));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,126);
assert(rc == EOK);
memcpy(mem2, TEST_STR,126);
assert(0 == memcmp(mem1 + 2, mem2, 126));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,127);
assert(rc == EOK);
memcpy(mem2, TEST_STR,127);
assert(0 == memcmp(mem1 + 2, mem2, 127));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,128);
assert(rc == EOK);
memcpy(mem2, TEST_STR,128);
assert(0 == memcmp(mem1 + 2, mem2, 128));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,129);
assert(rc == EOK);
memcpy(mem2, TEST_STR,129);
assert(0 == memcmp(mem1 + 2, mem2, 129));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,130);
assert(rc == EOK);
memcpy(mem2, TEST_STR,130);
assert(0 == memcmp(mem1 + 2, mem2, 130));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,131);
assert(rc == EOK);
memcpy(mem2, TEST_STR,131);
assert(0 == memcmp(mem1 + 2, mem2, 131));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,132);
assert(rc == EOK);
memcpy(mem2, TEST_STR,132);
assert(0 == memcmp(mem1 + 2, mem2, 132));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,133);
assert(rc == EOK);
memcpy(mem2, TEST_STR,133);
assert(0 == memcmp(mem1 + 2, mem2, 133));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,134);
assert(rc == EOK);
memcpy(mem2, TEST_STR,134);
assert(0 == memcmp(mem1 + 2, mem2, 134));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,135);
assert(rc == EOK);
memcpy(mem2, TEST_STR,135);
assert(0 == memcmp(mem1 + 2, mem2, 135));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,136);
assert(rc == EOK);
memcpy(mem2, TEST_STR,136);
assert(0 == memcmp(mem1 + 2, mem2, 136));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,137);
assert(rc == EOK);
memcpy(mem2, TEST_STR,137);
assert(0 == memcmp(mem1 + 2, mem2, 137));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,138);
assert(rc == EOK);
memcpy(mem2, TEST_STR,138);
assert(0 == memcmp(mem1 + 2, mem2, 138));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,139);
assert(rc == EOK);
memcpy(mem2, TEST_STR,139);
assert(0 == memcmp(mem1 + 2, mem2, 139));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,140);
assert(rc == EOK);
memcpy(mem2, TEST_STR,140);
assert(0 == memcmp(mem1 + 2, mem2, 140));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,141);
assert(rc == EOK);
memcpy(mem2, TEST_STR,141);
assert(0 == memcmp(mem1 + 2, mem2, 141));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,142);
assert(rc == EOK);
memcpy(mem2, TEST_STR,142);
assert(0 == memcmp(mem1 + 2, mem2, 142));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,143);
assert(rc == EOK);
memcpy(mem2, TEST_STR,143);
assert(0 == memcmp(mem1 + 2, mem2, 143));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,144);
assert(rc == EOK);
memcpy(mem2, TEST_STR,144);
assert(0 == memcmp(mem1 + 2, mem2, 144));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,145);
assert(rc == EOK);
memcpy(mem2, TEST_STR,145);
assert(0 == memcmp(mem1 + 2, mem2, 145));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,146);
assert(rc == EOK);
memcpy(mem2, TEST_STR,146);
assert(0 == memcmp(mem1 + 2, mem2, 146));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,147);
assert(rc == EOK);
memcpy(mem2, TEST_STR,147);
assert(0 == memcmp(mem1 + 2, mem2, 147));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,148);
assert(rc == EOK);
memcpy(mem2, TEST_STR,148);
assert(0 == memcmp(mem1 + 2, mem2, 148));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,149);
assert(rc == EOK);
memcpy(mem2, TEST_STR,149);
assert(0 == memcmp(mem1 + 2, mem2, 149));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,150);
assert(rc == EOK);
memcpy(mem2, TEST_STR,150);
assert(0 == memcmp(mem1 + 2, mem2, 150));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,151);
assert(rc == EOK);
memcpy(mem2, TEST_STR,151);
assert(0 == memcmp(mem1 + 2, mem2, 151));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,152);
assert(rc == EOK);
memcpy(mem2, TEST_STR,152);
assert(0 == memcmp(mem1 + 2, mem2, 152));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,153);
assert(rc == EOK);
memcpy(mem2, TEST_STR,153);
assert(0 == memcmp(mem1 + 2, mem2, 153));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,154);
assert(rc == EOK);
memcpy(mem2, TEST_STR,154);
assert(0 == memcmp(mem1 + 2, mem2, 154));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,155);
assert(rc == EOK);
memcpy(mem2, TEST_STR,155);
assert(0 == memcmp(mem1 + 2, mem2, 155));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,156);
assert(rc == EOK);
memcpy(mem2, TEST_STR,156);
assert(0 == memcmp(mem1 + 2, mem2, 156));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,157);
assert(rc == EOK);
memcpy(mem2, TEST_STR,157);
assert(0 == memcmp(mem1 + 2, mem2, 157));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,158);
assert(rc == EOK);
memcpy(mem2, TEST_STR,158);
assert(0 == memcmp(mem1 + 2, mem2, 158));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,159);
assert(rc == EOK);
memcpy(mem2, TEST_STR,159);
assert(0 == memcmp(mem1 + 2, mem2, 159));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,160);
assert(rc == EOK);
memcpy(mem2, TEST_STR,160);
assert(0 == memcmp(mem1 + 2, mem2, 160));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,161);
assert(rc == EOK);
memcpy(mem2, TEST_STR,161);
assert(0 == memcmp(mem1 + 2, mem2, 161));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,162);
assert(rc == EOK);
memcpy(mem2, TEST_STR,162);
assert(0 == memcmp(mem1 + 2, mem2, 162));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,163);
assert(rc == EOK);
memcpy(mem2, TEST_STR,163);
assert(0 == memcmp(mem1 + 2, mem2, 163));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,164);
assert(rc == EOK);
memcpy(mem2, TEST_STR,164);
assert(0 == memcmp(mem1 + 2, mem2, 164));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,165);
assert(rc == EOK);
memcpy(mem2, TEST_STR,165);
assert(0 == memcmp(mem1 + 2, mem2, 165));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,166);
assert(rc == EOK);
memcpy(mem2, TEST_STR,166);
assert(0 == memcmp(mem1 + 2, mem2, 166));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,167);
assert(rc == EOK);
memcpy(mem2, TEST_STR,167);
assert(0 == memcmp(mem1 + 2, mem2, 167));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,168);
assert(rc == EOK);
memcpy(mem2, TEST_STR,168);
assert(0 == memcmp(mem1 + 2, mem2, 168));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,169);
assert(rc == EOK);
memcpy(mem2, TEST_STR,169);
assert(0 == memcmp(mem1 + 2, mem2, 169));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,170);
assert(rc == EOK);
memcpy(mem2, TEST_STR,170);
assert(0 == memcmp(mem1 + 2, mem2, 170));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,171);
assert(rc == EOK);
memcpy(mem2, TEST_STR,171);
assert(0 == memcmp(mem1 + 2, mem2, 171));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,172);
assert(rc == EOK);
memcpy(mem2, TEST_STR,172);
assert(0 == memcmp(mem1 + 2, mem2, 172));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,173);
assert(rc == EOK);
memcpy(mem2, TEST_STR,173);
assert(0 == memcmp(mem1 + 2, mem2, 173));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,174);
assert(rc == EOK);
memcpy(mem2, TEST_STR,174);
assert(0 == memcmp(mem1 + 2, mem2, 174));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,175);
assert(rc == EOK);
memcpy(mem2, TEST_STR,175);
assert(0 == memcmp(mem1 + 2, mem2, 175));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,176);
assert(rc == EOK);
memcpy(mem2, TEST_STR,176);
assert(0 == memcmp(mem1 + 2, mem2, 176));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,177);
assert(rc == EOK);
memcpy(mem2, TEST_STR,177);
assert(0 == memcmp(mem1 + 2, mem2, 177));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,178);
assert(rc == EOK);
memcpy(mem2, TEST_STR,178);
assert(0 == memcmp(mem1 + 2, mem2, 178));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,179);
assert(rc == EOK);
memcpy(mem2, TEST_STR,179);
assert(0 == memcmp(mem1 + 2, mem2, 179));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,180);
assert(rc == EOK);
memcpy(mem2, TEST_STR,180);
assert(0 == memcmp(mem1 + 2, mem2, 180));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,181);
assert(rc == EOK);
memcpy(mem2, TEST_STR,181);
assert(0 == memcmp(mem1 + 2, mem2, 181));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,182);
assert(rc == EOK);
memcpy(mem2, TEST_STR,182);
assert(0 == memcmp(mem1 + 2, mem2, 182));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,183);
assert(rc == EOK);
memcpy(mem2, TEST_STR,183);
assert(0 == memcmp(mem1 + 2, mem2, 183));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,184);
assert(rc == EOK);
memcpy(mem2, TEST_STR,184);
assert(0 == memcmp(mem1 + 2, mem2, 184));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,185);
assert(rc == EOK);
memcpy(mem2, TEST_STR,185);
assert(0 == memcmp(mem1 + 2, mem2, 185));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,186);
assert(rc == EOK);
memcpy(mem2, TEST_STR,186);
assert(0 == memcmp(mem1 + 2, mem2, 186));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,187);
assert(rc == EOK);
memcpy(mem2, TEST_STR,187);
assert(0 == memcmp(mem1 + 2, mem2, 187));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,188);
assert(rc == EOK);
memcpy(mem2, TEST_STR,188);
assert(0 == memcmp(mem1 + 2, mem2, 188));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,189);
assert(rc == EOK);
memcpy(mem2, TEST_STR,189);
assert(0 == memcmp(mem1 + 2, mem2, 189));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,190);
assert(rc == EOK);
memcpy(mem2, TEST_STR,190);
assert(0 == memcmp(mem1 + 2, mem2, 190));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,191);
assert(rc == EOK);
memcpy(mem2, TEST_STR,191);
assert(0 == memcmp(mem1 + 2, mem2, 191));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,192);
assert(rc == EOK);
memcpy(mem2, TEST_STR,192);
assert(0 == memcmp(mem1 + 2, mem2, 192));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,193);
assert(rc == EOK);
memcpy(mem2, TEST_STR,193);
assert(0 == memcmp(mem1 + 2, mem2, 193));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,194);
assert(rc == EOK);
memcpy(mem2, TEST_STR,194);
assert(0 == memcmp(mem1 + 2, mem2, 194));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,195);
assert(rc == EOK);
memcpy(mem2, TEST_STR,195);
assert(0 == memcmp(mem1 + 2, mem2, 195));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,196);
assert(rc == EOK);
memcpy(mem2, TEST_STR,196);
assert(0 == memcmp(mem1 + 2, mem2, 196));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,197);
assert(rc == EOK);
memcpy(mem2, TEST_STR,197);
assert(0 == memcmp(mem1 + 2, mem2, 197));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,198);
assert(rc == EOK);
memcpy(mem2, TEST_STR,198);
assert(0 == memcmp(mem1 + 2, mem2, 198));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,199);
assert(rc == EOK);
memcpy(mem2, TEST_STR,199);
assert(0 == memcmp(mem1 + 2, mem2, 199));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 2, MEM_LEN, TEST_STR,200);
assert(rc == EOK);
memcpy(mem2, TEST_STR,200);
assert(0 == memcmp(mem1 + 2, mem2, 200));
memset_s(mem1, MEM_LEN, 0, 210);

}
void TestOffset3()
{
	static UINT8T  mem1[MEM_LEN + 2];
	static UINT8T  mem2[MEM_LEN + 2];
	errno_t rc;
rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,0);
assert(rc == EOK);
memcpy(mem2, TEST_STR,0);
assert(0 == memcmp(mem1 + 3, mem2, 0));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,1);
assert(rc == EOK);
memcpy(mem2, TEST_STR,1);
assert(0 == memcmp(mem1 + 3, mem2, 1));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,2);
assert(rc == EOK);
memcpy(mem2, TEST_STR,2);
assert(0 == memcmp(mem1 + 3, mem2, 2));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,3);
assert(rc == EOK);
memcpy(mem2, TEST_STR,3);
assert(0 == memcmp(mem1 + 3, mem2, 3));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,4);
assert(rc == EOK);
memcpy(mem2, TEST_STR,4);
assert(0 == memcmp(mem1 + 3, mem2, 4));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,5);
assert(rc == EOK);
memcpy(mem2, TEST_STR,5);
assert(0 == memcmp(mem1 + 3, mem2, 5));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,6);
assert(rc == EOK);
memcpy(mem2, TEST_STR,6);
assert(0 == memcmp(mem1 + 3, mem2, 6));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,7);
assert(rc == EOK);
memcpy(mem2, TEST_STR,7);
assert(0 == memcmp(mem1 + 3, mem2, 7));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,8);
assert(rc == EOK);
memcpy(mem2, TEST_STR,8);
assert(0 == memcmp(mem1 + 3, mem2, 8));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,9);
assert(rc == EOK);
memcpy(mem2, TEST_STR,9);
assert(0 == memcmp(mem1 + 3, mem2, 9));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,10);
assert(rc == EOK);
memcpy(mem2, TEST_STR,10);
assert(0 == memcmp(mem1 + 3, mem2, 10));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,11);
assert(rc == EOK);
memcpy(mem2, TEST_STR,11);
assert(0 == memcmp(mem1 + 3, mem2, 11));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,12);
assert(rc == EOK);
memcpy(mem2, TEST_STR,12);
assert(0 == memcmp(mem1 + 3, mem2, 12));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,13);
assert(rc == EOK);
memcpy(mem2, TEST_STR,13);
assert(0 == memcmp(mem1 + 3, mem2, 13));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,14);
assert(rc == EOK);
memcpy(mem2, TEST_STR,14);
assert(0 == memcmp(mem1 + 3, mem2, 14));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,15);
assert(rc == EOK);
memcpy(mem2, TEST_STR,15);
assert(0 == memcmp(mem1 + 3, mem2, 15));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,16);
assert(rc == EOK);
memcpy(mem2, TEST_STR,16);
assert(0 == memcmp(mem1 + 3, mem2, 16));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,17);
assert(rc == EOK);
memcpy(mem2, TEST_STR,17);
assert(0 == memcmp(mem1 + 3, mem2, 17));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,18);
assert(rc == EOK);
memcpy(mem2, TEST_STR,18);
assert(0 == memcmp(mem1 + 3, mem2, 18));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,19);
assert(rc == EOK);
memcpy(mem2, TEST_STR,19);
assert(0 == memcmp(mem1 + 3, mem2, 19));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,20);
assert(rc == EOK);
memcpy(mem2, TEST_STR,20);
assert(0 == memcmp(mem1 + 3, mem2, 20));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,21);
assert(rc == EOK);
memcpy(mem2, TEST_STR,21);
assert(0 == memcmp(mem1 + 3, mem2, 21));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,22);
assert(rc == EOK);
memcpy(mem2, TEST_STR,22);
assert(0 == memcmp(mem1 + 3, mem2, 22));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,23);
assert(rc == EOK);
memcpy(mem2, TEST_STR,23);
assert(0 == memcmp(mem1 + 3, mem2, 23));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,24);
assert(rc == EOK);
memcpy(mem2, TEST_STR,24);
assert(0 == memcmp(mem1 + 3, mem2, 24));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,25);
assert(rc == EOK);
memcpy(mem2, TEST_STR,25);
assert(0 == memcmp(mem1 + 3, mem2, 25));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,26);
assert(rc == EOK);
memcpy(mem2, TEST_STR,26);
assert(0 == memcmp(mem1 + 3, mem2, 26));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,27);
assert(rc == EOK);
memcpy(mem2, TEST_STR,27);
assert(0 == memcmp(mem1 + 3, mem2, 27));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,28);
assert(rc == EOK);
memcpy(mem2, TEST_STR,28);
assert(0 == memcmp(mem1 + 3, mem2, 28));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,29);
assert(rc == EOK);
memcpy(mem2, TEST_STR,29);
assert(0 == memcmp(mem1 + 3, mem2, 29));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,30);
assert(rc == EOK);
memcpy(mem2, TEST_STR,30);
assert(0 == memcmp(mem1 + 3, mem2, 30));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,31);
assert(rc == EOK);
memcpy(mem2, TEST_STR,31);
assert(0 == memcmp(mem1 + 3, mem2, 31));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,32);
assert(rc == EOK);
memcpy(mem2, TEST_STR,32);
assert(0 == memcmp(mem1 + 3, mem2, 32));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,33);
assert(rc == EOK);
memcpy(mem2, TEST_STR,33);
assert(0 == memcmp(mem1 + 3, mem2, 33));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,34);
assert(rc == EOK);
memcpy(mem2, TEST_STR,34);
assert(0 == memcmp(mem1 + 3, mem2, 34));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,35);
assert(rc == EOK);
memcpy(mem2, TEST_STR,35);
assert(0 == memcmp(mem1 + 3, mem2, 35));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,36);
assert(rc == EOK);
memcpy(mem2, TEST_STR,36);
assert(0 == memcmp(mem1 + 3, mem2, 36));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,37);
assert(rc == EOK);
memcpy(mem2, TEST_STR,37);
assert(0 == memcmp(mem1 + 3, mem2, 37));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,38);
assert(rc == EOK);
memcpy(mem2, TEST_STR,38);
assert(0 == memcmp(mem1 + 3, mem2, 38));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,39);
assert(rc == EOK);
memcpy(mem2, TEST_STR,39);
assert(0 == memcmp(mem1 + 3, mem2, 39));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,40);
assert(rc == EOK);
memcpy(mem2, TEST_STR,40);
assert(0 == memcmp(mem1 + 3, mem2, 40));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,41);
assert(rc == EOK);
memcpy(mem2, TEST_STR,41);
assert(0 == memcmp(mem1 + 3, mem2, 41));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,42);
assert(rc == EOK);
memcpy(mem2, TEST_STR,42);
assert(0 == memcmp(mem1 + 3, mem2, 42));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,43);
assert(rc == EOK);
memcpy(mem2, TEST_STR,43);
assert(0 == memcmp(mem1 + 3, mem2, 43));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,44);
assert(rc == EOK);
memcpy(mem2, TEST_STR,44);
assert(0 == memcmp(mem1 + 3, mem2, 44));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,45);
assert(rc == EOK);
memcpy(mem2, TEST_STR,45);
assert(0 == memcmp(mem1 + 3, mem2, 45));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,46);
assert(rc == EOK);
memcpy(mem2, TEST_STR,46);
assert(0 == memcmp(mem1 + 3, mem2, 46));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,47);
assert(rc == EOK);
memcpy(mem2, TEST_STR,47);
assert(0 == memcmp(mem1 + 3, mem2, 47));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,48);
assert(rc == EOK);
memcpy(mem2, TEST_STR,48);
assert(0 == memcmp(mem1 + 3, mem2, 48));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,49);
assert(rc == EOK);
memcpy(mem2, TEST_STR,49);
assert(0 == memcmp(mem1 + 3, mem2, 49));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,50);
assert(rc == EOK);
memcpy(mem2, TEST_STR,50);
assert(0 == memcmp(mem1 + 3, mem2, 50));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,51);
assert(rc == EOK);
memcpy(mem2, TEST_STR,51);
assert(0 == memcmp(mem1 + 3, mem2, 51));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,52);
assert(rc == EOK);
memcpy(mem2, TEST_STR,52);
assert(0 == memcmp(mem1 + 3, mem2, 52));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,53);
assert(rc == EOK);
memcpy(mem2, TEST_STR,53);
assert(0 == memcmp(mem1 + 3, mem2, 53));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,54);
assert(rc == EOK);
memcpy(mem2, TEST_STR,54);
assert(0 == memcmp(mem1 + 3, mem2, 54));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,55);
assert(rc == EOK);
memcpy(mem2, TEST_STR,55);
assert(0 == memcmp(mem1 + 3, mem2, 55));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,56);
assert(rc == EOK);
memcpy(mem2, TEST_STR,56);
assert(0 == memcmp(mem1 + 3, mem2, 56));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,57);
assert(rc == EOK);
memcpy(mem2, TEST_STR,57);
assert(0 == memcmp(mem1 + 3, mem2, 57));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,58);
assert(rc == EOK);
memcpy(mem2, TEST_STR,58);
assert(0 == memcmp(mem1 + 3, mem2, 58));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,59);
assert(rc == EOK);
memcpy(mem2, TEST_STR,59);
assert(0 == memcmp(mem1 + 3, mem2, 59));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,60);
assert(rc == EOK);
memcpy(mem2, TEST_STR,60);
assert(0 == memcmp(mem1 + 3, mem2, 60));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,61);
assert(rc == EOK);
memcpy(mem2, TEST_STR,61);
assert(0 == memcmp(mem1 + 3, mem2, 61));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,62);
assert(rc == EOK);
memcpy(mem2, TEST_STR,62);
assert(0 == memcmp(mem1 + 3, mem2, 62));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,63);
assert(rc == EOK);
memcpy(mem2, TEST_STR,63);
assert(0 == memcmp(mem1 + 3, mem2, 63));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,64);
assert(rc == EOK);
memcpy(mem2, TEST_STR,64);
assert(0 == memcmp(mem1 + 3, mem2, 64));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,65);
assert(rc == EOK);
memcpy(mem2, TEST_STR,65);
assert(0 == memcmp(mem1 + 3, mem2, 65));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,66);
assert(rc == EOK);
memcpy(mem2, TEST_STR,66);
assert(0 == memcmp(mem1 + 3, mem2, 66));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,67);
assert(rc == EOK);
memcpy(mem2, TEST_STR,67);
assert(0 == memcmp(mem1 + 3, mem2, 67));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,68);
assert(rc == EOK);
memcpy(mem2, TEST_STR,68);
assert(0 == memcmp(mem1 + 3, mem2, 68));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,69);
assert(rc == EOK);
memcpy(mem2, TEST_STR,69);
assert(0 == memcmp(mem1 + 3, mem2, 69));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,70);
assert(rc == EOK);
memcpy(mem2, TEST_STR,70);
assert(0 == memcmp(mem1 + 3, mem2, 70));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,71);
assert(rc == EOK);
memcpy(mem2, TEST_STR,71);
assert(0 == memcmp(mem1 + 3, mem2, 71));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,72);
assert(rc == EOK);
memcpy(mem2, TEST_STR,72);
assert(0 == memcmp(mem1 + 3, mem2, 72));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,73);
assert(rc == EOK);
memcpy(mem2, TEST_STR,73);
assert(0 == memcmp(mem1 + 3, mem2, 73));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,74);
assert(rc == EOK);
memcpy(mem2, TEST_STR,74);
assert(0 == memcmp(mem1 + 3, mem2, 74));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,75);
assert(rc == EOK);
memcpy(mem2, TEST_STR,75);
assert(0 == memcmp(mem1 + 3, mem2, 75));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,76);
assert(rc == EOK);
memcpy(mem2, TEST_STR,76);
assert(0 == memcmp(mem1 + 3, mem2, 76));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,77);
assert(rc == EOK);
memcpy(mem2, TEST_STR,77);
assert(0 == memcmp(mem1 + 3, mem2, 77));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,78);
assert(rc == EOK);
memcpy(mem2, TEST_STR,78);
assert(0 == memcmp(mem1 + 3, mem2, 78));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,79);
assert(rc == EOK);
memcpy(mem2, TEST_STR,79);
assert(0 == memcmp(mem1 + 3, mem2, 79));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,80);
assert(rc == EOK);
memcpy(mem2, TEST_STR,80);
assert(0 == memcmp(mem1 + 3, mem2, 80));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,81);
assert(rc == EOK);
memcpy(mem2, TEST_STR,81);
assert(0 == memcmp(mem1 + 3, mem2, 81));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,82);
assert(rc == EOK);
memcpy(mem2, TEST_STR,82);
assert(0 == memcmp(mem1 + 3, mem2, 82));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,83);
assert(rc == EOK);
memcpy(mem2, TEST_STR,83);
assert(0 == memcmp(mem1 + 3, mem2, 83));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,84);
assert(rc == EOK);
memcpy(mem2, TEST_STR,84);
assert(0 == memcmp(mem1 + 3, mem2, 84));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,85);
assert(rc == EOK);
memcpy(mem2, TEST_STR,85);
assert(0 == memcmp(mem1 + 3, mem2, 85));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,86);
assert(rc == EOK);
memcpy(mem2, TEST_STR,86);
assert(0 == memcmp(mem1 + 3, mem2, 86));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,87);
assert(rc == EOK);
memcpy(mem2, TEST_STR,87);
assert(0 == memcmp(mem1 + 3, mem2, 87));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,88);
assert(rc == EOK);
memcpy(mem2, TEST_STR,88);
assert(0 == memcmp(mem1 + 3, mem2, 88));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,89);
assert(rc == EOK);
memcpy(mem2, TEST_STR,89);
assert(0 == memcmp(mem1 + 3, mem2, 89));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,90);
assert(rc == EOK);
memcpy(mem2, TEST_STR,90);
assert(0 == memcmp(mem1 + 3, mem2, 90));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,91);
assert(rc == EOK);
memcpy(mem2, TEST_STR,91);
assert(0 == memcmp(mem1 + 3, mem2, 91));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,92);
assert(rc == EOK);
memcpy(mem2, TEST_STR,92);
assert(0 == memcmp(mem1 + 3, mem2, 92));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,93);
assert(rc == EOK);
memcpy(mem2, TEST_STR,93);
assert(0 == memcmp(mem1 + 3, mem2, 93));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,94);
assert(rc == EOK);
memcpy(mem2, TEST_STR,94);
assert(0 == memcmp(mem1 + 3, mem2, 94));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,95);
assert(rc == EOK);
memcpy(mem2, TEST_STR,95);
assert(0 == memcmp(mem1 + 3, mem2, 95));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,96);
assert(rc == EOK);
memcpy(mem2, TEST_STR,96);
assert(0 == memcmp(mem1 + 3, mem2, 96));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,97);
assert(rc == EOK);
memcpy(mem2, TEST_STR,97);
assert(0 == memcmp(mem1 + 3, mem2, 97));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,98);
assert(rc == EOK);
memcpy(mem2, TEST_STR,98);
assert(0 == memcmp(mem1 + 3, mem2, 98));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,99);
assert(rc == EOK);
memcpy(mem2, TEST_STR,99);
assert(0 == memcmp(mem1 + 3, mem2, 99));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,100);
assert(rc == EOK);
memcpy(mem2, TEST_STR,100);
assert(0 == memcmp(mem1 + 3, mem2, 100));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,101);
assert(rc == EOK);
memcpy(mem2, TEST_STR,101);
assert(0 == memcmp(mem1 + 3, mem2, 101));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,102);
assert(rc == EOK);
memcpy(mem2, TEST_STR,102);
assert(0 == memcmp(mem1 + 3, mem2, 102));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,103);
assert(rc == EOK);
memcpy(mem2, TEST_STR,103);
assert(0 == memcmp(mem1 + 3, mem2, 103));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,104);
assert(rc == EOK);
memcpy(mem2, TEST_STR,104);
assert(0 == memcmp(mem1 + 3, mem2, 104));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,105);
assert(rc == EOK);
memcpy(mem2, TEST_STR,105);
assert(0 == memcmp(mem1 + 3, mem2, 105));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,106);
assert(rc == EOK);
memcpy(mem2, TEST_STR,106);
assert(0 == memcmp(mem1 + 3, mem2, 106));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,107);
assert(rc == EOK);
memcpy(mem2, TEST_STR,107);
assert(0 == memcmp(mem1 + 3, mem2, 107));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,108);
assert(rc == EOK);
memcpy(mem2, TEST_STR,108);
assert(0 == memcmp(mem1 + 3, mem2, 108));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,109);
assert(rc == EOK);
memcpy(mem2, TEST_STR,109);
assert(0 == memcmp(mem1 + 3, mem2, 109));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,110);
assert(rc == EOK);
memcpy(mem2, TEST_STR,110);
assert(0 == memcmp(mem1 + 3, mem2, 110));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,111);
assert(rc == EOK);
memcpy(mem2, TEST_STR,111);
assert(0 == memcmp(mem1 + 3, mem2, 111));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,112);
assert(rc == EOK);
memcpy(mem2, TEST_STR,112);
assert(0 == memcmp(mem1 + 3, mem2, 112));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,113);
assert(rc == EOK);
memcpy(mem2, TEST_STR,113);
assert(0 == memcmp(mem1 + 3, mem2, 113));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,114);
assert(rc == EOK);
memcpy(mem2, TEST_STR,114);
assert(0 == memcmp(mem1 + 3, mem2, 114));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,115);
assert(rc == EOK);
memcpy(mem2, TEST_STR,115);
assert(0 == memcmp(mem1 + 3, mem2, 115));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,116);
assert(rc == EOK);
memcpy(mem2, TEST_STR,116);
assert(0 == memcmp(mem1 + 3, mem2, 116));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,117);
assert(rc == EOK);
memcpy(mem2, TEST_STR,117);
assert(0 == memcmp(mem1 + 3, mem2, 117));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,118);
assert(rc == EOK);
memcpy(mem2, TEST_STR,118);
assert(0 == memcmp(mem1 + 3, mem2, 118));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,119);
assert(rc == EOK);
memcpy(mem2, TEST_STR,119);
assert(0 == memcmp(mem1 + 3, mem2, 119));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,120);
assert(rc == EOK);
memcpy(mem2, TEST_STR,120);
assert(0 == memcmp(mem1 + 3, mem2, 120));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,121);
assert(rc == EOK);
memcpy(mem2, TEST_STR,121);
assert(0 == memcmp(mem1 + 3, mem2, 121));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,122);
assert(rc == EOK);
memcpy(mem2, TEST_STR,122);
assert(0 == memcmp(mem1 + 3, mem2, 122));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,123);
assert(rc == EOK);
memcpy(mem2, TEST_STR,123);
assert(0 == memcmp(mem1 + 3, mem2, 123));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,124);
assert(rc == EOK);
memcpy(mem2, TEST_STR,124);
assert(0 == memcmp(mem1 + 3, mem2, 124));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,125);
assert(rc == EOK);
memcpy(mem2, TEST_STR,125);
assert(0 == memcmp(mem1 + 3, mem2, 125));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,126);
assert(rc == EOK);
memcpy(mem2, TEST_STR,126);
assert(0 == memcmp(mem1 + 3, mem2, 126));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,127);
assert(rc == EOK);
memcpy(mem2, TEST_STR,127);
assert(0 == memcmp(mem1 + 3, mem2, 127));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,128);
assert(rc == EOK);
memcpy(mem2, TEST_STR,128);
assert(0 == memcmp(mem1 + 3, mem2, 128));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,129);
assert(rc == EOK);
memcpy(mem2, TEST_STR,129);
assert(0 == memcmp(mem1 + 3, mem2, 129));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,130);
assert(rc == EOK);
memcpy(mem2, TEST_STR,130);
assert(0 == memcmp(mem1 + 3, mem2, 130));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,131);
assert(rc == EOK);
memcpy(mem2, TEST_STR,131);
assert(0 == memcmp(mem1 + 3, mem2, 131));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,132);
assert(rc == EOK);
memcpy(mem2, TEST_STR,132);
assert(0 == memcmp(mem1 + 3, mem2, 132));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,133);
assert(rc == EOK);
memcpy(mem2, TEST_STR,133);
assert(0 == memcmp(mem1 + 3, mem2, 133));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,134);
assert(rc == EOK);
memcpy(mem2, TEST_STR,134);
assert(0 == memcmp(mem1 + 3, mem2, 134));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,135);
assert(rc == EOK);
memcpy(mem2, TEST_STR,135);
assert(0 == memcmp(mem1 + 3, mem2, 135));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,136);
assert(rc == EOK);
memcpy(mem2, TEST_STR,136);
assert(0 == memcmp(mem1 + 3, mem2, 136));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,137);
assert(rc == EOK);
memcpy(mem2, TEST_STR,137);
assert(0 == memcmp(mem1 + 3, mem2, 137));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,138);
assert(rc == EOK);
memcpy(mem2, TEST_STR,138);
assert(0 == memcmp(mem1 + 3, mem2, 138));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,139);
assert(rc == EOK);
memcpy(mem2, TEST_STR,139);
assert(0 == memcmp(mem1 + 3, mem2, 139));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,140);
assert(rc == EOK);
memcpy(mem2, TEST_STR,140);
assert(0 == memcmp(mem1 + 3, mem2, 140));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,141);
assert(rc == EOK);
memcpy(mem2, TEST_STR,141);
assert(0 == memcmp(mem1 + 3, mem2, 141));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,142);
assert(rc == EOK);
memcpy(mem2, TEST_STR,142);
assert(0 == memcmp(mem1 + 3, mem2, 142));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,143);
assert(rc == EOK);
memcpy(mem2, TEST_STR,143);
assert(0 == memcmp(mem1 + 3, mem2, 143));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,144);
assert(rc == EOK);
memcpy(mem2, TEST_STR,144);
assert(0 == memcmp(mem1 + 3, mem2, 144));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,145);
assert(rc == EOK);
memcpy(mem2, TEST_STR,145);
assert(0 == memcmp(mem1 + 3, mem2, 145));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,146);
assert(rc == EOK);
memcpy(mem2, TEST_STR,146);
assert(0 == memcmp(mem1 + 3, mem2, 146));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,147);
assert(rc == EOK);
memcpy(mem2, TEST_STR,147);
assert(0 == memcmp(mem1 + 3, mem2, 147));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,148);
assert(rc == EOK);
memcpy(mem2, TEST_STR,148);
assert(0 == memcmp(mem1 + 3, mem2, 148));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,149);
assert(rc == EOK);
memcpy(mem2, TEST_STR,149);
assert(0 == memcmp(mem1 + 3, mem2, 149));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,150);
assert(rc == EOK);
memcpy(mem2, TEST_STR,150);
assert(0 == memcmp(mem1 + 3, mem2, 150));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,151);
assert(rc == EOK);
memcpy(mem2, TEST_STR,151);
assert(0 == memcmp(mem1 + 3, mem2, 151));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,152);
assert(rc == EOK);
memcpy(mem2, TEST_STR,152);
assert(0 == memcmp(mem1 + 3, mem2, 152));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,153);
assert(rc == EOK);
memcpy(mem2, TEST_STR,153);
assert(0 == memcmp(mem1 + 3, mem2, 153));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,154);
assert(rc == EOK);
memcpy(mem2, TEST_STR,154);
assert(0 == memcmp(mem1 + 3, mem2, 154));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,155);
assert(rc == EOK);
memcpy(mem2, TEST_STR,155);
assert(0 == memcmp(mem1 + 3, mem2, 155));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,156);
assert(rc == EOK);
memcpy(mem2, TEST_STR,156);
assert(0 == memcmp(mem1 + 3, mem2, 156));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,157);
assert(rc == EOK);
memcpy(mem2, TEST_STR,157);
assert(0 == memcmp(mem1 + 3, mem2, 157));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,158);
assert(rc == EOK);
memcpy(mem2, TEST_STR,158);
assert(0 == memcmp(mem1 + 3, mem2, 158));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,159);
assert(rc == EOK);
memcpy(mem2, TEST_STR,159);
assert(0 == memcmp(mem1 + 3, mem2, 159));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,160);
assert(rc == EOK);
memcpy(mem2, TEST_STR,160);
assert(0 == memcmp(mem1 + 3, mem2, 160));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,161);
assert(rc == EOK);
memcpy(mem2, TEST_STR,161);
assert(0 == memcmp(mem1 + 3, mem2, 161));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,162);
assert(rc == EOK);
memcpy(mem2, TEST_STR,162);
assert(0 == memcmp(mem1 + 3, mem2, 162));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,163);
assert(rc == EOK);
memcpy(mem2, TEST_STR,163);
assert(0 == memcmp(mem1 + 3, mem2, 163));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,164);
assert(rc == EOK);
memcpy(mem2, TEST_STR,164);
assert(0 == memcmp(mem1 + 3, mem2, 164));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,165);
assert(rc == EOK);
memcpy(mem2, TEST_STR,165);
assert(0 == memcmp(mem1 + 3, mem2, 165));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,166);
assert(rc == EOK);
memcpy(mem2, TEST_STR,166);
assert(0 == memcmp(mem1 + 3, mem2, 166));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,167);
assert(rc == EOK);
memcpy(mem2, TEST_STR,167);
assert(0 == memcmp(mem1 + 3, mem2, 167));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,168);
assert(rc == EOK);
memcpy(mem2, TEST_STR,168);
assert(0 == memcmp(mem1 + 3, mem2, 168));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,169);
assert(rc == EOK);
memcpy(mem2, TEST_STR,169);
assert(0 == memcmp(mem1 + 3, mem2, 169));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,170);
assert(rc == EOK);
memcpy(mem2, TEST_STR,170);
assert(0 == memcmp(mem1 + 3, mem2, 170));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,171);
assert(rc == EOK);
memcpy(mem2, TEST_STR,171);
assert(0 == memcmp(mem1 + 3, mem2, 171));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,172);
assert(rc == EOK);
memcpy(mem2, TEST_STR,172);
assert(0 == memcmp(mem1 + 3, mem2, 172));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,173);
assert(rc == EOK);
memcpy(mem2, TEST_STR,173);
assert(0 == memcmp(mem1 + 3, mem2, 173));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,174);
assert(rc == EOK);
memcpy(mem2, TEST_STR,174);
assert(0 == memcmp(mem1 + 3, mem2, 174));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,175);
assert(rc == EOK);
memcpy(mem2, TEST_STR,175);
assert(0 == memcmp(mem1 + 3, mem2, 175));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,176);
assert(rc == EOK);
memcpy(mem2, TEST_STR,176);
assert(0 == memcmp(mem1 + 3, mem2, 176));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,177);
assert(rc == EOK);
memcpy(mem2, TEST_STR,177);
assert(0 == memcmp(mem1 + 3, mem2, 177));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,178);
assert(rc == EOK);
memcpy(mem2, TEST_STR,178);
assert(0 == memcmp(mem1 + 3, mem2, 178));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,179);
assert(rc == EOK);
memcpy(mem2, TEST_STR,179);
assert(0 == memcmp(mem1 + 3, mem2, 179));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,180);
assert(rc == EOK);
memcpy(mem2, TEST_STR,180);
assert(0 == memcmp(mem1 + 3, mem2, 180));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,181);
assert(rc == EOK);
memcpy(mem2, TEST_STR,181);
assert(0 == memcmp(mem1 + 3, mem2, 181));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,182);
assert(rc == EOK);
memcpy(mem2, TEST_STR,182);
assert(0 == memcmp(mem1 + 3, mem2, 182));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,183);
assert(rc == EOK);
memcpy(mem2, TEST_STR,183);
assert(0 == memcmp(mem1 + 3, mem2, 183));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,184);
assert(rc == EOK);
memcpy(mem2, TEST_STR,184);
assert(0 == memcmp(mem1 + 3, mem2, 184));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,185);
assert(rc == EOK);
memcpy(mem2, TEST_STR,185);
assert(0 == memcmp(mem1 + 3, mem2, 185));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,186);
assert(rc == EOK);
memcpy(mem2, TEST_STR,186);
assert(0 == memcmp(mem1 + 3, mem2, 186));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,187);
assert(rc == EOK);
memcpy(mem2, TEST_STR,187);
assert(0 == memcmp(mem1 + 3, mem2, 187));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,188);
assert(rc == EOK);
memcpy(mem2, TEST_STR,188);
assert(0 == memcmp(mem1 + 3, mem2, 188));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,189);
assert(rc == EOK);
memcpy(mem2, TEST_STR,189);
assert(0 == memcmp(mem1 + 3, mem2, 189));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,190);
assert(rc == EOK);
memcpy(mem2, TEST_STR,190);
assert(0 == memcmp(mem1 + 3, mem2, 190));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,191);
assert(rc == EOK);
memcpy(mem2, TEST_STR,191);
assert(0 == memcmp(mem1 + 3, mem2, 191));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,192);
assert(rc == EOK);
memcpy(mem2, TEST_STR,192);
assert(0 == memcmp(mem1 + 3, mem2, 192));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,193);
assert(rc == EOK);
memcpy(mem2, TEST_STR,193);
assert(0 == memcmp(mem1 + 3, mem2, 193));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,194);
assert(rc == EOK);
memcpy(mem2, TEST_STR,194);
assert(0 == memcmp(mem1 + 3, mem2, 194));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,195);
assert(rc == EOK);
memcpy(mem2, TEST_STR,195);
assert(0 == memcmp(mem1 + 3, mem2, 195));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,196);
assert(rc == EOK);
memcpy(mem2, TEST_STR,196);
assert(0 == memcmp(mem1 + 3, mem2, 196));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,197);
assert(rc == EOK);
memcpy(mem2, TEST_STR,197);
assert(0 == memcmp(mem1 + 3, mem2, 197));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,198);
assert(rc == EOK);
memcpy(mem2, TEST_STR,198);
assert(0 == memcmp(mem1 + 3, mem2, 198));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,199);
assert(rc == EOK);
memcpy(mem2, TEST_STR,199);
assert(0 == memcmp(mem1 + 3, mem2, 199));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 3, MEM_LEN, TEST_STR,200);
assert(rc == EOK);
memcpy(mem2, TEST_STR,200);
assert(0 == memcmp(mem1 + 3, mem2, 200));
memset_s(mem1, MEM_LEN, 0, 210);


}
void TestOffset4()
{
	static UINT8T  mem1[MEM_LEN + 2];
	static UINT8T  mem2[MEM_LEN + 2];
	errno_t rc;

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,0);
assert(rc == EOK);
memcpy(mem2, TEST_STR,0);
assert(0 == memcmp(mem1 + 4, mem2, 0));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,1);
assert(rc == EOK);
memcpy(mem2, TEST_STR,1);
assert(0 == memcmp(mem1 + 4, mem2, 1));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,2);
assert(rc == EOK);
memcpy(mem2, TEST_STR,2);
assert(0 == memcmp(mem1 + 4, mem2, 2));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,3);
assert(rc == EOK);
memcpy(mem2, TEST_STR,3);
assert(0 == memcmp(mem1 + 4, mem2, 3));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,4);
assert(rc == EOK);
memcpy(mem2, TEST_STR,4);
assert(0 == memcmp(mem1 + 4, mem2, 4));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,5);
assert(rc == EOK);
memcpy(mem2, TEST_STR,5);
assert(0 == memcmp(mem1 + 4, mem2, 5));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,6);
assert(rc == EOK);
memcpy(mem2, TEST_STR,6);
assert(0 == memcmp(mem1 + 4, mem2, 6));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,7);
assert(rc == EOK);
memcpy(mem2, TEST_STR,7);
assert(0 == memcmp(mem1 + 4, mem2, 7));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,8);
assert(rc == EOK);
memcpy(mem2, TEST_STR,8);
assert(0 == memcmp(mem1 + 4, mem2, 8));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,9);
assert(rc == EOK);
memcpy(mem2, TEST_STR,9);
assert(0 == memcmp(mem1 + 4, mem2, 9));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,10);
assert(rc == EOK);
memcpy(mem2, TEST_STR,10);
assert(0 == memcmp(mem1 + 4, mem2, 10));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,11);
assert(rc == EOK);
memcpy(mem2, TEST_STR,11);
assert(0 == memcmp(mem1 + 4, mem2, 11));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,12);
assert(rc == EOK);
memcpy(mem2, TEST_STR,12);
assert(0 == memcmp(mem1 + 4, mem2, 12));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,13);
assert(rc == EOK);
memcpy(mem2, TEST_STR,13);
assert(0 == memcmp(mem1 + 4, mem2, 13));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,14);
assert(rc == EOK);
memcpy(mem2, TEST_STR,14);
assert(0 == memcmp(mem1 + 4, mem2, 14));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,15);
assert(rc == EOK);
memcpy(mem2, TEST_STR,15);
assert(0 == memcmp(mem1 + 4, mem2, 15));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,16);
assert(rc == EOK);
memcpy(mem2, TEST_STR,16);
assert(0 == memcmp(mem1 + 4, mem2, 16));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,17);
assert(rc == EOK);
memcpy(mem2, TEST_STR,17);
assert(0 == memcmp(mem1 + 4, mem2, 17));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,18);
assert(rc == EOK);
memcpy(mem2, TEST_STR,18);
assert(0 == memcmp(mem1 + 4, mem2, 18));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,19);
assert(rc == EOK);
memcpy(mem2, TEST_STR,19);
assert(0 == memcmp(mem1 + 4, mem2, 19));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,20);
assert(rc == EOK);
memcpy(mem2, TEST_STR,20);
assert(0 == memcmp(mem1 + 4, mem2, 20));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,21);
assert(rc == EOK);
memcpy(mem2, TEST_STR,21);
assert(0 == memcmp(mem1 + 4, mem2, 21));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,22);
assert(rc == EOK);
memcpy(mem2, TEST_STR,22);
assert(0 == memcmp(mem1 + 4, mem2, 22));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,23);
assert(rc == EOK);
memcpy(mem2, TEST_STR,23);
assert(0 == memcmp(mem1 + 4, mem2, 23));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,24);
assert(rc == EOK);
memcpy(mem2, TEST_STR,24);
assert(0 == memcmp(mem1 + 4, mem2, 24));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,25);
assert(rc == EOK);
memcpy(mem2, TEST_STR,25);
assert(0 == memcmp(mem1 + 4, mem2, 25));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,26);
assert(rc == EOK);
memcpy(mem2, TEST_STR,26);
assert(0 == memcmp(mem1 + 4, mem2, 26));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,27);
assert(rc == EOK);
memcpy(mem2, TEST_STR,27);
assert(0 == memcmp(mem1 + 4, mem2, 27));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,28);
assert(rc == EOK);
memcpy(mem2, TEST_STR,28);
assert(0 == memcmp(mem1 + 4, mem2, 28));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,29);
assert(rc == EOK);
memcpy(mem2, TEST_STR,29);
assert(0 == memcmp(mem1 + 4, mem2, 29));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,30);
assert(rc == EOK);
memcpy(mem2, TEST_STR,30);
assert(0 == memcmp(mem1 + 4, mem2, 30));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,31);
assert(rc == EOK);
memcpy(mem2, TEST_STR,31);
assert(0 == memcmp(mem1 + 4, mem2, 31));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,32);
assert(rc == EOK);
memcpy(mem2, TEST_STR,32);
assert(0 == memcmp(mem1 + 4, mem2, 32));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,33);
assert(rc == EOK);
memcpy(mem2, TEST_STR,33);
assert(0 == memcmp(mem1 + 4, mem2, 33));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,34);
assert(rc == EOK);
memcpy(mem2, TEST_STR,34);
assert(0 == memcmp(mem1 + 4, mem2, 34));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,35);
assert(rc == EOK);
memcpy(mem2, TEST_STR,35);
assert(0 == memcmp(mem1 + 4, mem2, 35));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,36);
assert(rc == EOK);
memcpy(mem2, TEST_STR,36);
assert(0 == memcmp(mem1 + 4, mem2, 36));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,37);
assert(rc == EOK);
memcpy(mem2, TEST_STR,37);
assert(0 == memcmp(mem1 + 4, mem2, 37));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,38);
assert(rc == EOK);
memcpy(mem2, TEST_STR,38);
assert(0 == memcmp(mem1 + 4, mem2, 38));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,39);
assert(rc == EOK);
memcpy(mem2, TEST_STR,39);
assert(0 == memcmp(mem1 + 4, mem2, 39));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,40);
assert(rc == EOK);
memcpy(mem2, TEST_STR,40);
assert(0 == memcmp(mem1 + 4, mem2, 40));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,41);
assert(rc == EOK);
memcpy(mem2, TEST_STR,41);
assert(0 == memcmp(mem1 + 4, mem2, 41));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,42);
assert(rc == EOK);
memcpy(mem2, TEST_STR,42);
assert(0 == memcmp(mem1 + 4, mem2, 42));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,43);
assert(rc == EOK);
memcpy(mem2, TEST_STR,43);
assert(0 == memcmp(mem1 + 4, mem2, 43));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,44);
assert(rc == EOK);
memcpy(mem2, TEST_STR,44);
assert(0 == memcmp(mem1 + 4, mem2, 44));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,45);
assert(rc == EOK);
memcpy(mem2, TEST_STR,45);
assert(0 == memcmp(mem1 + 4, mem2, 45));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,46);
assert(rc == EOK);
memcpy(mem2, TEST_STR,46);
assert(0 == memcmp(mem1 + 4, mem2, 46));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,47);
assert(rc == EOK);
memcpy(mem2, TEST_STR,47);
assert(0 == memcmp(mem1 + 4, mem2, 47));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,48);
assert(rc == EOK);
memcpy(mem2, TEST_STR,48);
assert(0 == memcmp(mem1 + 4, mem2, 48));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,49);
assert(rc == EOK);
memcpy(mem2, TEST_STR,49);
assert(0 == memcmp(mem1 + 4, mem2, 49));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,50);
assert(rc == EOK);
memcpy(mem2, TEST_STR,50);
assert(0 == memcmp(mem1 + 4, mem2, 50));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,51);
assert(rc == EOK);
memcpy(mem2, TEST_STR,51);
assert(0 == memcmp(mem1 + 4, mem2, 51));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,52);
assert(rc == EOK);
memcpy(mem2, TEST_STR,52);
assert(0 == memcmp(mem1 + 4, mem2, 52));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,53);
assert(rc == EOK);
memcpy(mem2, TEST_STR,53);
assert(0 == memcmp(mem1 + 4, mem2, 53));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,54);
assert(rc == EOK);
memcpy(mem2, TEST_STR,54);
assert(0 == memcmp(mem1 + 4, mem2, 54));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,55);
assert(rc == EOK);
memcpy(mem2, TEST_STR,55);
assert(0 == memcmp(mem1 + 4, mem2, 55));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,56);
assert(rc == EOK);
memcpy(mem2, TEST_STR,56);
assert(0 == memcmp(mem1 + 4, mem2, 56));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,57);
assert(rc == EOK);
memcpy(mem2, TEST_STR,57);
assert(0 == memcmp(mem1 + 4, mem2, 57));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,58);
assert(rc == EOK);
memcpy(mem2, TEST_STR,58);
assert(0 == memcmp(mem1 + 4, mem2, 58));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,59);
assert(rc == EOK);
memcpy(mem2, TEST_STR,59);
assert(0 == memcmp(mem1 + 4, mem2, 59));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,60);
assert(rc == EOK);
memcpy(mem2, TEST_STR,60);
assert(0 == memcmp(mem1 + 4, mem2, 60));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,61);
assert(rc == EOK);
memcpy(mem2, TEST_STR,61);
assert(0 == memcmp(mem1 + 4, mem2, 61));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,62);
assert(rc == EOK);
memcpy(mem2, TEST_STR,62);
assert(0 == memcmp(mem1 + 4, mem2, 62));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,63);
assert(rc == EOK);
memcpy(mem2, TEST_STR,63);
assert(0 == memcmp(mem1 + 4, mem2, 63));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,64);
assert(rc == EOK);
memcpy(mem2, TEST_STR,64);
assert(0 == memcmp(mem1 + 4, mem2, 64));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,65);
assert(rc == EOK);
memcpy(mem2, TEST_STR,65);
assert(0 == memcmp(mem1 + 4, mem2, 65));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,66);
assert(rc == EOK);
memcpy(mem2, TEST_STR,66);
assert(0 == memcmp(mem1 + 4, mem2, 66));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,67);
assert(rc == EOK);
memcpy(mem2, TEST_STR,67);
assert(0 == memcmp(mem1 + 4, mem2, 67));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,68);
assert(rc == EOK);
memcpy(mem2, TEST_STR,68);
assert(0 == memcmp(mem1 + 4, mem2, 68));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,69);
assert(rc == EOK);
memcpy(mem2, TEST_STR,69);
assert(0 == memcmp(mem1 + 4, mem2, 69));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,70);
assert(rc == EOK);
memcpy(mem2, TEST_STR,70);
assert(0 == memcmp(mem1 + 4, mem2, 70));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,71);
assert(rc == EOK);
memcpy(mem2, TEST_STR,71);
assert(0 == memcmp(mem1 + 4, mem2, 71));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,72);
assert(rc == EOK);
memcpy(mem2, TEST_STR,72);
assert(0 == memcmp(mem1 + 4, mem2, 72));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,73);
assert(rc == EOK);
memcpy(mem2, TEST_STR,73);
assert(0 == memcmp(mem1 + 4, mem2, 73));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,74);
assert(rc == EOK);
memcpy(mem2, TEST_STR,74);
assert(0 == memcmp(mem1 + 4, mem2, 74));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,75);
assert(rc == EOK);
memcpy(mem2, TEST_STR,75);
assert(0 == memcmp(mem1 + 4, mem2, 75));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,76);
assert(rc == EOK);
memcpy(mem2, TEST_STR,76);
assert(0 == memcmp(mem1 + 4, mem2, 76));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,77);
assert(rc == EOK);
memcpy(mem2, TEST_STR,77);
assert(0 == memcmp(mem1 + 4, mem2, 77));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,78);
assert(rc == EOK);
memcpy(mem2, TEST_STR,78);
assert(0 == memcmp(mem1 + 4, mem2, 78));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,79);
assert(rc == EOK);
memcpy(mem2, TEST_STR,79);
assert(0 == memcmp(mem1 + 4, mem2, 79));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,80);
assert(rc == EOK);
memcpy(mem2, TEST_STR,80);
assert(0 == memcmp(mem1 + 4, mem2, 80));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,81);
assert(rc == EOK);
memcpy(mem2, TEST_STR,81);
assert(0 == memcmp(mem1 + 4, mem2, 81));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,82);
assert(rc == EOK);
memcpy(mem2, TEST_STR,82);
assert(0 == memcmp(mem1 + 4, mem2, 82));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,83);
assert(rc == EOK);
memcpy(mem2, TEST_STR,83);
assert(0 == memcmp(mem1 + 4, mem2, 83));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,84);
assert(rc == EOK);
memcpy(mem2, TEST_STR,84);
assert(0 == memcmp(mem1 + 4, mem2, 84));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,85);
assert(rc == EOK);
memcpy(mem2, TEST_STR,85);
assert(0 == memcmp(mem1 + 4, mem2, 85));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,86);
assert(rc == EOK);
memcpy(mem2, TEST_STR,86);
assert(0 == memcmp(mem1 + 4, mem2, 86));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,87);
assert(rc == EOK);
memcpy(mem2, TEST_STR,87);
assert(0 == memcmp(mem1 + 4, mem2, 87));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,88);
assert(rc == EOK);
memcpy(mem2, TEST_STR,88);
assert(0 == memcmp(mem1 + 4, mem2, 88));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,89);
assert(rc == EOK);
memcpy(mem2, TEST_STR,89);
assert(0 == memcmp(mem1 + 4, mem2, 89));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,90);
assert(rc == EOK);
memcpy(mem2, TEST_STR,90);
assert(0 == memcmp(mem1 + 4, mem2, 90));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,91);
assert(rc == EOK);
memcpy(mem2, TEST_STR,91);
assert(0 == memcmp(mem1 + 4, mem2, 91));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,92);
assert(rc == EOK);
memcpy(mem2, TEST_STR,92);
assert(0 == memcmp(mem1 + 4, mem2, 92));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,93);
assert(rc == EOK);
memcpy(mem2, TEST_STR,93);
assert(0 == memcmp(mem1 + 4, mem2, 93));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,94);
assert(rc == EOK);
memcpy(mem2, TEST_STR,94);
assert(0 == memcmp(mem1 + 4, mem2, 94));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,95);
assert(rc == EOK);
memcpy(mem2, TEST_STR,95);
assert(0 == memcmp(mem1 + 4, mem2, 95));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,96);
assert(rc == EOK);
memcpy(mem2, TEST_STR,96);
assert(0 == memcmp(mem1 + 4, mem2, 96));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,97);
assert(rc == EOK);
memcpy(mem2, TEST_STR,97);
assert(0 == memcmp(mem1 + 4, mem2, 97));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,98);
assert(rc == EOK);
memcpy(mem2, TEST_STR,98);
assert(0 == memcmp(mem1 + 4, mem2, 98));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,99);
assert(rc == EOK);
memcpy(mem2, TEST_STR,99);
assert(0 == memcmp(mem1 + 4, mem2, 99));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,100);
assert(rc == EOK);
memcpy(mem2, TEST_STR,100);
assert(0 == memcmp(mem1 + 4, mem2, 100));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,101);
assert(rc == EOK);
memcpy(mem2, TEST_STR,101);
assert(0 == memcmp(mem1 + 4, mem2, 101));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,102);
assert(rc == EOK);
memcpy(mem2, TEST_STR,102);
assert(0 == memcmp(mem1 + 4, mem2, 102));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,103);
assert(rc == EOK);
memcpy(mem2, TEST_STR,103);
assert(0 == memcmp(mem1 + 4, mem2, 103));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,104);
assert(rc == EOK);
memcpy(mem2, TEST_STR,104);
assert(0 == memcmp(mem1 + 4, mem2, 104));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,105);
assert(rc == EOK);
memcpy(mem2, TEST_STR,105);
assert(0 == memcmp(mem1 + 4, mem2, 105));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,106);
assert(rc == EOK);
memcpy(mem2, TEST_STR,106);
assert(0 == memcmp(mem1 + 4, mem2, 106));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,107);
assert(rc == EOK);
memcpy(mem2, TEST_STR,107);
assert(0 == memcmp(mem1 + 4, mem2, 107));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,108);
assert(rc == EOK);
memcpy(mem2, TEST_STR,108);
assert(0 == memcmp(mem1 + 4, mem2, 108));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,109);
assert(rc == EOK);
memcpy(mem2, TEST_STR,109);
assert(0 == memcmp(mem1 + 4, mem2, 109));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,110);
assert(rc == EOK);
memcpy(mem2, TEST_STR,110);
assert(0 == memcmp(mem1 + 4, mem2, 110));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,111);
assert(rc == EOK);
memcpy(mem2, TEST_STR,111);
assert(0 == memcmp(mem1 + 4, mem2, 111));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,112);
assert(rc == EOK);
memcpy(mem2, TEST_STR,112);
assert(0 == memcmp(mem1 + 4, mem2, 112));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,113);
assert(rc == EOK);
memcpy(mem2, TEST_STR,113);
assert(0 == memcmp(mem1 + 4, mem2, 113));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,114);
assert(rc == EOK);
memcpy(mem2, TEST_STR,114);
assert(0 == memcmp(mem1 + 4, mem2, 114));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,115);
assert(rc == EOK);
memcpy(mem2, TEST_STR,115);
assert(0 == memcmp(mem1 + 4, mem2, 115));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,116);
assert(rc == EOK);
memcpy(mem2, TEST_STR,116);
assert(0 == memcmp(mem1 + 4, mem2, 116));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,117);
assert(rc == EOK);
memcpy(mem2, TEST_STR,117);
assert(0 == memcmp(mem1 + 4, mem2, 117));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,118);
assert(rc == EOK);
memcpy(mem2, TEST_STR,118);
assert(0 == memcmp(mem1 + 4, mem2, 118));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,119);
assert(rc == EOK);
memcpy(mem2, TEST_STR,119);
assert(0 == memcmp(mem1 + 4, mem2, 119));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,120);
assert(rc == EOK);
memcpy(mem2, TEST_STR,120);
assert(0 == memcmp(mem1 + 4, mem2, 120));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,121);
assert(rc == EOK);
memcpy(mem2, TEST_STR,121);
assert(0 == memcmp(mem1 + 4, mem2, 121));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,122);
assert(rc == EOK);
memcpy(mem2, TEST_STR,122);
assert(0 == memcmp(mem1 + 4, mem2, 122));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,123);
assert(rc == EOK);
memcpy(mem2, TEST_STR,123);
assert(0 == memcmp(mem1 + 4, mem2, 123));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,124);
assert(rc == EOK);
memcpy(mem2, TEST_STR,124);
assert(0 == memcmp(mem1 + 4, mem2, 124));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,125);
assert(rc == EOK);
memcpy(mem2, TEST_STR,125);
assert(0 == memcmp(mem1 + 4, mem2, 125));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,126);
assert(rc == EOK);
memcpy(mem2, TEST_STR,126);
assert(0 == memcmp(mem1 + 4, mem2, 126));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,127);
assert(rc == EOK);
memcpy(mem2, TEST_STR,127);
assert(0 == memcmp(mem1 + 4, mem2, 127));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,128);
assert(rc == EOK);
memcpy(mem2, TEST_STR,128);
assert(0 == memcmp(mem1 + 4, mem2, 128));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,129);
assert(rc == EOK);
memcpy(mem2, TEST_STR,129);
assert(0 == memcmp(mem1 + 4, mem2, 129));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,130);
assert(rc == EOK);
memcpy(mem2, TEST_STR,130);
assert(0 == memcmp(mem1 + 4, mem2, 130));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,131);
assert(rc == EOK);
memcpy(mem2, TEST_STR,131);
assert(0 == memcmp(mem1 + 4, mem2, 131));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,132);
assert(rc == EOK);
memcpy(mem2, TEST_STR,132);
assert(0 == memcmp(mem1 + 4, mem2, 132));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,133);
assert(rc == EOK);
memcpy(mem2, TEST_STR,133);
assert(0 == memcmp(mem1 + 4, mem2, 133));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,134);
assert(rc == EOK);
memcpy(mem2, TEST_STR,134);
assert(0 == memcmp(mem1 + 4, mem2, 134));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,135);
assert(rc == EOK);
memcpy(mem2, TEST_STR,135);
assert(0 == memcmp(mem1 + 4, mem2, 135));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,136);
assert(rc == EOK);
memcpy(mem2, TEST_STR,136);
assert(0 == memcmp(mem1 + 4, mem2, 136));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,137);
assert(rc == EOK);
memcpy(mem2, TEST_STR,137);
assert(0 == memcmp(mem1 + 4, mem2, 137));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,138);
assert(rc == EOK);
memcpy(mem2, TEST_STR,138);
assert(0 == memcmp(mem1 + 4, mem2, 138));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,139);
assert(rc == EOK);
memcpy(mem2, TEST_STR,139);
assert(0 == memcmp(mem1 + 4, mem2, 139));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,140);
assert(rc == EOK);
memcpy(mem2, TEST_STR,140);
assert(0 == memcmp(mem1 + 4, mem2, 140));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,141);
assert(rc == EOK);
memcpy(mem2, TEST_STR,141);
assert(0 == memcmp(mem1 + 4, mem2, 141));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,142);
assert(rc == EOK);
memcpy(mem2, TEST_STR,142);
assert(0 == memcmp(mem1 + 4, mem2, 142));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,143);
assert(rc == EOK);
memcpy(mem2, TEST_STR,143);
assert(0 == memcmp(mem1 + 4, mem2, 143));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,144);
assert(rc == EOK);
memcpy(mem2, TEST_STR,144);
assert(0 == memcmp(mem1 + 4, mem2, 144));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,145);
assert(rc == EOK);
memcpy(mem2, TEST_STR,145);
assert(0 == memcmp(mem1 + 4, mem2, 145));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,146);
assert(rc == EOK);
memcpy(mem2, TEST_STR,146);
assert(0 == memcmp(mem1 + 4, mem2, 146));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,147);
assert(rc == EOK);
memcpy(mem2, TEST_STR,147);
assert(0 == memcmp(mem1 + 4, mem2, 147));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,148);
assert(rc == EOK);
memcpy(mem2, TEST_STR,148);
assert(0 == memcmp(mem1 + 4, mem2, 148));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,149);
assert(rc == EOK);
memcpy(mem2, TEST_STR,149);
assert(0 == memcmp(mem1 + 4, mem2, 149));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,150);
assert(rc == EOK);
memcpy(mem2, TEST_STR,150);
assert(0 == memcmp(mem1 + 4, mem2, 150));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,151);
assert(rc == EOK);
memcpy(mem2, TEST_STR,151);
assert(0 == memcmp(mem1 + 4, mem2, 151));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,152);
assert(rc == EOK);
memcpy(mem2, TEST_STR,152);
assert(0 == memcmp(mem1 + 4, mem2, 152));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,153);
assert(rc == EOK);
memcpy(mem2, TEST_STR,153);
assert(0 == memcmp(mem1 + 4, mem2, 153));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,154);
assert(rc == EOK);
memcpy(mem2, TEST_STR,154);
assert(0 == memcmp(mem1 + 4, mem2, 154));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,155);
assert(rc == EOK);
memcpy(mem2, TEST_STR,155);
assert(0 == memcmp(mem1 + 4, mem2, 155));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,156);
assert(rc == EOK);
memcpy(mem2, TEST_STR,156);
assert(0 == memcmp(mem1 + 4, mem2, 156));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,157);
assert(rc == EOK);
memcpy(mem2, TEST_STR,157);
assert(0 == memcmp(mem1 + 4, mem2, 157));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,158);
assert(rc == EOK);
memcpy(mem2, TEST_STR,158);
assert(0 == memcmp(mem1 + 4, mem2, 158));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,159);
assert(rc == EOK);
memcpy(mem2, TEST_STR,159);
assert(0 == memcmp(mem1 + 4, mem2, 159));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,160);
assert(rc == EOK);
memcpy(mem2, TEST_STR,160);
assert(0 == memcmp(mem1 + 4, mem2, 160));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,161);
assert(rc == EOK);
memcpy(mem2, TEST_STR,161);
assert(0 == memcmp(mem1 + 4, mem2, 161));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,162);
assert(rc == EOK);
memcpy(mem2, TEST_STR,162);
assert(0 == memcmp(mem1 + 4, mem2, 162));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,163);
assert(rc == EOK);
memcpy(mem2, TEST_STR,163);
assert(0 == memcmp(mem1 + 4, mem2, 163));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,164);
assert(rc == EOK);
memcpy(mem2, TEST_STR,164);
assert(0 == memcmp(mem1 + 4, mem2, 164));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,165);
assert(rc == EOK);
memcpy(mem2, TEST_STR,165);
assert(0 == memcmp(mem1 + 4, mem2, 165));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,166);
assert(rc == EOK);
memcpy(mem2, TEST_STR,166);
assert(0 == memcmp(mem1 + 4, mem2, 166));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,167);
assert(rc == EOK);
memcpy(mem2, TEST_STR,167);
assert(0 == memcmp(mem1 + 4, mem2, 167));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,168);
assert(rc == EOK);
memcpy(mem2, TEST_STR,168);
assert(0 == memcmp(mem1 + 4, mem2, 168));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,169);
assert(rc == EOK);
memcpy(mem2, TEST_STR,169);
assert(0 == memcmp(mem1 + 4, mem2, 169));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,170);
assert(rc == EOK);
memcpy(mem2, TEST_STR,170);
assert(0 == memcmp(mem1 + 4, mem2, 170));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,171);
assert(rc == EOK);
memcpy(mem2, TEST_STR,171);
assert(0 == memcmp(mem1 + 4, mem2, 171));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,172);
assert(rc == EOK);
memcpy(mem2, TEST_STR,172);
assert(0 == memcmp(mem1 + 4, mem2, 172));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,173);
assert(rc == EOK);
memcpy(mem2, TEST_STR,173);
assert(0 == memcmp(mem1 + 4, mem2, 173));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,174);
assert(rc == EOK);
memcpy(mem2, TEST_STR,174);
assert(0 == memcmp(mem1 + 4, mem2, 174));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,175);
assert(rc == EOK);
memcpy(mem2, TEST_STR,175);
assert(0 == memcmp(mem1 + 4, mem2, 175));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,176);
assert(rc == EOK);
memcpy(mem2, TEST_STR,176);
assert(0 == memcmp(mem1 + 4, mem2, 176));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,177);
assert(rc == EOK);
memcpy(mem2, TEST_STR,177);
assert(0 == memcmp(mem1 + 4, mem2, 177));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,178);
assert(rc == EOK);
memcpy(mem2, TEST_STR,178);
assert(0 == memcmp(mem1 + 4, mem2, 178));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,179);
assert(rc == EOK);
memcpy(mem2, TEST_STR,179);
assert(0 == memcmp(mem1 + 4, mem2, 179));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,180);
assert(rc == EOK);
memcpy(mem2, TEST_STR,180);
assert(0 == memcmp(mem1 + 4, mem2, 180));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,181);
assert(rc == EOK);
memcpy(mem2, TEST_STR,181);
assert(0 == memcmp(mem1 + 4, mem2, 181));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,182);
assert(rc == EOK);
memcpy(mem2, TEST_STR,182);
assert(0 == memcmp(mem1 + 4, mem2, 182));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,183);
assert(rc == EOK);
memcpy(mem2, TEST_STR,183);
assert(0 == memcmp(mem1 + 4, mem2, 183));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,184);
assert(rc == EOK);
memcpy(mem2, TEST_STR,184);
assert(0 == memcmp(mem1 + 4, mem2, 184));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,185);
assert(rc == EOK);
memcpy(mem2, TEST_STR,185);
assert(0 == memcmp(mem1 + 4, mem2, 185));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,186);
assert(rc == EOK);
memcpy(mem2, TEST_STR,186);
assert(0 == memcmp(mem1 + 4, mem2, 186));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,187);
assert(rc == EOK);
memcpy(mem2, TEST_STR,187);
assert(0 == memcmp(mem1 + 4, mem2, 187));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,188);
assert(rc == EOK);
memcpy(mem2, TEST_STR,188);
assert(0 == memcmp(mem1 + 4, mem2, 188));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,189);
assert(rc == EOK);
memcpy(mem2, TEST_STR,189);
assert(0 == memcmp(mem1 + 4, mem2, 189));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,190);
assert(rc == EOK);
memcpy(mem2, TEST_STR,190);
assert(0 == memcmp(mem1 + 4, mem2, 190));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,191);
assert(rc == EOK);
memcpy(mem2, TEST_STR,191);
assert(0 == memcmp(mem1 + 4, mem2, 191));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,192);
assert(rc == EOK);
memcpy(mem2, TEST_STR,192);
assert(0 == memcmp(mem1 + 4, mem2, 192));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,193);
assert(rc == EOK);
memcpy(mem2, TEST_STR,193);
assert(0 == memcmp(mem1 + 4, mem2, 193));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,194);
assert(rc == EOK);
memcpy(mem2, TEST_STR,194);
assert(0 == memcmp(mem1 + 4, mem2, 194));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,195);
assert(rc == EOK);
memcpy(mem2, TEST_STR,195);
assert(0 == memcmp(mem1 + 4, mem2, 195));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,196);
assert(rc == EOK);
memcpy(mem2, TEST_STR,196);
assert(0 == memcmp(mem1 + 4, mem2, 196));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,197);
assert(rc == EOK);
memcpy(mem2, TEST_STR,197);
assert(0 == memcmp(mem1 + 4, mem2, 197));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,198);
assert(rc == EOK);
memcpy(mem2, TEST_STR,198);
assert(0 == memcmp(mem1 + 4, mem2, 198));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,199);
assert(rc == EOK);
memcpy(mem2, TEST_STR,199);
assert(0 == memcmp(mem1 + 4, mem2, 199));
memset_s(mem1, MEM_LEN, 0, 210);

rc = memcpy_s(mem1 + 4, MEM_LEN, TEST_STR,200);
assert(rc == EOK);
memcpy(mem2, TEST_STR,200);
assert(0 == memcmp(mem1 + 4, mem2, 200));
memset_s(mem1, MEM_LEN, 0, 210);


}


void TestMemcpy_s()
{
	static UINT8T  mem1[MEM_LEN + 2];
	static UINT8T  mem2[MEM_LEN + 2];


	errno_t rc;
	uint32_t i;
	size_t len;


	rc = memcpy_s(mem1, 0, NULL, MEM_LEN);
	assert(rc == ERANGE);

	/*--------------------------------------------------*/

	rc = memcpy_s(NULL, MEM_LEN, mem2, MEM_LEN);
	assert(rc == EINVAL);

	/*--------------------------------------------------*/

	rc = memcpy_s(mem1, 0, mem2, MEM_LEN);
	assert(rc == ERANGE);

	/*--------------------------------------------------*/

	rc = memcpy_s(mem1, -2, mem2, MEM_LEN);
	assert(rc == ERANGE) ;

	/*--------------------------------------------------*/

	rc = memcpy_s(mem1, MEM_LEN, NULL, MEM_LEN);
	assert(rc == EINVAL_AND_RESET) ;

	for (i = 0; i < MEM_LEN; ++i){
		assert(mem1[i] == 0) ;
	}

	/*--------------------------------------------------*/

	rc = memcpy_s(mem1, 10, mem2, 0);
	assert(rc == EOK);

	for (i = 0; i < MEM_LEN; ++i){
		assert(mem1[i] == 0) ;
	}

	/*--------------------------------------------------*/

	rc = memcpy_s(mem1, MEM_LEN, mem2, INT_MAX + 1);
	assert(rc == ERANGE_AND_RESET);

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+1; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = memcpy_s(mem1, len, mem2, len);
	assert(rc == EOK) ;

	for (i=0; i<len; i++) {
		assert(mem1[i] == mem2[i]);
	}

	assert(mem1[i] == 33) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+1; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = memcpy_s(mem1, len, mem2, (len+1) );
	assert(rc == ERANGE_AND_RESET);
	for (i=0; i<len; i++) {
		assert(mem1[i] == 0);
	}

	assert(mem1[i] == 33);


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+2; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN/2;
	rc = memcpy_s(mem1, len, mem2, MEM_LEN);
	assert(rc == ERANGE_AND_RESET) ;

	for (i=0; i<len; i++) {
		assert(mem1[i] == 0) ;
	}

	assert(mem1[len] == 33) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+2; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = memcpy_s(mem1, len, mem2, 0);
	assert(rc == EOK);
	/* verify mem1 was zeroed */
	/*   for (i=0; i<len; i++) {
	assert(mem1[i] == 0);
	}

	assert(mem1[len] == 33) ;
	*/
	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = memcpy_s(mem1, len, mem2, INT_MAX + 1);
	assert(rc == ERANGE_AND_RESET) ;
	{
		/* verify mem1 was zeroed */
		for (i=0; i<len; i++) {
			assert(mem1[i] == 0) ;
		}
	}

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }

	/* same ptr - no move */
	rc = memcpy_s(mem1, MEM_LEN, mem1, MEM_LEN);
	assert(rc == EOK) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 65; }

	/* overlap */
	len = 100;
	rc = memcpy_s(&mem1[0], len, &mem1[10], len);
	assert(rc == EOVERLAP_AND_RESET);
	for (i=0; i<10; i++) {
		assert(mem1[i] == 0) ;
	}


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 65; }

	/* overlap */
	len = 100;
	rc = memcpy_s(&mem1[10], len, &mem1[0], len);
	assert(rc == EOVERLAP_AND_RESET);
	for (i=10; i<len+10; i++) {
		assert(mem1[i] == 0) ;
	}


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 35; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 55; }

	len = 5;
	rc = memcpy_s(mem1, len, mem2, len);
	assert(rc == EOK) ;
	for (i=0; i<len; i++) {
		assert(mem1[i] == 55) ;
	}

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 35; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 55; }

	rc = memcpy_s(mem1, MEM_LEN, mem2, MEM_LEN/2);
	assert(rc == EOK);


	for (i=0; i<MEM_LEN/2; i++) {
		assert(mem1[i] == 55);
	}

	rc = memcpy_s(mem1, MEM_LEN, "abc",3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "abc", 3));
	memset_s(mem1, MEM_LEN, 0, 3);

	rc = memcpy_s(mem1, MEM_LEN, "abcd123",7);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "abcd123", 7));
	memset_s(mem1, MEM_LEN, 0, 7);

	rc = memcpy_s(mem1, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1, MEM_LEN, "123456",3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1, MEM_LEN, "123456",4);
	assert(rc == EOK);
	assert(0 == memcmp(mem1, "123456", 4));
	memset_s(mem1, MEM_LEN, 0, 25);

	//for offset 1
	rc = memcpy_s(mem1 +1, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +1, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +1, MEM_LEN, "123456",3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +1, MEM_LEN, "123456",4);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 4));
	memset_s(mem1, MEM_LEN, 0, 25);

	//for offset 2
		rc = memcpy_s(mem1 +2, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +2, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +2, MEM_LEN, "123456",3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +2, MEM_LEN, "123456",4);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 4));
	memset_s(mem1, MEM_LEN, 0, 25);

	//for offset 3
	rc = memcpy_s(mem1 +3, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +3, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +3, MEM_LEN, "123456",3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +3, MEM_LEN, "123456",4);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 4));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 +3, MEM_LEN, "123456",5);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 5));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456", 3);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456", 4);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 4));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456", 5);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 5));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "123456", 6);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 1, "123456", 6));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456",2);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 2, "123456", 2));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456", 3);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 2, "123456", 3));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456", 4);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 2, "123456", 4));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456", 5);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 2, "123456", 5));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "123456", 6);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 2, "123456", 6));
	memset_s(mem1, MEM_LEN, 0, 25);

rc = memcpy_s(mem1 + 3, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 3, MEM_LEN, "123456",2);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 2));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 3, MEM_LEN, "123456", 3);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 3, "123456", 3));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 3, MEM_LEN, "123456", 4);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 3, "123456", 4));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 3, MEM_LEN, "123456", 5);
	assert(rc == EOK);
		assert(0 == memcmp(mem1 + 3, "123456", 5));
memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 3, MEM_LEN, "123456", 6);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 3, "123456", 6));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 4, MEM_LEN, "123456",1);
	assert(rc == EOK);
	assert(0 == memcmp(mem1 + 4, "123456", 1));
	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1, MEM_LEN, "012345678901234567890123456789",25);
	assert(rc == EOK);

	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 1, MEM_LEN, "012345678901234567890123456789",25);
	assert(rc == EOK);

	memset_s(mem1, MEM_LEN, 0, 25);

	rc = memcpy_s(mem1 + 2, MEM_LEN, "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",88);
	assert(rc == EOK);

	memset_s(mem1, MEM_LEN, 0, 25);

	TestOffset1();
	TestOffset2();
	TestOffset3();
	TestOffset4();

}

void Test_wmemcpy_s()
{

	static wchar_t  mem1[MEM_LEN + 2];
	static wchar_t  mem2[MEM_LEN + 2];

	errno_t rc;
	uint32_t i;
	size_t len;

	/*--------------------------------------------------*/

	rc = wmemcpy_s(NULL, MEM_LEN, mem2, MEM_LEN);
	assert(rc == EINVAL);

	/*--------------------------------------------------*/

	rc = wmemcpy_s(mem1, 0, mem2, MEM_LEN);
	assert(rc == ERANGE);

	/*--------------------------------------------------*/

	rc = wmemcpy_s(mem1, -2, mem2, MEM_LEN);
	assert(rc == ERANGE) ;

	/*--------------------------------------------------*/

	rc = wmemcpy_s(mem1, MEM_LEN, NULL, MEM_LEN);
	assert(rc == EINVAL_AND_RESET) ;

	for (i = 0; i < MEM_LEN; ++i){
		assert(mem1[i] == 0) ;
	}

	/*--------------------------------------------------*/

	rc = wmemcpy_s(mem1, 10, mem2, 0);
	assert(rc == EOK);

	for (i = 0; i < MEM_LEN; ++i){
		assert(mem1[i] == 0) ;
	}

	/*--------------------------------------------------*/

	rc = wmemcpy_s(mem1, MEM_LEN, mem2, INT_MAX + 1);
	assert(rc == ERANGE_AND_RESET);

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+1; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = wmemcpy_s(mem1, len, mem2, len);
	assert(rc == EOK) ;

	for (i=0; i<len; i++) {
		assert(mem1[i] == mem2[i]);
	}

	assert(mem1[i] == 33) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+1; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = wmemcpy_s(mem1, len, mem2, (len+1) );
	assert(rc == ERANGE_AND_RESET);
	for (i=0; i<len; i++) {
		assert(mem1[i] == 0);
	}

	assert(mem1[i] == 33);


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+2; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN/2;
	rc = wmemcpy_s(mem1, len, mem2, MEM_LEN);
	assert(rc == ERANGE_AND_RESET) ;

	for (i=0; i<len; i++) {
		assert(mem1[i] == 0) ;
	}

	assert(mem1[len] == 33) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN+2; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = wmemcpy_s(mem1, len, mem2, 0);
	assert(rc == EOK);
	/* verify mem1 was zeroed */
	/*   for (i=0; i<len; i++) {
	assert(mem1[i] == 0);
	}

	assert(mem1[len] == 33) ;
	*/
	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 33; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 44; }

	len = MEM_LEN;
	rc = wmemcpy_s(mem1, len, mem2, INT_MAX + 1);
	assert(rc == ERANGE_AND_RESET) ;
	{

		/* verify mem1 was zeroed */
		for (i=0; i<len; i++) {
			assert(mem1[i] == 0) ;
		}


	}

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }

	/* same ptr - no move */
	rc = wmemcpy_s(mem1, MEM_LEN, mem1, MEM_LEN);
	assert(rc == EOK) ;

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 65; }

	/* overlap */
	len = 100;
	rc = wmemcpy_s(&mem1[0], len, &mem1[10], len);
	assert(rc == EOVERLAP_AND_RESET);
	for (i=0; i<10; i++) {
		assert(mem1[i] == 0) ;
	}


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 55; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 65; }

	/* overlap */
	len = 100;
	rc = wmemcpy_s(&mem1[10], len, &mem1[0], len);
	assert(rc == EOVERLAP_AND_RESET);
	for (i=10; i<len+10; i++) {
		assert(mem1[i] == 0) ;
	}


	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 35; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 55; }

	len = 5;
	rc = wmemcpy_s(mem1, len, mem2, len);
	assert(rc == EOK) ;
	for (i=0; i<len; i++) {
		assert(mem1[i] == 55) ;
	}

	/*--------------------------------------------------*/

	for (i=0; i<MEM_LEN; i++) { mem1[i] = 35; }
	for (i=0; i<MEM_LEN; i++) { mem2[i] = 55; }

	rc = wmemcpy_s(mem1, MEM_LEN, mem2, MEM_LEN/2);
	assert(rc == EOK);


	for (i=0; i<MEM_LEN/2; i++) {
		assert(mem1[i] == 55);
	}

}
