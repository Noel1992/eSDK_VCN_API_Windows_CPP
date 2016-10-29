/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: WSEC_Array.h
* Decription:
  对外接口
*********************************************************************************/
#ifndef WIRELESS_SEC_ARRAY_D1ASA42_DCFCVB4E74C27
#define WIRELESS_SEC_ARRAY_D1ASA42_DCFCVB4E74C27

#include "wsec_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WSEC_ARR_GROW_BY (16) /* 当内存用完后, 重新分配内存时的增幅(元素个数) */

typedef WSEC_VOID* WSEC_ADDR;
typedef WSEC_VOID* WSEC_ARRAY;
typedef WSEC_VOID (* WSEC_FP_RemoveElement)(WSEC_VOID *pElement); /* pElement为存储在待删除元素上的值 */
typedef WSEC_INT32 (* WSEC_FP_Compare)(const WSEC_VOID *pA, const WSEC_VOID *pB);

/* 数组初始化: 指定初始数组长度, 数组增长幅度 */
WSEC_ARRAY WSEC_ARR_Initialize(WSEC_INT32 nElementNum, WSEC_INT32 nGrowNum, WSEC_FP_Compare pfCmpElement, WSEC_FP_RemoveElement pfRmvElement);
WSEC_ARRAY WSEC_ARR_Finalize(WSEC_ARRAY arr); /* 数组终结 */

WSEC_VOID* WSEC_ARR_GetAt(const WSEC_ARRAY arr, WSEC_INT32 Index); /* 获取指定位置上的数据 */
WSEC_INT32 WSEC_ARR_Add(WSEC_ARRAY arr, const WSEC_VOID* pElement); /* 新增元素 */
WSEC_INT32 WSEC_ARR_AddOrderly(WSEC_ARRAY arr, const WSEC_VOID* pElement); /* 按序新增元素 */
WSEC_BOOL WSEC_ARR_InsertAt(WSEC_ARRAY arr, WSEC_INT32 Index, const WSEC_VOID* pElement); /* 向指定位置上插入元素 */
WSEC_VOID WSEC_ARR_RemoveAt(WSEC_ARRAY arr, WSEC_INT32 Index); /* 删除指定位置上的元素 */
WSEC_VOID WSEC_ARR_RemoveAll(WSEC_ARRAY arr); /* 数组清空 */

WSEC_INT32 WSEC_ARR_GetCount(const WSEC_ARRAY arr); /* 获取数组中已占用元素个数 */
WSEC_BOOL WSEC_ARR_IsEmpty(const WSEC_ARRAY arr); /* 判断数组是否为空 */

WSEC_VOID  WSEC_ARR_QuickSort(WSEC_ARRAY arr);
WSEC_VOID* WSEC_ARR_BinarySearch(const WSEC_ARRAY arr, const WSEC_VOID* pvKey);
WSEC_INT32 WSEC_ARR_BinarySearchAt(const WSEC_ARRAY arr, const WSEC_VOID* pvKey);

WSEC_VOID WSEC_ARR_StdRemoveElement(WSEC_VOID *pElement);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif/* WIRELESS_SEC_ARRAY_D1ASA42_DCFCVB4E74C27 */
