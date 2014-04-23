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
* $Workfile: ILibParsers.h
* $Revision: #1.0.1189.31920
* $Author:   Intel Corporation, Intel Device Builder
* $Date:     Monday, April 07, 2003
*
*/
#ifndef __ILibParsers__
#define __ILibParsers__

#include <sys/socket.h>

#define UPnPMIN(a,b) (((a)<(b))?(a):(b))
#define MALLOC malloc
#define FREE free

struct parser_result_field
{
	char* data;
	int datalength;
	struct parser_result_field *NextResult;
};
struct parser_result
{
	struct parser_result_field *FirstResult;
	struct parser_result_field *LastResult;
	int NumResults;
};
struct packetheader_field_node
{
	char* Field;
	int FieldLength;
	char* FieldData;
	int FieldDataLength;
	int UserAllocStrings;
	struct packetheader_field_node* NextField;
};
struct packetheader
{
	char* Directive;
	int DirectiveLength;
	char* DirectiveObj;
	int DirectiveObjLength;
	int StatusCode;
	char* StatusData;
	int StatusDataLength;
	char* Version;
	int VersionLength;
	char* Body;
	int BodyLength;
	int UserAllocStrings;
	
	struct packetheader_field_node* FirstField;
	struct packetheader_field_node* LastField;
	struct sockaddr_in *Source;
	int ReceivingAddress;
};
struct ILibXMLNode
{
	char* Name;
	int NameLength;
	
	char* NSTag;
	int NSLength;
	int StartTag;
	int EmptyTag;
	
	void *Reserved;
	struct ILibXMLNode *Next;
	struct ILibXMLNode *Parent;
	struct ILibXMLNode *Peer;
	struct ILibXMLNode *ClosingTag;
};
struct ILibXMLAttribute
{
	char* Name;
	int NameLength;
	
	char* Prefix;
	int PrefixLength;
	
	char* Value;
	int ValueLength;
	struct ILibXMLAttribute *Next;
};

int ILibFindEntryInTable(char *Entry, char **Table);

/* Stack Methods */
void ILibCreateStack(void **TheStack);
void ILibPushStack(void **TheStack, void *data);
void *ILibPopStack(void **TheStack);
void *ILibPeekStack(void **TheStack);
void ILibClearStack(void **TheStack);

/* XML Parsing Methods */
int ILibReadInnerXML(struct ILibXMLNode *node, char **RetVal);
struct ILibXMLNode *ILibParseXML(char *buffer, int offset, int length);
struct ILibXMLAttribute *ILibGetXMLAttributes(struct ILibXMLNode *node);
int ILibProcessXMLNodeList(struct ILibXMLNode *nodeList);
void ILibDestructXMLNodeList(struct ILibXMLNode *node);
void ILibDestructXMLAttributeList(struct ILibXMLAttribute *attribute);

/* Chaining Methods */
void *ILibCreateChain();
void ILibAddToChain(void *chain, void *object);
void ILibStartChain(void *chain);
void ILibStopChain(void *chain);
void ILibForceUnBlockChain(void *Chain);

/* HashTree Methods */
void* ILibInitHashTree();
void ILibDestroyHashTree(void *tree);
void ILibAddEntry(void* hashtree, char* key, int keylength, void *value);
void* ILibGetEntry(void *hashtree, char* key, int keylength);
void ILibDeleteEntry(void *hashtree, char* key, int keylength);

/* LifeTimeMonitor Methods */
void ILibLifeTime_Add(void *LifetimeMonitorObject,void *data, int seconds, void* Callback, void* Destroy);
void ILibLifeTime_Remove(void *LifeTimeToken, void *data);
void ILibLifeTime_Flush();
void *ILibCreateLifeTime(void *Chain);

struct parser_result* ILibParseString(char* buffer, int offset, int length, char* Delimiter, int DelimiterLength);
struct parser_result* ILibParseStringAdv(char* buffer, int offset, int length, char* Delimiter, int DelimiterLength);
void ILibDestructParserResults(struct parser_result *result);
struct packetheader *ILibCreateEmptyPacket();
void ILibAddHeaderLine(struct packetheader *packet, char* FieldName, int FieldNameLength, char* FieldData, int FieldDataLength);
char* ILibGetHeaderLine(struct packetheader *packet, char* FieldName, int FieldNameLength);
void ILibSetStatusCode(struct packetheader *packet, int StatusCode, char* StatusData, int StatusDataLength);
void ILibSetDirective(struct packetheader *packet, char* Directive, int DirectiveLength, char* DirectiveObj, int DirectiveObjLength);
void ILibDestructPacket(struct packetheader *packet);
struct packetheader* ILibParsePacketHeader(char* buffer, int offset, int length);
int ILibGetRawPacket(struct packetheader *packet,char **buffer);
int ILibGetLocalIPAddressList(int** pp_int);
unsigned short ILibGetDGramSocket(int local, int *TheSocket);
unsigned short ILibGetStreamSocket(int local, int *TheSocket);
int ILibGetLong(char *TestValue, int TestValueLength, long* NumericValue);
int ILibGetULong(const char *TestValue, const int TestValueLength, unsigned long* NumericValue);

void* dbg_malloc(int sz);
void dbg_free(void* ptr);
int dbg_GetCount();

/* XML escaping methods */
int ILibXmlEscape(char* outdata, const char* indata);
int ILibXmlEscapeLength(const char* data);
int ILibInPlaceXmlUnEscape(char* data);

/* Base64 handling methods */
int ILibBase64Encode(unsigned char* input, const int inputlen, unsigned char** output);
int ILibBase64Decode(unsigned char* input, const int inputlen, unsigned char** output);

#endif
