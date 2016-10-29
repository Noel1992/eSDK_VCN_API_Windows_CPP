//
//
// Copyright 2001-2001 by ComLink Group Inc. All rights reserved.
//
// No part of this document may be reproduced, stored in
// a retrieval system, or transmitted, in any form or by any means,
// electronic, mechanical, photocopying, recording, or otherwise,
// without the prior written permission of ComLink Group Inc.
//
//
//    NAME  :  BufferManager.h
//    AUTHOR:  Eric Tasso
//    DESCRIPTION:
//       This class manages two buffer lists, one list of empty
//       buffers and one list of full buffers. To be used for example
//       by a video decoding application.

#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

//--------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
//#include <list>
#include "IVSPlayerDataType.h"
#include "BufferList2.h"
#include "Lock.h"
#include "vos.h"

//--------------------------------------------------------------------
// Public definitions, enumerations
//--------------------------------------------------------------------

//using namespace std;
//typedef std::list<BYTE*> BUFFERLIST;         // List of buffers
/*
const int BUFFER_COUNT = 256;  // Total number of pre-allocated buffers
const int BUFFER_SIZE  = 1648; // Buffer size in bytes
*/
const int UDP_BUFFER_COUNT = 256;  // Total number of pre-allocated buffers
const int UDP_BUFFER_SIZE  = 1648; // Buffer size in bytes

const int TCP_BUFFER_COUNT = 40;  // Total number of pre-allocated buffers
const int TCP_BUFFER_SIZE  = (256*1024); // Buffer size in bytes

//----------------------------------------------------------------------------
// Classes and public structures
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class definition.
//----------------------------------------------------------------------------

//
// CLASS NAME: CBufferManager
//
// BASE CLASS: none
//
// DESCRIPTION:
//          Buffer management class.
//

class CBufferManager
{

public:

    // Default constructor.
    CBufferManager();

    // Default destructor.
    ~CBufferManager();

    // Initialise
    bool Initialise(unsigned int uBufferCount, unsigned int uBufferSize);

    // Shutdown
    void Shutdown(void);

    // Start
    void Start(void);

    // Stop
    void Stop(void);

    // Returns an empty buffer
    BYTE* GetEmptyBuf(BOOL bBlock = TRUE);

    // Returns a full buffer
    BYTE* GetFullBuf(BOOL bBlock = TRUE);

    // Inserts an empty buffer
    bool InsertEmptyBuf(BYTE* pBuffer);

    // Inserts a full buffer
    bool InsertFullBuf(BYTE* pBuffer);

    // Returns the internal buffer length
    inline unsigned int GetBufferLength(void) const {return m_uBufferSize;}
    inline unsigned int GetBufferCount(void) const  {return m_uBufferCount;}

    inline unsigned int GetEptBufCount(void) const {return mEmptyBuffers.size();}
    inline unsigned int GetFullBufCount(void) const{return mFullBuffers.size();}

    inline void SetEptBufFull(BOOL bFull)
    {
        m_bEptBufFull = bFull;
    }

    inline const BOOL GetEptBufFull() const 
    {
        return m_bEptBufFull;
    }
#ifdef WIN32
    inline CRITICAL_SECTION* GetEptBufSection()
    {
        return &m_csEmpFlag; //lint !e1536
    }
#endif
	//inline void SetMsgWnd(HWND hWnd) {m_hMsgWng = hWnd;}
    //inline HWND GetMsgWnd() {return m_hMsgWng;}
    // Unlock all mutexes
    inline void IsOn(bool bState) {mbIsOn = bState;}

    // Buffer list reset
    void ResetBuffer(void);

private:

    // Members
    //   BUFFERLIST mEmptyBuffers;  // List of empty buffers
    //  BUFFERLIST mFullBuffers;   // List of buffers containing data to be decoded
    BufferList2 mEmptyBuffers;
    BufferList2 mFullBuffers;
    //    BufferList2 mManagerBuffers;
    unsigned int m_uBufferCount;
    unsigned int m_uBufferSize;

	VOS_Mutex* mhEmptyMutex;
	VOS_Mutex* mhFullMutex;
	VOS_Event* mhEventEmptyBufferInserted;
	VOS_Event* mhEventFullBufferInserted;

    bool mbIsOn;               // Is buffer management on or off. Used for shutdown purposes.
    BOOL m_bEptBufFull;
 //   HWND m_hMsgWng;

	LOCK_T  m_csEmpFlag;

    // Buffer list initialisation
    bool InitBuffers(void);    

    // Buffer list release
    void ReleaseBuffers(void);

    // Copy constructor: USE IS FORBIDDEN (That's why there is no implementation).
    CBufferManager(const CBufferManager& Src);

    // Copy operator: USE IS FORBIDDEN (That's why there is no implementation).
    CBufferManager& operator=(const CBufferManager& Src);

};

#endif

// End of file 
