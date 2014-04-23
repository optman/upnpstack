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
/* Intel StackBuilder Build#1.0.951.32471 */

#include <stdio.h>
#include "UPnPMicroStack.h"

void UPnPContentDirectory_Browse(void* upnptoken,char* ObjectID,char* BrowseFlag,char* Filter,unsigned int StartingIndex,unsigned int RequestedCount,char* SortCriteria)
{
	printf("UPnP Invoke: ContentDirectory_Browse(%s,%s,%s,%u,%u,%s);\r\n",ObjectID,BrowseFlag,Filter,StartingIndex,RequestedCount,SortCriteria);
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	/* UPnPResponse_ContentDirectory_Browse(upnptoken,"Sample String",250,250,250); */
	
	/* Fragmented response system, action result is constructed and sent on-the-fly. */
	UPnPAsyncResponse_START(upnptoken, "Browse", "urn:schemas-upnp-org:service:ContentDirectory:1");
	UPnPAsyncResponse_OUT(upnptoken, "Result", "", 0, 1, 1);
	UPnPAsyncResponse_OUT(upnptoken, "NumberReturned", "", 0, 1, 1);
	UPnPAsyncResponse_OUT(upnptoken, "TotalMatches", "", 0, 1, 1);
	UPnPAsyncResponse_OUT(upnptoken, "UpdateID", "", 0, 1, 1);
	UPnPAsyncResponse_DONE(upnptoken, "Browse");
}

void UPnPContentDirectory_GetSortCapabilities(void* upnptoken)
{
	printf("UPnP Invoke: ContentDirectory_GetSortCapabilities();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ContentDirectory_GetSortCapabilities(upnptoken,"Sample String");
}

void UPnPContentDirectory_GetSystemUpdateID(void* upnptoken)
{
	printf("UPnP Invoke: ContentDirectory_GetSystemUpdateID();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ContentDirectory_GetSystemUpdateID(upnptoken,250);
}

void UPnPContentDirectory_GetSearchCapabilities(void* upnptoken)
{
	printf("UPnP Invoke: ContentDirectory_GetSearchCapabilities();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ContentDirectory_GetSearchCapabilities(upnptoken,"Sample String");
}

void UPnPConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID)
{
	printf("UPnP Invoke: ConnectionManager_GetCurrentConnectionInfo(%d);\r\n",ConnectionID);
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ConnectionManager_GetCurrentConnectionInfo(upnptoken,25000,25000,"Sample String","Sample String",25000,"Sample String","Sample String");
}

void UPnPConnectionManager_GetProtocolInfo(void* upnptoken)
{
	printf("UPnP Invoke: ConnectionManager_GetProtocolInfo();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ConnectionManager_GetProtocolInfo(upnptoken,"Sample String","Sample String");
}

void UPnPConnectionManager_GetCurrentConnectionIDs(void* upnptoken)
{
	printf("UPnP Invoke: ConnectionManager_GetCurrentConnectionIDs();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UPnPResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UPnPResponse_ConnectionManager_GetCurrentConnectionIDs(upnptoken,"Sample String");
}

void UPnPPresentationRequest(void* upnptoken, char* directive)
{
	printf("Presentation Request: %s\r\n", directive);
	
	/* TODO: Add Web Response Code Here... */
	printf("HOST: %x\r\n",UPnPGetLocalInterfaceToHost(upnptoken));
	
	UPnPPresentationResponse(upnptoken, "HTTP/1.0 200 OK\r\n\r\n" , 19 , 1);
}

int main(void)
{
	void* UPnPFM_ContentDirectory[] = {&UPnPContentDirectory_Browse,&UPnPContentDirectory_GetSortCapabilities,&UPnPContentDirectory_GetSystemUpdateID,&UPnPContentDirectory_GetSearchCapabilities};
	void* UPnPFM_ConnectionManager[] = {&UPnPConnectionManager_GetCurrentConnectionInfo,&UPnPConnectionManager_GetProtocolInfo,&UPnPConnectionManager_GetCurrentConnectionIDs};
	void* UPnPFM_Presentation[] = {&UPnPPresentationRequest};
	UPnPSFP_ContentDirectory(UPnPFM_ContentDirectory);
	UPnPSFP_ConnectionManager(UPnPFM_ConnectionManager);
	UPnPSFP_PresentationPage(UPnPFM_Presentation);
	
	/* All evented state variables MUST be initialized before UPnPStart is called. */
	UPnPSetState_ContentDirectory_SystemUpdateID(250);
	UPnPSetState_ConnectionManager_SourceProtocolInfo("Sample String");
	UPnPSetState_ConnectionManager_SinkProtocolInfo("Sample String");
	UPnPSetState_ConnectionManager_CurrentConnectionIDs("Sample String");
	
	printf("Intel's UPnP MicroStack 1.0\r\nConnected & Extended PC Lab (CEL)\r\n\r\n");
	UPnPStart("1d3c433f-403b-4e44-b7a2-b6f25b2a9427","0000001", 120);
	
	return 0;
}

