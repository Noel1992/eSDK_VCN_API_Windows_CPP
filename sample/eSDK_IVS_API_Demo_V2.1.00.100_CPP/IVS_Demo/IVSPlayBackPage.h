#pragma once
#include "IVS_SDK.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include <list>
#include <map>


// CIVSPlayBackPage dialog
class CIVSSDKSheet;
class CIVSPlayBackPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSPlayBackPage)

public:
	CIVSPlayBackPage();
	virtual ~CIVSPlayBackPage();

// Dialog Data
	enum { IDD = IDD_DLG_PALYBACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;

	//查询起止时间
	CString m_strStartTime;
	CString m_strEndTime;

	//录像目录树
	CTreeCtrl m_treeRecord;
	HTREEITEM m_hItemRecord;

	//录像方式
	CComboBox m_ComboRecordMethod;

	//回放参数
	IVS_PLAYBACK_PARAM m_PlaybackParam;
	//回放摄像机编码
	CString m_strCameraCode;
	//回放起止时间
	CString m_strPlayDuration;
	//本地录像文件
	CString m_strLocalRecPath;

	//平台录像摄像机编码
	CString m_strPlatRecDevCode;
	//前端录像摄像机编码
	CString m_strPURecDevCode;
	//保存路径
	CString m_strSavePath;
	//录像时长
	IVS_INT32 m_iRecordLength;
	// playback NVR编码
	CString m_strPBNVRCode;

	//录像、设备列表
	std::list<IVS_RECORD_INFO*> m_RecordList;
	std::list<IVS_CAMERA_BRIEF_INFO*> m_DeviceList;
	std::map<HTREEITEM, HTREEITEM> m_RecordMap;//key:录像树节点，value:该录像树节点所属的父节点（摄像机树节点）
	std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*> m_DeviceMap;//key:摄像机树节点，value:对应的摄像机信息
	std::map<IVS_CAMERA_BRIEF_INFO*, std::list<IVS_RECORD_INFO*>> m_DeviceRecordMap;//key:摄像机信息，value:该摄像机的录像列表

	/*GroupBox & StaticLabel & Button*/
	CStatic m_GrpSearchRec;
	CStatic m_GrpPlayback;
	CStatic m_GrpRecord;
	CStatic m_LabStartTime;
	CStatic m_LabEndTime;
	CStatic m_LabLocalRecFile;
	CStatic m_LabPlatDevCode;
	CStatic m_LabPUDevCode;
	CStatic m_LabRecLength;
	CStatic m_LabSavePath;
	CButton m_BtnSearchRec;
	CButton m_BtnStartPlatPlay;
	CButton m_BtnStopPlatPlay;
	CButton m_BtnStartPUPlay;
	CButton m_BtnStopPUPlay;
	CButton m_BtnStartLocalPlay;
	CButton m_BtnStopLocalPlay;
	CButton m_BtnPalyPause;
	CButton m_BtnPlayResume;
	CButton m_BtnFastPlay;
	CButton m_BtnRewindPlay;
	CButton m_BtnFrameStep;
	CButton m_BtnStartPlatRec;
	CButton m_BtnStopPlatRec;
	CButton m_BtnStartPuRec;
	CButton m_BtnStopPURec;
	CButton m_BtnStartLocalRec;
	CButton m_BtnStopLocalRec;

public:
	virtual BOOL OnInitDialog();

	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);

	//清空列表
	void ClearList();
	//显示录像列表
	void ShowRecordList();
	//获取设备列表
	IVS_INT32 GetDeviceList();
	//获取录像列表
	afx_msg void OnBnClickedButtonGetRecordList();
	//单击录像列表目录树节点
	afx_msg void OnTvnSelchangedTreeRecordList(NMHDR *pNMHDR, LRESULT *pResult);
	//双击录像列表目录树节点
	afx_msg void OnNMDblclkTreeRecordList(NMHDR *pNMHDR, LRESULT *pResult);
	//更新录像列表
	afx_msg LRESULT OnUpdateRecordList(WPARAM wParam, LPARAM lParam);

	//开始远程（平台、前端）录像回放
	void StartRemotePlayBack();
	//开始平台录像回放
	afx_msg void OnBnClickedButtonStartPlatformPlayBack();
	//停止平台录像回放
	afx_msg void OnBnClickedButtonStopPlatformPlayBack();
	//开始前端录像回放
	afx_msg void OnBnClickedButtonStartPuPlayBack();
	//停止前端录像回放
	afx_msg void OnBnClickedButtonStopPuPlayBack();
	//开始本地录像回放
	afx_msg void OnBnClickedButtonStartLocalPlayBack();
	//停止本地录像回放
	afx_msg void OnBnClickedButtonStopLocalPlayBack();

	//暂停回放
	afx_msg void OnBnClickedButtonPlayBackPause();
	//恢复回放
	afx_msg void OnBnClickedButtonPlayBackResume();
	//快速播放
	afx_msg void OnBnClickedButtonFastPlayBack();
	//倒退播放
	afx_msg void OnBnClickedButtonRewindPlay();
	//单帧步进
	afx_msg void OnBnClickedButtonFramePlay();

	//开始本地录像
	afx_msg void OnBnClickedButtonStartLocalRecord();
	//停止本地录像
	afx_msg void OnBnClickedButtonStopLocalRecord();
	//开始平台录像
	afx_msg void OnBnClickedButtonStartPlatformRecord();
	//停止平台录像
	afx_msg void OnBnClickedButtonStopPlatformRecord();
	//开始前端录像
	afx_msg void OnBnClickedButtonStartPuRecord();
	//停止前端录像
	afx_msg void OnBnClickedButtonStopPuRecord();

};
