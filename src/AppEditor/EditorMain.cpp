#include "Core/Scripting/BindingTraits.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Foundation/Config.h"
#include "windows.h"

#undef main

using namespace luna;

CONFIG_IMPLEMENT(LString, Start, PythonHome, PYTHON3_PATH);

int main(int argc, char** argv)
{
	PyStatus status;

	PyConfig config;
	PyConfig_InitPythonConfig(&config);

	auto pythonHome = luna::StringToWstring(Config_PythonHome.GetValue().std_str());
	config.isolated = 1;
	
	TCHAR tempPath[1000];

	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录	
	ConfigLoader::instance();

	LString workDir = luna::LString(tempPath);
	LString startScript = workDir + "/editor/main.py";

	status = PyConfig_SetString(&config, &config.home, pythonHome.c_str());
		

	char* argv2[2];
	argc = 2;
	argv2[0] = argv[0];
	argv2[1] = new char[startScript.Length() + 1];
	memcpy(argv2[1], startScript.c_str(), startScript.Length());
	argv2[1][startScript.Length()] = 0;
	status = PyConfig_SetBytesArgv(&config, argc, argv2);

	if (PyStatus_Exception(status))
	{
		return -1;
	}

	status = Py_InitializeFromConfig(&config);

	
	if (PyStatus_Exception(status))
	{
		return -1;

	}
	PyConfig_Clear(&config);
	Py_RunMain();
	return 0;
}