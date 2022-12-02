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
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"

#include "render/rhi/vertex_layout.h"


#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"


#include "render/rhi/vulkan/vulkan_view.h"
#include "render/asset/texture_asset.h"
#include "renderer/render_device.h"


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
	if (ImGui::TreeNode("Render"))
	{
		if(mRenderDevice->mDeviceType == RenderDeviceType::DirectX12)
			ImGui::Text("Render Device: Directx12");
		else if (mRenderDevice->mDeviceType == RenderDeviceType::Vulkan)
			ImGui::Text("Render Device: Vulkan");
		ImGui::TreePop();
	}
}

bool RenderModule::OnShutdown()
{
	if (mFrameGraph)
		delete mFrameGraph;
	return true;
}

bool RenderModule::OnInit()
{	
	mRenderDevice = new RenderDevice();
	mRenderDevice->Init();

	RHIRenderQueue* m_queue_core = mRenderDevice->mGraphicQueue;
	//此处做Render系统的Init
	LWindow* mainWindow = gEngine->GetModule<WindowModule>()->GetMainWindow();
#ifdef _WIN32
	auto hwnd = mainWindow->GetWin32HWND();
#endif // _WIN32


	RHIWindowDesc windowDesc;
	windowDesc.mWidth = mainWindow->GetWindowWidth();
	windowDesc.mHeight = mainWindow->GetWindowHeight();
	windowDesc.mFrameNumber = 2;
	mMainSwapchain = sRenderModule->GetCmdQueueCore()->CreateSwapChain(
		mainWindow, windowDesc);


	mMainRT.SetPtr(TCreateObject<render::RenderTarget>());
	mMainRT->SetScreenRt(false);
	mMainRT->Ready();

	mRenderDevice->mFence->Wait(mRenderDevice->mFenceValue);	

	mRenderDevice->mGraphicCmd->ResourceBarrierExt({ mMainRT->mColorTexture, render::ResourceState::kUndefined , render::ResourceState::kCommon });
	mRenderDevice->mGraphicCmd->ResourceBarrierExt({ mMainSwapchain->GetBackBuffer(0), render::ResourceState::kUndefined , render::ResourceState::kPresent });
	mRenderDevice->mGraphicCmd->ResourceBarrierExt({ mMainSwapchain->GetBackBuffer(1), render::ResourceState::kUndefined , render::ResourceState::kPresent });
	mRenderDevice->mGraphicCmd->CloseCommondList();
	mRenderDevice->mGraphicQueue->ExecuteCommandLists(mRenderDevice->mGraphicCmd);
	mRenderDevice->mGraphicQueue->Signal(mRenderDevice->mFence, ++mRenderDevice->mFenceValue);

	mFrameGraph = new FrameGraphBuilder("MainFG");	

	mDefaultWhiteTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/white.png"));
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

	mRenderDevice->FlushStaging();
	mFrameGraph->Flush();

}

void RenderModule::OnFrameEnd(float deltaTime)
{
	mFrameGraph->Clear();
	mRenderDevice->OnFrameEnd();
}

RenderScene* RenderModule::AddScene()
{
	RenderScene* scene = new RenderScene(mRenderScenes.Size());
	mRenderScenes.PushBack(scene);
	return scene;
}

}

