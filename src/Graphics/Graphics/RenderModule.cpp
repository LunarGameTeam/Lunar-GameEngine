#include "Graphics/RenderModule.h"

#include "Core/Asset/AssetModule.h"
#include "Core/Object/Transform.h"
#include "Core/Object/Entity.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Foundation/profile.h"
#include "Core/Event/EventModule.h"


#include "Graphics/RHI/DirectX12/DX12DescriptorPool.h"


////临时先写DX11的Device
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"

#include "Graphics/Renderer/RenderDevice.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/ImGuiTexture.h"

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIMemory.h"
#include "Graphics/RHI/RHIRenderPass.h"

#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/DirectX12/DX12Memory.h"
#include "Graphics/RHI/DirectX12/DX12View.h"

#include "Graphics/RHI/VertexLayout.h"

#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"



#include "Graphics/RHI/Vulkan/VulkanView.h"

#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Renderer/RenderDevice.h"
#include "Graphics/Renderer/RenderTarget.h"

#include "Graphics/FrameGraph/FrameGraph.h"


#include "imgui_impl_sdl.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"


#include "RHI/Vulkan/VulkanCmdList.h"
#include "RHI/Vulkan/VulkanDescriptorPool.h"
#include "RHI/Vulkan/VulkanRenderPass.h"

#include "Core/Foundation/AwesomeFont.h"


RENDER_API luna::render::RenderModule* luna::sRenderModule = nullptr;

namespace luna::render
{


RegisterTypeEmbedd_Imp(RenderModule)
{
	cls->Ctor<RenderModule>();
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

}
RenderModule::RenderModule() :
	mMainRT(this)
{
	mNeedTick = true;
	mNeedRenderTick = true;
	sRenderModule = this;	
}

void RenderModule::OnIMGUI()
{
}

bool RenderModule::OnShutdown()
{
	if (mFrameGraph)
		delete mFrameGraph;
	ImGui::SaveIniSettingsToDisk("layout.ini");
	return true;
}

void RenderModule::SetupIMGUI()
{
	ImGuiIO& io = ImGui::GetIO();
	LSharedPtr<LFile> data = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/NotoSansSC-Regular.otf")->GetData();
	LSharedPtr<LFile> data2 = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/fa-regular-400.otf")->GetData();
	LSharedPtr<LFile> data3 = sAssetModule->LoadAsset<LBinaryAsset>("/assets/fonts/fa-solid-900.otf")->GetData();

	auto* font = io.Fonts->AddFontFromMemoryTTF((void*)data->GetData().data(), data->GetData().size(), 18, nullptr,
		io.Fonts->GetGlyphRangesChineseFull());


	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromMemoryTTF((void*)data2->GetData().data(), (int)data2->GetData().size(), 16.0f, &config,
		icon_ranges);
	io.Fonts->AddFontFromMemoryTTF((void*)data3->GetData().data(), (int)data3->GetData().size(), 16.0f, &config,
		icon_ranges);
	//ImGui::PushFont(font);
	io.ConfigFlags = io.ConfigFlags | ImGuiConfigFlags_DockingEnable;
	(void)io;

	io.IniFilename = nullptr;
	LSharedPtr<TextAsset> defaultLayout;
	defaultLayout.reset(sAssetModule->LoadAsset<TextAsset>("default_layout.ini"));
	LSharedPtr<TextAsset> layout;
	layout.reset(sAssetModule->LoadAsset<TextAsset>("layout.ini"));
	if(layout)
		ImGui::LoadIniSettingsFromMemory(layout->GetContent().c_str(), layout->GetContent().Length());
	else if(defaultLayout)
		ImGui::LoadIniSettingsFromMemory(defaultLayout->GetContent().c_str(), defaultLayout->GetContent().Length());

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


void RenderModule::UpdateFrameBuffer()
{
	render::RHIDevice* rhiDevice = GetRHIDevice();

	for (int i = 0; i < 2; i++)
	{
		render::FrameBufferDesc desc;
		desc.mPass = mRenderPass;
		desc.mColor.push_back(sRenderModule->GetSwapChain()->mViews[i]);
		desc.mDepthStencil = nullptr;
		desc.mWidth = sRenderModule->GetSwapChain()->mBackBuffers[i]->mResDesc.Width;
		desc.mHeight = sRenderModule->GetSwapChain()->mBackBuffers[i]->mResDesc.Height;
		mFrameBuffer[i] = rhiDevice->CreateFrameBuffer(desc);
	}

}

bool RenderModule::OnLoad()
{
	return true;
}

bool RenderModule::OnInit()
{
	mRenderDevice = new RenderDevice();	
	mRenderDevice->Init();

	render::RHIDevice* rhiDevice = GetRHIDevice();	
	//此处做Render系统的Init

	gEngine->GetModule<PlatformModule>()->OnWindowResize.Bind(AutoBind(&RenderModule::OnMainWindowResize, this));
	LWindow* mainWindow = gEngine->GetModule<PlatformModule>()->GetMainWindow();
	mSwapchainDesc.mWidth = mainWindow->GetWindowWidth();
	mSwapchainDesc.mHeight = mainWindow->GetWindowHeight();
	mSwapchainDesc.mFrameNumber = 2;
	mMainSwapchain = mRenderDevice->mGraphicQueue->CreateSwapChain(
		mainWindow, mSwapchainDesc);


	mMainRT.SetPtr(NewObject<render::RenderTarget>());
	mMainRT->Ready();

	mRenderDevice->mFence->Wait(mRenderDevice->mFenceValue);	

	mFrameGraph = new FrameGraphBuilder("MainFG");	

	mDefaultWhiteTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/white.png"));

	SetupIMGUI();

	render::RenderDevice* renderDevice = sRenderModule->GetRenderDevice();
	render::RenderPassDesc renderPassDesc;
	renderPassDesc.mColors.emplace_back(
		sRenderModule->GetSwapChain()->mBackBuffers[0]->mResDesc.Format,
		render::RenderPassLoadOp::kClear,
		render::RenderPassStoreOp::kStore,
		LVector4f(0, 0, 0, 1));
	mRenderPass = rhiDevice->CreateRenderPass(renderPassDesc);
	UpdateFrameBuffer();

	switch (sRenderModule->GetDeviceType())
	{
	case render::RenderDeviceType::DirectX12:
	{
		static render::Dx12DescriptorSet sDx12FontDescriptor;


		render::DX12Device* dx12_device = sRenderModule->GetDevice<render::DX12Device>();
		render::DX12DescriptorPool* dx12Pool = renderDevice->GetDefaultDescriptorPool()->As<render::DX12DescriptorPool>();


		dx12Pool->SelectSegment(render::DescriptorHeapType::CBV_SRV_UAV)->AllocDescriptorSet(1, sDx12FontDescriptor);
		ImGui_ImplSDL2_InitForD3D(sPlatformModule->GetMainWindow()->GetWindow());
		ImGui_ImplDX12_Init(dx12_device->GetDx12Device(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			sRenderModule->GetDevice<render::DX12Device>()->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetDeviceHeap(),
			sDx12FontDescriptor.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mCPUHandle,
			sDx12FontDescriptor.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle
		);
	}
	break;
	case render::RenderDeviceType::Vulkan:
	{
		ImGui_ImplSDL2_InitForVulkan(sPlatformModule->GetMainWindow()->GetWindow());
		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetVkInstance();
		vulkanInit.PhysicalDevice = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetPhysicalDevice();
		vulkanInit.Device = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetVkDevice();
		vulkanInit.Queue = mRenderDevice->mGraphicQueue->As<render::VulkanRenderQueue>()->mQueue;
		vulkanInit.DescriptorPool = mRenderDevice->GetDefaultDescriptorPool()->As<render::VulkanDescriptorPool>()->mPool;
		vulkanInit.MinImageCount = 3;
		vulkanInit.ImageCount = 3;
		vulkanInit.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&vulkanInit, mRenderPass->As<render::VulkanRenderPass>()->mRenderPass);

		sRenderModule->mRenderDevice->mFence->Wait(sRenderModule->mRenderDevice->mFenceValue);
		sRenderModule->mRenderDevice->mGraphicCmd->BeginEvent("ImguiFont");

		sRenderModule->mRenderDevice->mGraphicCmd->Reset();

		ImGui_ImplVulkan_CreateFontsTexture(sRenderModule->mRenderDevice->mGraphicCmd->As<render::VulkanGraphicCmdList>()->mCommandBuffer);

		sRenderModule->mRenderDevice->mGraphicCmd->CloseCommondList();


		sRenderModule->mRenderDevice->mGraphicCmd->EndEvent();
		sRenderModule->mRenderDevice->mGraphicQueue->ExecuteCommandLists(sRenderModule->mRenderDevice->mGraphicCmd);
		sRenderModule->mRenderDevice->mGraphicQueue->Signal(sRenderModule->mRenderDevice->mFence, ++sRenderModule->mRenderDevice->mFenceValue);
		vkDeviceWaitIdle(rhiDevice->As<render::VulkanDevice>()->GetVkDevice());
	
	}
	break;
	default:
		break;

	}
	mDefaultShader = sAssetModule->LoadAsset<ShaderAsset>("/assets/built-in/Debug.hlsl");
	return true;
}


void RenderModule::OnFrameBegin(float delta_time)
{
	mRenderDevice->OnFrameBegin();
}

void RenderModule::Tick(float delta_time)
{
	//RenderScene发起渲染
	mRenderDevice->mTransferCmd->BeginEvent("Frame Graph Prepare");
	for (RenderScene* it : mRenderScenes)
	{
		it->Render(mFrameGraph);
	}
	mRenderDevice->mTransferCmd->EndEvent();

	Render();


	if (sRenderModule->GetDeviceType() == render::RenderDeviceType::DirectX12)
	{
		ImGui_ImplDX12_NewFrame();
	}
	else if (sRenderModule->GetDeviceType() == render::RenderDeviceType::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
	}

	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//Engine IMGUI Begin
	gEngine->OnIMGUI();
	//Engine IMGUI End
	ImGui::Render();

	auto& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	RenderIMGUI();


}

void RenderModule::OnFrameEnd(float deltaTime)
{
	mFrameGraph->Clear();
	mRenderDevice->OnFrameEnd();
	mRenderDevice->mGraphicQueue->Wait(mRenderDevice->mFence, mRenderDevice->mFenceValue);
	mRenderDevice->mGraphicQueue->Present(mMainSwapchain);

	if (mNeedResizeSwapchain)
	{
		mMainSwapchain->Reset(mSwapchainDesc);
		UpdateFrameBuffer();
	}
}

RenderScene* RenderModule::AddScene()
{
	RenderScene* scene = new RenderScene();
	mRenderScenes.push_back(scene);
	return scene;
}

void RenderModule::RemoveScene(RenderScene* val)
{
	if (val)
		delete val;
	std::erase(mRenderScenes, val);
}


ImguiTexture* RenderModule::AddImguiTexture(RHIResource* res)
{
	ImguiTexture* imguiTexture = &mImguiTextures[res];
	ViewDesc desc;
	desc.mViewType = RHIViewType::kTexture;
	desc.mViewDimension = RHIViewDimension::TextureView2D;
	imguiTexture->mView = GetRHIDevice()->CreateView(desc);
	imguiTexture->mView->BindResource(res);
	if (sRenderModule->GetDeviceType() == render::RenderDeviceType::Vulkan)
	{
		VkDescriptorSet  newSet = ImGui_ImplVulkan_AddTexture(mRenderDevice->mClampSampler->As<render::VulkanResource>()->mSampler, imguiTexture->mView->As<VulkanView>()->mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		imguiTexture->mImg = newSet;
	}
	else
	{
		Dx12GpuDescriptorHeap* global = mRenderDevice->mDevice->As<DX12Device>()->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		imguiTexture->mDX12Pool = global->AllocPool(2);
		Dx12DescriptorSet empty;
		imguiTexture->mDX12Pool->As<DX12GpuDescriptorSegment>()->AllocDescriptorSet(1, empty);
		mRenderDevice->mDevice->As<DX12Device>()->GetDx12Device()->CopyDescriptorsSimple(
			1,
			empty.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mCPUHandle,
			imguiTexture->mView->As<DX12View>()->GetCpuHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		imguiTexture->mImg = (void*)(empty.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle.ptr);
	}
	return imguiTexture;
}

void RenderModule::Render()
{
	mRenderDevice->FlushStaging();
	mFrameGraph->Flush();
}

void RenderModule::RenderIMGUI()
{

	size_t& fenceValue = mRenderDevice->mFenceValue;
	
	mRenderDevice->mFence->Wait(fenceValue);
	mRenderDevice->mGraphicCmd->Reset();
	mMainSwapchain->NextImage();
	uint32_t index = mMainSwapchain->GetNowFrameID();

	RHIRenderQueue* graphQueue = mRenderDevice->mGraphicQueue;

	mRenderDevice->mFence->Wait(fenceValue);

	mRenderDevice->mGraphicCmd->BindDescriptorHeap();
	mRenderDevice->mGraphicCmd->BeginEvent("IMGUI");

	for (auto& it : mImguiTextures)
	{
		mRenderDevice->mGraphicCmd->ResourceBarrierExt({ it.second.mView->mBindResource, render::ResourceState::kUndefined, render::ResourceState::kShaderReadOnly });
	}

	mRenderDevice->mGraphicCmd->ResourceBarrierExt({ sRenderModule->GetSwapChain()->GetBackBuffer(index), render::ResourceState::kUndefined, render::ResourceState::kRenderTarget });
	mRenderDevice->mGraphicCmd->BeginRenderPass(mRenderPass, mFrameBuffer[index]);
	if (mRenderDevice->mDeviceType == render::RenderDeviceType::DirectX12)
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mRenderDevice->mGraphicCmd->As<render::DX12GraphicCmdList>()->mDxCmdList.Get());
	else if (mRenderDevice->mDeviceType == render::RenderDeviceType::Vulkan)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mRenderDevice->mGraphicCmd->As<render::VulkanGraphicCmdList>()->mCommandBuffer);
	mRenderDevice->mGraphicCmd->EndRenderPass();

	mRenderDevice->mGraphicCmd->ResourceBarrierExt({ sRenderModule->GetSwapChain()->GetBackBuffer(index), render::ResourceState::kRenderTarget , render::ResourceState::kPresent });
	mRenderDevice->mGraphicCmd->EndEvent();
	mRenderDevice->mGraphicCmd->CloseCommondList();

	graphQueue->ExecuteCommandLists(mRenderDevice->mGraphicCmd);
	graphQueue->Signal(mRenderDevice->mFence, ++fenceValue);
}

void RenderModule::OnMainWindowResize(LWindow& window, WindowEvent&evt)
{
	mNeedResizeSwapchain = true;
	mSwapchainDesc.mFrameNumber = 2;
	mSwapchainDesc.mHeight = evt.height;
	mSwapchainDesc.mWidth = evt.width;
}

bool RenderModule::IsImuiTexture(RHIResource* key)
{
	return mImguiTextures.find(key) != mImguiTextures.end();
}

ImguiTexture* RenderModule::GetImguiTexture(RHIResource* key)
{
	return &mImguiTextures[key];
}

}

