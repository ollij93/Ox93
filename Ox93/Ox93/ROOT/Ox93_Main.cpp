#include "Ox93_Core.h"

// Includes...
#include "ROOT/Audio/Ox93_AudioPlayer.h"
#include "ROOT/Ox93_System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// Create, Initialize and Run the system
	bool bResult = Ox93_System::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_System.");

	if (bResult)
	{
		//Ox93_AudioPlayer::PlayMP3File(L"test.mp3");
		Ox93_System::Run();
		//Ox93_AudioPlayer::StopSound();
	}

	// Shutdown and release the system
	Ox93_System::Destroy();

	return 0;
}

