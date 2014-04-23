using System.Xml;
using System.Collections;
using System;

namespace UPnPStack.CP
{
	/// <summary>
	/// DeviceDescription represent the xml file 
	/// </summary>
	public class DeviceDescription
	{
		public DeviceDescription()
		{
		}

		public static Device Load(string rootURL)
		{
	
			System.Net.HttpWebRequest request=(System.Net.HttpWebRequest)System.Net.WebRequest.Create(rootURL);

			System.Net.HttpWebResponse response=(System.Net.HttpWebResponse)request.GetResponse();

			System.IO.Stream s=response.GetResponseStream();


			XmlDocument doc=new XmlDocument();

			XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
			nsmgr.AddNamespace("s","urn:schemas-upnp-org:device-1-0");

			doc.Load(s);
			
			XmlNode rootNode=doc.DocumentElement;
		
			
			string urlBase;
			XmlNode urlBaseNode=rootNode.SelectSingleNode("s:URLBase",nsmgr);
			if(urlBaseNode!=null)
				urlBase=urlBaseNode.FirstChild.Value;
			else
				urlBase=rootURL.Substring(0,rootURL.IndexOf("/",rootURL.IndexOf("//")+2)+1);

	
			XmlNode deviceNode=rootNode.SelectSingleNode("s:device",nsmgr);


			return new Device(deviceNode,nsmgr,urlBase,rootURL);
			
		}
	}


	public class Device
	{
		public Device(XmlNode deviceNode,XmlNamespaceManager nsmgr,string urlBase,string rootURL)
		{
			m_IsRootDevice=(rootURL!=null);
			m_RootURL=rootURL;

			DeviceType=GetString(deviceNode,"s:deviceType",nsmgr);
			FriendlyName=GetString(deviceNode,"s:friendlyName",nsmgr);
			Manufacturer=GetString(deviceNode,"s:manufacturer",nsmgr);
			ManufacturerURL=GetString(deviceNode,"s:manufacturerURL",nsmgr);
			ModelDescription=GetString(deviceNode,"s:modelDescription",nsmgr);
			ModelName=GetString(deviceNode,"s:modelName",nsmgr);
			ModelNumber=GetString(deviceNode,"s:modelNumber",nsmgr);
			ModelURL=GetString(deviceNode,"s:modelURL",nsmgr);
			SerialNumber=GetString(deviceNode,"s:serialNumber",nsmgr);
			UDN=GetString(deviceNode,"s:UDN",nsmgr);
			UPC=GetString(deviceNode,"s:UPC",nsmgr);
			PresentationURL=GetString(deviceNode,"s:presentationURL",nsmgr);

			///TODO:Icons
			

			try
			{
				XmlNode devicesNode=deviceNode.SelectSingleNode("s:deviceList",nsmgr);
				XmlNodeList deviceNodes=devicesNode.SelectNodes("s:device",nsmgr);
				foreach(XmlNode subDeviceNode in deviceNodes)
				{
					m_SubDevices.Add(new Device(subDeviceNode,nsmgr,urlBase,null));
				}
			}
			catch(Exception)
			{
			}
		
			try
			{
				XmlNode servicesNode=deviceNode.SelectSingleNode("s:serviceList",nsmgr);
				XmlNodeList serviceNodes=servicesNode.SelectNodes("s:service",nsmgr);
				foreach(XmlNode serviceNode in serviceNodes)
				{
					m_Services.Add(new Service(serviceNode,nsmgr,urlBase));
				}	
			}
			catch(Exception)
			{
			}
			
		}

		private string GetString(XmlNode deviceNode,string nodeName,XmlNamespaceManager nsmgr)
		{
			try
			{
				return deviceNode.SelectSingleNode(nodeName,nsmgr).FirstChild.Value;
			}
			catch(Exception)
			{
				return null;
			}
		}

		public string DeviceType;
		public string FriendlyName;
		public string Manufacturer;
		public string ManufacturerURL;
		public string ModelDescription;
		public string ModelName;
		public string ModelNumber;
		public string ModelURL;
		public string SerialNumber;
		public string UDN;
		public string UPC;
		public string[] icons;
		public string PresentationURL;

		private string m_RootURL;
		public string RootURL
		{
			get{return m_RootURL;}
		}

		private bool m_IsRootDevice;
		public bool IsRootDevice
		{
			get{return m_IsRootDevice;}
		}

		private ArrayList m_SubDevices=new ArrayList();
		public ArrayList SubDevices
		{
			get{return m_SubDevices;}
		}

		private ArrayList m_Services=new ArrayList();
		public ArrayList Services
		{
			get{return m_Services;}
		}
	}

	public class Service
	{
		public Service(XmlNode serviceNode,XmlNamespaceManager nsmgr,string urlBase)
		{
			XmlNode serviceTypeNode=serviceNode.SelectSingleNode("s:serviceType",nsmgr);
			ServiceType=serviceTypeNode.FirstChild.Value;

			XmlNode serviceIdNode=serviceNode.SelectSingleNode("s:serviceId",nsmgr);
			ServiceID=serviceIdNode.FirstChild.Value;

			XmlNode scpdURLNode=serviceNode.SelectSingleNode("s:SCPDURL",nsmgr);
			SCPDURL=scpdURLNode.FirstChild.Value;
			if(SCPDURL.StartsWith("/"))
				SCPDURL=SCPDURL.Substring(1,SCPDURL.Length-1);
		
			SCPDURL=urlBase+SCPDURL;


			XmlNode controlURLNode=serviceNode.SelectSingleNode("s:controlURL",nsmgr);
			ControlURL=controlURLNode.FirstChild.Value;
			if(ControlURL.StartsWith("/"))
				ControlURL=ControlURL.Substring(1,ControlURL.Length-1);
			
			ControlURL=urlBase+ControlURL;

			XmlNode eventSubURLNode=serviceNode.SelectSingleNode("s:eventSubURL",nsmgr);
			EventSubURL=eventSubURLNode.FirstChild.Value;
			if(EventSubURL.StartsWith("/"))
				EventSubURL=EventSubURL.Substring(1,EventSubURL.Length-1);
			
			EventSubURL=urlBase+EventSubURL;

			System.Net.HttpWebRequest request=(System.Net.HttpWebRequest)System.Net.WebRequest.Create(SCPDURL);
			System.Net.HttpWebResponse response=(System.Net.HttpWebResponse)request.GetResponse();
			System.IO.Stream s=response.GetResponseStream();

			XmlDocument doc=new XmlDocument();
			doc.Load(s);

			XmlNamespaceManager nsmgr2 = new XmlNamespaceManager(doc.NameTable);
			nsmgr2.AddNamespace("s","urn:schemas-upnp-org:service-1-0");

			XmlNode rootNode=doc.DocumentElement;


			XmlNode statesNode=rootNode.SelectSingleNode("s:serviceStateTable",nsmgr2);
			XmlNodeList stateNodes=statesNode.SelectNodes("s:stateVariable",nsmgr2);
			foreach(XmlNode stateNode in stateNodes)
			{
				m_StateVariables.Add(new StateVariable(stateNode,nsmgr2,this));
			}

			XmlNode actionsNode=rootNode.SelectSingleNode("s:actionList",nsmgr2);
			XmlNodeList actionNodes=actionsNode.SelectNodes("s:action",nsmgr2);
			foreach(XmlNode actionNode in actionNodes)
			{
				m_Actions.Add(new Action(actionNode,nsmgr2,this));
			}


	
		}

		public int Subscribe(int duration,string callbackURL)
		{
			return Subscribe(duration,callbackURL,false);
		}

		public int Resubscribe(int duration)
		{
			return Subscribe(duration,null,true);
		}

		private int Subscribe(int duration,string callbackURL,bool renew)
		{
			HTTPRequest request=new HTTPRequest(EventSubURL);
			
			request.Method="SUBSCRIBE";

			if(!renew)
			{
				request.AddHeaderValue("CALLBACK","<"+callbackURL+">");
				request.AddHeaderValue("NT","upnp:event");
			}
			else
				request.AddHeaderValue("SID",SubscriptionID);

			request.AddHeaderValue("TIMEOUT","Second-"+duration.ToString());

			HTTPResponse response=request.GetResponse();

			string dur=null;
			if(response.GetHeaderValue("SID",ref SubscriptionID)&&
				response.GetHeaderValue("TIMEOUT",ref dur))
				SubscriptionDuration=int.Parse(dur.Substring(dur.IndexOf("-")+1));
			else
				throw new Exception("Invalid response");

			//Console.WriteLine(System.Text.Encoding.ASCII.GetString(request.GetBuffer()));
			//Console.WriteLine(System.Text.Encoding.ASCII.GetString(response.GetBuffer()));


			return SubscriptionDuration;
		}

		public void Unsubscribe()
		{
			HTTPRequest request=new HTTPRequest(EventSubURL);
			
			request.Method="UNSUBSCRIBE";

			request.AddHeaderValue("SID",SubscriptionID);
	
			request.GetResponse();

			SubscriptionID=null;
		}

		public string SubscriptionID;
		public int SubscriptionDuration;

		public string ServiceType;
		public string ServiceID;
		public string SCPDURL;
		public string ControlURL;
		public string EventSubURL;

		private ArrayList m_Actions=new ArrayList();
		public ArrayList Actions
		{
			get{return m_Actions;}
		}

		private ArrayList m_StateVariables=new ArrayList();
		public ArrayList StateVariables
		{
			get{return m_StateVariables;}
		}
	}

	public class Action
	{
		public Action(XmlNode actionNode,XmlNamespaceManager nsmgr,Service service)
		{
			m_Service=service;

			XmlNode nameNode=actionNode.SelectSingleNode("s:name",nsmgr);
			Name=nameNode.FirstChild.Value;
	
			try
			{
				XmlNode argsNode=actionNode.SelectSingleNode("s:argumentList",nsmgr);
				XmlNodeList argNodes=argsNode.SelectNodes("s:argument",nsmgr);
				foreach(XmlNode argNode in argNodes)
				{
					m_Arguments.Add(new Argument(argNode,nsmgr,service));
				}
			}
			catch(Exception)
			{
			}
		}

		public void Invoke(ref Argument[] args)
		{
			SoapAction action=new SoapAction(Name,m_Service.ServiceType);
		
			foreach(Argument arg in args)
			{
				if(arg.Direction!=Argument.DirectionMode.OUT)
				{
					action.Arguments.Add(new SoapArgument(arg.Name,arg.Value));
				}
			}

			SoapRequest request=new SoapRequest(m_Service.ControlURL,action);
			request.Method="POST";

			SoapResponse response=request.GetResponse();

			foreach(SoapArgument soapArg in response.ActionResult.Arguments)
			{
				foreach(Argument arg in args)
				{
					if(soapArg.Name==arg.Name)
						arg.Value=soapArg.Value;
				}
			}
			
			///TODO: lots of work to be done
		}

		public string Name;

		private Service m_Service;	
		
		private ArrayList m_Arguments=new ArrayList();
		public ArrayList Arguments
		{
			get{return m_Arguments;}
		}
	}

	public class Argument
	{
		public Argument()
		{
		}

		public Argument(XmlNode argumentNode,XmlNamespaceManager nsmgr,Service service)
		{
			XmlNode nameNode=argumentNode.SelectSingleNode("s:name",nsmgr);
			Name=nameNode.FirstChild.Value;

			XmlNode directionNode=argumentNode.SelectSingleNode("s:direction",nsmgr);
			Direction=(directionNode.FirstChild.Value=="in")?DirectionMode.IN:DirectionMode.OUT;

			XmlNode stateVarNode=argumentNode.SelectSingleNode("s:relatedStateVariable",nsmgr);
			string RelatedStateVariable=stateVarNode.FirstChild.Value;
			foreach(StateVariable var in service.StateVariables)
			{
				if(RelatedStateVariable==var.Name)
				{
					DataType=var.DataType;
					break;
				}
			}

		}		

		public enum DirectionMode{IN=0,OUT};

		public string Name;
		public string Value;
		public DirectionMode Direction;
		public string DataType;
		public string RelatedStateVariable;

	}

	public class StateVariable
	{
		public StateVariable()
		{
		}

		public StateVariable(XmlNode stateVariableNode,XmlNamespaceManager nsmgr,Service service)
		{
			m_Service=service;

			SendEvents=(stateVariableNode.Attributes["sendEvents"].Value=="yes")?true:false;

			XmlNode nameNode=stateVariableNode.SelectSingleNode("s:name",nsmgr);
			Name=nameNode.FirstChild.Value;

			XmlNode dataTypeNode=stateVariableNode.SelectSingleNode("s:dataType",nsmgr);
			DataType=dataTypeNode.FirstChild.Value;
		}

		public void Query()
		{
			SoapStateVariable var=new SoapStateVariable(Name,"");
	
			SoapRequest request=new SoapRequest(m_Service.ControlURL,var);
			request.Method="POST";

			SoapResponse response=request.GetResponse();

			Value=response.StateVariable.Value;
		}
		
		public string Name;
		public string Value;
		public string DataType;
		public bool SendEvents;

		private Service m_Service;	
	}
}
