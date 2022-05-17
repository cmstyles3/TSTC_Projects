
// Standard Includes
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "SocketObject.h"

using namespace std;

struct stChatPacket
{
	stPacketHeader	stHeader;
	char			szChatMessage[99];
};

void vServerConnection( int iListenPort );
//
// ----> Main Program Function (REQUIRED)
//
int main()
{
	
	char port[6];
	cout << "CIA HEADQUARTERS TERMINAL" << endl;
	cout << "Type in /d to decode last message" << endl;
	cout << "<Enter port number>";
	cin >> port;
	vServerConnection(atoi(port));

	
	return( 1 );
}

// Function for server
void vServerConnection( int iListenPort )
{
	SocketObject	ServerSocketObject;		// Server Socket Object
	SocketObject	ClientSocketObject;		// Client Socket Object
	char			DataPacket[99];		// Data Packet to Transmit
	int				iBytesReceived = 0;		// # of Bytes Received
	int				iBytesSent = 0;			// # of Bytes Sent
	char			szCommand[50];			// Chat Buffer
	char			szTempChar;
	int				i;
	stChatPacket	ChatPacket;
	char			szPacketBuffer[32768];
	char encodeBuffer[99], decodeBuffer[99];
	
	cout << "<HQ> Attempting to listen on Port " << iListenPort << endl;
	
	// Attempt to start the server
	if ( ServerSocketObject.Bind( iListenPort ) ) 
	{
		
		// Listen for connection on the Listen port
		ServerSocketObject.Listen( );
		
		// Accept the connection
		ServerSocketObject.Accept( ClientSocketObject );
		
		cout << "<HQ> Agent Connected to Port " << iListenPort << endl;
		
		// Loop forever or until a break is issued
		while( 1 ) 
		{
			printf("\n<HQ> ");
			
			// Loop through getting up to 127 characters to transmit, leave room for \n code
			for( i = 0; i < 49; i++ ) 
			{
				// Loop until a key is pressed
				while( !_kbhit() ) 
				{
					// Check for incomming data
					iBytesReceived = ClientSocketObject.vGetPacket(szPacketBuffer);
					if( iBytesReceived > 0 ) 
					{
						// Copy the received data into the chat packet
						memcpy(&ChatPacket,szPacketBuffer,sizeof(ChatPacket));
						strcpy(DataPacket,ChatPacket.szChatMessage);
						strcpy(decodeBuffer, DataPacket);
						if( !stricmp( DataPacket, "/exit" ) ) 
						{
							break;
						}
						else 
						{
							cout << endl << "<Agent> " << DataPacket << endl;
							printf("<HQ> ");
						}
					}
				}
				// Quit chatting if /exit command given
				if( !stricmp( DataPacket, "/exit" ) ) 
				{
					break;
				}
				// Get next key from keyboard input buffer
				szTempChar = getche();
				// Add the keypress to our command string
				szCommand[i] = szTempChar;
				// Check for enter key
				if( szTempChar == 13 ) 
				{
					
					break;
				}
			}
			for (int j = 0, k = 0; j < i; j++, k += 2)
			{
				itoa(szCommand[j], &encodeBuffer[k], 16);
							
			}
			// Check for user initiated exit code
			if( !stricmp( DataPacket, "/exit" ) ) 
			{
				break;
			}
			// Null Terminate the command
			szCommand[i] = '\0';
			// Populate the packet
			strcpy(DataPacket,encodeBuffer);
			
			// Quit chatting if /exit command given
			if( !stricmp( szCommand, "/exit" ) ) 
			{
				break;
			}
			else if (!stricmp(szCommand, "/d"))
			{
				char tempBuffer[99];
				memset(tempBuffer, 0, sizeof(tempBuffer));
				int i, j;
				for(i = 0, j = 0; i < strlen(decodeBuffer); i += 2, j++)
				{
					int temp = 0, temp2 = 0;
					if (decodeBuffer[i] >= '0' && decodeBuffer[i] <= '9')
						temp = decodeBuffer[i] - 48;
					else if (decodeBuffer[i] >= 'a' && decodeBuffer[i] <= 'f')
						temp = decodeBuffer[i] - 87;

					if (decodeBuffer[i + 1] >= '0' && decodeBuffer[i + 1] <= '9')
						temp2 = decodeBuffer[i + 1] - 48;
					else if (decodeBuffer[i + 1] >= 'a' && decodeBuffer[i + 1] <= 'f')
						temp2 = decodeBuffer[i + 1] - 87;
					
					tempBuffer[j] = (16 * temp) + temp2;
					
				}
				//tempBuffer[j] = '\0';
				cout << endl << "Decoding Message...\n" << tempBuffer;
			}
			else 
			{	
				// Load the packet data
				ChatPacket.stHeader.iID = 0;
				ChatPacket.stHeader.iType = 0;
				ChatPacket.stHeader.iLength = 99;
				ChatPacket.stHeader.iSender = 0;
				strcpy(ChatPacket.szChatMessage,DataPacket);
				ChatPacket.stHeader.iCheckSum = 99;
				// Load the packet buffer
				memcpy(szPacketBuffer,&ChatPacket,sizeof(stChatPacket));
				// Send the data
				iBytesSent = ClientSocketObject.Send(szPacketBuffer,sizeof(stPacketHeader)+99,0);
				// Check if client has disconnected
				if( iBytesSent == -1 ) 
				{
					cout << "<HQ> Client No Longer Online" << endl;
					break;
				}	
			}
		}
		
		// Disconnect the client
		ClientSocketObject.Disconnect();
		
		cout << "<HQ> Client Disconnected" << endl;
	}
	else 
	{
		cout << "<HQ> Failed to Listen" << endl;
	}
}
