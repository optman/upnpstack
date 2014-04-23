// Remote IO Server.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CRemoteIOServerApp:
// See Remote IO Server.cpp for the implementation of this class
//

class CRemoteIOServerApp : public CWinApp
{
public:
	CRemoteIOServerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRemoteIOServerApp theApp;