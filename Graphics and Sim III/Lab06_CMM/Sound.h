#ifndef SOUND_H
#define SOUND_H

#include <dmusici.h>
#include <dmusicf.h>

class SoundSystem
{
	public:
		SoundSystem();
		virtual ~SoundSystem();

		void SetVolume(long volume);

		IDirectMusicLoader8 *GetLoader();
		IDirectMusicPerformance8 *GetPerformance();

	private:
		IDirectMusicLoader8 *m_loader;
		IDirectMusicPerformance8 *m_performance;
};


class Sound
{
	public:
		Sound(char *filename);
		virtual ~Sound();

		void Play(bool loop = false, DWORD flags = DMUS_SEGF_AUTOTRANSITION);

		IDirectMusicSegment8 *GetSegment();

	private:
		IDirectMusicSegment8 *m_segment;
};

#endif