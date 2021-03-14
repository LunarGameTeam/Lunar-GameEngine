#pragma once
/*!
 * \file dict_data.h
 *
 * \author IsakW
 * \date 2021.3.14
 *
 * 
 */
#include "core/private_core.h"
#include "core/misc/string.h"
#include <json/json.h>

namespace Json
{
class Value;
}

namespace luna
{
template<typename>
struct default_value;

template<>
struct default_value<int> {
	static constexpr int value = 0;
};

template<>
struct default_value<float> {
	static constexpr float value = 0.0f;
};

template<>
struct default_value<double> {
	static constexpr double value = 0.0f;
};

template<>
struct default_value<LString> {
	static constexpr const char* value = "";
};

template<>
struct default_value<unsigned int> {
	static constexpr unsigned int value = 0;
};

template<>
struct default_value<char> {
	static constexpr char value = 0;
};

class CORE_API Dictionary
{
public:
	Dictionary()
	{

	}

	Dictionary(const Json::Value &value) :
		m_value(value)
	{

	}
	template<typename T>
	T Get(const LString& key,const T& default_value = default_value<T>::value)
	{
		return Get<T>(key.c_str());
	}

	template<typename T>
	T Get(const char * key,const T& default_value)
	{
		return m_value.get(key, default_value).as<T>();
	}

	template<>
	LString Get(const char *key, const LString &default_value)
	{
		return LString(m_value.get(key, default_value.c_str()).asCString());
	}

	template<>
	Dictionary Get(const char *key, const Dictionary &default_value)
	{
		return Dictionary(m_value.get(key, Json::nullValue));
	}

	static bool FromBinary(const char *byte, size_t size, Dictionary &result)
	{
		//读取Meta数据
		Json::CharReaderBuilder builder;
		std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		if (!reader->parse(byte, byte + size, &result.m_value, nullptr))
			return false;
		return true;
	}

private:
	Json::Value m_value;

};


}
