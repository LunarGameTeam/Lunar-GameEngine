#pragma once

#include "core/binding/binding_traits.h"
#include "core/binding/binding.h"
#include "core/reflection/method_traits.h"

#include "core/foundation/string.h"

#include <utility>

namespace luna
{

class LType;

enum class MethodType
{
	StaticFunction,
	MemberFunction
};

struct LArgument
{
	LType* type;
	bool is_const = false;
	bool is_ref = false;
	bool is_pointer = false;
	LString name;
};


class CORE_API LMethod 
{
public:
	LMethod() { mBindingMethodDef.ml_name = nullptr; }

	LString name;

	bool IsStaticMethod() const noexcept
	{
		return wrapper->is_static_method();
	}

	bool IsMemeberMethod() const noexcept
	{
		return wrapper->is_member_method();
	}

	template<typename Ret, typename Cls, typename... Args>
	Ret InvokeMember(Cls* cls, Args... arg)
	{
		using FN = Ret(Cls::*)(Args...);
		static_assert(std::is_pointer<Cls*>::value);
		if (ptr == nullptr)
			ptr = wrapper->get_func_ptr();
		function_pointer_container<FN> p;
		p.ptr = ptr;
		return (cls->*(p.fn))(arg...);
	}

	template<typename Ret, typename... Args>
	Ret InvokeStatic(Args... arg)
	{
		using FN = Ret(*)(Args...);
		if (ptr == nullptr)
			ptr = wrapper->get_func_ptr();
		function_pointer_container<FN> p;
		p.ptr = ptr;
		return (*p.fn)(std::forward<Args>(arg)...);
	}

	bool Valid() const { return ptr != nullptr; }
	inline operator bool() const { return ptr != nullptr; }


	template<auto fn>
	LMethod& Binding()
	{

		using FN = decltype(fn);
		PyCFunction py_func;
		LMethod& func = *this;
		PyMethodDef& def = mBindingMethodDef;
		def.ml_name = name.c_str();

		if constexpr (std::is_member_function_pointer<FN>::value)
		{
			constexpr auto args_count = function_traits<FN>::args_count;
			py_func = binding::pycfunction_select<fn>(std::make_index_sequence<args_count>{});
			def.ml_flags = METH_VARARGS;			
			def.ml_doc = LString::MakeStatic(binding::method_doc<fn>(name));
		}
		else
		{
			constexpr auto args_count = function_traits<FN>::args_count;
			py_func = binding::pycfunction_select<fn>(std::make_index_sequence<args_count>{});
			def.ml_flags = METH_VARARGS | METH_STATIC;			
			def.ml_doc = LString::MakeStatic(binding::method_doc<fn>(name));
		}
		def.ml_meth = py_func;
		return *this;
	}

	void Doc(const char* doc)
	{
		mBindingMethodDef.ml_doc = LString::MakeStatic(doc);
	}

	bool HasBindingMethodDef() {return mBindingMethodDef.ml_name != nullptr ; }
	PyMethodDef& GetBindingMethodDef() { return mBindingMethodDef; };

public:
	PyMethodDef                  mBindingMethodDef;
	std::vector<LType*>          m_args_type;
	LType*                       m_return_type;
	detail::method_wrapper_base* wrapper;
	void* ptr = nullptr;
};


namespace binding
{
template<>
struct binding_converter<LMethod*> : native_converter<LMethod> { };
template<>
struct binding_proxy<LMethod> : native_binding_proxy<LMethod> {};
}

}