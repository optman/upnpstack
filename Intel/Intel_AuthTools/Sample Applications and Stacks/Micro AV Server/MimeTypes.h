#ifndef MIMETYPES_H
/*
 *	Provides forward declarations for methods in
 *	MimeTypes.c
 */

#define CLASS_ITEM			"object.item"
#define CLASS_AUDIO_ITEM	"object.item.audioItem"
#define CLASS_PLAYLIST_M3U	"object.item.playlistItem"
#define CLASS_VIDEO_ITEM	"object.item.videoItem"
#define CLASS_IMAGE_ITEM	"object.item.imageItem"


#define EXTENSION_AUDIO_MPEG		".mp3"
#define MIME_TYPE_AUDIO_MPEG		"audio/mpeg"

#define EXTENSION_AUDIO_WMA			".wma"
#define MIME_TYPE_AUDIO_WMA			"audio/x-ms-wma"

#define EXTENSION_AUDIO_OGG			".ogg"
#define MIME_TYPE_AUDIO_OGG			"audio/x-ogg"

#define EXTENSION_VIDEO_ASF			".asf"
#define MIME_TYPE_VIDEO_ASF			"video/x-ms-asf"

#define EXTENSION_VIDEO_WMV			".wmv"
#define MIME_TYPE_VIDEO_WMV			"video/x-ms-wmv"

#define EXTENSION_VIDEO_MPEG		".mpg"
#define MIME_TYPE_VIDEO_MPEG		"video/mpeg"

#define EXTENSION_VIDEO_MOV			".mov"
#define MIME_TYPE_VIDEO_MOV			"video/quicktime"

#define EXTENSION_IMAGE_JPG			".jpg"
#define MIME_TYPE_IMAGE_JPG			"image/jpeg"

#define EXTENSION_IMAGE_JPEG		".jpeg"
#define MIME_TYPE_IMAGE_JPEG		"image/jpeg"

#define EXTENSION_IMAGE_BMP			".bmp"
#define MIME_TYPE_IMAGE_BMP			"image/bmp"

 /*
#define EXTENSION_PLAYLIST_M3U		".m3u"
#define MIME_TYPE_PLAYLIST_M3U		"audio/x-mpegurl"
*/

/*
 *	GetMimeType()
 *		extension				: the file extension, including the dot '.' char
 *
 *	Returns the mime-type of a file with the given file extension.
 *	The method returns static values.
 *	Returns NULL if mapping cannot be determined.
 *	DO NOT CALL FREE ON THE RETURNED VALUE.
 */
char* FileExtensionToMimeType (char* extension);

/*
 *	GetFileExtension()
 *		mime_type				: the mime-type
 *
 *	Returns the file extension of a file with the given mime type.
 *	The method returns static values.
 *	Returns NULL if mapping cannot be determined.
 *	DO NOT CALL FREE ON THE RETURNED VALUE.
 */
char* MimeTypeToFileExtension (char* mime_type);

/*
 *	GetUpnpClass()
 *		extension				: the file extension, including the dot '.' char
 *
 *	Returns the upnp:class of a file with the given file extension.
 *	The method returns static values.
 *	Returns NULL if mapping cannot be determined.
 *	DO NOT CALL FREE ON THE RETURNED VALUE.
 */
char* FileExtensionToUpnpClass (char* extension);

#endif
