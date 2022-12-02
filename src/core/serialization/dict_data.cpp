#include "dict_data.h"

namespace luna
{

namespace detail
{
Json::Value nullValue;


LVector3f default_value<LVector3f>::value = LVector3f();
LVector2f default_value<LVector2f>::value = LVector2f();
LQuaternion default_value<LQuaternion>::value = LQuaternion();
const Dictionary default_value<Dictionary>::value = Dictionary(nullValue);
const List default_value<List>::value = List(nullValue);

}

Dictionary List::GetDict(int index)
{
	Json::Value &val = mValue[index];
	Dictionary res(val);
	detail::convert_from_value(val, res);
	return res;
}

luna::Dictionary Dictionary::GetDict(const LString &key)
{
	Json::Value &val = mValue[key.c_str()];
	Dictionary res(val);
	detail::convert_from_value(val, res);
	return res;
}

luna::List Dictionary::GetList(const LString &key)
{
	Json::Value &val = mValue[key.c_str()];
	List res(val);
	detail::convert_from_value(val, res);
	return res;
}

std::string Dictionary::ToString()
{
	Json::FastWriter w;
	std::string str = w.write(mValue);
	return str;
}

}