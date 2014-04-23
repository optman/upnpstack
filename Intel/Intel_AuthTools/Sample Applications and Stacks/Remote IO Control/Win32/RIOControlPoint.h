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
* $Workfile: RIOControlPoint.h
* $Revision: #1.0.1186.32333
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Tuesday, April 01, 2003
*
*/
#ifndef __RIOControlPoint__
#define __RIOControlPoint__

struct UPnPDevice
{
	void* CP;
	char* DeviceType;
	char* UDN;
	
	char* LocationURL;
	char* PresentationURL;
	char* FriendlyName;
	char* ManufacturerName;
	char* ManufacturerURL;
	char* ModelName;
	char* ModelDescription;
	char* ModelNumber;
	char* ModelURL;
	
	int SCPDLeft;
	int ReferenceCount;
	char* InterfaceToHost;
	int CacheTime;
	void *Tag;
	
	struct UPnPDevice *Parent;
	struct UPnPDevice *EmbeddedDevices;
	struct UPnPService *Services;
	struct UPnPDevice *Next;
};

struct UPnPService
{
	char* ServiceType;
	char* ServiceId;
	char* ControlURL;
	char* SubscriptionURL;
	char* SCPDURL;
	char* SubscriptionID;
	
	struct UPnPAction *Actions;
	struct UPnPStateVariable *Variables;
	struct UPnPDevice *Parent;
	struct UPnPService *Next;
};

struct UPnPStateVariable
{
	struct UPnPStateVariable *Next;
	struct UPnPService *Parent;
	
	char* Name;
	char **AllowedValues;
	int NumAllowedValues;
	char* Min;
	char* Max;
	char* Step;
};

struct UPnPAction
{
	char* Name;
	struct UPnPAction *Next;
};

struct UPnPAllowedValue
{
	struct UPnPAllowedValue *Next;
	
	char* Value;
};


void RIOAddRef(struct UPnPDevice *device);
void RIORelease(struct UPnPDevice *device);

struct UPnPDevice* RIOGetDevice1(struct UPnPDevice *device,int index);
int RIOGetDeviceCount(struct UPnPDevice *device);

void PrintUPnPDevice(int indents, struct UPnPDevice *device);

void *RIOCreateControlPoint(void *Chain, void(*A)(struct UPnPDevice*),void(*R)(struct UPnPDevice*));
void RIO_CP_IPAddressListChanged(void *CPToken);
struct UPnPDevice* RIOGetDevice(struct UPnPDevice *device, char* DeviceType, int number);
int RIOHasAction(struct UPnPService *s, char* action);
void RIOSubscribeForUPnPEvents(struct UPnPService *service, void(*callbackPtr)(struct UPnPService* service,int OK));
struct UPnPService *RIOGetService(struct UPnPDevice *device, char* ServiceName, int length);
struct UPnPService *RIOGetService_ChannelManager(struct UPnPDevice *device);
struct UPnPService *RIOGetService_RemoteInput(struct UPnPDevice *device);
struct UPnPService *RIOGetService_RemoteIO(struct UPnPDevice *device);

extern void (*RIOEventCallback_ChannelManager_RegisteredChannelList)(struct UPnPService* Service,char* RegisteredChannelList);
extern void (*RIOEventCallback_RemoteIO_PeerConnection)(struct UPnPService* Service,char* PeerConnection);

void RIOInvoke_ChannelManager_RegisterChannel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, char* Name, char* PeerConnection, int Timeout);
void RIOInvoke_ChannelManager_UnregisterChannel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, char* PeerConnection);
void RIOInvoke_ChannelManager_ClearAllChannels(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user);
void RIOInvoke_ChannelManager_GetRegisteredChannelList(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* ChannelList),void* _user);
void RIOInvoke_RemoteInput_InputKeyPress(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int key);
void RIOInvoke_RemoteInput_InputMouseMove(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int X, int Y);
void RIOInvoke_RemoteInput_GetInputSetup(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* InputSetupIdentifier),void* _user);
void RIOInvoke_RemoteInput_InputMouseUp(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int X, int Y, int Button);
void RIOInvoke_RemoteInput_InputKeyUp(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int key);
void RIOInvoke_RemoteInput_InputKeyDown(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int key);
void RIOInvoke_RemoteInput_InputMouseDown(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, int X, int Y, int Button);
void RIOInvoke_RemoteIO_SetPeerOverride(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, char* PeerConnection);
void RIOInvoke_RemoteIO_GetPeerConnection(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* PeerConnection),void* _user);
void RIOInvoke_RemoteIO_ForceReset(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user);
void RIOInvoke_RemoteIO_GetDeviceInformation(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* Application,unsigned int MaxCommandSize,int DisplayEncoding,unsigned int DisplayWidth,unsigned int DisplayHeight,char* DeviceInformation),void* _user);
void RIOInvoke_RemoteIO_SetPeerInterlock(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* ActivePeerConnection),void* _user, char* PeerConnection);
void RIOInvoke_RemoteIO_ForceDisconnection(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user);

#endif
