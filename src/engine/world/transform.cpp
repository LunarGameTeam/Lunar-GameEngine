#include "transform.h"

namespace luna
{

void Transform::AddChild(Transform *child)
{
	auto parent = child->m_parent;
	if (parent == this)
		return;
	if (parent)
	{
		for (auto transform = parent->m_children.begin(); transform != parent->m_children.end(); ++transform)
		{
			if (*transform == child)
			{
				parent->m_children.erase(transform);
				break;
			}
		}
	}
	m_children.push_back(child);
	child->m_parent = this;
}

void Transform::RemoveChild(Transform *child)
{
	for (auto transform = m_children.begin(); transform != m_children.end(); ++transform)
	{
		if (*transform == child)
		{
			m_children.erase(transform);
			child->m_parent = nullptr;
			break;
		}
	}
}

}