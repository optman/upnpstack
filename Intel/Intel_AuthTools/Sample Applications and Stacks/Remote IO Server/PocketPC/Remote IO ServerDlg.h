// Remote IO ServerDlg.h : header file
//

#if !defined(AFX_REMOTEIOSERVERDLG_H__45D5AFFC_AA9F_4A07_B7FC_929C718E066A__INCLUDED_)
#define AFX_REMOTEIOSERVERDLG_H__45D5AFFC_AA9F_4A07_B7FC_929C718E066A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOServerDlg dialog

class CRemoteIOServerDlg : public CDialog
{
// Construction
public:
	CRemoteIOServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoteIOServerDlg)
	enum { IDD = IDD_REMOTEIOSERVER_DIALOG };
	CListCtrl	ClientList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteIOServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRemoteIOServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnFileConnect();
	afx_msg void OnFileDisconnect();
	afx_msg void OnFileExit();
	afx_msg void OnFileForceconnect();
	afx_msg void OnFileReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEIOSERVERDLG_H__45D5AFFC_AA9F_4A07_B7FC_929C718E066A__INCLUDED_)
