<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: MediaBrowserPPC - Win32 (WCE ARM) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP12F.tmp" with contents
[
/nologo /W3 /I "..\\" /D "ARM" /D "_ARM_" /D "NDEBUG" /D "MICROSTACK_NO_STDAFX" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC=310" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /D "MMSCP_LEAN_AND_MEAN" /Fo"ARMRel/" /Oxs /MC /c 
"C:\MicroStack\Micro Media Browser\MediaBrowserPPC\MSCPControlPoint.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP12F.tmp" 
Creating temporary file "C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP130.tmp" with contents
[
/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:no /pdb:"ARMRel/Media Browser.pdb" /out:"ARMRel/Media Browser.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
".\ARMRel\ILibHTTPClient.obj"
".\ARMRel\ILibMiniWebServer.obj"
".\ARMRel\ILibParsers.obj"
".\ARMRel\ILibSSDPClient.obj"
".\ARMRel\MediaBrowserPPC.obj"
".\ARMRel\MediaBrowserPPCDlg.obj"
".\ARMRel\MmsCp.obj"
".\ARMRel\MSCPControlPoint.obj"
".\ARMRel\MyString.obj"
".\ARMRel\StdAfx.obj"
".\ARMRel\MediaBrowserPPC.res"
".\winsock_arm.lib"
]
Creating command line "link.exe @C:\DOCUME~1\YSAINTHI.AMR\LOCALS~1\Temp\RSP130.tmp"
<h3>Output Window</h3>
Compiling...
MSCPControlPoint.c
Linking...



<h3>Results</h3>
Media Browser.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
