#pragma once

#include "private_world.h"
#include "core/core_module.h"
#include "core/math/lunar_math.h"
#include "component.h"

namespace luna
{

class WORLD_API Transform : public Component
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

protected:
	void SetTransformDirty()
	{
		m_dirty = true;
	}

private:
	Transform *m_parent;
	LVector<Transform *> m_children;
	bool m_dirty;
	LunarVector3 m_location;
	LunarVector3 m_scale;

};

}