#include <conio.h>
#include <iostream>
#include <WinSock2.h>

int main()
{
	char ServerIP[15] ="127.0.0.1";
	char SendData[80] = "Attempting a connection";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int ServerSize = sizeof(sockaddr_in);
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

	skServer = socket(AF_INET, SOCK_DGRAM, 0);
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
		printf("SOCKET Type: UDP\n");
	}
	
	printf("Attempting to Send Packets of Data\n");

	SendBytes = sendto(skServer, SendData, sizeof(SendData),0,(struct sockaddr*)&saServer,ServerSize);

	printf("Sent %i number of bytes to server\n",SendBytes);
	

	if(RecvBytes == 0 || (RecvBytes == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
	{
		RecvBytes = recvfrom(skServer, RecvData, sizeof(RecvData), 0,(struct sockaddr*)&saServer,&ServerSize);
		if(RecvBytes <  0)
		{
			if (RecvBytes == -1)
				printf("Connection closed.\n");
			return (0);
		}
		printf("Received Data: %s\n",RecvData);
		printf("Received %i bytes from server\n\n", RecvBytes);
	}

	printf("Disconnecting from server, See Ya\n\n");	
	closesocket(skServer);
	WSACleanup();

	printf("Connection closed....\n\n\n\n");
	
	printf("Press any key to continue....\n");
	getch();

	return(0);
}
