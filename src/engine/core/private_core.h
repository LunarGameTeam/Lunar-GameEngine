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

#include "boost/smart_ptr.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/container/string.hpp"
#include "core/memory/ptr.h"
#include "core/misc/container.h"

#include "core/log/log.h"

#ifndef CORE_API
#define CORE_API __declspec( dllexport )//宏定义
#endif

#define __DEBUG if (GL_NO_ERROR != error) \
	{\
		printf("GL Error %x encountered in.\n", error);\
	};


#define GETTER_REF(type,variable,name) \
	type& Get##name()  \
	{ \
		return variable; \
	};\
	const type& Get##name() const\
	{ \
		return variable; \
	};

#define GETTER(type,variable,name) \
	type Get##name()\
	{ \
		return variable; \
	};\
	type Get##name() const\
	{ \
		return variable; \
	};

#define GET_SET_REF(type,variable,name) \
	type& Get##name()  \
	{ \
		return variable; \
	};\
	void Set##name(type& value) \
	{\
		variable = value;\
	};

#define GET_SET_CONST_REF(type,variable,name) \
	const type& Get##name()  const\
	{ \
		return variable; \
	};\
	void Set##name(type& value) \
	{\
		variable = value;\
	};

#define GET_SET_VAULE(type,variable,name) \
	type Get##name()\
	{ \
		return variable; \
	};\
	type Get##name() const\
	{ \
		return variable; \
	};\
	void Set##name(const type& value) \
	{\
		variable = value;\
	};


extern luna::LogScope E_Core;
