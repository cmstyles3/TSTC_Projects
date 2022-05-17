#define WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <dplay8.h>
#include <mmsystem.h>
#include <stdio.h>
#include <vector>
#include "dxutil.h"

using namespace std;

#define APP_NAME "LAB10_CLIENT"

#define IDC_LB_OUTPUT 2000
#define IDC_LB_OUTPUT1 2011

#define IDC_BU_JOIN 2002
#define IDC_BU_SEND 2010
#define IDC_BU_SOUND 2011
#define IDC_BU_HELP 2012

#define IDC_ST_SERVERIP 2003
#define IDC_ST_SERVERPORT 2004
#define IDC_ST_NAME 2005
#define IDC_EB_SERVERIP 2006
#define IDC_EB_SERVERPORT 2007
#define IDC_EB_NAME 2008
#define IDC_EB_MESSAGE 2009

#define TIMERID_CONNECT_COMPLETE    1

// {4C6D1063-62B3-41a4-869C-F19FD1E09E19}
static const GUID GUID_SERVER = 
{ 0x4c6d1063, 0x62b3, 0x41a4, { 0x86, 0x9c, 0xf1, 0x9f, 0xd1, 0xe0, 0x9e, 0x19 } };

const char AppName[] = "LAB10_Client";
const char Title[] = "Chat Client";

//DIRECTPLAY
PDIRECTPLAY8CLIENT g_pDPClient = 0;
PDIRECTPLAY8ADDRESS g_pDeviceAddress = 0;
PDIRECTPLAY8ADDRESS g_pHostAddress = 0;
DPNHANDLE g_hConnectAsyncOp;
DPNID g_dpnidLocalPlayer = 0;

//MULTITHREADING
HANDLE                  g_hConnectCompleteEvent;
HRESULT					g_hrConnectComplete;

//-----------------------------------------------------------------------------------
// Packet Structures
//-----------------------------------------------------------------------------------
#define PACKET_TYPE_GENERIC 0
#define PACKET_TYPE_CHAT	1
#define PACKET_TYPE_SOUND   3

struct PacketGeneric
{
	DWORD	dwType;
	DWORD	dwSize;
};

struct PacketChat : public PacketGeneric
{
	char szText[128];
};

struct PacketSound : public PacketGeneric
{
	char szText[16];
};

// WINDOW HANDLES -------------------------------------------------------------------------------

//MAIN WINDOW HANDLE
HWND g_hWnd = 0;

//CHILD WINDOW HANDLES
HWND g_hLB_Output = 0;

HWND g_hBU_Join = 0;
HWND g_hBU_Send = 0;
HWND g_hBU_Sound = 0;
HWND g_hBU_Help = 0;

HWND g_hST_ServerIP = 0;
HWND g_hST_ServerPort = 0;
HWND g_hST_Name = 0;

HWND g_hEB_Name = 0;
HWND g_hEB_ServerIP = 0;
HWND g_hEB_ServerPort = 0;
HWND g_hEB_Message = 0;

HINSTANCE g_hInstance = 0;


// Function to display text in the output box
void ShowText(HWND	hChildHandle, char *szText);
// Function to setup display of GUI
void CreateInterface();
// Function called when program exits, cleans up allocated objects
void Cleanup();

//-----------------------------------------------------------------------------------
// DirectPlay Functions
//-----------------------------------------------------------------------------------

// Function to initialize direct play system
int InitDirectPlay();
// Function to handle all incoming Dplay messages
HRESULT WINAPI DirectPlayMessageHandler(PVOID pUserContext, DWORD dwMessageId, PVOID pMsgBuffer);
// Function to join a game
int JoinGame();
// Function to send a message to the server
void SendClientMessage(DWORD dwMessageType, PVOID pMsgBuffer);

void DisplayInstructions();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PacketSound pacSnd;
	PacketChat chatMsg;
	void *packet;
	char message[256];
	char name[32];
	char compMessage[256];
	switch(Msg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_BU_JOIN:
					JoinGame();
					break;
				case IDC_BU_SEND:
					GetWindowText(g_hEB_Message, message, 256);
					GetWindowText(g_hEB_Name, name, 32);
					//message[strlen(message)-1] = '\0';
					sprintf(compMessage, "<%s> %s", name, message);
					//SendServerMessage(SEND,compMessage);
					SetWindowText(g_hEB_Message,"");
					ShowText(g_hLB_Output, compMessage);

					//send a chat packet to all players
					chatMsg.dwSize = sizeof(PacketChat);
					chatMsg.dwType = PACKET_TYPE_CHAT;
					strcpy(chatMsg.szText, compMessage);

					// Convert the packet to a void stream
					packet = (void*)&chatMsg;
					// Send the chat packet to all clients
					SendClientMessage(PACKET_TYPE_CHAT,packet);

					break;
				case IDC_BU_SOUND:
				{
					GetWindowText(g_hEB_Name, name, 32);
					//SendServerMessage(SEND,compMessage);
					sprintf(compMessage, "Sound played by %s", name);
					//ShowText(g_hLB_Output, compMessage);

					//send a chat packet to all players
					pacSnd.dwSize = sizeof(PacketSound);
					pacSnd.dwType = PACKET_TYPE_SOUND;
					strcpy(pacSnd.szText, "/snd");

					// Convert the packet to a void stream
					packet = (void*)&pacSnd;
					// Send the chat packet to all clients
					SendClientMessage(PACKET_TYPE_SOUND, packet);

					chatMsg.dwSize = sizeof(PacketChat);
					chatMsg.dwType = PACKET_TYPE_CHAT;
					strcpy(chatMsg.szText, compMessage);

					packet = (void*)&chatMsg;
					SendClientMessage(PACKET_TYPE_SOUND, packet);

				}break;
				case IDC_BU_HELP:
				{
					DisplayInstructions();

				}break;
			}
			switch(HIWORD(wParam))
			{
				case EN_UPDATE:

					GetWindowText(g_hEB_Message, message, 256);
					if(strlen(message) >= 222) SetWindowText(g_hEB_Message, message);
					if (message[strlen(message)-1] == 10)
					{
						message[strlen(message)-2] = '\0';
						GetWindowText(g_hEB_Name, name, 32);
						sprintf(compMessage, "<%s> %s", name, message);
						SetWindowText(g_hEB_Message,"");
						ShowText(g_hLB_Output, compMessage);

						//send a chat packet to all players
						chatMsg.dwSize = sizeof(PacketChat);
						chatMsg.dwType = PACKET_TYPE_CHAT;
						strcpy(chatMsg.szText, compMessage);

						// Convert the packet to a void stream
						packet = (void*)&chatMsg;
						// Send the chat packet to all clients
						SendClientMessage(PACKET_TYPE_CHAT,packet);
					}
			}
		}break;
		case WM_DESTROY:
		{
			Cleanup();
			PostQuitMessage(0);
		}break;
		case WM_TIMER:
		{
			if (wParam == TIMERID_CONNECT_COMPLETE)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(g_hConnectCompleteEvent, 0))
				{
					if (FAILED(g_hrConnectComplete))
					{
						ShowText(g_hLB_Output,"<<CONNECTION IN-COMPLETE>>");
						PlaySound("connectionincomplete.wav", 0, SND_ASYNC);
					}
					else
					{
						ShowText(g_hLB_Output,"<<CONNECTION COMPLETE>>");
						PlaySound("connectioncomplete.wav", 0, SND_ASYNC);
					}
					KillTimer(hWnd, TIMERID_CONNECT_COMPLETE);
				}
			}
		}break;
	}
	return(DefWindowProc(hWnd, Msg, wParam, lParam));
}


// MAIN ----------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	DisplayInstructions();

	MSG msg;
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = AppName;
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc))
		return(0);
	if (!(g_hWnd = CreateWindow(AppName, Title, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 100, 100, 640, 400, 0, 0, hInstance, 0)))
		return(0);
	g_hInstance = hInstance;

	CreateInterface();
	//initialize directplay
	
	if (!InitDirectPlay())
	{
		Cleanup();
		return(0);
	}
	
		
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Cleanup();
	return(msg.wParam);
}

void ShowText(HWND hChildHandle, char *Text)
{
	int Line;
	SendMessage(hChildHandle, LB_ADDSTRING, 0, (LPARAM)Text);
	Line = SendMessage(hChildHandle, LB_GETCOUNT, 0, 0);
	SendMessage(hChildHandle, LB_SETCURSEL, Line - 1, 0);
	SendMessage(hChildHandle, LB_SETCURSEL, -1, 0);
}

void CreateInterface()
{
	g_hLB_Output = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", 0, 
		                          WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER, 
		                          10, 60, 615, 230, g_hWnd, (HMENU)IDC_LB_OUTPUT, g_hInstance, 0);

	g_hBU_Join = CreateWindow("BUTTON", "Join",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      450, 25, 60, 28, g_hWnd, (HMENU)IDC_BU_JOIN, g_hInstance, 0);

	g_hBU_Help = CreateWindow("BUTTON", "Help",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      520, 25, 60, 28, g_hWnd, (HMENU)IDC_BU_HELP, g_hInstance, 0);

	

	g_hBU_Send = CreateWindow("BUTTON", "Send",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      420, 300, 60, 28, g_hWnd, (HMENU)IDC_BU_SEND, g_hInstance, 0);

	g_hBU_Sound = CreateWindow("BUTTON", "Sound",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      520, 300, 60, 28, g_hWnd, (HMENU)IDC_BU_SOUND, g_hInstance, 0);

	g_hST_ServerIP = CreateWindow("STATIC", "Server IP", WS_CHILD | SS_CENTER | WS_VISIBLE, 
		                          10, 10, 120, 28, g_hWnd, (HMENU)IDC_ST_SERVERIP, g_hInstance, 0);

	g_hST_ServerPort = CreateWindow("STATIC", "Port", WS_CHILD | SS_CENTER | WS_VISIBLE, 
		                            130, 10, 50, 28, g_hWnd, (HMENU)IDC_ST_SERVERPORT, g_hInstance, 0);

	g_hST_Name = CreateWindow("STATIC", "Player Name", WS_CHILD | SS_CENTER | WS_VISIBLE, 
		                           220, 10, 135, 28, g_hWnd, (HMENU)IDC_ST_NAME, g_hInstance, 0);


	g_hEB_Name = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Name", 
		                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		                        220, 25, 135, 28, g_hWnd,(HMENU)IDC_EB_NAME, g_hInstance, 0);

	g_hEB_ServerIP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "127.0.0.1", 
		                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		                        10, 25, 120, 28, g_hWnd,(HMENU)IDC_EB_SERVERIP, g_hInstance, 0);

	g_hEB_ServerPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "6000", 
		                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		                        130, 25, 50, 28, g_hWnd,(HMENU)IDC_EB_SERVERPORT, g_hInstance, 0);

	g_hEB_Message = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
		                        WS_CHILD | WS_VISIBLE | WS_BORDER |WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE,
		                        10, 300, 350, 56, g_hWnd,(HMENU)IDC_EB_MESSAGE, g_hInstance, 0);
}

int InitDirectPlay()
{
	// Initialize COM
	if (FAILED(CoInitialize(0)))
	{
		MessageBox(g_hWnd, "Error Initializing COM", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Client, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Client, (void**)&g_pDPClient)))
	{
		MessageBox(g_hWnd, "Can't Create DirectPlay8Client", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pDPClient->Initialize(0, DirectPlayMessageHandler, 0)))
	{
		MessageBox(g_hWnd, "Failed to Message Handler", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Address, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (void**)&g_pDeviceAddress)))
	{
		MessageBox(g_hWnd, "Failed to Create Device", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP)))
	{
		MessageBox(g_hWnd, "Failed to SetSP() for Device Address", "Invalid Params", MB_ICONERROR);
		return(0);
	}

	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Address, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (void**)&g_pHostAddress)))
	{
		MessageBox(g_hWnd, "Failed to Create Host Address", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pHostAddress->SetSP(&CLSID_DP8SP_TCPIP)))
	{
		MessageBox(g_hWnd, "Failed to SetSP() for Host Address", "Invalid Params", MB_ICONERROR);
		return(0);
	}
	g_hConnectCompleteEvent = CreateEvent(0, 0, 0, 0);
	ShowText(g_hLB_Output, "<<--> TCP INITIALIZED <-->>");
	return(1);
}

void Cleanup()
{
	if (g_pDPClient)
	{
		g_pDPClient->Close(0);
		SAFE_RELEASE(g_pDPClient);
	}
	CloseHandle(g_hConnectCompleteEvent);
	SAFE_RELEASE(g_pDeviceAddress);
	SAFE_RELEASE(g_pHostAddress);
	CoUninitialize();
}

HRESULT WINAPI DirectPlayMessageHandler(PVOID pUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
	PacketChat *chatMsg;
	PacketSound *pacSnd;
	switch(dwMessageId)
	{
		case DPN_MSGID_TERMINATE_SESSION:
			ShowText(g_hLB_Output, "<-Terminate Session->");
			PlaySound("sessionterminated.wav", 0, SND_ASYNC);
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;
			break;
		case DPN_MSGID_RECEIVE:
			PDPNMSG_RECEIVE pReceiveMsg;
			PacketGeneric *pacGen;

			pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;

			pacGen = (PacketGeneric*)pReceiveMsg->pReceiveData;

			if (pacGen->dwType == PACKET_TYPE_CHAT)
			{
				chatMsg = (PacketChat*)pReceiveMsg->pReceiveData;
				ShowText(g_hLB_Output, chatMsg->szText);
			}
			else if (pacGen->dwType == PACKET_TYPE_SOUND)
			{
				pacSnd = (PacketSound*)pReceiveMsg->pReceiveData;
				if (!strcmp(pacSnd->szText, "/snd"))
					PlaySound("idostuff.wav", 0, SND_ASYNC);
			}
			break;
		case DPN_MSGID_CONNECT_COMPLETE:
			PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;
			g_hrConnectComplete = pConnectCompleteMsg->hResultCode;
			SetEvent(g_hConnectCompleteEvent);
			break;
	}
	return(S_OK);
}

int JoinGame()
{
	WCHAR wszHostName[256];
	WCHAR wszClientName[256];
	char szClientName[256];
	char ip[256];
	char port[256];
	DWORD portNum;
	DWORD length = 256;
	DPN_APPLICATION_DESC dpnAppDesc;
	DPN_PLAYER_INFO dpPlayerInfo;
	ShowText(g_hLB_Output, "Attempting Connection....");
	
	GetWindowText(g_hEB_Name, szClientName, 36);
	DXUtil_ConvertGenericStringToWide(wszClientName, szClientName);

	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wszClientName;

	if (FAILED(g_pDPClient->SetClientInfo(&dpPlayerInfo, 0, 0, DPNSETCLIENTINFO_SYNC)))
	{
		return(0);
	}

	ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GUID_SERVER;

	GetWindowText(g_hEB_ServerIP, ip, 32);
	GetWindowText(g_hEB_ServerPort, port, 6);
	DXUtil_ConvertGenericStringToWide(wszHostName, ip);
	portNum = atol(port);

	if (FAILED(g_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, wszHostName, (wcslen(wszHostName)+1) * sizeof(WCHAR), DPNA_DATATYPE_STRING)))
	{
		return(0);
	}

	if (FAILED(g_pHostAddress->AddComponent(DPNA_KEY_PORT, &portNum, sizeof(DWORD), DPNA_DATATYPE_DWORD)))
	{
		return(0);
	}

	HRESULT hReturn = g_pDPClient->Connect(&dpnAppDesc, g_pHostAddress, g_pDeviceAddress, 0, 0, 0, 0, 0, &g_hConnectAsyncOp, 0);

	if (hReturn != E_PENDING && FAILED(hReturn))
	{
		ShowText(g_hLB_Output, "Failed to Connect");
		return(0);
	}
	SetTimer(g_hWnd, TIMERID_CONNECT_COMPLETE, 100, 0);
	return(1);
}

void SendClientMessage(DWORD dwMessageType, PVOID pMsgBuffer)
{
	DPNHANDLE hAsync;
	DWORD length;
	DPN_BUFFER_DESC bufferDesc;
	PacketGeneric *pacGen;

	pacGen = (PacketGeneric*)pMsgBuffer;
	length = pacGen->dwSize;
	if (length == 0)
		return;

	bufferDesc.dwBufferSize = length;
	bufferDesc.pBufferData = (BYTE*)pMsgBuffer;

	g_pDPClient->Send(&bufferDesc, 1, 0, 0, &hAsync, 0);
}

void DisplayInstructions()
{
	MessageBox(0, 
		"CLIENT INSTRUCTS: \n"\
		"Enter the ip of server\n"\
		"Enter the port number, keep port at 6000 for my server\n"\
		"Enter name a name 32 characters max\n"\
		"Now press the [Join] button\n\n"\
		"Type in message in the edit box at the bottom\n"\
		"and then hit enter or press the [send] button\n"\
		"to send to everyone else who is connected\n"\
		"To trigger a sound to be played on everyone,\n"\
		"press the [sound] button\n"\
		"To disconnect from server and close the application\n"\
		"press the [x] button at top\n"\
		"To refer back to these instructions,\n"\
		"press the [help] button"
		, "INSTRUCTIONS", 0);
}