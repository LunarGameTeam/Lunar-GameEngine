#pragma once

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Misc.h"

/*!
 * \class Config
 *
 * \brief 含有默认值的引擎配置项，会在启动时读取工作目录下的config.ini，关闭时写入
 *
 * \author isAk wOng
 *
 */

namespace luna
{

struct CORE_API SerializeConfig
{
	SerializeConfig()
	{

	}

	LString mGroup;
	LString mKey;
	LString mValue;
};


/*!
 * \class ConfigManager
 *
 * \brief 配置管理器，负责了配置最基础的序列化和反序列化
 *
 * \author isAk wOng
 *
 */
class CORE_API ConfigLoader : public luna::Singleton<ConfigLoader>
{
public:
	ConfigLoader();
	~ConfigLoader();

	void LoadFromJson(const LString& val);
	void SaveJson(std::fstream& fs);
	void Load(const LString& val);
	void Save(std::fstream& fs);

	LString GetValue(const LString& val) { return sConfigs[val]->mValue; }
	void SetValue(const LString& key, const LString& val) { sConfigs[key]->mValue = val; }
private:
	LMap<LString, SerializeConfig*> sConfigs;

	template<typename T>
	friend class Config; // every B<T> is a friend of A
};


template<typename Type>
class Config : public SerializeConfig
{
public:
	Type GetValue()
	{
		return luna::FromString<Type>(mValue);
	}
	void SetValue(const Type& val)
	{
		mValue = luna::ToString(val);
	}

	Config(const char *group, const char *key, const Type &value)		
	{
		mGroup = group;
		mKey = key;
		mValue = luna::ToString(value);
		auto &ins = ConfigLoader::instance();
		auto &configs = ins.sConfigs;
		if (configs.contains(mKey))
		{
			mValue = configs[mKey]->mValue;
		}
		else
		{
			ins.sConfigs[key] = this;
		}
		
	}
};


#define CONFIG_DECLARE(Type, Group, Key, DefaultValue) extern Config<Type> Config_##Key;
#define CONFIG_IMPLEMENT(Type, Group, Key, DefaultValue) Config<Type> Config_##Key(#Group, #Key, DefaultValue);
}