#include "core/asset/asset_module.h"
#include "core/platform/platform_module.h"
#include "core/event/event_module.h"
#include "core/framework/application.h"

#include "windows.h"
#include "render/render_module.h"
#include "game/game_module.h"

using namespace luna;


void LoadGameLib()
{	
	gEngine->LoadModule<render::RenderModule>();
	gEngine->LoadModule<GameModule>();
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
