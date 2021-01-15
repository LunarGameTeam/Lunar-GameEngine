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

#include <boost/container/string.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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
	class LString
	{
		
	public:
		using StringContainer = std::string;
		using ElementType = char;
		using WStringContainer = std::wstring;

	private:
		StringContainer m_data;

	public:
		LString();
		LString(const LString& l_val);
		LString(const StringContainer& l_val) : m_data(l_val){}
		LString(const StringContainer&& r_val) : m_data(r_val){}
		//从字符串指针内新建，需要以\0结尾
		LString(const ElementType *value) : m_data(value) {}
		//从字节数组指针中新建，指定Begin和End
		LString(const ElementType *first, const ElementType *end) :m_data(first, end) {};

		void Assign(const char* str);
		void Assign(const StringContainer& container);
		void Assign(const LString& str);

		inline size_t Find(char const ch) const;
		inline size_t Find(const char* str) const;
		inline size_t FindLast(const char ch) const
		{
			return m_data.find_last_of(ch);
		}
		size_t FindLast(const char* str) const;
		bool StartWith(const char* str) const;
		bool StartWith(const char str) const;

		void Replace(const char* str,const char* des);
		void Replace(const StringContainer& str);
		void ReplaceAll(const char* str, const char* dest);

		void EraseAt(size_t index);

		operator const char*() const
		{
			return m_data.c_str();
		}
		void Append(const char* str);
		void Append(const StringContainer& container);
		void Append(const LString& str);

		// 接口
		inline size_t Length() const
		{
			return m_data.length();
		}
		inline LString Substr(size_t pos, size_t n)
		{
			return m_data.substr(pos, n);
		}
		inline LString Substr(size_t pos)
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
		inline auto Find(T&& v)
		{			
			return m_data.find(v);
		}

		inline const StringContainer& str() const
		{
			return m_data;
		}
		inline StringContainer &str()
		{
			return m_data;
		}
		inline const char* c_str() const
		{
			return m_data.c_str();
		}
		WStringContainer GetStdUnicodeString() const;

		// 运算符
		friend LString operator+(const LString& lValue, const LString& rValue);
		friend LString operator+(const LString& lValue, const char* rValue);
		friend LString operator+(const char *lValue, const LString &rValue);
		friend size_t hash_value(const luna::LString &key);

		LString& operator=(const char* const string_in);
		LString& operator=(const StringContainer& string_in);
		LString& operator=(const WStringContainer& string_in);
		LString& operator=(const wchar_t* const string_in);

		bool operator!=(const char* const string_in)
		{
			return m_data != string_in;
		}
		bool operator== (const LString& p) const
		{
			return m_data == p.m_data;
		}
		bool operator< (const LString &p) const
		{
			return m_data < p.m_data;
		}
// 		LString& operator+=(const char* string_in);
// 		LString& operator+=(const LString& string_in);
// 		LString& operator+=(const ContainerType& string_in);
// 		LString& operator+=(const wchar_t* const string_in);
		ElementType& operator[](size_t index);
		const ElementType& operator[](size_t index) const;

		const ElementType* operator*() const;
		const ElementType *operator*();
	public:
		static size_t npos;

		template<typename ... Args>
		static LString Format(const LString &format, Args ... args)
		{
			auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
			std::unique_ptr<char[]> buf(new(std::nothrow) char[size_buf]);

			if (!buf)
				return std::string("");

			std::snprintf(buf.get(), size_buf, format.c_str(), args ...);
			return LString(buf.get(), buf.get() + size_buf - 1);
		}
	};

	LString operator+(const LString& lValue, const LString& rValue);
	LString operator+(const LString& lValue, const char* rValue);
	LString operator+(const char* lValue, const LString& rValue);


	LString::WStringContainer StringToWstring(const LString::StringContainer& string_in);
	LString::StringContainer WstringToString(const LString::WStringContainer& wstring_in);

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

	size_t hash_value(const luna::LString &key);

	
}
