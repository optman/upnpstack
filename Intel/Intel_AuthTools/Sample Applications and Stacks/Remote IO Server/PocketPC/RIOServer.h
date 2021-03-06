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
* $Workfile: RemoteIOServer.h
* $Author:   Intel Corporation
* $Date:     Monday, March 24, 2003
*
*/

#ifndef __RemoteIOServer__
#define __RemoteIOServer__

extern void (*RemoteIOServerCallback_ClientAdded)     (struct RemoteIODevice* device);
extern void (*RemoteIOServerCallback_ClientRemoved)   (struct RemoteIODevice* device);
extern void (*RemoteIOServerCallback_CheckMatch)      (struct RemoteIODevice* device, int* match);
extern void (*RemoteIOServerCallback_Connection)      (struct RemoteIODevice* device);
extern void (*RemoteIOServerCallback_Disconnection)   (struct RemoteIODevice* device);
extern void (*RemoteIOServerCallback_Command)         (struct RemoteIODevice* device, int command, char* data, int len);
extern void (*RemoteIOServerCallback_CommandUnderFlow)(struct RemoteIODevice* device);

void* RemoteIOServerCreate(void* threadchain, char* ApplicationName, unsigned short ServerPort);

void RemoteIOServerSendCommand(struct RemoteIODevice* device,int command, char* data1, int datalength1, char* data2, int datalength2, int freedata2);
unsigned int GetBufferOverflow(struct RemoteIODevice* device);

void RemoteIOServer_ConnectInterlock(struct RemoteIODevice* device);
void RemoteIOServer_ConnectOverride(struct RemoteIODevice* device);
void RemoteIOServer_Disconnect(struct RemoteIODevice* device);

void RemoteIOServerSend_RESET(struct RemoteIODevice* device);
void RemoteIOServerSend_DRAWFILLBOX(struct RemoteIODevice* device, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b);
void RemoteIOServerSend_DRAWIMAGE(struct RemoteIODevice* device, int x, int y, char* image, int imagesize, int freeimage);
void RemoteIOServerSend_ALLOCATE(struct RemoteIODevice* device, int x, int y, int w, int h, int id);

void RemoteIOServer_Lock();
void RemoteIOServer_UnLock();

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

struct RIO_COMMAND_MOUSEINPUT
{
	int x;
	int y;
	int button;
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

#endif