// Remote IO ServerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CRemoteIOServerDlg dialog
class CRemoteIOServerDlg : public CDialog
{
// Construction
public:
	CRemoteIOServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_REMOTEIOSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnFileExit();
	CListCtrl ClientList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileForceconnect();
	afx_msg void OnFileConnect();
	afx_msg void OnFileDisconnect();
	afx_msg void OnFileReset();
};
