#include "sound.h"

LPDIRECTSOUND8 g_pDSound = 0;
LPDIRECTSOUNDBUFFER g_pPrimary = 0;
LPDIRECTSOUNDBUFFER8 g_pSecondary[4]= {0};
CWaveSoundRead *g_pWaveSoundRead = 0;
//DSBUFFERDESC g_dsbd;

int InitDirectSound()
{	
	if (FAILED(DirectSoundCreate8(NULL, &g_pDSound, NULL)))
	{

		MessageBox(0, "sound create",0,0);
		return(0);
	}

		
	if (FAILED(g_pDSound->SetCooperativeLevel(g_hwnd, DSSCL_PRIORITY)))
	{
		MessageBox(0, "sound coop",0,0);
		return(0);
	}

	LPDIRECTSOUNDBUFFER primary = 0;
	

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;
	
	if (FAILED(g_pDSound->CreateSoundBuffer(&dsbd, &primary, NULL)))
	{
		MessageBox(0, "sound buffer",0,0);
		return(0);
	}
	
	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag      = WAVE_FORMAT_PCM;
	wfx.nChannels       = 2;
	wfx.nSamplesPerSec  = 22050;
	wfx.wBitsPerSample  = 16;
	wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if(FAILED(primary->SetFormat(&wfx)))
		return(0);

	if (primary)
	{
		primary->Release();
		primary = 0;
	}

	
		// return sucess
	return(1);
}

void KillDirectSound()
{

	for (int i = 0; i < 4; i++)
	{
		if (g_pSecondary[i])
		{
			g_pSecondary[i]->Stop();
			g_pSecondary[i]->Release();
			g_pSecondary[i] = 0;
		}
	}
	if (g_pDSound)
	{
		g_pDSound->Release();
		g_pDSound = 0;
	}
}

int LoadWAV(char *filename, int index)
{
	if (g_pSecondary[index])
	{
		g_pSecondary[index]->Stop();
		g_pSecondary[index]->Release();
		g_pSecondary[index] = 0;
	}

	g_pWaveSoundRead = new CWaveSoundRead;

	if (FAILED(g_pWaveSoundRead->Open(filename)))
		return(0);

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_STATIC;
	dsbd.dwBufferBytes = g_pWaveSoundRead->m_ckIn.cksize;
	dsbd.lpwfxFormat = g_pWaveSoundRead->m_pwfx;


	LPDIRECTSOUNDBUFFER tempbuffer;
	if (FAILED(g_pDSound->CreateSoundBuffer(&dsbd, &tempbuffer, NULL)))
		return(0);
	if (FAILED(tempbuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&g_pSecondary[index])))
		return(0);
	tempbuffer->Release();


	//FILL BUFFER
	UCHAR* pbWavData;
	UINT cbWavSize;
	void* pbData = 0;
	void* pbData2 = 0;
	DWORD dwLength;
	DWORD dwLength2;

	UINT nWaveFileSize = g_pWaveSoundRead->m_ckIn.cksize;

	pbWavData = new UCHAR[nWaveFileSize];
	if (pbWavData == NULL)
	{
		delete [] pbWavData;
		return(0);
	}
	
	if (FAILED(g_pWaveSoundRead->Read(nWaveFileSize, pbWavData, &cbWavSize)))
		return(0);

	g_pWaveSoundRead->Reset();

	
	DWORD buffersize = dsbd.dwBufferBytes;
	if (FAILED(g_pSecondary[index]->Lock(0, buffersize, &pbData, &dwLength, &pbData2, &dwLength2, 0L)))
		return(0);

	memcpy(pbData, pbWavData, buffersize);
	g_pSecondary[index]->Unlock(pbData, buffersize, NULL, 0);
	
	delete [] pbWavData;

	return(1);
}

void Play(int index, bool loop)
{
	if (loop)
	{
		g_pSecondary[index]->Play(0, 0, DSBPLAY_LOOPING);
	}
	else
	{
		g_pSecondary[index]->SetCurrentPosition(0);
		g_pSecondary[index]->Play(0, 0, 0);
	}
}

void Stop(int index)
{
	g_pSecondary[index]->Stop();
}