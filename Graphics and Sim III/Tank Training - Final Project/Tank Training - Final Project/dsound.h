#ifndef DSOUND_H
#define DSOUND_H
#include <dsound.h>
#include "wavread.h"


int LoadWAV(char *filename, int index);
void Play(int i, bool loop = false);
void Stop(int i);
int  InitDirectSound();
void KillDirectSound();


extern HWND g_hwnd;
extern HINSTANCE g_hinstance;
extern LPDIRECTSOUND8 g_pDSound;
extern LPDIRECTSOUNDBUFFER g_pPrimary;
extern LPDIRECTSOUNDBUFFER8 g_pSecondary[4];


#endif