#include "MmsCp.h"
#include "MSCPControlPoint.h"
#include "ILibParsers.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef UNDER_CE
#include "assert.h"
#else
#define stricmp _stricmp
#endif

#define UNSUPPORTED_BY_CP printf("Action is not supported by this implementation."); ASSERT(1);

#define MMSCP_DEVICE_ADDED 1
#define MMSCP_DEVICE_REMOVED 0

#define MMSCP_BROWSE_FLAG_METADATA_STRING "BrowseMetadata"
#define MMSCP_BROWSE_FLAG_CHILDREN_STRING "BrowseDirectChildren"

/* CDS normative tag names and attributes */
#define MMSCP_ATTRIB_ID				"id"
#define MMSCP_ATTRIB_PARENTID		"parentID"
#define MMSCP_ATTRIB_REFID			"refID"
#define MMSCP_ATTRIB_RESTRICTED		"restricted"
#define MMSCP_ATTRIB_SEARCHABLE		"searchable"
#define MMSCP_ATTRIB_PROTOCOLINFO	"protocolInfo"
#define MMSCP_ATTRIB_RESOLUTION		"resolution"
#define MMSCP_ATTRIB_DURATION		"duration"
#define MMSCP_ATTRIB_BITRATE		"bitrate"
#define MMSCP_ATTRIB_COLORDEPTH		"colorDepth"
#define MMSCP_ATTRIB_SIZE			"size"

#define MMSCP_TAG_DIDL				"DIDL-Lite"
#define MMSCP_TAG_CONTAINER			"container"
#define MMSCP_TAG_ITEM				"item"
#define MMSCP_TAG_RESOURCE			"res"

#define MMSCP_TAG_CREATOR			"dc:creator"
#define MMSCP_TAG_TITLE				"dc:title"
#define MMSCP_TAG_MEDIACLASS		"upnp:class"

#ifdef _DEBUG
#define	ASSERT(x) assert (x)
#define MMSCP_MALLOC(x) MMSCP_malloc(x)
#define MMSCP_FREE(x) MMSCP_free(x)
#define DEBUGONLY(x) x
#else
#ifndef UNDER_CE
#define ASSERT(x)
#endif
#define MMSCP_MALLOC(x) malloc(x)
#define MMSCP_FREE(x) free(x)
#define DEBUGONLY(x)
#endif

#ifdef _TEMPDEBUG
#define TEMPDEBUGONLY(x) x
#else
#define TEMPDEBUGONLY(x) 
#endif

/***********************************************************************************************************************
 *	BEGIN: MMSCP static values
 ***********************************************************************************************************************/

/*
 *	The relative order of strings within these arrays must correspond to the MMSCP_CLASS_MASK_xxx bitmask mappings.
 */
const char* MMSCP_CLASS_OBJECT_TYPE[] = {"object", "item", "container"};
const char* MMSCP_CLASS_MAJOR_TYPE[] = {"", "imageItem", "audioItem", "videoItem", "playlistItem", "textItem", "person", "playlistContainer", "album", "genre", "storageSystem", "storageVolume", "storageFolder"};
const char* MMSCP_CLASS_MINOR_TYPE[] = {"", "photo", "musicTrack", "audioBroadcast", "audioBook", "movie", "videoBroadcast", "musicVideClip", "musicArtist", "musicAlbum", "photoAlbum", "musicGenre", "movieGenre"};

#define MMSCP_CLASS_FIRST_MAJOR_TYPE	MMSCP_CLASS_MASK_MAJOR_IMAGEITEM
/*
 *	Maximum length of a string in 
 *	MMSCP_CLASS_OBJECT_TYPE, MMSCP_CLASS_MAJOR_TYPE, and MMSCP_CLASS_MINOR_TYPE.
 *	Size includes null terminator.
 */
#define MMSCP_MAX_CLASS_FRAGMENT_LEN	17
#define MMSCP_MAX_CLASS_FRAGMENT_SIZE	18

#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

/*
 *	These are the types of strings that can be searched.
 */
enum MMSCP_SearchableStringTypes
{
	MMSCP_STRTYPE_ID,
	MMSCP_STRTYPE_CREATOR,
	MMSCP_STRTYPE_PROTOCOLINFO
};

const char* MMSCP_TRUE_STRINGS[] = {"1", "true", "yes"};
const char* MMSCP_FALSE_STRINGS[] = {"0", "false", "no"};
#define MMSCP_TRUE_STRINGS_LEN 3
#define MMSCP_FALSE_STRINGS_LEN 3

/***********************************************************************************************************************
 *	END: MMSCP static values
 ***********************************************************************************************************************/




/***********************************************************************************************************************
 *	BEGIN: MMSCP state variables
 ***********************************************************************************************************************/

/* Function pointer for sending Browse results back to caller */
MMSCP_Fn_Result_Browse				MMSCP_Callback_Browse;
MMSCP_Fn_Device_AddRemove			MMSCP_Callback_DeviceAddRemove;

int MMSCP_malloc_counter = 0;

/***********************************************************************************************************************
 *	END: MMSCP state variables
 ***********************************************************************************************************************/




/***********************************************************************************************************************
 *	BEGIN: Helper methods
 ***********************************************************************************************************************/

/* TODO: debug malloc/MMSCP_FREE is not thread safe */
void* MMSCP_malloc(int sz)
{
	++MMSCP_malloc_counter;
	return((void*)malloc(sz));
}
void MMSCP_free(void* ptr)
{
	--MMSCP_malloc_counter;
	free(ptr);	
}
int MMSCP_malloc_GetCount()
{
	return(MMSCP_malloc_counter);
}

/*
 *	Copies bytes from copyFrom to copyHere.
 *	Will not copy more than copyMaxChars bytes.
 *	Stops copying when ., <, null, or " char is found.
 */
void MMSCP_CopyUntilClassFragmentTerminator(char *copyHere, const char *copyFrom, int copyMaxChars)
{
	int i;
	char c;
	
	for (i=0; i < copyMaxChars; i++)
	{
		c = copyFrom[i];
		
		if (c == '.' || c == '<' || c == '\0' || c == '"')
		{
			copyHere[i] = '\0';
			return;
		}
		else
		{
			copyHere[i] = c;
		}
	}
}

/*
 *	Given an array of strings, finds the index in that array with a matching string.
 */
int MMSCP_FindStringInArray(const char* str,const char** strarray,const int strarraylen)
{
	int i;
	for (i=0;i<strarraylen;i++) {if (stricmp(str,strarray[i]) == 0) {return i;}}
	return -1;
}


void MMSCP_StringFixup(char **fixThis, char** di, char *emptyStr, const char *data, const char *rangeStart, const char *rangeEnd)
{
	int len;

	if (data != NULL)
	{
		if ((rangeStart <= data) && (data <= rangeEnd))
		{
			/* store an XML-unescaped representation */

			*fixThis = *di;
			len = (int) strlen(data);
			memcpy(*di, data, len);

			ILibInPlaceXmlUnEscape(*di);

			*di = *di + len + 1;
		}
		else
		{
			*fixThis = (char*)data;
		}
	}
	else
	{
		*fixThis = emptyStr;
	}
}


int MMSCP_GetRequiredSizeForMediaObject(struct MMSCP_MediaObject *obj, struct MMSCP_MediaObject *obj2)
{
	int retVal;
	struct MMSCP_MediaResource *res;
	struct MMSCP_MediaResource *res2;
	unsigned char addProtInfo;
	unsigned char addResolution;
	struct MMSCP_MediaResource *resProt;
	struct MMSCP_MediaResource *resRes;

	retVal = 0;

	if (obj->ID != NULL)
	{
		retVal += ((int) strlen(obj->ID) +1);
	}

	if (obj->ParentID != NULL)
	{
		if ((obj2 != NULL) && (strcmp(obj2->ParentID, obj->ParentID) == 0))
		{
			obj->ParentID = obj2->ParentID;
		}
		else
		{
			retVal += ((int) strlen(obj->ParentID) +1);
		}
	}

	if (obj->RefID != NULL)
	{
		retVal += ((int) strlen(obj->RefID) +1);
	}

	if (obj->Title != NULL)
	{
		retVal += ((int) strlen(obj->Title) +1);
	}

	if (obj->Creator != NULL)
	{
		if ((obj2 != NULL) && (strcmp(obj2->Creator, obj->Creator) == 0))
		{
			obj->Creator = obj2->Creator;
		}
		else
		{
			retVal += ((int) strlen(obj->Creator) +1);
		}
	}

	res = obj->Res;
	res2 = NULL;
	while (res != NULL)
	{
		//if (res->ProtocolInfo != NULL)
		addProtInfo = (res->ProtocolInfo != NULL);
		addResolution = (res->Resolution != NULL);
		{
			if (obj2 != NULL)
			{
				res2 = obj2->Res;
			}

			resProt = NULL;
			resRes = NULL;
			while (res2 != NULL)
			{
				if (addProtInfo && (res2->ProtocolInfo != NULL) && (strcmp(res2->ProtocolInfo, res->ProtocolInfo) == 0))
				{
					addProtInfo = 0;
					resProt = res2;
				}
				if (addResolution && (res2->Resolution != NULL) && (strcmp(res2->Resolution, res->Resolution) == 0))
				{
					addResolution = 0;
					resRes = res2;
				}

				if ((addProtInfo != 0) || (addResolution != 0))
				{
					res2 = res2->Next;
				}
				else
				{
					res2 = NULL;
				}
			}

			if (addProtInfo != 0)
			{
				retVal += ((int) strlen(res->ProtocolInfo) +1);
			}
			else if (resProt != NULL)
			{
				res->ProtocolInfo = resProt->ProtocolInfo;
			}
			
			if (addResolution != 0)
			{
				retVal += ((int) strlen(res->Resolution) +1);
			}
			else if (resRes != NULL)
			{
				res->Resolution = resRes->Resolution;
			}
		}

		if (res->Uri != NULL)
		{
			retVal += ((int) strlen(res->Uri) +1);
		}

		res = res->Next;
	}

	return retVal;
}

void MMSCP_RemoveQuotFromAttribValue(struct ILibXMLAttribute *att)
{
	if ((att->Value[0] = '"') || (att->Value[0] == '\''))
	{
		att->Value++;
		att->ValueLength -= 2;
	}
}

struct MMSCP_MediaObject* MMSCP_CreateMediaObject(struct ILibXMLNode *node, struct ILibXMLAttribute *attribs, int isItem, struct MMSCP_MediaObject *obj2, const char *rangeStart, const char *rangeEnd)
{
	struct ILibXMLNode *startNode;
	struct ILibXMLAttribute *att;

	struct MMSCP_MediaObject tempObj;
	struct MMSCP_MediaObject* newObj;

	struct MMSCP_MediaResource *res;

	char *innerXml;
	int innerXmlLen;
	char classFragment[MMSCP_MAX_CLASS_FRAGMENT_SIZE];
	int indexIntoArray;

	int dataSize;
	int mallocSize;

	char *di;
	char *emptyDI;

	#ifdef _DEBUG
	/* PRECONDITION: node is a start node*/
	if (node->StartTag == 0)
	{
		printf("MMSCP_CreateMediaObject requires node->StartTag!=0.\r\n");
		ASSERT(0);
	}
	
	/* PRECONDITION: node->Name is null terminated and this node is a container or item */
	if (!(
		(stricmp(node->Name, MMSCP_TAG_CONTAINER) == 0) ||
		(stricmp(node->Name, MMSCP_TAG_ITEM) == 0)
		))
	{
		printf("MMSCP_CreateMediaObject requires item or container node.\r\n");
		ASSERT(0);
	}
	#endif

	/* initialize temp obj to zero; init flags appropriately */
	memset(&tempObj, 0, sizeof(struct MMSCP_MediaObject));
	tempObj.Flags |= MMSCP_Flags_Restricted;	/* assume object is restricted */
	if (isItem == 0)
	{
		tempObj.Flags |= MMSCP_Flags_Searchable;/* assume container is searchable */
	}

	/*
	 *
	 *	Parse the item/container node and set the pointers in tempObj
	 *	to point into the memory referenced by node.
	 *
	 */

	/* Parse the attributes of the item/container */
	att = attribs;
	while (att != NULL)
	{
		/* [DONOTREPARSE] null terminate name and value. */
		att->Name[att->NameLength] = '\0';
		MMSCP_RemoveQuotFromAttribValue(att);
		att->Value[att->ValueLength] = '\0';

		if (stricmp(att->Name, MMSCP_ATTRIB_ID) == 0)
		{
			tempObj.ID = att->Value;
		}
		else if (stricmp(att->Name, MMSCP_ATTRIB_PARENTID) == 0)
		{
			tempObj.ParentID = att->Value;
		}
		else if (stricmp(att->Name, MMSCP_ATTRIB_RESTRICTED) == 0)
		{
			if (MMSCP_FindStringInArray(att->Value, MMSCP_TRUE_STRINGS, MMSCP_TRUE_STRINGS_LEN) >= 0)
			{
				/* set the restricted flag. */
				tempObj.Flags |= MMSCP_Flags_Restricted;
			}
			else
			{
				tempObj.Flags &= (~MMSCP_Flags_Restricted);
			}
		}
		else if ((isItem == 0) && (stricmp(att->Name, MMSCP_ATTRIB_SEARCHABLE) == 0))
		{
			if (MMSCP_FindStringInArray(att->Value, MMSCP_TRUE_STRINGS, MMSCP_TRUE_STRINGS_LEN) >= 0)
			{
				/* set the searchable flag. */
				tempObj.Flags |= MMSCP_Flags_Searchable;
			}
			else
			{
				tempObj.Flags &= (~MMSCP_Flags_Searchable);
			}
		}
		else if ((isItem != 0) && (stricmp(att->Name, MMSCP_ATTRIB_REFID) == 0))
		{
			tempObj.RefID = att->Value;
		}
		att = att->Next;
	}

	/*
	 *
	 *	Iterate through the child nodes of the startNode
	 *	and set the title, creator, and resources for
	 *	the media object.
	 *
	 */

	startNode = node;
	node = startNode->Next;
	while (node != startNode->ClosingTag)
	{
		/* [DONOTREPARSE] null terminate name */
		attribs = ILibGetXMLAttributes(node);
		att = attribs;
		node->Name[node->NameLength] = '\0';

		if (node->StartTag != 0)
		{
			if (stricmp(node->Name, MMSCP_TAG_RESOURCE) == 0)
			{
				/*
				 *
				 *	Create a new resource element and add it
				 *	to the existing list of resources for the
				 *	media object. The resource will point to 
				 *	memory in XML, but we'll change where they
				 *	point at the very end.
				 *
				 */
				
				if (tempObj.Res == NULL)
				{
					tempObj.Res = (struct MMSCP_MediaResource*) MMSCP_MALLOC (sizeof(struct MMSCP_MediaResource));
					res = tempObj.Res;
				}
				else
				{
					res->Next = (struct MMSCP_MediaResource*) MMSCP_MALLOC (sizeof(struct MMSCP_MediaResource));
					res = res->Next;
				}

				/* initialize everything to zero */
				memset(res, 0, sizeof(struct MMSCP_MediaResource));
				res->Bitrate = res->ColorDepth = res->Size = -1;

				/* Extract the protocolInfo from the element */
				while (att != NULL)
				{
					/* [DONOTREPARSE] */
					att->Name[att->NameLength] = '\0';
					MMSCP_RemoveQuotFromAttribValue(att);
					att->Value[att->ValueLength] = '\0';
								
					if (stricmp(att->Name, MMSCP_ATTRIB_PROTOCOLINFO) == 0)
					{
						res->ProtocolInfo = att->Value;
						break;
					}
					else if (stricmp(att->Name, MMSCP_ATTRIB_RESOLUTION) == 0)
					{
						res->Resolution = att->Value;
					}
					else if (stricmp(att->Name, MMSCP_ATTRIB_DURATION) == 0)
					{
						res->Duration = att->Value;
					}
					else if (stricmp(att->Name, MMSCP_ATTRIB_BITRATE) == 0)
					{
						ILibGetLong(att->Value, att->ValueLength, &(res->Bitrate));
					}
					else if (stricmp(att->Name, MMSCP_ATTRIB_COLORDEPTH) == 0)
					{
						ILibGetLong(att->Value, att->ValueLength, &(res->ColorDepth));
					}
					else if (stricmp(att->Name, MMSCP_ATTRIB_SIZE) == 0)
					{
						ILibGetLong(att->Value, att->ValueLength, &(res->Size));
					}
					
					att = att->Next;
				}

				/* grab the URI */
				innerXmlLen = ILibReadInnerXML(node, &innerXml);
				innerXml[innerXmlLen] = '\0';
				res->Uri = innerXml;
			}
			else if (stricmp(node->NSTag, MMSCP_TAG_MEDIACLASS) == 0)
			{
				/* Figure out proper enum value given the specified media class */
				innerXmlLen = ILibReadInnerXML(node, &innerXml);

				/* initialize to bad class */
				tempObj.MediaClass = MMSCP_CLASS_MASK_BADCLASS;
							
				/* determine object type */
				MMSCP_CopyUntilClassFragmentTerminator(classFragment, innerXml, MIN(innerXmlLen, MMSCP_MAX_CLASS_FRAGMENT_LEN));
				indexIntoArray = MMSCP_FindStringInArray(classFragment, MMSCP_CLASS_OBJECT_TYPE, MMSCP_CLASS_OBJECT_TYPE_LEN);

				if (indexIntoArray == 0)
				{
					innerXml += ((int) strlen(MMSCP_CLASS_OBJECT_TYPE[indexIntoArray]) + 1);
					MMSCP_CopyUntilClassFragmentTerminator(classFragment, innerXml, MIN(innerXmlLen, MMSCP_MAX_CLASS_FRAGMENT_LEN));
					indexIntoArray = MMSCP_FindStringInArray(classFragment, MMSCP_CLASS_OBJECT_TYPE, MMSCP_CLASS_OBJECT_TYPE_LEN);

					if (indexIntoArray > 0)
					{
						innerXml += ((int) strlen(MMSCP_CLASS_OBJECT_TYPE[indexIntoArray]) + 1);
						tempObj.MediaClass = indexIntoArray;
									
						/* Determine major type */
						MMSCP_CopyUntilClassFragmentTerminator(classFragment, innerXml, MIN(innerXmlLen, MMSCP_MAX_CLASS_FRAGMENT_LEN));
						indexIntoArray = MMSCP_FindStringInArray(classFragment, MMSCP_CLASS_MAJOR_TYPE, MMSCP_CLASS_MAJOR_TYPE_LEN);
						if (indexIntoArray > 0)
						{
							innerXml += ((int) strlen(MMSCP_CLASS_MAJOR_TYPE[indexIntoArray]) + 1);
							tempObj.MediaClass |= (indexIntoArray << MMSCP_SHIFT_MAJOR_TYPE);

							/* Determine minor type */
							MMSCP_CopyUntilClassFragmentTerminator(classFragment, innerXml, MIN(innerXmlLen, MMSCP_MAX_CLASS_FRAGMENT_LEN));
							indexIntoArray = MMSCP_FindStringInArray(classFragment, MMSCP_CLASS_MAJOR_TYPE, MMSCP_CLASS_MAJOR_TYPE_LEN);
							if (indexIntoArray > 0)
							{
								tempObj.MediaClass |= (indexIntoArray << MMSCP_SHIFT_MINOR1_TYPE);
								/* TODO : Add vendor-specific supported minor types parsing here */
							}
						}
					}
				}
			}
			else if (stricmp(node->NSTag, MMSCP_TAG_CREATOR) == 0)
			{
				innerXmlLen = ILibReadInnerXML(node, &innerXml);
				innerXml[innerXmlLen] = '\0';
				tempObj.Creator = innerXml;
			}
			else if (stricmp(node->NSTag, MMSCP_TAG_TITLE) == 0)
			{
				innerXmlLen = ILibReadInnerXML(node, &innerXml);
				innerXml[innerXmlLen] = '\0';
				tempObj.Title = innerXml;
			}
		}

		node = node->Next;
		#ifdef _DEBUG
		if (node == NULL)
		{
			printf("MMSCP_CreateMediaObject: Unexpected null node.\r\n");
			ASSERT(0);
		}
		#endif

		/* free attribute mapping */
		ILibDestructXMLAttributeList(attribs);
	}

	/*
	 *
	 *	At this point, we have a temp media object and possibly some media resources.
	 *	All string data is simply a pointer into the XML string. In order to
	 *	maximize on efficient memory usage, we do the following.
	 *
	 *	1)	Determine size needed for all new strings in results set. Also note which strings need to be copied in this step.
	 *	2)	Create a new media object, with additional memory for storing new string data.
	 *	3)	Point new media object's fields to either the new memory or to existing memory from a previous media object.
	 *	4)	Connect new media object to resource objects (attached to temp)
	 *	5)	Point each field of each resource to memory in new memory to existing memory from a previous media object.
	 *
	 *
	 */

	/*
	 *	Create the new media object, with additional memory for string data appended at the end.
	 */
	dataSize = MMSCP_GetRequiredSizeForMediaObject(&tempObj, obj2);
	mallocSize = dataSize + sizeof(struct MMSCP_MediaObject) + 1;
	newObj = (struct MMSCP_MediaObject*) MMSCP_MALLOC(mallocSize);
	memset(newObj, 0, mallocSize);

	newObj->MediaClass = tempObj.MediaClass;
	newObj->Flags = tempObj.Flags;

	/* di will point to where it's safe to write string data */
	di = (char*)newObj;
	di += sizeof(struct MMSCP_MediaObject);
	emptyDI = di;
	di ++;

	MMSCP_StringFixup(&(newObj->ID),		&di, emptyDI, tempObj.ID,		rangeStart, rangeEnd);
	MMSCP_StringFixup(&(newObj->ParentID),	&di, emptyDI, tempObj.ParentID,	rangeStart, rangeEnd);
	MMSCP_StringFixup(&(newObj->RefID),		&di, emptyDI, tempObj.RefID,	rangeStart, rangeEnd);
	MMSCP_StringFixup(&(newObj->Title),		&di, emptyDI, tempObj.Title,	rangeStart, rangeEnd);
	MMSCP_StringFixup(&(newObj->Creator),	&di, emptyDI, tempObj.Creator,	rangeStart, rangeEnd);

	newObj->Res = tempObj.Res;
	res = newObj->Res;
	while (res != NULL)
	{
		/*
		 *	Since resources are already allocated, we send the same parameters
		 *	for arg1 and arg3.
		 */
		MMSCP_StringFixup(&(res->ProtocolInfo), &di, emptyDI, res->ProtocolInfo,	rangeStart, rangeEnd);
		MMSCP_StringFixup(&(res->Resolution),	&di, emptyDI, res->Resolution,		rangeStart, rangeEnd);
		MMSCP_StringFixup(&(res->Uri),			&di, emptyDI, res->Uri,				rangeStart, rangeEnd);
		res = res->Next;
	}

	/* prevent memory corruption in debug version */
	ASSERT(di <= ((int)newObj) + mallocSize);

	return newObj;
}


/***********************************************************************************************************************
 *	END: Helper methods
 ***********************************************************************************************************************/

 
 
 
/***********************************************************************************************************************
 *	BEGIN: UPnP Callback Sinks
 *	These methods are callback sinks that are wired to the underlying UPNP stack.
 ***********************************************************************************************************************/

#ifndef MMSCP_LEAN_AND_MEAN
void MMSCP_ProcessResponse_GetCurrentConnectionInfo(struct UPnPService* Service,int ErrorCode,void *User,int RcsID,int AVTransportID,char* ProtocolInfo,char* PeerConnectionManager,int PeerConnectionID,char* Direction,char* Status)
{
	printf("MSCP Invoke Response: ConnectionManager/GetCurrentConnectionInfo(%d,%d,%s,%s,%d,%s,%s)\r\n",RcsID,AVTransportID,ProtocolInfo,PeerConnectionManager,PeerConnectionID,Direction,Status);
	UNSUPPORTED_BY_CP;
}

void MMSCP_ProcessResponse_GetProtocolInfo(struct UPnPService* Service,int ErrorCode,void *User,char* Source,char* Sink)
{
	printf("MSCP Invoke Response: ConnectionManager/GetProtocolInfo(%s,%s)\r\n",Source,Sink);
	UNSUPPORTED_BY_CP;
}

void MMSCP_ProcessResponse_GetCurrentConnectionIDs(struct UPnPService* Service,int ErrorCode,void *User,char* ConnectionIDs)
{
	printf("MSCP Invoke Response: ConnectionManager/GetCurrentConnectionIDs(%s)\r\n",ConnectionIDs);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_ExportResource(struct UPnPService* Service,int ErrorCode,void *User,unsigned int TransferID)
{
	printf("MSCP Invoke Response: ContentDirectory/ExportResource(%u)\r\n",TransferID);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_StopTransferResource(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("MSCP Invoke Response: ContentDirectory/StopTransferResource()\r\n");
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_DestroyObject(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("MSCP Invoke Response: ContentDirectory/DestroyObject()\r\n");
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_UpdateObject(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("MSCP Invoke Response: ContentDirectory/UpdateObject()\r\n");
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_GetSystemUpdateID(struct UPnPService* Service,int ErrorCode,void *User,unsigned int Id)
{
	printf("MSCP Invoke Response: ContentDirectory/GetSystemUpdateID(%u)\r\n",Id);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_GetTransferProgress(struct UPnPService* Service,int ErrorCode,void *User,char* TransferStatus,char* TransferLength,char* TransferTotal)
{
	printf("MSCP Invoke Response: ContentDirectory/GetTransferProgress(%s,%s,%s)\r\n",TransferStatus,TransferLength,TransferTotal);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_GetSortCapabilities(struct UPnPService* Service,int ErrorCode,void *User,char* SortCaps)
{
	printf("MSCP Invoke Response: ContentDirectory/GetSortCapabilities(%s)\r\n",SortCaps);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_GetSearchCapabilities(struct UPnPService* Service,int ErrorCode,void *User,char* SearchCaps)
{
	printf("MSCP Invoke Response: ContentDirectory/GetSearchCapabilities(%s)\r\n",SearchCaps);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_CreateObject(struct UPnPService* Service,int ErrorCode,void *User,char* ObjectID,char* Result)
{
	printf("MSCP Invoke Response: ContentDirectory/CreateObject(%s,%s)\r\n",ObjectID,Result);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_Search(struct UPnPService* Service,int ErrorCode,void *User,char* Result,unsigned int NumberReturned,unsigned int TotalMatches,unsigned int UpdateID)
{
	printf("MSCP Invoke Response: ContentDirectory/Search(%s,%u,%u,%u)\r\n",Result,NumberReturned,TotalMatches,UpdateID);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_ImportResource(struct UPnPService* Service,int ErrorCode,void *User,unsigned int TransferID)
{
	printf("MSCP Invoke Response: ContentDirectory/ImportResource(%u)\r\n",TransferID);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_CreateReference(struct UPnPService* Service,int ErrorCode,void *User,char* NewID)
{
	printf("MSCP Invoke Response: ContentDirectory/CreateReference(%s)\r\n",NewID);
	UNSUPPORTED_BY_CP;
}

void MSCPResponseSink_ContentDirectory_DeleteResource(struct UPnPService* Service,int ErrorCode,void *User)
{
	printf("MSCP Invoke Response: ContentDirectory/DeleteResource()\r\n");
	UNSUPPORTED_BY_CP;
}

void MMSCPEventSink_ConnectionManager_SourceProtocolInfo(struct UPnPService* Service,char* SourceProtocolInfo)
{
	printf("MSCP Event from %s/ConnectionManager/SourceProtocolInfo: %s\r\n",Service->Parent->FriendlyName,SourceProtocolInfo);
	UNSUPPORTED_BY_CP;
}

void MMSCPEventSink_ConnectionManager_SinkProtocolInfo(struct UPnPService* Service,char* SinkProtocolInfo)
{
	printf("MSCP Event from %s/ConnectionManager/SinkProtocolInfo: %s\r\n",Service->Parent->FriendlyName,SinkProtocolInfo);
	UNSUPPORTED_BY_CP;
}

void MMSCPEventSink_ConnectionManager_CurrentConnectionIDs(struct UPnPService* Service,char* CurrentConnectionIDs)
{
	printf("MSCP Event from %s/ConnectionManager/CurrentConnectionIDs: %s\r\n",Service->Parent->FriendlyName,CurrentConnectionIDs);
	UNSUPPORTED_BY_CP;
}

void MMSCPEventSink_ContentDirectory_TransferIDs(struct UPnPService* Service,char* TransferIDs)
{
	printf("MSCP Event from %s/ContentDirectory/TransferIDs: %s\r\n",Service->Parent->FriendlyName,TransferIDs);
	UNSUPPORTED_BY_CP;
}

#endif

void MSCPResponseSink_ContentDirectory_Browse(struct UPnPService* Service,int ErrorCode,void *User,char* Result,unsigned int NumberReturned,unsigned int TotalMatches,unsigned int UpdateID)
{
	struct ILibXMLNode* nodeList;
	struct ILibXMLNode* node;
	struct MMSCP_ResultsList *resultsList;
	struct ILibXMLAttribute *attribs;
	
	int resultLen;
	int parsePeerResult = 0;
	char *lastResultPos;

	struct MMSCP_MediaObject *newObj, *lastObj;

	TEMPDEBUGONLY(printf("MSCP Invoke Response: ContentDirectory/Browse(%s,%u,%u,%u)\r\n",Result,NumberReturned,TotalMatches,UpdateID);)

	if ((ErrorCode == 0) && (Result != NULL))
	{
		lastObj = newObj = NULL;
		resultLen = ILibInPlaceXmlUnEscape(Result);
		resultsList = (struct MMSCP_ResultsList*) MMSCP_MALLOC (sizeof(struct MMSCP_ResultsList));
		memset(resultsList, 0, sizeof(struct MMSCP_ResultsList));

		lastResultPos = Result + resultLen;
		nodeList = ILibParseXML(Result, 0, resultLen);
		parsePeerResult = ILibProcessXMLNodeList(nodeList);

		if (parsePeerResult != 0)
		{
			MMSCP_Callback_Browse(Service, User, (int)MMSC_Error_XmlNotWellFormed, NULL);
		}
		else if (resultLen == 0)
		{
			MMSCP_Callback_Browse(Service, User, ErrorCode, NULL);
		}
		else
		{
			node = nodeList;
			while (node != NULL)
			{
				if (node->StartTag != 0)
				{
					/*[DONOTREPARSE] null terminate string */
					attribs = ILibGetXMLAttributes(node);
					node->Name[node->NameLength] = '\0';

					newObj = NULL;
					if (stricmp(node->Name, MMSCP_TAG_CONTAINER) == 0)
					{
						newObj = MMSCP_CreateMediaObject(node, attribs, 0, lastObj, Result, lastResultPos);
						node = node->Next;
					}
					else if (stricmp(node->Name, MMSCP_TAG_ITEM) == 0)
					{
						newObj = MMSCP_CreateMediaObject(node, attribs, 1, lastObj, Result, lastResultPos);
						node = node->Next;
					}
					else if (stricmp(node->Name, MMSCP_TAG_DIDL) == 0)
					{
						/* this is didl-lite root node, go to first child */
						node = node->Next;
					}
					else
					{
						/* this node is not supported, go to next sibling/peer */
						if (node->Peer != NULL)
						{
							node = node->Peer;
						}
						else
						{
							node = node->Parent->Peer;
						}
					}

					if (newObj != NULL)
					{
						if (resultsList->FirstObject == NULL)
						{
							lastObj = resultsList->FirstObject = newObj;
						}
						else
						{
							lastObj->Next = newObj;
							lastObj = newObj;
						}
					}

					/* free attribute mappings */
					ILibDestructXMLAttributeList(attribs);
				}
				else
				{
					node = node->Next;
				}
			}
		}

		resultsList->NumberReturned = NumberReturned;
		resultsList->TotalMatches = TotalMatches;
		resultsList->UpdateID = UpdateID;

		/* validate number of parsed objects against returned count */
		lastObj = resultsList->FirstObject;
		resultsList->NumberParsed = 0;
		while (lastObj != NULL)
		{
			resultsList->NumberParsed++;
			lastObj = lastObj->Next;
		}

		if ((int)resultsList->NumberParsed != (int)resultsList->NumberReturned)
		{
			printf("MSCPResponseSink_ContentDirectory_Browse: Detected mismatch with number of objects returned=%u and parsed=%d.\r\n", resultsList->NumberReturned, resultsList->NumberParsed);
		}

		/* free resources from XML parsing */
		ILibDestructXMLNodeList(nodeList);

		/* execute callback with results */
		MMSCP_Callback_Browse(Service, User, ErrorCode, resultsList);
	}
	else
	{
		MMSCP_Callback_Browse(Service, User, ErrorCode, NULL);
	}
}


void MMSCPEventSink_ContentDirectory_ContainerUpdateIDs(struct UPnPService* Service,char* ContainerUpdateIDs)
{
	printf("MSCP Event from %s/ContentDirectory/ContainerUpdateIDs: %s\r\n",Service->Parent->FriendlyName,ContainerUpdateIDs);
	UNSUPPORTED_BY_CP;
}

void MMSCPEventSink_ContentDirectory_SystemUpdateID(struct UPnPService* Service,unsigned int SystemUpdateID)
{
	printf("MSCP Event from %s/ContentDirectory/SystemUpdateID: %u\r\n",Service->Parent->FriendlyName,SystemUpdateID);
	UNSUPPORTED_BY_CP;
}


/* Called whenever a new device on the correct type is discovered */
void MMSCP_UPnPSink_DeviceAdd(struct UPnPDevice *device)
{
	printf("MSCP Device Added: %s\r\n", device->FriendlyName);
	
	if (MMSCP_Callback_DeviceAddRemove != NULL)
	{
		MMSCP_Callback_DeviceAddRemove(device, MMSCP_DEVICE_ADDED);
	}
}

/* Called whenever a discovered device was removed from the network */
void MMSCP_UPnPSink_DeviceRemove(struct UPnPDevice *device)
{
	printf("MSCP Device Removed: %s\r\n", device->FriendlyName);

	if (MMSCP_Callback_DeviceAddRemove != NULL)
	{
		MMSCP_Callback_DeviceAddRemove(device, MMSCP_DEVICE_REMOVED);
	}
}

/***********************************************************************************************************************
 *	END: UPnP Callback Sinks
 ***********************************************************************************************************************/





/***********************************************************************************************************************
 *	BEGIN: API method implementations
 ***********************************************************************************************************************/
void MMSCP_DestroyResultsList (struct MMSCP_ResultsList *resultsList)
{
	struct MMSCP_MediaObject *obj;
	struct MMSCP_MediaObject *nextObj;
	struct MMSCP_MediaResource *res;
	struct MMSCP_MediaResource *nextRes;

	obj = resultsList->FirstObject;

	while (obj != NULL)
	{
		nextObj = obj->Next;

		res = obj->Res;
		while (res != NULL)
		{
			nextRes = res->Next;
			MMSCP_FREE(res);
			res = nextRes;
		}

		MMSCP_FREE(obj);
		obj = nextObj;
	}

	MMSCP_FREE (resultsList);
}

void MMSCP_Init(void *chain, MMSCP_Fn_Result_Browse callbackBrowse, MMSCP_Fn_Device_AddRemove callbackDeviceAddRemove)
{
	MMSCP_Callback_Browse = callbackBrowse;
	MMSCP_Callback_DeviceAddRemove = callbackDeviceAddRemove;

	/* Event callback function registration code */

#ifndef MMSCP_LEAN_AND_MEAN
	MSCPEventCallback_ConnectionManager_SourceProtocolInfo=&MMSCPEventSink_ConnectionManager_SourceProtocolInfo;
	MSCPEventCallback_ConnectionManager_SinkProtocolInfo=&MMSCPEventSink_ConnectionManager_SinkProtocolInfo;
	MSCPEventCallback_ConnectionManager_CurrentConnectionIDs=&MMSCPEventSink_ConnectionManager_CurrentConnectionIDs;
	MSCPEventCallback_ContentDirectory_TransferIDs=&MMSCPEventSink_ContentDirectory_TransferIDs;
#endif

	MSCPEventCallback_ContentDirectory_ContainerUpdateIDs=&MMSCPEventSink_ContentDirectory_ContainerUpdateIDs;
	MSCPEventCallback_ContentDirectory_SystemUpdateID=&MMSCPEventSink_ContentDirectory_SystemUpdateID;

	/* create the underlying UPnP control point stack */
	MSCPCreateControlPoint(chain, &MMSCP_UPnPSink_DeviceAdd, &MMSCP_UPnPSink_DeviceRemove);
}

void MMSCP_Invoke_Browse(void *serviceObj, struct MMSCP_BrowseArgs *args)
{
	char *browseFlagString;

	if (args->BrowseFlag == MMSCP_BrowseFlag_Metadata)
	{
		browseFlagString = MMSCP_BROWSE_FLAG_METADATA_STRING;
	}
	else
	{
		browseFlagString = MMSCP_BROWSE_FLAG_CHILDREN_STRING;
	}

	MSCPInvoke_ContentDirectory_Browse
		(
		serviceObj, 
		MSCPResponseSink_ContentDirectory_Browse, 
		args, 
		args->ObjectID, 
		browseFlagString, 
		args->Filter, 
		args->StartingIndex, 
		args->RequestedCount, 
		args->SortCriteria
		);
}

void MMSCP_Uninit()
{
	MMSCP_Callback_Browse = NULL;
	MMSCP_Callback_DeviceAddRemove = NULL;
}

/***********************************************************************************************************************
 *	END: API method implementations
 ***********************************************************************************************************************/
