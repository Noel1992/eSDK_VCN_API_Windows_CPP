// IVSUsrMgrPage.cpp : implementation file
//

#include "stdafx.h"
#include "IVS_Demo.h"
#include "IVSUsrMgrPage.h"
#include "afxdialogex.h"
#include "IVSSDKSheet.h"
#include "Language.h"


// CIVSUsrMgrPage dialog

IMPLEMENT_DYNAMIC(CIVSUsrMgrPage, CPropertyPage)

CIVSUsrMgrPage::CIVSUsrMgrPage()
	: CPropertyPage(CIVSUsrMgrPage::IDD)
	, m_strUsrName(_T("admin"))
	, m_strPassword(_T("1qw2!QW@"))
	, m_strIPAddress(_T("172.22.9.51"))
	, m_iPort(9900)
	, m_strOldPwd(_T(""))
	, m_strNewPwd(_T(""))
	, m_strConfirmPwd(_T(""))
	, m_strVersion(_T(""))
{
	m_pSheet = NULL;
	m_iSessionID = SDK_SESSIONID_INIT;
}

CIVSUsrMgrPage::~CIVSUsrMgrPage()
{
}

void CIVSUsrMgrPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USRNAME, m_strUsrName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_strIPAddress);
	DDX_Text(pDX, IDC_EDIT_PORT, m_iPort);
	DDX_Text(pDX, IDC_EDIT_OLDPWD, m_strOldPwd);
	DDX_Text(pDX, IDC_EDIT_NEWPWD, m_strNewPwd);
	DDX_Text(pDX, IDC_EDIT_CONFIRM, m_strConfirmPwd);
	DDX_Text(pDX, IDC_EDIT_VERSION, m_strVersion);
	DDX_Control(pDX, IDC_STATIC_RESOURCE, m_GrpSDKCtrl);
	DDX_Control(pDX, IDC_STATIC_LOGIN, m_GrpLoginCtrl);
	DDX_Control(pDX, IDC_STATIC_PWDMGR, m_GrpPwdCtrl);
	DDX_Control(pDX, IDC_STATIC_USRNAME, m_LabUsrName);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_LabPwd);
	DDX_Control(pDX, IDC_STATIC_SERVER, m_LabServer);
	DDX_Control(pDX, IDC_STATIC_PORT, m_LabPort);
	DDX_Control(pDX, IDC_STATIC_OLDPWD, m_LabOldPwd);
	DDX_Control(pDX, IDC_STATIC_NEWPWD, m_LabNewPwd);
	DDX_Control(pDX, IDC_STATIC_CONFIRM, m_LabConfirmPwd);
	DDX_Control(pDX, IDC_BUTTON_INIT, m_BtnInit);
	DDX_Control(pDX, IDC_BUTTON_CLEAN, m_BtnUninit);
	DDX_Control(pDX, IDC_BUTTON_GET_VER, m_BtnGetVer);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_BtnLogin);
	DDX_Control(pDX, IDC_BUTTON_LOGOUT, m_BtnLogout);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_PWD, m_BtnChangePwd);
}


BEGIN_MESSAGE_MAP(CIVSUsrMgrPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CIVSUsrMgrPage::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CIVSUsrMgrPage::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CIVSUsrMgrPage::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CIVSUsrMgrPage::OnBnClickedButtonLogout)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_PWD, &CIVSUsrMgrPage::OnBnClickedButtonChangePwd)
	ON_BN_CLICKED(IDC_BUTTON_GET_VER, &CIVSUsrMgrPage::OnBnClickedButtonGetVer)
END_MESSAGE_MAP()


// CIVSUsrMgrPage message handlers


BOOL CIVSUsrMgrPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	eSDK::CLanguage* pLang = m_pSheet->m_pLang;
	//设置标签、按钮显示名称
	m_GrpSDKCtrl.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SDKCTRL)));
	m_GrpLoginCtrl.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_LOGINCTRL)));
	m_GrpPwdCtrl.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PWDCTRL)));
	m_LabUsrName.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_USER_NAME)));
	m_LabPwd.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PASSWORD)));
	m_LabServer.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SERVER)));
	m_LabPort.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PORT)));
	m_LabOldPwd.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_OLDPWD)));
	m_LabNewPwd.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_NEWPWD)));
	m_LabConfirmPwd.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_CONFIRMPWD)));
	m_BtnInit.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_INITIALIZE)));
	m_BtnUninit.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_RELEASE)));
	m_BtnGetVer.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_VERSION)));
	m_BtnLogin.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_LOGIN)));
	m_BtnLogout.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_LOGOUT)));
	m_BtnChangePwd.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_CHANGEPWD)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//设置属性对话框
void CIVSUsrMgrPage::SetSheet(CIVSSDKSheet* pSheet)
{
	m_pSheet = pSheet;
}


//初始化SDK
void CIVSUsrMgrPage::OnBnClickedButtonInit()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_Init();

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_INITSDK_OK), GET_STRING_PARAM(IDS_INITSDK_FAIL));
}


//释放SDK
void CIVSUsrMgrPage::OnBnClickedButtonClean()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_Cleanup();

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_RELEASESDK_OK), GET_STRING_PARAM(IDS_RELEASESDK_FAIL));
}


//用户登录
void CIVSUsrMgrPage::OnBnClickedButtonLogin()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	IVS_LOGIN_INFO LoginReqInfo = {0};
	LoginReqInfo.stIP.uiIPType = IP_V4;
	strncpy_s(LoginReqInfo.stIP.cIP, m_strIPAddress, IVS_IP_LEN);
	LoginReqInfo.uiPort = static_cast<IVS_UINT32>(m_iPort);
	strncpy_s(LoginReqInfo.cUserName, m_strUsrName, IVS_NAME_LEN);
	strncpy_s(LoginReqInfo.pPWD, m_strPassword, IVS_PWD_LEN);
	
	IVS_INT32 iRet = IVS_SDK_Login(&LoginReqInfo, &m_iSessionID);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_LOGIN_OK), GET_STRING_PARAM(IDS_LOGIN_FAIL));

	if (IVS_SUCCEED == iRet)
	{
		m_pSheet->SetSessionID(m_iSessionID);
		m_pSheet->SetEventCallback();
	} 
}


//用户签出
void CIVSUsrMgrPage::OnBnClickedButtonLogout()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_Logout(m_iSessionID);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_LOGOUT_OK), GET_STRING_PARAM(IDS_LOGOUT_FAIL));

	if (IVS_SUCCEED == iRet)
	{
		m_pSheet->SetSessionID(SDK_SESSIONID_INIT);
	} 
}


//修改登录密码
void CIVSUsrMgrPage::OnBnClickedButtonChangePwd()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
    //当新密码和确认新密码相同时，才能修改密码
	if (m_strNewPwd == m_strConfirmPwd)
	{
		IVS_INT32 iRet = IVS_SDK_ChangePWD(m_iSessionID, m_strOldPwd, m_strNewPwd);

		m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_CHANGEPWD_OK), GET_STRING_PARAM(IDS_CHANGEPWD_FAIL));
	}
	else
	{
		CString strTitle(_T("IVS Demo   "));
		strTitle.Append(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_NEWPWDS_UNEQUAL)));
		m_pSheet->SetWindowText(strTitle);
	}
}


//获取版本号
void CIVSUsrMgrPage::OnBnClickedButtonGetVer()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iVersion = IVS_SDK_GetVersion();

	IVS_INT32 iMainVer = (iVersion  & 0xFF000000)>>24;
	IVS_INT32 iSubVer = (iVersion & 0x00FF0000)>>16;
	IVS_INT32 iModVer = (iVersion & 0x0000FF00)>>8;
	IVS_INT32 iReserveVer = (iVersion & 0x000000FF);
	/*
	m_strVersion.Format(_T("%x"), iVersion);       
	IVS_INT32 iReserveVer = atoi(m_strVersion.Right(2));
	m_strVersion.Delete(m_strVersion.GetLength()-2, 2);
	IVS_INT32 iModVer = atoi(m_strVersion.Right(2));
	m_strVersion.Delete(m_strVersion.GetLength()-2, 2);
	IVS_INT32 iSubVer = atoi(m_strVersion.Right(2));
	m_strVersion.Delete(m_strVersion.GetLength()-2, 2);
	IVS_INT32 iMainVer = atoi(m_strVersion);
	*/
	m_strVersion.Format(_T(" %d.%d.%d.%d"), iMainVer, iSubVer, iModVer, iReserveVer);
	CString strTitle(_T("IVS Demo   "));
	strTitle.Append(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_VERSION_SN)));
	strTitle.Append(m_strVersion);
	m_pSheet->SetWindowText(strTitle);

	UpdateData(FALSE);
}
