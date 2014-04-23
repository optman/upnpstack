using System.Xml;
using System.Text;
using System.Collections;
using System.IO;
using System.Reflection;
using System.Net;
using System;

namespace UPnPStack
{
	public class SoapConst
	{
		 public static readonly string SOAP_ENVELOPE_NS="http://schemas.xmlsoap.org/soap/envelope/";
		 public static readonly string SOAP_ENCODING_NS="http://schemas.xmlsoap.org/soap/encoding/";
	}

	public class SoapRequest : HTTPRequest
	{
		public SoapRequest()
		{
		}

		public SoapRequest(string url,SoapAction action):base(url)
		{
			Action=action;
			m_Type=RequestType.Action;
			
			m_ContentType="text/xml";

			this.AddHeaderValue("SOAPACTION","\""+action.ServiceNS+"#"+action.Name+"\"");
		}

		public SoapRequest(string url,SoapStateVariable var):base(url)
		{
			StateVariable=var;
			m_Type=RequestType.QueryStateVariable;
		
			m_ContentType="text/xml";

			this.AddHeaderValue("SOAPACTION","\"urn:schemas-upnp-org:control-1-0#QueryStateVariable\"");
		}

		public SoapRequest(HTTPRequest request)
		{
			string soapActionType="";
			if(!request.GetHeaderValue("SOAPACTION",ref soapActionType))
				throw new Exception("Not a valid soap request!");

			if(soapActionType=="\"urn:schemas-upnp-org:control-1-0#QueryStateVariable\"")
				m_Type=RequestType.QueryStateVariable;
			else
				m_Type=RequestType.Action;

			XmlDocument doc=new XmlDocument();
			doc.LoadXml(Encoding.ASCII.GetString(request.Content));

			XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
			nsmgr.AddNamespace("s","http://schemas.xmlsoap.org/soap/envelope/");

			XmlElement rootNode=doc.DocumentElement;
			XmlElement bodyNode=(XmlElement)rootNode.SelectSingleNode("s:Body",nsmgr);
			XmlElement actionNode=(XmlElement)bodyNode.FirstChild;
		
			if(m_Type==RequestType.QueryStateVariable)
			{
				if(actionNode.LocalName!="QueryStateVariable")
					throw new Exception("Must be <u:QueryStateVariable...!");

				XmlNode variableNode=actionNode.FirstChild;				

				StateVariable=new SoapStateVariable(variableNode.FirstChild.Value,"");

			}
			else
			{
				Action=new SoapAction(actionNode.LocalName,actionNode.NamespaceURI);
                
				foreach(XmlNode argNode in actionNode.ChildNodes)
				{
					SoapArgument arg=new SoapArgument(argNode.LocalName,argNode.FirstChild.Value);
					Action.Arguments.Add(arg);
				}
			}
		
		}

		public override byte[] GetBuffer()
		{
			MemoryStream ms=new MemoryStream();

			XmlTextWriter writer=new XmlTextWriter(ms,Encoding.ASCII);
			writer.Formatting = Formatting.Indented;
			writer.Indentation = 4;
			writer.WriteStartDocument(false);


			//Envolope
			writer.WriteStartElement("s","Envelope",SoapConst.SOAP_ENVELOPE_NS);
			writer.WriteAttributeString("s","encodingStyle",SoapConst.SOAP_ENVELOPE_NS,SoapConst.SOAP_ENCODING_NS);

			//Body
			writer.WriteStartElement("s","Body",SoapConst.SOAP_ENVELOPE_NS);
			
			if(m_Type==RequestType.Action)
				WriteActionRequest(writer);
			else
				WriteQueryVarRequest(writer);
			
			//End Body
			writer.WriteEndElement();

			//End Envolope
			writer.WriteEndElement();

			writer.WriteEndDocument();

			writer.Flush();

			byte[] data=new byte[ms.Length];
			Array.Copy(ms.GetBuffer(),0,data,0,(int)ms.Length);
			
			writer.Close();

			Content=data;

			return base.GetBuffer();
		}

		private void WriteActionRequest(XmlTextWriter writer)
		{
			writer.WriteStartElement("u",Action.Name,Action.ServiceNS);
			foreach(SoapArgument arg in Action.Arguments)
			{
				writer.WriteElementString(arg.Name,arg.Value);
			}
			writer.WriteEndElement();
		}

		private void WriteQueryVarRequest(XmlTextWriter writer)
		{
			writer.WriteStartElement("u","QueryStateVariable","urn:schemas-upnp-org:control-1-0");
				writer.WriteElementString("varName","urn:schemas-upnp-org:control-1-0",StateVariable.Name);
			writer.WriteEndElement();
		}

		public new SoapResponse GetResponse()
		{
			return new SoapResponse(base.GetResponse());
		}

		public SoapAction Action;
		public SoapStateVariable StateVariable;

		public enum RequestType{Action=0,QueryStateVariable};

		private RequestType m_Type;
		public RequestType Type
		{
			get{return m_Type;}
			set{m_Type=value;}
		}

	}

	public class SoapResponse
	{
		public SoapResponse(int errorCode,string errorDescription)
		{
			m_ErrorCode=errorCode;
			m_ErrorDescription=errorDescription;

			//m_Success=false;
		}

		public SoapResponse(SoapActionResult actionResutl)
		{
			ActionResult=actionResutl;

			//m_Success=true;
		}

		public SoapResponse(SoapStateVariable stateVariable)
		{
			StateVariable=stateVariable;

			//m_Success=true;
		}

		public SoapResponse(HTTPResponse response)
		{
			if(response.StatusCode==HttpStatusCode.OK)
			{
				XmlDocument doc=new XmlDocument();
				doc.LoadXml(Encoding.ASCII.GetString(response.Content));

				XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
				nsmgr.AddNamespace("s","http://schemas.xmlsoap.org/soap/envelope/");
				nsmgr.AddNamespace("u","urn:schemas-upnp-org:control-1-0");

				XmlElement rootNode=doc.DocumentElement;
				XmlElement bodyNode=(XmlElement)rootNode.SelectSingleNode("s:Body",nsmgr);
				XmlElement queryNode=(XmlElement)bodyNode.SelectSingleNode("u:QueryStateVariableResponse",nsmgr);
				if(queryNode!=null)
				{
					string val="";
					if(queryNode.FirstChild.FirstChild!=null)
						val=queryNode.FirstChild.FirstChild.Value;

					StateVariable=new SoapStateVariable("",val);
				}
				else
				{
					XmlElement actionNode=(XmlElement)bodyNode.FirstChild;

					ActionResult=new SoapActionResult();
					foreach(XmlNode arg in actionNode.ChildNodes)
					{
						string val="";
						if(arg.FirstChild!=null)
							val=arg.FirstChild.Value;

						ActionResult.Arguments.Add(new SoapArgument(arg.LocalName,val));
					}
						
				}


			}
			else if(response.StatusCode==HttpStatusCode.InternalServerError)
			{
				throw new Exception("Internal Server Error");
			}
			else
				throw new Exception("bad response");

		}
		
		public byte[] GetBuffer()
		{
			MemoryStream ms=new MemoryStream();

			XmlTextWriter writer=new XmlTextWriter(ms,Encoding.ASCII);
			writer.Formatting = Formatting.Indented;
			writer.Indentation = 4;

			writer.WriteStartDocument(false);

			//Envolope
			writer.WriteStartElement("s","Envelope",SoapConst.SOAP_ENVELOPE_NS);
			writer.WriteAttributeString("s","encodingStyle",SoapConst.SOAP_ENVELOPE_NS,SoapConst.SOAP_ENCODING_NS);

				//Body
				writer.WriteStartElement("s","Body",SoapConst.SOAP_ENVELOPE_NS);
				
				if(ActionResult!=null)
					WriteActionResponse(writer);
				else if(StateVariable!=null)
					WriteStateVarResponse(writer);
				else
					WriteErrorResponse(writer);
				
				//End Body
				writer.WriteEndElement();

			//End Envolope
			writer.WriteEndElement();

			writer.WriteEndDocument();

			writer.Flush();

			byte[] data=new byte[ms.Length];
			Array.Copy(ms.GetBuffer(),0,data,0,(int)ms.Length);
			
			writer.Close();

			return data;
		}

		private void WriteStateVarResponse(XmlTextWriter writer)
		{
			//queryStateVariable response
			writer.WriteStartElement("u","QueryStateVariableResponse","urn:schemas-upnp-org:control-1-0");

			//state variable value
			writer.WriteElementString(StateVariable.Name,StateVariable.Value);

			writer.WriteEndElement();
		}

		private void WriteActionResponse(XmlTextWriter writer)
		{
			//action response
			writer.WriteStartElement("u",ActionResult.Name+"Response",ActionResult.ServiceNS);

			//out params
			foreach(SoapArgument arg in ActionResult.Arguments)
				writer.WriteElementString(arg.Name,arg.Value);

			writer.WriteEndElement();
		}

		private void WriteErrorResponse(XmlTextWriter writer)
		{
			writer.WriteStartElement("s","Fault",SoapConst.SOAP_ENVELOPE_NS);
				writer.WriteElementString("faultcode","s:Client");
				writer.WriteElementString("faultstring","UPnPError");

				writer.WriteStartElement("detail");
					writer.WriteStartElement("UPnPError","urn:schemas-upnp-org:control-1-0");
						writer.WriteElementString("errorCode",m_ErrorCode.ToString());
						writer.WriteElementString("errorDescription",m_ErrorDescription);
					writer.WriteEndElement();
				writer.WriteEndElement();

			writer.WriteEndElement();
		}

		public SoapStateVariable StateVariable;
		public SoapActionResult ActionResult;
//		private bool m_Success;
		private int m_ErrorCode;
		private string m_ErrorDescription;
	}

	public class SoapAction
	{
		public SoapAction(string name,string serviceNS)
		{
			Name=name;
			ServiceNS=serviceNS;
		}

		public string Name;
		public string ServiceNS;
		public ArrayList Arguments=new ArrayList();
	}

	public class SoapActionResult
	{
		public string Name;
		public string ServiceNS;
		public ArrayList Arguments=new ArrayList();
	}


	public class SoapArgument
	{
		public SoapArgument(string name,string val)
		{
			Name=name;
			Value=val;
		}

		public string Name;
		public string Value;
	}

	public class SoapStateVariable
	{
		public SoapStateVariable(string name,string val)
		{
			Name=name;
			Value=val;
		}

		public string Name;
		public string Value;
	}

	/// <summary>
	/// SoapHandler -- a plugin HTTPRequestHandler to deal with SOAP
	/// </summary>
	public class SoapHandler : HTTPRequestHandler
	{
		public SoapHandler()
		{
		}

		public virtual void Handle(HTTPRequest request,out HTTPResponse response)
		{
			response=new HTTPResponse();
			
			try
			{
				SoapRequest soapRequest=new SoapRequest(request);

				string serviceID="";
				request.GetParam("ServiceID",ref serviceID);

				object serviceObj=GetServiceObject(serviceID);
				if(serviceObj==null)
					throw new Exception("service not found!");

				if(soapRequest.Type==SoapRequest.RequestType.Action)
				{
					SoapActionResult actionResult;
					DoSoapAction(serviceObj,soapRequest.Action,out actionResult);
					response.Content=new SoapResponse(actionResult).GetBuffer();

				}
				else if(soapRequest.Type==SoapRequest.RequestType.QueryStateVariable)
				{
					SoapStateVariable queryResult;
					DoSoapQueryStateVariable(serviceObj,soapRequest.StateVariable,out queryResult);
					response.Content=new SoapResponse(queryResult).GetBuffer();
				}

				response.StatusCode=HttpStatusCode.OK;
				response.ContentType="text/xml";
				response.AddHeaderValue("EXT","");
				response.AddHeaderValue("SERVER","xp/1.0 UPnP/1.0 x/1.0");

			}
			catch(Exception e)
			{

				response.StatusCode=HttpStatusCode.InternalServerError;
				response.Content=new SoapResponse(501,e.Message).GetBuffer();
			}
		}

		private void DoSoapAction(object serviceObj,SoapAction action,out SoapActionResult result)
		{
			Type type=serviceObj.GetType();
			MethodInfo methodInfo=type.GetMethod(action.Name);
			if(methodInfo==null)
				throw new Exception("Method not found!");

			int i;
			ParameterInfo[] paramInfos=methodInfo.GetParameters();

			object[] parameters=new object[paramInfos.Length];
			
			i=0;
			foreach(ParameterInfo paramInfo in paramInfos)
			{
				if(!paramInfo.IsOut)
				{
					foreach(SoapArgument arg in action.Arguments)
					{
						if(arg.Name==paramInfo.Name)
						{
							parameters[i]=TypeConvert.StringToObject(paramInfo.ParameterType,arg.Value);
							break;
						}
					}
				}
				i++;
			}


			methodInfo.Invoke(serviceObj,parameters);

			//Set result
			result=new SoapActionResult();
			result.Name=action.Name;
			result.ServiceNS=action.ServiceNS;

			i=0;
			foreach(ParameterInfo paramInfo in paramInfos)
			{
				if(paramInfo.IsOut)
					result.Arguments.Add(new SoapArgument(paramInfo.Name,TypeConvert.ObjectToString(parameters[i])));
				i++;
			}
								
		}

		private void DoSoapQueryStateVariable(object serviceObj,SoapStateVariable var,out SoapStateVariable result)
		{
			Type type=serviceObj.GetType();
			PropertyInfo propInfo=type.GetProperty(var.Name);
			if(propInfo==null)
				throw new Exception("State Variable not found!");

			

			object propValue=propInfo.GetValue(serviceObj,null);

			result=new SoapStateVariable(var.Name,
						TypeConvert.ObjectToString(propValue));
		}


		private Hashtable m_ServiceObjects=new Hashtable();

		public void InsertServiceObject(string serviceID,object obj)
		{
			m_ServiceObjects.Add(serviceID,obj);
		}

		public object GetServiceObject(string serviceID)
		{
			return m_ServiceObjects[serviceID];
		}
	
	}
}
