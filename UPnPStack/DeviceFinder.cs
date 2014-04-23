using System.Threading;
using System.Net;
using System;

namespace UPnPStack.CP
{

	/// <summary>
	/// DeviceFinder is usey by client to find specific devices or services
	/// </summary>
	public class DeviceFinder
	{
		private readonly IPEndPoint m_SSDPMulticastEP=
			new IPEndPoint(IPAddress.Parse("239.255.255.250"),1900);

		public DeviceFinder(int expirateion)
		{
			m_Expiration=expirateion;

			m_Listener=new SSDPListener();
			m_Listener.OnSearchResultMessage+=new SSDPListener.SearchResultMessageHandler(this.OnSearchResult);
		}

		public void StartFind(string type)
		{
			m_Listener.Start();

			HTTPUDPSender sender =new HTTPUDPSender(m_Listener.Socket,false);

			SSDPSearchMsg msg=new SSDPSearchMsg(type,m_Expiration);

			sender.Send(m_SSDPMulticastEP,msg);

			m_Searching=true;
	
			m_ExpireTimer=new Timer(new TimerCallback(this.OnExpire),null,m_Expiration*1000,Timeout.Infinite);
		}

		public void CancelFind()
		{
			if(m_Searching)
			{
				m_Listener.Stop();

				m_ExpireTimer.Dispose();

				m_Searching=false;
			}
		}
		
		private void OnExpire(object o)
		{
			CancelFind();

			if(OnSearchComplete!=null)
				OnSearchComplete();
			
			//Console.WriteLine("Search complete!");
		}

		private void OnSearchResult(string st,string usn,string location)
		{
			//Console.WriteLine("ST:{0}\nusn:{1}\nlocation:{2}",st,usn,location);

			Device device=DeviceDescription.Load(location);

			if(OnNewDeviceFound!=null)
				OnNewDeviceFound(device);

		}

		public delegate void NewDeviceFoundHandler(Device device);
		public event NewDeviceFoundHandler OnNewDeviceFound;

		public delegate void SearchCompleteHandler();
		public event SearchCompleteHandler OnSearchComplete;

		private SSDPListener m_Listener;

		private bool m_Searching;

		private Timer m_ExpireTimer;
		private int m_Expiration;
	}
}
