#pragma once
#include "afxwin.h"
#include "IVS_SDK.h"
#include "afxcmn.h"
#include <list>


// CIVSRealPlayPage dialog
class CIVSSDKSheet;
class CIVSRealPlayPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSRealPlayPage)

public:
	CIVSRealPlayPage();
	virtual ~CIVSRealPlayPage();

// Dialog Data
	enum { IDD = IDD_DLG_REALPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;

	//设备列表
	std::list<IVS_CAMERA_BRIEF_INFO*> m_DeviceList;

	//预置位列表
	std::list<IVS_PTZ_PRESET*> m_PresetList;

	//摄像机编码、预置位名称、预置位索引号
	CString m_strCameraCode;
	CString m_strPresetName;
	UINT m_uiPresetIndex;

	//设备、预置位目录树
	CTreeCtrl m_treeDevice;
	CTreeCtrl m_treePreset;
	HTREEITEM m_hItemDevice;
	HTREEITEM m_hItemPreset;
	CImageList m_imageList;

	HGLOBAL m_hMem;
	//协议类型下拉选框
	CComboBox m_ComboProtocol;

	/*GroupBox & StaticLabel & Button*/
	CStatic m_GrpRealplay;
	CStatic m_GrpPtzCtrl;
	CStatic m_GrpPresetCtrl;
	CStatic m_LabChosenDev;
	CStatic m_LabPresetName;
	CStatic m_LabPresetIndex;
	CButton m_BtnStartRealplay;
	CButton m_BtnStopRealplay;
	CButton m_BtnZoomIn;
	CButton m_BtnZoomOut;
	CButton m_BtnPtzLock;
	CButton m_BtnPtzUnlock;
	CButton m_BtnNewPreset;
	CButton m_BtnDelPreset;
	CButton m_checkIsDirect;
	CButton m_checkIsMulticast;

public:
	virtual BOOL OnInitDialog();

	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);

	//开始实况
	afx_msg void OnBnClickedButtonPlay();
	//停止实况
	afx_msg void OnBnClickedButtonStop();

	//清空设备列表
	void ClearDeviceList();
	//显示设备列表
	void ShowDeviceList();
	//更新设备列表
	afx_msg LRESULT OnUpdateDeviceList(WPARAM wParam, LPARAM lParam);
	//单击设备列表目录树节点
	afx_msg void OnTvnSelchangedTreeDevice(NMHDR *pNMHDR, LRESULT *pResult);
	//双击设备列表目录树节点
	afx_msg void OnNMDblclkTreeDevice(NMHDR *pNMHDR, LRESULT *pResult);
	//拷贝设备编码到剪切板
	void CopyDevCodeToClipBoard(IVS_CHAR* pszDevCode, IVS_CHAR* pszDevName);

	//云台控制
	void PtzControl(const IVS_INT32 iCtrlCode,const IVS_CHAR* pcPara1,const IVS_CHAR* pcPara2);
	afx_msg void OnBnClickedButtonPtzStop();//停止云台动作
	afx_msg void OnBnClickedButtonUpLeft();//左上
	afx_msg void OnBnClickedButtonUp();//上
	afx_msg void OnBnClickedButtonUpRight();//右上
	afx_msg void OnBnClickedButtonLeft();//左
	afx_msg void OnBnClickedButtonRight();//右
	afx_msg void OnBnClickedButtonDownLeft();//左下
	afx_msg void OnBnClickedButtonDown();//下
	afx_msg void OnBnClickedButtonDownRight();//右下
	afx_msg void OnBnClickedButtonZoomIn();//放大
	afx_msg void OnBnClickedButtonZoomOut();//缩小
	afx_msg void OnBnClickedButtonPtzLock();//云台加锁
	afx_msg void OnBnClickedButtonPtzUnlock();//云台解锁

	//清空预置位列表
	void ClearPresetList();
	//显示预置位列表
	void ShowPresetList();
	//更新预置位列表
	afx_msg LRESULT OnUpdatePresetList(WPARAM wParam, LPARAM lParam);
	//单击预置位列表目录树节点
	afx_msg void OnTvnSelchangedTreePreset(NMHDR *pNMHDR, LRESULT *pResult);
	//双击预置位列表目录树节点
	afx_msg void OnNMDblclkTreePreset(NMHDR *pNMHDR, LRESULT *pResult);
	//增加预置位
	afx_msg void OnBnClickedButtonAddPreset();
	//删除预置位
	afx_msg void OnBnClickedButtonDeletePreset();
	CButton m_BtnPtzUp;
	CButton m_BtnPtzDown;
	CButton m_BtnPtzLeft;
	CButton m_BtnPtzRight;
	CButton m_BtnPtzUpLeft;
	CButton m_BtnPtzUpRight;
	CButton m_BtnPtzDownLeft;
	CButton m_BtnPtzDownRight;
	CButton m_BtnPtzStop;
};
