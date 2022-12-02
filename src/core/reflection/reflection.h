#pragma once

#include "core/reflection/type_traits.h"
#include "core/reflection/type.h"

#include "core/math/math.h"
#include "core/misc/container.h"
#include <functional>

namespace luna
{

template<typename T>
struct static_type<LSharedPtr<T>>
{
	static LType* StaticType()
	{
		static LType* type = NewTemplateType<T>("SharedPtr", sizeof(LSharedPtr<T>), nullptr);
		return type;
	}
};

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
	static LType* StaticType()
	{
		static LType* type = NewTemplateType<T>("TSubPtr", sizeof(TSubPtr<T>), LType::Get<LSubPtr>());
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
	static LType* StaticType()
	{
		static LType* type = NewTemplateType<T>("TSubPtrArray", sizeof(TSubPtrArray<T>),
			LType::Get<SubPtrArray>());
		return type;
	}
};

template<typename T>
struct static_type<std::function<T>>
{
	static LType* StaticType()
	{
		static LType* type = NewTemplateType<std::function<T>>("Function", sizeof(std::function<T>), nullptr);
		return type;
	}
};

template<>
struct static_type<LType>
{
	static LType* StaticType()
	{
		static LType* type = NewType("LType", sizeof(LType), nullptr);
		return type;
	}
};

class LString;

RegisterType(CORE_API, bool, bool)
RegisterType(CORE_API, const char*, const_char)
RegisterType(CORE_API, int, int)
RegisterType(CORE_API, float, float)
RegisterType(CORE_API, double, double)
RegisterType(CORE_API, LString, LString)

RegisterType(CORE_API, LVector3f, LVector3f)
RegisterType(CORE_API, LVector2f, LVector2f)
RegisterType(CORE_API, LQuaternion, LQuaternion)


}