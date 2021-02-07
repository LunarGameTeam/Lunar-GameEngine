#pragma once
#include "core/private_core.h"
#include "core/misc/string.h"
#include "core/misc/container.h"
#include "core/memory/ptr.h"
#include <fstream>
#include <iostream> 

namespace luna
{

struct CORE_API SerializeConfig;
/*!
 * \class ConfigManager
 *
 * \brief 配置管理器，负责了配置最基础的序列化和反序列化
 *
 * \author isAk wOng
 * 
 */
class CORE_API ConfigManager
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