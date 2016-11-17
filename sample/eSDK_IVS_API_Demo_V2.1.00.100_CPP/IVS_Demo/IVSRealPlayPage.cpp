// IVSRealPlayPage.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include "afxdialogex.h"
#include "IVS_Demo.h"
#include "IVSRealPlayPage.h"
#include "IVSSDKSheet.h"
#include "Language.h"


#define  PTZ_CONTROL_PARA_LEN 2
const IVS_CHAR PTZ_CONTROL_MODE[PTZ_CONTROL_PARA_LEN]  = "2";//连续模式
const IVS_CHAR PTZ_CONTROL_STEP[PTZ_CONTROL_PARA_LEN] = "3";//速度3 [1-10]
const IVS_UINT32 WIDTH = 14;//关联图像宽度
const IVS_UINT32 HEIGHT = 14;//关联图像高度
const IVS_UINT32 IMAGE_SIZE = 6;//关联图像个数
// CIVSRealPlayPage dialog

IMPLEMENT_DYNAMIC(CIVSRealPlayPage, CPropertyPage)

CIVSRealPlayPage::CIVSRealPlayPage()
	: CPropertyPage(CIVSRealPlayPage::IDD)
	, m_strCameraCode(_T(""))
	, m_strPresetName(_T(""))
	, m_uiPresetIndex(0)
	,m_pSheet(NULL)
	,m_hMem(NULL)
{
	m_DeviceList.clear();
	m_PresetList.clear();
}

CIVSRealPlayPage::~CIVSRealPlayPage()
{
	m_imageList.Detach();
	ClearDeviceList();//清空设备列表
	ClearPresetList();//清空预置位列表
	GlobalFree(m_hMem);
}

void CIVSRealPlayPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEVICE, m_treeDevice);
	DDX_Text(pDX, IDC_EDIT_CAMERA_CODE, m_strCameraCode);
	DDX_Control(pDX, IDC_TREE_PRESET, m_treePreset);
	DDX_Text(pDX, IDC_EDIT_PRESETNAME, m_strPresetName);
	DDX_Text(pDX, IDC_EDIT_PRESETINDEX, m_uiPresetIndex);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL_TYPE, m_ComboProtocol);
	DDX_Control(pDX, IDC_CHECK_DIRECT_FIRST, m_checkIsDirect);
	DDX_Control(pDX, IDC_CHECK_MULTI_CAST, m_checkIsMulticast);
	DDX_Control(pDX, IDC_STATIC_LAB_REALPLAY, m_GrpRealplay);
	DDX_Control(pDX, IDC_STATIC_LAB_PTZCTRL, m_GrpPtzCtrl);
	DDX_Control(pDX, IDC_STATIC_LAB_PRESETCTRL, m_GrpPresetCtrl);
	DDX_Control(pDX, IDC_STATIC_CHOSENDEV, m_LabChosenDev);
	DDX_Control(pDX, IDC_STATIC_PRESETNAME, m_LabPresetName);
	DDX_Control(pDX, IDC_STATIC_PRESETINDEX, m_LabPresetIndex);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_BtnStartRealplay);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_BtnStopRealplay);
	DDX_Control(pDX, IDC_BUTTON_ZOOM_IN, m_BtnZoomIn);
	DDX_Control(pDX, IDC_BUTTON_ZOOM_OUT, m_BtnZoomOut);
	DDX_Control(pDX, IDC_BUTTON_PTZ_LOCK, m_BtnPtzLock);
	DDX_Control(pDX, IDC_BUTTON_PTZ_UNLOCK, m_BtnPtzUnlock);
	DDX_Control(pDX, IDC_BUTTON_ADD_PRESET, m_BtnNewPreset);
	DDX_Control(pDX, IDC_BUTTON_DELETE_PRESET, m_BtnDelPreset);
	DDX_Control(pDX, IDC_BUTTON_UP, m_BtnPtzUp);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_BtnPtzDown);
	DDX_Control(pDX, IDC_BUTTON_LEFT, m_BtnPtzLeft);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_BtnPtzRight);
	DDX_Control(pDX, IDC_BUTTON_UP_LEFT, m_BtnPtzUpLeft);
	DDX_Control(pDX, IDC_BUTTON_UP_RIGHT, m_BtnPtzUpRight);
	DDX_Control(pDX, IDC_BUTTON_DOWN_LEFT, m_BtnPtzDownLeft);
	DDX_Control(pDX, IDC_BUTTON_DOWN_RIGHT, m_BtnPtzDownRight);
	DDX_Control(pDX, IDC_BUTTON_PTZ_STOP, m_BtnPtzStop);
}


BEGIN_MESSAGE_MAP(CIVSRealPlayPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CIVSRealPlayPage::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CIVSRealPlayPage::OnBnClickedButtonStop)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DEVICE, &CIVSRealPlayPage::OnTvnSelchangedTreeDevice)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DEVICE, &CIVSRealPlayPage::OnNMDblclkTreeDevice)
	ON_BN_CLICKED(IDC_BUTTON_PTZ_STOP, &CIVSRealPlayPage::OnBnClickedButtonPtzStop)
	ON_BN_CLICKED(IDC_BUTTON_UP_LEFT, &CIVSRealPlayPage::OnBnClickedButtonUpLeft)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CIVSRealPlayPage::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_UP_RIGHT, &CIVSRealPlayPage::OnBnClickedButtonUpRight)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CIVSRealPlayPage::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CIVSRealPlayPage::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_DOWN_LEFT, &CIVSRealPlayPage::OnBnClickedButtonDownLeft)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CIVSRealPlayPage::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_DOWN_RIGHT, &CIVSRealPlayPage::OnBnClickedButtonDownRight)
	ON_MESSAGE(WM_UPDATEDEVICELIST, &CIVSRealPlayPage::OnUpdateDeviceList)
	ON_MESSAGE(WM_UPDATEPRESETLIST, &CIVSRealPlayPage::OnUpdatePresetList)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PRESET, &CIVSRealPlayPage::OnNMDblclkTreePreset)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_IN, &CIVSRealPlayPage::OnBnClickedButtonZoomIn)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_OUT, &CIVSRealPlayPage::OnBnClickedButtonZoomOut)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PRESET, &CIVSRealPlayPage::OnBnClickedButtonAddPreset)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_PRESET, &CIVSRealPlayPage::OnBnClickedButtonDeletePreset)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRESET, &CIVSRealPlayPage::OnTvnSelchangedTreePreset)
	ON_BN_CLICKED(IDC_BUTTON_PTZ_LOCK, &CIVSRealPlayPage::OnBnClickedButtonPtzLock)
	ON_BN_CLICKED(IDC_BUTTON_PTZ_UNLOCK, &CIVSRealPlayPage::OnBnClickedButtonPtzUnlock)
END_MESSAGE_MAP()


// CIVSRealPlayPage message handlers


static void AddImage(CImageList& imageList, IVS_UINT32 uID)
{
	CBitmap bm;
	bm.LoadBitmap(uID);
	imageList.Add(&bm, RGB(0,0,0));
	bm.DeleteObject();
}


BOOL CIVSRealPlayPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	CHECK_POINTER(m_pSheet, FALSE);
	eSDK::CLanguage* pLang = m_pSheet->m_pLang;
	CHECK_POINTER(pLang, FALSE);

	// 状态图片列表
	m_imageList.Create(WIDTH, HEIGHT, ILC_COLORDDB, IMAGE_SIZE, 1);
	AddImage(m_imageList, IDB_BITMAP_FOLD);
	AddImage(m_imageList, IDB_BITMAP_UNFOLD);
	AddImage(m_imageList, IDB_BITMAP_BALLCAM_ON);
	AddImage(m_imageList, IDB_BITMAP_BALLCAM_OFF);
	AddImage(m_imageList, IDB_BITMAP_FIXEDCAM_ON);
	AddImage(m_imageList, IDB_BITMAP_FIXEDCAM_OFF);

	m_treeDevice.ModifyStyle(0, /*TVS_LINESATROOT|TVS_HASBUTTONS|*/TVS_HASLINES);
	m_treeDevice.SetImageList(&m_imageList, LVSIL_NORMAL);
	m_hItemDevice = m_treeDevice.InsertItem(pLang->GetString(GET_STRING_PARAM(IDS_CAMERA_LIST)));

	m_treePreset.ModifyStyle(0, TVS_LINESATROOT|TVS_HASBUTTONS|TVS_HASLINES);
	m_hItemPreset = m_treePreset.InsertItem(pLang->GetString(GET_STRING_PARAM(IDS_PRESET_LIST)));

	//实况协议类型
	m_ComboProtocol.InsertString(0, _T("UDP"));
	m_ComboProtocol.InsertString(1, _T("TCP"));
	m_ComboProtocol.SetCurSel(1);

	m_checkIsDirect.SetCheck(TRUE);//是否直连
	m_checkIsMulticast.SetCheck(FALSE);//是否组播

	//设置标签、按钮显示名称
	m_GrpRealplay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_REALPLAY)));
	m_GrpPtzCtrl.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PTZCTRL)));
	m_GrpPresetCtrl.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PRESETCTRL)));
	m_LabChosenDev.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_CHOSENDEV)));
	m_LabPresetName.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PRESETNAME)));
	m_LabPresetIndex.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_PRESETINDEX)));
	m_BtnStartRealplay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STARTREALPLAY)));
	m_BtnStopRealplay.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_STOPREALPLAY)));
	m_BtnZoomIn.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_ZOOMIN)));
	m_BtnZoomOut.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_ZOOMOUT)));
	m_BtnPtzLock.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_PTZLOCK)));
	m_BtnPtzUnlock.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_PTZUNLOCK)));
	m_BtnNewPreset.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_NEWPRESET)));
	m_BtnDelPreset.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_BTN_DELPRESET)));
	m_checkIsDirect.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_DIRECTFIRSE)));
	m_checkIsMulticast.SetWindowText(pLang->GetString(GET_STRING_PARAM(IDS_LAB_MULTICAST)));

	//设置云台控制按钮图片
	m_BtnPtzStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STOP)));
	m_BtnPtzUp.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_UP)));
	m_BtnPtzDown.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOWN)));
	m_BtnPtzLeft.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LEFT)));
	m_BtnPtzRight.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RIGHT)));
	m_BtnPtzUpLeft.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_UPLEFT)));
	m_BtnPtzUpRight.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_UPRIGHT)));
	m_BtnPtzDownLeft.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOWNLEFT)));
	m_BtnPtzDownRight.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_DOWNRIGHT)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//设置属性对话框
void CIVSRealPlayPage::SetSheet(CIVSSDKSheet* pSheet)
{
	m_pSheet = pSheet;
}


//开始实况
void CIVSRealPlayPage::OnBnClickedButtonPlay()
{
	 //TODO: Add your control notification handler code here

	IVS_REALPLAY_PARAM RealplayPara = {0};//实况播放参数
	RealplayPara.bDirectFirst = m_checkIsDirect.GetCheck();
	RealplayPara.bMultiCast = m_checkIsMulticast.GetCheck();
	RealplayPara.uiProtocolType = m_ComboProtocol.GetCurSel() + 1;
	RealplayPara.uiStreamType = STREAM_TYPE_MAIN;

	m_pSheet->StopVideoPlay();
	m_pSheet->SetRealPlayPara(m_strCameraCode, &RealplayPara);
	SendMessage(WM_UPDATEPRESETLIST);//更新实况摄像机的预置位
	m_pSheet->StartRealPlay();
}


//停止实况
void CIVSRealPlayPage::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	m_pSheet->StopRealPlay();
}


//清空设备列表
void CIVSRealPlayPage::ClearDeviceList()
{
	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceList.begin();
	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iterEnd = m_DeviceList.end();
	for (; iter != iterEnd; iter++)
	{
		delete[] *iter;
	}

	m_DeviceList.clear();
}


//单击设备列表目录树节点
void CIVSRealPlayPage::OnTvnSelchangedTreeDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treeDevice.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)
	
	IVS_CAMERA_BRIEF_INFO* pCameraInfo = reinterpret_cast<IVS_CAMERA_BRIEF_INFO*>(m_treeDevice.GetItemData(hItem));
	if (NULL != pCameraInfo)
	{
		m_strCameraCode.Format("%s", pCameraInfo->cCode);
		UpdateData(FALSE);//显示被单击设备的编码

		CopyDevCodeToClipBoard(pCameraInfo->cCode, pCameraInfo->cName);
	}
}


//双击设备列表目录树节点
void CIVSRealPlayPage::OnNMDblclkTreeDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treeDevice.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)

	if (m_hItemDevice == hItem)
	{
		PostMessage(WM_UPDATEDEVICELIST);//双击根节点时，更新设备列表
		return;
	}

	IVS_CAMERA_BRIEF_INFO* pCameraInfo = reinterpret_cast<IVS_CAMERA_BRIEF_INFO*>(m_treeDevice.GetItemData(hItem));
	if (NULL != pCameraInfo)
	{
		m_strCameraCode.Format("%s",pCameraInfo->cCode);
		UpdateData(FALSE);

		CopyDevCodeToClipBoard(pCameraInfo->cCode, pCameraInfo->cName);

		OnBnClickedButtonPlay();//播放被双击设备的实况
	}
}


//拷贝设备编码到剪切板
void CIVSRealPlayPage::CopyDevCodeToClipBoard(IVS_CHAR* pszDevCode, IVS_CHAR* pszDevName)
{
	if (OpenClipboard() && EmptyClipboard())
	{
		GlobalFree(m_hMem);
		m_hMem = GlobalAlloc(GHND, IVS_DEV_CODE_LEN * sizeof(IVS_CHAR));
		char* pBuffer = (char*)GlobalLock(m_hMem);
		memcpy(pBuffer, pszDevCode, strlen(pszDevCode));
		GlobalUnlock(m_hMem);
		SetClipboardData(CF_TEXT, m_hMem);
		CloseClipboard();

		CString strTitle(_T("IVS Demo   "));
		strTitle.Append(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_CAMERACODE_COPIED)));
		m_pSheet->SetWindowText(strTitle);
	}
}


//更新设备列表
afx_msg LRESULT CIVSRealPlayPage::OnUpdateDeviceList(WPARAM, LPARAM)
{
	IVS_UINT32 uiReqNum = 1000;//查询数量
	IVS_UINT32 uiReqBufSize = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) + (uiReqNum-1) * sizeof(IVS_CAMERA_BRIEF_INFO);//所需内存大小
	IVS_CAMERA_BRIEF_INFO_LIST* pCameraInfoList = reinterpret_cast<IVS_CAMERA_BRIEF_INFO_LIST*>(new IVS_CHAR[uiReqBufSize]);//分配内存
	CHECK_POINTER(pCameraInfoList, 0);
	memset(pCameraInfoList,0,uiReqBufSize);
	IVS_INDEX_RANGE IndexRange = {1, uiReqNum};//索引范围

	IVS_INT32 iRet = IVS_SDK_GetDeviceList(m_pSheet->GetSessionID(), DEVICE_TYPE_CAMERA, &IndexRange, pCameraInfoList, uiReqBufSize);//获取设备列表

	if (IVS_SUCCEED == iRet)
	{
		//获取设备列表成功，保存
		IVS_UINT32 uiNum = (pCameraInfoList->stIndexRange.uiToIndex - pCameraInfoList->stIndexRange.uiFromIndex) + 1;
		uiNum = (uiNum > pCameraInfoList->uiTotal) ? pCameraInfoList->uiTotal : uiNum;//实际返回的设备个数

		ClearDeviceList();
		for (IVS_UINT32 i=0; i<uiNum; i++)
		{
			IVS_CAMERA_BRIEF_INFO *pDevInfo = new IVS_CAMERA_BRIEF_INFO;
			CHECK_POINTER(pDevInfo, 0);
			memset(pDevInfo,0,sizeof(IVS_CAMERA_BRIEF_INFO));
			memcpy(pDevInfo, &(pCameraInfoList->stCameraBriefInfo[i]), sizeof(IVS_CAMERA_BRIEF_INFO));
			m_DeviceList.push_back(pDevInfo);
		}
	}

	delete[] pCameraInfoList;//释放内存
	pCameraInfoList = NULL;

	ShowDeviceList();//显示设备列表
	CHECK_POINTER(m_pSheet, 0);
	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_GETDEVLIST_OK), GET_STRING_PARAM(IDS_GETDEVLISR_FAIL));
	return 0;
}


//显示设备列表
void CIVSRealPlayPage::ShowDeviceList()
{
	m_treeDevice.DeleteAllItems();//删除所有节点
	m_hItemDevice = m_treeDevice.InsertItem(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_CAMERA_LIST)));//添加根节点

	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceList.begin();
	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iterEnd = m_DeviceList.end();
	for (; iter != iterEnd; iter++)
	{
		TVINSERTSTRUCT tvStruct;
		tvStruct.hParent = m_hItemDevice;
		tvStruct.hInsertAfter = NULL;
		tvStruct.item.mask = TVIF_TEXT|TVIF_PARAM;

		CString strTmp;
		strTmp.Format("%-6s", (*iter)->cName);
		
		switch ((*iter)->uiStatus)
		{
		case DEV_STATUS_ONLINE:
			strTmp += m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_ONLINE));
			break;
		case DEV_STATUS_OFFLINE:
			strTmp += m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_OFFLINE));
			break;
		case DEV_STATUS_SLEEP:
			strTmp += m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_SLEEP));
			break;
		default:
			break;
		}

		IVS_UINT32 uImageID = -1;
		if ((CAMERA_DOME == (*iter)->uiType) || (CAMERA_FIX_DOME == (*iter)->uiType))
		{
			uImageID = (DEV_STATUS_ONLINE == (*iter)->uiStatus) ? 2 : 3;//球机
		} 
		else
		{
			uImageID = (DEV_STATUS_ONLINE == (*iter)->uiStatus) ? 4 : 5;//枪机
		}

		tvStruct.item.pszText = strTmp.GetBuffer();
		strTmp.ReleaseBuffer();
		HTREEITEM hItemSub = m_treeDevice.InsertItem(&tvStruct);//添加子节点
		m_treeDevice.SetItemData(hItemSub, reinterpret_cast<DWORD_PTR>(*iter));//绑定数据到子节点
		m_treeDevice.SetItemImage(hItemSub, uImageID, uImageID);
	}

	m_treeDevice.SetItemImage(m_hItemDevice, 1, 1);//UNFOLD
	m_treeDevice.Expand(m_hItemDevice, TVE_EXPAND);//展开目录树
}


//清空预置位列表
void CIVSRealPlayPage::ClearPresetList()
{
	std::list<IVS_PTZ_PRESET*>::iterator iter = m_PresetList.begin();
	std::list<IVS_PTZ_PRESET*>::iterator iterEnd = m_PresetList.end();
	for (; iter != iterEnd; iter++)
	{
		delete[] (*iter);
		*iter = NULL;
	}

	m_PresetList.clear();
}


//单击预置位列表目录树节点
void CIVSRealPlayPage::OnTvnSelchangedTreePreset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treePreset.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)

	IVS_PTZ_PRESET* pPresetInfo = reinterpret_cast<IVS_PTZ_PRESET*>(m_treePreset.GetItemData(hItem));
	if (NULL != pPresetInfo)
	{
		m_uiPresetIndex = pPresetInfo->uiPresetIndex;
		UpdateData(FALSE);//显示被选中预置位的索引号
	}
}


//双击预置位列表目录树节点
void CIVSRealPlayPage::OnNMDblclkTreePreset(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_treePreset.GetSelectedItem();
	CHECK_POINTER_VOID(hItem)

	if (m_hItemPreset == hItem)
	{
		PostMessage(WM_UPDATEPRESETLIST);//双击根节点时，更新预置位列表
		return;
	}

	IVS_PTZ_PRESET* pPresetInfo =reinterpret_cast<IVS_PTZ_PRESET*>(m_treePreset.GetItemData(hItem));
	if (NULL != pPresetInfo)
	{
		CString strPresteIndex;
		strPresteIndex.Format("%d", pPresetInfo->uiPresetIndex);

		PtzControl(PTZ_PREFAB_BIT_RUN, strPresteIndex, "");//双击子节点时调用该节点的预置位
	}
}


//更新预置位列表
afx_msg LRESULT CIVSRealPlayPage::OnUpdatePresetList(WPARAM wParam, LPARAM lParam)
{
	IVS_UINT32 uiPresetNum;
	IVS_PTZ_PRESET PresetList[IVS_MAX_PRESET_NUM] = {0};
	IVS_INT32 iRet = IVS_SDK_GetPTZPresetList(m_pSheet->GetSessionID(), m_strCameraCode, PresetList, sizeof(PresetList), &uiPresetNum);//获取预置位列表
	
	if (IVS_SUCCEED == iRet)
	{
		//获取预置位列表成功，保存
		ClearPresetList();
		for (IVS_UINT32 i=0; i<uiPresetNum; i++)
		{
			IVS_PTZ_PRESET *pPresetInfo = new IVS_PTZ_PRESET;
			CHECK_POINTER(pPresetInfo, 0);
			memcpy(pPresetInfo, &PresetList[i], sizeof(IVS_PTZ_PRESET));
			m_PresetList.push_back(pPresetInfo);
		}
	}

	ShowPresetList();//显示预置位列表
	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_GETPRESET_OK), GET_STRING_PARAM(IDS_GETPRESET_FAIL));
	return 0;
}


//预置位排序规则，按索引号升序
static bool AscendOrder(const IVS_PTZ_PRESET* pPresetA, const IVS_PTZ_PRESET* pPresetB)
{
	return pPresetA->uiPresetIndex < pPresetB->uiPresetIndex;
}


//显示预置位列表
void CIVSRealPlayPage::ShowPresetList()
{
	m_treePreset.DeleteAllItems();//删除所有节点
	m_hItemPreset = m_treePreset.InsertItem(m_pSheet->m_pLang->GetString(GET_STRING_PARAM(IDS_PRESET_LIST)));//添加根节点
	HTREEITEM hItemParent = m_hItemPreset;

	TVINSERTSTRUCT tvStruct;
	tvStruct.hParent = m_hItemPreset;
	tvStruct.hInsertAfter = NULL;
	tvStruct.item.mask = TVIF_TEXT|TVIF_PARAM;

	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iter = m_DeviceList.begin();
	std::list<IVS_CAMERA_BRIEF_INFO*>::iterator iterEnd = m_DeviceList.end();
	for (; iter != iterEnd; iter++)
	{
		if (0 == m_strCameraCode.Compare((*iter)->cCode))
		{
			tvStruct.item.pszText = (*iter)->cName;

			hItemParent = m_treePreset.InsertItem(&tvStruct);
			m_treeDevice.SetItemData(hItemParent, NULL);
			break;
		}
	}

	stable_sort(m_PresetList.begin(), m_PresetList.end(), AscendOrder);//按预置位索引号升序排序

	std::list<IVS_PTZ_PRESET*>::iterator iterPreset = m_PresetList.begin();
	std::list<IVS_PTZ_PRESET*>::iterator iterEndPreset = m_PresetList.end();
	for (; iterPreset != iterEndPreset; iterPreset++)
	{
		CString strTmp;
		strTmp.Format("%3d %s", (*iterPreset)->uiPresetIndex, (*iterPreset)->cPresetName);

		tvStruct.hParent = hItemParent;
		tvStruct.item.pszText = strTmp.GetBuffer();
		strTmp.ReleaseBuffer();
		HTREEITEM hItemSub = m_treePreset.InsertItem(&tvStruct);//添加子节点
		m_treeDevice.SetItemData(hItemSub, (DWORD_PTR)(*iterPreset));//绑定数据
	}

	//展开目录树
	m_treePreset.Expand(hItemParent, TVE_EXPAND);
	m_treePreset.Expand(m_hItemPreset, TVE_EXPAND);
}


//云台控制
void CIVSRealPlayPage::PtzControl(const IVS_INT32 iCtrlCode,const IVS_CHAR* pcPara1, const IVS_CHAR* pcPara2)
{
	IVS_UINT32 uiLockStatus = 0;
	IVS_INT32 iRet = IVS_SDK_PtzControl(m_pSheet->GetSessionID(), m_strCameraCode, iCtrlCode, pcPara1, pcPara2, &uiLockStatus);

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_PTZCTRL_OK), GET_STRING_PARAM(IDS_PTZCTRL_FAIL));
}


void CIVSRealPlayPage::OnBnClickedButtonPtzStop()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_STOP, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//停止云台动作
}


void CIVSRealPlayPage::OnBnClickedButtonUpLeft()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_UP_LEFT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//左上
}


void CIVSRealPlayPage::OnBnClickedButtonUp()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_UP, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//上
}


void CIVSRealPlayPage::OnBnClickedButtonUpRight()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_UP_RIGHT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//右上
}


void CIVSRealPlayPage::OnBnClickedButtonLeft()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_LEFT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//左
}


void CIVSRealPlayPage::OnBnClickedButtonRight()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_RIGHT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//右
}


void CIVSRealPlayPage::OnBnClickedButtonDownLeft()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_DOWN_LEFT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//左下
}


void CIVSRealPlayPage::OnBnClickedButtonDown()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_DOWN, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//下
}


void CIVSRealPlayPage::OnBnClickedButtonDownRight()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_DOWN_RIGHT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//右下
}


void CIVSRealPlayPage::OnBnClickedButtonZoomIn()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_LENS_ZOOM_IN, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//放大
}


void CIVSRealPlayPage::OnBnClickedButtonZoomOut()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_LENS_ZOOM_OUT, PTZ_CONTROL_MODE, PTZ_CONTROL_STEP);//缩小
}


void CIVSRealPlayPage::OnBnClickedButtonPtzLock()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_LOCK, "", "");//云台加锁
}


void CIVSRealPlayPage::OnBnClickedButtonPtzUnlock()
{
	// TODO: Add your control notification handler code here
	PtzControl(PTZ_UNLOCK, "", "");//云台解锁
}


//增加预置位
void CIVSRealPlayPage::OnBnClickedButtonAddPreset()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	IVS_UINT32 uiIndex = 0;

	IVS_INT32 iRet = IVS_SDK_AddPTZPreset(m_pSheet->GetSessionID(), m_strCameraCode, m_strPresetName, &uiIndex);

	if (IVS_SUCCEED == iRet)
	{
		SendMessage(WM_UPDATEPRESETLIST);//添加成功后更新预置位列表
	} 

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_ADDPRESET_OK), GET_STRING_PARAM(IDS_ADDPRESET_FAIL));
}


//删除预置位
void CIVSRealPlayPage::OnBnClickedButtonDeletePreset()
{
	// TODO: Add your control notification handler code here
	IVS_INT32 iRet = IVS_SDK_DelPTZPreset(m_pSheet->GetSessionID(), m_strCameraCode, m_uiPresetIndex);

	if (IVS_SUCCEED == iRet)
	{
		SendMessage(WM_UPDATEPRESETLIST);//删除成功后更新预置位列表
	} 

	m_pSheet->ShowTitleMsg(iRet, GET_STRING_PARAM(IDS_DELPRESET_OK), GET_STRING_PARAM(IDS_DELPRESET_FAIL));
}
