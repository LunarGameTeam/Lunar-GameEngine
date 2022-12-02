/*!
 * \file core.h
 * \date 2020/08/09 10:11
 *
 * \author isAk wOng
 *
 * \brief 提供给其他模块包含的头文件，会包含本模块的基本内容和接口
 *
 * TODO: long description
 *
 * \note
*/
#pragma once


#include "core/framework/module.h"
#include "core/core_library.h"
#include "core/framework/luna_core.h"

#include "core/object/base_object.h"
#include "core/log/log.h"

#include "core/misc/misc.h"
#include "core/misc/container.h"
#include "core/misc/signal.h"
#include "core/misc/string.h"

#include "core/memory/ptr.h"
#include "core/config/config.h"

namespace luna
{

STATIC_INIT(Core)
{
	LType::Get<SubPtrArray>()->Binding<SubPtrArray>();
	LBindingModule::Luna()->AddType(LType::Get<SubPtrArray>());
	
	LBindingModule::Luna()->AddMethod<&LoadLib>("load_library");
};

}