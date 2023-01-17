#pragma once
#include "Core/CoreConfig.h"

#include "Core/Object/BaseObject.h"

#include "Core/Memory/Ptr.h"

#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Foundation/Config.h"
#include "Core/Foundation/Misc.h"
#include "Core/Foundation/Log.h"

#include "Core/Math/Math.h"

#include "Core/Reflection/Reflection.h"

#include "Core/Framework/Module.h"
#include "Core/Framework/LunaCore.h"


CORE_API void LoadLib(const luna::LString& val);
CORE_API void Print(PyObject* self, PyObject* args);
