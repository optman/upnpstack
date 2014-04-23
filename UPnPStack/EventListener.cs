using System.Xml;
using System.Collections;
using System.Net;
using System;

namespace UPnPStack.CP
{
	/// <summary>
	/// Summary description for EventListener.
	/// </summary>
	public class EventListener : HTTPRequestHandler
	{
		public EventListener()
		{
			m_HttpServer=new HTTPServer("");
			
			m_EventSubURL="http://"+m_HttpServer.Host+":"+m_HttpServer.Port+"/event";

			m_HttpServer.AddRequestHandler("/event",this);

		}

		public void Start()
		{
			m_HttpServer.Start();
		}

		public void Stop()
		{
			m_HttpServer.Stop();
		}

		public string GetEventHandler(string cookie)
		{
			return m_EventSubURL+"?cookie="+cookie;
		}

		public void Handle(HTTPRequest request,out HTTPResponse response)
		{
			response=new HTTPResponse();
			response.StatusCode=HttpStatusCode.OK;

			string cookie=null;

			string NULL=null;
			if(request.GetParam("cookie",ref cookie)&&
				request.GetHeaderValue("NT",ref NULL)&&
				request.GetHeaderValue("NTS",ref NULL)&&
				request.GetHeaderValue("SID",ref NULL)&&
				request.GetHeaderValue("SEQ",ref NULL))
			{

				

				XmlDocument doc=new XmlDocument();

				XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
				nsmgr.AddNamespace("e","urn:schemas-upnp-org:event-1-0");

				doc.LoadXml(System.Text.Encoding.ASCII.GetString(request.Content));
			
				XmlNode rootNode=doc.DocumentElement;
		
				StateVariable[] vars=new StateVariable[rootNode.ChildNodes.Count];

				int i=0;
				foreach(XmlNode varNode in rootNode.ChildNodes)
				{
	
					vars[i]=new StateVariable();
					vars[i].Name=varNode.FirstChild.LocalName;
					vars[i].Value=varNode.FirstChild.FirstChild.Value;

					i++;
				}

				if(OnEvent!=null)
					OnEvent(vars,cookie);
			}
		}


		public delegate void EventHandler(StateVariable[] vars,string cookie);
		public event EventHandler OnEvent;

		private HTTPServer m_HttpServer;
		private string m_EventSubURL;
	}
}
