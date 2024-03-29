#include "Core/CoreMin.h"

#include "Core/Asset/AssetModule.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Event/EventModule.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Object/System.h"

#include "windows.h"

using namespace luna;


void LoadCoreLib()
{
	std::fstream logFile;
	std::fstream configFile;

#ifdef WIN32
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString workDir(tempPath);
#endif // WIN32

	LString logPath = workDir + "/log.txt";
	LString configPath = workDir + "/config.ini";
	
	configFile.open(configPath.c_str(), std::fstream::in | std::fstream::out);
	
	Logger::instance().RedirectLogFile(logPath.std_str());

	LString configContent;
	std::ostringstream ss;
	ss << configFile.rdbuf(); // reading data
	configContent = ss.str();	
	
	ConfigLoader::instance().LoadFromJson(configContent);

	LunaCore::Ins()->LoadModule<AssetModule>();
	LunaCore::Ins()->LoadModule<EventModule>();
	LunaCore::Ins()->LoadModule<ECSModule>();
}

void UnLoadCoreLib()
{
	std::fstream fs;
	// Perform any necessary cleanup.
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString path(tempPath);
	path = path + "/config.ini";
	fs.open(path.c_str(), std::fstream::out | std::fstream::trunc);
	ConfigLoader::instance().SaveJson(fs);
	fs.close();
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
		LoadCoreLib();
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:		
		break;
	case DLL_PROCESS_DETACH:
		UnLoadCoreLib();
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
