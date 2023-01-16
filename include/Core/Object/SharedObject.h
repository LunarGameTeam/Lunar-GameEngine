#pragma once


#include "Core/Foundation/Container.h"
#include "Core/Object/BaseObject.h"
#include "Core/Memory/Ptr.h"
#include "Core/Reflection/Type.h"

namespace luna
{

class CORE_API SharedObject : public LObject
{
	RegisterTypeEmbedd(SharedObject, LObject)

};


}