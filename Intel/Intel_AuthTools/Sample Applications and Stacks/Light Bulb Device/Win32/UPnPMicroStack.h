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
* $Workfile: UPnPMicrostack.h
* $Revision: #1.0.1181.32801
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Thursday, March 27, 2003
*
*/
#ifndef __UPnPMicrostack__
#define __UPnPMicrostack__

struct UPnPDataObject;
struct packetheader;

/* These methods must be implemented by the user */
extern void UPnPDimmingService_GetLoadLevelStatus(void* upnptoken);
extern void UPnPDimmingService_GetMinLevel(void* upnptoken);
extern void UPnPDimmingService_SetLoadLevelTarget(void* upnptoken,unsigned char NewLoadLevelTarget);
extern void UPnPSwitchPower_GetStatus(void* upnptoken);
extern void UPnPSwitchPower_SetTarget(void* upnptoken,int newTargetValue);
extern void UPnPPresentationRequest(void* upnptoken, struct packetheader *packet);

/* UPnP Stack Management */
void *UPnPCreateMicroStack(void *Chain, const char* FriendlyName, const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void *UPnP(void *Chain, const char* FriendlyName,const char* UDN, const char* SerialNumber, const int NotifyCycleSeconds, const unsigned short PortNum);
void UPnPIPAddressListChanged(void *MicroStackToken);
int UPnPGetLocalPortNumber(void *token);
int   UPnPGetLocalInterfaceToHost(const void* UPnPToken);
void* UPnPGetInstance(const void* UPnPToken);

/* Invocation Response Methods */
void UPnPResponse_Error(const void* UPnPToken, const int ErrorCode, const char* ErrorMsg);
void UPnPResponseGeneric(const void* UPnPToken,const char* ServiceURI,const char* MethodName,const char* Params);
int  UPnPPresentationResponse(const void* UPnPToken, const char* Data, const int DataLength, const int Terminate);
void UPnPResponse_SwitchPower_GetStatus(const void* UPnPToken, const int ResultStatus);
void UPnPResponse_SwitchPower_SetTarget(const void* UPnPToken);
void UPnPResponse_DimmingService_GetLoadLevelStatus(const void* UPnPToken, const unsigned char RetLoadLevelStatus);
void UPnPResponse_DimmingService_GetMinLevel(const void* UPnPToken, const unsigned char MinLevel);
void UPnPResponse_DimmingService_SetLoadLevelTarget(const void* UPnPToken);

/* State Variable Eventing Methods */
void UPnPSetState_SwitchPower_Status(void *microstack,int val);
void UPnPSetState_DimmingService_LoadLevelStatus(void *microstack,unsigned char val);

#endif
