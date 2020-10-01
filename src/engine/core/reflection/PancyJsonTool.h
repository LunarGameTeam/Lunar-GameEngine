#pragma once
#include <json\json.h>

#include"core\core_module.h"

#pragma comment(lib,"../../lib/Debug/jsoncpp_static.lib")
using namespace std;
enum pancy_json_data_type
{
	json_data_int = 0,
	json_data_float,
	json_data_enum,
	json_data_string,
	json_data_bool
};
enum Pancy_json_shader_type
{
	json_shader_vertex = 0,
	json_shader_pixel,
	json_shader_geometry,
	json_shader_hull,
	json_shader_domin,
	json_shader_compute
};

struct pancy_json_value
{
	pancy_json_data_type value_type;
	int32_t int_value;
	float float_value;
	bool bool_value;
	std::string string_value;
};
enum PancyJsonMemberType
{
	//未识别的类型
	json_member_unknown = 0,
	//普通变量
	json_member_int8,
	json_member_int16,
	json_member_int32,
	json_member_int64,
	json_member_uint8,
	json_member_uint16,
	json_member_uint32,
	json_member_uint64,
	json_member_float,
	json_member_double,
	json_member_enum,
	json_member_string,
	json_member_bool,
	//链表变量
	json_member_int8_list,
	json_member_int16_list,
	json_member_int32_list,
	json_member_int64_list,
	json_member_uint8_list,
	json_member_uint16_list,
	json_member_uint32_list,
	json_member_uint64_list,
	json_member_float_list,
	json_member_double_list,
	json_member_enum_list,
	json_member_string_list,
	json_member_bool_list,
	//数组变量
	json_member_int8_array,
	json_member_int16_array,
	json_member_int32_array,
	json_member_int64_array,
	json_member_uint8_array,
	json_member_uint16_array,
	json_member_uint32_array,
	json_member_uint64_array,
	json_member_float_array,
	json_member_double_array,
	json_member_enum_array,
	json_member_string_array,
	json_member_bool_array,
	//节点变量
	json_member_node,
	json_member_node_list,
	json_member_node_array
};
class PancyJsonTool
{
	Json::StreamWriterBuilder Jwriter;
	Json::CharReaderBuilder builder;
	ifstream FileOpen;
	ofstream FileWrite;
	//todo:删掉关于shader的特殊反射
	std::string name_value_type[7];
	//json反射相关......
	std::unordered_map<size_t, PancyJsonMemberType> json_type_map;//所有已经被识别的json类型
	std::unordered_map<std::string, std::string> enum_pointer_value_map;//枚举指针对应的基本类型
private:
	PancyJsonTool();
public:
	~PancyJsonTool();
	static PancyJsonTool* GetInstance()
	{
		static PancyJsonTool* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new PancyJsonTool();
		}
		return this_instance;
	}
	//读取json数据
	LunarEngine::LResult LoadJsonFile(const std::string &file_name, Json::Value &root_value);
	LunarEngine::LResult GetJsonData
	(
		const std::string &file_name,
		const Json::Value &root_value,
		const std::string &member_name,
		const pancy_json_data_type &json_type,
		pancy_json_value &variable_value
	);
	LunarEngine::LResult GetJsonData
	(
		const std::string &file_name,
		const Json::Value &root_value,
		const int32_t &member_num,
		const pancy_json_data_type &json_type,
		pancy_json_value &variable_value
	);
	//更改及输出json数据
	template<class T>
	void SetJsonValue(
		Json::Value &insert_value,
		const std::string &value_name,
		const T &value
	)
	{
		insert_value[value_name] = value;
	}
	template<class T>
	void AddJsonArrayValue(
		Json::Value &insert_value,
		const std::string &value_name,
		const T &value
	)
	{
		insert_value[value_name].append(value);
	}
	LunarEngine::LResult WriteValueToJson(
		const Json::Value &insert_value,
		const std::string &Json_name
	);
	PancyJsonMemberType GetVariableJsonType(const size_t &variable_type);
private:
	void InitBasicType();
	LunarEngine::LResult GetJsonMemberData
	(
		const std::string &file_name,
		const Json::Value &enum_type_value,
		const std::string &member_name,
		const pancy_json_data_type &json_type,
		pancy_json_value &variable_value
	);
	void SplitString(std::string str, const std::string &pattern,std::vector<std::string> &result);
};

