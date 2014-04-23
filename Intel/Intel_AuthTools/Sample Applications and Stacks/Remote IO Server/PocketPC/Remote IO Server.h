// Remote IO Server.h : main header file for the REMOTE IO SERVER application
//

#if !defined(AFX_REMOTEIOSERVER_H__B743ABCA_A69B_4066_8F25_4DA676BB0483__INCLUDED_)
#define AFX_REMOTEIOSERVER_H__B743ABCA_A69B_4066_8F25_4DA676BB0483__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOServerApp:
// See Remote IO Server.cpp for the implementation of this class
//

class CRemoteIOServerApp : public CWinApp
{
public:
	CRemoteIOServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteIOServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemoteIOServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEIOSERVER_H__B743ABCA_A69B_4066_8F25_4DA676BB0483__INCLUDED_)
