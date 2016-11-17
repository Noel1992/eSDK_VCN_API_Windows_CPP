// IVSTalkBackPage.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "Language.h"
#include "IVS_Demo.h"
#include "IVSTalkBackPage.h"
#include "IVSSDKSheet.h"


// CIVSTalkBackPage dialog

IMPLEMENT_DYNAMIC(CIVSTalkBackPage, CPropertyPage)

CIVSTalkBackPage::CIVSTalkBackPage()
	: CPropertyPage(CIVSTalkBackPage::IDD)
	, m_strTalkbackDevice(_T(""))
	, m_strAddDevCode(_T(""))
	, m_strDeleteDevCode(_T(""))
	, m_strBrodcastFile(_T("D:\\Download\\Ludovico Einaudi.mp3"))
{

}

CIVSTalkBackPage::~CIVSTalkBackPage()
{
}

void CIVSTalkBackPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TALKBACK_DEV, m_strTalkbackDevice);
	DDX_Control(pDX, IDC_CHECK_DIRECT, m_CheckDirect);
	DDX_Text(pDX, IDC_EDIT_BRODCAST_DEV, m_strAddDevCode);
	DDX_Text(pDX, IDC_EDIT_BRODCAST_DEV2, m_strDeleteDevCode);
	DDX_Control(pDX, IDC_LIST_BRODCAST_DEV, m_listBrodcast);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, m_ComboProtocol);
	DDX_Text(pDX, IDC_EDIT_BRODCAST_FILE, m_strBrodcastFile);
	DDX_Control(pDX, IDC_COMBO_CYCLE, m_ComboCycle);
	DDX_Control(pDX, IDC_STATIC_TALKBACK, m_GrpTalkback);
	DDX_Control(pDX, IDC_STATIC_BRODCAST_DEV_MGR, m_GrpBrodcastDev);
	DDX_Control(pDX, IDC_STATIC_BRODCAST, m_GrpBrodcast);
	DDX_Control(pDX, IDC_STATIC_TALKBACK_DEVCODE, m_LabDevCode1);
	DDX_Control(pDX, IDC_STATIC_BRODCAST_DEV1, m_LabDevCode2);
	DDX_Control(pDX, IDC_STATIC_BRODCAST_DEV2, m_LabDevCode3);
	DDX_Control(pDX, IDC_BUTTON_START_TALK_BACK, m_BtnStartTalkback);
	DDX_Control(pDX, IDC_BUTTON_STOP_TALK_BACK, m_BtnStopTalkback);
	DDX_Control(pDX, IDC_BUTTON_ADD_BRODCAST_DEVICE, m_BtnAddDevice);
	DDX_Control(pDX, IDC_BUTTON_DELETE_BRODCAST_DEVICE, m_BtnDeleteDevice);
	DDX_Control(pDX, IDC_BUTTON_START_FILE_BRODCAST, m_BtnStartFileBrodcast);
	DDX_Control(pDX, IDC_BUTTON_STOP_FILE_BRODCAST, m_BtnStopFileBrodcast);
	DDX_Control(pDX, IDC_BUTTON_START_REAL_BRODCAST, m_BtnStartRealBrodcast);
	DDX_Control(pDX, IDC_BUTTON_STOP_REAL_BRODCAST, m_BtnStopRealBrodcast);
	DDX_Control(pDX, IDC_STATIC_BRODCAST_FILE, m_LabBrodcastFile);
}


BEGIN_MESSAGE_MAP(CIVSTalkBackPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_START_TALK_BACK, &CIVSTalkBackPage::OnBnClickedButtonStartTalkBack)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TALK_BACK, &CIVSTalkBackPage::OnBnClickedButtonStopTalkBack)
	ON_BN_CLICKED(IDC_BUTTON_ADD_BRODCAST_DEVICE, &CIVSTalkBackPage::OnBnClickedButtonAddBrodcastDevice)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_BRODCAST_DEVICE, &CIVSTalkBackPage::OnBnClickedButtonDeleteBrodcastDevice)
	ON_MESSAGE(WM_UPDATEBRODCASTLIST, &CIVSTalkBackPage::OnUpdateBrodcastDevList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BRODCAST_DEV, &CIVSTalkBackPage::OnNMClickListBrodcastDev)
	ON_BN_CLICKED(IDC_BUTTON_STOP_FILE_BRODCAST, &CIVSTalkBackPage::OnBnClickedButtonStopFileBrodcast)
	ON_BN_CLICKED(IDC_BUTTON_START_FILE_BRODCAST, &CIVSTalkBackPage::OnBnClickedButtonStartFileBrodcast)
	ON_BN_CLICKED(IDC_BUTTON_START_REAL_BRODCAST, &CIVSTalkBackPage::OnBnClickedButtonStartRealBrodcast)
	ON_BN_CLICKED(IDC_BUTTON_STOP_REAL_BRODCAST, &CIVSTalkBackPage::OnBnClickedButtonStopRealBrodcast)
END_MESSAGE_MAP()


// CIVSTalkBackPage message handlers


BOOL CIVSTalkBackPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	eSDK::CLanguage* pLang = m_pSheet->m_pLang;

	//协议类型
	m_ComboProtocol.InsertString(0, _T("TCP"));
	m_ComboProtocol.InsertString(1, _T("UDP"));
	m_ComboProtocol.SetCurSel(1);
	//是否循环
	m_ComboCycle.InsertString(0, pLang->GetString(GET_STRING_PARAM(IDS_BTN_NOCYCLE)));
	m_ComboCycle.InsertString(1, pLang->GetString(GET_STRING_PARAM(IDS_BTN_CYCLE)));
	m_ComboCycle.SetCurSel(1);

	//设置广播设备列表属性
	m_listBrodcast.ModifyStyle(0, LVS_REPORT);
	m_listBrodcast.SetExtendedStyle(m_listBrodcast.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listBrodcast.InsertColumn(0, pLang->GetString(GET_STRING_PARAM(IDS_LAB_BRODCASTDEVLIST)));
	CRect rect;
	m_listBrodcast.GetClientRect(&rect);
	m_listBrodcast.SetColumnWidth(0, rect.Width());

	//设置标签、按钮显示名称
	m_GrpTalkback.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_TALKBACK)));
	m_GrpBrodcastDev.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_BRODCASTDEV)));
	m_GrpBrodcast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_BRODCAST)));
	m_LabDevCode1.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabDevCode2.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabDevCode3.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabBrodcastFile.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_BRODCASTFILE)));
	m_CheckDirect.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DIRECTFIRSE)));
	m_BtnStartTalkback.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTTALKBACK)));
	m_BtnStopTalkback.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPTALKBACK)));
	m_BtnAddDevice.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_ADDDEVICE)));
	m_BtnDeleteDevice.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_DELETEDEVICE)));
	m_BtnStartFileBrodcast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTFILEBRODCAST)));
	m_BtnStopFileBrodcast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPFILEBRODCAST)));
	m_BtnStartRealBrodcast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTREALBRODCAST)));
	m_BtnStopRealBrodcast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPREALBRODCAST)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//设置属性对话框
void CIVSTalkBackPage::SetSheet(CIVSSDKSheet* pSheet)
{
	m_pSheet = pSheet;
}


//开始语音对讲
void CIVSTalkBackPage::OnBnClickedButtonStartTalkBack()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_VOICE_TALKBACK_PARAM TalkbackParam = {0};
	TalkbackParam.uiProtocolType = m_ComboProtocol.GetCurSel();
	TalkbackParam.bDirectFirst = m_CheckDirect.GetCheck();

	IVS_INT32 iRet =  IVS_SDK_StartVoiceTalkback(m_pSheet->GetSessionID(), &TalkbackParam,	m_strTalkbackDevice, &m_ulTalkbackHandle);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTTALKBACK_OK), GET_STRING_PARAM(IDS_STARTTALKBACK_FAIL));
}


//停止语音对讲
void CIVSTalkBackPage::OnBnClickedButtonStopTalkBack()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet =  IVS_SDK_StopVoiceTalkback(m_pSheet->GetSessionID(), m_ulTalkbackHandle);

	if (IVS_SUCCEED == iRet)
	{
		m_ulTalkbackHandle = 0;
	}

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPTALKBACK_OK), GET_STRING_PARAM(IDS_STOPTALKBACK_FAIL));
}


//添加广播设备
void CIVSTalkBackPage::OnBnClickedButtonAddBrodcastDevice()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_INT32 iRet = IVS_SDK_AddBroadcastDevice(m_pSheet->GetSessionID(), m_strAddDevCode);

	if (IVS_SUCCEED == iRet)
	{
		m_BrodcastDevList.push_back(m_strAddDevCode);
		m_strAddDevCode.Empty();
		UpdateData(FALSE);
		PostMessage(WM_UPDATEBRODCASTLIST);
	}

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_ADDBRODCASTDEV_OK), GET_STRING_PARAM(IDS_ADDBRODCASTDEV_FAIL));
}


//删除广播设备
void CIVSTalkBackPage::OnBnClickedButtonDeleteBrodcastDevice()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_INT32 iRet = IVS_SDK_DeleteBroadcastDevice(m_pSheet->GetSessionID(), m_strDeleteDevCode);

	if (IVS_SUCCEED == iRet)
	{
		m_BrodcastDevList.remove(m_strDeleteDevCode);
		m_strDeleteDevCode.Empty();
		UpdateData(FALSE);
		PostMessage(WM_UPDATEBRODCASTLIST);
	}

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_DELBRODCASTDEV_OK), GET_STRING_PARAM(IDS_DELBRODCASTDEV_FAIL));
}


//刷新广播设备列表
afx_msg LRESULT CIVSTalkBackPage::OnUpdateBrodcastDevList(WPARAM wParam, LPARAM lParam)
{
	m_listBrodcast.DeleteAllItems();

	IVS_UINT32 uiCnt = 0;
	std::list<CString>::iterator iter_end = m_BrodcastDevList.end();
	for (std::list<CString>::iterator iter = m_BrodcastDevList.begin(); iter != iter_end; iter++)
	{
		m_listBrodcast.InsertItem(uiCnt, *iter);
		m_listBrodcast.SetItemData(uiCnt, (DWORD_PTR)&(*iter));
		uiCnt++;
	}

	return 0;
}


//选中列表中某个广播设备
void CIVSTalkBackPage::OnNMClickListBrodcastDev(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	// TODO: Add your control notification handler code here
	int iItem = pNMItemActivate->iItem;
	if (-1 == iItem)
	{
		return;
	}

	CString* pszDevCode = (CString*)m_listBrodcast.GetItemData(iItem);
	CHECK_POINTER_VOID(pszDevCode)

	m_strDeleteDevCode = *pszDevCode;
	UpdateData(FALSE);
}


//开始语音广播
void CIVSTalkBackPage::OnBnClickedButtonStartRealBrodcast()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StartRealBroadcast(m_pSheet->GetSessionID());

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTREALBRODCAST_OK), GET_STRING_PARAM(IDS_STARTREALBRODCAST_FAIL));
}

//停止语音广播
void CIVSTalkBackPage::OnBnClickedButtonStopRealBrodcast()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopRealBroadcast(m_pSheet->GetSessionID());

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPREALBRODCAST_OK), GET_STRING_PARAM(IDS_STOPREALBRODCAST_FAIL));
}


//开始文件广播
void CIVSTalkBackPage::OnBnClickedButtonStartFileBrodcast()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StartFileBroadcast(m_pSheet->GetSessionID(), m_strBrodcastFile, m_ComboCycle.GetCurSel());

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTFILEBRODCAST_OK), GET_STRING_PARAM(IDS_STARTFILEBRODCAST_FAIL));
}


//停止文件广播
void CIVSTalkBackPage::OnBnClickedButtonStopFileBrodcast()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopFileBroadcast(m_pSheet->GetSessionID());

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPFILEBRODCAST_OK), GET_STRING_PARAM(IDS_STOPFILEBRODCAST_FAIL));
}
