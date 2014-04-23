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
* $Workfile: RemoteIOClientStack.c
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, March 24, 2003
*
*/
#include <stdio.h>
#include <stdlib.h>

/*
Include the correct files for the OS. In some cases, the compiler will still try
to find all header files even if only a few are used, causing warnings.
*/

#ifdef _WIN32_WCE
	//#error "PocketPC compilation detected"
	#include "ILibParsers.h"
	#include "ILibAsyncSocket.h"
	#include "UpnpMicroStack.h"

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
	#include "UpnpMicroStack.h"

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
	#include "UpnpMicroStack.h"

	pthread_t RIOWorkerThread;
#endif

#include "RemoteIOClientStack.h"

#ifdef _DEBUG
	#define DEBUGONLY(x) x

	#define RIO_MALLOC	rio_malloc
	#define RIO_FREE	rio_free

	int rio_malloc_counter = 0;
	void* rio_malloc (int size)
	{
		++rio_malloc_counter;
		#ifdef TRACK_MALLOC_VERBOSE
			printf("rio_malloc_counter=%d\r\n", rio_malloc_counter);
		#endif
		return malloc(size);
	}

	void rio_free (void *ptr)
	{
		--rio_malloc_counter;
		#ifdef TRACK_MALLOC_VERBOSE
			printf("rio_malloc_counter=%d\r\n", rio_malloc_counter);
		#endif
		free(ptr);
	}
#endif

#ifndef _DEBUG
	#define DEBUGONLY(x) 

	#define RIO_MALLOC	malloc
	#define RIO_FREE	free
#endif


// This is the main state structure for the Remote I/O Client Stack
struct RIODataObject
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);

	void*  RIOmicroStack;
	void*  Session;
	int    SessionPort;
	struct RemoteIOChannel* ChannelList;
	char*  PeerConnection;
	int    JumboCommandSize;
	void*  RIOLifeTime;
	int    EventModerationSet;
};

// Chained list structure for Remote I/O Channels
struct RemoteIOChannel
{
	char* name;
	char* uri;

	#ifdef _WIN32_WCE
		int expiration;
	#elif WIN32
		int expiration;
	#elif _POSIX
		struct timeval expiration;
	#endif

	struct RemoteIOChannel* next;
};

struct RIODataObject* RIO = NULL;
static sem_t RemoteIOLock;
static int RemoteIO_RefCounter = 0;

void (*RemoteIOConnectionChanged) (char* PeerConnection) = NULL;
void (*RemoteIOReset) () = NULL;
void (*RemoteIOCommand) (unsigned short command, char* data, int datalength) = NULL;

char*			RemoteIO_FriendlyName = "";
char*			RemoteIO_UniqueIdentifier = "";
char*			RemoteIO_SerialNumber = "";
int				RemoteIO_NotifyPeriod = 0;
unsigned short	RemoteIO_UPnPPort = 0;
char*			RemoteIO_Application = "";
unsigned int	RemoteIO_MaxCommandSize = 0;
int				RemoteIO_DisplayEncoding = 2;
unsigned int	RemoteIO_DisplayWidth = 0;
unsigned int	RemoteIO_DisplayHeight = 0;
char*			RemoteIO_DeviceInformation = "";

// Parse a URI string and returns the IP, port and Path portions of the URI
void ParseUri(char* URI, char** IP, int* Port, char** Path)
{
	struct parser_result *result,*result2,*result3;
	char *TempString,*TempString2;
	int TempStringLength,TempStringLength2;
	
	result = ILibParseString(URI, 0, (int)strlen(URI), "://", 3);
	TempString = result->LastResult->data;
	TempStringLength = result->LastResult->datalength;
	
	/* Parse Path */
	result2 = ILibParseString(TempString,0,TempStringLength,"/",1);
	TempStringLength2 = TempStringLength-result2->FirstResult->datalength;
	*Path = (char*)MALLOC(TempStringLength2+1);
	memcpy(*Path,TempString+(result2->FirstResult->datalength),TempStringLength2);
	(*Path)[TempStringLength2] = '\0';
	
	/* Parse Port Number */
	result3 = ILibParseString(result2->FirstResult->data,0,result2->FirstResult->datalength,":",1);
	if(result3->NumResults==1)
	{
		*Port = 80;
	}
	else
	{
		TempString2 = (char*)MALLOC(result3->LastResult->datalength+1);
		memcpy(TempString2,result3->LastResult->data,result3->LastResult->datalength);
		TempString2[result3->LastResult->datalength] = '\0';
		*Port = atoi(TempString2);
		FREE(TempString2);
	}
	/* Parse IP Address */
	TempStringLength2 = result3->FirstResult->datalength;
	*IP = (char*)MALLOC(TempStringLength2+1);
	memcpy(*IP,result3->FirstResult->data,TempStringLength2);
	(*IP)[TempStringLength2] = '\0';
	ILibDestructParserResults(result3);
	ILibDestructParserResults(result2);
	ILibDestructParserResults(result);
}

// Called by the UPnP Remote I/O Microstack
// Implements the ForceDisconnect call, lets a CP disconnect this RIO client.
// If this RIO client is not connected, this call has no effect.
void UpnpRemoteIO_ForceDisconnection(void* upnptoken)
{
	sem_wait(&RemoteIOLock);
	if (RIO->PeerConnection != NULL)
	{
		// Disconnect the socket
		ILibAsyncDisconnect(RIO->Session);

		// Set new connection state
		RIO_FREE(RIO->PeerConnection);
		RIO->PeerConnection = NULL;

		// Disconnect the session
		if (RIO->Session != NULL)
		{
			// Disconnect RemoteIOSession
			ILibAsyncDisconnect(RIO->Session);
		}
		// Event the new connection
		UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,"");
		UpnpResponse_RemoteIO_ForceDisconnection(upnptoken);
		sem_post(&RemoteIOLock);

		// Event the user
		if (RemoteIOConnectionChanged != NULL) RemoteIOConnectionChanged(RIO->PeerConnection);
	}
	else
	{
		UpnpResponse_RemoteIO_ForceDisconnection(upnptoken);
		sem_post(&RemoteIOLock);
	}
}

// Called by the UPnP Remote I/O Microstack
// Implements the GetPeerConnection call. Allows a CP to get the URI to
// which this RIO client is currently connected to.
void UpnpRemoteIO_GetPeerConnection(void* upnptoken)
{
	sem_wait(&RemoteIOLock);
	if (RIO->PeerConnection == NULL)
	{
		UpnpResponse_RemoteIO_GetPeerConnection(upnptoken,"");
	}
	else
	{
		UpnpResponse_RemoteIO_GetPeerConnection(upnptoken,RIO->PeerConnection);
	}
	sem_post(&RemoteIOLock);
}

// Called by the UPnP Remote I/O Microstack
// Implements the ForceReset call, lets a CP force reset this RIO client.
// Used mostly for debugging
void UpnpRemoteIO_ForceReset(void* upnptoken)
{
	UpnpResponse_RemoteIO_ForceReset(upnptoken);

	// Event the user
	if (RemoteIOReset != NULL) RemoteIOReset();
}

// Called by the UPnP Remote I/O Microstack
// Implements the ForceDisconnect call, lets a CP connect this RIO client
// to a URI if, and only if, this RIO client is not currently connected.
void UpnpRemoteIO_SetPeerInterlock(void* upnptoken,char* PeerConnection)
{
	struct parser_result* ParsedAddress = NULL;
	char* RemoteIOSessionPath = NULL;
	char* RemoteIOSessionAddress = NULL;
	int address = 0;

	if (PeerConnection == NULL || (int)strlen(PeerConnection) < 7)
	{
		UpnpResponse_Error(upnptoken,700,"Invalid PeerConnection");
		return;
	}

	sem_wait(&RemoteIOLock);
	if (RIO->PeerConnection == NULL)
	{
		RIO->PeerConnection = (char*)RIO_MALLOC((int)strlen(PeerConnection) + 1);
		strcpy(RIO->PeerConnection,PeerConnection);

		// Event the new connection
		UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,RIO->PeerConnection);

		// Connect session
		ParseUri(RIO->PeerConnection,&RemoteIOSessionAddress,&RIO->SessionPort,&RemoteIOSessionPath);
		RIO_FREE(RemoteIOSessionPath);

		ParsedAddress = ILibParseString(RemoteIOSessionAddress,0,(int)strlen(RemoteIOSessionAddress),".",1);

		address  = atoi(ParsedAddress->FirstResult->data);
		address += atoi(ParsedAddress->FirstResult->NextResult->data) << 8;
		address += atoi(ParsedAddress->FirstResult->NextResult->NextResult->data) << 16;
		address += atoi(ParsedAddress->FirstResult->NextResult->NextResult->NextResult->data) << 24;

		ILibConnectTo(RIO->Session,0,address,RIO->SessionPort);
		ILibDestructParserResults(ParsedAddress);
		RIO_FREE(RemoteIOSessionAddress);
		UpnpResponse_RemoteIO_SetPeerInterlock(upnptoken,RIO->PeerConnection);
		sem_post(&RemoteIOLock);

		// Event the user
		if (RemoteIOConnectionChanged != NULL) RemoteIOConnectionChanged(RIO->PeerConnection);
	}
	else
	{
		UpnpResponse_RemoteIO_SetPeerInterlock(upnptoken,RIO->PeerConnection);
		sem_post(&RemoteIOLock);
	}
}

// Called by the UPnP Remote I/O Microstack
// Implements the GetDeviceInformation call, lets a CP get information about this RIO client.
void UpnpRemoteIO_GetDeviceInformation(void* upnptoken)
{
	UpnpResponse_RemoteIO_GetDeviceInformation(upnptoken,RemoteIO_Application,RemoteIO_MaxCommandSize,RemoteIO_DisplayEncoding,RemoteIO_DisplayWidth,RemoteIO_DisplayHeight,RemoteIO_DeviceInformation);
}

// Called by the UPnP Remote I/O Microstack
// Implements the SetPeerOverride call, lets a CP connect this RIO client to
// a new URI. If this RIO client is currently connected, it will disconnect and
// switch to the new URI.
void UpnpRemoteIO_SetPeerOverride(void* upnptoken,char* PeerConnection)
{
	struct parser_result* ParsedAddress = NULL;
	char* RemoteIOSessionPath = NULL;
	char* RemoteIOSessionAddress = NULL;
	int address = 0;

	if (PeerConnection == NULL || (int)strlen(PeerConnection) < 7)
	{
		UpnpResponse_Error(upnptoken,700,"Invalid PeerConnection");
		return;
	}

	sem_wait(&RemoteIOLock);
	if (RIO->PeerConnection == NULL || strcmp(RIO->PeerConnection,PeerConnection) != 0)
	{
		if (RIO->PeerConnection != NULL)
		{
			// Disconnect the socket
			ILibAsyncDisconnect(RIO->Session);
			RIO_FREE(RIO->PeerConnection);
		}

		// Set the new session URI
		RIO->PeerConnection = (char*)RIO_MALLOC((int)strlen(PeerConnection) + 1);
		strcpy(RIO->PeerConnection,PeerConnection);

		// Event the new connection
		UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,RIO->PeerConnection);

		// Connect session
		ParseUri(RIO->PeerConnection,&RemoteIOSessionAddress,&RIO->SessionPort,&RemoteIOSessionPath);
		RIO_FREE(RemoteIOSessionPath);

		ParsedAddress = ILibParseString(RemoteIOSessionAddress,0,(int)strlen(RemoteIOSessionAddress),".",1);

		address  = atoi(ParsedAddress->FirstResult->data);
		address += atoi(ParsedAddress->FirstResult->NextResult->data) << 8;
		address += atoi(ParsedAddress->FirstResult->NextResult->NextResult->data) << 16;
		address += atoi(ParsedAddress->FirstResult->NextResult->NextResult->NextResult->data) << 24;

		ILibConnectTo(RIO->Session,0,address,RIO->SessionPort);
		ILibDestructParserResults(ParsedAddress);
		RIO_FREE(RemoteIOSessionAddress);
		sem_post(&RemoteIOLock);

		// Event the user
		if (RemoteIOConnectionChanged != NULL) RemoteIOConnectionChanged(RIO->PeerConnection);
	}
	else 
	{
		sem_post(&RemoteIOLock);
	}
	
	UpnpResponse_RemoteIO_SetPeerOverride(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the GetInputSetup call, lets a CP get information about Remote Input
// for this device. This information is rarly used, but good for future proofing.
void UpnpRemoteInput_GetInputSetup(void* upnptoken)
{
	UpnpResponse_RemoteInput_GetInputSetup(upnptoken,"intel.desktop");
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputKeyPress(void* upnptoken,int key)
{
	RemoteIO_SendCommand(RIO_KEY_PRESS,(char*)&key,4);
	UpnpResponse_RemoteInput_InputKeyPress(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputKeyUp(void* upnptoken,int key)
{
	RemoteIO_SendCommand(RIO_KEY_UP,(char*)&key,4);
	UpnpResponse_RemoteInput_InputKeyUp(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputKeyDown(void* upnptoken,int key)
{
	RemoteIO_SendCommand(RIO_KEY_DOWN,(char*)&key,4);
	UpnpResponse_RemoteInput_InputKeyDown(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputMouseUp(void* upnptoken,int X,int Y,int Button)
{
	RemoteIO_SendMouseUp(X,Y,Button);
	UpnpResponse_RemoteInput_InputMouseUp(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputMouseDown(void* upnptoken,int X,int Y,int Button)
{
	RemoteIO_SendMouseDown(X,Y,Button);
	UpnpResponse_RemoteInput_InputMouseDown(upnptoken);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void UpnpRemoteInput_InputMouseMove(void* upnptoken,int X,int Y)
{
	RemoteIO_SendMouseMove(X,Y);
	UpnpResponse_RemoteInput_InputMouseMove(upnptoken);
}

// PRIVATE - Creates a list of all channels creating a char* that can be
// send to the CP as a result of and actions or as an event.
// Make sure to call free() on the return value when done, also, make sure
// to hold the Remote I/O lock before calling the method (to lock the channel list)
char* UpnpChannelManager_CreateChannelList()
{
	// Scan the channel list and compute the total size
	int totalsize = 1;
	char* channellist;
	char* channellistindex;
	int templength;
	struct RemoteIOChannel* channelindex = RIO->ChannelList;
	while (channelindex != NULL)
	{
		totalsize += (int)strlen(channelindex->name);
		totalsize += (int)strlen(channelindex->uri);
		totalsize += 4;
		channelindex = channelindex->next;
	}

	if (totalsize == 0)
	{
		// Creat an empty channel list
		channellist = (char*)RIO_MALLOC(1);
		channellist[0] = 0;
		return channellist;
	}
	else
	{
		channellist = (char*)RIO_MALLOC(totalsize);
		channellistindex = channellist;
		channelindex = RIO->ChannelList;
		while (channelindex != NULL)
		{
			// Copy name
			templength = (int)strlen(channelindex->name);
			memcpy(channellistindex,channelindex->name,templength);
			channellistindex[templength  ] = '\r';
			channellistindex[templength+1] = '\n';
			channellistindex += (templength + 2);

			// Copy URI
			templength = (int)strlen(channelindex->uri);
			memcpy(channellistindex,channelindex->uri,templength);
			channellistindex[templength  ] = '\r';
			channellistindex[templength+1] = '\n';
			channellistindex += (templength + 2);

			channelindex = channelindex->next;
		}
		channellistindex[0] = 0;;
		return channellist;
	}
}

// PRIVATE - Called by the lifetime monitor when a channel expires. This call will
// remove the channel by calling the UpnpChannelManager_UnregisterChannel method.
void RemoteIO_ChannelExpireSink(void *data)
{
	struct RemoteIOChannel* channel = (struct RemoteIOChannel*)data;
	char* channeluri = MALLOC(strlen(channel->uri)+1);
	strcpy(channeluri,channel->uri);
	UpnpChannelManager_UnregisterChannel(NULL,channeluri);
	RIO_FREE(channeluri);
}

// PRIVATE - Called internaly to event the current list of channels. This event is
// moderated, so this call will be invoked by the lifetime monitor once the moderation
// time expired.
void RemoteIO_EventChannelList(void *data)
{
	char* channellist;

	// Event available channels
	sem_wait(&RemoteIOLock);
	channellist = UpnpChannelManager_CreateChannelList();
	UpnpSetState_ChannelManager_RegisteredChannelList(RIO->RIOmicroStack,channellist);
	RIO_FREE(channellist);
	RIO->EventModerationSet = 0;
	sem_post(&RemoteIOLock);
}

// Called by the UPnP Remote I/O Microstack
// Implements the RegisterChannel call, lets the CP register a new RIO channels for a
// certain amont of time. The CP must re-register the channel from time-to-time to
// prevent the channel from expiring.
void UpnpChannelManager_RegisterChannel(void* upnptoken,char* Name,char* PeerConnection,int Timeout)
{
	// Scan the channel list for an existing channel
	struct RemoteIOChannel* channelindex = RIO->ChannelList;
	struct RemoteIOChannel* newchannel;

	printf("RegisterChannel (%d): %s\r\n",Timeout,Name);

	if (PeerConnection == NULL)
	{
		if (upnptoken != NULL) UpnpResponse_Error(upnptoken,800,"Invalid PeerConnection URI");
		return;
	}

	sem_wait(&RemoteIOLock);

	while (channelindex != NULL)
	{
		// Look for a match
		if (strcmp(channelindex->uri,PeerConnection) == 0) break;
		channelindex = channelindex->next;
	}

	if (channelindex != NULL)
	{
		// Update the expiration time
		ILibLifeTime_Remove(RIO->RIOLifeTime,channelindex);
		#ifdef _WIN32_WCE
			channelindex->expiration = (GetTickCount() / 1000) + Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,channelindex,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#elif WIN32
			channelindex->expiration = (GetTickCount() / 1000) + Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,channelindex,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#elif _POSIX
			gettimeofday(&(channelindex->expiration),NULL);
			(channelindex->expiration).tv_sec += (int)Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,channelindex,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#endif
	}
	else
	{
		// Add a new channel to the channel list
		newchannel = (struct RemoteIOChannel*)RIO_MALLOC(sizeof(struct RemoteIOChannel));
		newchannel->name = (char*)RIO_MALLOC(strlen(Name)+1);
		strcpy(newchannel->name,Name);
		newchannel->uri = (char*)RIO_MALLOC(strlen(PeerConnection)+1);
		strcpy(newchannel->uri,PeerConnection);
		#ifdef _WIN32_WCE
			newchannel->expiration = (GetTickCount() / 1000) + Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,newchannel,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#elif WIN32
			newchannel->expiration = (GetTickCount() / 1000) + Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,newchannel,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#elif _POSIX
			gettimeofday(&(newchannel->expiration),NULL);
			(newchannel->expiration).tv_sec += (int)Timeout;
			ILibLifeTime_Add(RIO->RIOLifeTime,newchannel,Timeout,&RemoteIO_ChannelExpireSink, NULL);
		#endif
		newchannel->next = RIO->ChannelList;
		RIO->ChannelList = newchannel;

		// Set the channels to be evented
		if (RIO->EventModerationSet == 0)
		{
			ILibLifeTime_Add(RIO->RIOLifeTime,NULL,2,&RemoteIO_EventChannelList, NULL);
			RIO->EventModerationSet = 1;
		}
	}

	UpnpResponse_ChannelManager_RegisterChannel(upnptoken);

	sem_post(&RemoteIOLock);
}

// Called by the UPnP Remote I/O Microstack
// Implements the RegisterChannel call, lets the CP un-register a RIO channel.
void UpnpChannelManager_UnregisterChannel(void* upnptoken,char* PeerConnection)
{
	// Scan the channel list for an existing channel
	struct RemoteIOChannel* channelprevious = NULL;
	struct RemoteIOChannel* channelindex = RIO->ChannelList;

	printf("UnRegisterChannel: %s\r\n",PeerConnection);

	if (PeerConnection == NULL)
	{
		if (upnptoken != NULL) UpnpResponse_Error(upnptoken,800,"Invalid PeerConnection URI");
		return;
	}

	sem_wait(&RemoteIOLock);
	while (channelindex != NULL)
	{
		// Look for a match
		if (strcmp(channelindex->uri,PeerConnection) == 0) break;
		channelprevious = channelindex;
		channelindex = channelindex->next;
	}

	// Delete the channel from the list, and free the channel struct
	if (channelindex != NULL)
	{
		ILibLifeTime_Remove(RIO->RIOLifeTime,channelindex);

		if (channelprevious == NULL)
		{
			RIO->ChannelList = channelindex->next;
		}
		else
		{
			channelprevious->next = channelindex->next;
		}
		RIO_FREE(channelindex->name);
		RIO_FREE(channelindex->uri);
		RIO_FREE(channelindex);

		// Set the channels to be evented
		if (RIO->EventModerationSet == 0)
		{
			ILibLifeTime_Add(RIO->RIOLifeTime,NULL,2,&RemoteIO_EventChannelList, NULL);
			RIO->EventModerationSet = 1;
		}
	}
	
	if (upnptoken != NULL) UpnpResponse_ChannelManager_UnregisterChannel(upnptoken);

	sem_post(&RemoteIOLock);
}

// Called by the UPnP Remote I/O Microstack
// Implements the RegisterChannel call, lets the CP clear all registered RIO channels.
// Usualy, RIO Servers should check to see if their own channels are in the registration
// list. If not, they re-register. In practice, this call will cause all servers to immidiatly
// re-register all channels.
void UpnpChannelManager_ClearAllChannels(void* upnptoken)
{
	// Scan the channel list and delete
	struct RemoteIOChannel* channelnext;
	struct RemoteIOChannel* channelindex = RIO->ChannelList;
	RIO->ChannelList = NULL;
	
	sem_wait(&RemoteIOLock);

	// No channels to clean, respond and exit
	if (channelindex == NULL)
	{
		UpnpResponse_ChannelManager_ClearAllChannels(upnptoken);
		sem_post(&RemoteIOLock);
		return;
	}

	// Clear all of the channels one by one.
	while (channelindex != NULL)
	{
		channelnext = channelindex->next;
		RIO_FREE(channelindex->name);
		RIO_FREE(channelindex->uri);
		RIO_FREE(channelindex);
		channelindex = channelnext;
	}
	
	UpnpResponse_ChannelManager_ClearAllChannels(upnptoken);

	// Set the channels to be evented
	if (RIO->EventModerationSet == 0)
	{
		ILibLifeTime_Add(RIO->RIOLifeTime,NULL,2,&RemoteIO_EventChannelList, NULL);
		RIO->EventModerationSet = 1;
	}
	
	sem_post(&RemoteIOLock);
}

// Called by the UPnP Remote I/O Microstack
// Implements the GetRegisteredChannelList call, lets the CP obtain the complete
// list of all registered channels.
void UpnpChannelManager_GetRegisteredChannelList(void* upnptoken)
{
	char* channellist;
	sem_wait(&RemoteIOLock);
	channellist = UpnpChannelManager_CreateChannelList();
	UpnpResponse_ChannelManager_GetRegisteredChannelList(upnptoken,channellist);
	RIO_FREE(channellist);
	sem_post(&RemoteIOLock);
}

#ifndef EXCLUDE_RIO_PRESENTATION_REQUEST 
void UpnpPresentationRequest(void* upnptoken, struct packetheader *packet)
{
	UpnpPresentationResponse(upnptoken, "HTTP/1.0 200 OK\r\n\r\n" , 19 , 1);
}
#endif

// Normal coding rules are suspended in the function for the benefit of speed.
// This method supports Jumbo commands, that feature can be removed to optimize on sub-64k devices.
// This method supports the obsolete BigImage command - Jumbo should always be used
void OnRemoteIODataSink(void* socketModule,char* buf,int *p_beginPointer, int endPointer)
{
	int datalength = 0;
	unsigned char* buffer = buf;
	int command;

	ProcessNextCommand:
	if (endPointer < 4) return;

	if (RIO->JumboCommandSize == 0)
	{
		datalength = buffer[0] | (buffer[1] << 8);
	}
	else
	{
		datalength = RIO->JumboCommandSize;
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
			RIO->JumboCommandSize = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
		}
		else
		{
			// The user is assumer to have the RemoteIOCommand pointer set, no check.
			RemoteIOCommand((unsigned short)command,(char*)(buffer+4),datalength-4);
			RIO->JumboCommandSize = 0;
		}
		*p_beginPointer += datalength;
		buffer += datalength;
		endPointer -= datalength;
		goto ProcessNextCommand;
	}
}

// PRIVATE - Called by the AsyncSocket module when the XRT connection is established. Once connected,
// the stack immidiatly sends the XRT REQUEST command to get things started.
void OnRemoteIOConnectSink(void* socketModule, int Connected)
{
	if (Connected != 0)
	{
		// Send REQUEST command
		RemoteIO_SendCommand(RIO_REQUEST,RIO->PeerConnection,(int)strlen(RIO->PeerConnection));
	}
	else
	{
		sem_wait(&RemoteIOLock);

		// Set new connection state
		if (RIO->PeerConnection != NULL) RIO_FREE(RIO->PeerConnection);
		RIO->PeerConnection = NULL;

		// Event the new connection
		UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,"");

		sem_post(&RemoteIOLock);

		// Event the user
		if (RemoteIOConnectionChanged != NULL) RemoteIOConnectionChanged(RIO->PeerConnection);
	}
}

// PRIVATE - Called by the AsyncSocket module when the XRT socket as disconnected.
// This will cause the RIO Client to return to un-connected state.
void OnRemoteIODisconnectSink(void* socketModule)
{
	// Set new connection state
	RIO_FREE(RIO->PeerConnection);
	RIO->PeerConnection = NULL;

	// Event the user
	if (RemoteIOConnectionChanged != NULL) RemoteIOConnectionChanged(RIO->PeerConnection);

	// Event the new connection
	UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,"");
}

// PRIVATE - Called by the chaining system to clean up. Usualy called before the
// application is terminated.
void RemoteIODestroyChain(void* object)
{
	struct RIODataObject* remoteio = (struct RIODataObject*)object;
	ILibStopAsyncSocketModule(remoteio->Session);
	RemoteIO_RefCounter--;
	if (RemoteIO_RefCounter == 0) sem_destroy(&RemoteIOLock);
}

// PRIVATE - Entry point for the XRT thread. In the Remote I/O stack, the XRT
// commands are handled by a different thread than the UPnP Microstack thread.
// Both modules could have been chained on the same thread, but it is expected that
// XRT image decoding will take a bit of time and may lock up the UPnP processing.
// So it is best to keep them seperate.
#ifdef _WIN32_WCE
DWORD WINAPI RemoteIOSessionThreadEntry(void* param)
#elif WIN32
DWORD WINAPI RemoteIOSessionThreadEntry(void* param)
#elif _POSIX
void* RemoteIOSessionThreadEntry(void* param)
#endif
{
	ILibStartAsyncSocketModule(RIO->Session);
	RIO_FREE(RIO);
	
	#ifdef _WIN32_WCE
	#elif WIN32
	#elif _POSIX	
		pthread_detach(RIOWorkerThread);
	#endif
	return 0;
}

// PUBLIC - Creates a Remote I/O Client Device Stack. This stack does not include the UPnP stack
// one must be build into the project with the "Upnp" prefix using Intel Device Builder. All
// callbacks must be externs. Remote I/O can be a root or embedded device.
void* CreateRemoteIO(void* Chain, void* UpnpStack)
{
	if (RemoteIO_RefCounter == 0) sem_init(&RemoteIOLock,0,1);
	RemoteIO_RefCounter++;

	RIO = (struct RIODataObject*)RIO_MALLOC(sizeof(struct RIODataObject));
	memset(RIO,0,sizeof(struct RIODataObject));

	// Start the new Remote IO session
	RIO->Destroy = &RemoteIODestroyChain;
	RIO->Session = ILibCreateAsyncSocketModule(650000,&OnRemoteIODataSink,&OnRemoteIOConnectSink,&OnRemoteIODisconnectSink);

	#ifdef _WIN32_WCE
		CreateThread(NULL,0,RemoteIOSessionThreadEntry,NULL,0,NULL);
	#elif WIN32
		CreateThread(NULL,0,RemoteIOSessionThreadEntry,NULL,0,NULL);
	#elif _POSIX
		pthread_create(&RIOWorkerThread,NULL,RemoteIOSessionThreadEntry,NULL);
	#endif
	
	RIO->RIOmicroStack = UpnpStack;

	/* All evented state variables MUST be initialized before UPnPStart is called. */
	UpnpSetState_RemoteIO_PeerConnection(RIO->RIOmicroStack,"");
	UpnpSetState_ChannelManager_RegisteredChannelList(RIO->RIOmicroStack,"");

	RIO->RIOLifeTime = ILibCreateLifeTime(Chain);
	//ILibAddToChain(Chain,RIO->RIOLifeTime);
	ILibAddToChain(Chain,RIO);

	return RIO;
}

// PRIVATE - User internaly to build and send XRT commands. If XRT is not connected,
// the command is ignored.
void RemoteIO_SendCommand(unsigned short command, char* data, int datalength)
{
	char header[4];
	header[0] = (datalength+4) & 0xFF;
	header[1] = (datalength+4) >> 8;
	header[2] = command & 0xFF;
	header[3] = command >> 8;

	if (RIO != NULL)
	{
		if (datalength > 0xFFFB) RemoteIO_SendCommand(RIO_JUMBO,(char*)&datalength,4);
		ILibAsyncSend(RIO->Session,header,4);
		if (data != NULL) ILibAsyncSend(RIO->Session,data,datalength);
	}
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendKeyPress(int key)
{
	RemoteIO_SendCommand(RIO_KEY_PRESS,(char*)&key,4);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendKeyUp(int key)
{
	RemoteIO_SendCommand(RIO_KEY_UP,(char*)&key,4);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendKeyDown(int key)
{
	RemoteIO_SendCommand(RIO_KEY_DOWN,(char*)&key,4);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendMouseUp(int X,int Y,int Button)
{
	char data[12];
	memcpy(data+0,(char*)&X,4);
	memcpy(data+4,(char*)&Y,4);
	memcpy(data+8,(char*)&Button,4);
	RemoteIO_SendCommand(RIO_MOUSE_UP,(char*)&data,12);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendMouseDown(int X,int Y,int Button)
{
	char data[12];
	memcpy(data+0,(char*)&X,4);
	memcpy(data+4,(char*)&Y,4);
	memcpy(data+8,(char*)&Button,4);
	RemoteIO_SendCommand(RIO_MOUSE_DOWN,(char*)&data,12);
}

// Called by the UPnP Remote I/O Microstack
// Implements the InputKeyPress call, lets a CP inject user input into this RIO client.
void RemoteIO_SendMouseMove(int X,int Y)
{
	char data[12];
	memcpy(data+0,(char*)&X,4);
	memcpy(data+4,(char*)&Y,4);
	memset(data+8,0,4);
	RemoteIO_SendCommand(RIO_MOUSE_MOVE,(char*)&data,12);
}

// PUBLIC - Used by the programmer to lock the RIO Stack state. This is useful
// for example to get the list of available channels without having the list while
// looking at each node one-by-one.
// Must never be called twice in a row, will lock up.
void RemoteIO_Lock()
{
	sem_wait(&RemoteIOLock);
}

// PUBLIC - Must be called each time RemoteIO_Lock is called.
void RemoteIO_UnLock()
{
	sem_post(&RemoteIOLock);
}
