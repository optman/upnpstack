using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Reflection;
using System.Runtime.InteropServices;
using UPnPStack.CP;
using log4net;

namespace DeviceSpy
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private static readonly ILog log=LogManager.GetLogger(typeof(MainForm));

		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.Splitter splitter1;
		private System.Windows.Forms.MenuItem ExitItem;
		private System.Windows.Forms.MenuItem RescanNetworkItem;
		private System.Windows.Forms.MenuItem AboutItem;
		private System.Windows.Forms.StatusBar m_StatusBar;
		private System.Windows.Forms.TreeView m_DeviceView;
		private System.Windows.Forms.ImageList imageList1;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.MenuItem ManuallyAddDeviceItem;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.ContextMenu DeviceViewMenu;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.MenuItem menuItem8;
		private System.Windows.Forms.MenuItem EventLogItem;
		private System.Windows.Forms.MenuItem StatusBarItem;
	

		private System.Windows.Forms.ContextMenu EventViewMenu;
		private System.Windows.Forms.MenuItem ClearEventLogItem2;
		private System.Windows.Forms.MenuItem ClearEventLogItem;
		private System.Windows.Forms.MenuItem menuItem11;
		private System.Windows.Forms.ListView m_EventView;
		private System.Windows.Forms.ColumnHeader columnHeader3;
		private System.Windows.Forms.ColumnHeader columnHeader4;
		private System.Windows.Forms.ColumnHeader columnHeader5;
		private System.Windows.Forms.ColumnHeader columnHeader6;
		private System.Windows.Forms.Splitter splitter2;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.ListView m_ListView;
		private System.Windows.Forms.ColumnHeader columnHeader1;
		private System.Windows.Forms.ColumnHeader columnHeader2;
		private System.Windows.Forms.ContextMenu ListViewMenu;

		public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			m_DeviceManager=new DeviceManager();
			m_DeviceManager.OnDeviceAvailable+=new DeviceManager.DeviceAvailableHandler(this.OnDeviceAvaile);
			m_DeviceManager.OnDeviceUnavailable+=new DeviceManager.DeviceUnavailableHandler(this.OnDeviceUnavailable);

			m_DeviceManager.Start();

			m_EventListener=new EventListener();
			m_EventListener.OnEvent+=new EventListener.EventHandler(this.OnEvent);	
		
			m_EventListener.Start();

			OnRescanNetwork(null,null);

			SetEventViewVisiable(false);
			SetStatusBarVisible(true);

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.ManuallyAddDeviceItem = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.ClearEventLogItem = new System.Windows.Forms.MenuItem();
			this.menuItem11 = new System.Windows.Forms.MenuItem();
			this.ExitItem = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.RescanNetworkItem = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.menuItem8 = new System.Windows.Forms.MenuItem();
			this.EventLogItem = new System.Windows.Forms.MenuItem();
			this.StatusBarItem = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.AboutItem = new System.Windows.Forms.MenuItem();
			this.m_StatusBar = new System.Windows.Forms.StatusBar();
			this.m_DeviceView = new System.Windows.Forms.TreeView();
			this.DeviceViewMenu = new System.Windows.Forms.ContextMenu();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.splitter1 = new System.Windows.Forms.Splitter();
			this.panel1 = new System.Windows.Forms.Panel();
			this.panel2 = new System.Windows.Forms.Panel();
			this.m_ListView = new System.Windows.Forms.ListView();
			this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
			this.splitter2 = new System.Windows.Forms.Splitter();
			this.m_EventView = new System.Windows.Forms.ListView();
			this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
			this.EventViewMenu = new System.Windows.Forms.ContextMenu();
			this.ClearEventLogItem2 = new System.Windows.Forms.MenuItem();
			this.ListViewMenu = new System.Windows.Forms.ContextMenu();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem1,
																					  this.menuItem5,
																					  this.menuItem2});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.ManuallyAddDeviceItem,
																					  this.menuItem4,
																					  this.ClearEventLogItem,
																					  this.menuItem11,
																					  this.ExitItem});
			this.menuItem1.Text = "&File";
			// 
			// ManuallyAddDeviceItem
			// 
			this.ManuallyAddDeviceItem.Index = 0;
			this.ManuallyAddDeviceItem.Text = "Manually Add Device";
			this.ManuallyAddDeviceItem.Click += new System.EventHandler(this.ManuallyAddDevice);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 1;
			this.menuItem4.Text = "-";
			// 
			// ClearEventLogItem
			// 
			this.ClearEventLogItem.Index = 2;
			this.ClearEventLogItem.Text = "Clear Event Log";
			this.ClearEventLogItem.Click += new System.EventHandler(this.ClearEventLogItem_Click);
			// 
			// menuItem11
			// 
			this.menuItem11.Index = 3;
			this.menuItem11.Text = "-";
			// 
			// ExitItem
			// 
			this.ExitItem.Index = 4;
			this.ExitItem.Text = "E&xit";
			this.ExitItem.Click += new System.EventHandler(this.Exit);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 1;
			this.menuItem5.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.RescanNetworkItem,
																					  this.menuItem3,
																					  this.menuItem6,
																					  this.menuItem7,
																					  this.menuItem8,
																					  this.EventLogItem,
																					  this.StatusBarItem});
			this.menuItem5.Text = "&View";
			// 
			// RescanNetworkItem
			// 
			this.RescanNetworkItem.Index = 0;
			this.RescanNetworkItem.Text = "Rescan network";
			this.RescanNetworkItem.Click += new System.EventHandler(this.OnRescanNetwork);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 1;
			this.menuItem3.Text = "-";
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 2;
			this.menuItem6.Text = "Expand all devices";
			this.menuItem6.Click += new System.EventHandler(this.ExpandAllDevices);
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 3;
			this.menuItem7.Text = "Collapse all devices";
			this.menuItem7.Click += new System.EventHandler(this.CollapseAllDevices);
			// 
			// menuItem8
			// 
			this.menuItem8.Index = 4;
			this.menuItem8.Text = "-";
			// 
			// EventLogItem
			// 
			this.EventLogItem.Index = 5;
			this.EventLogItem.Text = "Event log";
			this.EventLogItem.Click += new System.EventHandler(this.EventLogItem_Click);
			// 
			// StatusBarItem
			// 
			this.StatusBarItem.Index = 6;
			this.StatusBarItem.Text = "Status bar";
			this.StatusBarItem.Click += new System.EventHandler(this.StatusBarItem_Click);
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 2;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.AboutItem});
			this.menuItem2.Text = "&Help";
			// 
			// AboutItem
			// 
			this.AboutItem.Index = 0;
			this.AboutItem.Text = "&About Device Spy";
			this.AboutItem.Click += new System.EventHandler(this.About);
			// 
			// m_StatusBar
			// 
			this.m_StatusBar.Location = new System.Drawing.Point(0, 363);
			this.m_StatusBar.Name = "m_StatusBar";
			this.m_StatusBar.Size = new System.Drawing.Size(688, 22);
			this.m_StatusBar.TabIndex = 0;
			// 
			// m_DeviceView
			// 
			this.m_DeviceView.ContextMenu = this.DeviceViewMenu;
			this.m_DeviceView.Dock = System.Windows.Forms.DockStyle.Left;
			this.m_DeviceView.HideSelection = false;
			this.m_DeviceView.ImageList = this.imageList1;
			this.m_DeviceView.Name = "m_DeviceView";
			this.m_DeviceView.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
																					 new System.Windows.Forms.TreeNode("UPnP Devices")});
			this.m_DeviceView.Size = new System.Drawing.Size(272, 363);
			this.m_DeviceView.TabIndex = 1;
			this.m_DeviceView.MouseDown += new System.Windows.Forms.MouseEventHandler(this.m_DeviceView_MouseDown);
			this.m_DeviceView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.m_DeviceView_AfterSelect);
			// 
			// DeviceViewMenu
			// 
			this.DeviceViewMenu.Popup += new System.EventHandler(this.DeviceViewMenu_Popup);
			// 
			// imageList1
			// 
			this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// splitter1
			// 
			this.splitter1.Location = new System.Drawing.Point(272, 0);
			this.splitter1.Name = "splitter1";
			this.splitter1.Size = new System.Drawing.Size(3, 363);
			this.splitter1.TabIndex = 2;
			this.splitter1.TabStop = false;
			// 
			// panel1
			// 
			this.panel1.Controls.AddRange(new System.Windows.Forms.Control[] {
																				 this.panel2,
																				 this.splitter2,
																				 this.m_EventView});
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(275, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(413, 363);
			this.panel1.TabIndex = 4;
			// 
			// panel2
			// 
			this.panel2.Controls.AddRange(new System.Windows.Forms.Control[] {
																				 this.m_ListView});
			this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(413, 256);
			this.panel2.TabIndex = 10;
			// 
			// m_ListView
			// 
			this.m_ListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																						 this.columnHeader1,
																						 this.columnHeader2});
			this.m_ListView.ContextMenu = this.ListViewMenu;
			this.m_ListView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.m_ListView.Font = new System.Drawing.Font("SimSun", 9F);
			this.m_ListView.FullRowSelect = true;
			this.m_ListView.MultiSelect = false;
			this.m_ListView.Name = "m_ListView";
			this.m_ListView.Size = new System.Drawing.Size(413, 256);
			this.m_ListView.TabIndex = 8;
			this.m_ListView.View = System.Windows.Forms.View.Details;
			// 
			// columnHeader1
			// 
			this.columnHeader1.Text = "Name";
			this.columnHeader1.Width = 100;
			// 
			// columnHeader2
			// 
			this.columnHeader2.Text = "Value";
			this.columnHeader2.Width = 300;
			// 
			// splitter2
			// 
			this.splitter2.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.splitter2.Location = new System.Drawing.Point(0, 256);
			this.splitter2.Name = "splitter2";
			this.splitter2.Size = new System.Drawing.Size(413, 3);
			this.splitter2.TabIndex = 9;
			this.splitter2.TabStop = false;
			// 
			// m_EventView
			// 
			this.m_EventView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																						  this.columnHeader3,
																						  this.columnHeader4,
																						  this.columnHeader5,
																						  this.columnHeader6});
			this.m_EventView.ContextMenu = this.EventViewMenu;
			this.m_EventView.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.m_EventView.Font = new System.Drawing.Font("SimSun", 9F);
			this.m_EventView.FullRowSelect = true;
			this.m_EventView.Location = new System.Drawing.Point(0, 259);
			this.m_EventView.MultiSelect = false;
			this.m_EventView.Name = "m_EventView";
			this.m_EventView.Size = new System.Drawing.Size(413, 104);
			this.m_EventView.TabIndex = 8;
			this.m_EventView.View = System.Windows.Forms.View.Details;
			// 
			// columnHeader3
			// 
			this.columnHeader3.Text = "Time";
			this.columnHeader3.Width = 66;
			// 
			// columnHeader4
			// 
			this.columnHeader4.Text = "Event Source";
			this.columnHeader4.Width = 147;
			// 
			// columnHeader5
			// 
			this.columnHeader5.Text = "State Variable";
			this.columnHeader5.Width = 124;
			// 
			// columnHeader6
			// 
			this.columnHeader6.Text = "Value";
			this.columnHeader6.Width = 68;
			// 
			// EventViewMenu
			// 
			this.EventViewMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						  this.ClearEventLogItem2});
			// 
			// ClearEventLogItem2
			// 
			this.ClearEventLogItem2.Index = 0;
			this.ClearEventLogItem2.Text = "Clear Event Log";
			this.ClearEventLogItem2.Click += new System.EventHandler(this.ClearEventLogItem_Click);
			// 
			// ListViewMenu
			// 
			this.ListViewMenu.Popup += new System.EventHandler(this.ListViewMenu_Popup);
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(688, 385);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.panel1,
																		  this.splitter1,
																		  this.m_DeviceView,
																		  this.m_StatusBar});
			this.Menu = this.mainMenu1;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Device Spy";
			this.Closed += new System.EventHandler(this.FormClosed);
			this.panel1.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MainForm());
		}

		private void OnRescanNetwork(object sender, System.EventArgs e)
		{
			DeviceFinder finder=new DeviceFinder(5);
			finder.OnNewDeviceFound+=new DeviceFinder.NewDeviceFoundHandler(this.OnNewDeviceFound);
			finder.OnSearchComplete+=new DeviceFinder.SearchCompleteHandler(this.OnSearchComplete);
			finder.StartFind("upnp:rootdevice");

			m_StatusBar.Text="Rescanning network...";
		}

		private void OnNewDeviceFound(Device device)
		{
			try
			{
				m_DeviceManager.AddDevice(device);
			}
			catch(Exception)
			{
			}

		}

		private delegate void DeviceAvaileHandler(Device device);
		private void OnDeviceAvaile(Device device)
		{
			Invoke(new InsertRootDeviceDelegate(this.InsertRootDevice),new object[]{device});
		}

		private delegate void DeviceUnavaileHandler(Device device);
		private void OnDeviceUnavailable(Device device)
		{
			Invoke(new RemoveRootDeviceDelegate(this.RemoveRootDevice),new object[]{device});
		}

		

		private delegate void InsertRootDeviceDelegate(Device device);
		private void InsertRootDevice(Device device)
		{
			if(GetRootDeviceNode(device)!=null)
				return;

			InsertDevice(m_DeviceView.Nodes[0],device);

			m_DeviceView.Nodes[0].Expand();
		}

		private delegate void RemoveRootDeviceDelegate(Device device);
		private void RemoveRootDevice(Device device)
		{
			TreeNode node=GetRootDeviceNode(device);

			if(node!=null)
				m_DeviceView.Nodes[0].Nodes.Remove(node);

		}

		private TreeNode GetRootDeviceNode(Device device)
		{
			foreach(TreeNode node in m_DeviceView.Nodes[0].Nodes)
			{
				if(((Device)node.Tag).UDN==device.UDN)
					return node;
			}

			return null;
		}

		private enum ImageListEnum{UPnPDevices=0,Device,Service,Folder,Property,Method,ServiceSubscribed}

		private void InsertDevice(TreeNode parentNode,Device device)
		{
			TreeNode deviceNode=parentNode.Nodes.Add(device.FriendlyName);
			deviceNode.ImageIndex=(int)ImageListEnum.Device;
			deviceNode.SelectedImageIndex=deviceNode.ImageIndex;
			deviceNode.Tag=device;
		
			foreach(Service service in device.Services)
			{
				InsertService(deviceNode,service);
			}
	
			foreach(Device subDevice in device.SubDevices)
			{
				InsertDevice(deviceNode,subDevice);
			}

		}

		private void InsertService(TreeNode parentNode,Service service)
		{
			TreeNode serviceNode=parentNode.Nodes.Add(service.ServiceType);
			serviceNode.ImageIndex=(int)ImageListEnum.Service;
			serviceNode.SelectedImageIndex=serviceNode.ImageIndex;
			serviceNode.Tag=service;

			TreeNode statesNode=serviceNode.Nodes.Add("State variables");
			statesNode.ImageIndex=(int)ImageListEnum.Folder;
			statesNode.SelectedImageIndex=statesNode.ImageIndex;
			foreach(StateVariable var in service.StateVariables)
				InsertStateVariable(statesNode,var);

			foreach(Action action in service.Actions)
				InsertAction(serviceNode,action);
		}

		private void InsertAction(TreeNode parentNode,Action action)
		{
			TreeNode actionNode=parentNode.Nodes.Add(action.Name);
			actionNode.ImageIndex=(int)ImageListEnum.Method;
			actionNode.SelectedImageIndex=actionNode.ImageIndex;
			actionNode.Tag=action;

			int i=0;
			string text=action.Name+"(";
			foreach(Argument arg in action.Arguments)
			{
				text+=arg.DataType;
				text+=" ";
				text+=arg.Name;

				if(i!=action.Arguments.Count-1)
					text+=",";

				i++;
			}

			text+=")";

			actionNode.Text=text;
		}

		private void InsertStateVariable(TreeNode parentNode,StateVariable var)
		{
			TreeNode varNode=parentNode.Nodes.Add(var.Name);
			varNode.ImageIndex=(int)ImageListEnum.Property;
			varNode.SelectedImageIndex=varNode.ImageIndex;

			varNode.Tag=var;
		}

		private void OnSearchComplete()
		{
			//m_StatusBar.Text="";

            Invoke((MethodInvoker) delegate { m_StatusBar.Text = ""; });
		}

		private void FormClosed(object sender, System.EventArgs e)
		{
			m_DeviceManager.Stop();
			m_EventListener.Stop();
		}

		private void ManuallyAddDevice(object sender, System.EventArgs e)
		{
			DeviceRootUrlForm form=new DeviceRootUrlForm();
			if(DialogResult.OK!=form.ShowDialog(this))
				return;
			try
			{
				m_DeviceManager.AddDevice(form.m_RootURL);
			}
			catch(Exception)
			{
				System.Windows.Forms.MessageBox.Show(this,"Can't load device!","Device Spy");
			}
		}

		private void m_DeviceView_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			m_ListView.Items.Clear();

			TreeNode selectedNode=m_DeviceView.SelectedNode;
	
			if(selectedNode==null||selectedNode.Tag==null)
			{
				ShowProductInfo();
				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(Device))
			{
				ShowDeviceInfo((Device)selectedNode.Tag);
				return;
			}

			else if(selectedNode.Tag.GetType()==typeof(Service))
			{
				ShowServiceInfo((Service)selectedNode.Tag);
				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(Action))
			{
				ShowActionInfo((Action)selectedNode.Tag);
				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(StateVariable))
			{
				ShowStateVariableInfo((StateVariable)selectedNode.Tag);
				return;
			}


		}

		private void AddListeViewItem(string name,string val)
		{
			m_ListView.Items.Add(new ListViewItem(new string[]{name,val}));
		}

		private void ShowProductInfo()
		{
			AddListeViewItem("Manufacturer","optman's world");
			AddListeViewItem("Product name","UPnP Spy");
			AddListeViewItem("Assembly",Assembly.GetCallingAssembly().ToString());
		}

		private void ShowDeviceInfo(Device device)
		{
			AddListeViewItem("Device Type",device.DeviceType);
			AddListeViewItem("FriendlyName",device.FriendlyName);
			AddListeViewItem("Manufacturer",device.Manufacturer);
			AddListeViewItem("ManufacturerURL",device.ManufacturerURL);
			AddListeViewItem("ModelDescription",device.ModelDescription);
			AddListeViewItem("ModelName",device.ModelName);
			AddListeViewItem("ModelNumber",device.ModelNumber);
			AddListeViewItem("ModelURL",device.ModelURL);
			AddListeViewItem("SerialNumber",device.SerialNumber);
			AddListeViewItem("UDN",device.UDN);
			AddListeViewItem("UPC",device.UPC);
			AddListeViewItem("Icons","");
			AddListeViewItem("PresentationURL",device.PresentationURL);
		}

		private void ShowServiceInfo(Service service)
		{
			AddListeViewItem("ServiceType",service.ServiceType);
			AddListeViewItem("ServiceId",service.ServiceID);
			AddListeViewItem("SCPDURL",service.SCPDURL);
			AddListeViewItem("ControlURL",service.ControlURL);
			AddListeViewItem("EventSubURL",service.EventSubURL);
		}

		private void ShowActionInfo(Action action)
		{
			AddListeViewItem("Name",action.Name);

		}
		
		private void ShowStateVariableInfo(StateVariable var)
		{
			AddListeViewItem("Name",var.Name);
			AddListeViewItem("DataType",var.DataType);
			AddListeViewItem("Evented",var.SendEvents?"yes":"no");
		}

		private void DeviceViewMenu_Popup(object sender, System.EventArgs e)
		{
			DeviceViewMenu.MenuItems.Clear();

			TreeNode selectedNode=m_DeviceView.SelectedNode;
		
			MenuItem RescanItem=new MenuItem("Rescan network",new System.EventHandler(this.OnRescanNetwork));
			DeviceViewMenu.MenuItems.Add(RescanItem);


			DeviceViewMenu.MenuItems.Add(0,new MenuItem("-"));

			DeviceViewMenu.MenuItems.Add(0,new MenuItem("Expand all devices",new System.EventHandler(this.ExpandAllDevices)));
			DeviceViewMenu.MenuItems.Add(1,new MenuItem("Collapse all devices",new System.EventHandler(this.CollapseAllDevices)));
	
			if(selectedNode==null||selectedNode.Tag==null)
			{
				return;
			}

			if(selectedNode.Tag.GetType()==typeof(Device))
			{
				//is root device?
				if(((Device)selectedNode.Tag).IsRootDevice)
				{
					DeviceViewMenu.MenuItems.Add(0,new MenuItem("-"));

					MenuItem removeItem=new MenuItem("Remove Device",new System.EventHandler(this.RemoveDevice));
					DeviceViewMenu.MenuItems.Add(0,removeItem);

					MenuItem getXmlItem=new MenuItem("Get Device XML",new System.EventHandler(this.GetDeviceXML));
					DeviceViewMenu.MenuItems.Add(0,getXmlItem);
				}

				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(Service))
			{
				DeviceViewMenu.MenuItems.Add(0,new MenuItem("-"));

				MenuItem getXmlItem=new MenuItem("Get Service XML",new System.EventHandler(this.GetServiceXML));
				DeviceViewMenu.MenuItems.Add(0,getXmlItem);

				MenuItem subscribeItem=new MenuItem("Subscribe to Events",new System.EventHandler(this.SubscribeToEvents));
				subscribeItem.Checked=((Service)selectedNode.Tag).SubscriptionID!=null;
				DeviceViewMenu.MenuItems.Add(0,subscribeItem);
				subscribeItem.DefaultItem=true;


				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(Action))
			{
				DeviceViewMenu.MenuItems.Add(0,new MenuItem("-"));

				MenuItem subscribeItem=new MenuItem("Invoke Action",new System.EventHandler(this.InvokeAction));
				DeviceViewMenu.MenuItems.Add(0,subscribeItem);

				return;
			}
			else if(selectedNode.Tag.GetType()==typeof(StateVariable))
			{
				DeviceViewMenu.MenuItems.Add(0,new MenuItem("-"));

				MenuItem queryItem=new MenuItem("Query State Variable",new System.EventHandler(this.QueryStateVariable));
				DeviceViewMenu.MenuItems.Add(0,queryItem);
				return;
			}
		
		}
		
		private void RemoveDevice(object sender, EventArgs e)
		{
			m_DeviceManager.RemoveDevice((Device)(m_DeviceView.SelectedNode.Tag));
		}

		[DllImport("shell32.dll")]
		static extern int ShellExecute(int hwnd,string operation,string file,string param,string directory,int show);

		private void GetDeviceXML(object sender, EventArgs e)
		{
			Device device=(Device)(m_DeviceView.SelectedNode.Tag);

			ShellExecute(0,"open",device.RootURL,"","",0);
		}

		private void GetServiceXML(object sender, EventArgs e)
		{
			Service service=(Service)(m_DeviceView.SelectedNode.Tag);

			ShellExecute(0,"open",service.SCPDURL,"","",0);
		}


		private void AddServiceNode(string cookie,TreeNode node)
		{
			m_ServiceNodes.Add(cookie,node);
		}

		private void RemoveServiceNode(TreeNode node)
		{
			System.Collections.IDictionaryEnumerator enumerator=m_ServiceNodes.GetEnumerator();

			while(enumerator.MoveNext())
			{
				if(enumerator.Value==node)
				{
					m_ServiceNodes.Remove(enumerator.Key);

					return;					
				}
			}

		}

		private TreeNode GetServiceNode(string cookie)
		{
			return (TreeNode)m_ServiceNodes[cookie];
		}

		private void SubscribeToEvents(object sender, EventArgs e)
		{
			Service service=(Service)(m_DeviceView.SelectedNode.Tag);

			if(service.SubscriptionID==null)
			{
				string cookie=System.Guid.NewGuid().ToString();

				service.Subscribe(1800,m_EventListener.GetEventHandler(cookie));

				AddServiceNode(cookie,m_DeviceView.SelectedNode);

				m_DeviceView.SelectedNode.ImageIndex=(int)ImageListEnum.ServiceSubscribed;
				m_DeviceView.SelectedNode.SelectedImageIndex=(int)ImageListEnum.ServiceSubscribed;

				SetEventViewVisiable(true);
			}
			else
			{
				service.Unsubscribe();

				RemoveServiceNode(m_DeviceView.SelectedNode);

				m_DeviceView.SelectedNode.ImageIndex=(int)ImageListEnum.Service;
				m_DeviceView.SelectedNode.SelectedImageIndex=(int)ImageListEnum.Service;
			}
		}

		private void InvokeAction(object sender, EventArgs e)
		{
			Action action=(Action)(m_DeviceView.SelectedNode.Tag);
			
			InvokeActionForm form=new InvokeActionForm(action);

			form.Show();

		}

		private void QueryStateVariable(object sender, EventArgs e)
		{
			StateVariable var=(StateVariable)(m_DeviceView.SelectedNode.Tag);

			var.Query();

			m_DeviceView.SelectedNode.Text=var.Name+"["+var.Value+"]";

	
		}

		private void m_DeviceView_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			m_DeviceView.SelectedNode=m_DeviceView.GetNodeAt(e.X,e.Y);
		}

		private void ExpandAllDevices(object sender, System.EventArgs e)
		{
			m_DeviceView.ExpandAll();
		}

		private void CollapseAllDevices(object sender, System.EventArgs e)
		{
			m_DeviceView.CollapseAll();
		}

		private void SetEventViewVisiable(bool show)
		{
			splitter2.Visible=show;
			m_EventView.Visible=show;

			EventLogItem.Checked=show;
			m_ShowEventLog=show;
		}
		
		private void EventLogItem_Click(object sender, System.EventArgs e)
		{
			SetEventViewVisiable(!m_ShowEventLog);
		}

		private void SetStatusBarVisible(bool show)
		{
			m_StatusBar.Visible=show;
			StatusBarItem.Checked=show;
			m_ShowStatusBar=show;
		}

		private void StatusBarItem_Click(object sender, System.EventArgs e)
		{
			SetStatusBarVisible(!m_ShowStatusBar);
		}

		private void OnEvent(StateVariable[] vars,string cookie)
		{
			foreach(StateVariable var in vars)
				m_EventView.Items.Add(new ListViewItem(new string[]{System.DateTime.Now.ToShortTimeString(),cookie,var.Name,var.Value}));

			TreeNode node=GetServiceNode(cookie);
			if(node==null)
				return;

			//refresh device view
			foreach(TreeNode subNode in node.Nodes)
			{
				//state variables node
				if(subNode.Tag==null)
				{
					//refresh each node
					foreach(TreeNode varNode in subNode.Nodes)
					{
						StateVariable var=(StateVariable)varNode.Tag;

						//find match variable
						foreach(StateVariable var2 in vars)
						{
							if(var2.Name==var.Name)
							{
								varNode.Text=var.Name+"["+var2.Value+"]";
								break;
							}
						}
					}

					break;
				}
			}

		}

		private void ClearEventLogItem_Click(object sender, System.EventArgs e)
		{
			m_EventView.Items.Clear();
		}

		private void Exit(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void About(object sender, System.EventArgs e)
		{
			new AboutForm().ShowDialog(this);
		}

		private void ListViewMenu_Popup(object sender, System.EventArgs e)
		{
			ListViewMenu.MenuItems.Clear();

			if(m_ListView.SelectedItems.Count>0)
			{
				ListViewMenu.MenuItems.Add(new MenuItem("Copy Value to Clipboard",new System.EventHandler(this.CopyListViewValueToClipboard)));
			}
		}

		private void CopyListViewValueToClipboard(object sender, System.EventArgs e)
		{
			System.Windows.Forms.Clipboard.SetDataObject(m_ListView.SelectedItems[0].SubItems[1].Text);
		}
/*
		private void EventViewMenu_Popup(object sender, System.EventArgs e)
		{
			if(m_EventView.SelectedItems.Count>0)
			{
				EventViewMenu.MenuItems.Add(0,new MenuItem("-"));
				EventViewMenu.MenuItems.Add(0,new MenuItem("Copy Value to Clipboard",new System.EventHandler(this.CopyEventValueToClipboard)));
			}
	
		}

		private void CopyEventValueToClipboard(object sender, System.EventArgs e)
		{
			System.Windows.Forms.Clipboard.SetDataObject(m_EventView.SelectedItems[0].SubItems[0].Text);
		}
*/
	
		private DeviceManager m_DeviceManager;
		private EventListener m_EventListener;
		private bool m_ShowEventLog;
		private bool m_ShowStatusBar;

		private Hashtable m_ServiceNodes=new Hashtable();

	}
}
