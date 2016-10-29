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

//#include "StdAfx.h"
#include "StreamData.h"
//#include "IVSSDKDef.h"
#include "ivs_error.h"
#include "eSDK_Securec.h"

//�Զ���ͬ���࣬������Ҫ����ɿ�ƽ̨
CStreamLock::CStreamLock(HANDLE hHandle)
{
    m_hHandle = hHandle;
    WaitForSingleObject(hHandle, INFINITE);
    (void)ResetEvent(m_hHandle);
}

CStreamLock::CStreamLock()
{
    m_hHandle = 0;
}

CStreamLock::~CStreamLock()
{
	if (0 != m_hHandle)
	{
		(void)SetEvent(m_hHandle);
	}
}//lint !e1740 !e1712

//�������ɾ��ֵ
static volatile long g_BaseHandle = BASE_HANDLE_TALK_BACK; 
long IVSCreateHandle()
{
    return ::InterlockedIncrement(&g_BaseHandle);
}

CStreamData::CStreamData(void)
{
	;
}

CStreamData::~CStreamData(void)
{
	;
}

/*************************************************
Function:    CreateHandle    
Description: �������  
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
long CStreamData::CreateHandle()
{
	//ͬ��������Ҫ�滻
	m_criticalSectionEx.Lock();
	long lHandle = IVSCreateHandle();
	//m_criticalSectionEx.LeaveCriticalSectionEx();

	return lHandle;
}

/*************************************************
Function:    AddStream    
Description: ����Stream��Ϣ��Map 
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:      hWnd����Ϊ��(�ص���ʽû�о��),szDeviceID����Ϊ��(�����ļ�����û���豸ID)   
*************************************************/
long CStreamData::AddStream(long lHandle, void *pStream, HWND /*hWnd*/, char* szDeviceID,RTSP_INFO* pRtspInfo)
{
	m_criticalSectionEx.Lock();

	if(NULL == pStream)
	{
		//m_criticalSectionEx.LeaveCriticalSectionEx();
		return IVS_NULL_POINTER;
	}

	INNER_STREAM_INFO *pInfo = NULL;
	try
	{
		pInfo = new INNER_STREAM_INFO;
	}
	catch (...)
	{
		//m_criticalSectionEx.LeaveCriticalSectionEx();
		return IVS_ALLOC_MEMORY_ERROR;
	}
	eSDK_MEMSET(pInfo, 0, sizeof(INNER_STREAM_INFO));

	//���ڶ�����ʵ��ָ��pStream����ͬ������������֧�ֶ��̵߳���
	HANDLE lEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == lEventHandle)
	{
		//m_criticalSectionEx.LeaveCriticalSectionEx();
		if(pInfo)//lint !e774
		{
			delete pInfo;
		}
		return IVS_NULL_POINTER; 
	}

	(void)SetEvent(lEventHandle);

	pInfo->pStream = pStream;
	pInfo->lEventHandle = lEventHandle;
	pInfo->lStreamHandle = lHandle;
	
	if(szDeviceID != NULL)
	{
		eSDK_MEMCPY(pInfo->szDeviceID, IVS_DEV_CODE_LEN+1, szDeviceID, IVS_DEV_CODE_LEN); 
	}

	eSDK_MEMCPY(&pInfo->voiceRtspInfo, sizeof(pInfo->voiceRtspInfo), pRtspInfo,sizeof(RTSP_INFO));
	m_MapStream[lHandle] = pInfo;

	//m_criticalSectionEx.LeaveCriticalSectionEx();

	return lHandle;
}//lint !e818

/*************************************************
Function:    GetStream    
Description: ��Stream��ϢMap�л�ȡStream��Ϣ(���ݾ��)  
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
void* CStreamData::GetStream(long lHandle)
{
	m_criticalSectionEx.Lock();

    //INNER_STREAM_INFO *pInfo = NULL;
    INNER_STREAM_INFO * pInfo = m_MapStream[lHandle];
	//m_criticalSectionEx.LeaveCriticalSectionEx();

	return pInfo;
}

/*************************************************
Function:    GetStream    
Description: ��Stream��ϢMap�л�ȡStream��Ϣ(�����豸ID)  
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
void* CStreamData::GetStream(char* /*pszDeviceID*/)
{
	//CSingleLock playerMapLock(&m_MapLock, TRUE);
    /*long lHandle = 0;
    POSITION pos = m_MapStream.GetStartPosition();

    while(pos != NULL)
    {
        INNER_STREAM_INFO *pInfo = NULL;
        m_MapStream.GetNextAssoc(pos,lHandle,pInfo);

		if(pInfo != NULL && pInfo->pStream != NULL)
        {
			if(0 == strncmp(pInfo->szDeviceID, pszDeviceID, DEVICEID_LEN))
            {
				playerMapLock.Unlock();
                return pInfo;
            }
        }
    }*/
	
	//playerMapLock.Unlock();
    return NULL;
}//lint !e818 !e1762

/*************************************************
Function:    RemoveStream    
Description: ��Stream��ϢMap��ɾ��(���ݾ��)
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
void*  CStreamData::RemoveStream(long lHandle)
{
	m_criticalSectionEx.Lock();

    //INNER_STREAM_INFO *pInfo = NULL;
    INNER_STREAM_INFO * pInfo = m_MapStream[lHandle];
    if(NULL == pInfo)
    {
		//m_criticalSectionEx.LeaveCriticalSectionEx();
        return NULL;
    }

	m_MapStream.erase(lHandle);

	//m_criticalSectionEx.LeaveCriticalSectionEx();
	return pInfo;
}

/*************************************************
Function:    GetStream    
Description: ��Stream��ϢMap�л�ȡ��һ��Stream��Ϣ   
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
void* CStreamData::GetFirstStream()
{
	/*CSingleLock playerMapLock(&m_MapLock, TRUE);

	POSITION pos = m_MapStream.GetStartPosition();

    while(pos != NULL)
    {
        long lHandle = 0;
        INNER_STREAM_INFO *pInfo = NULL;
        m_MapStream.GetNextAssoc(pos, lHandle, pInfo);
        if(pInfo != NULL)
        {
			playerMapLock.Unlock();
			return pInfo;
        }
    }

	playerMapLock.Unlock();*/
	return NULL;
}//lint !e1762

/*************************************************
Function:    FindStream    
Description: ����Stream�Ƿ����(���ݾ��) 
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
BOOL CStreamData::FindStream(long lHandle)
{
	if(GetStream(lHandle) != NULL)
	{
		return TRUE;
	}

	return FALSE;
}

/*************************************************
Function:    FindStream    
Description: ����Stream�Ƿ����(�����豸ID) 
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
BOOL CStreamData::FindStream(char* /*pszDeviceID*/)
{
	/*if(GetStream(pszDeviceID) != NULL)
	{
		return TRUE;
	}*/

	return FALSE;
}//lint !e818 !e1762

/*************************************************
Function:    GetHandleByID    
Description: ��ȡ���ž��(�����豸ID) 
Calls:          
Called By:      
Input:          
Output:         
Return:         
Others:         
*************************************************/
long CStreamData::GetHandleByID(char* /*pszDeviceID*/)
{
	/*CSingleLock playerMapLock(&m_MapLock, TRUE);

	long lHandle = 0;
    int iCount = m_MapStream.GetCount();
    POSITION pos = m_MapStream.GetStartPosition();

    for(int index = 0; index < iCount; index++)
    {
        INNER_STREAM_INFO *pInfo = NULL;
        m_MapStream.GetNextAssoc(pos,lHandle,pInfo);

		if(pInfo != NULL && pInfo->pStream != NULL)
        {
			if(0 == strncmp(pInfo->szDeviceID, pszDeviceID, DEVICEID_LEN))
            {
				playerMapLock.Unlock();
				return lHandle;
            }
        }
    }
	playerMapLock.Unlock();*/
    return IVS_FAIL;
}//lint !e818 !e1762
