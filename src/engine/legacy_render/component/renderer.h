#pragma once

#include "legacy_render/private_render.h"
#include "core/core_module.h"
#include "legacy_render/d3d11/node.h"
#include "core/object/component.h"


namespace luna
{
namespace legacy_render
{

class LEGACY_RENDER_API RendererComponent : public Component
{
public:
	void OnCreate() override;
	void OnDestroy() override;
	void OnEnable() override;
	void OnDisable() override;
	
	virtual void PopulateRenderNode(RenderNode &render_nodes) = 0;

	GET_SET_VALUE(bool, m_dirty, RendererDirty);
public:

	void OnTick(float delta_time) override;

private:
	bool m_dirty;

};

}
}