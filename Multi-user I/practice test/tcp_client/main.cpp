#include <conio.h>
#include <iostream>
#include <winsock2.h>

int main()
{
	char ServerIP[15] = "127.0.0.1";
	char SendData[80] = "whats up";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int ServerSize = sizeof(sockaddr_in);
	SOCKET skServer;
	sockaddr_in saServer;
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,0),&wsaData) != NO_ERROR)
	{
		printf("WSAStartup - Failed\n");
		getch();
		return(0);
	}

	saServer.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(2007);

	skServer = socket(AF_INET,SOCK_STREAM,0);
	if (skServer == INVALID_SOCKET)
	{
		printf("Error opening server socket\n");
		WSACleanup();
		getch();
		return(0);
	}
	if (connect(skServer,(struct sockaddr *)&saServer,ServerSize) == SOCKET_ERROR)
	{
		printf("Could not connect to server\n");
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}

	SendBytes = send(skServer,SendData,sizeof(SendData),0);
	printf("Data Sent: %s\n", SendData);
	printf("%i bytes sent\n", SendBytes);
	if (RecvBytes == 0 || (RecvBytes == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
	{
		RecvBytes = recv(skServer,RecvData,sizeof(RecvData),0);
		printf("Recieved Data: %s\n",RecvData);
		if (RecvBytes < 0)
		{
			if (RecvBytes == -1)
			{
				printf("Connection closed.\n");
			}
			return(0);
		}
	}
	
	return(1);
}