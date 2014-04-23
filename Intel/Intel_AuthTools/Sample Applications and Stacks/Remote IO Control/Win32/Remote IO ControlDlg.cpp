// Remote IO ControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Remote IO Control.h"
#include "Remote IO ControlDlg.h"

extern "C"
{
	#include "RemoteIOControl.h"
	#include "ILibParsers.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HWND	hWndMainWindow = NULL;
unsigned long WINAPI UPnPMain(void* ptr);
CRemoteIOControlDlg* MainDialog = NULL;
HTREEITEM MainRootNode = NULL;
void *RemoteIOControl;
void *ThreadChain;
CImageList* TreeImageList;

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


// CRemoteIOControlDlg dialog



CRemoteIOControlDlg::CRemoteIOControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteIOControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_RIOCONTROL);
}

void CRemoteIOControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICETREE, DeviceTree);
	DDX_Control(pDX, IDC_STATIC1, StaticText1);
	DDX_Control(pDX, IDC_STATIC2, StaticText2);
	DDX_Control(pDX, IDC_STATIC3, StaticText3);
}

BEGIN_MESSAGE_MAP(CRemoteIOControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_DEVICETREE, OnTvnSelchangedDevicetree)
	ON_NOTIFY(NM_DBLCLK, IDC_DEVICETREE, OnNMDblclkDevicetree)
END_MESSAGE_MAP()


// CRemoteIOControlDlg message handlers

BOOL CRemoteIOControlDlg::OnInitDialog()
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
	MainDialog = this;

	TreeImageList = new CImageList();
	TreeImageList->Create(16,16,ILC_COLOR16,8,8);

	CBitmap b1;
	b1.LoadBitmap(IDB_COMPUTER);
	TreeImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	b1.LoadBitmap(IDB_DEVICE);
	TreeImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	b1.LoadBitmap(IDB_CHANNEL);
	TreeImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	b1.LoadBitmap(IDB_CHANNELCONNECTED);
	TreeImageList->Add(&b1,RGB(0,0,0));
	b1.DeleteObject();

	DeviceTree.SetImageList(TreeImageList,TVSIL_NORMAL);

	MainRootNode = DeviceTree.InsertItem("Remote I/O Clients",0,0,TVI_ROOT,TVI_LAST);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&UPnPMain, 0, 0, NULL );

	hWndMainWindow = this->GetSafeHwnd();

	this->MoveWindow(100,100,240,320,true);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteIOControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRemoteIOControlDlg::OnPaint() 
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
HCURSOR CRemoteIOControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void RemoteIOCallback_ClientAdded(struct RemoteIODevice* device)
{
	int i, icon;
	HTREEITEM DeviceTreeItem = (HTREEITEM)device->Tag;
	HTREEITEM TreeItem1;

	device->Tag = MainDialog->DeviceTree.InsertItem(device->FriendlyName,1,1,MainRootNode,TVI_LAST);
	MainDialog->DeviceTree.SetItemData((HTREEITEM)device->Tag,(DWORD_PTR)(device));
	
	for(i=0;i<(device->ChannelCount);i++)
	{
		icon = 2;
		if (device->PeerConnection != NULL && strcmp(device->Channels[i]->URI,device->PeerConnection) == 0) icon = 3;
		TreeItem1 = MainDialog->DeviceTree.InsertItem(device->Channels[i]->Name,icon,icon,(HTREEITEM)device->Tag,TVI_LAST);
		MainDialog->DeviceTree.SetItemData(TreeItem1,(DWORD_PTR)(device->Channels[i]->URI));
	}

	MainDialog->DeviceTree.Expand(MainRootNode,TVE_EXPAND);	
}

void RemoteIOCallback_ClientRemoved(struct RemoteIODevice* device)
{
	MainDialog->DeviceTree.DeleteItem((HTREEITEM)device->Tag);
}

void RemoteIOCallback_ChannelsChanged(struct RemoteIODevice* device)
{
	int i,icon;
	HTREEITEM DeviceTreeItem = (HTREEITEM)device->Tag;
	HTREEITEM TreeItem1;

	MainDialog->DeviceTree.ShowWindow(SW_HIDE);

	if (DeviceTreeItem != NULL)
	{
		TreeItem1 = MainDialog->DeviceTree.GetChildItem(DeviceTreeItem);
		while (TreeItem1 != NULL)
		{
			MainDialog->DeviceTree.DeleteItem(TreeItem1);
			TreeItem1 = MainDialog->DeviceTree.GetChildItem(DeviceTreeItem);
		}
	}

	for(i=0;i<(device->ChannelCount);i++)
	{
		icon = 2;
		if (device->PeerConnection != NULL && strcmp(device->Channels[i]->URI,device->PeerConnection) == 0) icon = 3;
		TreeItem1 = MainDialog->DeviceTree.InsertItem(device->Channels[i]->Name,icon,icon,DeviceTreeItem,TVI_LAST);
		MainDialog->DeviceTree.SetItemData(TreeItem1,(DWORD_PTR)(device->Channels[i]->URI));
	}

	MainDialog->DeviceTree.ShowWindow(SW_SHOWNA);
}

void RemoteIOCallback_StateChanged(struct RemoteIODevice* device)
{
	RemoteIOCallback_ChannelsChanged(device);
}

unsigned long WINAPI UPnPMain(void* ptr)
{
	RemoteIOEventCallback_ClientAdded=&RemoteIOCallback_ClientAdded;
	RemoteIOEventCallback_ClientRemoved=&RemoteIOCallback_ClientRemoved;
	RemoteIOEventCallback_StateChanged=&RemoteIOCallback_StateChanged;
	RemoteIOEventCallback_ChannelsChanged=&RemoteIOCallback_ChannelsChanged;

	ThreadChain = ILibCreateChain();
	RemoteIOControl = RemoteIOCreateControl(ThreadChain);
	ILibStartChain(ThreadChain);
	ThreadChain = NULL;

	SendMessage(hWndMainWindow, WM_CLOSE, 0, 0);
	return 0;
}

void CRemoteIOControlDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (DeviceTree.GetSafeHwnd() != 0)
	{
		RECT clientRect;
		this->GetClientRect(&clientRect);
		clientRect.bottom -= 42;
		DeviceTree.MoveWindow(&clientRect,false);

		clientRect.left += 2;
		clientRect.right -= 2;
		clientRect.top = clientRect.bottom + 2;
		clientRect.bottom = clientRect.top + 13;
		this->StaticText1.MoveWindow(&clientRect,false);

		clientRect.bottom += 13;
		clientRect.top += 13;
		this->StaticText2.MoveWindow(&clientRect,false);

		clientRect.bottom += 13;
		clientRect.top += 13;
		this->StaticText3.MoveWindow(&clientRect,false);

		this->Invalidate(false);
	}
}

void CRemoteIOControlDlg::OnClose()
{
	if (ThreadChain == NULL)
	{
		// Uninitialize here
		delete TreeImageList;

		CDialog::OnClose();
	}
	else
	{
		ILibStopChain(ThreadChain);
	}
}

void CRemoteIOControlDlg::OnTvnSelchangedDevicetree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	void* itemdata1;
	void* itemdata2;
	char* temp;
	int i;
	HTREEITEM selecteditem,parentitem;
	struct RemoteIODevice* device;

	*pResult = 0;
	selecteditem = MainDialog->DeviceTree.GetSelectedItem();
	if (selecteditem == NULL)
	{
		// Dealing with an error or no selection
		StaticText1.SetWindowText("");
		StaticText2.SetWindowText("");
		StaticText3.SetWindowText("");
		return;
	}

	itemdata1 = (void*)(MainDialog->DeviceTree.GetItemData(selecteditem));
	if (itemdata1 == NULL)
	{
		// Dealing with tree root node
		StaticText1.SetWindowText("Remote I/O Control");
		StaticText2.SetWindowText("Intel Labs, www.intel.com");
		StaticText3.SetWindowText("");
		return;
	}

	parentitem = MainDialog->DeviceTree.GetParentItem(selecteditem);
	if (parentitem == NULL)
	{
		// Dealing with an error
		StaticText1.SetWindowText("");
		StaticText2.SetWindowText("");
		StaticText3.SetWindowText("");
		return;
	}

	itemdata2 = (void*)(MainDialog->DeviceTree.GetItemData(parentitem));
	if (itemdata2 == NULL)
	{
		// Dealing with a Remote I/O Client device
		device = (struct RemoteIODevice*)itemdata1;
		temp = (char*)MALLOC(300);
		sprintf(temp,"Display: %dx%d, Encoding %d",device->DisplayWidth,device->DisplayHeight,device->DisplayEncoding);
		StaticText1.SetWindowText(device->FriendlyName);
		StaticText2.SetWindowText(temp);
		StaticText3.SetWindowText(device->PeerConnection);
		FREE(temp);
		return;
	}

	if (itemdata2 != NULL)
	{
		// Dealing with a Remote I/O channel
		device = (struct RemoteIODevice*)itemdata2;
		temp = (char*)itemdata1;

		for(i=0;i<(device->ChannelCount);i++)
		{
			if (device->Channels[i]->URI == temp)
			{
				
				StaticText1.SetWindowText(device->Channels[i]->Name);
				StaticText2.SetWindowText(device->Channels[i]->URI);
				StaticText3.SetWindowText("");
				return;
			}
		}

		StaticText1.SetWindowText("Unknown Remote I/O channel");
		StaticText2.SetWindowText("");
		StaticText3.SetWindowText("");

		return;
	}
}

void CRemoteIOControlDlg::OnNMDblclkDevicetree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM selecteditem,parentitem;
	struct RemoteIODevice* device;
	char* channel;

	*pResult = 0;

	selecteditem = MainDialog->DeviceTree.GetSelectedItem();
	if (selecteditem == NULL) return;
	parentitem = MainDialog->DeviceTree.GetParentItem(selecteditem);
	if (parentitem == NULL) return;
	device = (struct RemoteIODevice*)(MainDialog->DeviceTree.GetItemData(parentitem));
	if (device == NULL) return;
	channel = (char*)(MainDialog->DeviceTree.GetItemData(selecteditem));
	if (channel == NULL) return;

	RemoteIO_ConnectOverride(device,channel);
}
