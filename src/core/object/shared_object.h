#pragma once


#include "core/foundation/container.h"
#include "core/object/base_object.h"
#include "core/memory/ptr.h"
#include "core/reflection/type.h"

namespace luna
{

class CORE_API SharedObject : public LObject
{
	RegisterTypeEmbedd(SharedObject, LObject)

};


}