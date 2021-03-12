#include "entity.h"
#include "core/object/transform.h"

namespace luna
{

void Entity::OnCreate()
{
	m_cached_transform = RequireComponent<Transform>();
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
	return m_cached_transform;
}

Entity::Entity(const LString &name)
{
	m_need_tick = false;
	m_transform = AddComponent<Transform>();
}

}