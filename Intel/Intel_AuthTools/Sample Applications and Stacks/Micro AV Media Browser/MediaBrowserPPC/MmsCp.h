#ifndef _MMS_CP_H
#define _MMS_CP_H

#define MMSCP_CLASS_OBJECT_TYPE_LEN	3
#define MMSCP_CLASS_MAJOR_TYPE_LEN	13 
#define MMSCP_CLASS_MINOR_TYPE_LEN	13

extern const char* MMSCP_CLASS_OBJECT_TYPE[];
extern const char* MMSCP_CLASS_MAJOR_TYPE[];
extern const char* MMSCP_CLASS_MINOR_TYPE[];

/*
 *	Bit mapping of MediaClass types. Assumes 'unsigned int' is 64 bits wide.
 *
 *	0:		object has bad class
 *	1:		object is item
 *	2:		object is container
 *
 *	16-31:	minor 2 class type
 *			Extract these bits out, shift right MMSCP_SHIFT_MINOR2_TYPE bits 
 *			and use the value as an index into a custom array. String should append to
 *			the rest of the string that could be formed from the other bits.
 *
 *	32-47:	minor 1 class type
 *			Extract these bits out, shift right MMSCP_SHIFT_MINOR1_TYPE bits 
 *			and use the value as an index into the as MMSCP_CLASS_MINOR_TYPE array.
 *
 *	48-63:	major class type
 *			Extract these bits out, shift right MMSCP_SHIFT_MAJOR_TYPE bits 
 *			and use the value as an index into the as MMSCP_CLASS_MAJOR_TYPE array.
 */

#define MMSCP_SHIFT_MAJOR_TYPE						24
#define MMSCP_SHIFT_MINOR1_TYPE						16
#define MMSCP_SHIFT_MINOR2_TYPE						8

#define MMSCP_CLASS_MASK_MAJOR						0xFF000000
#define MMSCP_CLASS_MASK_MINOR1						0x00FF0000
#define MMSCP_CLASS_MASK_MINOR2						0x0000FF00
#define MMSCP_CLASS_MASK_OBJECTTYPE					0x00000003

#define MMSCP_CLASS_MASK_BADCLASS					0x00000000
#define MMSCP_CLASS_MASK_ITEM						0x00000001
#define MMSCP_CLASS_MASK_CONTAINER					0x00000002

#define MMSCP_CLASS_MASK_MAJOR_IMAGEITEM			0x01000000
#define MMSCP_CLASS_MASK_MAJOR_AUDIOITEM			0x02000000
#define MMSCP_CLASS_MASK_MAJOR_VIDEOITEM			0x03000000
#define MMSCP_CLASS_MASK_MAJOR_PLAYLISTITEM			0x04000000
#define MMSCP_CLASS_MASK_MAJOR_TEXTITEM				0x05000000
#define MMSCP_CLASS_MASK_MAJOR_PERSON				0x06000000
#define MMSCP_CLASS_MASK_MAJOR_PLAYLISTCONTAINER	0x07000000
#define MMSCP_CLASS_MASK_MAJOR_ALBUM				0x08000000
#define MMSCP_CLASS_MASK_MAJOR_GENRE				0x09000000
#define MMSCP_CLASS_MASK_MAJOR_STRGSYS				0x0A000000
#define MMSCP_CLASS_MASK_MAJOR_STRGVOL				0x0B000000
#define MMSCP_CLASS_MASK_MAJOR_STRGFOL				0x0C000000

#define MMSCP_CLASS_MASK_MINOR_PHOTO				0x00010000
#define MMSCP_CLASS_MASK_MINOR_MUSICTRACK			0x00020000
#define MMSCP_CLASS_MASK_MINOR_AUDIOBROADCAST		0x00030000
#define MMSCP_CLASS_MASK_MINOR_AUDIOBOOK			0x00040000
#define MMSCP_CLASS_MASK_MINOR_MOVIE				0x00050000
#define MMSCP_CLASS_MASK_MINOR_VIDEOBROADCAST		0x00060000
#define MMSCP_CLASS_MASK_MINOR_MUSICVIDEOCLIP		0x00070000
#define MMSCP_CLASS_MASK_MINOR_MUSICARTIST			0x00080000
#define MMSCP_CLASS_MASK_MINOR_MUSICALBUM			0x00090000
#define MMSCP_CLASS_MASK_MINOR_PHOTOALBUM			0x000A0000
#define MMSCP_CLASS_MASK_MINOR_MUSICGENRE			0x000B0000
#define MMSCP_CLASS_MASK_MINOR_MOVIEGENRE			0x000C0000

/*
 *	None of these error codes are allowed to overlap
 *	with the UPnP, UPnP-AV error code ranges.
 */
enum MMSCP_NonstandardErrorCodes
{
	MMSC_Error_XmlNotWellFormed		= 1000,
};

/*
 *	Provides mapping for MMSCP_Mediaobject's Flags field.
 *	Values must be friendly for bit operations.
 */
enum MMSCP_Enum_Flags
{
	MMSCP_Flags_Restricted = 1,			/* restricted attribute of media object */
	MMSCP_Flags_Searchable = 2			/* container is searchable */
};

enum MMSCP_Enum_BrowseFlag
{
	MMSCP_BrowseFlag_Metadata = 0,		/* browse metadata */
	MMSCP_BrowseFlag_Children			/* browse children */
};

/*
 *	Minimalistic representation of a resource.
 */
struct MMSCP_MediaResource
{
	char *Uri;
	char *ProtocolInfo;

	char *Resolution;
	char *Duration;
	long Bitrate;				/* if negative, has not been set */

	long ColorDepth;	/* if negative, has not been set */
	long Size;					/* if negative, has not been set */

	struct MMSCP_MediaResource *Next;
};

/*
 *	Minimalistic representation of a media object.
 */
struct MMSCP_MediaObject
{
	char *ID;			/* Object ID */
	char *ParentID;		/* Parent object ID */
	char *RefID;		/* Object ID of underlying item: for reference item only*/

	char *Title;		/* Title metadata */
	char *Creator;		/* Creator metadata */

	/* media class of object: masked values */
	unsigned int MediaClass;

	/* Boolean flags, bits mapped by MMSCP_Enum_Flags */
	unsigned int Flags;

	struct MMSCP_MediaResource *Res;		/* first resource for the media object*/
	struct MMSCP_MediaObject *Next;			/* next media object in list */
};

struct MMSCP_ResultsList
{
	/* points to the first media object in the results list */
	struct MMSCP_MediaObject *FirstObject;
	unsigned int NumberReturned;
	unsigned int TotalMatches;
	unsigned int UpdateID;

	/* number of media objects that were successfully parsed */
	int NumberParsed;

};

struct MMSCP_BrowseArgs
{
	char *ObjectID;
	enum MMSCP_Enum_BrowseFlag BrowseFlag;
	char *Filter;
	unsigned int StartingIndex;
	unsigned int RequestedCount;
	char *SortCriteria;
	void *UserObject;
};

typedef void (*MMSCP_Fn_Result_Browse) (void *serviceObj, struct MMSCP_BrowseArgs *args, int errorCode, struct MMSCP_ResultsList *results);
typedef void (*MMSCP_Fn_Device_AddRemove) (struct UPnPDevice *device, int added);

void MMSCP_DestroyResultsList (struct MMSCP_ResultsList *resultsList);

void MMSCP_Init(void *chain, MMSCP_Fn_Result_Browse callbackBrowse, MMSCP_Fn_Device_AddRemove callbackDeviceAddRemove);
void MMSCP_Invoke_Browse(void *serviceObj, struct MMSCP_BrowseArgs *args);

void MMSCP_Uninit();

#endif
