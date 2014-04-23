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
 * $Workfile: PortingFunctions.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 *
 * This wraps a few simple tasks that are different between platforms. 
 * Most of it focuses on the file system calls that are completely 
 * different on POSIX, Windows and Pocket PC.
 * PocketPC is probably the most different since all of the calls have
 * to be made using wide character arrays (16 bit chars). So code was
 * added to convert multibyte (UTF-8) to wide.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyString.h"
#include "MicroMediaServer.h"

/* Windows 32 */
#ifdef WIN32
#include "stdafx.h"
#include <windows.h>
#include <time.h>
#endif

/* Win CE */
#ifdef UNDER_CE
#include <Winbase.h>
#endif

/* POSIX */
#ifdef _POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#endif

//#define MMS_MALLOC malloc
//#define MMS_FREE free

void PCRandomize()
{
#ifdef WIN32
	srand((int)GetTickCount());
#endif

#ifdef UNDER_CE
	srand((int)GetTickCount());
#endif

#ifdef _POSIX
	struct timeval tv;
	gettimeofday(&tv,NULL);
	srand((int)tv.tv_sec);
#endif
}

// Windows Version
void PCCloseDir(void* handle)
{
#ifdef WIN32
	FindClose(*((HANDLE*)handle));
	MMS_FREE(handle);
#endif

#ifdef UNDER_CE
	FindClose(*((HANDLE*)handle));
	MMS_FREE(handle);
#endif

#ifdef _POSIX
	DIR* dirObj = (DIR*) handle;
	closedir(dirObj);
#endif
}


void* PCGetDirFirstFile(const char* directory, const char* dirName, char* filename, int filenamelength, int* filesize)
{
#ifdef WIN32
	WIN32_FIND_DATA FileData;
	HANDLE* hSearch;
	char* direx;

	hSearch = MMS_MALLOC(sizeof(HANDLE));
	direx = MMS_MALLOC(filenamelength + 5);

	if (directory[(int) strlen(directory) - 1] == '\\')
	{
		sprintf(direx,"%s*.*",directory);
	}
	else
	{
		sprintf(direx,"%s\\*.*",directory);
	}

	*hSearch = FindFirstFile(direx, &FileData);
	MMS_FREE(direx);

	if (*hSearch == INVALID_HANDLE_VALUE)
	{
		MMS_FREE(hSearch);
		hSearch = NULL;
	}
	else
	{
		strToUtf8(filename,FileData.cFileName, filenamelength, 0);

		if (filesize != NULL)
		{
			*filesize = FileData.nFileSizeLow;
		}
	}

	return hSearch;
#endif


#ifdef UNDER_CE
	WIN32_FIND_DATA FileData;
	HANDLE* hSearch;			/* must MMS_FREE */
	wchar_t* wdirectory;		/* must MMS_FREE */
	int wDirLen;
	int wDirSize;
	int mbDirLen;
	int mbDirSize;
	char* direx;				/* must MMS_FREE */

	hSearch = MMS_MALLOC(sizeof(HANDLE));
	direx = MMS_MALLOC(filenamelength + 5);

	if (directory[(int) strlen(directory) - 1] == '\\')
	{
		sprintf(direx,"%s*.*",directory);
	}
	else
	{
		sprintf(direx,"%s\\*.*",directory);
	}

	mbDirLen = (int) strlen(direx);
	mbDirSize = mbDirLen+1;
	wDirLen = mbDirLen * 2;
	wDirSize = mbDirSize * 2;
	wdirectory = (wchar_t*)MMS_MALLOC(wDirSize);

	if (mbstowcs(wdirectory,direx,wDirSize) == -1)
	{
		MMS_FREE(hSearch);
		hSearch = NULL;
	}
	else
	{
		*hSearch = FindFirstFile(wdirectory, &FileData);
		if (*hSearch == INVALID_HANDLE_VALUE)
		{
			MMS_FREE(hSearch);
			hSearch = NULL;
		}
		else
		{
			if (strToUtf8(filename,(char*)FileData.cFileName,filenamelength, 1) == -1)
			{
				FindClose(*hSearch);
				MMS_FREE(hSearch);
				hSearch = NULL;
			}
			else
			{
				if (filesize != NULL)
				{
					*filesize = FileData.nFileSizeLow;
				}
			}
		}
	}

	MMS_FREE(direx);
	MMS_FREE(wdirectory);

	return hSearch;
#endif

#ifdef _POSIX
	DIR* dirObj;
	struct dirent* dirEntry;	/* dirEntry is a pointer to static memory in the C runtime lib for readdir()*/
	struct stat _si;
	char fullPath[1024];
	
	dirObj = opendir(directory);

	if (dirObj != NULL)
	{
		dirEntry = readdir(dirObj);

		if ((dirEntry != NULL) && ((int) strlen(dirEntry->d_name) < filenamelength))
		{
			strToUtf8(filename, dirEntry->d_name, filenamelength, 0);
			sprintf(fullPath, "%s%s", dirName, dirEntry->d_name);

			if (filesize != NULL)
			{
				if (stat(fullPath, &_si) != -1)
				{
					if ((_si.st_mode & S_IFDIR) == S_IFDIR)
					{
						*filesize = 0;
					}
					else
					{
						*filesize = _si.st_size;
					}
				}
			}
		}
	}

	return dirObj;
#endif
}


// Windows Version
// 0 = No More Files
// 1 = Next File
int PCGetDirNextFile(void* handle, const char* dirName, char* filename, int filenamelength, int* filesize)
{
#ifdef WIN32
	WIN32_FIND_DATA FileData;
	
	if (FindNextFile(*((HANDLE*)handle), &FileData) == 0) {return 0;}
	strToUtf8(filename,FileData.cFileName,filenamelength,0);

	if (filesize != NULL)
	{
		*filesize = FileData.nFileSizeLow;
	}

	return 1;
#endif

#ifdef UNDER_CE
    WIN32_FIND_DATA FileData;
    int fnf = 0;
    int conv = -1;
    
    fnf = FindNextFile(*((HANDLE*)handle), &FileData);
    if (fnf == 0) {return 0;}

    conv = strToUtf8(filename, (char*)FileData.cFileName, filenamelength, 1);
    if (conv == -1) {return 0;}

	if (filesize != NULL)
	{
		*filesize = FileData.nFileSizeLow;
	}
    return 1;
#endif

#ifdef _POSIX
	DIR* dirObj;
	struct dirent* dirEntry;	/* dirEntry is a pointer to static memory in the C runtime lib for readdir()*/
	struct stat _si;
	char fullPath[1024];

	dirObj = (DIR*) handle;
	dirEntry = readdir(dirObj);

	if ((dirEntry != NULL) && ((int) strlen(dirEntry->d_name) < filenamelength))
	{
		strToUtf8(filename, dirEntry->d_name, filenamelength, 0);
		sprintf(fullPath, "%s%s", dirName, dirEntry->d_name);

		if (filesize != NULL)
		{
			/* WTF? Cygwin has a memory leak with stat. */
			if (stat(fullPath, &_si) != -1)
			{
				if ((_si.st_mode & S_IFDIR) == S_IFDIR)
				{
					*filesize = 0;
				}
				else
				{
					*filesize = _si.st_size;
				}
			}
		}

		return 1;
	}

	return 0;
#endif
}

// WINCE Version
// 0 = Does Not Exist
// 1 = Is a File
// 2 = Is a Directory
int PCGetFileDirType(char* directory)
{
#ifdef WIN32
	DWORD _si;
	int dirLen,dirSize;
	char *fullpath;

	dirLen = (int) strlen(directory);
	dirSize = dirLen+1;
	fullpath = (char*) MMS_MALLOC(dirSize);
	Utf8ToAnsi(fullpath, directory, dirSize);

	_si = GetFileAttributes(fullpath);
	
	MMS_FREE(fullpath);
	
	if (_si == 0xFFFFFFFF)
	{
		return 0;
	}

	if ((_si & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		return 1;
	}
	else 
	{
		return 2;
	}
#endif

#ifdef UNDER_CE
	wchar_t* wfullPath;
	DWORD _si;
	int mbDirSize;
	int wPathSize;
	int dirLen,dirSize;
	char *fullpath;
	int retVal = 0;

	dirLen = (int) strlen(directory);
	dirSize = dirLen+1;
	fullpath = (char*) MMS_MALLOC(dirSize);
	Utf8ToAnsi(fullpath, directory, dirSize);

	mbDirSize = (int) strlen(fullpath) + 1;
	wPathSize = mbDirSize * 2;

	wfullPath = (wchar_t*)MMS_MALLOC(wPathSize);
	if (mbstowcs(wfullPath,fullpath,wPathSize) == -1)
	{
		retVal = 0;
	}
	else
	{
		_si = GetFileAttributes(wfullPath);
		if (_si == 0xFFFFFFFF)
		{
			retVal = 0;
		}
		else
		{
			if ((_si & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				retVal = 1;
			}
			else 
			{
				retVal = 2;
			}
		}
	}

	MMS_FREE(fullpath);
	MMS_FREE(wfullPath);

	return retVal;
#endif

#ifdef _POSIX
	struct stat _si;
	int fpi;
	int dirLen,dirSize;
	char *fullpath;
	int pathExists;
	int retVal = 0;

	dirLen = (int) strlen(directory);
	dirSize = dirLen+1;
	fullpath = (char*) MMS_MALLOC(dirSize);
	Utf8ToAnsi(fullpath, directory, dirSize);

	pathExists = stat (fullpath, &_si);

	MMS_FREE(fullpath);

	if (pathExists != -1)
	{
		if ((_si.st_mode & S_IFDIR) == S_IFDIR)
		{
			retVal = 2;
		}
		else
		{
			retVal = 1;
		}
	}

	return retVal;
#endif
}

#ifdef _POSIX
/* only needed for posix because readdir returns statically allocated values */
pthread_mutex_t BrowseLock;
#endif

void InitBrowseLock()
{
#ifdef _POSIX
	pthread_mutex_init(&BrowseLock, NULL);
#endif
}

void LockBrowse()
{
#ifdef _POSIX
	pthread_mutex_lock(&BrowseLock);
#endif
}

void UnlockBrowse()
{
#ifdef _POSIX
	pthread_mutex_unlock(&BrowseLock);
#endif
}

void DestroyBrowseLock()
{
#ifdef _POSIX
	pthread_mutex_destroy(&BrowseLock);
#endif
}

void EndThisThread()
{
#ifdef _POSIX
	pthread_exit(NULL);
#endif

#ifdef WIN32
	ExitThread(0);
#endif

#ifdef UNDER_CE
	ExitThread(0);
#endif
}

void* SpawnNormalThread(void* method, void* arg)
{
#ifdef _POSIX
	int result;
	void* (*fptr) (void* a);
	pthread_t newThread;
	fptr = method;
	result = pthread_create(&newThread, NULL, fptr, arg);
	pthread_detach(newThread);
	return (void*) result;
#endif

#ifdef WIN32
	return CreateThread(NULL, 0, method, arg, 0, NULL );
#endif

#ifdef UNDER_CE
	return CreateThread(NULL, 0, method, arg, 0, NULL );
#endif
}

int PCGetFileSize(char* fullPath)
{
	int filesize = -1;

#ifdef _POSIX
	struct stat _si;
	int fpi;
	int pathLen,pathSize;
	char *fp;

	pathLen = (int) strlen(fullPath);
	pathSize = pathLen+1;
	fp = (char*) MMS_MALLOC(pathSize);
	Utf8ToAnsi(fp, fullPath, pathSize);

	if (stat(fp, &_si) != -1)
	{
		if (!((_si.st_mode & S_IFDIR) == S_IFDIR))
		{
			filesize = _si.st_size;
		}
	}

	MMS_FREE(fp);
#endif

#ifdef WIN32
	WIN32_FIND_DATA FileData;
	HANDLE* hSearch;			/* must MMS_FREE */
	int pathLen,pathSize;
	char *fp;

	pathLen = (int) strlen(fullPath);
	pathSize = pathLen+1;
	fp = (char*) MMS_MALLOC(pathSize);
	Utf8ToAnsi(fp, fullPath, pathSize);

	hSearch = MMS_MALLOC(sizeof(HANDLE));

	*hSearch = FindFirstFile(fp, &FileData);
	MMS_FREE(fp);

	if (*hSearch == INVALID_HANDLE_VALUE)
	{
		filesize = 0;
	}
	else
	{
		filesize = FileData.nFileSizeLow;
	}

	FindClose(*hSearch);
	MMS_FREE(hSearch);
#endif

#ifdef UNDER_CE
	WIN32_FIND_DATA FileData;
	HANDLE* hSearch;			/* must MMS_FREE */
	wchar_t* wdirectory;		/* must MMS_FREE */
	int wPathLen;
	int wPathSize;
	int fullPathLen;
	int fullPathSize;
	char* fp;

	fullPathLen = (int) strlen(fullPath);
	fullPathSize = fullPathLen + 1;
	fp = (char*) MMS_MALLOC(fullPathSize);
	Utf8ToAnsi(fp, fullPath, fullPathSize);

	hSearch = MMS_MALLOC(sizeof(HANDLE));

	wPathLen = fullPathLen * 2;
	wPathSize = fullPathSize * 2;
	wdirectory = (wchar_t*)MMS_MALLOC(wPathSize);

	if (mbstowcs(wdirectory,fp,wPathSize) == -1)
	{
		filesize = -1;
	}
	else
	{
		*hSearch = FindFirstFile(wdirectory, &FileData);
		if (*hSearch == INVALID_HANDLE_VALUE)
		{
			filesize = -1;
		}
		else
		{
			FindClose(*hSearch);
			filesize = FileData.nFileSizeLow;
		}
	}

	MMS_FREE(fp);
	MMS_FREE(wdirectory);
	MMS_FREE(hSearch);
#endif

	return filesize;
}

/*
	 returns 0 if directory entry should not be processed.
	 returns nonzero if directory entry should be processed
*/
int ProceedWithDirEntry(const char* dirName, const char* filename, int maxPathLength)
{
	int dirLen;
	int fnLen;
	int val;

	char *fullpath;

	dirLen = (int) strlen(dirName);
	fnLen = (int) strlen(filename);

	if ((strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0))
	{
		/* NOP */
		return 0;
	}
	
	if ((dirLen+fnLen+2) > maxPathLength)
	{
		/* directory is too long */
		return 0;
	}

	/* prevent hidden files from showing up */
	fullpath = (char*) MMS_MALLOC(maxPathLength);
	memcpy(fullpath, dirName, dirLen);
	memcpy(fullpath+dirLen, filename, fnLen);
	fullpath[ dirLen+fnLen ] = '\0';
	val = PCGetFileDirType(fullpath);
	MMS_FREE(fullpath);
	if (val == 0)
	{
		return 0;
	}

		/*
		#ifdef UNDER_CE
		#define CF_CARD "CF Card"
		#define SD_CARD "SD Card"
		#define MMC_CARD "MMC Card"
		#define BUILT_IN_STORAGE "Built-in Storage"
		#define STORAGE "Storage"

				if (strcmp(dirName, "\\") == 0)
				{
					// only reveal directories that are storage cards from the root

					if (StartsWith(filename, CF_CARD, 1) != 0)
					{
						return 1;
					}
					if (StartsWith(filename, SD_CARD, 1) != 0)
					{
						return 1;
					}
					if (StartsWith(filename, MMC_CARD, 1) != 0)
					{
						return 1;
					}
					if (StartsWith(filename, BUILT_IN_STORAGE, 1) != 0)
					{
						return 1;
					}
					if (StartsWith(filename, STORAGE, 1) != 0)
					{
						return 1;
					}

					return 0;
				}
		#endif
		*/

		return 1;
}

