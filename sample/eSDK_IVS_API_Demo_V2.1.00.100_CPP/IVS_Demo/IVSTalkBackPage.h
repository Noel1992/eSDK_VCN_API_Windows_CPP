#pragma once
#include "IVS_SDK.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <list>


// CIVSTalkBackPage dialog
class CIVSSDKSheet;
class CIVSTalkBackPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSTalkBackPage)

public:
	CIVSTalkBackPage();
	virtual ~CIVSTalkBackPage();

// Dialog Data
	enum { IDD = IDD_DLG_TALKBACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;

	//协议类型下拉选框
	CComboBox m_ComboProtocol;
	//是否循环下拉选框
	CComboBox m_ComboCycle;

	//对讲设备、对讲句柄
	CString m_strTalkbackDevice;
	IVS_ULONG m_ulTalkbackHandle;

	//添加、删除广播设备的编码
	CString m_strAddDevCode;
	CString m_strDeleteDevCode;
	//广播设备列表
	CListCtrl m_listBrodcast;
	std::list<CString> m_BrodcastDevList;
	//本地广播文件
	CString m_strBrodcastFile;

	/*GroupBox & StaticLabel & Button*/
	CStatic m_GrpTalkback;
	CStatic m_GrpBrodcastDev;
	CStatic m_GrpBrodcast;
	CStatic m_LabDevCode1;
	CStatic m_LabDevCode2;
	CStatic m_LabDevCode3;
	CStatic m_LabBrodcastFile;
	CButton m_CheckDirect;
	CButton m_BtnStartTalkback;
	CButton m_BtnStopTalkback;
	CButton m_BtnAddDevice;
	CButton m_BtnDeleteDevice;
	CButton m_BtnStartFileBrodcast;
	CButton m_BtnStopFileBrodcast;
	CButton m_BtnStartRealBrodcast;
	CButton m_BtnStopRealBrodcast;

public:
	virtual BOOL OnInitDialog();

	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);

	//语音对讲
	afx_msg void OnBnClickedButtonStartTalkBack();
	afx_msg void OnBnClickedButtonStopTalkBack();

	//广播设备管理
	afx_msg void OnBnClickedButtonAddBrodcastDevice();
	afx_msg void OnBnClickedButtonDeleteBrodcastDevice();
	afx_msg LRESULT OnUpdateBrodcastDevList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMClickListBrodcastDev(NMHDR *pNMHDR, LRESULT *pResult);

	//文件广播
	afx_msg void OnBnClickedButtonStopFileBrodcast();
	afx_msg void OnBnClickedButtonStartFileBrodcast();

	//语音广播
	afx_msg void OnBnClickedButtonStartRealBrodcast();
	afx_msg void OnBnClickedButtonStopRealBrodcast();
};
