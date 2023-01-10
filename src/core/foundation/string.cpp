
#include "string.h"
#include "windows.h"

namespace luna
{

std::set<LString>* LString::sStaticStrPool = nullptr;

LString::LString()
{
}

LString::LString(const LString &lValue) :
	mStdStr(lValue.mStdStr)
{
}

LString &LString::operator=(const char *const string_in)
{
	mStdStr = string_in;
	return *this;
}
LString &LString::operator=(const container_t &lv)
{
	mStdStr = lv.c_str();
	return *this;
}
LString &LString::operator=(const wchar_t *string_in)
{
	mStdStr = WstringToString(string_in);
	return *this;
}
LString &LString::operator=(const wcontainer_t &lv)
{
	auto unicode_string = WstringToString(lv.c_str());
	return *this;
}

void LString::Append(const LString &lv)
{
	mStdStr.append(lv.mStdStr);
}

void LString::Append(const container_t &lv)
{
	mStdStr.append(lv);
}

void LString::Append(const char *str)
{
	mStdStr.append(str);
}

void LString::Assign(const char *str)
{
	mStdStr.assign(str);
}

void LString::Assign(const container_t &lv)
{
	mStdStr.assign(lv);
}

void LString::Assign(const LString &lv)
{
	mStdStr.assign(lv.mStdStr);
}

char &LString::operator[](size_t index)
{
	return mStdStr[index];
}

const LString::element_t &LString::operator[](size_t index) const
{
	return mStdStr[index];
}

const LString::element_t *LString::operator*() const
{
	return mStdStr.c_str();
}

const luna::LString::element_t *LString::operator*()
{
	return mStdStr.c_str();
}

size_t LString::Hash() const
{
	static auto hasher = std::hash<container_t>();
	return hasher(mStdStr);
}

size_t LString::Find(char const ch, size_t offset/*=0*/) const
{
	return mStdStr.find(ch, offset);
}

LString::wcontainer_t LString::GetStdUnicodeString() const
{
	return StringToWstring(mStdStr);
}

void LString::Replace(const char *str, const char *des)
{
	boost::algorithm::replace_all(mStdStr, str, des);
}

bool LString::StartWith(const char *str) const
{
	return boost::algorithm::starts_with(mStdStr, str);
}
bool LString::StartWith(const char str) const
{
	if (mStdStr.length() > 0)
		return mStdStr[0] == str;
	return false;
}

bool LString::EndsWith(const char *str) const
{
	return mStdStr.ends_with(str);
}

size_t LString::FindLast(const char *str) const
{
	return mStdStr.find_last_of(str);
}

void luna::LString::Replace(const container_t &str)
{
	boost::algorithm::replace_all(mStdStr, str, str);
}

void LString::EraseAt(size_t index)
{
	mStdStr.erase(index);
}

void LString::ReplaceAll(const char *str, const char *dest)
{
	boost::algorithm::replace_all(mStdStr, str, dest);
}

// LString& LString::operator+=(const char* const string_in)
// {
// 	mStdStr += string_in;
// 	return *this;
// }
// LString& LString::operator+=(const ContainerType& string_in)
// {
// 	mStdStr += string_in;
// 	return *this;
// }
//
// LString& LString::operator+=(const wchar_t* const string_in)
// {
// 	WStringDataType now_wstring = StringToWstring(mStdStr);
// 	now_wstring += string_in;
// 	mStdStr = WstringToString(now_wstring);
// 	return *this;
// }

LString operator+(const LString &lValue, const LString &rValue)
{
	LString res;
	res.mStdStr = lValue.mStdStr + rValue.mStdStr;
	return res;
}

LString operator+(const char *lValue, const LString &rValue)
{
	LString res;
	res.mStdStr.assign(lValue);
	res.mStdStr += rValue.mStdStr;
	return res;
}

LString operator+(const LString &lValue, const char *rValue)
{
	LString res(lValue);
	res.Append(rValue);
	return res;
}

LString::wcontainer_t StringToWstring(const LString::container_t &string_in)
{
	LString::wcontainer_t unicode_string;
	int nLen = (int)string_in.length();
	unicode_string.resize(nLen, L' ');
	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)string_in.c_str(), nLen, (LPWSTR)unicode_string.c_str(), nLen);
	return unicode_string;
}
LString::container_t WstringToString(const LString::wcontainer_t &wstring_in)
{
	LString::container_t ascii_string;
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