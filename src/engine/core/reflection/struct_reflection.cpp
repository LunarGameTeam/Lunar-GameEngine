#include"struct_reflection.h"

luna::LResult LReflectNodeBasic::GetSingleEnumValueFromNode(std::string& output_enum_name) const
{
	luna::LResult check_error;
	check_error = LReflectVariableCopySystem::GetInstance()->GetEnumValueFromPointer(m_value_type, output_enum_name, m_data_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
};
luna::LResult LReflectNodeBasic::SetSingleEnumValueToNode(const std::string& input_enum_name)
{
	luna::LResult check_error;
	check_error = LReflectVariableCopySystem::GetInstance()->SetEnumValueToPointer(m_value_type, input_enum_name, m_value_type, m_data_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
luna::LResult LReflectNodeBasic::GetArrayEnumValueFromNode(std::string& output_enum_name, const int32_t& index) const
{
	luna::LResult check_error;
	//先检查最大值是否超过了数组范围
	if (index >= m_array_size)
	{
		LunarDebugLogError(0, "could not get array value from overflow index", check_error);
		return check_error;
	}
	//根据当前节点的类型去进行值转换
	check_error = LReflectVariableCopySystem::GetInstance()->GetEnumArrayValueFromPointerByIndex(m_value_type, output_enum_name, index, m_data_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
luna::LResult LReflectNodeBasic::SetArrayEnumValueToNode(const std::string& output_enum_name, const int32_t& index)
{
	//先检查最大值是否超过了数组范围
	luna::LResult check_error;
	if (index >= m_array_size)
	{
		LunarDebugLogError(0, "could not get array value from overflow index", check_error);
		return check_error;
	}
	//根据当前节点的类型去进行值转换
	check_error = LReflectVariableCopySystem::GetInstance()->SetEnumArrayValueToPointerByIndex(m_value_type,output_enum_name, index, m_data_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
LBasicVariableNode::LBasicVariableNode(
	const size_t& class_type_hash_in,
	void* variable_data_in,
	const size_t& variable_size_in
) :
	m_class_type_hash(class_type_hash_in),
	m_variable_size(variable_size_in),
	m_variable_data(variable_data_in)
{
}
LSerializeType LReflectNodeBasic::GetSerializeType() const
{
	if (m_if_empty_struct_node) 
	{
		LSerializeType parent_node_type;
		parent_node_type.serialize_struct_type = SerializeDataStructType::Serialize_Data_Struct_Node;
		parent_node_type.serialize_type = SerializeNodeType::Serialize_Node_Custom;
		return parent_node_type;
	}
	return LReflectVariableCopySystem::GetInstance()->GetSerializeType(m_value_type);
}

LReflectVariableCopySystem::LReflectVariableCopySystem()
{
	InitNewVariable<double, float>(0.0, SerializeNodeType::Serialize_Node_Float, 0.0f);
	InitNewVariable<float, double>(0.0f, SerializeNodeType::Serialize_Node_Float, 0.0);
	InitNewVariable<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>(0, SerializeNodeType::Serialize_Node_Int, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<int16_t, int8_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>(0, SerializeNodeType::Serialize_Node_Int, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<int32_t, int16_t, int8_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>(0, SerializeNodeType::Serialize_Node_Int, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<int64_t, int32_t, int16_t, int8_t, uint8_t, uint16_t, uint32_t, uint64_t>(0, SerializeNodeType::Serialize_Node_Int, 0, 0, 0, 0, 0, 0, 0);

	InitNewVariable<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(0, SerializeNodeType::Serialize_Node_Uint, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<uint16_t, uint8_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(0, SerializeNodeType::Serialize_Node_Uint, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<uint32_t, uint16_t, uint8_t, uint64_t, int8_t, int16_t, int32_t, int64_t>(0, SerializeNodeType::Serialize_Node_Uint, 0, 0, 0, 0, 0, 0, 0);
	InitNewVariable<uint64_t, uint32_t, uint16_t, uint8_t, int8_t, int16_t, int32_t, int64_t>(0, SerializeNodeType::Serialize_Node_Uint, 0, 0, 0, 0, 0, 0, 0);

	InitNewVariable<bool>(false, SerializeNodeType::Serialize_Node_Bool);
	InitNewVariable<std::string>("", SerializeNodeType::Serialize_Node_String);
	InitNewVariable<boost::container::string>("", SerializeNodeType::Serialize_Node_String);
}
bool LReflectVariableCopySystem::GetTypeByArrayType(const size_t& type_id_in, size_t& type_id_out)
{
	auto type_find = all_variable_array_type.find(type_id_in);
	if (type_find == all_variable_array_type.end())
	{
		return false;
	}
	type_id_out = type_find->second.node_variable_type;
	return true;
}
bool LReflectVariableCopySystem::GetDynamicSizeByArrayType(const size_t& type_id_in, const void* array_data, size_t& dynamic_size_out)
{
	auto type_find = m_dynamic_size_func_get.find(type_id_in);
	if (type_find == m_dynamic_size_func_get.end())
	{
		return false;
	}
	dynamic_size_out = type_find->second(array_data);
	return true;
}
luna::LResult LReflectVariableCopySystem::GetEnumValueFromName(const size_t& enum_type_id, const std::string& enum_name, LEnumMidType& enum_value)
{
	//获取枚举的类名和值表
	auto now_enum_value_map = enum_variable_list.find(enum_type_id);
	if (now_enum_value_map == enum_variable_list.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find enum type of" + enum_name, error_message);
		return error_message;
	}
	//获取枚举的值
	auto now_enum_value_data = now_enum_value_map->second.find(enum_name);
	if (now_enum_value_data == now_enum_value_map->second.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find enum value " + enum_name, error_message);
		return error_message;
	}
	enum_value = now_enum_value_data->second;
	return luna::g_Succeed;
}
luna::LResult LReflectVariableCopySystem::GetEnumNameFromValue(const size_t& enum_type_id, std::string& enum_name, const LEnumMidType& enum_value)
{
	//获取枚举的类名和变量名
	auto now_enum_name_table = enum_name_list.find(enum_type_id);
	if (now_enum_name_table == enum_name_list.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find enum type of "+ enum_name, error_message);
		return error_message;
	}
	auto now_enum_name = now_enum_name_table->second.find(enum_value);
	if (now_enum_name == now_enum_name_table->second.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find enum value " + std::to_string(enum_value), error_message);
		return error_message;
	}
	enum_name = now_enum_name->second;
	return luna::g_Succeed;
}
luna::LResult LReflectVariableCopySystem::SetEnumValueToPointer(const size_t& enum_type_id, const std::string& enum_name, const size_t deal_value_type_in, void* pointer_data)
{
	luna::LResult check_error;
	//将字符串转换为枚举变量
	LEnumMidType enum_value;
	check_error = GetEnumValueFromName(enum_type_id, enum_name, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//根据枚举类型以及枚举的值，将枚举变量写入指针
	check_error = SetValueToPointer(deal_value_type_in, enum_value, pointer_data);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
};
luna::LResult LReflectVariableCopySystem::GetEnumValueFromPointer(const size_t& enum_type_id, std::string& enum_name, const void* pointer_data)
{
	luna::LResult check_error;
	//根据枚举类型获取枚举的值
	LEnumMidType enum_value;
	check_error = GetValueFromPointer(enum_type_id, enum_value, pointer_data);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//将枚举的值转换为字符串
	check_error = GetEnumNameFromValue(enum_type_id, enum_name, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
};
luna::LResult LReflectVariableCopySystem::SetEnumArrayValueToPointerByIndex(const size_t enum_type_id, const std::string& enum_name, const size_t& array_index, void* pointer_data)
{
	luna::LResult check_error;

	size_t now_enum_member_type;
	bool if_succeed = GetTypeByArrayType(enum_type_id, now_enum_member_type);
	if (!if_succeed) 
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find type of enum" + enum_name, error_message);
		return error_message;
	}
	//将字符串转换为枚举变量
	LEnumMidType enum_value;
	check_error = GetEnumValueFromName(now_enum_member_type, enum_name, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//根据枚举类型以及枚举的值，将枚举变量写入指针
	check_error = SetArrayValueToPointerByIndex(enum_type_id, array_index, pointer_data, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
luna::LResult LReflectVariableCopySystem::GetEnumArrayValueFromPointerByIndex(const size_t enum_type_id, std::string& enum_name, const size_t& array_index, const void* pointer_data)
{
	luna::LResult check_error;
	//根据枚举类型获取枚举的值
	LEnumMidType enum_value;
	check_error = GetArrayValueFromPointerByIndex(enum_type_id, array_index, pointer_data, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//获取枚举的个体类型
	size_t deal_value_member_type;
	bool if_succeed = GetTypeByArrayType(enum_type_id, deal_value_member_type);
	if (!if_succeed)
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "haven't init array type or type is not array", error_message);
		return error_message;
	}
	//将枚举的值转换为字符串
	check_error = GetEnumNameFromValue(deal_value_member_type, enum_name, enum_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
LSerializeType LReflectVariableCopySystem::GetSerializeType(const size_t& index_in)
{
	auto now_value = all_variable_desc.find(index_in);
	if (now_value == all_variable_desc.end())
	{
		LSerializeType new_type;
		new_type.serialize_struct_type = SerializeDataStructType::Serialize_Data_Struct_Unknow;
		new_type.serialize_type = SerializeNodeType::Serialize_Node_Unknow;
		return new_type;
	}
	return now_value->second;
}
LReflectVariableCopySystem::~LReflectVariableCopySystem()
{
	for (auto now_remove_variable = all_variable_init.begin(); now_remove_variable != all_variable_init.end(); ++now_remove_variable) 
	{
		delete now_remove_variable->second;
	}
	all_variable_init.clear();
}
void LSrtructReflect::Create()
{
	InitBasicVariable();
	BindStaticArraySize();
	BuildChildValueMap();
}
const std::string LSrtructReflect::GetParentName(const std::string& name_in)
{
	int32_t parent_value_tail = static_cast<int32_t>(name_in.find_last_of('.'));
	int32_t parent_pointer_tail = static_cast<int32_t>(name_in.rfind("->"));
	auto max_value = max(parent_value_tail, parent_pointer_tail);
	if (max_value <= 0)
	{
		return "";
	}
	return name_in.substr(0, max_value);
}
void LSrtructReflect::DivideStringForReflect(const char* name_input, std::vector<std::string>& data_out)
{
	std::string mid_str;
	size_t now_string_size = strlen(name_input);
	for (size_t now_char_id = 0; now_char_id < now_string_size; ++now_char_id) 
	{
		if (name_input[now_char_id] == ',') 
		{
			if (mid_str != "") 
			{
				data_out.push_back(mid_str);
			}
			mid_str = "";
		}
		else if (name_input[now_char_id] != ' ') 
		{
			mid_str += name_input[now_char_id];
		}
	}
	if (mid_str != "") 
	{
		data_out.push_back(mid_str);
	}
}
luna::LResult LSrtructReflect::AddReflectData(const std::string& node_name, LReflectNodeBasic& node_data)
{
	if (name_to_id_map.find(node_name) != name_to_id_map.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "repeat init variable",error_message);
		return error_message;
	}
	//先创建当前节点
	int32_t now_used_id = m_value_self_add;
	name_map.insert(std::pair<int32_t, std::string>(now_used_id, node_name));
	name_to_id_map.insert(std::pair<std::string, int32_t>(node_name, now_used_id));
	value_map.insert(std::pair<int32_t, LReflectNodeBasic>(now_used_id, node_data));
	m_value_self_add += 1;
	//检查该节点是否有父节点并一并创建
	std::string parent_name = GetParentName(node_name);
	if (parent_name == "")
	{
		parent_map.insert(std::pair<int32_t, int32_t>(now_used_id, -1));
		return luna::g_Succeed;
	}
	auto parent_find = name_to_id_map.find(parent_name);
	if (parent_find != name_to_id_map.end())
	{
		parent_map.insert(std::pair<int32_t, int32_t>(now_used_id, parent_find->second));
	}
	else
	{
		//父节点不存在，需要创建父节点
		int32_t now_pre_id = now_used_id;
		int32_t now_next_id = m_value_self_add;
		m_value_self_add += 1;
		std::string next_parent_name = parent_name;
		bool if_root_not_create = true;
		while (next_parent_name != "")
		{
			//未找到父节点，递归创建父节点
			LReflectNodeBasic empty_node(next_parent_name, now_next_id);
			name_map.insert(std::pair<int32_t, std::string>(now_next_id, next_parent_name));
			name_to_id_map.insert(std::pair<std::string, int32_t>(next_parent_name, now_next_id));
			value_map.insert(std::pair<int32_t, LReflectNodeBasic>(now_next_id, empty_node));
			parent_map.insert(std::pair<int32_t, int32_t>(now_pre_id, now_next_id));
			//创建完毕，开始寻找上一级的父节点
			now_pre_id = now_next_id;
			m_value_self_add += 1;
			now_next_id = m_value_self_add;
			next_parent_name = GetParentName(next_parent_name);
			//检查接下来要创建的父节点是否存在
			auto check_parent_index = name_to_id_map.find(next_parent_name);
			if (check_parent_index != name_to_id_map.end())
			{
				//上一级的父节点已经存在了
				parent_map.insert(std::pair<int32_t, int32_t>(now_pre_id, check_parent_index->second));
				if_root_not_create = false;
				break;
			}
		}
		if (if_root_not_create)
		{
			parent_map.insert(std::pair<int32_t, int32_t>(now_pre_id, -1));
		}
	}
	return luna::g_Succeed;
}
//从json中获取数据
luna::LResult LSrtructReflect::LoadFromJsonFile(const std::string& Json_file)
{
	ClearMemory();
	Json::Value jsonRoot;
	auto check_error = PancyJsonTool::GetInstance()->LoadJsonFile(Json_file, jsonRoot);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return LoadFromJsonNode("reflect_data", Json_file, jsonRoot);
}
luna::LResult LSrtructReflect::LoadFromJsonMemory(const std::string& value_name, const Json::Value& root_value)
{
	ClearMemory();
	return LoadFromJsonNode("reflect_data", value_name, root_value);
}
luna::LResult LSrtructReflect::LoadFromJsonNode(const std::string& parent_name, const std::string& value_name, const Json::Value& root_value)
{
	luna::LResult check_error;
	std::vector<std::string> all_member_name = root_value.getMemberNames();
	std::vector<Json::ValueType> check_value;
	for (int member_index = 0; member_index < all_member_name.size(); ++member_index)
	{
		//获取子节点的json数据
		Json::Value now_child_value = root_value.get(all_member_name[member_index], Json::Value::null);
		//查找对应节点的反射信息
		std::string now_combine_value_common = parent_name;//通过.链接的结构体信息
		std::string now_combine_value_pointer = parent_name;//通过->链接的结构体指针信息
		if (parent_name != "")
		{
			now_combine_value_common += ".";
			now_combine_value_pointer += "->";
		}
		now_combine_value_common += all_member_name[member_index];
		now_combine_value_pointer += all_member_name[member_index];
		//存储当前节点的真正的名称，由于有些node依靠.来标记子节点，而有些需要依靠->标记
		auto now_reflect_index = name_to_id_map.find(now_combine_value_common);
		std::string now_node_name = now_combine_value_common;
		if (now_reflect_index == name_to_id_map.end())
		{
			now_reflect_index = name_to_id_map.find(now_combine_value_pointer);
			if (now_reflect_index == name_to_id_map.end())
			{
				//未找到对应的反射数据
				luna::LResult error_message;
				LunarDebugLogError(E_FAIL, "could not find JSON reflect variable: " + now_combine_value_common + " while load json node " + value_name, error_message);
				return error_message;
			}
			now_node_name = now_combine_value_pointer;
		}
		auto now_reflect_data = value_map.find(now_reflect_index->second);
		auto json_data_type = now_child_value.type();
		//根据反射数据的类型，决定数据的载入方式
		switch (json_data_type)
		{
		case Json::nullValue:
		{
			//损坏的数据
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the variable: " + all_member_name[member_index] + " could not be recognized by json tool", error_message);
			return error_message;
		}
		break;
		case Json::intValue:
		{
			int64_t int_data = now_child_value.asInt64();
			check_error = now_reflect_data->second.SetSingleValueToNode(int_data);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::uintValue:
		{
			uint64_t uint_data = now_child_value.asUInt64();
			check_error = now_reflect_data->second.SetSingleValueToNode(uint_data);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::realValue:
		{
			double double_value = now_child_value.asDouble();
			check_error = now_reflect_data->second.SetSingleValueToNode(double_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::stringValue:
		{
			std::string string_value = now_child_value.asString();
			check_error = SetStringValue(now_reflect_data->second, string_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::booleanValue:
		{
			bool bool_value = now_child_value.asBool();
			check_error = now_reflect_data->second.SetSingleValueToNode(bool_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::arrayValue:
		{
			check_error = LoadFromJsonArray(now_reflect_index->second, now_child_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		case Json::objectValue:
		{
			//节点数据
			check_error = LoadFromJsonNode(now_node_name, value_name, now_child_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		break;
		default:
			break;
		}

	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::LoadFromJsonArray(const int32_t& value_index, const Json::Value& root_value)
{
	auto now_reflect_data = value_map.find(value_index);
	if (now_reflect_data == value_map.end())
	{
		//未找到对应的反射数据
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find JSON reflect variable: " + name_map[value_index], error_message);
		return error_message;
	}
	return SetArrayValue(now_reflect_data->second, root_value);
}
luna::LResult LSrtructReflect::SetArrayValue(LReflectNodeBasic& reflect_data, const Json::Value& now_child_value)
{
	luna::LResult check_error;
	//数组变量对应的大小变量不需要进行存储(json中会有额外的变量映射过来)
	for (Json::ArrayIndex array_index = 0; array_index < now_child_value.size(); ++array_index)
	{
		//先检查数组是否越界
		if (array_index >= reflect_data.GetArraySize())
		{
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "the array variable: " + reflect_data.GetName() + " size only have " + std::to_string(reflect_data.GetArraySize()) + " could not get index" + std::to_string(array_index), error_message);
			return error_message;
		}
		switch (now_child_value[array_index].type())
		{
		case Json::intValue:
		{
			check_error = reflect_data.SetArrayValueToNode(now_child_value[array_index].asInt64(), array_index);
		}
		break;
		case Json::uintValue:
		{
			check_error = reflect_data.SetArrayValueToNode(now_child_value[array_index].asUInt64(), array_index);
		}
		break;
		case Json::realValue:
		{
			check_error = reflect_data.SetArrayValueToNode(now_child_value[array_index].asDouble(), array_index);
		}
		break;
		case Json::booleanValue:
		{
			check_error = reflect_data.SetArrayValueToNode(now_child_value[array_index].asBool(), array_index);
		}
		break;
		case Json::stringValue:
		{
			check_error = SetStringArrayValue(reflect_data, now_child_value[array_index].asString(), array_index);
		}
		break;
		case Json::objectValue:
		{
			//节点数组
			check_error = SetNodeArrayValue(reflect_data, array_index, now_child_value[array_index]);
		}
		break;
		default:
			break;
		}
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SetNodeArrayValue(LReflectNodeBasic& reflect_data, const int32_t& offset_value, const Json::Value& now_child_value)
{
	luna::LResult check_error;
	//根据节点的类型创建一个新的处理类来解析节点数据
	auto child_reflect_pointer = LSrtructReflectControl::GetInstance()->GetJsonReflectByArray(reflect_data.GetValueType());
	if (child_reflect_pointer == NULL)
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find struct node reflect: " + reflect_data.GetName(), error_message);

		return error_message;
	}
	check_error = child_reflect_pointer->LoadFromJsonMemory(reflect_data.GetName() + "array:" + std::to_string(offset_value), now_child_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	child_reflect_pointer->CopyValueToArrayData(reflect_data, offset_value);
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SetStringArrayValue(LReflectNodeBasic& reflect_data, const std::string& string_value, const int32_t &index)
{
	luna::LResult check_error;
	switch (reflect_data.GetSerializeType().serialize_type)
	{
	case SerializeNodeType::Serialize_Node_String:
	{
		//普通的字符串
		reflect_data.SetArrayValueToNode(string_value, index);
	}
	break;
	case SerializeNodeType::Serialize_Node_Enum:
	{
		check_error = reflect_data.SetArrayEnumValueToNode(string_value, index);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	break;
	default:
		luna::LResult error_message;
		LunarDebugLogError(0, "the variable: " + reflect_data.GetName() + " is not a string value", error_message);
		return error_message;
		break;
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SetStringValue(LReflectNodeBasic& reflect_data, const std::string& string_value)
{
	luna::LResult check_error;
	switch (reflect_data.GetSerializeType().serialize_type)
	{
	case SerializeNodeType::Serialize_Node_String:
	{
		//普通的字符串
		reflect_data.SetSingleValueToNode(string_value);
	}
	break;
	case SerializeNodeType::Serialize_Node_Enum:
	{
		check_error = reflect_data.SetSingleEnumValueToNode(string_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	break;
	default:
		luna::LResult error_message;
		LunarDebugLogError(0, "the variable: " + reflect_data.GetName() + " is not a string value", error_message);
		return error_message;
		break;
	}
	return luna::g_Succeed;
}
void LSrtructReflect::BuildChildValueMap() 
{
	for (auto now_member_value = parent_map.begin(); now_member_value != parent_map.end(); ++now_member_value)
	{
		child_value_list.emplace(now_member_value->first, std::vector<int32_t>());
	}
	for (auto now_member_value = parent_map.begin(); now_member_value != parent_map.end(); ++now_member_value)
	{
		if (now_member_value->second != -1)
		{
			child_value_list[now_member_value->second].push_back(now_member_value->first);
		}
	}
}
//存储到json
luna::LResult LSrtructReflect::SaveToJsonFile(const std::string& json_name)
{
	Json::Value file_value;
	auto check_error = SaveToJsonMemory(file_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = PancyJsonTool::GetInstance()->WriteValueToJson(file_value, json_name);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SaveToJsonMemory(Json::Value& root_value)
{
	auto child_value_member = name_to_id_map.find("reflect_data");
	if (child_value_member == name_to_id_map.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find JSON reflect variable: reflect_data", error_message);

		return error_message;
	}
	auto now_root_child_list = child_value_list.find(child_value_member->second);
	if (now_root_child_list == child_value_list.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find JSON reflect variable: reflect_data", error_message);

		return error_message;
	}
	luna::LResult check_error;
	for (int32_t child_value_index = 0; child_value_index < now_root_child_list->second.size(); ++child_value_index)
	{
		auto now_child_id = now_root_child_list->second[child_value_index];
		check_error = SaveToJsonNode(now_child_id, root_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SaveToJsonNode(const int32_t& now_value_index, Json::Value& root_value)
{
	luna::LResult check_error;
	auto now_reflect_data = value_map.find(now_value_index);
	if (now_reflect_data == value_map.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find JSON reflect variable: " + name_map[now_value_index], error_message);
		return error_message;
	}
	auto now_serialize_type = now_reflect_data->second.GetSerializeType();
	if (now_serialize_type.serialize_struct_type == SerializeDataStructType::Serialize_Data_Struct_Node)
	{
		check_error = SaveSingleValueMemberToJson(now_reflect_data->second, root_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	else if (now_serialize_type.serialize_struct_type == SerializeDataStructType::Serialize_Data_Struct_Array)
	{
		check_error = SaveArrayValueMemberToJson(now_reflect_data->second, root_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	else
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not recognize JSON reflect variable: " + name_map[now_value_index], error_message);
		return error_message;
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SaveSingleValueMemberToJson(const LReflectNodeBasic& reflect_data, Json::Value& root_value)
{
	std::string real_name_node = TranslateFullNameToRealName(reflect_data.GetName());
	switch (reflect_data.GetSerializeType().serialize_type)
	{
	case SerializeNodeType::Serialize_Node_Int:
	{
		int64_t now_data;
		reflect_data.GetSingleValueFromNode(now_data);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, now_data);
		break;
	}
	case SerializeNodeType::Serialize_Node_Uint:
	{
		uint64_t mid_data;
		reflect_data.GetSingleValueFromNode(mid_data);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, mid_data);
		break;
	}
	case SerializeNodeType::Serialize_Node_Float:
	{
		double mid_data;
		reflect_data.GetSingleValueFromNode(mid_data);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, mid_data);
		break;
	}
	case SerializeNodeType::Serialize_Node_Bool:
	{
		bool mid_data;
		reflect_data.GetSingleValueFromNode(mid_data);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, mid_data);
		break;
	}
	case SerializeNodeType::Serialize_Node_String:
	{
		std::string mid_data;
		reflect_data.GetSingleValueFromNode(mid_data);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, mid_data);
		break;
	}
	case SerializeNodeType::Serialize_Node_Enum:
	{
		std::string enum_value_name;
		luna::LResult check_error = reflect_data.GetSingleEnumValueFromNode(enum_value_name);
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, real_name_node, enum_value_name);
		break;
	}
	case SerializeNodeType::Serialize_Node_Custom:
	{
		auto child_value_member = child_value_list.find(reflect_data.GetID());
		if (child_value_member == child_value_list.end())
		{
			luna::LResult error_message;
			LunarDebugLogError(E_FAIL, "could not find JSON reflect variable: " + reflect_data.GetName(), error_message);
			return error_message;
		}
		Json::Value struct_root_value;
		for (int32_t child_value_index = 0; child_value_index < child_value_member->second.size(); ++child_value_index)
		{
			int32_t now_child_index = child_value_member->second[child_value_index];
			Json::Value child_root_value;
			auto check_error = SaveToJsonNode(now_child_index, struct_root_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		PancyJsonTool::GetInstance()->SetJsonValue(root_value, TranslateFullNameToRealName(reflect_data.GetName()), struct_root_value);
		break;
	}
	default:
	{
		luna::LResult error_message;

		LunarDebugLogError(E_FAIL, "could not recognize JSON reflect type: " + reflect_data.GetName(), error_message);
		return error_message;
	}
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::SaveArrayValueMemberToJson(const LReflectNodeBasic& reflect_data, Json::Value& root_value)
{
	luna::LResult check_error;
	std::string real_name_node = TranslateFullNameToRealName(reflect_data.GetName());
	size_t now_array_size;
	check_error = GetArrayDataSize(reflect_data, now_array_size);
	for (int32_t now_index_array = 0; now_index_array < static_cast<int32_t>(now_array_size); ++now_index_array)
	{
		switch (reflect_data.GetSerializeType().serialize_type)
		{
		case SerializeNodeType::Serialize_Node_Int:
		{
			int64_t mid_data;
			reflect_data.GetArrayValueFromNode(mid_data, now_index_array);
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, mid_data);
			break;
		}
		case SerializeNodeType::Serialize_Node_Uint:
		{
			uint64_t mid_data;
			reflect_data.GetArrayValueFromNode(mid_data, now_index_array);
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, mid_data);
			break;
		}
		case SerializeNodeType::Serialize_Node_Float:
		{
			double mid_data;
			reflect_data.GetArrayValueFromNode(mid_data, now_index_array);
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, mid_data);
			break;
		}
		case SerializeNodeType::Serialize_Node_Bool:
		{
			bool mid_data;
			reflect_data.GetArrayValueFromNode(mid_data, now_index_array);
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, mid_data);
			break;
		}
		case SerializeNodeType::Serialize_Node_String:
		{
			std::string mid_data;
			reflect_data.GetArrayValueFromNode(mid_data, now_index_array);
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, mid_data);
			break;
		}
		case SerializeNodeType::Serialize_Node_Enum:
		{
			std::string enum_value_name;
			check_error = reflect_data.GetArrayEnumValueFromNode(enum_value_name, now_index_array);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, enum_value_name);
			break;
		}
		case SerializeNodeType::Serialize_Node_Custom:
		{
			auto child_reflect_pointer = LSrtructReflectControl::GetInstance()->GetJsonReflectByArray(reflect_data.GetValueType());
			if (child_reflect_pointer == NULL)
			{
				luna::LResult error_message;
				LunarDebugLogError(E_FAIL, "could not find struct node reflect: " + reflect_data.GetName(), error_message);
				return error_message;
			}
			check_error = child_reflect_pointer->ResetValueByArrayData(reflect_data, now_index_array);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			Json::Value struct_root_value;
			check_error = child_reflect_pointer->SaveToJsonMemory(struct_root_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			PancyJsonTool::GetInstance()->AddJsonArrayValue(root_value, real_name_node, struct_root_value);
			break;
		}
		default:
		{
			luna::LResult error_message;

			LunarDebugLogError(E_FAIL, "could not recognize JSON reflect type: " + reflect_data.GetName(), error_message);
			return error_message;
		}
		}
	}
	return luna::g_Succeed;
}
luna::LResult LSrtructReflect::GetArrayDataSize(const LReflectNodeBasic& reflect_data, size_t& size_out)
{
	luna::LResult check_error;
	auto array_dynamic_size_variable = static_array_size_used_map.find(reflect_data.GetID());
	if (array_dynamic_size_variable != static_array_size_used_map.end())
	{
		//有动态大小标记量，则直接从标记量里取值
		size_t dynamic_size = 0;
		check_error = value_map[array_dynamic_size_variable->second].GetSingleValueFromNode(dynamic_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		size_out = dynamic_size;
	}
	else
	{
		//无动态大小标记量，则探查其动态大小
		size_out = reflect_data.GetDynamicArraySize();
		if (size_out == gMaxDynamicArraySize)
		{
			luna::LResult error_message;
			LunarDebugLogError(0, "could not find array size of variable: " + reflect_data.GetName(), error_message);
			return error_message;
		}
	}
	return luna::g_Succeed;
}
std::string LSrtructReflect::TranslateFullNameToRealName(const std::string& full_name)
{
	size_t self_offset = 1;
	int32_t offset_value = static_cast<int32_t>(full_name.rfind("."));
	int32_t offset_pointer = static_cast<int32_t>(full_name.rfind("->"));
	if (offset_value < offset_pointer)
	{
		self_offset = 2;
		offset_value = offset_pointer;
	}
	std::string real_name;
	if (offset_value != -1)
	{
		real_name = full_name.substr(self_offset + offset_value, real_name.size() - (self_offset + offset_value));
	}
	else
	{
		real_name = full_name;
	}
	return real_name;
}

LSrtructReflectControl::LSrtructReflectControl()
{
}
LSrtructReflectControl::~LSrtructReflectControl()
{
	for (auto reflect_parse_object = reflect_map.begin(); reflect_parse_object != reflect_map.end(); ++reflect_parse_object)
	{
		delete reflect_parse_object->second;
	}
	reflect_map.clear();
}
bool LSrtructReflectControl::CheckIfStructArrayInit(const size_t& class_id)
{
	size_t member_type;
	bool if_succeed = LReflectVariableCopySystem::GetInstance()->GetTypeByArrayType(class_id, member_type);
	if (!if_succeed)
	{
		return false;
	}
	return CheckIfStructMemberInit(member_type);
}
bool LSrtructReflectControl::CheckIfStructMemberInit(const size_t& class_id)
{
	auto now_reflect_data = reflect_map.find(class_id);
	if (now_reflect_data == reflect_map.end())
	{
		return false;
	}
	return true;
}
LSrtructReflect* LSrtructReflectControl::GetJsonReflectByArray(const size_t& class_id)
{
	size_t member_type;
	bool if_succeed = LReflectVariableCopySystem::GetInstance()->GetTypeByArrayType(class_id, member_type);
	if (!if_succeed)
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "class haven't init to reflect class", error_message);

		return NULL;
	}
	return GetJsonReflect(member_type);
}
LSrtructReflect* LSrtructReflectControl::GetJsonReflect(const size_t& class_id)
{
	auto now_reflect_data = reflect_map.find(class_id);
	if (now_reflect_data == reflect_map.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "class haven't init to reflect class", error_message);

		return NULL;
	}
	return now_reflect_data->second;
}