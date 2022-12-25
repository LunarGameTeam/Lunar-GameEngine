#pragma once


#include "core/reflection/reflection.h"
#include "core/binding/binding_traits.h"
#include "core/memory/ptr.h"

namespace luna
{


template<>
struct static_type<LSubPtr>
{
	static LType* StaticType()
	{
		static LType* type = NewType("LSubPtr", sizeof(LSubPtr), nullptr);
		return type;
	}
};

template<typename T>
struct static_type<TSubPtr<T>>
{
	static LType* InitTSubPtrType()
	{
		LType* type = NewTemplateType<T>("TSubPtr", sizeof(TSubPtr<T>), LType::Get<LSubPtr>());
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
struct static_type<SubPtrArray>
{
	static LType* StaticType()
	{
		static LType* type = NewType("SubPtrArray", sizeof(SubPtrArray),
									 nullptr);
		return type;
	}
};

template<typename T>
struct static_type<TSubPtrArray<T>>
{
	static LType* InitTSubPtrType()
	{
		LType* type = NewTemplateType<T>("TSubPtrArray", sizeof(TSubPtrArray<T>), LType::Get<SubPtrArray>());
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