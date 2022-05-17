#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "../Engine/Engine.h"

#define MSGID_START_COUNTDOWN 0x12005
#define MSGID_TRACK_CHANGE 0x12006
#define MSGID_MOVE_UPDATE 0x12007
#define MSGID_REQUEST_TRACK 0x12008
#define MSGID_UPDATE_TRACK 0x12009
#define MSGID_FINAL_TIME 0x12010
#define MSGID_REQUEST_STATUS 0x12011
#define MSGID_STATUS 0x12012

struct TrackChangeMsg : public NetworkMessage
{
	UINT track;
};

struct MoveUpdateMsg : public NetworkMessage
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;
};

struct UpdateTrackMsg : public NetworkMessage
{
	UINT track;
};

struct FinalTimeMsg : public NetworkMessage
{
	float time;
};

struct StatusMsg : public NetworkMessage
{
	bool locked;
};


struct Player
{
	DPNID dpnid;
	char name[16];
	UINT ship;

	bool finished;
	float time;

	Object3D object;
};

class Multiplayer : public State
{
	enum {START, INITGAME, COUNTDOWN, PLAYGAME, RESULTS};
	public:
		Multiplayer();
		void Load();
		void Close();

		void OnLostDevice();
		void OnResetDevice();

		void Update(float dt);
		void Draw();

		void NetworkMessageHandler(ReceivedMessage *msg);

	private:
		Camera m_camera;
		std::vector<Player> m_players;
		int m_currState;
		float m_shipSpeed;
		int m_throttle;
		int m_currRing;
		float m_raceTimer;
		float m_countDown;
		bool loaded;
		bool m_locked;
		
};

#endif