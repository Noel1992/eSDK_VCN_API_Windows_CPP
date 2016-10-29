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

//lint -e1733

#ifndef __STATIC_PREASSIGN_BUFFER_H__
#define __STATIC_PREASSIGN_BUFFER_H__

#include <ace_header.h>
#include "eSDK_Securec.h"
#define DEFAULT_BUFFER_SIZE  (128 * 1024)

class Static_PreAssign_Buffer
{
public:
    Static_PreAssign_Buffer()
    {
        m_bufBlock = NULL;
        try
        {
            m_bufBlock = new char[DEFAULT_BUFFER_SIZE]; // lint !e1733
        }
        catch(...)
        {
            m_nBufferLength = 0;
            m_pHead = NULL;
            m_pTail = NULL;
            m_iLength = 0;
            return;
        }
        eSDK_MEMSET(this->m_bufBlock, 0, DEFAULT_BUFFER_SIZE);
        m_nBufferLength = DEFAULT_BUFFER_SIZE;
        m_pHead = m_bufBlock;
        m_pTail = m_bufBlock;
        m_iLength = 0;
    }



    //Static_PreAssign_Buffer(const Static_PreAssign_Buffer& tmp)
    //{
    //    copyTmpBuff(tmp); 
    //}

    Static_PreAssign_Buffer& operator=(const Static_PreAssign_Buffer& tmp)
    {
        if (this == &tmp)
        {
            return *this;
        }
        else
        {
            if (NULL != m_bufBlock)
            {
                delete []m_bufBlock;
                m_bufBlock = NULL;
            }

            copyTmpBuff(tmp);

        }
        return *this;
    }

    virtual ~Static_PreAssign_Buffer()
    {
        if (NULL != m_bufBlock)
        {
            delete[] m_bufBlock;
        }

        m_bufBlock = NULL;
        m_pHead = NULL;
        m_pTail = NULL;
        m_iLength = 0;
    }


    void copyTmpBuff(const Static_PreAssign_Buffer& tmp)
    {
        m_bufBlock = NULL;
        if (NULL != tmp.m_bufBlock)
        {
            try
            {
                m_bufBlock = new char[tmp.m_nBufferLength];
            }
            catch(...)
            {
                m_nBufferLength = 0;
                m_pHead = NULL;
                m_pTail = NULL;
                m_iLength = 0;
                return;
            }
            eSDK_MEMSET(this->m_bufBlock, 0, tmp.m_nBufferLength);
            eSDK_MEMCPY(m_bufBlock, tmp.m_nBufferLength, tmp.m_bufBlock, tmp.m_nBufferLength);

            m_nBufferLength = tmp.m_nBufferLength;
            m_pHead = m_bufBlock + (tmp.m_pHead - tmp.m_bufBlock);
            m_pTail = m_bufBlock + (tmp.m_pTail - tmp.m_bufBlock);
            m_iLength = tmp.m_iLength;
        }
    }



    void wr_ptr(size_t n)
    {
        if (n > m_nBufferLength)
        {
            return;
        }

        if(NULL != m_bufBlock)
        {
            m_pTail = (char *) m_bufBlock + n;
            m_iLength = n;
        }
    }

    char * wr_ptr()
    {
        return m_pTail;
    }

    void rd_ptr(size_t n)
    {
        if (n > m_nBufferLength)
        {
            return;
        }

        if(NULL != m_bufBlock)
        {
            m_pHead = (char *) m_bufBlock + n;
        }
    }

    char * rd_ptr()
    {
        return m_pHead;
    }

    size_t size() const
    {
        return m_nBufferLength;
    }

    size_t length() const
    {
        return m_iLength;
    }

    int copy(const char *buf, size_t n)
    {
        if (n > m_nBufferLength)
        {
            return -1;
        }

        if(NULL != m_bufBlock)
        {
            eSDK_MEMMOVE(m_bufBlock, m_nBufferLength, buf, n);
            m_pHead = m_bufBlock;
            m_pTail = (char *) m_bufBlock + n;
            m_iLength = n;
        }

        return (int) n;
    }

private:
    char    *m_bufBlock;
    size_t  m_nBufferLength;
    char    *m_pHead;
    char    *m_pTail;
    size_t  m_iLength;
};

#endif /* __STATIC_PREASSIGN_BUFFER_H__ */

