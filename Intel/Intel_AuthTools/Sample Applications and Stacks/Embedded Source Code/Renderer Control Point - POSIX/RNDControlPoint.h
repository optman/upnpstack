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
* $Workfile: RNDControlPoint.h
* $Revision: #1.0.1189.31920
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, April 07, 2003
*
*/
#ifndef __RNDControlPoint__
#define __RNDControlPoint__

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


void RNDAddRef(struct UPnPDevice *device);
void RNDRelease(struct UPnPDevice *device);

struct UPnPDevice* RNDGetDevice1(struct UPnPDevice *device,int index);
int RNDGetDeviceCount(struct UPnPDevice *device);

void PrintUPnPDevice(int indents, struct UPnPDevice *device);

void *RNDCreateControlPoint(void *Chain, void(*A)(struct UPnPDevice*),void(*R)(struct UPnPDevice*));
void RND_CP_IPAddressListChanged(void *CPToken);
struct UPnPDevice* RNDGetDevice(struct UPnPDevice *device, char* DeviceType, int number);
int RNDHasAction(struct UPnPService *s, char* action);
void RNDSubscribeForUPnPEvents(struct UPnPService *service, void(*callbackPtr)(struct UPnPService* service,int OK));
struct UPnPService *RNDGetService(struct UPnPDevice *device, char* ServiceName, int length);
struct UPnPService *RNDGetService_ConnectionManager(struct UPnPDevice *device);
struct UPnPService *RNDGetService_AVTransport(struct UPnPDevice *device);
struct UPnPService *RNDGetService_RenderingControl(struct UPnPDevice *device);

extern void (*RNDEventCallback_ConnectionManager_SourceProtocolInfo)(struct UPnPService* Service,char* SourceProtocolInfo);
extern void (*RNDEventCallback_ConnectionManager_SinkProtocolInfo)(struct UPnPService* Service,char* SinkProtocolInfo);
extern void (*RNDEventCallback_ConnectionManager_CurrentConnectionIDs)(struct UPnPService* Service,char* CurrentConnectionIDs);
extern void (*RNDEventCallback_AVTransport_LastChange)(struct UPnPService* Service,char* LastChange);
extern void (*RNDEventCallback_RenderingControl_LastChange)(struct UPnPService* Service,char* LastChange);

void RNDInvoke_ConnectionManager_GetCurrentConnectionInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status),void* _user, int ConnectionID);
void RNDInvoke_ConnectionManager_GetProtocolInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* Source,char* Sink),void* _user);
void RNDInvoke_ConnectionManager_GetCurrentConnectionIDs(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* ConnectionIDs),void* _user);
void RNDInvoke_AVTransport_GetCurrentTransportActions(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* Actions),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_Play(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Speed);
void RNDInvoke_AVTransport_Previous(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_Next(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_Stop(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_GetTransportSettings(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* PlayMode,char* RecQualityMode),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_Seek(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Unit, char* Target);
void RNDInvoke_AVTransport_Pause(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_GetPositionInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned int Track,char* TrackDuration,char* TrackMetaData,char* TrackURI,char* RelTime,char* AbsTime,int RelCount,int AbsCount),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_GetTransportInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* CurrentTransportState,char* CurrentTransportStatus,char* CurrentSpeed),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_SetAVTransportURI(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* CurrentURI, char* CurrentURIMetaData);
void RNDInvoke_AVTransport_GetDeviceCapabilities(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* PlayMedia,char* RecMedia,char* RecQualityModes),void* _user, unsigned int InstanceID);
void RNDInvoke_AVTransport_SetPlayMode(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* NewPlayMode);
void RNDInvoke_AVTransport_GetMediaInfo(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned int NrTracks,char* MediaDuration,char* CurrentURI,char* CurrentURIMetaData,char* NextURI,char* NextURIMetaData,char* PlayMedium,char* RecordMedium,char* WriteStatus),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetHorizontalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,short CurrentHorizontalKeystone),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetVolume(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentVolume),void* _user, unsigned int InstanceID, char* Channel);
void RNDInvoke_RenderingControl_SelectPreset(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* PresetName);
void RNDInvoke_RenderingControl_SetVolume(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Channel, unsigned short DesiredVolume);
void RNDInvoke_RenderingControl_ListPresets(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,char* CurrentPresetNameList),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_SetVolumeDB(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Channel, short DesiredVolume);
void RNDInvoke_RenderingControl_SetRedVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredRedVideoBlackLevel);
void RNDInvoke_RenderingControl_SetContrast(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredContrast);
void RNDInvoke_RenderingControl_SetLoudness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Channel, int DesiredLoudness);
void RNDInvoke_RenderingControl_SetBrightness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBrightness);
void RNDInvoke_RenderingControl_GetLoudness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,int CurrentLoudness),void* _user, unsigned int InstanceID, char* Channel);
void RNDInvoke_RenderingControl_GetColorTemperature(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentColorTemperature),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetSharpness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentSharpness),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetContrast(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentContrast),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetGreenVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentGreenVideoGain),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_SetRedVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredRedVideoGain);
void RNDInvoke_RenderingControl_SetGreenVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredGreenVideoBlackLevel);
void RNDInvoke_RenderingControl_GetVolumeDBRange(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,short MinValue,short MaxValue),void* _user, unsigned int InstanceID, char* Channel);
void RNDInvoke_RenderingControl_GetRedVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentRedVideoBlackLevel),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetBlueVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentBlueVideoBlackLevel),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetBlueVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentBlueVideoGain),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_SetBlueVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBlueVideoBlackLevel);
void RNDInvoke_RenderingControl_GetMute(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,int CurrentMute),void* _user, unsigned int InstanceID, char* Channel);
void RNDInvoke_RenderingControl_SetBlueVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredBlueVideoGain);
void RNDInvoke_RenderingControl_GetVerticalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,short CurrentVerticalKeystone),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_SetVerticalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, short DesiredVerticalKeystone);
void RNDInvoke_RenderingControl_GetBrightness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentBrightness),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetVolumeDB(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,short CurrentVolume),void* _user, unsigned int InstanceID, char* Channel);
void RNDInvoke_RenderingControl_GetGreenVideoBlackLevel(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentGreenVideoBlackLevel),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_GetRedVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user,unsigned short CurrentRedVideoGain),void* _user, unsigned int InstanceID);
void RNDInvoke_RenderingControl_SetMute(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, char* Channel, int DesiredMute);
void RNDInvoke_RenderingControl_SetGreenVideoGain(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredGreenVideoGain);
void RNDInvoke_RenderingControl_SetSharpness(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredSharpness);
void RNDInvoke_RenderingControl_SetHorizontalKeystone(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, short DesiredHorizontalKeystone);
void RNDInvoke_RenderingControl_SetColorTemperature(struct UPnPService *service, void (*CallbackPtr)(struct UPnPService*,int ErrorCode,void *user),void* _user, unsigned int InstanceID, unsigned short DesiredColorTemperature);

#endif
