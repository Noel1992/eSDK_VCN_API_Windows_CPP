// BufferList2.h: interface for the BufferList2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFERLIST2_H__D86184E1_4578_4F18_8082_C18CC021CA07__INCLUDED_)
#define AFX_BUFFERLIST2_H__D86184E1_4578_4F18_8082_C18CC021CA07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ivs_log.h"

#define IVS_BUFFER_FAIL (-1)
#define IVS_BUFFER_SUCC (0)
#define IVS_BUFFER_INIT_VALUE (0)

class BufferList2  
{
public:
    void release();
    int alloc(unsigned int size);
    BufferList2();
    virtual ~BufferList2();

    void clear();
    unsigned char* back() const;
    unsigned char* pop_back();
    unsigned int push_back(unsigned char* pdata);
	unsigned int push_front(unsigned char* pdata);
    unsigned char* pop_front();
    unsigned char* front() const;
    inline unsigned int size() const  { return m_uSize; }
	inline bool empty() const { return (m_uSize == 0); }

    struct node{
        unsigned char* pData;
        node* pNext;
        node* pPre;
    };

    node* m_pHeader;
    node* m_pTail;

private:
    unsigned int m_uSize;


};

#endif // !defined(AFX_BUFFERLIST2_H__D86184E1_4578_4F18_8082_C18CC021CA07__INCLUDED_)
