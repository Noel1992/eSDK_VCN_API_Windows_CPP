#pragma once
#include "IVSRealPlayPage.h"
#include "IVSPlayBackPage.h"
#include "IVSUsrMgrPage.h"
#include "IVSDownLoadPage.h"
#include "IVSAlarmPage.h"
#include "IVSTalkBackPage.h"
#include "Language.h"
#include <list>


// CIVSSDKSheet

class CIVSSDKSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CIVSSDKSheet)

public:
	CIVSSDKSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CIVSSDKSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CIVSSDKSheet();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_ShowBox;
	IVS_INT32 m_iSessionID;
	CBrush m_Brush;

	//当前视频播放类型
	IVS_PLAY_TYPE m_PlayingType;

	//视频播放句柄
	IVS_ULONG m_ulRealPlayHandle;
	IVS_ULONG m_ulPUPlayHandle;
	IVS_ULONG m_ulPlatPlayHandle;
	IVS_ULONG m_ulLocalPlayHandle;

	//摄像机编码
	CString m_strRealPlayDevCode;
	CString m_strPUPlayDevCode;
	CString m_strPlatPlayDevCode;

	//本地录像文件名
	CString m_strLocalFileName;

	//实况、录像播放参数
	IVS_REALPLAY_PARAM  m_RealPlayPara;
	IVS_PLAYBACK_PARAM m_PlayBackPara;
	IVS_LOCAL_PLAYBACK_PARAM m_LocalPlayParam;

public:
	eSDK::CLanguage* m_pLang;

	//登录、实况、回放、下载、告警标签页
	CIVSUsrMgrPage m_UsrMgrPage;
	CIVSRealPlayPage m_RealPlayPage;
	CIVSPlayBackPage m_PlayBackPage;
	CIVSDownLoadPage m_DownLoadPage;
	CIVSAlarmPage m_AlarmPage;
	CIVSTalkBackPage m_TalkBackPage;

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void InitSheet();

	//在标题栏显示操作信息
	void ShowTitleMsg(IVS_INT32 iRet, IVS_UINT32 IDS_OK, CString strOK, IVS_UINT32 IDS_FAIL, CString strFAIL);

	//设置属性对话框
	void SetSheet(void);
	//设置标签页标题
	void SetPageTitle(void);

	//设置SessionID
	void SetSessionID(IVS_INT32 iSessionID);
	//获取SessionID
	IVS_INT32 GetSessionID(void);

	//获取播放句柄
	IVS_ULONG GetPlayHandle();

	//设置回调函数
	void SetEventCallback();

	//停止正在播放的视频
	void StopVideoPlay();
	//开始实况
	void StartRealPlay();
	//开始前端录像回放
	void StartPUPlayBack();
	//开始平台录像回放
	void StartPlatformPlayBack();
	//开始本地录像回放
	void StartLocalPlayBack();
	//停止实况
	void StopRealPlay();
	//停止前端录像回放
	void StopPUPlayBack();
	//停止平台录像回放
	void StopPlatformPlayBack();
	//停止本地录像回放
	void StopLocalPlayBack();

	//设置实况播放参数
	void SetRealPlayPara(const CString strCameraCode, const IVS_REALPLAY_PARAM* pRealplayPara);
	//设置前端回放参数
	void SetPUPlayBackPara(const CString strCameraCode, const IVS_PLAYBACK_PARAM* pPlaybackParam);
	//设置平台回放参数
	void SetPlatPlayBackPara(const CString strCameraCode, const IVS_PLAYBACK_PARAM* pPlaybackParam);
	//设置本地回放参数
	void SetLocalPlayBackPara(const CString strLocalFileName, const IVS_LOCAL_PLAYBACK_PARAM* pPlaybackParam);

	//暂停回放
	void PlayBackPause();
	//恢复回放
	void PlayBackResume();
	//回放单帧步进
	void PlayBackFrameStepForward();
	//设置回放速度
	void SetPlayBackSpeed(IVS_FLOAT fSpeed);
};


