#pragma once
#include "IVS_SDK.h"
#include "afxcmn.h"
#include <list>
#include <map>
#include "afxwin.h"


// CIVSAlarmPage dialog
class CIVSSDKSheet;
class CIVSAlarmPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSAlarmPage)

public:
	CIVSAlarmPage();
	virtual ~CIVSAlarmPage();

// Dialog Data
	enum { IDD = IDD_DLG_ALARMPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;

	//告警表目录和告警设备树目录
	CListCtrl m_listAlarm;
	CTreeCtrl m_treeAlarmDevice;
	HTREEITEM m_hItemRoot;

	//设备列表
	std::list<IVS_CAMERA_BRIEF_INFO*> m_DeviceList;
	std::map<HTREEITEM, IVS_CAMERA_BRIEF_INFO*> m_DeviceItemMap;//key:树节点 value:设备信息

	//告警级别滑竿
	CSliderCtrl m_SliderMin;
	CSliderCtrl m_SliderMax;
	CString m_strMinLevel;
	CString m_strMaxLevel;

	/*GroupBox & StaticLabel & Button*/
	CButton m_CheckMoveDection;
	CButton m_CheckShield;
	CButton m_CheckNoSignal;
	CButton m_CheckShake;
	CButton m_CheckRecognition;
	CButton m_CheckIntrusion;
	CButton m_CheckByAlarmType;
	CButton m_CheckByAlarmLevel;
	CButton m_CheckSubscribeAll;
	CStatic m_GrpSubscribe;
	CStatic m_GrpAlarmReport;
	CStatic m_LabMinLevel;
	CStatic m_LabMaxLevel;
	CButton m_BtnSubscribe;

public:
	virtual BOOL OnInitDialog();

	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);

	//刷新设备列表
	afx_msg LRESULT OnUpdateDeviceList(WPARAM wParam, LPARAM lParam);
	//清空设备列表
	void ClearDeviceList();
	//显示设备列表
	void ShowDeviceList();
	afx_msg void OnNMDblclkTreeAlarmDevice(NMHDR *pNMHDR, LRESULT *pResult);

	//滑竿滑动，实时显示告警级别
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//告警订阅类型控制
	afx_msg void OnBnClickedCheckByAlarmType();
	afx_msg void OnBnClickedCheckByAlarmLevel();
	afx_msg void OnBnClickedCheckSubscribeAll();
	void EnableAlarmTypeCheckbox(BOOL bCheck);

	//获取订阅信息
	CString GetSubscribeInfo();
	//告警订阅
	afx_msg void OnBnClickedButtonSubscribeAlarm();
	//刷新告警列表
	afx_msg LRESULT OnUpdateAlarmList(WPARAM wParam, LPARAM lParam);
};
