
#define WIN32_LEAN_AND_MEAN
#define ASYNC_MSG 1000
#define IDC_LISTBOX 2000

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include "resource.h"
using namespace std;

#define IDC_BUTTON_HOST 2001
#define IDC_BUTTON_SEND 2002
#define IDC_EDITBOX_IP 2003
#define IDC_EDITBOX_PORT 2004
#define IDC_EDITBOX_MESSAGE 2005

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1


HINSTANCE hinstance_app = 0;
HWND hlistbox = 0;
HWND hButtonHost = 0;
HWND hButtonSend = 0;
HWND hEditboxIP = 0;
HWND hEditboxPort = 0;
HWND hEditboxMessage = 0;

SOCKET ServerSocket;
SOCKET ClientSocket;

int numConnected = 0;

sockaddr_in saServer;
sockaddr_in saClient;

WSADATA WSAData;

bool Gloop = true;
char Buffer[100];
char ServerIP[15] = "127.0.0.1";
char SendData[80] = "474F4420426C65737320416D657269636121";
char RecvData[80];
int SendBytes = 0;
int RecvBytes = 0;
int ClientSize = sizeof(sockaddr_in);

 
vector<SOCKET> clientSocket;
char MessageBuffer[76];

DWORD start_time;
BOOL debugOpt = TRUE;
bool clientConnected = false;

void ShowText(HWND hChildHandle, char *Text)
{
	int Line;
	SendMessage(hChildHandle, LB_ADDSTRING, 0, (LPARAM)Text);
	Line = SendMessage(hChildHandle, LB_GETCOUNT, 0, 0);
	SendMessage(hChildHandle, LB_SETCURSEL, Line - 1, 0);
	SendMessage(hChildHandle, LB_SETCURSEL, -1, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	WORD WSAEvent, WSAErr;

	switch(msg)
	{
		case WM_CREATE:
		{
			return(0);
		}break;
		case WM_COMMAND:
		{
			switch(LOWORD(wparam))
			{
				case MENU_HELP_ABOUT:
					MessageBox(hwnd,"Collin Munson \nCopyright 2007\nCompleted 11/19/07", "About", MB_OK);
					break;
				case IDC_BUTTON_HOST:
				{
					closesocket(ServerSocket);
					char port[5], ip[16];
					int portNum = 0;
					GetWindowText(hEditboxIP, ip, 16);
					GetWindowText(hEditboxPort, port, 5);
					portNum = atoi(port);

					//ShowText(hlistbox, Buffer);

					if (WSAStartup(MAKEWORD(2, 0), &WSAData) != NO_ERROR)
					{
						MessageBox(hwnd, "Error with WSAStartup()", "Error", MB_OK);
						return(0);
					}
					
					memset(&saServer, 0, sizeof(sockaddr_in));

					saServer.sin_family = AF_INET;
					saServer.sin_addr.S_un.S_addr = inet_addr(ip);
					saServer.sin_port = htons(portNum);

					ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

					sprintf(Buffer, "%i", ServerSocket);
					MessageBox(hwnd, Buffer, "Message Inbox", MB_OK);
					if (ServerSocket == INVALID_SOCKET)
					{
						MessageBox(hwnd, "Error with socket", "Error", MB_OK);
						WSACleanup();
						return(0);
					}

					//MessageBox(hwnd, "All is good, about to issue WSAAsyncSelect()", "Good News", MB_OK);
					WSAAsyncSelect(ServerSocket, hwnd, ASYNC_MSG, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE);

					if (bind(ServerSocket, (struct sockaddr *)&saServer, sizeof(sockaddr)) == SOCKET_ERROR)
					{
						MessageBox(hwnd, "Error binding socket()", "Error", MB_OK);
						closesocket(ServerSocket);
						WSACleanup();
						return(0);
					}

					
					if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
					{
						MessageBox(hwnd, "Error listening on socket()", "Error", MB_OK);
						closesocket(ServerSocket);
						WSACleanup();
						return(0);
					}
					

				}break;
				case IDC_BUTTON_SEND:
				{

					GetWindowText(hEditboxMessage, MessageBuffer, 76);
					char tempBuffer[100];
					sprintf(tempBuffer,"<Server> %s", MessageBuffer);
					ShowText(hlistbox, tempBuffer);
					SetWindowText(hEditboxMessage, "");
					for (int i = 0; i < clientSocket.size(); i++)
					{
						send(clientSocket[i], MessageBuffer, sizeof(MessageBuffer), 0);
					}
					//send(ClientSocket[1], MessageBuffer, sizeof(MessageBuffer), 0);
				}break;
			}
		}
		case ASYNC_MSG:
		{
			WSAEvent = WSAGETSELECTEVENT(lparam);
			WSAErr = WSAGETSELECTERROR(lparam);

			switch(WSAEvent)
			{
				case FD_WRITE:
				{
					//MessageBox(hwnd, "FD_WRITE received", "Message Inbox", MB_OK);
				}break;

				case FD_CLOSE:
				{
					
					vector <SOCKET> temp;
					for (int i = 0; i < clientSocket.size(); i++)
					{
						if (clientSocket[i] != wparam)
							temp.push_back(clientSocket[i]);
					}
					while(!clientSocket.empty()) clientSocket.pop_back();
					for (int i = 0; i < temp.size(); i++)
					{
						clientSocket.push_back(temp[i]);
					}
					
					ShowText(hlistbox, "Disconnected");
					clientConnected = false;
				}break;
				case FD_READ:
				{
					//MessageBox(hwnd, "FD_READ received", "Message Inbox", MB_OK);
					for (int i = 0; i < clientSocket.size(); i++)
					{
						RecvBytes = recv(clientSocket[i], RecvData, sizeof(RecvData), 0);
						if (RecvBytes > -1)
						{
							char tempBuffer[100];
							sprintf(tempBuffer, "<Client> %s", RecvData);
							ShowText(hlistbox, tempBuffer);
							strcpy(SendData, RecvData);
							for (int j = 0; j < clientSocket.size(); j++)
							{
								if (i != j)
									SendBytes = send(clientSocket[j], SendData, sizeof(SendData), 0);
							}
						}
					}

					
				}break;
				case FD_ACCEPT:
				{
					//clientSocket.push_back(ClientSocket);
			
					//if (numConnected < 2)
					{
						//MessageBox(hwnd, "FD_ACCEPT received", "Message Inbox", MB_OK);
						ClientSocket = accept(ServerSocket, (struct sockaddr*)&saClient, &ClientSize);
						clientSocket.push_back(ClientSocket);
						//closesocket(ClientSocket);
						ShowText(hlistbox, "Client Connected");
						UpdateWindow(hwnd);
						//numConnected++;
					}	
				}break;
				default:break;
			}
		}break;
		
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			TextOut(hdc, 15, 15, "IP Address", strlen("IP Address"));
			TextOut(hdc, 260, 15, "Port Number", strlen("Port Number"));
			sprintf(Buffer, "%i Clients connected", clientSocket.size());
			TextOut(hdc, 15, 332, Buffer, strlen(Buffer));
			EndPaint(hwnd, &ps);
			return(0);
		}break;
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			Gloop = false;
			return(0);
		}break;
		default:break;
	}
	return(DefWindowProc(hwnd, msg, wparam, lparam));
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int)
{
	
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadIcon(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "WCMAIN";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	hinstance_app = hinstance;

	if (!RegisterClassEx(&wc))
		return(0);

	if (!(hwnd = CreateWindowEx(0, "WCMAIN", "Server Async", 
		                        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
								50, 50, 650, 400, 0, 0, hinstance, 0)))
		return(0);

	hlistbox = CreateWindowEx(0, "LISTBOX", 0, 
		                      WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER, 
							  10, 50, 620, 250, hwnd, (HMENU)IDC_LISTBOX, hinstance, 0);

	hButtonHost = CreateWindow("BUTTON", "HOST", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 10, 100, 28, hwnd, (HMENU)IDC_BUTTON_HOST, hinstance, NULL);

	hButtonSend = CreateWindow("BUTTON", "SEND", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 575, 300, 50, 28, hwnd, (HMENU)IDC_BUTTON_SEND, hinstance, NULL);

	hEditboxIP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 100, 10, 135,28, hwnd, (HMENU)IDC_EDITBOX_IP, hinstance, 0);
	hEditboxPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "6000", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 350,10, 135,28, hwnd, (HMENU)IDC_EDITBOX_PORT, hinstance, 0);
	
	
	hEditboxMessage = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Enter Message Here", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 10,300, 560,28, hwnd, (HMENU)IDC_EDITBOX_MESSAGE, hinstance, 0);
	
	while(Gloop)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(KEYDOWN(VK_RETURN))
		{
			//memcpy(MessageBuffer, 0, sizeof(MessageBuffer));
			GetWindowText(hEditboxMessage, MessageBuffer, 76);
			if (strlen(MessageBuffer))
			{
				char tempBuffer[100];
				sprintf(tempBuffer,"<Server Num clients %i> %s", clientSocket.size(), MessageBuffer);
				ShowText(hlistbox, tempBuffer);
				SetWindowText(hEditboxMessage, "");
				for (int i = 0; i < clientSocket.size(); i++)
				{
					send(clientSocket[i], MessageBuffer, sizeof(MessageBuffer), 0);
				}
			}
		}
		
	}
	return(msg.wParam);
}