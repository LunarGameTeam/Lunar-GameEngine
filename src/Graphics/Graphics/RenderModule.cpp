#include "Graphics/RenderModule.h"

#include "Core/Asset/AssetModule.h"
#include "Core/Object/Transform.h"
#include "Core/Object/Entity.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Foundation/profile.h"
#include "Core/Event/EventModule.h"
#include "imgui.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/RHI/RHISwapchain.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
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
#include "Graphics/RHI/RhiImgui/RHIImguiHelper.h"


luna::graphics::RenderModule* luna::sRenderModule = nullptr;

namespace luna::graphics
{


RegisterTypeEmbedd_Imp(RenderModule)
{
	cls->Ctor<RenderModule>();
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

}
RenderModule::RenderModule()
{
	mNeedTick = true;
	mNeedRenderTick = true;
	sRenderModule = this;	
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
	GenerateRenderDevice();
	GenerateRhiResourceGenerator();
	mGraphicQueue = GenerateRenderQueue();
	GenerateGlobelRenderResourceContext();
	GenerateGlobelEncoderHelper();

	mGraphicCmd = sGlobelRenderDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);
		
	//此处做Render系统的Init

	gEngine->GetTModule<PlatformModule>()->OnWindowResize.Bind(AutoBind(&RenderModule::OnMainWindowResize, this));
	LWindow* mainWindow = gEngine->GetTModule<PlatformModule>()->GetMainWindow();
	mSwapchainDesc.mWidth = mainWindow->GetWindowWidth();
	mSwapchainDesc.mHeight = mainWindow->GetWindowHeight();
	mSwapchainDesc.mFrameNumber = 2;
	mMainSwapchain = mGraphicQueue->CreateSwapChain(
		mainWindow, mSwapchainDesc);

	mFrameFence = sGlobelRenderDevice->CreateFence();

	mDefaultWhiteTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/White.png"));
	mDefaultNormalTexture = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/Normal.png"));
	
	mRenderer = MakeShared<SceneRenderer>();
	InitBasePipeline(mRenderer->GetSceneRenderPipeline());
	mGuiRenderer = MakeShared<ImguiRenderer>();
	mGuiRenderer->Init(mMainSwapchain, mGraphicQueue);
	return true;
}


void RenderModule::Tick(float deltaTime)
{
	ImGuiRhiGenNewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	gEngine->OnImGUI();
	ImGui::EndFrame();
	mGuiRenderer->UpdateViewportsWindow();
}

void RenderModule::RenderTick(float delta_time)
{
	ZoneScoped;
	//检查是数据拷贝池是否需要刷新
	sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->CheckStageRefresh();
	mRenderer->OnFrameBegin();
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
	RHICmdList* prepareCommand = PrepareRender();
	//提交渲染指令
	mGraphicCmd->GetCmdList()->CloseCommondList();
	LArray<RHICmdList*> allCommand;
	if (prepareCommand != nullptr)
	{
		allCommand.push_back(prepareCommand);
	}
	allCommand.push_back(mGraphicCmd->GetCmdList());
	mGraphicQueue->ExecuteMultiCommandLists(allCommand);
	mGuiRenderer->RenderTick();	
	
	mFrameFenceValue = mFrameFence->IncSignal(mGraphicQueue.get());
	{
		ZoneScopedN("Present");
		mMainSwapchain->PresentFrame(mFrameFence, mFrameFenceValue);
	}
	
	if (mNeedResizeSwapchain)
	{
		mMainSwapchain->Reset(mSwapchainDesc);
		mGuiRenderer->OnSwapchainChange(mMainSwapchain);
	}
	mGuiRenderer->RenderToOtherViewportsWindow();
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

RHICmdList* RenderModule::PrepareRender()
{
	ZoneScoped;
	return sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->FlushStaging();
}

void RenderModule::OnMainWindowResize(LWindow& window, WindowEvent&evt)
{
	mNeedResizeSwapchain = true;
	mSwapchainDesc.mFrameNumber = 2;
	mSwapchainDesc.mHeight = evt.height;
	mSwapchainDesc.mWidth = evt.width;
}



}

