#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef WIN32
#include "vos.h"
#else
#include "vos.h"
#endif
#include "eSDK_Securec.h"

VOID *VOS_malloc( ULONG size )
{
	CHAR *buf = VOS_NULL ;
	if (0 == size)
	{
		return buf;
	}

    buf = ( CHAR *)malloc( size );
    if( NULL == buf )
    {
        return buf ;
    }
    eSDK_MEMSET( buf , 0x00 , size );

    return buf ;
}

VOID VOS_free( VOID *buff )
{
    free( buff );
    buff = VOS_NULL ;

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

