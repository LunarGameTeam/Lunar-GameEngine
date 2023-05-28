/*!
 * \file string.h
 * \date 2020/08/11 13:44
 *
 * \author isakwang
 * Contact: user@company.com
 *
 * \brief
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


#include <xhash>
#include <set>
#include <format>

#ifndef CORE_API
#ifdef CORE_EXPORT
#define CORE_API __declspec( dllexport )//宏定义
#else
#define CORE_API __declspec( dllimport )
#endif
#endif

namespace luna
{
/*!
 * \class LString
 *
 * \brief LString是最基础的多字节字符存储类，他不负责处理编码等问题。
 *
 * \author IsakWong
 *
*/
//各种类字符串处理类
class CORE_API LString
{
public:
	using container_t = std::string;
	using element_t = char;
	using wcontainer_t = std::wstring;

private:
	container_t mStdStr;

public:
	LString();
	LString(const LString &l_val);
	LString(const container_t &l_val) : mStdStr(l_val) {}
	LString(const container_t &&r_val) : mStdStr(r_val) {}

	//从字符串指针内新建，需要以\0结尾
	constexpr LString(const element_t *value) : mStdStr(value) 
	{

	}

	//从字节数组指针中新建，指定Begin和End
	LString(const element_t *first, const element_t *end) :mStdStr(first, end) {};

	void Assign(const char *str);
	void Assign(const container_t &container);
	void Assign(const LString &str);

	inline size_t Find(char const ch, size_t offset=0) const;

	inline size_t Find(const char* str) const
	{
		return mStdStr.find(str);
	};

	inline size_t FindLast(const char ch) const
	{
		return mStdStr.find_last_of(ch);
	}
	size_t FindLast(const char *str) const;
	bool StartWith(const char *str) const;
	bool StartWith(const char str) const;
	bool EndsWith(const char *str) const;

	void Replace(const char *str, const char *des);
	void Replace(const container_t &str);
	void ReplaceAll(const char *str, const char *dest);

	void EraseAt(size_t index);

	operator const char *() const
	{
		return mStdStr.c_str();
	}
	void Append(const char *str);
	void Append(const container_t &container);
	void Append(const LString &str);
	void PushBack(const char str);
	// 接口
	inline size_t Length() const
	{
		return mStdStr.length();
	}

	inline LString Substr(size_t pos, size_t n) const
	{
		return mStdStr.substr(pos, n);
	}

	inline LString Substr(size_t pos) const
	{
		return mStdStr.substr(pos);
	}

	inline int Compare(const LString& other) const
	{
		return mStdStr.compare(other.c_str());
	}

	void SplitOnce(std::vector<LString>& result, const char* s) const
	{
		auto it = mStdStr.find_first_of(s);
		if (it == std::string::npos)
			return;
		result.resize(2);
		result[0] = mStdStr.substr(0, it);
		result[1] = mStdStr.substr(it);
	}

	void RSplitOnce(std::vector<LString>& result, const char* s) const
	{

		auto it = mStdStr.find_last_of(s);
		if (it == std::string::npos)
			return;
		result.resize(2);
		result[0] = mStdStr.substr(0, it);
		result[1] = mStdStr.substr(it);
		return;
	}

	template<typename T>
	inline void EraseFirst(T &&v)
	{
		boost::algorithm::erase_first(mStdStr, v);
	}
	template<typename T>
	inline void EraseLast(T &&v)
	{
		boost::algorithm::erase_last(mStdStr, v);
	}
	template<typename T>
	inline bool Contains(T &&v)
	{
		return boost::algorithm::contains(mStdStr, v);
	}

	template<typename T>
	inline auto Find(T &&v)
	{
		return mStdStr.find(v);
	}

	inline const container_t &std_str() const
	{
		return mStdStr;
	}

	inline container_t &std_str()
	{
		return mStdStr;
	}

	inline const char *c_str() const
	{
		return mStdStr.c_str();
	}

	inline void AddLine(const char* newLine)
	{
		mStdStr = LString::Format("{0}\n{1}", mStdStr, newLine);
	}

	wcontainer_t GetStdUnicodeString() const;

	// 运算符
	friend CORE_API LString operator+(const LString &lValue, const LString &rValue);
	friend CORE_API LString operator+(const LString &lValue, const char *rValue);
	friend CORE_API LString operator+(const char *lValue, const LString &rValue);

	friend CORE_API size_t hash_value(const luna::LString &key);

	LString &operator=(const char *const string_in);
	LString &operator=(const container_t &string_in);
	LString &operator=(const wcontainer_t &string_in);
	LString &operator=(const wchar_t *const string_in);

	bool operator!=(const char *const string_in)
	{
		return mStdStr != string_in;
	}
	bool operator== (const LString &p) const
	{
		return mStdStr == p.mStdStr;
	}
	bool operator== (const char *const p) const
	{
		return mStdStr == p;
	}
	bool operator< (const LString &p) const
	{
		return mStdStr < p.mStdStr;
	}
	// 		LString& operator+=(const char* string_in);
	// 		LString& operator+=(const LString& string_in);
	// 		LString& operator+=(const ContainerType& string_in);
	// 		LString& operator+=(const wchar_t* const string_in);
	element_t &operator[](size_t index);
	const element_t &operator[](size_t index) const;

	const element_t *operator*() const;
	const element_t *operator*();
public:
	auto static constexpr npos { static_cast<size_t>(-1) };

	template<typename ... Args>
	inline static LString Format(const LString& format, Args ... args)
	{		
		std::string_view view = format.c_str();
		std::string result = std::vformat(view, std::make_format_args(args...));
		return LString(std::move(result));
	}

	template<typename ... Args>
	inline static LString Format(const char* format, Args ... args)
	{
		std::string_view view = format;
		std::string result = std::vformat(view, std::make_format_args(args...));
		return LString(std::move(result));
	}


	template<>
	inline static LString Format(const LString& format)
	{		
		return format;
	}

	template<>
	inline static LString Format(const char* format)
	{
		return LString(format);
	}

	static const char* MakeStatic(const LString source)
	{
		auto& statiStrPool = StaticStringPool();
		auto it = statiStrPool.find(source);
		if (it != statiStrPool.end())
			return *it;
		statiStrPool.insert(source);
		it = statiStrPool.find(source);
		return *it;
	}

	static std::set<LString>& StaticStringPool()
	{
		if (!sStaticStrPool)
			sStaticStrPool = new std::set<LString>();
		return *sStaticStrPool;
	}

	void Split(std::vector<LString>& res, const char* s)
	{
		std::vector<std::string> result;
		boost::split(result, mStdStr, boost::is_any_of(s));
		res.reserve(result.size());
		for (auto& it : result)
		{
			res.emplace_back(it);
		}
	}

	size_t Hash() const;

private:
	static  std::set<LString>* sStaticStrPool;
};

LString CORE_API operator+(const LString &lValue, const LString &rValue);
LString CORE_API operator+(const LString &lValue, const char *rValue);
LString CORE_API operator+(const char *lValue, const LString &rValue);

CORE_API LString::wcontainer_t StringToWstring(const LString::container_t &string_in);
CORE_API LString::container_t WstringToString(const LString::wcontainer_t &wstring_in);
CORE_API bool SplitString(const LString& Src, const char ch, std::vector<LString>& res);

template<typename Source, typename Target>
inline Target LexicalCast(const Source &source)
{
	return boost::lexical_cast<Target>(Source);
}

template<typename Source>
inline LString ToString(const Source &source)
{
	return LString(boost::lexical_cast<LString::container_t>(source));
}

template<>
inline LString ToString<LString>(const LString& source)
{
	return source;
}

template<typename Target>
inline Target FromString(const LString &source)
{
	return boost::lexical_cast<Target>(source.c_str());
}

template<>
inline LString FromString<LString>(const LString& source)
{
	return source;
}

template<typename const char *>
const char *FromString(const LString &source)
{
	return source.c_str();
}
}


namespace std
{

template<> struct std::formatter<luna::LString> : std::formatter<luna::LString::container_t>
{
	auto format(const luna::LString& p, std::format_context& ctx) const -> decltype(ctx.out()) {
		// ctx.out() is an output iterator to write to.		
		return  std::formatter<luna::LString::container_t>::format(p.std_str(), ctx);
	}
};

template<> struct std::hash<luna::LString>
{
	std::size_t operator() (const luna::LString &p) const
	{
		return std::hash<luna::LString::container_t>()(p.std_str());
	}
};
}
