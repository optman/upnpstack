<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Renderer - Win32 (WCE ARM) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP699.tmp" with contents
[
/nologo /W3 /Zi /Od /I "..\PocketPC_MmrRslMp" /I "..\DeviceBuilder\PocketPC" /I "..\\" /D "DEBUG" /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC=310" /D "UNICODE" /D "_UNICODE" /D "MICROSTACK_NO_STDAFX" /FR"ARMDbg/" /Fo"ARMDbg/" /Fd"ARMDbg/" /MC /c 
"C:\cygwin\home\Administrator\MicroAvRenderer\MediaPlayerVersions_Methods.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP699.tmp" 
Creating temporary file "C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP69A.tmp" with contents
[
commctrl.lib coredll.lib aygshell.lib atlce300.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMDbg/Media Renderer.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /nodefaultlib:oldnames.lib" /out:"ARMDbg/Media Renderer.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
.\ARMDbg\CMicroMediaRenderer.obj
.\ARMDbg\HttpPlaylistParser.obj
.\ARMDbg\ILibHTTPClient.obj
.\ARMDbg\ILibParsers.obj
.\ARMDbg\MediaPlayerVersions_Methods.obj
.\ARMDbg\MicroMediaRenderer.obj
.\ARMDbg\MicroMediaRendererApp.obj
.\ARMDbg\MyString.obj
.\ARMDbg\RendererStateLogic.obj
.\ARMDbg\stdafx.obj
.\ARMDbg\UpnpMicroStack.obj
.\ARMDbg\MicroMediaRendererApp.res
.\winsock_arm.lib
.\winsock_x86.lib
]
Creating command line "link.exe @C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP69A.tmp"
<h3>Output Window</h3>
Compiling...
MediaPlayerVersions_Methods.cpp
Linking...
Creating temporary file "C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP69D.tmp" with contents
[
/nologo /o"ARMDbg/MicroMediaRendererApp.bsc" 
.\ARMDbg\CMicroMediaRenderer.sbr
.\ARMDbg\HttpPlaylistParser.sbr
.\ARMDbg\ILibHTTPClient.sbr
.\ARMDbg\ILibParsers.sbr
.\ARMDbg\MediaPlayerVersions_Methods.sbr
.\ARMDbg\MicroMediaRenderer.sbr
.\ARMDbg\MicroMediaRendererApp.sbr
.\ARMDbg\MyString.sbr
.\ARMDbg\RendererStateLogic.sbr
.\ARMDbg\stdafx.sbr
.\ARMDbg\UpnpMicroStack.sbr]
Creating command line "bscmake.exe @C:\DOCUME~1\nkidd\LOCALS~1\Temp\RSP69D.tmp"
Creating browse info file...
<h3>Output Window</h3>



<h3>Results</h3>
Media Renderer.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
