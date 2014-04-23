using System.Xml;
using System.Text;
using System.Threading;
using System.IO;
using System.Collections;
using System.Net;
using System.Reflection;
using System;

namespace UPnPStack
{
	[AttributeUsage(AttributeTargets.Class,Inherited = true, AllowMultiple = false)]
	public class UPnPDeviceAttribute :Attribute
	{
		public UPnPDeviceAttribute()
		{
		}

		public string m_DeviceType;
		public string DeviceType
		{
			get{return m_DeviceType;}
			set{m_DeviceType=value;}
		}

		public string m_FriendlyName;
		public string FriendlyName
		{
			get{return m_FriendlyName;}
			set{m_FriendlyName=value;}
		}

		public string m_Manufacturer;
		public string Manufacturer
		{
			get{return m_Manufacturer;}
			set{m_Manufacturer=value;}
		}

		public string m_ManufacturerURL;
		public string ManufacturerURL
		{
			get{return m_ManufacturerURL;}
			set{m_ManufacturerURL=value;}
		}
		
		public string m_ModelDescription;
		public string ModelDescription
		{
			get{return m_ModelDescription;}
			set{m_ModelDescription=value;}
		}
		
		public string m_ModelName;
		public string ModelName
		{
			get{return m_ModelName;}
			set{m_ModelName=value;}
		}

		public string m_ModelNumber;
		public string ModelNumber
		{
			get{return m_ModelNumber;}
			set{m_ModelNumber=value;}
		}

		public string m_ModelURL;
		public string ModelURL
		{
			get{return m_ModelURL;}
			set{m_ModelURL=value;}
		}
		
		public int m_Expiration;
		public int Expiration
		{
			get{return m_Expiration;}
			set{m_Expiration=value;}
		}

	}


	public class Device
	{
		public Device()
		{	
			Expiration=1800;
			m_IsRootDevice=false;	
		
			DeviceID="uuid:"+Guid.NewGuid().ToString();

			//set device info
			SetDeviceInfo();

			//Create device services
			CreateServices();
		}

		public byte[] CetDescription()
		{
			MemoryStream  ms=new MemoryStream();
		
			XmlTextWriter writer=new XmlTextWriter(ms,Encoding.ASCII);
			
			writer.Formatting = Formatting.Indented;
			writer.Indentation = 4;

			writer.WriteStartDocument(false);
			
			//root
			writer.WriteStartElement("root","urn:schemas-upnp-org:device-1-0");
			//specVersion
			writer.WriteStartElement("specVersion");
			//major
			writer.WriteElementString("major","1");
			//minor
			writer.WriteElementString("minor","0");
			writer.WriteEndElement();
				
			//URLBase
			writer.WriteElementString("URLBase",URLBase);
				
			//device
			GetDeviceDescription(writer);

			//end root
			writer.WriteEndElement();

			writer.WriteEndDocument();
		
			writer.Flush();

			byte[] data=new byte[ms.Length];
			Array.Copy(ms.GetBuffer(),0,data,0,(int)ms.Length);

			writer.Close();

			return data;
		}

		public void GetDeviceDescription(XmlTextWriter writer)
		{
			//device
			writer.WriteStartElement("device");
			//deviceType
			writer.WriteElementString("deviceType",DeviceType);
			//friendlyName
			writer.WriteElementString("friendlyName",FriendlyName);
			//manufacturer
			writer.WriteElementString("manufacturer",Manufacturer);
			//manufacturerURL
			writer.WriteElementString("manufacturerURL",ManufacturerURL);
			//modelDescription
			writer.WriteElementString("modelDescription",ModelDescription);
			//modelName
			writer.WriteElementString("modelName",ModelName);
			//modelNumber
			writer.WriteElementString("modelNumber",ModelNumber);
			//modelURL
			writer.WriteElementString("modelURL",ModelURL);
			//UDN
			writer.WriteElementString("UDN",DeviceID);

			//serviceList
			writer.WriteStartElement("serviceList");
		
			foreach(Service service in m_Services)
			{

				GetServiceDescription(service,writer);

			}			

			//deviceList
			writer.WriteEndElement();

			//serviceList
			writer.WriteStartElement("deviceList");
		
			foreach(Device device in m_SubDevices)
			{

				device.GetDeviceDescription(writer);

			}			

			//end deviceList
			writer.WriteEndElement();
					
			//end device
			writer.WriteEndElement();
		}

		public void GetServiceDescription(Service service,XmlTextWriter writer)
		{	//service
			writer.WriteStartElement("service");
			//serviceType
			writer.WriteElementString("serviceType",service.ServiceType);
			//serviceId
			writer.WriteElementString("serviceId",service.ServiceID);
			//SCPDURL
			writer.WriteElementString("SCPDURL",service.SCPDURL);
			//controlURL
			writer.WriteElementString("controlURL","SoapHandler?ServiceID="+service.ServiceID);
			//eventSubURL
			writer.WriteElementString("eventSubURL","EventHandler?ServiceID="+service.ServiceID);							
			writer.WriteEndElement();	
		}

		private ArrayList m_Services=new ArrayList();
		public Service[] Services
		{
			get{return (Service[])m_Services.ToArray(typeof(Service));}
		}

		private void AddService(Service service)
		{
			m_Services.Add(service);
		}

		private Service GetService(string serviceID)
		{
			foreach(Service service in m_Services)
			{
				if(service.GetShortServiceID()==serviceID)
					return service;
			}

			return null;			
		}


		protected virtual void SetDeviceInfo()
		{
			Type t=this.GetType();

			UPnPDeviceAttribute deviceAttri=(UPnPDeviceAttribute)Attribute.GetCustomAttribute(t,typeof(UPnPDeviceAttribute));

			DeviceType=deviceAttri.DeviceType;
			FriendlyName=deviceAttri.FriendlyName;
			ManufacturerURL=deviceAttri.ManufacturerURL;
			Manufacturer=deviceAttri.Manufacturer;
			ModelDescription=deviceAttri.ModelDescription;
			ModelName=deviceAttri.ModelName;
			ModelNumber=deviceAttri.ModelNumber;
			ModelURL=deviceAttri.ModelURL;
			Expiration=deviceAttri.Expiration;
		}

		protected virtual void CreateServices()
		{
			Type t=this.GetType();

			//Create Services

			UPnPServiceAttribute[] serviceAttris=(UPnPServiceAttribute[])Attribute.GetCustomAttributes(t,typeof(UPnPServiceAttribute));
			foreach(UPnPServiceAttribute serviceAttri in serviceAttris)
			{
				Service service=new Service(serviceAttri.ServiceType,serviceAttri.ServiceID,this);

				AddService(service);
			}

			//Create Action

			MethodInfo[] methodInfos=t.GetMethods();
			foreach(MethodInfo methodInfo in methodInfos)
			{
				UPnPActionAttribute[] actionAttris=(UPnPActionAttribute[])Attribute.GetCustomAttributes(methodInfo,typeof(UPnPActionAttribute));
				foreach(UPnPActionAttribute actionAttri in actionAttris)
				{
					Service service=GetService(actionAttri.ServiceID);
					if(service!=null)
					{
						Action action=new Action(methodInfo.Name);
						service.AddAction(action);

						ParameterInfo[] paramInfos=methodInfo.GetParameters();
						int i=0;
						foreach(ParameterInfo paramInfo in paramInfos)
						{
							Argument arg=new Argument(paramInfo.Name,actionAttri.StateVariables[i],
								paramInfo.IsOut?Argument.DirectionMode.OUT:
								Argument.DirectionMode.IN);

							action.AddArgument(arg);

							i++;
						}

					}
				}
			}
			//Create StateVariable
			PropertyInfo[] propertyInfos=t.GetProperties();
			foreach(PropertyInfo propertyInfo in propertyInfos)
			{
				UPnPStateVariableAttribute[] stateVarAttris=(UPnPStateVariableAttribute[])Attribute.GetCustomAttributes(propertyInfo,typeof(UPnPStateVariableAttribute));
				foreach(UPnPStateVariableAttribute stateVarAttri in stateVarAttris)
				{
					Service service=GetService(stateVarAttri.ServiceID);
					if(service!=null)
					{
						StateVariable var=new StateVariable(propertyInfo.Name,TypeConvert.GetType(propertyInfo.PropertyType),stateVarAttri.Evented);
						service.AddStateVariable(var);
					}
				}
			}
	
		}

		private ArrayList m_SubDevices=new ArrayList();
		public Device[] SubDevices
		{
			get{return (Device[])m_SubDevices.ToArray(typeof(Device));}
		}

		public void AddSubDevice(Device device)
		{
			m_SubDevices.Add(device);
		}

		protected void FireStateChanged(params string[] vars)
		{
			ArrayList varsList=new ArrayList();

			foreach(string var in vars)
			{
				try
				{
					PropertyInfo propInfo=GetType().GetProperty(var);
					object result=propInfo.GetValue(this,null);
					varsList.Add(new NamedValue(var,result));
				}
				catch(Exception)
				{
				}
			}

			OnStateChanged(this,(NamedValue[])varsList.ToArray(typeof(NamedValue)));
		}



		//when any of the state variable have changed,fire the event
		public delegate void StateChangedHandler(Device device,NamedValue[] vars);
		public event StateChangedHandler OnStateChanged;

		public string RootURL;

		public string URLBase;
		public string DeviceIcon;
		public string DeviceType;
		public string FriendlyName;
		public string Manufacturer;
		public string ManufacturerURL;
		public string ModelDescription;
		public string ModelName;
		public string ModelNumber;
		public string ModelURL;
		public string PresentationURL;
		public string ProductCode;
		public string SerialNumber;
		public string DeviceID;
		public int Expiration;

		private bool m_IsRootDevice;
		public bool IsRootDevice
		{
			get{return m_IsRootDevice;}
			set{m_IsRootDevice=value;}
		}

	}
}
