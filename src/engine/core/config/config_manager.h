#pragma once
#include "core/private_core.h"
#include "core/misc/string.h"
#include <fstream>
#include <iostream> 

namespace luna
{

struct SerializeConfig;
/*!
 * \class ConfigManager
 *
 * \brief 配置管理器，负责了配置最基础的序列化和反序列化
 *
 * \author isakwong
 * 
 */
class ConfigManager
{
public:
	static ConfigManager &instance()
	{
		static ConfigManager manager;
		return manager;
	}

	ConfigManager();
	~ConfigManager();

	LMap<LString, SerializeConfig> s_configs;

private:

};

}