using System.IO;
using System.Xml;
using System.Text;
using System.Collections;
using System;


namespace UPnPStack
{
	[AttributeUsage(AttributeTargets.Class,Inherited = true, AllowMultiple = true)]
	public class UPnPServiceAttribute : System.Attribute
	{
		public UPnPServiceAttribute(string serviceType,string serviceID)
		{
			m_ServiceType=serviceType;
			m_ServiceID=serviceID;
		}

		private string m_ServiceType;
		public string ServiceType
		{
			get{return m_ServiceType;}
		}

		private string m_ServiceID;
		public string ServiceID
		{
			get{return m_ServiceID;}
		}
	}
	
	[AttributeUsage(AttributeTargets.Method, AllowMultiple = true)]
	public class UPnPActionAttribute : System.Attribute
	{
		public UPnPActionAttribute(string serviceID,params string[] stateVariables)
		{
			m_ServiceID=serviceID;
			m_StateVariables=stateVariables;
		}

		private string m_ServiceID;
		public string ServiceID
		{
			get{return m_ServiceID;}
		}

		private string[] m_StateVariables;
		public string[] StateVariables
		{
			get{return m_StateVariables;}
		}

	}

	[AttributeUsage(AttributeTargets.Property, AllowMultiple = true)]
	public class UPnPStateVariableAttribute : System.Attribute
	{
		public UPnPStateVariableAttribute(string serviceID,bool evented)
		{
			m_ServiceID=serviceID;
			m_Evented=evented;
		}

		private string m_ServiceID;
		public string ServiceID
		{
			get{return m_ServiceID;}
		}

		private bool m_Evented;
		public bool Evented
		{
			get{return m_Evented;}
		}
	}




	public class Argument
	{
		public Argument(string name,string stateVar,DirectionMode direction)
		{
			Name=name;
			RelatedStateVar=stateVar;
			Direction=direction;
		}

		public enum DirectionMode{IN=0,OUT};

		public string Name;

		public DirectionMode Direction;

		public string RelatedStateVar;

	}

	public class Action
	{
		public Action(string name)
		{
			Name=name;
		}

		public string Name;
		public ArrayList Arguments=new ArrayList();

		public void AddArgument(Argument arg)
		{
			Arguments.Add(arg);
		}
	}

	public class StateVariable
	{
		public StateVariable(string name,string type,bool evented)
		{
			Name=name;
			Type=type;
			Evented=evented;
		}

		public string Name;
		public string Type;
		public bool  Evented;
	}

	/// <summary>
	/// Service
	/// </summary>
	public class Service
	{
		public Service(string serviceType,string serviceID,Device hostDevice)
		{
			m_ServiceType=serviceType;
			m_ShortServiceID=serviceID;
			m_HostDevice=hostDevice;

			m_ServiceID_Postfix=Guid.NewGuid().ToString();
		}

		private string m_ServiceID_Postfix;
		
		private string m_ShortServiceID;
		//private string m_ServiceID;
		public string ServiceID
		{
			get{return "urn:upnp-org:serviceId:"+m_ShortServiceID+"-"+m_ServiceID_Postfix;}
		}

		private string m_ServiceType;
		public string ServiceType
		{
			get{return m_ServiceType;}
		}

		private Device m_HostDevice;
		public Device HostDevice
		{
			get{return m_HostDevice;}
		}

		private ArrayList m_Actions=new ArrayList();
		public Action[] Actions
		{
			get{return (Action[])m_Actions.ToArray(typeof(Action));}
		}

		public void AddAction(Action action)
		{
			m_Actions.Add(action);
		}

		private ArrayList m_StateVars=new ArrayList();
		public StateVariable[] StateVariables
		{
			get{return (StateVariable[])m_StateVars.ToArray(typeof(StateVariable));}
		}

		public void AddStateVariable(StateVariable stateVar)
		{
			m_StateVars.Add(stateVar);
		}

		private void WriteActionDes(Action action,XmlTextWriter writer)
		{
			writer.WriteStartElement("action");
			
				writer.WriteElementString("name",action.Name);

				writer.WriteStartElement("argumentList");
					foreach(Argument arg in action.Arguments)
					{
						writer.WriteStartElement("argument");
							writer.WriteElementString("name",arg.Name);
							writer.WriteElementString("direction",arg.Direction==Argument.DirectionMode.IN?"in":"out");
							writer.WriteElementString("relatedStateVariable",arg.RelatedStateVar);
						writer.WriteEndElement();
					}
				writer.WriteEndElement();

			writer.WriteEndElement();
		}

		private void WriteStateVariableDes(StateVariable stateVar,XmlTextWriter writer)
		{
			writer.WriteStartElement("stateVariable");

			writer.WriteAttributeString("sendEvents",stateVar.Evented?"yes":"no");
			
			writer.WriteElementString("name",stateVar.Name);
			writer.WriteElementString("dataType",stateVar.Type);

			writer.WriteEndElement();
		}

		public byte[] GetDescription()
		{
			MemoryStream  ms=new MemoryStream();

			XmlTextWriter writer=new XmlTextWriter(ms,Encoding.ASCII);
			writer.Formatting = Formatting.Indented;
			writer.Indentation = 4;

			//Document
			writer.WriteStartDocument(false);
			
			//scpd
			writer.WriteStartElement("scpd","urn:schemas-upnp-org:service-1-0");
			//specVersion
			writer.WriteStartElement("specVersion");
			//major
			writer.WriteElementString("major","1");
			//minor
			writer.WriteElementString("minor","0");
			//End specVersion
			writer.WriteEndElement();

			//actionList
			writer.WriteStartElement("actionList");
			///actions
			foreach(Action action in Actions)
				WriteActionDes(action,writer);
			//End actionList
			writer.WriteEndElement();

			//serviceStateTable
			writer.WriteStartElement("serviceStateTable");
			///stateVariables
			foreach(StateVariable stateVar in m_StateVars)
				WriteStateVariableDes(stateVar,writer);
			//End serviceStateTable
			writer.WriteEndElement();
				
			//End scpd
			writer.WriteEndElement();

			//End Doctument
			writer.WriteEndDocument();

			writer.Flush();

			byte[] data=new byte[ms.Length];
			Array.Copy(ms.GetBuffer(),0,data,0,(int)ms.Length);
		
			writer.Close();
		


			return data;
		}

		public string GetShortServiceID()
		{
			return m_ShortServiceID;
		}

		public string SCPDURL
		{
			get{return ServiceID+"_scpd.xml";}
		}
	}
}
