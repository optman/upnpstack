/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2002, 2003 Intel Corporation.  All rights reserved.
 * 
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors.  Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors.  The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and
 * licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.  No part of the Material
 * may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission. 
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *  
 * $Workfile: CMicroMediaRenderer.h
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */

#include "resource.h" 
#include <commdlg.h>
#include "PlayerOCX.h"
#include "events_dispid.h"

#define IDC_PLAYER			1000
#define IDB_PLAY			1001
#define IDB_PAUSE			1002
#define	IDB_STOP			1003
#define IDB_FASTREVERSE		1004
#define IDB_FASTFORWARD		1005
#define MAX_PLAYER_HEIGHT	198
#define STATUS_BAR_HEIGHT	14
#define WM_MPINVOKE			(WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////
// CMicroMediaRenderer

class CMicroMediaRenderer : 
	public CWindowImpl<CMicroMediaRenderer, CWindow, CWinTraits<WS_VISIBLE, NULL> >,
	public CComObjectRootEx<CComSingleThreadModel>,
	public _IWMPEvents
{
	CAxWindow					m_wndView;						// The window that hosts the Windows Media Player control
	CComPtr<IWMP>				m_spWMPPlayer;					// A pointer to the Windows Media Player control interface
	CComPtr<IConnectionPoint>	m_spConnectionPoint;			// The connection point through which events are handled
	DWORD						m_dwAdviseCookie;				// A cookie used by the connection point to cease event notifications
	DWORD						m_nFilterIndex;					// An index to the current file name filter
	TCHAR*						m_szCurrentFile;				// The name of the current file (without path information)

public:

	BEGIN_COM_MAP(CMicroMediaRenderer)
		COM_INTERFACE_ENTRY_IID(__uuidof(_IWMPEvents), _IWMPEvents)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CMicroMediaRenderer)

		MESSAGE_HANDLER(	WM_CREATE,			OnCreate)
		MESSAGE_HANDLER(	WM_PAINT,			OnPaint)
		MESSAGE_HANDLER(	WM_DESTROY,			OnDestroy)
		MESSAGE_HANDLER(	WM_MPINVOKE,		OnMPINVOKE)

		COMMAND_ID_HANDLER( IDOK,				OnOK)
		COMMAND_ID_HANDLER( ID_FILE_OPEN,		OnFileOpen)
		COMMAND_ID_HANDLER( ID_FILE_EXIT,		OnFileExit)

	END_MSG_MAP()

	
	// CMediaBookmarker methods:

	CMicroMediaRenderer();
	~CMicroMediaRenderer();

	// Window methods:
	
	LRESULT OnCreate(		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	// User-interface methods:

	LRESULT OnOK(			WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFileOpen(		WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFileExit(		WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// IDispatch methods:

	STDMETHOD(GetIDsOfNames)(	REFIID				riid, 
								OLECHAR FAR *FAR	*rgszNames,
								unsigned int		cNames, 
								LCID				lcid, 
								DISPID FAR			*rgDispId )
																{ return( E_NOTIMPL ); };

	STDMETHOD(GetTypeInfo)(		unsigned int		iTInfo, 
								LCID				lcid, 
								ITypeInfo FAR *FAR	*ppTInfo )
																{ return( E_NOTIMPL ); };

	STDMETHOD(GetTypeInfoCount)(unsigned int FAR	*pctinfo )
																{ return( E_NOTIMPL ); };

	STDMETHOD(Invoke)(			DISPID				dispIdMember,	  
								REFIID				riid,			  
								LCID				lcid,				
								WORD				wFlags,			  
								DISPPARAMS FAR		*pDispParams,  
								VARIANT FAR			*pVarResult,  
								EXCEPINFO FAR		*pExcepInfo,  
								unsigned int FAR	*puArgErr );


	// Windows Media Player Control event methods:

	void OnPlayStateChange(long NewState);

	void SetMediaPlayerVolume();

	LRESULT OnMPINVOKE(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};

typedef CComObject<CMicroMediaRenderer> CComMicroMediaRenderer;
