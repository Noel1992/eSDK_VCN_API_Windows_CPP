/********************************************************************
filename    : PacketMemoryPoolMgr.h
author      : c00206592
created     : 2012/11/17
description : �ڴ�����ɣ�ʹ����
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2012/11/17 ��ʼ�汾
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


//һ���Է��� 512*1.6K ��С���ڴ�ؿռ�
class PacketPool
{
public:
	PacketPool();

	~PacketPool();

	//��ʼ���ڴ��
	int Init();

	int Init(size_t count, size_t size);

	//�ͷ��ڴ������
	void FreeMemPool();

	//���տ����ڴ��
	int  RecMemBlock(void* pMenAddr);

	//��ȡ�����ڴ����׵�ַ
	char* GetAvailableMemBlock();

	//��ȡ�����ڴ��ĸ���
	int  GetUsedMemBlockNum() const;
protected:

	//�����ڴ���׵�ַ
	MEMORY_HEAD* m_phead;

	//�ڴ��
	char* m_pMemoryPool;

	int  m_iUsedMemBlockNum;

	unsigned long m_ulMinAddrNo;

	unsigned long m_ulMaxAddrNo;

	size_t   m_uSize;
	size_t   m_uCount;

};

#endif//__PACKET_MEN_POOL_MGR_H__

