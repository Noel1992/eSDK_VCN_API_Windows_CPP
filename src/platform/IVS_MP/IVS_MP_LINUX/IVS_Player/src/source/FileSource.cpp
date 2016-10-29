#ifdef WIN32
#include <process.h>   // _beginthread, _endthread 
#endif

#include <stdio.h>

#include "FileSource.h"
#include "ivs_log.h"


CFileSource::CFileSource(void)
{
	m_bStarted       = false;
	m_bInited        = false;
	m_bRunFlag       = false;
	m_hReadThread    = NULL;
	m_pfFrameCB      = NULL;
	m_pUserFrame     = NULL;
	m_pfExceptionCB  = NULL;
	m_pUserException = NULL;
	m_nState         = SOURCE_STOPPED;
	m_TmBase         = 0;
	memset(m_file, 0, sizeof(m_file));

//     m_strFileName    = "";
    m_bCycle         = false;

    m_uDstAudioPayLoadType = PAY_LOAD_TYPE_PCMU;
	memset(&m_stAudioExceptionData, 0, sizeof(m_stAudioExceptionData));
}

CFileSource::~CFileSource(void)
{  
	try {
        Stop();
	}
	catch (...)
	{
	}
	
	m_pfFrameCB      = NULL;
	m_pUserFrame     = NULL;
	m_pfExceptionCB  = NULL;
	m_pUserException = NULL;
	m_hReadThread    = NULL;
}


/*
 * ReadThreadProc
 * 文件读取工作线程
 */
unsigned int __SDK_CALL CFileSource::ReadThreadProc(void* lpParameter)
{
	CFileSource *pSource = (CFileSource *)lpParameter;
	if (NULL == pSource)
	{
		//_endthreadex(0);
		return 0;
	}

	while (pSource->m_bRunFlag)
	{
		if (SOURCE_PAUSED == pSource->m_nState)
		{
			//Sleep(100);
			VOS_Sleep(100);
			continue;
		}

		MEDIA_FRAME_INFO info;
		memset(&info, 0, sizeof(info));
		int result = pSource->GetNextFrame(info);
		if (IVS_SUCCEED != result)
		{
			if (IVS_PLAYER_RET_BROADCAST_FILE_IS_END == result
				&& pSource->IsCycle())
			{
				continue;
			}

			//pSource->ExcuteExceptionCallBack(result);

			info.bEOS = 1;
			pSource->ExcuteFrameDataCallBack(info);
			IVS_LOG(IVS_LOG_INFO, "File Read Proc", "End of File");
			//break;

			pSource->m_nState = SOURCE_PAUSED;
			continue;
		}

		pSource->ExcuteFrameDataCallBack(info);
	}
	//_endthreadex(0);
	return 0;
}

/*
 * Start
 * 启动读取线程
 */
int CFileSource::Start()
{
    IVS_ASSERT(SOURCE_STOPPED == m_nState);
	if (SOURCE_STOPPED != m_nState)
	{
		IVS_LOG(IVS_LOG_NOTICE, "FileSource Start", "Source Busy");
		return IVS_SUCCEED;
	}

	if (!m_bInited)
	{
		IVS_LOG(IVS_LOG_ERR, "FileSource Start", "Source Not Ready");
		return IVS_FAIL;
	}

	m_bRunFlag = true;
	//m_hReadThread = (HANDLE)_beginthreadex(NULL, 0, ReadThreadProc, this, 0, NULL);
	if (VOS_OK != VOS_CreateThread((VOS_THREAD_FUNC)ReadThreadProc, (void *)this, &m_hReadThread, 0))
	{
		return IVS_FAIL;
	}

	if (NULL == m_hReadThread)
	{
		//IVS_LOG(IVS_LOG_ERR, "Start FileSource", "Thread: %lu", GetLastError());
		return IVS_FAIL;
	}
	m_nState = SOURCE_READING;
	m_bStarted = true;
	return IVS_SUCCEED;
}


/*
 * Stop
 * 停止播放线程
 */
void CFileSource::Stop()
{
	if (m_hReadThread)
	{
		m_bRunFlag = false;
		/*
		WaitForSingleObject(m_hReadThread, INFINITE);
		CloseHandle(m_hReadThread);
		*/
		VOS_ThreadJoin(m_hReadThread);
		m_hReadThread = NULL;
	}
	m_nState = SOURCE_STOPPED;
}

/* 
 * Pause
 */
int CFileSource::Pause()
{
	IVS_ASSERT(SOURCE_READING == m_nState);
	m_nState = SOURCE_PAUSED;
	return IVS_SUCCEED;
}

/*
 * Resume
 */
int CFileSource::Resume()
{
	//IVS_ASSERT(SOURCE_PAUSED == m_nState);
	if (SOURCE_STOPPED != m_nState)
	{
		m_nState = SOURCE_READING;
		return IVS_SUCCEED;
	}
    return IVS_FAIL;
}

/*
 * Init
 */
int CFileSource::Init(const char *filename, bool bCycle)
{
	if (NULL == filename || '\0' == filename[0])
	{
		IVS_LOG(IVS_LOG_ERR, "FileSource Init", "File Invalid");
		return IVS_PARA_INVALID;
	}
#ifdef WIN32
	(void)strncpy_s(m_file, filename, sizeof(m_file) - 1);
#else
	strncpy(m_file,filename,sizeof(m_file) - 1);
#endif

	if (IVS_SUCCEED != InitInternal())
	{
		return IVS_FAIL;
    }

    m_bCycle = bCycle;

	m_bInited = true;
	return IVS_SUCCEED;
}

void CFileSource::ExcuteFrameDataCallBack(MEDIA_FRAME_INFO &info)
{
	if (m_pfFrameCB)
	{
		m_pfFrameCB(&info, m_pUserFrame);
	}
}

void CFileSource::ExcuteExceptionCallBack(long ExceptionCode)
{
	if (m_pfExceptionCB)
	{
		m_pfExceptionCB(0, IVS_EVENT_LOCAL_PLAYBACK_FAILED, NULL, m_pUserException);
	}
}//lint !e1762 

void CFileSource::SetFrameDataCallBack(FILE_DATA_CALLBACK callback, void *pUser)
{
    m_pfFrameCB  = callback;
	m_pUserFrame = pUser;
}

void CFileSource::SetExceptionCallBack(PLAYER_EVENT_CALLBACK callback, void *pUser)
{
	m_pfExceptionCB  = callback;
	m_pUserException = pUser;
}


