// Remote IO ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Remote IO Server.h"
#include "Remote IO ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{
	#include "ILibParsers.h"
	#include "RemoteIOControl.h"
	#include "RIOServer.h"
	#include "MyString.h"
}

static HWND	hWndMainWindow = NULL;
unsigned long WINAPI UPnPMain(void* ptr);
CRemoteIOServerDlg* MainDialog = NULL;
CImageList* MediaImageList = NULL;
void *RemoteIOServer;
void *ThreadChain;

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOServerDlg dialog

CRemoteIOServerDlg::CRemoteIOServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteIOServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoteIOServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteIOServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteIOServerDlg)
	DDX_Control(pDX, IDC_CLIENTLIST, ClientList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRemoteIOServerDlg, CDialog)
	//{{AFX_MSG_MAP(CRemoteIOServerDlg)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_CONNECT, OnFileConnect)
	ON_COMMAND(ID_FILE_DISCONNECT, OnFileDisconnect)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_FORCECONNECT, OnFileForceconnect)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOServerDlg message handlers

BOOL CRemoteIOServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// Extra initialization here
	CCeCommandBar *pCommandBar = (CCeCommandBar*)m_pWndEmptyCB;
	pCommandBar->InsertMenuBar(IDR_MAINMENU);

	srand(GetTickCount());

	MediaImageList = new CImageList();
	MediaImageList->Create(16,16,0,8,8);

	CBitmap b1;
	b1.LoadBitmap(IDB_APPMATCH);
	MediaImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	b1.LoadBitmap(IDB_APPCONNECTED);
	MediaImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	b1.LoadBitmap(IDB_APPMISMATCH);
	MediaImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	ClientList.SetImageList(MediaImageList,LVSIL_SMALL);

	MainDialog = this;
	hWndMainWindow = this->GetSafeHwnd();
	//this->MoveWindow(100,100,240,320,true);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&UPnPMain, 0, 0, NULL );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void RemoteIOServerCallbackSink_ClientAdded(struct RemoteIODevice* device)
{
	int i=0,j;
	wchar_t str[202];

	if (device->RIOServeApplication == 0) i = 2; 
	Utf8ToWide((wchar_t*)str,device->FriendlyName,100);
	j = MainDialog->ClientList.InsertItem(0,str,i);
	MainDialog->ClientList.SetItemData(j,(DWORD_PTR)device);
}

void RemoteIOServerCallbackSink_ClientRemoved(struct RemoteIODevice* device)
{
	int j = 0;
	while (MainDialog->ClientList.GetItemCount() > j)
	{
		if (device == (struct RemoteIODevice*)MainDialog->ClientList.GetItemData(j))
		{
			MainDialog->ClientList.DeleteItem(j);
			break;
		}
		j++;
	}
}

void RemoteIOServerCallbackSink_CheckMatch(struct RemoteIODevice* device, int* match)
{
	*match = 1;
	//if (device->DisplayWidth == 240 && device->DisplayHeight == 320) *match = 1;
}

void RemoteIOServerCallbackSink_Connection(struct RemoteIODevice* device)
{
	int j = 0;
	wchar_t str[202];

	while (MainDialog->ClientList.GetItemCount() > j)
	{
		if (device == (struct RemoteIODevice*)MainDialog->ClientList.GetItemData(j))
		{
			MainDialog->ClientList.DeleteItem(j);
			Utf8ToWide((wchar_t*)str,device->FriendlyName,100);
			j = MainDialog->ClientList.InsertItem(0,str,1);
			MainDialog->ClientList.SetItemData(j,(DWORD_PTR)device);
			break;
		}
		j++;
	}
}

void RemoteIOServerCallbackSink_Disconnection(struct RemoteIODevice* device)
{
	int i=0,j=0;
	wchar_t str[202];

	while (MainDialog->ClientList.GetItemCount() > j)
	{
		if (device == (struct RemoteIODevice*)MainDialog->ClientList.GetItemData(j))
		{
			MainDialog->ClientList.DeleteItem(j);
			if (device->RIOServeApplication == 0) i = 2; 
			Utf8ToWide((wchar_t*)str,device->FriendlyName,100);
			j = MainDialog->ClientList.InsertItem(0,str,i);
			MainDialog->ClientList.SetItemData(j,(DWORD_PTR)device);
			break;
		}
		j++;
	}
}

void DrawRandomBox(struct RemoteIODevice* device)
{
	int x = rand() % (device->DisplayWidth - 15);
	int y = rand() % (device->DisplayHeight - 15);
	int w = rand() % (device->DisplayWidth - x);
	int h = rand() % (device->DisplayHeight - y);
	unsigned char r = rand() % 255;
	unsigned char g = rand() % 255;
	unsigned char b = rand() % 255;
	RemoteIOServerSend_DRAWFILLBOX(device,x,y,w,h,r,g,b);
}

void RemoteIOServerCallbackSink_Command(struct RemoteIODevice* device, int command, char* data, int datalength)
{
	RIO_COMMAND_MOUSEINPUT* MouseInput;
	int key,i;

	switch (command)
	{
		case RIO_RESET:
			printf("RemoteIO RESET Command\r\n");
			// TODO: Debug only, do nothing 
			break;
		case RIO_REPAINT:
			printf("RemoteIO REPAINT Command\r\n");
			// TODO: Send the complete screen back to the client
			break;
		case RIO_KEY_DOWN:
			printf("RemoteIO KEY_DOWN Command\r\n");
			key = *((int*)data);
			// TODO: Process user key input
			break;
		case RIO_KEY_UP:
			printf("RemoteIO KEY_UP Command\r\n");
			key = *((int*)data);
			// TODO: Process user key input
			break;
		case RIO_KEY_PRESS:
			printf("RemoteIO KEY_PRESS Command\r\n");
			key = *((int*)data);
			// TODO: Process user key input

			// Draw 100 fill boxes on the display
			for (i=0;i<100;i++) DrawRandomBox(device);

			break;
		case RIO_MOUSE_DOWN:
			printf("RemoteIO MOUSE_DOWN Command\r\n");
			MouseInput = (RIO_COMMAND_MOUSEINPUT*)data;
			// TODO: Process user mouse input
			break;
		case RIO_MOUSE_UP:
			printf("RemoteIO MOUSE_UP Command\r\n");
			MouseInput = (RIO_COMMAND_MOUSEINPUT*)data;
			// TODO: Process user mouse input
			break;
		default:
			printf("RemoteIOCommand: Command = %d, DataLength = %d\r\n",command,datalength);
			// TODO: Do nothing. Used for custom commands such as an LCD panel, LED's and more.
			break;
	}

}

void RemoteIOServerCallbackSink_CommandUnderFlow(struct RemoteIODevice* device)
{
	
}

unsigned long WINAPI UPnPMain(void* ptr)
{
	RemoteIOServerCallback_ClientAdded       = &RemoteIOServerCallbackSink_ClientAdded;
	RemoteIOServerCallback_ClientRemoved     = &RemoteIOServerCallbackSink_ClientRemoved;
	RemoteIOServerCallback_CheckMatch        = &RemoteIOServerCallbackSink_CheckMatch;
	RemoteIOServerCallback_Connection        = &RemoteIOServerCallbackSink_Connection;
	RemoteIOServerCallback_Disconnection     = &RemoteIOServerCallbackSink_Disconnection;
	RemoteIOServerCallback_Command           = &RemoteIOServerCallbackSink_Command;
	RemoteIOServerCallback_CommandUnderFlow  = &RemoteIOServerCallbackSink_CommandUnderFlow;

	ThreadChain = ILibCreateChain();
	RemoteIOServer = RemoteIOServerCreate(ThreadChain,"Test Application",555);
	ILibStartChain(ThreadChain);
	ThreadChain = NULL;

	PostMessage(hWndMainWindow, WM_CLOSE, 0, 0);
	return 0;
}

void CRemoteIOServerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (ClientList.GetSafeHwnd() != 0)
	{
		RECT clientRect;
		this->GetClientRect(&clientRect);
		ClientList.MoveWindow(&clientRect,true);
	}
}

void CRemoteIOServerDlg::OnClose() 
{
	if (ThreadChain == NULL)
	{
		delete MediaImageList;
		CDialog::OnClose();
	}
	else
	{
		ILibStopChain(ThreadChain);
	}
}

void CRemoteIOServerDlg::OnFileConnect() 
{
	int index;
	POSITION pos;
	struct RemoteIODevice* device;

	pos = ClientList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	index = ClientList.GetNextSelectedItem(pos);
	device = (struct RemoteIODevice*)ClientList.GetItemData(index);
	if (device == NULL) return;
	RemoteIOServer_ConnectInterlock(device);	
}

void CRemoteIOServerDlg::OnFileDisconnect() 
{
	int index;
	POSITION pos;
	struct RemoteIODevice* device;

	pos = ClientList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	index = ClientList.GetNextSelectedItem(pos);
	device = (struct RemoteIODevice*)ClientList.GetItemData(index);
	if (device == NULL) return;
	RemoteIOServer_Disconnect(device);	
}

void CRemoteIOServerDlg::OnFileExit() 
{
	ILibStopChain(ThreadChain);
	
}

void CRemoteIOServerDlg::OnFileForceconnect() 
{
	int index;
	POSITION pos;
	struct RemoteIODevice* device;

	pos = ClientList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	index = ClientList.GetNextSelectedItem(pos);
	device = (struct RemoteIODevice*)ClientList.GetItemData(index);
	if (device == NULL) return;
	RemoteIOServer_ConnectOverride(device);
}

void CRemoteIOServerDlg::OnFileReset() 
{
	int index;
	POSITION pos;
	struct RemoteIODevice* device;

	pos = ClientList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	index = ClientList.GetNextSelectedItem(pos);
	device = (struct RemoteIODevice*)ClientList.GetItemData(index);
	if (device == NULL) return;
	RemoteIO_Reset(device);
}