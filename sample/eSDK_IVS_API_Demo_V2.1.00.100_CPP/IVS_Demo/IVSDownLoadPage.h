#pragma once
#include "IVS_SDK.h"
#include "afxcmn.h"
#include "afxwin.h"


// CIVSDownLoadPage dialog
class CIVSSDKSheet;
class CIVSDownLoadPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSDownLoadPage)

public:
	CIVSDownLoadPage();
	virtual ~CIVSDownLoadPage();

// Dialog Data
	enum { IDD = IDD_DLG_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;

	//摄像机编码
	CString m_strDevCode;
	//录像起止时间
	CString m_strStartTime;
	CString m_strEndTime;
	//录像保存路径
	CString m_strRecSavePath;

	//抓拍保存路径
	CString m_strSnapSavePath;
	//抓拍图片保存名称
	CString m_strPicName;
	//抓拍图片保存格式
	CComboBox m_ComboPicFormat;

	//下载信息
	CString m_strDownloadInfo;
	//平台录像下载句柄
	IVS_ULONG m_ulPlatDlHandle;
	//前端录像下载句柄
	IVS_ULONG m_ulPUDlHandle;
	// download NVR编码
	CString m_strDLNVRCode;

	/*GroupBox & StaticLabel & Button*/
	CStatic m_GrpDownload;
	CStatic m_GrpSanpshot;
	CStatic m_LabDevCode;
	CStatic m_LabStartTime;
	CStatic m_LabEndTime;
	CStatic m_LabDlSavePath;
	CStatic m_LabPicSavePath;
	CStatic m_LabPicName;
	CButton m_BtnStartPlatDL;
	CButton m_BtnStopPlatDL;
	CButton m_BtnStartPUDL;
	CButton m_BtnStopPUDL;
	CButton m_BtnPauseDL;
	CButton m_BtnResumeDL;
	CButton m_BtnGetDLInfo;
	CButton m_BtnSnapshot;

public:
	virtual BOOL OnInitDialog();

	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);

	//开始平台录像下载
	afx_msg void OnBnClickedButtonStartPlatformDownload();
	//停止平台录像下砸
	afx_msg void OnBnClickedButtonStopPlatformDownload();
	//开始前端录像下载
	afx_msg void OnBnClickedButtonStartPuDownload();
	//停止前端录像下载
	afx_msg void OnBnClickedButtonStopPuDownload();
	//暂停下载
	afx_msg void OnBnClickedButtonPauseDownload();
	//恢复下载
	afx_msg void OnBnClickedButtonResumeDownload();
	//获取下载信息
	afx_msg void OnBnClickedButtonGetDownloadInfo();
	//下载完成处理函数
	afx_msg LRESULT OnDownloadSuccess(WPARAM wParam, LPARAM lParam);

	//本地图片抓拍
	afx_msg void OnBnClickedButtonLocalSnapshot();

};
