#pragma once

#include"uuid.h"

namespace luna
{
//这里强制index的类型为定义类型，防止出现不同index混用导致出错的情况
template<typename IndexValueType>
class LFastUintId
{
	IndexValueType m_id_value;
public:
	LFastUintId(const IndexValueType &id_value)
	{
		m_id_value = id_value;
	};
	~LFastUintId()
	{
	};
	bool operator== (const LFastUintId<IndexValueType> &p) const
	{
		return m_id_value == p.m_id_value;
	}
	template<typename other_type>
	bool operator== (const other_type &p) const
	{
		return m_id_value == p.m_id_value;
	}
	inline const IndexValueType &GetValue() const
	{
		return m_id_value;
	};
};
template<typename IndexValueType, typename IndexDataType>
class LFastUuidMultithread
{
	std::atomic<IndexValueType> uuid_value_self_add;
public:
	LFastUuidMultithread()
	{
		uuid_value_self_add.store(0);
	};
	~LFastUuidMultithread()
	{
	};
	IndexDataType GetUuid()
	{
		auto new_value = uuid_value_self_add.fetch_add(1);
		IndexDataType now_uuid_class(new_value);
		return now_uuid_class;
	}
};
}
#define LInitNewFastIdClass(class_name,index_value_type)\
namespace luna                                                                                           \
{                                                                                                               \
	namespace LSafeIndexType                                                                                    \
	{                                                                                                           \
		class class_name : public luna::LFastUintId<index_value_type>                                    \
		{                                                                                                       \
		public:                                                                                                 \
			class_name(const index_value_type& id_value) : luna::LFastUintId<index_value_type>(id_value) \
			{                                                                                                   \
			}                                                                                                   \
			~class_name()                                                                                       \
			{                                                                                                   \
			}                                                                                                   \
		};                                                                                                      \
		class class_name##IndexGennerator : public luna::LFastUuidMultithread<index_value_type, class_name>  \
		{                                                                                                       \
		public:                                                                                                 \
			class_name##IndexGennerator()                                                                       \
			{                                                                                                   \
			};                                                                                                  \
			~class_name##IndexGennerator()                                                                      \
			{                                                                                                   \
			};                                                                                                  \
		};                                                                                                      \
	}                                                                                                           \
}                                                                                                               \
                                                                                                                \
namespace std                                                                                                   \
{                                                                                                               \
	template <>                                                                                                 \
	struct std::hash<luna::LSafeIndexType::##class_name>                                                 \
	{                                                                                                           \
		std::size_t operator()(const luna::LSafeIndexType::##class_name##& key) const                    \
		{                                                                                                       \
			const index_value_type& now_value = key.GetValue();                                                 \
			return std::hash<index_value_type>()(now_value);                                                    \
		}                                                                                                       \
	};                                                                                                          \
}
