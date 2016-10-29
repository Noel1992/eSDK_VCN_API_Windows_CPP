// BufferList2.cpp: implementation of the BufferList2 class.
//
//////////////////////////////////////////////////////////////////////

#include "BufferList2.h"
#include "malloc.h"
#ifdef WIN32
#include <WTypes.h>
#endif
#include "ivs_log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////


BufferList2::BufferList2()
{ 
    this->m_uSize = IVS_BUFFER_INIT_VALUE;
    this->m_pHeader = NULL;
    this->m_pTail = NULL;

}

BufferList2::~BufferList2()
{
    try
    {
        release();
    }
    catch(...)
    {
        IVS_LOG(IVS_LOG_ERR, "BufferList2::~BufferList2()",
            "BufferList2::~BufferList2() exception!\n"); //lint !e1551
        //exit(1);
    }
    m_pHeader = NULL;
    m_pTail = NULL;
    
}

#if 0
bool BufferList2::empty() const
{
    return (m_uSize == 0);
}

unsigned int BufferList2::size() const
{
    return m_uSize;
}
#endif 



unsigned char* BufferList2::back() const
{
    if (m_pTail)
    {
        return m_pTail->pData;
    }
    else
    {
        return NULL;
    }
}

unsigned char* BufferList2::front() const
{
    if (m_pHeader)
    {
        return m_pHeader->pData;
    }
    else
    {
        return NULL;
    }
}

unsigned char* BufferList2::pop_back()
{
    node* pCur = NULL;
    unsigned char* pdata = NULL;

    if ( !empty() )
    {
        // pop the header
        pCur = m_pTail;
        m_pTail = pCur->pPre;   //lint !e613 pCur不可能为空

        pdata = pCur->pData;    //lint !e613
        pCur->pData = NULL;     //lint !e613

        m_uSize--;
    
        return pdata;

    }
    else
    {
        return NULL;
    }
}

unsigned char* BufferList2::pop_front()
{
    node* pCur = NULL;
    unsigned char* pdata = NULL;

    if (!this->empty() )
    {
        // pop the header
        pCur = m_pHeader;
        m_pHeader = pCur->pNext;   //lint !e613 pCur不可能为空

        pdata = pCur->pData;       //lint !e613 pCur不可能为空
        pCur->pData = NULL;        //lint !e613 pCur不可能为空

        m_uSize--;
    
        return pdata;
    }
    else
    {
        return NULL;
    }
}

unsigned int BufferList2::push_front(unsigned char* pdata)
{
    node* pCur = NULL;

    //代码检视--指针判空 [4/17/2010 zhangyx]
    if ( NULL != m_pHeader )
    {
        //new node
        pCur = m_pHeader->pPre ;
    }


    if(!pCur || pCur->pData != NULL)
    {
        IVS_LOG(IVS_LOG_ERR, "BufferList2::push_front()",
            "pCur == NULL");
        return (unsigned int)(IVS_BUFFER_FAIL);
    }

    pCur->pData = pdata;
     
    if (this->empty())
    {

        m_pHeader = pCur;
        m_pTail = pCur;

    }
    else
    {
        m_pHeader = pCur;
    }

    //代码检视，应为加一后返回 [2010/4/17 zhangyx]
    m_uSize++;
    return m_uSize;
    
}

unsigned int BufferList2::push_back(unsigned char* pdata)
{
    node* pCur = NULL;

    if ( NULL != m_pTail)
    {
        //new node
        pCur = m_pTail->pNext;
    }

    if(!pCur || pCur->pData != NULL)
    {
        IVS_LOG(IVS_LOG_ERR, "BufferList2::push_back()",
                "pCur == NULL");
        return (unsigned int)(IVS_BUFFER_FAIL);
    }

    pCur->pData = pdata;
     
    //list is empty
    if (this->empty())
    {
    
        m_pHeader = pCur;
        m_pTail = pCur;

    }
    else
    {
        m_pTail = pCur;
    }

    m_uSize++;
    return m_uSize;
    
}

void BufferList2::clear()
{
    release();
    m_pTail = m_pHeader = NULL;
}

int BufferList2::alloc(unsigned int nSize)
{
    //create a loop list
    node* pcur = NULL;
    node* ph = NULL;
    node* pre = NULL;

    try
    {
        if (nSize > 0)
        {
            pcur = (node *)malloc(sizeof(node));
            if (!pcur)
            {
                return IVS_BUFFER_FAIL;
            }
            pre = ph = pcur;
            pcur->pData = NULL;
        }
        else
        {
            return IVS_BUFFER_FAIL;
        }
    }
    catch(...)
    {
//         ::AfxMessageBox("内存出现异常！请退出程序重新启动！");
        return IVS_BUFFER_FAIL;
    }
    
    try
    {
        for(unsigned int i =1; i<nSize; i++)
        {
            pcur = (node *)malloc(sizeof(node));
            if (!pcur) 
            {
                return IVS_BUFFER_FAIL;
            }
            pcur->pData = NULL;
            pcur->pPre = pre;
            pre->pNext = pcur;
            
            pre = pcur;
        }
    }
    catch(...)
    {
//         ::AfxMessageBox("内存出现异常！请退出程序重新启动！");
        return IVS_BUFFER_FAIL;
    }
    
    pcur->pNext = ph;
    m_pHeader = m_pTail = ph->pPre = pcur;
    
    return IVS_BUFFER_SUCC;
}

void BufferList2::release()
{
    node* ph = NULL;
    node* pcur = NULL;

    //get loop list header
    ph = m_pHeader;

    // add for error 613 [3/19/2009 z90003203]
    if (NULL == ph)
    {
        return;
    }
    // end add

    pcur = ph->pNext;
    //解决只有一个结点时，会出现多删一个结点的问题
    bool IsOnlyOne = ph->pNext == m_pHeader ? true:false;

    do{
        // go next and release the node
        m_pHeader = pcur->pNext;
        //代码检视 -- pData已经有外部释放 [2010-4-17 zhangyx]
        
        free( pcur );
        pcur = NULL;
        pcur = m_pHeader;

    }while( m_pHeader && (m_pHeader != ph) );
    
    //free the header
    if (!IsOnlyOne 
        && m_pHeader != NULL
        )
    {
        free(ph);
    }

    m_pTail = m_pHeader = NULL;
    m_uSize = IVS_BUFFER_INIT_VALUE;

}
