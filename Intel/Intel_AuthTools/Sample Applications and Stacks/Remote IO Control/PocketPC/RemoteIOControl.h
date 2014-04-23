#ifndef __RemoteIOControl__
#define __RemoteIOControl__

enum RemoteIOStateEnum
{
	RIO_INITIALIZE = 0,
	RIO_READY      = 1,
	RIO_CONNECTED  = 2,
};

struct RemoteIODevice
{
	char*  UDN;
	char*  FriendlyName;
	void*  Device;
	int    HasRemoteInput;
	int    HasChannelManager;
	char*  PeerConnection;
	void*  Tag;

	char*        Application;
	unsigned int MaxCommandSize;
	int          DisplayEncoding;
	unsigned int DisplayWidth;
	unsigned int DisplayHeight;
	char*        DeviceInformation;
	char*        InputSetup;

	enum RemoteIOStateEnum  State;
	int ChannelCount;
	struct RemoteIOChannel** Channels;
	char* ChannelData;
};

struct RemoteIOChannel
{
	char* Name;
	char* URI;
};

extern void (*RemoteIOEventCallback_ClientAdded)     (struct RemoteIODevice* device);
extern void (*RemoteIOEventCallback_ClientRemoved)   (struct RemoteIODevice* device);
extern void (*RemoteIOEventCallback_StateChanged)    (struct RemoteIODevice* device);
extern void (*RemoteIOEventCallback_ChannelsChanged) (struct RemoteIODevice* device);
extern struct RemoteIODevice* RemoteIODeviceList;

void* RemoteIOCreateControl(void* threadchain);

void  RemoteIO_Lock();
void  RemoteIO_UnLock();

void  RemoteIO_Reset(struct RemoteIODevice* device);
void  RemoteIO_ForceDisconnection(struct RemoteIODevice* device);
void  RemoteIO_ConnectInterlock(struct RemoteIODevice* device,char* URI);
void  RemoteIO_ConnectOverride(struct RemoteIODevice* device,char* URI);

void  RemoteIO_InputKeyDown(struct RemoteIODevice* device,int key);
void  RemoteIO_InputKeyUp(struct RemoteIODevice* device,int key);
void  RemoteIO_InputKeyPress(struct RemoteIODevice* device,int key);
void  RemoteIO_InputMouseMove(struct RemoteIODevice* device,int x,int y);
void  RemoteIO_InputMouseUp(struct RemoteIODevice* device,int x,int y,int buttons);
void  RemoteIO_InputMouseDown(struct RemoteIODevice* device,int x,int y,int buttons);

#endif
