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

	SOCKET skClient;
	sockaddr_in saClient;
	sockaddr_in saC;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2,0), &wsaData);
	if(iResult != NO_ERROR)
	{
		printf("Error with WSAStartup()\n");
		return (0);
	}
	memset(RecvData, 0, sizeof(RecvData));
	memset(&saClient, 0, sizeof(sockaddr_in));

	saClient.sin_addr.s_addr = inet_addr(ServerIP);
	saClient.sin_family = AF_INET;
	saClient.sin_port = htons(2007);

	skClient = socket(AF_INET, SOCK_DGRAM, 0);
	if(skClient == INVALID_SOCKET)
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
	
	iResult = bind(skClient, (struct sockaddr *)&saClient, sizeof(sockaddr));

	if(iResult == SOCKET_ERROR)
	{
		printf("Got problems with socket creation\n");
		closesocket(skClient);
		WSACleanup();
		getch();
		return (0);
	}
	else
	{
		printf("BIND Server: successful\n");

		ClientIP.seg1 = saClient.sin_addr.S_un.S_un_b.s_b1;
		ClientIP.seg2 = saClient.sin_addr.S_un.S_un_b.s_b2;
		ClientIP.seg3 = saClient.sin_addr.S_un.S_un_b.s_b3;
		ClientIP.seg4 = saClient.sin_addr.S_un.S_un_b.s_b4;
		
		RecvBytes = recvfrom(skClient, RecvData, sizeof(RecvData), 0,(struct sockaddr*)&saClient,&ClientSize);
		printf("Connection was established with client.\n");
		printf("Attempting to Receive Packets of Data\n");	

		printf("Received Data: %s\n",RecvData);
		printf("Received %i bytes from client at IP: %i.%i.%i.%i\n\n", RecvBytes, ClientIP.seg1, ClientIP.seg2,ClientIP.seg3, ClientIP.seg4);
		
		printf("Attempting to Send Response\n");

		SendBytes = sendto(skClient, SendData, sizeof(SendData),0,(struct sockaddr*)&saClient,ClientSize);

		printf("Sent %i number of bytes to client\n",SendBytes);
		printf("Disconnecting from client, See Ya\n\n");

		closesocket(skClient);
		WSACleanup();

		printf("Connection closed....\n\n\n\n");
	}

	printf("Press any key to continue....\n");
	getch();

	return(0);
}
