#pragma once 

#include "core/misc/container.h"
#include "core/misc/string.h"
#include "config_manager.h"

namespace luna
{

struct CORE_API SerializeConfig
{
	LString m_group;
	LString m_key;
	LString m_value;
};
/*!
 * \class Config
 *
 * \brief 含有默认值的引擎配置项，会在启动时读取工作目录下的config.ini，关闭时写入
 *
 * \author isAk wOng
 * 
 */

template<typename Type>
class Config
{
	LString m_group;
	LString m_key;
	Type m_value = DefaultValue;
public:
	Type GetValue() {
		return m_value;
	}

	Config(const char *group, const char *key, const Type &value) :
		m_group(group),
		m_key(key),
		m_value(value)
	{
		auto &ins = ConfigManager::instance();
		auto &configs = ins.s_configs;
		if (configs.contains(m_key))
		{
			m_value = luna::FromString<Type>(configs[m_key].m_value);
		}
		else
		{
			configs[m_key].m_group = m_group;
			configs[m_key].m_key = m_key;
			configs[m_key].m_value = luna::ToString<Type>(m_value);
		}
	}
};

template<>
class Config<LString>
{
	LString m_group;
	LString m_key;
	LString m_value;
public:
	LString GetValue() {
		return m_value;
	}

	Config(const char *group, const char *key, const char* value) :
		m_group(group),
		m_key(key),
		m_value(value)
	{
		static auto &ins = ConfigManager::instance();
		static auto &configs = ins.s_configs;
		if (configs.contains(m_key))
		{
			m_value = configs[m_key].m_value;
		}
		else
		{
			configs[m_key].m_group = m_group;
			configs[m_key].m_key = m_key;
			configs[m_key].m_value = m_value;
		}
	}
};

#define CONFIG_DECLARE(Type, Group, Key, DefaultValue) extern Config<Type> Key;
#define CONFIG_IMPLEMENT(Type, Group, Key, DefaultValue) Config<Type> Key(#Group, #Key, DefaultValue);

}