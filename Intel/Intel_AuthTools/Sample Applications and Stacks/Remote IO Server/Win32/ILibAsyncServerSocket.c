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

#include "ILibAsyncServerSocket.h"
#include "ILibAsyncSocket.h"
#include "ILibParsers.h"

#ifdef _WIN32_WCE
	#include <math.h>
	#include <winerror.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <stddef.h>
	#include <string.h>
	#include <winsock.h>
	#include <wininet.h>
	#include <windows.h>
	#include <winioctl.h>
	#include <winbase.h>
#elif WIN32
	#include <math.h>
	#include <winerror.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <stddef.h>
	#include <string.h>
	#ifdef WINSOCK2
		#include <winsock2.h>
		#include <ws2tcpip.h>
	#else
		#include <winsock.h>
		#include <wininet.h>
	#endif
	#include <windows.h>
	#include <winioctl.h>
	#include <winbase.h>
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


#ifdef _WIN32_WCE
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#define gettimeofday(x,y) (x)->tv_sec = GetTickCount()/1000
#define sem_t HANDLE
#define sem_init(x,pshared,pvalue) *x=CreateSemaphore(NULL,pvalue,FD_SETSIZE,NULL)
#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
#define sem_post(x) ReleaseSemaphore(*x,1,NULL)

#elif WIN32
#include <errno.h>
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#define gettimeofday(x,y) (x)->tv_sec = GetTickCount()/1000
#define sem_t HANDLE
#define sem_init(x,y,z) *x=CreateSemaphore(NULL,z,FD_SETSIZE,NULL)
#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
#define sem_post(x) ReleaseSemaphore(*x,1,NULL)

#elif _POSIX
#include <errno.h>
#include <semaphore.h>
#endif


#define DEBUGSTATEMENT(x)


struct AsyncServerSocketModule
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);
	void *Chain;

	int MaxConnection;
	void **AsyncSockets;
	
	#ifdef _WIN32_WCE
		SOCKET ListenSocket;
	#elif WIN32
		SOCKET ListenSocket;
	#elif _POSIX
		int ListenSocket;
	#endif
	unsigned short portNumber,usePort;

	void (*OnReceive)(void *AsyncServerSocketModule, void *ConnectionToken,char* buffer,int *p_beginPointer, int endPointer, void (**OnInterrupt)(void *AsyncServerSocketMoudle, void *ConnectionToken, void *user),void *user);
	void (*OnConnect)(void *AsyncServerSocketModule, void *ConnectionToken,void **user);
	void (*OnDisconnect)(void *AsyncServerSocketModule, void *ConnectionToken, void *user);
	void (*OnInterrupt)(void *AsyncServerSocketModule, void *ConnectionToken, void *user);
	void (*OnSendOK)(void *AsyncServerSocketModule, void *ConnectionToken, void *user);
};
struct AsyncServerSocket_Data
{
	struct AsyncServerSocketModule *module;
	void *user;
};

void ILibAsyncServerSocket_OnInterrupt(void *socketModule, void *user)
{
	struct AsyncServerSocket_Data *data = (struct AsyncServerSocket_Data*)user;
	if(data->module->OnInterrupt!=NULL)
	{
		data->module->OnInterrupt(data->module,socketModule,data->user);
	}
	FREE(user);
}
void ILibAsyncServerSocket_PreSelect(void* socketModule,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime)
{
	struct AsyncServerSocketModule *module = (struct AsyncServerSocketModule*)socketModule;
	int flags,i;

	if(module->portNumber==0)
	{
		module->portNumber = ILibGetStreamSocket(htonl(INADDR_ANY),module->usePort,&(module->ListenSocket));
		#ifdef _WIN32_WCE
			flags = 1;
			ioctlsocket(module->ListenSocket,FIONBIO,&flags);
		#elif WIN32
			flags = 1;
			ioctlsocket(module->ListenSocket,FIONBIO,&flags);
		#elif _POSIX
			flags = fcntl(module->ListenSocket,F_GETFL,0);
			fcntl(module->ListenSocket,F_SETFL,O_NONBLOCK|flags);
		#endif
		listen(module->ListenSocket,4);
		FD_SET(module->ListenSocket,readset);
	}
	else
	{
		// Only put the ListenSocket in the readset, if we are able to handle a new socket
		for(i=0;i<module->MaxConnection;++i)
		{
			if(ILibAsyncSocket_IsFree(module->AsyncSockets[i])!=0)
			{
				FD_SET(module->ListenSocket,readset);
				break;
			}
		}
	}
}
void ILibAsyncServerSocket_PostSelect(void* socketModule,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset)
{
	struct AsyncServerSocket_Data *data;
	struct sockaddr_in addr;
	int addrlen;
	struct AsyncServerSocketModule *module = (struct AsyncServerSocketModule*)socketModule;
	int i,flags;
	#ifdef _WIN32_WCE
		SOCKET NewSocket;
	#elif WIN32
		SOCKET NewSocket;
	#elif _POSIX
		int NewSocket;
	#endif

	if(FD_ISSET(module->ListenSocket,readset)!=0)
	{
		for(i=0;i<module->MaxConnection;++i)
		{
			if(ILibAsyncSocket_IsFree(module->AsyncSockets[i])!=0)
			{
				addrlen = sizeof(addr);
				NewSocket = accept(module->ListenSocket,(struct sockaddr*)&addr,&addrlen);
				if (NewSocket != ~0)
				{
					#ifdef _WIN32_WCE
						flags = 1;
						ioctlsocket(NewSocket,FIONBIO,&flags);
					#elif WIN32
						flags = 1;
						ioctlsocket(NewSocket,FIONBIO,&flags);
					#elif _POSIX
						flags = fcntl(NewSocket,F_GETFL,0);
						fcntl(NewSocket,F_SETFL,O_NONBLOCK|flags);
					#endif
					data = (struct AsyncServerSocket_Data*)MALLOC(sizeof(struct AsyncServerSocket_Data));
					data->module = socketModule;
					data->user = NULL;

					ILibAsyncSocket_UseThisSocket(module->AsyncSockets[i],NewSocket,&ILibAsyncServerSocket_OnInterrupt,data);

					if(module->OnConnect!=NULL)
					{
						module->OnConnect(module,module->AsyncSockets[i],&(data->user));
					}
				}
				else {break;}
			}
		}
	}
}
void ILibAsyncServerSocket_Destroy(void *socketModule)
{
	struct AsyncServerSocketModule *module =(struct AsyncServerSocketModule*)socketModule;

	FREE(module->AsyncSockets);
	#ifdef _WIN32_WCE
		closesocket(module->ListenSocket);
	#elif WIN32
		closesocket(module->ListenSocket);
	#elif _POSIX
		close(module->ListenSocket);
	#endif	
}
void ILibAsyncServerSocket_OnData(void* socketModule,char* buffer,int *p_beginPointer, int endPointer,void (**OnInterrupt)(void *AsyncSocketMoudle, void *user),void **user)
{
	struct AsyncServerSocket_Data *data = (struct AsyncServerSocket_Data*)(*user);
	int bpointer = *p_beginPointer;

	if(data->module->OnReceive!=NULL)
	{
		data->module->OnReceive(data->module,socketModule,buffer,&bpointer,endPointer,&(data->module->OnInterrupt),&(data->user));
		*p_beginPointer = bpointer;
	}
}

void ILibAsyncServerSocket_OnDisconnect(void* socketModule, void *user)
{
	struct AsyncServerSocket_Data *data = (struct AsyncServerSocket_Data*)user;

	if(data->module->OnDisconnect!=NULL)
	{
		data->module->OnDisconnect(data->module,socketModule,data->user);
	}
	FREE(data);
}
void ILibAsyncServerSocket_OnSendOK(void* socketModule, void *user)
{
	struct AsyncServerSocket_Data *data = (struct AsyncServerSocket_Data*)user;

	if(data->module->OnSendOK!=NULL)
	{
		data->module->OnSendOK(data->module,socketModule,data->user);
	}
}

void *ILibCreateAsyncServerSocketModule(void *Chain, int MaxConnections, int PortNumber, int initialBufferSize, void (*OnConnect)(void *AsyncServerSocketModule, void *ConnectionToken,void **user),void (*OnDisconnect)(void *AsyncServerSocketModule, void *ConnectionToken, void *user),void (*OnReceive)(void *AsyncServerSocketModule, void *ConnectionToken,char* buffer,int *p_beginPointer, int endPointer,void (**OnInterrupt)(void *AsyncServerSocketMoudle, void *ConnectionToken, void *user), void **user),void (*OnInterrupt)(void *AsyncServerSocketModule, void *ConnectionToken, void *user), void (*OnSendOK)(void *AsyncServerSocketModule, void *ConnectionToken, void *user))
{
	struct AsyncServerSocketModule *RetVal;
	int i;

	RetVal = (struct AsyncServerSocketModule*)MALLOC(sizeof(struct AsyncServerSocketModule));
	RetVal->PreSelect = &ILibAsyncServerSocket_PreSelect;
	RetVal->PostSelect = &ILibAsyncServerSocket_PostSelect;
	RetVal->Destroy = &ILibAsyncServerSocket_Destroy;
	RetVal->Chain = Chain;
	RetVal->OnConnect = OnConnect;
	RetVal->OnDisconnect = OnDisconnect;
	RetVal->OnInterrupt = OnInterrupt;
	RetVal->OnSendOK = OnSendOK;
	RetVal->OnReceive = OnReceive;
	RetVal->MaxConnection = MaxConnections;
	RetVal->AsyncSockets = (void**)MALLOC(MaxConnections*sizeof(void*));
	RetVal->portNumber = 0;
	RetVal->usePort = PortNumber;
	
	for(i=0;i<MaxConnections;++i)
	{
		RetVal->AsyncSockets[i] = ILibCreateAsyncSocketModule(Chain,initialBufferSize,&ILibAsyncServerSocket_OnData,NULL,&ILibAsyncServerSocket_OnDisconnect, &ILibAsyncServerSocket_OnSendOK);
		//RetVal->AsyncSockets[i] = ILibCreateAsyncSocketModule(Chain,initialBufferSize,&ILibAsyncServerSocket_OnData,NULL,&ILibAsyncServerSocket_OnDisconnect, &ILibAsyncServerSocket_OnSendOK);
	}
	ILibAddToChain(Chain,RetVal);
	return(RetVal);
}
void ILibAsyncServerSocket_Send(void *ServerSocketModule, void *ConnectionToken, char* buffer, int bufferLength, int UserFreeBuffer)
{
	ILibAsyncSocket_Send(ConnectionToken,buffer,bufferLength,UserFreeBuffer);
}
void ILibAsyncServerSocket_Disconnect(void *ServerSocketModule, void *ConnectionToken)
{
	ILibAsyncSocket_Disconnect(ConnectionToken);
}
unsigned int ILibAsyncServreSocket_GetPendingBytesToSend(void *ServerSocketModule, void *ConnectionToken)
{
	return(ILibAsyncSocket_GetPendingBytesToSend(ConnectionToken));
}