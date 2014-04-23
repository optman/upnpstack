#include <string.h>
#include "MimeTypes.h"

#ifdef UNDER_CE
#define stricmp _stricmp
#endif

#ifdef _POSIX
#define stricmp strcasecmp
#endif

char* FileExtensionToMimeType (char* extension)
{
	if (stricmp(extension, EXTENSION_AUDIO_MPEG) == 0)
	{
		return MIME_TYPE_AUDIO_MPEG;
	}
	else if (stricmp(extension, EXTENSION_AUDIO_WMA) == 0)
	{
		return MIME_TYPE_AUDIO_WMA;
	}
	else if (stricmp(extension, EXTENSION_AUDIO_OGG) == 0)
	{
		return MIME_TYPE_AUDIO_OGG;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_ASF) == 0)
	{
		return MIME_TYPE_VIDEO_ASF;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_WMV) == 0)
	{
		return MIME_TYPE_VIDEO_WMV;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_MPEG) == 0)
	{
		return MIME_TYPE_VIDEO_MPEG;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_MOV) == 0)
	{
		return MIME_TYPE_VIDEO_MOV;
	}
	else if (stricmp(extension, EXTENSION_IMAGE_JPG) == 0)
	{
		return MIME_TYPE_IMAGE_JPG;
	}	
	else if (stricmp(extension, EXTENSION_IMAGE_JPEG) == 0)
	{
		return MIME_TYPE_IMAGE_JPEG;
	}	
	else if (stricmp(extension, EXTENSION_IMAGE_BMP) == 0)
	{
		return MIME_TYPE_IMAGE_BMP;
	}	
	
	/*
	else if (strcmpi(extension, EXTENSION_PLAYLIST_M3U) == 0)
	{
		return MIME_TYPE_PLAYLIST_M3U;
	}
	*/

	return "application/octet-stream";
}

char* MimeTypeToFileExtension (char* mime_type)
{
	if (stricmp(mime_type, MIME_TYPE_AUDIO_MPEG) == 0)
	{
		return EXTENSION_AUDIO_MPEG;
	}
	else if (stricmp(mime_type, MIME_TYPE_AUDIO_WMA) == 0)
	{
		return EXTENSION_AUDIO_WMA;
	}
	else if (stricmp(mime_type, MIME_TYPE_AUDIO_OGG) == 0)
	{
		return EXTENSION_AUDIO_OGG;
	}
	else if (stricmp(mime_type, MIME_TYPE_VIDEO_ASF) == 0)
	{
		return EXTENSION_VIDEO_ASF;
	}
	else if (stricmp(mime_type, MIME_TYPE_VIDEO_WMV) == 0)
	{
		return EXTENSION_VIDEO_WMV;
	}
	else if (stricmp(mime_type, MIME_TYPE_VIDEO_MPEG) == 0)
	{
		return EXTENSION_VIDEO_MPEG;
	}
	else if (stricmp(mime_type, MIME_TYPE_VIDEO_MOV) == 0)
	{
		return EXTENSION_VIDEO_MOV;
	}
	else if (stricmp(mime_type, MIME_TYPE_IMAGE_JPG) == 0)
	{
		return EXTENSION_IMAGE_JPG;
	}
	else if (stricmp(mime_type, MIME_TYPE_IMAGE_JPEG) == 0)
	{
		return EXTENSION_IMAGE_JPEG;
	}
	else if (stricmp(mime_type, MIME_TYPE_IMAGE_BMP) == 0)
	{
		return EXTENSION_IMAGE_BMP;
	}

	/*
	else if (strcmpi(mime_type, MIME_TYPE_PLAYLIST_M3U) == 0)
	{
		return EXTENSION_PLAYLIST_M3U;
	}
	*/

	return "application/octet-stream";
}

char* FileExtensionToUpnpClass (char* extension)
{
	if (stricmp(extension, EXTENSION_AUDIO_MPEG) == 0)
	{
		return CLASS_AUDIO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_AUDIO_WMA) == 0)
	{
		return CLASS_AUDIO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_AUDIO_OGG) == 0)
	{
		return CLASS_AUDIO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_ASF) == 0)
	{
		return CLASS_VIDEO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_WMV) == 0)
	{
		return CLASS_VIDEO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_MPEG) == 0)
	{
		return CLASS_VIDEO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_VIDEO_MOV) == 0)
	{
		return CLASS_VIDEO_ITEM;
	}
	else if (stricmp(extension, EXTENSION_IMAGE_JPG) == 0)
	{
		return CLASS_IMAGE_ITEM;
	}	
	else if (stricmp(extension, EXTENSION_IMAGE_JPEG) == 0)
	{
		return CLASS_IMAGE_ITEM;
	}	
	else if (stricmp(extension, EXTENSION_IMAGE_BMP) == 0)
	{
		return CLASS_IMAGE_ITEM;
	}	


	/*
	else if (strcmpi(extension, EXTENSION_PLAYLIST_M3U) == 0)
	{
		return CLASS_PLAYLIST_M3U;
	}
	*/


	return CLASS_ITEM;
}
