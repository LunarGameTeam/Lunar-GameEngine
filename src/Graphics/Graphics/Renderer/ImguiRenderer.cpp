#include"Graphics/Renderer/ImguiRenderer.h"
#include"Graphics/RHI/RHISwapchain.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "Core/Asset/AssetModule.h"
#include "Core/Foundation/AwesomeFont.h"
#include "Graphics/RHI/RhiImgui/RHIImguiHelper.h"
namespace luna::graphics
{
	void ImguiRenderer::GenerateFountForImgui()
	{
		ImGuiIO& curIO = ImGui::GetIO();
		static SharedPtr<LFile> data = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/NotoSansSC-Regular.otf")->GetFileData();
		static SharedPtr<LFile> data2 = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/fa-regular-400.otf")->GetFileData();
		static SharedPtr<LFile> data3 = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/fa-solid-900.otf")->GetFileData();

		auto* font = curIO.Fonts->AddFontFromMemoryTTF((void*)data->GetData().data(), data->GetData().size(), 18, nullptr,
			curIO.Fonts->GetGlyphRangesChineseFull());


		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		curIO.Fonts->AddFontFromMemoryTTF((void*)data2->GetData().data(), (int)data2->GetData().size(), 16.0f, &config,
			icon_ranges);
		curIO.Fonts->AddFontFromMemoryTTF((void*)data3->GetData().data(), (int)data3->GetData().size(), 16.0f, &config,
			icon_ranges);
		//ImGui::PushFont(font);
		curIO.ConfigFlags = curIO.ConfigFlags | ImGuiConfigFlags_DockingEnable;
		curIO.ConfigFlags = curIO.ConfigFlags | ImGuiConfigFlags_ViewportsEnable;
		(void)curIO;
		curIO.IniFilename = nullptr;
	}

	void ImguiRenderer::GenerateWindowLayout()
	{
		static SharedPtr<TextAsset> defaultLayout = sAssetModule->LoadAsset<TextAsset>("default_layout.ini");
		static SharedPtr<TextAsset> layout = sAssetModule->LoadAsset<TextAsset>("layout.ini");
		if (layout)
			ImGui::LoadIniSettingsFromMemory(layout->GetContent().c_str(), layout->GetContent().Length());
		else if (defaultLayout)
			ImGui::LoadIniSettingsFromMemory(defaultLayout->GetContent().c_str(), defaultLayout->GetContent().Length());
	}

	void ImguiRenderer::GenerateStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.23f, 0.23f, 0.23f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(0xff1a1a1a);
		style.Colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(0xff101010);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.23f, 0.24f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.GrabRounding = style.FrameRounding = 2.3f;

		ImGui::GetStyle().FrameRounding = 2.0f;
		ImGui::GetStyle().FrameBorderSize = 1.0f;
		ImGui::GetStyle().WindowBorderSize = 0.5f;
		ImGui::GetStyle().IndentSpacing = 16.0f;
		ImGui::GetStyle().WindowPadding = ImVec2(8, 8);
		ImGui::GetStyle().FramePadding = ImVec2(8, 6);
		ImGui::GetStyle().ItemSpacing = ImVec2(8, 6);
	}

	void ImguiRenderer::OnSwapchainChange(RHISwapChain* mainSwapchain)
	{
		for (int i = 0; i < 2; i++)
		{
			graphics::FrameBufferDesc desc;
			desc.mPass = mIMGUIRenderPass;
			desc.mColor.push_back(mMainSwapchain->mViews[i]);
			desc.mDepthStencil = nullptr;
			desc.mWidth = mMainSwapchain->mBackBuffers[i]->GetDesc().Width;
			desc.mHeight = mMainSwapchain->mBackBuffers[i]->GetDesc().Height;
			mFrameBuffer[i] = sGlobelRenderDevice->CreateFrameBuffer(desc);
		}
	}

	void ImguiRenderer::Init(RHISwapChain* mainSwapchain, RHIRenderQueue* renderQueue)
	{
		mMainSwapchain = mainSwapchain;
		mGraphQueue = renderQueue;
		
		mGuiCmd = sGlobelRenderDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);
		GenerateFountForImgui();
		GenerateWindowLayout();
		GenerateStyle();
		graphics::RenderPassDesc renderPassDesc;
		renderPassDesc.mColorView.push_back(mMainSwapchain->mViews[0]);
		renderPassDesc.mColors.emplace_back(
			graphics::LoadOp::kClear,
			graphics::StoreOp::kStore,
			LVector4f(0, 0, 0, 1));
		mIMGUIRenderPass = sGlobelRenderDevice->CreateRenderPass(renderPassDesc);
		OnSwapchainChange(mainSwapchain);
	
		RenderImguiInit(sPlatformModule->GetMainWindow()->GetWindow(), mGraphQueue, mIMGUIRenderPass, mGuiCmd);
	}
	
	void ImguiRenderer::RenderImpl()
	{
		ZoneScoped;
		//size_t& fenceValue = mRenderContext->mFenceValue;

		//mRenderContext->mFence->Wait(fenceValue);
		mGuiCmd->Reset();
		//mMainSwapchain->NextImage();
		uint32_t index = mMainSwapchain->GetNowFrameID();

		//mRenderContext->mFence->Wait(fenceValue);
		//mGuiCmd->GetCmdList()->BindDescriptorHeap();
		mGuiCmd->GetCmdList()->BeginEvent("IMGUI");
		for (auto& it : mImguiTextures)
		{
			mGuiCmd->GetCmdList()->ResourceBarrierExt({ it.second.mView->mBindResource, it.second.mView->mBindResource->mState, graphics::ResourceState::kShaderReadOnly });
		}
		mGuiCmd->GetCmdList()->BindDescriptorHeap();
		mGuiCmd->GetCmdList()->ResourceBarrierExt({ mMainSwapchain->GetBackBuffer(index), graphics::ResourceState::kUndefined, graphics::ResourceState::kRenderTarget });
		mGuiCmd->GetCmdList()->BeginRenderPass(mIMGUIRenderPass, mFrameBuffer[index]);
		ImGuiRhiRenderDrawData(ImGui::GetDrawData(), mGuiCmd);
		mGuiCmd->GetCmdList()->EndRenderPass();
		mGuiCmd->GetCmdList()->ResourceBarrierExt({ mMainSwapchain->GetBackBuffer(index), graphics::ResourceState::kRenderTarget , graphics::ResourceState::kPresent });
		mGuiCmd->GetCmdList()->EndEvent();
		mGuiCmd->GetCmdList()->CloseCommondList();
		mGraphQueue->ExecuteCommandLists(mGuiCmd->GetCmdList());
	}
	
	void ImguiRenderer::RenderTick()
	{
		ZoneScopedN("IMGUI");
		ImGui::Render();
		RenderImpl();
	}

	void ImguiRenderer::UpdateViewportsWindow()
	{
		auto& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
		}
	}

	void ImguiRenderer::RenderToOtherViewportsWindow()
	{
		auto& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	bool ImguiRenderer::IsImuiTexture(RHIResource* key)
	{
		return mImguiTextures.find(key) != mImguiTextures.end();
	}

	ImguiTexture* ImguiRenderer::GetImguiTexture(RHIResource* key)
	{
		return &mImguiTextures[key];
	}

	ImguiTexture* ImguiRenderer::AddImguiTexture(RHIResource* res)
	{
		ImguiTexture* imguiTexture = &mImguiTextures[res];
		ViewDesc desc;
		desc.mViewType = RHIViewType::kTexture;
		desc.mViewDimension = RHIViewDimension::TextureView2D;
		imguiTexture->mView = sGlobelRenderDevice->CreateView(desc);
		imguiTexture->mView->BindResource(res);
		AddImguiRhiTexture(imguiTexture, res);
		return imguiTexture;
	}
}