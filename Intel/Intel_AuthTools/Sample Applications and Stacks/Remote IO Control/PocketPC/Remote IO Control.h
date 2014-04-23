// Remote IO Control.h : main header file for the REMOTE IO CONTROL application
//

#if !defined(AFX_REMOTEIOCONTROL_H__14F28055_08D0_42FF_9724_C1C8AFD79941__INCLUDED_)
#define AFX_REMOTEIOCONTROL_H__14F28055_08D0_42FF_9724_C1C8AFD79941__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOControlApp:
// See Remote IO Control.cpp for the implementation of this class
//

class CRemoteIOControlApp : public CWinApp
{
public:
	CRemoteIOControlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteIOControlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemoteIOControlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTEIOCONTROL_H__14F28055_08D0_42FF_9724_C1C8AFD79941__INCLUDED_)
