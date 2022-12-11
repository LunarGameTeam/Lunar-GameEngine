#include "core/serialization/serialization.h"

#include "core/foundation/string.h"
#include "core/math/math.h"
#include "core/asset/json_asset.h"
#include "core/asset/asset_module.h"

#include <algorithm>

namespace luna
{
template<typename T>
static void PropertyDeserializeHelper(LProperty &prop, LObject *obj, Dictionary &dict)
{
	LType *type = obj->GetClass();
	prop.SetValue(obj, dict.As<T>());
}

template<typename T>
static void PropertySerializeHelper(LProperty &prop, LObject *obj, Dictionary &dict)
{
	LType *type = obj->GetClass();
	T &val = prop.GetValue<T>(obj);
	dict.Set<T>(prop.GetName(), val);
}

bool JsonSerializer::Serialize(LObject *obj)
{
	LUuid uuid = obj->GetUUID();
	std::string strUUID = boost::lexical_cast<std::string>(uuid);
	auto it = mDatas.find(uuid);
	Dictionary propDict = mDict.GetDict(strUUID.c_str());

	LType *type = obj->GetClass();
	//TODO 优化写法，容错
	LVector<LProperty*> properties;
	type->GetAllProperties(properties);
	propDict.Set("type_name", type->GetName());
	for (LProperty* it : properties)
	{
		LProperty &prop = *it;
		SerializeProperty(prop, obj, propDict);
	}
	auto s = mDict.ToString();
	auto s1 = propDict.ToString();
	return true;
}

bool JsonSerializer::DeSerialize(LObject *obj)
{
	auto *asset = dynamic_cast<LBasicAsset *>(obj);
	Json::Value& value = mDict.GetJsonValue();

	{
		LUuid uuid = obj->GetUUID();
		mObjects[uuid] = obj;
	}	

	//
	
	for (auto it = value.begin(); it != value.end(); ++it)
	{
		std::string strUUID = it.key().as<std::string>();
		LUuid uuid = boost::lexical_cast<LUuid>(strUUID);
		Json::Value &val = *it;
		Dictionary dict(val);
		LString typeName = dict.Get<LString>("type_name");
		LType *type = LType::Get(typeName);
		auto objIt = mObjects.find(uuid);
		if (objIt == mObjects.end())
		{
			LObject* element = NewObject<LObject>(type);
			assert(element != nullptr);
			element->GenerateUUID(uuid);
			mObjects[uuid] = element;
		}
	}

	//
	for (auto it : mObjects)
	{
		LString strUUID = boost::uuids::to_string(it.first);
		LObject* object = it.second;
		LType* type = object->GetClass();		
		//TODO 优化写法，容错
		LVector<LProperty*> properties;
		type->GetAllProperties(properties);
		for (LProperty *propIT : properties)
		{
			LProperty &prop = *propIT;
			Dictionary dict = mDict.GetDict(strUUID);
			auto s = dict.ToString();
			DeserializeProperty(prop, object, dict);
		}
	}
	asset->OnLoad();
	//TODO 优化写法，容错
	return true;
}

void JsonSerializer::SerializeProperty(LProperty& prop, LObject* target, Dictionary& dict)
{
	if (prop.CheckMeta(MetaInfo::Transient))
		return;
	static std::map<LType *, void(*)(LProperty &prop, LObject *target, Dictionary &dict)> helper = {
		{LType::Get<float>(),  &PropertySerializeHelper<float>},
		{LType::Get<int>(),  &PropertySerializeHelper<int>},
		{LType::Get<LVector3f>(),  &PropertySerializeHelper<LVector3f>},
		{LType::Get<LVector2f>(),  &PropertySerializeHelper<LVector2f>},
		{LType::Get<LString>(),  &PropertySerializeHelper<LString>},
		{LType::Get<LQuaternion>(),  &PropertySerializeHelper<LQuaternion>},
	};
	LType *objectType = target->GetClass();
	LType *propType = prop.GetType();
	if (prop.IsSubPointer())
	{
		if (propType->GetTemplateArg()->IsAsset())
		{
			LSubPtr &ptr = prop.GetValue<LSubPtr>(target);
			if (ptr.Get() == nullptr)
			{
				dict.Set(prop.GetName(), "");
			}
			else
			{
				LBasicAsset *asset = dynamic_cast<LBasicAsset *>(ptr.Get());
				dict.Set(prop.GetName(), asset->GetAssetPath());
			}
		}
		else
		{
			LSubPtr &propRef = prop.GetValue<LSubPtr>(target);
			JsonSerializer propSerializer(mDict);
			if (propRef.Get())
			{
				propSerializer.Serialize(propRef.Get());
				dict.Set(prop.GetNameStr(), propRef.Get()->GetStrUUID());
			}			
		}
	}
	else if (propType->IsSubPtrArray())
	{
		TSubPtrArray<LObject> &propValue = prop.GetValue<TSubPtrArray<LObject>>(target);
		List propList = dict.GetList(prop.GetName());
		int index = 0;
		for (TSubPtr<LObject> &ptr : propValue)
		{
			Json::Value val;
			Dictionary element_dict(val);
			LObject *aryElement = ptr.Get();
			JsonSerializer elementSerializer(mDict);
			if (aryElement)
			{
				elementSerializer.Serialize(aryElement);
				propList.Set(index, aryElement->GetStrUUID());
			}
			else
				propList.Set(index, Json::nullValue);
			index++;
		}
		dict.Set(prop.GetNameStr(), propList);
	}
	auto fn = helper[propType];
	if (fn)
		fn(prop, target, dict);
}

void JsonSerializer::DeserializeProperty(LProperty &prop, LObject *obj, Dictionary &dict)
{
	static std::map<LType *, void(*)(LProperty &prop, LObject *obj, Dictionary &dict)> helper = {
		{LType::Get<float>(),  &PropertyDeserializeHelper<float>},
		{LType::Get<int>(),  &PropertyDeserializeHelper<int>},
		{LType::Get<LVector3f>(),  &PropertyDeserializeHelper<LVector3f>},
		{LType::Get<LVector2f>(),  &PropertyDeserializeHelper<LVector2f>},
		{LType::Get<LString>(),  &PropertyDeserializeHelper<LString>},
		{LType::Get<LQuaternion>(),  &PropertyDeserializeHelper<LQuaternion>},
	};
	
	LType *obj_type = obj->GetClass();
	LType *type = prop.GetType();
	if (!dict.Has(prop.GetNameStr()))
		return;
	Dictionary propDict = dict.GetDict(prop.GetNameStr());
	if (prop.IsSubPointer())
	{
		if (type->GetTemplateArg()->IsDerivedFrom(LType::Get<LBasicAsset>()))
		{
			LSubPtr &ptr = prop.GetValue<LSubPtr>(obj);
			LString assetPath = propDict.As<LString>();
			if(assetPath != "")
			{
				LBasicAsset *asset = sAssetModule->LoadAsset(assetPath, type->GetTemplateArg());				
				ptr.SetPtr(asset);
			}
			else
				ptr.SetPtr(nullptr);
		}
		else 
		{
			LSubPtr &ptr = prop.GetValue<LSubPtr>(obj);
			LString strUUID = propDict.As<LString>();
			if (strUUID == "")
			{
				ptr.SetPtr(nullptr);
			}
			else 
			{
				LUuid uuid = boost::lexical_cast<LUuid>(strUUID);
				ptr.SetPtr(GetObject(uuid));
			}
			
		}
	}
	else if (type->IsSubPtrArray())
	{
		TSubPtrArray<LObject> &ary = prop.GetValue<TSubPtrArray<LObject>>(obj);
		if (propDict)
		{

			List propList = propDict.As<List>();

			if (type->GetTemplateArg()->IsDerivedFrom(LType::Get<LBasicAsset>()))
			{
				for (int idx = 0; idx < propList.Size(); idx++)
				{
					LString assetPath = propList.Get<LString>(idx);
					ary.PushBack(sAssetModule->LoadAsset(assetPath, type->GetTemplateArg()));
				}
			}
			else
			{
				for (int index = 0; index < propList.Size(); index++)
				{
					LString strUUID = propList.Get<LString>(index);
					LUuid uuid = boost::lexical_cast<LUuid>(strUUID);
					ary.PushBack(GetObject(uuid));
				}
			}
		}

	}
	auto fn = helper[type];
	if (fn)
		fn(prop, obj, propDict);


}

}