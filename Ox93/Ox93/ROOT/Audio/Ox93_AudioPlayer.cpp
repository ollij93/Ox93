
// Includes...
#include "Ox93_AudioPlayer.h"
#include <cstdio>

// For IID_IGraphBuilder, IID_IMediaControl, IID_IMediaEvent
#pragma comment(lib, "strmiids.lib") 

// Static Variables...
Ox93_AudioHandle* Ox93_AudioPlayer::s_pxHandle = nullptr;

void Ox93_AudioPlayer::PlayMP3File(const wchar_t* pszFilename)
{
	Ox93_Assert(!s_pxHandle, "Already playing a sound. Try stopping the previous one first.");
	if (s_pxHandle) { return; }

	s_pxHandle = new Ox93_AudioHandle;

	// Initialize the COM library.
	HRESULT hResult = CoInitialize(nullptr);
	Ox93_Assert(!FAILED(hResult), "Could not initialize the COM library for Ox93_AudioPlayer.");
	if (FAILED(hResult)) { return; }

	// Create the filter graph manager and query for interfaces.
	hResult = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)(&(s_pxHandle->m_pxGraph)));
	Ox93_Assert(!FAILED(hResult), "Failed to create the filter graph manager for Ox93_AudioPlayer.");
	if (FAILED(hResult)) { return; }

	if (s_pxHandle->m_pxGraph)
	{
		hResult = s_pxHandle->m_pxGraph->QueryInterface(IID_IMediaControl, (void **)&s_pxHandle->m_pxControl);
		hResult = s_pxHandle->m_pxGraph->QueryInterface(IID_IMediaEvent  , (void **)&s_pxHandle->m_pxEvent  );

		// Build the graph.
		hResult = s_pxHandle->m_pxGraph->RenderFile(pszFilename, nullptr);
		Ox93_Assert(SUCCEEDED(hResult), "Failed to build the audio graph for Ox93_AudioPlayer.");
		if (SUCCEEDED(hResult))
		{
			hResult = s_pxHandle->m_pxControl->Run(); // Run the graph.
		}
	}
}

void Ox93_AudioPlayer::StopSound()
{
	Ox93_Assert(s_pxHandle, "Cannot stop a sound since the currnet handle is null.");
	if (!s_pxHandle) { return; }

	if (s_pxHandle->m_pxEvent)
	{
		s_pxHandle->m_pxEvent->Release();
	}
	if (s_pxHandle->m_pxControl)
	{
		s_pxHandle->m_pxControl->Release();
	}
	if (s_pxHandle->m_pxGraph)
	{
		s_pxHandle->m_pxGraph->Release();
	}

	CoUninitialize();
}