using System.Net;

namespace UPnPStack
{
	public class SSDPListener : HTTPUDPListener
	{

		//multicast listener
		public SSDPListener()
		{
		}

		public SSDPListener(IPEndPoint localEP):base(localEP)
		{
		}

		public delegate void NotifyAliveMessageHandler(string nt,string usn,string location);
		public delegate void NotifyByeMessageHandler(string nt,string usn);
		public delegate void SearchMessageHandler(string searchTarget,int mx,IPEndPoint ep);
		public delegate void SearchResultMessageHandler(string st,string usn,string location);

		public event NotifyAliveMessageHandler OnNotifyAliveMessage;
		public event NotifyByeMessageHandler OnNotifyByeMessage;
		public event SearchMessageHandler OnSearchMessage;
		public event SearchResultMessageHandler OnSearchResultMessage;
		
		protected override void FireRequest(HTTPRequest request,IPEndPoint sourceEP)
		{
			base.FireRequest(request,sourceEP);

			OnRequest(request,sourceEP);
		}

		protected override void FireResponse(HTTPResponse response,IPEndPoint sourceEP)
		{
			base.FireResponse(response,sourceEP);

			OnResponse(response,sourceEP);
		}

		private void OnRequest(HTTPRequest request,IPEndPoint sourceEP)
		{
			if(request.Method=="M-SEARCH")
			{

				string man=null;
				if(request.GetHeaderValue("MAN",ref man)&&man=="\"ssdp:discover\"")
				{
					string searchTarget=null;
					string mx=null;
					if(request.GetHeaderValue("ST",ref searchTarget)&&
						request.GetHeaderValue("MX",ref mx))
						if(OnSearchMessage!=null)
							OnSearchMessage(searchTarget,int.Parse(mx),sourceEP);
				}
			}
			if(request.Method=="NOTIFY")
			{
				string nt=null;
				string nts=null;
				string usn=null;
				if(request.GetHeaderValue("NT",ref nt)&&
					request.GetHeaderValue("NTS",ref nts)&&
					request.GetHeaderValue("USN",ref usn))
				{
					if(nts=="ssdp:alive")
					{
						string location=null;
						if(request.GetHeaderValue("LOCATION",ref location)&&
							OnNotifyAliveMessage!=null)
							OnNotifyAliveMessage(nt,usn,location);
					}
					else if(nts=="ssdp:byebye")
					{
						if(OnNotifyByeMessage!=null)
							OnNotifyByeMessage(nt,usn);							
					}
				}


				//if(OnNotifyMessage!=null)
				//	OnNotifyMessage();
			}
		}

		private void OnResponse(HTTPResponse response,IPEndPoint sourceEP)
		{
			string st=null;
			string usn=null;
			string location=null;
			if( response.GetHeaderValue("ST",ref st)&&
				response.GetHeaderValue("USN",ref usn)&&
				response.GetHeaderValue("LOCATION",ref location))
			{
				if(OnSearchResultMessage!=null)
					OnSearchResultMessage(st,usn,location);
			}
			
		}

	}

	public class SSDPAliveMsg : HTTPRequest
	{
		public SSDPAliveMsg(int expiration,string rootURL,string nt,string usn)
		{
			Method="NOTIFY";
			URL="*";

			m_Host="239.255.255.250";
			m_Port=1900;

			AddHeaderValue("CACHE-CONTROL","max-age="+expiration.ToString());
			AddHeaderValue("LOCATION",rootURL);
			AddHeaderValue("NT",nt);
			AddHeaderValue("NTS","ssdp:alive");
			AddHeaderValue("SERVER","xp/1.0 UPnP/1.0 xxx/1.0");
			AddHeaderValue("USN",usn);
		}
	}

	public class SSDPByeMsg : HTTPRequest
	{
		public SSDPByeMsg(string nt,string usn)
		{
			Method="NOTIFY";
			URL="*";
	
			m_Host="239.255.255.250";
			m_Port=1900;

			AddHeaderValue("NT",nt);
			AddHeaderValue("NTS","ssdp:byebye");
			AddHeaderValue("USN",usn);
		}
	}

	public class SSDPSearchMsg : HTTPRequest
	{
		public SSDPSearchMsg(string st,int mx)
		{
			Method="M-SEARCH";
			URL="*";
		
			m_Host="239.255.255.250";
			m_Port=1900;

			AddHeaderValue("MAN","\"ssdp:discover\"");
			AddHeaderValue("MX",mx.ToString());
			AddHeaderValue("ST",st);
		}
	}

	public class SSDPSearchResponseMsg : HTTPResponse
	{
		public SSDPSearchResponseMsg(int expiration,string rootUrl,string st,string usn)
		{
			StatusCode=HttpStatusCode.OK;

			AddHeaderValue("CACHE-CONTROL","max-age="+expiration.ToString());
	//		AddHeaderValue("DATE",System.DateTime.UtcNow.ToString());
			AddHeaderValue("EXT"," ");
			AddHeaderValue("LOCATION",rootUrl);
			AddHeaderValue("SERVER","xp/1.0 UPnP/1.0 xxx/1.0");
			AddHeaderValue("ST",st);
			AddHeaderValue("USN",usn);

		}
	}

}
