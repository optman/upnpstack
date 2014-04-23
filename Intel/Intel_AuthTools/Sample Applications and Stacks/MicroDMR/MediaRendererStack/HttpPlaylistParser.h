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
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *  
 * $Workfile: HttpPlaylistParser.h
 * $Revision:
 * $Author: Intel, DPA, Solution Architecture
 * $Date: 10/05/02
 * $Archive:
 */

#ifndef _HTTP_PLAYLIST_PARSER_H_
#define _HTTP_PLAYLIST_PARSER_H_

/* max # of bytes for a URI */
#define MAX_URI_SIZE 1024

/*
 *	Creates an instance of a playlist parser. 
 *
 *	Chain:				The thread chain where this parser belongs.
 *
 *	numberOfSockets:	Zero indicates a default value. The number of sockets
 *						largely depends on how many simultaneous HTTP requests
 *						the application wants to handle at once. 
 */
void* CreatePlaylistParser(void* Chain, int numberOfSockets);

/*
 *	Returns the result of a HttpPlaylistParser_FindTargetUri request.
 *
 *	actualIndex:		Actual index of targetUri. (itemIndex may point to something out of range)
 *
 *	targetUri:			Uri of the target specified. Caller must free this memory.
 *						Value is null if target was not found.
 *
 *	duration:			duration in seconds, as specified in the #EXTINF line before the targetUri.
 *						The value is -1 if duration is unknown or was not present in the M3U.
 *
 *	comment:			Comment portion of the #EXTINF line before the targetUri line.
 *						Caller must free this memory when no longer needed.
 */
typedef void (*HttpPlaylistParser_Callback_OnResult_FindTargetUri) (void *parserObject, int wrapAround, const char* playlistUri, int itemIndex, void *userObject, /*OUT*/ int actualIndex, /*OUT - MUST COPY THIS*/ const char *targetUri, /*OUT*/int duration, /*OUT - MUST COPY THIS*/ const char* comment);

/*
 *	Asynchronously reports the maximum number of tracks as they are discovered.
 *	
 *	maxIndexNotKnown:	Nonzero indicates that the last item index for the playlist uri is not yet known.
 *
 *	itemCount:			Number of items known to exist in the M3U.
 */
typedef void (*HttpPlaylistParser_Callback_OnUpdate_ItemCount) (void *parserObject, const char* playlistUri, void* userObject, /*OUT*/int maxIndexNotKnown, /*OUT*/int itemCount);

/*
 *	This function pointer type is used to asynchronously notify that a specified
 *	playlistUri exists. 
 *
 *	uriExists:			nonzero indicates the URI exists
 *
 *	Method implementations intended to be used as a callback must not block or
 *	execute Playlist_FindTargetUri().
 */
typedef void (*HttpPlaylistParser_Callback_PlaylistUriExists) (void* parserObject, const char* playlistUri, void* userObject, int uriExists);


/*
 *	Requests the parser to obtain the item URI for the specified playlist and item index.
 *
 *	If the caller issues request before the CallbackFoundTarget
 *	is executed, then the caller won't get a result callback for the earlier request.
 *	This design decision was chosen largely because the overhead needed to track simultaneous
 *	requests
 *
 *	parserObject:		The void* from CreatePlaylistParser().
 *
 *	playlistUri:		The URI where the M3U can be downloaded.
 *
 *	itemIndex:			The item that should be returned, identified by its position within the M3U.
 *
 *	wrapAround:			If itemIndex is out of range and wrapAround is nonzero, then an appropriate
 *						track from somewhere in the M3U will be selected. Otherwise, the last track
 *						is selected as the output.
 *
 *	userObject:			Application-specified object that can be used to key this request, or used
 *						as an input parameter for the asynchronous result callback.
 */
void HttpPlaylistParser_FindTargetUri
	(
	void *parserObject, 
	int wrapAround, 
	const char* playlistUri, 
	int itemIndex, 
	void *userObject,
	HttpPlaylistParser_Callback_PlaylistUriExists CallbackUriExists,
	HttpPlaylistParser_Callback_OnUpdate_ItemCount CallbackItemCount,
	HttpPlaylistParser_Callback_OnResult_FindTargetUri CallbackFoundTarget
	);

#endif
