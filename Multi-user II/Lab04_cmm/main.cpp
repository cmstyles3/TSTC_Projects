
// Standard Includes
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "header.h"

using namespace std;

struct stChatPacket
{
	stPacketHeader	stHeader;
	char			szChatMessage[128];
};

void vServerConnection( int iListenPort );
void vClientConnection( char *szServerIP, int iServerListenPort );

//
// ----> Main Program Function (REQUIRED)

//

int NumClients = 0;

int main()
{

	//
	// If user selected server, listen on the given port 
	//
	char Type[16], port[6];
	cout << "Server or Client -->";
	cin >> Type;
	if( !stricmp( Type, "server" ) )
	{
		cout << "Enter port number -->";
		cin >> port;
		cout << "Enter the number of Clients -->";
		cin >> NumClients;
		vServerConnection( atoi(port) );
	}
	//
	// User selected client, connect to given port and IP address
	//
	else 
	{
		char ip[16];
		cout << "Enter IP address -->";
		cin >> ip;

		cout << "Enter port number -->";
		cin >> port;
		vClientConnection( ip, atoi( port ) );	
	}
	
	return( 1 );
}

// Function for server
void vServerConnection( int iListenPort )
{
	SOCKETTYPE	ServerSocket;		// Server Socket Object
	SOCKETTYPE	ClientSocket[4];		// Client Socket Object
	char			DataPacket[128];		// Data Packet to Transmit
	int				iBytesReceived = 0;		// # of Bytes Received
	int				iBytesSent = 0;			// # of Bytes Sent
	char			szCommand[128];			// Chat Buffer
	char			szTempChar;
	int				i;
	stChatPacket	ChatPacket;
	char			szPacketBuffer[32768];
	
	InitSocket(&ServerSocket);
	for (int i = 0; i < NumClients; i++)
	{
		InitSocket(&ClientSocket[i]);
	}
	cout << "<Server> Attempting to listen on Port " << iListenPort << endl;
	
	// Attempt to start the server
	if ( Bind(&ServerSocket, iListenPort ) ) 
	{
		
		// Listen for connection on the Listen port
		Listen(&ServerSocket);
		
		// Accept the connection
		for (int i = 0; i < NumClients; i++)
			Accept(&ServerSocket, &ClientSocket[i] );
		
		
		cout << "<Server> Client Connected to Port " << iListenPort << endl;
		
		// Loop forever or until a break is issued
		while( 1 ) 
		{
			printf("\n<Server> ");
			
			// Loop through getting up to 127 characters to transmit, leave room for \n code
			for( i = 0; i < 127; i++ ) 
			{
				// Loop until a key is pressed
				while( !_kbhit() ) 
				{
					// Check for incomming data
					for (int i = 0; i < NumClients; i++)
					{
						iBytesReceived = vGetPacket(&ClientSocket[i], szPacketBuffer);
						if( iBytesReceived > 0 ) 
						{
							// Copy the received data into the chat packet
							memcpy(&ChatPacket,szPacketBuffer,sizeof(ChatPacket));
							strcpy(DataPacket,ChatPacket.szChatMessage);
							for (int j = 0; j < NumClients; j++)
								if ( i != j)
								{
									// Load the packet data
									ChatPacket.stHeader.iID = 0;
									ChatPacket.stHeader.iType = 0;
									ChatPacket.stHeader.iLength = 128;
									ChatPacket.stHeader.iSender = 0;
									strcpy(ChatPacket.szChatMessage,DataPacket);
									ChatPacket.stHeader.iCheckSum = 128;
									// Load the packet buffer
									memcpy(szPacketBuffer,&ChatPacket,sizeof(stChatPacket));
									Send(&ClientSocket[j], szPacketBuffer,sizeof(stPacketHeader)+128,0);
								}
							if( !stricmp( DataPacket, "/exit" ) ) 
							{
								break;
							}
							else 
							{
								cout << endl << "<Client> " << DataPacket << endl;
								printf("<Server> ");
							}
							if( !stricmp( DataPacket, "/stuff" ) ) 
							{
								//PlaySound(
							}
							
						}
						
					}
				}
				// Quit chatting if /exit command given
				if( !stricmp( DataPacket, "/exit" ) ) {
					break;
				}
				// Get next key from keyboard input buffer
				szTempChar = getche();
				// Add the keypress to our command string
				szCommand[i] = szTempChar;
				// Check for enter key
				if( szTempChar == 13 ) {
					break;
				}
			}
			// Check for user initiated exit code
			if( !stricmp( DataPacket, "/exit" ) ) {
				break;
			}
			// Null Terminate the command
			szCommand[i] = '\0';
			// Populate the packet
			strcpy(DataPacket,szCommand);
			
			// Quit chatting if /exit command given
			if( !stricmp( szCommand, "/exit" ) ) 
			{
				break;
			}
			else 
			{	
				// Load the packet data
				ChatPacket.stHeader.iID = 0;
				ChatPacket.stHeader.iType = 0;
				ChatPacket.stHeader.iLength = 128;
				ChatPacket.stHeader.iSender = 0;
				strcpy(ChatPacket.szChatMessage,DataPacket);
				ChatPacket.stHeader.iCheckSum = 128;
				// Load the packet buffer
				memcpy(szPacketBuffer,&ChatPacket,sizeof(stChatPacket));
				// Send the data
				for (int i = 0; i < NumClients; i++)
				{
					iBytesSent = Send(&ClientSocket[i], szPacketBuffer,sizeof(stPacketHeader)+128,0);
					// Check if client has disconnected
					if( iBytesSent == -1 ) 
					{
						cout << "<Server> Client No Longer Online" << endl;
						break;
					}	
				}
			}
		}
		
		// Disconnect the clients
		for (int i = 0; i < NumClients; i++)
			Disconnect(&ClientSocket[i]);
		
		cout << "<Server> Client Disconnected" << endl;
	}
	else 
	{
		cout << "<Server> Failed to Listen" << endl;
	}
}

// Function for client
void vClientConnection( char *szServerIP, int iServerListenPort )
{
	SOCKETTYPE	ClientSocket;		// Client Socket Object
	char			DataPacket[128];		// Data Packet to Transmit
	int				iBytesReceived = 0;		// # of Bytes Received
	int				iBytesSent = 0;			// # of Bytes Sent
	char			szCommand[128];			// Chat Buffer
	char			szTempChar;
	int				i;
	char			szPacketBuffer[32768];
	stChatPacket	ChatPacket;
	
	InitSocket(&ClientSocket);
	cout << "<Client> Connecting to " << szServerIP << ", Port " << iServerListenPort << endl;
	
	// Connect to the IP and Port 
	if( Connect(&ClientSocket, szServerIP, iServerListenPort ) ) 
	{
		cout << "<Client> Connected" << endl;
		
		// Keep Chatting until a /exit is given
		while( 1 ) 
		{
			printf("\n<Client> ");
			
			// Loop through getting up to 127 characters to transmit, leave room for \n code
			for( i = 0; i < 127; i++ ) 
			{
				// Loop until a key is pressed
				while( !_kbhit() ) 
				{
					// Check for incomming data
					iBytesReceived = vGetPacket(&ClientSocket, szPacketBuffer);
					if( iBytesReceived ) 
					{
						// Copy the received data into the chat packet
						memcpy(&ChatPacket,szPacketBuffer,sizeof(ChatPacket));
						strcpy(DataPacket,ChatPacket.szChatMessage);
						cout << endl << "<Server or Remote Client> " << DataPacket << endl;
						printf("<Client> ");
						if (!stricmp(DataPacket, "/sound"))
						{
							PlaySound("ammo.wav", 0, 0);
						}
					}
				}
				// Get next key from keyboard input buffer
				szTempChar = getche();
				// Add the keypress to our command string
				szCommand[i] = szTempChar;
				// Check for enter key
				if( szTempChar == 13 ) 
					break;
			}
			// Null Terminate the command
			szCommand[i] = '\0';
			// Populate the packet
			strcpy(DataPacket,szCommand);
			// Quit chatting if /exit command given
			if( !stricmp( szCommand, "/exit" ) ) 
			{
				break;
			}
			else 
			{	
				// Load the packet data
				ChatPacket.stHeader.iID = 0;
				ChatPacket.stHeader.iType = 0;
				ChatPacket.stHeader.iLength = 128;
				ChatPacket.stHeader.iSender = 0;
				strcpy(ChatPacket.szChatMessage,DataPacket);
				ChatPacket.stHeader.iCheckSum = 128;
				// Load the packet buffer
				memcpy(szPacketBuffer,&ChatPacket,sizeof(stChatPacket));
				// Send the data
				iBytesSent = Send(&ClientSocket, szPacketBuffer,sizeof(stPacketHeader)+128,0);
				// Check if client has disconnected
				if( iBytesSent == -1 ) 
				{
					cout << "<Server> Client No Longer Online" << endl;
					break;
				}	
				
				// Check for server disconnection
				if( iBytesSent == -1 ) 
				{
					cout << "<Client> Server No Longer Online" << endl;
					break;
				}	
				
				// Quit chatting if /exit command given
				if( !stricmp( szCommand, "/exit" ) ) 
				{
					break;
				}
			}
		}
		
		// Disconnect from the server
		Disconnect(&ClientSocket);
		
		cout << "<Client> Disconnected From Server" << endl;
	}
	else {
		cout << "<Client> Failed to Connect" << endl;
	}
}
