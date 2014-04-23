using System.Net;
using System.Threading;
using System.Reflection;
using System.Collections;
using System;

namespace UPnPStack
{
	/// <summary>
	/// Summary description for DeviceHost.
	/// </summary>
	public class DeviceHost
	{
		private readonly IPEndPoint m_SSDPMulticastEP=
			new IPEndPoint(IPAddress.Parse("239.255.255.250"),1900);

		public DeviceHost(Device rootDevice)
		{
			m_RootDevice=rootDevice;

			//Create Web Server
			WebRootFolder=AppDomain.CurrentDomain.BaseDirectory;
	
			m_HttpServer=new HTTPServer(WebRootFolder);
			WebPort=m_HttpServer.Port;
			WebHost=m_HttpServer.Host;
			
			//Set root device url
			m_RootDevice.RootURL="http://"+WebHost+":"+WebPort+"/root.xml";
			m_RootDevice.URLBase="http://"+WebHost+":"+WebPort+"/";

			//Add Soap handler 
			m_SoapHandler=new SoapHandler();

			m_HttpServer.AddRequestHandler("/SoapHandler",m_SoapHandler);

			//Add Event handler
			m_EventHandler=new EventHandler();

			m_HttpServer.AddRequestHandler("/EventHandler",m_EventHandler);


			//create ssdp sender
			m_SSDPSender=new HTTPUDPSender();

			//create ssdp listener
			m_SSDPListener=new SSDPListener(m_SSDPMulticastEP);
			m_SSDPListener.OnSearchMessage+=new SSDPListener.SearchMessageHandler(this.OnSearchRequest);


			//create root device description
			m_HttpServer.InsertCacheItem("/root.xml",m_RootDevice.CetDescription());

		
			//recruit create services description
			CreateServices(m_RootDevice);
			

		}

		private void CreateServices(Device device)
		{
			foreach(Service service in device.Services)
			{
				string url="/"+service.SCPDURL;

				//link url and device
				m_SoapHandler.InsertServiceObject(service.ServiceID,device);

				//SCPD 
				m_HttpServer.InsertCacheItem(url,service.GetDescription());

				///TODO:Control & Event

			}

			foreach(Device subDevice in device.SubDevices)
			{
				CreateServices(subDevice);
			}

			//insert device to event handler
			m_EventHandler.InsertDevice(device);

		}

		public void Stop()
		{
			SayBye();

			m_SSDPListener.Stop();

			m_HttpServer.Stop();

			m_AdvertiseTimer.Dispose();
		}
	
		public void Start()
		{
			m_HttpServer.Start();

			m_SSDPListener.Start();
			
			//no need to say here,the timer will do it immediately
			//SayHi();

			//advertise 10 seconde before expiration
			int period=m_RootDevice.Expiration-10;
			//the min period is 1 second
			if(period<=0)
				period=1;

			m_AdvertiseTimer=new Timer(new TimerCallback(this.ReAdvertise),null,0,period*1000);
            
		}

		private void ReAdvertise(object o)
		{
			SayHi();
		}

		private void SayHi()
		{
			SSDPAliveMsg msg;

			//advertise root device
			msg=new SSDPAliveMsg(m_RootDevice.Expiration,m_RootDevice.RootURL,
							"upnp:rootdevice",m_RootDevice.DeviceID+"::"+"upnp:rootdevice");
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);
	
			msg=new SSDPAliveMsg(m_RootDevice.Expiration,m_RootDevice.RootURL,
							m_RootDevice.DeviceType,m_RootDevice.DeviceID+"::"+m_RootDevice.DeviceType);
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);

			msg=new SSDPAliveMsg(m_RootDevice.Expiration,m_RootDevice.RootURL,
							m_RootDevice.DeviceID,m_RootDevice.DeviceID);
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);
		
			//advertise embedded devices

			//advertise services
			///TODO:
			/*
			Service service;
			IDictionaryEnumerator serviceEnumerator=Services.GetEnumerator();
			while(serviceEnumerator.MoveNext())
			{
				service=(Service)serviceEnumerator.Value;

				m_SSDPSender.NT=service.ServiceType;
				m_SSDPSender.USN=DeviceID+"::"+service.ServiceType;
				m_SSDPSender.SayHi();
			}
			*/
			
		}

		private void SayBye()
		{
			SSDPByeMsg msg;

			//advertise root device
			msg=new SSDPByeMsg("upnp:rootdevice",m_RootDevice.DeviceID+"::"+"upnp:rootdevice");
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);
	
			msg=new SSDPByeMsg(m_RootDevice.DeviceType,m_RootDevice.DeviceID+"::"+m_RootDevice.DeviceType);
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);

			msg=new SSDPByeMsg(m_RootDevice.DeviceID,m_RootDevice.DeviceID);
			m_SSDPSender.Send(m_SSDPMulticastEP,msg);


			//advertise embedded devices

			//advertise services
			///TODO:
			/*
			Service service;
			IDictionaryEnumerator serviceEnumerator=Services.GetEnumerator();
			while(serviceEnumerator.MoveNext())
			{
				service=(Service)serviceEnumerator.Value;
				m_SSDPSender.NT=service.ServiceType;
				m_SSDPSender.USN=DeviceID+"::"+service.ServiceType;
				m_SSDPSender.SayBye();
			}
			*/
		}

		private void OnSearchRequest(string searchTarget,int mx,IPEndPoint sourceEP)
		{
			if(searchTarget=="upnp:rootdevice")
			{
				SSDPSearchResponseMsg msg=new SSDPSearchResponseMsg(m_RootDevice.Expiration,
					m_RootDevice.RootURL,
					"upnp:rootdevice",
					m_RootDevice.DeviceID+"::"+"upnp:rootdevice");

				SearchResponseItem item=new SearchResponseItem();
				item.SourceEP=sourceEP;
				item.Message=msg;

				//maximum delay is mx-1 ,leave 1 seconds for network transport
				mx=new Random().Next(mx-1)*1000;

				item.Timer=new Timer(new TimerCallback(this.SendSearchResponse),item,mx,Timeout.Infinite);			
			}

			///TODO:other search target
		}

		class SearchResponseItem
		{
			public IPEndPoint SourceEP;
			public SSDPSearchResponseMsg Message;
			public Timer Timer;
		}

		private void SendSearchResponse(object o)
		{
			SearchResponseItem item=(SearchResponseItem)o;
			
			item.Timer.Dispose();

			m_SSDPSender.Send(item.SourceEP,item.Message);
		}


		private Timer m_AdvertiseTimer;

		private HTTPServer m_HttpServer;
		private string WebRootFolder;
		private string WebHost;
		private int WebPort;

		private SoapHandler m_SoapHandler;
		private EventHandler m_EventHandler;

		private Device m_RootDevice;

		private SSDPListener m_SSDPListener;
		private HTTPUDPSender m_SSDPSender;
	}
}
