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
* $Workfile: ILibMiniWebServer.h
* $Revision: #1.0.1200.16858
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Tuesday, April 15, 2003
*
*/
#ifndef __ILibMiniWebServer__
#define __ILibMiniWebServer__

/* Forward Declaration */
struct packetheader;

void* ILibCreateMiniWebServer(void *chain,int MaxSockets,void (*OnReceive) (void *ReaderObject, struct packetheader *header, char* buffer, int *BeginPointer, int BufferSize, int done, void* user),void *user);
void ILibDestroyMiniWebServer(void *WebServerModule);
void ILibStartMiniWebServerModule(void *WebServerModule);
void ILibStopMiniWebServerModule(void *WebServerModule);

void ILibMiniWebServer_SetReserved(void *MWS, void *object);
void *ILibMiniWebServer_GetReserved(void *MWS);
void *ILibMiniWebServer_GetMiniWebServerFromReader(void *Reader);

int ILibGetMiniWebServerPortNumber(void *WebServerModule);
void ILibMiniWebServerSend(void *ReaderObject, struct packetheader *packet);
void ILibMiniWebServerCloseSession(void *ReaderObject);

char* ILibGetReceivingInterface(void* ReaderObject);
void ILibCloseRequest(void* ReaderObject);	

#endif
