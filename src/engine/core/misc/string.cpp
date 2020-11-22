#include "string.h"
#include "core/private_core.h"

using namespace LunarEngine;

size_t LString::npos = LString::ContainerType::npos;

LString::LString()
{

}

LString::LString(const LString& lValue) :
	m_Data(lValue.m_Data)
{

}




LString& LString::operator=(const char* const string_in)
{
	m_Data = string_in;
	return *this;
}
LString& LString::operator=(const ContainerType& string_in)
{
	m_Data = string_in.c_str();
	return *this;
}
LString& LString::operator=(const wchar_t* string_in)
{
	m_Data = WstringToString(string_in);
	return *this;
}
LString& LString::operator=(const WStringDataType& string_in)
{
	auto unicode_string = WstringToString(string_in.c_str());
	return *this;
}

void LString::Append(const LString& str)
{
	m_Data.append(str.m_Data);
}

void LString::Append(const ContainerType& container)
{
	m_Data.append(container);
}

void LString::Append(const char* str)
{
	m_Data.append(str);
}

void LString::Assign(const char* str)
{
	m_Data.assign(str);
}

void LString::Assign(const ContainerType& container)
{
	m_Data.assign(container);

}

void LString::Assign(const LString& str)
{
	m_Data.assign(str.m_Data);
}



char& LString::operator[](size_t index)
{
	return m_Data[index];
}

const LString::ElementType& LString::operator[](size_t index) const
{
	return m_Data[index];
}

const LString::ElementType* LString::operator*() const
{
	return m_Data.c_str();
}

const LunarEngine::LString::ElementType *LString::operator*()
{
	return m_Data.c_str();
}

LString::WStringDataType LString::GetStdUnicodeString() const
{
	return StringToWstring(m_Data).c_str();
}

void LString::Replace(const char* str, const char* des)
{
	boost::algorithm::replace_all(m_Data, str, des);
}

bool LString::StartWith(const char* str) const
{
	return boost::algorithm::starts_with(m_Data, str);
}
bool LString::StartWith(const char str) const
{
	assert(m_Data.length() > 0);
	return m_Data[0] == str;
}

size_t LString::Find(const char* str) const
{
	return m_Data.find_first_of(str);
}
size_t LString::Find(const char ch) const
{
	return m_Data.find_first_of(ch);
}

size_t LString::FindLast(const char* str) const
{
	return m_Data.find_last_of(str);
}

void LunarEngine::LString::Replace(const ContainerType& str)
{
	boost::algorithm::replace_all(m_Data, str, str);
}

void LString::EraseAt(size_t index)
{
	m_Data.erase(index);
}

void LString::ReplaceAll(const char* str, const char* dest)
{
	boost::algorithm::replace_all(m_Data, str, dest);
}

// LString& LString::operator+=(const char* const string_in)
// {
// 	m_Data += string_in;
// 	return *this;
// }
// LString& LString::operator+=(const ContainerType& string_in)
// {
// 	m_Data += string_in;
// 	return *this;
// }
// 
// LString& LString::operator+=(const wchar_t* const string_in)
// {
// 	WStringDataType now_wstring = StringToWstring(m_Data);
// 	now_wstring += string_in;
// 	m_Data = WstringToString(now_wstring);
// 	return *this;
// }

LString LunarEngine::operator+(const LString& lValue, const LString& rValue)
{
	LString res;
	res.m_Data = lValue.m_Data + rValue.m_Data;
	return res;
}



LString LunarEngine::operator+(const char* lValue, const LString& rValue)
{
	LString res;
	res.m_Data.assign(lValue);
	res.m_Data += rValue.m_Data;
	return res;
}

LString LunarEngine::operator+(const LString& lValue, const char* rValue)
{
	LString res(lValue);
	res.Append(rValue);
	return res;
}

LString::WStringDataType LunarEngine::StringToWstring(const LString::ContainerType& string_in)
{
	LString::WStringDataType unicode_string;
	int nLen = (int)string_in.length();
	unicode_string.resize(nLen, L' ');
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)string_in.c_str(), nLen, (LPWSTR)unicode_string.c_str(), nLen);
	return unicode_string;
}
LString::ContainerType LunarEngine::WstringToString(const LString::WStringDataType& wstring_in)
{
	LString::ContainerType ascii_string;
	int nLen = (int)wstring_in.length();
	ascii_string.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstring_in.c_str(), nLen, (LPSTR)ascii_string.c_str(), nLen, NULL, NULL);
	return ascii_string;
}

