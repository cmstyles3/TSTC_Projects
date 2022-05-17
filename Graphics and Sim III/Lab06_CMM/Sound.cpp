#include "Sound.h"
#include "engine.h"

SoundSystem::SoundSystem()
{
	CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void**)&m_loader);

	CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&m_performance);

	m_performance->InitAudio(NULL, NULL, NULL, DMUS_APATH_DYNAMIC_STEREO, 32, DMUS_AUDIOF_ALL, NULL);

}

SoundSystem::~SoundSystem()
{
	m_performance->CloseDown();
	SAFE_RELEASE(m_performance);
	SAFE_RELEASE(m_loader);
}

void SoundSystem::SetVolume(long volume)
{
	m_performance->SetGlobalParam(GUID_PerfMasterVolume, &volume, sizeof(long));
}

IDirectMusicLoader8 *SoundSystem::GetLoader()
{
	return m_loader;
}

IDirectMusicPerformance8 *SoundSystem::GetPerformance()
{
	return m_performance;
}





Sound::Sound(char *filename)
{
	WCHAR *wideFilename = new WCHAR[strlen(filename)+1];
	MultiByteToWideChar(CP_ACP, 0, filename, -1, wideFilename, strlen(filename)+1);
	wideFilename[strlen(filename)] = 0;

	g_engine->GetSoundSystem()->GetLoader()->LoadObjectFromFile(
		CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wideFilename, (void**)&m_segment);
	m_segment->Download(g_engine->GetSoundSystem()->GetPerformance());

	SAFE_DELETE(wideFilename);
}

Sound::~Sound()
{
	m_segment->Unload(g_engine->GetSoundSystem()->GetPerformance());
	g_engine->GetSoundSystem()->GetLoader()->ReleaseObjectByUnknown(m_segment);
	SAFE_RELEASE(m_segment);
}

void Sound::Play(bool loop, DWORD flags)
{
	if (loop)
		m_segment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
	else
		m_segment->SetRepeats(0);

	g_engine->GetSoundSystem()->GetPerformance()->PlaySegment(
		m_segment, DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY, 0, NULL);
}

IDirectMusicSegment8 *Sound::GetSegment()
{
	return m_segment;
}