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
* $Workfile: ILibAsyncSocket.c
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, March 24, 2003
*
*/
#include "ILibAsyncSocket.h"
#include "ILibParsers.h"

#ifdef _WIN32_WCE
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#define gettimeofday(x,y) (x)->tv_sec = GetTickCount()/1000


#elif WIN32
#include <errno.h>
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#define gettimeofday(x,y) (x)->tv_sec = GetTickCount()/1000


#elif _POSIX
#include <errno.h>
#endif

#define DEBUGSTATEMENT(x)
struct AsyncSocketModule
{
	#ifdef _WIN32_WCE
		SOCKET internalSocket;
	#elif WIN32
		SOCKET internalSocket;
	#elif _POSIX
		int internalSocket;
	#endif

	int LocalIPAddress;
	int InitialBufferSize;
	
	void(*OnData)(void* socketModule,char* buffer,int *p_beginPointer, int endPointer);
	void(*OnConnect)(void* socketModule, int OK);
	void(*OnDisconnect)(void* socketModule);
	
	int FinConnect;
	int BeginPointer;
	int EndPointer;
	
	char* buffer;
	int MallocSize;
	int Terminate;
};
extern int errno;

void ILibDestroyAsyncSocketModule(void *socketModule)
{
	struct AsyncSocketModule* module = (struct AsyncSocketModule*)socketModule;
	
	if(module->internalSocket!=-1)
	{
	#ifdef _WIN32_WCE
			closesocket(module->internalSocket);
	#elif WIN32
			closesocket(module->internalSocket);
	#elif _POSIX
			close(module->internalSocket);
	#endif
	}
	
	if(module->buffer!=NULL)
	{
		FREE(module->buffer);
		module->buffer = NULL;
		module->MallocSize = 0;
	}
	
	#ifdef _WIN32_WCE
		WSACleanup();
	#elif WIN32
		WSACleanup();
	#endif
}
void* ILibCreateAsyncSocketModule(int initialBufferSize, void(*OnData)(void* socketModule,char* buffer,int *p_beginPointer, int endPointer), void(*OnConnect)(void* socketModule, int Connected),void(*OnDisconnect)(void* socketModule))
{
	struct AsyncSocketModule *RetVal = (struct AsyncSocketModule*)MALLOC(sizeof(struct AsyncSocketModule));
	struct timeval tv;

	#ifdef _WIN32_WCE
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD( 1, 1 );
		if (WSAStartup( wVersionRequested, &wsaData ) != 0) {exit(1);}
	#elif WIN32
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD( 1, 1 );
		if (WSAStartup( wVersionRequested, &wsaData ) != 0) {exit(1);}
	#endif
	
	gettimeofday(&tv,NULL);
	srand((int)tv.tv_sec);
	
	RetVal->Terminate = 0;
	RetVal->InitialBufferSize = initialBufferSize;
	RetVal->internalSocket = -1;
	RetVal->OnData = OnData;
	RetVal->OnConnect = OnConnect;
	RetVal->OnDisconnect = OnDisconnect;
	RetVal->buffer = NULL;
	RetVal->MallocSize = 0;
	RetVal->BeginPointer = 0;
	RetVal->EndPointer = 0;
	RetVal->FinConnect = 0;
	
	return((void*)RetVal);
}
void ILibAsyncSend(void* socketModule, char* buffer, int length)
{
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;
	
	send(module->internalSocket,buffer,length,0);
}
void ILibAsyncDisconnect(void* socketModule)
{
	#ifdef _WIN32_WCE
		SOCKET s;
	#elif WIN32
		SOCKET s;
	#elif _POSIX
		int s;
	#endif

	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;

	s = module->internalSocket;
	module->internalSocket = -1;
	if(s!=-1)
	{
	#ifdef _WIN32_WCE
			closesocket(s);
	#elif WIN32
			closesocket(s);
	#elif _POSIX
			close(s);
	#endif
	}

	if(module->buffer!=NULL)
	{
		FREE(module->buffer);
		module->buffer = NULL;
		module->MallocSize = 0;
	}
}
void ILibConnectTo(void* socketModule, int localInterface, int remoteInterface, int remotePortNumber)
{
	int flags;
	struct sockaddr_in addr;
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;
	
	memset((char *)&addr, 0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = remoteInterface;

	#ifdef _WIN32_WCE
		addr.sin_port = htons((unsigned short)remotePortNumber);
	#elif WIN32
		addr.sin_port = htons(remotePortNumber);
	#elif _POSIX
		addr.sin_port = htons(remotePortNumber);
	#endif
	
	if(module->internalSocket==-1)
	{
		ILibGetStreamSocket(localInterface,&(module->internalSocket));
	}
	
	module->FinConnect = 0;
	module->BeginPointer = 0;
	module->EndPointer = 0;
	
	module->buffer = (char*)MALLOC(module->InitialBufferSize);
	module->MallocSize = module->InitialBufferSize;

	#ifdef _WIN32_WCE
		flags = 1;
		ioctlsocket(module->internalSocket,FIONBIO,&flags);
	#elif WIN32
		flags = 1;
		ioctlsocket(module->internalSocket,FIONBIO,&flags);
	#elif _POSIX
		flags = fcntl(module->internalSocket,F_GETFL,0);
		fcntl(module->internalSocket,F_SETFL,O_NONBLOCK|flags);
	#endif

	connect(module->internalSocket,(struct sockaddr*)&addr,sizeof(addr));
}
void ILibProcessAsyncSocket(struct AsyncSocketModule *Reader)
{
	int bytesReceived;
	
	/* Reading Body Only */
	if(Reader->BeginPointer == Reader->EndPointer)
	{
		Reader->BeginPointer = 0;
		Reader->EndPointer = 0;
	}
	else
	{
		if(Reader->BeginPointer!=0)
		{
			Reader->EndPointer = Reader->BeginPointer;
		}
	}
	
	bytesReceived = recv(Reader->internalSocket,Reader->buffer+Reader->EndPointer,Reader->MallocSize-Reader->EndPointer,0);
	Reader->EndPointer += bytesReceived;
	
	if(bytesReceived<=0)
	{
		#ifdef _WIN32_WCE
			closesocket(Reader->internalSocket);
		#elif WIN32
			closesocket(Reader->internalSocket);
		#elif _POSIX
			close(Reader->internalSocket);
		#endif

		if(Reader->buffer!=NULL)
		{
			FREE(Reader->buffer);
			Reader->buffer = NULL;
			Reader->MallocSize = 0;
		}
		Reader->internalSocket = -1;

		if(Reader->OnDisconnect!=NULL) {Reader->OnDisconnect(Reader);}
	}
	else
	{
		if(Reader->OnData!=NULL)
		{
			Reader->OnData(Reader,Reader->buffer,&(Reader->BeginPointer),Reader->EndPointer);
		}
		while(Reader->BeginPointer!=Reader->EndPointer && Reader->BeginPointer!=0)
		{
			memcpy(Reader->buffer,Reader->buffer+Reader->BeginPointer,Reader->EndPointer-Reader->BeginPointer);
			Reader->EndPointer = Reader->EndPointer-Reader->BeginPointer;
			Reader->BeginPointer = 0;
			if(Reader->OnData!=NULL)
			{
				Reader->OnData(Reader,Reader->buffer,&(Reader->BeginPointer),Reader->EndPointer);
			}
		}
		
		if(Reader->BeginPointer==Reader->EndPointer)
		{
			Reader->BeginPointer = 0;
			Reader->EndPointer = 0;
		}
		
		if(Reader->MallocSize - Reader->EndPointer <1024)
		{
			Reader->MallocSize += 4096;
			Reader->buffer = (char*)realloc(Reader->buffer,Reader->MallocSize);
		}
	}
}
void ILibStopAsyncSocketModule(void *socketModule)
{
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;
	module->Terminate = -1;
}
void ILibAsyncSocket_PreSelect(void *socketModule,int block,fd_set *readset, fd_set *writeset, fd_set *errorset)
{
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;
	
	if(module->Terminate==0)
	{
		/* Pre Select */
		if(module->internalSocket!=-1)
		{
			if(module->FinConnect==0)
			{
				/* Not Connected Yet */
				FD_SET(module->internalSocket,writeset);
				FD_SET(module->internalSocket,errorset);
			}
			else
			{
				/* Already Connected, just needs reading */
				FD_SET(module->internalSocket,readset);
				FD_SET(module->internalSocket,errorset);
			}
		}
	}
}
void ILibAsyncSocket_PostSelect(void *socketModule, fd_set *readset, fd_set *writeset, fd_set *errorset)
{
	int flags;
	struct sockaddr_in receivingAddress;
	int receivingAddressLength = sizeof(struct sockaddr_in);
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)socketModule;
	
	if(module->internalSocket!=-1)
	{
		if(module->FinConnect==0)
		{
			/* Not Connected Yet */
			if(FD_ISSET(module->internalSocket,writeset)!=0)
			{
				/* Connected */
				getsockname(module->internalSocket,(struct sockaddr*)&receivingAddress,&receivingAddressLength);
				module->LocalIPAddress = receivingAddress.sin_addr.s_addr;
				module->FinConnect = 1;
				
				#ifdef _WIN32_WCE
					flags = 0;
					ioctlsocket(module->internalSocket,FIONBIO,&flags);
				#elif WIN32
					flags = 0;
					ioctlsocket(module->internalSocket,FIONBIO,&flags);
				#elif _POSIX
					flags = fcntl(module->internalSocket,F_GETFL,0);
					fcntl(module->internalSocket,F_SETFL,(~O_NONBLOCK)&flags);
				#endif

				/* Connection Complete */
				if(module->OnConnect!=NULL)
				{
					module->OnConnect(module,-1);
				}
			}
			if(FD_ISSET(module->internalSocket,errorset)!=0)
			{
				/* Connection Failed */
				module->internalSocket = -1;
				if(module->OnConnect!=NULL)
				{
					module->OnConnect(module,0);
				}
			}
		}
		else
		{
			/* Check if PeerReset */
			if(FD_ISSET(module->internalSocket,errorset)!=0)
			{
				/* Socket Closed */
				//module->internalSocket = -1;
				ILibAsyncDisconnect(module);
				if(module->OnDisconnect!=NULL) {module->OnDisconnect(module);}
			}
			/* Already Connected, just needs reading */
			if(FD_ISSET(module->internalSocket,readset)!=0)
			{
				/* Data Available */
				ILibProcessAsyncSocket(module);
			}
		}
	}
	
}

void ILibStartAsyncSocketModule(void *ClientModule)
{
	struct AsyncSocketModule *module = (struct AsyncSocketModule*)ClientModule;
	struct timeval tv;
	
	fd_set readset;
	fd_set errorset;
	fd_set writeset;
	
	while(module->Terminate==0)
	{
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&errorset);
		
		ILibAsyncSocket_PreSelect(module,-1,&readset, &writeset, &errorset);
		if(select(FD_SETSIZE,&readset,&writeset,&errorset,&tv)>0)
		{
			ILibAsyncSocket_PostSelect(module, &readset, &writeset, &errorset);
		}
		else
		{
			/* Select Failed */
		}
	}
}
