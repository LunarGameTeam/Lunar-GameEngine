#pragma once

#include "binding_python.h"

namespace luna::binding::detail
{

template<typename Args, int Idx>
struct args_traits_impl
{
	using arg_t = std::remove_cvref_t <std::tuple_element_t <Idx - 1, Args >>;

	static void args_list(std::vector<LType*> res)
	{
		LType* type = static_type<arg_t>::StaticType();
		res.push_back(type);
		args_traits_impl<Args, Idx - 2>::args_list(res);
	}

	static LString args_name()
	{
		LString binding_type_name = binding_converter<arg_t> ::binding_fullname();
		LString res = LString::Format(", param{0}: '{1}'", Idx - 1, binding_type_name);
		res = args_traits_impl<Args, Idx - 1>::args_name() + res;
		return res;
	}
};

template<typename Args>
struct args_traits_impl<Args, 1>
{
	using arg_t = std::remove_cvref_t< std::tuple_element_t <0, Args >>;

	static void args_list(std::vector<LType*>& res)
	{
		LType* type = static_type<arg_t>::StaticType();
		res.push_back(type);
	}
	static LString args_name()
	{
		LString binding_type_name = binding_converter<arg_t>::binding_fullname();
		LString res = LString::Format("param{0}: '{1}'", 0, binding_type_name);
		return res;
	}
};

template<typename Args>
struct args_traits_impl<Args, 0>
{
	static void args_list(std::vector<LType*>& res) {	}
	static LString args_name() { return ""; }
};

template<int Idx, class T>
constexpr inline auto arg_get(PyObject* args) noexcept
-> typename std::remove_cvref_t<std::tuple_element_t<Idx, T>>
{
	auto tuple_size = PyTuple_GET_SIZE(args);
	int idx = Idx;
	PyObject* obj = idx < tuple_size ? PyTuple_GetItem(args, idx) : Py_None;
	return binding_converter<std::remove_cvref_t<std::tuple_element_t<Idx, T>>>::from_binding(obj);
}

template<auto fn, size_t ...I>
PyObject* static_wrap_impl(PyObject* self, PyObject* args)
{
	using FN = decltype(fn);
	using return_type = function_traits<FN>::return_type;
	using args_type = function_traits<FN>::args_type;
	if constexpr (std::is_same_v<return_type, void>)
	{
		std::invoke(fn, arg_get<I, args_type>(args)...);
		Py_RETURN_NONE;
	}
	else
	{
		return to_binding<return_type>(std::invoke(fn, arg_get<I, args_type>(args)...));
	}
}

template<auto fn, size_t ...I>
PyObject* wrap_impl(PyObject* self, PyObject* args)
{
	using FN = decltype(fn);
	using return_type = function_traits<FN>::return_type;
	using args_type = function_traits<FN>::args_type;
	using class_type = function_traits<FN>::class_type;
	class_type* cls = (class_type*)binding_converter<class_type*>::from_binding(self);
	if constexpr (std::is_same_v<return_type, void>)
	{
		std::invoke(fn, cls, arg_get<I, args_type>(args)...);
		Py_RETURN_NONE;
	}
	else
	{
		return to_binding<return_type>(std::invoke(fn, cls, arg_get<I, args_type>(args)...));
	}
}
}

namespace luna::binding
{

template<typename Args, int Idx>
struct args_traits_impl;


template<auto fn>
LString method_doc(const char* name)
{
	using FN = decltype(fn);
	using return_type = function_traits<FN>::return_type;
	using args_type = function_traits<FN>::args_type;
	LString format;
	LString return_name = "None";
	if constexpr (!std::is_same_v<return_type, void>)
		return_name = binding_converter<return_type>::binding_fullname();
	if constexpr (function_traits<FN>::args_count != 0)
	{
		LString res = detail::args_traits_impl<args_type, function_traits<FN>::args_count>::args_name();
		format = LString::Format("def {0}(self, {1})->'{2}':", name, res, return_name);
	}
	else
	{
		format = LString::Format("def {0}(self) -> '{1}':", name, return_name);
	}
	return format;
}

template<auto fn>
LString static_method_doc(const char* name)
{
	using FN = decltype(fn);
	using return_type = function_traits<FN>::return_type;
	using args_type = function_traits<FN>::args_type;
	LString return_name = "None";
	if constexpr (!std::is_same_v<return_type, void>)
		return_name = binding_converter<return_type>::binding_fullname();
	LString format;
	if constexpr (function_traits<FN>::args_count != 0)
	{
		LString res = detail::args_traits_impl<args_type, function_traits<FN>::args_count>::args_name();

		format = LString::Format("def {0}({1}) -> '{2}':\n", name, res, return_name);
	}
	else
	{
		format = LString::Format("def {}() -> '{}':\n", name, return_name);
	}
	return format;
}

template<auto fn, size_t ...I>
PyCFunction pycfunction_select(std::index_sequence<I...>)
{
	using FN = decltype(fn);
	if constexpr (std::is_member_function_pointer_v<FN>)
		return (PyCFunction)detail::wrap_impl<fn, I...>;
	else
		return (PyCFunction)detail::static_wrap_impl<fn, I...>;
}

}