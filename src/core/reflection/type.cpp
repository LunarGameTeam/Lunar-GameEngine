#include "core/reflection/type.h"
#include "core/reflection/reflection.h"
#include "core/asset/asset.h"
#include "core/foundation/container.h"
#include "core/memory/ptr_binding.h"

namespace luna
{

CORE_API std::map<LString, LType*>* sTypes = nullptr;
CORE_API std::map<PyTypeObject*, LType*>* sBindingTypes = nullptr;

LType* LType::StaticType()
{	
	static LType* type = NewType("LType", sizeof(LType), nullptr);
	return type;
}

CORE_API PyTypeObject* sMetaType = nullptr;


LType::LType(const LString &name, size_t size, LType *base)
	: mTypeName(name)
	, mTypeSize(size)
	, mBase(base)
{	
	PyMethodDef end = { NULL };
	mMethodDefCache.push_back(end);
}

bool LType::IsSubPtrArray()
{
	return mBase == LType::Get<SubPtrArray>();
}

bool LType::IsDerivedFrom(LType* base)
{
	LType *_base = this;
	if (base == nullptr)
		return false;
	while (_base != nullptr)
	{
		if (_base == base)
			return true;
		_base = _base->mBase;
	}
	return false;
}

bool LType::IsAsset()
{
	return IsDerivedFrom(LType::Get<LBasicAsset>());
}

bool LType::IsObject()
{
	return IsDerivedFrom(LType::Get<LObject>());
}

LMethod &LType::GetMethod(const LString &name)
{
	auto it = mMethods.find(name);
	if (it == mMethods.end())
	{
		LType *base = mBase;
		while (base != nullptr)
		{
			auto it2 = base->mMethods.find(name);
			if (it2 != base->mMethods.end())
				return it2->second;
			base = base->mBase;
		}
	}
	return mMethods[name];
}

const LString& LType::GetBindingFullName()
{
	if (mBindingModule)
	{
		if(mFullName.Length() == 0)
			mFullName = mBindingModule->GetModuleName() + "." + mTypeName;

		return mFullName;
	}
	else
	{
		return mTypeName;
	}
	assert(false);
}

void LType::GetAllProperties(LArray<LProperty*>& result)
{
	LType *base = mBase;
	for (auto &prop : mProperties)
	{
		result.push_back(&prop.second);
	}
	if (base)
		base->GetAllProperties(result);
}

void LType::GenerateBindingDoc()
{
	LType* base = GetBase();
	LString selfType = GetName();
	LString baseType = "object";
	if (base)
		baseType = base->GetBindingFullName();
	LString typeDoc = LString::Format("class {0}({1}):", selfType, baseType);
	typeDoc.AddLine("");
	auto properties = GetBindingGetSet();

	for (PyGetSetDef& it : properties)
	{
		if (it.name)
			typeDoc.AddLine(LString::Format("\t{}", it.doc));
	}

	typeDoc.AddLine("\n\tdef __init__(self):");
	typeDoc.AddLine(LString::Format("\t\tsuper({0}, self).__init__()", selfType));
	
	for (PyMethodDef& it : GetBindingMethods())
	{
		if (it.ml_name)
		{
			typeDoc.AddLine(LString::Format("\t{}", it.ml_doc));
			typeDoc.AddLine("\t\tpass");
		}
	}

	for (LString& it : mExtraDocs)
		typeDoc = LString::Format("{}\n\t{}", typeDoc, it);
		
	mPyType->tp_doc = LString::MakeStatic(typeDoc);
}

LProperty* LType::GetProperty(const char* value)
{
	LProperty* prop = nullptr;
	auto it = mProperties.find(value);
	if (it == mProperties.end())
	{
		LType* base = mBase;
		while (base != nullptr)
		{
			auto it2 = base->mProperties.find(value);
			if (it2 != base->mProperties.end())
			{
				prop = &it2->second;
				break;
			}
			base = base->mBase;
		}
	}
	else
	{
		prop = &it->second;
	}
	return prop;
}

LType *NewType(const char* name, size_t size, LType *base)
{	
	if (sTypes == nullptr)
		sTypes = new std::map<LString, LType *>();
	auto it = sTypes->find(name);
	if (it == sTypes->end())
	{
		LType *type = new LType(name, size, base);
		sTypes->insert(std::make_pair(name, type));
		return type;
	}
	return it->second;	
}

}

