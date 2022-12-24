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
#include "core/event/event_module.h"
#include "core/framework/application.h"

#include "windows.h"

using namespace luna;


CORE_API void LoadLib(const luna::LString& val)
{
	HINSTANCE hDll = LoadLibraryA(val.c_str());
	assert(hDll != nullptr);
	return;
}

void LoadCoreLibrary()
{
	std::fstream logFile;
	std::fstream configFile;

#ifdef WIN32
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录

	LString workDir(tempPath);
	LString logPath = workDir + "/log.txt";
	LString configPath = workDir + "/config.ini";
	
#endif // WIN32
	configFile.open(configPath.c_str(), std::fstream::in | std::fstream::out);
	
	Logger::instance().RedirectLogFile(logPath.std_str());

	LString configContent;
	std::ostringstream ss;
	ss << configFile.rdbuf(); // reading data
	configContent = ss.str();	
	
	ConfigLoader::instance().Load(configContent);

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
		std::fstream fs;
		// Perform any necessary cleanup.
		TCHAR tempPath[1000];
		GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
		LString path(tempPath);
		path = path + "/config.ini";
		fs.open(path.c_str(), std::fstream::out | std::fstream::trunc);
		ConfigLoader::instance().Save(fs);
		fs.close();
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
