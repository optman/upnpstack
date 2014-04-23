/* Control Point Main Module */

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <memory.h>
#include "RIOControlPoint.h"
#include "ILibParsers.h"

void RIOResponseSink_RemoteInput_InputKeyPress(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputKeyPress()\r\n");
}

void RIOResponseSink_RemoteInput_InputMouseMove(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputMouseMove()\r\n");
}

void RIOResponseSink_RemoteInput_GetInputSetup(struct UPnPService* Service,int ErrorCode,void *User,char* InputSetupIdentifier)
{
	printf("RIO Invoke Response: RemoteInput/GetInputSetup(%s)\r\n",InputSetupIdentifier);
}

void RIOResponseSink_RemoteInput_InputMouseUp(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputMouseUp()\r\n");
}

void RIOResponseSink_RemoteInput_InputKeyUp(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputKeyUp()\r\n");
}

void RIOResponseSink_RemoteInput_InputKeyDown(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputKeyDown()\r\n");
}

void RIOResponseSink_RemoteInput_InputMouseDown(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteInput/InputMouseDown()\r\n");
}

void RIOResponseSink_RemoteIO_SetPeerOverride(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteIO/SetPeerOverride()\r\n");
}

void RIOResponseSink_RemoteIO_GetPeerConnection(struct UPnPService* Service,int ErrorCode,void *User,char* PeerConnection)
{
	printf("RIO Invoke Response: RemoteIO/GetPeerConnection(%s)\r\n",PeerConnection);
}

void RIOResponseSink_RemoteIO_ForceReset(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteIO/ForceReset()\r\n");
}

void RIOResponseSink_RemoteIO_GetDeviceInformation(struct UPnPService* Service,int ErrorCode,void *User,char* Application,unsigned int MaxCommandSize,int DisplayEncoding,unsigned int DisplayWidth,unsigned int DisplayHeight,char* DeviceInformation)
{
	printf("RIO Invoke Response: RemoteIO/GetDeviceInformation(%s,%u,%d,%u,%u,%s)\r\n",Application,MaxCommandSize,DisplayEncoding,DisplayWidth,DisplayHeight,DeviceInformation);
}

void RIOResponseSink_RemoteIO_SetPeerInterlock(struct UPnPService* Service,int ErrorCode,void *User,char* ActivePeerConnection)
{
	printf("RIO Invoke Response: RemoteIO/SetPeerInterlock(%s)\r\n",ActivePeerConnection);
}

void RIOResponseSink_RemoteIO_ForceDisconnection(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: RemoteIO/ForceDisconnection()\r\n");
}

void RIOResponseSink_ChannelManager_RegisterChannel(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: ChannelManager/RegisterChannel()\r\n");
}

void RIOResponseSink_ChannelManager_UnregisterChannel(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: ChannelManager/UnregisterChannel()\r\n");
}

void RIOResponseSink_ChannelManager_ClearAllChannels(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("RIO Invoke Response: ChannelManager/ClearAllChannels()\r\n");
}

void RIOResponseSink_ChannelManager_GetRegisteredChannelList(struct UPnPService* Service,int ErrorCode,void *User,char* ChannelList)
{
	printf("RIO Invoke Response: ChannelManager/GetRegisteredChannelList(%s)\r\n",ChannelList);
}

void RIOEventSink_RemoteIO_PeerConnection(struct UPnPService* Service,char* PeerConnection)
{
	printf("RIO Event from %s/RemoteIO/PeerConnection: %s\r\n",Service->Parent->FriendlyName,PeerConnection);
}

void RIOEventSink_ChannelManager_RegisteredChannelList(struct UPnPService* Service,char* RegisteredChannelList)
{
	printf("RIO Event from %s/ChannelManager/RegisteredChannelList: %s\r\n",Service->Parent->FriendlyName,RegisteredChannelList);
}

/* Called whenever a new device on the correct type is discovered */
void RIODeviceDiscoverSink(struct UPnPDevice *device)
{
	struct UPnPDevice *tempDevice = device;
	struct UPnPService *tempService;
	
	printf("RIO Device Added: %s\r\n", device->FriendlyName);
	
	/* This call will print the device, all embedded devices and service to the console. */
	/* It is just used for debugging. */
	/* 	RIOPrintUPnPDevice(0,device); */
	
	/* The following subscribes for events on all services */
	while(tempDevice!=NULL)
	{
		tempService = tempDevice->Services;
		while(tempService!=NULL)
		{
			RIOSubscribeForUPnPEvents(tempService,NULL);
			tempService = tempService->Next;
		}
		tempDevice = tempDevice->Next;
	}
	
	/* The following will call every method of every service in the device with sample values */
	/* You can cut & paste these lines where needed. The user value is NULL, it can be freely used */
	/* to pass state information. */
	/* The RIOGetService call can return NULL, a correct application must check this since a device */
	/* can be implemented without some services. */
	
	/* You can check for the existence of an action by calling: RIOHasAction(serviceStruct,serviceType) */
	/* where serviceStruct is the struct like tempService, and serviceType, is a null terminated string representing */
	/* the service urn. */
	
	tempService = RIOGetService_RemoteIO(device);
	RIOInvoke_RemoteIO_SetPeerOverride(tempService, &RIOResponseSink_RemoteIO_SetPeerOverride,NULL,"Sample String");
	RIOInvoke_RemoteIO_GetPeerConnection(tempService, &RIOResponseSink_RemoteIO_GetPeerConnection,NULL);
	RIOInvoke_RemoteIO_ForceReset(tempService, &RIOResponseSink_RemoteIO_ForceReset,NULL);
	RIOInvoke_RemoteIO_GetDeviceInformation(tempService, &RIOResponseSink_RemoteIO_GetDeviceInformation,NULL);
	RIOInvoke_RemoteIO_SetPeerInterlock(tempService, &RIOResponseSink_RemoteIO_SetPeerInterlock,NULL,"Sample String");
	RIOInvoke_RemoteIO_ForceDisconnection(tempService, &RIOResponseSink_RemoteIO_ForceDisconnection,NULL);
	
	tempService = RIOGetService_RemoteInput(device);
	RIOInvoke_RemoteInput_InputKeyPress(tempService, &RIOResponseSink_RemoteInput_InputKeyPress,NULL,25000);
	RIOInvoke_RemoteInput_InputMouseMove(tempService, &RIOResponseSink_RemoteInput_InputMouseMove,NULL,25000,25000);
	RIOInvoke_RemoteInput_GetInputSetup(tempService, &RIOResponseSink_RemoteInput_GetInputSetup,NULL);
	RIOInvoke_RemoteInput_InputMouseUp(tempService, &RIOResponseSink_RemoteInput_InputMouseUp,NULL,25000,25000,25000);
	RIOInvoke_RemoteInput_InputKeyUp(tempService, &RIOResponseSink_RemoteInput_InputKeyUp,NULL,25000);
	RIOInvoke_RemoteInput_InputKeyDown(tempService, &RIOResponseSink_RemoteInput_InputKeyDown,NULL,25000);
	RIOInvoke_RemoteInput_InputMouseDown(tempService, &RIOResponseSink_RemoteInput_InputMouseDown,NULL,25000,25000,25000);
	
	tempService = RIOGetService_ChannelManager(device);
	RIOInvoke_ChannelManager_RegisterChannel(tempService, &RIOResponseSink_ChannelManager_RegisterChannel,NULL,"Sample String","Sample String",25000);
	RIOInvoke_ChannelManager_UnregisterChannel(tempService, &RIOResponseSink_ChannelManager_UnregisterChannel,NULL,"Sample String");
	RIOInvoke_ChannelManager_ClearAllChannels(tempService, &RIOResponseSink_ChannelManager_ClearAllChannels,NULL);
	RIOInvoke_ChannelManager_GetRegisteredChannelList(tempService, &RIOResponseSink_ChannelManager_GetRegisteredChannelList,NULL);
	
}

/* Called whenever a discovered device was removed from the network */
void RIODeviceRemoveSink(struct UPnPDevice *device)
{
	printf("RIO Device Removed: %s\r\n", device->FriendlyName);
}

void *RIO_CP;
void *RIO_CP_chain;
/* This thread is used to monitor the user for a return keyboard input */
/* Calling RIOStopCP() will cause the Control Point thread to fall out after clean up */
DWORD WINAPI Run(LPVOID args)
{
	getchar();
	ILibStopChain(RIO_CP_chain);
	return 0;
}

/* Main entry point to the sample application */
int _tmain(int argc, _TCHAR* argv[])
{
	/* Event callback function registration code */
	RIOEventCallback_RemoteIO_PeerConnection=&RIOEventSink_RemoteIO_PeerConnection;
	RIOEventCallback_ChannelManager_RegisteredChannelList=&RIOEventSink_ChannelManager_RegisteredChannelList;
	
	printf("Intel Control Point Microstack 1.0\r\n");
	
	
	RIO_CP_chain = ILibCreateChain();
	RIO_CP = RIOCreateControlPoint(RIO_CP_chain,&RIODeviceDiscoverSink,&RIODeviceRemoveSink);
	
	CreateThread(NULL,0,&Run,NULL,0,NULL);
	ILibStartChain(RIO_CP_chain);
	return 0;
}

