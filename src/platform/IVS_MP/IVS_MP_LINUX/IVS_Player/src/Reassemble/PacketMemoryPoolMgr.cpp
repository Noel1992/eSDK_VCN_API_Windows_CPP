/*******************************************************************************
//  版权所有    华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  PacketMemoryPoolMgr.cpp
//  文件说明:
//  版    本:   IVS V100R001C02
//  作    者:   chenxianxiao/00206592
//  创建日期:   2012/11/01
//  修改标识：
//  修改说明：
*******************************************************************************/

#include "PacketMemoryPoolMgr.h"
#include "ivs_error.h"
#include "IVSCommon.h"
#ifdef WIN32
#include "..\..\inc\common\log\ivs_log.h"
#else
#include "ivs_log.h"
#endif 


PacketPool::PacketPool()
{//lint !e1927
    m_phead = NULL;

    m_pMemoryPool = NULL;

    m_iUsedMemBlockNum = 0;

	m_ulMaxAddrNo = 0;

	m_ulMinAddrNo = 0;

	m_uSize  = 0;
	m_uCount = 0;

}

PacketPool::~PacketPool()
{
    m_iUsedMemBlockNum = 0;
    try
    {
        FreeMemPool();
        m_phead = NULL;
        m_pMemoryPool = NULL;
    }
    catch (...)
    {
        return;
    }
}

int PacketPool::Init()
{
    //一次性分配G_POOL_MAX_SIZE个1.6K大小的内存空间
    try
    {
        m_pMemoryPool = IVS_NEW(m_pMemoryPool, POOL_MAX_SIZE * BLOCK_SIZE);

        if (NULL != m_pMemoryPool)
        {
			int tmp_size = POOL_MAX_SIZE - 1;

            m_phead = (MEMORY_HEAD*)m_pMemoryPool; //lint !e826

			MEMORY_HEAD* pNode = m_phead;

			char* poolTmp = m_pMemoryPool;

            for (int i = 0; i < tmp_size; i++)
            {
				
				pNode->pNext = (MEMORY_HEAD*)(poolTmp + BLOCK_SIZE);//lint !e826

				poolTmp += BLOCK_SIZE;

				pNode = (MEMORY_HEAD*)poolTmp;//lint !e826
                
            }

            //最后一块内存没有指向下一块的地址
            pNode->pNext = NULL; //lint !e826

            m_iUsedMemBlockNum = 0;
			//设置最大最小地址位置值
			m_ulMinAddrNo = (unsigned long)m_pMemoryPool;

			m_ulMaxAddrNo = (unsigned long)pNode;

			m_uSize = BLOCK_SIZE;
			m_uCount = POOL_MAX_SIZE;

            return IVS_SUCCEED;
        }

        return IVS_FAIL;
    }
    catch (...)
    {
        if (NULL != m_pMemoryPool)
        {
            IVS_DELETE(m_pMemoryPool, MUILI);
        }

        return IVS_FAIL;
    }
}

/*
 * DTS2013122700356 
 * 增加设置动态大小的接口
 */
int PacketPool::Init(size_t count, size_t size)
{
	//一次性分配G_POOL_MAX_SIZE个1.6K大小的内存空间
	try
	{
		m_pMemoryPool = IVS_NEW(m_pMemoryPool, count * size);

		if (NULL != m_pMemoryPool)
		{
			int tmp_size = (int)count - 1;

			m_phead = (MEMORY_HEAD*)m_pMemoryPool; //lint !e826

			MEMORY_HEAD* pNode = m_phead;

			char* poolTmp = m_pMemoryPool;

			for (int i = 0; i < tmp_size; i++)
			{

				pNode->pNext = (MEMORY_HEAD*)(poolTmp + size);//lint !e826

				poolTmp += size;

				pNode = (MEMORY_HEAD*)poolTmp;//lint !e826

			}

			//最后一块内存没有指向下一块的地址
			pNode->pNext = NULL; //lint !e826

			m_iUsedMemBlockNum = 0;
			//设置最大最小地址位置值
			m_ulMinAddrNo = (unsigned long)m_pMemoryPool;

			m_ulMaxAddrNo = (unsigned long)pNode;

			m_uSize  = size;
			m_uCount = count;

			return IVS_SUCCEED;
		}

		return IVS_FAIL;
	}
	catch (...)
	{
		if (NULL != m_pMemoryPool)
		{
			IVS_DELETE(m_pMemoryPool, MUILI);
		}
		return IVS_FAIL;
	}
}


void PacketPool::FreeMemPool()
{
    m_phead = NULL;

    m_iUsedMemBlockNum = 0;
    try
    {
        if (m_pMemoryPool != NULL)
        {
            IVS_DELETE(m_pMemoryPool, MUILI);
        }

        m_pMemoryPool = NULL;
    }
    catch (...)
    {
        return;
    }
}

int PacketPool::RecMemBlock(void* pMenAddr)
{
    if (NULL == pMenAddr)
    {
        return IVS_FAIL;
    }
	unsigned long addrNo = (unsigned long) pMenAddr;

	if (addrNo < m_ulMinAddrNo || addrNo > m_ulMaxAddrNo)
	{
		 IVS_LOG(IVS_LOG_ERR, "RecMemBlock", "Addr Value Error");
		 return IVS_FAIL;
	}
	 //完成空闲内存的回收
	((MEMORY_HEAD*)pMenAddr)->pNext = m_phead; //lint !e613

	 m_phead = ((MEMORY_HEAD*) pMenAddr);

    m_iUsedMemBlockNum--;
    return IVS_SUCCEED;
}


char* PacketPool::GetAvailableMemBlock()
{
    if ((m_iUsedMemBlockNum >= 0) && (m_iUsedMemBlockNum < POOL_MAX_SIZE) && (NULL != m_phead))
    {
        MEMORY_HEAD* p = m_phead;

        m_phead = p->pNext; //lint !e826

        m_iUsedMemBlockNum++;

        return (char* )p;
    }

    IVS_LOG(IVS_LOG_ERR, "GetAvailableMemBlock", "No available memory");
    return NULL;
}

int PacketPool::GetUsedMemBlockNum() const
{
    return m_iUsedMemBlockNum;
}
