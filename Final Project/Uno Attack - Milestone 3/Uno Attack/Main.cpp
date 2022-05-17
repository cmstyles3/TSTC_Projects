#include "Main.h"

Font *g_font = NULL;
LPDIRECT3DTEXTURE9 g_cardTexture = NULL;
LPDIRECT3DTEXTURE9 g_tableTexture = NULL;
LPDIRECT3DTEXTURE9 g_launcherTexture;

void Init()
{
	g_font = new Font("Assets/Textures/font.dds", 32);

	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/cards.dds", &g_cardTexture);
	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/table.bmp", &g_tableTexture);
	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/launcher.bmp", &g_launcherTexture);
	
	CalcCardRects();

//	g_engine->AddState(new Test(GS_TEST), false);

	g_engine->AddState(new Menu(GS_MENU), true);

	
	g_engine->AddState(new SinglePlayer(GS_SINGLEPLAYER), false);
	/*
	g_engine->AddState(new MultiPlayer(GS_MULTIPLAYER), false);*/
}

void Kill()
{
	SAFE_RELEASE(g_launcherTexture);
	SAFE_RELEASE(g_tableTexture);
	SAFE_RELEASE(g_cardTexture);
	SAFE_DELETE(g_font);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	EngineSetup setup;
	setup.width = 1024;
	setup.height = 768;
	setup.windowed = false;
	setup.instance = instance;
	setup.Init = Init;
	setup.Kill = Kill;

	new Engine(&setup);
	g_engine->Run();

	return(0);
}