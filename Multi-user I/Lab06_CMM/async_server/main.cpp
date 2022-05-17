#define WIN32_LEAN_AND_MEAN
#define ASYNC_MSG 1000
#define IDC_LISTBOX 2000

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <iostream>
#include "resource.h"

HINSTANCE hinstance_app = 0;
HWND hlistbox = 0;

SOCKET ServerSocket;
SOCKET ClientSocket;

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

void vShowText(HWND hChildHandle, char *Text)
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
					MessageBox(hwnd,"Collin Munson \nCopyright 2007\nCompleted 10/17/07", "About", MB_OK);
					break;
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
					MessageBox(hwnd, "FD_WRITE received", "Message Inbox", MB_OK);
				}break;

				case FD_CLOSE:
				{
					MessageBox(hwnd, "FD_CLOSE received", "Message Inbox", MB_OK);
					vShowText(hlistbox, "Disconnected");
				}break;
				case FD_READ:
				{
					MessageBox(hwnd, "FD_READ received", "Message Inbox", MB_OK);
					RecvBytes = recv(ClientSocket, RecvData, sizeof(RecvData), 0);
					if (RecvData[0] == 'R' && RecvData[1] == 'E' && RecvData[2] == 'Q')
					{
						vShowText(hlistbox, "Client Requested Data. Server Sending Now.");
						SendBytes = send(ClientSocket, SendData, sizeof(SendData), 0);
						memset(&RecvData, 0, sizeof(RecvData));
						break;
					}
					else
					{
						vShowText(hlistbox, RecvData);
					}
				}break;
				case FD_ACCEPT:
				{
					MessageBox(hwnd, "FD_ACCEPT received", "Message Inbox", MB_OK);
					ClientSocket = accept(ServerSocket, (struct sockaddr*)&saClient, &ClientSize);
					vShowText(hlistbox, "Client Connected");
				}break;
				default:break;
			}
		}break;
		
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
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
	wc.lpszMenuName = MAKEINTRESOURCE(MainMenu);
	wc.lpszClassName = "WCMAIN";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	hinstance_app = hinstance;

	if (!RegisterClassEx(&wc))
		return(0);

	if (!(hwnd = CreateWindowEx(0, "WCMAIN", "Server Async", 
		                        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
								50, 50, 400, 200, 0, 0, hinstance, 0)))
		return(0);

	hlistbox = CreateWindowEx(0, "LISTBOX", 0, 
		                      WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER, 
							  2, 2, 380, 160, hwnd, (HMENU)IDC_LISTBOX, hinstance, 0);
	
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != NO_ERROR)
	{
		MessageBox(hwnd, "Error with WSAStartup()", "Error", MB_OK);
		return(0);
	}
	
	memset(&saServer, 0, sizeof(sockaddr_in));

	saServer.sin_family = AF_INET;
	saServer.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	saServer.sin_port = htons(2007);

	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (ServerSocket == INVALID_SOCKET)
	{
		MessageBox(hwnd, "Error with socket", "Error", MB_OK);
		WSACleanup();
		return(0);
	}

	MessageBox(hwnd, "All is good, about to issue WSAAsyncSelect()", "Good News", MB_OK);
	WSAAsyncSelect(ServerSocket, hwnd, ASYNC_MSG, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE);

	if (bind(ServerSocket, (struct sockaddr *)&saServer, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		MessageBox(hwnd, "Error binding socket()", "Error", MB_OK);
		closesocket(ServerSocket);
		WSACleanup();
		return(0);
	}

	MessageBox(hwnd, "Server bound", "good news", MB_OK);
	if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		MessageBox(hwnd, "Error listening on socket()", "Error", MB_OK);
		closesocket(ServerSocket);
		WSACleanup();
		return(0);
	}
	MessageBox(hwnd, "Everything is ready to roll", "awesome news", MB_OK);
	while(Gloop)
	{
		if (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return(msg.wParam);
}