// Remote IO Control.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CRemoteIOControlApp:
// See Remote IO Control.cpp for the implementation of this class
//

class CRemoteIOControlApp : public CWinApp
{
public:
	CRemoteIOControlApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRemoteIOControlApp theApp;