/* Control Point Main Module */

//#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <memory.h>
#include "RIOControlPoint.h"
#include "ILibParsers.h"
#include "RemoteIOControl.h"

#define sem_t HANDLE
#define sem_init(x,y,z) *x=CreateSemaphore(NULL,z,FD_SETSIZE,NULL)
#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
#define sem_post(x) ReleaseSemaphore(*x,1,NULL)

static sem_t RemoteIOLock = NULL;
static int RemoteIOLock_RefCounter = 0;

void (*RemoteIOEventCallback_ClientAdded)    (struct RemoteIODevice* device) = NULL;
void (*RemoteIOEventCallback_ClientRemoved)  (struct RemoteIODevice* device) = NULL;
void (*RemoteIOEventCallback_StateChanged)   (struct RemoteIODevice* device) = NULL;
void (*RemoteIOEventCallback_ChannelsChanged)(struct RemoteIODevice* device) = NULL;
struct RemoteIODevice* RemoteIODeviceList = NULL;

void RIOResponseSink_RemoteIO_GetPeerConnection(struct UPnPService* Service,int ErrorCode,void *User,char* PeerConnection)
{
	struct RemoteIODevice* riodevice = (struct RemoteIODevice*)Service->Parent->Tag;

	if (ErrorCode != 0) return;
	sem_wait(&RemoteIOLock);
	if (riodevice->PeerConnection != NULL) free(riodevice->PeerConnection);
	riodevice->PeerConnection = NULL;
	if (PeerConnection != NULL && strlen(PeerConnection) != 0)
	{
		riodevice->PeerConnection = (char*)malloc((int)strlen(PeerConnection)+1);
		strcpy(riodevice->PeerConnection,PeerConnection);
	}
	if (riodevice->State != RIO_INITIALIZE)
	{
		riodevice->State = (riodevice->PeerConnection == NULL?RIO_READY:RIO_CONNECTED);
		sem_post(&RemoteIOLock);
		if (RemoteIOEventCallback_StateChanged != NULL) RemoteIOEventCallback_StateChanged(riodevice);
	}
	else
	{
		sem_post(&RemoteIOLock);
	}
}

void RIOResponseSink_RemoteIO_GetDeviceInformation(struct UPnPService* Service,int ErrorCode,void *User,char* Application,unsigned int MaxCommandSize,int DisplayEncoding,unsigned int DisplayWidth,unsigned int DisplayHeight,char* DeviceInformation)
{
	struct RemoteIODevice* riodevice = (struct RemoteIODevice*)Service->Parent->Tag;

	if (ErrorCode != 0) return;
	
	sem_wait(&RemoteIOLock);
	if (riodevice->Application != NULL) free(riodevice->Application);
	riodevice->Application = NULL;
	if (Application != NULL)
	{
		riodevice->Application = (char*)malloc((int)strlen(Application)+1);
		strcpy(riodevice->Application,Application);
	}

	riodevice->MaxCommandSize = MaxCommandSize;
	riodevice->DisplayEncoding = DisplayEncoding;
	riodevice->DisplayWidth = DisplayWidth;
	riodevice->DisplayHeight = DisplayHeight;
	
	if (riodevice->DeviceInformation != NULL) free(riodevice->DeviceInformation);
	if (DeviceInformation != NULL)
	{
		riodevice->DeviceInformation = (char*)malloc((int)strlen(DeviceInformation)+1);
		strcpy(riodevice->DeviceInformation,DeviceInformation);
	}
	
	riodevice->State = (riodevice->PeerConnection == NULL?RIO_READY:RIO_CONNECTED);
	sem_post(&RemoteIOLock);

	if (RemoteIOEventCallback_ClientAdded != NULL) RemoteIOEventCallback_ClientAdded(riodevice);
}

void RIOResponseSink_RemoteIO_ForceDisconnection(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteIO_ForceReset(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteIO_SetPeerInterlock(struct UPnPService* Service,int ErrorCode,void *User,char* ActivePeerConnection) {RIOResponseSink_RemoteIO_GetPeerConnection(Service,0,NULL,ActivePeerConnection);}
void RIOResponseSink_RemoteIO_SetPeerOverride(struct UPnPService* Service,int ErrorCode,void *User) {}

void RIOResponseSink_ChannelManager_RegisterChannel(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_ChannelManager_UnregisterChannel(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_ChannelManager_ClearAllChannels(struct UPnPService* Service,int ErrorCode,void *User) {}

void RIOResponseSink_ChannelManager_GetRegisteredChannelList(struct UPnPService* Service,int ErrorCode,void *User,char* ChannelList)
{
	struct RemoteIODevice* riodevice = (struct RemoteIODevice*)Service->Parent->Tag;
	int ChannelListLen;
	int ChannelCount = 0;
	char* ptr;
	struct RemoteIOChannel* ChannelPtr = NULL;
	int i,j,k;

	sem_wait(&RemoteIOLock);

	// Free the channel list
	for (i=0;i<riodevice->ChannelCount;i++) free(riodevice->Channels[i]);
	if (riodevice->ChannelData != NULL) free(riodevice->ChannelData);
	free(riodevice->Channels);
	riodevice->Channels = NULL;
	riodevice->ChannelData = NULL;
	riodevice->ChannelCount = 0;

	if (ChannelList == NULL)
	{
		if (RemoteIOEventCallback_ChannelsChanged != NULL) RemoteIOEventCallback_ChannelsChanged(riodevice);
		sem_post(&RemoteIOLock);
		return;
	}

	ChannelListLen = (int)strlen(ChannelList);

	// First, count the number of channels
	for (i=0;i<ChannelListLen;i++) if (ChannelList[i] == '\r') ChannelCount++;
	ChannelCount = (ChannelCount+1) >> 1; // Divide by 2

	// If no channels, just return
	if (ChannelCount == 0)
	{
		if (RemoteIOEventCallback_ChannelsChanged != NULL) RemoteIOEventCallback_ChannelsChanged(riodevice);
		sem_post(&RemoteIOLock);
		return;
	}

	riodevice->ChannelCount = ChannelCount;
	riodevice->Channels = malloc(sizeof(void*) * ChannelCount);
	riodevice->ChannelData = malloc(ChannelListLen+1);
	strcpy(riodevice->ChannelData,ChannelList);

	ptr = riodevice->ChannelData;
	j = 0;
	k = 0;
	for (i=0;i<ChannelListLen && k < ChannelCount;i++)
	{
		if (ChannelList[i] == '\r')
		{
			if (ChannelPtr == NULL)
			{
				ChannelPtr = malloc(sizeof(struct RemoteIOChannel));
				ChannelPtr->Name = ptr;
				ChannelPtr->Name[i-j] = 0;
				ptr = riodevice->ChannelData + i + 2;
				j = i+2;
			}
			else
			{
				ChannelPtr->URI = ptr;
				ChannelPtr->URI[i-j] = 0;
				ptr = riodevice->ChannelData + i + 2;
				j = i+2;
				riodevice->Channels[k] = ChannelPtr;
				k++;
				ChannelPtr = NULL;
			}
		}
	}
	if (ChannelPtr != NULL) free(ChannelPtr);
	sem_post(&RemoteIOLock);

	if (riodevice->State != RIO_INITIALIZE && RemoteIOEventCallback_ChannelsChanged != NULL) RemoteIOEventCallback_ChannelsChanged(riodevice);
}

void RIOResponseSink_RemoteInput_GetInputSetup(struct UPnPService* Service,int ErrorCode,void *User,char* InputSetupIdentifier)
{
	struct RemoteIODevice* riodevice = (struct RemoteIODevice*)Service->Parent->Tag;

	sem_wait(&RemoteIOLock);
	if (riodevice->InputSetup != NULL) free(riodevice->InputSetup);
	riodevice->InputSetup = NULL;
	if (InputSetupIdentifier != NULL && strlen(InputSetupIdentifier) != 0)
	{
		riodevice->InputSetup = (char*)malloc((int)strlen(InputSetupIdentifier)+1);
		strcpy(riodevice->InputSetup,InputSetupIdentifier);
	}
	sem_post(&RemoteIOLock);
}

void RIOResponseSink_RemoteInput_InputKeyUp(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteInput_InputKeyPress(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteInput_InputKeyDown(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteInput_InputMouseUp(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteInput_InputMouseMove(struct UPnPService* Service,int ErrorCode,void *User) {}
void RIOResponseSink_RemoteInput_InputMouseDown(struct UPnPService* Service,int ErrorCode,void *User) {}

void RIOEventSink_RemoteIO_PeerConnection(struct UPnPService* Service,char* PeerConnection)
{
	// Route to the action return call
	RIOResponseSink_RemoteIO_GetPeerConnection(Service,0,NULL,PeerConnection);
}

void RIOEventSink_ChannelManager_RegisteredChannelList(struct UPnPService* Service,char* RegisteredChannelList)
{
	// Route to the action return call
	RIOResponseSink_ChannelManager_GetRegisteredChannelList(Service,0,NULL,RegisteredChannelList);
}

/* Called whenever a new device on the correct type is discovered */
void RIODeviceDiscoverSink(struct UPnPDevice *device)
{
	struct UPnPDevice *tempDevice = device;
	struct UPnPService *tempService;
	struct RemoteIODevice* riodevice = malloc(sizeof(struct RemoteIODevice));

	if (RemoteIOLock_RefCounter == 0) sem_init(&RemoteIOLock,0,1);
	RemoteIOLock_RefCounter++;

	device->Tag = riodevice;
	riodevice->FriendlyName = device->FriendlyName;
	riodevice->UDN = device->UDN;
	riodevice->Device = device;
	riodevice->State = RIO_INITIALIZE;
	riodevice->HasRemoteInput = (RIOGetService_RemoteInput(device)==NULL?0:1);
	riodevice->HasChannelManager = (RIOGetService_ChannelManager(device)==NULL?0:1);
	riodevice->PeerConnection = NULL;
	riodevice->Application = NULL;
	riodevice->MaxCommandSize = 0;
	riodevice->DisplayEncoding = 0;
	riodevice->DisplayWidth = 0;
	riodevice->DisplayHeight = 0;
	riodevice->DeviceInformation = NULL;
	riodevice->InputSetup = NULL;
	riodevice->ChannelCount = 0;
	riodevice->Channels = NULL;
	riodevice->ChannelData = NULL;
	riodevice->Tag = NULL;

	tempService = RIOGetService_RemoteIO(device);
	RIOInvoke_RemoteIO_GetPeerConnection(tempService, &RIOResponseSink_RemoteIO_GetPeerConnection,NULL);
	RIOInvoke_RemoteIO_GetDeviceInformation(tempService, &RIOResponseSink_RemoteIO_GetDeviceInformation,NULL);

	tempService = RIOGetService_ChannelManager(device);
	if (tempService != NULL) RIOInvoke_ChannelManager_GetRegisteredChannelList(tempService, &RIOResponseSink_ChannelManager_GetRegisteredChannelList,NULL);

	/* The following subscribes for events on all services */
	/* TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 
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
	*/

	/* The following will call every method of every service in the device with sample values */
	/* You can cut & paste these lines where needed. The user value is NULL, it can be freely used */
	/* to pass state information. */
	/* The RIOGetService call can return NULL, a correct application must check this since a device */
	/* can be implemented without some services. */
	
	/* You can check for the existence of an action by calling: RIOHasAction(serviceStruct,serviceType) */
	/* where serviceStruct is the struct like tempService, and serviceType, is a null terminated string representing */
	/* the service urn. */
	
	/*
	tempService = RIOGetService_RemoteIO(device);
	RIOInvoke_RemoteIO_ForceDisconnection(tempService, &RIOResponseSink_RemoteIO_ForceDisconnection,NULL);
	RIOInvoke_RemoteIO_GetPeerConnection(tempService, &RIOResponseSink_RemoteIO_GetPeerConnection,NULL);
	RIOInvoke_RemoteIO_ForceReset(tempService, &RIOResponseSink_RemoteIO_ForceReset,NULL);
	RIOInvoke_RemoteIO_SetPeerInterlock(tempService, &RIOResponseSink_RemoteIO_SetPeerInterlock,NULL,"Sample String");
	RIOInvoke_RemoteIO_GetDeviceInformation(tempService, &RIOResponseSink_RemoteIO_GetDeviceInformation,NULL);
	RIOInvoke_RemoteIO_SetPeerOverride(tempService, &RIOResponseSink_RemoteIO_SetPeerOverride,NULL,"Sample String");
	
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
	*/
	
}

void RemoteIO_Reset(struct RemoteIODevice* device)                                  {RIOInvoke_RemoteIO_ForceReset(RIOGetService_RemoteIO(device->Device), &RIOResponseSink_RemoteIO_ForceReset,NULL);}
void RemoteIO_ForceDisconnection(struct RemoteIODevice* device)                     {RIOInvoke_RemoteIO_ForceDisconnection(RIOGetService_RemoteIO(device->Device),&RIOResponseSink_RemoteIO_ForceDisconnection,NULL);}
void RemoteIO_ConnectInterlock(struct RemoteIODevice* device,char* URI)             {RIOInvoke_RemoteIO_SetPeerInterlock(RIOGetService_RemoteIO(device->Device), &RIOResponseSink_RemoteIO_SetPeerInterlock,NULL,URI);}
void RemoteIO_ConnectOverride(struct RemoteIODevice* device,char* URI)              {RIOInvoke_RemoteIO_SetPeerOverride(RIOGetService_RemoteIO(device->Device), &RIOResponseSink_RemoteIO_SetPeerOverride,NULL,URI);}

void RemoteIO_InputKeyDown(struct RemoteIODevice* device,int key)                   {RIOInvoke_RemoteInput_InputKeyDown (RIOGetService_RemoteInput(device->Device), &RIOResponseSink_RemoteInput_InputKeyDown,NULL,key);}
void RemoteIO_InputKeyUp(struct RemoteIODevice* device,int key)                     {RIOInvoke_RemoteInput_InputKeyUp   (RIOGetService_RemoteInput(device->Device), &RIOResponseSink_RemoteInput_InputKeyUp,NULL,key);}
void RemoteIO_InputKeyPress(struct RemoteIODevice* device,int key)                  {RIOInvoke_RemoteInput_InputKeyPress(RIOGetService_RemoteInput(device->Device), &RIOResponseSink_RemoteInput_InputKeyPress,NULL,key);}
void RemoteIO_InputMouseMove(struct RemoteIODevice* device,int x,int y)             {RIOInvoke_RemoteInput_InputMouseMove(RIOGetService_RemoteInput(device->Device),&RIOResponseSink_RemoteInput_InputMouseMove,NULL,x,y);}
void RemoteIO_InputMouseUp(struct RemoteIODevice* device,int x,int y,int buttons)   {RIOInvoke_RemoteInput_InputMouseUp  (RIOGetService_RemoteInput(device->Device),&RIOResponseSink_RemoteInput_InputMouseUp,NULL,x,y,buttons);}
void RemoteIO_InputMouseDown(struct RemoteIODevice* device,int x,int y,int buttons) {RIOInvoke_RemoteInput_InputMouseDown(RIOGetService_RemoteInput(device->Device),&RIOResponseSink_RemoteInput_InputMouseDown,NULL,x,y,buttons);}

/* Called whenever a discovered device was removed from the network */
void RIODeviceRemoveSink(struct UPnPDevice *device)
{
	int i;
	struct RemoteIODevice* riodevice = (struct RemoteIODevice*)device->Tag;

	if (riodevice->State != RIO_INITIALIZE && RemoteIOEventCallback_ClientRemoved != NULL) RemoteIOEventCallback_ClientRemoved(riodevice);

	sem_wait(&RemoteIOLock);
	device->Tag = NULL;
	if (riodevice->PeerConnection    != NULL) free(riodevice->PeerConnection);
	if (riodevice->Application       != NULL) free(riodevice->Application);
	if (riodevice->DeviceInformation != NULL) free(riodevice->DeviceInformation);

	// Free the channel list
	for (i=0;i<riodevice->ChannelCount;i++) free(riodevice->Channels[i]);
	if (riodevice->ChannelData != NULL) free(riodevice->ChannelData);
	free(riodevice->Channels);
	riodevice->Channels = NULL;
	riodevice->ChannelData = NULL;
	riodevice->ChannelCount = 0;
	sem_post(&RemoteIOLock);

	RemoteIOLock_RefCounter--;
	if (RemoteIOLock_RefCounter == 0) sem_destroy(&RemoteIOLock);
}

/* Main entry point to the sample application */
void* RemoteIOCreateControl(void* threadchain)
{
	/* Event callback function registration code */
	RIOEventCallback_RemoteIO_PeerConnection=&RIOEventSink_RemoteIO_PeerConnection;
	RIOEventCallback_ChannelManager_RegisteredChannelList=&RIOEventSink_ChannelManager_RegisteredChannelList;
	return RIOCreateControlPoint(threadchain,&RIODeviceDiscoverSink,&RIODeviceRemoveSink);
}

void RemoteIO_Lock()
{
	sem_wait(&RemoteIOLock);
}

void RemoteIO_UnLock()
{
	sem_post(&RemoteIOLock);
}
