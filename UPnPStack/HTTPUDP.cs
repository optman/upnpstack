using System.Net.Sockets;
using System.Net;
using System.Text;
using System.IO;
using System.Collections;
using System.Threading;
using System;
using log4net;


namespace UPnPStack
{
	public class HTTPUDPListener 
	{
		private static readonly ILog log=LogManager.GetLogger(typeof(HTTPUDPListener));


		private readonly int  DATAGRAM_MAX_SIZE=1024*10;

		public HTTPUDPListener()
		{
			m_LocalEndPoint=new IPEndPoint(IPAddress.Any,0);

			CreateSocket(false);
		}

		public HTTPUDPListener(IPEndPoint localEndPoint)
		{
			m_LocalEndPoint=localEndPoint;

			CreateSocket(true);
		}

		private void CreateSocket(bool multicast)
		{
			m_Socket = new Socket(AddressFamily.InterNetwork, 
				SocketType.Dgram, ProtocolType.Udp);

			if(multicast)
				m_Socket.SetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.ReuseAddress,
					1);

			m_Socket.Bind(new IPEndPoint(IPAddress.Any,m_LocalEndPoint.Port));

			if(multicast)
			{
				MulticastOption multicastOption = new MulticastOption(m_LocalEndPoint.Address);
				m_Socket.SetSocketOption(SocketOptionLevel.IP, 
					SocketOptionName.AddMembership, 
					multicastOption);
			}
		}


		protected void Finalize()
		{
		}


		public void Stop()
		{
			ProcessingMutex.WaitOne();
			ListenThread.Abort();
			ProcessingMutex.Close();

			// TODO: should not put it here
			m_Socket.Close();
		}
	
		public void Start()
		{
			ListenThread=new Thread(new ThreadStart(this.DoListen));
			ListenThread.Start();
		}

		private void DoListen()
		{
			ProcessingMutex=new Mutex();

			while(true)
			{

				EndPoint sourceEP=m_LocalEndPoint;

				byte[] buf=new byte[DATAGRAM_MAX_SIZE];

				int read=m_Socket.ReceiveFrom(buf,ref sourceEP);

				//enter processing
				ProcessingMutex.WaitOne();
		
				IPEndPoint sourceEP2=(IPEndPoint)sourceEP;

				byte[] data=new byte[read];
				
				Array.Copy(buf,data,read);
	
				//is this a request?
				try
				{
					HTTPRequest request=new HTTPRequest(data);
					FireRequest(request,sourceEP2);

					log.Debug(System.Text.Encoding.ASCII.GetString(request.GetBuffer()));

					goto nextloop;
				}
				catch(Exception)
				{
				}

				//or is this a response?
				try
				{
					HTTPResponse response=new HTTPResponse(data);
					FireResponse(response,sourceEP2);

					log.Debug(System.Text.Encoding.ASCII.GetString(response.GetBuffer()));

					goto nextloop;
				}
				catch(Exception)
				{
				}
			
				nextloop:
				//leave processing
				ProcessingMutex.ReleaseMutex();
			}
			
		}

		protected virtual void FireRequest(HTTPRequest request,IPEndPoint sourceEP)
		{
			if(OnNewRequest!=null)
				OnNewRequest(request,sourceEP);
		}

		protected virtual void FireResponse(HTTPResponse response,IPEndPoint sourceEP)
		{
			if(OnNewResponse!=null)
				OnNewResponse(response,sourceEP);
		}

		Socket m_Socket;
		public Socket Socket
		{
			get{return m_Socket;}
		}

		private Thread ListenThread;
		private Mutex ProcessingMutex;

		public delegate void RequestHandler(HTTPRequest request,IPEndPoint sourceEP);
		public event RequestHandler OnNewRequest;

		public delegate void ResponseHandler(HTTPResponse response,IPEndPoint sourceEP);
		public event ResponseHandler OnNewResponse;


		private IPEndPoint m_LocalEndPoint;
		public IPEndPoint LocalEndPoint
		{
			get{return (IPEndPoint)m_Socket.LocalEndPoint;}
		}
	}
}

namespace UPnPStack
{
	public class HTTPUDPSender : IDisposable
	{
		public HTTPUDPSender()
		{
			m_Socket = new Socket(AddressFamily.InterNetwork, 
				SocketType.Dgram, ProtocolType.Udp);

			m_Socket.Bind(new IPEndPoint(IPAddress.Any,0));

			m_Owned=true;
		}

		public HTTPUDPSender(Socket s,bool owned)
		{
			m_Socket=s;

			m_Owned=owned;
		}

		public void Dispose()
		{
			if(m_Owned)
				m_Socket.Close();
		}
	
		public void Send(IPEndPoint remoteEP,HTTPRequest request)
		{
	
			byte[] data=request.GetBuffer();
			
			m_Socket.SendTo(data,remoteEP);

			//Console.WriteLine("Send:\n{0}",System.Text.Encoding.ASCII.GetString(request.GetBuffer()));
		}

		public void Send(IPEndPoint remoteEP,HTTPResponse response)
		{
	
			byte[] data=response.GetBuffer();
			
			m_Socket.SendTo(data,remoteEP);

			//Console.WriteLine("Send:\n{0}",System.Text.Encoding.ASCII.GetString(response.GetBuffer()));
		}

		private Socket m_Socket;

		private bool m_Owned;

	}
}

