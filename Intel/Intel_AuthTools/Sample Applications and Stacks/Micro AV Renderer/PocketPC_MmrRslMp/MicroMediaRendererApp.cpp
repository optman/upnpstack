// MediaBookmarker.cpp : Main window procedure
//
// Copyright (c) Microsoft Corporation. All rights reserved.  THIS CODE IS MADE AVAILABLE AS 
// IS, WITHOUT WARRANTY OF ANY KIND. THE ENTIRE RISK OF THE USE OR THE RESULTS FROM THE USE 
// OF THIS CODE REMAINS WITH THE USER. USE AND REDISTRIBUTION OF THIS CODE, WITH OUR 
// WITHOUT MODICATION, IS HEREBY PERMITTED.
//

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "CMicroMediaRenderer.h"
#include <commctrl.h>

// ATL bookkeeping:
CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	// If the program is already running, bring its window to the foreground.
	HWND hwnd = FindWindow(NULL, TEXT("Intel Media Renderer"));	
	if (hwnd) 
	{
		SetForegroundWindow(hwnd);
		return 0;
	} 

	// Initialize COM.
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) 
	{
		return 0;
	}

	// Initialize ATL.
	if (FAILED(_Module.Init(ObjectMap, hInstance)))
	{
		CoUninitialize();
		return 0;
	}

	// Create the main window.
	CComMicroMediaRenderer frame;
	RECT rcPos = { 0, 26, 240, 294 };
	frame.Create(NULL, rcPos, TEXT("Intel Media Renderer"), 0, 0, 0);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Clean up.
	_Module.Term();
	CoUninitialize();
	return 0;
}
