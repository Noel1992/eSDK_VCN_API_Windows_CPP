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

#ifndef _SDK_VOICE_STREAMDATA_H_
#define _SDK_VOICE_STREAMDATA_H_

#include <map>
// #include "IVSSDKDef.h"
#include "Lock.h"
#include "hwsdk.h"
//#include "IVS_MLIB.h"

#define BASE_HANDLE_TALK_BACK 1000       // �����Խ�����Сhandleֵ

//�Զ���ͬ���࣬������Ҫ����ɿ�ƽ̨
class CStreamLock
{
public:
	CStreamLock();
    CStreamLock(HANDLE hHandle);
    ~CStreamLock();

private:
    HANDLE m_hHandle;
};

// ����Rtsp������Ϣ
typedef struct _stRTSP_INFO
{
    int     iHandle;
    int     refCont;
    char    szUrl[IVS_URL_LEN];
    bool    bIsTcpDirect;
}RTSP_INFO;
 
typedef struct tagInnerStreamInfo
{
    void   *pStream;
    HANDLE lEventHandle; //���ڶ�����ʵ��ָ��pStream����ͬ������������֧�ֶ��̵߳���
	long   lStreamHandle;
	HWND   hWnd;
    char   szDeviceID[IVS_DEV_CODE_LEN+1];
	RTSP_INFO voiceRtspInfo;
}INNER_STREAM_INFO,*pINNER_STREAM_INFO;

class CStreamData
{
public:
	CStreamData(void);
	~CStreamData(void);
	long CreateHandle();

	//����pStreamΪ���������ʵ��
	long AddStream(long lHandle, void *pStream, HWND /*hWnd*/, char* szDeviceID,RTSP_INFO* pRtspInfo);
	void* RemoveStream(long lHandle);
	void* GetStream(long lHandle);
	void* GetStream(char* pszDeviceID);
	void* GetFirstStream();
	BOOL FindStream(long lHandle);
	BOOL FindStream(char* /*pszDeviceID*/);
	long GetHandleByID(char* /*pszDeviceID*/);

private:
    //CCriticalSection m_MapLock;
	CIVSMutex m_criticalSectionEx;
    //CMap <long, long, INNER_STREAM_INFO*, INNER_STREAM_INFO*> m_MapStream;
	std::map <long,  INNER_STREAM_INFO*> m_MapStream;

};

#endif	//_SDK_VOICE_STREAMDATA_H_
