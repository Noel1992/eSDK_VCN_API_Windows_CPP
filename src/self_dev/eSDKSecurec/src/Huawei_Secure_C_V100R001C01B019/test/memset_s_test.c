#include "securec.h"
#include "unittest.h"
#include <assert.h>
#include <string.h>

#define LEN   ( 256 )


typedef unsigned int uint32_t;
typedef int int32_t;



void test_memset_s(void)
{
	errno_t rc;
    uint32_t len;
    uint32_t i;

    UINT8T value;
	UINT8T mem1[LEN];
		char hugeBuf[2072];

	rc = memset_s(mem1, LEN, 2, 3);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1, LEN, 0, 3);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1, LEN, 3, 7);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1, LEN, 0, 7);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1, LEN, 5, 24);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1, LEN, 0, 24);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1+ 1, LEN, 5, 5);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1 +1, LEN, 0, 5);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1+ 3, LEN, 0x23, 5);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1 +3, LEN, 0, 5);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1+ 1, LEN, 0x45, 73);
	assert((rc & 0x7F) == EOK);

	rc = memset_s(mem1 +1, LEN,  0, 73);
	assert((rc & 0x7F) == EOK);

/*--------------------------------------------------*/

    value = 34;

	rc = memset_s(NULL, LEN, value, LEN);

	assert((rc & 0x7F) == EINVAL);

/*--------------------------------------------------*/

    value = 34;

	rc = memset_s(mem1, 0, value, LEN);

	assert((rc & 0x7F) == ERANGE);


/*--------------------------------------------------*/

	value = 34;

	rc = memset_s(hugeBuf, SECUREC_MEM_MAX_LEN , 0xFF, 2072);

	assert((rc & 0x7F) == EOK);

/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 99; }

    len = 1;
    value = 34;

    rc = memset_s(mem1, len, value, len);

	assert((rc & 0x7F) == EOK);

/*--------------------------------------------------*/

    for (i=0; i<LEN; i++) { mem1[i] = 99; }

    len = 12;
    value = 34;

    rc = memset_s(mem1, len, value, 99 );

	assert((rc & 0x7F) == ERANGE);


}
