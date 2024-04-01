#pragma once
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RhiImgui/ImGuiTexture.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_sdl2.h"
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
