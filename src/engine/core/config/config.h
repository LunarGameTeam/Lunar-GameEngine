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
template<typename Type, Type DefaultValue>
class CORE_API Config
{
	LString m_group;
	LString m_key;
	Type m_value = DefaultValue;
public:
	Type GetValue() {
		return m_value;
	}

	Config(const char *group, const char *key) :
		m_group(group),
		m_key(key)
	{
		if (ConfigManager::instance().s_configs.contains(m_key))
		{
			m_value = luna::FromString<Type>(ConfigManager::instance().s_configs[m_key].m_value);
		}
		else
		{
			ConfigManager::instance().s_configs[m_key].m_group = m_group;
			ConfigManager::instance().s_configs[m_key].m_key = m_key;
			ConfigManager::instance().s_configs[m_key].m_value = luna::ToString<Type>(m_value);
		}
	}
};



}