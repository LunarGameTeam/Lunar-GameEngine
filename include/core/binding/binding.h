#pragma once

#include "binding_traits.h"
#include "binding_proxy.h"
#include "function_wrap.h"

#include "core/reflection/type_traits.h"

#include <tuple>
#include <type_traits>

namespace luna
{


template<typename T>
bool CheckBindingType(PyObject* obj)
{
	return PyObject_TypeCheck(obj, static_type<T>::StaticType()->GetBindingType());
}

template<typename T>
inline PyObject* to_binding(T t)
{
	return binding::binding_converter<std::remove_cvref_t<T>>::to_binding(std::forward<T>(t));
}

template<typename T>
inline auto from_binding(PyObject* val)
{
	return binding::binding_converter<std::remove_cvref_t<T>>::from_binding(val);
}

template<typename... Args>
PyObject* binding_return(Args... args)
{
	constexpr size_t arg_count = sizeof...(args);
	PyObject* tup = PyTuple_New(arg_count);
	binding::pack_binding_args<sizeof...(args)>(tup, 0, std::forward<Args>(args)...);
	return tup;
}

}
