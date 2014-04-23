using System.Collections;
using System.Threading;
using System.Reflection;
using System.Net;
using System;

namespace UPnPStack
{
	public class NamedValue
	{
		public NamedValue(string name,object val)
		{
			Name=name;
			Value=val;
		}

		public string Name;
		public object Value;
	}

	public class SubscribeItem
	{
		private string m_SID;
		public string SID
		{
			get{return m_SID;}
			set{m_SID="uuid:"+value;}
		}
		public string ServiceID;
		public string CallbackURL;
		public int TimeLeftToExpire;
	}

	/// <summary>
	/// EventHandler -- a plugin HTTPRequestHanlder to deal with Event
	/// </summary>
	public class EventHandler : HTTPRequestHandler
	{
		private readonly int DefaultExpiration=1800;	//1800 seconds or 30 minutes
		private readonly int RefreshPeriod=10;			//10 seconds

		public EventHandler()
		{
			//refresh subscription status every 10 seconds
			new Timer(new TimerCallback(this.RefreshSubscription),null,RefreshPeriod*1000,RefreshPeriod*1000);
		}
		
		private void RefreshSubscription(object o)
		{
			//remove expired subscription
			foreach(SubscribeItem item in m_Subscriptions)
			{
				item.TimeLeftToExpire-=RefreshPeriod;

				if(item.TimeLeftToExpire<=0)
					m_Subscriptions.Remove(item);
			}
		}


		public virtual void Handle(HTTPRequest request,out HTTPResponse response)
		{
			response=new HTTPResponse();

			try
			{
				string method=request.Method;
				if(method=="SUBSCRIBE")
				{
					string nt="";
					//first time subscribe
					if(request.GetHeaderValue("NT",ref nt)&&nt=="upnp:event")
					{
						string callbackUrl="";
						if(!request.GetHeaderValue("CALLBACK",ref callbackUrl))
							throw new Exception("Bad request");

						//trip off "<>"
						callbackUrl=callbackUrl.Substring(1,callbackUrl.Length-2);

						string serviceID="";
						request.GetParam("ServiceID",ref serviceID);

						Subscribe(serviceID,callbackUrl,ref response);
					}
					//renew subscribe
					else
					{
						string sid="";
						if(!request.GetHeaderValue("SID",ref sid))
							throw new Exception("Bad request");

						Resubscribe(sid,ref response);

					}
				}
				else if(method=="UNSUBSCRIBE")
				{
					string sid="";
					if(!request.GetHeaderValue("SID",ref sid))
						throw new Exception("Bad request");

					RemoveSubscription(sid);
				}
				else
				{
					throw new Exception("Bad request");
				}
				
				response.StatusCode=HttpStatusCode.OK;
			}
			catch(Exception)
			{
				response.StatusCode=HttpStatusCode.PreconditionFailed;
			}

		}


		private ArrayList m_Subscriptions=new ArrayList();

		private void Subscribe(string serviceID,string callbackURL,ref HTTPResponse response)
		{
			if(GetService(serviceID)==null)
				throw new Exception("No such service");
	
			SubscribeItem item=new SubscribeItem();
			item.ServiceID=serviceID;
            item.CallbackURL=callbackURL;
			item.SID=Guid.NewGuid().ToString();
			item.TimeLeftToExpire=DefaultExpiration;

			m_Subscriptions.Add(item);

			response.AddHeaderValue("SID",item.SID.ToString());
			response.AddHeaderValue("TIMEOUT","Second-"+DefaultExpiration.ToString());

			//Send first event ,maybe fail if response is not send in 1 second
			//we should use more syn method instead of timer,that will wait the response
			//is actually send.
			InitEventObject obj=new InitEventObject();
			obj.SubItem=item;
			obj.timer=new Timer(new TimerCallback(this.SendInitEvent),obj,1000,Timeout.Infinite);

		}

		private void Resubscribe(string sid,ref HTTPResponse response)
		{
			SubscribeItem item=GetSubscription(sid);

			if(item==null)
				throw new Exception("Not such SID");

			item.TimeLeftToExpire=DefaultExpiration;
		
			response.AddHeaderValue("SID",item.SID.ToString());
			response.AddHeaderValue("TIMEOUT",DefaultExpiration.ToString());
		}

		private void Unsubscribe(string sid)
		{
			RemoveSubscription(sid);
		}

		private void OnDeviceStateChanged(Device device,NamedValue[] vars)
		{
			foreach(Service service in device.Services)
			{
				foreach(SubscribeItem item in m_Subscriptions)
				{
					if(item.ServiceID==service.ServiceID)
						NotifyEvent(item,vars);
				}
			}
		}

		private ArrayList m_Devices=new ArrayList();
		public void InsertDevice(Device device)
		{
			m_Devices.Add(device);

			device.OnStateChanged+=new Device.StateChangedHandler(this.OnDeviceStateChanged);
		}

		private Service GetService(string serviceID)
		{
			foreach(Device device in m_Devices)
			{
				foreach(Service service in device.Services)
				{
					if(service.ServiceID==serviceID)
						return service;
				}
			}

			return null;
		}

		private SubscribeItem GetSubscription(string sid)
		{
			foreach(SubscribeItem item in m_Subscriptions)
			{
				if(item.SID==sid)
					return item;
			}

			return null;
		}

		private void RemoveSubscription(string sid)
		{
			foreach(SubscribeItem item in m_Subscriptions)
			{
				if(item.SID==sid)
				{
					m_Subscriptions.Remove(item);
					return;
				}
			}

			throw new Exception("Can't unsubscribe,no such subscription!");
		}


		private class InitEventObject
		{
			public SubscribeItem SubItem;
			public Timer timer;
		}

		private void SendInitEvent(object o)
		{
			InitEventObject obj=(InitEventObject)o;

			NotifyEvent(obj.SubItem,GetStateVariables(GetService(obj.SubItem.ServiceID)));

			obj.timer.Dispose();
			obj.timer=null;
		}

		private void NotifyEvent(SubscribeItem item,NamedValue[] vars)
		{
			if(vars==null||vars.Length==0)
				return;

			HTTPRequest request=new HTTPRequest(item.CallbackURL);
			request.Method="NOTIFY";
			request.ContentType="text/xml";
			request.AddHeaderValue("NT","upnp:event");
			request.AddHeaderValue("NTS","upnp:propchange");
			request.AddHeaderValue("SID",item.SID);
			request.AddHeaderValue("SEQ","0");
	

			System.IO.MemoryStream ms=new System.IO.MemoryStream();
			System.Xml.XmlTextWriter writer=new System.Xml.XmlTextWriter(ms,System.Text.Encoding.ASCII);
			writer.Formatting = System.Xml.Formatting.Indented;
			writer.Indentation = 4;

			writer.WriteStartDocument(false);

			//propertyset
			writer.WriteStartElement("e","propertyset","urn:schemas-upnp-org:event-1-0");

		
			foreach(NamedValue var in vars)
			{
				//property
				writer.WriteStartElement("e","property","urn:schemas-upnp-org:event-1-0");
				
				
				writer.WriteElementString(var.Name,TypeConvert.ObjectToString(var.Value));
			
				//End property
				writer.WriteEndElement();
			}

			//End propertyset
			writer.WriteEndElement();
			
			writer.WriteEndDocument();

			writer.Flush();

			byte[] data=new byte[ms.Length];
			Array.Copy(ms.GetBuffer(),0,data,0,(int)ms.Length);

			writer.Close();

			request.Content=data;
	
			request.GetResponse();
		}

		private NamedValue GetStateVariable(Service service,string var)
		{
			try
			{
				Type t=service.HostDevice.GetType();
				PropertyInfo propInfo=t.GetProperty(var);
				object result=propInfo.GetValue(service.HostDevice,null);
				return new NamedValue(var,result);
			}
			catch(Exception)
			{
				return null;
			}
						
		}

		private NamedValue[] GetStateVariables(Service service)
		{
			if(service==null)
				return null;
	
			ArrayList values=new ArrayList();
			
			foreach(StateVariable var in service.StateVariables)
			{
				if(var.Evented)
				{
					try
					{
						values.Add(GetStateVariable(service,var.Name));
					}
					catch(Exception)
					{

					}
				}
			}	

			return (NamedValue[])values.ToArray(typeof(NamedValue));
		}
	}
}
