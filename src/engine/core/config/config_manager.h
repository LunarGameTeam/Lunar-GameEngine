#pragma once
#include "core/private_core.h"
#include "core/misc/string.h"
#include "core/misc/container.h"
#include "core/memory/ptr.h"
#include "core/misc/misc.h"
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
class CORE_API ConfigManager;

CORE_API extern LSharedPtr<ConfigManager> s_config_instance;

class CORE_API ConfigManager
{
public:
	static ConfigManager &instance()
	{
		if (s_config_instance.get() == nullptr)
			s_config_instance = LSharedPtr<ConfigManager>(new ConfigManager());
		return *s_config_instance.get();
	}

	ConfigManager();
	virtual ~ConfigManager();

private:
	LMap<LString, SerializeConfig> s_configs;

	template<typename T>
	friend class Config; // every B<T> is a friend of A
};

}