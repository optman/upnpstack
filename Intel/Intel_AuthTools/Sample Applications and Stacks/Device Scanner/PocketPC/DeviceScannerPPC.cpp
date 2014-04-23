// DeviceScannerPPC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeviceScannerPPC.h"
#include "DeviceScannerPPCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceScannerPPCApp

BEGIN_MESSAGE_MAP(CDeviceScannerPPCApp, CWinApp)
	//{{AFX_MSG_MAP(CDeviceScannerPPCApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceScannerPPCApp construction

CDeviceScannerPPCApp::CDeviceScannerPPCApp()
	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDeviceScannerPPCApp object

CDeviceScannerPPCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDeviceScannerPPCApp initialization

BOOL CDeviceScannerPPCApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CDeviceScannerPPCDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
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
