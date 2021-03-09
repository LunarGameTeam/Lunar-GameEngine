#include "config_manager.h"
#include "config.h"
#include <tchar.h>
#include "windows.h"

namespace luna
{

using namespace boost::algorithm;

CORE_API LSharedPtr<ConfigManager> s_config_instance = LSharedPtr<ConfigManager>(nullptr);

ConfigManager::ConfigManager()
{
	
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录

	LString config_path(tempPath);
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
				s_configs[key].m_key = line.Substr(0, pos);
				s_configs[key].m_value = line.Substr(pos + 1);
				s_configs[key].m_group = group;
			}
		}

	}
	fs.flush();
	fs.close();
}

ConfigManager::~ConfigManager()
{
	std::fstream fs;
	TCHAR tempPath[1000];
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	LString path(tempPath);
	path = path + "/config.ini";
	fs.open(path.c_str(), std::fstream::out | std::fstream::trunc);
	LMap<LString, LVector<SerializeConfig>> configs;

	for (auto &it : s_configs)
	{
		auto &group_config = configs[it.second.m_group];
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
		LString group = LString::Format("[%s]\n", it.first.c_str());
		fs << group.c_str();
		for (auto &config : it.second)
		{
			fs << config.m_key << "=" << config.m_value << std::endl;
		}
	}
	fs.flush();
	fs.close();
}


}