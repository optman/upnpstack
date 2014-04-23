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
 * $Workfile: MicroMediaRendererDlg.h
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */


// MicroAVRendererDlg.h : header file
//

#pragma once
#include "mediaplayer1.h"

extern "C"
{
	#include "../MicroMediaRenderer.h"
}

// CMicroAVRendererDlg dialog
class CMicroAVRendererDlg : public CDialog
{
// Construction
public:
	CMicroAVRendererDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MICROAVRENDERER_DIALOG };

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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	DECLARE_EVENTSINK_MAP()
	void PositionChangeActivemoviecontrol2(double oldPosition, double newPosition);
	void StateChangeActivemoviecontrol2(long oldState, long newState);
	afx_msg LRESULT CMicroAVRendererDlg::OnMPINVOKE(WPARAM wp, LPARAM lp);
	void ErrorActivemoviecontrol2(short SCode, LPCTSTR Description, LPCTSTR Source, BOOL* CancelDisplay);
	CMediaplayer1 MediaPlayer;
	void PositionChangeMediaplayer1(double oldPosition, double newPosition);
	void PlayStateChangeMediaplayer1(long OldState, long NewState);
	void ErrorMediaplayer1();
	void OpenStateChangeMediaplayer1(long OldState, long NewState);
	afx_msg void OnFileExit();
	afx_msg void OnControlPlay();
	afx_msg void OnControlStop();
	afx_msg void OnControlPause();
public:
	void CloseDialog();

};
