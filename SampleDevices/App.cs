using System;
using UPnPStack;
using log4net;

namespace SampleDevices
{
	/// <summary>
	/// Summary description for App.
	/// </summary>
	class App
	{
		private static readonly ILog log=LogManager.GetLogger(typeof(App));

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			Device house=new Bighouse();
			Device room1=new Room();
			Device room2=new Room();
			
			Device light1=new LightDevice();
			Device light2=new LightDevice();

			room1.AddSubDevice(light1);
			room2.AddSubDevice(light2);

			house.AddSubDevice(room1);
			house.AddSubDevice(room2);
			
			DeviceHost host=new DeviceHost(light1);
			host.Start();
	
			DeviceHost host2=new DeviceHost(house);
			host2.Start();
				
			//log.Debug("Log begin");

			Console.WriteLine("Devices started ...<Press any key to exit>");
			Console.Read();
	
			host.Stop();	
			host2.Stop();	
		}
	}
}
