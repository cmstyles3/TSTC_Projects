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
	char SendData[80] = "stuff";
	int SendBytes = 0;
	char RecvData[80];
	int RecvBytes = 0;
	int ClientSize = sizeof(sockaddr_in);

	WSADATA wsaData;
	SOCKET skServer, skClient;
	sockaddr_in saServer, saClient;
	
	if (WSAStartup(MAKEWORD(2,0),&wsaData) != NO_ERROR)
	{
		return(0);
	}

	saClient.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	saClient.sin_family = AF_INET;
	saClient.sin_port = htons(2007);

	skClient = socket(AF_INET, SOCK_DGRAM, 0);

	if (skClient == INVALID_SOCKET)
	{
		printf("Invalid Socket\n");
		WSACleanup();
		getch();
		return(0);
	}
	else
	{
		printf("Socket Creation Successful\n");
	}

	if (bind(skClient, (struct sockaddr *)&saClient,sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf("bind was unsuccessful\n");
		closesocket(skClient);
		WSACleanup();
		getch();
		return(0);
	}
	
	/*if (listen(skServer, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}*/
	
	//skClient = accept(skServer,(struct sockaddr *)&saClient, &ClientSize);

	/*if (skClient == INVALID_SOCKET)
	{
		closesocket(skClient);
		WSACleanup();
		return(0);
	}*/
	ClientIP.seq1 = saClient.sin_addr.S_un.S_un_b.s_b1;
	ClientIP.seq2 = saClient.sin_addr.S_un.S_un_b.s_b2;
	ClientIP.seq3 = saClient.sin_addr.S_un.S_un_b.s_b3;
	ClientIP.seq4 = saClient.sin_addr.S_un.S_un_b.s_b4;

	RecvBytes = recvfrom(skClient, RecvData,sizeof(RecvData), 0, (struct sockaddr *)&saClient, &ClientSize);
	printf("Recieved Data: %s\n", RecvData);
	printf("Recieved %i bytes from client at IP: %i.%i.%i.%i\n",RecvBytes, ClientIP.seq1, ClientIP.seq2, ClientIP.seq3, ClientIP.seq4);

	SendBytes = sendto(skClient, SendData, sizeof(SendData), 0, (struct sockaddr *)&saClient, ClientSize);
	//printf("Sent Data: %s\n", RecvData);
	//printf("Recieved %i bytes from client at IP: %i.%i.%i.%i\n",RecvBytes, ClientIP.seq1, ClientIP.seq2, ClientIP.seq3, ClientIP.seq4);

	closesocket(skClient);
	//closesocket(skServer);
	WSACleanup();
	printf("press any key to continue\n");
	return(1);
}