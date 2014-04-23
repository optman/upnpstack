// Remote IO ControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Remote IO Control.h"
#include "Remote IO ControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{
	#include "RemoteIOControl.h"
	#include "ILibParsers.h"
	#include "MyString.h"
}

static HWND	hWndMainWindow = NULL;
unsigned long WINAPI UPnPMain(void* ptr);
CRemoteIOControlDlg* MainDialog = NULL;
HTREEITEM MainRootNode = NULL;
void *RemoteIOControl;
void *ThreadChain;
CImageList* TreeImageList;


/////////////////////////////////////////////////////////////////////////////
// CRemoteIOControlDlg dialog

CRemoteIOControlDlg::CRemoteIOControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteIOControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoteIOControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteIOControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteIOControlDlg)
	DDX_Control(pDX, IDC_DEVICETREE, m_DeviceTree);
	DDX_Control(pDX, IDC_STATIC3, m_StaticText3);
	DDX_Control(pDX, IDC_STATIC2, m_StaticText2);
	DDX_Control(pDX, IDC_STATIC1, m_StaticText1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRemoteIOControlDlg, CDialog)
	//{{AFX_MSG_MAP(CRemoteIOControlDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_DEVICETREE, OnDblclkDevicetree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DEVICETREE, OnSelchangedDevicetree)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteIOControlDlg message handlers

BOOL CRemoteIOControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

	// Add extra initialization here
	CCeCommandBar *pCommandBar = (CCeCommandBar*)m_pWndEmptyCB;
	pCommandBar->InsertMenuBar(IDR_MAINMENU);

	MainDialog = this;

	TreeImageList = new CImageList();
	TreeImageList->Create(16,16,1,1,1);

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

	m_DeviceTree.SetImageList(TreeImageList,TVSIL_NORMAL);

	MainRootNode = m_DeviceTree.InsertItem(TEXT("Remote I/O Clients"),0,0,TVI_ROOT,TVI_LAST);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&UPnPMain, 0, 0, NULL );

	hWndMainWindow = this->GetSafeHwnd();

	//this->MoveWindow(100,100,240,320,true);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CRemoteIOControlDlg::OnDblclkDevicetree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM selecteditem,parentitem;
	struct RemoteIODevice* device;
	char* channel;

	*pResult = 0;

	selecteditem = MainDialog->m_DeviceTree.GetSelectedItem();
	if (selecteditem == NULL) return;
	parentitem = MainDialog->m_DeviceTree.GetParentItem(selecteditem);
	if (parentitem == NULL) return;
	device = (struct RemoteIODevice*)(MainDialog->m_DeviceTree.GetItemData(parentitem));
	if (device == NULL) return;
	channel = (char*)(MainDialog->m_DeviceTree.GetItemData(selecteditem));
	if (channel == NULL) return;

	RemoteIO_ConnectOverride(device,channel);
}

void CRemoteIOControlDlg::OnSelchangedDevicetree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	void* itemdata1;
	void* itemdata2;
	char* temp;
	unsigned short wtemp[400];
	int i;
	HTREEITEM selecteditem,parentitem;
	struct RemoteIODevice* device;

	*pResult = 0;
	selecteditem = MainDialog->m_DeviceTree.GetSelectedItem();
	if (selecteditem == NULL)
	{
		// Dealing with an error or no selection
		m_StaticText1.SetWindowText(TEXT(""));
		m_StaticText2.SetWindowText(TEXT(""));
		m_StaticText3.SetWindowText(TEXT(""));
		return;
	}

	itemdata1 = (void*)(MainDialog->m_DeviceTree.GetItemData(selecteditem));
	if (itemdata1 == NULL)
	{
		// Dealing with tree root node
		m_StaticText1.SetWindowText(TEXT("Remote I/O Control"));
		m_StaticText2.SetWindowText(TEXT("Intel Labs, www.intel.com"));
		m_StaticText3.SetWindowText(TEXT(""));
		return;
	}

	parentitem = MainDialog->m_DeviceTree.GetParentItem(selecteditem);
	if (parentitem == NULL)
	{
		// Dealing with an error
		m_StaticText1.SetWindowText(TEXT(""));
		m_StaticText2.SetWindowText(TEXT(""));
		m_StaticText3.SetWindowText(TEXT(""));
		return;
	}

	itemdata2 = (void*)(MainDialog->m_DeviceTree.GetItemData(parentitem));
	if (itemdata2 == NULL)
	{
		// Dealing with a Remote I/O Client device
		device = (struct RemoteIODevice*)itemdata1;

		Utf8ToWide(wtemp,device->FriendlyName,800);
		m_StaticText1.SetWindowText(wtemp);

		swprintf(wtemp,TEXT("Display: %dx%d, Encoding %d"),device->DisplayWidth,device->DisplayHeight,device->DisplayEncoding);
		m_StaticText2.SetWindowText(wtemp);

		Utf8ToWide(wtemp,device->PeerConnection,800);
		m_StaticText3.SetWindowText(wtemp);

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
				Utf8ToWide(wtemp,device->Channels[i]->Name,800);
				m_StaticText1.SetWindowText(wtemp);
				Utf8ToWide(wtemp,device->Channels[i]->URI,800);
				m_StaticText2.SetWindowText(wtemp);
				m_StaticText3.SetWindowText(TEXT(""));
				return;
			}
		}

		m_StaticText1.SetWindowText(TEXT("Unknown Remote I/O channel"));
		m_StaticText2.SetWindowText(TEXT(""));
		m_StaticText3.SetWindowText(TEXT(""));

		return;
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

void CRemoteIOControlDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (m_DeviceTree.GetSafeHwnd() != 0)
	{
		RECT clientRect;
		this->GetClientRect(&clientRect);
		clientRect.bottom -= 42;
		m_DeviceTree.MoveWindow(&clientRect,false);

		clientRect.left += 2;
		clientRect.right -= 2;
		clientRect.top = clientRect.bottom + 2;
		clientRect.bottom = clientRect.top + 13;
		this->m_StaticText1.MoveWindow(&clientRect,false);

		clientRect.bottom += 13;
		clientRect.top += 13;
		this->m_StaticText2.MoveWindow(&clientRect,false);

		clientRect.bottom += 13;
		clientRect.top += 13;
		this->m_StaticText3.MoveWindow(&clientRect,false);

		this->Invalidate(false);
	}
}

void RemoteIOCallback_ClientAdded(struct RemoteIODevice* device)
{
	int i, icon;
	unsigned short temp[100];
	HTREEITEM DeviceTreeItem = (HTREEITEM)device->Tag;
	HTREEITEM TreeItem1;

	Utf8ToWide(temp,device->FriendlyName,200);
	device->Tag = MainDialog->m_DeviceTree.InsertItem(temp,1,1,MainRootNode,TVI_LAST);
	MainDialog->m_DeviceTree.SetItemData((HTREEITEM)device->Tag,(DWORD_PTR)(device));
	
	for(i=0;i<(device->ChannelCount);i++)
	{
		icon = 2;
		if (device->PeerConnection != NULL && strcmp(device->Channels[i]->URI,device->PeerConnection) == 0) icon = 3;
		Utf8ToWide(temp,device->Channels[i]->Name,200);
		TreeItem1 = MainDialog->m_DeviceTree.InsertItem(temp,icon,icon,(HTREEITEM)device->Tag,TVI_LAST);
		MainDialog->m_DeviceTree.SetItemData(TreeItem1,(DWORD_PTR)(device->Channels[i]->URI));
	}

	MainDialog->m_DeviceTree.Expand(MainRootNode,TVE_EXPAND);	
}

void RemoteIOCallback_ClientRemoved(struct RemoteIODevice* device)
{
	MainDialog->m_DeviceTree.DeleteItem((HTREEITEM)device->Tag);
}

void RemoteIOCallback_StateChanged(struct RemoteIODevice* device)
{

}

void RemoteIOCallback_ChannelsChanged(struct RemoteIODevice* device)
{
	int i,icon;
	unsigned short temp[100];
	HTREEITEM DeviceTreeItem = (HTREEITEM)device->Tag;
	HTREEITEM TreeItem1, TreeItem2;

	TreeItem1 = MainDialog->m_DeviceTree.GetNextSiblingItem(DeviceTreeItem);
	while (TreeItem1 != NULL)
	{
		TreeItem2 = MainDialog->m_DeviceTree.GetNextItem(TreeItem1,TVGN_NEXT);
		MainDialog->m_DeviceTree.DeleteItem(TreeItem1);
		TreeItem1 = TreeItem2;
	}

	for(i=0;i<(device->ChannelCount);i++)
	{
		icon = 2;
		if (device->PeerConnection != NULL && strcmp(device->Channels[i]->URI,device->PeerConnection) == 0) icon = 3;
		Utf8ToWide(temp,device->Channels[i]->Name,200);
		TreeItem1 = MainDialog->m_DeviceTree.InsertItem(temp,icon,icon,DeviceTreeItem,TVI_LAST);
		MainDialog->m_DeviceTree.SetItemData(TreeItem1,(DWORD_PTR)(device->Channels[i]->URI));
	}
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

void CRemoteIOControlDlg::OnFileExit() 
{
	this->DestroyWindow();
}
