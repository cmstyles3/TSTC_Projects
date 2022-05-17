#include <conio.h>
#include <iostream>
#include <winsock2.h>

struct IPAddress
{
	int seq1, seq2, seq3, seq4;
};


int main()
{
	char ServerIP[15] = "127.0.0.1";
	IPAddress ClientIP;
	char SendData[80] = "hello";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int ClientSize = sizeof(sockaddr_in);
	sockaddr_in saServer, saClient;
	SOCKET skServer, skClient;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
	{
		printf("WSAStartup - failed\n");
		getch();
		return(0);
	}
	memset(&saServer, 0, sizeof(sockaddr_in));
	saServer.sin_addr.s_addr = inet_addr(ServerIP);
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(2007);
	skServer = socket(AF_INET, SOCK_STREAM, 0);
	if (skServer == INVALID_SOCKET)
	{
		printf("Invalid Socket\n");
		WSACleanup();
		getch();
		return(0);
	}
	if (bind(skServer, (struct sockaddr *)&saServer,sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf("bind failed\n");
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}
	if (listen(skServer, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("lissten failed\n");
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}
	skClient = accept(skServer, (struct sockaddr *) &saClient, &ClientSize);


	ClientIP.seq1 = saClient.sin_addr.S_un.S_un_b.s_b1;
	ClientIP.seq2 = saClient.sin_addr.S_un.S_un_b.s_b2;
	ClientIP.seq3 = saClient.sin_addr.S_un.S_un_b.s_b3;
	ClientIP.seq4 = saClient.sin_addr.S_un.S_un_b.s_b4;

	RecvBytes = recv(skClient, RecvData, sizeof(RecvData), 0);

	SendBytes = send(skClient, SendData, sizeof(SendData), 0);

	closesocket(skClient);
	closesocket(skServer);
	WSACleanup();
	return(1);
}