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
 * $Workfile: <FILE>
 * $Revision: #1.0.1195.17540
 * $Author:   Intel Corporation, Intel Device Builder
 * $Date:     Thursday, April 10, 2003
 *
 */

#ifndef ___ILibAsyncSocket___
#define ___ILibAsyncSocket___

#ifdef _WIN32_WCE
	#include <winsock.h>
#elif WIN32
	#ifdef WINSOCK2
		#include <winsock2.h>
	#else
		#include <winsock.h>
	#endif
#endif

void* ILibCreateAsyncSocketModule(void *Chain, int initialBufferSize, void(*OnData)(void* socketModule,char* buffer,int *p_beginPointer, int endPointer,void (**InterruptPtr)(void *socketModule, void *user), void **user), void(*OnConnect)(void* socketModule, int Connected, void *user),void(*OnDisconnect)(void* socketModule, void *user),void(*OnSendOK)(void *socketModule, void *user));
unsigned int ILibAsyncSocket_GetPendingBytesToSend(void *socketModule);
void ILibAsyncSocket_ConnectTo(void* socketModule, int localInterface, int remoteInterface, int remotePortNumber,void (*InterruptPtr)(void *socketModule, void *user),void *user);
int ILibAsyncSocket_Send(void* socketModule, char* buffer, int length, int UserFree);
void ILibAsyncSocket_Disconnect(void* socketModule);
#ifdef _WIN32_WCE
	void ILibAsyncSocket_UseThisSocket(void *socketModule,SOCKET TheSocket,void (*InterruptPtr)(void *socketModule, void *user),void *user);
#elif WIN32
	void ILibAsyncSocket_UseThisSocket(void *socketModule,SOCKET TheSocket,void (*InterruptPtr)(void *socketModule, void *user),void *user);
#elif _POSIX
	void ILibAsyncSocket_UseThisSocket(void *socketModule,int TheSocket,void (*InterruptPtr)(void *socketModule, void *user),void *user);
#endif
int ILibAsyncSocket_IsFree(void *socketModule);


char* ILibGetReceivingInterface(void* ReaderObject);
#endif