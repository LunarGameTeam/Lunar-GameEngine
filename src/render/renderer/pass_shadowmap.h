#pragma once

#include "render/render_config.h"

#include "render/renderer/render_pass.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_view.h"
#include "render/renderer/render_light.h"
#include "render/renderer/render_scene.h"
#include "render/renderer/render_device.h"

#include <tuple>
#include <functional>


namespace luna::render
{

class RenderView;

class RENDER_API ShadowRenderPass : public RenderPass
{
	 RHIResDesc              shadowTextureDesc;
	 RHIResDesc              shadowDepthTextureDesc;
	 RHITextureDesc          textureDesc;
	 RHIRenderPassPtr		      mRenderPass;
	 PipelinePair			         mPipeline;
	 LSharedPtr<ShaderAsset> mShadowShader;
	 FGResourceView*             mShadowMapName;
	 FGResourceView*             mShadowMapDepthName;
 public:
	ShadowRenderPass() = default;

	void BuildRenderPass(FrameGraphBuilder* builder, RenderView* view_info, RenderScene* render_scene) override;

	void RenderPassSetup(
		FrameGraphBuilder* builder,
		FGNode& node
	) override;

	void RenderPassExcute(
		FrameGraphBuilder* builder, FGNode& node, RenderDevice* device
	) override;

	int width = 4096;
	int height = 4096;
private:
	void InitImpl() override;
	std::pair<RHIPipelineStatePtr, RHIBindingSetPtr> mShadowPipeline;
};

}
