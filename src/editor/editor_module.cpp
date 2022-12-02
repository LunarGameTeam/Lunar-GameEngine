#include "editor_module.h"

#include "core/asset/asset_module.h"
#include "core/misc/profile.h"

#include "engine/camera.h"

#include "editor/ui/hierarchy_panel.h"
#include "editor/ui/scene_panel.h"
#include "editor/ui/icon_font.h"
#include "editor/ui/inspector_panel.h"
#include "editor/ui/library_panel.h"

#include "window/window_module.h"
#include "imgui_impl_sdl.h"

#include "render/pch.h"
#include "render/render_module.h"

#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_cmd_list.h"

#include "imgui_impl_dx12.h"
#include "render/rhi/DirectX12/dx12_command_list.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"
#include "render/rhi/DirectX12/dx12_view.h"

#include "imgui_impl_vulkan.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"
#include "render/rhi/vulkan/vulkan_descriptor_pool.h"
#include "render/rhi/vulkan/vulkan_render_pass.h"
#include "render/rhi/vulkan/vulkan_cmd_list.h"
#include "render/rhi/vulkan/vulkan_resource.h"
#include "render/rhi/vulkan/vulkan_view.h"




namespace luna::editor
{
EditorModule* sEditorModule = nullptr;

static void ImGUIText(const LString& name)
{
	ImGui::Text(name.c_str());
}

RegisterTypeEmbedd_Imp(EditorModule)
{
	cls->BindingMethod<&EditorModule::RegisterEditor>("register_editor").GetBindingMethodDef().ml_doc = LString::MakeStatic("def register_editor(self, t: typing.Type[T] ) -> T:\n\tpass\n");

	cls->Binding<EditorModule>();

	LBindingModule* editor_module = LBindingModule::Get("luna.editor");
	editor_module->AddType(cls);
	editor_module->AddMethod<&ImGUIText>("imgui_text");

	LBindingModule::Get("luna.editor.IMGUIFlags")->AddConstant("TextClip", 1);
}


EditorModule::EditorModule() :
	mScreenRt(this)
{
	sEditorModule = this;
	mNeedTick = true;
}

bool EditorModule::OnLoad()
{
	return true;
}

bool EditorModule::OnInit()
{
	auto func = std::bind(&EditorModule::OnWindowResize, this, std::placeholders::_1, std::placeholders::_2);
	static auto handle = sWindowModule->OnWindowResize.Bind(func);


	SetupIMGUI();

	
	mRHIDevice = sRenderModule->GetRHIDevice();
	render::RenderDevice* renderDevice = sRenderModule->GetRenderDevice();
	render::RenderPassDesc renderPassDesc;
	renderPassDesc.mColors.emplace_back(sRenderModule->GetMainSwapchain()->mBackBuffers[0]->mFormat, render::RenderPassLoadOp::kClear, render::RenderPassStoreOp::kStore);
	mRenderPass = mRHIDevice->CreateRenderPass(renderPassDesc);

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
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = mRHIDevice->As<render::VulkanDevice>()->GetVkInstance();
		init_info.PhysicalDevice = mRHIDevice->As<render::VulkanDevice>()->GetPhysicalDevice();
		init_info.Device = mRHIDevice->As<render::VulkanDevice>()->GetVkDevice();
		init_info.Queue = renderDevice->mGraphicQueue->As<render::VulkanRenderQueue>()->mQueue;
		init_info.DescriptorPool = renderDevice->mDefaultPool->As<render::VulkanDescriptorPool>()->mPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&init_info, mRenderPass->As<render::VulkanRenderPass>()->mRenderPass);

		sRenderModule->mRenderDevice->mFence->Wait(sRenderModule->mRenderDevice->mFenceValue);
		sRenderModule->mRenderDevice->mGraphicCmd->BeginEvent("ImguiFont");

		sRenderModule->mRenderDevice->mGraphicCmd->Reset();
				
		sRenderModule->mRenderDevice->mGraphicCmd->ResourceBarrierExt({ sRenderModule->mMainRT->mColorTexture, render::ResourceState::kUndefined, render::ResourceState::kShaderReadOnly });

		ImGui_ImplVulkan_CreateFontsTexture(sRenderModule->mRenderDevice->mGraphicCmd->As<render::VulkanGraphicCmdList>()->mCommandBuffer);
		
		sRenderModule->mRenderDevice->mGraphicCmd->CloseCommondList();


		sRenderModule->mRenderDevice->mGraphicCmd->EndEvent();
		sRenderModule->GetCmdQueueCore()->ExecuteCommandLists(sRenderModule->mRenderDevice->mGraphicCmd);
		sRenderModule->mRenderDevice->mGraphicQueue->Signal(sRenderModule->mRenderDevice->mFence, ++sRenderModule->mRenderDevice->mFenceValue);
		vkDeviceWaitIdle(mRHIDevice->As<render::VulkanDevice>()->GetVkDevice());
		
		render::ViewDesc srvDesc;
		srvDesc.mViewDimension = render::RHIViewDimension::TextureView2D;
		srvDesc.mViewType = render::RHIViewType::kTexture;		
		mImguiSRV = mRHIDevice->CreateView(srvDesc);
		mImguiSRV->BindResource(sRenderModule->mMainRT->mColorTexture);
		mSceneImage = ImGui_ImplVulkan_AddTexture(renderDevice->mClampSampler->As<render::VulkanResource>()->mSampler, mImguiSRV->As<render::VulkanView>()->mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	break;
	default:
		break;

	}

	gEngine->GetModule<render::RenderModule>();

	for (int i = 0; i < 2; i++)
	{
		render::RHIResourcePtr backBufferImage = sRenderModule->GetMainSwapchain()->GetBackBuffer(i);
		render::ViewDesc backbufferViewDesc;
		backbufferViewDesc.mBaseMipLevel = 0;
		backbufferViewDesc.mViewType = render::RHIViewType::kRenderTarget;
		backbufferViewDesc.mViewDimension = render::RHIViewDimension::TextureView2D;
		mBackBufferView[i] = renderDevice->CreateView(backbufferViewDesc);
		mBackBufferView[i]->BindResource(backBufferImage);

		render::FrameBufferDesc desc;
		desc.mPass = mRenderPass;
		desc.mColor.push_back(mBackBufferView[i]);
		desc.mDepthStencil = nullptr;
		desc.mWidth = backBufferImage->mWidth;
		desc.mHeight = backBufferImage->mHeight;
		mFrameBuffer[i] = mRHIDevice->CreateFrameBuffer(desc);
	}

	//m_srv_scene->BindResource(rt_texture, desc, 0);

	//mTask = NewObject<TodoTaskEditor>(TodoTaskEditor::StaticType());
	//m_editors.push_back(mTask);
	return true;
}

bool EditorModule::OnShutdown()
{
	return true;
}

void EditorModule::Tick(float delta_time)
{
	for (EditorBase* editro : m_editors)
	{
		if (!editro->mInited)
		{
			editro->Init();
			editro->mInited = true;
		}
	}

	if (mResize)
	{
		sRenderModule->GetMainSwapchain()->Reset(mWindowDesc);
		//m_scene_editor->UpdateRT();
	}
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

	for (EditorBase* ed : m_editors)
	{
		ed->DoIMGUI();
	}

	gEngine->OnIMGUI();

	render::RenderDevice* device = sRenderModule->GetRenderDevice();
	ImGui::Render();

	size_t& mFenceValue3D = device->mFenceValue;

	uint32_t index = 0;
	device->mFence->Wait(mFenceValue3D);
	device->mGraphicCmd->Reset();

	index = sRenderModule->GetMainSwapchain()->NextImage(device->mFence, ++mFenceValue3D);

	device->mFence->Wait(mFenceValue3D);

	device->mGraphicCmd->BindDescriptorHeap();
	device->mGraphicCmd->BeginEvent("Imgui");

	device->mGraphicCmd->ResourceBarrierExt({ sRenderModule->mMainRT->mColorTexture, render::ResourceState::kRenderTarget, render::ResourceState::kShaderReadOnly });

	device->mGraphicCmd->ResourceBarrierExt({ sRenderModule->GetMainSwapchain()->GetBackBuffer(index), render::ResourceState::kPresent, render::ResourceState::kRenderTarget });

	device->mGraphicCmd->BeginRenderPass(mRenderPass, mFrameBuffer[index]);
	device->mGraphicCmd->ClearRTView(mBackBufferView[index],LVector4f(1,1,1,1));

	if(device->mDeviceType == render::RenderDeviceType::DirectX12)
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), device->mGraphicCmd->As<render::DX12GraphicCmdList>()->mDxCmdList.Get());
	else if(device->mDeviceType == render::RenderDeviceType::Vulkan)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), device->mGraphicCmd->As<render::VulkanGraphicCmdList>()->mCommandBuffer);
	device->mGraphicCmd->EndRenderPass();


	device->mGraphicCmd->ResourceBarrierExt({ sRenderModule->mMainRT->mColorTexture, render::ResourceState::kShaderReadOnly, render::ResourceState::kRenderTarget });
	device->mGraphicCmd->ResourceBarrierExt({ sRenderModule->GetMainSwapchain()->GetBackBuffer(index), render::ResourceState::kRenderTarget , render::ResourceState::kPresent });
 	device->mGraphicCmd->EndEvent();

	device->mGraphicCmd->CloseCommondList();
	device->mGraphicQueue->ExecuteCommandLists(device->mGraphicCmd);

	device->mGraphicQueue->Signal(device->mFence, ++mFenceValue3D);
	sRenderModule->GetMainSwapchain()->PresentFrame(device->mFence, mFenceValue3D);

}

void EditorModule::OnIMGUI()
{
	if (ImGui::TreeNode("Editor"))
	{
		ImGui::Text("PyLObject数量: %d", binding:: BindingLObject::sBindingObjectNum);
		ImGui::TreePop();
	}

}

void EditorModule::SetupIMGUI()
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

void EditorModule::OnWindowResize(LWindow&, WindowEvent& evt)
{
	// 	render::RHISwapchainDesc desc;
	// 		desc.MaxFrameNum = 60;
	// 		desc.WindowWidth = evt.width;
	// 		desc.WindowHeight = evt.height;
	//render::RHIDevice *device = g_render_sys->GetDevice();
	//device->As<render::DX11Device>()->GetSwapchain()->Init(desc);
	GetMainEditor()->mWidth = evt.width;
	GetMainEditor()->mHeight = evt.height;

	mWindowDesc.mFrameNumber = 2;
	mWindowDesc.mHeight = evt.height;
	mWindowDesc.mWidth = evt.width;
	mResize = true;

}

EditorBase* EditorModule::RegisterEditor(EditorBase* base)
{
	m_editors.push_back(base);
	return base;
}

InspectorEditor* EditorModule::GetInspectorEditor()
{
	if (!mInspector)
		mInspector = GetEditor < InspectorEditor>();
	return mInspector;
}

SceneEditor* EditorModule::GetSceneEditor()
{
	if (!mSceneEditor)
		mSceneEditor = GetEditor<SceneEditor>();
	return mSceneEditor;
}

HierarchyEditor* EditorModule::GetHierarchyEditor()
{

	if (!mHierarchy)
		mHierarchy = GetEditor < HierarchyEditor>();
	return mHierarchy;
}

LibraryEditor* EditorModule::GetLibraryEditor()
{
	if (!mLibrary)
		mLibrary = GetEditor < LibraryEditor>();
	return mLibrary;
}

MainEditor* EditorModule::GetMainEditor()
{
	if (!mMainEditor)
		mMainEditor = GetEditor < MainEditor>();
	return mMainEditor;
}

}
