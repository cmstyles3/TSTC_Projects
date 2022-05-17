#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <dplay8.h>
#include <mmsystem.h>
#include <stdio.h>
#include <vector>
#include "dxutil.h"

using namespace std;

#define IDC_LB_OUTPUT 2000

#define IDC_BU_SEND 2010

#define IDC_BU_HELP 2012


#define IDC_EB_MESSAGE 2009

// Timer ids
#define TIMERID_CONNECT_COMPLETE    1


// {4C6D1063-62B3-41a4-869C-F19FD1E09E19}
static const GUID GUID_SERVER = 
{ 0x4c6d1063, 0x62b3, 0x41a4, { 0x86, 0x9c, 0xf1, 0x9f, 0xd1, 0xe0, 0x9e, 0x19 } };

const char AppName[] = "Lab10_Server";
const char Title[] = "Chat Server";

//DIRECTPLAY
PDIRECTPLAY8SERVER g_pDPServer = 0;
PDIRECTPLAY8ADDRESS g_pDeviceAddress = 0;
DPNHANDLE g_hConnectAsyncOp;
DPNID g_dpnidLocalPlayer = 0;

struct PLAYER_INFO
{
	bool bActive;
	DPNID dpnidPlayer;
	char name[32];
};

vector<PLAYER_INFO> PlayerInfo;

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

//MULTITHREADING
HANDLE g_hConnectCompleteEvent;
HRESULT g_hrConnectComplete;
CRITICAL_SECTION g_csModifyPlayer;

LONG g_lNumberOfActivePlayers = 0;

// WINDOW HANDLES -------------------------------------------------------------------------------

//MAIN WINDOW HANDLE
HWND g_hWnd = 0;

//CHILD WINDOW HANDLES
HWND g_hLB_Output = 0;

HWND g_hBU_Send = 0;

HWND g_hBU_Help = 0;


HWND g_hEB_Message = 0;

HINSTANCE g_hInstance = 0;


// FUNCTION PROTOTYPES ---------------------------------------------------------------------------

void ShowText(HWND hChildHandle, char *Text);

void CreateInterface();

int InitDirectPlay();

void Cleanup();

HRESULT WINAPI DirectPlayMessageHandler(void*  pUserContext, DWORD MessageID, void* pMsgBuffer);

int HostGame();

int CreatePlayer(PVOID pUserContext, PVOID pMsgBuffer);

void DestroyPlayer(PVOID pUserContext, PVOID pMsgBuffer);

int SendServerMessage(int player, DWORD dwMessageType, PVOID pMsgBuffer);

void DisplayInstructions();


LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PacketChat chatMsg;
	void *packet;
	char message[256];
	char compMessage[256];
	switch(Msg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_BU_SEND:
					GetWindowText(g_hEB_Message, message, 256);
					sprintf(compMessage, "<Server> %s", message);
					SetWindowText(g_hEB_Message,"");
					ShowText(g_hLB_Output, compMessage);

					//send a chat packet to all players
					chatMsg.dwSize = sizeof(PacketChat);
					chatMsg.dwType = PACKET_TYPE_CHAT;
					strcpy(chatMsg.szText, compMessage);

					// Convert the packet to a void stream
					packet = (void*)&chatMsg;
					// Send the chat packet to all clients
					SendServerMessage(DPNID_ALL_PLAYERS_GROUP,PACKET_TYPE_CHAT,packet);

					break;
				
				case IDC_BU_HELP:
				{
					DisplayInstructions();
					

				}break;
			}
			switch(HIWORD(wParam))
			{
				case EN_UPDATE:

					GetWindowText(g_hEB_Message, message, 256);
					if (message[strlen(message)-1] == 10)
					{
						message[strlen(message)-2] = '\0';
						sprintf(compMessage, "<Server> %s", message);
						SetWindowText(g_hEB_Message,"");
						ShowText(g_hLB_Output, compMessage);

						//send a chat packet to all players
						chatMsg.dwSize = sizeof(PacketChat);
						chatMsg.dwType = PACKET_TYPE_CHAT;
						strcpy(chatMsg.szText, compMessage);

						// Convert the packet to a void stream
						packet = (void*)&chatMsg;
						// Send the chat packet to all clients
						SendServerMessage(DPNID_ALL_PLAYERS_GROUP,PACKET_TYPE_CHAT,packet);
					}
			}
		}break;
		case WM_DESTROY:
		{
			Cleanup();
			PostQuitMessage(0);
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

	HostGame();
		
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(AppName, g_hInstance);
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
		                          10, 10, 615, 270, g_hWnd, (HMENU)IDC_LB_OUTPUT, g_hInstance, 0);

	

	g_hBU_Help = CreateWindow("BUTTON", "Help",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      520, 300, 60, 28, g_hWnd, (HMENU)IDC_BU_HELP, g_hInstance, 0);

	

	g_hBU_Send = CreateWindow("BUTTON", "Send",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						      420, 300, 60, 28, g_hWnd, (HMENU)IDC_BU_SEND, g_hInstance, 0);

	

	g_hEB_Message = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
		                        WS_CHILD | WS_VISIBLE | WS_BORDER |WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE,
		                        10, 300, 350, 56, g_hWnd,(HMENU)IDC_EB_MESSAGE, g_hInstance, 0);
}

int InitDirectPlay()
{
	//Initialize COM
	
	if (FAILED(CoInitialize(0)))
	{
		MessageBox(g_hWnd,  "Error Initializing COM", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	InitializeCriticalSection( &g_csModifyPlayer );

	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Server, 0, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server, (void**)&g_pDPServer)))
	{
		MessageBox(g_hWnd, "Can't Create DirectPlay Server", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pDPServer->Initialize(0, DirectPlayMessageHandler, 0)))
	{
		MessageBox(g_hWnd, "Can't Initialize message handler for Server", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(CoCreateInstance(CLSID_DirectPlay8Address, 0, CLSCTX_ALL, IID_IDirectPlay8Address, (void**)&g_pDeviceAddress)))
	{
		MessageBox(g_hWnd, "Can't Create Device Address", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP)))
	{
		MessageBox(g_hWnd, "Can't Set Service provider", "DirectPlay Error", MB_ICONERROR);
		return(0);
	}

	ShowText(g_hLB_Output,"<<--TCP INITED-->>");

	
	
	return(1);
}

void Cleanup()
{
	if(g_pDPServer) 
	{
		// Close our connection
        g_pDPServer->Close(0);
		// Release the DirectPlay object
        SAFE_RELEASE( g_pDPServer );
    }
	
	DeleteCriticalSection( &g_csModifyPlayer );
	SAFE_RELEASE( g_pDeviceAddress );
	
	// Uninitialize the COM library
	CoUninitialize();
}

HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
	HRESULT	hReturn = S_OK;
	
	switch(dwMessageId) 
	{
		case DPN_MSGID_CREATE_PLAYER:
		{
			CreatePlayer(pvUserContext,pMsgBuffer);
			break;
		}
		
		case DPN_MSGID_DESTROY_PLAYER:
		{
			PacketChat *pacChat;
			PDPNMSG_DESTROY_PLAYER pDestroyMsg;
			pDestroyMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
			char output[256];
			for(int i = 0; i < PlayerInfo.size(); i++)
			{
				if (PlayerInfo[i].dpnidPlayer == pDestroyMsg->dpnidPlayer)
				{
					sprintf(output, "<%s> has left the game", PlayerInfo[i].name);
					ShowText(g_hLB_Output, output);
					PlaySound("somebodylefted.wav", 0, SND_ASYNC);
					
					break;
				}
			}
			
			DestroyPlayer(pvUserContext,pMsgBuffer);
			break;
		}
		
		case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE pReceiveMsg;
			PacketGeneric *pacGen;

			pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
			pacGen = (PacketGeneric*)pReceiveMsg->pReceiveData;

			// Ignore the packet if we sent it originally
			if (pReceiveMsg->dpnidSender == g_dpnidLocalPlayer) 
				break;
			
			// If it is a chat packet, send it out to all of the players
			if (pacGen->dwType == PACKET_TYPE_CHAT) 
			{
				void *packet;
				PacketChat *ChatMsg;
				// Convert the packet to a void stream
				ChatMsg = (PacketChat*)pReceiveMsg->pReceiveData;
				packet = (void*)ChatMsg;
				// Send the chat packet to all clients
				SendServerMessage(DPNID_ALL_PLAYERS_GROUP, PACKET_TYPE_CHAT, packet);
				// Output it on our screen
				ShowText(g_hLB_Output, ChatMsg->szText);
			}
			else if (pacGen->dwType == PACKET_TYPE_SOUND)
			{
				void *packet;
				PacketSound *pacSnd;
				pacSnd = (PacketSound*)pReceiveMsg->pReceiveData;
				packet = (void*)pacSnd;
				SendServerMessage(DPNID_ALL_PLAYERS_GROUP, PACKET_TYPE_SOUND, packet);
				if (!strcmp(pacSnd->szText, "/snd"))
					PlaySound("idostuff.wav", 0, SND_ASYNC);
			}
			break;
		}
	}
	
	return hReturn;
}

int HostGame()
{
	WCHAR wszServerName[256];
	WCHAR wszSessionName[256];
	DPN_APPLICATION_DESC dnAppDesc;
	DWORD length = 256;
	DPN_PLAYER_INFO dpPlayerInfo;
	DWORD port = 6000;

	DXUtil_ConvertGenericStringToWide(wszServerName, "Chat Server");
	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wszServerName;

	// set up to be non-asynchronous
	if (FAILED(g_pDPServer->SetServerInfo(&dpPlayerInfo, 0, 0, DPNSETSERVERINFO_SYNC)))
	{
		MessageBox(g_hWnd, "Failed to SetServerInfo()", "Unknown Error", MB_ICONERROR);
		return(0);
	}

	DXUtil_ConvertGenericStringToWide(wszSessionName, "Chat Session");
	ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dnAppDesc.guidApplication = GUID_SERVER;
	dnAppDesc.pwszSessionName = wszSessionName;
	dnAppDesc.dwMaxPlayers = 0; //allow unlimited
	dnAppDesc.dwFlags = DPNSESSION_CLIENT_SERVER;

	if (FAILED(g_pDeviceAddress->AddComponent(DPNA_KEY_PORT, &port, sizeof(DWORD), DPNA_DATATYPE_DWORD)))
	{
		MessageBox(g_hWnd, "Failed to AddComponent()", "HostGame()", MB_ICONERROR);
		return(0);
	}

	if (FAILED(g_pDPServer->Host(&dnAppDesc, &g_pDeviceAddress, 1, 0, 0, 0, 0)))
	{
		MessageBox(g_hWnd, "Failed to Host()", "HostGame()", MB_ICONERROR);
		return(0);
	}

	return(1);
} 

int CreatePlayer(PVOID pUserContext, PVOID pMsgBuffer)
{
	HRESULT hResult;
	PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
	char strName[256];
	char szOutput[256];
	DWORD size = 0;
	DPN_PLAYER_INFO *pdpPlayerInfo = 0;
	int i;

	//Get a create message pointer to the buffer
	pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;
	
	hResult = g_pDPServer->GetClientInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &size, 0);
	if (FAILED(hResult) && hResult != DPNERR_BUFFERTOOSMALL)
	{
		if (hResult == DPNERR_INVALIDPLAYER)
		{
			ShowText(g_hLB_Output, "Adding Ourselves");
		}
		return(0);
	}
	else
	{
		pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[size];
		ZeroMemory(pdpPlayerInfo, size);
		pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
		
		if (FAILED(g_pDPServer->GetClientInfo(pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &size, 0)))
		{
			ShowText(g_hLB_Output, "Error Getting Client Info");
			return(0);
		}
		else
		{
			EnterCriticalSection(&g_csModifyPlayer);
			DXUtil_ConvertWideStringToGeneric(strName, pdpPlayerInfo->pwszName);
			PLAYER_INFO playerInfo;
			playerInfo.dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
			strcpy(playerInfo.name, strName);
			PlayerInfo.push_back(playerInfo);
		

			if (pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL)
			{
				g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
				ShowText(g_hLB_Output, "Added Ourselves");
			}
			else
			{
				sprintf(szOutput, "<%s> Is in the game", strName);
				ShowText(g_hLB_Output, szOutput);
			}
			SAFE_DELETE_ARRAY(pdpPlayerInfo);
			InterlockedIncrement(&g_lNumberOfActivePlayers);
			LeaveCriticalSection(&g_csModifyPlayer);
		}
	}
	return(1);
}

void DestroyPlayer(PVOID pUserContext, PVOID pMsgBuffer)
{
	PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
	char szOutput[256];
	vector<PLAYER_INFO> temp;

	pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
	EnterCriticalSection(&g_csModifyPlayer);
	InterlockedDecrement(&g_lNumberOfActivePlayers);

	for (int i = 0; i < PlayerInfo.size(); i++)
	{
		if (PlayerInfo[i].dpnidPlayer != pDestroyPlayerMsg->dpnidPlayer)
			temp.push_back(PlayerInfo[i]);
	}
	PlayerInfo = temp;
	LeaveCriticalSection(&g_csModifyPlayer);
}

int SendServerMessage(int player, DWORD dwMessageType, PVOID pMsgBuffer)
{
	DPNHANDLE hAsync;
	DWORD length;
	DPN_BUFFER_DESC bufferDesc;
	PacketGeneric *pacGen;

	pacGen = (PacketGeneric*)pMsgBuffer;
	length = pacGen->dwSize;
	if (length == 0)
		return(0);
	bufferDesc.dwBufferSize = length;
	bufferDesc.pBufferData = (BYTE*)pMsgBuffer;

	if (player == DPNID_ALL_PLAYERS_GROUP)
		g_pDPServer->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, 0, &hAsync, DPNSEND_NOLOOPBACK);
	return(1);
}

void DisplayInstructions()
{
	MessageBox(0, 
		"SERVER INSTRUCTIONS: \n"\
		"Type in message in the edit box at the bottom\n"\
		"and then hit enter or press the [send] button\n"\
		"to send a message to all clients that currently connected\n"\
		"To terminate session and close the application\n"\
		"press the [x] button at top\n"\
		"To refer back to these instructions,\n"\
		"press the [help] button"
		, "INSTRUCTIONS", 0);
}