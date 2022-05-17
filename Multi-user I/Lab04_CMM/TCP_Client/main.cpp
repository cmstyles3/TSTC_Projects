#include <conio.h>
#include <iostream>
#include <WinSock2.h>

int main()
{
	char ServerIP[15] ="127.0.0.1";
	char SendData[80] = "Connection established";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int iResult = 0;

	SOCKET skServer;
	sockaddr_in saServer;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2,0), &wsaData);
	if(iResult != NO_ERROR)
	{
		printf("Error with WSAStartup()\n");
		return (0);
	}
	memset(RecvData, 0, sizeof(RecvData));
	memset(&saServer, 0, sizeof(sockaddr_in));

	saServer.sin_addr.s_addr = inet_addr(ServerIP);
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(2007);

	skServer = socket(AF_INET, SOCK_STREAM, 0);
	if(skServer == INVALID_SOCKET)
	{
		printf("Got problems with socket creation.\n");
		WSACleanup();
		getch();
		return (0);
	}
	else
	{
		printf("Server Socket: opened successfully.\n");
		printf("SOCKET Type: TCP\n");
	}
	
	iResult = connect(skServer, (struct sockaddr *) &saServer, sizeof(sockaddr));
	if(iResult == SOCKET_ERROR)
	{
		printf("got problem with socket connection\n");
		closesocket(skServer);
		WSACleanup();
		getch();
		return (0);
	}
	else
	{
		printf("connection established\n");	
		
		printf("Attempting to Send Response\n");

		SendBytes = send(skServer, SendData, sizeof(SendData),0);

		printf("Sent %i number of bytes to server\n",SendBytes);
		if (RecvBytes == 0 || (RecvBytes == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
		{
			printf("Attempting to Receive Packets of Data\n");
		
			RecvBytes = recv(skServer, RecvData, sizeof(RecvData), 0);
			if (RecvBytes < 0)
			{
				if (RecvBytes == -1)
				{
					printf("connection closed.\n");
				}
				return(0);
			}
			printf("Received Data: %s\n",RecvData);
			printf("Received %i bytes from server\n", RecvBytes);
		}

		printf("Disconnecting from server, See Ya\n\n");
		closesocket(skServer);
		WSACleanup();

		printf("Connection closed....\n\n\n\n");
	}
	printf("Press any key to continue....\n");
	getch();

	return(0);
}

