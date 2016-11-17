// IVSDownLoadPage.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "IVS_Demo.h"
#include "IVSDownLoadPage.h"
#include "IVSSDKSheet.h"
#include "Language.h"

#define MAX_DOWNLOAD_SPEED 255

// CIVSDownLoadPage dialog

IMPLEMENT_DYNAMIC(CIVSDownLoadPage, CPropertyPage)

CIVSDownLoadPage::CIVSDownLoadPage()
	: CPropertyPage(CIVSDownLoadPage::IDD)
	, m_strDevCode(_T(""))
	, m_strStartTime(_T(""))
	, m_strEndTime(_T(""))
	, m_strRecSavePath(_T("D:\\Download"))
	, m_strSnapSavePath(_T("D:\\Download"))
	, m_strDownloadInfo(_T(""))
	, m_strPicName(_T("LocalSnapshot"))
	, m_ulPlatDlHandle(0)
	, m_ulPUDlHandle(0)
	, m_strDLNVRCode(_T(""))
{
}

CIVSDownLoadPage::~CIVSDownLoadPage()
{
}

void CIVSDownLoadPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DevCode, m_strDevCode);
	DDX_Text(pDX, IDC_EDIT_StartTime, m_strStartTime);
	DDX_Text(pDX, IDC_EDIT_EndTime, m_strEndTime);
	DDX_Text(pDX, IDC_EDIT_RecSavePath, m_strRecSavePath);
	DDX_Text(pDX, IDC_EDIT_SnapSavePath, m_strSnapSavePath);
	DDX_Text(pDX, IDC_EDIT_DownloadInfo, m_strDownloadInfo);
	DDX_Control(pDX, IDC_COMBO_PIC_FORMAT, m_ComboPicFormat);
	DDX_Text(pDX, IDC_EDIT_PicName, m_strPicName);
	DDX_Control(pDX, IDC_STATIC_LAB_RECDOWNLOAD, m_GrpDownload);
	DDX_Control(pDX, IDC_STATIC_LAB_SNAPSHOT, m_GrpSanpshot);
	DDX_Control(pDX, IDC_STATIC_LAB_DEVCODE3, m_LabDevCode);
	DDX_Control(pDX, IDC_STATIC_LAB_STARTTIME, m_LabStartTime);
	DDX_Control(pDX, IDC_STATIC_LAB_ENDTIME, m_LabEndTime);
	DDX_Control(pDX, IDC_STATIC_LAB_SAVEPATH2, m_LabDlSavePath);
	DDX_Control(pDX, IDC_STATIC_LAB_SAVEPATH3, m_LabPicSavePath);
	DDX_Control(pDX, IDC_STATIC_LAB_PICNAME, m_LabPicName);
	DDX_Control(pDX, IDC_BUTTON_START_PLATFORM_DOWNLOAD, m_BtnStartPlatDL);
	DDX_Control(pDX, IDC_BUTTON_STOP_PLATFORM_DOWNLOAD, m_BtnStopPlatDL);
	DDX_Control(pDX, IDC_BUTTON_START_PU_DOWNLOAD, m_BtnStartPUDL);
	DDX_Control(pDX, IDC_BUTTON_STOP_PU_DOWNLOAD, m_BtnStopPUDL);
	DDX_Control(pDX, IDC_BUTTON_PAUSE_DOWNLOAD, m_BtnPauseDL);
	DDX_Control(pDX, IDC_BUTTON_RESUME_DOWNLOAD, m_BtnResumeDL);
	DDX_Control(pDX, IDC_BUTTON_GET_DOWNLOAD_INFO, m_BtnGetDLInfo);
	DDX_Control(pDX, IDC_BUTTON_LOCAL_SNAPSHOT, m_BtnSnapshot);
	DDX_Text(pDX, IDC_EDIT_NVR_CODE_DOWNLOAD, m_strDLNVRCode);
}


BEGIN_MESSAGE_MAP(CIVSDownLoadPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_START_PLATFORM_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonStartPlatformDownload)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PLATFORM_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonStopPlatformDownload)
	ON_BN_CLICKED(IDC_BUTTON_START_PU_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonStartPuDownload)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PU_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonStopPuDownload)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonPauseDownload)
	ON_BN_CLICKED(IDC_BUTTON_RESUME_DOWNLOAD, &CIVSDownLoadPage::OnBnClickedButtonResumeDownload)
	ON_BN_CLICKED(IDC_BUTTON_GET_DOWNLOAD_INFO, &CIVSDownLoadPage::OnBnClickedButtonGetDownloadInfo)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_SNAPSHOT, &CIVSDownLoadPage::OnBnClickedButtonLocalSnapshot)
	ON_MESSAGE(WM_DOWNLOADSUCCESS, &CIVSDownLoadPage::OnDownloadSuccess)
END_MESSAGE_MAP()


// CIVSDownLoadPage message handlers


BOOL CIVSDownLoadPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	CHECK_POINTER(m_pSheet, FALSE);
	eSDK::CLanguage* pLang = m_pSheet->m_pLang;
	CHECK_POINTER(pLang, FALSE);

	//设置标签、按钮显示名称
	m_GrpDownload.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_RECDOWNLOAD)));
	m_GrpSanpshot.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SNAPSHOT)));
	m_LabDevCode.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabStartTime.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_STARTTIME)));
	m_LabEndTime.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_ENDTIME)));
	m_LabDlSavePath.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SAVEPATH)));
	m_LabPicSavePath.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SAVEPATH)));
	m_LabPicName.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PICNAME)));
	m_BtnStartPlatDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPLATDL)));
	m_BtnStopPlatDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPLATDL)));
	m_BtnStartPUDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPUDL)));
	m_BtnStopPUDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPUDL)));
	m_BtnPauseDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_DLPAUSE)));
	m_BtnResumeDL.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_DLRESUME)));
	m_BtnGetDLInfo.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_GETDLINFO)));
	m_BtnSnapshot.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_LOCALSNAPSHOT)));

	//图片格式
	m_ComboPicFormat.InsertString(0, _T("JPG"));
	m_ComboPicFormat.InsertString(1, _T("BMP"));
	m_ComboPicFormat.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//设置属性对话框
void CIVSDownLoadPage::SetSheet(CIVSSDKSheet* pSheet)
{
	m_pSheet = pSheet;
}


//下载完成处理函数
afx_msg LRESULT CIVSDownLoadPage::OnDownloadSuccess(WPARAM wParam, LPARAM lParam)
{
	CString strTitle(_T("IVS Demo   "));

	if (0 != m_ulPlatDlHandle)
	{
		OnBnClickedButtonStopPlatformDownload();//停止平台下载
		strTitle.Append(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_PLATDL_COMPLETED)));
	}

	if (0 != m_ulPUDlHandle)
	{
		OnBnClickedButtonStopPuDownload();//停止前端下载
		strTitle.Append(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_PUDL_COMPLETED)));
	}

	m_pSheet->SetWindowText(strTitle);
	return 0;
}


//开始平台录像下载
void CIVSDownLoadPage::OnBnClickedButtonStartPlatformDownload()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_DOWNLOAD_PARAM DownloadParam = {0};//下载参数
	DownloadParam.uiDownloadSpeed = MAX_DOWNLOAD_SPEED;//全速
	DownloadParam.uiRecordFormat = IVS_FILE_MP4;//保存为MP4格式
	DownloadParam.uiSplitterType = RECORD_SPLITE_TIME;//按时间分割
	DownloadParam.uiSplitterValue = 15;//每15分钟分割一次
	DownloadParam.uiNameRule = RULE_NAME_NUM_TIME;//命名规则
	strncpy_s(DownloadParam.stTimeSpan.cStart, m_strStartTime, IVS_TIME_LEN);//录像开始时间
	strncpy_s(DownloadParam.stTimeSpan.cEnd, m_strEndTime, IVS_TIME_LEN);//录像结束时间
	strncpy_s(DownloadParam.cSavePath, m_strRecSavePath, IVS_FILE_NAME_LEN);//保存路径
	memcpy(DownloadParam.cNVRCode, m_strDLNVRCode, IVS_NVR_CODE_LEN); //NVR编码

	IVS_INT32 iRet = IVS_SDK_StartPlatformDownload(m_pSheet->GetSessionID(), m_strDevCode, &DownloadParam, &m_ulPlatDlHandle);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTPLATDL_OK), GET_STRING_PARAM(IDS_STARTPLATDL_FAIL));
}


//停止平台录像下砸
void CIVSDownLoadPage::OnBnClickedButtonStopPlatformDownload()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopPlatformDownload(m_pSheet->GetSessionID(), m_ulPlatDlHandle);

	if (IVS_SUCCEED == iRet)
	{
		m_ulPlatDlHandle = 0;//下载句柄置为无效值
		m_strDownloadInfo = _T("");
		UpdateData(FALSE);
	} 

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPPLATDL_OK), GET_STRING_PARAM(IDS_STOPPLATDL_FAIL));
}


//开始前端录像下载
void CIVSDownLoadPage::OnBnClickedButtonStartPuDownload()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_DOWNLOAD_PARAM DownloadParam = {0};//下载参数
	DownloadParam.uiDownloadSpeed = MAX_DOWNLOAD_SPEED;//全速
	DownloadParam.uiRecordFormat = IVS_FILE_MP4;//保存为MP4格式
	DownloadParam.uiSplitterType = RECORD_SPLITE_TIME;//按时间分割
	DownloadParam.uiSplitterValue = 15;//每15分钟分割一次
	DownloadParam.uiNameRule = RULE_NAME_NUM_TIME;//命名规则
	strncpy_s(DownloadParam.stTimeSpan.cStart, m_strStartTime, IVS_TIME_LEN);//录像开始时间
	strncpy_s(DownloadParam.stTimeSpan.cEnd, m_strEndTime, IVS_TIME_LEN);//录像结束时间
	strncpy_s(DownloadParam.cSavePath, m_strRecSavePath, IVS_FILE_NAME_LEN);//保存路径

	IVS_INT32 iRet = IVS_SDK_StartPUDownload(m_pSheet->GetSessionID(), m_strDevCode, &DownloadParam,&m_ulPUDlHandle);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTPUDL_OK), GET_STRING_PARAM(IDS_STARTPUDL_FAIL));
}


//停止前端录像下载
void CIVSDownLoadPage::OnBnClickedButtonStopPuDownload()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopPUDownload(m_pSheet->GetSessionID(), m_ulPUDlHandle);

	if (IVS_SUCCEED == iRet)
	{
		m_ulPUDlHandle = 0;//下载句柄置为无效值
		m_strDownloadInfo = _T("");
		UpdateData(FALSE);
	} 

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPPUDL_OK), GET_STRING_PARAM(IDS_STOPPUDL_FAIL));
}


//暂停下载
void CIVSDownLoadPage::OnBnClickedButtonPauseDownload()
{
	// TODO: Add your control notification handler code here
	IVS_ULONG ulDlHandle = (0 != m_ulPUDlHandle) ? m_ulPUDlHandle : m_ulPlatDlHandle;//下载句柄
	IVS_INT32 iRet = IVS_SDK_DownloadPause(m_pSheet->GetSessionID(), ulDlHandle);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_DOWNLOADPAUSE_OK), GET_STRING_PARAM(IDS_DOWNLOADPAUSE_FAIL));
}


//恢复下载
void CIVSDownLoadPage::OnBnClickedButtonResumeDownload()
{
	// TODO: Add your control notification handler code here
	IVS_ULONG ulDlHandle = (0 != m_ulPUDlHandle) ? m_ulPUDlHandle : m_ulPlatDlHandle;//下载句柄
	IVS_INT32 iRet = IVS_SDK_DownloadResume(m_pSheet->GetSessionID(), ulDlHandle);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_DOWNLOADRESUME_OK), GET_STRING_PARAM(IDS_DOWNLOADRESUME_FAIL));
}


//获取下载信息
void CIVSDownLoadPage::OnBnClickedButtonGetDownloadInfo()
{
	// TODO: Add your control notification handler code here
	IVS_ULONG ulDlHandle = (0 != m_ulPUDlHandle) ? m_ulPUDlHandle : m_ulPlatDlHandle;//下载句柄
	IVS_DOWNLOAD_INFO DownloadInfo = {0};
	IVS_INT32 iRet = IVS_SDK_GetDownloadInfo(m_pSheet->GetSessionID(), ulDlHandle, &DownloadInfo);

	if (IVS_SUCCEED == iRet)
	{
		//获取下载信息成功
		CString strStatus = m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_STATUS));
		CString strSpeed = m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_SPEED));
		CString strSize = m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_SIZE));
		CString strProgress = m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_PROGRESS));
		CString strTimeLeft = m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_TIMELEFT));
		CString strPause = (0 == DownloadInfo.uiDownloadStatus) ? 
			m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_DLPAUSE)) : m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_DLGOING));
		m_strDownloadInfo.Format(_T("%s:%s %s:%uKB/s %s:%lluKB %s:%u%% %s:%us"), strStatus, strPause, strSpeed, DownloadInfo.uiDownloadSpeed,
			strSize, DownloadInfo.uiDownloadSize, strProgress, DownloadInfo.uiProgress, strTimeLeft, DownloadInfo.uiTimeLeft);
		UpdateData(FALSE);//显示下载信息
	} 

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_GETDLINFO_OK), GET_STRING_PARAM(IDS_GETDLINFO_FAIL));
}


//本地图片抓拍
void CIVSDownLoadPage::OnBnClickedButtonLocalSnapshot()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strFileName;
	strFileName.Append(m_strSnapSavePath);
	strFileName.Append(_T("\\"));
	strFileName.Append(m_strPicName);
	IVS_INT32 iRet = IVS_SDK_LocalSnapshot(m_pSheet->GetSessionID(), m_pSheet->GetPlayHandle(), m_ComboPicFormat.GetCurSel()+1, strFileName);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_LOCALSNAPSHOT_OK), GET_STRING_PARAM(IDS_LOCALSNAPSHOT_FAIL));
}


