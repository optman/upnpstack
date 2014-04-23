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


/* Intel's UPnP MicroStack, Main Module */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "UpnpMicroStack.h"
#include "ILibParsers.h"
#include "MicroMediaServer.h"


void *TheChain;
void *TheStack;

void *UpnpMonitor;
int UpnpIPAddressLength;
int *UpnpIPAddressList;

/*
 *	Method gets periodically executed on the microstack
 *	thread to update the list of known IP addresses.
 *	This allows the upnp layer to adjust to changes
 *	in the IP address list for the platform.
 *	This applies only if winsock1 is used.
 */
void UpnpIPAddressMonitor(void *data)
{
	int length;
	int *list;
	
	length = ILibGetLocalIPAddressList(&list);
	if(length!=UpnpIPAddressLength || memcmp((void*)list,(void*)UpnpIPAddressList,sizeof(int)*length)!=0)
	{
		UpnpIPAddressListChanged(TheStack);
		
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

void BreakSink(int s)
{
	ILibStopChain(TheChain);
}

int main(int argv, char** argc)
{
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

	/* get friendly name with hostname */
	memcpy(friendlyname,"Intel Micro AV Server (",23);
	gethostname(friendlyname+23,68);
	memcpy(friendlyname+strlen(friendlyname),")/Posix\0",8);

	/* command line arg processing */
	TheChain = ILibCreateChain();

	TheStack = UpnpCreateMicroStack(TheChain, friendlyname, udn,"0000001",1800,0);

	if (argv != 2)
	{
		printf("\r\n\r\nUSAGE: You can specify a path name when running this application.\r\n\r\n");
		InitMms(TheChain, TheStack, "./");
	}
	else
	{
		InitMms(TheChain, TheStack, argc[1]);
	}

	/*
	 *	Set up the app to periodically monitor the available list
	 *	of IP addresses.
	 */
	UpnpMonitor = ILibCreateLifeTime(TheChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);

	/* start UPnP - blocking call*/
	signal(SIGINT,BreakSink);
	ILibStartChain(TheChain);

	return 0;
}
