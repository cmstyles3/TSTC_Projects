

#ifndef SOCKET_OBJECT

#define SOCKET_OBJECT

// Windows Sockets Include, Also need ws2_32.lib included in project
#include <winsock2.h>
#include <stdio.h>

struct stPacketHeader
{
	int		iType;
	int		iLength;
	int		iID;
	int		iCheckSum;
	int		iSender;
};

struct SocketTransmissionStruct 
{
	SOCKET		skSocket;
	char		szBuffer[64000];
	int			iWritePos;
	int			iReadPos;
	int			iTerminate;
};


typedef struct SocketType
{
	int iStatus;
	SOCKET skSocket;
	SocketTransmissionStruct stReceive;
	SocketTransmissionStruct stSend;
	DWORD dwReceiveHandle;
	DWORD dwSendHandle;
}SOCKETTYPE, *PSOCKET;

void InitSocket(PSOCKET Socket);
bool Accept(PSOCKET Server, PSOCKET Client);
int Listen(PSOCKET Server);
int Bind(PSOCKET Server, int iPort);
void Disconnect(PSOCKET Socket);
bool Connect(PSOCKET Client, char* szServerAddress, int iPort);
int Recv(PSOCKET Socket, char *szBuffer, int iBufLen, int iFlags);
int Send(PSOCKET Socket, char *szBuffer, int iBufLen, int iFlags);
int iCalculateChecksum(stPacketHeader stHead);
void thrReceiveThread(SocketTransmissionStruct *rs);
void thrSendThread(SocketTransmissionStruct &rs);
int vGetPacket(PSOCKET Socket, char *szbuffer);


void InitSocket(PSOCKET Socket)
{
	WSADATA wsaData;
	WORD	wVersionRequested;

	wVersionRequested = MAKEWORD( 2, 0 );

	Socket->skSocket = INVALID_SOCKET;
	Socket->iStatus = WSAStartup(wVersionRequested,&wsaData);
}

bool Accept(PSOCKET Server, PSOCKET Client)
{
	sockaddr_in saClientAddress;
	int			iClientSize = sizeof(sockaddr_in);
	SOCKADDR	IPAddress;

	Client->skSocket = accept( Server->skSocket, (struct sockaddr*)&saClientAddress, &iClientSize );
	
	if( Client->skSocket == INVALID_SOCKET ) 
	{
		return false;
	}
	else 
	{
		memcpy(&IPAddress,&saClientAddress,sizeof(saClientAddress));
		printf("%d.%d.%d.%d is Connecting\n",saClientAddress.sin_addr.S_un.S_un_b.s_b1,saClientAddress.sin_addr.S_un.S_un_b.s_b2,saClientAddress.sin_addr.S_un.S_un_b.s_b3,saClientAddress.sin_addr.S_un.S_un_b.s_b4);

		Client->stReceive.skSocket = Client->skSocket;
		Client->stReceive.szBuffer[0] = NULL;
		Client->stReceive.iReadPos = 0;
		Client->stReceive.iWritePos = 0;
		Client->stReceive.iTerminate = 0;
		
		// Create the thread to receive data
		CreateThread(
			NULL,															// pointer to security attributes
			NULL,															// initial thread stack size
			(LPTHREAD_START_ROUTINE ) &thrReceiveThread,     // pointer to thread function
			&Client->stReceive,              // argument for new thread
			NULL,									// creation flags
			&Client->dwReceiveHandle         // pointer to receive thread ID
		);

		return true;
	}
}

int Listen(PSOCKET Server)
{
	return listen( Server->skSocket, 32 );
}

int Bind(PSOCKET Server, int iPort)
{
	sockaddr_in saServerAddress;

	Server->skSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(Server->skSocket == INVALID_SOCKET)
	{
		return false;
	}

	memset(&saServerAddress, 0, sizeof(sockaddr_in));

	saServerAddress.sin_family = AF_INET;
	saServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	saServerAddress.sin_port = htons(iPort);

	if( bind(Server->skSocket, (sockaddr*) &saServerAddress, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Disconnect(Server);
		return false;
	}
	else
		return true;
}


		// Close connection
void Disconnect(PSOCKET Socket)
{
	if (Socket->skSocket != INVALID_SOCKET)
	{
		closesocket(Socket->skSocket);
		Socket->skSocket = INVALID_SOCKET;
	}

}

bool Connect(PSOCKET Client, char* szServerAddress, int iPort)
{
	struct		sockaddr_in serv_addr;
	LPHOSTENT	lphost;

	memset(&serv_addr,0,sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(szServerAddress);

	if (serv_addr.sin_addr.s_addr == INADDR_NONE)
	{
		lphost = gethostbyname(szServerAddress);
		if (lphost != NULL)
			serv_addr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}

	serv_addr.sin_port = htons(iPort);

	// Open the socket
	Client->skSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(Client->skSocket == INVALID_SOCKET)
	{
		return false;
	}

	int err = connect(Client->skSocket, (struct sockaddr*)&serv_addr,sizeof(sockaddr));
	if(err == SOCKET_ERROR)
	{
		Disconnect(Client);
		return false;
	}

	
	Client->stReceive.skSocket = Client->skSocket;
	Client->stReceive.szBuffer[0] = NULL;
	Client->stReceive.iReadPos = 0;
	Client->stReceive.iWritePos = 0;
	Client->stReceive.iTerminate = 0;

	// Create the thread to receive data
	CreateThread(
		NULL,  // pointer to security attributes
		NULL,                         // initial thread stack size
		(LPTHREAD_START_ROUTINE ) &thrReceiveThread,     // pointer to thread function
		&Client->stReceive,                        // argument for new thread
		NULL,                     // creation flags
		&Client->dwReceiveHandle          // pointer to receive thread ID
	);

	return true;
}

int Recv(PSOCKET Socket, char *szBuffer, int iBufLen, int iFlags)
{
	return recv(Socket->skSocket, szBuffer, iBufLen, iFlags);
}

int Send(PSOCKET Socket, char *szBuffer, int iBufLen, int iFlags)
{
	return send(Socket->skSocket,szBuffer,iBufLen,iFlags);
}

int iCalculateChecksum(stPacketHeader stHead)
{
	int	iChecksum = 0;

	iChecksum = (stHead.iID+stHead.iLength+stHead.iType);

	return(iChecksum);
}


void thrReceiveThread(SocketTransmissionStruct *rs)
{
	int				iBytesReceived;
	char			*szTempBuffer;
	int				iBytesPart1;
	int				iBytesPart2;

	szTempBuffer = new char[32768];

	// Receive data until given notice to terminate
	while( rs->iTerminate != 1 ) {
		// Read from the pipe
		iBytesReceived = recv( rs->skSocket, szTempBuffer, 32768, 0 );
		if( iBytesReceived > 0 ) {
			// Make sure the packet does not overrun the write buffer
			if( (rs->iWritePos+iBytesReceived) >= 64000 ) {
				iBytesPart1 = ((rs->iWritePos+iBytesReceived)-64000);
				iBytesPart2 = (64000 - rs->iWritePos);
				memcpy( &rs->szBuffer[rs->iWritePos], szTempBuffer, iBytesPart1 );
				memcpy( &rs->szBuffer[0], &szTempBuffer[iBytesPart1], iBytesPart2 );
				rs->iWritePos = iBytesPart2;
			}
			else {
				// Write to the permanent buffer
				memcpy( &rs->szBuffer[rs->iWritePos], szTempBuffer, iBytesReceived );
				rs->iWritePos += iBytesReceived;
			}
		}
	}

	delete [] szTempBuffer;
}

void thrSendThread(SocketTransmissionStruct &rs)
{
	int iBytesSent;
	
	iBytesSent = send( rs.skSocket, rs.szBuffer, 128, 0 );
}

int vGetPacket(PSOCKET Socket, char *szbuffer)
{
	
	int				iBytesReceived = 0;
	int				iBytesWaiting = 0;
	stPacketHeader	stHeader;

	// Check if write pos moved
	if( Socket->stReceive.iWritePos != Socket->stReceive.iReadPos ) {
		//
		// Pull packet header from buffer
		//
		iBytesWaiting = (Socket->stReceive.iWritePos - Socket->stReceive.iReadPos);
		// Make sure a full size header is present
		if( iBytesWaiting < sizeof(stHeader) ) {
			return(0);
		}
		// Copy the header in
		memcpy(&stHeader,&Socket->stReceive.szBuffer[Socket->stReceive.iReadPos],sizeof(stHeader));
		// Check the checksum
		if( ((stHeader.iType+stHeader.iLength+stHeader.iID)) != stHeader.iCheckSum ) 
		{
			// Skip the first bad byte in an attempt to find a good packet
			Socket->stReceive.iReadPos++;
			if( Socket->stReceive.iReadPos >= 64000 ) 
			{
				Socket->stReceive.iReadPos = 0;
			}
			// Try again for a good packet
			vGetPacket(Socket, szbuffer);
		}
		else 
		{
		}

		//
		// Pull the body of the packet according to the size 
		//
		
		// Make sure enough data is waiting, if not leave and try again later
		if( (iBytesWaiting-sizeof(stHeader)) < (unsigned)stHeader.iLength ) {
			return(0);
		}
		// Copy into the return buffer
		memcpy(szbuffer,&stHeader,sizeof(stHeader));
		memcpy(&szbuffer[sizeof(stHeader)],&Socket->stReceive.szBuffer[Socket->stReceive.iReadPos+sizeof(stHeader)],stHeader.iLength);

		// Update Read Position & Return Values
		Socket->stReceive.iReadPos += (stHeader.iLength+sizeof(stHeader));
		iBytesReceived = (stHeader.iLength+sizeof(stHeader));
		
		// Check if reading too far
		if( Socket->stReceive.iReadPos >= 64000 ) {
			Socket->stReceive.iReadPos = 0;
		}
	}

	return(iBytesReceived);
}
		


//// Class Object
//class SocketObject  
//{
//	private:
//
//	public:
//		SOCKET						skSocket;
//		int							iStatus;
//		SocketTransmissionStruct	stReceive;	
//		SocketTransmissionStruct	stSend;	
//		DWORD						dwReceiveHandle;
//		DWORD						dwSendHandle;
//
//		// Constructor
//		SocketObject();
//		// Desctrucot
//		~SocketObject();
//
//		// Accept a client's request to connect
//		bool Accept(SocketObject& skAcceptSocket);
//		// Listen for clients to connect
//		int Listen( void );
//		// Open a server listening port
//		int Bind(int iPort);
//		// Close connection
//		void Disconnect();
//		// Connect to a server
//		bool Connect(char* szServerAddress, int iPort);
//
//		int Recv(char *szBuffer, int iBufLen, int iFlags);
//		int Send(char *szBuffer, int iBufLen, int iFlags);
//
//		// Misc Functions
//		int iCalculateChecksum(stPacketHeader stHead);
//
//		// Asynchronous Functions
//		static void thrReceiveThread(SocketTransmissionStruct *rs);
//		static void thrSendThread(SocketTransmissionStruct &rs);
//		int vGetPacket(char *szbuffer);
//};

#endif
