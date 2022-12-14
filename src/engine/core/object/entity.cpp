#include "entity.h"
#include "core/object/transform.h"

namespace luna
{

void Entity::OnCreate()
{
}

void Entity::OnDestroy()
{

}


void Entity::OnTick(float delta_time)
{

}

Entity *Entity::Parent()
{
	if (!m_parent) return nullptr;
	return m_parent->m_owner;
}

Transform *Entity::GetTransform()
{
	return m_transform;
}

Entity::Entity(const LString &name)
{
	m_need_tick = false;
	m_transform = RequireComponent<Transform>();
}

}