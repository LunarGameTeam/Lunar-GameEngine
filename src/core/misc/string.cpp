
#include "string.h"
#include "windows.h"

namespace luna
{

std::set<LString>* LString::s_const_str_pool = nullptr;

LString::LString()
{
}

LString::LString(const LString &lValue) :
	m_data(lValue.m_data)
{
}

LString &LString::operator=(const char *const string_in)
{
	m_data = string_in;
	return *this;
}
LString &LString::operator=(const StringContainer &string_in)
{
	m_data = string_in.c_str();
	return *this;
}
LString &LString::operator=(const wchar_t *string_in)
{
	m_data = WstringToString(string_in);
	return *this;
}
LString &LString::operator=(const WStringContainer &string_in)
{
	auto unicode_string = WstringToString(string_in.c_str());
	return *this;
}

void LString::Append(const LString &str)
{
	m_data.append(str.m_data);
}

void LString::Append(const StringContainer &container)
{
	m_data.append(container);
}

void LString::Append(const char *str)
{
	m_data.append(str);
}

void LString::Assign(const char *str)
{
	m_data.assign(str);
}

void LString::Assign(const StringContainer &container)
{
	m_data.assign(container);
}

void LString::Assign(const LString &str)
{
	m_data.assign(str.m_data);
}

char &LString::operator[](size_t index)
{
	return m_data[index];
}

const LString::ElementType &LString::operator[](size_t index) const
{
	return m_data[index];
}

const LString::ElementType *LString::operator*() const
{
	return m_data.c_str();
}

const luna::LString::ElementType *LString::operator*()
{
	return m_data.c_str();
}

size_t LString::Hash() const
{
	static auto hasher = std::hash<StringContainer>();
	return hasher(m_data);
}

size_t LString::Find(char const ch, size_t offset/*=0*/) const
{
	return m_data.find(ch, offset);
}

LString::WStringContainer LString::GetStdUnicodeString() const
{
	return StringToWstring(m_data);
}

void LString::Replace(const char *str, const char *des)
{
	boost::algorithm::replace_all(m_data, str, des);
}

bool LString::StartWith(const char *str) const
{
	return boost::algorithm::starts_with(m_data, str);
}
bool LString::StartWith(const char str) const
{
	if (m_data.length() > 0)
		return m_data[0] == str;
	return false;
}

bool LString::EndsWith(const char *str) const
{
	return m_data.ends_with(str);
}

size_t LString::FindLast(const char *str) const
{
	return m_data.find_last_of(str);
}

void luna::LString::Replace(const StringContainer &str)
{
	boost::algorithm::replace_all(m_data, str, str);
}

void LString::EraseAt(size_t index)
{
	m_data.erase(index);
}

void LString::ReplaceAll(const char *str, const char *dest)
{
	boost::algorithm::replace_all(m_data, str, dest);
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

LString operator+(const LString &lValue, const LString &rValue)
{
	LString res;
	res.m_data = lValue.m_data + rValue.m_data;
	return res;
}

LString operator+(const char *lValue, const LString &rValue)
{
	LString res;
	res.m_data.assign(lValue);
	res.m_data += rValue.m_data;
	return res;
}

LString operator+(const LString &lValue, const char *rValue)
{
	LString res(lValue);
	res.Append(rValue);
	return res;
}

LString::WStringContainer StringToWstring(const LString::StringContainer &string_in)
{
	LString::WStringContainer unicode_string;
	int nLen = (int)string_in.length();
	unicode_string.resize(nLen, L' ');
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)string_in.c_str(), nLen, (LPWSTR)unicode_string.c_str(), nLen);
	return unicode_string;
}
LString::StringContainer WstringToString(const LString::WStringContainer &wstring_in)
{
	LString::StringContainer ascii_string;
	int nLen = (int)wstring_in.length();
	ascii_string.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstring_in.c_str(), nLen, (LPSTR)ascii_string.c_str(), nLen, NULL, NULL);
	return ascii_string;
}

CORE_API bool SplitString(const LString& Src, const char ch, std::vector<LString>& res)
{
	LString rest = Src;
	size_t first_pos = 0;
	size_t ch_pos = Src.Find(ch);
	if (ch_pos == Src.npos)
		return false;
	while (ch_pos != Src.npos)
	{
		LString ele = Src.Substr(first_pos, ch_pos - first_pos);
		res.push_back(ele);
		if(ch_pos == Src.Length())
			break;
		first_pos = ch_pos + 1;
		ch_pos = Src.Find(ch, first_pos);
	}
	return true;
}

}