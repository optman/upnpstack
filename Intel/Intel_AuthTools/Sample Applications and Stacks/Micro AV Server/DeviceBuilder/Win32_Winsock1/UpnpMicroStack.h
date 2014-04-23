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
* $Workfile: UpnpMicrostack.h
* $Revision: #1.0.1189.22189
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Friday, April 04, 2003
*
*/
#ifndef __UpnpMicrostack__
#define __UpnpMicrostack__

struct UpnpDataObject;
struct packetheader;

/* These methods must be implemented by the user */
extern void UpnpConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID);
extern void UpnpConnectionManager_GetProtocolInfo(void* upnptoken);
extern void UpnpConnectionManager_GetCurrentConnectionIDs(void* upnptoken);
extern void UpnpContentDirectory_Browse(void* upnptoken,char* ObjectID,char* BrowseFlag,char* Filter,unsigned int StartingIndex,unsigned int RequestedCount,char* SortCriteria);
extern void UpnpContentDirectory_GetSortCapabilities(void* upnptoken);
extern void UpnpContentDirectory_GetSystemUpdateID(void* upnptoken);
extern void UpnpContentDirectory_GetSearchCapabilities(void* upnptoken);
extern void UpnpPresentationRequest(void* upnptoken, struct packetheader *packet);

/* UPnP Stack Management */
void *UpnpCreateMicroStack(void *Chain, const char* FriendlyName, const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void *Upnp(void *Chain, const char* FriendlyName,const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void UpnpIPAddressListChanged(void *MicroStackToken);
int UpnpGetLocalPortNumber(void *token);
int   UpnpGetLocalInterfaceToHost(const void* UPnPToken);
void* UpnpGetInstance(const void* UPnPToken);

/* Invocation Response Methods */
void UpnpResponse_Error(const void* UPnPToken, const int ErrorCode, const char* ErrorMsg);
void UpnpResponseGeneric(const void* UPnPToken,const char* ServiceURI,const char* MethodName,const char* Params);
int  UpnpPresentationResponse(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate);
void UpnpAsyncResponse_START(const void* UPnPToken, const char* actionName, const char* serviceUrnWithVersion);
void UpnpAsyncResponse_DONE(const void* UPnPToken, const char* actionName);
void UpnpAsyncResponse_OUT(const void* UPnPToken, const char* outArgName, const char* bytes, const int byteLength, const int startArg, const int endArg);
void UpnpResponse_ContentDirectory_Browse(const void* UPnPToken, const char* Result, const unsigned int NumberReturned, const unsigned int TotalMatches, const unsigned int UpdateID);
void UpnpResponse_ContentDirectory_GetSortCapabilities(const void* UPnPToken, const char* SortCaps);
void UpnpResponse_ContentDirectory_GetSystemUpdateID(const void* UPnPToken, const unsigned int Id);
void UpnpResponse_ContentDirectory_GetSearchCapabilities(const void* UPnPToken, const char* SearchCaps);
void UpnpResponse_ConnectionManager_GetCurrentConnectionInfo(const void* UPnPToken, const int RcsID, const int AVTransportID, const char* ProtocolInfo, const char* PeerConnectionManager, const int PeerConnectionID, const char* Direction, const char* Status);
void UpnpResponse_ConnectionManager_GetProtocolInfo(const void* UPnPToken, const char* Source, const char* Sink);
void UpnpResponse_ConnectionManager_GetCurrentConnectionIDs(const void* UPnPToken, const char* ConnectionIDs);

/* State Variable Eventing Methods */
void UpnpSetState_ContentDirectory_SystemUpdateID(void *microstack,unsigned int val);
void UpnpSetState_ConnectionManager_SourceProtocolInfo(void *microstack,char* val);
void UpnpSetState_ConnectionManager_SinkProtocolInfo(void *microstack,char* val);
void UpnpSetState_ConnectionManager_CurrentConnectionIDs(void *microstack,char* val);

#endif
