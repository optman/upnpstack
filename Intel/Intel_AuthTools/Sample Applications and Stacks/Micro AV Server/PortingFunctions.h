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
 * $Workfile: PortingFunctions.h
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

#ifndef PORTINGFUNCTIONS_H
#define PORTINGFUNCTIONS

void PCRandomize();
int PCGetFileDirType(char* directory);
void PCCloseDir(void* handle);
void* PCGetDirFirstFile(const char* directory, const char* dirName, char* filename, int filenamelength, int* fileSize);
int PCGetDirNextFile(void* handle, const char* dirName, char* filename, int filenamelength, int* fileSize);
int PCGetFileSize(const char* fullPath);
void SendHttpResponse_1_0(char* fullPath, void* upnpToken);
void EndThisThread();
void LockBrowse();
void UnlockBrowse();
void InitBrowseLock();
void DestroyBrowseLock();
void* SpawnNormalThread(void* method, void* arg);
int ProceedWithDirEntry(char* dirName, char* filename, int maxPathLength);
#endif
