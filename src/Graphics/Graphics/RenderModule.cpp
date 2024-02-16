#include "Graphics/RenderModule.h"

#include "Core/Asset/AssetModule.h"
#include "Core/Object/Transform.h"
#include "Core/Object/Entity.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Foundation/profile.h"
#include "Core/Event/EventModule.h"



////临时先写DX11的Device
#include "imgui.h"

#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/ImGuiTexture.h"

#include "Graphics/RHI/RHISwapchain.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIMemory.h"
#include "Graphics/RHI/RHIRenderPass.h"
#include "Graphics/RHI/VertexLayout.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Core/Foundation/AwesomeFont.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"


luna::graphics::RenderModule* luna::sRenderModule = nullptr;

namespace luna::graphics
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
	ImGui::SaveIniSettingsToDisk("layout.ini");
	return true;
}

bool RenderModule::OnLoad()
{
	return true;
}

bool RenderModule::OnInit()
{
	mRenderContext = new RenderResourceGenerateHelper();
	mRenderContext->Init();
	mRenderCommandEncoder = new RenderCommandGenerateHelper();
	mRenderCommandEncoder->Init(mRenderContext->mDevice);

	graphics::RHIDevice* rhiDevice = GetRHIDevice();
	mGraphicCmd = rhiDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);
		
	//此处做Render系统的Init

	gEngine->GetTModule<PlatformModule>()->OnWindowResize.Bind(AutoBind(&RenderModule::OnMainWindowResize, this));
	LWindow* mainWindow = gEngine->GetTModule<PlatformModule>()->GetMainWindow();
	mSwapchainDesc.mWidth = mainWindow->GetWindowWidth();
	mSwapchainDesc.mHeight = mainWindow->GetWindowHeight();
	mSwapchainDesc.mFrameNumber = 2;
	mMainSwapchain = mRenderContext->mGraphicQueue->CreateSwapChain(
		mainWindow, mSwapchainDesc);


	mMainRT.SetPtr(NewObject<graphics::RenderTarget>());	
	mMainRT->Ready();

	mFrameFence = rhiDevice->CreateFence();

	mDefaultWhiteTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/White.png"));
	mDefaultNormalTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/Normal.png"));
	
	mRenderer = MakeShared<SceneRenderer>();
	InitBasePipeline(mRenderer->GetSceneRenderPipeline());
	mGuiRenderer = MakeShared<ImguiRenderer>();
	mGuiRenderer->Init(mMainSwapchain, mRenderContext);
	return true;
}


void RenderModule::Tick(float deltaTime)
{
	if (sRenderModule->GetDeviceType() == graphics::RenderDeviceType::DirectX12)
	{
		ImGui_ImplDX12_NewFrame();
	}
	else if (sRenderModule->GetDeviceType() == graphics::RenderDeviceType::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
	}
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	gEngine->OnImGUI();
	ImGui::EndFrame();
	mLogicUpdated.store(true);
}

void RenderModule::RenderTick(float delta_time)
{
	ZoneScoped;
	mRenderContext->OnFrameBegin();
	//RenderScene发起渲染
	//mFrameFence->Wait(mFrameFenceValue);
	//sRenderModule->GetRenderContext()->mFence->Wait(sRenderModule->GetRenderContext()->mFenceValue);
	//开始录制渲染指令
	mGraphicCmd->Reset();
	for (RenderScene* it : mRenderScenes)
	{
		mRenderer->PrepareSceneRender(it);
		mGraphicCmd->GetCmdList()->BeginEvent("RenderSceneDataUpdateCommand");
		mGraphicCmd->GetCmdList()->BindDescriptorHeap();
		it->ExcuteCopy(mGraphicCmd->GetCmdList());
		mGraphicCmd->GetCmdList()->EndEvent();
		mRenderer->Render(it,mGraphicCmd->GetCmdList());
	}
	//先把barrier相关的资源指令提交
	PrepareRender();
	//提交渲染指令
	mGraphicCmd->GetCmdList()->CloseCommondList();
	mRenderContext->mGraphicQueue->ExecuteCommandLists(mGraphicCmd->GetCmdList());
	mGuiRenderer->RenderTick();	
	if (mLogicUpdated.load())
	{
		//这里更新每个viewport的window需要等待前一次的ImGui::EndFrame，否则会出现帧数不匹配导致的crash
		mLogicUpdated.store(false);
		mGuiRenderer->UpdateViewportsWindow();
	}
	mFrameFenceValue = mFrameFence->IncSignal(mRenderContext->mGraphicQueue.get());
	mRenderContext->OnFrameEnd();
	{
		ZoneScopedN("Present");
		mMainSwapchain->PresentFrame(mFrameFence, mFrameFenceValue);
	}
	
	if (mNeedResizeSwapchain)
	{
		mMainSwapchain->Reset(mSwapchainDesc);
		mGuiRenderer->OnSwapchainChange(mMainSwapchain);
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

void RenderModule::PrepareRender()
{
	ZoneScoped;
	mRenderContext->FlushStaging();
}

void RenderModule::OnMainWindowResize(LWindow& window, WindowEvent&evt)
{
	mNeedResizeSwapchain = true;
	mSwapchainDesc.mFrameNumber = 2;
	mSwapchainDesc.mHeight = evt.height;
	mSwapchainDesc.mWidth = evt.width;
}



}

