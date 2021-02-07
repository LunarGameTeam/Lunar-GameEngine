#pragma once
#include "core/object/object.h"
#include "core/memory/garbage_colloector.h"
#include "core/log/log.h"
#include "asset.h"

template<typename ObjectDescType>
class CORE_API LTemplateAsset : public luna::LBasicAsset
{
	ObjectDescType m_assert_desc;
public:
	luna::LResult InitCommon() override
	{
		luna::LResult check_error;
		check_error = InitResorceByDesc(m_assert_desc);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return luna::g_Succeed;
	}
	LTemplateAsset(const luna::LString& resource_name_in, const ObjectDescType& assert_desc)
	{
		m_Name = resource_name_in;
		m_assert_desc = assert_desc;
	};
	LTemplateAsset(const luna::LString & resource_name_in, const Json::Value& resource_desc)
	{
		luna::LResult check_error;
		m_Name = resource_name_in;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonMemory<ObjectDescType>(m_assert_desc, resource_desc, m_Name.c_str());
		if (!check_error.m_IsOK)
		{
			return;
		}
	};
	LTemplateAsset(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size)
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
	virtual ~LTemplateAsset() {};
	ObjectDescType GetDesc()
	{
		return m_assert_desc;
	}
private:
	virtual luna::LResult InitResorceByDesc(const ObjectDescType& resource_desc) = 0;
};

template<typename ObjectType>
static ObjectType* LCreateAssetByJson(const luna::LString& resource_name_in, const Json::Value& resource_desc, bool if_have_name)
{
	luna::LResult check_error;
	ObjectType* object_data;
	if (if_have_name)
	{
		object_data = LCreateObjectWithName<ObjectType>(resource_name_in, resource_name_in, resource_desc);
	}
	else
	{
		object_data = LCreateObject<ObjectType>(resource_name_in, resource_desc);
	}
	check_error = object_data->InitResource();
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return object_data;
};
template<typename ObjectType>
static ObjectType* LCreateAssetByBinary(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size, bool if_have_name)
{
	return nullptr;
	//return GrabageColloector::GetInstance()->CreateObject<ObjectType>(resource_name_in, resource_desc, resource_size);
}
template<typename ObjectType, typename ObjectDescType>
static ObjectType* LCreateAssetByDesc(const luna::LString& resource_name_in, const ObjectDescType& resource_desc, bool if_have_name)
{
	luna::LResult check_error;
	ObjectType* object_data;
	if (if_have_name)
	{
		object_data = LCreateObjectWithName<ObjectType>(resource_name_in, resource_name_in, resource_desc);
	}
	else
	{
		object_data = LCreateObject<ObjectType>(resource_name_in, resource_desc);
	}
	check_error = object_data->InitResource();
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return object_data;
};

#define LDefaultAssert(AssertClassName,AssertInitDesc)\
AssertClassName(const luna::LString& resource_name_in, const AssertInitDesc& assert_desc) :LTemplateAsset<AssertInitDesc>(resource_name_in, assert_desc)\
{\
};\
AssertClassName(const luna::LString& resource_name_in, const Json::Value& resource_desc) :LTemplateAsset<AssertInitDesc>(resource_name_in, resource_desc)\
{\
};\
AssertClassName(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size) :LTemplateAsset<AssertInitDesc>(resource_name_in, resource_desc, resource_size)\
{\
};