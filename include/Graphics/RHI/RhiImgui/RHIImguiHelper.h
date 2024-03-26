#pragma once
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RhiImgui/ImGuiTexture.h"
class SDL_Window;
class ImDrawData;
namespace luna::graphics
{
	void RENDER_API RenderImguiInit(
		SDL_Window* window,
		RHIRenderQueue* renderQueue,
		graphics::RHIRenderPass* renderPass,
		RHISinglePoolSingleCmdList* cmdList
	);

	void RENDER_API ImGuiRhiRenderDrawData(ImDrawData* drawData, RHISinglePoolSingleCmdList* cmdList);

	void RENDER_API AddImguiRhiTexture(ImguiTexture* imguiTexture,RHIResource* res);

	void RENDER_API ImGuiRhiGenNewFrame();
}
