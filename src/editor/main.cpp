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

#include "engine/camera.h"
#include "engine/scene_module.h"
#include "engine/light.h"
#include "engine/mesh_renderer.h"

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
	gEngine->LoadModule<editor::EditorModule>();
	ConfigLoader::instance();
	ConfigLoader::instance().Save();
	BindingModule::Get("luna.editor")->Init();
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
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}