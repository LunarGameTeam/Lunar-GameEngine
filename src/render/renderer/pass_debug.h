#pragma once

#include "render/pch.h"
#include "render/renderer/render_pass.h"
#include "render/renderer/render_target.h"

#include <tuple>
#include <functional>
#include "core/reflection/method.h"
#include <core/reflection/type_traits.h>


namespace luna::render
{

class RENDER_API DebugRenderPass : public RenderPass
{
public:
	DebugRenderPass();
	void InitImpl() override;
	void BuildRenderPass(FrameGraphBuilder* fg, RenderView* view, RenderScene* renderScene) override;

};

}
