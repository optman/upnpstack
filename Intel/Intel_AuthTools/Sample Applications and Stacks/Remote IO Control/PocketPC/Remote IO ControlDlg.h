// Remote IO ControlDlg.h : header file
//

#if !defined(AFX_REMOTEIOCONTROLDLG_H__5DCD4A75_1436_4209_87DE_2D3E16B70E59__INCLUDED_)
#define AFX_REMOTEIOCONTROLDLG_H__5DCD4A75_1436_4209_87DE_2D3E16B70E59__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOControlDlg dialog

class CRemoteIOControlDlg : public CDialog
{
// Construction
public:
	CRemoteIOControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoteIOControlDlg)
	enum { IDD = IDD_REMOTEIOCONTROL_DIALOG };
	CTreeCtrl	m_DeviceTree;
	CStatic	m_StaticText3;
	CStatic	m_StaticText2;
	CStatic	m_StaticText1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteIOControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRemoteIOControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkDevicetree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedDevicetree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEIOCONTROLDLG_H__5DCD4A75_1436_4209_87DE_2D3E16B70E59__INCLUDED_)
