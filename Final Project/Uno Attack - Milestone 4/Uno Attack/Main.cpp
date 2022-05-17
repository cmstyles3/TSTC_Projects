#include "Main.h"

Font *g_font = NULL;
LPDIRECT3DTEXTURE9 g_cardTexture = NULL;
LPDIRECT3DTEXTURE9 g_tableTexture = NULL;

float angle[7] = {-30.0f, -20.0f, -10.0f, 0.0f, 10.0f, 20.0f, 30.0f };

void Init()
{
	g_font = new Font("Assets/Textures/font.dds", 32);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/cards.dds", &g_cardTexture);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/table.bmp", &g_tableTexture);
	
	CalcCardRects();

//	g_engine->AddState(new Test(GS_TEST), false);

	g_engine->AddState(new Menu(GS_MENU), true);
	g_engine->AddState(new SinglePlayer(GS_SINGLEPLAYER), false);
	g_engine->AddState(new MultiPlayer(GS_MULTIPLAYER), false);

	g_engine->GetInput()->SetCursorPos(512, 384);
}

void Kill()
{
	SAFE_RELEASE(g_tableTexture);
	SAFE_RELEASE(g_cardTexture);
	SAFE_DELETE(g_font);
}

void NetworkMessageHandler(ReceivedMessage *msg)
{
	if (g_engine->GetCurrentState() != NULL)
	{
		if (g_engine->GetCurrentState()->GetID() != GS_MULTIPLAYER)
			return;
	}
	else return;

	((MultiPlayer*)(g_engine->GetCurrentState()))->NetworkMessageHandler(msg);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	// {ACDB2083-CB7B-4578-B746-1EA2B2C02F83}
	static const GUID guid = 
	{ 
		0xacdb2083, 
		0xcb7b, 
		0x4578, 
		{ 0xb7, 0x46, 0x1e, 0xa2, 0xb2, 0xc0, 0x2f, 0x83 } 
	};


	EngineSetup setup;
	setup.width = 1024;
	setup.height = 768;
	setup.windowed = false;
	setup.instance = instance;
	setup.Init = Init;
	setup.Kill = Kill;
	setup.guid = guid;
	setup.port = 8565;
	setup.name = "Uno Attack";
	setup.NetworkMessageHandler = NetworkMessageHandler;

	new Engine(&setup);
	g_engine->Run();

	return(0);
}