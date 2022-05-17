#ifndef MAIN_H
#define MAIN_H

#include "../Engine/Engine.h"

#include "Test.h"
#include "Menu.h"
#include "SinglePlayer.h"
#include "MultiPlayer.h"

enum 
{
	GS_TEST,
	GS_MENU,
	GS_SINGLEPLAYER,
	GS_MULTIPLAYER,
};

extern Font *g_font;

#endif