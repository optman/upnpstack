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
extern void UpnpChannelManager_RegisterChannel(void* upnptoken,char* Name,char* PeerConnection,int Timeout);
extern void UpnpChannelManager_UnregisterChannel(void* upnptoken,char* PeerConnection);
extern void UpnpChannelManager_ClearAllChannels(void* upnptoken);
extern void UpnpChannelManager_GetRegisteredChannelList(void* upnptoken);
extern void UpnpRemoteInput_InputKeyPress(void* upnptoken,int key);
extern void UpnpRemoteInput_InputMouseMove(void* upnptoken,int X,int Y);
extern void UpnpRemoteInput_GetInputSetup(void* upnptoken);
extern void UpnpRemoteInput_InputMouseUp(void* upnptoken,int X,int Y,int Button);
extern void UpnpRemoteInput_InputKeyUp(void* upnptoken,int key);
extern void UpnpRemoteInput_InputKeyDown(void* upnptoken,int key);
extern void UpnpRemoteInput_InputMouseDown(void* upnptoken,int X,int Y,int Button);
extern void UpnpRemoteIO_ForceDisconnection(void* upnptoken);
extern void UpnpRemoteIO_GetPeerConnection(void* upnptoken);
extern void UpnpRemoteIO_ForceReset(void* upnptoken);
extern void UpnpRemoteIO_SetPeerInterlock(void* upnptoken,char* PeerConnection);
extern void UpnpRemoteIO_GetDeviceInformation(void* upnptoken);
extern void UpnpRemoteIO_SetPeerOverride(void* upnptoken,char* PeerConnection);
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
void UpnpResponse_RemoteIO_ForceDisconnection(const void* UPnPToken);
void UpnpResponse_RemoteIO_GetPeerConnection(const void* UPnPToken, const char* PeerConnection);
void UpnpResponse_RemoteIO_ForceReset(const void* UPnPToken);
void UpnpResponse_RemoteIO_SetPeerInterlock(const void* UPnPToken, const char* ActivePeerConnection);
void UpnpResponse_RemoteIO_GetDeviceInformation(const void* UPnPToken, const char* Application, const unsigned int MaxCommandSize, const int DisplayEncoding, const unsigned int DisplayWidth, const unsigned int DisplayHeight, const char* DeviceInformation);
void UpnpResponse_RemoteIO_SetPeerOverride(const void* UPnPToken);
void UpnpResponse_RemoteInput_InputKeyPress(const void* UPnPToken);
void UpnpResponse_RemoteInput_InputMouseMove(const void* UPnPToken);
void UpnpResponse_RemoteInput_GetInputSetup(const void* UPnPToken, const char* InputSetupIdentifier);
void UpnpResponse_RemoteInput_InputMouseUp(const void* UPnPToken);
void UpnpResponse_RemoteInput_InputKeyUp(const void* UPnPToken);
void UpnpResponse_RemoteInput_InputKeyDown(const void* UPnPToken);
void UpnpResponse_RemoteInput_InputMouseDown(const void* UPnPToken);
void UpnpResponse_ChannelManager_RegisterChannel(const void* UPnPToken);
void UpnpResponse_ChannelManager_UnregisterChannel(const void* UPnPToken);
void UpnpResponse_ChannelManager_ClearAllChannels(const void* UPnPToken);
void UpnpResponse_ChannelManager_GetRegisteredChannelList(const void* UPnPToken, const char* ChannelList);

/* State Variable Eventing Methods */
void UpnpSetState_RemoteIO_PeerConnection(void *microstack,char* val);
void UpnpSetState_ChannelManager_RegisteredChannelList(void *microstack,char* val);

#endif
