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

luna::LMatrix4f Transform::GetMatrix()
{
	LTransform trans = LTransform::Identity();
	trans.translate(m_pos);
	trans.rotate(m_rotation);
	trans.scale(m_scale);
	m_cached_matrix = trans.matrix();
	return m_cached_matrix;
}

luna::LVector3f Transform::RightDirection()
{
	LVector3f result = m_rotation * LVector3f::UnitX();
	result.normalize();
	return result;
}

luna::LVector3f Transform::UpDirection()
{
	LVector3f result = m_rotation * LVector3f::UnitY();
	result.normalize();
	return result;
}

luna::LVector3f Transform::FrontDirection()
{
	LVector3f result = m_rotation * LVector3f::UnitZ();
	result.normalize();
	return result;
}

}