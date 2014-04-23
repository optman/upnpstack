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

* No license under any patent, copyright, trade secret or other
* intellectual property right is granted to or conferred upon you
* by disclosure or delivery of the Materials, either expressly, by
* implication, inducement, estoppel or otherwise. Any license
* under such intellectual property rights must be express and
* approved by Intel in writing.
* 
* $Workfile: ILibAsyncSocket.h
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, March 24, 2003
*
*/
#ifdef _WIN32_WCE
	#include <windows.h>
	#include <math.h>
	#include <winioctl.h>
	#include <winbase.h>
	#include <winerror.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <stddef.h>
	#include <string.h>
	#include <winsock.h>
	#include <wininet.h>
#elif WIN32
	#include <windows.h>
	#include <math.h>
	#include <winioctl.h>
	#include <winbase.h>
	#include <winerror.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <stddef.h>
	#include <string.h>
	#include <winsock.h>
	#include <wininet.h>
#elif _POSIX
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <string.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <sys/utsname.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
#endif

void* ILibCreateAsyncSocketModule(int initialBufferSize, void(*OnData)(void* socketModule,char* buffer,int *p_beginPointer, int endPointer), void(*OnConnect)(void* socketModule, int Connected),void(*OnDisconnect)(void* socketModule));
void ILibDestroyAsyncSocketModule(void *socketModule);
void ILibStartAsyncSocketModule(void *socketModule);
void ILibStopAsyncSocketModule(void *socketModule);

void ILibConnectTo(void* socketModule, int localInterface, int remoteInterface, int remotePortNumber);
void ILibAsyncSend(void* socketModule, char* buffer, int length);
void ILibAsyncDisconnect(void* socketModule);

void ILibAsyncSocketModule_PreSelect(void *socketModule,int block,fd_set *readset, fd_set *writeset, fd_set *errorset);
void ILibAsyncSocketModule_PostSelect(void *socketModule, fd_set *readset, fd_set *writeset, fd_set *errorset);

char* ILibGetReceivingInterface(void* ReaderObject);
