<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Remote IO Server - Win32 (WCE ARM) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP78F.tmp" with contents
[
/nologo /W3 /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC=310" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_AFXDLL" /D "MICROSTACK_NO_STDAFX" /Fo"ARMRel/" /Oxs /MC /c 
"C:\MicroStack\Remote IO Server\PocketPC\ILibAsyncServerSocket.c"
"C:\MicroStack\Remote IO Server\PocketPC\ILibAsyncSocket.c"
"C:\MicroStack\Remote IO Server\PocketPC\ILibHTTPClient.c"
"C:\MicroStack\Remote IO Server\PocketPC\ILibMiniWebServer.c"
"C:\MicroStack\Remote IO Server\PocketPC\ILibParsers.c"
"C:\MicroStack\Remote IO Server\PocketPC\ILibSSDPClient.c"
"C:\MicroStack\Remote IO Server\PocketPC\Remote IO ServerDlg.cpp"
"C:\MicroStack\Remote IO Server\PocketPC\RemoteIOControl.c"
"C:\MicroStack\Remote IO Server\PocketPC\RIOControlPoint.c"
"C:\MicroStack\Remote IO Server\PocketPC\RIOServer.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP78F.tmp" 
Creating temporary file "C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP790.tmp" with contents
[
/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:no /pdb:"ARMRel/Remote IO Server.pdb" /out:"ARMRel/Remote IO Server.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
".\ARMRel\ILibAsyncServerSocket.obj"
".\ARMRel\ILibAsyncSocket.obj"
".\ARMRel\ILibHTTPClient.obj"
".\ARMRel\ILibMiniWebServer.obj"
".\ARMRel\ILibParsers.obj"
".\ARMRel\ILibSSDPClient.obj"
".\ARMRel\MyString.obj"
".\ARMRel\Remote IO Server.obj"
".\ARMRel\Remote IO ServerDlg.obj"
".\ARMRel\RemoteIOControl.obj"
".\ARMRel\RIOControlPoint.obj"
".\ARMRel\RIOServer.obj"
".\ARMRel\StdAfx.obj"
".\ARMRel\Remote IO Server.res"
".\winsock_arm.lib"
]
Creating command line "link.exe @C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP790.tmp"
<h3>Output Window</h3>
Compiling...
ILibAsyncServerSocket.c
ILibAsyncSocket.c
ILibHTTPClient.c
ILibMiniWebServer.c
ILibParsers.c
ILibSSDPClient.c
Generating Code...
Compiling...
Remote IO ServerDlg.cpp
Generating Code...
Compiling...
RemoteIOControl.c
RIOControlPoint.c
C:\MicroStack\Remote IO Server\PocketPC\RIOControlPoint.c(2086) : warning C4018: '==' : signed/unsigned mismatch
RIOServer.c
Generating Code...
Linking...



<h3>Results</h3>
Remote IO Server.exe - 0 error(s), 1 warning(s)
</pre>
</body>
</html>
