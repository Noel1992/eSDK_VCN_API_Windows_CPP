#pragma once
#include "IVS_SDK.h"
#include "afxwin.h"


// CIVSUsrMgrPage dialog
class CIVSSDKSheet;
class CIVSUsrMgrPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIVSUsrMgrPage)

public:
	CIVSUsrMgrPage();
	virtual ~CIVSUsrMgrPage();

// Dialog Data
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CIVSSDKSheet* m_pSheet;
	IVS_INT32 m_iSessionID;

	//登录信息
	CString m_strUsrName;
	CString m_strPassword;
	CString m_strIPAddress;
	IVS_INT32 m_iPort;

	//密码修改信息
	CString m_strOldPwd;
	CString m_strNewPwd;
	CString m_strConfirmPwd;

	//版本号
	CString m_strVersion;

	/*GroupBox & StaticLabel & Button*/
	CStatic m_GrpSDKCtrl;
	CStatic m_GrpLoginCtrl;
	CStatic m_GrpPwdCtrl;
	CStatic m_LabUsrName;
	CStatic m_LabPwd;
	CStatic m_LabServer;
	CStatic m_LabPort;
	CStatic m_LabOldPwd;
	CStatic m_LabNewPwd;
	CStatic m_LabConfirmPwd;
	CButton m_BtnInit;
	CButton m_BtnUninit;
	CButton m_BtnGetVer;
	CButton m_BtnLogin;
	CButton m_BtnLogout;
	CButton m_BtnChangePwd;

public:
	virtual BOOL OnInitDialog();
	//设置属性对话框
	void SetSheet(CIVSSDKSheet* pSheet);
	//初始化SDK
	afx_msg void OnBnClickedButtonInit();
	//释放SDK
	afx_msg void OnBnClickedButtonClean();
	//用户登录
	afx_msg void OnBnClickedButtonLogin();
	//用户签出
	afx_msg void OnBnClickedButtonLogout();
	//修改登录密码
	afx_msg void OnBnClickedButtonChangePwd();
	//获取版本号
	afx_msg void OnBnClickedButtonGetVer();
};
