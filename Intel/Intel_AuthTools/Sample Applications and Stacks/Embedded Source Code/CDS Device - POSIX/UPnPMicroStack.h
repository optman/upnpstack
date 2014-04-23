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
* $Workfile: UPnPMicrostack.h
* $Revision: #1.0.1189.31920
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, April 07, 2003
*
*/
#ifndef __UPnPMicrostack__
#define __UPnPMicrostack__

struct UPnPDataObject;
struct packetheader;

/* These methods must be implemented by the user */
extern void UPnPContentDirectory_Search(void* upnptoken,char* ContainerID,char* SearchCriteria,char* Filter,unsigned int StartingIndex,unsigned int RequestedCount,char* SortCriteria);
extern void UPnPContentDirectory_StopTransferResource(void* upnptoken,unsigned int TransferID);
extern void UPnPContentDirectory_DestroyObject(void* upnptoken,char* ObjectID);
extern void UPnPContentDirectory_UpdateObject(void* upnptoken,char* ObjectID,char* CurrentTagValue,char* NewTagValue);
extern void UPnPContentDirectory_ExportResource(void* upnptoken,char* SourceURI,char* DestinationURI);
extern void UPnPContentDirectory_GetTransferProgress(void* upnptoken,unsigned int TransferID);
extern void UPnPContentDirectory_GetSearchCapabilities(void* upnptoken);
extern void UPnPContentDirectory_GetSystemUpdateID(void* upnptoken);
extern void UPnPContentDirectory_CreateObject(void* upnptoken,char* ContainerID,char* Elements);
extern void UPnPContentDirectory_GetSortCapabilities(void* upnptoken);
extern void UPnPContentDirectory_Browse(void* upnptoken,char* ObjectID,char* BrowseFlag,char* Filter,unsigned int StartingIndex,unsigned int RequestedCount,char* SortCriteria);
extern void UPnPContentDirectory_ImportResource(void* upnptoken,char* SourceURI,char* DestinationURI);
extern void UPnPContentDirectory_CreateReference(void* upnptoken,char* ContainerID,char* ObjectID);
extern void UPnPContentDirectory_DeleteResource(void* upnptoken,char* ResourceURI);
extern void UPnPConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID);
extern void UPnPConnectionManager_PrepareForConnection(void* upnptoken,char* RemoteProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction);
extern void UPnPConnectionManager_ConnectionComplete(void* upnptoken,int ConnectionID);
extern void UPnPConnectionManager_GetProtocolInfo(void* upnptoken);
extern void UPnPConnectionManager_GetCurrentConnectionIDs(void* upnptoken);
extern void UPnPPresentationRequest(void* upnptoken, struct packetheader *packet);

/* UPnP Stack Management */
void *UPnPCreateMicroStack(void *Chain, const char* FriendlyName, const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void *UPnP(void *Chain, const char* FriendlyName,const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void UPnPIPAddressListChanged(void *MicroStackToken);
int UPnPGetLocalPortNumber(void *token);
int   UPnPGetLocalInterfaceToHost(const void* UPnPToken);
void* UPnPGetInstance(const void* UPnPToken);

/* Invocation Response Methods */
void UPnPResponse_Error(const void* UPnPToken, const int ErrorCode, const char* ErrorMsg);
void UPnPResponseGeneric(const void* UPnPToken,const char* ServiceURI,const char* MethodName,const char* Params);
int  UPnPPresentationResponse(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate);
void UPnPAsyncResponse_START(const void* UPnPToken, const char* actionName, const char* serviceUrnWithVersion);
void UPnPAsyncResponse_DONE(const void* UPnPToken, const char* actionName);
void UPnPAsyncResponse_OUT(const void* UPnPToken, const char* outArgName, const char* bytes, const int byteLength, const int startArg, const int endArg);
void UPnPResponse_ConnectionManager_GetCurrentConnectionInfo(const void* UPnPToken, const int RcsID, const int AVTransportID, const char* ProtocolInfo, const char* PeerConnectionManager, const int PeerConnectionID, const char* Direction, const char* Status);
void UPnPResponse_ConnectionManager_PrepareForConnection(const void* UPnPToken, const int ConnectionID, const int AVTransportID, const int RcsID);
void UPnPResponse_ConnectionManager_ConnectionComplete(const void* UPnPToken);
void UPnPResponse_ConnectionManager_GetProtocolInfo(const void* UPnPToken, const char* Source, const char* Sink);
void UPnPResponse_ConnectionManager_GetCurrentConnectionIDs(const void* UPnPToken, const char* ConnectionIDs);
void UPnPResponse_ContentDirectory_Search(const void* UPnPToken, const char* Result, const unsigned int NumberReturned, const unsigned int TotalMatches, const unsigned int UpdateID);
void UPnPResponse_ContentDirectory_StopTransferResource(const void* UPnPToken);
void UPnPResponse_ContentDirectory_DestroyObject(const void* UPnPToken);
void UPnPResponse_ContentDirectory_UpdateObject(const void* UPnPToken);
void UPnPResponse_ContentDirectory_ExportResource(const void* UPnPToken, const unsigned int TransferID);
void UPnPResponse_ContentDirectory_GetTransferProgress(const void* UPnPToken, const char* TransferStatus, const char* TransferLength, const char* TransferTotal);
void UPnPResponse_ContentDirectory_GetSearchCapabilities(const void* UPnPToken, const char* SearchCaps);
void UPnPResponse_ContentDirectory_GetSystemUpdateID(const void* UPnPToken, const unsigned int Id);
void UPnPResponse_ContentDirectory_CreateObject(const void* UPnPToken, const char* ObjectID, const char* Result);
void UPnPResponse_ContentDirectory_GetSortCapabilities(const void* UPnPToken, const char* SortCaps);
void UPnPResponse_ContentDirectory_Browse(const void* UPnPToken, const char* Result, const unsigned int NumberReturned, const unsigned int TotalMatches, const unsigned int UpdateID);
void UPnPResponse_ContentDirectory_ImportResource(const void* UPnPToken, const unsigned int TransferID);
void UPnPResponse_ContentDirectory_CreateReference(const void* UPnPToken, const char* NewID);
void UPnPResponse_ContentDirectory_DeleteResource(const void* UPnPToken);

/* State Variable Eventing Methods */
void UPnPSetState_ConnectionManager_SourceProtocolInfo(void *microstack,char* val);
void UPnPSetState_ConnectionManager_SinkProtocolInfo(void *microstack,char* val);
void UPnPSetState_ConnectionManager_CurrentConnectionIDs(void *microstack,char* val);
void UPnPSetState_ContentDirectory_TransferIDs(void *microstack,char* val);
void UPnPSetState_ContentDirectory_ContainerUpdateIDs(void *microstack,char* val);
void UPnPSetState_ContentDirectory_SystemUpdateID(void *microstack,unsigned int val);

#endif
