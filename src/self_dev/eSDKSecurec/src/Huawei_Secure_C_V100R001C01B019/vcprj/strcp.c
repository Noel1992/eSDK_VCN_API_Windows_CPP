
#include "securec.h"
#include "securecutil.h"
#include <string.h>
#include "../test/unittest.h"

errno_t strcpy_s_s(char *strDest, size_t numberOfElements, const char *strSource)
{
    char* p = strDest;
    size_t maxSize = numberOfElements;
    const char* overlapGuard = NULL;
    size_t sLen = 0;

    if (numberOfElements == 0 || numberOfElements > SECUREC_STRING_MAX_LEN)
    {
        SECUREC_ERROR_INVALID_RANGE("strcpy_s");
        return ERANGE;
    }
    if (strDest == NULL || strSource == NULL)
    {
        if (strDest != NULL)
        {
            strDest[0] = '\0';
        }

        SECUREC_ERROR_INVALID_PARAMTER("strcpy_s");
        return EINVAL;
    }
    if (strDest == strSource)
    {
        return EOK;
    }

    sLen = strlen(strSource);

    if(sLen >= maxSize) {
        strDest[0] = '\0';
        SECUREC_ERROR_INVALID_RANGE("strcpy_s");
        return ERANGE;
    }
    if ( (strDest < strSource && strDest + sLen >= strSource) || 
        ( strSource < strDest && strSource + sLen >= strDest)) {
            strDest[0] = '\0';
            return EOVERLAP_AND_RESET;
    }
    /**/	
    strcpy(strDest, strSource);


    return EOK;
}
