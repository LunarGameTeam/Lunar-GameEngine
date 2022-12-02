#include "config_manager.h"
#include "config.h"
#include <tchar.h>
#include "windows.h"

#include "core/file/platform_module.h"
#include "core/framework/luna_core.h"

namespace luna
{
using namespace boost::algorithm;

ConfigManager::ConfigManager()
{
	TCHAR tempPath[1000];
	auto platform_module = LunaCore::Ins()->GetModule<PlatformModule>();
	IPlatformFileManager* file_manager = platform_module->GetPlatformFileManager();	
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString config_path = file_manager->EngineDir();
	config_path = config_path + "/config.ini";
	std::fstream fs;
	fs.open(config_path.c_str(), std::fstream::in | std::fstream::out);

	if (!fs.is_open())
		return;

	LString line;
	LString group = "";

	while (getline(fs, line.str()))
	{
		if (line.StartWith("["))
		{
			line.EraseFirst("[");
			line.EraseLast("]");
			group = line;
		}
		else
		{
			if (line.Contains("="))
			{
				auto pos = line.Find('=');
				LString key = line.Substr(0, pos);
				sConfigs[key].m_key = line.Substr(0, pos);
				sConfigs[key].m_value = line.Substr(pos + 1);
				sConfigs[key].m_group = group;
			}
		}
	}
	fs.flush();
	fs.close();
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::Save()
{

	std::fstream fs;
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString path(tempPath);
	path = path + "/config.ini";
	fs.open(path.c_str(), std::fstream::out | std::fstream::trunc);
	LMap<LString, LVector<SerializeConfig>> configs;

	for (auto& it : sConfigs)
	{
		auto& group_config = configs[it.second.m_group];
		SerializeConfig config
		{
			it.second.m_group,
			it.second.m_key,
			it.second.m_value
		};
		group_config.push_back(config);
	}

	for (auto& it : configs)
	{
		LString group = LString::Format("[{0}]\n", it.first.c_str());
		fs << group.c_str();
		for (auto& config : it.second)
		{
			fs << config.m_key << "=" << config.m_value << std::endl;
		}
	}
	fs.flush();
	fs.close();
}

}