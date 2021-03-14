#pragma once

#include "core/private_core.h"
#include "core/object/component.h"
#include "core/math/math.h"
#include "core/math/lunar_math.h"

namespace luna
{

class CORE_API Transform : public Component
{
public:
	Transform *GetParentTransform()
	{
		return m_parent;
	}
	LVector<Transform *> GetChildren()
	{
		return m_children;
	}
	size_t GetChildrenCount()
	{
		return m_children.size();
	}
	void AddChild(Transform *child);
	void RemoveChild(Transform *child);
	LMatrix4f GetMatrix();
	LMatrix4f &GetCachedMatrix();

	GET_SET_REF(LVector3f, m_pos, Position);
	GET_SET_REF(LQuaternion, m_rotation, Rotation);
	GET_SET_REF(LVector3f, m_scale, Scale);

	LVector3f RightDirection();
	LVector3f UpDirection();
	LVector3f FrontDirection();
protected:
	void SetTransformDirty()
	{
		m_dirty = true;
	}

private:
	LMatrix4f m_cached_matrix;
	Transform *m_parent;
	LVector<Transform *> m_children;

	bool m_dirty = true;
	LVector3f m_pos = LVector3f::Zero();
	LQuaternion m_rotation = LQuaternion::Identity();
	LVector3f m_scale = LVector3f(1, 1, 1);

};

}