#pragma once
#include "core/core_module.h"
template<typename KeyType, typename ValueType>
class LMultiThreadUnorderedMapDynamic
{
	std::unordered_map<KeyType, ValueType> map_value;//map的数据存放
	std::mutex map_use_lock;                      //map之间需要加锁
public:
	LMultiThreadUnorderedMapDynamic();
	~LMultiThreadUnorderedMapDynamic();
	luna::LResult GetValue(const KeyType& value_index, ValueType& value_data);
	luna::LResult AddValue(const KeyType& value_index, const ValueType& value_data);
	luna::LResult RemoveValue(const KeyType& value_index);
	bool HasValue(const KeyType& value_index);
};
template<typename KeyType, typename ValueType>
LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::LMultiThreadUnorderedMapDynamic()
{
}
template<typename KeyType, typename ValueType>
LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::~LMultiThreadUnorderedMapDynamic()
{
}
template<typename KeyType, typename ValueType>
luna::LResult LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::GetValue(const KeyType& value_index, ValueType& value_data)
{
	luna::LResult check_error = luna::g_Succeed;
	map_use_lock.lock();
	//开始进行数据的读取
	auto data_find = map_value.find(value_index);
	if (data_find == map_value.end())
	{
		LunarDebugLogError(0, "could not find map value", check_error);
	}
	value_data = data_find->second;
	map_use_lock.unlock();
	return check_error;
}
template<typename KeyType, typename ValueType>
luna::LResult LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::AddValue(const KeyType& value_index, const ValueType& value_data)
{
	luna::LResult check_error = luna::g_Succeed;
	map_use_lock.lock();
	//开始进行数据的插入
	auto data_find = map_value.find(value_index);
	if (data_find != map_value.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "repeat insert value to map", check_error);
	}
	else
	{
		map_value.insert({ value_index ,value_data });
	}
	map_use_lock.unlock();
	return check_error;
}
template<typename KeyType, typename ValueType>
luna::LResult  LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::RemoveValue(const KeyType& value_index)
{
	luna::LResult check_error = luna::g_Succeed;
	map_use_lock.lock();
	//开始进行数据的删除
	auto data_find = map_value.find(value_index);
	if (data_find == map_value.end())
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not find value in map", check_error);
	}
	else
	{
		map_value.erase(value_index);
	}
	map_use_lock.unlock();
	return check_error;
}
template<typename KeyType, typename ValueType>
bool LMultiThreadUnorderedMapDynamic<KeyType, ValueType>::HasValue(const KeyType& value_index)
{
	bool if_have_value = false;
	map_use_lock.lock();
	auto data_find = map_value.find(value_index);
	if (data_find != map_value.end())
	{
		if_have_value = true;
	}
	map_use_lock.unlock();
	return if_have_value;
}
