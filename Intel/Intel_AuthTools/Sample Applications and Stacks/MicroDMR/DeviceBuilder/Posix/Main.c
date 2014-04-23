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

void UpnpConnectionManager_GetCurrentConnectionInfo(void* upnptoken,int ConnectionID)
{
	printf("Invoke: UpnpConnectionManager_GetCurrentConnectionInfo(%d);\r\n",ConnectionID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ConnectionManager_GetCurrentConnectionInfo(upnptoken,25000,25000,"Sample String","Sample String",25000,"Sample String","Sample String");
}

void UpnpConnectionManager_GetProtocolInfo(void* upnptoken)
{
	printf("Invoke: UpnpConnectionManager_GetProtocolInfo();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ConnectionManager_GetProtocolInfo(upnptoken,"Sample String","Sample String");
}

void UpnpConnectionManager_GetCurrentConnectionIDs(void* upnptoken)
{
	printf("Invoke: UpnpConnectionManager_GetCurrentConnectionIDs();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_ConnectionManager_GetCurrentConnectionIDs(upnptoken,"Sample String");
}

void UpnpRenderingControl_SetVolume(void* upnptoken,unsigned int InstanceID,char* Channel,unsigned short DesiredVolume)
{
	printf("Invoke: UpnpRenderingControl_SetVolume(%u,%s,%u);\r\n",InstanceID,Channel,DesiredVolume);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RenderingControl_SetVolume(upnptoken);
}

void UpnpRenderingControl_GetMute(void* upnptoken,unsigned int InstanceID,char* Channel)
{
	printf("Invoke: UpnpRenderingControl_GetMute(%u,%s);\r\n",InstanceID,Channel);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RenderingControl_GetMute(upnptoken,1);
}

void UpnpRenderingControl_SetMute(void* upnptoken,unsigned int InstanceID,char* Channel,int DesiredMute)
{
	printf("Invoke: UpnpRenderingControl_SetMute(%u,%s,%d);\r\n",InstanceID,Channel,DesiredMute);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RenderingControl_SetMute(upnptoken);
}

void UpnpRenderingControl_GetVolume(void* upnptoken,unsigned int InstanceID,char* Channel)
{
	printf("Invoke: UpnpRenderingControl_GetVolume(%u,%s);\r\n",InstanceID,Channel);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RenderingControl_GetVolume(upnptoken,250);
}

void UpnpAVTransport_GetCurrentTransportActions(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetCurrentTransportActions(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetCurrentTransportActions(upnptoken,"Sample String");
}

void UpnpAVTransport_Play(void* upnptoken,unsigned int InstanceID,char* Speed)
{
	printf("Invoke: UpnpAVTransport_Play(%u,%s);\r\n",InstanceID,Speed);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Play(upnptoken);
}

void UpnpAVTransport_GetDeviceCapabilities(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetDeviceCapabilities(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetDeviceCapabilities(upnptoken,"Sample String","Sample String","Sample String");
}

void UpnpAVTransport_GetMediaInfo(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetMediaInfo(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetMediaInfo(upnptoken,250,"Sample String","Sample String","Sample String","Sample String","Sample String","Sample String","Sample String","Sample String");
}

void UpnpAVTransport_Previous(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_Previous(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Previous(upnptoken);
}

void UpnpAVTransport_Next(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_Next(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Next(upnptoken);
}

void UpnpAVTransport_GetTransportSettings(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetTransportSettings(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetTransportSettings(upnptoken,"Sample String","Sample String");
}

void UpnpAVTransport_SetAVTransportURI(void* upnptoken,unsigned int InstanceID,char* CurrentURI,char* CurrentURIMetaData)
{
	printf("Invoke: UpnpAVTransport_SetAVTransportURI(%u,%s,%s);\r\n",InstanceID,CurrentURI,CurrentURIMetaData);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_SetAVTransportURI(upnptoken);
}

void UpnpAVTransport_Pause(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_Pause(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Pause(upnptoken);
}

void UpnpAVTransport_GetPositionInfo(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetPositionInfo(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetPositionInfo(upnptoken,250,"Sample String","Sample String","Sample String","Sample String","Sample String",25000,25000);
}

void UpnpAVTransport_Seek(void* upnptoken,unsigned int InstanceID,char* Unit,char* Target)
{
	printf("Invoke: UpnpAVTransport_Seek(%u,%s,%s);\r\n",InstanceID,Unit,Target);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Seek(upnptoken);
}

void UpnpAVTransport_GetTransportInfo(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_GetTransportInfo(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_GetTransportInfo(upnptoken,"Sample String","Sample String","Sample String");
}

void UpnpAVTransport_SetPlayMode(void* upnptoken,unsigned int InstanceID,char* NewPlayMode)
{
	printf("Invoke: UpnpAVTransport_SetPlayMode(%u,%s);\r\n",InstanceID,NewPlayMode);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_SetPlayMode(upnptoken);
}

void UpnpAVTransport_Stop(void* upnptoken,unsigned int InstanceID)
{
	printf("Invoke: UpnpAVTransport_Stop(%u);\r\n",InstanceID);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_AVTransport_Stop(upnptoken);
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

void UpnpRemoteIO_GetDeviceInformation(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_GetDeviceInformation();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_GetDeviceInformation(upnptoken,"Sample String",250,25000,250,250,"Sample String");
}

void UpnpRemoteIO_SetPeerInterlock(void* upnptoken,char* PeerConnection)
{
	printf("Invoke: UpnpRemoteIO_SetPeerInterlock(%s);\r\n",PeerConnection);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_SetPeerInterlock(upnptoken,"Sample String");
}

void UpnpRemoteIO_SetPeerOverride(void* upnptoken,char* PeerConnection)
{
	printf("Invoke: UpnpRemoteIO_SetPeerOverride(%s);\r\n",PeerConnection);
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_SetPeerOverride(upnptoken);
}

void UpnpRemoteIO_ForceDisconnection(void* upnptoken)
{
	printf("Invoke: UpnpRemoteIO_ForceDisconnection();\r\n");
	
	/* TODO: Place Action Code Here... */
	
	/* UpnpResponse_Error(upnptoken,404,"Method Not Implemented"); */
	UpnpResponse_RemoteIO_ForceDisconnection(upnptoken);
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
	UpnpmicroStack = UpnpCreateMicroStack(UpnpmicroStackChain,"MicroDMR","6a8dbfd2-8f18-488e-b833-3ea64c9df5c1","0000001",1800,0);
	
	/* All evented state variables MUST be initialized before UPnPStart is called. */
	UpnpSetState_ConnectionManager_SourceProtocolInfo(UpnpmicroStack,"Sample String");
	UpnpSetState_ConnectionManager_SinkProtocolInfo(UpnpmicroStack,"Sample String");
	UpnpSetState_ConnectionManager_CurrentConnectionIDs(UpnpmicroStack,"Sample String");
	UpnpSetState_RenderingControl_LastChange(UpnpmicroStack,"Sample String");
	UpnpSetState_AVTransport_LastChange(UpnpmicroStack,"Sample String");
	UpnpSetState_RemoteIO_PeerConnection(UpnpmicroStack,"Sample String");
	UpnpSetState_ChannelManager_RegisteredChannelList(UpnpmicroStack,"Sample String");
	
	printf("Intel MicroStack 1.0 - MicroDMR\r\n\r\n");
	
	UpnpMonitor = ILibCreateLifeTime(UpnpmicroStackChain);
	UpnpIPAddressLength = ILibGetLocalIPAddressList(&UpnpIPAddressList);
	ILibLifeTime_Add(UpnpMonitor,NULL,4,&UpnpIPAddressMonitor,NULL);
	
	signal(SIGINT,BreakSink);
	ILibStartChain(UpnpmicroStackChain);
	
	FREE(UpnpIPAddressList);
	return 0;
}

