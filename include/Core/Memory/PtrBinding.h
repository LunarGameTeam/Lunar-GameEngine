#pragma once


#include "Core/Reflection/Reflection.h"
#include "Core/Scripting/BindingTraits.h"
#include "Core/Memory/Ptr.h"

namespace luna
{


template<>
struct static_type<PPtr>
{
	static LType* StaticType()
	{
		static LType* type = NewType("PPtr", sizeof(PPtr), nullptr);
		return type;
	}
};

template<typename T>
struct static_type<TPPtr<T>>
{
	static LType* InitTSubPtrType()
	{
		LType* type = NewTemplateType<T>("TPPtr", sizeof(TPPtr<T>), LType::Get<PPtr>());
		type->mPyType = binding::binding_type<T>::StaticType();
		return type;
	}
	static LType* StaticType()
	{
		static LType* type = InitTSubPtrType();
		return type;
	}
};

template<>
struct static_type<PPtrArray>
{
	static LType* StaticType()
	{
		static LType* type = NewType("PPtrArray", sizeof(PPtrArray), nullptr);
		return type;
	}
};

template<typename T>
struct static_type<TPPtrArray<T>>
{
	static LType* InitTSubPtrType()
	{
		LType* type = NewTemplateType<T>("TPPtrArray", sizeof(TPPtrArray<T>), LType::Get<PPtrArray>());
		Py_XINCREF(&PyList_Type);
		type->mPyType = &PyList_Type;
		return type;
	}
	static LType* StaticType()
	{
		static LType* type = InitTSubPtrType();
		return type;
	}
};


}