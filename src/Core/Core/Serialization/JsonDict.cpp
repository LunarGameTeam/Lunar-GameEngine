#include "Core/Serialization/JsonDict.h"

namespace luna
{

namespace detail
{
Json::Value nullValue;


LVector3f default_value<LVector3f>::value = LVector3f();
LVector2f default_value<LVector2f>::value = LVector2f();
LQuaternion default_value<LQuaternion>::value = LQuaternion();
const JsonDict default_value<JsonDict>::value = JsonDict(nullValue);
const JsonList default_value<JsonList>::value = JsonList(nullValue);

}

JsonDict JsonList::GetDict(int index)
{
	Json::Value &val = mValue[index];
	JsonDict res(val);
	detail::convert_from_value(val, res);
	return res;
}

luna::JsonDict JsonDict::GetDict(const LString &key)
{
	Json::Value &val = mValue[key.c_str()];
	JsonDict res(val);
	detail::convert_from_value(val, res);
	return res;
}

luna::JsonList JsonDict::GetList(const LString &key)
{
	Json::Value &val = mValue[key.c_str()];	
	JsonList res(val);	
	detail::convert_from_value(val, res);
	return res;
}

std::string JsonDict::ToString()
{
	Json::FastWriter w;
	std::string str = w.write(mValue);
	return str;
}

}