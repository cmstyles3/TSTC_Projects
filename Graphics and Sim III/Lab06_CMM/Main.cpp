#include "main.h"
#include "BaseballTossGame.h"
#include "DartThrowGame.h"
#include "DuckShootGame.h"

Sound *g_music = 0;

void Init()
{
	g_engine->AddState(new MenuState);
	g_engine->AddState(new DuckShootGameState, false);
	g_engine->AddState(new BaseballTossGameState, false);
	g_engine->AddState(new DartThrowGameState, false);

	g_music = new Sound("song.mid");
	g_music->Play(true);
	
}

void Kill()
{
	SAFE_DELETE(g_music);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	EngineSetup setup;
	setup.instance = instance;
	setup.windowed = false;
	setup.name = "LAB06";
	setup.Init = Init;
	setup.Kill = Kill;

	new Engine(&setup);
	g_engine->Run();

	return(1);
}
