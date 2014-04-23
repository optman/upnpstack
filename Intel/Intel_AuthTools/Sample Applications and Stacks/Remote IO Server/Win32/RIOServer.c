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
* $Workfile: RemoteIOServer.c
* $Author:   Intel Corporation
* $Date:     Monday, March 24, 2003
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "RIOServer.h"
#include "RIOControlPoint.h"
#include "ILibAsyncServerSocket.h"

/*
Include the correct files for the OS. In some cases, the compiler will still try
to find all header files even if only a few are used, causing warnings.
*/

#ifdef _WIN32_WCE
	//#error "PocketPC compilation detected"
	#include "ILibParsers.h"
	#include "ILibAsyncSocket.h"
	#include "RemoteIOControl.h"

	#define sem_t HANDLE
	#define sem_init(x,y,z) *x=CreateSemaphore(NULL,z,FD_SETSIZE,NULL)
	#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
	#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
	#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
	#define sem_post(x) ReleaseSemaphore(*x,1,NULL)

#elif WIN32
	//#error "Windows compilation detected"
	#include "ILibParsers.h"
	#include "ILibAsyncSocket.h"
	#include "RemoteIOControl.h"

	#define sem_t HANDLE
	#define sem_init(x,y,z) *x=CreateSemaphore(NULL,z,FD_SETSIZE,NULL)
	#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
	#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
	#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
	#define sem_post(x) ReleaseSemaphore(*x,1,NULL)

#elif _POSIX
	#include <stdlib.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <time.h>

	#include "ILibParsers.h"
	#include "ILibAsyncSocket.h"
	#include "RemoteIOControl.h"
#endif

#ifdef _DEBUG
	#define DEBUGONLY(x) x

	#define RIO_MALLOC	malloc
	#define RIO_FREE	free
#endif

void (*RemoteIOServerCallback_ClientAdded)     (struct RemoteIODevice* device);
void (*RemoteIOServerCallback_ClientRemoved)   (struct RemoteIODevice* device);
void (*RemoteIOServerCallback_CheckMatch)      (struct RemoteIODevice* device, int* match) = NULL;
void (*RemoteIOServerCallback_Connection)      (struct RemoteIODevice* device) = NULL;
void (*RemoteIOServerCallback_Disconnection)   (struct RemoteIODevice* device) = NULL;
void (*RemoteIOServerCallback_Command)         (struct RemoteIODevice* device, int command, char* data, int len) = NULL;
void (*RemoteIOServerCallback_CommandUnderFlow)(struct RemoteIODevice* device) = NULL;

// This is the main state structure for the Remote I/O Client Stack
struct RIOServerObject
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);

	void* Control;
	//void* ControlPoint;
	void* SockServer;
	int   ServerPort;
	int   JumboCommandSize;

	char* ApplicationName;
	void* LifeTimeMonitor;
};

struct RIOServerObject* RIOServer = NULL;

// PRIVATE
void RemoteIOServer_ChannelReRegisterSink(void *data)
{
	struct RemoteIODevice* device = (struct RemoteIODevice*)data;
	RemoteIO_RegisterChannel(device,RIOServer->ApplicationName,device->ApplicationURI,1800);
}

// PRIVATE
void RemoteIOEventCallback_ClientAddedSink(struct RemoteIODevice* device)
{
	struct UPnPDevice* upnpdevice = (struct UPnPDevice*)device->Device;

	device->RIOServeApplication = 0;
	if (RemoteIOServerCallback_CheckMatch != NULL) RemoteIOServerCallback_CheckMatch(device,&(device->RIOServeApplication));
	if (RemoteIOServerCallback_ClientAdded != NULL) RemoteIOServerCallback_ClientAdded(device);
	if (device->RIOServeApplication != 0)
	{
		// Build Application URI
		device->ApplicationURI = malloc(100 + strlen(device->UDN));
		sprintf(device->ApplicationURI,"XRT2://%s:%d/%s",upnpdevice->InterfaceToHost,RIOServer->ServerPort,device->UDN);
		RemoteIO_RegisterChannel(device,RIOServer->ApplicationName,device->ApplicationURI,1800);
		ILibLifeTime_Add(RIOServer->LifeTimeMonitor,device,1700,&RemoteIOServer_ChannelReRegisterSink, NULL);
	}
}

// PRIVATE
void RemoteIOEventCallback_ClientRemovedSink(struct RemoteIODevice* device)
{
	if (RemoteIOServerCallback_ClientRemoved != NULL) RemoteIOServerCallback_ClientRemoved(device);
	if (device->ApplicationURI != NULL)
	{
		ILibLifeTime_Remove(RIOServer->LifeTimeMonitor,device);
		free(device->ApplicationURI);
	}
	if (device->RIOAsyncServerSocket == NULL) return;
	if (ILibAsyncServerSocket_Disconnect != NULL) ILibAsyncServerSocket_Disconnect(RIOServer->SockServer,device->RIOAsyncServerSocket);
	device->RIOAsyncServerSocket = NULL;
}

// PRIVATE
void RemoteIOEventCallback_StateChangedSink(struct RemoteIODevice* device)
{
	// NOP - This server is unaware of client state
}

// PRIVATE
void RemoteIOEventCallback_ChannelsChangedSink(struct RemoteIODevice* device)
{
	int i,match=0;

	// Application not served to that device
	if (device->RIOServeApplication == 0) return;
	
	// Check if device is in device list
	for (i=0;i<device->ChannelCount;i++)
	{
		if (strcmp(device->Channels[i]->URI,device->ApplicationURI) == 0)
		{
			match = 1;
			break;
		}
	}

	// If channel not found, register it
	if (match == 0)
	{
		RemoteIO_RegisterChannel(device,RIOServer->ApplicationName,device->ApplicationURI,1800);
	}
}

// PRIVATE
void RIOServerDestroyChain(void* object)
{
	if (RIOServer->ApplicationName != NULL) free(RIOServer->ApplicationName);
}

// PRIVATE
void RIOSOnConnectSink(void *AsyncServerSocketModule, void *ConnectionToken,void **user)
{
	// NOP, At this point, we don't know who is connecting, only after getting the XRT
	// REQUEST commadn will we know which client we are talking to.
}

// PRIVATE
void RIOSOnDisconnectSink(void *AsyncServerSocketModule, void *ConnectionToken, void *user)
{
	struct RemoteIODevice* device = (struct RemoteIODevice*)user;
	if (device == NULL) return;
	device->RIOAsyncServerSocket = NULL;
	if (RemoteIOServerCallback_Disconnection != NULL) RemoteIOServerCallback_Disconnection(device);
}

// PRIVATE
void RIOSOnReceiveSink(void *AsyncServerSocketModule, void *ConnectionToken,char* buf,int *p_beginPointer, int endPointer, void (**InterruptPtr)(void *socketModule, void *ConnectionToken, void *user), void **user)
{
	struct RemoteIODevice* device = (struct RemoteIODevice*)(*user);

	struct UPnPDevice* upnpdevice = NULL;
	int datalength = 0;
	unsigned char* buffer = buf;
	int command;
	char* IP;
	unsigned short Port;
	char* Path;

	ProcessNextCommand:
	if (endPointer < 4) return;

	if (RIOServer->JumboCommandSize == 0)
	{
		datalength = buffer[0] | (buffer[1] << 8);
	}
	else
	{
		datalength = RIOServer->JumboCommandSize;
	}

	command = (unsigned short)(buffer[2] | (buffer[3] << 8));	

	if (command == RIO_XWPC_BIGIMAGE)
	{
		if (endPointer < 8) return;
		datalength = 8 + (buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24));
	}

	if (endPointer >= datalength)
	{
		if (command == RIO_JUMBO)
		{
			RIOServer->JumboCommandSize = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
		}
		else
		{
			// Process XRT Command
			if (device == NULL && command == RIO_REQUEST)
			{
				// XRT REQUEST command
				buffer[datalength] = 0;
				ILibParseUri(buffer+4, &IP, &Port, &Path);
				if (strlen(Path) > 1)
				{
					upnpdevice = RIOGetDeviceAtUDN(RIOServer->Control,Path+1);
				}
				if (upnpdevice != NULL)
				{
					device = (struct RemoteIODevice*)upnpdevice->Tag;
					*user = device;
					device->RIOAsyncServerSocket = ConnectionToken;
					if (RemoteIOServerCallback_Connection != NULL) RemoteIOServerCallback_Connection(device);
				}
				else
				{
					// Invalid REQUEST command, closing socket
					ILibAsyncServerSocket_Disconnect(RIOServer->SockServer,ConnectionToken);
				}
				if (IP != NULL) free(IP);
				if (Path != NULL) free(Path);
			}
			else
			{
				// User command processing
				if (device != NULL && RemoteIOServerCallback_Command != NULL) RemoteIOServerCallback_Command(device,(unsigned short)command,(char*)(buffer+4),datalength-4);
			}
			RIOServer->JumboCommandSize = 0;
		}
		*p_beginPointer += datalength;
		buffer += datalength;
		endPointer -= datalength;
		goto ProcessNextCommand;
	}
}

// PRIVATE
void RIOSOnInterruptSink(void *AsyncServerSocketMoudle, void *ConnectionToken, void *user)
{
	// NOP
}

// PRIVATE
void RIOSOnSendOKSink(void *AsyncServerSocketModule,void *ConnectionToken, void *user)
{
	if (user == NULL || RemoteIOServerCallback_CommandUnderFlow == NULL) return;
	RemoteIOServerCallback_CommandUnderFlow((struct RemoteIODevice*)user);
}

// PUBLIC
void* RemoteIOServerCreate(void* threadchain, char* ApplicationName, unsigned short ServerPort)
{
	RIOServer = (struct RIOServerObject*)malloc(sizeof(struct RIOServerObject));
	memset(RIOServer,0,sizeof(struct RIOServerObject));

	RIOServer->ApplicationName = malloc(strlen(ApplicationName)+1);
	strcpy(RIOServer->ApplicationName,ApplicationName);

	RemoteIOEventCallback_ClientAdded     = &RemoteIOEventCallback_ClientAddedSink;
	RemoteIOEventCallback_ClientRemoved   = &RemoteIOEventCallback_ClientRemovedSink;
	RemoteIOEventCallback_StateChanged    = &RemoteIOEventCallback_StateChangedSink;
	RemoteIOEventCallback_ChannelsChanged = &RemoteIOEventCallback_ChannelsChangedSink;

	RIOServer->Destroy = &RIOServerDestroyChain;
	RIOServer->Control = RemoteIOCreateControl(threadchain);

	// TODO: Undefined port
	RIOServer->LifeTimeMonitor = ILibCreateLifeTime(threadchain);
	RIOServer->SockServer = ILibCreateAsyncServerSocketModule(threadchain,10,ServerPort,4096,&RIOSOnConnectSink,&RIOSOnDisconnectSink,&RIOSOnReceiveSink,&RIOSOnInterruptSink,&RIOSOnSendOKSink);
	RIOServer->ServerPort = ServerPort;
	
	ILibAddToChain(threadchain,RIOServer);

	return RIOServer;
}

// PUBLIC
void RemoteIOServerDisconnect(struct RemoteIODevice* device)
{
	ILibAsyncServerSocket_Disconnect(RIOServer->SockServer,device->RIOAsyncServerSocket);
	device->RIOAsyncServerSocket = NULL;
}

// PUBLIC
void RemoteIOServerSendCommand(struct RemoteIODevice* device,int command, char* data1, int datalength1, char* data2, int datalength2, int freedata2)
{
	char* header;
	int* tmp;
	int totallength = datalength1 + datalength2;
	if (device->RIOAsyncServerSocket == NULL) return;
	header = (char*)malloc(4);
	header[0] = (totallength+4) & 0xFF;
	header[1] = (totallength+4) >> 8;
	header[2] = command & 0xFF;
	header[3] = command >> 8;

	if (device != NULL)
	{
		if (totallength > 0xFFFB)
		{
			tmp = (int*)malloc(4);
			*tmp = totallength;
			RemoteIOServerSendCommand(device,RIO_JUMBO,(char*)tmp,4,NULL,0,0);
		}
		ILibAsyncServerSocket_Send(RIOServer->SockServer,device->RIOAsyncServerSocket,header,4,0);
		if (data1 != NULL && datalength1 != 0) ILibAsyncServerSocket_Send(RIOServer->SockServer,device->RIOAsyncServerSocket,data1,datalength1,0);
		if (data2 != NULL && datalength2 != 0) ILibAsyncServerSocket_Send(RIOServer->SockServer,device->RIOAsyncServerSocket,data2,datalength2,freedata2);
	}
}

void RemoteIOServerSend_RESET(struct RemoteIODevice* device)
{
	RemoteIOServerSendCommand(device,RIO_RESET,NULL,0,NULL,0,1);
}

void RemoteIOServerSend_DRAWFILLBOX(struct RemoteIODevice* device, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)
{
	struct RIO_COMMAND_DRAWFILLBOX* command = (struct RIO_COMMAND_DRAWFILLBOX*)malloc(sizeof(struct RIO_COMMAND_DRAWFILLBOX));
	command->x = x;
	command->y = y;
	command->w = w;
	command->h = h;
	command->r = r;
	command->g = g;
	command->b = b;
	RemoteIOServerSendCommand(device,RIO_DRAWFILLBOX,(char*)command,sizeof(struct RIO_COMMAND_DRAWFILLBOX),NULL,0,1);
}

void RemoteIOServerSend_DRAWIMAGE(struct RemoteIODevice* device, int x, int y, char* image, int imagesize, int dontfreeimage)
{
	struct RIO_COMMAND_DRAWIMAGE* command = (struct RIO_COMMAND_DRAWIMAGE*)malloc(sizeof(struct RIO_COMMAND_DRAWIMAGE));
	command->x = x;
	command->y = y;
	RemoteIOServerSendCommand(device,RIO_DRAWIMAGE,(char*)command,sizeof(struct RIO_COMMAND_DRAWIMAGE),image,imagesize,dontfreeimage);
}

void RemoteIOServerSend_ALLOCATE(struct RemoteIODevice* device, int x, int y, int w, int h, int id)
{
	struct RIO_COMMAND_ALLOCATE* command = (struct RIO_COMMAND_ALLOCATE*)malloc(sizeof(struct RIO_COMMAND_ALLOCATE));
	command->x  = x;
	command->y  = y;
	command->w  = w;
	command->h  = h;
	command->id = id;
	RemoteIOServerSendCommand(device,RIO_ALLOCATE,(char*)command,sizeof(struct RIO_COMMAND_ALLOCATE),NULL,0,1);
}

// PUBLIC
unsigned int RemoteIOServerGetBufferOverflow(struct RemoteIODevice* device)
{
	if (device->RIOAsyncServerSocket == NULL) return 0;
	return ILibAsyncServreSocket_GetPendingBytesToSend(RIOServer->SockServer,device->RIOAsyncServerSocket);
}

// PUBLIC
void RemoteIOServer_ConnectInterlock(struct RemoteIODevice* device)
{
	RemoteIO_ConnectInterlock(device,device->ApplicationURI);
}

// PUBLIC
void RemoteIOServer_ConnectOverride(struct RemoteIODevice* device)
{
	RemoteIO_ConnectOverride(device,device->ApplicationURI);
}

// PUBLIC
void RemoteIOServer_Disconnect(struct RemoteIODevice* device)
{
	RemoteIO_ForceDisconnection(device);
}

// PUBLIC
void RemoteIOServer_Lock()
{
	RemoteIO_Lock();
}

// PUBLIC
void RemoteIOServer_UnLock()
{
	RemoteIO_UnLock();
}
