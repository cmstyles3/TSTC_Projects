#include <conio.h>
#include <iostream>
#include <WinSock2.h>

struct IPAddress
{
	int seg1,seg2,seg3,seg4;
};

int main()
{
	char ServerIP[15] ="127.0.0.1";
	IPAddress ClientIP;
	char SendData[80] = "Connection established";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int ClientSize = sizeof(sockaddr_in);
	int iResult = 0;

	SOCKET skServer;
	SOCKET skClient;
	sockaddr_in saServer;
	sockaddr_in saClient;
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
	
	iResult = bind(skServer, (struct sockaddr *)&saServer, sizeof(sockaddr));

	if(iResult == SOCKET_ERROR)
	{
		printf("Got problems with socket creation\n");
		closesocket(skServer);
		WSACleanup();
		getch();
		return (0);
	}
	else
	{
		printf("BIND Server: successful\n");
		if(listen(skServer, SOMAXCONN) == SOCKET_ERROR)
		{
			printf("Got problem with listening: %i\n",WSAGetLastError());
			closesocket(skServer);
			WSACleanup();
			getch();
			return (0);
		}
		else
		{
			printf("Listening on port: 2007\n");

			skClient = accept(skServer, (struct sockaddr *) &saClient, &ClientSize);
			
			ClientIP.seg1 = saClient.sin_addr.S_un.S_un_b.s_b1;
			ClientIP.seg2 = saClient.sin_addr.S_un.S_un_b.s_b2;
			ClientIP.seg3 = saClient.sin_addr.S_un.S_un_b.s_b3;
			ClientIP.seg4 = saClient.sin_addr.S_un.S_un_b.s_b4;

			printf("Connection was established with client.\n");
			printf("Attempting to Receive Packets of Data\n");
			
			RecvBytes = recv(skClient, RecvData, sizeof(RecvData), 0);

			printf("Received Data: %s\n",RecvData);
			printf("Received %i bytes from client at IP: %i.%i.%i.%i\n\n", RecvBytes, ClientIP.seg1, ClientIP.seg2,ClientIP.seg3, ClientIP.seg4);
			
			printf("Attempting to Send Response\n");

			SendBytes = send(skClient, SendData, sizeof(SendData),0);

			printf("Sent %i number of bytes to client\n",SendBytes);
			printf("Disconnecting from client, See Ya\n\n");

			closesocket(skClient);
			closesocket(skServer);
			WSACleanup();

			printf("Connection closed....\n\n\n\n");
		}
	}
	printf("Press any key to continue....\n");
	getch();

	return(0);
}

