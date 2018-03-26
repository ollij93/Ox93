#ifndef __OX93_AUDIOPLAYER_H__
#define __OX93_AUDIOPLAYER_H__

#include <dshow.h>

struct Ox93_AudioHandle
{
	IGraphBuilder* m_pxGraph = nullptr;
	IMediaControl* m_pxControl = nullptr;
	IMediaEvent* m_pxEvent = nullptr;
};

class Ox93_AudioPlayer
{
public:
	static void PlayMP3File(const wchar_t* pszFilename);
	static void StopSound();

	static Ox93_AudioHandle* s_pxHandle;
};

#endif // ifndef __OX93_AUDIOPLAYER_H__