#pragma once

#include "core/reflection/type_traits.h"
#include "core/reflection/type.h"

#include "core/math/math.h"
#include "core/foundation/container.h"
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

RegisterType(CORE_API, LVector2f, LVector2f)
RegisterType(CORE_API, LVector3f, LVector3f)
RegisterType(CORE_API, LVector4f, LVector4f)
RegisterType(CORE_API, LQuaternion, LQuaternion)


}