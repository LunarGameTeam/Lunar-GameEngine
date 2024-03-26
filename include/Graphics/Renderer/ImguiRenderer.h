#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RhiImgui/ImGuiTexture.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{
	class RENDER_API ImguiRenderer
	{
	public:
		ImguiRenderer() {};

		void Init(
			RHISwapChain* mainSwapchain,
			RHIRenderQueue* renderQueue
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

		graphics::RHIFrameBufferPtr        mFrameBuffer[2];

		RHISinglePoolSingleCmdListPtr      mGuiCmd;

		RHIRenderQueue*                    mGraphQueue = nullptr;

		void GenerateFountForImgui();

		void GenerateWindowLayout();

		void GenerateStyle();

		void RenderImpl();
	};



}
