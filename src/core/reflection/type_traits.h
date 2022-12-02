#pragma once

#include <type_traits>
#include <functional>
#include <tuple> 


namespace luna
{

class LType;

CORE_API LType* NewType(const char* name, size_t size, LType* base);



template<typename> struct raw_class_type;
template<typename> struct member_pointer_traits;

//用来萃取一个类型的LType
template<typename T, class = void>
struct static_type
{
	static LType* StaticType()
	{
		return T::StaticType();
	}
};

template<>
struct static_type<void>
{
	static LType* StaticType() { return nullptr; }
};

struct InvalidType {};

template<>
struct static_type<InvalidType>
{
	static LType* StaticType() { return nullptr; }
};

#define REGISTER_ENUM_TYPE(type, base)\
template<>\
struct static_type<type>\
{\
	static LType *StaticType()\
	{\
		static LType *cls = LType::Get<base>();\
		return cls;\
	}\
};

//注册类（嵌入类声明）
#define RegisterTypeEmbedd(type, super) \
	public:\
		using Super = super;\
		using Self = type;\
		static void InitType(LType* cls);\
		static LType* StaticType(){\
			if(sType)return sType;\
			sType = NewType(#type, sizeof(Self), static_type<Super>::StaticType());\
			InitType(sType);\
			return sType;\
		}\
	protected:\
		static LType* sType;\
	private:\
		friend class LType;\
		static void _InitType(LType* cls);

#define RegisterTypeEmbedd_Imp(type) \
		STATIC_INIT(__##type)\
		{\
			type::StaticType();\
		};\
		LType* type::sType = nullptr;\
		void type::InitType(LType* cls)\


//注册类（非嵌入类声明）
#define RegisterType(API, type, unique_typename) API void init_##unique_typename(LType* cls);\
template<>\
struct API static_type<type>\
{\
	static LType *StaticType()\
	{\
		if(_##unique_typename) return _##unique_typename;\
		_##unique_typename = NewType(#type, sizeof(type), nullptr);\
		init_##unique_typename(_##unique_typename);\
		return _##unique_typename;\
	}\
	inline static LType* _##unique_typename = nullptr;\
};

#define RegisterType_Imp(type, unique_typename) void init_##unique_typename(LType* cls)\


template<typename T> struct remove_all_pointer 
{
	using type = T;
};

template<typename T> struct remove_all_pointer<T*> 
{
	using type = remove_all_pointer<T>::type;
};

template<typename Cls>
struct raw_class_type 
{ 
	using type = std::remove_reference_t<typename remove_all_pointer<Cls>::type>;
};

template<typename T, typename U>
struct member_pointer_traits<T U:: *> 
{
	using class_type = U;
	using member_type = T;
};


template<typename FN, typename Cls>
auto AutoBind(FN fn, Cls* instance)
{
	using func_type = function_traits<FN>::func_type;
	const static int arg_size = function_traits<FN>::args_count;
	func_type res;
	if constexpr (arg_size == 0)
	{
		res = std::bind(fn, instance);
		return res;
	}
	else if constexpr (arg_size == 1)
	{
		res = std::bind(fn, instance, std::placeholders::_1);
		return res;
	}
	else if constexpr (arg_size == 2)
	{
		res = std::bind(fn, instance, std::placeholders::_1, std::placeholders::_2);
		return res;
	}
	else if constexpr (arg_size == 3)
	{
		res = std::bind(fn, instance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		return res;
	}
	else
	{

	}
	return res;
};
}

