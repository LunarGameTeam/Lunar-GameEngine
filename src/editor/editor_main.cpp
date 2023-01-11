#include "core/core_library.h"
#include "core/object/entity.h"
#include "core/foundation/config.h"
#include "core/object/transform.h"
#include "core/asset/asset_module.h"
#include "core/framework/application.h"
#include "core/foundation/profile.h"
#include "core/foundation/signal.h"
#include "core/event/event_module.h"

#include "window/window_module.h"
#include "window/window.h"

#include "game/camera.h"
#include "game/scene_module.h"
#include "game/light.h"
#include "game/mesh_renderer.h"

#include <numbers>

#include "render/render_module.h"
#include "render/asset/mesh_asset.h"
#include "render/asset/shader_asset.h"

#include "render/asset/material_template.h"
#include "editor/editor_module.h"

using namespace luna;

void LoadEditorLibrary()
{
	gEngine->LoadModule<WindowModule>();
	gEngine->LoadModule<SceneModule>();
	gEngine->LoadModule<render::RenderModule>();
	ConfigLoader::instance();
	BindingModule::Get("luna.editor")->Init();
}

void UnLoadEditorLibrary()
{

}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{	
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		LoadEditorLibrary();
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		UnLoadEditorLibrary();
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}