#include "Main.h"

Font *g_font = NULL;

void Init()
{
	g_font = new Font("Assets/Textures/font.dds", 32);

	g_engine->AddState(new Test(GS_TEST), true);

	/*g_engine->AddState(new Menu(GS_MENU), false);
	g_engine->AddState(new SinglePlayer(GS_SINGLEPLAYER), false);
	g_engine->AddState(new MultiPlayer(GS_MULTIPLAYER), false);*/
}

void Kill()
{
	SAFE_DELETE(g_font);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	EngineSetup setup;
	setup.width = 1024;
	setup.height = 768;
	setup.windowed = true;
	setup.instance = instance;
	setup.Init = Init;
	setup.Kill = Kill;

	new Engine(&setup);
	g_engine->Run();

	return(0);
}