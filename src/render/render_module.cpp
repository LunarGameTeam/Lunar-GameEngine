#include "render_module.h"

#include "core/asset/asset_module.h"
#include "core/object/transform.h"
#include "core/object/entity.h"
#include "core/framework/luna_core.h"
#include "core/misc/profile.h"

#include "window/window_module.h"


#include "engine/pch.h"
#include "engine/light.h"
#include "engine/scene_module.h"


////临时先写DX11的Device
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"

//RenderPass
#include "render/renderer/pass_lighting.h"
#include "render/renderer/pass_shadowmap.h"

#include "engine/camera.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_render_queue.h"
#include "render/rhi/DirectX12/dx12_memory.h"
#include "render/rhi/DirectX12/dx12_view.h"

#include "render/rhi/vertex_layout.h"


#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"


#include "render/rhi/vulkan/vulkan_view.h"
#include "render/asset/texture_asset.h"
#include "renderer/render_device.h"


#include "imgui_impl_sdl.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"
#include "rhi/vulkan/vulkan_cmd_list.h"
#include "rhi/vulkan/vulkan_descriptor_pool.h"
#include "rhi/vulkan/vulkan_render_pass.h"
#include "editor/ui/icon_font.h"

namespace luna::render
{

RegisterTypeEmbedd_Imp(RenderModule)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);

}
RenderModule::RenderModule() :
	mMainRT(this),
	mRenderScenes(this)
{
	mNeedTick = true;
	sRenderModule = this;	
}

void RenderModule::OnIMGUI()
{
	/*
	if (ImGui::TreeNode("Render"))
	{
		if (mRenderDevice->mDeviceType == RenderDeviceType::DirectX12)
			ImGui::Text("Render Device: Directx12");
		else if (mRenderDevice->mDeviceType == RenderDeviceType::Vulkan)
			ImGui::Text("Render Device: Vulkan");
		ImGui::TreePop();
	}*/
}

bool RenderModule::OnShutdown()
{
	if (mFrameGraph)
		delete mFrameGraph;
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

	ImGui::StyleColorsLight();

	ImGuiStyle* style = &ImGui::GetStyle();
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

	RHIRenderQueue* m_queue_core = mRenderDevice->mGraphicQueue;
	//此处做Render系统的Init
	LWindow* mainWindow = gEngine->GetModule<WindowModule>()->GetMainWindow();
#ifdef _WIN32
	auto hwnd = mainWindow->GetWin32HWND();
#endif // _WIN32


	RHISwapchainDesc windowDesc;
	windowDesc.mWidth = mainWindow->GetWindowWidth();
	windowDesc.mHeight = mainWindow->GetWindowHeight();
	windowDesc.mFrameNumber = 2;
	mMainSwapchain = sRenderModule->GetCmdQueueCore()->CreateSwapChain(
		mainWindow, windowDesc);


	mMainRT.SetPtr(TCreateObject<render::RenderTarget>());
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
		render::DX12DescriptorPool* dx12Pool = renderDevice->mDefaultPool->As<render::DX12DescriptorPool>();


		dx12Pool->SelectSegment(render::DescriptorHeapType::CBV_SRV_UAV)->AllocDescriptorSet(1, sDx12FontDescriptor);
		ImGui_ImplSDL2_InitForD3D(sWindowModule->GetMainWindow()->GetWindow());
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
		ImGui_ImplSDL2_InitForVulkan(sWindowModule->GetMainWindow()->GetWindow());
		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetVkInstance();
		vulkanInit.PhysicalDevice = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetPhysicalDevice();
		vulkanInit.Device = mRenderDevice->mDevice->As<render::VulkanDevice>()->GetVkDevice();
		vulkanInit.Queue = mRenderDevice->mGraphicQueue->As<render::VulkanRenderQueue>()->mQueue;
		vulkanInit.DescriptorPool = mRenderDevice->mDefaultPool->As<render::VulkanDescriptorPool>()->mPool;
		vulkanInit.MinImageCount = 3;
		vulkanInit.ImageCount = 3;
		vulkanInit.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&vulkanInit, mRenderPass->As<render::VulkanRenderPass>()->mRenderPass);

		sRenderModule->mRenderDevice->mFence->Wait(sRenderModule->mRenderDevice->mFenceValue);
		sRenderModule->mRenderDevice->mGraphicCmd->BeginEvent("ImguiFont");

		sRenderModule->mRenderDevice->mGraphicCmd->Reset();

		sRenderModule->mRenderDevice->mGraphicCmd->ResourceBarrierExt({ sRenderModule->mMainRT->mColorTexture, render::ResourceState::kUndefined, render::ResourceState::kShaderReadOnly });

		ImGui_ImplVulkan_CreateFontsTexture(sRenderModule->mRenderDevice->mGraphicCmd->As<render::VulkanGraphicCmdList>()->mCommandBuffer);

		sRenderModule->mRenderDevice->mGraphicCmd->CloseCommondList();


		sRenderModule->mRenderDevice->mGraphicCmd->EndEvent();
		sRenderModule->GetCmdQueueCore()->ExecuteCommandLists(sRenderModule->mRenderDevice->mGraphicCmd);
		sRenderModule->mRenderDevice->mGraphicQueue->Signal(sRenderModule->mRenderDevice->mFence, ++sRenderModule->mRenderDevice->mFenceValue);
		vkDeviceWaitIdle(rhiDevice->As<render::VulkanDevice>()->GetVkDevice());
	
	}
	break;
	default:
		break;

	}

	return true;
}


void RenderModule::OnFrameBegin(float delta_time)
{
	mRenderDevice->OnFrameBegin();
}

void RenderModule::Tick(float delta_time)
{
	LModule::Tick(delta_time);
	//RenderScene发起渲染

	mRenderDevice->mTransferCmd->BeginEvent("Frame Graph Prepare");
	for (auto& it : mRenderScenes)
	{
		it->Render(mFrameGraph);
	}
	mRenderDevice->mTransferCmd->EndEvent();

	Render();
	mRenderDevice->FlushStaging();
	mFrameGraph->Flush();


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

	RenderIMGUI();


}

void RenderModule::OnFrameEnd(float deltaTime)
{
	mFrameGraph->Clear();
	mRenderDevice->OnFrameEnd();
	mRenderDevice->mGraphicQueue->Wait(mRenderDevice->mFence, mRenderDevice->mFenceValue);
	mRenderDevice->mGraphicQueue->Present(mMainSwapchain);
}

RenderScene* RenderModule::AddScene()
{
	RenderScene* scene = new RenderScene(mRenderScenes.Size());
	mRenderScenes.PushBack(scene);
	return scene;
}

ImguiTexture* RenderModule::AddImguiTexture(RHIResource* res)
{
	ImguiTexture* imguiTexture = &mImguiTextures.emplace_back();
	ViewDesc desc;
	desc.mViewType = RHIViewType::kTexture;
	desc.mViewDimension = RHIViewDimension::TextureView2D;
	imguiTexture->mView = GetRHIDevice()->CreateView(desc);
	imguiTexture->mView->BindResource(res);
	if (sRenderModule->GetDeviceType() == render::RenderDeviceType::Vulkan)
	{
		VkDescriptorSet  new_set = ImGui_ImplVulkan_AddTexture(mRenderDevice->mClampSampler->As<render::VulkanResource>()->mSampler, imguiTexture->mView->As<VulkanView>()->mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		imguiTexture->mImg = new_set;
	}
	else
	{
		Dx12GpuDescriptorHeap* globel_heap = mRenderDevice->mDevice->As<DX12Device>()->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		imguiTexture->descriptorPoolSave = globel_heap->AllocPool(2);
		Dx12DescriptorSet empty_set;
		imguiTexture->descriptorPoolSave->AllocDescriptorSet(1, empty_set);
		mRenderDevice->mDevice->As<DX12Device>()->GetDx12Device()->CopyDescriptorsSimple(
			1,
			empty_set.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mCPUHandle,
			imguiTexture->mView->As<DX12View>()->GetCpuHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		imguiTexture->mImg = (void*)(empty_set.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle.ptr);
	}
	return imguiTexture;
}

void RenderModule::Render()
{

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

	for (ImguiTexture& it : mImguiTextures)
	{
		mRenderDevice->mGraphicCmd->ResourceBarrierExt({ it.mView->mBindResource, render::ResourceState::kUndefined, render::ResourceState::kShaderReadOnly });
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

}

