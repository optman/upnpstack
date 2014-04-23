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
 * $Workfile: PortFn_MsTime.h
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */


#ifndef PORTFN_MS_TIME_H
#define PORTFN_MS_TIME_H

#ifdef WIN32
#include <windows.h>
#define PORTFN_WINDOWS_IMPLEMENTATION
#endif

#ifdef UNDER_CE
#include <winbase.h>
#define PORTFN_WINDOWS_IMPLEMENTATION
#endif

#ifdef _UNIX
#include <sys/time.h>
#include <unistd.h>
#endif

/*
 *	[IMPLEMENTATION SPECIFIC CODE]
 *
 *	Different platforms have different means
 *	of calculating high resolution times.
 *	For example, Win32 supports ftime() but
 *	WinCE does not. ftime() and timeval are 
 *	supported in UNIX, but neither is really
 *	a POSIX.1 convention.
 *
 *	The goal of this library is really to provide
 *	a fast way for an app to determine if
 *	a particular amount of milliseconds have elapsed.
 *	It does not intend to abstract high-resolution
 *	date/time functions.
 *
 *	This header file assumes that Win32, WinCe,
 *	and Unix are used in the implementation. 
 *	External modules should #define
 *		WIN32
 *		UNDER_CE
 *		_UNIX
 *	to indicate which implementation they want
 *	to use. 
 *
 *	The Win32 and WinCE implementations use
 *	GetTickCount() as the basis for calculating
 *	high resolution times.
 *
 *	Unix uses gettimeofday(struct timeval *tv, struct timezone *tz)
 *	to achieve the same effect.
 */	

/*
 *	MsTime is understood to be opaque
 *	representation of a millisecond
 *	time value.
 */
struct MillisecTime
{
#ifdef PORTFN_WINDOWS_IMPLEMENTATION
	DWORD val;
#endif

#ifdef _UNIX
	struct timeval val;
#endif
};

/*
 *	Stores the high resolution representation
 *	of the current time in the provided
 *	HrTime.
 */
void GetMillisecTime(struct MillisecTime* millisecTime);

/*
 *	Returns millisecond difference of t1-t2.
 */
long DiffMsTimes(struct MillisecTime* t1, struct MillisecTime* t2);

/*
 *	Sets the specified millisecond time to zero.
 */
void SetZeroMsTime(struct MillisecTime* makeZero);

/*
 *	Sleeps for a number of milliseconds
 */
void SleepMsTime(int millisecTime);

#endif
