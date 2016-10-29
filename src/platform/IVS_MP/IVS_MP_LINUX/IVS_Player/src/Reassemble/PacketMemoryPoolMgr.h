/********************************************************************
filename    : PacketMemoryPoolMgr.h
author      : c00206592
created     : 2012/11/17
description : 内存池生成，使用类
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 初始版本
*********************************************************************/

#ifndef __PACKET_MEN_POOL_MGR_H__
#define __PACKET_MEN_POOL_MGR_H__

#ifndef WIN32
#include <stdlib.h>
using namespace std;
#endif 

#define POOL_MAX_SIZE  2048

#define BLOCK_SIZE  1600

#define JUMBO_BLOCK_SIZE  (8 * 1024)
#define JUMBO_BLOCK_COUNT (512)


typedef struct _MEMORY_HEAD {

	struct _MEMORY_HEAD *pNext;

} MEMORY_HEAD;


//一次性分配 512*1.6K 大小的内存池空间
class PacketPool
{
public:
	PacketPool();

	~PacketPool();

	//初始化内存池
	int Init();

	int Init(size_t count, size_t size);

	//释放内存池数据
	void FreeMemPool();

	//回收可用内存块
	int  RecMemBlock(void* pMenAddr);

	//获取可用内存块的首地址
	char* GetAvailableMemBlock();

	//获取可用内存块的个数
	int  GetUsedMemBlockNum() const;
protected:

	//可用内存池首地址
	MEMORY_HEAD* m_phead;

	//内存池
	char* m_pMemoryPool;

	int  m_iUsedMemBlockNum;

	unsigned long m_ulMinAddrNo;

	unsigned long m_ulMaxAddrNo;

	size_t   m_uSize;
	size_t   m_uCount;

};

#endif//__PACKET_MEN_POOL_MGR_H__

