using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading;
using System.IO;
using System.Collections;
using System;

namespace UPnPStack
{

	public interface HTTPRequestHandler
	{
		void Handle(HTTPRequest request,out HTTPResponse response);
	}

	/// <summary>
	/// HTTPRequest -- Http Request Object
	/// </summary>
	public class HTTPRequest
	{

		public HTTPRequest()
		{
		}

		public HTTPRequest(string url)
		{
			try
			{
				string protocol=url.Substring(0,url.IndexOf("://",0));
				url=url.Substring(protocol.Length+3,url.Length-protocol.Length-3);
				if(url.IndexOf(":")<0)
				{
					m_Host=url.Substring(0,url.IndexOf("/"));
					m_Port=80;
				}
				else
				{
					m_Host=url.Substring(0,url.IndexOf(":"));
					m_Port=int.Parse(url.Substring(m_Host.Length+1,url.IndexOf("/")-m_Host.Length-1));
				}

				m_URL=url.Substring(url.IndexOf("/"));
			}
			catch(Exception)
			{
			}
		}

		public HTTPRequest(byte[] data)
		{
			Parse(new StringReader(Encoding.ASCII.GetString(data)));
		}

		public HTTPRequest(Socket socket)
		{
			Parse(new StreamReader(new NetworkStream(socket,false)));
		}

		private void Parse(TextReader reader)
		{

			string line;
			string[] parts;
			
			//first line
			line=reader.ReadLine();
			parts=line.Split(new char[]{' '},4);
            
			//check http version
			if(parts[2].Trim()!="HTTP/1.1"&&parts[2].Trim()!="HTTP/1.0")
				throw new Exception("Not a valid HTTP request!");

		
			//command
			m_Method=parts[0];

			int index=parts[1].IndexOf("?");
			//URL
			if(index<0)
				m_URL=parts[1];
			else
			{
				m_URL=parts[1].Substring(0,index);

				//Params
				try
				{	
					//Decode url string
					parts[1]=parts[1].Replace("%26","&");

					string[] subParts=parts[1].Substring(index+1).Split(new char[]{'&'});
					foreach(string param in subParts)
					{
						string[] pairs=param.Split(new char[]{'='},2);
						AddParam(pairs[0],pairs[1]);
					}
				}
				catch(Exception)
				{
				}
			}



			string contentLength=null;

			//Header values
			while((line=reader.ReadLine())!=null)
			{
				if(line==string.Empty)
					break;
				string[] pair=line.Split(new char[]{':'},2);
				pair[0]=pair[0].Trim().ToUpper();
				pair[1]=pair[1].Trim();

				
				if(pair[0]=="CONTENT-LENGTH")
					contentLength=pair[1];
				else if(pair[0]=="CONTENT-TYPE")
					this.m_ContentType=pair[1];
				else if(pair[0]=="HOST")
				{
					if(pair[1].IndexOf(":")<0)
					{
						m_Host=pair[1];
						m_Port=80;
					}
					else
					{
						m_Host=pair[1].Substring(0,pair[1].IndexOf(":"));
						m_Port=int.Parse(pair[1].Substring(m_Host.Length+1));
					}

				}

				else
					AddHeaderValue(pair[0],pair[1]);
			}

			//Content
			if(contentLength!=null)
			{
				char[] content=new char[int.Parse(contentLength)];
				reader.ReadBlock(content,0,content.Length);
				if(content.Length>0)
					Content=Encoding.ASCII.GetBytes(content);
			}
					
			
		}

		private string m_Method;
		public string Method
		{
			get{return m_Method;}
			set{m_Method=value;}
		}

		protected string m_Host;
		public string Host
		{
			get{return m_Host;}
		}

		protected int m_Port;
		public int Port
		{
			get{return Port;}
		}

		private string m_URL;
		public string URL
		{
			get{return m_URL;}
			set{m_URL=value;}
		}
		private Hashtable m_Params=new Hashtable();
		public bool GetParam(string name,ref string val)
		{
			string tempVal=(string)m_Params[name.ToLower()];
		
			if(tempVal==null)
				return false;

			val=tempVal;
			return true;
		}
		public void AddParam(string name,string val)
		{
			m_Params.Add(name.ToLower(),val);
		}
		private Hashtable m_HeaderValues=new Hashtable();
		public bool GetHeaderValue(string name,ref string val)
		{
			string tempVal=(string)m_HeaderValues[name.ToUpper()];
		
			if(tempVal==null)
				return false;

			val=tempVal;
			return true;
		}
		public void AddHeaderValue(string name,string val)
		{
			m_HeaderValues.Add(name.ToUpper(),val);
		}

		byte[] m_Content;
		public byte[] Content
		{
			get{return m_Content;}
			set{m_Content=value;}
		}

		public string m_ContentType;
		public string ContentType
		{
			get{return m_ContentType;}
			set{m_ContentType=value;}
		}

		public virtual byte[] GetBuffer()
		{
			MemoryStream s=new MemoryStream();

			StringBuilder header=new StringBuilder();

			//first line
			header.Append(m_Method);
			header.Append(" ");
			header.Append(m_URL);
			header.Append(" ");
			header.Append("HTTP/1.1");
			header.Append("\r\n");

			header.Append("HOST: ");
			header.Append(m_Host);
			header.Append(":");
			header.Append(m_Port);
			header.Append("\r\n");

			//other header value
			IDictionaryEnumerator enumerator = m_HeaderValues.GetEnumerator();
			while ( enumerator.MoveNext() )
			{
				header.Append((string)enumerator.Key);
				header.Append(": ");
				header.Append((string)enumerator.Value);
				header.Append("\r\n");
			}
			
			//content length
			int contentSize=0;
			if(m_Content!=null)
				contentSize=m_Content.Length;
		
			header.Append("CONTENT-TYPE: "+m_ContentType);
			header.Append("\r\n");

			header.Append("CONTENT-LENGTH: "+contentSize);
			header.Append("\r\n");

			//blank line
			header.Append("\r\n");

			//write header
			byte[] buf=Encoding.ASCII.GetBytes(header.ToString());
			s.Write(buf,0,buf.Length);

			//Write content
			if(m_Content!=null)
				s.Write(m_Content,0,m_Content.Length);

			return s.GetBuffer();
		}

		public HTTPResponse GetResponse()
		{
			TcpClient client=new TcpClient();
			client.Connect(m_Host,m_Port);
			byte[] data=GetBuffer();
			client.GetStream().Write(data,0,data.Length);

			client.GetStream().Flush();

			return new HTTPResponse(client.GetStream());
		}
	}

	/// <summary>
	/// HTTPResponse -- Http Response Object
	/// </summary>
	public class HTTPResponse
	{
		public HTTPResponse()
		{
			m_StatusCode=HttpStatusCode.OK;
		}

		public HTTPResponse(byte[] data)
		{	
			Parse(new StringReader(Encoding.ASCII.GetString(data)));
		}

		public HTTPResponse(Socket socket)
		{
			Parse(new StreamReader(new NetworkStream(socket,false)));
		}

		public HTTPResponse(Stream s)
		{
			Parse(new StreamReader(s));
		}

		private void Parse(TextReader reader)
		{
	
			// TODO: parse the data
			string line;
			string[] parts;
			
			//first line
			line=reader.ReadLine();
			parts=line.Split(new char[]{' '},3);
            
			//check http version
			if(parts[0].Trim()!="HTTP/1.1"&&parts[0].Trim()!="HTTP/1.0")
				throw new Exception("Not a valid HTTP response!");
		
			//command
			m_StatusCode=(HttpStatusCode)Enum.Parse(typeof(HttpStatusCode),parts[1].Trim());

			string contentLength=null;

			//Header values
			while((line=reader.ReadLine())!=null)
			{
				if(line==string.Empty)
					break;
				string[] pair=line.Split(new char[]{':'},2);
				pair[0]=pair[0].Trim().ToUpper();
				pair[1]=pair[1].Trim();

				
				if(pair[0]=="CONTENT-LENGTH")
					contentLength=pair[1];
				else if(pair[0]=="CONTENT-TYPE")
					this.m_ContentType=pair[1];
				else
					AddHeaderValue(pair[0],pair[1]);
			}

			//Content
			if(contentLength!=null)
			{
				char[] content=new char[int.Parse(contentLength)];
				reader.ReadBlock(content,0,content.Length);
				if(content.Length>0)
					Content=Encoding.ASCII.GetBytes(content);
			}
						
		}

		private Hashtable m_HeaderValues=new Hashtable();
		public bool GetHeaderValue(string name,ref string val)
		{
			val=(string)m_HeaderValues[name.ToUpper()];
		
			if(val==null)
				return false;

			return true;
		}
		public void AddHeaderValue(string name,string val)
		{
			m_HeaderValues.Add(name.ToUpper(),val);
		}

		public byte[] GetBuffer()
		{
			MemoryStream s=new MemoryStream();

			StringBuilder header=new StringBuilder();

			//first line
			header.Append("HTTP/1.1");
			header.Append(" ");
			header.Append(((int)m_StatusCode).ToString());
			header.Append(" ");
			header.Append(m_StatusCode.ToString());
			header.Append("\r\n");

			//other header value
			IDictionaryEnumerator enumerator = m_HeaderValues.GetEnumerator();
			while ( enumerator.MoveNext() )
			{
				header.Append((string)enumerator.Key);
				header.Append(": ");
				header.Append((string)enumerator.Value);
				header.Append("\r\n");
			}
			
			//content length
			int contentSize=0;
			if(m_Content!=null)
				contentSize=m_Content.Length;

			header.Append("CONTENT-TYPE: "+m_ContentType);
			header.Append("\r\n");

			header.Append("CONTENT-LENGTH: "+contentSize);
			header.Append("\r\n");

			//blank line
			header.Append("\r\n");

			//write header
			byte[] buf=Encoding.ASCII.GetBytes(header.ToString());
			s.Write(buf,0,buf.Length);

			//Write content
			if(m_Content!=null)
				s.Write(m_Content,0,m_Content.Length);


			return s.GetBuffer();
		}

		public string m_ContentType;
		public string ContentType
		{
			get{return m_ContentType;}
			set{m_ContentType=value;}
		}

		private byte[] m_Content;
		public byte[] Content
		{
			get{return m_Content;}
			set{m_Content=value;}
		}

		private HttpStatusCode m_StatusCode;
		public HttpStatusCode StatusCode
		{
			get{return m_StatusCode;}
			set{m_StatusCode=value;}
		}

	}


	/// <summary>
	/// HTTPServer -- a simle Web Server
	/// </summary>
	public class HTTPServer
	{
		TcpListener m_Listener;
		Thread m_ListenThread;

		public HTTPServer(int port,string baseFolder)
		{
			m_BaseFolder=baseFolder;

			m_Listener=new TcpListener(port);
		}

		public HTTPServer(string baseFolder)
		{
			m_BaseFolder=baseFolder;

			Random random=new Random();
			while(true)
			{
				try
				{
					m_Listener=new TcpListener(random.Next()/10000);
					break;
				}
				catch(ArgumentOutOfRangeException)
				{
				}
				catch(SocketException)
				{
				}
			}
		}

		public void Stop()
		{
			m_ListenThread.Abort();

			m_Listener.Stop();
		}
	
		public void Start()
		{
	
			m_ListenThread=new Thread(new ThreadStart(this.DoListen));
			m_ListenThread.Start();


		}

		protected void DoListen()
		{
			m_Listener.Start();
	
			while(true)
			{
				Socket client=m_Listener.AcceptSocket();

				HandleRequest(ref client);

				client.Close();
			}
		}


		protected HTTPRequest GetRequest(ref Socket client)
		{
			return new HTTPRequest(client);			
		}

		protected void HandleRequest(ref Socket client)
		{
			HTTPRequest request=GetRequest(ref client);
			HTTPResponse response;

			//any plugin handler do the job?
			HTTPRequestHandler handler;
			if(GetRequestHandler(request.URL,out handler))
			{
				handler.Handle(request,out response);
				if(response!=null)
					client.Send(response.GetBuffer());

				return;
			}


			byte[] data;
			response=new HTTPResponse();
			response.AddHeaderValue("Server","UPnP Web Server");
			response.StatusCode=HttpStatusCode.OK;

            if(!GetCacheItem(request.URL,out data))
			{
				try
				{
					FileStream fs=new FileStream(MakeLocalPath(request.URL),FileMode.Open,FileAccess.Read,FileShare.Read);

					BinaryReader reader=new BinaryReader(fs);
					data=new Byte[fs.Length];
					reader.Read(data,0,data.Length);
					reader.Close();
					fs.Close();
				}
				catch(FileNotFoundException)
				{
					response.StatusCode=HttpStatusCode.NotFound;
				}
				catch(DirectoryNotFoundException)
				{
					response.StatusCode=HttpStatusCode.NotFound;
				}
				catch(Exception)
				{
					response.StatusCode=HttpStatusCode.NotFound;
				}
			}

			if(data!=null)
			{
				response.ContentType=MakeMIMEType(request.URL);
				//response.AddHeaderValue("Accept-Ranges","bytes");

				response.Content=data;
			}

			//Send response
			client.Send(response.GetBuffer());
		}

		protected string MakeLocalPath(string url)
		{
			return m_BaseFolder+url;
		}

		protected string MakeMIMEType(string url)
		{
			int index=url.LastIndexOf('.')+1;
			string ext=url.Substring(index,url.Length-index).ToUpper();
			if(ext=="HTM"||ext=="HTML")
				return "text/html";
			else if(ext=="XML")
				return "text/xml";
			else if(ext=="JPG"||ext=="JPEG")
				return "image/jpeg";
			else if(ext=="BMP")
				return "image/bmp";
			else if(ext=="PNG")
				return "image/png";
			else if(ext=="GIF")
				return "image/gif";
			else
				return "text/html";

		}

		public Hashtable CacheItems=new Hashtable();

		public void InsertCacheItem(string name,byte[] data)
		{
			CacheItems.Add(name,data);
		}

		public void RemoveCacheItem(string name)
		{
			CacheItems.Remove(name);
		}

		private bool GetCacheItem(string name,out byte[] data)
		{
			data=(byte[])CacheItems[name];

			if(data==null)
                return false;

			return true;
		}
	
		private Hashtable RequestHandlers=new Hashtable();
		
		public void AddRequestHandler(string name,HTTPRequestHandler handler)
		{
			RequestHandlers.Add(name,handler);
		}

		public void RemoveRequestHandler(string name)
		{
			RequestHandlers.Remove(name);
		}

		public bool GetRequestHandler(string name,out HTTPRequestHandler hanlder)
		{
			hanlder=(HTTPRequestHandler)RequestHandlers[name];

			if(hanlder==null)
				return false;

			return true;
		}

		public 	int Port
		{
			get	{return ((IPEndPoint)m_Listener.LocalEndpoint).Port;}
		}

		public 	string Host
		{
			get	
			{
				System.Collections.IEnumerator addressList=System.Net.Dns.GetHostByName("").AddressList.GetEnumerator();
				addressList.MoveNext();
				return ((IPAddress)addressList.Current).ToString();
			}
		}

		protected string m_BaseFolder;
		public string BaseFolder
		{
			get{return m_BaseFolder;}
		}
	}
}
