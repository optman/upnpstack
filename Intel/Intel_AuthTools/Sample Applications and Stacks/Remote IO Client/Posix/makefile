
#
# Intel's UPnP MicroStack/1.0, Sample MakeFile */
# Intel Device Builder - Build#1.0.1067.18585
#

# List all object files here
OFILES = \
	PosixRioClient.o \
	ILibParsers.o\
	UpnpMicroStack.o\
	ILibHTTPClient.o\
	ILibAsyncSocket.o\
	RemoteIOClientStack.o

# Compiler command name
CC = gcc

# Compiler flags applied to all files
CFLAGS = -Os -Wall -D_POSIX -D_DEBUG -D_VERBOSE -g -lpthread -I./Posix -I../ -I./ -I./DeviceBuilder/Posix -I../DeviceBuilder/Posix
 
# Search paths
VPATH = src: ./ ../ ./Posix_MmrRslEc ./DeviceBuilder/Posix ../DeviceBuilder/Posix 

# Name of the executable
EXENAME = PosixRioClient

# Builds all object files and executable
${EXENAME} : $(OFILES)
	$(CC) $(CFLAGS) -o ${EXENAME} $(OFILES)

# Macro rule for all object files.
$(OFILES) : \
	ILibParsers.h\
	UpnpMicroStack.h\
	ILibHTTPClient.h\
	ILibAsyncSocket.h\
	RemoteIOClientStack.h

# Clean up
clean :
	rm -f $(OFILES) ${EXENAME} ${EXENAME}.exe
