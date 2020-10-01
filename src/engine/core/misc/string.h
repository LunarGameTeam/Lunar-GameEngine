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

namespace LunarEngine
{

	/*!
	 * \class LString
	 *
	 * \brief LString是最基础的多字节字符存储类，他不负责处理编码等问题，
	 *
	 * \author IsakWong
	 * 
	*/
	//各种类字符串处理类
	class LString
	{
		
	public:
		using ContainerType = std::string;
		using ElementType = char;
		using WStringDataType = std::wstring;

	private:
		ContainerType m_Data;

	public:

		LString();
		LString(const LString& lValue);

		LString(const ContainerType& lValue) : m_Data(lValue)
		{

		}

		LString(const ContainerType&& rValue) : m_Data(rValue)
		{

		}


		LString(const ElementType * value) : m_Data(value)
		{

		}

		void Assign(const char* str);
		void Assign(const ContainerType& container);
		void Assign(const LString& str);

		inline size_t Find(char const ch) const;
		inline size_t Find(const char* str) const;
		inline size_t FindLast(const char ch) const
		{
			return m_Data.find_last_of(ch);
		}
		size_t FindLast(const char* str) const;
		bool StartWith(const char* str) const;
		bool StartWith(const char str) const;

		void Replace(const char* str,const char* des);
		void Replace(const ContainerType& str);
		void ReplaceAll(const char* str, const char* dest);

		void EraseAt(size_t index);

		operator const char* () const
		{
			return m_Data.c_str();
		}
		void Append(const char* str);
		void Append(const ContainerType& container);
		void Append(const LString& str);

		// 接口
		inline size_t Length() const
		{
			return m_Data.length();
		}
		inline LString Substr(size_t pos, size_t n)
		{
			return m_Data.substr(pos, n);
		}

		inline const ContainerType& str() const
		{
			return m_Data;
		}
		inline const char* c_str() const
		{
			return m_Data.c_str();
		}
		WStringDataType GetStdUnicodeString() const;
		// 运算符
		friend LString operator+(const LString& lValue, const LString& rValue);
		friend LString operator+(const LString& lValue, const char* rValue);
		friend LString operator+(const char* lValue, const LString& rValue);

		LString& operator=(const char* const string_in);
		LString& operator=(const ContainerType& string_in);
		LString& operator=(const WStringDataType& string_in);
		LString& operator=(const wchar_t* const string_in);

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
	};

	LString operator+(const LString& lValue, const LString& rValue);
	LString operator+(const LString& lValue, const char* rValue);
	LString operator+(const char* lValue, const LString& rValue);


	LString::WStringDataType StringToWstring(const LString::ContainerType& string_in);
	LString::ContainerType WstringToString(const LString::WStringDataType& wstring_in);

	template<typename T, typename Source>
	inline T LexicalCast(const Source &source)
	{
		return boost::lexical_cast<T>(source);
	}
}