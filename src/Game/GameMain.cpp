#include "Core/Asset/AssetModule.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Event/EventModule.h"
#include "Core/Framework/LunaCore.h"

#include "windows.h"
#include "Animation/AnimationModule.h"
#include "Graphics/RenderModule.h"
#include "Game/GameModule.h"

using namespace luna;


void LoadGameLib()
{
	gEngine->LoadModule<GameModule>();
	gEngine->LoadModule<animation::AnimationModule>();
	gEngine->LoadModule<graphics::RenderModule>();
}

void UnLoadGameLib()
{

}

__declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		LoadGameLib();
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnLoadGameLib();
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
