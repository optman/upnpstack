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
 * $Workfile: Main.c
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ILibParsers.h"
#include "MicroMediaRenderer.h"
#include "RendererStateLogic.h"
#include "Emulator_Methods.h"
#include "HttpPlaylistParser.h"
#include "PortFn_MsTime.h"

/* [CODEC]: BEGIN */
#include "CodecWrapper.h"
/* [CODEC]: END */

#ifdef _DEBUG
#define DEBUGONLY(x) x
#endif

#ifndef _DEBUG
#define DEBUGONLY(x) 
#endif

#ifdef _TEMPDEBUG
#define TEMPDEBUGONLY(x) x
#endif

#ifndef _TEMPDEBUG
#define TEMPDEBUGONLY(x)
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
		UpnpIPAddressListChanged(MR_UpnpStack);
		
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

DWORD WINAPI Run(LPVOID args)
{
	getchar();
	ILibStopChain(MR_RendererChain);
	CodecWrapper_UnInit();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	void *ltm;
	char udn[20];
	char friendlyname[100];
	WSADATA wsaData;
	int i;

	/* Randomized udn generation */
	srand(GetTickCount());
	for (i=0;i<19;i++)
	{
		udn[i] = (rand() % 25) + 66;
	}
	udn[19] = 0;

	/* generate a friendly name that has the host name in it */
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {exit(1);}
	memcpy(friendlyname,"Intel Micro AV Renderer (",25);
	gethostname(friendlyname+25,48);
	memcpy(friendlyname+strlen(friendlyname),")/Win32/Emulated Playback\0",26);

	/*
	 *	Set all of the renderer callbacks.
	 *	If the UPnP device has multiple renderers, it will need
	 *	to map function pointer callbacks for each renderer device.
	 */
	MROnVolumeChangeRequest			= &MROnVolumeChangeRequestSink;
	MROnMuteChangeRequest			= &MROnMuteChangeRequestSink;
	MROnMediaChangeRequest			= &MROnMediaChangeRequestSink;
	MROnGetPositionRequest			= &MROnGetPositionRequestSink;
	MROnSeekRequest					= &MROnSeekRequestSink;
	MROnNextPreviousRequest			= &MROnNextPreviousRequestSink;
	MROnStateChangeRequest			= &MROnStateChangeRequestSink;
	MROnPlayModeChangeRequest		= &MROnPlayModeChangeRequestSink;

	/* TODO: Each device must have a unique device identifier (UDN) - The UDN should be generated dynamically*/
	MR_RendererChain = ILibCreateChain();

	MR_UpnpStack = UpnpCreateMicroStack(MR_RendererChain, friendlyname, udn,"000001", 1800, 0);
	MR_ExtendedM3uProcessor = CreatePlaylistParser(MR_RendererChain, 3);

	ltm = ILibCreateLifeTime(MR_RendererChain);
	//ILibAddToChain(MR_RendererChain, ltm);

	MR_MediaRenderer = CreateMediaRenderer(MR_RendererChain, MR_UpnpStack, ltm);
	MR_RendererLogic = RSL_CreateRendererStateLogic
		(
		MR_RendererChain,
		MR_MediaRenderer,
		InstructPlaylistLogic_FindTargetUri,
		InstructCodec_SetupStream,
		InstructCodec_Play,
		InstructCodec_Stop,
		InstructCodec_Pause,
		QueryCodec_IsBusy,
		Validate_MediaUri
		);

	/*
	 *	Initialize codec framework - do this after state machine is initialized.
	 *	Intentionally sleep the processor to allow the codec wrapper thread
	 *	to prime.
	 */
	CodecWrapper_Init(MAX_STREAMS);
	SleepMsTime(100);

	/* Setup a thread to allow user to stop renderer when user hits key */
	CreateThread(NULL,0,&Run,NULL,0,NULL);

	/*
	 *	Initialize the streaming engine to empty or last known stream - 
	 *	do this after streaming framework is set up.
	 */
	RSL_SetMediaUri(MR_RendererLogic, "");

	/*
	 *	Set up the app to periodically monitor the available list
	 *	of IP addresses.
	 */
	#ifdef _WINSOCK1
	UpnpMonitor = ILibCreateLifeTime(MR_RendererChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
	#endif
	#ifdef _WINSOCK2
	UpnpMonitorSocket = socket(AF_INET,SOCK_DGRAM,0);
	WSAIoctl(UpnpMonitorSocket,SIO_ADDRESS_LIST_CHANGE,NULL,0,NULL,0,&UpnpMonitorSocketReserved,&UpnpMonitorSocketStateObject,&UpnpIPAddressMonitor);
	#endif

	/* Start the renderer thread chain */
	printf("Intel MicroStack 1.0 - Micro Media Renderer\r\n\r\n");
	ILibStartChain(MR_RendererChain);
	
	/* be sure to free the address list */
	FREE(UpnpIPAddressList);

	/* clean up wsastartup */
	WSACleanup();

	return 0;
}