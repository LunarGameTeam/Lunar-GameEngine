#include "core/binding/binding_traits.h"
#include "core/framework/application.h"
#include "core/foundation/config.h"
#include "windows.h"

#undef main

namespace luna
{

CONFIG_IMPLEMENT(LString, Start, InitScript, "\\editor\\main.py");

}


int main(int argc, char** argv)
{
	PyStatus status;

	PyConfig config;
	PyConfig_InitPythonConfig(&config);

	auto pythonHome = luna::StringToWstring(PYTHON3_PATH);
	config.isolated = 1;
	
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录	
	luna::ConfigLoader::instance();
	luna::LString luna_dir = luna::LString(tempPath);
	luna::LString init_script = luna_dir + luna::Config_InitScript.GetValue();

	status = PyConfig_SetString(&config, &config.home, pythonHome.c_str());


	char** argv2 = new char* [2];
	argc = 2;
	argv2[0] = argv[0];
	argv2[1] = new char[init_script.Length() + 1];
	memcpy(argv2[1], init_script.c_str(), init_script.Length());
	argv2[1][init_script.Length()] = 0;
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
	return Py_RunMain();
}