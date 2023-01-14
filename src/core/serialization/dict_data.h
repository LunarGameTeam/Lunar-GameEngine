#pragma once
/*!
 * \file dict_data.h
 *
 * \author IsakW
 * \date 2021.3.14
 *
 *
 */

#include "core/foundation/string.h"
#include "core/math/math.h"
#include "core/foundation/log.h"

#include <json/json.h>


namespace luna
{

class Dictionary;
class List;

namespace detail
{

extern Json::Value nullValue;

template<typename T>
struct default_value {	static constexpr T value = 0; };

template<typename T>
static void convert_to_value(Json::Value &val, const T &t) { val = t; };

template<typename T>
static void convert_from_value(const Json::Value &val, T &t) { t = val.as<T>(); };

template<>
struct CORE_API default_value<LString> { static constexpr const char *value = ""; };

template<>
struct CORE_API default_value<Dictionary> { static const Dictionary value; };

template<>
struct CORE_API default_value<List> { static const List value; };

template<>
struct CORE_API default_value<LVector3f> { static LVector3f value; };

template<>
struct CORE_API default_value<LVector2f> { static LVector2f value; };

template<>
struct CORE_API default_value<LQuaternion> { static LQuaternion value; };

}

//TODO 优化写法，容错检查

/*!
 * \class List
 *
 * \brief
 *
 * \author wangyunfei02
 * \date 2021.3.15
 */
class CORE_API List
{
public:
	List() :
		mValue(detail::nullValue)
	{
		assert(0);
	}

	List(Json::Value &val) :
		mValue(val)
	{

	}
	List(List &&value) noexcept :
		mValue(value.mValue)
	{

	}
	List(const List &value) :
		mValue(value.mValue)
	{

	}

	inline uint32_t Size()
	{
		return mValue.size();
	}

	void Resize(uint32_t size)
	{
		mValue.resize(size);
	}
	
	template<typename T>
	bool Set(int index, const T &t)
	{
		Json::Value val;
		detail::convert_to_value<T>(val, t);
		mValue[index] = val;
		return true;
	}

	template<typename T>
	T Get(int index, const T &default_value = detail::default_value<T>::value)
	{
		T res;
		const Json::Value &val = mValue[index];
		if (val.isNull())
			return default_value;
		detail::convert_from_value(val, res);
		return res;
	}

	Dictionary GetDict(int index);
	const Json::Value &GetJsonValue() const
	{
		return mValue;
	};

private:
	Json::Value &mValue;

	template<typename T>
	friend void detail::convert_from_value(const Json::Value &, T &);
};
/*!
 * \class Dictionary
 *
 * \brief
 *
 * \author wangyunfei02
 * \date 2021.3.15
 */
class CORE_API Dictionary
{
public:
	Dictionary():
		mValue(detail::nullValue)
	{
		assert(0);
	}

	Dictionary(Json::Value& val) :
		mValue(val)
	{

	}
	Dictionary(Dictionary &&value) noexcept :
		mValue(value.mValue)
	{

	}
	Dictionary(const Dictionary &value) :
		mValue(value.mValue)
	{

	}
	template<typename T>
	T As()
	{
		T res = detail::default_value<T>::value;
		if (mValue.isNull())
			return res;
		detail::convert_from_value(mValue, res);
		return res;
	}

	template<typename T>
	bool Set(const LString &key, const T &t)
	{
		Json::Value val;
		detail::convert_to_value<T>(val, t);
		mValue[key.c_str()] = val;
		return true;
	}

	bool Has(const LString& key)
	{
		return mValue.isMember(key.c_str());
	}

	operator bool()
	{
		return !mValue.isNull();
	}

	uint32_t Size() const
	{
		return mValue.size();
	}

	template<typename T>
	T Get(const LString &key, const T& default_val = detail::default_value<T>::value)
	{
		if (!mValue.isMember(key.c_str()))
			return default_val;
		T res;
		const Json::Value& val = mValue[key.c_str()];	
		detail::convert_from_value(val, res);
 		return res;
	}

	template<typename T>
	T Get(uint32_t key, const T& default_val = detail::default_value<T>::value)
	{
		if (!mValue.isMember(key.c_str()))
			return default_val;
		T res;
		const Json::Value& val = mValue.get(key, Json::nullValue);
		detail::convert_from_value(val, res);
		return res;
	}


	Json::Value GetItem(uint32_t index)
	{
		return mValue.get(index, Json::nullValue);
	}

	Dictionary GetDict(const LString &key);

	List GetList(const LString &key);

	static bool FromLString(const LString &src, Json::Value &result)
	{
		return FromBinary((const byte *)src.c_str(), src.Length(), result);
	}

	static bool FromBinary(const byte *src, size_t size, Json::Value &result)
	{
		//读取Meta数据
		Json::CharReaderBuilder builder;	
		JSONCPP_STRING errs;
		std::unique_ptr<Json::CharReader> reader(builder.newCharReader());		
		if (!reader->parse((const char*)src, (const char*)src + size, &result, &errs))
		{
			LogError("Core", "{}", errs.c_str());
			return false;
		}
		return true;
	}
	std::string ToString();
	Json::Value &GetJsonValue() const
	{
		return mValue;
	};

private:
	Json::Value& mValue;

	template<typename T>
	friend void detail::convert_from_value(const Json::Value &, T&);
};

namespace detail
{

template<>
static void convert_to_value(Json::Value &val, const LString &t) { val = t.c_str(); };
template<>
static void convert_to_value(Json::Value &val, const LVector3f &t) { val[0] = t.x();	val[1] = t.y();	val[2] = t.z(); };
template<>
static void convert_to_value(Json::Value &val, const LQuaternion &t) { val[0] = t.x();	val[1] = t.y();	val[2] = t.z(); val[3] = t.w(); };
template<>
static void convert_to_value(Json::Value &val, const LVector2f &t) { val[0] = t.x();	val[1] = t.y(); };
template<>
static void convert_to_value(Json::Value &val, const Dictionary &t) { val = t.GetJsonValue(); };
template<>
static void convert_to_value(Json::Value &val, const List &t) { val = t.GetJsonValue(); };


template<>
static void convert_from_value(const Json::Value &val, LString &t) 
{
	if(!val.isNull())
		t.Assign(val.asCString()); 
};

template<>
static void convert_from_value(const Json::Value &val, LQuaternion &t)
{
	t.x() = val[0].as<float>();
	t.y() = val[1].as<float>();
	t.z() = val[2].as<float>();
	t.w() = val[3].as<float>();
};
template<>
static void convert_from_value(const Json::Value &val, LVector3f &t)
{ 
	t.x() = val[0].as<float>();
	t.y() = val[1].as<float>();
	t.z() = val[2].as<float>();
};
template<>
static void convert_from_value(const Json::Value &val, LVector2f &t)
{
	t.x() = val[0].as<float>();
	t.y() = val[1].as<float>();
};
template<>
static void convert_from_value(const Json::Value &val, Dictionary &t)
{
	t.mValue = val;	
};
template<>
static void convert_from_value(const Json::Value &val, List &t)
{
	t.mValue = val;
};

}

}
