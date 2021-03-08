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
#ifndef CORE_API
#ifdef CORE_EXPORT
#define CORE_API __declspec( dllexport )//宏定义
#define CORE_TEMPLATE __declspec( dllexport )//宏定义
#else
#define CORE_API __declspec( dllimport )
#define CORE_TEMPLATE
#endif

#endif

#define __DEBUG if (GL_NO_ERROR != error) \
	{\
		printf("GL Error %x encountered in.\n", error);\
	};

/*
 *	Getter和Setter宏
 */

#define GETTER_REF(type,var,name) \
	type& Get##name()  \
	{ \
		return var; \
	};\
	const type& Get##name() const\
	{ \
		return var; \
	};

#define GETTER_REF_CONST(type,var,name) \
	const type& Get##name()  \
	{ \
		return var; \
	};\
	const type& Get##name() const\
	{ \
		return var; \
	};

#define GETTER(type,var,name) \
	type Get##name()\
	{ \
		return var; \
	};\
	type Get##name() const\
	{ \
		return var; \
	};

#define SETTER_REF(type, var, name)\
	void Set##name(const type& value) \
	{\
		var = value;\
	};

#define SETTER(type, var, name)\
	void Set##name(type value) \
	{\
		var = value;\
	};


#define GET_SET_VALUE(type,var,name) \
	GETTER(type, var, name)\
	SETTER(type, var, name)

#define GET_SET_REF(type, var, name) \
	GETTER_REF(type, var, name)  \
	SETTER_REF(type, var, name)


#define GET_SET_REF_CONST(type, var, name) \
	GETTER_REF_CONST(type, var, name)  \
	SETTER_REF(type, var, name)


namespace luna
{

struct LogScope;

}

CORE_API extern luna::LogScope E_Core;
