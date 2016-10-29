/*******************************************************************************
//  ��Ȩ����    ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  PacketMemoryPoolMgr.cpp
//  �ļ�˵��:
//  ��    ��:   IVS V100R001C02
//  ��    ��:   chenxianxiao/00206592
//  ��������:   2012/11/01
//  �޸ı�ʶ��
//  �޸�˵����
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
    //һ���Է���G_POOL_MAX_SIZE��1.6K��С���ڴ�ռ�
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

            //���һ���ڴ�û��ָ����һ��ĵ�ַ
            pNode->pNext = NULL; //lint !e826

            m_iUsedMemBlockNum = 0;
			//���������С��ַλ��ֵ
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
 * �������ö�̬��С�Ľӿ�
 */
int PacketPool::Init(size_t count, size_t size)
{
	//һ���Է���G_POOL_MAX_SIZE��1.6K��С���ڴ�ռ�
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

			//���һ���ڴ�û��ָ����һ��ĵ�ַ
			pNode->pNext = NULL; //lint !e826

			m_iUsedMemBlockNum = 0;
			//���������С��ַλ��ֵ
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
	 //��ɿ����ڴ�Ļ���
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
