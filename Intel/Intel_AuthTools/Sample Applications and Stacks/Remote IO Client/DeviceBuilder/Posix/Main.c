/* UPnP Device Main Module */

#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "ILibParsers.h"
#include "UpnpMicroStack.h"

void *UpnpmicroStackChain;
void *UpnpmicroStack;

void *UpnpMonitor;
int UpnpIPAddressLength;
int *UpnpIPAddressList;

void UpnpRemoteIO_ForceDisconnection(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_ForceDisconnection();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_ForceDisconnection(upnptoken);
}

void UpnpRemoteIO_GetPeerConnection(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_GetPeerConnection();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_GetPeerConnection(upnptoken,"Sample String");
}

void UpnpRemoteIO_ForceReset(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_ForceReset();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_ForceReset(upnptoken);
}

void UpnpRemoteIO_SetPeerInterlock(void* upnptoken,char* PeerConnection)
{
	printf("Invoke: UpnpRemoteIO_SetPeerInterlock(%s);\r\n",PeerConnection);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_SetPeerInterlock(upnptoken,"Sample String");
}

void UpnpRemoteIO_GetDeviceInformation(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_GetDeviceInformation();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_GetDeviceInformation(upnptoken,"Sample String",250,25000,250,250,"Sample String");
}

void UpnpRemoteIO_SetPeerOverride(void* upnptoken,char* PeerConnection)
{
	printf("Invoke: UpnpRemoteIO_SetPeerOverride(%s);\r\n",PeerConnection);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_SetPeerOverride(upnptoken);
}

void UpnpRemoteInput_InputKeyPress(void* upnptoken,int key)
{
	printf("Invoke: UpnpRemoteInput_InputKeyPress(%d);\r\n",key);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputKeyPress(upnptoken);
}

void UpnpRemoteInput_InputMouseMove(void* upnptoken,int X,int Y)
{
	printf("Invoke: UpnpRemoteInput_InputMouseMove(%d,%d);\r\n",X,Y);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputMouseMove(upnptoken);
}

void UpnpRemoteInput_GetInputSetup(void* upnptoken)
{
	printf("Invoke: UpnpRemoteInput_GetInputSetup();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_GetInputSetup(upnptoken,"Sample String");
}

void UpnpRemoteInput_InputMouseUp(void* upnptoken,int X,int Y,int Button)
{
	printf("Invoke: UpnpRemoteInput_InputMouseUp(%d,%d,%d);\r\n",X,Y,Button);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputMouseUp(upnptoken);
}

void UpnpRemoteInput_InputKeyUp(void* upnptoken,int key)
{
	printf("Invoke: UpnpRemoteInput_InputKeyUp(%d);\r\n",key);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputKeyUp(upnptoken);
}

void UpnpRemoteInput_InputKeyDown(void* upnptoken,int key)
{
	printf("Invoke: UpnpRemoteInput_InputKeyDown(%d);\r\n",key);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputKeyDown(upnptoken);
}

void UpnpRemoteInput_InputMouseDown(void* upnptoken,int X,int Y,int Button)
{
	printf("Invoke: UpnpRemoteInput_InputMouseDown(%d,%d,%d);\r\n",X,Y,Button);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteInput_InputMouseDown(upnptoken);
}

void UpnpChannelManager_RegisterChannel(void* upnptoken,char* Name,char* PeerConnection,int Timeout)
{
	printf("Invoke: UpnpChannelManager_RegisterChannel(%s,%s,%d);\r\n",Name,PeerConnection,Timeout);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ChannelManager_RegisterChannel(upnptoken);
}

void UpnpChannelManager_UnregisterChannel(void* upnptoken,char* PeerConnection)
{
	printf("Invoke: UpnpChannelManager_UnregisterChannel(%s);\r\n",PeerConnection);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ChannelManager_UnregisterChannel(upnptoken);
}

void UpnpChannelManager_ClearAllChannels(void* upnptoken)
{
	printf("Invoke: UpnpChannelManager_ClearAllChannels();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ChannelManager_ClearAllChannels(upnptoken);
}

void UpnpChannelManager_GetRegisteredChannelList(void* upnptoken)
{
	printf("Invoke: UpnpChannelManager_GetRegisteredChannelList();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ChannelManager_GetRegisteredChannelList(upnptoken,"Sample String");
}

void UpnpPresentationRequest(void* upnptoken, struct packetheader *packet)
{
	printf("Upnp Presentation Request: %s %s\r\n", packet->Directive,packet->DirectiveObj);
	
	/* TODO: Add Web Response Code Here... */
	printf("HOST: %x\r\n",UpnpGetLocalInterfaceToHost(upnptoken));
	
	UpnpPresentationResponse(upnptoken, "HTTP/1.0 200 OK\r\n\r\n" , 19 , 1);
}

void UpnpIPAddressMonitor(void *data)
{
	int length;
	int *list;
	
	length = ILibGetLocalIPAddressList(&list);
	if(length!=UpnpIPAddressLength || memcmp((void*)list,(void*)UpnpIPAddressList,sizeof(int)*length)!=0)
	{
		UpnpIPAddressListChanged(UpnpmicroStack);
		
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
	ILibStopChain(UpnpmicroStackChain);
}
int main(void)
{
	UpnpmicroStackChain = ILibCreateChain();
	
	/* TODO: Each device must have a unique device identifier (UDN) */
	UpnpmicroStack = UpnpCreateMicroStack(UpnpmicroStackChain,"Remote IO Micro Client","22324477-e00e-4c5f-a18f-622f0dac4b6f","0000001",1800,0);
	
	/* All evented state variables MUST be initialized before UPnPStart is called. */
	UpnpSetState_RemoteIO_PeerConnection(UpnpmicroStack,"Sample String");
	UpnpSetState_ChannelManager_RegisteredChannelList(UpnpmicroStack,"Sample String");
	
	printf("Intel MicroStack 1.0 - Remote IO Micro Client\r\n\r\n");
	
	UpnpMonitor = ILibCreateLifeTime(UpnpmicroStackChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
	
	signal(SIGINT,BreakSink);
	ILibStartChain(UpnpmicroStackChain);
	
	FREE(UpnpIPAddressList);
	return 0;
}

