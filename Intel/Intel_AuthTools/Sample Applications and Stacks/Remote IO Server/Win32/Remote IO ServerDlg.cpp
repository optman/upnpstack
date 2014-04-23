// Remote IO ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Remote IO Server.h"
#include "Remote IO ServerDlg.h"

extern "C"
{
	#include "ILibParsers.h"
	#include "RemoteIOControl.h"
	#include "RIOServer.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HWND	hWndMainWindow = NULL;
unsigned long WINAPI UPnPMain(void* ptr);
CRemoteIOServerDlg* MainDialog = NULL;
CImageList* MediaImageList = NULL;
void *RemoteIOServer;
void *ThreadChain;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRemoteIOServerDlg dialog



CRemoteIOServerDlg::CRemoteIOServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteIOServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
}

void CRemoteIOServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAINLIST, ClientList);
}

BEGIN_MESSAGE_MAP(CRemoteIOServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_FORCECONNECT, OnFileForceconnect)
	ON_COMMAND(ID_FILE_CONNECT, OnFileConnect)
	ON_COMMAND(ID_FILE_DISCONNECT, OnFileDisconnect)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
END_MESSAGE_MAP()


// CRemoteIOServerDlg message handlers

BOOL CRemoteIOServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Extra initialization here
	srand((unsigned)time(NULL));

	MediaImageList = new CImageList();
	MediaImageList->Create(16,16,ILC_COLOR16,8,8);

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
	this->MoveWindow(100,100,240,320,true);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&UPnPMain, 0, 0, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteIOServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteIOServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteIOServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void RemoteIOServerCallbackSink_ClientAdded(struct RemoteIODevice* device)
{
	int i=0,j;
	if (device->RIOServeApplication == 0) i = 2; 
	j = MainDialog->ClientList.InsertItem(0,device->FriendlyName,i);
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
	while (MainDialog->ClientList.GetItemCount() > j)
	{
		if (device == (struct RemoteIODevice*)MainDialog->ClientList.GetItemData(j))
		{
			MainDialog->ClientList.DeleteItem(j);
			j = MainDialog->ClientList.InsertItem(0,device->FriendlyName,1);
			MainDialog->ClientList.SetItemData(j,(DWORD_PTR)device);
			break;
		}
		j++;
	}
}

void RemoteIOServerCallbackSink_Disconnection(struct RemoteIODevice* device)
{
	int i=0,j=0;
	while (MainDialog->ClientList.GetItemCount() > j)
	{
		if (device == (struct RemoteIODevice*)MainDialog->ClientList.GetItemData(j))
		{
			MainDialog->ClientList.DeleteItem(j);
			if (device->RIOServeApplication == 0) i = 2; 
			j = MainDialog->ClientList.InsertItem(0,device->FriendlyName,i);
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
	int key;

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
			for (int i=0;i<100;i++) DrawRandomBox(device);
			//RemoteIOServerSend_DRAWFILLBOX(device,10,10,50,50,255,0,0);
			//RemoteIOServerSend_RESET(device);

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

void CRemoteIOServerDlg::OnFileExit()
{
	ILibStopChain(ThreadChain);
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
