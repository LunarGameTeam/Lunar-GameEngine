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
	LString mGroup;
	LString mKey;
	Type mValue;
public:
	Type GetValue() {
		return mValue;
	}

	Config(const char *group, const char *key, const Type &value) :
		mGroup(group),
		mKey(key),
		mValue(value)
	{
		auto &ins = ConfigManager::instance();
		auto &configs = ins.sConfigs;
		if (configs.contains(mKey))
		{
			mValue = luna::FromString<Type>(configs[mKey].m_value);
		}
		else
		{
			configs[mKey].m_group = mGroup;
			configs[mKey].m_key = mKey;
			configs[mKey].m_value = luna::ToString<Type>(mValue);
		}
	}
};

template<>
class Config<LString>
{
	LString mGroup;
	LString mKey;
	LString mValue;
public:
	LString GetValue() {
		return mValue;
	}

	Config(const char *group, const char *key, const char *value) :
		mGroup(group),
		mKey(key),
		mValue(value)
	{
		static auto &ins = ConfigManager::instance();
		static auto &configs = ins.sConfigs;
		if (configs.contains(mKey))
		{
			mValue = configs[mKey].m_value;
		}
		else
		{
			configs[mKey].m_group = mGroup;
			configs[mKey].m_key = mKey;
			configs[mKey].m_value = mValue;
		}
	}
};

#define CONFIG_DECLARE(Type, Group, Key, DefaultValue) extern Config<Type> Config_##Key;
#define CONFIG_IMPLEMENT(Type, Group, Key, DefaultValue) Config<Type> Config_##Key(#Group, #Key, DefaultValue);
}