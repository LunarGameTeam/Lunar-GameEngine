#pragma once

#include "binding_traits.h"
#include "binding_proxy.h"
#include "function_wrap.h"

#include <tuple>
#include <type_traits>

namespace luna
{



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


}
