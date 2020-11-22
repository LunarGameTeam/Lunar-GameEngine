#pragma once 
#include "core/private_core.h"
#include "core/misc/string.h"

#include <boost/lexical_cast.hpp>     

class ConfigManager
{
public:
	static Map<const char *, LunarEngine::LString> s_configs;
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
		if (ConfigManager::s_configs.contains(m_key))
			m_value = FromString<Type>(ConfigManager::s_configs[m_key]);
	}
};
