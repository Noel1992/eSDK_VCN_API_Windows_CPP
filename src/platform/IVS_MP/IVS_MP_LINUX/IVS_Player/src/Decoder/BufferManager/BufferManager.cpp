//
//
// Copyright 2001-2001 by ComLink Group Inc. All rights reserved.
//
// No part of this document may be reproduced, stored in
// a retrieval system, or transmitted, in any form or by any means,
// electronic, mechanical, photocopying, recording, or otherwise,
// without the prior written permission of ComLink Group Inc..
//
// No part of this document may be reproduced, stored in
// a retrieval system, or transmitted, in any form or by any means,
// electronic, mechanical, photocopying, recording, or otherwise,
// without the prior written permission of ComLink Group Inc..
//
//
//    TITLE :  BufferManager.cpp
//    AUTHOR:  Eric Tasso
//    DESCRIPTION:
//             Implementation of buffer manager
//

//----------------------------------------------------
// Include files
//----------------------------------------------------
#include "BufferManager.h"
#include "ivs_log.h"
//#include "dout.h"

//----------------------------------------------------
// Classes and structures
//----------------------------------------------------


//-----------------------------------------------------------------------------
//
// NAME: CBufferManager
//
// DESCRIPTION:
//      Default Constructor
//-----------------------------------------------------------------------------
CBufferManager::CBufferManager()
: mEmptyBuffers()
, mFullBuffers()
, m_uBufferCount(0)
, m_uBufferSize(0)
, mbIsOn(false)
, m_bEptBufFull(FALSE)
{
	mhEmptyMutex = VOS_CreateMutex();
	mhFullMutex = VOS_CreateMutex();
	VOS_CreateEvent(mhEventEmptyBufferInserted);
	VOS_CreateEvent(mhEventFullBufferInserted);

	LOCK_INIT(m_csEmpFlag);
}

//-----------------------------------------------------------------------------
//
// NAME: ~CBufferManager
//
// DESCRIPTION:
//       Default Destructor
//-----------------------------------------------------------------------------
CBufferManager::~CBufferManager()
{
    try
    {
        Shutdown();
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "BufferList2::push_back()",
                "CBufferManager::~CBufferManager"); //lint !e1551
    }

    if (mhEmptyMutex)
    {
		VOS_DestroyMutex(mhEmptyMutex);
        mhEmptyMutex = NULL;
    }

    if (mhFullMutex)
    {
		VOS_DestroyMutex(mhFullMutex);
        mhFullMutex = NULL;
    }

    if (mhEventEmptyBufferInserted)
    {
		VOS_DestroyEvent(mhEventEmptyBufferInserted);
        mhEventEmptyBufferInserted = NULL;
    }

    if (mhEventFullBufferInserted)
    {
		VOS_DestroyEvent(mhEventFullBufferInserted);
        mhEventFullBufferInserted = NULL;
    }
	LOCK_DESTROY(m_csEmpFlag);
}

//-----------------------------------------------------------------------------
//
// NAME: Initialise
//
// DESCRIPTION:
//       Used to initialise the video source
//
// RETURN:
//       True if successful, false otherwise.
//
//-----------------------------------------------------------------------------
bool CBufferManager::Initialise(unsigned int uBufferCount,
                                unsigned int uBufferSize)
{
    m_uBufferCount = uBufferCount;
    m_uBufferSize = uBufferSize;

    if (!InitBuffers())
    {
        return false;
    }
    else
    {
        mbIsOn = true;
        return true;
    }
}

//-----------------------------------------------------------------------------
//
// NAME: Shutdown
//
// DESCRIPTION:
//       Used to shutdown the video source
//
//-----------------------------------------------------------------------------
void CBufferManager::Shutdown(void)
{
    mbIsOn = false;
    ReleaseBuffers();
}

//-----------------------------------------------------------------------------
//
// NAME: Start
//
// DESCRIPTION:
//       Used to Start the buffer manager
//
//-----------------------------------------------------------------------------
void CBufferManager::Start(void)
{
    mbIsOn = true;
}

//-----------------------------------------------------------------------------
//
// NAME: Stop
//
// DESCRIPTION:
//       Used to stop the buffer manager
//
//-----------------------------------------------------------------------------
void CBufferManager::Stop(void)
{
    mbIsOn = false;
}

//-----------------------------------------------------------------------------
//
// NAME: GetEmptyBuf
//
// DESCRIPTION:
//       Used to return an empty buffer
//
// RETURN:
//       Pointer to an empty BYTE buffer
//
//-----------------------------------------------------------------------------
BYTE* CBufferManager::GetEmptyBuf(BOOL bBlock)
{
    // Init
    BYTE* pBuffer = NULL;
    bool bFound = false;
    // delete for error 550 [3/19/2009 z90003203]
    //DWORD dwWaitResult;

    // Let's get the mutex
    //WaitForSingleObject( mhEmptyMutex, INFINITE );
	VOS_MutexLock(mhEmptyMutex);

    do
    {  
        // Is buffer management on?
        if(mbIsOn)
        {  
            // YES
            // Are there any empty buffers available?
            if(mEmptyBuffers.empty())
            {
                // NO, let's wait for an insertion
                // :DEBUG: EricT
                // DOUT((DOUT_ERR, "No more empty buffers.\n"));
				VOS_ReSetEvent(mhEventEmptyBufferInserted);
				VOS_MutexUnlock(mhEmptyMutex);

                if (bBlock)
                {
                    // modify for error 550 [3/19/2009 z90003203]
                    //dwWaitResult = WaitForSingleObject(mhEventEmptyBufferInserted, 1000);
					VOS_WaitEvent(mhEventEmptyBufferInserted,1000);

					VOS_MutexLock(mhEmptyMutex);
                }
                else
                {
                    // NO, let's return a NULL pointer
                    bFound = true;
                    pBuffer = NULL;
                }
            }
            else
            {
                // YES, let's get the next one 
                pBuffer = mEmptyBuffers.front();
                (void)mEmptyBuffers.pop_front();
                bFound = true;
            }
        }
        else
        {
            // NO, let's return a NULL pointer
            bFound = true;
            pBuffer = NULL;
        }
    }
    while(!bFound);

    // Release the mutex
    //ReleaseMutex( mhEmptyMutex);
	VOS_MutexUnlock(mhEmptyMutex);

    return pBuffer;
}

//-----------------------------------------------------------------------------
//
// NAME: GetFullBuf
//
// DESCRIPTION:
//       Used to return an empty buffer
//
// RETURN:
//       Pointer to a full BYTE buffer
//
//-----------------------------------------------------------------------------
BYTE* CBufferManager::GetFullBuf(BOOL bBlock)
{
    // Init
    BYTE* pBuffer = NULL;
    bool bFound = false;
    // delete for error 550 [3/19/2009 z90003203]
    //DWORD dwWaitResult;

    // Let's get the mutex
	VOS_MutexLock(mhFullMutex);

    do
    {
        // Is buffer management on?
        if(mbIsOn)
        {
            // YES
            // Are there any full buffers available?
            if(mFullBuffers.empty())
            {
                // NO, let's wait for an insertion
                // :DEBUG: EricT: 
                // DOUT((DOUT_ERR, "No more full buffers.\n"));
				VOS_ReSetEvent(mhEventFullBufferInserted);
				VOS_MutexUnlock(mhFullMutex);

                if (bBlock)
                {
                    // delete for error 550 [3/19/2009 z90003203]
                    //dwWaitResult = WaitForSingleObject(mhEventFullBufferInserted, 1000);
                    //WaitForSingleObject(mhEventFullBufferInserted, 200);//减少用包的时候，等待的时间
					VOS_WaitEvent(mhEventFullBufferInserted,200);

					VOS_MutexLock(mhFullMutex);
                }
                else
                {
                    // NO, let's return a NULL pointer
                    bFound = true;
                    pBuffer = NULL;
                }
            }
            else
            {
                // YES, let's get the next one
                pBuffer = mFullBuffers.front();
                (void)mFullBuffers.pop_front();
                bFound = true;
            }
        }
        else
        {
            // NO, let's return a NULL pointer
            bFound = true;
            pBuffer = NULL;
        }
    }
    while(!bFound);

    // Release the mutex
	VOS_MutexUnlock(mhFullMutex);

    return pBuffer;
}

//-----------------------------------------------------------------------------
//
// NAME: InsertEmptyBuf
//
// DESCRIPTION:
//       Used to insert an empty buffer
//
// INPUT:
//       pBuffer: pointer to the buffer to be inserted
//       
// RETURN: 
//       true if successful, false otherwise
//
//-----------------------------------------------------------------------------
bool CBufferManager::InsertEmptyBuf(BYTE* pBuffer)
{
    // Verify params
    if (NULL != pBuffer)
    {
        // Let's get the mutex
		VOS_MutexLock(mhEmptyMutex);

        // Insert the buffer in list

        unsigned int ret = mEmptyBuffers.push_back(pBuffer);
		VOS_SetEvent(mhEventEmptyBufferInserted);
        // Release the mutex
		VOS_MutexUnlock(mhEmptyMutex);
        if(IVS_BUFFER_FAIL == int(ret))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
//
// NAME: InsertFullBuf
//
// DESCRIPTION:
//       Used to insert a full buffer
//
// INPUT:
//       pBuffer: pointer to the buffer to be inserted
//       
// RETURN: 
//       true if successful, false otherwise
//
//-----------------------------------------------------------------------------
bool CBufferManager::InsertFullBuf(BYTE* pBuffer)
{
    // Verify params
    if (NULL != pBuffer)
    {
        // Let's get the mutex
		VOS_MutexLock(mhFullMutex);

        // Insert the buffer in list
        unsigned int ret = mFullBuffers.push_back(pBuffer);
        //(void)SetEvent(mhEventFullBufferInserted);
		VOS_SetEvent(mhEventFullBufferInserted);
        // Release the mutex
		VOS_MutexUnlock(mhFullMutex);
        if(IVS_BUFFER_FAIL == int(ret))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
//
// NAME: InitBuffers
//
// DESCRIPTION:
//       Used to initialise internal buffers
//
// RETURN:
//       True if successful, false otherwise.
//
//-----------------------------------------------------------------------------
bool CBufferManager::InitBuffers(void)
{
    // Init
    BYTE* pBuffer = NULL;

    //lint -esym(429, pBuffer, 349)
    (void)mEmptyBuffers.alloc(m_uBufferCount + 1);

    (void)mFullBuffers.alloc(m_uBufferCount + 1);

    //   mManagerBuffers.alloc(m_iBufferCount + 1);

    // Allocation of BUFFER_COUNT buffers
    for(unsigned int i=0; i<m_uBufferCount; i++)
    {
        try
        {
            pBuffer = new unsigned char[m_uBufferSize];
        }
        catch (...)
        {
            pBuffer = NULL;
            IVS_LOG(IVS_LOG_ERR, "BufferList2::InitBuffers()",
                    "CBufferManager::InitBuffers(void) exception!");

            continue;
        }
        // Let's add the link to the end of the list
        (void)mEmptyBuffers.push_back(pBuffer);
        //     mManagerBuffers.push_back(pBuffer);
    }
    
    SetEptBufFull(TRUE);
    
    return true;
}

//-----------------------------------------------------------------------------
//
// NAME: ReleaseBuffers
//
// DESCRIPTION:
//       Used to release internal buffers
//
//-----------------------------------------------------------------------------
void CBufferManager::ReleaseBuffers(void)
{
    try
    {
        // Init
        BYTE* pTmpBuffer = NULL;

        // Delete all buffers in empty list
        while (mEmptyBuffers.size() > 0)
        {
            pTmpBuffer = mEmptyBuffers.back();  // get last element
            (void)mEmptyBuffers.pop_back();           // remove it from the list
            if(pTmpBuffer)
            {
                delete [] pTmpBuffer;               // deallocate it
                pTmpBuffer = NULL;
            }
        }
        mEmptyBuffers.clear();

        IVS_LOG(IVS_LOG_ERR, "BufferList2::ReleaseBuffers()",
                "mFullBuffersSize is %d", mFullBuffers.size());

        // Delete all buffers in full list
        while (mFullBuffers.size() > 0)
        {
            pTmpBuffer = mFullBuffers.back();  // get last element
            (void)mFullBuffers.pop_back();           // remove it from the list

            if(pTmpBuffer)
            {
                delete [] pTmpBuffer;              // deallocate it
                pTmpBuffer = NULL;
            }
        }
        mFullBuffers.clear();

        /*        while (!mManagerBuffers.empty())
        {
        pTmpBuffer = mManagerBuffers.back();  // get last element
        mManagerBuffers.pop_back();           // remove it from the list

        if(pTmpBuffer)
        {
        delete [] pTmpBuffer;              // deallocate it
        pTmpBuffer = NULL;
        }
        }
        mManagerBuffers.clear();*/
    }
    catch (...)
    {
        IVS_LOG(IVS_LOG_ERR, "BufferList2::ReleaseBuffers()",
                "CBufferManager::ReleaseBuffers(void) exception!");
    }
}

//-----------------------------------------------------------------------------
//
// NAME: ResetBuffer
//
// DESCRIPTION:
//       Used to reset internal buffers
//
//-----------------------------------------------------------------------------
void CBufferManager::ResetBuffer(void)
{
    BYTE* pTmpBuffer = NULL;

    // Let's get the mutex
	VOS_MutexLock(mhFullMutex);

    // Delete all buffers in full list
    while (mFullBuffers.size() > 0)
    {
        pTmpBuffer = mFullBuffers.back();  // get last element
        (void)mFullBuffers.pop_back();           // remove it from the list

        (void)InsertEmptyBuf(pTmpBuffer);
    }

    // Release the mutex
	VOS_MutexUnlock(mhFullMutex);
}
// End of file




