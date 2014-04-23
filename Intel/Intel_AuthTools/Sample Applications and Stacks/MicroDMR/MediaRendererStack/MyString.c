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
 * $Workfile: MyString.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */


 /*
 * Implements additional string functionality.
 */

#include <string.h>
#include "MyString.h"

#ifdef UNDER_CE
#define strnicmp _strnicmp
#endif

#ifdef _POSIX
#define strnicmp strncasecmp
#endif

int EndsWith(const char* str, const char* endsWith, int ignoreCase)
{
	int strLen, ewLen, offset;
	int cmp = 0;

	strLen = (int)strlen(str);
	ewLen = (int)strlen(endsWith);
	if (ewLen > strLen) return 0;
	offset = strLen - ewLen;

	if (ignoreCase != 0)
	{
		cmp = strnicmp(str+offset, endsWith, ewLen);
	}
	else
	{
		cmp = strncmp(str+offset, endsWith, ewLen);
	}

	return cmp == 0?1:0;
}

int IndexOf(const char* str, const char* findThis)
{
	int i,j,strLen,ftLen;
	
	strLen = (int)strlen(str);
	ftLen = (int)strlen(findThis);

	if (ftLen <= strLen)
	{
		for (i=0; i < strLen; i++)
		{
			for (j=0; j < ftLen; j++)
			{
				if (str[i+j] != findThis[j]) break;
			}
			if (j == ftLen) return i;
		}
	}

	return -1;
}

int LastIndexOf(const char* str, const char* findThis)
{
	int i,j,strLen,ftLen;

	strLen = (int)strlen(str);
	ftLen = (int)strlen(findThis);

	if (ftLen <= strLen)
	{
		for (i=strLen-ftLen; i >= 0; i--)
		{
			for (j=0; j < ftLen; j++)
			{
				if (str[i+j] != findThis[j]) break;
			}
			if (j == ftLen) return i;
		}
	}

	return -1;
}

int StartsWith(const char* str, const char* startsWith, int ignoreCase)
{
	int cmp;

	if (ignoreCase != 0)
	{
		cmp = strnicmp(str, startsWith, (int)strlen(startsWith));
	}
	else
	{
		cmp = strncmp(str, startsWith, (int)strlen(startsWith));
	}

	return cmp == 0?1:0;
}

