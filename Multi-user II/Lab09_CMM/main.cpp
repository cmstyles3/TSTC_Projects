// LAB10_Server
// DirectPlay Server for Server/Client Chat program

#include <windows.h>
#include <dplay8.h>
#include <stdio.h>
#include "dxutil.h"

#define APP_NAME "LAB010_SERVER"

#define IDC_LB_OUTPUT 2000
#define IDC_LB_OUTPUT1 2011
#define IDC_BU_HOST 2001
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

static const GUID Lab09_CMM = 
{ 0xb5d50f7f, 0x5b3f, 0x4821, { 0x8e, 0xb6, 0x24, 0x7a, 0x3b, 0x4d, 0xd3, 0x24 } };


//DIRECTPLAY
PDIRECTPLAY8SERVER g_pDPServer;
PDIRECTPLAY8PEER g_pDP = 0;
PDIRECTPLAY8ADDRESS g_pDeviceAddress = 0;
PDIRECTPLAY8ADDRESS g_pHostAddress = 0;
DPNHANDLE g_hConnectAsyncOp;
DPNID g_dpnidLocalPlayer = 0;

//MULTITHREADING
HANDLE                  g_hConnectCompleteEvent;
HRESULT					g_hrConnectComplete;
CRITICAL_SECTION        g_csModifyPlayer;

LONG	g_lNumberOfActivePlayers = 0;
BOOL	g_bHost = 0;

struct PACKET_CHAT
{
	char szText[256];
};


struct PLAYER_INFO
{
	//bool bActive;
	DPNID dpnidPlayer;
	char name[32];
};

#define MAX_PLAYERS 4

PLAYER_INFO g_playerInfo[MAX_PLAYERS];


// WINDOW HANDLES -------------------------------------------------------------------------------

//MAIN WINDOW HANDLE
HWND g_hWnd = 0;

//CHILD WINDOW HANDLES
HWND g_hLB_Output = 0;
HWND g_hBU_Host = 0;
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


// FUNCTION PROTOTYPES ---------------------------------------------------------------------------

void ShowText(HWND hChildHandle, char *Text);

void CreateInterface();

int InitDirectPlay();

void Cleanup();

HRESULT WINAPI DirectPlayMessageHandler(void*  pUserContext, DWORD MessageID, void* pMsgBuffer);

HRESULT HostGame();

HRESULT JoinGame();

HRESULT CreatePlayer(PVOID pUserContext, PVOID pMsgBuffer);

HRESULT DestroyPlayer(PVOID pUserContext, PVOID pMsgBuffer);

HRESULT SendChatMessage(int player, char *message);


LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char message[256];
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
				case IDC_BU_HOST:
					
					HostGame();
					break;
				case IDC_BU_SEND:
					GetWindowText(g_hEB_Message, message, 256);
					//message[strlen(message)-1] = '\0';
					sprintf(compMessage, "<%s> %s", g_playerInfo[0].name, message);
					SendChatMessage(-1,compMessage);
					SetWindowText(g_hEB_Message,"");
					break;
				case IDC_BU_SOUND:
				{
					sprintf(compMessage, "SOUND PLAYED by %s",g_playerInfo[0].name);
					SendChatMessage(-1, compMessage);
					SendChatMessage(-1, "/snd");
				}break;
				case IDC_BU_HELP:
				{
					MessageBox(0, 
							"HOST INSTRUCTIONS: \n"\
							"Type in IP address in xxx.xxx.xxx.xxx format \n"\
							"Type in port number range from 1000-10000\n"\
							"Enter name up to 32 characters\n"\
							"Now press the [Host] button\n\n"\
							"JOIN INSTRUCTIONS: \n"\
							"Just specify port number\n"\
							"Enter name just like with host\n"\
							"Now press the [Join] button\n\n"\
							"GENERAL INSTRUCTIONS: \n"\
							"Type in message in the edit box at the bottom\n"\
							"and then hit enter or press the [send] button\n"\
							"to send to everyone else who is connected\n"\
							"To trigger a sound to be played on everyone,\n"\
							"press the [sound] button\n"\
							"To terminate session and close the application\n"\
							"press the [x] button at top\n"\
							"To refer back to these instructions,\n"\
							"press the [help] button"
							, "INSTRUCTIONS", 0);

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
						sprintf(compMessage, "<%s> %s", g_playerInfo[0].name, message);
						SendChatMessage(-1,compMessage);
						SetWindowText(g_hEB_Message,"");
					}
			}
		}break;
		case WM_DESTROY:
		{
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

	MessageBox(0, 
		"HOST INSTRUCTIONS: \n"\
		"Type in IP address in xxx.xxx.xxx.xxx format \n"\
		"Type in port number range from 1000-10000\n"\
		"Enter name up to 32 characters\n"\
		"Now press the [Host] button\n\n"\
		"JOIN INSTRUCTIONS: \n"\
		"Just specify port number\n"\
		"Enter name just like with host\n"\
		"Now press the [Join] button\n\n"\
		"GENERAL INSTRUCTIONS: \n"\
		"Type in message in the edit box at the bottom\n"\
		"and then hit enter or press the [send] button\n"\
		"to send to everyone else who is connected\n"\
		"To trigger a sound to be played on everyone,\n"\
		"press the [sound] button\n"\
		"To terminate session and close the application\n"\
		"press the [x] button at top\n"\
		"To refer back to these instructions,\n"\
		"press the [help] button"
		, "INSTRUCTIONS", 0);
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
	wc.lpszClassName = APP_NAME;
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wc))
		return(0);
	if (!(g_hWnd = CreateWindow(APP_NAME, "DirectPlay Peer-to-Peer Chat", WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 100, 100, 640, 400, 0, 0, hInstance, 0)))
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

	g_hBU_Host = CreateWindow("BUTTON", "Host",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
							  380, 25, 60, 28, g_hWnd, (HMENU)IDC_BU_HOST, g_hInstance, 0);

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
	int i;

	//Initialize COM
	
	if (FAILED(CoInitialize(0)))
	{
		MessageBox(g_hWnd,  "Error Initializing COM", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	//Initialize critical sections for multithreading
	InitializeCriticalSection(&g_csModifyPlayer);

	//Create IDirectPlay8Peer object
	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Peer, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Peer, (LPVOID*)&g_pDP)))
	{
		MessageBox(g_hWnd,  "Can't Create DPlayPeer", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	//Initialize IDirectPlay8Peer message handler
	if (FAILED(g_pDP->Initialize(0, DirectPlayMessageHandler, 0)))
	{
		MessageBox(g_hWnd,  "Error Initializing COM", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	//Create a device address
	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Address, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (LPVOID*)&g_pDeviceAddress)))
	{
		MessageBox(g_hWnd,  "Failed to Create Device", "CoCreateInstance", MB_ICONERROR);
		return(0);
	}

	//Set service provider to TCP/IP
	if (FAILED(g_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP)))
	{
		MessageBox(g_hWnd,  "Failed to SetSP for Device Address", "Invalid Param", MB_ICONERROR);
		return(0);
	}

	//Create host address
	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Address, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (LPVOID*)&g_pHostAddress)))
	{
		MessageBox(g_hWnd,  "Failed to Host Device", "CoCreateInstance", MB_ICONERROR);
		return(0);
	}

	//Set the host service provider to TCP/IP
	if (FAILED(g_pHostAddress->SetSP(&CLSID_DP8SP_TCPIP)))
	{
		MessageBox(g_hWnd,  "Failed to SetSP for Host Address", "Invalid Param", MB_ICONERROR);
		return(0);
	}

	//Create connection complete event
	g_hConnectCompleteEvent = CreateEvent(0, false, false, 0);

	ShowText(g_hLB_Output, "<<TCP INITIALIZED>>");

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		g_playerInfo[i].bActive = false;
	}


	return(1);
}

void Cleanup()
{
	if (g_pDP)
	{
		g_pDP->Close(0);
		// Release the DirectPlay object
		SAFE_RELEASE( g_pDP );
    }
	
	DeleteCriticalSection( &g_csModifyPlayer );
	CloseHandle( g_hConnectCompleteEvent );

	
	SAFE_RELEASE( g_pDeviceAddress );
    SAFE_RELEASE( g_pHostAddress );
	
	// Uninitialize the COM library
	CoUninitialize();
}


HRESULT WINAPI DirectPlayMessageHandler(void*  pUserContext, DWORD MessageID, void* pMsgBuffer)
{
	switch(MessageID)
	{
		case DPN_MSGID_CREATE_PLAYER:
		{
			CreatePlayer(pUserContext, pMsgBuffer);

		}break;
	
		case DPN_MSGID_DESTROY_PLAYER:
		{
			DestroyPlayer(pUserContext, pMsgBuffer);
			PlaySound("somebodyleft.wav", 0, SND_ASYNC);

		}break;
		
		case DPN_MSGID_TERMINATE_SESSION:
		{
			ShowText(g_hLB_Output, "<<Terminate Session>>");
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg = 
				(PDPNMSG_TERMINATE_SESSION)pMsgBuffer;
		}break;

		case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
			//pReceiveMsg->dpnidSender
			if (!strcmp("/snd", (char*)pReceiveMsg->pReceiveData))
				PlaySound("idostuff.wav", 0, SND_ASYNC);
			else
				ShowText(g_hLB_Output, (char*)pReceiveMsg->pReceiveData);
		}break;

		case DPN_MSGID_CONNECT_COMPLETE:
		{
			PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg =
				(PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;
			g_hrConnectComplete = pConnectCompleteMsg->hResultCode;
			SetEvent(g_hConnectCompleteEvent);
		}break;

	}
	return(S_OK);
}

HRESULT HostGame()
{
	HRESULT ret;
	char peerName[256];
	char sessionName[256];
	WCHAR wPeerName[256];
	WCHAR wSessionName[256];
	DPN_APPLICATION_DESC dnAppDesc;
	DWORD length = 256;
	char strPort[6];
	DPN_PLAYER_INFO dpPlayerInfo;
	DWORD port = 9000;

	// Set up our player information
	GetWindowText(g_hEB_Name, peerName, 36);
	DXUtil_ConvertGenericStringToWide(wPeerName, peerName);
	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wPeerName;

	// set up to be non-asynchronous
	if (FAILED(ret = g_pDP->SetPeerInfo(&dpPlayerInfo, 0, 0, DPNSETPEERINFO_SYNC)))
	{
		MessageBox(g_hWnd, "Failed to SetPeerInfo", "DirectPlay Error", MB_ICONERROR);
		return -1;
	}
	
	// Set up the application description
	sprintf(sessionName, "%s's Game", peerName);
	DXUtil_ConvertGenericStringToWide(wSessionName, sessionName);
	ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dnAppDesc.guidApplication = Lab09_CMM;
	dnAppDesc.pwszSessionName = wSessionName;
	dnAppDesc.dwMaxPlayers = MAX_PLAYERS;
	//dnAppDesc.dwFlags = DPNSESSION_MIGRATE_HOST;

	// Get port from edit box
	GetWindowText(g_hEB_ServerPort, strPort, 6);
	port = atol(strPort);

	// Add port number to address
	ret = g_pDeviceAddress->AddComponent(DPNA_KEY_PORT, &port, sizeof(DWORD), DPNA_DATATYPE_DWORD);
	if (ret != S_OK)
	{
		MessageBox(g_hWnd, "Failed to AddComponent()", "HostGame()", MB_ICONERROR);
		return -1;
	}


	// Host the game
	ret = g_pDP->Host(&dnAppDesc, &g_pDeviceAddress, 1, 0, 0, 0, 0);
	if (FAILED(ret))
	{
		MessageBox(g_hWnd, "Failed to Host()", "DirectPlay Error", MB_ICONERROR);
		return -1;
	}

	//
	g_bHost = true;
	ShowText(g_hLB_Output, "<- Hosting ->");
	return ret;
}

HRESULT CreatePlayer(PVOID pUserContext, PVOID pMsgBuffer)
{
	HRESULT ret = S_OK;
	PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
	char name[256];
	char output[256];
	DWORD size = 0;
	DPN_PLAYER_INFO *pdpPlayerInfo = 0;
	int i;


	// Get a Create Message point to the buffer
	pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

	// Get the peer info and extract its name
	ret = g_pDP->GetPeerInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &size, 0);
	if (FAILED(ret) && ret != DPNERR_BUFFERTOOSMALL)
	{
		ret = -1;
	}
	else
	{
		pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[size];
		ZeroMemory(pdpPlayerInfo, size);
		pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
		ret = g_pDP->GetPeerInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &size, 0);
		if (FAILED(ret))
		{
			ret = -1;
		}
		else
		{
			EnterCriticalSection(&g_csModifyPlayer);
			// Convert player name to ANSI
			DXUtil_ConvertWideStringToGeneric(name, pdpPlayerInfo->pwszName);

			//Add player to list
			for (i = 0; i < MAX_PLAYERS; i++)
			{
				if (!g_playerInfo[i].bActive)
				{
					g_playerInfo[i].bActive = true;
					g_playerInfo[i].dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
					strcpy(g_playerInfo[i].name, name);
					break;
				}
			}

			// Check if free slot was not found
			if (i == MAX_PLAYERS)	ShowText(g_hLB_Output, "No free slots in game");

			// Check if adding ourselves
			else if(pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL)
			{
				g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
				sprintf(output, "Welcome to the game, %s!", name);
				ShowText(g_hLB_Output, output);
			}
			else
			{
				sprintf(output, "<Slot%d><%s> Is in the game", i, name);
				ShowText(g_hLB_Output, output);
				if (g_bHost)
				{
					//SendChatMessage(i, output)
				}
			}
			SAFE_DELETE_ARRAY(pdpPlayerInfo);

			// Update the number of active players in a thread-safe way
			InterlockedIncrement(&g_lNumberOfActivePlayers);
			LeaveCriticalSection(&g_csModifyPlayer);
		}
	}
	return(ret);
}

HRESULT JoinGame()
{
	HRESULT ret = S_OK;
	WCHAR wHostName[256];
	WCHAR wPeerName[256];
	char peerName[256];
	char ip[256];
	char strPort[256];
	DWORD port;
	DWORD length;
	DPN_APPLICATION_DESC dpnAppDesc;
	DPN_PLAYER_INFO dpPlayerInfo;

	ShowText(g_hLB_Output, "Attempting Connection...");

	// Set peer info
	GetWindowText(g_hEB_Name, peerName, 36);
	DXUtil_ConvertGenericStringToWide(wPeerName, peerName);
	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wPeerName;

	// Make this a synchronous call
	if (FAILED(ret = g_pDP->SetPeerInfo(&dpPlayerInfo, 0, 0, DPNSETPEERINFO_SYNC)))
	{
		ShowText(g_hLB_Output, "Failed to set peer info");
		return -1;
	}

	// Prepare the application description
	ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = Lab09_CMM;
	
	// Get IP from edit box
	GetWindowText(g_hEB_ServerIP, ip, 32);

	// Get port from edit box
	GetWindowText(g_hEB_ServerPort, strPort, 6);

	// Convert the IP to a wide string
	DXUtil_ConvertGenericStringToWide(wHostName, ip);
	// Convert the port string to a DWORD
	port = atol(strPort);
	// Add hostname to address
	ret = g_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, wHostName, (wcslen(wHostName) + 1) * sizeof(WCHAR), DPNA_DATATYPE_STRING);
	if (ret != S_OK)
	{
		MessageBox(g_hWnd, "Failed to AddComponent()", "JoinGame()", MB_ICONERROR);
		return -1;
	}
	
	// Add port number to address
	ret = g_pHostAddress->AddComponent(DPNA_KEY_PORT, &port, sizeof(DWORD), DPNA_DATATYPE_DWORD);
	if (ret != S_OK)
	{
		MessageBox(g_hWnd, "Failed to AddComponent()", "JoinGame()", MB_ICONERROR);
		return -1;
	}
	
	// Connect to the session
	ret = g_pDP->Connect(&dpnAppDesc, g_pHostAddress, g_pDeviceAddress, 0,0,0,0,0,0, &g_hConnectAsyncOp, 0);
	if (ret != E_PENDING && FAILED(ret))
	{
		ShowText(g_hLB_Output, "Failed to Connect");
		return -1;
	}
	SetTimer(g_hWnd, TIMERID_CONNECT_COMPLETE, 100, 0);

	return(ret);
}

HRESULT SendChatMessage(int player, char *message)
{
	PACKET_CHAT msgChat;
	DPNHANDLE hAsync;
	DWORD length = strlen(message);
	DPN_BUFFER_DESC bufferDesc;

	// If no message to send, then just return
	if (length == 0)
		return S_OK;
	// Copy the message to sent into the packet
	strcpy(msgChat.szText, message);
	// Set the size of the packet to send
	bufferDesc.dwBufferSize = sizeof(PACKET_CHAT) + length;
	// Copy our packet into the send buffer
	bufferDesc.pBufferData = (BYTE*)&msgChat;
	// Send message to everyone, including ourselves if -1 passed
	if (player == -1)
		g_pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, 0, &hAsync, 0);
	else
		g_pDP->SendTo(g_playerInfo[player].dpnidPlayer, &bufferDesc, 1, 0, 0, &hAsync, 0);
	return S_OK;
}

HRESULT DestroyPlayer(PVOID pUserContext, PVOID pMsgBuffer)
{
	PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
	HRESULT ret = S_OK;
	int i;
	char output[256];
	pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
	if (pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_NORMAL)
	{
		ShowText(g_hLB_Output, "Player Left Normally");
	}
	else if (pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_SESSIONTERMINATED)
	{
		ShowText(g_hLB_Output, "Player Terminated Session");
	}
	else if (pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER)
	{
		ShowText(g_hLB_Output, "Player Kicked by Host");
	}
	// Update the number of active players in a thread-safe way
	InterlockedDecrement(&g_lNumberOfActivePlayers);
	EnterCriticalSection(&g_csModifyPlayer);
	// Remove player from list
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (g_playerInfo[i].bActive)
		{
			if (g_playerInfo[i].dpnidPlayer == pDestroyPlayerMsg->dpnidPlayer)
			{
				g_playerInfo[i].bActive = false;
				sprintf(output, "<slot%d><%s> Left the Game", i, g_playerInfo[i].name);
				ShowText(g_hLB_Output, output);
				break;
			}
		}
	}
	LeaveCriticalSection(&g_csModifyPlayer);
	return(ret);
}