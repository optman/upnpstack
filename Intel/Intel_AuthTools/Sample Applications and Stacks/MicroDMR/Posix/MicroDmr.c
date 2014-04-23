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
 * $Workfile: MicroDmr.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifdef _POSIX
#include <signal.h>
#endif

#include "ILibParsers.h"

#include "RemoteIOClientStack.h"

#include "MicroMediaRenderer.h"
#include "RendererStateLogic.h"
#include "Emulator_Methods.h"
#include "HttpPlaylistParser.h"

/* [CODEC]: BEGIN */
#include "CodecWrapper.h"
/* [CODEC]: END */



void *DmrStack;
void *DmrChain;
void *DmrLTM;
void *DmrRenderer;
void *DmrRendererLogic;
void *DmrExtendedM3uProcessor;

#ifdef _POSIX
void *UpnpMonitor;
int UpnpIPAddressLength;
int *UpnpIPAddressList;
#endif

#ifdef _WINSOCK1
void *UpnpMonitor;
int UpnpIPAddressLength;
int *UpnpIPAddressList;
#endif

#ifdef _WINSOCK2
DWORD UpnpMonitorSocketReserved;
WSAOVERLAPPED UpnpMonitorSocketStateObject;
SOCKET UpnpMonitorSocket;
#endif



void RemoteIOConnectionChangedSink(char* PeerConnection)
{
	printf("RemoteIOConnectionChanged: PeerConnection = %s\r\n",PeerConnection);
	
	// TODO: Generaly if PeerConnection == NULL, clear the screen. Otherwise, do nothing.
}

void RemoteIOResetSink()
{
	printf("RemoteIOReset\r\n");
	
	// TODO: Used for debugging, do nothing.
}

void RemoteIOCommandSink(unsigned short command, char* data, int datalength)
{
	struct RIO_COMMAND_DRAWFILLBOX* cmd_fillbox;
	struct RIO_COMMAND_DRAWIMAGE* cmd_image;
	struct RIO_COMMAND_ALLOCATE* cmd_allocate;
	struct RIO_COMMAND_OBJECT* cmd_setobject;
	struct RIO_COMMAND_COPYOBJECT* cmd_copyobject;
	struct RIO_XWPC_BIGIMAGE* cmd_bigimage;

	switch (command)
	{
		case RIO_RESET:
			printf("RemoteIO RESET Command\r\n");
			// TODO: Reset, debuggin do nothing
			break;
		case RIO_EXIT:
			printf("RemoteIO EXIT Command\r\n");
			// TODO: Exit, debugging do nothing.
			break;
		case RIO_DRAWFILLBOX:
			if (datalength != 11) break;
			cmd_fillbox = (struct RIO_COMMAND_DRAWFILLBOX*)data;
			printf("RemoteIO DRAWFILLBOX Command: (X=%d,Y=%d,W=%d,H=%d,CR=%d,CG=%d,CB=%d)\r\n",cmd_fillbox->x,cmd_fillbox->y,cmd_fillbox->w,cmd_fillbox->h,cmd_fillbox->r,cmd_fillbox->g,cmd_fillbox->b);
			// TODO: Draw Fill Box on screen. Box location & color provided
			break;
		case RIO_DRAWIMAGE:
			if (datalength < 4) break;
			cmd_image = (struct RIO_COMMAND_DRAWIMAGE*)data;
			printf("RemoteIO DRAWIMAGE Command: (X=%d,Y=%d,SIZE=%d)\r\n",cmd_image->x,cmd_image->y,datalength-4);
			// TODO: Draw Image at (data+4).
			// Image width and height must be obtained by decoding the image.
			// Image format is set in the main() method.
			break;
		case RIO_ALLOCATE:
			if (datalength < 10) break;
			cmd_allocate = (struct RIO_COMMAND_ALLOCATE*)data;
			printf("RemoteIO ALLOCATE Command: (X=%d,Y=%d,W=%d,H=%d,ID=%d)\r\n",cmd_allocate->x,cmd_allocate->y,cmd_allocate->w,cmd_allocate->h,cmd_allocate->id);
			// TODO: OPTIONAL - Place video window
			// Generaly, ID = 1 for the main video window, Window is allocated but should not show
			// unless video is actualy playing.
			break;
		case RIO_SETOBJECT:
			if (datalength < 4) break;
			cmd_setobject = (struct RIO_COMMAND_OBJECT*)data;
			printf("RemoteIO SETOBJECT Command: (ID=%d,DataLength=%d)\r\n",cmd_setobject->id,datalength-2);
			// TODO: OPTIONAL - Place this object into object cache
			break;
		case RIO_CLEAROBJECT:
			if (datalength < 2) break;
			cmd_setobject = (struct RIO_COMMAND_OBJECT*)data;
			printf("RemoteIO CLEAROBJECT Command: (ID=%d)\r\n",cmd_setobject->id);
			// TODO: OPTIONAL - Clear object ID from cache
			break;
		case RIO_COPYOBJECT:
			if (datalength < 6) break;
			cmd_copyobject = (struct RIO_COMMAND_COPYOBJECT*)data;
			printf("RemoteIO COPYOBJECT Command: (ID=%d,X=%d,Y=%d)\r\n",cmd_copyobject->id,cmd_copyobject->x,cmd_copyobject->y);
			// TODO: OPTIONAL - Copy object ID to location X,Y on screen
			break;
		case RIO_XWPC_BIGIMAGE:
			cmd_bigimage = (struct RIO_XWPC_BIGIMAGE*)data;
			printf("RemoteIO XWPC_BIGIMAGE Command: (X=%d,Y=%d,DataLength=%d)\r\n",cmd_bigimage->x,cmd_bigimage->y,datalength-12);
			// TODO: PROPRIATARY OPTIONAL - Draw image (data+12) to position X,Y on screen
			break;
		case RIO_XWPC_PING:
			// Respon with PONG command
			printf("RemoteIO got PING, sending PONG\r\n");
			RemoteIO_SendCommand(RIO_XWPC_PONG,"X",1);
			break;
		default:
			printf("RemoteIOCommand: Command = %d, DataLength = %d\r\n",command,datalength);
			// TODO: Do nothing. Used for custom commands such as an LCD panel, LED's and more.
			break;
	}
}

void UpnpPresentationRequest(void* upnptoken, struct packetheader *packet)
{
	/* TODO: Add Web Response Code Here... */
	
	UpnpPresentationResponse(upnptoken, "HTTP/1.0 200 OK\r\n\r\n" , 19 , 1);
}

#ifdef _POSIX
/*
 *	Method gets periodically executed on the microstack
 *	thread to update the list of known IP addresses.
 *	This allows the upnp layer to adjust to changes
 *	in the IP address list for the platform.
 *
 *	This applies only if posix is used.
 */
void UpnpIPAddressMonitor(void *data)
{
	int length;
	int *list;
	
	length = ILibGetLocalIPAddressList(&list);
	if(length!=UpnpIPAddressLength || memcmp((void*)list,(void*)UpnpIPAddressList,sizeof(int)*length)!=0)
	{
		UpnpIPAddressListChanged(DmrStack);
		
		FREE(UpnpIPAddressList);
		UpnpIPAddressList = list;
		UpnpIPAddressLength = length;
	}
	else
	{
		FREE(list);
	}
	
	
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
}
#endif

#ifdef _WINSOCK1
/*
 *	Method gets periodically executed on the microstack
 *	thread to update the list of known IP addresses.
 *	This allows the upnp layer to adjust to changes
 *	in the IP address list for the platform.
 *
 *	This applies only if winsock1 is used.
 */
void UpnpIPAddressMonitor(void *data)
{
	int length;
	int *list;
	
	length = ILibGetLocalIPAddressList(&list);
	if(length!=UpnpIPAddressLength || memcmp((void*)list,(void*)UpnpIPAddressList,sizeof(int)*length)!=0)
	{
		UpnpIPAddressListChanged(DmrStack);
		
		FREE(UpnpIPAddressList);
		UpnpIPAddressList = list;
		UpnpIPAddressLength = length;
	}
	else
	{
		FREE(list);
	}
	
	
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
}
#endif

#ifdef _WINSOCK2
/*
 *	Method gets periodically executed on the microstack
 *	thread to update the list of known IP addresses.
 *	This allows the upnp layer to adjust to changes
 *	in the IP address list for the platform.
 *
 *	This applies only if winsock2 is used.
 */
void CALLBACK UpnpIPAddressMonitor
	(
	IN DWORD dwError, 
	IN DWORD cbTransferred, 
	IN LPWSAOVERLAPPED lpOverlapped, 
	IN DWORD dwFlags 
	)
{
	UpnpIPAddressListChanged(UpnpStack);
	WSAIoctl(UpnpMonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&UpnpMonitorSocketReserved,&UpnpMonitorSocketStateObject,&UpnpIPAddressMonitor);
}
#endif

#ifdef _POSIX
void BreakSink(int s)
{
	ILibStopChain(DmrChain);
}
#endif

#ifdef WIN32
DWORD WINAPI Run(LPVOID args)
{
	getchar();
	ILibStopChain(DmrChain);
	return 0;
}
#endif


int main(void)
{
	char udn[20];
	int i;
	char friendlyname[100];

	#ifdef WIN32
	WSADATA wsaData;
	#endif

	/* Randomized udn generation */
	#ifdef WIN32
	srand(GetTickCount());
	#endif

	#ifdef _POSIX
	srand((unsigned int)time(NULL));
	#endif

	for (i=0;i<19;i++)
	{
		udn[i] = (rand() % 25) + 66;
	}
	udn[19] = 0;

	/* generate a friendly name that has the host name in it */
	#ifdef WIN32
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {exit(1);}
	#endif
	memcpy(friendlyname,"Intel MicroDMR (",16);
	gethostname(friendlyname+16,75);
	#ifdef WIN32
	memcpy(friendlyname+strlen(friendlyname),")/Win32\0",8);
	#endif
	#ifdef _POSIX
	memcpy(friendlyname+strlen(friendlyname),")/Posix\0",8);
	#endif

	/* Setup the callbacks */
	RemoteIOConnectionChanged = &RemoteIOConnectionChangedSink;
	RemoteIOReset = &RemoteIOResetSink;
	RemoteIOCommand = &RemoteIOCommandSink;
	
	/* Setup the Remote IO Global Values */
	RemoteIO_Application		= "XRT20:Sample Client";
	RemoteIO_MaxCommandSize		= 65000;			// Set the maximum command size, keep this at 64k
	RemoteIO_DisplayEncoding	= RemoteIO_JPEG;	// Set the image format, see (enum RemoteIOImageFormat) for complete list.
	RemoteIO_DisplayWidth		= 640;				// Set the display width
	RemoteIO_DisplayHeight		= 480;				// Set the display height
	RemoteIO_DeviceInformation	= "";				// Set propriatary information about the device

	/*
	 *	Set of commands that can be sent back to the host PC. These can be used even
	 *	if remoting is not connected. For a device with an IR remote, only SendKeyPress is used.	
	 */
	/*
	RemoteIO_SendCommand(unsigned short command, char* data, int datalength);
	RemoteIO_SendKeyPress(int key);
	RemoteIO_SendKeyUp(int key);
	RemoteIO_SendKeyDown(int key);
	RemoteIO_SendMouseUp(int X,int Y,int Button);
	RemoteIO_SendMouseDown(int X,int Y,int Button);
	RemoteIO_SendMouseMove(int X,int Y);
	*/

	/* renderer callbacks. */
	MROnVolumeChangeRequest			= &MROnVolumeChangeRequestSink;
	MROnMuteChangeRequest			= &MROnMuteChangeRequestSink;
	MROnMediaChangeRequest			= &MROnMediaChangeRequestSink;
	MROnGetPositionRequest			= &MROnGetPositionRequestSink;
	MROnSeekRequest					= &MROnSeekRequestSink;
	MROnNextPreviousRequest			= &MROnNextPreviousRequestSink;
	MROnStateChangeRequest			= &MROnStateChangeRequestSink;
	MROnPlayModeChangeRequest		= &MROnPlayModeChangeRequestSink;

	printf("Intel MicroDMR Sample Client\r\n");


	DmrChain = MR_RendererChain = ILibCreateChain();
	DmrLTM = ILibCreateLifeTime(DmrChain);

	#ifdef _DEBUG
	printf("DmrLTM=%x\r\n", DmrLTM);
	#endif

	DmrStack = MR_UpnpStack = UpnpCreateMicroStack(DmrChain,friendlyname, udn, "0000001", 20, 0);

	//ILibAddToChain(DmrChain, DmrLTM); /* seems to resolve dealyed event delivery */

    DmrExtendedM3uProcessor = MR_ExtendedM3uProcessor = CreatePlaylistParser(DmrChain, 3);
	DmrRenderer = MR_MediaRenderer = CreateMediaRenderer(DmrChain, DmrStack, DmrLTM);
	DmrRendererLogic = MR_RendererLogic = RSL_CreateRendererStateLogic
		(
		DmrChain,
		DmrRenderer,
		InstructPlaylistLogic_FindTargetUri,
		InstructCodec_SetupStream,
		InstructCodec_Play,
		InstructCodec_Stop,
		InstructCodec_Pause,
		QueryCodec_IsBusy,
		Validate_MediaUri
		);


	/* Create the RemoteIO client */
	CreateRemoteIO(DmrChain, DmrStack);

	/* initialize emulated rendering module */
	CodecWrapper_Init(MAX_STREAMS);

	/*
	 *	Set up the app to periodically monitor the available list
	 *	of IP addresses.
	 */
	#ifdef _POSIX
	UpnpMonitor = ILibCreateLifeTime(DmrChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
	#endif
	#ifdef _WINSOCK1
	UpnpMonitor = ILibCreateLifeTime(DmrChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
	#endif
	#ifdef _WINSOCK2
	UpnpMonitorSocket = socket(AF_INET,SOCK_DGRAM,0);
	WSAIoctl(UpnpMonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&UpnpMonitorSocketReserved,&UpnpMonitorSocketStateObject,&UpnpIPAddressMonitor);
	#endif

	/* start UPnP - blocking call*/
	#ifdef _POSIX
	signal(SIGINT,BreakSink);
	#endif

	#ifdef WIN32
	/* Setup a thread to allow user to stop renderer when user hits key */
	CreateThread(NULL,0,&Run,NULL,0,NULL);
	#endif

	ILibStartChain(DmrChain); 	

	/* be sure to free the address list */
	FREE(UpnpIPAddressList);
	
	/* clean up wsastartup */
	#ifdef WIN32
	WSACleanup();
	#endif

	return 0;
}
