using System;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading;
using System.Xml;
using UPnPStack;
using UPnPStack.CP;
using log4net;


namespace TestApp
{
	/// <summary>
	/// Summary description for Class1.
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
			App app=new App();


			System.Collections.IEnumerator x=System.Net.Dns.GetHostByName("").AddressList.GetEnumerator();
			x.MoveNext();
			Console.WriteLine(((IPAddress)x.Current).ToString());
		

			log.Debug("log message");
			
					
			Console.WriteLine("Press any key to exit.");
			Console.Read();


	
		}
	}
}
