#include "core/serialization/serialization.h"

#include "core/reflection/type.h"
#include "core/foundation/string.h"
#include "core/math/math.h"
#include "core/asset/json_asset.h"
#include "core/asset/asset_module.h"

#include <algorithm>
#include <queue>

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

		LArray<LProperty*> properties;
		LType* type = top->GetClass();
		type->GetAllProperties(properties);
		for (LProperty* prop : properties)
		{
			LType* propType = prop->GetType();
			if (prop->IsSubPointer())
			{
				if (!propType->GetTemplateArg()->IsAsset())
				{
					LSubPtr& propRef = prop->GetValue<LSubPtr>(top);
					toSerializeObjects.push(propRef.Get());
				}
			}
			else if (propType->IsSubPtrArray())
			{
				TSubPtrArray<LObject>& propValue = prop->GetValue<TSubPtrArray<LObject>>(top);
				for (TSubPtr<LObject>& ptr : propValue)
				{
					LObject* aryElement = ptr.Get();
					toSerializeObjects.push(aryElement);
				}
			}
		}
	}

	std::string rootID = boost::lexical_cast<std::string>(mFileIDMap.Get(root));
	List objectList = mDict.GetList("objects");

	//TODO 优化写法，容错
	int index = 0;
	for (LObject* obj : allObjects)
	{
		LArray<LProperty*> properties;
		Dictionary objectDict = objectList.GetDict(index);
		LType* type = obj->GetClass();
		type->GetAllProperties(properties);
		objectDict.Set("__class__", type->GetName());
		objectDict.Set("__fid__", mFileIDMap.Get(obj));
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
	auto *asset = dynamic_cast<LBasicAsset *>(obj);
	List list = mDict.GetList("objects");
	std::vector<std::pair<LObject*, Dictionary>> datas;
	for (int i = 0; i < list.Size(); ++i)
	{
		Dictionary dict = list.GetDict(i);
		LString typeName = dict.Get<LString>("__class__");
		FileID fid = dict.Get<FileID>("__fid__");
		LType* type = LType::Get(typeName);
		LObject* element;
		if (fid == 1)
			element = obj;
		else
			element = NewObject<LObject>(type);
		mFileIDMap.Set(fid, element);
		datas.emplace_back(element, dict);
	}

	for (std::pair<LObject*, Dictionary>& it : datas)
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

void JsonSerializer::SerializeProperty(LProperty& prop, LObject* target, Dictionary& dict)
{
	if (prop.CheckFlag(PropertyFlag::Transient))
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
				dict.Set(prop.GetNameStr(), mFileIDMap.Get(propRef.Get()));
			}			
		}
	}
	else if (propType->IsSubPtrArray())
	{
		TSubPtrArray<LObject> &propValue = prop.GetValue<TSubPtrArray<LObject>>(target);
		List propList = dict.GetList(prop.GetName());
		int index = 0;
		propList.Resize(propValue.Size());
		for (TSubPtr<LObject> &ptr : propValue)
		{
			Json::Value val;
			Dictionary element_dict(val);
			LObject *aryElement = ptr.Get();
			JsonSerializer elementSerializer(mDict);
			if (aryElement)
			{
				propList.Set(index, mFileIDMap.Get(aryElement));
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
			FileID strUUID = propDict.As<FileID>();
			ptr.SetPtr(mFileIDMap.Get(strUUID));
			
			
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
					FileID strUUID = propList.Get<FileID>(index);					
					ary.PushBack(mFileIDMap.Get(strUUID));
				}
			}
		}

	}
	auto fn = helper[type];
	if (fn)
		fn(prop, obj, propDict);


}

}