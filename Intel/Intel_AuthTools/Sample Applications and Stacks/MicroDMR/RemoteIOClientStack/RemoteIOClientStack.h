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
* $Workfile: RemoteIOClientStack.h
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, March 24, 2003
*
*/

// Remote IO Events
extern void (*RemoteIOConnectionChanged) (char* PeerConnection);
extern void (*RemoteIOReset) ();
extern void (*RemoteIOCommand) (unsigned short command, char* data, int datalength);

// Remote IO Settings
// Must be set before RemoteIOStart() is called
extern char*			RemoteIO_Application;
extern unsigned int		RemoteIO_MaxCommandSize;
extern int				RemoteIO_DisplayEncoding;
extern unsigned int		RemoteIO_DisplayWidth;
extern unsigned int		RemoteIO_DisplayHeight;
extern char*			RemoteIO_DeviceInformation;

// Remote IO Methods
void* CreateRemoteIO(void* Chain, void* UpnpStack);
void RemoteIO_SendCommand(unsigned short command, char* data, int datalength);

void RemoteIO_Lock();
void RemoteIO_UnLock();

void RemoteIO_SendKeyPress(int key);
void RemoteIO_SendKeyUp(int key);
void RemoteIO_SendKeyDown(int key);
void RemoteIO_SendMouseUp(int X,int Y,int Button);
void RemoteIO_SendMouseDown(int X,int Y,int Button);
void RemoteIO_SendMouseMove(int X,int Y);

// Remote IO Command Numbers
enum RemoteIOCommandNumber
{
	// Device setup
	RIO_RESET		  =     1,
	RIO_REQUEST		  =	    2,
	RIO_QUERY_INFO	  =	    3,
	RIO_DEVICE_INFO	  =     4,
	RIO_EXIT		  =     5,
	RIO_JUMBO         =     6,

	// Display remoting
	RIO_FLUSH		  =  1002,
	RIO_REPAINT		  =  1003,
	RIO_DRAWFILLBOX	  =  1004,
	RIO_DRAWIMAGE	  =  1005,
	RIO_MOVEIMAGE	  =  1006,
	RIO_ALLOCATE	  =  1007,

	// Input remoting
	RIO_KEY_DOWN	  =  2001,
	RIO_KEY_UP		  =  2002,
	RIO_KEY_PRESS	  =  2003,
	RIO_MOUSE_MOVE	  =  2004,
	RIO_MOUSE_DOWN	  =  2005,
	RIO_MOUSE_UP	  =  2006,
	RIO_CUSTOM_INPUT  =  3000,

	RIO_SETOBJECT	  =  4001,
	RIO_COPYOBJECT    =  4002,
	RIO_CLEAROBJECT	  =  4003,
	RIO_SETVECTOR     =  4004,
	RIO_CLEARVECTOR   =  4005,

	// Custom messages
	RIO_CUSTOM_DATA	  = 10000,

	RIO_XWPC_BIGIMAGE = 30001,
	RIO_XWPC_PING     = 30010,
	RIO_XWPC_PONG     = 30011
};

#pragma pack(1)
struct RIO_COMMAND_DRAWFILLBOX
{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct RIO_COMMAND_DRAWIMAGE
{
	unsigned short x;
	unsigned short y;
};

struct RIO_COMMAND_MOVEIMAGE
{
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
	unsigned short w;
	unsigned short h;
};

struct RIO_COMMAND_ALLOCATE
{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
	unsigned short id;		// Surface Identifier
};

struct RIO_COMMAND_OBJECT
{
	short id;		// Object Identifier
};

struct RIO_COMMAND_COPYOBJECT
{
	short id;		// Object Identifier
	short x;
	short y;
};

struct RIO_COMMAND_SETVECTOR
{
	short id;		// Object Identifier
	short xs;		// X Start
	short ys;		// Y Start
	short xt;		// X Target
	short yt;		// Y Target
	short pc;		// Position XY Count
	short ss;		// Scale Start
	short st;		// Scale Target
	short sc;		// Scale Count
	short flag;		// Flags
};

struct RIO_XWPC_BIGIMAGE
{
	unsigned int   datasize; // Data size of x,y,width,height and the following image.
	unsigned short x;
	unsigned short y;
	unsigned short width;
	unsigned short height;
};

// Remote IO Command Numbers
enum RemoteIOImageFormat
{
	RemoteIO_JPEG = 1,
	RemoteIO_PNG  = 2,
	RemoteIO_BMP  = 3,
	RemoteIO_GIF  = 4,
	RemoteIO_TIFF = 5
};

#pragma pack()
