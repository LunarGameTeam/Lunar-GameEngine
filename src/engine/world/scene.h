#pragma once

#include "core/core_module.h"
#include "world/entity.h"

namespace luna
{

class Scene : LObject
{

private:
	LVector<Entity *> m_entities;
};

}