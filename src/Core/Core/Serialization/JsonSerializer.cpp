#include "Core/Serialization/JsonSerializer.h"

#include "Core/Reflection/Type.h"
#include "Core/Foundation/String.h"
#include "Core/Math/Math.h"
#include "Core/Asset/JsonAsset.h"
#include "Core/Asset/AssetModule.h"

#include <algorithm>
#include <queue>

namespace luna
{
template<typename T>
static void PropertyDeserializeHelper(LProperty &prop, LObject *obj, JsonDict &dict)
{
	LType *type = obj->GetClass();
	prop.SetValue(obj, dict.As<T>());
}

template<typename T>
static void PropertySerializeHelper(LProperty &prop, LObject *obj, JsonDict &dict)
{
	LType *type = obj->GetClass();
	T &val = prop.GetValue<T>(obj);
	dict.Set<T>(prop.GetName(), val);
}

bool JsonSerializer::Serialize(LObject *root)
{

	std::stack<LObject*> toSerializeObjects;
	std::vector<LObject*> allObjects;

	toSerializeObjects.push(root);

	while (!toSerializeObjects.empty())
	{
		LObject* top = toSerializeObjects.top();		
		toSerializeObjects.pop();
		allObjects.push_back(top);
		FileID fid = mFileIds.Alloc();
		mFileIDMap.Set(fid, top);
		top->ForEachSubObject([&](size_t idx, LObject* subobject)
		{
			toSerializeObjects.push(subobject);
		});
		LArray<LProperty*> properties;
		LType* type = top->GetClass();
		type->GetAllProperties(properties);		
	}

	std::string rootID = boost::lexical_cast<std::string>(mFileIDMap.Get(root));
	JsonList objectList = mDict.GetList("objects");

	//TODO 优化写法，容错
	int index = 0;
	for (LObject* obj : allObjects)
	{
		LArray<LProperty*> properties;
		JsonDict objectDict = objectList.GetDict(index);
		LType* type = obj->GetClass();
		type->GetAllProperties(properties);
		objectDict.Set("__class__", type->GetName());
		objectDict.Set("__fid__", mFileIDMap.Get(obj));
		objectDict.Set("__parent__", mFileIDMap.Get(obj->GetParent()));
		objectDict.Set("__index__", obj->Index());
		for (LProperty* prop : properties)
		{			
			SerializeProperty(*prop, obj, objectDict);
		}
		index++;
	}
	return true;
}
bool JsonSerializer::DeSerialize(LObject *obj)
{
	auto *asset = dynamic_cast<Asset *>(obj);
	JsonList list = mDict.GetList("objects");
	std::vector<std::pair<LObject*, JsonDict>> datas;
	for (auto i = 0; i < list.Size(); ++i)
	{
		JsonDict dict = list.GetDict(i);
		LString typeName = dict.Get<LString>("__class__");
		FileID fid = dict.Get<FileID>("__fid__");
		FileID parentFid = dict.Get<FileID>("__parent__");
		size_t index = dict.Get<size_t>("__index__");
		LType* type = LType::Get(typeName);
		LObject* element;
		if (fid == 1)
			element = obj;
		else
		{
			element = NewObject<LObject>(type);
			LObject* parent = mFileIDMap.Get(parentFid);
			element->SetParent(parent);
			auto newIndex = element->Index();
			//assert(newIndex == index);
		}
		mFileIDMap.Set(fid, element);
		datas.emplace_back(element, dict);
	}

	for (std::pair<LObject*, JsonDict>& it : datas)
	{		
		LObject* object = it.first;
		LType* type = object->GetClass();		
		//TODO 优化写法，容错
		LArray<LProperty*> properties;
		type->GetAllProperties(properties);
		for (LProperty *propIT : properties)
		{
			LProperty &prop = *propIT;
			DeserializeProperty(prop, object, it.second);
		}
	}
	asset->OnLoad();
	//TODO 优化写法，容错
	return true;
}

void JsonSerializer::SerializeProperty(LProperty& prop, LObject* target, JsonDict& dict)
{
	if (!prop.CheckFlag(PropertyFlag::Serializable))
		return;
	static std::map<LType *, void(*)(LProperty &prop, LObject *target, JsonDict &dict)> helper = {
		{LType::Get<float>(),  &PropertySerializeHelper<float>},
		{LType::Get<bool>(),  &PropertySerializeHelper<bool>},
		{LType::Get<int>(),  &PropertySerializeHelper<int>},
		{LType::Get<LVector4f>(),  &PropertySerializeHelper<LVector4f>},
		{LType::Get<LVector3f>(),  &PropertySerializeHelper<LVector3f>},
		{LType::Get<LVector2f>(),  &PropertySerializeHelper<LVector2f>},
		{LType::Get<LString>(),  &PropertySerializeHelper<LString>},
		{LType::Get<LQuaternion>(),  &PropertySerializeHelper<LQuaternion>},
	};
	LType *objectType = target->GetClass();
	LType *propType = prop.GetType();
	if (propType->IsPPtr())
	{
		PPtr& propRef = prop.GetValue<PPtr>(target);
		JsonSerializer propSerializer(mDict);
		if (propRef.Get())
		{
			dict.Set(prop.GetNameStr(), mFileIDMap.Get(propRef.Get()));
		}
	}
	else if (propType->IsAssetPtr())
	{
		SharedPtr<Asset>& ptr = prop.GetValue<SharedPtr<Asset>>(target);
		if (ptr.get() == nullptr)
		{
			dict.Set(prop.GetName(), "");
		}
		else
		{
			Asset* asset = dynamic_cast<Asset*>(ptr.get());
			dict.Set(prop.GetName(), asset->GetAssetPath());
		}
	}
	else if (propType->IsAssetPTRArray())
	{
		TAssetPtrArray<Asset>& propValue = prop.GetValue<TAssetPtrArray<Asset>>(target);

		JsonList propList = dict.GetList(prop.GetName());
		int index = 0;
		propList.Resize(propValue.size());
		for (auto& ptr : propValue)
		{
			propList.Set(index, ptr->GetAssetPath());
			dict.Set(prop.GetNameStr(), propList);
		}
		
	}
	else if (propType->IsPPtrArray())
	{
		TPPtrArray<LObject> &propValue = prop.GetValue<TPPtrArray<LObject>>(target);
		JsonList propList = dict.GetList(prop.GetName());
		int index = 0;
		propList.Resize(propValue.Size());
		for (auto& ptr : propValue)
		{
			Json::Value val;
			JsonDict element_dict(val);
			JsonSerializer elementSerializer(mDict);
			LObject* aryElement = ptr.Get();
			if (aryElement)
			{
				propList.Set(index, mFileIDMap.Get(aryElement));
			}
			else
			{
				propList.Set(index, 0);
			}
			index++;
		}
		dict.Set(prop.GetNameStr(), propList);
	}
	auto fn = helper[propType];
	if (fn)
		fn(prop, target, dict);
}

void JsonSerializer::DeserializeProperty(LProperty &prop, LObject *obj, JsonDict &dict)
{
	if (!prop.CheckFlag(PropertyFlag::Serializable))
		return;
	static std::map<LType *, void(*)(LProperty &prop, LObject *obj, JsonDict &dict)> helper = {
		{LType::Get<float>(),  &PropertyDeserializeHelper<float>},
		{LType::Get<int>(),  &PropertyDeserializeHelper<int>},
		{LType::Get<bool>(),  &PropertyDeserializeHelper<bool>},
		{LType::Get<LVector4f>(),  &PropertyDeserializeHelper<LVector4f>},
		{LType::Get<LVector3f>(),  &PropertyDeserializeHelper<LVector3f>},
		{LType::Get<LVector2f>(),  &PropertyDeserializeHelper<LVector2f>},
		{LType::Get<LString>(),  &PropertyDeserializeHelper<LString>},
		{LType::Get<LQuaternion>(),  &PropertyDeserializeHelper<LQuaternion>},
	};
	
	LType *obj_type = obj->GetClass();
	LType *type = prop.GetType();
	if (!dict.Has(prop.GetNameStr()))
		return;
	JsonDict propDict = dict.GetDict(prop.GetNameStr());

	if (type->IsPPtr())
	{
		assert(type->GetTemplateArg()->IsDerivedFrom(LType::Get<LObject>()));
		PPtr& ptr = prop.GetValue<PPtr>(obj);
		FileID strUUID = propDict.As<FileID>();
		ptr.SetPtr(mFileIDMap.Get(strUUID));
		return;		
	}
	else if (type->IsPPtrArray())
	{
		TPPtrArray<LObject>& ary = prop.GetValue<TPPtrArray<LObject>>(obj);

		if (propDict)
		{
			JsonList propList = propDict.As<JsonList>();
			for (uint32_t index = 0; index < propList.Size(); index++)
			{
				FileID strUUID = propList.Get<FileID>(index);
				ary.PushBack(mFileIDMap.Get(strUUID));
			}
		}		
		return;
	}
	else if (type->IsAssetPtr())
	{
		SharedPtr<Asset>& ptr = prop.GetValue<SharedPtr<Asset>>(obj);
		LString assetPath = propDict.As<LString>();
		auto setter = prop.GetSetter();
		SharedPtr<Asset> asset = nullptr;
		assert(type->GetTemplateArg()->IsDerivedFrom(LType::Get<Asset>()));
		if (assetPath != "")
			asset = sAssetModule->LoadAsset(assetPath, type->GetTemplateArg());

		if (setter)
		{
			setter.InvokeMember<void, LObject, Asset*>(obj, asset.get());
		}
		else
		{
			ptr = asset;
		}		
		return;
	}else if(type->IsAssetPTRArray())
	{
		TAssetPtrArray<Asset>& ary = prop.GetValue<TAssetPtrArray<Asset>>(obj);
		if (propDict)
		{
			JsonList propList = propDict.As<JsonList>();			
			for (uint32_t idx = 0; idx < propList.Size(); idx++)
			{
				LString assetPath = propList.Get<LString>(idx);
				ary.push_back(sAssetModule->LoadAsset(assetPath, type->GetTemplateArg()));
			}			
		}
		return;
	}
	
	auto fn = helper[type];
	if (fn)
		fn(prop, obj, propDict);
	else
		LUNA_ASSERT(false);


}

}