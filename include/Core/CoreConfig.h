#pragma once
/*!
 * \file private_core.h
 * \date 2020/08/09 10:05
 *
 * \author IsakWong
 *
 * \brief Core模块内部的预处理头文件，处理一些Core模块公共的内部定义
 *
 *
 *
 * \note
*/

#include <cstddef>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#ifndef CORE_API
#ifdef CORE_EXPORT
#define CORE_API __declspec( dllexport )//宏定义
#define CORE_TEMPLATE __declspec( dllexport )//宏定义
#else
#define CORE_API __declspec( dllimport )
#define CORE_TEMPLATE
#endif
#endif

#include "tracy/Tracy.hpp"
#include <chrono>
/*
 *	Getter和Setter宏
 */

#define GETTER_REF(type,var,name) \
	const type& Get##name() const\
	{ \
		return var; \
	};

#define GETTER_REF_CONST(type,var,name) \
	const type& Get##name() const\
	{ \
		return var; \
	};

#define GETTER(type,var,name) \
	type Get##name() const\
	{ \
		return var; \
	};

#define SETTER_REF(type, var, name)\
	void Set##name(const type& value) \
	{\
		var = value;\
	};

#define SETTER_REF_DIRTY(type, var, name, dirty_var)\
	void Set##name(const type& value) \
	{\
		if(var == value)\
			return;\
		var = value; \
		dirty_var = true;\
	};

#define SETTER_DIRTY(type, var, name, dirty_var)\
	void Set##name(type value) \
	{\
		if(var == value)\
			return;\
		var = value; \
		dirty_var = true;\
	};

#define SETTER(type, var, name)\
	void Set##name(type value) \
	{\
		var = value;\
	};

#define GET_SET_VALUE(type,var,name) \
	GETTER(type, var, name)\
	SETTER(type, var, name)

#define GET_SET_VALUE_DIRTY(type,var,name,dirty_var) \
	GETTER(type, var, name)\
	SETTER_DIRTY(type, var, name, dirty_var)

#define GET_SET_REF(type, var, name) \
	GETTER_REF(type, var, name)  \
	SETTER_REF(type, var, name)

#define GET_SET_REF_DIRTY(type, var, name, dirty_var) \
	GETTER_REF(type, var, name)  \
	SETTER_REF_DIRTY(type, var, name, dirty_var)

#define GET_SET_REF_CONST(type, var, name) \
	GETTER_REF_CONST(type, var, name)  \
	SETTER_REF(type, var, name)

//无序的Init, 在静态数据初始化的时候init，注意顺序不确定
#define STATIC_INIT(name) \
	struct __##name{\
		__##name();\
	\
	};\
	static __##name _##name;\
	__##name::__##name()\

typedef unsigned char byte;


// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/

template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum> operator |(Enum lhs, Enum rhs)
{
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(lhs) |
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
}


template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, bool> Has(Enum lhs, Enum rhs)
{
	return static_cast<bool>(
		( static_cast<std::underlying_type_t<Enum>>(lhs) &
		static_cast<std::underlying_type_t<Enum>>(rhs) ) != 0
		);
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum> operator &(Enum lhs, Enum rhs)
{
	return static_cast<Enum> (
		static_cast<std::underlying_type_t<Enum>>(lhs) &
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
}
template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, bool> operator ==(Enum lhs, Enum rhs)
{
	return static_cast<bool>(
		static_cast<std::underlying_type_t<Enum>>(lhs) ==
		static_cast<std::underlying_type_t<Enum>>(rhs)
	);
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum> operator ^(Enum lhs, Enum rhs)
{
	return static_cast<Enum> (
		static_cast<std::underlying_type_t<Enum>>(lhs) ^
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum> operator ~(Enum rhs)
{
	return static_cast<Enum> (
		~static_cast<std::underlying_type_t<Enum>>(rhs)
		);
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>& operator |=(Enum& lhs, Enum rhs)
{
	lhs = static_cast<Enum> (
		static_cast<std::underlying_type_t<Enum>>(lhs) |
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
	return lhs;
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>& operator &=(Enum& lhs, Enum rhs)
{
	lhs = static_cast<Enum> (
		static_cast<std::underlying_type_t<Enum>>(lhs) &
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
	return lhs;
}

template<typename Enum>
inline std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>& operator ^=(Enum& lhs, Enum rhs)
{
	lhs = static_cast<Enum> (
		static_cast<std::underlying_type_t<Enum>>(lhs) ^
		static_cast<std::underlying_type_t<Enum>>(rhs)
		);
	return lhs;
}

#pragma region Legacy Smart Pointer 

template<typename T>
using LSharedPtr = std::shared_ptr<T>;

template<typename T>
using LWeakPtr = std::weak_ptr<T>;

template<typename T>
using LUniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
inline LSharedPtr<T> MakeShared(Args && ... args)
{
	return std::make_shared<T>(args...);
}

#include "Core/Foundation/Log.h"

#ifdef NDEBUG
#define LUNA_ASSERT(exp) if(!(exp)) LogError("Assert", "File:{} Line:{}", __FILE__, __LINE__);
#else
#define LUNA_ASSERT(exp) assert(exp);
#endif


#pragma endregion