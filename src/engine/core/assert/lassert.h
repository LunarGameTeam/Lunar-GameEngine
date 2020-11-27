#pragma once
#include "core/object/object.h"
#include "core/memory/garbage_colloector.h"
class LBasicAsset : public LObject
{
public:
	LBasicAsset()
	{
	};
	~LBasicAsset()
	{
	};
};
template<typename ObjectDescType>
class LTemplateAssert : public LBasicAsset
{
	ObjectDescType m_assert_desc;
public:
	LunarEngine::LResult InitCommon() override;
	LTemplateAssert(const LunarEngine::LString& resource_name_in, const ObjectDescType& assert_desc)
	{
		m_Name = resource_name_in;
		m_assert_desc = assert_desc;
	};
	LTemplateAssert(const LunarEngine::LString & resource_name_in, const Json::Value& resource_desc)
	{
		LunarEngine::LResult check_error;
		m_Name = resource_name_in;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonMemory<ObjectDescType>(m_assert_desc, resource_desc, m_Name.c_str());
		if (!check_error.m_IsOK)
		{
			return;
		}
	};
	LTemplateAssert(const LunarEngine::LString& resource_name_in, const void* resource_desc, const size_t& resource_size)
	{
		LunarEngine::LResult check_error;
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
	virtual LunarEngine::LResult InitResorceByDesc(const ObjectDescType& resource_desc) = 0;
};
template<typename ObjectDescType>
LunarEngine::LResult LTemplateAssert<ObjectDescType>::InitCommon()
{
	LunarEngine::LResult check_error;
	check_error = InitResorceByDesc(m_assert_desc);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return LunarEngine::g_Succeed;
}

template<typename ObjectType>
static ObjectType* LCreateAssetByJson(const LunarEngine::LString& resource_name_in, const Json::Value& resource_desc)
{
	return LResourceController::GetInstance()->CreateObject<ObjectType>(resource_name_in, resource_desc);
};
template<typename ObjectType>
static ObjectType* LCreateAssetByBinary(const LunarEngine::LString& resource_name_in, const void* resource_desc, const size_t& resource_size)
{
	return LResourceController::GetInstance()->CreateObject<ObjectType>(resource_name_in, resource_desc, resource_size);
}