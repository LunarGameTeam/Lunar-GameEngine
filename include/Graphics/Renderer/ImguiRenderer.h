#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Renderer/ImGuiTexture.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{
	class RENDER_API ImguiRenderer
	{
	public:
		ImguiRenderer() {};

		void Init(
			RHISwapChain* mainSwapchain,
			RenderResourceGenerateHelper* renderContext
		);

		void OnSwapchainChange(RHISwapChain* mainSwapchain);

		void RenderTick();

		void UpdateViewportsWindow();

		void RenderToOtherViewportsWindow();

		ImguiTexture* GetImguiTexture(RHIResource* key);

		ImguiTexture* AddImguiTexture(RHIResource* res);

		bool          IsImuiTexture(RHIResource* key);
	private:
		graphics::RHIRenderPassPtr         mIMGUIRenderPass;

		LMap<RHIResourcePtr, ImguiTexture> mImguiTextures;

		RHISwapChain*                      mMainSwapchain;

		RenderResourceGenerateHelper*      mRenderContext;

		graphics::RHIFrameBufferPtr        mFrameBuffer[2];

		RHISinglePoolSingleCmdListPtr      mGuiCmd;

		void GenerateFountForImgui();

		void GenerateWindowLayout();

		void GenerateStyle();

		void RenderImpl();
	};



}
