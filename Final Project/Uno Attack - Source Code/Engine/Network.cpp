// NETWORK.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine.h"

Network::Network(GUID guid, unsigned int port, void (*HandleNetworkMsgFunc)(ReceivedMessage *))
{
	InitializeCriticalSection(&m_sessionCS);
	InitializeCriticalSection(&m_playerCS);
	InitializeCriticalSection(&m_messageCS);

	m_dpp = NULL;
	m_device = NULL;
	m_hostAddr = NULL;

	memcpy(&m_guid, &guid, sizeof(GUID));

	m_port = port;
	m_sendTimeOut = 500;
	m_processingTime = 100;

	m_receiveAllowed = false;

	HandleNetworkMessage = HandleNetworkMsgFunc;

	CoCreateInstance(CLSID_DirectPlay8Peer, NULL, CLSCTX_INPROC, IID_IDirectPlay8Peer, (LPVOID*)&m_dpp);
	m_dpp->Initialize((PVOID)this, NetworkMessageHandler, 0);

	CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC, IID_IDirectPlay8Address, (LPVOID*)&m_device);

	m_device->SetSP(&CLSID_DP8SP_TCPIP);
	m_device->AddComponent(DPNA_KEY_PORT, &m_port, sizeof(DWORD), DPNA_DATATYPE_DWORD);

	CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC, IID_IDirectPlay8Address, (LPVOID*)&m_hostAddr);
	m_hostAddr->SetSP(&CLSID_DP8SP_TCPIP);
	m_hostAddr->AddComponent(DPNA_KEY_PORT, &m_port, sizeof(DWORD), DPNA_DATATYPE_DWORD);

	//m_hostAddr->AddComponent(DPNA_KEY_PORT, &m_port, sizeof(DWORD), DPNA_DATATYPE_DWORD);

}

Network::~Network()
{
	SAFE_RELEASE(m_hostAddr);
	SAFE_RELEASE(m_device);

	if (m_dpp)
		m_dpp->Close(DPNCLOSE_IMMEDIATE);
	SAFE_RELEASE(m_dpp);

	DeleteCriticalSection(&m_sessionCS);
	DeleteCriticalSection(&m_playerCS);
	DeleteCriticalSection(&m_messageCS);
}

void Network::Update()
{
	if (m_messages.empty())
		return;

	EnterCriticalSection(&m_messageCS);

	ReceivedMessage *message = m_messages.front();

	unsigned long endTime = timeGetTime() + m_processingTime;
	while (endTime > timeGetTime() && !m_messages.empty())
	{
		HandleNetworkMessage(m_messages.front());
		m_messages.pop();
	}
	LeaveCriticalSection(&m_messageCS);
}

void Network::EnumerateSessions()
{
	m_players.clear();
	m_sessions.clear();
	while(!m_messages.empty())
		m_messages.pop();

	DPN_APPLICATION_DESC description;
	ZeroMemory(&description, sizeof(DPN_APPLICATION_DESC));
	description.dwSize = sizeof(DPN_APPLICATION_DESC);
	description.guidApplication = m_guid;

	m_dpp->EnumHosts(&description, NULL, m_device, NULL, 0, 1, 0, 0, NULL, NULL, DPNENUMHOSTS_SYNC);
}

void Network::EnumerateSessions(char *ip)
{
	m_players.clear();
	m_sessions.clear();
	while(!m_messages.empty())
		m_messages.pop();
	
	DPN_APPLICATION_DESC description;
	ZeroMemory(&description, sizeof(DPN_APPLICATION_DESC));
	description.dwSize = sizeof(DPN_APPLICATION_DESC);
	description.guidApplication = m_guid;

	WCHAR wide[MAX_PATH];
	mbstowcs(wide, ip, MAX_PATH);

	m_hostAddr->AddComponent(DPNA_KEY_HOSTNAME, wide, (wcslen(wide)+1)*sizeof(WCHAR), DPNA_DATATYPE_STRING);

	m_dpp->EnumHosts(&description, m_hostAddr, m_device, NULL, 0, 1, 0, 0, NULL, NULL, DPNENUMHOSTS_SYNC);

}

bool Network::Host(char *name, char *session, int players, void *playerData, unsigned long dataSize)
{
	WCHAR wide[MAX_PATH];

	DPN_PLAYER_INFO player;
	ZeroMemory(&player, sizeof(DPN_PLAYER_INFO));
	player.dwSize = sizeof(DPN_PLAYER_INFO);
	player.pvData = playerData;
	player.dwDataSize = dataSize;
	player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
	mbstowcs(wide, name, MAX_PATH);
	player.pwszName = wide;

	if (FAILED(m_dpp->SetPeerInfo(&player, NULL, NULL, DPNSETPEERINFO_SYNC)))
		return false;

	DPN_APPLICATION_DESC description;
	ZeroMemory(&description, sizeof(DPN_APPLICATION_DESC));
	description.dwSize = sizeof(DPN_APPLICATION_DESC);
	description.guidApplication = m_guid;
	description.dwMaxPlayers = players;
	mbstowcs(wide, session, MAX_PATH);
	description.pwszSessionName = wide;
	//description.dwFlags = DPNSESSION_MIGRATE_HOST;

	if (FAILED(m_dpp->Host(&description, &m_device, 1, NULL, NULL, NULL, 0)))
		return false;

	return true;
}

bool Network::Join(char *name, int session, void *playerData, unsigned long dataSize)
{
	WCHAR wide[MAX_PATH];

	m_players.clear();
	while (!m_messages.empty())
		m_messages.pop();

	if (m_sessions.empty() || session < 0 || session >= m_sessions.size())
		return false;

	DPN_PLAYER_INFO player;
	ZeroMemory(&player, sizeof(DPN_PLAYER_INFO));
	player.dwSize = sizeof(DPN_PLAYER_INFO);
	player.pvData = playerData;
	player.dwDataSize = dataSize;
	player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
	mbstowcs(wide, name, MAX_PATH);
	player.pwszName = wide;

	if (FAILED(m_dpp->SetPeerInfo(&player, NULL, NULL, DPNSETPEERINFO_SYNC)))
		return false;

	EnterCriticalSection(&m_sessionCS);

	if (FAILED(m_dpp->Connect(
		&m_sessions[session]->description, m_sessions[session]->address, m_device, 
		NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC)))
	{
		LeaveCriticalSection(&m_sessionCS);
		return false;
	}

	LeaveCriticalSection(&m_sessionCS);
	return true;

}

void Network::Close()
{
	if (m_dpp != NULL)
		m_dpp->Close(DPNCLOSE_IMMEDIATE);
	m_dpp->Initialize((PVOID)this, NetworkMessageHandler, 0);
}

void Network::SetReceiveAllowed(bool allowed)
{
	m_receiveAllowed = allowed;
}

SessionInfo *Network::GetSession(int index)
{
	if (index >= m_sessions.size())
		return 0;

	return m_sessions[index];
}

PlayerInfo *Network::GetPlayer(DPNID dpnid)
{
	for (int i = 0; i < m_players.size(); i++)
	{
		if (m_players[i]->dpnid == dpnid)
			return m_players[i];
	}

	return 0;
}

DPNID Network::GetLocalID()
{
	return m_dpnidLocal;
}

DPNID Network::GetHostID()
{
	return m_dpnidHost;
}

bool Network::IsHost()
{
	if (m_dpnidLocal == m_dpnidHost)
		return true;

	return false;
}

void Network::Send(void *data, long size, DPNID dpnid, long flags)
{
	DPNHANDLE hAsync;
	DPN_BUFFER_DESC dpbd;

	if ((dpbd.dwBufferSize = size) == 0)
		return;
	dpbd.pBufferData = (BYTE*)data;

	// DPNSEND_NOCOMPLETE
	m_dpp->SendTo(dpnid, &dpbd, 1, m_sendTimeOut, NULL, &hAsync,  flags | DPNSEND_GUARANTEED | DPNSEND_COALESCE);
}


HRESULT WINAPI Network::NetworkMessageHandler(PVOID context, DWORD msg, PVOID data)
{
	// Get a pointer to the calling network object.
	Network *network = (Network*)context;

	switch(msg)
	{
		case DPN_MSGID_CREATE_PLAYER:
		{
			unsigned long size = 0;
			DPN_PLAYER_INFO *info = NULL;
			HRESULT hr = DPNERR_CONNECTING;
			PDPNMSG_CREATE_PLAYER msgCreatePlayer = (PDPNMSG_CREATE_PLAYER)data;

			// Create a player information structure for the new player.
			PlayerInfo *playerInfo = new PlayerInfo;
			ZeroMemory(playerInfo, sizeof(PlayerInfo));
			playerInfo->dpnid = msgCreatePlayer->dpnidPlayer;

			// Keep calling GetPeerInfo(), as it may still be trying to connect.
			while (hr == DPNERR_CONNECTING)
				hr = network->m_dpp->GetPeerInfo(playerInfo->dpnid, info, &size, 0);

			// Check if GetPeerInfo() has returned the size of the DPN_PLAYER_INFO structure.
			if ( hr == DPNERR_BUFFERTOOSMALL )
			{
				info = (DPN_PLAYER_INFO*) new BYTE[size];
				ZeroMemory(info, size);
				info->dwSize = sizeof(DPN_PLAYER_INFO);

				// Try again using the correct size.
				if (SUCCEEDED(network->m_dpp->GetPeerInfo(playerInfo->dpnid, info, &size, 0)))
				{
					// Store the name of the new player.
					playerInfo->name = new char[wcslen(info->pwszName) + 1];
					ZeroMemory(playerInfo->name, wcslen(info->pwszName) + 1);
					wcstombs(playerInfo->name, info->pwszName, wcslen(info->pwszName));

					// Store the player data.
					playerInfo->data = new BYTE[info->dwDataSize];
					memcpy(playerInfo->data, info->pvData, info->dwDataSize);
					playerInfo->size = info->dwDataSize;

					// Store any local and host player details.
					if (info->dwPlayerFlags & DPNPLAYER_LOCAL)
						network->m_dpnidLocal = playerInfo->dpnid;
					if (info->dwPlayerFlags & DPNPLAYER_HOST)
						network->m_dpnidHost = playerInfo->dpnid;
				}

				SAFE_DELETE_ARRAY(info);
			}

			// Add the new player to the player list.
			EnterCriticalSection(&network->m_playerCS);
			network->m_players.push_back(playerInfo);
			LeaveCriticalSection(&network->m_playerCS);

			// If there is no network message handler, then break now.
			if (network->HandleNetworkMessage == NULL)
				break;

			// Create a create player message.
			ReceivedMessage *message = new ReceivedMessage;
			message->msgid = MSGID_CREATE_PLAYER;
			message->dpnid = playerInfo->dpnid;

			// Store the message so that it can be processed by the application later.
			EnterCriticalSection(&network->m_messageCS);
			network->m_messages.push(message);
			LeaveCriticalSection(&network->m_messageCS);
			break;
		}

		case DPN_MSGID_DESTROY_PLAYER:
		{
			// Find the player to destroy and remove it from the player list.
			EnterCriticalSection(&network->m_playerCS);
			for (int i = 0; i < network->m_players.size(); i++)
			{
				if (network->m_players[i]->dpnid == ((PDPNMSG_DESTROY_PLAYER)data)->dpnidPlayer)
				{
					network->m_players[i] = network->m_players[network->m_players.size() - 1];
					network->m_players.pop_back();
					break;
				}
			}
			LeaveCriticalSection(&network->m_playerCS);

			// If there is no network message handler, then break now.
			if (network->HandleNetworkMessage == NULL )
				break;

			// Create a destroy player message.
			ReceivedMessage *message = new ReceivedMessage;
			message->msgid = MSGID_DESTROY_PLAYER;
			message->dpnid = ((PDPNMSG_DESTROY_PLAYER)data)->dpnidPlayer;

			// Store the message so that it can be processed by the application later.
			EnterCriticalSection(&network->m_messageCS);
			network->m_messages.push(message);
			LeaveCriticalSection(&network->m_messageCS);

			break;
		}

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			PDPNMSG_ENUM_HOSTS_RESPONSE response = (PDPNMSG_ENUM_HOSTS_RESPONSE)data;

			// Create a session information structure for the new session.
			SessionInfo *sessionInfo = new SessionInfo;
			response->pAddressSender->Duplicate(&sessionInfo->address);
			memcpy(&sessionInfo->description, response->pApplicationDescription, sizeof(DPN_APPLICATION_DESC));

			// Add the new session to the session list.
			EnterCriticalSection( &network->m_sessionCS );
			network->m_sessions.push_back(sessionInfo);
			LeaveCriticalSection( &network->m_sessionCS );

			break;
		}

		case DPN_MSGID_RECEIVE:
		{
			
			// If there is no network message handler, then break now.
			if (network->HandleNetworkMessage == NULL )
				break;

			// Check if the network is allowed to receive application specific messages.
			if (network->m_receiveAllowed == false )
				break;

			// Create a receive message.
			ReceivedMessage *message = new ReceivedMessage;
			memcpy( message, ((PDPNMSG_RECEIVE)data)->pReceiveData, ((PDPNMSG_RECEIVE)data)->dwReceiveDataSize);

			// Store the message so that it can be processed by the application later.
			EnterCriticalSection(&network->m_messageCS);
			network->m_messages.push(message);
			LeaveCriticalSection(&network->m_messageCS);

			break;
		}

		case DPN_MSGID_HOST_MIGRATE:
		{
			// Migrate host, update the network of the new host id.
			PDPNMSG_HOST_MIGRATE migrate = (PDPNMSG_HOST_MIGRATE)data;
			network->m_dpnidHost = migrate->dpnidNewHost;

			if (network->HandleNetworkMessage == NULL)
				break;

			ReceivedMessage *message = new ReceivedMessage;
			message->msgid = MSGID_HOST_MIGRATE;
			message->dpnid = migrate->dpnidNewHost;

			EnterCriticalSection(&network->m_messageCS);
			network->m_messages.push(message);
			LeaveCriticalSection(&network->m_messageCS);
			
			break;
		}

		case DPN_MSGID_TERMINATE_SESSION:
		{
			// If there is no network message handler, then break now.
			if (network->HandleNetworkMessage == NULL)
				break;

			// Create a terminate session message.
			ReceivedMessage *message = new ReceivedMessage;
			message->msgid = MSGID_TERMINATE_SESSION;

			// Store the message so that it can be processed by the application later.
			EnterCriticalSection(&network->m_messageCS);
			network->m_messages.push(message);
			LeaveCriticalSection(&network->m_messageCS);

			break;
		}

		
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////