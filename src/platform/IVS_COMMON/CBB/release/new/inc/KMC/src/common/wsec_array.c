/* 如下pc lint告警可忽略 */
/*lint -e506 -e522 -e533 -e534 -e632 -e633 -e634 -e636 -e638 -e639 -e665 -e830 -e960 */

#include "wsec_pri.h"
#include "wsec_array.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct tagWSEC_ARRAY_DATA
{
    WSEC_ADDR* pItemAddr;  /* 指向各元素指针的地址(注意, 元素数据存储空间由APP管理) */
    WSEC_INT32   nCountUsed; /* 实际已使用元素个数 */
    WSEC_INT32   nCountMax;  /* 数组最大长度, 如果元素个数超nCountUse，则按nGrowNum追加内存分配 */
    WSEC_INT32   nGrowNum;   /* 内存增长幅度(过大则浪费, 过小容易出现碎片) */
    WSEC_FP_Compare       pfCompare;
    WSEC_FP_RemoveElement pfRemoveElement;
} WSEC_ARRAY_DATA_STRU;

#define WSEC_ARR_MEM_SIZE(Num) ((Num) * sizeof(WSEC_VOID*))
#define WSEC_ARR_ITEM_ADDR(pArr, Index) (pArr->pItemAddr + (Index))

WSEC_BOOL WSEC_ARR_CfmSpaceUseful(WSEC_ARRAY_DATA_STRU* pArr);

/*****************************************************************************
 函 数 名  : WSEC_ARR_CfmSpaceUseful
 功能描述  : 确保数组的空间可用
             当需要向数组新增元素时, 检查当前元素个数(nCountUsed)和分配
             长度(nCountMax)之间的关系，若空间不够则以nGrowNum为增幅扩充
             内存.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : pArr: 被管理数组
 返 回 值  : WSEC_TRUE=成功, WSEC_FALSE=失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_ARR_CfmSpaceUseful(WSEC_ARRAY_DATA_STRU* pArr)
{
    WSEC_ADDR* pNew = (WSEC_ADDR*)WSEC_NULL_PTR;
    WSEC_VOID* ptr;
    WSEC_SIZE_T nSizeOld, nSizeNew;

    WSEC_ASSERT(pArr);
    if (pArr->nCountMax > pArr->nCountUsed) {return WSEC_TRUE;} /* 空间足够 */

    if_oper(pArr->nGrowNum < 1, pArr->nGrowNum = WSEC_ARR_GROW_BY);

    nSizeOld = WSEC_ARR_MEM_SIZE(pArr->nCountMax);
    nSizeNew = WSEC_ARR_MEM_SIZE(pArr->nCountMax + pArr->nGrowNum);

    pNew = (WSEC_ADDR*)WSEC_MALLOC(nSizeNew);

    if (!pNew)
    {
        return WSEC_FALSE;
    }

    /* 将旧内存搬迁到新内存 */
    if (pArr->pItemAddr)
    {
        if (WSEC_MEMCPY(pNew, nSizeNew, pArr->pItemAddr, nSizeOld) != EOK) /* 搬迁失败, 则释放新的, 保留旧的 */
        {
            ptr = (WSEC_VOID*)pNew;
            WSEC_FREE(ptr);
            return WSEC_FALSE;
        }

        /* 搬迁成功, 释放旧的 */
        ptr = (WSEC_VOID*)(pArr->pItemAddr);
        WSEC_FREE(ptr);
    }

    pArr->pItemAddr = pNew;
    pArr->nCountMax += pArr->nGrowNum;

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_Initialize
 功能描述  : 数组初始化.
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : nElementNum: 初始分配元素个数
             nGrowNum: 当需要扩展空间时, 以多大幅度扩充内存
             pfCmpElement: 比较两个元素大小的回调函数
             pfRmvElement: 删除元素前回调
 返 回 值  : 指向数组管理的数据结构
 特别注意  : 本函数申请内存用于对数组进行管理, APP必须调用WSEC_ARR_Finalize
             释放内存.

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ARRAY WSEC_ARR_Initialize(WSEC_INT32 nElementNum, WSEC_INT32 nGrowNum, WSEC_FP_Compare pfCmpElement, WSEC_FP_RemoveElement pfRmvElement)
{
    WSEC_ARRAY_DATA_STRU* pData = WSEC_NULL_PTR;

    pData = (WSEC_ARRAY_DATA_STRU*)WSEC_MALLOC(sizeof(WSEC_ARRAY_DATA_STRU));
    if (!pData) {return (WSEC_ARRAY)pData;}

    if (nElementNum < 0) {nElementNum = 0;}
    pData->nGrowNum        = nGrowNum; /* 这里不用检查, 扩展内存时再检查 */
    pData->pfCompare       = pfCmpElement;
    pData->pfRemoveElement = pfRmvElement;

    if (0 == nElementNum) {return (WSEC_ARRAY)pData;}

    /* Misinformation: FORTIFY.Dead_Code */
    pData->pItemAddr = (WSEC_ADDR*)WSEC_MALLOC(WSEC_ARR_MEM_SIZE(nElementNum));
    if (pData->pItemAddr)
    {
        pData->nCountMax = nElementNum;
    }
    else
    {
        WSEC_FREE(pData);
    }

    return (WSEC_ARRAY)pData;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_Finalize
 功能描述  : 数组销毁.
 纯 入 参  : arr: 被销毁的数组
 纯 出 参  : 无
 入参出参  : pArr: 被管理数组
 返 回 值  : NULL指针(方便调用者置数组无效)
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_ARRAY WSEC_ARR_Finalize(WSEC_ARRAY arr)
{
    if (arr)
    {
        WSEC_ARR_RemoveAll(arr);
        WSEC_FREE(arr);
    }
    
    return WSEC_NULL_PTR;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_GetAt
 功能描述  : 获取指定位置上的数据
 纯 入 参  : arr: 数组
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 存放在该位置指向APP管理数据的指针.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID* WSEC_ARR_GetAt(const WSEC_ARRAY arr, WSEC_INT32 Index)
{
    const WSEC_ARRAY_DATA_STRU* pArr = (const WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_VOID* pVal = WSEC_NULL_PTR;

    WSEC_ASSERT(arr);
    if (Index < pArr->nCountUsed)
    {
        pVal = *(pArr->pItemAddr + Index);
    }

    return pVal;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_Add
 功能描述  : 新增元素
 纯 入 参  : pElement: APP管理的数据指针.
 纯 出 参  : 无
 入参出参  : arr: 数组
 返 回 值  : 若新增成功, 则返回在存放到数组的下标.
             -1: 失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 WSEC_ARR_Add(WSEC_ARRAY arr, const WSEC_VOID* pElement)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_INT32 nAt = -1;

    WSEC_ASSERT(arr && pElement);
    if (WSEC_ARR_CfmSpaceUseful(pArr))
    {
        nAt = pArr->nCountUsed;
        *(pArr->pItemAddr + nAt) = (WSEC_VOID*)pElement;
        pArr->nCountUsed++;
    }

    return nAt;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_AddOrderly
 功能描述  : 按序新增元素
 纯 入 参  : pElement: APP管理的数据指针.
 纯 出 参  : 无
 入参出参  : arr: 数组
 返 回 值  : 若新增成功, 则返回在存放到数组的下标.
             -1: 失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 WSEC_ARR_AddOrderly(WSEC_ARRAY arr, const WSEC_VOID* pElement)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_INT32 i, nInsertAt = -1;

    WSEC_ASSERT(arr && pElement && pArr->pfCompare);
    if (!WSEC_ARR_CfmSpaceUseful(pArr)) {return -1;}

    for (i = 0; i < pArr->nCountUsed; i++)
    {
        if (pArr->pfCompare(&pElement, pArr->pItemAddr + i) < 0)
        {
            nInsertAt = i;
            break;
        }
    }

    if_oper(nInsertAt < 0, nInsertAt = pArr->nCountUsed);

    if (!WSEC_ARR_InsertAt(arr, nInsertAt, pElement)) {nInsertAt = -1;}

    return nInsertAt;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_InsertAt
 功能描述  : 向指定位置上插入元素
 纯 入 参  : Index: 指定位置(数组的下标)
             pElement: APP管理的数据指针.
 纯 出 参  : 无
 入参出参  : arr: 数组
 返 回 值  : WSEC_TRUE=成功, WSEC_FALSE=失败.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_ARR_InsertAt(WSEC_ARRAY arr, WSEC_INT32 Index, const WSEC_VOID* pElement)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_ADDR* pAddr = (WSEC_ADDR*)WSEC_NULL_PTR;
    WSEC_INT32 i;

    WSEC_ASSERT(pArr && pElement);
    if ((Index < 0) || (Index > pArr->nCountUsed)) {return WSEC_FALSE;}/* 最多只能紧邻尾部插入 */
    if (!WSEC_ARR_CfmSpaceUseful(pArr)) {return WSEC_FALSE;}

    pArr->nCountUsed++;

    /* [Index, nCountUsed-1]向右挪 */
    pAddr = WSEC_ARR_ITEM_ADDR(pArr, pArr->nCountUsed - 1);
    for (i = pArr->nCountUsed - 1; i >= Index; i--, pAddr--)
    {
        *pAddr = *(pAddr - 1);
    }

    /* 待插数据写入 */
    *(pArr->pItemAddr + Index) = (WSEC_VOID*)pElement;

    return WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_RemoveAt
 功能描述  : 删除指定位置上的元素
 纯 入 参  : Index: 指定位置(数组的下标)
             pElement: APP管理的数据指针.
 纯 出 参  : 无
 入参出参  : arr: 数组
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_ARR_RemoveAt(WSEC_ARRAY arr, WSEC_INT32 Index)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_INT32 i;

    WSEC_ASSERT(arr);
    if (Index >= pArr->nCountUsed) {return;}

    /* 删除该元素 */
    if (pArr->pfRemoveElement) {pArr->pfRemoveElement(*(pArr->pItemAddr + Index));}

    /* [Index+1, nCountUsed-1] 向左挪一个位置 */
    for (i = Index + 1; i <= pArr->nCountUsed - 1; i++)
    {
        *(pArr->pItemAddr + i - 1) = *(pArr->pItemAddr + i);
    }

    pArr->nCountUsed--;
    return;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_RemoveAll
 功能描述  : 数组清空
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : arr: 数组
 返 回 值  : 无
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_ARR_RemoveAll(WSEC_ARRAY arr)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_ADDR* p;
    WSEC_VOID* pAddr;
    WSEC_INT32 i;

    if (!pArr) {return;}
    if (!pArr->pItemAddr) {return;}
    
    if (pArr->pfRemoveElement)
    {
        p = pArr->pItemAddr;
        for (i = 0; i < pArr->nCountUsed; i++, p++)
        {
            pArr->pfRemoveElement(*p);
        }
    }else{;}

    pArr->nCountUsed = 0;
    pArr->nCountMax  = 0;

    pAddr = (WSEC_VOID*)pArr->pItemAddr;
    WSEC_FREE(pAddr);
    pArr->pItemAddr = (WSEC_ADDR*)WSEC_NULL_PTR;

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_GetCount
 功能描述  : 获取数组中已占用元素个数
 纯 入 参  : arr: 数组
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 数组中元素个数.
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 WSEC_ARR_GetCount(const WSEC_ARRAY arr)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;

    return pArr ? pArr->nCountUsed : 0;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_IsEmpty
 功能描述  : 判断数组是否为空
 纯 入 参  : arr: 数组
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 数组是否为空
 特别注意  : 无

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_BOOL WSEC_ARR_IsEmpty(const WSEC_ARRAY arr)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;

    return pArr ? (pArr->nCountUsed < 1) : WSEC_TRUE;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_QuickSort
 功能描述  : 对数组快速排序
 纯 入 参  : 无
 纯 出 参  : 无
 入参出参  : arr: 排序前后的数组
 返 回 值  : 数组中元素个数.
 特别注意  : APP必须在WSEC_ARR_Initialize()调用中提供比较两个元素大小的回调函数.
             该回调函数中的两个参数, 分别为参与比较的两元素地址, 而数组中的元素
             存储的是APP给的数据地址, 因此 *pA 才是指向APP数据的指针. 例如:
            WSEC_INT32 ComparePersonId(const WSEC_VOID* p1, const WSEC_VOID* p2)
            {
                const PERSON_STRU* pManA;
                const PERSON_STRU* pManB;
                WSEC_INT32 nCmp = 0;
            
                pManA = (PERSON_STRU*)(*(const WSEC_VOID**)p1);
                pManB = (PERSON_STRU*)(*(const WSEC_VOID**)p2);
            
                if (pManA->nId > pManB->nId)
                {
                    nCmp = 1;
                }
                else if (pManA->nId < pManB->nId)
                {
                    nCmp = -1;
                }
            
                return nCmp;
            }

 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_ARR_QuickSort(WSEC_ARRAY arr)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;

    if (pArr && pArr->pItemAddr && pArr->pfCompare)
    {
        qsort((WSEC_VOID*)pArr->pItemAddr, pArr->nCountUsed, sizeof(WSEC_VOID*), pArr->pfCompare);
    }

    return;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_BinarySearch
 功能描述  : 二分法查找数组
 纯 入 参  : arr:  数组
             pvKey: 指向关键字的指针(1级)
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 与关键字匹配的数据所在地址.
 特别注意  : (1) 确保数组arr为升序排列
             (2) 搜索过程需要调用比较两个元素大小的回调函数. 和排序的回调函数相同.
                 应在调用WSEC_ARR_Initialize()时提供.
             (3) 示例:
            ...
            PERSON_STRU* pMan;
            PERSON_STRU stKey = {0};

            stKey.nId = 99; // 查找ID=99的人
            pMan = (PERSON_STRU*)WSEC_ARR_BinarySearch(g_pArr, &stKey, ComparePersonId);
            ...
 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID* WSEC_ARR_BinarySearch(const WSEC_ARRAY arr, const WSEC_VOID* pvKey)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_ADDR* pFoundAddr;
    WSEC_VOID* pFoundData = WSEC_NULL_PTR;

    if (WSEC_ARR_GetCount(arr) > 0)
    {
        WSEC_ASSERT(pArr && pvKey && pArr->pfCompare);
        if (! pArr->pItemAddr) {return pFoundData;}

        /* Misinformation: FORTIFY.Out-of-Bounds_Read--Off-by-One */
        pFoundAddr = (WSEC_ADDR*)bsearch((const WSEC_VOID*)&pvKey, (WSEC_VOID*)pArr->pItemAddr, pArr->nCountUsed, sizeof(WSEC_VOID*), pArr->pfCompare);

        if (pFoundAddr) {pFoundData = *pFoundAddr;}
    }

    return pFoundData;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_BinarySearchAt
 功能描述  : 二分法查找数组
 纯 入 参  : arr:  数组
             pvKey: 指向关键字的指针(1级)
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 非负数: 与关键字匹配的数据在数组中的下标;
                 -1: 查找失败.
 特别注意  : (1) 确保数组pArr为升序排列
             (2) 搜索过程需要调用比较两个元素大小的回调函数. 和排序的回调函数相同.
                 应在调用WSEC_ARR_Initialize()时提供.
             (3) 示例:
            ...
            WSEC_INT32 nAt;
            PERSON_STRU* pMan;
            PERSON_STRU stKey = {0};

            stKey.nId = 99; // 查找ID=99的人
            nAt = WSEC_ARR_BinarySearchAt(g_pArr, &stKey, ComparePersonId);
            if (nAt < 0)
            {
                err_out("cannot find.");
            }
            else
            {
                pMan = (PERSON_STRU*)WSEC_ARR_GetAt(g_pArr, nAt);
            }
            ...
 修改历史
  1.日    期   : 2014年10月27日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_INT32 WSEC_ARR_BinarySearchAt(const WSEC_ARRAY arr, const WSEC_VOID* pvKey)
{
    WSEC_ARRAY_DATA_STRU* pArr = (WSEC_ARRAY_DATA_STRU*)arr;
    WSEC_ADDR* pFoundAddr;
    WSEC_INT32 nAt = -1;

    if (WSEC_ARR_GetCount(arr) > 0)
    {
        WSEC_ASSERT(pArr && pArr->pItemAddr && pvKey && pArr->pfCompare);
        pFoundAddr = (WSEC_ADDR*)bsearch((const WSEC_VOID*)&pvKey, (WSEC_VOID*)pArr->pItemAddr, pArr->nCountUsed, sizeof(WSEC_VOID*), pArr->pfCompare);

        if (pFoundAddr) {nAt = pFoundAddr - pArr->pItemAddr;}
    }

    return nAt;
}

/*****************************************************************************
 函 数 名  : WSEC_ARR_StdRemoveElement
 功能描述  : 标准的删除数组节点
             数组的节点存放的是用户的数据地址, 这个地址‘标准’用法是动态申请
             的内存, 因此删除该元素时应释放内存.
 纯 入 参  : pElement: 数组元素所在地址
 纯 出 参  : 无
 入参出参  : 无
 返 回 值  : 
 特别注意  : 

 修改历史
  1.日    期   : 2015年3月24日
    作    者   : z00118096
    修改内容   : 新生成函数
*****************************************************************************/
WSEC_VOID WSEC_ARR_StdRemoveElement(WSEC_VOID *pElement)
{
    WSEC_FREE(pElement);
    return;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
