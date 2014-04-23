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
* $Workfile: UpnpMicroStack.c
* $Revision: #1.0.1189.22189
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Friday, April 04, 2003
*
*/
#ifndef MICROSTACK_NO_STDAFX
#include "stdafx.h"
#endif
#include <windows.h>
#include <math.h>
#include <winioctl.h>
#include <winbase.h>
#include <winerror.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <winsock.h>
#include <wininet.h>
#include "ILibParsers.h"
#include "UpnpMicroStack.h"
#include "ILibHTTPClient.h"

#define sem_t HANDLE
#define sem_init(x,y,z) *x=CreateSemaphore(NULL,z,FD_SETSIZE,NULL)
#define sem_destroy(x) (CloseHandle(*x)==0?1:0)
#define sem_wait(x) WaitForSingleObject(*x,INFINITE)
#define sem_trywait(x) ((WaitForSingleObject(*x,0)==WAIT_OBJECT_0)?0:1)
#define sem_post(x) ReleaseSemaphore(*x,1,NULL)
#define UPNP_PORT 1900
#define UPNP_GROUP "239.255.255.250"
#define UpnpMIN(a,b) (((a)<(b))?(a):(b))

#define LVL3DEBUG(x)

#pragma warning( disable : 4305 )
const int UpnpDeviceDescriptionTemplateLengthUX = 1381;
const int UpnpDeviceDescriptionTemplateLength = 620;
const char UpnpDeviceDescriptionTemplate[620]={
	0x3F,0x48,0x54,0x54,0x50,0x2F,0x31,0x2E,0x30,0x20,0x32,0x30,0x30,0x20,0x20,0x4F,0x4B,0x0D,0x0A,0x43
	,0x4F,0x4E,0x54,0x45,0x4E,0x54,0x2D,0x54,0x59,0x50,0x45,0x3A,0x20,0x20,0x74,0x65,0x78,0x74,0x2F,0x78
	,0x6D,0x6C,0x0D,0x0A,0x53,0x65,0x72,0x76,0x65,0x72,0x3A,0x20,0x50,0x50,0x43,0x32,0x30,0x30,0x32,0x2C
	,0x20,0x55,0x50,0x6E,0x05,0x0F,0x12,0x2C,0x20,0x49,0x6E,0x74,0x65,0x6C,0x20,0x4D,0x69,0x63,0x72,0x6F
	,0x53,0x74,0x61,0x63,0x6B,0x84,0x14,0x63,0x2E,0x31,0x31,0x38,0x39,0x0D,0x0A,0x0D,0x0A,0x3C,0x3F,0x78
	,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x65,0x6E,0x63
	,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x75,0x74,0x66,0x2D,0x38,0x22,0x3F,0x3E,0x3C,0x72,0x6F,0x6F,0x74
	,0x20,0x78,0x6D,0x6C,0x6E,0x73,0x3D,0x22,0x75,0x72,0x6E,0x3A,0x73,0x63,0x68,0x65,0x6D,0x61,0x73,0x2D
	,0x75,0x70,0x6E,0x70,0x2D,0x6F,0x72,0x67,0x3A,0x64,0x65,0x76,0x69,0x63,0x65,0x2D,0x31,0x2D,0x30,0x22
	,0x3E,0x3C,0x73,0x70,0x65,0x63,0x56,0xC6,0x14,0x0B,0x3E,0x3C,0x6D,0x61,0x6A,0x6F,0x72,0x3E,0x31,0x3C
	,0x2F,0x46,0x02,0x0A,0x3C,0x6D,0x69,0x6E,0x6F,0x72,0x3E,0x30,0x3C,0x2F,0x46,0x02,0x02,0x3C,0x2F,0x8D
	,0x0B,0x00,0x06,0x12,0x00,0x08,0x02,0x05,0x54,0x79,0x70,0x65,0x3E,0x1B,0x1C,0x0D,0x3A,0x52,0x65,0x6D
	,0x6F,0x74,0x65,0x49,0x4F,0x3A,0x31,0x3C,0x2F,0xCB,0x0C,0x12,0x3C,0x66,0x72,0x69,0x65,0x6E,0x64,0x6C
	,0x79,0x4E,0x61,0x6D,0x65,0x3E,0x25,0x73,0x3C,0x2F,0x4D,0x04,0x0E,0x3C,0x6D,0x61,0x6E,0x75,0x66,0x61
	,0x63,0x74,0x75,0x72,0x65,0x72,0x3E,0x06,0x48,0x0D,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,0x74,0x69,0x6F
	,0x6E,0x3C,0x2F,0x0D,0x08,0x00,0x8D,0x0B,0x10,0x55,0x52,0x4C,0x3E,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F
	,0x77,0x77,0x77,0x2E,0x69,0x04,0x57,0x06,0x2E,0x63,0x6F,0x6D,0x3C,0x2F,0x90,0x09,0x0D,0x3C,0x6D,0x6F
	,0x64,0x65,0x6C,0x44,0x65,0x73,0x63,0x72,0x69,0x70,0xC4,0x15,0x02,0x20,0x2F,0x07,0x05,0x00,0x44,0x25
	,0x00,0x4A,0x03,0x05,0x75,0x6D,0x62,0x65,0x72,0x04,0x07,0x05,0x73,0x65,0x72,0x69,0x61,0x07,0x04,0x00
	,0x05,0x2D,0x00,0x4D,0x04,0x0A,0x3C,0x55,0x44,0x4E,0x3E,0x75,0x75,0x69,0x64,0x3A,0xC4,0x33,0x03,0x55
	,0x44,0x4E,0x45,0x0C,0x00,0x44,0x5D,0x04,0x4C,0x69,0x73,0x74,0x49,0x03,0x00,0x89,0x05,0x00,0xDA,0x4E
	,0x03,0x73,0x65,0x72,0x11,0x4F,0x00,0x4C,0x0D,0x00,0x08,0x16,0x02,0x49,0x64,0xC5,0x5E,0x00,0x10,0x0E
	,0x08,0x49,0x64,0x3A,0x52,0x49,0x4F,0x3C,0x2F,0x8A,0x09,0x05,0x3C,0x53,0x43,0x50,0x44,0x44,0x46,0x00
	,0x88,0x63,0x0B,0x2F,0x73,0x63,0x70,0x64,0x2E,0x78,0x6D,0x6C,0x3C,0x2F,0xC8,0x06,0x08,0x3C,0x63,0x6F
	,0x6E,0x74,0x72,0x6F,0x6C,0xCD,0x09,0x00,0x07,0x05,0x02,0x3C,0x2F,0x4B,0x07,0x09,0x3C,0x65,0x76,0x65
	,0x6E,0x74,0x53,0x75,0x62,0x4D,0x14,0x00,0x45,0x05,0x02,0x3C,0x2F,0x0C,0x07,0x02,0x3C,0x2F,0xD0,0x3C
	,0x00,0x32,0x3F,0x04,0x6E,0x70,0x75,0x74,0xF4,0x3F,0x00,0x04,0x0E,0x00,0x9C,0x40,0x00,0x04,0x16,0x00
	,0x66,0x41,0x00,0x85,0x0A,0x00,0x28,0x42,0x00,0xC5,0x15,0x00,0xFF,0x42,0x00,0xD1,0x81,0x0E,0x43,0x68
	,0x61,0x6E,0x6E,0x65,0x6C,0x4D,0x61,0x6E,0x61,0x67,0x65,0x72,0x72,0x83,0x04,0x43,0x4D,0x47,0x52,0x95
	,0x83,0x00,0x4E,0x16,0x00,0x1F,0x85,0x00,0x4F,0x0B,0x00,0xA1,0x86,0x00,0x4F,0x17,0x00,0x1E,0x88,0x01
	,0x2F,0x8D,0xCA,0x03,0x2F,0x64,0x65,0xC7,0x05,0x02,0x72,0x6F,0x00,0x00,0x03,0x6F,0x74,0x3E,0x00,0x00
};
/* RemoteIO */
const int UpnpRemoteIODescriptionLengthUX = 2746;
const int UpnpRemoteIODescriptionLength = 748;
const char UpnpRemoteIODescription[748] = {
	0x3F,0x48,0x54,0x54,0x50,0x2F,0x31,0x2E,0x30,0x20,0x32,0x30,0x30,0x20,0x20,0x4F,0x4B,0x0D,0x0A,0x43
	,0x4F,0x4E,0x54,0x45,0x4E,0x54,0x2D,0x54,0x59,0x50,0x45,0x3A,0x20,0x20,0x74,0x65,0x78,0x74,0x2F,0x78
	,0x6D,0x6C,0x0D,0x0A,0x53,0x65,0x72,0x76,0x65,0x72,0x3A,0x20,0x50,0x50,0x43,0x32,0x30,0x30,0x32,0x2C
	,0x20,0x55,0x50,0x6E,0x05,0x0F,0x12,0x2C,0x20,0x49,0x6E,0x74,0x65,0x6C,0x20,0x4D,0x69,0x63,0x72,0x6F
	,0x53,0x74,0x61,0x63,0x6B,0x84,0x14,0x64,0x2E,0x31,0x31,0x38,0x39,0x0D,0x0A,0x0D,0x0A,0x3C,0x3F,0x78
	,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x65,0x6E,0x63
	,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x75,0x74,0x66,0x2D,0x38,0x22,0x3F,0x3E,0x3C,0x73,0x63,0x70,0x64
	,0x20,0x78,0x6D,0x6C,0x6E,0x73,0x3D,0x22,0x75,0x72,0x6E,0x3A,0x73,0x63,0x68,0x65,0x6D,0x61,0x73,0x2D
	,0x75,0x70,0x6E,0x70,0x2D,0x6F,0x72,0x67,0x3A,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x2D,0x31,0x2D,0x30
	,0x22,0x3E,0x3C,0x73,0x70,0x65,0x63,0x56,0x06,0x15,0x0B,0x3E,0x3C,0x6D,0x61,0x6A,0x6F,0x72,0x3E,0x31
	,0x3C,0x2F,0x46,0x02,0x0A,0x3C,0x6D,0x69,0x6E,0x6F,0x72,0x3E,0x30,0x3C,0x2F,0x46,0x02,0x02,0x3C,0x2F
	,0x8D,0x0B,0x0A,0x61,0x63,0x74,0x69,0x6F,0x6E,0x4C,0x69,0x73,0x74,0x08,0x03,0x14,0x3E,0x3C,0x6E,0x61
	,0x6D,0x65,0x3E,0x46,0x6F,0x72,0x63,0x65,0x44,0x69,0x73,0x63,0x6F,0x6E,0x6E,0x65,0x45,0x09,0x02,0x3C
	,0x2F,0x45,0x06,0x05,0x3C,0x2F,0x61,0x63,0x74,0x0B,0x0F,0x00,0x07,0x0C,0x08,0x47,0x65,0x74,0x50,0x65
	,0x65,0x72,0x43,0xD1,0x0B,0x08,0x61,0x72,0x67,0x75,0x6D,0x65,0x6E,0x74,0x07,0x19,0x00,0x87,0x03,0x00
	,0x87,0x19,0x00,0xD6,0x0C,0x03,0x64,0x69,0x72,0xC6,0x1C,0x06,0x3E,0x6F,0x75,0x74,0x3C,0x2F,0xCA,0x03
	,0x14,0x3C,0x72,0x65,0x6C,0x61,0x74,0x65,0x64,0x53,0x74,0x61,0x74,0x65,0x56,0x61,0x72,0x69,0x61,0x62
	,0x6C,0x52,0x11,0x00,0x55,0x09,0x02,0x3C,0x2F,0xCA,0x1E,0x01,0x2F,0x0E,0x25,0x00,0x56,0x34,0x00,0x45
	,0x40,0x05,0x52,0x65,0x73,0x65,0x74,0x5E,0x3E,0x01,0x53,0x46,0x3E,0x00,0xC4,0x7D,0x05,0x72,0x6C,0x6F
	,0x63,0x6B,0x3F,0x3E,0x00,0x06,0x61,0x02,0x69,0x6E,0xFF,0x3D,0x00,0xD4,0x3D,0x00,0x0F,0x5F,0x06,0x41
	,0x63,0x74,0x69,0x76,0x65,0xBF,0x60,0x00,0xBF,0x60,0x00,0x9D,0x60,0x05,0x47,0x65,0x74,0x44,0x65,0xC4
	,0xB9,0x07,0x49,0x6E,0x66,0x6F,0x72,0x6D,0x61,0xA9,0x95,0x07,0x41,0x70,0x70,0x6C,0x69,0x63,0x61,0xFB
	,0x94,0x00,0x8B,0x10,0x0A,0x49,0x64,0x65,0x6E,0x74,0x69,0x66,0x69,0x65,0x72,0xF2,0x58,0x0E,0x4D,0x61
	,0x78,0x43,0x6F,0x6D,0x6D,0x61,0x6E,0x64,0x53,0x69,0x7A,0x65,0xF7,0xB7,0x00,0x50,0x11,0x00,0x30,0x7A
	,0x0F,0x44,0x69,0x73,0x70,0x6C,0x61,0x79,0x45,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x77,0xD9,0x00,0x91
	,0x11,0x00,0xF7,0x21,0x05,0x57,0x69,0x64,0x74,0x68,0x3E,0x21,0x00,0xFD,0x41,0x06,0x48,0x65,0x69,0x67
	,0x68,0x74,0x7F,0x20,0x00,0x36,0x62,0x00,0x18,0xB3,0x00,0xB0,0x62,0x00,0x12,0xC5,0x00,0x3F,0xDC,0x00
	,0x07,0xDC,0x03,0x53,0x65,0x74,0x44,0xF2,0x08,0x4F,0x76,0x65,0x72,0x72,0x69,0x64,0x65,0xE5,0xDA,0x00
	,0x90,0xFE,0x00,0x90,0xDB,0x02,0x69,0x6E,0x62,0xDB,0x00,0x10,0x11,0x00,0xB9,0x33,0x00,0xC7,0x35,0x00
	,0x06,0x39,0x03,0x73,0x65,0x72,0x04,0x5C,0x00,0x05,0xF6,0x01,0x54,0x46,0xEA,0x01,0x73,0x0C,0xF9,0x10
	,0x20,0x73,0x65,0x6E,0x64,0x45,0x76,0x65,0x6E,0x74,0x73,0x3D,0x22,0x6E,0x6F,0x22,0xA1,0x69,0x0B,0x61
	,0x74,0x61,0x54,0x79,0x70,0x65,0x3E,0x73,0x74,0x72,0xC5,0xD1,0x00,0x49,0x04,0x03,0x3C,0x2F,0x73,0x0E
	,0xE3,0x00,0x24,0x1A,0x00,0x90,0xF5,0x00,0x4F,0x19,0x03,0x75,0x69,0x34,0xBF,0x18,0x07,0x3E,0x41,0x70
	,0x70,0x6C,0x69,0x63,0x85,0x9A,0x0A,0x49,0x64,0x65,0x6E,0x74,0x69,0x66,0x69,0x65,0x72,0xBF,0x33,0x00
	,0x8E,0x4D,0x04,0x79,0x65,0x73,0x22,0x1E,0x82,0x00,0x3F,0x4D,0x00,0x0F,0x67,0x00,0x4A,0x9A,0x04,0x50
	,0x6F,0x72,0x74,0x13,0x4D,0x01,0x32,0xBF,0x65,0x00,0x48,0xF8,0x08,0x45,0x6E,0x63,0x6F,0x64,0x69,0x6E
	,0x67,0x11,0x7F,0x00,0xBF,0x65,0x00,0x8A,0x18,0x00,0x74,0x7D,0x01,0x2F,0xD3,0xB4,0x03,0x2F,0x73,0x63
	,0x00,0x00,0x03,0x70,0x64,0x3E,0x00,0x00};
/* RemoteInput */
const int UpnpRemoteInputDescriptionLengthUX = 2743;
const int UpnpRemoteInputDescriptionLength = 681;
const char UpnpRemoteInputDescription[681] = {
	0x3F,0x48,0x54,0x54,0x50,0x2F,0x31,0x2E,0x30,0x20,0x32,0x30,0x30,0x20,0x20,0x4F,0x4B,0x0D,0x0A,0x43
	,0x4F,0x4E,0x54,0x45,0x4E,0x54,0x2D,0x54,0x59,0x50,0x45,0x3A,0x20,0x20,0x74,0x65,0x78,0x74,0x2F,0x78
	,0x6D,0x6C,0x0D,0x0A,0x53,0x65,0x72,0x76,0x65,0x72,0x3A,0x20,0x50,0x50,0x43,0x32,0x30,0x30,0x32,0x2C
	,0x20,0x55,0x50,0x6E,0x05,0x0F,0x12,0x2C,0x20,0x49,0x6E,0x74,0x65,0x6C,0x20,0x4D,0x69,0x63,0x72,0x6F
	,0x53,0x74,0x61,0x63,0x6B,0x84,0x14,0x64,0x2E,0x31,0x31,0x38,0x39,0x0D,0x0A,0x0D,0x0A,0x3C,0x3F,0x78
	,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x65,0x6E,0x63
	,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x75,0x74,0x66,0x2D,0x38,0x22,0x3F,0x3E,0x3C,0x73,0x63,0x70,0x64
	,0x20,0x78,0x6D,0x6C,0x6E,0x73,0x3D,0x22,0x75,0x72,0x6E,0x3A,0x73,0x63,0x68,0x65,0x6D,0x61,0x73,0x2D
	,0x75,0x70,0x6E,0x70,0x2D,0x6F,0x72,0x67,0x3A,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x2D,0x31,0x2D,0x30
	,0x22,0x3E,0x3C,0x73,0x70,0x65,0x63,0x56,0x06,0x15,0x0B,0x3E,0x3C,0x6D,0x61,0x6A,0x6F,0x72,0x3E,0x31
	,0x3C,0x2F,0x46,0x02,0x0A,0x3C,0x6D,0x69,0x6E,0x6F,0x72,0x3E,0x30,0x3C,0x2F,0x46,0x02,0x02,0x3C,0x2F
	,0x8D,0x0B,0x0A,0x61,0x63,0x74,0x69,0x6F,0x6E,0x4C,0x69,0x73,0x74,0x08,0x03,0x16,0x3E,0x3C,0x6E,0x61
	,0x6D,0x65,0x3E,0x49,0x6E,0x70,0x75,0x74,0x4B,0x65,0x79,0x50,0x72,0x65,0x73,0x73,0x3C,0x2F,0x05,0x05
	,0x09,0x3C,0x61,0x72,0x67,0x75,0x6D,0x65,0x6E,0x74,0x07,0x0C,0x00,0x87,0x03,0x00,0x87,0x0C,0x03,0x6B
	,0x65,0x79,0x08,0x0A,0x04,0x64,0x69,0x72,0x65,0x46,0x13,0x04,0x69,0x6E,0x3C,0x2F,0x8A,0x03,0x1F,0x3C
	,0x72,0x65,0x6C,0x61,0x74,0x65,0x64,0x53,0x74,0x61,0x74,0x65,0x56,0x61,0x72,0x69,0x61,0x62,0x6C,0x65
	,0x3E,0x4B,0x65,0x79,0x43,0x6F,0x64,0x65,0x3C,0x2F,0x95,0x07,0x02,0x3C,0x2F,0x0A,0x1A,0x01,0x2F,0x4E
	,0x20,0x04,0x2F,0x61,0x63,0x74,0x8B,0x31,0x00,0x8C,0x2E,0x09,0x4D,0x6F,0x75,0x73,0x65,0x4D,0x6F,0x76
	,0x65,0xE5,0x2E,0x01,0x58,0x76,0x2E,0x00,0x45,0x19,0x04,0x50,0x6F,0x73,0x69,0x04,0x51,0x01,0x58,0x23
	,0x30,0x00,0x8F,0x4C,0x01,0x59,0xFF,0x1D,0x00,0xC4,0x6E,0x01,0x59,0xFF,0x4D,0x00,0x49,0x7C,0x03,0x47
	,0x65,0x74,0x05,0x7D,0x05,0x53,0x65,0x74,0x75,0x70,0x65,0x7C,0x00,0xCA,0x0B,0x0A,0x49,0x64,0x65,0x6E
	,0x74,0x69,0x66,0x69,0x65,0x72,0x92,0x80,0x03,0x6F,0x75,0x74,0xE2,0x80,0x00,0xD6,0x12,0x00,0x3F,0x84
	,0x00,0x11,0x84,0x02,0x55,0x70,0xBF,0x83,0x00,0xBF,0x83,0x00,0xBF,0x83,0x00,0xBF,0x83,0x00,0x57,0xA1
	,0x06,0x42,0x75,0x74,0x74,0x6F,0x6E,0x7B,0xC0,0x00,0x48,0x10,0x00,0xBF,0xEF,0x00,0x8C,0xEF,0x03,0x4B
	,0x65,0x79,0x27,0x6B,0x03,0x6B,0x65,0x79,0x36,0xEF,0x07,0x4B,0x65,0x79,0x43,0x6F,0x64,0x65,0xFF,0x2D
	,0x00,0xD1,0x2D,0x04,0x44,0x6F,0x77,0x6E,0x7F,0x2E,0x00,0x7F,0x2E,0x00,0xB9,0xC7,0x00,0xE9,0x2E,0x00
	,0x3F,0xC8,0x00,0x3F,0xC8,0x00,0x3F,0xC8,0x00,0x3F,0xC8,0x00,0x3F,0xC8,0x00,0x35,0xC8,0x00,0x47,0xCA
	,0x00,0x86,0xCD,0x07,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0xC5,0xF7,0x01,0x54,0x06,0xF7,0x01,0x73,0xCC
	,0xFA,0x10,0x20,0x73,0x65,0x6E,0x64,0x45,0x76,0x65,0x6E,0x74,0x73,0x3D,0x22,0x6E,0x6F,0x22,0x87,0xF9
	,0x00,0x8D,0xEA,0x00,0xC7,0xFA,0x0C,0x61,0x74,0x61,0x54,0x79,0x70,0x65,0x3E,0x69,0x34,0x3C,0x2F,0x49
	,0x03,0x03,0x3C,0x2F,0x73,0x4E,0xF0,0x00,0xA9,0x17,0x00,0xCB,0x76,0x00,0x7F,0x18,0x00,0x5F,0x18,0x01
	,0x59,0xBF,0x30,0x00,0x14,0x48,0x00,0x89,0xD5,0x00,0x3F,0x47,0x00,0x92,0x5E,0x00,0x45,0xD8,0x0F,0x53
	,0x65,0x74,0x75,0x70,0x49,0x64,0x65,0x6E,0x74,0x69,0x66,0x69,0x65,0x72,0xD1,0x60,0x06,0x73,0x74,0x72
	,0x69,0x6E,0x67,0xDC,0x61,0x01,0x2F,0x53,0x7E,0x03,0x2F,0x73,0x63,0x00,0x00,0x03,0x70,0x64,0x3E,0x00
	,0x00};
/* ChannelManager */
const int UpnpChannelManagerDescriptionLengthUX = 1664;
const int UpnpChannelManagerDescriptionLength = 581;
const char UpnpChannelManagerDescription[581] = {
	0x3F,0x48,0x54,0x54,0x50,0x2F,0x31,0x2E,0x30,0x20,0x32,0x30,0x30,0x20,0x20,0x4F,0x4B,0x0D,0x0A,0x43
	,0x4F,0x4E,0x54,0x45,0x4E,0x54,0x2D,0x54,0x59,0x50,0x45,0x3A,0x20,0x20,0x74,0x65,0x78,0x74,0x2F,0x78
	,0x6D,0x6C,0x0D,0x0A,0x53,0x65,0x72,0x76,0x65,0x72,0x3A,0x20,0x50,0x50,0x43,0x32,0x30,0x30,0x32,0x2C
	,0x20,0x55,0x50,0x6E,0x05,0x0F,0x12,0x2C,0x20,0x49,0x6E,0x74,0x65,0x6C,0x20,0x4D,0x69,0x63,0x72,0x6F
	,0x53,0x74,0x61,0x63,0x6B,0x84,0x14,0x64,0x2E,0x31,0x31,0x38,0x39,0x0D,0x0A,0x0D,0x0A,0x3C,0x3F,0x78
	,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x65,0x6E,0x63
	,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x75,0x74,0x66,0x2D,0x38,0x22,0x3F,0x3E,0x3C,0x73,0x63,0x70,0x64
	,0x20,0x78,0x6D,0x6C,0x6E,0x73,0x3D,0x22,0x75,0x72,0x6E,0x3A,0x73,0x63,0x68,0x65,0x6D,0x61,0x73,0x2D
	,0x75,0x70,0x6E,0x70,0x2D,0x6F,0x72,0x67,0x3A,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x2D,0x31,0x2D,0x30
	,0x22,0x3E,0x3C,0x73,0x70,0x65,0x63,0x56,0x06,0x15,0x0B,0x3E,0x3C,0x6D,0x61,0x6A,0x6F,0x72,0x3E,0x31
	,0x3C,0x2F,0x46,0x02,0x0A,0x3C,0x6D,0x69,0x6E,0x6F,0x72,0x3E,0x30,0x3C,0x2F,0x46,0x02,0x02,0x3C,0x2F
	,0x8D,0x0B,0x0A,0x61,0x63,0x74,0x69,0x6F,0x6E,0x4C,0x69,0x73,0x74,0x08,0x03,0x18,0x3E,0x3C,0x6E,0x61
	,0x6D,0x65,0x3E,0x52,0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x43,0x68,0x61,0x6E,0x6E,0x65,0x6C,0x3C,0x2F
	,0x85,0x05,0x09,0x3C,0x61,0x72,0x67,0x75,0x6D,0x65,0x6E,0x74,0x87,0x0C,0x00,0x87,0x03,0x00,0x07,0x0D
	,0x04,0x4E,0x61,0x6D,0x65,0x48,0x0A,0x04,0x64,0x69,0x72,0x65,0x06,0x14,0x04,0x69,0x6E,0x3C,0x2F,0x8A
	,0x03,0x16,0x3C,0x72,0x65,0x6C,0x61,0x74,0x65,0x64,0x53,0x74,0x61,0x74,0x65,0x56,0x61,0x72,0x69,0x61
	,0x62,0x6C,0x65,0x3E,0x87,0x19,0x00,0x46,0x10,0x00,0x95,0x08,0x02,0x3C,0x2F,0x4A,0x1B,0x00,0xCF,0x1D
	,0x08,0x50,0x65,0x65,0x72,0x43,0x6F,0x6E,0x6E,0x06,0x1C,0x00,0x7D,0x20,0x00,0xD0,0x12,0x00,0xF0,0x22
	,0x07,0x54,0x69,0x6D,0x65,0x6F,0x75,0x74,0x7D,0x41,0x00,0x09,0x11,0x00,0x21,0x42,0x01,0x2F,0x8E,0x63
	,0x04,0x2F,0x61,0x63,0x74,0x4B,0x75,0x00,0x47,0x72,0x03,0x55,0x6E,0x72,0xF3,0x72,0x00,0x3F,0x55,0x00
	,0x3D,0x55,0x00,0xE5,0x35,0x08,0x43,0x6C,0x65,0x61,0x72,0x41,0x6C,0x6C,0x07,0xA8,0x01,0x73,0x48,0xA8
	,0x00,0x56,0x41,0x03,0x47,0x65,0x74,0x48,0xB4,0x02,0x65,0x64,0xC7,0xB4,0x03,0x4C,0x69,0x73,0x49,0x6A
	,0x00,0xDD,0xB5,0x00,0x13,0x0C,0x00,0x8A,0xB7,0x03,0x6F,0x75,0x74,0xE2,0xB7,0x00,0x17,0x1F,0x00,0x79
	,0x78,0x01,0x2F,0xCC,0xED,0x07,0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x45,0xD4,0x01,0x54,0x06,0xCB,0x01
	,0x73,0x4C,0xD7,0x10,0x20,0x73,0x65,0x6E,0x64,0x45,0x76,0x65,0x6E,0x74,0x73,0x3D,0x22,0x6E,0x6F,0x22
	,0x8E,0x35,0x00,0x0D,0xED,0x10,0x61,0x74,0x61,0x54,0x79,0x70,0x65,0x3E,0x73,0x74,0x72,0x69,0x6E,0x67
	,0x3C,0x2F,0x49,0x04,0x03,0x3C,0x2F,0x73,0x8E,0xE3,0x00,0x9A,0x18,0x03,0x79,0x65,0x73,0xC8,0x18,0x00
	,0xDD,0x5C,0x00,0x7F,0x1B,0x00,0xD7,0x33,0x00,0x50,0xE0,0x00,0x88,0x34,0x02,0x69,0x34,0x7F,0x18,0x00
	,0x98,0xB3,0x00,0xB1,0x4E,0x01,0x2F,0x13,0x6C,0x03,0x2F,0x73,0x63,0x00,0x00,0x03,0x70,0x64,0x3E,0x00
	,0x00};
#pragma warning( default : 4305 )

void perror(char* msg) {}

struct UpnpDataObject;

struct HTTPReaderObject
{
	char Header[4000];
	char* Body;
	struct packetheader *ParsedHeader;
	int BodySize;
	int HeaderIndex;
	int BodyIndex;
	SOCKET ClientSocket;
	int FinRead;
	struct UpnpDataObject *Parent;
};
struct SubscriberInfo
{
	char* SID;
	int SIDLength;
	int SEQ;
	
	int Address;
	unsigned short Port;
	char* Path;
	int PathLength;
	int RefCount;
	int Disposing;
	
	unsigned int RenewByTime;
	struct SubscriberInfo *Next;
	struct SubscriberInfo *Previous;
};
struct UpnpDataObject
{
	void (*PreSelect)(void* object,fd_set *readset, fd_set *writeset, fd_set *errorset, int* blocktime);
	void (*PostSelect)(void* object,int slct, fd_set *readset, fd_set *writeset, fd_set *errorset);
	void (*Destroy)(void* object);
	
	void *EventClient;
	void *Chain;
	int UpdateFlag;
	
	/* Network Poll */
	unsigned int NetworkPollTime;
	
	int ForceExit;
	char *UUID;
	char *UDN;
	char *Serial;
	
	void *WebServerTimer;
	
	char *DeviceDescription;
	int DeviceDescriptionLength;
	int InitialNotify;
	char* ChannelManager_RegisteredChannelList;
	char* RemoteIO_PeerConnection;
	struct sockaddr_in addr;
	int addrlen;
	SOCKET MSEARCH_sock;
	struct ip_mreq mreq;
	char message[4096];
	int *AddressList;
	int AddressListLength;
	
	int _NumEmbeddedDevices;
	SOCKET WebSocket;
	int WebSocketPortNumber;
	struct HTTPReaderObject ReaderObjects[5];
	SOCKET *NOTIFY_SEND_socks;
	SOCKET NOTIFY_RECEIVE_sock;
	
	int SID;
	
	unsigned int CurrentTime;
	int NotifyCycleTime;
	unsigned int NotifyTime;
	
	sem_t EventLock;
	struct SubscriberInfo *HeadSubscriberPtr_ChannelManager;
	int NumberOfSubscribers_ChannelManager;
	struct SubscriberInfo *HeadSubscriberPtr_RemoteInput;
	int NumberOfSubscribers_RemoteInput;
	struct SubscriberInfo *HeadSubscriberPtr_RemoteIO;
	int NumberOfSubscribers_RemoteIO;
};

struct MSEARCH_state
{
	char *ST;
	int STLength;
	void *upnp;
	struct sockaddr_in dest_addr;
};

/* Pre-declarations */
void UpnpSendNotify(const struct UpnpDataObject *upnp);
void UpnpSendByeBye();
void UpnpMainInvokeSwitch();
void UpnpSendDataXmlEscaped(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate);
void UpnpSendData(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate);
int UpnpPeriodicNotify(struct UpnpDataObject *upnp);
void UpnpSendEvent_Body(void *upnptoken, char *body, int bodylength, struct SubscriberInfo *info);

char* UpnpDecompressString(unsigned char* CurrentCompressed, const int bufferLength, const int DecompressedLength)
{
	unsigned char *RetVal = (char*)MALLOC(DecompressedLength+1);
	unsigned char *CurrentUnCompressed = RetVal;
	unsigned char *EndPtr = RetVal + DecompressedLength;
	int offset,length;
	
	do
	{
		/* UnCompressed Data Block */
		memcpy(CurrentUnCompressed,CurrentCompressed+1,(int)*CurrentCompressed);
		CurrentUnCompressed += (int)*CurrentCompressed;
		CurrentCompressed += 1+((int)*CurrentCompressed);
		
		/* CompressedBlock */
		length = (unsigned short)((*(CurrentCompressed)) & 63);
		offset = ((unsigned short)(*(CurrentCompressed+1))<<2) + (((unsigned short)(*(CurrentCompressed))) >> 6);
		memcpy(CurrentUnCompressed,CurrentUnCompressed-offset,length);
		CurrentCompressed += 2;
		CurrentUnCompressed += length;
	} while(CurrentUnCompressed < EndPtr);
	RetVal[DecompressedLength] = 0;
	return(RetVal);
}
void* UpnpGetInstance(const void* UPnPToken)
{
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	return (void*)(ReaderObject->Parent);
}

#define UpnpBuildSsdpResponsePacket(outpacket,outlenght,ipaddr,port,EmbeddedDeviceNumber,USN,USNex,ST,NTex,NotifyTime)\
{\
	*outlenght = sprintf(outpacket,"HTTP/1.1 200 OK\r\nLOCATION: http://%d.%d.%d.%d:%d/\r\nEXT:\r\nSERVER: PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189\r\nUSN: uuid:%s%s\r\nCACHE-CONTROL: max-age=%d\r\nST: %s%s\r\n\r\n" ,(ipaddr&0xFF),((ipaddr>>8)&0xFF),((ipaddr>>16)&0xFF),((ipaddr>>24)&0xFF),port,USN,USNex,NotifyTime,ST,NTex);\
}

#define UpnpBuildSsdpNotifyPacket(outpacket,outlenght,ipaddr,port,EmbeddedDeviceNumber,USN,USNex,NT,NTex,NotifyTime)\
{\
	*outlenght = sprintf(outpacket,"NOTIFY * HTTP/1.1\r\nLOCATION: http://%d.%d.%d.%d:%d/\r\nHOST: 239.255.255.250:1900\r\nSERVER: PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189\r\nNTS: ssdp:alive\r\nUSN: uuid:%s%s\r\nCACHE-CONTROL: max-age=%d\r\nNT: %s%s\r\n\r\n",(ipaddr&0xFF),((ipaddr>>8)&0xFF),((ipaddr>>16)&0xFF),((ipaddr>>24)&0xFF),port,USN,USNex,NotifyTime,NT,NTex);\
}

void UpnpIPAddressListChanged(void *MicroStackToken)
{
	((struct UpnpDataObject*)MicroStackToken)->UpdateFlag = 1;
	ILibForceUnBlockChain(((struct UpnpDataObject*)MicroStackToken)->Chain);
}
void UpnpInit(struct UpnpDataObject *state,const int NotifyCycleSeconds,const unsigned short PortNumber)
{
	int ra = 1;
	int i,flags;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	unsigned char TTL = 4;
	
	/* Complete State Reset */
	memset(state,0,sizeof(struct UpnpDataObject));
	
	/* Setup Notification Timer */
	state->NotifyCycleTime = NotifyCycleSeconds;
	state->CurrentTime = GetTickCount() / 1000;
	state->NotifyTime = state->CurrentTime  + (state->NotifyCycleTime/2);
	
	/* Initialize Client Sockets */
	for(i=0;i<5;++i)
	{
		memset(&(state->ReaderObjects[i]),0,sizeof(state->ReaderObjects[i]));
	}
	/* Setup WebSocket */
	if(PortNumber!=0)
	{
		memset((char *)&(addr), 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = (unsigned short)htons(PortNumber);
		state->WebSocket = socket(AF_INET, SOCK_STREAM, 0);
		flags = 1;
		ioctlsocket(state->WebSocket,FIONBIO,&flags);
		if (setsockopt(state->WebSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&ra, sizeof(ra)) < 0)
		{
			printf("Setting SockOpt SO_REUSEADDR failed (HTTP)");
			exit(1);
		}
		if (bind(state->WebSocket, (struct sockaddr *) &(addr), sizeof(addr)) < 0)
		{
			printf("WebSocket bind");
			exit(1);
		}
		state->WebSocketPortNumber = PortNumber;
	}
	else
	{
		state->WebSocketPortNumber = ILibGetStreamSocket(htonl(INADDR_ANY),&(state->WebSocket));
		flags = 1;
		ioctlsocket(state->WebSocket,FIONBIO,&flags);
	}
	if (listen(state->WebSocket,5)!=0)
	{
		printf("WebSocket listen");
		exit(1);
	}
	memset((char *)&(state->addr), 0, sizeof(state->addr));
	state->addr.sin_family = AF_INET;
	state->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	state->addr.sin_port = (unsigned short)htons(UPNP_PORT);
	state->addrlen = sizeof(state->addr);
	/* Set up socket */
	state->AddressListLength = ILibGetLocalIPAddressList(&(state->AddressList));
	state->NOTIFY_SEND_socks = (SOCKET*)MALLOC(sizeof(int)*(state->AddressListLength));
	state->NOTIFY_RECEIVE_sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset((char *)&(addr), 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = (unsigned short)htons(UPNP_PORT);
	if (setsockopt(state->NOTIFY_RECEIVE_sock, SOL_SOCKET, SO_REUSEADDR,(char*)&ra, sizeof(ra)) < 0)
	{
		printf("Setting SockOpt SO_REUSEADDR failed\r\n");
		exit(1);
	}
	if (bind(state->NOTIFY_RECEIVE_sock, (struct sockaddr *) &(addr), sizeof(addr)) < 0)
	{
		printf("Could not bind to UPnP Listen Port\r\n");
		exit(1);
	}
	for(i=0;i<state->AddressListLength;++i)
	{
		state->NOTIFY_SEND_socks[i] = socket(AF_INET, SOCK_DGRAM, 0);
		memset((char *)&(addr), 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = state->AddressList[i];
		addr.sin_port = (unsigned short)htons(UPNP_PORT);
		if (setsockopt(state->NOTIFY_SEND_socks[i], SOL_SOCKET, SO_REUSEADDR,(char*)&ra, sizeof(ra)) == 0)
		{
			if (setsockopt(state->NOTIFY_SEND_socks[i], IPPROTO_IP, IP_MULTICAST_TTL,(char*)&TTL, sizeof(TTL)) < 0)
			{
				/* Ignore this case */
			}
			if (bind(state->NOTIFY_SEND_socks[i], (struct sockaddr *) &(addr), sizeof(addr)) == 0)
			{
				mreq.imr_multiaddr.s_addr = inet_addr(UPNP_GROUP);
				mreq.imr_interface.s_addr = state->AddressList[i];
				if (setsockopt(state->NOTIFY_RECEIVE_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq, sizeof(mreq)) < 0)
				{
					/* Does not matter */
				}
			}
		}
	}
}
void UpnpPostMX_Destroy(void *object)
{
	struct MSEARCH_state *mss = (struct MSEARCH_state*)object;
	FREE(mss->ST);
	FREE(mss);
}
void UpnpPostMX_MSEARCH(void *object)
{
	struct MSEARCH_state *mss = (struct MSEARCH_state*)object;
	
	char *b = (char*)MALLOC(sizeof(char)*5000);
	int packetlength;
	struct sockaddr_in response_addr;
	int response_addrlen;
	SOCKET *response_socket;
	int cnt;
	int i;
	struct sockaddr_in dest_addr = mss->dest_addr;
	char *ST = mss->ST;
	int STLength = mss->STLength;
	struct UpnpDataObject *upnp = (struct UpnpDataObject*)mss->upnp;
	
	response_socket = (SOCKET*)MALLOC(upnp->AddressListLength*sizeof(int));
	for(i=0;i<upnp->AddressListLength;++i)
	{
		response_socket[i] = socket(AF_INET, SOCK_DGRAM, 0);
		if (response_socket[i]< 0)
		{
			printf("response socket");
			exit(1);
		}
		memset((char *)&(response_addr), 0, sizeof(response_addr));
		response_addr.sin_family = AF_INET;
		response_addr.sin_addr.s_addr = upnp->AddressList[i];
		response_addr.sin_port = (unsigned short)htons(0);
		response_addrlen = sizeof(response_addr);	
		if (bind(response_socket[i], (struct sockaddr *) &(response_addr), sizeof(response_addr)) < 0)
		{
			/* Ignore if this happens */
		}
	}
	if(STLength==15 && memcmp(ST,"upnp:rootdevice",15)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::upnp:rootdevice","upnp:rootdevice","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	else if(STLength==8 && memcmp(ST,"ssdp:all",8)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::upnp:rootdevice","upnp:rootdevice","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"",upnp->UUID,"",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:device:RemoteIO:1","urn:schemas-upnp-org:device:RemoteIO:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteIO:1","urn:schemas-upnp-org:service:RemoteIO:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteInput:1","urn:schemas-upnp-org:service:RemoteInput:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:ChannelManager:1","urn:schemas-upnp-org:service:ChannelManager:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	if(STLength==(int)strlen(upnp->UUID) && memcmp(ST,upnp->UUID,(int)strlen(upnp->UUID))==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"",upnp->UUID,"",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	if(STLength>=38 && memcmp(ST,"urn:schemas-upnp-org:device:RemoteIO:1",38)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:device:RemoteIO:1","urn:schemas-upnp-org:device:RemoteIO:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	if(STLength>=39 && memcmp(ST,"urn:schemas-upnp-org:service:RemoteIO:1",39)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteIO:1","urn:schemas-upnp-org:service:RemoteIO:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	if(STLength>=42 && memcmp(ST,"urn:schemas-upnp-org:service:RemoteInput:1",42)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteInput:1","urn:schemas-upnp-org:service:RemoteInput:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	if(STLength>=45 && memcmp(ST,"urn:schemas-upnp-org:service:ChannelManager:1",45)==0)
	{
		for(i=0;i<upnp->AddressListLength;++i)
		{
			UpnpBuildSsdpResponsePacket(b,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:ChannelManager:1","urn:schemas-upnp-org:service:ChannelManager:1","",upnp->NotifyCycleTime);
			cnt = sendto(response_socket[i], b, packetlength, 0,
			(struct sockaddr *) &dest_addr, sizeof(dest_addr));
		}
	}
	for(i=0;i<upnp->AddressListLength;++i)
	{
		closesocket(response_socket[i]);
	}
	FREE(response_socket);
	FREE(mss->ST);
	FREE(mss);
	FREE(b);
}
void UpnpProcessMSEARCH(struct UpnpDataObject *upnp, struct packetheader *packet)
{
	char* ST = NULL;
	int STLength = 0;
	struct packetheader_field_node *node;
	int MANOK = 0;
	unsigned long MXVal;
	int MXOK = 0;
	int MX;
	struct MSEARCH_state *mss = NULL;
	
	if(memcmp(packet->DirectiveObj,"*",1)==0)
	{
		if(memcmp(packet->Version,"1.1",3)==0)
		{
			node = packet->FirstField;
			while(node!=NULL)
			{
				if(_strnicmp(node->Field,"ST",2)==0)
				{
					ST = (char*)MALLOC(1+node->FieldDataLength);
					memcpy(ST,node->FieldData,node->FieldDataLength);
					ST[node->FieldDataLength] = 0;
					STLength = node->FieldDataLength;
				}
				else if(_strnicmp(node->Field,"MAN",3)==0 && memcmp(node->FieldData,"\"ssdp:discover\"",15)==0)
				{
					MANOK = 1;
				}
				else if(_strnicmp(node->Field,"MX",2)==0 && ILibGetULong(node->FieldData,node->FieldDataLength,&MXVal)==0)
				{
					MXOK = 1;
					MXVal = MXVal>10?10:MXVal;
				}
				node = node->NextField;
			}
			if(MANOK!=0 && MXOK!=0)
			{
				MX = (int)(0 + ((unsigned short)rand() % MXVal));
				mss = (struct MSEARCH_state*)MALLOC(sizeof(struct MSEARCH_state));
				mss->ST = ST;
				mss->STLength = STLength;
				mss->upnp = upnp;
				memset((char *)&(mss->dest_addr), 0, sizeof(mss->dest_addr));
				mss->dest_addr.sin_family = AF_INET;
				mss->dest_addr.sin_addr = packet->Source->sin_addr;
				mss->dest_addr.sin_port = packet->Source->sin_port;
				
				ILibLifeTime_Add(upnp->WebServerTimer,mss,MX,&UpnpPostMX_MSEARCH,&UpnpPostMX_Destroy);
			}
			else
			{
				FREE(ST);
			}
		}
	}
}
void UpnpDispatch_ChannelManager_RegisterChannel(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_Name = NULL;
	int p_NameLength = 0;
	char* _Name = "";
	int _NameLength;
	char *p_PeerConnection = NULL;
	int p_PeerConnectionLength = 0;
	char* _PeerConnection = "";
	int _PeerConnectionLength;
	char *p_Timeout = NULL;
	int p_TimeoutLength = 0;
	int _Timeout = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==4 && memcmp(VarName,"Name",4) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Name = temp3->LastResult->data;
					p_NameLength = temp3->LastResult->datalength;
					p_Name[p_NameLength] = 0;
				}
				else
				{
					p_Name = temp3->LastResult->data;
					p_NameLength = 0;
					p_Name[p_NameLength] = 0;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==14 && memcmp(VarName,"PeerConnection",14) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = temp3->LastResult->datalength;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				else
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = 0;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				OK |= 2;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==7 && memcmp(VarName,"Timeout",7) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Timeout = temp3->LastResult->data;
					p_TimeoutLength = temp3->LastResult->datalength;
				}
				OK |= 4;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 7)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	_NameLength = ILibInPlaceXmlUnEscape(p_Name);
	_Name = p_Name;
	_PeerConnectionLength = ILibInPlaceXmlUnEscape(p_PeerConnection);
	_PeerConnection = p_PeerConnection;
	OK = ILibGetLong(p_Timeout,p_TimeoutLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Timeout = (int)TempLong;
	UpnpChannelManager_RegisterChannel((void*)ReaderObject,_Name,_PeerConnection,_Timeout);
}

void UpnpDispatch_ChannelManager_UnregisterChannel(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	int OK = 0;
	char *p_PeerConnection = NULL;
	int p_PeerConnectionLength = 0;
	char* _PeerConnection = "";
	int _PeerConnectionLength;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==14 && memcmp(VarName,"PeerConnection",14) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = temp3->LastResult->datalength;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				else
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = 0;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	_PeerConnectionLength = ILibInPlaceXmlUnEscape(p_PeerConnection);
	_PeerConnection = p_PeerConnection;
	UpnpChannelManager_UnregisterChannel((void*)ReaderObject,_PeerConnection);
}

#define UpnpDispatch_ChannelManager_ClearAllChannels(xml, ReaderObject)\
{\
	UpnpChannelManager_ClearAllChannels((void*)ReaderObject);\
}

#define UpnpDispatch_ChannelManager_GetRegisteredChannelList(xml, ReaderObject)\
{\
	UpnpChannelManager_GetRegisteredChannelList((void*)ReaderObject);\
}

void UpnpDispatch_RemoteInput_InputKeyPress(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_key = NULL;
	int p_keyLength = 0;
	int _key = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==3 && memcmp(VarName,"key",3) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_key = temp3->LastResult->data;
					p_keyLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_key,p_keyLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_key = (int)TempLong;
	UpnpRemoteInput_InputKeyPress((void*)ReaderObject,_key);
}

void UpnpDispatch_RemoteInput_InputMouseMove(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_X = NULL;
	int p_XLength = 0;
	int _X = 0;
	char *p_Y = NULL;
	int p_YLength = 0;
	int _Y = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==1 && memcmp(VarName,"X",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_X = temp3->LastResult->data;
					p_XLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==1 && memcmp(VarName,"Y",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Y = temp3->LastResult->data;
					p_YLength = temp3->LastResult->datalength;
				}
				OK |= 2;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 3)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_X,p_XLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_X = (int)TempLong;
	OK = ILibGetLong(p_Y,p_YLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Y = (int)TempLong;
	UpnpRemoteInput_InputMouseMove((void*)ReaderObject,_X,_Y);
}

#define UpnpDispatch_RemoteInput_GetInputSetup(xml, ReaderObject)\
{\
	UpnpRemoteInput_GetInputSetup((void*)ReaderObject);\
}

void UpnpDispatch_RemoteInput_InputMouseUp(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_X = NULL;
	int p_XLength = 0;
	int _X = 0;
	char *p_Y = NULL;
	int p_YLength = 0;
	int _Y = 0;
	char *p_Button = NULL;
	int p_ButtonLength = 0;
	int _Button = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==1 && memcmp(VarName,"X",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_X = temp3->LastResult->data;
					p_XLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==1 && memcmp(VarName,"Y",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Y = temp3->LastResult->data;
					p_YLength = temp3->LastResult->datalength;
				}
				OK |= 2;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==6 && memcmp(VarName,"Button",6) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Button = temp3->LastResult->data;
					p_ButtonLength = temp3->LastResult->datalength;
				}
				OK |= 4;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 7)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_X,p_XLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_X = (int)TempLong;
	OK = ILibGetLong(p_Y,p_YLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Y = (int)TempLong;
	OK = ILibGetLong(p_Button,p_ButtonLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Button = (int)TempLong;
	UpnpRemoteInput_InputMouseUp((void*)ReaderObject,_X,_Y,_Button);
}

void UpnpDispatch_RemoteInput_InputKeyUp(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_key = NULL;
	int p_keyLength = 0;
	int _key = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==3 && memcmp(VarName,"key",3) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_key = temp3->LastResult->data;
					p_keyLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_key,p_keyLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_key = (int)TempLong;
	UpnpRemoteInput_InputKeyUp((void*)ReaderObject,_key);
}

void UpnpDispatch_RemoteInput_InputKeyDown(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_key = NULL;
	int p_keyLength = 0;
	int _key = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==3 && memcmp(VarName,"key",3) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_key = temp3->LastResult->data;
					p_keyLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_key,p_keyLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_key = (int)TempLong;
	UpnpRemoteInput_InputKeyDown((void*)ReaderObject,_key);
}

void UpnpDispatch_RemoteInput_InputMouseDown(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	long TempLong;
	int OK = 0;
	char *p_X = NULL;
	int p_XLength = 0;
	int _X = 0;
	char *p_Y = NULL;
	int p_YLength = 0;
	int _Y = 0;
	char *p_Button = NULL;
	int p_ButtonLength = 0;
	int _Button = 0;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==1 && memcmp(VarName,"X",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_X = temp3->LastResult->data;
					p_XLength = temp3->LastResult->datalength;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==1 && memcmp(VarName,"Y",1) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Y = temp3->LastResult->data;
					p_YLength = temp3->LastResult->datalength;
				}
				OK |= 2;
				ILibDestructParserResults(temp3);
			}
			else if(VarNameLength==6 && memcmp(VarName,"Button",6) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_Button = temp3->LastResult->data;
					p_ButtonLength = temp3->LastResult->datalength;
				}
				OK |= 4;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 7)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	OK = ILibGetLong(p_X,p_XLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_X = (int)TempLong;
	OK = ILibGetLong(p_Y,p_YLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Y = (int)TempLong;
	OK = ILibGetLong(p_Button,p_ButtonLength, &TempLong);
	if(OK!=0)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	_Button = (int)TempLong;
	UpnpRemoteInput_InputMouseDown((void*)ReaderObject,_X,_Y,_Button);
}

#define UpnpDispatch_RemoteIO_ForceDisconnection(xml, ReaderObject)\
{\
	UpnpRemoteIO_ForceDisconnection((void*)ReaderObject);\
}

#define UpnpDispatch_RemoteIO_GetPeerConnection(xml, ReaderObject)\
{\
	UpnpRemoteIO_GetPeerConnection((void*)ReaderObject);\
}

#define UpnpDispatch_RemoteIO_ForceReset(xml, ReaderObject)\
{\
	UpnpRemoteIO_ForceReset((void*)ReaderObject);\
}

void UpnpDispatch_RemoteIO_SetPeerInterlock(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	int OK = 0;
	char *p_PeerConnection = NULL;
	int p_PeerConnectionLength = 0;
	char* _PeerConnection = "";
	int _PeerConnectionLength;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==14 && memcmp(VarName,"PeerConnection",14) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = temp3->LastResult->datalength;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				else
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = 0;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	_PeerConnectionLength = ILibInPlaceXmlUnEscape(p_PeerConnection);
	_PeerConnection = p_PeerConnection;
	UpnpRemoteIO_SetPeerInterlock((void*)ReaderObject,_PeerConnection);
}

#define UpnpDispatch_RemoteIO_GetDeviceInformation(xml, ReaderObject)\
{\
	UpnpRemoteIO_GetDeviceInformation((void*)ReaderObject);\
}

void UpnpDispatch_RemoteIO_SetPeerOverride(struct parser_result *xml, struct HTTPReaderObject *ReaderObject)
{
	struct parser_result *temp;
	struct parser_result *temp2;
	struct parser_result *temp3;
	struct parser_result_field *field;
	char *VarName;
	int VarNameLength;
	int i;
	int OK = 0;
	char *p_PeerConnection = NULL;
	int p_PeerConnectionLength = 0;
	char* _PeerConnection = "";
	int _PeerConnectionLength;
	field = xml->FirstResult;
	while(field!=NULL)
	{
		if((memcmp(field->data,"?",1)!=0) && (memcmp(field->data,"/",1)!=0))
		{
			temp = ILibParseString(field->data,0,field->datalength," ",1);
			temp2 = ILibParseString(temp->FirstResult->data,0,temp->FirstResult->datalength,":",1);
			if(temp2->NumResults==1)
			{
				VarName = temp2->FirstResult->data;
				VarNameLength = temp2->FirstResult->datalength;
			}
			else
			{
				temp3 = ILibParseString(temp2->FirstResult->data,0,temp2->FirstResult->datalength,">",1);
				if(temp3->NumResults==1)
				{
					VarName = temp2->FirstResult->NextResult->data;
					VarNameLength = temp2->FirstResult->NextResult->datalength;
				}
				else
				{
					VarName = temp2->FirstResult->data;
					VarNameLength = temp2->FirstResult->datalength;
				}
				ILibDestructParserResults(temp3);
			}
			for(i=0;i<VarNameLength;++i)
			{
				if( i!=0 && ((VarName[i]==' ')||(VarName[i]=='/')||(VarName[i]=='>')) )
				{
					VarNameLength = i;
					break;
				}
			}
			if(VarNameLength==14 && memcmp(VarName,"PeerConnection",14) == 0)
			{
				temp3 = ILibParseString(field->data,0,field->datalength,">",1);
				if(memcmp(temp3->FirstResult->data+temp3->FirstResult->datalength-1,"/",1) != 0)
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = temp3->LastResult->datalength;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				else
				{
					p_PeerConnection = temp3->LastResult->data;
					p_PeerConnectionLength = 0;
					p_PeerConnection[p_PeerConnectionLength] = 0;
				}
				OK |= 1;
				ILibDestructParserResults(temp3);
			}
			ILibDestructParserResults(temp2);
			ILibDestructParserResults(temp);
		}
		field = field->NextResult;
	}
	
	if (OK != 1)
	{
		UpnpResponse_Error(ReaderObject,402,"Illegal value");
		return;
	}
	
	/* Type Checking */
	_PeerConnectionLength = ILibInPlaceXmlUnEscape(p_PeerConnection);
	_PeerConnection = p_PeerConnection;
	UpnpRemoteIO_SetPeerOverride((void*)ReaderObject,_PeerConnection);
}

void UpnpProcessPOST(struct packetheader* header, struct HTTPReaderObject *ReaderObject)
{
	struct packetheader_field_node *f = header->FirstField;
	char* HOST;
	char* SOAPACTION = NULL;
	int SOAPACTIONLength = 0;
	struct parser_result *r;
	struct parser_result *xml;
	
	xml = ILibParseString(header->Body,0,header->BodyLength,"<",1);
	while(f!=NULL)
	{
		if(f->FieldLength==4 && _strnicmp(f->Field,"HOST",4)==0)
		{
			HOST = f->FieldData;
		}
		else if(f->FieldLength==10 && _strnicmp(f->Field,"SOAPACTION",10)==0)
		{
			r = ILibParseString(f->FieldData,0,f->FieldDataLength,"#",1);
			SOAPACTION = r->LastResult->data;
			SOAPACTIONLength = r->LastResult->datalength-1;
			ILibDestructParserResults(r);
		}
		f = f->NextField;
	}
	if(header->DirectiveObjLength==17 && memcmp((header->DirectiveObj)+1,"RemoteIO/control",16)==0)
	{
		if(SOAPACTIONLength==18 && memcmp(SOAPACTION,"ForceDisconnection",18)==0)
		{
			UpnpDispatch_RemoteIO_ForceDisconnection(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==17 && memcmp(SOAPACTION,"GetPeerConnection",17)==0)
		{
			UpnpDispatch_RemoteIO_GetPeerConnection(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==10 && memcmp(SOAPACTION,"ForceReset",10)==0)
		{
			UpnpDispatch_RemoteIO_ForceReset(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==16 && memcmp(SOAPACTION,"SetPeerInterlock",16)==0)
		{
			UpnpDispatch_RemoteIO_SetPeerInterlock(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==20 && memcmp(SOAPACTION,"GetDeviceInformation",20)==0)
		{
			UpnpDispatch_RemoteIO_GetDeviceInformation(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==15 && memcmp(SOAPACTION,"SetPeerOverride",15)==0)
		{
			UpnpDispatch_RemoteIO_SetPeerOverride(xml, ReaderObject);
		}
	}
	else if(header->DirectiveObjLength==20 && memcmp((header->DirectiveObj)+1,"RemoteInput/control",19)==0)
	{
		if(SOAPACTIONLength==13 && memcmp(SOAPACTION,"InputKeyPress",13)==0)
		{
			UpnpDispatch_RemoteInput_InputKeyPress(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==14 && memcmp(SOAPACTION,"InputMouseMove",14)==0)
		{
			UpnpDispatch_RemoteInput_InputMouseMove(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==13 && memcmp(SOAPACTION,"GetInputSetup",13)==0)
		{
			UpnpDispatch_RemoteInput_GetInputSetup(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==12 && memcmp(SOAPACTION,"InputMouseUp",12)==0)
		{
			UpnpDispatch_RemoteInput_InputMouseUp(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==10 && memcmp(SOAPACTION,"InputKeyUp",10)==0)
		{
			UpnpDispatch_RemoteInput_InputKeyUp(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==12 && memcmp(SOAPACTION,"InputKeyDown",12)==0)
		{
			UpnpDispatch_RemoteInput_InputKeyDown(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==14 && memcmp(SOAPACTION,"InputMouseDown",14)==0)
		{
			UpnpDispatch_RemoteInput_InputMouseDown(xml, ReaderObject);
		}
	}
	else if(header->DirectiveObjLength==23 && memcmp((header->DirectiveObj)+1,"ChannelManager/control",22)==0)
	{
		if(SOAPACTIONLength==15 && memcmp(SOAPACTION,"RegisterChannel",15)==0)
		{
			UpnpDispatch_ChannelManager_RegisterChannel(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==17 && memcmp(SOAPACTION,"UnregisterChannel",17)==0)
		{
			UpnpDispatch_ChannelManager_UnregisterChannel(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==16 && memcmp(SOAPACTION,"ClearAllChannels",16)==0)
		{
			UpnpDispatch_ChannelManager_ClearAllChannels(xml, ReaderObject);
		}
		else if(SOAPACTIONLength==24 && memcmp(SOAPACTION,"GetRegisteredChannelList",24)==0)
		{
			UpnpDispatch_ChannelManager_GetRegisteredChannelList(xml, ReaderObject);
		}
	}
	ILibDestructParserResults(xml);
}
struct SubscriberInfo* UpnpRemoveSubscriberInfo(struct SubscriberInfo **Head, int *TotalSubscribers,char* SID, int SIDLength)
{
	struct SubscriberInfo *info = *Head;
	struct SubscriberInfo **ptr = Head;
	while(info!=NULL)
	{
		if(info->SIDLength==SIDLength && memcmp(info->SID,SID,SIDLength)==0)
		{
			*ptr = info->Next;
			if(info->Next!=NULL) 
			{
				(*ptr)->Previous = info->Previous;
				if((*ptr)->Previous!=NULL) 
				{
					(*ptr)->Previous->Next = info->Next;
					if((*ptr)->Previous->Next!=NULL)
					{
						(*ptr)->Previous->Next->Previous = (*ptr)->Previous;
					}
				}
			}
			break;
		}
		ptr = &(info->Next);
		info = info->Next;
	}
	if(info!=NULL)
	{
		info->Previous = NULL;
		info->Next = NULL;
		--(*TotalSubscribers);
	}
	return(info);
}

#define UpnpDestructSubscriberInfo(info)\
{\
	FREE(info->Path);\
	FREE(info->SID);\
	FREE(info);\
}

#define UpnpDestructEventObject(EvObject)\
{\
	FREE(EvObject->PacketBody);\
	FREE(EvObject);\
}

#define UpnpDestructEventDataObject(EvData)\
{\
	FREE(EvData);\
}
void UpnpExpireSubscriberInfo(struct UpnpDataObject *d, struct SubscriberInfo *info)
{
	struct SubscriberInfo *t = info;
	while(t->Previous!=NULL)
	{
		t = t->Previous;
	}
	if(d->HeadSubscriberPtr_ChannelManager==t)
	{
		--(d->NumberOfSubscribers_ChannelManager);
	}
	else if(d->HeadSubscriberPtr_RemoteInput==t)
	{
		--(d->NumberOfSubscribers_RemoteInput);
	}
	else if(d->HeadSubscriberPtr_RemoteIO==t)
	{
		--(d->NumberOfSubscribers_RemoteIO);
	}
	if(info->Previous!=NULL)
	{
		// This is not the Head
		info->Previous->Next = info->Next;
		if(info->Next!=NULL)
		{
			info->Previous->Next->Previous = info->Previous;
		}
	}
	else
	{
		// This is the Head
		if(d->HeadSubscriberPtr_ChannelManager==info)
		{
			d->HeadSubscriberPtr_ChannelManager = info->Next;
			if(info->Next!=NULL)
			{
				info->Next->Previous = d->HeadSubscriberPtr_ChannelManager;
			}
		}
		else if(d->HeadSubscriberPtr_RemoteInput==info)
		{
			d->HeadSubscriberPtr_RemoteInput = info->Next;
			if(info->Next!=NULL)
			{
				info->Next->Previous = d->HeadSubscriberPtr_RemoteInput;
			}
		}
		else if(d->HeadSubscriberPtr_RemoteIO==info)
		{
			d->HeadSubscriberPtr_RemoteIO = info->Next;
			if(info->Next!=NULL)
			{
				info->Next->Previous = d->HeadSubscriberPtr_RemoteIO;
			}
		}
		else
		{
			// Error
			return;
		}
	}
	ILibDeleteRequests(d->EventClient,info);
	--info->RefCount;
	if(info->RefCount==0)
	{
		UpnpDestructSubscriberInfo(info);
	}
}

int UpnpSubscriptionExpired(struct SubscriberInfo *info)
{
	int RetVal = 0;
	if(info->RenewByTime < GetTickCount()/1000) {RetVal = -1;}
	return(RetVal);
}
void UpnpGetInitialEventBody_ChannelManager(struct UpnpDataObject *UPnPObject,char ** body, int *bodylength)
{
	int TempLength;
	TempLength = (int)(47+(int)strlen(UPnPObject->ChannelManager_RegisteredChannelList));
	*body = (char*)MALLOC(sizeof(char)*TempLength);
	*bodylength = sprintf(*body,"RegisteredChannelList>%s</RegisteredChannelList",UPnPObject->ChannelManager_RegisteredChannelList);
}
void UpnpGetInitialEventBody_RemoteIO(struct UpnpDataObject *UPnPObject,char ** body, int *bodylength)
{
	int TempLength;
	TempLength = (int)(33+(int)strlen(UPnPObject->RemoteIO_PeerConnection));
	*body = (char*)MALLOC(sizeof(char)*TempLength);
	*bodylength = sprintf(*body,"PeerConnection>%s</PeerConnection",UPnPObject->RemoteIO_PeerConnection);
}
void UpnpProcessUNSUBSCRIBE(struct packetheader *header, struct HTTPReaderObject *ReaderObject)
{
	char* SID = NULL;
	int SIDLength = 0;
	struct SubscriberInfo *Info;
	struct packetheader_field_node *f;
	char* packet = (char*)MALLOC(sizeof(char)*40);
	int packetlength;
	
	f = header->FirstField;
	while(f!=NULL)
	{
		if(f->FieldLength==3)
		{
			if(_strnicmp(f->Field,"SID",3)==0)
			{
				SID = f->FieldData;
				SIDLength = f->FieldDataLength;
			}
		}
		f = f->NextField;
	}
	sem_wait(&(ReaderObject->Parent->EventLock));
	if(header->DirectiveObjLength==21 && memcmp(header->DirectiveObj + 1,"ChannelManager/event",20)==0)
	{
		Info = UpnpRemoveSubscriberInfo(&(ReaderObject->Parent->HeadSubscriberPtr_ChannelManager),&(ReaderObject->Parent->NumberOfSubscribers_ChannelManager),SID,SIDLength);
		if(Info!=NULL)
		{
			--Info->RefCount;
			if(Info->RefCount==0)
			{
				UpnpDestructSubscriberInfo(Info);
			}
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",200,"OK");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
		else
		{
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",412,"Invalid SID");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
	}
	else if(header->DirectiveObjLength==18 && memcmp(header->DirectiveObj + 1,"RemoteInput/event",17)==0)
	{
		Info = UpnpRemoveSubscriberInfo(&(ReaderObject->Parent->HeadSubscriberPtr_RemoteInput),&(ReaderObject->Parent->NumberOfSubscribers_RemoteInput),SID,SIDLength);
		if(Info!=NULL)
		{
			--Info->RefCount;
			if(Info->RefCount==0)
			{
				UpnpDestructSubscriberInfo(Info);
			}
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",200,"OK");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
		else
		{
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",412,"Invalid SID");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
	}
	else if(header->DirectiveObjLength==15 && memcmp(header->DirectiveObj + 1,"RemoteIO/event",14)==0)
	{
		Info = UpnpRemoveSubscriberInfo(&(ReaderObject->Parent->HeadSubscriberPtr_RemoteIO),&(ReaderObject->Parent->NumberOfSubscribers_RemoteIO),SID,SIDLength);
		if(Info!=NULL)
		{
			--Info->RefCount;
			if(Info->RefCount==0)
			{
				UpnpDestructSubscriberInfo(Info);
			}
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",200,"OK");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
		else
		{
			packetlength = sprintf(packet,"HTTP/1.0 %d %s\r\n\r\n",412,"Invalid SID");
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket=0;
		}
	}
	sem_post(&(ReaderObject->Parent->EventLock));
	FREE(packet);
}
void UpnpTryToSubscribe(char* ServiceName, long Timeout, char* URL, int URLLength,struct HTTPReaderObject *ReaderObject)
{
	int *TotalSubscribers = NULL;
	struct SubscriberInfo **HeadPtr = NULL;
	struct SubscriberInfo *NewSubscriber,*TempSubscriber;
	int SIDNumber;
	char *SID;
	char *TempString;
	int TempStringLength;
	char *TempString2;
	long TempLong;
	char *packet;
	int packetlength;
	char* path;
	
	char *packetbody = NULL;
	int packetbodyLength;
	
	struct parser_result *p;
	struct parser_result *p2;
	
	if(strncmp(ServiceName,"ChannelManager",14)==0)
	{
		TotalSubscribers = &(ReaderObject->Parent->NumberOfSubscribers_ChannelManager);
		HeadPtr = &(ReaderObject->Parent->HeadSubscriberPtr_ChannelManager);
	}
	if(strncmp(ServiceName,"RemoteInput",11)==0)
	{
		TotalSubscribers = &(ReaderObject->Parent->NumberOfSubscribers_RemoteInput);
		HeadPtr = &(ReaderObject->Parent->HeadSubscriberPtr_RemoteInput);
	}
	if(strncmp(ServiceName,"RemoteIO",8)==0)
	{
		TotalSubscribers = &(ReaderObject->Parent->NumberOfSubscribers_RemoteIO);
		HeadPtr = &(ReaderObject->Parent->HeadSubscriberPtr_RemoteIO);
	}
	if(*HeadPtr!=NULL)
	{
		NewSubscriber = *HeadPtr;
		while(NewSubscriber!=NULL)
		{
			if(UpnpSubscriptionExpired(NewSubscriber)!=0)
			{
				TempSubscriber = NewSubscriber->Next;
				NewSubscriber = UpnpRemoveSubscriberInfo(HeadPtr,TotalSubscribers,NewSubscriber->SID,NewSubscriber->SIDLength);
				UpnpDestructSubscriberInfo(NewSubscriber);
				NewSubscriber = TempSubscriber;
			}
			else
			{
				NewSubscriber = NewSubscriber->Next;
			}
		}
	}
	if(*TotalSubscribers<10)
	{
		NewSubscriber = (struct SubscriberInfo*)MALLOC(sizeof(struct SubscriberInfo));
		SIDNumber = ++ReaderObject->Parent->SID;
		SID = (char*)MALLOC(10 + 6);
		sprintf(SID,"uuid:%d",SIDNumber);
		p = ILibParseString(URL,0,URLLength,"://",3);
		if(p->NumResults==1)
		{
			send(ReaderObject->ClientSocket,"HTTP/1.1 412 Precondition Failed\r\n\r\n",36,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
			ILibDestructParserResults(p);
			return;
		}
		TempString = p->LastResult->data;
		TempStringLength = p->LastResult->datalength;
		ILibDestructParserResults(p);
		p = ILibParseString(TempString,0,TempStringLength,"/",1);
		p2 = ILibParseString(p->FirstResult->data,0,p->FirstResult->datalength,":",1);
		TempString2 = (char*)MALLOC(1+sizeof(char)*p2->FirstResult->datalength);
		memcpy(TempString2,p2->FirstResult->data,p2->FirstResult->datalength);
		TempString2[p2->FirstResult->datalength] = '\0';
		NewSubscriber->Address = inet_addr(TempString2);
		if(p2->NumResults==1)
		{
			NewSubscriber->Port = 80;
			path = (char*)MALLOC(1+TempStringLength - p2->FirstResult->datalength -1);
			memcpy(path,TempString + p2->FirstResult->datalength,TempStringLength - p2->FirstResult->datalength -1);
			path[TempStringLength - p2->FirstResult->datalength - 1] = '\0';
			NewSubscriber->Path = path;
			NewSubscriber->PathLength = (int)strlen(path);
		}
		else
		{
			ILibGetLong(p2->LastResult->data,p2->LastResult->datalength,&TempLong);
			NewSubscriber->Port = (unsigned short)TempLong;
			if(TempStringLength==p->FirstResult->datalength)
			{
				path = (char*)MALLOC(2);
				memcpy(path,"/",1);
				path[1] = '\0';
			}
			else
			{
				path = (char*)MALLOC(1+TempStringLength - p->FirstResult->datalength -1);
				memcpy(path,TempString + p->FirstResult->datalength,TempStringLength - p->FirstResult->datalength -1);
				path[TempStringLength - p->FirstResult->datalength -1] = '\0';
			}
			NewSubscriber->Path = path;
			NewSubscriber->PathLength = (int)strlen(path);
		}
		ILibDestructParserResults(p);
		ILibDestructParserResults(p2);
		FREE(TempString2);
		NewSubscriber->RefCount = 1;
		NewSubscriber->Disposing = 0;
		NewSubscriber->Previous = NULL;
		NewSubscriber->SID = SID;
		NewSubscriber->SIDLength = (int)strlen(SID);
		NewSubscriber->SEQ = 0;
		NewSubscriber->RenewByTime = (GetTickCount() / 1000) + Timeout;
		NewSubscriber->Next = *HeadPtr;
		if(*HeadPtr!=NULL) {(*HeadPtr)->Previous = NewSubscriber;}
		*HeadPtr = NewSubscriber;
		++(*TotalSubscribers);
		LVL3DEBUG(printf("\r\n\r\nSubscribed [%s] %d.%d.%d.%d:%d FOR %d Duration\r\n",NewSubscriber->SID,(NewSubscriber->Address)&0xFF,(NewSubscriber->Address>>8)&0xFF,(NewSubscriber->Address>>16)&0xFF,(NewSubscriber->Address>>24)&0xFF,NewSubscriber->Port,Timeout);)
		LVL3DEBUG(printf("TIMESTAMP: %d <%d>\r\n\r\n",(NewSubscriber->RenewByTime)-Timeout,NewSubscriber);)
		packet = (char*)MALLOC(134 + (int)strlen(SID) + 4);
		packetlength = sprintf(packet,"HTTP/1.1 200 OK\r\nSERVER: PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189\r\nSID: %s\r\nTIMEOUT: Second-%ld\r\nContent-Length: 0\r\n\r\n",SID,Timeout);
		if(strcmp(ServiceName,"ChannelManager")==0)
		{
			UpnpGetInitialEventBody_ChannelManager(ReaderObject->Parent,&packetbody,&packetbodyLength);
		}
		else if(strcmp(ServiceName,"RemoteInput")==0)
		{
			/* Service has no events */
			send(ReaderObject->ClientSocket,"HTTP/1.1 412 Service has no Events\r\n\r\n",38,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
		}
		else if(strcmp(ServiceName,"RemoteIO")==0)
		{
			UpnpGetInitialEventBody_RemoteIO(ReaderObject->Parent,&packetbody,&packetbodyLength);
		}
		if (packetbody != NULL)	    {
			send(ReaderObject->ClientSocket,packet,packetlength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
			FREE(packet);
			
			UpnpSendEvent_Body(ReaderObject->Parent,packetbody,packetbodyLength,NewSubscriber);
			FREE(packetbody);
		} 
	}
	else
	{
		/* Too many subscribers */
		send(ReaderObject->ClientSocket,"HTTP/1.1 412 Too Many Subscribers\r\n\r\n",37,0);
		closesocket(ReaderObject->ClientSocket);
		ReaderObject->ClientSocket = 0;
	}
}
void UpnpSubscribeEvents(char* path,int pathlength,char* Timeout,int TimeoutLength,char* URL,int URLLength,struct HTTPReaderObject* ReaderObject)
{
	long TimeoutVal;
	char* buffer = (char*)MALLOC(1+sizeof(char)*pathlength);
	
	ILibGetLong(Timeout,TimeoutLength,&TimeoutVal);
	memcpy(buffer,path,pathlength);
	buffer[pathlength] = '\0';
	FREE(buffer);
	if(TimeoutVal>7200) {TimeoutVal=7200;}
	
	if(pathlength==15 && memcmp(path+1,"RemoteIO/event",14)==0)
	{
		UpnpTryToSubscribe("RemoteIO",TimeoutVal,URL,URLLength,ReaderObject);
	}
	else if(pathlength==21 && memcmp(path+1,"ChannelManager/event",20)==0)
	{
		UpnpTryToSubscribe("ChannelManager",TimeoutVal,URL,URLLength,ReaderObject);
	}
	else
	{
		send(ReaderObject->ClientSocket,"HTTP/1.1 412 Invalid Service Name\r\n\r\n",37,0);
		closesocket(ReaderObject->ClientSocket);
		ReaderObject->ClientSocket = 0;
	}
}
void UpnpRenewEvents(char* path,int pathlength,char *_SID,int SIDLength, char* Timeout, int TimeoutLength, struct HTTPReaderObject *ReaderObject)
{
	struct SubscriberInfo *info = NULL;
	long TimeoutVal;
	char* packet;
	int packetlength;
	char* SID = (char*)MALLOC(SIDLength+1);
	memcpy(SID,_SID,SIDLength);
	SID[SIDLength] ='\0';
	LVL3DEBUG(printf("\r\n\r\nTIMESTAMP: %d\r\n",GetTickCount()/1000);)
	LVL3DEBUG(printf("SUBSCRIBER [%s] attempting to Renew Events for %s Duration [",SID,Timeout);)
	if(pathlength==21 && memcmp(path+1,"ChannelManager/event",20)==0)
	{
		info = ReaderObject->Parent->HeadSubscriberPtr_ChannelManager;
	}
	else if(pathlength==18 && memcmp(path+1,"RemoteInput/event",17)==0)
	{
		info = ReaderObject->Parent->HeadSubscriberPtr_RemoteInput;
	}
	else if(pathlength==15 && memcmp(path+1,"RemoteIO/event",14)==0)
	{
		info = ReaderObject->Parent->HeadSubscriberPtr_RemoteIO;
	}
	while(info!=NULL && strcmp(info->SID,SID)!=0)
	{
		info = info->Next;
	}
	if(info!=NULL)
	{
		ILibGetLong(Timeout,TimeoutLength,&TimeoutVal);
		info->RenewByTime = TimeoutVal + (GetTickCount() / 1000);
		packet = (char*)MALLOC(113 + (int)strlen(SID) + 4);
		packetlength = sprintf(packet,"HTTP/1.1 200 OK\r\nSERVER: PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189\r\nSID: %s\r\nTIMEOUT: Second-%ld\r\n\r\n",SID,TimeoutVal);
		send(ReaderObject->ClientSocket,packet,packetlength,0);
		FREE(packet);
		LVL3DEBUG(printf("OK] {%d} <%d>\r\n\r\n",TimeoutVal,info);)
	}
	else
	{
		LVL3DEBUG(printf("FAILED]\r\n\r\n");)
		send(ReaderObject->ClientSocket,"HTTP/1.1 412 Precondition Failed\r\n\r\n",36,0);
	}
	closesocket(ReaderObject->ClientSocket);
	ReaderObject->ClientSocket = 0;
	FREE(SID);
}
void UpnpProcessSUBSCRIBE(struct packetheader *header, struct HTTPReaderObject *ReaderObject)
{
	char* SID = NULL;
	int SIDLength = 0;
	char* Timeout = NULL;
	int TimeoutLength = 0;
	char* URL = NULL;
	int URLLength = 0;
	struct parser_result *p;
	struct packetheader_field_node *f;
	
	f = header->FirstField;
	while(f!=NULL)
	{
		if(f->FieldLength==3 && _strnicmp(f->Field,"SID",3)==0)
		{
			SID = f->FieldData;
			SIDLength = f->FieldDataLength;
		}
		else if(f->FieldLength==8 && _strnicmp(f->Field,"Callback",8)==0)
		{
			URL = f->FieldData;
			URLLength = f->FieldDataLength;
		}
		else if(f->FieldLength==7 && _strnicmp(f->Field,"Timeout",7)==0)
		{
			Timeout = f->FieldData;
			TimeoutLength = f->FieldDataLength;
		}
		f = f->NextField;
	}
	if(Timeout==NULL)
	{
		Timeout = "7200";
		TimeoutLength = 4;
	}
	else
	{
		p = ILibParseString(Timeout,0,TimeoutLength,"-",1);
		if(p->NumResults==2)
		{
			Timeout = p->LastResult->data;
			TimeoutLength = p->LastResult->datalength;
			if(TimeoutLength==8 && _strnicmp(Timeout,"INFINITE",8)==0)
			{
				Timeout = "7200";
				TimeoutLength = 4;
			}
		}
		else
		{
			Timeout = "7200";
			TimeoutLength = 4;
		}
		ILibDestructParserResults(p);
	}
	if(SID==NULL)
	{
		/* Subscribe */
		UpnpSubscribeEvents(header->DirectiveObj,header->DirectiveObjLength,Timeout,TimeoutLength,URL,URLLength,ReaderObject);
	}
	else
	{
		/* Renew */
		UpnpRenewEvents(header->DirectiveObj,header->DirectiveObjLength,SID,SIDLength,Timeout,TimeoutLength,ReaderObject);
	}
}
void UpnpProcessHTTPPacket(struct packetheader* header, struct HTTPReaderObject *ReaderObject)
{
	char *errorTemplate = "HTTP/1.0 %d %s\r\nServer: %s\r\n\r\n";
	char errorPacket[100];
	int errorPacketLength;
	char *buffer;
	/* Virtual Directory Support */
	if(header->DirectiveObjLength>=4 && memcmp(header->DirectiveObj,"/web",4)==0)
	{
		UpnpPresentationRequest((void*)ReaderObject,header);
	}
	else if(header->DirectiveLength==3 && memcmp(header->Directive,"GET",3)==0)
	{
		if(header->DirectiveObjLength==1 && memcmp(header->DirectiveObj,"/",1)==0)
		{
			send(ReaderObject->ClientSocket,ReaderObject->Parent->DeviceDescription,ReaderObject->Parent->DeviceDescriptionLength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
			return;
		}
		else if(header->DirectiveObjLength==18 && memcmp((header->DirectiveObj)+1,"RemoteIO/scpd.xml",17)==0)
		{
			buffer = UpnpDecompressString((char*)UpnpRemoteIODescription,UpnpRemoteIODescriptionLength,UpnpRemoteIODescriptionLengthUX);
			send(ReaderObject->ClientSocket, buffer, UpnpRemoteIODescriptionLengthUX, 0);
			FREE(buffer);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
		}
		else if(header->DirectiveObjLength==21 && memcmp((header->DirectiveObj)+1,"RemoteInput/scpd.xml",20)==0)
		{
			buffer = UpnpDecompressString((char*)UpnpRemoteInputDescription,UpnpRemoteInputDescriptionLength,UpnpRemoteInputDescriptionLengthUX);
			send(ReaderObject->ClientSocket, buffer, UpnpRemoteInputDescriptionLengthUX, 0);
			FREE(buffer);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
		}
		else if(header->DirectiveObjLength==24 && memcmp((header->DirectiveObj)+1,"ChannelManager/scpd.xml",23)==0)
		{
			buffer = UpnpDecompressString((char*)UpnpChannelManagerDescription,UpnpChannelManagerDescriptionLength,UpnpChannelManagerDescriptionLengthUX);
			send(ReaderObject->ClientSocket, buffer, UpnpChannelManagerDescriptionLengthUX, 0);
			FREE(buffer);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
		}
		else
		{
			errorPacketLength = sprintf(errorPacket,errorTemplate,404,"File Not Found","PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189");
			send(ReaderObject->ClientSocket,errorPacket,errorPacketLength,0);
			closesocket(ReaderObject->ClientSocket);
			ReaderObject->ClientSocket = 0;
			return;
		}
	}
	else if(header->DirectiveLength==4 && memcmp(header->Directive,"POST",4)==0)
	{
		UpnpProcessPOST(header,ReaderObject);
	}
	else if(header->DirectiveLength==9 && memcmp(header->Directive,"SUBSCRIBE",9)==0)
	{
		UpnpProcessSUBSCRIBE(header,ReaderObject);
	}
	else if(header->DirectiveLength==11 && memcmp(header->Directive,"UNSUBSCRIBE",11)==0)
	{
		UpnpProcessUNSUBSCRIBE(header,ReaderObject);
	}
	else
	{
		errorPacketLength = sprintf(errorPacket,errorTemplate,400,"Bad Request","PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189");
		send(ReaderObject->ClientSocket,errorPacket,errorPacketLength,0);
		closesocket(ReaderObject->ClientSocket);
		ReaderObject->ClientSocket = 0;
		return;
	}
}
void UpnpProcessHTTPSocket(struct HTTPReaderObject *ReaderObject)
{
	int bytesReceived = 0;
	int ContentLength = 0;
	struct packetheader_field_node *field;
	int headsize = 0;
	int x;
	
	if(ReaderObject->Body == NULL)
	{
		/* Still Reading Headers */
		bytesReceived = recv(ReaderObject->ClientSocket,ReaderObject->Header+ReaderObject->HeaderIndex,4000-ReaderObject->HeaderIndex,0);
		if(bytesReceived!=0 && bytesReceived!=0xFFFFFFFF)
		{
			/* Received Data
			*/
			ReaderObject->HeaderIndex += bytesReceived;
			if(ReaderObject->HeaderIndex >= 4)
			{
				/* Must have read at least 4 bytes to have a header */
				
				headsize = 0;
				for(x=0;x<(ReaderObject->HeaderIndex - 3);x++)
				{
					//printf("CMP: %x\r\n",*((int*)(ReaderObject->Header + x)));
					//if (*((int*)((ReaderObject->Header) + x)) == 0x0A0D0A0D)
					if (ReaderObject->Header[x] == '\r' && ReaderObject->Header[x+1] == '\n' && ReaderObject->Header[x+2] == '\r' && ReaderObject->Header[x+3] == '\n')
					{
						headsize = x + 4;
						break;
					}
				}
				
				if(headsize != 0)
				{
					/* Complete reading header */
					ReaderObject->ParsedHeader = ILibParsePacketHeader(ReaderObject->Header,0,headsize);
					field = ReaderObject->ParsedHeader->FirstField;
					while(field!=NULL)
					{
						if(field->FieldLength>=14)
						{
							if(_strnicmp(field->Field,"content-length",14)==0)
							{
								ContentLength = atoi(field->FieldData);
								break;
							}
						}
						field = field->NextField;
					}
					if(ContentLength==0)
					{
						/* No Body */
						ReaderObject->FinRead = 1;
						UpnpProcessHTTPPacket(ReaderObject->ParsedHeader, ReaderObject);
					}
					else
					{
						/* There is a Body */
						
						/* Check to see if over reading has occured */
						if (headsize < ReaderObject->HeaderIndex)
						{
							if(ReaderObject->HeaderIndex - headsize >= ContentLength)
							{
								ReaderObject->FinRead=1;
								ReaderObject->ParsedHeader->Body = ReaderObject->Header + headsize;
								ReaderObject->ParsedHeader->BodyLength = ContentLength;
								UpnpProcessHTTPPacket(ReaderObject->ParsedHeader, ReaderObject);
							}
							else
							{
								ReaderObject->Body = (char*)MALLOC(sizeof(char)*ContentLength);
								ReaderObject->BodySize = ContentLength;
								
								memcpy(ReaderObject->Body,ReaderObject->Header + headsize,UpnpMIN(ReaderObject->HeaderIndex - headsize,ContentLength));
								ReaderObject->BodyIndex = ReaderObject->HeaderIndex - headsize;
							}
						}
						else
						{
							ReaderObject->Body = (char*)MALLOC(sizeof(char)*ContentLength);
							ReaderObject->BodySize = ContentLength;
						}
					}
					//ILibDestructPacket(header);
				}
			}
		}
		else
		if(bytesReceived==0)
		{
			/* Socket Closed */
			ReaderObject->ClientSocket = 0;
		}
	}
	else
	{
		/* Reading Body */
		bytesReceived = recv(ReaderObject->ClientSocket,
		ReaderObject->Body+ReaderObject->BodyIndex,
		ReaderObject->BodySize-ReaderObject->BodyIndex,
		0);
		if(bytesReceived!=0)
		{
			/* Received Data */
			ReaderObject->BodyIndex += bytesReceived;
			if(ReaderObject->BodyIndex==ReaderObject->BodySize)
			{
				ReaderObject->FinRead=1;
				//header = ILibParsePacketHeader(ReaderObject->Header,0,ReaderObject->HeaderIndex);
				ReaderObject->ParsedHeader->Body = ReaderObject->Body;
				ReaderObject->ParsedHeader->BodyLength = ReaderObject->BodySize;
				UpnpProcessHTTPPacket(ReaderObject->ParsedHeader, ReaderObject);
				//ILibDestructPacket(header);
			}
		}
		else
		{
			/* Socket Closed/Error */
			ReaderObject->ClientSocket = 0;
		}
	}
}
void UpnpMasterPreSelect(void* object,fd_set *socketset, fd_set *writeset, fd_set *errorset, int* blocktime)
{
	int i;
	int NumFree = 5;
	struct UpnpDataObject *UPnPObject = (struct UpnpDataObject*)object;
	int notifytime;
	
	int ra = 1;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	unsigned char TTL = 4;
	
	if(UPnPObject->InitialNotify==0)
	{
		UPnPObject->InitialNotify = -1;
		UpnpSendByeBye(UPnPObject);
		UpnpSendNotify(UPnPObject);
	}
	if(UPnPObject->UpdateFlag!=0)
	{
		UPnPObject->UpdateFlag = 0;
		
		/* Clear Sockets */
		for(i=0;i<UPnPObject->AddressListLength;++i)
		{
			closesocket(UPnPObject->NOTIFY_SEND_socks[i]);
		}
		FREE(UPnPObject->NOTIFY_SEND_socks);
		
		/* Set up socket */
		FREE(UPnPObject->AddressList);
		UPnPObject->AddressListLength = ILibGetLocalIPAddressList(&(UPnPObject->AddressList));
		UPnPObject->NOTIFY_SEND_socks = (SOCKET*)MALLOC(sizeof(int)*(UPnPObject->AddressListLength));
		
		for(i=0;i<UPnPObject->AddressListLength;++i)
		{
			UPnPObject->NOTIFY_SEND_socks[i] = socket(AF_INET, SOCK_DGRAM, 0);
			memset((char *)&(addr), 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = UPnPObject->AddressList[i];
			addr.sin_port = (unsigned short)htons(UPNP_PORT);
			if (setsockopt(UPnPObject->NOTIFY_SEND_socks[i], SOL_SOCKET, SO_REUSEADDR,(char*)&ra, sizeof(ra)) == 0)
			{
				if (setsockopt(UPnPObject->NOTIFY_SEND_socks[i], IPPROTO_IP, IP_MULTICAST_TTL,(char*)&TTL, sizeof(TTL)) < 0)
				{
					// Ignore the case if setting the Multicast-TTL fails
				}
				if (bind(UPnPObject->NOTIFY_SEND_socks[i], (struct sockaddr *) &(addr), sizeof(addr)) == 0)
				{
					mreq.imr_multiaddr.s_addr = inet_addr(UPNP_GROUP);
					mreq.imr_interface.s_addr = UPnPObject->AddressList[i];
					if (setsockopt(UPnPObject->NOTIFY_RECEIVE_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,(char*)&mreq, sizeof(mreq)) < 0)
					{
						// Does not matter if it fails, just ignore
					}
				}
			}
		}
		UpnpSendNotify(UPnPObject);
	}
	FD_SET(UPnPObject->NOTIFY_RECEIVE_sock,socketset);
	for(i=0;i<5;++i)
	{
		if(UPnPObject->ReaderObjects[i].ClientSocket!=0)
		{
			if(UPnPObject->ReaderObjects[i].FinRead==0)
			{
				FD_SET(UPnPObject->ReaderObjects[i].ClientSocket,socketset);
				FD_SET(UPnPObject->ReaderObjects[i].ClientSocket,errorset);
			}
			--NumFree;
		}
	}
	
	notifytime = UpnpPeriodicNotify(UPnPObject);
	if(NumFree!=0)
	{
		FD_SET(UPnPObject->WebSocket,socketset);
		if(notifytime<*blocktime) {*blocktime=notifytime;}
	}
	else
	{
		if(*blocktime>1)
		{
			*blocktime = 1;
		}
	}
}

void UpnpWebServerTimerSink(void *data)
{
	struct HTTPReaderObject* RO = (struct HTTPReaderObject*)data;
	
	if(RO->ClientSocket!=0)
	{
		closesocket(RO->ClientSocket);
		RO->ClientSocket = 0;
	}
}
void UpnpMasterPostSelect(void* object,int slct, fd_set *socketset, fd_set *writeset, fd_set *errorset)
{
	unsigned long flags=0;
	int cnt = 0;
	int i;
	struct packetheader *packet;
	SOCKET NewSocket;
	struct sockaddr addr;
	int addrlen;
	struct UpnpDataObject *UPnPObject = (struct UpnpDataObject*)object;
	
	if(slct>0)
	{
		if(FD_ISSET(UPnPObject->WebSocket,socketset)!=0)
		{
			for(i=0;i<5;++i)
			{
				if(UPnPObject->ReaderObjects[i].ClientSocket==0)
				{
					addrlen = sizeof(addr);
					NewSocket = accept(UPnPObject->WebSocket,&addr,&addrlen);
					ioctlsocket(NewSocket,FIONBIO,&flags);
					if (NewSocket != 0xFFFFFFFF)
					{
						ILibLifeTime_Add(UPnPObject->WebServerTimer,&(UPnPObject->ReaderObjects[i]),3,&UpnpWebServerTimerSink,NULL);
						if(UPnPObject->ReaderObjects[i].Body != NULL)
						{
							FREE(UPnPObject->ReaderObjects[i].Body);
							UPnPObject->ReaderObjects[i].Body = NULL;
						}
						if(UPnPObject->ReaderObjects[i].ParsedHeader!=NULL)
						{
							ILibDestructPacket(UPnPObject->ReaderObjects[i].ParsedHeader);
						}
						UPnPObject->ReaderObjects[i].ClientSocket = NewSocket;
						UPnPObject->ReaderObjects[i].HeaderIndex = 0;
						UPnPObject->ReaderObjects[i].BodyIndex = 0;
						UPnPObject->ReaderObjects[i].Body = NULL;
						UPnPObject->ReaderObjects[i].BodySize = 0;
						UPnPObject->ReaderObjects[i].FinRead = 0;
						UPnPObject->ReaderObjects[i].Parent = UPnPObject;
						UPnPObject->ReaderObjects[i].ParsedHeader = NULL;
					}
					else {break;}
				}
			}
		}
		for(i=0;i<5;++i)
		{
			if(UPnPObject->ReaderObjects[i].ClientSocket!=0)
			{
				if(FD_ISSET(UPnPObject->ReaderObjects[i].ClientSocket,socketset)!=0)
				{
					UpnpProcessHTTPSocket(&(UPnPObject->ReaderObjects[i]));
				}
				if(FD_ISSET(UPnPObject->ReaderObjects[i].ClientSocket,errorset)!=0)
				{
					/* Socket is probably closed */
					UPnPObject->ReaderObjects[i].ClientSocket = 0;
					if(UPnPObject->ReaderObjects[i].Body != NULL)
					{
						FREE(UPnPObject->ReaderObjects[i].Body);
						UPnPObject->ReaderObjects[i].Body = NULL;
					}
				}
				if(UPnPObject->ReaderObjects[i].ClientSocket==0 || UPnPObject->ReaderObjects[i].FinRead!=0 || UPnPObject->ReaderObjects[i].Body!=NULL || (UPnPObject->ReaderObjects[i].ParsedHeader!=NULL && UPnPObject->ReaderObjects[i].ParsedHeader->Body != NULL))
				{
					ILibLifeTime_Remove(UPnPObject->WebServerTimer,&(UPnPObject->ReaderObjects[i]));
				}
			}
		}
		if(FD_ISSET(UPnPObject->NOTIFY_RECEIVE_sock,socketset)!=0)
		{	
			cnt = recvfrom(UPnPObject->NOTIFY_RECEIVE_sock, UPnPObject->message, sizeof(UPnPObject->message), 0,
			(struct sockaddr *) &(UPnPObject->addr), &(UPnPObject->addrlen));
			if (cnt < 0)
			{
				printf("recvfrom");
				exit(1);
			}
			else if (cnt == 0)
			{
				/* Socket Closed? */
			}
			packet = ILibParsePacketHeader(UPnPObject->message,0,cnt);
			packet->Source = (struct sockaddr_in*)&(UPnPObject->addr);
			packet->ReceivingAddress = 0;
			if(packet->StatusCode==-1 && memcmp(packet->Directive,"M-SEARCH",8)==0)
			{
				UpnpProcessMSEARCH(UPnPObject, packet);
			}
			ILibDestructPacket(packet);
		}
		
	}
}
int UpnpPeriodicNotify(struct UpnpDataObject *upnp)
{
	upnp->CurrentTime = GetTickCount() / 1000;
	if(upnp->CurrentTime >= upnp->NotifyTime)
	{
		upnp->NotifyTime = upnp->CurrentTime + (upnp->NotifyCycleTime / 3);
		UpnpSendNotify(upnp);
	}
	return(upnp->NotifyTime-upnp->CurrentTime);
}
void UpnpSendNotify(const struct UpnpDataObject *upnp)
{
	int packetlength;
	char* packet = (char*)MALLOC(5000);
	int i,i2;
	struct sockaddr_in addr;
	int addrlen;
	struct in_addr interface_addr;
	
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(UPNP_GROUP);
	addr.sin_port = (unsigned short)htons(UPNP_PORT);
	addrlen = sizeof(addr);
	
	memset((char *)&interface_addr, 0, sizeof(interface_addr));
	
	for(i=0;i<upnp->AddressListLength;++i)
	{
		interface_addr.s_addr = upnp->AddressList[i];
		/* PocketPC 2002 can't switch multicast interface correctly */
		/*
		if (setsockopt(upnp->NOTIFY_SEND_socks[i], IPPROTO_IP, IP_MULTICAST_IF,(char*)&interface_addr, sizeof(interface_addr)) == 0)
		{
			*/
			for (i2=0;i2<2;i2++)
			{
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::upnp:rootdevice","upnp:rootdevice","",upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"","uuid:",upnp->UDN,upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:device:RemoteIO:1","urn:schemas-upnp-org:device:RemoteIO:1","",upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteIO:1","urn:schemas-upnp-org:service:RemoteIO:1","",upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteInput:1","urn:schemas-upnp-org:service:RemoteInput:1","",upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
				UpnpBuildSsdpNotifyPacket(packet,&packetlength,upnp->AddressList[i],(unsigned short)upnp->WebSocketPortNumber,0,upnp->UDN,"::urn:schemas-upnp-org:service:ChannelManager:1","urn:schemas-upnp-org:service:ChannelManager:1","",upnp->NotifyCycleTime);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) {exit(1);}
			}
			/*
		}
		*/
	}
	FREE(packet);
}

#define UpnpBuildSsdpByeByePacket(outpacket,outlenght,USN,USNex,NT,NTex)\
{\
	*outlenght = sprintf(outpacket,"NOTIFY * HTTP/1.0\r\nHOST: 239.255.255.250:1900\r\nNTS: ssdp:byebye\r\nUSN: uuid:%s%s\r\nNT: %s%s\r\nContent-Length: 0\r\n\r\n",USN,USNex,NT,NTex);\
}

void UpnpSendByeBye(const struct UpnpDataObject *upnp)
{
	int packetlength;
	char* packet = (char*)MALLOC(5000);
	int i, i2;
	struct sockaddr_in addr;
	int addrlen;
	struct in_addr interface_addr;
	
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(UPNP_GROUP);
	addr.sin_port = (unsigned short)htons(UPNP_PORT);
	addrlen = sizeof(addr);
	
	memset((char *)&interface_addr, 0, sizeof(interface_addr));
	
	for(i=0;i<upnp->AddressListLength;++i)
	{
		
		/* PocketPC 2002 can't switch multicast interface correctly */
		/*
		interface_addr.s_addr = upnp->AddressList[i];
		if (setsockopt(upnp->NOTIFY_SEND_socks[i], IPPROTO_IP, IP_MULTICAST_IF,(char*)&interface_addr, sizeof(interface_addr)) == 0)
		{
			*/
			
			for (i2=0;i2<2;i2++)
			{
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"::upnp:rootdevice","upnp:rootdevice","");
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"","uuid:",upnp->UDN);
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"::urn:schemas-upnp-org:device:RemoteIO:1","urn:schemas-upnp-org:device:RemoteIO:1","");
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteIO:1","urn:schemas-upnp-org:service:RemoteIO:1","");
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"::urn:schemas-upnp-org:service:RemoteInput:1","urn:schemas-upnp-org:service:RemoteInput:1","");
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
				UpnpBuildSsdpByeByePacket(packet,&packetlength,upnp->UDN,"::urn:schemas-upnp-org:service:ChannelManager:1","urn:schemas-upnp-org:service:ChannelManager:1","");
				if (sendto(upnp->NOTIFY_SEND_socks[i], packet, packetlength, 0, (struct sockaddr *) &addr, addrlen) < 0) exit(1);
			}
			//			}
	}
	FREE(packet);
}

void UpnpResponse_Error(const void* UPnPToken, const int ErrorCode, const char* ErrorMsg)
{
	char* body;
	int bodylength;
	char* head;
	int headlength;
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	body = (char*)MALLOC(395 + (int)strlen(ErrorMsg));
	bodylength = sprintf(body,"<s:Envelope\r\n xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><s:Fault><faultcode>s:Client</faultcode><faultstring>UPnPError</faultstring><detail><UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\"><errorCode>%d</errorCode><errorDescription>%s</errorDescription></UPnPError></detail></s:Fault></s:Body></s:Envelope>",ErrorCode,ErrorMsg);
	head = (char*)MALLOC(59);
	headlength = sprintf(head,"HTTP/1.0 500 Internal\r\nContent-Length: %d\r\n\r\n",bodylength);
	send(ReaderObject->ClientSocket,head,headlength,0);
	send(ReaderObject->ClientSocket,body,bodylength,0);
	closesocket(ReaderObject->ClientSocket);
	ReaderObject->ClientSocket = 0;
	FREE(head);
	FREE(body);
}

int UpnpPresentationResponse(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate)
{
	int status = -1;
	SOCKET TempSocket;
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	if(DataLength>0)
	{
		status = send(ReaderObject->ClientSocket,Data,DataLength,0);
	}
	if (Terminate != 0)
	{
		TempSocket = ReaderObject->ClientSocket;
		ReaderObject->ClientSocket = 0;
		closesocket(TempSocket);
	}
	return status;
}

int UpnpGetLocalInterfaceToHost(const void* UPnPToken)
{
	struct sockaddr_in addr;
	int addrsize = sizeof(addr);
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	if (getsockname(ReaderObject->ClientSocket, (struct sockaddr*) &addr, &addrsize) != 0) return 0;
	return (addr.sin_addr.s_addr);
}

void UpnpResponseGeneric(const void* UPnPToken,const char* ServiceURI,const char* MethodName,const char* Params)
{
	char* packet;
	int packetlength;
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	
	packet = (char*)MALLOC(67+strlen(ServiceURI)+strlen(Params)+(strlen(MethodName)*2));
	packetlength = sprintf(packet,"<u:%sResponse xmlns:u=\"%s\">%s</u:%sResponse></s:Body></s:Envelope>",MethodName,ServiceURI,Params,MethodName);
	send(ReaderObject->ClientSocket,"HTTP/1.0 200 OK\r\nEXT:\r\nCONTENT-TYPE: text/xml\r\nSERVER: PPC2002, UPnP/1.0, Intel MicroStack/1.0.1189\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body>",275,0);
	send(ReaderObject->ClientSocket,packet,packetlength,0);
	closesocket(ReaderObject->ClientSocket);
	ReaderObject->ClientSocket = 0;
	FREE(packet);}

void UpnpResponse_RemoteIO_ForceDisconnection(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","ForceDisconnection","");
}

void UpnpResponse_RemoteIO_GetPeerConnection(const void* UPnPToken, const char* unescaped_PeerConnection)
{
	char* body;
	char *PeerConnection = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_PeerConnection));
	
	ILibXmlEscape(PeerConnection,unescaped_PeerConnection);
	body = (char*)MALLOC(34+strlen(PeerConnection));
	sprintf(body,"<PeerConnection>%s</PeerConnection>",PeerConnection);
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","GetPeerConnection",body);
	FREE(body);
	FREE(PeerConnection);
}

void UpnpResponse_RemoteIO_ForceReset(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","ForceReset","");
}

void UpnpResponse_RemoteIO_SetPeerInterlock(const void* UPnPToken, const char* unescaped_ActivePeerConnection)
{
	char* body;
	char *ActivePeerConnection = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_ActivePeerConnection));
	
	ILibXmlEscape(ActivePeerConnection,unescaped_ActivePeerConnection);
	body = (char*)MALLOC(46+strlen(ActivePeerConnection));
	sprintf(body,"<ActivePeerConnection>%s</ActivePeerConnection>",ActivePeerConnection);
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","SetPeerInterlock",body);
	FREE(body);
	FREE(ActivePeerConnection);
}

void UpnpResponse_RemoteIO_GetDeviceInformation(const void* UPnPToken, const char* unescaped_Application, const unsigned int MaxCommandSize, const int DisplayEncoding, const unsigned int DisplayWidth, const unsigned int DisplayHeight, const char* unescaped_DeviceInformation)
{
	char* body;
	char *Application = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_Application));
	char *DeviceInformation = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_DeviceInformation));
	
	ILibXmlEscape(Application,unescaped_Application);
	ILibXmlEscape(DeviceInformation,unescaped_DeviceInformation);
	body = (char*)MALLOC(239+strlen(Application)+strlen(DeviceInformation));
	sprintf(body,"<Application>%s</Application><MaxCommandSize>%u</MaxCommandSize><DisplayEncoding>%d</DisplayEncoding><DisplayWidth>%u</DisplayWidth><DisplayHeight>%u</DisplayHeight><DeviceInformation>%s</DeviceInformation>",Application,MaxCommandSize,DisplayEncoding,DisplayWidth,DisplayHeight,DeviceInformation);
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","GetDeviceInformation",body);
	FREE(body);
	FREE(Application);
	FREE(DeviceInformation);
}

void UpnpResponse_RemoteIO_SetPeerOverride(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteIO:1","SetPeerOverride","");
}

void UpnpResponse_RemoteInput_InputKeyPress(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputKeyPress","");
}

void UpnpResponse_RemoteInput_InputMouseMove(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputMouseMove","");
}

void UpnpResponse_RemoteInput_GetInputSetup(const void* UPnPToken, const char* unescaped_InputSetupIdentifier)
{
	char* body;
	char *InputSetupIdentifier = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_InputSetupIdentifier));
	
	ILibXmlEscape(InputSetupIdentifier,unescaped_InputSetupIdentifier);
	body = (char*)MALLOC(46+strlen(InputSetupIdentifier));
	sprintf(body,"<InputSetupIdentifier>%s</InputSetupIdentifier>",InputSetupIdentifier);
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","GetInputSetup",body);
	FREE(body);
	FREE(InputSetupIdentifier);
}

void UpnpResponse_RemoteInput_InputMouseUp(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputMouseUp","");
}

void UpnpResponse_RemoteInput_InputKeyUp(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputKeyUp","");
}

void UpnpResponse_RemoteInput_InputKeyDown(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputKeyDown","");
}

void UpnpResponse_RemoteInput_InputMouseDown(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:RemoteInput:1","InputMouseDown","");
}

void UpnpResponse_ChannelManager_RegisterChannel(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:ChannelManager:1","RegisterChannel","");
}

void UpnpResponse_ChannelManager_UnregisterChannel(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:ChannelManager:1","UnregisterChannel","");
}

void UpnpResponse_ChannelManager_ClearAllChannels(const void* UPnPToken)
{
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:ChannelManager:1","ClearAllChannels","");
}

void UpnpResponse_ChannelManager_GetRegisteredChannelList(const void* UPnPToken, const char* unescaped_ChannelList)
{
	char* body;
	char *ChannelList = (char*)MALLOC(1+ILibXmlEscapeLength(unescaped_ChannelList));
	
	ILibXmlEscape(ChannelList,unescaped_ChannelList);
	body = (char*)MALLOC(28+strlen(ChannelList));
	sprintf(body,"<ChannelList>%s</ChannelList>",ChannelList);
	UpnpResponseGeneric(UPnPToken,"urn:schemas-upnp-org:service:ChannelManager:1","GetRegisteredChannelList",body);
	FREE(body);
	FREE(ChannelList);
}

void UpnpSendEventSink(void *reader, struct packetheader *header, char* buffer, int *p_BeginPointer, int EndPointer, int done, void* subscriber, void *upnp)
{
	if(done!=0 && ((struct SubscriberInfo*)subscriber)->Disposing==0)
	{
		sem_wait(&(((struct UpnpDataObject*)upnp)->EventLock));
		--((struct SubscriberInfo*)subscriber)->RefCount;
		if(((struct SubscriberInfo*)subscriber)->RefCount==0)
		{
			LVL3DEBUG(printf("\r\n\r\nSubscriber at [%s] %d.%d.%d.%d:%d was/did UNSUBSCRIBE while trying to send event\r\n\r\n",((struct SubscriberInfo*)subscriber)->SID,(((struct SubscriberInfo*)subscriber)->Address&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>8)&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>16)&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>24)&0xFF),((struct SubscriberInfo*)subscriber)->Port);)
			UpnpDestructSubscriberInfo(((struct SubscriberInfo*)subscriber));
		}
		else if(header==NULL)
		{
			LVL3DEBUG(printf("\r\n\r\nCould not deliver event for [%s] %d.%d.%d.%d:%d UNSUBSCRIBING\r\n\r\n",((struct SubscriberInfo*)subscriber)->SID,(((struct SubscriberInfo*)subscriber)->Address&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>8)&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>16)&0xFF),((((struct SubscriberInfo*)subscriber)->Address>>24)&0xFF),((struct SubscriberInfo*)subscriber)->Port);)
			// Could not send Event, so unsubscribe the subscriber
			((struct SubscriberInfo*)subscriber)->Disposing = 1;
			UpnpExpireSubscriberInfo(upnp,subscriber);
		}
		sem_post(&(((struct UpnpDataObject*)upnp)->EventLock));
	}
}
void UpnpSendEvent_Body(void *upnptoken,char *body,int bodylength,struct SubscriberInfo *info)
{
	struct UpnpDataObject* UPnPObject = (struct UpnpDataObject*)upnptoken;
	struct sockaddr_in dest;
	int packetLength;
	char *packet;
	int ipaddr;
	
	memset(&dest,0,sizeof(dest));
	dest.sin_addr.s_addr = info->Address;
	dest.sin_port = htons(info->Port);
	dest.sin_family = AF_INET;
	ipaddr = info->Address;
	
	packet = (char*)MALLOC(info->PathLength + bodylength + 383);
	packetLength = sprintf(packet,"NOTIFY %s HTTP/1.0\r\nHOST: %d.%d.%d.%d:%d\r\nContent-Type: text/xml\r\nNT: upnp:event\r\nNTS: upnp:propchange\r\nSID: %s\r\nSEQ: %d\r\nContent-Length: %d\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\"><e:property><%s></e:property></e:propertyset>",info->Path,(ipaddr&0xFF),((ipaddr>>8)&0xFF),((ipaddr>>16)&0xFF),((ipaddr>>24)&0xFF),info->Port,info->SID,info->SEQ,bodylength+137,body);
	++info->SEQ;
	
	++info->RefCount;
	ILibAddRequest_Direct(UPnPObject->EventClient,packet,packetLength,&dest,&UpnpSendEventSink,NULL,info,upnptoken);
}
void UpnpSendEvent(void *upnptoken, char* body, const int bodylength, const char* eventname)
{
	struct SubscriberInfo *info = NULL;
	struct UpnpDataObject* UPnPObject = (struct UpnpDataObject*)upnptoken;
	struct sockaddr_in dest;
	LVL3DEBUG(struct timeval tv;)
	
	if(UPnPObject==NULL)
	{
		FREE(body);
		return;
	}
	sem_wait(&(UPnPObject->EventLock));
	if(strncmp(eventname,"ChannelManager",14)==0)
	{
		info = UPnPObject->HeadSubscriberPtr_ChannelManager;
	}
	if(strncmp(eventname,"RemoteInput",11)==0)
	{
		info = UPnPObject->HeadSubscriberPtr_RemoteInput;
	}
	if(strncmp(eventname,"RemoteIO",8)==0)
	{
		info = UPnPObject->HeadSubscriberPtr_RemoteIO;
	}
	memset(&dest,0,sizeof(dest));
	while(info!=NULL)
	{
		if(!UpnpSubscriptionExpired(info))
		{
			UpnpSendEvent_Body(upnptoken,body,bodylength,info);
		}
		else
		{
			//Remove Subscriber
			LVL3DEBUG(printf("\r\n\r\nTIMESTAMP: %d\r\n",GetTickCount()/1000);)
			LVL3DEBUG(printf("Did not renew [%s] %d.%d.%d.%d:%d UNSUBSCRIBING <%d>\r\n\r\n",((struct SubscriberInfo*)info)->SID,(((struct SubscriberInfo*)info)->Address&0xFF),((((struct SubscriberInfo*)info)->Address>>8)&0xFF),((((struct SubscriberInfo*)info)->Address>>16)&0xFF),((((struct SubscriberInfo*)info)->Address>>24)&0xFF),((struct SubscriberInfo*)info)->Port,info);)
		}
		
		info = info->Next;
	}
	
	sem_post(&(UPnPObject->EventLock));
}

void UpnpSetState_RemoteIO_PeerConnection(void *upnptoken, char* val)
{
	struct UpnpDataObject *UPnPObject = (struct UpnpDataObject*)upnptoken;
	char* body;
	int bodylength;
	char* valstr;
	valstr = (char*)MALLOC(ILibXmlEscapeLength(val)+1);
	ILibXmlEscape(valstr,val);
	if (UPnPObject->RemoteIO_PeerConnection != NULL) FREE(UPnPObject->RemoteIO_PeerConnection);
	UPnPObject->RemoteIO_PeerConnection = valstr;
	body = (char*)MALLOC(38 + (int)strlen(valstr));
	bodylength = sprintf(body,"%s>%s</%s","PeerConnection",valstr,"PeerConnection");
	UpnpSendEvent(upnptoken,body,bodylength,"RemoteIO");
	FREE(body);
}

void UpnpSetState_ChannelManager_RegisteredChannelList(void *upnptoken, char* val)
{
	struct UpnpDataObject *UPnPObject = (struct UpnpDataObject*)upnptoken;
	char* body;
	int bodylength;
	char* valstr;
	valstr = (char*)MALLOC(ILibXmlEscapeLength(val)+1);
	ILibXmlEscape(valstr,val);
	if (UPnPObject->ChannelManager_RegisteredChannelList != NULL) FREE(UPnPObject->ChannelManager_RegisteredChannelList);
	UPnPObject->ChannelManager_RegisteredChannelList = valstr;
	body = (char*)MALLOC(52 + (int)strlen(valstr));
	bodylength = sprintf(body,"%s>%s</%s","RegisteredChannelList",valstr,"RegisteredChannelList");
	UpnpSendEvent(upnptoken,body,bodylength,"ChannelManager");
	FREE(body);
}


void UpnpDestroyMicroStack(void *object)
{
	struct UpnpDataObject *upnp = (struct UpnpDataObject*)object;
	struct SubscriberInfo  *sinfo,*sinfo2;
	int i;
	UpnpSendByeBye(upnp);
	
	sem_destroy(&(upnp->EventLock));
	FREE(upnp->ChannelManager_RegisteredChannelList);
	FREE(upnp->RemoteIO_PeerConnection);
	
	FREE(upnp->AddressList);
	FREE(upnp->NOTIFY_SEND_socks);
	FREE(upnp->UUID);
	FREE(upnp->Serial);
	FREE(upnp->DeviceDescription);
	
	sinfo = upnp->HeadSubscriberPtr_ChannelManager;
	while(sinfo!=NULL)
	{
		sinfo2 = sinfo->Next;
		UpnpDestructSubscriberInfo(sinfo);
		sinfo = sinfo2;
	}
	sinfo = upnp->HeadSubscriberPtr_RemoteInput;
	while(sinfo!=NULL)
	{
		sinfo2 = sinfo->Next;
		UpnpDestructSubscriberInfo(sinfo);
		sinfo = sinfo2;
	}
	sinfo = upnp->HeadSubscriberPtr_RemoteIO;
	while(sinfo!=NULL)
	{
		sinfo2 = sinfo->Next;
		UpnpDestructSubscriberInfo(sinfo);
		sinfo = sinfo2;
	}
	
	for(i=0;i<5;++i)
	{
		if(upnp->ReaderObjects[i].Body!=NULL) {FREE(upnp->ReaderObjects[i].Body);}
		if(upnp->ReaderObjects[i].ParsedHeader!=NULL) {ILibDestructPacket(upnp->ReaderObjects[i].ParsedHeader);}
	}
	WSACleanup();
}
int UpnpGetLocalPortNumber(void *token)
{
	return(((struct UpnpDataObject*)token)->WebSocketPortNumber);
}
void *UpnpCreateMicroStack(void *Chain, const char* FriendlyName, const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum)
{
	struct UpnpDataObject* RetVal = (struct UpnpDataObject*)MALLOC(sizeof(struct UpnpDataObject));
	char* DDT;
	WORD wVersionRequested;
	WSADATA wsaData;
	
	srand((int)GetTickCount());
	wVersionRequested = MAKEWORD( 1, 1 );
	if (WSAStartup( wVersionRequested, &wsaData ) != 0) {exit(1);}
	UpnpInit(RetVal,NotifyCycleSeconds,PortNum);
	RetVal->ForceExit = 0;
	RetVal->PreSelect = &UpnpMasterPreSelect;
	RetVal->PostSelect = &UpnpMasterPostSelect;
	RetVal->Destroy = &UpnpDestroyMicroStack;
	RetVal->InitialNotify = 0;
	if (UDN != NULL)
	{
		RetVal->UUID = (char*)MALLOC((int)strlen(UDN)+6);
		sprintf(RetVal->UUID,"uuid:%s",UDN);
		RetVal->UDN = RetVal->UUID + 5;
	}
	if (SerialNumber != NULL)
	{
		RetVal->Serial = (char*)MALLOC((int)strlen(SerialNumber)+1);
		strcpy(RetVal->Serial,SerialNumber);
	}
	
	RetVal->DeviceDescription = (char*)MALLOC(UpnpDeviceDescriptionTemplateLengthUX + (int)strlen(FriendlyName) + (((int)strlen(RetVal->Serial) + (int)strlen(RetVal->UUID)) * 1));
	DDT = UpnpDecompressString((char*)UpnpDeviceDescriptionTemplate,UpnpDeviceDescriptionTemplateLength,UpnpDeviceDescriptionTemplateLengthUX);
	RetVal->DeviceDescriptionLength = sprintf(RetVal->DeviceDescription,DDT,FriendlyName,RetVal->Serial,RetVal->UDN);
	FREE(DDT);
	RetVal->ChannelManager_RegisteredChannelList = NULL;
	RetVal->RemoteIO_PeerConnection = NULL;
	
	RetVal->WebServerTimer = ILibCreateLifeTime(Chain);
	
	ILibAddToChain(Chain,RetVal);
	RetVal->EventClient = ILibCreateHTTPClientModule(Chain,5);
	RetVal->Chain = Chain;
	RetVal->UpdateFlag = 0;
	
	sem_init(&(RetVal->EventLock),0,1);
	return(RetVal);
}

void UpnpSendDataXmlEscaped(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate)
{
	int escsize;
	char* buf;
	
	escsize = (int)ILibXmlEscapeLength(Data);
	buf = (char*)MALLOC(escsize);
	
	if (buf != NULL)
	{
		escsize = ILibXmlEscape(buf,Data);
		UpnpSendData(UPnPToken,buf,escsize,Terminate);
		FREE(buf);
	}
}

void UpnpSendData(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate)
{
	struct HTTPReaderObject *ReaderObject = (struct HTTPReaderObject*)UPnPToken;
	send(ReaderObject->ClientSocket,Data,DataLength,0);
	if (Terminate != 0)
	{
		closesocket(ReaderObject->ClientSocket);
		ReaderObject->ClientSocket = 0;
	}
}

