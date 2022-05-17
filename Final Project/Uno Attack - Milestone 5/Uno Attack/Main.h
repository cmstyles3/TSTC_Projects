#ifndef MAIN_H
#define MAIN_H

#include "../Engine/Engine.h"

//#include "Test.h"
#include "Menu.h"
#include "SinglePlayer.h"
#include "MultiPlayer.h"
#include "Cards.h"
#include "CardParticle.h"

enum 
{
	GS_TEST,
	GS_MENU,
	GS_SINGLEPLAYER,
	GS_MULTIPLAYER,
};

extern Font *g_font;
extern LPDIRECT3DTEXTURE9 g_cardTexture;
extern LPDIRECT3DTEXTURE9 g_tableTexture;
extern Sound *g_dingSound;
extern Sound *g_buttonSound;

extern float angle[7];
extern char *colorStr[];

#endif