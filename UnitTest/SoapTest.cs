using System.Xml;
using System.IO;
using System;
using UPnPStack;
using NUnit.Framework;

[TestFixture]
public class SoapTest
{
	public SoapTest()
	{
	}

	public SoapRequest ParseSoapRequest(string fileName,string SoapActionType)
	{
		FileStream fs=new FileStream(fileName,FileMode.Open,FileAccess.Read,FileShare.Read);
		BinaryReader reader=new BinaryReader(fs);

		byte[] data=new Byte[fs.Length];

		reader.Read(data,0,data.Length);
		reader.Close();
		fs.Close();


		HTTPRequest httpRequest=new HTTPRequest();
		httpRequest.AddHeaderValue("SOAPACTION",SoapActionType);
		httpRequest.Content=data;

		return new SoapRequest(httpRequest);
	}

	[Test]
	public void ParseSoapRequest_Action()
	{
		SoapRequest request=ParseSoapRequest(@"C:\Temp\UPnP\SampleDevices\SoapRequest.xml",
								"xxx");
		
		Assertion.Assert(request.Type==SoapRequest.RequestType.Action);
	    Assertion.Assert(request.Action.Name=="Switch");
	}

	[Test]
	public void ParseSoapRequest_QueryState()
	{
		SoapRequest request=ParseSoapRequest(@"C:\Temp\UPnP\SampleDevices\SoapRequest-QueryStateVariable.xml",
								"\"urn:schemas-upnp-org:control-1-0#QueryStateVariable\"");
		
		Assertion.Assert(request.Type==SoapRequest.RequestType.QueryStateVariable);
	    Assertion.Assert(request.StateVariable.Name=="PowerState");
	}
}
