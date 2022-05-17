// NETWORK.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NETWORK_H
#define NETWORK_H

#include <queue>

#define MSGID_CREATE_PLAYER 0x12001
#define MSGID_DESTROY_PLAYER 0x12002
#define MSGID_TERMINATE_SESSION 0x12003
#define MSGID_HOST_MIGRATE 0x12004

struct NetworkMessage
{
	unsigned long msgid;
	DPNID dpnid;
};

struct ReceivedMessage : public NetworkMessage
{
	char data[64];
};


struct SessionInfo
{
	LPDIRECTPLAY8ADDRESS address;
	DPN_APPLICATION_DESC description;
};

struct PlayerInfo
{
	DPNID dpnid;
	char *name;
	void *data;
	unsigned long size;

	PlayerInfo()
	{
		dpnid = 0;
		name = NULL;
		data = NULL;
		size = 0;
	}

	~PlayerInfo()
	{
		SAFE_DELETE(name);
		SAFE_DELETE(data);
	}
};



class Network
{
	public:

		Network(GUID guid, unsigned int port, void (*HandleNetworkMsgFunc)(ReceivedMessage *msg));

		virtual ~Network();

		void Update();

		void EnumerateSessions();
		void EnumerateSessions(char *ip);

		bool Host(char *name, char *session, int players = 0, void *playerData = NULL, unsigned long dataSize = 0);
		bool Join(char *name, int session = 0, void *playerData = NULL, unsigned long dataSize = 0);

		void Close();

		void SetReceiveAllowed(bool allowed);

		SessionInfo *GetSession(int index);
		PlayerInfo *GetPlayer(DPNID dpnid);

		DPNID GetLocalID();
		DPNID GetHostID();
		bool IsHost();

		void Send(void *data, long size, DPNID dpnid = DPNID_ALL_PLAYERS_GROUP, long flags = 0);

	private:

		static HRESULT WINAPI NetworkMessageHandler(void *context, unsigned long msgid, void *data);

		void (*HandleNetworkMessage)(ReceivedMessage *msg);

		void SetHostID();

		// Member variables

		GUID m_guid;
		PDIRECTPLAY8PEER m_dpp;
		PDIRECTPLAY8ADDRESS m_device;
		PDIRECTPLAY8ADDRESS m_hostAddr;

		unsigned long m_port;
		unsigned long m_sendTimeOut;
		unsigned long m_processingTime;

		DPNID m_dpnidLocal;
		DPNID m_dpnidHost;

		CRITICAL_SECTION m_sessionCS;
		std::vector<SessionInfo*> m_sessions;

		CRITICAL_SECTION m_playerCS;
		std::vector<PlayerInfo*> m_players;

		bool m_receiveAllowed;
		CRITICAL_SECTION m_messageCS;
		std::queue<ReceivedMessage*> m_messages;

};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////