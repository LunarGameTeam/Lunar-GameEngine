/*!
 * \file core.h
 * \date 2020/08/09 10:11
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief 提供给其他模块包含的头文件，会包含本模块的基本内容和接口
 *
 * TODO: long description
 *
 * \note
*/
#include "core_library.h"

#include "core/asset/asset_module.h"
#include "core/file/platform_module.h"
#include "core/event/EventModule.h"
#include "core/framework/application.h"

using namespace luna;


CORE_API void LoadLib(const luna::LString& val)
{
	HINSTANCE hDll = LoadLibraryA(val.c_str());
	assert(hDll != nullptr);
	return;
}

void LoadCoreLibrary()
{
	luna::LApplication::Instance();
	gEngine->LoadModule<AssetModule>();
	gEngine->LoadModule<EventModule>();	
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
		LoadCoreLibrary();
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
