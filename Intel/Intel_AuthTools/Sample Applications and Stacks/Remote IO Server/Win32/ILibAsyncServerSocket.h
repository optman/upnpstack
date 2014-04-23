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
 * $Workfile: <FILE>
 * $Revision: #1.0.1195.17540
 * $Author:   Intel Corporation, Intel Device Builder
 * $Date:     Thursday, April 10, 2003
 *
 */

#ifndef ___ILibAsyncServerSocket___
#define ___ILibAsyncServerSocket___

void *ILibCreateAsyncServerSocketModule(void *Chain, int MaxConnections, int PortNumber, int initialBufferSize, void (*OnConnect)(void *AsyncServerSocketModule, void *ConnectionToken,void **user),void (*OnDisconnect)(void *AsyncServerSocketModule, void *ConnectionToken, void *user),void (*OnReceive)(void *AsyncServerSocketModule, void *ConnectionToken,char* buffer,int *p_beginPointer, int endPointer,void (**OnInterrupt)(void *AsyncServerSocketMoudle, void *ConnectionToken, void *user), void **user),void (*OnInterrupt)(void *AsyncServerSocketMoudle, void *ConnectionToken, void *user),void (*OnSendOK)(void *AsyncServerSocketModule,void *ConnectionToken, void *user));
unsigned int ILibAsyncServreSocket_GetPendingBytesToSend(void *ServerSocketModule, void *ConnectionToken);

void ILibAsyncServerSocket_Send(void *ServerSocketModule, void *ConnectionToken, char* buffer, int bufferLength, int UserFreeBuffer);
void ILibAsyncServerSocket_Disconnect(void *ServerSocketModule, void *ConnectionToken);
#endif