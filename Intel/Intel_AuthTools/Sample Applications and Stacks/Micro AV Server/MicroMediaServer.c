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
 * $Workfile: MicroMediaServer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UpnpMicroStack.h"
#include "ILibParsers.h"
#include "MicroMediaServer.h"
#include "MyString.h"
#include "PortingFunctions.h"
#include "MimeTypes.h"

#ifdef UNDER_CE
#define strnicmp _strnicmp
#define assert ASSERT
#endif

#ifdef WIN32
#ifndef UNDER_CE
#include "assert.h"
#endif
#endif

#ifdef _POSIX
#include "assert.h"
#define strnicmp strncasecmp
#endif

#ifdef _DEBUG
#define MMS_MALLOC mms_malloc
#define MMS_FREE mms_free
#else
#define MMS_MALLOC malloc
#define MMS_FREE free
#endif

int mms_malloc_counter = 0;

void* mms_malloc(int sz)
{
	++mms_malloc_counter;
	return((void*)malloc(sz));
}
void mms_free(void* ptr)
{
	--mms_malloc_counter;
	free(ptr);	
}

/************************************************************************************/
/* START SECTION - Configuration info for the media server. */

/* This value should be one of the xxx_DIR_DELIMITER_STR values defined in the FileSystem configuration section. */
char* DIRDELIMITER;

/* The value of the shared root path. String value and its corresponding length variable initialized through SetRootPath(). */
char* ROOTPATH;
int ROOTPATHLENGTH;

/* MMS Stats */
void (*MmsOnStatsChanged) (void) = NULL;
void (*MmsOnTransfersChanged) (int) = NULL;
int MmsBrowseCount = 0;
int MmsHttpRequestCount = 0;
int MmsCurrentTransfersCount = 0;
struct MMSMEDIATRANSFERSTAT MmsMediaTransferStats[DOWNLOAD_STATS_ARRAY_SIZE];
void *MMS_Chain;
void *MMS_MicroStack;
/* END SECTION - Internal state variables and configuration for the media server. */
/************************************************************************************/


/************************************************************************************/
/* START SECTION - Stuff specific to FileSystem stuff. */

/* FileName To Didl struct */
struct FNTD
{
	char*	DirDelimiter;	/* Delimiter used for directory names */
	char*	Root;			/* Root path. */
	int		RootLength;		/* Length of Root */

    char*	Filter;			/* Comma separated list of tags to include. Use the * char to indicate all fields. NULL indicates minimum.*/
	unsigned int SI;		/* Starting index */
	unsigned int RC;		/* Requested count */
	unsigned int CI;		/* Current index - used internally. */
	unsigned int NR;		/* OUT: Number returned in DIDL response. used internally. */
	unsigned int TM;		/* OUT: Total number of matches. used internally */
	unsigned long UpdateID;	/* OUT: UpdateID. used internally. */

	FILE*	File;			/* Print DIDL here if non-NULL. Works in conjuction with Socket. */
	char*	String;			/* strcat DIDL here if non-NULL */
	int		FileSize;		/* length of the file in bytes - used for res@size */

	const char*	ArgName;		/* Needed if UpnpToken != NULL */
	void*	UpnpToken;		/* Print DIDL here if non-NULL. Works in conjunction with File. */

	char* BaseUri;			/* http://[ip address]:[port]/dir */
};

/* CHAR and STRING defines for Win32 directory delimiter */
#define WIN32_DIR_DELIMITER_CHR '\\'
#define WIN32_DIR_DELIMITER_STR "\\"

/* CHAR and STRING defines for UNIX directory delimiter */
#define UNIX_DIR_DELIMITER_CHR '/'
#define UNIX_DIR_DELIMITER_STR "/"

#define MAX_PATH_LENGTH 1024

/* #define stuff used for ProcessDir */
#define RECURSE_NEVER			0	/* Never recurse */
#define RECURSE_WHEN_FOUND		1	/* Recurse directories immediately when entry found. */
#define RECURSE_AFTER_PROCESS	2	/* NOT IMPLEMENTED: Recurse subdirs after done processing entries in the directory. */
#define RECURSE_BEFORE_PROCESS	3	/* NOT IMPLEMENTED: Recurse subdirs before processing any entries in the directory.*/
#define PROCESS_WHEN_FOUND	0	/* Process the entry immediately. */
#define PROCESS_DIRS_FIRST	1	/* Process directory entries first. */
#define PROCESS_FILE_FIRST	2	/* Process file entries first. */

/* Returns file extension for ASCII-encoded paths. */
char* GetFileExtension(char* pathName, int returnCopy)
{
	int len;
	int i;

	len = (int) strlen(pathName);

	for (i = len-1; i >= 0; i--)
	{
		if (
			(WIN32_DIR_DELIMITER_CHR == pathName[i]) ||
			(UNIX_DIR_DELIMITER_CHR == pathName[i])
			)
		{
			return NULL;
		}

		if ('.' == pathName[i])
		{
			if (returnCopy == 0)
			{
				return pathName+i;
			}
			else
			{
				char* deepCopy = (char*) MMS_MALLOC(len+1);
				strcpy(deepCopy, pathName);
				return deepCopy;
			}
		}
	}

	return NULL;
}

int GetLastIndexOfParentPath(char* pathName, char* dirDelimiter, int includeDelimiter)
{
	int dLen;
	int pLen;
	int i, j, ij, len;
	int foundDelim = 0;

	dLen = (int) strlen(dirDelimiter);
	pLen = (int) strlen(pathName);

	for (i = pLen; i >= 0; i--)
	{
		for (j = 0; j < dLen; j++)
		{
			ij = i+j;
			if (i+j < pLen)
			{
				if (dirDelimiter[j] != pathName[ij])
				{
					/* if a delimiter char doesn't match, then go to pathName[i-1] */
					break;
				}
				if (j == dLen-1)
				{
					/* make sure we didn't find a delimiter that ends pathName */
					if (i+dLen < pLen)
					{
						foundDelim = 1;
					}
				}
			}
		}

		if (0 != foundDelim)
		{
			if (0 != includeDelimiter)
			{
				len = i+dLen;
			}
			else
			{
				len = i;
			}

			return len-1;
		}
	}

	return -1;
}

/* Returns parent path of ASCII encoded path */
char* GetParentPath(char* pathName, char* dirDelimiter, int includeDelimiter)
{
	char* substring;
	int pos;

	pos = GetLastIndexOfParentPath(pathName, dirDelimiter, includeDelimiter) + 1;
	if (pos >= 0)
	{
		substring = (char*) MMS_MALLOC(pos+1);
		strncpy(substring, pathName, pos);
		substring[pos] = '\0';
		return substring;
	}
	return NULL;
}

/* Returns filename for ascii-encoded path */
char* GetFileName(char* pathName, char* dirDelimiter, int returnExtension)
{
	int pos = GetLastIndexOfParentPath(pathName, dirDelimiter, 1);
	int pLen = (int) strlen(pathName);
	int len = pLen - pos;
	int dotPos;
	char* name;
	int i,j;
	int nlen;

	if (returnExtension == 0)
	{
		dotPos = LastIndexOf(pathName, ".");
		if ((dotPos >= 0) && (dotPos >= pos))
		{
			len = len - (pLen-dotPos);
		}
	}

	name = (char*)MMS_MALLOC(len+1);
	j=pos+1;
	nlen = len - 1;
	for (i=0; i < len; i++)
	{
		name[i] = pathName[j];
		j++;
	}
	name[nlen] = '\0';

	if (EndsWith(name, dirDelimiter, 0))
	{
		name[nlen-(int) strlen(dirDelimiter)] = '\0';
	}

	return name;
}

void ProcessDir(char* dir, int processWhen, int recurseWhen, void (*callForEachFile)(char*, void*), void* arg)
{
	char path[MAX_PATH_LENGTH];
	char filename[MAX_PATH_LENGTH];
	void* dirObj;
	int ewDD;
	struct FNTD* fntd = (struct FNTD*) arg;
	int nextFile = 0;
	int fileSize = 0;

	if (callForEachFile == NULL) return;

	dirObj = PCGetDirFirstFile(dir,dir,filename,MAX_PATH_LENGTH,&fileSize);

	if (dirObj == NULL)
	{
		fprintf(stderr, "ProcessDir: can't open %s\n", dir);
		return;
	}

	ewDD = EndsWith(dir, DIRDELIMITER, 0);

	do
	{
		if (ProceedWithDirEntry(dir, filename, MAX_PATH_LENGTH) != 0)
		{

			/* path is acceptable length */

			if (ewDD != 0)
			{
				sprintf(path, "%s%s", dir, filename);
			}
			else
			{
				sprintf(path, "%s%s%s", dir, DIRDELIMITER, filename);
			}

			fntd->FileSize = fileSize;
			(callForEachFile)(path, fntd);
		}

		nextFile = PCGetDirNextFile(dirObj,dir,filename,MAX_PATH_LENGTH, &fileSize);
	}
	while (nextFile != 0);

	PCCloseDir(dirObj);
}

/* END SECTION - Stuff specific to FileSystem stuff. */
/************************************************************************************/


/************************************************************************************/
/* START SECTION - Virtual Directory stuff*/

struct WebRequest
{
	void* UpnpToken;
	
	char* Directive;
	int DirectiveLen;

	char* DirectiveObj;
	int DirectiveObjLen;

	char* UnescapedDirective;
	int UnescapedDirectiveLen;
	
	char* Root;
	int RootLen;
	
	char* DirDelimiter;
	int DirDelimLen;
	
	char* VirtualDir;
	int VirDirLen;
};

char MungeHexDigit(char* one_hexdigit)
{
	char r = -1;
	char c = *one_hexdigit;
	if (c >= '0' && c <= '9')
	{
		r = c - '0';
	}
	else if (c >= 'A' && c <= 'F')
	{
		r = c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'F')
	{
		r = c - 'a' + 10;
	}

	return r;
}

char GetCharFromHex(char* two_hexdigits)
{
	char c1 = MungeHexDigit(two_hexdigits);
	char c2 = MungeHexDigit(two_hexdigits+1);

	char result = 0;
	if (c1 != -1 && c2 != -1)
	{
		result = (c1 << 4) + c2;
	}
	return result;
}

#define SENDSIZE 32768
char *MMS_STRING_ROOT = "Root";
char *MMS_STRING_UNKNOWN = "Unknown";
char *MMS_STRING_RESULT = "Result";
void* HandleWebRequest(void* webRequest)
{
	struct WebRequest* wr;
	int ewSlash;
	char* lastChar;
	char* dj;
	char* di;
	int copied, slashCount, si;
	int fpBufLen;
	char* fullPath = NULL, *fp, *ud;
	int k;
	char* ext;
	char* ct;
	int cl = 0;
	char buf[SENDSIZE];
	int dirEntryType = 0;
	FILE* f;
	int totalSent;
	int sendStatus;
	int numRead;
	int incompleteSend = 0;
	int fpLen,fpSize;
	int transferStatIndex = -1;
	char *ansiPath = NULL;

	MmsCurrentTransfersCount++;
	if (MmsOnStatsChanged != NULL) MmsOnStatsChanged();

	wr = (struct WebRequest*) webRequest;

	/* remove trailing slash from directive, if present */
#ifdef _DEBUG
	printf("\r\nDirective1='%s'", wr->Directive);
#endif
	ewSlash = EndsWith(wr->Directive, "/", 0);
	if (ewSlash != 0)
	{
		wr->Directive[wr->DirectiveLen-1] = '\0';
		wr->DirectiveLen = wr->DirectiveLen - 1;
	}
#ifdef _DEBUG
	printf("\r\nDirective2='%s'", wr->Directive);
#endif

	/* convert from escaped HTTP directive to unescaped directive */
	wr->UnescapedDirective = (char*) MMS_MALLOC (wr->DirectiveLen+1);
	wr->UnescapedDirectiveLen = wr->DirectiveLen;
	lastChar = wr->Directive + wr->DirectiveLen;
	dj = wr->Directive;
	di = wr->UnescapedDirective;

	while (dj < lastChar)
	{
		copied = 0;
		if (*dj == '%')
		{
			char r = GetCharFromHex(dj+1);

			if (r != 0)
			{
				*di = r;
				dj = dj + 2;
				copied = 1;
			}
		}

		if (copied == 0)
		{
			*di = *dj;
		}
		dj = dj + 1;
		di = di + 1;
	}
	*di = '\0';
	wr->UnescapedDirectiveLen = (int) (di - wr->UnescapedDirective);
	printf("\r\nUnescapedDirective='%s' %d=%d\r\n", wr->UnescapedDirective, wr->UnescapedDirectiveLen, (int) strlen(wr->UnescapedDirective));


	/* determine the full local path where the directive should map to */
	slashCount = 0;

	for (si=0; si <wr->UnescapedDirectiveLen; si++)
	{
		if (wr->UnescapedDirective[si] == '/')
		{
			slashCount = slashCount + 1;
		}
	}
	fpBufLen = wr->RootLen +wr->UnescapedDirectiveLen + (slashCount*wr->DirDelimLen) + 1;
	fullPath = (char*) MMS_MALLOC (fpBufLen);
	sprintf(fullPath, "%s", wr->Root);

	fp = fullPath + wr->RootLen;
	ud = wr->UnescapedDirective + wr->VirDirLen + 2;


	while (*ud != '\0')
	{
		if (*ud == '/')
		{
			for (k=0; k < wr->DirDelimLen; k++)
			{
				*fp = wr->DirDelimiter[k];
			}
			fp = fp + k;
		}
		else
		{
			*fp = *ud;
			fp = fp + 1;
		}

		ud = ud + 1;
	}
	*fp = '\0';

	fpLen = (int) strlen(fullPath);
	printf("Requesting='%s' strlen='%d'\r\n", fullPath, fpLen);

	fpSize = fpLen+1;
	ansiPath = (char*) MMS_MALLOC(fpSize);
	Utf8ToAnsi(ansiPath, fullPath, fpSize);

	for (k=0;k<20;k++)
	{
		if (MmsMediaTransferStats[k].filename == NULL)
		{
			transferStatIndex = k;
			MmsMediaTransferStats[transferStatIndex].filename = fullPath;
			MmsMediaTransferStats[transferStatIndex].download = 1;
			MmsMediaTransferStats[transferStatIndex].length = 0;
			MmsMediaTransferStats[transferStatIndex].position = 0;
			if (MmsOnTransfersChanged != NULL) MmsOnTransfersChanged(transferStatIndex);
			break;
		}
	}

	/* determine if the path is a file or a directory, or nonexistent */

	dirEntryType = PCGetFileDirType(fullPath);

	if (dirEntryType > 0)
	{
		if (dirEntryType == 1)
		{
			/* is a file */
			printf("\r\nFound='%s' strlen='%d'\r\n", fullPath, (int) strlen(ansiPath));
			/* otherwise, just send the file */

			ext = (char*) GetFileExtension(ansiPath, 0);
			ct = (char*) FileExtensionToMimeType(ext);

			if (*ct == '\0') { ct = "application/octet-stream"; }

			f = fopen(ansiPath, "rb");

			if (transferStatIndex != -1)
			{
				#ifdef _WIN32_WCE
				fseek(f,0,SEEK_END);
				MmsMediaTransferStats[transferStatIndex].length = ftell(f);
				fseek(f,0,SEEK_SET);
				#endif

				#ifdef WIN32
				fseek(f,0,SEEK_END);
				MmsMediaTransferStats[transferStatIndex].length = ftell(f);
				fseek(f,0,SEEK_SET);
				#endif
			}

			totalSent =0;
			sendStatus = 0;
			if (f != NULL)
			{
				cl = PCGetFileSize(fullPath);
				totalSent = sprintf(buf, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\nServer: Intel CEL / MicroMediaServer\r\nAccept-Range: bytes\r\nContent-Type: %s\r\n\r\n", cl, ct);
				sendStatus = UpnpPresentationResponse (wr->UpnpToken, buf, totalSent, 0);

				if (sendStatus == totalSent)
				{
					totalSent = 0;
					while ( (numRead = (int)fread(buf, 1, SENDSIZE, f)) > 0)
					{
						if (transferStatIndex != -1)
						{
							MmsMediaTransferStats[transferStatIndex].position = totalSent;
							if (MmsOnTransfersChanged != NULL) MmsOnTransfersChanged(transferStatIndex);
						}

						sendStatus = UpnpPresentationResponse (wr->UpnpToken, buf, numRead, 0);

						if (sendStatus != numRead)
						{
							printf("Send status#1=0x%x\r\n", sendStatus);
							incompleteSend = 1;
							break;
						}
						else
						{
							totalSent = totalSent + numRead;
						}
					}
				}
				else
				{
					printf("Send status#0=0x%x\r\n", sendStatus);
					incompleteSend = 1;
				}

				if (incompleteSend == 0)
				{
					printf("Successfully sent all %d bytes\r\n", totalSent);
				}
				else
				{
					printf("Only sent %d bytes\r\n", totalSent);
				}

				UpnpPresentationResponse (wr->UpnpToken, "", 0, 1);
				fclose(f);
			}
			else
			{
				UpnpPresentationResponse (wr->UpnpToken, "HTTP/1.0 404 File Not Found\r\n\r\n" , 31 , 1);
			}
		}
		else
		{
			UpnpPresentationResponse (wr->UpnpToken, "HTTP/1.0 404 File Not Found\r\n\r\n" , 31 , 1);
		}
	}
	else
	{
		UpnpPresentationResponse (wr->UpnpToken, "HTTP/1.0 404 File Not Found\r\n\r\n" , 31 , 1);
	}

	if (transferStatIndex != -1)
	{
		MmsMediaTransferStats[transferStatIndex].filename = NULL;
		MmsMediaTransferStats[transferStatIndex].download = 0;
		MmsMediaTransferStats[transferStatIndex].length = 0;
		MmsMediaTransferStats[transferStatIndex].position = 0;
		if (MmsOnTransfersChanged != NULL) MmsOnTransfersChanged(transferStatIndex);
	}

	if (ansiPath != NULL) MMS_FREE (ansiPath);
	if (fullPath != NULL) MMS_FREE (fullPath);
	if (wr->UnescapedDirective != NULL) MMS_FREE (wr->UnescapedDirective);
	if (wr->Directive != NULL) MMS_FREE (wr->Directive);
	if (wr != NULL) MMS_FREE (wr);

	MmsCurrentTransfersCount--;
	if (MmsOnStatsChanged != NULL) MmsOnStatsChanged();

	EndThisThread();
	return NULL;
}

/* END SECTION - Virtual Directory stuff */
/************************************************************************************/


/************************************************************************************/
/* START SECTION - Stuff specific to CDS */

/* BrowseFlags */
#define BROWSEMETADATA "BrowseMetadata"
#define BROWSEDIRECTCHILDREN "BrowseDirectChildren"

/* DIDL formating */
#define DIDL_HEADER "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp\">"
#define DIDL_FOOTER "\r\n</DIDL-Lite>"

#define DIDL_HEADER_ESCAPED "&lt;DIDL-Lite xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite&quot; xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp&quot;&gt;"
#define DIDL_FOOTER_ESCAPED "\r\n&lt;/DIDL-Lite&gt;"
#define DIDL_HEADER_ESCAPED_LEN 195
#define DIDL_FOOTER_ESCAPED_LEN 20

char* DIDL_ITEM_ALL = "\r\n<item id=\"%s\" parentID=\"%s\" restricted=\"true\">\r\n\t<dc:title>%s</dc:title>\r\n\t<upnp:class>%s</upnp:class>\r\n\t<res protocolInfo=\"http-get:*:%s:*\" size=\"%d\">%s/%s</res>\r\n</item>";
char* DIDL_ITEM_NOSIZE = "\r\n<item id=\"%s\" parentID=\"%s\" restricted=\"true\">\r\n\t<dc:title>%s</dc:title>\r\n\t<upnp:class>%s</upnp:class>\r\n\t<res protocolInfo=\"http-get:*:%s:*\">%s/%s</res>\r\n</item>";
char* DIDL_ITEM_MIN = "\r\n<item id=\"%s\" parentID=\"%s\" restricted=\"true\">\r\n\t<dc:title>%s</dc:title>\r\n\t<upnp:class>%s</upnp:class>\r\n</item>";
char* DIDL_CONTAINER_ALL = "\r\n<container id=\"%s\" parentID=\"%s\" restricted=\"true\">\r\n\t<dc:title>%s</dc:title>\r\n\t<upnp:class>object.container</upnp:class>\r\n</container>";

/* MAX length for a DIDL entry should be 1024 */
#define DIDL_ENTRY_LEN 1024

struct BrowseRequest
{
	void* UpnpToken;	/* no copy */
	char* ObjectID;		/* deep copied */
	char* BrowseFlag;	/* deep copied */
	char* Filter;		/* deep copied */
	unsigned int StartingIndex;
	unsigned int RequestedCount;
	char* SortCriteria;	/* deep copied */
	char* BaseUri;		/* must MMS_FREE */
};

void InitFntd(struct FNTD* fntd)
{
	/*fntd->Extensions = NULL;*/
	/*fntd->MimeTypes = NULL;*/
	fntd->Root = NULL;
	fntd->RootLength = -1;
	fntd->DirDelimiter = NULL;

	fntd->Filter = NULL;
	fntd->SI = 0;
	fntd->RC = 0;
	fntd->CI = 0;
	fntd->NR = 0;
	fntd->TM = 0;
	fntd->UpdateID = 0;

	fntd->File = NULL;
	fntd->String = NULL;
	fntd->UpnpToken = NULL;
	fntd->ArgName = NULL;

	fntd->BaseUri = NULL;
}

void EscapeString(char* dest, char* src, int srcLen)
{
	char* memLoc = dest;
	int i;

	for (i=0; i < srcLen; i++)
	{
		if (src[i] == '&')
		{
			memcpy(memLoc, "&amp;", 5);
			memLoc = memLoc + 5;
		}
		else if (src[i] == '<')
		{
			memcpy(memLoc, "&lt;", 4);
			memLoc = memLoc + 4;
		}
		else if (src[i] == '>')
		{
			memcpy(memLoc, "&gt;", 4);
			memLoc = memLoc + 4;
		}
		else
		{
			memLoc[0] = src[i];
			memLoc = memLoc + 1;
		}
	}
	memLoc[0] = '\0';
}

void DirectoryEntryToDidl(char* pathName, struct FNTD* fntd)
{
	char* ext = NULL;
	char* upnpClass = NULL;
	char* mime = NULL;
	char entry[DIDL_ENTRY_LEN];
	char* parentDir = NULL;
	char* parentID = NULL;
	int fnLen;
	char* title = NULL;
	int ddLen;
	int idLen;
	int pidLen;
	char* id = NULL;
	int j;
	int escapedIDLen;
	int escapedIDSize;
	char* escapedID = NULL;
	int parentIDLen;
	int escapedParentIDLen;
	int escapedParentIDSize;
	char* escapedParentID = NULL;
	char* escapedPath = NULL;
	int escapedPathLen;
	int escapedPathSize;
	int titleLen;
	int escapedTitleLen;
	int escapedTitleSize;
	char* escapedTitle = NULL;
	int pos;
	int filesize;
	int ewDD;
	char* baseUri = NULL;
	int pathLen;
	int fdType = 0xFFFFFFFF;
	char* pchar;
	int escapeLen;
	char *escapeStr;
	int len2;

	if (pathName == NULL) return;
	if (fntd == NULL) return;

	baseUri = fntd->BaseUri;
	fdType = PCGetFileDirType(pathName);

	if (fdType == 2)
	{
		upnpClass = "object.container";
		mime = "";
		ext = "";
	}
	else
	{
		ext = GetFileExtension(pathName, 0);
		if (ext != NULL)
		{
			upnpClass = (char*) FileExtensionToUpnpClass(ext);
			mime = (char*) FileExtensionToMimeType(ext);
		}
	}

	if ((fntd->CI >= fntd->SI) && ((fntd->NR < fntd->RC) || (fntd->RC == 0)))
	{
		memset(entry, 0, DIDL_ENTRY_LEN);

		/* get parentID and parent directory */
		parentDir = NULL;
		parentID = NULL;

		if (strcmp(pathName, fntd->Root) == 0)
		{
			parentID = (char*) MMS_MALLOC(3);
			strcpy(parentID, "-1");
		}
		else
		{
			parentDir = GetParentPath(pathName, fntd->DirDelimiter, 1);

			if (strcmp(parentDir, fntd->Root) == 0)
			{
				parentID = (char*) MMS_MALLOC(2);
				strcpy(parentID, "0");
			}
			else
			{
				parentID = (char*) MMS_MALLOC((int) strlen(parentDir) + 3);
				sprintf(parentID, "0%s%s", fntd->DirDelimiter, parentDir+fntd->RootLength);
				pidLen = (int) strlen(parentID);
				parentID [pidLen-1] = '\0';
				pidLen = pidLen - 1;
			}
		}

		/* get title*/
		fnLen = (int) strlen(pathName);
		title = GetFileName(pathName, fntd->DirDelimiter, 0);

		if (title[0] == '\0')
		{
			MMS_FREE(title);
			if (strnicmp(pathName, fntd->Root, (int) strlen(pathName)) == 0)
			{
				title = MMS_STRING_ROOT;
			}
			else
			{
				title = MMS_STRING_UNKNOWN;
			}
		}

		/* get object id: full path, where root=0 */
		ddLen = (int) strlen(fntd->DirDelimiter);
		idLen = fnLen - fntd->RootLength + 1 + ddLen;
		id = (char*) MMS_MALLOC (idLen+1);
		j = fntd->RootLength;
		pchar = pathName+j;

		if (pathName[j] == '\0')
		{
			sprintf(id, "0");
		}
		else
		{
			/*
			sprintf(id, "0%s", fntd->DirDelimiter);
			for (i = ddLen+1; i < idLen; i++)
			{
				if (j < fnLen)
				{
					id[i] = pathName[j];
					j++;
				}
			}
			id[idLen] = '\0';
			*/
			sprintf(id, "0%s%s", fntd->DirDelimiter, pchar);
		}

		/* determine if directory or file */
		if ((id != NULL) && (parentID != NULL) && (title != NULL))
		{
			idLen = (int) strlen(id);
			escapedIDSize = idLen*5 + 1;
			escapedID = (char*) MMS_MALLOC(escapedIDSize);
			EscapeString(escapedID, id, idLen);
			escapedIDLen = (int) strlen(escapedID);
			ewDD = EndsWith(escapedID, DIRDELIMITER, 0);
			if (ewDD != 0)
			{
				escapedID[escapedIDLen-1] = '\0';
				escapedIDLen--;
			}

			parentIDLen = (int) strlen(parentID);
			escapedParentIDSize = parentIDLen*5+1;
			escapedParentID = (char*) MMS_MALLOC(escapedParentIDSize);
			EscapeString(escapedParentID, parentID, parentIDLen);
			escapedParentIDLen = (int) strlen(escapedParentID);
			ewDD = EndsWith(escapedParentID, DIRDELIMITER, 0);
			if (ewDD != 0)
			{
				escapedParentID[escapedParentIDLen-1] = '\0';
				escapedParentIDLen--;
			}

			titleLen = (int) strlen(title);
			escapedTitleSize = titleLen*5+1;
			escapedTitle = (char*) MMS_MALLOC(escapedTitleSize);
			EscapeString(escapedTitle, title, titleLen);
			escapedTitleLen = (int) strlen(escapedTitle);

			pathLen = (int) strlen(pathName);
			escapedPathSize = pathLen*5+1;
			escapedPath = (char*) MMS_MALLOC(escapedPathSize);
			EscapeString(escapedPath, pathName+ROOTPATHLENGTH, pathLen);
			escapedPathLen = (int) strlen(escapedPath);

			if (strcmp(UNIX_DIR_DELIMITER_STR, DIRDELIMITER) != 0)
			{
				int x;
				for (x=0; x < escapedPathLen; x++)
				{
					if (escapedPath[x] == WIN32_DIR_DELIMITER_CHR)
					{
						escapedPath[x] = UNIX_DIR_DELIMITER_CHR;
					}
				}
			}

			if (PCGetFileDirType(pathName) == 2)
			{
				if (title != NULL)
				{
					sprintf(entry, DIDL_CONTAINER_ALL, escapedID, escapedParentID, escapedTitle);
					ext = "NOT-NULL";
				}
			}
			else
			{
				if (ext != NULL)
				{
					if ((upnpClass != NULL) && (mime != NULL) && (baseUri != NULL) && (escapedPath != NULL))
					{
						filesize = fntd->FileSize;
						if (fntd->Filter == NULL)
						{
							sprintf(entry, DIDL_ITEM_MIN, escapedID, escapedParentID, escapedTitle, upnpClass);
						}
						else
						{
							pos = IndexOf(fntd->Filter, "*");
							if (pos >= 0)
							{
								if (fntd->FileSize >= 0)
								{
									sprintf(entry, DIDL_ITEM_ALL, escapedID, escapedParentID, escapedTitle, upnpClass, mime, filesize, baseUri, escapedPath);
								}
								else
								{
									sprintf(entry, DIDL_ITEM_NOSIZE, escapedID, escapedParentID, escapedTitle, upnpClass, mime, baseUri, escapedPath);
								}
							}
							else
							{
								int showRes = 0;	/*0: dont' show res; 1: show res, hide size; 2: show res and size*/

								pos = IndexOf(fntd->Filter, "res");

								if (pos >= 0)
								{
									/* show res but hide size */
									showRes = 1;
								}
								else
								{
									/* check for protocolInfo */
									if (IndexOf(fntd->Filter, "@protocolInfo") >= 0)
									{
										showRes = 1;
									}
								}

								/* check for size */
								pos = IndexOf(fntd->Filter, "@size"); 
								if (pos < 0) { pos = IndexOf(fntd->Filter, "res@size"); }

								if (showRes == 0)
								{
									/* we did not find "res" in first scan */

									if (pos >= 0)
									{
										/* but we did find @size or res@size in following scans */
										showRes = 2;
									}
									else
									{
										/* we didn't find scan */
										showRes = 0;
									}
								}
								else
								{
									/* we know we need to show res, but do we show size too? */

									if (pos >= 0)
									{
										/* show size too */
										showRes = 2;
									}
									else
									{
										/* only show res */
										showRes = 1;
									}
								}

								if (showRes == 1)
								{
									sprintf(entry, DIDL_ITEM_NOSIZE, escapedID, escapedParentID, escapedTitle, upnpClass, mime, baseUri, escapedPath);
								}
								else if (showRes == 2)
								{
									if (fntd->FileSize >= 0)
									{
										sprintf(entry, DIDL_ITEM_ALL, escapedID, escapedParentID, escapedTitle, upnpClass, mime, filesize, baseUri, escapedPath);
									}
									else
									{
										sprintf(entry, DIDL_ITEM_NOSIZE, escapedID, escapedParentID, escapedTitle, upnpClass, mime, baseUri, escapedPath);
									}
								}
								else
								{
									sprintf(entry, DIDL_ITEM_MIN, escapedID, escapedParentID, escapedTitle, upnpClass);
								}
							}
						}
					}
				}
			}

			MMS_FREE(escapedTitle);
			MMS_FREE(escapedParentID);
			MMS_FREE(escapedID);
			MMS_FREE(escapedPath);
		}

		/* print the entry to the file, socket, and string */
		if (entry[0] != '\0')
		{
			if (fntd->File != NULL)
			{
				fprintf(fntd->File, "%s", entry);
			}

			if (fntd->String != NULL)
			{
				/* BUGBUG: For some reason the code only works consistently if I stick this printf here. */
				printf("");
				strcat(fntd->String, entry);
			}

			if (fntd->UpnpToken != NULL)
			{
				escapeLen = ILibXmlEscapeLength(entry);
				escapeStr = MMS_MALLOC(escapeLen+1);
				len2 = ILibXmlEscape(escapeStr, entry);
				assert(len2 == escapeLen);
				escapeStr[escapeLen] = '\0';

				UpnpAsyncResponse_OUT(fntd->UpnpToken, fntd->ArgName, escapeStr, escapeLen, 0, 0);

				MMS_FREE(escapeStr);
			}

			fntd->NR = fntd->NR + 1;
		}

		/* Clean up */
		if (parentID != NULL) MMS_FREE(parentID);
		if (parentDir != NULL) MMS_FREE(parentDir);
		if (id != NULL) MMS_FREE(id);
		if ((title != MMS_STRING_UNKNOWN) && (title != MMS_STRING_ROOT) && (title != NULL)) MMS_FREE(title);
	}

	/*
	 *	TODO: determine updateID using date of last modify */
	/*
	struct tm* _tm = gmtime(&_si.st_mtime);
	unsigned int t_modified = (_tm->tm_yday * (24 * 60 * 60)) + (_tm->tm_hour*60) + (_tm->tm_sec);
	if (fntd->UpdateID < t_modified)
	{
		fntd->UpdateID = t_modified;
	}
	*/

	/* increment total matches */
	if ((ext != NULL) && (upnpClass != NULL) && (mime != NULL))
	{
		fntd->TM = fntd->TM + 1;
		fntd->CI = fntd->CI + 1;
	}
}

void DirectoryEntryToDidl2(char* pathName, void* fntd)
{
	DirectoryEntryToDidl(pathName, (struct FNTD*)fntd);
}

void BrowseDirectChildren(void* upnpToken, char* dirPath, char* Filter, unsigned int StartingIndex, unsigned int RequestedCount, char* baseUri)
{
	struct FNTD fntd;
	char numResult[30];

	InitFntd(&fntd);

	fntd.DirDelimiter = DIRDELIMITER;
	fntd.Root = ROOTPATH;
	fntd.RootLength = ROOTPATHLENGTH;
	
	fntd.BaseUri = baseUri;

	fntd.Filter = Filter;
	fntd.SI = StartingIndex;
	fntd.RC = RequestedCount;

	/*fntd.File = stdout;*/
	fntd.UpnpToken = upnpToken;
	fntd.ArgName = MMS_STRING_RESULT;
	
	UpnpAsyncResponse_START(upnpToken, "Browse", "urn:schemas-upnp-org:service:ContentDirectory:1");
	
	UpnpAsyncResponse_OUT(upnpToken, MMS_STRING_RESULT, DIDL_HEADER_ESCAPED, DIDL_HEADER_ESCAPED_LEN,1, 0);
	ProcessDir(dirPath, RECURSE_NEVER, PROCESS_WHEN_FOUND, DirectoryEntryToDidl2, &fntd);
	UpnpAsyncResponse_OUT(upnpToken, MMS_STRING_RESULT, DIDL_FOOTER_ESCAPED, DIDL_FOOTER_ESCAPED_LEN,0, 1);

	sprintf(numResult, "%d", fntd.NR);
	UpnpAsyncResponse_OUT(upnpToken, "NumberReturned", numResult, (int) strlen(numResult), 1,1);

	sprintf(numResult, "%d", fntd.TM);
	UpnpAsyncResponse_OUT(upnpToken, "TotalMatches", numResult, (int) strlen(numResult), 1,1);

	fntd.UpdateID = 0;
	sprintf(numResult, "%u", fntd.UpdateID);
	UpnpAsyncResponse_OUT(upnpToken, "UpdateID", numResult, (int) strlen(numResult), 1,1);

	UpnpAsyncResponse_DONE(upnpToken, "Browse");
}

void BrowseMetadata(void* upnpToken, char* pathName, char* Filter, char* baseUri)
{
	struct FNTD fntd;
	struct FNTD fntd2;
	char numResult[30];
	int filesize;

	/*
	if (EndsWith(pathName, DIRDELIMITER))
	{
		pathName[(int) strlen(pathName) - (int) strlen(DIRDELIMITER)] = '\0';
	}
	*/

	filesize = PCGetFileSize(pathName);

	InitFntd(&fntd);

	fntd.DirDelimiter = DIRDELIMITER;
	fntd.Root = ROOTPATH;
	fntd.RootLength = ROOTPATHLENGTH;
	
	fntd.BaseUri = baseUri;

	fntd.Filter = Filter;
	fntd.SI = 0;
	fntd.RC = 0;

	/* fntd.File = stdout; */
	fntd.UpnpToken = upnpToken;
	fntd.ArgName = MMS_STRING_RESULT;

	fntd.FileSize = filesize;

	/* get container updateID */
	InitFntd(&fntd2);
	fntd2.DirDelimiter = DIRDELIMITER;
	fntd2.Root = ROOTPATH;
	fntd2.RootLength = ROOTPATHLENGTH;
	fntd2.Filter = "";
	fntd2.UpdateID = 0;

	UpnpAsyncResponse_START(upnpToken, "Browse", "urn:schemas-upnp-org:service:ContentDirectory:1");
	
	UpnpAsyncResponse_OUT(upnpToken, MMS_STRING_RESULT , DIDL_HEADER_ESCAPED, DIDL_HEADER_ESCAPED_LEN,1, 0);
	DirectoryEntryToDidl (pathName, &fntd);
	UpnpAsyncResponse_OUT(upnpToken, MMS_STRING_RESULT , DIDL_FOOTER_ESCAPED, DIDL_FOOTER_ESCAPED_LEN,0, 1);

	UpnpAsyncResponse_OUT(upnpToken, "NumberReturned", "1", 1, 1, 1);
	UpnpAsyncResponse_OUT(upnpToken, "TotalMatches", "1", 1, 1, 1);

	sprintf(numResult, "%u", fntd2.UpdateID);
	UpnpAsyncResponse_OUT(upnpToken, "UpdateID", numResult, (int) strlen(numResult), 1, 1);

	UpnpAsyncResponse_DONE(upnpToken, "Browse");
}

void CdsBrowse(void* upnpToken, char* ObjectID, char* BrowseFlag, char* Filter, unsigned int StartingIndex, unsigned int RequestedCount, char* SortCriteria, char* baseUri)
{
	int objIdLen;
	char* filepath;
	char* errorMsg;

	if (ObjectID[0] != '0')
	{
		/* error */
		fprintf(stderr, "\r\nERROR: CdsBrowse() - ObjectID not found.");
		UpnpResponse_Error(upnpToken,701,"ObjectID does not exist.");
		return;
	}

	if (SortCriteria != NULL)
	{
		if (SortCriteria[0] != '\0')
		{
			/* error */
			fprintf(stderr, "\r\nERROR: CdsBrowse() - SortCriteria not allowed.");
			UpnpResponse_Error(upnpToken,709,"MediaServer does not support sorting.");
			return;
		}
	}

	/* translate the ObjectID into a usable path */
	objIdLen = ILibInPlaceXmlUnEscape(ObjectID);
	filepath = (char*)MMS_MALLOC(ROOTPATHLENGTH + objIdLen + 10);

	if (objIdLen > 2) 
	{
		if (BrowseFlag[6] == 'M')
		{
			sprintf(filepath,"%s%s",ROOTPATH,ObjectID+2);
		}
		else
		{
			sprintf(filepath,"%s%s%s",ROOTPATH,ObjectID+2,DIRDELIMITER);
		}
	} 
	else 
	{
		sprintf(filepath,"%s",ROOTPATH);
	}

	printf("CdsBrowse(): filepath=%s\r\n", filepath);

	switch (PCGetFileDirType(filepath))
	{
		case 0:	/* Does not exist */
			errorMsg = (char*) MMS_MALLOC((int) strlen(filepath)+100);
			sprintf(errorMsg, "GetFileAttributes(%s) failed", filepath);
			UpnpResponse_Error(upnpToken,800,errorMsg);
			MMS_FREE(errorMsg);
			break;
		case 1: /* Is a File */
			if (strcmp(BrowseFlag, BROWSEMETADATA) == 0)
			{
				BrowseMetadata(upnpToken, filepath, Filter, baseUri);
			}
			else if (strcmp(BrowseFlag, BROWSEDIRECTCHILDREN) == 0)
			{
				/* error */
				fprintf(stderr, "\r\nERROR: CdsBrowse() - BrowseFlag not allowed on an item.");
				UpnpResponse_Error(upnpToken,710,"ObjectID is not a container.");
			}
			break;
		case 2: /* Is a Directory */
			if (strcmp(BrowseFlag, BROWSEMETADATA) == 0)
			{
				BrowseMetadata(upnpToken, filepath, Filter, baseUri);
			}
			else
			{
				BrowseDirectChildren(upnpToken, filepath, Filter, StartingIndex, RequestedCount, baseUri);
			}
			break;
	}

	MMS_FREE(filepath);
}


void* DoBrowse(void* browseRequest)
{
	struct BrowseRequest* br = (struct BrowseRequest*) browseRequest;

	printf("DoBrowse()\r\n");

	LockBrowse();

	CdsBrowse(br->UpnpToken, br->ObjectID, br->BrowseFlag, br->Filter, br->StartingIndex, br->RequestedCount, br->SortCriteria, br->BaseUri);

	UnlockBrowse();

	MMS_FREE (br->ObjectID);
	MMS_FREE (br->BrowseFlag);
	if (br->Filter != NULL)
	{
		MMS_FREE (br->Filter);
	}
	if (br->SortCriteria != NULL)
	{
		MMS_FREE (br->SortCriteria);
	}
	MMS_FREE (br->BaseUri);
	MMS_FREE (br);

#ifdef _DEBUG
	printf(">mms_malloc_counter=%d\r\n", mms_malloc_counter);
#endif

#ifdef SPAWN_BROWSE_THREAD
	EndThisThread();
#endif

	return NULL;
}
/* END SECTION - Stuff specific to CDS */
/************************************************************************************/


/************************************************************************************/
/* START SECTION - Dispatch sinks generated in original main.c */

void UpnpContentDirectory_Browse(void* upnptoken,char* ObjectID,char* BrowseFlag,char* Filter,unsigned int StartingIndex,unsigned int RequestedCount,char* SortCriteria)
{
	int ipaddr;
	struct BrowseRequest* br;
	int filterLen, sortCriteriaLen;
	int portNum;

	// Update the statictics
	MmsBrowseCount++;
	if (MmsOnStatsChanged != NULL) MmsOnStatsChanged();

	printf("UPnP Invoke: UpnpContentDirectory_Browse('%s','%s','%s',%d,%d,'%s');\r\n",ObjectID,BrowseFlag,Filter,StartingIndex,RequestedCount,SortCriteria);

#ifdef _DEBUG
	printf("<mms_malloc_counter=%d\r\n", mms_malloc_counter);
#endif

	if (ObjectID == NULL)
	{
		UpnpResponse_Error(upnptoken,701,"ObjectID does not exist.");
		return;
	}

	if (BrowseFlag == NULL)
	{
		UpnpResponse_Error(upnptoken,402,"Invalid value for BrowseFlag. Value was null.");
		return;
	}

	/*spawn thread and respond*/
	br = (struct BrowseRequest*) MMS_MALLOC(sizeof (struct BrowseRequest));
	br->UpnpToken = upnptoken;

	br->ObjectID = (char*) MMS_MALLOC ((int) strlen(ObjectID)+1);
	strcpy(br->ObjectID, ObjectID);

	br->BrowseFlag = (char*) MMS_MALLOC ((int) strlen(BrowseFlag)+1);
	strcpy(br->BrowseFlag, BrowseFlag);
  
	if (Filter == NULL)
	{
		br->Filter = NULL;
	}
	else
	{
		filterLen = (int) strlen(Filter);
		br->Filter = (char*) MMS_MALLOC (filterLen+1);
		strcpy(br->Filter, Filter);
	}

	br->StartingIndex = StartingIndex;
	br->RequestedCount = RequestedCount;
	
	if (SortCriteria == NULL)
	{
		br->SortCriteria = NULL;
	}
	else
	{
		sortCriteriaLen = (int) strlen(SortCriteria);
		br->SortCriteria = (char*) MMS_MALLOC (sortCriteriaLen+1);
		strcpy(br->SortCriteria, SortCriteria);
	}

	ipaddr =  UpnpGetLocalInterfaceToHost(br->UpnpToken);
	br->BaseUri = (char*) MMS_MALLOC(1025);
	portNum = UpnpGetLocalPortNumber (UpnpGetInstance(br->UpnpToken));
	sprintf(br->BaseUri, "http://%d.%d.%d.%d:%d/web", (ipaddr&0xFF),((ipaddr>>8)&0xFF),((ipaddr>>16)&0xFF),((ipaddr>>24)&0xFF), portNum);

#ifdef SPAWN_BROWSE_THREAD
	SpawnNormalThread(&DoBrowse, (void*) br);
#else
	DoBrowse((void*) br);
#endif
	
}

void UpnpContentDirectory_GetSortCapabilities(void* upnptoken)
{
  printf("UPnP Invoke: UpnpContentDirectory_GetSortCapabilities();\r\n");

  /* TODO: Place Action Code Here... */

  /* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UpnpResponse_ContentDirectory_GetSortCapabilities(upnptoken,"");
}

void UpnpContentDirectory_GetSystemUpdateID(void* upnptoken)
{
  printf("UPnP Invoke: UpnpContentDirectory_GetSystemUpdateID();\r\n");

  /* TODO: Place Action Code Here... */

  /* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UpnpResponse_ContentDirectory_GetSystemUpdateID(upnptoken,0);
}

void UpnpContentDirectory_GetSearchCapabilities(void* upnptoken)
{
  printf("UPnP Invoke: UpnpContentDirectory_GetSearchCapabilities();\r\n");

  /* TODO: Place Action Code Here... */

  /* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UpnpResponse_ContentDirectory_GetSearchCapabilities(upnptoken,"");
}

void UpnpConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID)
{
  printf("UPnP Invoke: UpnpConnectionManager_GetCurrentConnectionInfo(%u);\r\n",ConnectionID);

  /* TODO: Place Action Code Here... */

  UpnpResponse_Error(upnptoken,706,"Specified connection does not exist.");
}

void UpnpConnectionManager_GetProtocolInfo(void* upnptoken)
{
  printf("UPnP Invoke: UpnpConnectionManager_GetProtocolInfo();\r\n");

  /* TODO: Place Action Code Here... */

  /* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UpnpResponse_ConnectionManager_GetProtocolInfo(upnptoken,"http-get:*:*:*","");
}

void UpnpConnectionManager_GetCurrentConnectionIDs(void* upnptoken)
{
  printf("UPnP Invoke: UpnpConnectionManager_GetCurrentConnectionIDs();\r\n");

  /* TODO: Place Action Code Here... */

  /* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
  UpnpResponse_ConnectionManager_GetCurrentConnectionIDs(upnptoken,"");
}

void UpnpPresentationRequest(void* upnptoken, struct packetheader *packet)
{
	struct WebRequest* webRequest = (struct WebRequest*) MMS_MALLOC(sizeof(struct WebRequest));
	webRequest->UpnpToken = upnptoken;

	MmsHttpRequestCount++;
	if (MmsOnStatsChanged != NULL) MmsOnStatsChanged();

	webRequest->Directive = (char*) MMS_MALLOC( packet->DirectiveObjLength + 1);
	strcpy(webRequest->Directive, packet->DirectiveObj);
	webRequest->DirectiveLen = packet->DirectiveObjLength;

	webRequest->Root = ROOTPATH;
	webRequest->RootLen = ROOTPATHLENGTH;

	webRequest->DirDelimiter = DIRDELIMITER;
	webRequest->DirDelimLen = (int) strlen(DIRDELIMITER);

	webRequest->VirtualDir = "web";
	webRequest->VirDirLen = (int) strlen(webRequest->VirtualDir);

	SpawnNormalThread(&HandleWebRequest, (void*) webRequest);
}

/* END SECTION - Dispatch sinks generated in original main.c */
/************************************************************************************/


/************************************************************************************/
/* START SECTION - MicroMediaServer interface implementations */

void InitMms(void* chain, void *stack, char *sharedRootPath)
{
	char* rootPath;
	int i, rootLen, ew;

	MMS_Chain = chain;
	MMS_MicroStack = stack;

	memset(MmsMediaTransferStats,0,sizeof(struct MMSMEDIATRANSFERSTAT)*20);

#ifdef SPAWN_BROWSE_THREAD
	InitBrowseLock();
#endif

	/* set up the root path settings */
	rootPath = sharedRootPath;
	rootLen = (int) strlen(rootPath);
	DIRDELIMITER = NULL;
	for (i = 0; i < rootLen; i++)
	{
		if (WIN32_DIR_DELIMITER_CHR == rootPath[i])
		{
			DIRDELIMITER = WIN32_DIR_DELIMITER_STR;
		}
		else if (UNIX_DIR_DELIMITER_CHR == rootPath[i])
		{
			DIRDELIMITER = UNIX_DIR_DELIMITER_STR;
		}
	}
	if (DIRDELIMITER == NULL)
	{
		DIRDELIMITER = UNIX_DIR_DELIMITER_STR;
	}
	ew = EndsWith(rootPath, DIRDELIMITER, 0);
	if (ew == 0)
	{
		ROOTPATH = (char*) malloc (rootLen+1+(int) strlen(DIRDELIMITER));
		sprintf(ROOTPATH, "%s%s", rootPath, DIRDELIMITER);
	}
	else
	{
		ROOTPATH = (char*) malloc (rootLen+1);
		strcpy(ROOTPATH, rootPath);
	}
	ROOTPATHLENGTH = (int) strlen(ROOTPATH);

	/* Taken from code-generated main.c - does state variable initialization */

	/* All evented state variables MUST be initialized before UPnPStart is called. */
	UpnpSetState_ConnectionManager_SourceProtocolInfo(MMS_MicroStack, "http-get:*:audio/mpeg:*,http-get:*:audio/x-ms-wma:*");
	UpnpSetState_ConnectionManager_SinkProtocolInfo(MMS_MicroStack, "");
	UpnpSetState_ConnectionManager_CurrentConnectionIDs(MMS_MicroStack, "");
	UpnpSetState_ContentDirectory_SystemUpdateID(MMS_MicroStack, 0);

	printf("Intel's UPnP MicroStack 1.0\r\nConnected & Extended PC Lab (CEL)\r\n\r\n");

#ifdef SPAWN_BROWSE_THREAD
	DestroyBrowseLock();
#endif

}

void StopMms()
{
	MMS_MicroStack = NULL;
	MMS_Chain = NULL;
	free (ROOTPATH);
}
/* END SECTION - MicroMediaServer interface implementations */
/************************************************************************************/
