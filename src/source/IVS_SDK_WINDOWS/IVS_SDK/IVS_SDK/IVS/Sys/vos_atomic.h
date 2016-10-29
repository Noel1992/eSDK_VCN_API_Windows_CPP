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

/*******************************************************************************
//  ��Ȩ����    2012 ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  vos_atomic.h
//  �ļ�˵��:   ԭ�ӱ�������
//  ��    ��:   IVS V100R002C01
//  ��    ��:   w00210470
//  ��������:   2013/10/10
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#ifndef __VOS_ATOMIC_H__
#define __VOS_ATOMIC_H__

#ifdef WIN32
#include <windows.h>
#else
#include <asm/atomic.h>
#endif 

typedef struct {
	volatile long counter;
} vos_atomic_t;

#if defined WIN32 || defined _MSC_VER

static inline long atomic_inc(vos_atomic_t *v)
{
	return InterlockedIncrement(&v->counter);
}

static inline long atomic_dec(vos_atomic_t *v)
{
	return InterlockedDecrement(&v->counter);
}

static inline bool atomic_dec_and_test(vos_atomic_t *v)
{
	return (0 == InterlockedDecrement(&v->counter));
}

static inline bool atomic_inc_and_test(vos_atomic_t *v)
{
	return  (0 == InterlockedIncrement(&v->counter));
}

#else  // linux





#endif //  defined WIN32 || defined _MSC_VER






#endif 



