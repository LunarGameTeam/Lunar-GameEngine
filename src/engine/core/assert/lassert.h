#pragma once
#include "core/object/object.h"
#include "core/memory/garbage_colloector.h"
#include "core/log/log.h"
class LBasicAsset : public LObject
{	
public:
	LBasicAsset()
	{
	};
	~LBasicAsset()
	{
	};
	//默认加载资源
	luna::LResult InitResource();
	//检查资源的加载状态
	const LLoadState& GetLoadState();
private:
	virtual void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) = 0;
	virtual luna::LResult InitCommon() = 0;
	LLoadState m_object_load_state;
};
template<typename ObjectDescType>
class LTemplateAssert : public LBasicAsset
{
	ObjectDescType m_assert_desc;
public:
	luna::LResult InitCommon() override;
	LTemplateAssert(const luna::LString& resource_name_in, const ObjectDescType& assert_desc)
	{
		m_Name = resource_name_in;
		m_assert_desc = assert_desc;
	};
	LTemplateAssert(const luna::LString & resource_name_in, const Json::Value& resource_desc)
	{
		luna::LResult check_error;
		m_Name = resource_name_in;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonMemory<ObjectDescType>(m_assert_desc, resource_desc, m_Name.c_str());
		if (!check_error.m_IsOK)
		{
			return;
		}
	};
	LTemplateAssert(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size)
	{
		luna::LResult check_error;
		m_Name = resource_name_in;
		//todo:二进制序列化
		//check_error = LSrtructReflectControl::GetInstance()->DeSerializeFromBinary<ObjectDescType>(m_assert_desc, resource_desc, m_Name.c_str());
		if (!check_error.m_IsOK)
		{
			return;
		}
	};
	virtual ~LTemplateAssert() {};
private:
	virtual luna::LResult InitResorceByDesc(const ObjectDescType& resource_desc) = 0;
};
template<typename ObjectDescType>
luna::LResult LTemplateAssert<ObjectDescType>::InitCommon()
{
	luna::LResult check_error;
	check_error = InitResorceByDesc(m_assert_desc);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}

template<typename ObjectType>
static ObjectType* LCreateAssetByJson(const luna::LString& resource_name_in, const Json::Value& resource_desc)
{
	luna::LResult check_error;
	ObjectType* object_data = luna::GrabageColloector::GetInstance()->CreateObject<ObjectType>(resource_name_in, resource_desc);
	check_error = object_data->InitResource();
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return object_data;
};
template<typename ObjectType>
static ObjectType* LCreateAssetByBinary(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size)
{
	return GrabageColloector::GetInstance()->CreateObject<ObjectType>(resource_name_in, resource_desc, resource_size);
}