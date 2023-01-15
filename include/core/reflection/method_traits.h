#pragma once

#include <type_traits>
#include "core/reflection/type_traits.h"

namespace luna
{

class LType;

template<typename FN>
union function_pointer_container
{
	void* ptr;
	FN fn;
};
template<typename> struct function_traits
{
	using return_type = void;
	using class_type = void;
	using args_type = std::tuple<void>;
	static constexpr std::size_t args_count = 0;
};


// std::function traits
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
	using return_type = Ret;
	using args_type = std::tuple<Args...>;
	using func_type = std::function<Ret(Args...)>;

	static constexpr std::size_t args_count = sizeof...(Args);
};

// static function
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)>
{
	using return_type = Ret;
	using args_type = std::tuple<Args...>;
	using func_type = std::function<Ret(Args...)>;

	static constexpr std::size_t args_count = sizeof...(Args);
};

// member method
template<typename Ret, typename Cls, typename... Args>
struct function_traits<Ret(Cls::*)(Args...)>
{
	using return_type = Ret;
	using class_type = Cls;
	using args_type = std::tuple<Args...>;
	using func_type = std::function<Ret(Args...)>;

	static constexpr std::size_t args_count = sizeof...(Args);
};


}


namespace luna::detail
{


struct method_wrapper_base
{
	virtual bool is_static_method() const noexcept = 0;
	virtual bool is_member_method() const noexcept = 0;
	virtual void* get_func_ptr() const noexcept = 0;
	virtual LType* get_this_type() const noexcept = 0;

};

template<auto fn>
struct method_wrapper : method_wrapper_base
{
	using FN = decltype(fn);

	function_pointer_container<FN> fn_ptr;

	method_wrapper() { fn_ptr.fn = fn; }

	bool is_static_method() const noexcept override
	{
		if constexpr (std::is_member_function_pointer_v<FN>)
			return false;
		else
			return true;
	}

	void* get_func_ptr() const noexcept override
	{
		return fn_ptr.ptr;
	}

	bool is_member_method() const noexcept override
	{
		if constexpr (std::is_member_function_pointer_v<FN>)
			return true;
		else
			return false;
	}

	LType* get_this_type() const noexcept  override
	{
		if constexpr (std::is_member_function_pointer_v<FN>)
			return static_type<function_traits<FN>::class_type>::StaticType();
		else
			return nullptr;
	}
};

}
