#pragma once


#include "core/serialization/dict_data.h"
#include "core/object/base_object.h"
#include "core/foundation/container.h"
#include "core/math/math.h"
#include <json/json.h>


namespace luna
{

class LObject;

class CORE_API JsonSerializer : public ISerializer
{
public:
	JsonSerializer(luna::Dictionary &_dict) :
		mDict(_dict)
	{

	};
	//TODO 优化写法
	virtual bool Serialize(LObject *obj);
	virtual bool DeSerialize(LObject *obj);

private:
	LObject* GetObject(const LUuid& uuid)
	{
		auto it = mObjects.find(uuid);
		if (it != mObjects.end())
			return it->second;
		return nullptr;
	}
	void SerializeProperty(LProperty &prop, LObject *obj, Dictionary &dict);
	void DeserializeProperty(LProperty& prop,LObject* obj, Dictionary &dict);
	LMap<LUuid, Dictionary> mDatas;
	LMap<LUuid, LObject*> mObjects;
	Dictionary mDict;
};

class CORE_API BinarySerializer : public JsonSerializer
{
public:
	BinarySerializer(luna::Dictionary& _dict) : JsonSerializer(_dict)
	{

	};
};

}