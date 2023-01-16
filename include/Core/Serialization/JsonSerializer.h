#pragma once


#include "Core/Serialization/JsonDict.h"
#include "Core/Object/BaseObject.h"
#include "Core/Foundation/Container.h"
#include "Core/Math/Math.h"
#include <json/json.h>


namespace luna
{

class LObject;

using FileID = size_t;

struct FileIDManager
{
	FileID Alloc()
	{
		size_t res = mMax;
		mMax++;
		return res;
	}
	FileID mMax = 1;
};

//默认Json
class CORE_API Serializer
{
public:
	virtual bool DeSerialize(LObject* obj) = 0;
	virtual bool Serialize(LObject* obj) = 0;
};


class CORE_API JsonSerializer : public Serializer
{
public:
	JsonSerializer(luna::JsonDict &_dict) :
		mDict(_dict)
	{

	};
	//TODO 优化写法
	virtual bool Serialize(LObject *obj);
	virtual bool DeSerialize(LObject *obj);

private:
	void SerializeProperty(LProperty &prop, LObject *obj, JsonDict &dict);
	void DeserializeProperty(LProperty& prop, LObject* obj, JsonDict& dict);

	FileIDManager mFileIds;

	DoubleConverter<FileID, LObject*> mFileIDMap;
	
	JsonDict mDict;
};

class CORE_API BinarySerializer : public JsonSerializer
{
public:
	BinarySerializer(luna::JsonDict& _dict) : JsonSerializer(_dict)
	{

	};
};

}