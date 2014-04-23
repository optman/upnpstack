/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2001, 2002 Intel Corporation.  All rights reserved.
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
 
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 * 
 * $Workfile: MicroMediaServer.h
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 *
 * Intel Micro Media Server portable library. This file is the main
 * module of the UPnP Content Directory Service (CDS) library. It
 * contains the entry point and most of the management code.
 * This library is portable to POSIX, Windows and PocketPC.
 */

#ifndef MICROMEDIASERVER_H
#define MICROMEDIASERVER_H

/*
 *	InitMms(char*)
 *		sharedRootPath:				Indicates the root directory to share for this media server.
 *
 *	The main() function should call this function. This method will call
 *	UPnPStart() and will also do standard initialization and all that
 *	other good stuff in main().
 */
void InitMms(void* chain, void *stack, char *sharedRootPath);
void StopMms();

struct MMSMEDIATRANSFERSTAT
{
	char* filename;
	int   length;
	int   position;
	int   download;
};

extern void (*MmsOnStatsChanged) (void);
extern void (*MmsOnTransfersChanged) (int);
extern int MmsBrowseCount;
extern int MmsHttpRequestCount;
extern int MmsCurrentTransfersCount;
#define DOWNLOAD_STATS_ARRAY_SIZE 20
extern struct MMSMEDIATRANSFERSTAT MmsMediaTransferStats[DOWNLOAD_STATS_ARRAY_SIZE];
#ifdef _DEBUG
#define MMS_MALLOC mms_malloc
#define MMS_FREE mms_free
void* mms_malloc(int sz);
void mms_free(void* ptr);
#else
#define MMS_MALLOC malloc
#define MMS_FREE free
#endif

#endif

