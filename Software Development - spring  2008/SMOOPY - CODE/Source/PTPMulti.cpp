#include "PTPMulti.h"
GUID PTP = { 0x1b12317f, 0x39c9, 0x4920, { 0xa3, 0xa4, 0xf7, 0xa2, 0xc1, 0x60, 0x5a, 0x42 } };

//Dplay objects

IDirectPlay8Peer*       g_pDP;
IDirectPlay8Address*    g_pDeviceAddress;
IDirectPlay8Address*    g_pHostAddress;
DPNHANDLE				g_hConnectAsyncOp;
DPNID					g_dpnidLocalPlayer = 0;


PLAYER_INFORMATION	PlayerInfo[MAX_PLAYERS];

BULLETS Bullets[MAX_BULLETS];
D3DXMATRIX bulletRot;

//other variables

HANDLE                  g_hConnectCompleteEvent;
HRESULT					g_hrConnectComplete;
CRITICAL_SECTION        g_csModifyPlayer;

LONG	g_lNumberOfActivePlayers = 0;
BOOL	bHost = 0;

extern char dpOutput[80];

HRESULT InitDirectPlay( HWND hwnd ) 
{
	HRESULT	hReturn;
	int		i;
	
	hReturn = CoInitialize( NULL ); // Init COM
	if( FAILED(hReturn) ) 
	{
		MessageBox( hwnd, "Error Initializing COM", "DirectPlay Error", MB_ICONERROR );
		return hReturn;
	}
	
	InitializeCriticalSection( &g_csModifyPlayer ); // Init critical sections for multi-thread
	
	if( FAILED( hReturn = CoCreateInstance( CLSID_DirectPlay8Peer, // Create IDirectPlay8Peer Object
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_IDirectPlay8Peer, 
		(LPVOID*) &g_pDP ) ) )
        MessageBox( hwnd, "Can't Create DPlayPeer", "DirectPlay Error", MB_ICONERROR );
	
	if( FAILED( hReturn = g_pDP->Initialize( NULL, DirectPlayMessageHandler, 0 ) ) ) // Init IDirectPlay8Peer Message Handler
	{
		MessageBox( hwnd, "Failed to Message Handler", "DirectPlay Error", MB_ICONERROR );
		return -1;
	}
	
	hReturn = CoCreateInstance( CLSID_DirectPlay8Address, NULL,CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (LPVOID*) &g_pDeviceAddress ); // Create a device address
	if( FAILED(hReturn) ) 
	{
		MessageBox( hwnd, "Failed to Create Device", "CoCreateInstance()", MB_ICONERROR );
		return -1;
	}
	
	if( FAILED( hReturn = g_pDeviceAddress->SetSP( &CLSID_DP8SP_TCPIP ) ) ) // Set our service provider to TCP/IP
	{
		MessageBox( hwnd, "Failed to SetSP() for Device Address", "Invalid Param", MB_ICONERROR );
		return -1;
	}
	
	hReturn = CoCreateInstance( CLSID_DirectPlay8Address, NULL,CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (LPVOID*) &g_pHostAddress ); // Create a host address
	if( FAILED(hReturn) ) 
	{
		MessageBox( hwnd, "Failed to Create Host Address()", "Invalid Param", MB_ICONERROR );
		return -1;
	}

	if( FAILED( hReturn = g_pHostAddress->SetSP( &CLSID_DP8SP_TCPIP ) ) ) // Set the host address to TCP/IP
	{
		MessageBox( hwnd, "Failed to SetSP() for Host Address", "Invalid Param", MB_ICONERROR );
		return -1;
	}

	g_hConnectCompleteEvent = CreateEvent( NULL, FALSE, FALSE, NULL ); // Create connection complete event for later use

	//vShowText(hLB_Output,"<<--TCP INITED-->>");
	//MessageBox( hwnd, "------TCP INITED------", "Check Point", MB_OK );

	for( i = 0 ; i < MAX_PLAYERS ; i++ ) // Init miscellaneous variables
	{	
		PlayerInfo[i].bActive = 0;
	}
	
	return S_OK;
}

void vCleanup(void)
{
	if( g_pDP ) 
	{
        g_pDP->Close(0);
        SAFE_RELEASE( g_pDP );
    }
	
	DeleteCriticalSection( &g_csModifyPlayer );
	CloseHandle( g_hConnectCompleteEvent );
	SAFE_RELEASE( g_pDeviceAddress );
    SAFE_RELEASE( g_pHostAddress );
	
	// Uninitialize the COM library
	CoUninitialize();
}

HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
	HRESULT	hReturn = S_OK;
	int playerID;
	MAGICMATRIX_PACKET *pacMagicMatrix;
	SHIP_PACKET *pacShip;
	BULLET_PACKET *pacBul;
	POSITION_PACKET *pacPos;
	switch( dwMessageId ) 
	{
		case DPN_MSGID_CREATE_PLAYER:
		{
			PDPNMSG_CREATE_PLAYER pCreatePlayer;
			pCreatePlayer = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;
			//pCreatePlayer->

			//vShowText(hLB_Output,"Creating Player");
			CreatePlayer(pvUserContext,pMsgBuffer);
			break;
		}
		
		case DPN_MSGID_DESTROY_PLAYER:
		{
			DestroyPlayer(pvUserContext,pMsgBuffer);
			break;
		}
		
		case DPN_MSGID_HOST_MIGRATE:
		{
			//vShowText(hLB_Output,"Migrate Host");
			PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
			pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;
			
			// Check to see if we are the new host
			if( pHostMigrateMsg->dpnidNewHost == g_dpnidLocalPlayer ) 
			{
				sprintf(dpOutput,"You are now Hosting");
				//vShowText(hLB_Output,"(HOSTING)");
			}
			break;
		}
		
		case DPN_MSGID_TERMINATE_SESSION:
		{
			//vShowText(hLB_Output,"<-Terminating Session->");
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;
			break;
		}
		
		case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE pReceiveMsg;
			GENERIC_PACKET *PGen;
			pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
			PGen = (GENERIC_PACKET*)pReceiveMsg->pReceiveData;
			pacBul = (BULLET_PACKET*)pReceiveMsg->pReceiveData;
			if (PGen->type == PACKET_TYPE_MAGICMATRIX)
			{
				playerID = GetPlayerID(pReceiveMsg->dpnidSender);
				pacMagicMatrix = (MAGICMATRIX_PACKET*)pReceiveMsg->pReceiveData;
				EnterCriticalSection(&g_csModifyPlayer);
				PlayerInfo[playerID].matrixMagic = pacMagicMatrix->magicMatrix;
				LeaveCriticalSection(&g_csModifyPlayer);
			}
			else if (PGen->type == PACKET_TYPE_SHIP)
			{
				playerID = GetPlayerID(pReceiveMsg->dpnidSender);
				pacShip = (SHIP_PACKET*)pReceiveMsg->pReceiveData;
				EnterCriticalSection(&g_csModifyPlayer);
				PlayerInfo[playerID].ship = pacShip->ship;
				LeaveCriticalSection(&g_csModifyPlayer);
			}
			else if (PGen->type == PACKET_TYPE_BULLET && pacBul->bullet.dpOwner != g_dpnidLocalPlayer)
			{
				
				CreateBullet(pacBul->bullet.dpOwner, pacBul->bullet.R, pacBul->bullet.x, pacBul->bullet.y,
					pacBul->bullet.z, pacBul->bullet.xv, pacBul->bullet.yv, pacBul->bullet.zv);
				static int bulletNum = 0;
				bulletNum++;
					sprintf(dpOutput, "bullet %i", bulletNum);
			}
			else if (PGen->type == PACKET_TYPE_POSITION)
			{
				playerID = GetPlayerID(pReceiveMsg->dpnidSender);
				pacPos = (POSITION_PACKET*)pReceiveMsg->pReceiveData;
				EnterCriticalSection(&g_csModifyPlayer);
				PlayerInfo[playerID].x = pacPos->x;
				PlayerInfo[playerID].y = pacPos->y;
				PlayerInfo[playerID].z = pacPos->z;
				LeaveCriticalSection(&g_csModifyPlayer);
			}

			//sprintf(dpOutput,(char*)pReceiveMsg->pReceiveData);
			
			//vShowText(hLB_Output,(char*)pReceiveMsg->pReceiveData);
			
			break;
		}
		
		case DPN_MSGID_CONNECT_COMPLETE:
		{
			PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
			pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;
			
			g_hrConnectComplete = pConnectCompleteMsg->hResultCode;
			SetEvent( g_hConnectCompleteEvent );
	
			break;
		}
	}
	
	return hReturn;
}

HRESULT	SendPeerMessage(int player, PVOID pMsgBuffer)
{
	
	DPNHANDLE		hAsync;
    DWORD			dwLength;
	DPN_BUFFER_DESC bufferDesc;
	GENERIC_PACKET *PGen;

	PGen = (GENERIC_PACKET*)pMsgBuffer;
	dwLength = PGen->size;
	
	
	// return if no messages to send
    if( dwLength == 0 )
		return S_OK;
	

	// Set the size of the packet to send
    bufferDesc.dwBufferSize = dwLength;
	// Copy our packet into the send buffer
    bufferDesc.pBufferData  = (BYTE*)pMsgBuffer;
	// Send message to everyone including ourselves if -1 passed
	if( player == -1)
		g_pDP->SendTo( DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, 0 );
	// Send to specific player otherwise
	else
		g_pDP->SendTo( PlayerInfo[player].dpnidPlayer, &bufferDesc, 1, 0, NULL, &hAsync, 0 );
		
    return S_OK;
}

HRESULT	CreatePlayer( PVOID pvUserContext, PVOID pMsgBuffer )
{
	HRESULT					hReturn = S_OK;
    PDPNMSG_CREATE_PLAYER	pCreatePlayerMsg;
	char					strName[256];
	char					szOutput[256];
	DWORD					dwSize = 0;
	DPN_PLAYER_INFO			*pdpPlayerInfo = NULL;
	int						i;
		
	// Get a Create Message pointer to the buffer
	pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;
	
    // Get the peer info and extract its name
    hReturn = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
    if( FAILED(hReturn) && hReturn != DPNERR_BUFFERTOOSMALL ) {
        hReturn = -1;
	}
	else {
		// Allocate memory for the player info
		pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
		
		ZeroMemory( pdpPlayerInfo, dwSize );
		pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
		// Load the player info into the newly allocated data
		hReturn = g_pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
		if( FAILED(hReturn) ) {
			hReturn = -1;
		}
		else {
			EnterCriticalSection( &g_csModifyPlayer );
			
			// Convert player name to ANSI
			DXUtil_ConvertWideStringToGeneric( strName, pdpPlayerInfo->pwszName );
			// Add player to list
			for( i = 0 ; i < MAX_PLAYERS ; i++ ) 
			{
				if( !PlayerInfo[i].bActive ) {
					PlayerInfo[i].bActive = 1;
					PlayerInfo[i].dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
					strcpy(PlayerInfo[i].szPlayerName,strName);
					break;
				}
			}
			// Check if no free slot found
			if( i == MAX_PLAYERS ) 
			{
				//vShowText(hLB_Output,"No free slots in game!");
			}
			// Check if we are adding ourselves
			else if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL ) 
			{
				g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
				sprintf(dpOutput,"<Slot%d> Added Ourselves",i);
				//vShowText(hLB_Output,szOutput);
			}
			else 
			{
				sprintf(dpOutput,"<Slot%d><%s> Is In The Game",i,strName);
				//vShowText(hLB_Output,szOutput);
				// Send them a welcoming message if we are the host
				sprintf(dpOutput,"Welcome to the game, %s!",strName);
				/*if( bHost ) 
				{
					SendMessage(i,szOutput);	
				}*/
			}

			SAFE_DELETE_ARRAY( pdpPlayerInfo );
			
			// Update the number of active players in a thread safe way
			InterlockedIncrement( &g_lNumberOfActivePlayers );

			LeaveCriticalSection( &g_csModifyPlayer );
		}
	}
		
	return hReturn;
}

HRESULT	DestroyPlayer( PVOID pvUserContext, PVOID pMsgBuffer )
{
	PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg;
	HRESULT					hReturn = S_OK;
	int						i;
	char					szOutput[256];
		
	// Get a Destroy Message pointer to the buffer
	pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
	
	if( pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_NORMAL ) 
	{
		//vShowText(hLB_Output,"Player Left Normally");
	}
	else if( pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_CONNECTIONLOST  ) 
	{
		//vShowText(hLB_Output,"Connection Lost w/Player");
	}
	else if( pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_SESSIONTERMINATED  ) 
	{
		//vShowText(hLB_Output,"Player Terminated Session");
	}
	else if( pDestroyPlayerMsg->dwReason == DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER  ) 
	{
		//vShowText(hLB_Output,"Player Kicked By Host");
	}
	
	// Update the number of active players in a thread safe way
	InterlockedDecrement( &g_lNumberOfActivePlayers );

	EnterCriticalSection( &g_csModifyPlayer );

	// Remove Player from list
	for( i = 0 ; i < MAX_PLAYERS ; i++ ) 
	{
		if( PlayerInfo[i].bActive ) {
			if( PlayerInfo[i].dpnidPlayer == pDestroyPlayerMsg->dpnidPlayer ) {
				PlayerInfo[i].bActive = 0;
				sprintf(szOutput,"<Slot%d><%s> Left The Game",i,PlayerInfo[i].szPlayerName);
				//vShowText(hLB_Output,szOutput);
				break;
			}
		}
	}
	
	LeaveCriticalSection( &g_csModifyPlayer );
	
	return(hReturn);
}

HRESULT JoinGame( HWND hwnd )
{
	HRESULT					hReturn = S_OK;
	WCHAR					wszHostName[256];
	WCHAR					wszPeerName[256];
	char					szPeerName[256];
	char					szIP[256];
	char					szPort[256];
	DWORD					dwPort;
	DWORD					dwLength = 256;
	DPN_APPLICATION_DESC	dpnAppDesc;
	DPN_PLAYER_INFO			dpPlayerInfo;

	//vShowText(hLB_Output,"Attempting Connection...");

	FILE *fp = fopen("config.txt","r");
	fgets(szIP,16,fp);
	szIP[strlen(szIP)-1] = '\0';
	fgets(szPeerName,32,fp);
	szPeerName[strlen(szPeerName)-1] = '\0';
	fclose(fp);

	// Set the peer info
	//GetWindowText(hEB_InputName,szPeerName,36);	// Get name from Window Edit Box
	DXUtil_ConvertGenericStringToWide( wszPeerName, szPeerName );
	
	ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wszPeerName;
	
	// Make this a synchronous call
	if( FAILED( hReturn = g_pDP->SetPeerInfo( &dpPlayerInfo, NULL, NULL, DPNSETPEERINFO_SYNC ) ) ) 
	{
		//vShowText(hLB_Output,"Failed to set peer info");
		return -1;
	}
		
	// Prepare the application description
	ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
	dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
	dpnAppDesc.guidApplication = PTP;

	// Get IP from edit box
	//GetWindowText(hEB_InputServerIP,szIP,32);
	// Get Port from edit box
	//GetWindowText(hEB_InputServerPort,szPort,6);
	// Convert the IP to a wide string
	//DXUtil_ConvertGenericStringToWide( wszHostName, szIP );
	// Convert the port string to a DWORD
	//dwPort = atol(szPort);

	// Set the IP
	DXUtil_ConvertGenericStringToWide( wszHostName, szIP );
	// Set the port number
	dwPort = 6000;

	// Add host name to address
	hReturn = g_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME,wszHostName,(wcslen(wszHostName)+1)*sizeof(WCHAR),DPNA_DATATYPE_STRING);
	if( hReturn != S_OK ) 
	{
		MessageBox( hwnd, "Failed to AddComponent()", "hrJoinGame()", MB_ICONERROR );
		return -1;
	}
	// Add port number to address
	hReturn = g_pHostAddress->AddComponent(DPNA_KEY_PORT,&dwPort,sizeof(DWORD),DPNA_DATATYPE_DWORD);
	if( hReturn != S_OK ) 
	{
		MessageBox( hwnd, "Failed to AddComponent()", "hrJoinGame()", MB_ICONERROR );
		return -1;
	}

	// Connect to the session
	hReturn = g_pDP->Connect(	&dpnAppDesc,
								g_pHostAddress,    
								g_pDeviceAddress,
								NULL,
								NULL,
								NULL,	
								0,
								NULL,
								NULL, 
								&g_hConnectAsyncOp,
								NULL); // flags
	
	if( hReturn != E_PENDING && FAILED(hReturn) ) 
	{
		//vShowText(hLB_Output,"Failed to Connect");
		return -1;
	}
	SetTimer( hwnd, TIMERID_CONNECT_COMPLETE, 100, NULL );

	return(hReturn);
}

HRESULT	HostGame( HWND hwnd)
{
	HRESULT					hReturn;
	char					szPeerName[256];
	char					szSessionName[256];
	WCHAR					wszPeerName[256];
	WCHAR					wszSessionName[256];
	DPN_APPLICATION_DESC	dnAppDesc= {0};
	char					szIP[16];
	DWORD					dwLength = 256;
	DPN_PLAYER_INFO			dpPlayerInfo;
	DWORD					dwPort = 9000;
	
	FILE *fp = fopen("config.txt","r");
	fgets(szIP,16,fp);
	szIP[strlen(szIP)-1] = '\0';
	fgets(szPeerName,32,fp);
	szPeerName[strlen(szPeerName)-1] = '\0';
	fclose(fp);

	//
	// Setup our player information
	//
	//GetWindowText(hEB_InputName,szPeerName,36);	// Get name from Window Edit Box
	DXUtil_ConvertGenericStringToWide( wszPeerName, szPeerName );
	ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = wszPeerName;

	MessageBox( hwnd, "SetPeerInfo()", "DirectPlay", MB_ICONERROR );
	
	// Set us up to be non-asynchronous
	if( FAILED( hReturn = g_pDP->SetPeerInfo( &dpPlayerInfo, NULL, NULL, DPNSETPEERINFO_SYNC ) ) ) 
	{
		MessageBox( hwnd, "Failed to SetPeerInfo()", "DirectPlay Error", MB_ICONERROR );
		return -1;
	}
	
	// Setup the application description
	sprintf(szSessionName,"%s's Game",szPeerName);
	DXUtil_ConvertGenericStringToWide( wszSessionName, szSessionName );
	
	ZeroMemory( &dnAppDesc, sizeof(DPN_APPLICATION_DESC) );
	dnAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
	dnAppDesc.guidApplication	= PTP;
	dnAppDesc.pwszSessionName	= wszSessionName;
	dnAppDesc.dwMaxPlayers		= MAX_PLAYERS;
	dnAppDesc.dwFlags			= DPNSESSION_MIGRATE_HOST;
	
	// Get Port from edit box
	//GetWindowText(hEB_InputServerPort,szPort,6);
	// Convert the port string to a DWORD
	//dwPort = atol(szPort);
	
	MessageBox( hwnd, "AddComponent()", "DirectPlay", MB_ICONERROR );

	dwPort = 6000;

	// Add port number to address
	hReturn = g_pDeviceAddress->AddComponent(DPNA_KEY_PORT,&dwPort,sizeof(DWORD),DPNA_DATATYPE_DWORD);
	if( hReturn != S_OK ) 
	{
		MessageBox( hwnd, "Failed to AddComponent()", "hrHostGame()", MB_ICONERROR );
		return -1;
	}
	MessageBox( hwnd, "host()", "DirectPlay", MB_ICONERROR );
	// Host the game
	hReturn = g_pDP->Host(	&dnAppDesc,               
		&g_pDeviceAddress,        
		1,                        
		NULL, 
		NULL,               
		NULL,                     
		NULL );
	if( FAILED( hReturn ) ) 
	{
		if( hReturn == DPNERR_INVALIDPARAM ) 
			MessageBox( hwnd, "Failed to Host()", "Invalid Param", MB_ICONERROR );
		else if( hReturn == DPNERR_INVALIDDEVICEADDRESS  ) 
			MessageBox( hwnd, "Failed to Host()", "Invalid Device Address", MB_ICONERROR );
		else
			MessageBox( hwnd, "Failed to Host()", "DirectPlay Error", MB_ICONERROR );
		return -1;
	}
	else
	{
		MessageBox( hwnd, "host2()", "DirectPlay", MB_ICONERROR );
	}
	
	// Let us know we are the host
	bHost = 1;
	
	//vShowText(hLB_Output,"<- Hosting ->");
	MessageBox( hwnd, "------Hosting------", "Check Point", MB_OK );
	
	return hReturn;
}

int GetPlayerID(DPNID dpid)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (PlayerInfo[i].bActive)
		{
			if (PlayerInfo[i].dpnidPlayer == dpid)
			{
				return(i);
			}
		}
	}
	return(0);
}

void CreateBullet(DPNID dpowner, D3DXMATRIX R, float x, float y, float z, float xv, float yv, float zv)
{
	int i = 0;
	BULLET_PACKET pacBul;
	void *packet;
	
	for (i = 0; i < MAX_BULLETS; i++)
		if (!Bullets[i].bActive)
		{
			
			break;
		}
	
	if (i == MAX_BULLETS) return;
	//PlaySound(sound[1]);
	Bullets[i].bActive = true;
	Bullets[i].dpOwner = dpowner;
	Bullets[i].x = x;
	Bullets[i].y = y;
	Bullets[i].z = z;
	Bullets[i].xv = xv;
	Bullets[i].yv = yv;
	Bullets[i].zv = zv;
	Bullets[i].life = 500.0;
	Bullets[i].R = R;
		
	if (dpowner == g_dpnidLocalPlayer)
	{
		pacBul.size = sizeof(BULLET_PACKET);
		pacBul.type = PACKET_TYPE_BULLET;
		pacBul.bullet.bActive = 0;
		pacBul.bullet.dpOwner = g_dpnidLocalPlayer;
		pacBul.bullet.x = x;
		pacBul.bullet.y = y;
		pacBul.bullet.z = z;
		pacBul.bullet.xv = xv;
		pacBul.bullet.yv = yv;
		pacBul.bullet.zv = zv;
		pacBul.bullet.life = 500.0;
		pacBul.bullet.R = R;
		packet = (VOID*)&pacBul;
		SendPeerMessage(-1, packet);
	}
	
	/*if (i >= MAX_BULLETS)
		return;*/

}