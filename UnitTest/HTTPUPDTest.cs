using System;
using System.Net;
using System.Threading;
using UPnPStack;
using NUnit.Framework;

[TestFixture]
public class HTTPUPDTest
{
	private readonly IPEndPoint m_MuticastEndPoint=
						new IPEndPoint(IPAddress.Parse("239.255.255.250"),1900);

	AutoResetEvent waitEvent;

	[SetUp]
	public void Init()
	{
		waitEvent=new AutoResetEvent(false);
	}

	[TearDown]
	public void Clear()
	{
		waitEvent.Close();
	}


	private void OnRequest(HTTPRequest request,IPEndPoint ep)
	{
		waitEvent.Set();				
         
	}

	[Test]
	public void Multicast()
	{
		HTTPUDPListener listener=new HTTPUDPListener(m_MuticastEndPoint);
	

		listener.OnNewRequest+=new HTTPUDPListener.RequestHandler(this.OnRequest);

		listener.Start();

		//Wait the listener to startup
		Thread.Sleep(100);

		HTTPUDPSender sender=new HTTPUDPSender();

		HTTPRequest request=new HTTPRequest();
		request.Method="NOTIFY";
		request.URL="*";

		sender.Send(m_MuticastEndPoint,request);

		Assertion.Assert("Should receive multicast in 2 second",waitEvent.WaitOne(2000,false));       

		listener.Stop();
	}

	[Test]
	public void MulticastM()
	{
		Multicast();
		Multicast();
		Multicast();
	}

}
