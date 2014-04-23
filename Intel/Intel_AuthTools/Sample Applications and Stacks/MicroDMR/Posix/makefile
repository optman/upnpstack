# List all object files here
OFILES = \
	Emulator_Methods.o\
	ILibParsers.o\
	ILibHTTPClient.o\
	UpnpMicroStack.o\
	MicroMediaRenderer.o\
	CodecWrapper.o\
	RendererStateLogic.o\
	Utility.o\
	PortFn_MsTime.o\
	HttpPlaylistParser.o\
	MyString.o\
	ILibAsyncSocket.o\
	RemoteIOClientStack.o\
	MicroDmr.o 

# Compiler command name
CC = gcc

# Compiler flags applied to all files
# Optional flags: -D_VERBOSE -D_DEBUG -D_DEBUG_CODECWRAPPER -D_TEMPDEBUG -DTRACK_MALLOC_VERBOSE
# If using the sample codec-wrapper-emulator, then you need the _UNIX #define.
# -g puts debug symbols
#
CFLAGS = -Os -Wall -g -lpthread -DTRACK_MALLOC_VERBOSE -D_POSIX -D_DEBUG -D_UNIX -DEXCLUDE_MMR_PRESENTATION_REQUEST -DEXCLUDE_RIO_PRESENTATION_REQUEST -I./Posix -I../Posix -I./DeviceBuilder/Posix -I../DeviceBuilder/Posix -I./MediaRendererStack -I../MediaRendererStack -I./RemoteIOClientStack -I../RemoteIOClientStack 

# Search paths
VPATH = src: ./ ../ ../Posix ../MediaRendererStack ../RemoteIOClientStack ../DeviceBuilder/Posix 

# Name of the executable
EXENAME = MicroDMR

# Builds all object files and executable
${EXENAME} : $(OFILES)
	$(CC) $(CFLAGS) -o ${EXENAME} $(OFILES)

# Macro rule for all object files.
$(OFILES) : \
	Emulator_Methods.h\
	ILibParsers.h\
	ILibHTTPClient.h\
	UpnpMicroStack.h\
	MicroMediaRenderer.h\
	CodecWrapper.h\
	RendererStateLogic.h\
	Utility.h\
	PortFn_MsTime.h\
	HttpPlaylistParser.h\
	ILibAsyncSocket.h\
	RemoteIOClientStack.h\
	MyString.h

# Clean up
clean :
	rm -f $(OFILES) ${EXENAME} ${EXENAME}.exe
