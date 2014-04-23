// Remote IO ControlDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CRemoteIOControlDlg dialog
class CRemoteIOControlDlg : public CDialog
{
// Construction
public:
	CRemoteIOControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_REMOTEIOCONTROL_DIALOG };

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
	CTreeCtrl DeviceTree;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	CStatic StaticText1;
	CStatic StaticText2;
	CStatic StaticText3;
	afx_msg void OnTvnSelchangedDevicetree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkDevicetree(NMHDR *pNMHDR, LRESULT *pResult);
};
