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
 * $Workfile: PortFn_MsTime.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */

#include "PortFn_MsTime.h"

#ifdef PORTFN_WINDOWS_IMPLEMENTATION
/* for timeval in Win32 */
#include <winsock.h>
#include <time.h>
#include <process.h>
#endif

#ifdef _UNIX
#include <sys/time.h>
#endif

void GetMillisecTime(struct MillisecTime* msTime)
{
#ifdef PORTFN_WINDOWS_IMPLEMENTATION
	msTime->val = GetTickCount();
#endif

#ifdef _UNIX
	gettimeofday(&(msTime->val), NULL);
#endif
}

long DiffMsTimes(struct MillisecTime* t1, struct MillisecTime* t2)
{
#ifdef PORTFN_WINDOWS_IMPLEMENTATION
	return (t1->val - t2->val);
#endif

#ifdef _UNIX
	long t1ms, t2ms;

	/* convert t1 to milliseconds */
	t1ms = (t1->val.tv_sec * 1000) + (t1->val.tv_usec / 1000);
	t2ms = (t2->val.tv_sec * 1000) + (t2->val.tv_usec / 1000);

	return (t1ms - t2ms);
#endif
}

void SetZeroMsTime(struct MillisecTime* makeZero)
{
#ifdef PORTFN_WINDOWS_IMPLEMENTATION
	makeZero->val = 0;
#endif

#ifdef _UNIX
	makeZero->val.tv_sec = 0;
	makeZero->val.tv_usec = 0;
#endif
}

void SleepMsTime(int millisecTime)
{
#ifdef PORTFN_WINDOWS_IMPLEMENTATION
	Sleep(millisecTime);
#endif

#ifdef _UNIX
	struct timeval delay;
	int seconds;

	seconds = millisecTime / 1000;
	delay.tv_sec = seconds;
	delay.tv_usec = (millisecTime - (seconds * 1000)) * 1000;

	select(0, NULL, NULL, NULL, &delay);
#endif
}

