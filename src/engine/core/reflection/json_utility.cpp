#include"json_utility.h"
PancyJsonTool::PancyJsonTool()
{
	builder["collectComments"] = false;
	std::string name_value_need[] = {
		"nullValue"
		"intValue",
		"uintValue",
		"realValue",
		"stringValue",
		"booleanValue",
		"arrayValue",
		"objectValue"
	};
	for (int i = 0; i < 7; ++i)
	{
		name_value_type[i] = name_value_need[i];
	}
	InitBasicType();
}
PancyJsonTool::~PancyJsonTool() 
{
}
luna::LResult PancyJsonTool::LoadJsonFile(const std::string &file_name, Json::Value &root_value)
{
	FileOpen.open(file_name);
	if (!FileOpen.is_open())
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not open json file " + file_name,error_message);
		
		return error_message; 
	}
	root_value.clear();
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, FileOpen, &root_value, &errs))
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, errs.c_str(),error_message);
		
		return error_message;
	}
	FileOpen.close();
	return luna::g_Succeed;
}
luna::LResult PancyJsonTool::GetJsonData
(
	const std::string &file_name,
	const Json::Value &root_value,
	const std::string &member_name,
	const pancy_json_data_type &json_type,
	pancy_json_value &variable_value
)
{
	auto enum_type_value = root_value.get(member_name, Json::Value::null);
	return GetJsonMemberData(file_name, enum_type_value, member_name, json_type, variable_value);
}
luna::LResult PancyJsonTool::GetJsonData
(
	const std::string &file_name,
	const Json::Value &root_value,
	const int32_t &member_num,
	const pancy_json_data_type &json_type,
	pancy_json_value &variable_value
)
{
	auto enum_type_value = root_value[member_num];
	return GetJsonMemberData(file_name, enum_type_value, "array::" + std::to_string(member_num), json_type, variable_value);
}
luna::LResult PancyJsonTool::GetJsonMemberData
(
	const std::string &file_name,
	const Json::Value &enum_type_value,
	const std::string &member_name,
	const pancy_json_data_type &json_type,
	pancy_json_value &variable_value
)
{
	if (enum_type_value == Json::Value::null)
	{
		//未能获得json数据
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find value of variable " + member_name,error_message);
		
		return error_message;
	}
	//整数数据
	else if (json_type == pancy_json_data_type::json_data_int)
	{
		if (enum_type_value.type() != Json::ValueType::intValue && enum_type_value.type() != Json::ValueType::uintValue)
		{
			int now_type_name = static_cast<int32_t>(enum_type_value.type());
			//json数据对应的类型不是整数类型
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the value of variable " + member_name + " need int but find " + name_value_type[now_type_name],error_message);
			
			return error_message;
		}
		if (enum_type_value.type() == Json::ValueType::intValue)
		{
			variable_value.int_value = enum_type_value.asInt();
		}
		else if (enum_type_value.type() == Json::ValueType::uintValue)
		{
			variable_value.int_value = static_cast<int32_t>(enum_type_value.asUInt());
		}
	}
	//浮点数数据
	else if (json_type == pancy_json_data_type::json_data_float)
	{
		if (enum_type_value.type() != Json::ValueType::realValue)
		{
			int now_type_name = static_cast<int32_t>(enum_type_value.type());
			//json数据对应的类型不是浮点类型
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the value of variable " + member_name + " need float but find " + name_value_type[now_type_name],error_message);
			
			return error_message;
		}
		variable_value.float_value = enum_type_value.asFloat();
	}
	//字符串数据
	else if (json_type == pancy_json_data_type::json_data_string)
	{
		if (enum_type_value.type() != Json::ValueType::stringValue)
		{
			int now_type_name = static_cast<int32_t>(enum_type_value.type());
			//json数据对应的类型不是浮点类型
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the value of variable " + member_name + " need string but find " + name_value_type[now_type_name],error_message);
			
			return error_message;
		}
		variable_value.string_value = enum_type_value.asCString();
	}
	//bool数据
	else if (json_type == pancy_json_data_type::json_data_bool)
	{
		if (enum_type_value.type() != Json::ValueType::booleanValue)
		{
			int now_type_name = static_cast<int32_t>(enum_type_value.type());
			//json数据对应的类型不是浮点类型
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the value of variable " + member_name + " need bool but find " + name_value_type[now_type_name],error_message);
			
			return error_message;
		}
		variable_value.bool_value = enum_type_value.asBool();
	}
	else 
	{
		//未能获得json数据
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not parse value of variable " + member_name,error_message);
		
		return error_message;
	}
	return luna::g_Succeed;
}
void PancyJsonTool::SplitString(std::string str, const std::string &pattern, std::vector<std::string> &result)
{
	std::string::size_type pos;
	str += pattern;//扩展字符串以方便操作
	size_t size = str.size();
	for (size_t i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
}
luna::LResult PancyJsonTool::WriteValueToJson(
	const Json::Value &insert_value,
	const std::string &Json_name
)
{
	std::string save_file_name = Json_name;
	bool if_json_tail = false;
	if (Json_name.size() >= 5)
	{
		std::string tail_name = Json_name.substr(Json_name.size() - 5,5);
		if (tail_name == ".json")
		{
			if_json_tail = true;
		}
	}
	if (!if_json_tail)
	{
		save_file_name += ".json";
	}
	std::unique_ptr<Json::StreamWriter> writer(Jwriter.newStreamWriter());
	FileWrite.open(save_file_name);
	writer->write(insert_value,&FileWrite);
	FileWrite.close();
	return luna::g_Succeed;
}
void PancyJsonTool::InitBasicType()
{
	//注册数组类型的数据
	json_type_map[typeid(int64_t*).hash_code()] = json_member_int64_array;
	json_type_map[typeid(uint64_t*).hash_code()] = json_member_uint64_array;
	json_type_map[typeid(int32_t*).hash_code()] = json_member_int32_array;
	json_type_map[typeid(uint32_t*).hash_code()] = json_member_uint32_array;
	json_type_map[typeid(int16_t*).hash_code()] = json_member_int16_array;
	json_type_map[typeid(uint16_t*).hash_code()] = json_member_uint16_array;
	json_type_map[typeid(int8_t*).hash_code()] = json_member_int8_array;
	json_type_map[typeid(uint8_t*).hash_code()] = json_member_uint8_array;
	json_type_map[typeid(float*).hash_code()] = json_member_float_array;
	json_type_map[typeid(double*).hash_code()] = json_member_double_array;
	json_type_map[typeid(bool*).hash_code()] = json_member_bool_array;
	json_type_map[typeid(std::string*).hash_code()] = json_member_string_array;
	//注册普通类型的数据
	json_type_map[typeid(int64_t).hash_code()] = json_member_int64;
	json_type_map[typeid(uint64_t).hash_code()] = json_member_uint64;
	json_type_map[typeid(int32_t).hash_code()] = json_member_int32;
	json_type_map[typeid(uint32_t).hash_code()] = json_member_uint32;
	json_type_map[typeid(int16_t).hash_code()] = json_member_int16;
	json_type_map[typeid(uint16_t).hash_code()] = json_member_uint16;
	json_type_map[typeid(int8_t).hash_code()] = json_member_int8;
	json_type_map[typeid(uint8_t).hash_code()] = json_member_uint8;
	json_type_map[typeid(float).hash_code()] = json_member_float;
	json_type_map[typeid(double).hash_code()] = json_member_double;
	json_type_map[typeid(bool).hash_code()] = json_member_bool;
	json_type_map[typeid(std::string).hash_code()] = json_member_string;
	//注册vector类型的数据
	json_type_map[typeid(std::vector<int64_t>).hash_code()] = json_member_int64_list;
	json_type_map[typeid(std::vector<uint64_t>).hash_code()] = json_member_uint64_list;
	json_type_map[typeid(std::vector<int32_t>).hash_code()] = json_member_int32_list;
	json_type_map[typeid(std::vector<uint32_t>).hash_code()] = json_member_uint32_list;
	json_type_map[typeid(std::vector<int16_t>).hash_code()] = json_member_int16_list;
	json_type_map[typeid(std::vector<uint16_t>).hash_code()] = json_member_uint16_list;
	json_type_map[typeid(std::vector<int8_t>).hash_code()] = json_member_int8_list;
	json_type_map[typeid(std::vector<uint8_t>).hash_code()] = json_member_uint8_list;
	json_type_map[typeid(std::vector<float>).hash_code()] = json_member_float_list;
	json_type_map[typeid(std::vector<double>).hash_code()] = json_member_double_list;
	json_type_map[typeid(std::vector<bool>).hash_code()] = json_member_bool_list;
	json_type_map[typeid(std::vector<std::string>).hash_code()] = json_member_string_list;
}
PancyJsonMemberType PancyJsonTool::GetVariableJsonType(const size_t &variable_type)
{
	auto check_variable_type = json_type_map.find(variable_type);
	if (check_variable_type == json_type_map.end())
	{
		return PancyJsonMemberType::json_member_unknown;
	}
	return check_variable_type->second;
}