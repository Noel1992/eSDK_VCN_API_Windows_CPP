// IVSPlayBackPage.cpp : implementation file
//

#include "stdafx.h"
#include "IVS_Demo.h"
#include "IVSPlayBackPage.h"
#include "afxdialogex.h"
#include "IVSSDKSheet.h"
#include "Language.h"


// CIVSPlayBackPage dialog

IMPLEMENT_DYNAMIC(CIVSPlayBackPage, CPropertyPage)

CIVSPlayBackPage::CIVSPlayBackPage()
	: CPropertyPage(CIVSPlayBackPage::IDD)
	, m_strStartTime(_T("20130801000000"))
	, m_strEndTime(_T("20131201000000"))
	, m_strPlayDuration(_T(""))
	, m_strLocalRecPath(_T("D:\\Download\\H12P-235_001.wav"))
	, m_iRecordLength(300)
	, m_strSavePath(_T("D:\\Download"))
	, m_strPlatRecDevCode(_T(""))
	, m_strPURecDevCode(_T(""))
	, m_strPBNVRCode(_T(""))
{
	m_pSheet = NULL;
	m_strCameraCode = _T("");
	m_hItemRecord = NULL;
	m_RecordList.clear();
	m_DeviceList.clear();
	m_RecordMap.clear();
	m_DeviceMap.clear();
	m_DeviceRecordMap.clear();
	memset(&m_PlaybackParam, 0, sizeof(IVS_PLAYBACK_PARAM));
	m_PlaybackParam.fSpeed = 1;//1倍速
	m_PlaybackParam.uiProtocolType = 1;//UDP
}

CIVSPlayBackPage::~CIVSPlayBackPage()
{
	ClearList();
	m_RecordList.clear();
	m_DeviceList.clear();
	m_RecordMap.clear();
	m_DeviceMap.clear();
}

void CIVSPlayBackPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RECORD_LIST, m_treeRecord);
	DDX_Control(pDX, IDC_COMBO_RECORD_METHOD, m_ComboRecordMethod);
	DDX_Text(pDX, IDC_EDIT_START_TIME, m_strStartTime);
	DDX_Text(pDX, IDC_EDIT_END_TIME, m_strEndTime);
	DDX_Text(pDX, IDC_EDIT_SHOW_DURATION, m_strPlayDuration);
	DDX_Text(pDX, IDC_EDIT_LOCAL_RECORD_PATH, m_strLocalRecPath);
	DDX_Text(pDX, IDC_EDIT_RECORD_LENGTH, m_iRecordLength);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strSavePath);
	DDX_Text(pDX, IDC_EDIT_PLATFORM_RECORD_DEV, m_strPlatRecDevCode);
	DDX_Text(pDX, IDC_EDIT_PU_RECORD_DEV, m_strPURecDevCode);
	DDX_Control(pDX, IDC_STATIC_LAB_SEARCHREC, m_GrpSearchRec);
	DDX_Control(pDX, IDC_STATIC_LAB_PLAYBACK, m_GrpPlayback);
	DDX_Control(pDX, IDC_STATIC_LAB_RECORD, m_GrpRecord);
	DDX_Control(pDX, IDC_STATIC_START_TIME, m_LabStartTime);
	DDX_Control(pDX, IDC_STATIC_END_TIME, m_LabEndTime);
	DDX_Control(pDX, IDC_STATIC_LOCAL_PATH, m_LabLocalRecFile);
	DDX_Control(pDX, IDC_STATIC_LAB_DEVCODE1, m_LabPlatDevCode);
	DDX_Control(pDX, IDC_STATIC_LAB_DEVCODE2, m_LabPUDevCode);
	DDX_Control(pDX, IDC_STATIC_LAB_RECLENGTH, m_LabRecLength);
	DDX_Control(pDX, IDC_STATIC_SAVEPATH1, m_LabSavePath);
	DDX_Control(pDX, IDC_BUTTON_GET_RECORD_LIST, m_BtnSearchRec);
	DDX_Control(pDX, IDC_BUTTON_STOP_PLATFORM_PLAY_BACK, m_BtnStopPlatPlay);
	DDX_Control(pDX, IDC_BUTTON_STOP_PU_PLAY_BACK, m_BtnStopPUPlay);
	DDX_Control(pDX, IDC_BUTTON_START_LOCAL_PLAY_BACK, m_BtnStartLocalPlay);
	DDX_Control(pDX, IDC_BUTTON_STOP_LOCAL_PLAY_BACK, m_BtnStopLocalPlay);
	DDX_Control(pDX, IDC_BUTTON_PLAY_BACK_PAUSE, m_BtnPalyPause);
	DDX_Control(pDX, IDC_BUTTON_PLAY_BACK_RESUME, m_BtnPlayResume);
	DDX_Control(pDX, IDC_BUTTON_FAST_PLAY_BACK, m_BtnFastPlay);
	DDX_Control(pDX, IDC_BUTTON_REWIND_PLAY, m_BtnRewindPlay);
	DDX_Control(pDX, IDC_BUTTON_FRAME_PLAY, m_BtnFrameStep);
	DDX_Control(pDX, IDC_BUTTON_START_PLATFORM_RECORD, m_BtnStartPlatRec);
	DDX_Control(pDX, IDC_BUTTON_STOP_PLATFORM_RECORD, m_BtnStopPlatRec);
	DDX_Control(pDX, IDC_BUTTON_START_PU_RECORD, m_BtnStartPuRec);
	DDX_Control(pDX, IDC_BUTTON_STOP_PU_RECORD, m_BtnStopPURec);
	DDX_Control(pDX, IDC_BUTTON_START_LOCAL_RECORD, m_BtnStartLocalRec);
	DDX_Control(pDX, IDC_BUTTON_STOP_LOCAL_RECORD, m_BtnStopLocalRec);
	DDX_Control(pDX, IDC_BUTTON_START_PLATFORM_PLAY_BACK, m_BtnStartPlatPlay);
	DDX_Control(pDX, IDC_BUTTON_START_PU_PLAY_BACK, m_BtnStartPUPlay);
}


BEGIN_MESSAGE_MAP(CIVSPlayBackPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GET_RECORD_LIST, &CIVSPlayBackPage::OnBnClickedButtonGetRecordList)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_RECORD_LIST, &CIVSPlayBackPage::OnNMDblclkTreeRecordList)
	ON_MESSAGE(WM_UPDATERECORDLIST, &CIVSPlayBackPage::OnUpdateRecordList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RECORD_LIST, &CIVSPlayBackPage::OnTvnSelchangedTreeRecordList)
	ON_BN_CLICKED(IDC_BUTTON_START_LOCAL_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStartLocalPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_STOP_LOCAL_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStopLocalPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_START_LOCAL_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStartLocalRecord)
	ON_BN_CLICKED(IDC_BUTTON_STOP_LOCAL_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStopLocalRecord)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PLATFORM_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStopPlatformPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PU_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStopPuPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_START_PLATFORM_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStartPlatformRecord)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PLATFORM_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStopPlatformRecord)
	ON_BN_CLICKED(IDC_BUTTON_START_PU_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStartPuRecord)
	ON_BN_CLICKED(IDC_BUTTON_STOP_PU_RECORD, &CIVSPlayBackPage::OnBnClickedButtonStopPuRecord)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_BACK_PAUSE, &CIVSPlayBackPage::OnBnClickedButtonPlayBackPause)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_BACK_RESUME, &CIVSPlayBackPage::OnBnClickedButtonPlayBackResume)
	ON_BN_CLICKED(IDC_BUTTON_FAST_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonFastPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_REWIND_PLAY, &CIVSPlayBackPage::OnBnClickedButtonRewindPlay)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_PLAY, &CIVSPlayBackPage::OnBnClickedButtonFramePlay)
	ON_BN_CLICKED(IDC_BUTTON_START_PLATFORM_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStartPlatformPlayBack)
	ON_BN_CLICKED(IDC_BUTTON_START_PU_PLAY_BACK, &CIVSPlayBackPage::OnBnClickedButtonStartPuPlayBack)
END_MESSAGE_MAP()


// CIVSPlayBackPage message handlers


BOOL CIVSPlayBackPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	eSDK::CLanguage* pLang = m_pSheet->m_pLang;

	m_treeRecord.ModifyStyle(0, TVS_LINESATROOT|TVS_HASBUTTONS|TVS_HASLINES);
	m_hItemRecord = m_treeRecord.InsertItem(pLang->GetString(GET_STRING_PARAM(IDS_RECORD_LIST)));

	//录像方式
	m_ComboRecordMethod.InsertString(0, pLang->GetString(GET_STRING_PARAM(IDS_PLATFORM)));//添加第0个值
	m_ComboRecordMethod.InsertString(1, pLang->GetString(GET_STRING_PARAM(IDS_PU)));//添加第1个值
	m_ComboRecordMethod.SetCurSel(0);

	//设置标签、按钮显示名称
	m_GrpSearchRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SEARCHRECORD)));
	m_GrpPlayback.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PLAYBACK)));
	m_GrpRecord.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_RECORDVIDEO)));
	m_LabStartTime.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_STARTTIME)));
	m_LabEndTime.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_ENDTIME)));
	m_LabLocalRecFile.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_LOCALRECORD)));
	m_LabPlatDevCode.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabPUDevCode.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DEVCODE)));
	m_LabRecLength.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_RECLENGTH)));
	m_LabSavePath.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_SAVEPATH)));
	m_BtnSearchRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_SEARCHRECORD)));
	m_BtnStartPlatPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPLATPLAYBACK)));
	m_BtnStopPlatPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPLATPLAYBACK)));
	m_BtnStartPUPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPUPLAYBACK)));
	m_BtnStopPUPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPUPLAYBACK)));
	m_BtnStartLocalPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTLOCALPLAYBACK)));
	m_BtnStopLocalPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPLOCALPLAYBACK)));
	m_BtnPalyPause.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_PLAYBACKPAUSE)));
	m_BtnPlayResume.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_PLAYRESUME)));
	m_BtnFastPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_FASTPLAY)));
	m_BtnRewindPlay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_REWINDPLAY)));
	m_BtnFrameStep.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_FRAMESTEP)));
	m_BtnStartPlatRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPLATREC)));
	m_BtnStopPlatRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPLATREC)));
	m_BtnStartPuRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTPUREC)));
	m_BtnStopPURec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPPUREC)));
	m_BtnStartLocalRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTLOCALREC)));
	m_BtnStopLocalRec.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPLOCALREC)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//设置属性对话框
void CIVSPlayBackPage::SetSheet(CIVSSDKSheet* pSheet)
{
	m_pSheet = pSheet;
}


//清空列表
void CIVSPlayBackPage::ClearList()
{
	std::map<IVS_CAMERA_BRIEF_INFO*, std::list<IVS_RECORD_INFO*>>::iterator iterxEnd = m_DeviceRecordMap.end();
	for (std::map<IVS_CAMERA_BRIEF_INFO*, std::list<IVS_RECORD_INFO*>>::iterator iterx = m_DeviceRecordMap.begin(); iterx != iterxEnd; iterx++)
	{
		delete[] iterx->first;
		std::list<IVS_RECORD_INFO*>::iterator iteryEnd = (iterx->second).end();
		for (std::list<IVS_RECORD_INFO*>::iterator itery = (iterx->second).begin(); itery != iteryEnd; itery++)
		{
			delete[] (*itery);
		}
	}

	m_DeviceRecordMap.clear();
}


//双击录像列表目录树节点
void CIVSPlayBackPage::OnNMDblclkTreeRecordList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treeRecord.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)

	if (m_hItemRecord == hItem)
	{
		PostMessage(WM_UPDATERECORDLIST);//双击根节点时，更新录像列表
		return;
	}

	std::map<HTREEITEM, HTREEITEM>::iterator iterx = m_RecordMap.find(hItem);
	if (m_RecordMap.end() != iterx)
	{
		std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*>::iterator itery = m_DeviceMap.find(iterx->second);
		if (m_DeviceMap.end() != itery)
		{
			m_strCameraCode.Format(_T("%s"), itery->second->cCode);//获取被双击录像所属摄像机的编码
		}

		IVS_RECORD_INFO* pRecordInfo = (IVS_RECORD_INFO*)m_treeRecord.GetItemData(hItem);
		if (NULL != pRecordInfo)
		{
			memcpy(&(m_PlaybackParam.stTimeSpan), &(pRecordInfo->stTime), sizeof(IVS_TIME_SPAN));	//获取被双击录像的起止时间
			memcpy(m_PlaybackParam.cNVRCode, pRecordInfo->cNvrCode, sizeof(pRecordInfo->cNvrCode));	//获取被双击录像的NVR编码
			StartRemotePlayBack();//播放被双击的录像
		}
	}
}


//单击录像列表目录树节点
void CIVSPlayBackPage::OnTvnSelchangedTreeRecordList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treeRecord.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)

	std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceMap.find(hItem);
	if (m_DeviceMap.end() != iter)
	{
		m_strPlayDuration.Format(_T("%s"), iter->second->cCode);
		UpdateData(FALSE);//显示被单击摄像机的编码
	}

	std::map<HTREEITEM, HTREEITEM>::iterator iterx = m_RecordMap.find(hItem);
	if (m_RecordMap.end() != iterx)
	{
		std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*>::iterator itery = m_DeviceMap.find(iterx->second);
		if (m_DeviceMap.end() != itery)
		{
			m_strCameraCode.Format(_T("%s"), itery->second->cCode);
			m_strPlayDuration.Format(_T("%s"), itery->second->cName);
		}

		IVS_RECORD_INFO* pRecordInfo = (IVS_RECORD_INFO*)m_treeRecord.GetItemData(hItem);
		if (NULL != pRecordInfo)
		{
			CString strDuration;
			strDuration.Format(_T(": %s—%s"), pRecordInfo->stTime.cStart, pRecordInfo->stTime.cEnd);
			m_strPlayDuration.Append(strDuration);
			memcpy(&(m_PlaybackParam.stTimeSpan), &(pRecordInfo->stTime), sizeof(IVS_TIME_SPAN));	//获取被双击录像的起止时间
			memcpy(m_PlaybackParam.cNVRCode, pRecordInfo->cNvrCode, sizeof(pRecordInfo->cNvrCode));	//获取被双击录像的NVR编码
			UpdateData(FALSE);//显示被单击录像的详细信息（格式如 摄像机名称：录像开始时间—录像结束时间）
		}
	}
}


//更新录像列表
afx_msg LRESULT CIVSPlayBackPage::OnUpdateRecordList(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedButtonGetRecordList();
	return 0;
}


//显示录像列表
void CIVSPlayBackPage::ShowRecordList()
{
	m_RecordMap.clear();
	m_DeviceMap.clear();
	m_treeRecord.DeleteAllItems();//删除所有节点
	m_hItemRecord = m_treeRecord.InsertItem(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_RECORD_LIST)));//添加根节点

	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceList.begin();
	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iterEnd = m_DeviceList.end();
	for (; iter != iterEnd; iter++)
	{
		TVINSERTSTRUCT tvStruct;
		tvStruct.hParent = m_hItemRecord;
		tvStruct.hInsertAfter = NULL;
		tvStruct.item.mask = TVIF_TEXT|TVIF_PARAM;

		CString strTmp;
		strTmp.Format("%s", (*iter)->cName);

		tvStruct.item.pszText = strTmp.GetBuffer();
		strTmp.ReleaseBuffer();
		HTREEITEM hItemSub = m_treeRecord.InsertItem(&tvStruct);//添加摄像机子节点，父节点为根节点
		m_DeviceMap[hItemSub] = *iter;
		m_treeRecord.SetItemData(hItemSub, (DWORD_PTR)(*iter));//绑定数据到节点
	}

	std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*>::iterator iterx = m_DeviceMap.begin();
	std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*>::iterator iterxEnd = m_DeviceMap.end();
	for (; iterx != iterxEnd; iterx++)
	{
		std::map<IVS_CAMERA_BRIEF_INFO*, std::list<IVS_RECORD_INFO*>>::iterator itery = m_DeviceRecordMap.find(iterx->second);
		if (m_DeviceRecordMap.end() != itery)
		{
			std::list<IVS_RECORD_INFO*>::iterator iterz = (itery->second).begin();
			std::list<IVS_RECORD_INFO*>::iterator iterzEnd = (itery->second).end();
			for (; iterz != iterzEnd; iterz++)
			{
				TVINSERTSTRUCT tvStruct;
				tvStruct.hParent = iterx->first;
				tvStruct.hInsertAfter = NULL;
				tvStruct.item.mask = TVIF_TEXT|TVIF_PARAM;

				CString strTmp;
				strTmp.Format("%s", (*iterz)->stTime.cStart);

				tvStruct.item.pszText = strTmp.GetBuffer();
				strTmp.ReleaseBuffer();
				HTREEITEM hItemSub = m_treeRecord.InsertItem(&tvStruct);//添加录像子节点，父节点为摄像机节点
				m_RecordMap[hItemSub] = iterx->first;
				m_treeRecord.SetItemData(hItemSub, (DWORD_PTR)(*iterz));//绑定数据到节点
			}
		}
	}

	m_treeRecord.Expand(m_hItemRecord, TVE_EXPAND);
}


//获取录像列表
void CIVSPlayBackPage::OnBnClickedButtonGetRecordList()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ClearList();

	IVS_INT32 iRet = GetDeviceList();//先获取设备列表

	if (IVS_SUCCEED == iRet)
	{
		IVS_TIME_SPAN TimeSpan = {0};//查询时间范围
		strncpy_s(TimeSpan.cStart, m_strStartTime, IVS_TIME_LEN);
		strncpy_s(TimeSpan.cEnd, m_strEndTime, IVS_TIME_LEN);

		IVS_UINT32 uiReqNum = 1000;//查询数量
		IVS_INDEX_RANGE IndexRange = {1, uiReqNum};//索引范围
		IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_INFO_LIST) + (uiReqNum - 1) * sizeof(IVS_RECORD_INFO);//所需内存大小
		IVS_RECORD_INFO_LIST* pRecordList = (IVS_RECORD_INFO_LIST*)(new IVS_CHAR[uiBufSize]);//分配内存
		CHECK_POINTER_VOID(pRecordList);

		IVS_INT32 iRecordMethod = m_ComboRecordMethod.GetCurSel();//录像方式（类型）

		for (std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceList.begin(); iter != m_DeviceList.end(); iter++)
		{
			memset(pRecordList, 0 ,uiBufSize);

			iRet = IVS_SDK_GetRecordList(m_pSheet->GetSessionID(), (*iter)->cCode, iRecordMethod, &TimeSpan, &IndexRange, pRecordList, uiBufSize);//获取录像列表

			if (IVS_SUCCEED == iRet)
			{
				//获取录像列表成功，保存
				IVS_UINT32 uiNum = (pRecordList->stIndexRange.uiToIndex - pRecordList->stIndexRange.uiFromIndex) + 1;
				uiNum = (uiNum > pRecordList->uiTotal) ? pRecordList->uiTotal : uiNum;//实际返回的录像个数

				m_RecordList.clear();
				for (IVS_UINT32 i=0; i<uiNum; i++)
				{
					IVS_RECORD_INFO *pRecordInfo = new IVS_RECORD_INFO;
					if(NULL == pRecordInfo)
					{
						delete[] pRecordList;//释放内存
						pRecordList = NULL;
					}
					memset(pRecordInfo,0,sizeof(IVS_RECORD_INFO));
					memcpy(pRecordInfo, &(pRecordList->stRecordInfo[i]), sizeof(IVS_RECORD_INFO));
					m_RecordList.push_back(pRecordInfo);
				}

				m_DeviceRecordMap[*iter] = m_RecordList;

				m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_GETRECORDLIST_OK), GET_STRING_PARAM(IDS_GETRECORDLIST_FAIL));
			}
		}

		delete[] pRecordList;//释放内存
		pRecordList = NULL;
	}

	ShowRecordList();//显示录像列表
}


//获取设备列表
IVS_INT32 CIVSPlayBackPage::GetDeviceList()
{
	IVS_UINT32 uiReqNum = 1000;//查询数量
	IVS_UINT32 uiRsqBufSize = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) + (uiReqNum-1) * sizeof(IVS_CAMERA_BRIEF_INFO);//所需内存大小
	IVS_CAMERA_BRIEF_INFO_LIST* pCameraInfoList = (IVS_CAMERA_BRIEF_INFO_LIST*)(new IVS_CHAR[uiRsqBufSize]);//分配内存
	CHECK_POINTER(pCameraInfoList, IVS_FAIL);
	memset(pCameraInfoList,0,uiRsqBufSize);
	IVS_INDEX_RANGE IndexRange = {1, uiReqNum};//索引范围

	IVS_INT32 iRet = IVS_SDK_GetDeviceList(m_pSheet->GetSessionID(), DEVICE_TYPE_CAMERA, &IndexRange, pCameraInfoList, uiRsqBufSize);//获取设备列表

	if (IVS_SUCCEED != iRet)
	{
		delete[] pCameraInfoList;
		pCameraInfoList = NULL;
		return iRet;
	}

	IVS_UINT32 uiNum = (pCameraInfoList->stIndexRange.uiToIndex - pCameraInfoList->stIndexRange.uiFromIndex) + 1;
	uiNum = (uiNum > pCameraInfoList->uiTotal) ? pCameraInfoList->uiTotal : uiNum;

	m_DeviceList.clear();
	for (IVS_UINT32 i=0; i<uiNum; i++)
	{
		IVS_CAMERA_BRIEF_INFO *pDevInfo = new IVS_CAMERA_BRIEF_INFO;
		if(NULL == pDevInfo)
		{
			delete[] pCameraInfoList;
			pCameraInfoList = NULL;
			return IVS_FAIL;
		}

		memset(pDevInfo,0,sizeof(IVS_CAMERA_BRIEF_INFO));
		memcpy(pDevInfo, &(pCameraInfoList->stCameraBriefInfo[i]), sizeof(IVS_CAMERA_BRIEF_INFO));
		m_DeviceList.push_back(pDevInfo);
	}

	delete[] pCameraInfoList;
	pCameraInfoList = NULL;
	return iRet;
}


//开始远程（平台、前端）录像回放
void CIVSPlayBackPage::StartRemotePlayBack()
{
	if (RECORD_METHOD_PLATFORM == m_ComboRecordMethod.GetCurSel())
	{
		//平台录像回放
		m_pSheet->StopVideoPlay();
		m_pSheet->SetPlatPlayBackPara(m_strCameraCode, &m_PlaybackParam);
		m_pSheet->StartPlatformPlayBack();
	} 
	else
	{
		//前端录像回放
		m_pSheet->StopVideoPlay();
		m_pSheet->SetPUPlayBackPara(m_strCameraCode, &m_PlaybackParam);
		m_pSheet->StartPUPlayBack();
	}
}


//开始平台录像回放
void CIVSPlayBackPage::OnBnClickedButtonStartPlatformPlayBack()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pSheet->StopVideoPlay();
	m_pSheet->SetPlatPlayBackPara(m_strCameraCode, &m_PlaybackParam);
	m_pSheet->StartPlatformPlayBack();
}


//停止平台录像回放
void CIVSPlayBackPage::OnBnClickedButtonStopPlatformPlayBack()
{
	// TODO: Add your control notification handler code here
	m_pSheet->StopPlatformPlayBack();
}


//开始前端录像回放
void CIVSPlayBackPage::OnBnClickedButtonStartPuPlayBack()
{
	// TODO: Add your control notification handler code here
	m_pSheet->StopVideoPlay();
	m_pSheet->SetPUPlayBackPara(m_strCameraCode, &m_PlaybackParam);
	m_pSheet->StartPUPlayBack();
}


//停止前端录像回放
void CIVSPlayBackPage::OnBnClickedButtonStopPuPlayBack()
{
	// TODO: Add your control notification handler code here
	m_pSheet->StopPUPlayBack();
}


//开始本地录像回放
void CIVSPlayBackPage::OnBnClickedButtonStartLocalPlayBack()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_LOCAL_PLAYBACK_PARAM PlaybackParam = {0};
	PlaybackParam.fSpeed = 1;//本地回放参数，1倍速

	m_pSheet->StopVideoPlay();
	m_pSheet->SetLocalPlayBackPara(m_strLocalRecPath, &PlaybackParam);
	m_pSheet->StartLocalPlayBack();
}


//停止本地录像回放
void CIVSPlayBackPage::OnBnClickedButtonStopLocalPlayBack()
{
	// TODO: Add your control notification handler code here
	m_pSheet->StopLocalPlayBack();
}


//开始本地录像
void CIVSPlayBackPage::OnBnClickedButtonStartLocalRecord()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	IVS_LOCAL_RECORD_PARAM RecordParam = {0};//录像参数
	RecordParam.uiRecordTime = m_iRecordLength;//录像时长
	RecordParam.uiRecordFormat = IVS_FILE_MP4;//保存为MP4格式
	RecordParam.uiSplitterType = RECORD_SPLITE_TIME;//按时间分割
	RecordParam.uiSplitterValue = 5;//每5分钟分割一次
	RecordParam.uiNameRule = RULE_NAME_NUM_TIME;//命名规则
	strncpy_s(RecordParam.cSavePath, m_strSavePath, IVS_FILE_NAME_LEN);//保存路径

	IVS_INT32 iRet = IVS_SDK_StartLocalRecord(m_pSheet->GetSessionID(), &RecordParam, m_pSheet->GetPlayHandle(), "");//绝对路径为空则根据命名规则命名

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTLOCALREC_OK), GET_STRING_PARAM(IDS_STARTLOCALREC_FAIL));
}


//停止本地录像
void CIVSPlayBackPage::OnBnClickedButtonStopLocalRecord()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopLocalRecord(m_pSheet->GetSessionID(), m_pSheet->GetPlayHandle());

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPLOCALREC_OK), GET_STRING_PARAM(IDS_STOPLOCALREC_FAIL));
}


//开始平台录像
void CIVSPlayBackPage::OnBnClickedButtonStartPlatformRecord()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_INT32 iRet = IVS_SDK_StartPlatformRecord(m_pSheet->GetSessionID(), m_strPlatRecDevCode, m_iRecordLength);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTPLATREC_OK), GET_STRING_PARAM(IDS_STARTPLATREC_FAIL));
}


//停止平台录像
void CIVSPlayBackPage::OnBnClickedButtonStopPlatformRecord()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopPlatformRecord(m_pSheet->GetSessionID(), m_strPlatRecDevCode);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPPLATREC_OK), GET_STRING_PARAM(IDS_STOPPLATREC_FAIL));
}


//开始前端录像
void CIVSPlayBackPage::OnBnClickedButtonStartPuRecord()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	IVS_INT32 iRet = IVS_SDK_StartPURecord(m_pSheet->GetSessionID(), m_strPURecDevCode, m_iRecordLength);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STARTPUREC_OK), GET_STRING_PARAM(IDS_STARTPUREC_FAIL));
}


//停止前端录像
void CIVSPlayBackPage::OnBnClickedButtonStopPuRecord()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_StopPURecord(m_pSheet->GetSessionID(), m_strPURecDevCode);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_STOPPUREC_OK), GET_STRING_PARAM(IDS_STOPPUREC_FAIL)); 
}


//暂停回放
void CIVSPlayBackPage::OnBnClickedButtonPlayBackPause()
{
	// TODO: Add your control notification handler code here
	m_pSheet->PlayBackPause();
}


//恢复回放
void CIVSPlayBackPage::OnBnClickedButtonPlayBackResume()
{
	// TODO: Add your control notification handler code here
	m_pSheet->PlayBackResume();
}


//快速播放
void CIVSPlayBackPage::OnBnClickedButtonFastPlayBack()
{
	// TODO: Add your control notification handler code here
	m_pSheet->SetPlayBackSpeed(4.0);//设置回放速度，4倍速
}


//倒退播放
void CIVSPlayBackPage::OnBnClickedButtonRewindPlay()
{
	// TODO: Add your control notification handler code here
	m_pSheet->SetPlayBackSpeed(-1.0);//设置回放速度，-1.0表示1倍速倒退播放
}


//单帧步进
void CIVSPlayBackPage::OnBnClickedButtonFramePlay()
{
	// TODO: Add your control notification handler code here
	m_pSheet->PlayBackFrameStepForward();
}
