#pragma once
#include "core/core_config.h"

#include "core/object/base_object.h"

#include "core/memory/ptr.h"
#include "core/memory/ptr_binding.h"

#include "core/foundation/string.h"
#include "core/foundation/config.h"
#include "core/foundation/log.h"

#include "core/reflection/reflection.h"

#include "core/framework/module.h"
#include "core/framework/luna_core.h"

#include "core/math/math.h"

CORE_API void LoadLib(const luna::LString& val);
CORE_API void Print(PyObject* self, PyObject* args);
