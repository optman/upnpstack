// DeviceScanner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeviceScanner.h"
#include "DeviceScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeviceScannerApp

BEGIN_MESSAGE_MAP(CDeviceScannerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDeviceScannerApp construction

CDeviceScannerApp::CDeviceScannerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDeviceScannerApp object

CDeviceScannerApp theApp;


// CDeviceScannerApp initialization

BOOL CDeviceScannerApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();


	CDeviceScannerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
