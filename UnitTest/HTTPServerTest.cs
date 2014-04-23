using System;
using System.Net;
using System.Threading;
using System.Net.Sockets;
using UPnPStack;
using NUnit.Framework;

[TestFixture]
public class HTTPServerTest
{
	UPnPStack.HTTPServer server;
		
	[SetUp]
	public void Init()
	{
		server=new HTTPServer(81,@"C:\temp\UPnP\WebServer");
		server.Start();		
	}


	[Test]
	public void GetFile()
	{
	
		WebRequest myRequest = WebRequest.Create("http://localhost:81/index.htm");

		// Return the response. 
		WebResponse myResponse = myRequest.GetResponse();

		// Close the response to free resources.
		myResponse.Close();

	}
	[Test]
	public void GetFileM()
	{
		for(int i=0;i<3;i++)
			GetFile();
	
	}	
	[Test]
	public void ConnectTest()
	{
		for(int i=0;i<10;i++)
		{
			TcpClient client=new TcpClient();
			client.Connect("localhost",81);
			client.Close();
		}
	}


	[TearDown]
	public void Clear()
	{
		server.Stop();
	}

}

namespace UPnPStack
{
	using NUnit.Framework;

	

}
