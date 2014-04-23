using System;
using UPnPStack;

namespace SampleDevices
{
	[UPnPService("urn:schemas-upnp-org:service:BinaryLight:1",
		 "SwitchPower.0001")]

	[UPnPService("urn:schemas-upnp-org:service:BinaryLight:1",
		 "SwitchPower.0002")]
	

	[UPnPDeviceAttribute(DeviceType="urn:schemas-upnp-org:device:BinaryLight:1",
			FriendlyName="Simple Light Source",
			Manufacturer="optman's world",
			ManufacturerURL="http://www.upnp.org",
			ModelDescription="A simple UPnP device",
			ModelName="Simple Light Source",
			ModelNumber="xxx",
			ModelURL="http://www.upnp.org",
			Expiration=300	//expire in 300 second
			)]
	public class LightDevice :  UPnPStack.Device
	{
		
		protected override void SetDeviceInfo()
		{
			base.SetDeviceInfo();

			FriendlyName+="("+System.Net.Dns.GetHostName()+")";
		}

		[UPnPAction("SwitchPower.0001","PowerState","PowerState")]
		public bool Switch(bool powerState,out bool resultPowerState)
		{
			m_PowerState=powerState;

			resultPowerState=m_PowerState;

			Console.WriteLine("LightDevice-Switch({0})",powerState);

			FireStateChanged("PowerState","PowerState2");

			return false;
		}
		
		
		private bool m_PowerState;

		[UPnPStateVariable("SwitchPower.0001",true)]
		[UPnPStateVariable("SwitchPower.0002",true)]
		public bool PowerState
		{
			get
			{
				Console.WriteLine("Query PowerState");

				return m_PowerState;
			}
		}


		[UPnPStateVariable("SwitchPower.0002",true)]
		public string Name
		{
			get
			{
				Console.WriteLine("Query Name");

				return System.Net.Dns.GetHostName();
			}
		}

	}


	[UPnPDeviceAttribute(DeviceType="urn:schemas-upnp-org:device:House:1",
		 FriendlyName="BigHouse",
		 Manufacturer="optman's world",
		 ManufacturerURL="http://www.upnp.org",
		 ModelDescription="A simple UPnP device",
		 ModelName="Big house",
		 ModelNumber="xxx",
		 ModelURL="http://www.upnp.org",
		 Expiration=300	//expire in 300 second
		 )]
	public class Bighouse :  UPnPStack.Device
	{

	}


	[UPnPService("urn:schemas-upnp-org:service:CleanWork:1",
		 "Cleanwork.0001")]

	[UPnPDeviceAttribute(DeviceType="urn:schemas-upnp-org:device:Room:1",
		 FriendlyName="Room",
		 Manufacturer="optman's world",
		 ManufacturerURL="http://www.upnp.org",
		 ModelDescription="A simple UPnP device",
		 ModelName="Room",
		 ModelNumber="xxx",
		 ModelURL="http://www.upnp.org",
		 Expiration=300	//expire in 300 second
		 )]
	public class Room :  UPnPStack.Device
	{
		[UPnPAction("Cleanwork.0001",null)]
		public void Clean()
		{
			Console.WriteLine("Room-Clean()");
		}
	}

}
