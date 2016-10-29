/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos.h"
#include "eSDK_Securec.h"

VOID *VOS_malloc( ULONG size )
{
	CHAR *buf = VOS_NULL ;
	if (0 == size)
	{
		return buf;
	}

    buf = ( CHAR *)malloc( size );//lint !e838
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

