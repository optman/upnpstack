using System.Collections;
using System.Net;
using System;
using UPnPStack;

namespace UPnPStack.CP
{
	/// <summary>
	/// Summary description for DeviceManager.
	/// </summary>
	public class DeviceManager
	{
		private readonly IPEndPoint m_SSDPMulticastEP=
			new IPEndPoint(IPAddress.Parse("239.255.255.250"),1900);

		public DeviceManager()
		{
			m_Listener=new SSDPListener(m_SSDPMulticastEP);
			m_Listener.OnNotifyAliveMessage+=new SSDPListener.NotifyAliveMessageHandler(this.OnNotifyAliveMessage);
			m_Listener.OnNotifyByeMessage+=new SSDPListener.NotifyByeMessageHandler(this.OnNotifyByeMessage);
		}

		public void Start()
		{
			m_Listener.Start();
		}

		public void Stop()
		{
			m_Listener.Stop();
		}

		private void OnNotifyAliveMessage(string nt,string usn,string location)
		{
			if(nt=="upnp:rootdevice")
			{
				try
				{
					AddDevice(location);


				}
				catch(Exception)
				{
				}

			}
		}

		private void OnNotifyByeMessage(string nt,string usn)
		{
			if(nt=="upnp:rootdevice")
			{
				try
				{
					RemoveDevice(usn.Substring(0,usn.IndexOf("::")));
				}
				catch(Exception)
				{
				}

			}
		}


		public void RemoveDevice(Device device)
		{
			RemoveDevice(device.UDN);
		}

		public void RemoveDevice(string usn)
		{
			foreach(Device device in Devices)
			if(device.UDN==usn)
			{
				Devices.Remove(device);

				if(OnDeviceUnavailable!=null)
					OnDeviceUnavailable(device);

				return;
			}
		
			throw new Exception("Device doesn't exit");
		}
		
		public void AddDevice(Device newDevice)
		{

			foreach(Device device in Devices)
				if(device.UDN==newDevice.UDN)
					throw new Exception("Device already exit");

			Devices.Add(newDevice);

			if(OnDeviceAvailable!=null)
				OnDeviceAvailable(newDevice);
		}

		public void AddDevice(string rootURL)
		{
			AddDevice(DeviceDescription.Load(rootURL));
		}

		public delegate void DeviceAvailableHandler(Device device);
		public event DeviceAvailableHandler OnDeviceAvailable;

		public delegate void DeviceUnavailableHandler(Device device);
		public event DeviceUnavailableHandler OnDeviceUnavailable;

		public ArrayList Devices=new ArrayList();

		private SSDPListener m_Listener;
		
	}
}
