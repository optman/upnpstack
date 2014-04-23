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
 * $Workfile: Utility.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
	#include "Windows.h"
#endif

#ifdef _DEBUG
	#define UTL_MALLOC	utl_malloc
	#define UTL_FREE	utl_free

	int utl_malloc_counter = 0;
	void* utl_malloc (int size)
	{
		++utl_malloc_counter;
		#ifdef TRACK_MALLOC_VERBOSE
			printf("utl_malloc_counter=%d\r\n", utl_malloc_counter);
		#endif
		return malloc(size);
	}

	void utl_free (void *ptr)
	{
		--utl_malloc_counter;
		#ifdef TRACK_MALLOC_VERBOSE
			printf("utl_malloc_counter=%d\r\n", utl_malloc_counter);
		#endif
		free(ptr);
	}
#endif

#ifndef _DEBUG
	#define UTL_MALLOC	malloc
	#define UTL_FREE	free
#endif


void* CopyArray(int elementSize, int numElements, const void* data)
{
	int size;
	void* dataCopy = NULL;

	size =  elementSize * numElements;
	dataCopy = (void*) UTL_MALLOC (size);
	memcpy(dataCopy, data, size);

	return dataCopy;
}


void _SafeFree (void* freeThis)
{
	if (freeThis != NULL)
	{
		UTL_FREE (freeThis);
	}
}

void SafeFree (void** freeThis)
{
	_SafeFree(*freeThis);

	*freeThis = NULL;
}


char* SafeStringCopy (char* storeHere, const char* str)
{
	char* retVal = storeHere;
	int size = 1;
	
	if (str != NULL)
	{
		if (storeHere == NULL)
		{
			size = (int) strlen(str) + 1;
			retVal = CopyArray(1, size, str);
		}
		else
		{
			strcpy(retVal, str);
		}
	}
	else
	{
		if (storeHere == NULL)
		{
			retVal = (char*) UTL_MALLOC(1);
		}
		retVal[0] = '\0';
	}

#ifdef _DEBUG
#ifdef WIN32
	/*
	if (!HeapValidate(GetProcessHeap(), 0, NULL))
	{
		foo = 3;
	}
	*/
#endif
#endif

	return retVal;
}
