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
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  Timer.h
//  �ļ�˵��:   
//  ��    ��:   IVS V100R002C01
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/11/14
//  �޸ı�ʶ��
//  �޸�˵����
*******************************************************************************/

#ifndef __IVS_TIMER_H__
#define __IVS_TIMER_H__

//time_a after time_b
#define TIME_AFTER(time_a, time_b) ((long)(time_b) - (long)(time_a) < 0)

//time_a before time_b
#define TIME_BEFORE(time_a, time_b) TIME_AFTER(time_b, time_a)

int  IVS_StarTimer();
void IVS_StopTimer();

unsigned long  IVS_GetTimeTick();

extern unsigned long timeTick;

unsigned long long IVS_GetSysTickUs(void);

#endif
