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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <string.h>

#include "ILibParsers.h"
#include "UpnpMicroStack.h"
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

void *UpnpMonitor;
int UpnpIPAddressLength;
int *UpnpIPAddressList;

/*
 *	Method executes periodically on the UPnP microstack thread
 *	and updates the microstack with the platform's current list of
 *	active IP addresses.
 *
 *	This methodology can be optimized, but the exact nature of the
 *	optimization will be differ depending on the platform.
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

/*
 *	This method executes when somebody does CTRL-BREAK.
 */
void BreakSink(int s)
{
	ILibStopChain(MR_RendererChain);
}

int main(void)
{
	void *tempLTM;
	char udn[20];
	char friendlyname[100];
	int i;

	/* Randomized udn generation */
	srand((unsigned int)time(NULL));
	for (i=0;i<19;i++)
	{
		udn[i] = (rand() % 25) + 66;
	}
	udn[19] = 0;

	memcpy(friendlyname,"Intel Micro AV Renderer (",25);
	gethostname(friendlyname+25,48);
	memcpy(friendlyname+strlen(friendlyname),")/Posix/Emulated Playback\0",26);

	printf("Intel MicroStack 1.0 - Sample Renderer\r\n\r\n");
	
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
	MR_ExtendedM3uProcessor = CreatePlaylistParser(MR_RendererChain, 3);

	/* for some silly reason, I need to create the lifetime, create stack, and add lifetime */
	tempLTM = ILibCreateLifeTime(MR_RendererChain);
	MR_UpnpStack = UpnpCreateMicroStack(MR_RendererChain, friendlyname, udn,"000001", 1800, 0);
	MR_MediaRenderer = CreateMediaRenderer(MR_RendererChain, MR_UpnpStack, tempLTM);
	//ILibAddToChain(MR_RendererChain, tempLTM);

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

	/*
	 *	Initialize the streaming engine to empty or last known stream - 
	 *	do this after streaming framework is set up.
	 */
	RSL_SetMediaUri(MR_RendererLogic, "");

	/*
	 *	Set up the solution to periodically monitor 
	 *	the platform's current list of IP addresses.
	 *	This will allow the upnp layers to appropriately
	 *	react to IP address changes.
	 */
	UpnpMonitor = ILibCreateLifeTime(MR_RendererChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);

	/* start UPnP - blocking call*/
	signal(SIGINT,BreakSink);
	ILibStartChain(MR_RendererChain);

	CodecWrapper_UnInit();
	
	/* be sure to free the address list */
	FREE(UpnpIPAddressList);

	return 0;
}
