// RemoteIOClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RemoteIOClient.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include <gx.h>
#include <winsock.h>
#include "imgdecmp.h"

extern "C"
{
	#include "..\DeviceBuilder\PocketPC\ILibParsers.h"
	#include "RemoteIOClientStack.h"
	#include "..\DeviceBuilder\PocketPC\UpnpMicroStack.h"
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;				// The current instance
HWND				g_hwndCB;				// The command bar handle
HWND				hWnd = NULL;
GXDisplayProperties g_gxdp;
GXKeyList			g_gxkl;
void*				UpnpStack = NULL;
void*				RemoteIOStack = NULL;
void*				RemoteIOStackChain = NULL;

// Forward Declarations
void RemoteIOConnectionChangedSink(char* PeerConnection);
void RemoteIOResetSink();
void RemoteIOCommandSink(unsigned short command, char* data, int datalength);
void DirectDrawRectangle(int x, int y, int w, int h, COLORREF color);

static SHACTIVATEINFO s_sai;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
HWND				CreateRpCommandBar(HWND);


int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_REMOTEIOCLIENT);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//GXCloseDisplay();
	GXCloseInput();

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REMOTEIOCLIENT));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	g_hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_REMOTEIOCLIENT, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//If it is already running, then focus on the window
	hWnd = FindWindow(szWindowClass, szTitle);	
	if (hWnd) 
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return 0;
	} 

	MyRegisterClass(hInstance, szWindowClass);
	
	RECT	rect;
	GetClientRect(hWnd, &rect);
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{	
		return FALSE;
	}


	MoveWindow(hWnd,0,0,240,320, false);
	ShowWindow(hWnd, nCmdShow);

	// Try opening the Display for Full screen access
	if (GXOpenDisplay(hWnd, GX_FULLSCREEN) == 0) exit(1);

	// Initialize the Hardware Buttons
	GXOpenInput();

	// Get the Display properties
	g_gxdp = GXGetDisplayProperties();

	// Get information about the Hardware Keys and fills
	// the g_gxkl structure
	g_gxkl = GXGetDefaultKeys(GX_NORMALKEYS);

	if (g_gxdp.cBPP == 8) exit(2);

	DirectDrawRectangle(0,0,g_gxdp.cxWidth,g_gxdp.cyHeight,0);

	return TRUE;
}

DWORD WINAPI UPnPThreadEntry(void* param)
{
	char uid[20];
	char friendlyname[100];
	int i;
	WSADATA wsaData;

	RemoteIOConnectionChanged = &RemoteIOConnectionChangedSink;
	RemoteIOReset = &RemoteIOResetSink;
	RemoteIOCommand = &RemoteIOCommandSink;

	// Generate a UDI
	srand(GetTickCount());
	for (i=0;i<19;i++) uid[i] = (rand() % 25) + 66;
	uid[19] = 0;

	// Generate the Friendly name
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {exit(1);}
	memcpy(friendlyname,"Intel MicroClient (",19);
	gethostname(friendlyname+19,70);
	memcpy(friendlyname+strlen(friendlyname),")\0",2);

	RemoteIOConnectionChanged = &RemoteIOConnectionChangedSink;
	RemoteIOReset = &RemoteIOResetSink;
	RemoteIOCommand = &RemoteIOCommandSink;

	RemoteIO_Application       = "XRT20:PocketPC";
	RemoteIO_MaxCommandSize    = 65500;
	RemoteIO_DisplayEncoding   = RemoteIO_JPEG; // other formats not supported in pocketpc
	RemoteIO_DisplayWidth      = 240;
	RemoteIO_DisplayHeight     = 320;
	RemoteIO_DeviceInformation = "";

	RemoteIOStackChain = ILibCreateChain();

	UpnpStack = UpnpCreateMicroStack(RemoteIOStackChain,friendlyname,uid,"0000001",1800,0);
	RemoteIOStack = CreateRemoteIO(RemoteIOStackChain,UpnpStack);
	ILibStartChain(RemoteIOStackChain);

	GXCloseInput();
	GXCloseDisplay();

	DestroyWindow(hWnd);

	return 0;
}

DWORD CALLBACK GetImageData(LPSTR szBuffer, DWORD dwBufferMax, LPARAM lParam )
{
	return dwBufferMax;
}

void DirectDrawImage(int x, int y, unsigned char* data, int datalength)
{
	HDC hdc, memhdc;
	HRESULT hr;
	BITMAP	bmp;
	HBITMAP m_hbitmap;
	DecompressImageInfo	dii;

	hdc = GetDC(hWnd);
	memhdc = CreateCompatibleDC(hdc);

	// Fill in the 'DecompressImageInfo' structure
	dii.dwSize = sizeof( DecompressImageInfo );		// Size of this structure
	dii.pbBuffer = data;							// Pointer to the buffer to use for data
	dii.dwBufferMax = datalength;					// Size of the buffer
	dii.dwBufferCurrent = 0;						// The amount of data which is current in the buffer
	dii.phBM = &m_hbitmap;							// Pointer to the bitmap returned (can be NULL)
	dii.ppImageRender = NULL;						// Pointer to an IImageRender object (can be NULL)
	dii.iBitDepth = GetDeviceCaps(memhdc,BITSPIXEL);// Bit depth of the output image
	dii.lParam = ( LPARAM ) NULL;					// User parameter for callback functions
	dii.hdc = memhdc;								// HDC to use for retrieving palettes
	dii.iScale = 100;								// Scale factor (1 - 100)
	dii.iMaxWidth = 240;							// Maximum width of the output image
	dii.iMaxHeight = 320;							// Maxumum height of the output image
	dii.pfnGetData = GetImageData;					// Callback function to get image data
	dii.pfnImageProgress = NULL;					// Callback function to notify caller of progress decoding the image
	dii.crTransparentOverride = ( UINT ) -1;		// If this color is not (UINT)-1, it will override the
													// transparent color in the image with this color. (GIF ONLY)
	// Process and decompress the image data
	hr = DecompressImageIndirect( &dii );

	GetObject(m_hbitmap, sizeof(BITMAP), &bmp);
	SelectObject (memhdc, m_hbitmap);
	BitBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, memhdc, 0, 0, SRCCOPY);
	
	ReleaseDC(hWnd,memhdc);
	ReleaseDC(hWnd,hdc);
	DeleteObject(m_hbitmap);
}

void DirectDrawRectangleGDI(int x, int y, int w, int h, COLORREF color)
{
	RECT rect;
	rect.left = x;
	rect.right = x+w;
	rect.top = y;
	rect.bottom = y+h;

	HDC dc = GetDC(hWnd);
	HBRUSH br = CreateSolidBrush(color);
	FillRect(dc,&rect,br);
	DeleteObject(br);
	ReleaseDC(hWnd,dc);
}

void DirectDrawRectangle(int x, int y, int w, int h, COLORREF color)
{
	unsigned short PixelCol = 0;
	if (g_gxdp.ffFormat & kfDirect565)
	{
		PixelCol = (unsigned short) ((GetRValue(color) & 0xff)<< 11 | (GetGValue(color) & 0xff) << 6 | (GetBValue(color) & 0xff));
	}
	else
	if (g_gxdp.ffFormat & kfDirect555)
	{
		PixelCol = (unsigned short) ((GetRValue(color) & 0xff)<< 10 | (GetGValue(color) & 0xff) << 5 | (GetBValue(color) & 0xff));
	}

	unsigned short * pusLine = (unsigned short *)GXBeginDraw();
	if (pusLine == NULL) return; // NOT OK TO DRAW

	pusLine += ((g_gxdp.cbxPitch >> 1) * x) + ((g_gxdp.cbyPitch >> 1) * y);

	for (int ty = y; ty < (y+h); ty++)
	{
		unsigned short * pusDest = pusLine;
		for (int tx = x;tx < (x+w); tx++)
		{
			*pusDest = PixelCol;
			pusDest += g_gxdp.cbxPitch >> 1;
		}
		pusLine += g_gxdp.cbyPitch >> 1;
	}

	GXEndDraw();
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE:
			CreateThread(NULL,NULL,&UPnPThreadEntry,NULL,0,NULL);
			//CreateThread(NULL,NULL,&DirectXThreadEntry,NULL,0,NULL);
			break;
		case WM_PAINT:
			break;
		case WM_DESTROY:
			WSACleanup();
			PostQuitMessage(0);
			break;
		case WM_KEYUP:
			if ((short)wParam != g_gxkl.vkC) RemoteIO_SendKeyUp(wParam);
			break;
		case WM_KEYDOWN:
			if ((short)wParam == g_gxkl.vkC)
			{
				PostMessage(hWnd,WM_DESTROY,0,0);
				ILibStopChain(RemoteIOStackChain);
			}
			else RemoteIO_SendKeyDown(wParam);
			break;
		case WM_LBUTTONUP:
			RemoteIO_SendMouseUp(LOWORD(lParam),HIWORD(lParam),0x100000);
			break;
		case WM_LBUTTONDOWN:
			RemoteIO_SendMouseDown(LOWORD(lParam),HIWORD(lParam),0x100000);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void RemoteIOConnectionChangedSink(char* PeerConnection)
{
	if (PeerConnection == NULL)
	{
		DirectDrawRectangle(0,0,240,320,0);
	}
}

void RemoteIOResetSink()
{
	DirectDrawRectangle(0,0,240,320,0);
}

void RemoteIOCommandSink(unsigned short command, char* data, int datalength)
{
	RIO_COMMAND_DRAWFILLBOX* cmd_fillbox;
	RIO_COMMAND_DRAWIMAGE* cmd_image;

	switch (command)
	{
		case RIO_RESET:
			RemoteIOResetSink();
			break;
		case RIO_EXIT:
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case RIO_DRAWFILLBOX:
			if (datalength != sizeof(RIO_COMMAND_DRAWFILLBOX)) break;
			cmd_fillbox = (RIO_COMMAND_DRAWFILLBOX*)data;
			DirectDrawRectangle(cmd_fillbox->x,cmd_fillbox->y,cmd_fillbox->w,cmd_fillbox->h,RGB(cmd_fillbox->r,cmd_fillbox->g,cmd_fillbox->b));
			break;
		case RIO_DRAWIMAGE:
			if (datalength < 4) break;
			cmd_image = (RIO_COMMAND_DRAWIMAGE*)data;
			DirectDrawImage(cmd_image->x,cmd_image->y,(unsigned char*)data+4,datalength-4);
			break;
	}
}

