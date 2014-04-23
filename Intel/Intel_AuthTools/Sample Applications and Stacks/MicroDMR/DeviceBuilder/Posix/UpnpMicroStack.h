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
extern void UpnpRenderingControl_SetVolume(void* upnptoken,unsigned int InstanceID,char* Channel,unsigned short DesiredVolume);
extern void UpnpRenderingControl_GetMute(void* upnptoken,unsigned int InstanceID,char* Channel);
extern void UpnpRenderingControl_SetMute(void* upnptoken,unsigned int InstanceID,char* Channel,int DesiredMute);
extern void UpnpRenderingControl_GetVolume(void* upnptoken,unsigned int InstanceID,char* Channel);
extern void UpnpRemoteIO_GetPeerConnection(void* upnptoken);
extern void UpnpRemoteIO_ForceReset(void* upnptoken);
extern void UpnpRemoteIO_GetDeviceInformation(void* upnptoken);
extern void UpnpRemoteIO_SetPeerInterlock(void* upnptoken,char* PeerConnection);
extern void UpnpRemoteIO_SetPeerOverride(void* upnptoken,char* PeerConnection);
extern void UpnpRemoteIO_ForceDisconnection(void* upnptoken);
extern void UpnpConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID);
extern void UpnpConnectionManager_GetProtocolInfo(void* upnptoken);
extern void UpnpConnectionManager_GetCurrentConnectionIDs(void* upnptoken);
extern void UpnpAVTransport_GetCurrentTransportActions(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_Play(void* upnptoken,unsigned int InstanceID,char* Speed);
extern void UpnpAVTransport_GetDeviceCapabilities(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_GetMediaInfo(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_Previous(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_Next(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_GetTransportSettings(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_SetAVTransportURI(void* upnptoken,unsigned int InstanceID,char* CurrentURI,char* CurrentURIMetaData);
extern void UpnpAVTransport_Pause(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_GetPositionInfo(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_Seek(void* upnptoken,unsigned int InstanceID,char* Unit,char* Target);
extern void UpnpAVTransport_GetTransportInfo(void* upnptoken,unsigned int InstanceID);
extern void UpnpAVTransport_SetPlayMode(void* upnptoken,unsigned int InstanceID,char* NewPlayMode);
extern void UpnpAVTransport_Stop(void* upnptoken,unsigned int InstanceID);
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
void UpnpResponse_ConnectionManager_GetCurrentConnectionInfo(const void* UPnPToken, const int RcsID, const int AVTransportID, const char* ProtocolInfo, const char* PeerConnectionManager, const int PeerConnectionID, const char* Direction, const char* Status);
void UpnpResponse_ConnectionManager_GetProtocolInfo(const void* UPnPToken, const char* Source, const char* Sink);
void UpnpResponse_ConnectionManager_GetCurrentConnectionIDs(const void* UPnPToken, const char* ConnectionIDs);
void UpnpResponse_RenderingControl_SetVolume(const void* UPnPToken);
void UpnpResponse_RenderingControl_GetMute(const void* UPnPToken, const int CurrentMute);
void UpnpResponse_RenderingControl_SetMute(const void* UPnPToken);
void UpnpResponse_RenderingControl_GetVolume(const void* UPnPToken, const unsigned short CurrentVolume);
void UpnpResponse_AVTransport_GetCurrentTransportActions(const void* UPnPToken, const char* Actions);
void UpnpResponse_AVTransport_Play(const void* UPnPToken);
void UpnpResponse_AVTransport_GetDeviceCapabilities(const void* UPnPToken, const char* PlayMedia, const char* RecMedia, const char* RecQualityModes);
void UpnpResponse_AVTransport_GetMediaInfo(const void* UPnPToken, const unsigned int NrTracks, const char* MediaDuration, const char* CurrentURI, const char* CurrentURIMetaData, const char* NextURI, const char* NextURIMetaData, const char* PlayMedium, const char* RecordMedium, const char* WriteStatus);
void UpnpResponse_AVTransport_Previous(const void* UPnPToken);
void UpnpResponse_AVTransport_Next(const void* UPnPToken);
void UpnpResponse_AVTransport_GetTransportSettings(const void* UPnPToken, const char* PlayMode, const char* RecQualityMode);
void UpnpResponse_AVTransport_SetAVTransportURI(const void* UPnPToken);
void UpnpResponse_AVTransport_Pause(const void* UPnPToken);
void UpnpResponse_AVTransport_GetPositionInfo(const void* UPnPToken, const unsigned int Track, const char* TrackDuration, const char* TrackMetaData, const char* TrackURI, const char* RelTime, const char* AbsTime, const int RelCount, const int AbsCount);
void UpnpResponse_AVTransport_Seek(const void* UPnPToken);
void UpnpResponse_AVTransport_GetTransportInfo(const void* UPnPToken, const char* CurrentTransportState, const char* CurrentTransportStatus, const char* CurrentSpeed);
void UpnpResponse_AVTransport_SetPlayMode(const void* UPnPToken);
void UpnpResponse_AVTransport_Stop(const void* UPnPToken);
void UpnpResponse_RemoteIO_GetPeerConnection(const void* UPnPToken, const char* PeerConnection);
void UpnpResponse_RemoteIO_ForceReset(const void* UPnPToken);
void UpnpResponse_RemoteIO_GetDeviceInformation(const void* UPnPToken, const char* Application, const unsigned int MaxCommandSize, const int DisplayEncoding, const unsigned int DisplayWidth, const unsigned int DisplayHeight, const char* DeviceInformation);
void UpnpResponse_RemoteIO_SetPeerInterlock(const void* UPnPToken, const char* ActivePeerConnection);
void UpnpResponse_RemoteIO_SetPeerOverride(const void* UPnPToken);
void UpnpResponse_RemoteIO_ForceDisconnection(const void* UPnPToken);
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
void UpnpSetState_ConnectionManager_SourceProtocolInfo(void *microstack,char* val);
void UpnpSetState_ConnectionManager_SinkProtocolInfo(void *microstack,char* val);
void UpnpSetState_ConnectionManager_CurrentConnectionIDs(void *microstack,char* val);
void UpnpSetState_RenderingControl_LastChange(void *microstack,char* val);
void UpnpSetState_AVTransport_LastChange(void *microstack,char* val);
void UpnpSetState_RemoteIO_PeerConnection(void *microstack,char* val);
void UpnpSetState_ChannelManager_RegisteredChannelList(void *microstack,char* val);

#endif
