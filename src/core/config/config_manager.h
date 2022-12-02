#pragma once

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
class CORE_API ConfigManager : public luna::Singleton<ConfigManager>
{
public:
	ConfigManager();
	~ConfigManager();

	void Save();
private:
	LMap<LString, SerializeConfig> sConfigs;

	template<typename T>
	friend class Config; // every B<T> is a friend of A
};
}