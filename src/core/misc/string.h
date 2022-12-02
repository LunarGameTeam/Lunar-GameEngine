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
	using StringContainer = std::string;
	using ElementType = char;
	using WStringContainer = std::wstring;

private:
	StringContainer m_data;

public:
	LString();
	LString(const LString &l_val);
	LString(const StringContainer &l_val) : m_data(l_val) {}
	LString(const StringContainer &&r_val) : m_data(r_val) {}
	//从字符串指针内新建，需要以\0结尾
	LString(const ElementType *value) : m_data(value) {}
	//从字节数组指针中新建，指定Begin和End
	LString(const ElementType *first, const ElementType *end) :m_data(first, end) {};

	void Assign(const char *str);
	void Assign(const StringContainer &container);
	void Assign(const LString &str);

	inline size_t Find(char const ch, size_t offset=0) const;
	inline size_t Find(const char* str) const
	{
		return m_data.find(str);
	};
	inline size_t FindLast(const char ch) const
	{
		return m_data.find_last_of(ch);
	}
	size_t FindLast(const char *str) const;
	bool StartWith(const char *str) const;
	bool StartWith(const char str) const;
	bool EndsWith(const char *str) const;

	void Replace(const char *str, const char *des);
	void Replace(const StringContainer &str);
	void ReplaceAll(const char *str, const char *dest);

	void EraseAt(size_t index);

	operator const char *() const
	{
		return m_data.c_str();
	}
	void Append(const char *str);
	void Append(const StringContainer &container);
	void Append(const LString &str);

	// 接口
	inline size_t Length() const
	{
		return m_data.length();
	}
	inline LString Substr(size_t pos, size_t n) const
	{
		return m_data.substr(pos, n);
	}
	inline LString Substr(size_t pos) const
	{
		return m_data.substr(pos);
	}
	template<typename T>
	inline void EraseFirst(T &&v)
	{
		boost::algorithm::erase_first(m_data, v);
	}
	template<typename T>
	inline void EraseLast(T &&v)
	{
		boost::algorithm::erase_last(m_data, v);
	}
	template<typename T>
	inline bool Contains(T &&v)
	{
		return boost::algorithm::contains(m_data, v);
	}
	template<typename T>
	inline auto Find(T &&v)
	{
		return m_data.find(v);
	}

	inline const StringContainer &str() const
	{
		return m_data;
	}
	inline StringContainer &str()
	{
		return m_data;
	}
	inline const char *c_str() const
	{
		return m_data.c_str();
	}
	WStringContainer GetStdUnicodeString() const;

	// 运算符
	friend CORE_API LString operator+(const LString &lValue, const LString &rValue);
	friend CORE_API LString operator+(const LString &lValue, const char *rValue);
	friend CORE_API LString operator+(const char *lValue, const LString &rValue);

	friend CORE_API size_t hash_value(const luna::LString &key);

	LString &operator=(const char *const string_in);
	LString &operator=(const StringContainer &string_in);
	LString &operator=(const WStringContainer &string_in);
	LString &operator=(const wchar_t *const string_in);

	bool operator!=(const char *const string_in)
	{
		return m_data != string_in;
	}
	bool operator== (const LString &p) const
	{
		return m_data == p.m_data;
	}
	bool operator== (const char *const p) const
	{
		return m_data == p;
	}
	bool operator< (const LString &p) const
	{
		return m_data < p.m_data;
	}
	// 		LString& operator+=(const char* string_in);
	// 		LString& operator+=(const LString& string_in);
	// 		LString& operator+=(const ContainerType& string_in);
	// 		LString& operator+=(const wchar_t* const string_in);
	ElementType &operator[](size_t index);
	const ElementType &operator[](size_t index) const;

	const ElementType *operator*() const;
	const ElementType *operator*();
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

	static const LString& MakeStatic(const LString& source)
	{
		auto& str_pool = StaticStringPool();
		auto it = str_pool.find(source);
		if (it != str_pool.end())
			return *it;
		str_pool.insert(source);
		it = str_pool.find(source);
		return *it;
	}

	static std::set<LString>& StaticStringPool()
	{
		if (!s_const_str_pool)
			s_const_str_pool = new std::set<LString>();
		return *s_const_str_pool;
	}

	size_t Hash() const;

private:
	static  std::set<LString>* s_const_str_pool;
};

LString CORE_API operator+(const LString &lValue, const LString &rValue);
LString CORE_API operator+(const LString &lValue, const char *rValue);
LString CORE_API operator+(const char *lValue, const LString &rValue);

CORE_API LString::WStringContainer StringToWstring(const LString::StringContainer &string_in);
CORE_API LString::StringContainer WstringToString(const LString::WStringContainer &wstring_in);
CORE_API bool SplitString(const LString& Src, const char ch, std::vector<LString>& res);

template<typename Source, typename Target>
inline Target LexicalCast(const Source &source)
{
	return boost::lexical_cast<Target>(Source);
}
template<typename Source>
inline LString ToString(const Source &source)
{
	return LString(boost::lexical_cast<LString::StringContainer>(source));
}
template<typename Target>
inline Target FromString(const LString &source)
{
	return boost::lexical_cast<Target>(source.c_str());
}
template<typename const char *>
inline const char *FromString(const LString &source)
{
	return source.c_str();
}
}


namespace std
{

template<> struct std::formatter<luna::LString> : std::formatter<luna::LString::StringContainer>
{
	auto format(const luna::LString& p, std::format_context& ctx) const -> decltype(ctx.out()) {
		// ctx.out() is an output iterator to write to.		
		return  std::formatter<luna::LString::StringContainer>::format(p.str(), ctx);
	}
};

template<> struct std::hash<luna::LString>
{
	std::size_t operator() (const luna::LString &p) const
	{
		return std::hash<luna::LString::StringContainer>()(p.str());
	}
};
}