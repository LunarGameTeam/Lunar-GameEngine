#pragma once 
#include "core/private_core.h"
#include "core/misc/string.h"
#include <boost/lexical_cast.hpp>    
#include <boost/container/string.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <fstream>
#include <iostream> 

struct SerializeConfig
{
	luna::LString m_group;
	luna::LString m_key;
	luna::LString m_value;
};

class ConfigManager
{
public:
	class Config;

	ConfigManager()
	{		
		m_config_file.open("Config.ini", std::fstream::in | std::fstream::out);
		boost::container::string temp;
		boost::container::string current_group = "";
		if (!m_config_file.is_open())
			return;

		while (getline(m_config_file, temp))
		{
			
			if (boost::algorithm::starts_with(temp, "["))
			{
				boost::algorithm::erase_first(temp, "[");
				boost::algorithm::erase_last(temp, "[");
				current_group = temp;
			}
			else
			{
				if (boost::algorithm::contains(temp, "="))
				{
					boost::container::string key = "";
					boost::container::string value = "";
					key = temp.substr(0, temp.find(temp, '='));
					value = temp.substr(temp.find(temp, '=') + 1);

					SerializeConfig config;
					config.m_group = current_group.c_str();
					config.m_key = key.c_str();
					config.m_value = value.c_str();

					s_configs[config.m_key.c_str()] = config;
				}
			}
			
		}
		m_config_file.flush();
		m_config_file.close();
	}
	~ConfigManager()
	{
		m_config_file.open("Config.ini", std::fstream::out | std::fstream::trunc);
		boost::container::map<const char *, boost::container::vector<SerializeConfig>> configs;

		for (auto& it : s_configs)
		{
			auto& grouped_config = configs[it.first];
			SerializeConfig config;
			config.m_group = it.second.m_group;
			config.m_key = it.second.m_key;
			config.m_value = it.second.m_value;
			grouped_config.push_back(config);
		}

	}
	static ConfigManager &instance()
	{
		static ConfigManager manager;
		return manager;
	}
	boost::container::map<const char *, SerializeConfig> s_configs;
private:
	std::fstream m_config_file;
};

template<typename Type,Type DefaultValue>
class Config
{
	const char *m_group;
	const char *m_key;
	Type m_value = DefaultValue;
public:
	Type GetValue() {
		return m_value;
	}
	Config(const char* group,const char* key):
		m_group(group),
		m_key(key)
	{
		if (ConfigManager::instance().s_configs.contains(m_key))
		{
			m_value = luna::FromString<Type>(luna::LString(ConfigManager::instance().s_configs[m_key].m_value));
		}
		else
		{
			ConfigManager::instance().s_configs[m_key].m_group = m_group; 
			ConfigManager::instance().s_configs[m_key].m_key = m_key;
			ConfigManager::instance().s_configs[m_key].m_value = luna::ToString<Type>(m_value);
		}
	}
};
