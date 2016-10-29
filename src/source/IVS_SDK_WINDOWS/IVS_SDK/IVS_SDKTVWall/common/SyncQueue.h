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

/********************************************************************
 filename    :    SyncQueue.h
 author      :    g00209332
 created     :    2012/12/19
 description :    带锁队列
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#ifndef __CSYNCQUEUE_H
#define __CSYNCQUEUE_H

#include <queue>
using namespace std;
#include "Synchronized.h"

extern void CONN_WRITE_LOG(long lLevel, const char *format, ...);

template <class Type>
class CSyncQueue:public CSynchronized
{
public:
    CSyncQueue( );    
    ~CSyncQueue();
    long init(long maxQueueLen);
    bool empty(void);
    long size(void);
    long pushBackEv(Type *ev,long timeOut = 0,long mode = QUEUE_MODE_NOWAIT );
    long popFrontEv(Type *&ev,long timeout = 0,long mode = QUEUE_MODE_WAIT );
private:    
    queue<Type *> p_Queue;
    long m_maxQueueLen;
};

template <class Type> 
CSyncQueue<Type>::CSyncQueue()
{
    m_maxQueueLen = 0;
}

template <class Type>
CSyncQueue<Type>::~CSyncQueue()
{
}

template <class Type>
long CSyncQueue<Type>::init(long maxQueueLen)
{
    long result = VOS_OK;
    
    m_maxQueueLen = maxQueueLen;
    result = start();
    
    return result ;    
}

template <class Type>
bool CSyncQueue<Type>::empty(void)
{
    bool em;
    
    lock();
    em = p_Queue.empty();
    unlock();
    
    return em ;
}

template <class Type>
long CSyncQueue<Type>::size(void)
{
    long sz;
    
    lock();
    sz = p_Queue.size();
    unlock();
    
    return sz ;
}

template <class Type>
long CSyncQueue<Type>::popFrontEv(Type *&ev,long timeout , long mode )
{
    long result = VOS_OK ;

    lock();    
    while( VOS_TRUE )
    { 
        result = p_Queue.empty();        
        if( VOS_OK == result )
        {
            break ;
        }        

        if( QUEUE_MODE_NOWAIT == mode )    
        {
            unlock();
            return VOS_ERR_QUE_EMPTY ;
        }
        
        result = popWait(timeout);
        if( VOS_OK != result )
        {
            unlock();
            return result ;
        }
    }

    ev = p_Queue.front();    
    p_Queue.pop();    
    unlock();

    (void)notifyWrite();


    return result ;
}

template <class Type>
long  CSyncQueue<Type>::pushBackEv(Type *ev, long timeout,long mode)
{
    long result = VOS_OK;   
    long queulen = 0 ;    

    lock();
    queulen = p_Queue.size();        
    if( queulen >= m_maxQueueLen )    
    {
        if( QUEUE_MODE_NOWAIT == mode )
        {
            unlock();
            return VOS_ERR_QUE_LEN;
        }
        
        result = pushWait(timeout);
        if( VOS_OK != result )           
        {
            unlock();
            return VOS_ERR_SYS;
        }
    }

    p_Queue.push(ev);
    unlock();

   (void)notifyRead();

    return result ;
}

#endif
