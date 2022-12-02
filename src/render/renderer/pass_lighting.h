#pragma once

#include "render/pch.h"
#include "render/renderer/render_pass.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_view.h"
#include "render/renderer/render_scene.h"
namespace luna::render
{


class RENDER_API LightingPass : public RenderPass
{
public:
	LightingPass() = default;

	void BuildRenderPass(FrameGraphBuilder* builder, RenderView* view_info, RenderScene* render_scene) override;
};
}
