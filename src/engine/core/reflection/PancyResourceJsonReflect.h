#pragma once
#include <typeinfo>
#include<type_traits>
#include"PancyJsonTool.h"
#define Init_Json_Data_Vatriable(var_name) AddAnyVariable(#var_name, var_name)
#define Bind_Json_Data_Array_Size(array_value,size_value) BindArraySizeValue<decltype(array_value)>(#array_value, #size_value)
enum class SerializeNodeType
{
	Serialize_Node_Unknow = 0,
	Serialize_Node_Int,
	Serialize_Node_Uint,
	Serialize_Node_Float,
	Serialize_Node_String,
	Serialize_Node_Bool,
	Serialize_Node_Enum,
	Serialize_Node_Custom,
};
enum class SerializeDataStructType
{
	Serialize_Data_Struct_Unknow = 0,
	Serialize_Data_Struct_Node,
	Serialize_Data_Struct_Array,
};
struct LSerializeType
{
	SerializeNodeType serialize_type = SerializeNodeType::Serialize_Node_Unknow;
	SerializeDataStructType serialize_struct_type = SerializeDataStructType::Serialize_Data_Struct_Unknow;
};
//基础变量类型，用于表达任意类型的变量以及其可接受的转换类型
class LBasicVariableNode 
{
protected:
	size_t m_class_type_hash;
	size_t m_variable_size;
	void* m_variable_data;
public:
	LBasicVariableNode(
		const size_t &_class_type_hash_in,
		void* variable_data_in,
		const size_t& variable_size_in
	);
	virtual ~LBasicVariableNode() 
	{
		//父类只保有数据操作权，不对数据进行释放
	};
	//获取中间数据
	inline void* GetMidData()
	{
		return m_variable_data;
	};
	const size_t& GetClassType() const
	{
		return m_class_type_hash;
	};
	virtual LunarEngine::LResult GetValueFromNode(const size_t& class_type_hash_in, void* variable_data_out) = 0;
	virtual LunarEngine::LResult SetValueToNode(const size_t& class_type_hash_in, const void* variable_data_in) = 0;
};
template<typename VariableNodeType, typename ...AccessTypeList>
class LVariableNodeTemplate : public LBasicVariableNode
{
	VariableNodeType m_mid_data;//用于转换的中间数据
	std::unordered_map<size_t, void(*)(VariableNodeType&, const void*)> m_type_translate_func_set;
	std::unordered_map<size_t, void(*)(const VariableNodeType&, void*)> m_type_translate_func_get;
public:
	LVariableNodeTemplate(
		VariableNodeType self_value_in,
		AccessTypeList... other_value_in
	) :LBasicVariableNode(typeid(VariableNodeType).hash_code(), &m_mid_data, sizeof(VariableNodeType))
	{
		InitAllValue(other_value_in...);
	};
	~LVariableNodeTemplate()
	{
	}
	LunarEngine::LResult GetValueFromNode(const size_t & class_type_hash_in, void* variable_data_out) override 
	{
		if (class_type_hash_in == m_class_type_hash) 
		{
			//如果传入的参数类型一致，则直接赋值
			 *reinterpret_cast<VariableNodeType*>(variable_data_out) = m_mid_data;
			 return LunarEngine::g_Succeed;
		}
		//如果参数不一致，则需要查找是否可以通过类型转换进行赋值
		auto translate_type_func = m_type_translate_func_get.find(class_type_hash_in);
		if (translate_type_func == m_type_translate_func_get.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "variable type dismatch", error_message);
			return error_message;
		}
		translate_type_func->second(m_mid_data, variable_data_out);
		return LunarEngine::g_Succeed;
	};
	LunarEngine::LResult SetValueToNode(const size_t& class_type_hash_in, const void* variable_data_in) override 
	{
		if (class_type_hash_in == m_class_type_hash)
		{
			//如果传入的参数类型一致，则直接赋值
			m_mid_data = *reinterpret_cast<const VariableNodeType*>(variable_data_in);
			return LunarEngine::g_Succeed;
		}
		//如果参数不一致，则需要查找是否可以通过类型转换进行赋值
		auto translate_type_func = m_type_translate_func_set.find(class_type_hash_in);
		if (translate_type_func == m_type_translate_func_set.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "variable type dismatch", error_message);
			return error_message;
		}
		translate_type_func->second(m_mid_data, variable_data_in);
		return LunarEngine::g_Succeed;
	}
private:
	template<typename DealType,typename ...Args>
	void InitAllValue(DealType add_data, Args... others)
	{
		//这里通过展开模板的方式，生成每个类型的赋值lambda函数并存储起来
		auto type_tranlate_lambda_set = [](VariableNodeType& data_output, const void* data_input)
		{
			const DealType* new_data = reinterpret_cast<const DealType*>(data_input);
			data_output = static_cast<VariableNodeType>(*new_data);
		};
		auto type_tranlate_lambda_get = [](const VariableNodeType& data_output, void* data_input)
		{
			DealType* new_data = reinterpret_cast<DealType*>(data_input);
			*new_data = static_cast<const DealType>(data_output);
		};
		void(*deal_func_set)(VariableNodeType&, const void*) = type_tranlate_lambda_set;
		void(*deal_func_get)(const VariableNodeType&, void*) = type_tranlate_lambda_get;
		m_type_translate_func_set.insert(std::pair<size_t, void(*)(VariableNodeType&, const void*)>(typeid(DealType).hash_code(), deal_func_set));
		m_type_translate_func_get.insert(std::pair<size_t, void(*)(const VariableNodeType&, void*)>(typeid(DealType).hash_code(), deal_func_get));
		InitAllValue(others...);
	}
	void InitAllValue() 
	{
	}
};
struct NodeTypeMessageBasic 
{
	size_t node_variable_type;
};
#define LEnumMidType int32_t
static const size_t gMaxDynamicArraySize = 999999999;
//基础变量类型系统，用于将任意类型的变量进行安全的转换和赋值
class LReflectVariableCopySystem
{
	//所有注册的普通变量(非数组)
	std::unordered_map<size_t, LBasicVariableNode*> all_variable_init;
	//所有变量的类型信息
	std::unordered_map<size_t, LSerializeType> all_variable_desc;
	//所有的类型的数组数据类型(指针，vector等结构，可拓展注册)
	std::unordered_map<size_t, NodeTypeMessageBasic> all_variable_array_type;
	//数组类型对应的处理函数(取数组数据，填充数组数据，获取数组的动态大小)
	std::unordered_map<size_t, LunarEngine::LResult(*)(const size_t&, void*, const void*)> m_type_translate_func_set;
	std::unordered_map<size_t, LunarEngine::LResult(*)(const size_t&, const void*, void*)> m_type_translate_func_get;
	std::unordered_map<size_t, size_t(*)(const void*)> m_dynamic_size_func_get;
	//通过枚举名得到枚举变量
	std::unordered_map<size_t, std::unordered_map<std::string, LEnumMidType>> enum_variable_list;
	//通过枚举变量得到枚举名
	std::unordered_map<size_t, std::unordered_map<LEnumMidType, std::string>> enum_name_list;
	//记录已经注册的所有枚举类型
	std::unordered_map<size_t, std::string> enum_class_map;
	LReflectVariableCopySystem();
public:
	static LReflectVariableCopySystem* GetInstance()
	{
		static LReflectVariableCopySystem* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new LReflectVariableCopySystem();
		}
		return this_instance;
	}
	~LReflectVariableCopySystem();
	template<typename VariableNodeType, typename ...AccessTypeList>
	void InitNewVariable(VariableNodeType self_value, SerializeNodeType serialize_type, AccessTypeList... other_value_in)
	{
		size_t now_type_id = typeid(VariableNodeType).hash_code();
		if(all_variable_init.find(now_type_id) != all_variable_init.end())
		{
			LogVerboseFormat(0,"reflect type %s has inited,do not reapeat init", typeid(VariableNodeType).name());
			return;
		}
		LBasicVariableNode* new_variable = new LVariableNodeTemplate(self_value, other_value_in...);
		all_variable_init.insert(std::pair<size_t, LBasicVariableNode*>(typeid(VariableNodeType).hash_code(), new_variable));
		LSerializeType now_member_desc;
		now_member_desc.serialize_type = serialize_type;
		now_member_desc.serialize_struct_type = SerializeDataStructType::Serialize_Data_Struct_Node;
		all_variable_desc.insert(std::pair<size_t, LSerializeType>(typeid(VariableNodeType).hash_code(), now_member_desc));
		InitArray<VariableNodeType>(serialize_type);
	};
	template<typename DataType>
	LunarEngine::LResult GetValueFromPointer(const size_t deal_value_type,DataType& output_data,const void* pointer_data)
	{
		//先检查这个值的类型是否被注册过
		auto deal_class = all_variable_init.find(deal_value_type);
		if (deal_class == all_variable_init.end()) 
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0,"could not find deal class", error_message);
			return error_message;
		}
		//如果注册过，则取出注册的类来处理这个变量的赋值拷贝
		LunarEngine::LResult check_error = deal_class->second->SetValueToNode(deal_value_type, pointer_data);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		check_error = deal_class->second->GetValueFromNode(typeid(DataType).hash_code(), &output_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	template<typename DataType>
	LunarEngine::LResult SetValueToPointer(const size_t deal_value_type, const DataType& input_data, void* pointer_data)
	{
		//先检查这个值的类型是否被注册过
		auto deal_class = all_variable_init.find(deal_value_type);
		if (deal_class == all_variable_init.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "could not find deal class", error_message);
			return error_message;
		}
		//如果注册过，则取出注册的类来处理这个变量的赋值拷贝
		LunarEngine::LResult check_error = deal_class->second->SetValueToNode(typeid(DataType).hash_code(), &input_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = deal_class->second->GetValueFromNode(deal_value_type, pointer_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	template<typename DataType>
	LunarEngine::LResult GetArrayValueFromPointerByIndex(const size_t deal_value_type_in, const size_t& array_index, const void* pointer_data, DataType& output_data)
	{
		LunarEngine::LResult check_error;
		//先检查类型是否是数组类型
		size_t deal_value_member_type;
		bool if_succeed = GetTypeByArrayType(deal_value_type_in, deal_value_member_type);
		if (!if_succeed)
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "haven't init array type or type is not array", error_message);
			return error_message;
		}
		//再检查这个值的类型是否被注册过
		auto deal_class = all_variable_init.find(deal_value_member_type);
		if (deal_class == all_variable_init.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "could not find deal class", error_message);
			return error_message;
		}
		//如果是注册过的类型，则开始寻找数组对应的数据提取函数
		check_error = m_type_translate_func_get[deal_value_type_in](array_index, pointer_data, deal_class->second->GetMidData());
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//如果成员数据提取成功，使用其数组成员注册的类来处理这个变量的赋值拷贝
		check_error = deal_class->second->GetValueFromNode(typeid(DataType).hash_code(), &output_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	template<typename DataType>
	LunarEngine::LResult SetArrayValueToPointerByIndex(const size_t deal_value_type_in, const size_t& array_index, void* pointer_data, const DataType& input_data)
	{
		LunarEngine::LResult check_error;
		//先检查类型是否是数组类型
		size_t deal_value_member_type;
		bool if_succeed = GetTypeByArrayType(deal_value_type_in, deal_value_member_type);
		if (!if_succeed)
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "haven't init array type or type is not array", error_message);
			return error_message;
		}
		//再检查这个值的类型是否被注册过
		auto deal_class = all_variable_init.find(deal_value_member_type);
		if (deal_class == all_variable_init.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "could not find deal class", error_message);
			return error_message;
		}
		//如果是注册过的类型，先把输入的数据转换成目标类型的数据
		check_error = deal_class->second->SetValueToNode(typeid(DataType).hash_code(), &input_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//如果是注册过的类型，则开始寻找数组对应的数据提取函数
		check_error = m_type_translate_func_set[deal_value_type_in](array_index, pointer_data, deal_class->second->GetMidData());
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	//字符串处理枚举变量的信息
	LunarEngine::LResult SetEnumValueToPointer(const size_t& enum_type_id, const std::string& enum_name, const size_t deal_value_type_in, void* pointer_data);
	LunarEngine::LResult GetEnumValueFromPointer(const size_t& enum_type_id, std::string& enum_name, const void* pointer_data);
	LunarEngine::LResult SetEnumArrayValueToPointerByIndex(const size_t enum_type_id, const std::string& enum_name, const size_t& array_index, void* pointer_data);
	LunarEngine::LResult GetEnumArrayValueFromPointerByIndex(const size_t enum_type_id, std::string& enum_name, const size_t& array_index, const void* pointer_data);
	template<typename EnumType>
	void InitNewEnum(const std::string enum_name,EnumType enum_value) 
	{
		//在枚举变量查找表中添加一个元素
		size_t enum_type_index = typeid(EnumType).hash_code();
		auto now_enum_value_table = enum_variable_list.find(enum_type_index);
		if (now_enum_value_table == enum_variable_list.end()) 
		{
			enum_variable_list.insert(std::pair<size_t, std::unordered_map<std::string, LEnumMidType>>(enum_type_index, std::unordered_map<std::string, LEnumMidType>()));
			now_enum_value_table = enum_variable_list.find(enum_type_index);
		}
		auto now_enum_value_check = now_enum_value_table->second.find(enum_name);
		if (now_enum_value_check == now_enum_value_table->second.end()) 
		{
			now_enum_value_table->second.insert(std::pair<std::string, LEnumMidType>(enum_name,static_cast<LEnumMidType>(enum_value)));
		}
		else 
		{
			LunarEngine::LResult error_message;
			LunarDebugLogWarning(0,"repeat load enum: " + enum_name, error_message);
		}
		//在枚举名查找表中添加一个元素
		auto now_enum_name_table = enum_name_list.find(enum_type_index);
		if (now_enum_name_table == enum_name_list.end())
		{
			enum_name_list.insert(std::pair<size_t, std::unordered_map<LEnumMidType,std::string>>(enum_type_index, std::unordered_map<LEnumMidType,std::string>()));
			now_enum_name_table = enum_name_list.find(enum_type_index);
		}
		auto now_enum_name_check = now_enum_name_table->second.find(static_cast<LEnumMidType>(enum_value));
		if (now_enum_name_check == now_enum_name_table->second.end())
		{
			now_enum_name_table->second.insert(std::pair<LEnumMidType, std::string>(static_cast<LEnumMidType>(enum_value), enum_name));
		}
		else
		{
			LunarEngine::LResult error_message;
			LunarDebugLogWarning(0, "repeat load enum: " + enum_name, error_message);
		}
		//如果是第一次注册这个类型的变量则需要注册这个类型
		if(enum_class_map.find(enum_type_index) == enum_class_map.end())
		{
			EnumType empty_value(enum_value);
			InitNewVariable<EnumType, LEnumMidType>(empty_value, SerializeNodeType::Serialize_Node_Enum, 0);
			enum_class_map.insert(std::pair<size_t,std::string>(enum_type_index, typeid(EnumType).name()));
		}
		
	};
	//根据一个类型的数组类型获取其内部元素的类型
	bool GetTypeByArrayType(const size_t &type_id_in, size_t& type_id_out);
	bool GetDynamicSizeByArrayType(const size_t& type_id_in, const void* array_data, size_t& dynamic_size_out);
	//获取类型的序列化信息
	LSerializeType GetSerializeType(const size_t& index_in);
private:
	template<typename VariableNodeType>
	void InitArray(SerializeNodeType serialize_type)
	{
		//注册类型的基础信息
		NodeTypeMessageBasic now_array_type;
		now_array_type.node_variable_type = typeid(VariableNodeType).hash_code();
		//创建数组类型的序列化格式信息
		LSerializeType now_member_desc;
		now_member_desc.serialize_type = serialize_type;
		now_member_desc.serialize_struct_type = SerializeDataStructType::Serialize_Data_Struct_Array;
		//注册普通类型的数组
		InitArrayTypeMessage<VariableNodeType*>(now_array_type, now_member_desc);
		auto simple_array_lambda_get = [](const size_t& array_index, const void* array_data, void* reflect_member_out)->LunarEngine::LResult
		{
			//这里因为是注册赋值函数，并不能确定指针的大小，需要上面的反射层严格保证不会出现越界的情况！！！！
			const VariableNodeType* real_value_pointer = reinterpret_cast<const VariableNodeType*> (array_data);
			if (real_value_pointer == NULL) 
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			VariableNodeType* copy_value_pointer = reinterpret_cast<VariableNodeType*> (reflect_member_out);
			if (copy_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			*copy_value_pointer = real_value_pointer[array_index];
			return LunarEngine::g_Succeed;
		};
		auto simple_array_lambda_set = [](const size_t& array_index, void* array_data, const void* reflect_member_out)->LunarEngine::LResult
		{
			//这里因为是注册赋值函数，并不能确定指针的大小，需要上面的反射层严格保证不会出现越界的情况！！！！
			VariableNodeType* real_value_pointer = reinterpret_cast<VariableNodeType*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			//数组元素直接引用，不必动态创建
			const VariableNodeType* copy_value_pointer = reinterpret_cast<const VariableNodeType*> (reflect_member_out);
			if (copy_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			real_value_pointer[array_index] = *copy_value_pointer;
			return LunarEngine::g_Succeed;
		};
		auto simple_array_lambda_get_size = [](const void* array_data)
		{
			return gMaxDynamicArraySize;
		};
		m_type_translate_func_get.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, const void*, void*)>(typeid(VariableNodeType*).hash_code(), simple_array_lambda_get));
		m_type_translate_func_set.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, void*, const void*)>(typeid(VariableNodeType*).hash_code(), simple_array_lambda_set));
		m_dynamic_size_func_get.insert(std::pair<size_t, size_t(*)(const void*)>(typeid(VariableNodeType*).hash_code(), simple_array_lambda_get_size));
		//注册STL vector 类型的数组
		InitArrayTypeMessage<std::vector<VariableNodeType>>(now_array_type, now_member_desc);
		auto simple_stdvector_lambda_get = [](const size_t& array_index, const void* array_data, void* reflect_member_out)->LunarEngine::LResult
		{
			const std::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<const std::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			//这里获取函数没有扩充数组的权力，一旦发现数组的容量不足，立即报错
			if (real_value_pointer->size() <= array_index) 
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error,std::vector size is lessequal than request index,could not get value", error_message);
				return error_message;
			}
			//这里只需要把相应的数据暴露出去即可，赋值的工作由拷贝系统处理
			VariableNodeType* copy_value_pointer = reinterpret_cast<VariableNodeType*> (reflect_member_out);
			if (copy_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			*copy_value_pointer = (*real_value_pointer)[array_index];
			return LunarEngine::g_Succeed;
		};
		auto simple_stdvector_lambda_set = [](const size_t& array_index, void* array_data, const void* reflect_member_out)->LunarEngine::LResult
		{
			std::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<std::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			//这里vector的扩充极限是一个元素，超过了一个元素不允许扩充
			if (real_value_pointer->size() < array_index)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error,std::vector size is less than request index,could not set", error_message);
				return error_message;
			}
			else if (real_value_pointer->size() == array_index) 
			{
				//拷贝数据
				const VariableNodeType* copy_value_pointer = reinterpret_cast<const VariableNodeType*> (reflect_member_out);
				if (copy_value_pointer == NULL)
				{
					LunarEngine::LResult error_message;
					LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
					return error_message;
				}
				real_value_pointer->push_back(*copy_value_pointer);
			}
			return LunarEngine::g_Succeed;
		};
		auto simple_stdvector_lambda_get_size = [](const void* array_data)
		{
			const std::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<const std::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return gMaxDynamicArraySize;
			}
			return (*real_value_pointer).size();
		};
		m_type_translate_func_get.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, const void*, void*)>(typeid(std::vector<VariableNodeType>).hash_code(), simple_stdvector_lambda_get));
		m_type_translate_func_set.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, void*, const void*)>(typeid(std::vector<VariableNodeType>).hash_code(), simple_stdvector_lambda_set));
		m_dynamic_size_func_get.insert(std::pair<size_t, size_t(*)(const void*)>(typeid(std::vector<VariableNodeType>).hash_code(), simple_stdvector_lambda_get_size));
		//注册Boost vector 类型的数组
		InitArrayTypeMessage<boost::container::vector<VariableNodeType>>(now_array_type, now_member_desc);
		auto simple_boostvector_lambda_get = [](const size_t& array_index, const void* array_data, void* reflect_member_out)->LunarEngine::LResult
		{
			const boost::container::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<const boost::container::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			//这里获取函数没有扩充数组的权力，一旦发现数组的容量不足，立即报错
			if (real_value_pointer->size() <= array_index)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error,std::vector size is lessequal than request index,could not get value", error_message);
				return error_message;
			}
			VariableNodeType* copy_value_pointer = reinterpret_cast<VariableNodeType*> (reflect_member_out);
			if (copy_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			*copy_value_pointer = (*real_value_pointer)[array_index];
			return LunarEngine::g_Succeed;
		};
		auto simple_boostvector_lambda_set = [](const size_t& array_index, void* array_data, const void* reflect_member_out)->LunarEngine::LResult
		{
			boost::container::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<boost::container::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return error_message;
			}
			//这里vector的扩充极限是一个元素，超过了一个元素不允许扩充
			if (real_value_pointer->size() < array_index)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error,std::vector size is less than request index,could not set", error_message);
				return error_message;
			}
			else if (real_value_pointer->size() == array_index)
			{
				//拷贝数据
				const VariableNodeType* copy_value_pointer = reinterpret_cast<const VariableNodeType*> (reflect_member_out);
				if (copy_value_pointer == NULL)
				{
					LunarEngine::LResult error_message;
					LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
					return error_message;
				}
				real_value_pointer->push_back(*copy_value_pointer);
			}
			return LunarEngine::g_Succeed;
		};
		auto simple_boostvector_lambda_get_size = [](const void* array_data)
		{
			const boost::container::vector<VariableNodeType>* real_value_pointer = reinterpret_cast<const boost::container::vector<VariableNodeType>*> (array_data);
			if (real_value_pointer == NULL)
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0, "error null ptr,failed translate type", error_message);
				return gMaxDynamicArraySize;
			}
			return (*real_value_pointer).size();
		};
		m_type_translate_func_get.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, const void*, void*)>(typeid(boost::container::vector<VariableNodeType>).hash_code(), simple_boostvector_lambda_get));
		m_type_translate_func_set.insert(std::pair<size_t, LunarEngine::LResult(*)(const size_t&, void*, const void*)>(typeid(boost::container::vector<VariableNodeType>).hash_code(), simple_boostvector_lambda_set));
		m_dynamic_size_func_get.insert(std::pair<size_t, size_t(*)(const void*)>(typeid(boost::container::vector<VariableNodeType>).hash_code(), simple_boostvector_lambda_get_size));
	};
	template<typename VariableArrayType>
	void InitArrayTypeMessage(const NodeTypeMessageBasic& node_type, const LSerializeType& serialize_type)
	{
		all_variable_array_type.insert(std::pair<size_t, NodeTypeMessageBasic>(typeid(VariableArrayType).hash_code(), node_type));
		all_variable_desc.insert(std::pair<size_t, LSerializeType>(typeid(VariableArrayType).hash_code(), serialize_type));
	}

	LunarEngine::LResult GetEnumValueFromName(const size_t& enum_type_id, const std::string& enum_name, LEnumMidType& enum_value);
	LunarEngine::LResult GetEnumNameFromValue(const size_t& enum_type_id, std::string& enum_name, const LEnumMidType& enum_value);
};

//反射节点，结构体中的每一个元变量都会有一个节点，节点会存储父子信息以简化编写，但是不会表达数组结构的父子信息
class LReflectNodeBasic
{
	int32_t m_node_id;                           //节点id
	std::string m_node_name;                     //节点名称
	size_t m_value_type;                         //数据的类型
	void* m_data_value;                          //标识数据的值地址
	size_t m_array_size = gMaxDynamicArraySize;  //标识数据的最大可容纳大小(兼容数组)
	bool m_if_empty_struct_node = false;         //标识是否是用于存储父节点的空节点
public:
	LReflectNodeBasic()
	{
		m_node_name = "";
		m_value_type = 0;
		m_data_value = NULL;
		m_array_size = 0;
		m_node_id = -1;
		m_if_empty_struct_node = false;
	}
	LReflectNodeBasic(const std::string &node_name,const int32_t &node_id)
	{
		m_node_name = node_name;
		m_value_type = 0;
		m_data_value = NULL;
		m_array_size = 0;
		m_node_id = node_id;
		m_if_empty_struct_node = true;
	}
	inline size_t GetArraySize() 
	{
		return m_array_size;
	};
	inline std::string GetName() const
	{
		return m_node_name;
	};
	inline int32_t GetID() const
	{
		return m_node_id;
	};
	inline size_t GetValueType() const
	{
		return m_value_type;
	};
	inline size_t GetDynamicArraySize() const
	{
		size_t now_size = gMaxDynamicArraySize;
		LReflectVariableCopySystem::GetInstance()->GetDynamicSizeByArrayType(m_value_type, m_data_value, now_size);
		return now_size;
	}
	inline void* GetDataPointer() 
	{
		return m_data_value;
	}
	LSerializeType GetSerializeType() const;
	template<typename ValueType>
	LReflectNodeBasic(const std::string& node_name,const int32_t &node_id,ValueType& data_value)
	{
		m_node_id = node_id;
		m_node_name = node_name;
		size_t m_value_type_member;//数据如果是数组类型，记录一下他所包含的元素的类型
		m_if_empty_struct_node = false;
		if constexpr (std::is_array<ValueType>::value)
		{
			//裸数组直接绑定指针
			m_value_type = typeid(&data_value[0]).hash_code();
			m_data_value = reinterpret_cast<void*>(data_value);
			m_array_size = sizeof(data_value) / sizeof(data_value[0]);
		}
		else
		{
			//其他的变量和c++封装的数组，取地址保存
			m_value_type = typeid(ValueType).hash_code();
			bool if_array = LReflectVariableCopySystem::GetInstance()->GetTypeByArrayType(m_value_type, m_value_type_member);
			m_data_value = reinterpret_cast<void*>(&data_value);
		}
	};
	template<typename DataType>
	LunarEngine::LResult GetSingleValueFromNode(DataType& output_data) const
	{
		LunarEngine::LResult check_error;
		check_error = LReflectVariableCopySystem::GetInstance()->GetValueFromPointer(m_value_type, output_data, m_data_value);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	template<typename DataType>
	LunarEngine::LResult SetSingleValueToNode(const DataType& input_data) 
	{
		LunarEngine::LResult check_error;
		check_error = LReflectVariableCopySystem::GetInstance()->SetValueToPointer(m_value_type, input_data, m_data_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename DataType>
	LunarEngine::LResult GetArrayValueFromNode(DataType& output_data, const int32_t& index)const
	{
		LunarEngine::LResult check_error;
		//先检查最大值是否超过了数组范围
		if (index >= m_array_size)
		{
			LunarDebugLogError(0, "could not get array value from overflow index", check_error);
			return check_error;
		}
		//根据当前节点的类型去进行值转换
		check_error = LReflectVariableCopySystem::GetInstance()->GetArrayValueFromPointerByIndex(m_value_type, index, m_data_value,output_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename DataType>
	LunarEngine::LResult SetArrayValueToNode(const DataType& input_data, const int32_t& index) 
	{
		//先检查最大值是否超过了数组范围
		LunarEngine::LResult check_error;
		if (index >= m_array_size)
		{
			LunarDebugLogError(0, "could not get array value from overflow index", check_error);
			return check_error;
		}
		//根据当前节点的类型去进行值转换
		check_error = LReflectVariableCopySystem::GetInstance()->SetArrayValueToPointerByIndex(m_value_type, index, m_data_value, input_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	//枚举变量
	LunarEngine::LResult GetSingleEnumValueFromNode(std::string& output_enum_name) const;
	LunarEngine::LResult SetSingleEnumValueToNode(const std::string& input_enum_name);
	LunarEngine::LResult GetArrayEnumValueFromNode(std::string& output_enum_name, const int32_t& index) const;
	LunarEngine::LResult SetArrayEnumValueToNode(const std::string& output_enum_name, const int32_t& index);
};

class LSrtructReflect
{
	int32_t m_value_self_add = 0;
	//将json数据绑定到普通结构体的数据结构
	std::unordered_map<int32_t, LReflectNodeBasic> value_map;                  //每个成员变量对应的反射节点
	std::unordered_map<int32_t, std::string> name_map;                         //每个成员变量的名字
	std::unordered_map<std::string,int32_t> name_to_id_map;                    //每个成员变量的名字
	std::unordered_map<int32_t, int32_t> parent_map;                           //每个成员变量的父节点
	std::unordered_map<int32_t, int32_t> static_array_size_used_map;           //每个静态数组真正使用的元素数量
	std::unordered_map<int32_t, std::vector<std::int32_t>> child_value_list;   //每个成员变量的子变量
public:
	LSrtructReflect() {};
	virtual ~LSrtructReflect() {};
	void Create();
	//从json文件中加载数据
	LunarEngine::LResult LoadFromJsonFile(const std::string& Json_file);
	//从json内存中加载数据
	LunarEngine::LResult LoadFromJsonMemory(const std::string& value_name, const Json::Value& root_value);
	//将类成员数据存储到json文件
	LunarEngine::LResult SaveToJsonFile(const std::string& json_name);
	//将类成员数据存储到json内存
	LunarEngine::LResult SaveToJsonMemory(Json::Value& root_value);
	//将类成员数据拷贝到指定指针
	virtual LunarEngine::LResult CopyValueToData(LReflectNodeBasic &dst_pointer) = 0;
	virtual LunarEngine::LResult CopyValueToArrayData(LReflectNodeBasic& dst_pointer,const int32_t &index_array) = 0;
	//将指定指针的数据拷贝到类成员数据
	virtual LunarEngine::LResult ResetValueByData(const LReflectNodeBasic& dst_pointer) = 0;
	virtual LunarEngine::LResult ResetValueByArrayData(const LReflectNodeBasic& dst_pointer, const int32_t& index_array) = 0;
	//获取当前的成员变量
	virtual LunarEngine::LResult GetMemberValue(void* value_data) = 0;
	//将指定指针的数据拷贝到类成员数据
	virtual LunarEngine::LResult SetMemberValue(const void* value_data) = 0;
private:
	//创建子节点映射
	void BuildChildValueMap();
	//从json节点中加载数据
	LunarEngine::LResult LoadFromJsonNode(const std::string& parent_name, const std::string& value_name, const Json::Value& root_value);
	//从json数组中加载数据
	LunarEngine::LResult LoadFromJsonArray(const int32_t& value_index, const Json::Value& root_value);
	//将数据写入到json节点
	LunarEngine::LResult SaveToJsonNode(const int32_t& now_value_index, Json::Value& root_value);
	//注册所有的反射变量
	virtual void InitBasicVariable() = 0;
	//绑定所有的静态数组大小控制变量
	virtual void BindStaticArraySize() = 0;
	//获取节点的父节点名称
	const std::string GetParentName(const std::string& name_in);
	//设置数组变量
	LunarEngine::LResult SetArrayValue(LReflectNodeBasic& reflect_data, const Json::Value& now_child_value);
	//设置结构体数组变量
	LunarEngine::LResult SetNodeArrayValue(LReflectNodeBasic& reflect_data, const int32_t& offset_value, const Json::Value& now_child_value);
	//获取基础变量
	LunarEngine::LResult SaveSingleValueMemberToJson(const LReflectNodeBasic& reflect_data, Json::Value& root_value);
	//获取数组变量
	LunarEngine::LResult SaveArrayValueMemberToJson(const LReflectNodeBasic& reflect_data, Json::Value& root_value);
	//将变量的完整名称转化为基础名称
	std::string TranslateFullNameToRealName(const std::string& full_name);
	//获取数组数据的真实大小
	LunarEngine::LResult GetArrayDataSize(const LReflectNodeBasic& reflect_data, size_t& size_out);
	//分割逗号字符串
	void DivideStringForReflect(const char* name_input, std::vector<std::string>& data_out);
protected:
	template<typename VariableType>
	LunarEngine::LResult AddAnyVariable(
		const std::string& name,
		VariableType& variable_data
	) 
	{
		LunarEngine::LResult check_error;
		LReflectNodeBasic new_reflect_node(name, m_value_self_add,variable_data);
		check_error = AddReflectData(name, new_reflect_node);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename ArrayType>
	LunarEngine::LResult BindArraySizeValue(
		const std::string& array_name,
		const std::string& value_name
	) 
	{
		if constexpr (std::is_array<ArrayType>::value)
		{
			auto array_value_id = name_to_id_map.find(array_name);
			auto array_size_id = name_to_id_map.find(value_name);
			if (array_value_id == name_to_id_map.end() || array_size_id == name_to_id_map.end())
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(0,"could not find array value or array size value in reflect", error_message);
				return error_message;
			}
			static_array_size_used_map.insert(std::pair<int32_t, int32_t>(array_value_id->second, array_size_id->second));
		}
		else
		{
			//未找到变量，无法绑定
			LunarEngine::LResult error_message;
			LunarDebugLogError(E_FAIL, array_name + " is not an array value, could not bind it's size to other member: ", error_message);
			return error_message;
		}
		return LunarEngine::g_Succeed;
	}
	LunarEngine::LResult AddReflectData(const std::string &node_name,LReflectNodeBasic &node_data);
	LunarEngine::LResult SetStringValue(LReflectNodeBasic& reflect_data, const std::string& string_value);
	LunarEngine::LResult SetStringArrayValue(LReflectNodeBasic& reflect_data, const std::string& string_value, const int32_t &index);
	virtual void ClearMemory() = 0;
	//绑定所有的静态数组的大小标记变量
	template <class NowDealedVariable, class NowDealedSizeVariable, class ...Args>
	LunarEngine::LResult BindAllStaticVariable(const char* variable_name, NowDealedVariable& head, NowDealedSizeVariable &head_size, Args&... rest)
	{
		std::vector<std::string> all_variable_name;
		DivideStringForReflect(variable_name, all_variable_name);
		return BindAllStaticVariable(all_variable_name, 0, head, head_size, rest...);
	}
	template <class NowDealedVariable, class NowDealedSizeVariable, class ...Args>
	LunarEngine::LResult BindAllStaticVariable(const std::vector<std::string>& variable_name, const int32_t now_index, NowDealedVariable& head, NowDealedSizeVariable& head_size, Args&... rest)
	{
		LunarEngine::LResult check_error;
		check_error = BindArraySizeValue<NowDealedVariable>(variable_name[now_index], variable_name[now_index+1]);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return BindAllStaticVariable(variable_name, now_index + 2, rest...);
	}
	LunarEngine::LResult BindAllStaticVariable(const std::vector<std::string>& variable_name, const int32_t now_index)
	{
		if (now_index != variable_name.size())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "reflect variable size dismatch", error_message);
			return error_message;
		}
		return LunarEngine::g_Succeed;
	}
	//注册所有的成员函数
	template <class NowDealedVariable, class ...Args>
	LunarEngine::LResult AddAllVariable(const char* variable_name, NowDealedVariable& head, Args&... rest)
	{
		std::vector<std::string> all_variable_name;
		DivideStringForReflect(variable_name, all_variable_name);
		return AddAllVariable(all_variable_name, 0, head, rest...);
	}
	template <class NowDealedVariable, class ...Args>
	LunarEngine::LResult AddAllVariable(const std::vector<std::string>& variable_name,const int32_t now_index, NowDealedVariable& head, Args&... rest)
	{
		LunarEngine::LResult check_error;
		check_error = AddAnyVariable(variable_name[now_index], head);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		return AddAllVariable(variable_name, now_index + 1, rest...);
	}
	LunarEngine::LResult AddAllVariable(const std::vector<std::string>& variable_name, const int32_t now_index)
	{
		
		if (now_index != variable_name.size()) 
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "reflect variable size dismatch", error_message);
			return error_message;
		}
		return LunarEngine::g_Succeed;
	}
};
template<typename ReflectDataType>
class LSrtructReflectTemplate :public LSrtructReflect 
{
protected:
	ReflectDataType reflect_data = {};
public:
	LSrtructReflectTemplate() :LSrtructReflect()
	{

	}
	virtual ~LSrtructReflectTemplate() 
	{
	}
	//将类成员数据拷贝到指定指针
	LunarEngine::LResult CopyValueToData(LReflectNodeBasic& dst_pointer) override 
	{
		auto check_error = dst_pointer.SetSingleValueToNode(reflect_data);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	LunarEngine::LResult CopyValueToArrayData(LReflectNodeBasic& dst_pointer, const int32_t& index_array) override
	{
		auto check_error = dst_pointer.SetArrayValueToNode(reflect_data, index_array);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	//将指定指针的数据拷贝到类成员数据
	LunarEngine::LResult ResetValueByData(const LReflectNodeBasic& dst_pointer) override 
	{
		auto check_error = dst_pointer.GetSingleValueFromNode(reflect_data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	LunarEngine::LResult ResetValueByArrayData(const LReflectNodeBasic& dst_pointer, const int32_t& index_array) override
	{
		auto check_error = dst_pointer.GetArrayValueFromNode(reflect_data, index_array);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	};
	//获取当前的成员变量
	virtual LunarEngine::LResult GetMemberValue(void* value_data) override
	{
		ReflectDataType* now_value_pointer = reinterpret_cast<ReflectDataType*>(value_data);
		if (now_value_pointer == NULL) 
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0,"variable type dismatch", error_message);
			return error_message;
		}
		*now_value_pointer = reflect_data;
		return LunarEngine::g_Succeed;
	}
	//将指定指针的数据拷贝到类成员数据
	virtual LunarEngine::LResult SetMemberValue(const void* value_data)override
	{
		const ReflectDataType* now_value_pointer = reinterpret_cast<const ReflectDataType*>(value_data);
		if (now_value_pointer == NULL)
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "variable type dismatch", error_message);
			return error_message;
		}
		reflect_data = *now_value_pointer;
		return LunarEngine::g_Succeed;
	};
private:
	void ClearMemory()
	{
		ReflectDataType new_reflect_data = {};
		reflect_data = new_reflect_data;
	}
};
class LSrtructReflectControl
{
	std::unordered_map<size_t, LSrtructReflect*> reflect_map;
	LSrtructReflectControl();
public:
	~LSrtructReflectControl();
	template<typename ReflectStructType>
	void InitJsonReflect(LSrtructReflect* reflect_parse_class);
	template<typename class_name>
	LunarEngine::LResult SerializeToJsonFile(const class_name& data, const std::string& file_name)
	{
		Json::Value now_json_value;
		LunarEngine::LResult check_error = SerializeToJsonMemory(data, now_json_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = PancyJsonTool::GetInstance()->WriteValueToJson(now_json_value, file_name);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename class_name>
	LunarEngine::LResult SerializeToJsonMemory(const class_name& data, Json::Value& now_json_value) 
	{
		size_t now_class_id = typeid(class_name).hash_code();
		auto check_class = reflect_map.find(now_class_id);
		if (check_class == reflect_map.end()) 
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0,std::string("haven't init reflect class: ") + typeid(class_name).name(), error_message);
			return error_message;
		}
		LunarEngine::LResult check_error;
		check_error = check_class->second->SetMemberValue(&data);
		if (!check_error.m_IsOK) 
		{
			return check_error;
		}
		check_error = check_class->second->SaveToJsonMemory(now_json_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename class_name>
	LunarEngine::LResult DeserializeFromJsonMemory(class_name& data, const Json::Value& now_json_value,const std::string &json_name)
	{
		size_t now_class_id = typeid(class_name).hash_code();
		auto check_class = reflect_map.find(now_class_id);
		if (check_class == reflect_map.end())
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, std::string("haven't init reflect class: ") + typeid(class_name).name(), error_message);
			return error_message;
		}
		LunarEngine::LResult check_error;
		check_error = check_class->second->LoadFromJsonMemory(json_name,now_json_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = check_class->second->GetMemberValue(&data);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename class_name>
	LunarEngine::LResult DeserializeFromJsonFile(class_name& data,const std::string& file_name)
	{
		Json::Value now_json_value;
		LunarEngine::LResult check_error = PancyJsonTool::GetInstance()->LoadJsonFile(file_name, now_json_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = DeserializeFromJsonMemory(data, now_json_value, file_name);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	LSrtructReflect* GetJsonReflect(const size_t& class_id);
	LSrtructReflect* GetJsonReflectByArray(const size_t& class_id);
	//通过vector或者array的方式检查结构体是否注册
	bool CheckIfStructArrayInit(const size_t& class_id);
	//通过变量的方式检查结构体是否注册
	bool CheckIfStructMemberInit(const size_t& class_id);
	static LSrtructReflectControl* GetInstance()
	{
		static LSrtructReflectControl* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new LSrtructReflectControl();
		}
		return this_instance;
	}
};
template<typename ReflectStructType>
void LSrtructReflectControl::InitJsonReflect(LSrtructReflect* new_reflect_data)
{
	ReflectStructType empty_data = {};
	LReflectVariableCopySystem::GetInstance()->InitNewVariable<ReflectStructType>(empty_data,SerializeNodeType::Serialize_Node_Custom);
	size_t class_id = typeid(ReflectStructType).hash_code();
	auto now_reflect_data = reflect_map.find(class_id);
	if (now_reflect_data == reflect_map.end())
	{
		new_reflect_data->Create();
		reflect_map.insert(std::pair<size_t, LSrtructReflect*>(class_id, new_reflect_data));
	}
	else if (new_reflect_data != NULL)
	{
		delete new_reflect_data;
	}
}
#define InitNewEnumValue(enum_value) LReflectVariableCopySystem::GetInstance()->InitNewEnum(#enum_value, enum_value);
//创建反射处理类
#define INIT_REFLECTION_CLASS(deal_class_type,...) \
class LReflection_Class_##deal_class_type :public LSrtructReflectTemplate<deal_class_type>\
{\
public:\
	LReflection_Class_##deal_class_type() {};\
	~LReflection_Class_##deal_class_type() {};\
private:\
	void InitBasicVariable() override\
	{\
		AddAllVariable(#__VA_ARGS__,__VA_ARGS__);\
	};\
	void BindStaticArraySize() {};\
};

#define INIT_REFLECTION_CLASS_WITH_STATIC_ARRAY(deal_class_type,...) \
class LReflection_Class_##deal_class_type :public LSrtructReflectTemplate<deal_class_type>\
{\
public:\
	LReflection_Class_##deal_class_type() {};\
	~LReflection_Class_##deal_class_type() {};\
private:\
	void InitBasicVariable() override\
	{\
		AddAllVariable(#__VA_ARGS__,__VA_ARGS__);\
	};\
	void BindStaticArraySize();\
};

#define BIND_REFLECTION_CLASS_STATIC_ARRAY(deal_class_type,...) \
void LReflection_Class_##deal_class_type::BindStaticArraySize()\
{\
	BindAllStaticVariable(#__VA_ARGS__,__VA_ARGS__);\
};
//注册反射结构
#define InitNewStructToReflection(class_name)\
LSrtructReflectTemplate<class_name>* new_reflectinstance_##class_name = new LReflection_Class_##class_name();\
LSrtructReflectControl::GetInstance()->InitJsonReflect<class_name>(new_reflectinstance_##class_name);